// SPDX-License-Identifier: Unlicense

#define STRING_QOL_IMPL
#include "stringqol.h"
#include <stdio.h>
#include <string.h>

static int passed = 0;
static int failed = 0;

static void check(int cond, const char *msg) {
  if (cond) {
    passed++;
    printf("SUCCESS %s\n", msg);
  } else {
    failed++;
    fprintf(stderr, "FAIL: %s\n", msg);
  }
}

// ---------------------------------------------------------------------------

static void test_new_string(void) {
  // Basic
  String *s = new_string("hello");
  check(s != NULL, "new_string: not null");
  check(strcmp(s->string, "hello") == 0, "new_string: content correct");
  check(s->size == 5, "new_string: size correct");
  check(s->string[s->size] == '\0', "new_string: null terminator at s->size");
  delete_string(s);

  // Empty string
  s = new_string("");
  check(s != NULL, "new_string empty: not null");
  check(s->size == 0, "new_string empty: size is 0");
  check(s->string[0] == '\0', "new_string empty: null terminated");
  delete_string(s);

  // NULL input
  s = new_string(NULL);
  check(s == NULL, "new_string null: returns NULL");
}

static void test_string_append(void) {
  String *s = new_string("ab");

  // Normal append
  check(string_append(s, 'c') == SQOL_SUCCESS, "append: returns success");
  check(strcmp(s->string, "abc") == 0, "append: content correct");
  check(s->size == 3, "append: size updated");
  check(s->string[s->size] == '\0', "append: null terminator");

  // Appending null terminator should fail
  check(string_append(s, '\0') == SQOL_FAILURE,
        "append null char: returns failure");
  check(s->size == 3, "append null char: size unchanged");

  // NULL string pointer
  check(string_append(NULL, 'x') == SQOL_FAILURE,
        "append to NULL: returns failure");

  delete_string(s);
}

static void test_string_append_str(void) {
  String *s = new_string("foo");

  check(string_append_str(s, "bar") == SQOL_SUCCESS,
        "append_str: returns success");
  check(strcmp(s->string, "foobar") == 0, "append_str: content correct");
  check(s->size == 6, "append_str: size correct");
  check(s->string[s->size] == '\0', "append_str: null terminated");

  // Append empty string — valid, no-op effectively
  check(string_append_str(s, "") == SQOL_SUCCESS, "append_str empty: success");
  check(strcmp(s->string, "foobar") == 0,
        "append_str empty: content unchanged");
  check(s->size == 6, "append_str empty: size unchanged");

  // NULL args
  check(string_append_str(NULL, "x") == SQOL_FAILURE,
        "append_str NULL dst: failure");
  check(string_append_str(s, NULL) == SQOL_FAILURE,
        "append_str NULL str: failure");

  delete_string(s);
}

static void test_string_append_string(void) {
  String *dst = new_string("hello ");
  String *src = new_string("world");

  check(string_append_string(dst, src) == SQOL_SUCCESS,
        "append_string: success");
  check(strcmp(dst->string, "hello world") == 0,
        "append_string: content correct");
  check(dst->size == 11, "append_string: size correct");
  check(dst->string[dst->size] == '\0', "append_string: null terminated");

  // NULL args
  check(string_append_string(NULL, src) == SQOL_FAILURE,
        "append_string NULL dst: failure");
  check(string_append_string(dst, NULL) == SQOL_FAILURE,
        "append_string NULL src: failure");

  // Self-append (same pointer for dst and src) — this is an edge case
  // your code will alias src->string through string_append_str which reads
  // from it while potentially reallocating. Documenting the actual behavior:
  // If it succeeds, size should double; if it crashes, that's a real bug.
  // Uncomment to probe:
  // string_append_string(src, src);

  delete_string(dst);
  delete_string(src);
}

static void test_string_cpy(void) {
  String *dst = new_string("short");
  String *src = new_string("much longer string");

  // Copy into smaller dst — requires realloc
  check(string_cpy(dst, src) == SQOL_SUCCESS, "cpy: returns success");
  check(strcmp(dst->string, src->string) == 0, "cpy: content matches src");
  check(dst->size == src->size, "cpy: size matches src");
  check(dst->string[dst->size] == '\0', "cpy: null terminated");

  // Copy into larger dst — no realloc needed
  String *big = new_string("a very long string indeed yes");
  String *small = new_string("hi");
  check(string_cpy(big, small) == SQOL_SUCCESS, "cpy into larger: success");
  check(strcmp(big->string, "hi") == 0, "cpy into larger: content correct");
  check(big->size == 2, "cpy into larger: size correct");
  check(big->string[big->size] == '\0', "cpy into larger: null terminated");

  // NULL args
  check(string_cpy(NULL, src) == SQOL_FAILURE, "cpy NULL dst: failure");
  check(string_cpy(dst, NULL) == SQOL_FAILURE, "cpy NULL src: failure");

  delete_string(dst);
  delete_string(src);
  delete_string(big);
  delete_string(small);
}

static void test_string_replace(void) {
  String *s = new_string("old content");

  check(string_replace(s, "new") == SQOL_SUCCESS, "replace: success");
  check(strcmp(s->string, "new") == 0, "replace: content correct");
  check(s->size == 3, "replace: size correct");
  check(s->string[s->size] == '\0', "replace: null terminated");

  // Replace with empty string
  check(string_replace(s, "") == SQOL_SUCCESS, "replace with empty: success");
  check(s->size == 0, "replace with empty: size 0");
  check(s->string[0] == '\0', "replace with empty: null terminated");

  // NULL args
  check(string_replace(NULL, "x") == SQOL_FAILURE,
        "replace NULL string: failure");
  check(string_replace(s, NULL) == SQOL_FAILURE,
        "replace NULL new_str: failure");

  delete_string(s);
}

static void test_string_reset(void) {
  String *s = new_string("original");

  check(string_reset(s) == SQOL_SUCCESS, "reset: returns success");
  check(s->size == 0, "reset: size is 0");
  check(s->cursor == 0, "reset: cursor is 0");
  check(s->string[0] == '\0', "reset: null terminated");
  check(s->cap == SQOL_ARENA_DEFAULT_CAP_VALUE, "reset: cap set to default");

  // NULL arg
  check(string_reset(NULL) == SQOL_FAILURE, "reset NULL: failure");

  delete_string(s);
}

static void test_string_compare(void) {
  String *s1 = new_string("hello");
  String *s2 = new_string("hello");
  String *s3 = new_string("world");

  check(string_compare(s1, "hello") == SQOL_SUCCESS, "compare equal: success");
  check(string_compare(s1, "world") == SQOL_FAILURE,
        "compare unequal: failure");
  check(string_compare(s1, "") == SQOL_FAILURE, "compare with empty: failure");

  // NULL args
  check(string_compare(NULL, "x") == SQOL_FAILURE,
        "compare NULL string: failure");
  check(string_compare(s1, NULL) == SQOL_FAILURE, "compare NULL str: failure");

  // String with null internal string
  String *bad = new_string("temp");
  SQOL_FREE(bad->string);
  bad->string = NULL;
  check(string_compare(bad, "x") == SQOL_FAILURE,
        "compare null internal: failure");
  bad->string = (char *)SQOL_MALLOC(1); // restore for delete
  bad->string[0] = '\0';

  delete_string(s1);
  delete_string(s2);
  delete_string(s3);
  delete_string(bad);
}

static void test_string_compare_string(void) {
  String *s1 = new_string("test");
  String *s2 = new_string("test");
  String *s3 = new_string("different");

  check(string_compare_string(s1, s2) == SQOL_SUCCESS,
        "compare_string equal: success");
  check(string_compare_string(s1, s3) == SQOL_FAILURE,
        "compare_string unequal: failure");

  // NULL args
  check(string_compare_string(NULL, s1) == SQOL_FAILURE,
        "compare_string NULL s: failure");
  check(string_compare_string(s1, NULL) == SQOL_FAILURE,
        "compare_string NULL string: failure");

  // Null internal strings
  String *bad1 = new_string("a");
  String *bad2 = new_string("b");
  SQOL_FREE(bad1->string);
  bad1->string = NULL;
  check(string_compare_string(bad1, bad2) == SQOL_FAILURE,
        "compare_string null internal s: failure");
  check(string_compare_string(s1, bad1) == SQOL_FAILURE,
        "compare_string null internal string: failure");
  bad1->string = (char *)SQOL_MALLOC(1);
  bad1->string[0] = '\0';

  delete_string(s1);
  delete_string(s2);
  delete_string(s3);
  delete_string(bad1);
  delete_string(bad2);
}

static void test_string_backspace(void) {
  String *s = new_string("hello");

  check(string_backspace(s) == SQOL_SUCCESS, "backspace: success");
  check(strcmp(s->string, "hell") == 0, "backspace: content correct");
  check(s->size == 4, "backspace: size decremented");

  // Backspace on empty string
  String *empty = new_string("");
  check(string_backspace(empty) == SQOL_FAILURE, "backspace empty: failure");
  check(empty->size == 0, "backspace empty: size unchanged");

  // NULL arg
  check(string_backspace(NULL) == SQOL_FAILURE, "backspace NULL: failure");

  // Null internal string
  String *bad = new_string("x");
  SQOL_FREE(bad->string);
  bad->string = NULL;
  check(string_backspace(bad) == SQOL_FAILURE,
        "backspace null internal: failure");
  bad->string = (char *)SQOL_MALLOC(1);
  bad->string[0] = '\0';

  delete_string(s);
  delete_string(empty);
  delete_string(bad);
}

static void test_string_peek_consume_match(void) {
  String *s = new_string("abc");

  // Initial state
  check(s->cursor == 0, "cursor initial: 0");

  // Peek current
  check(string_peek(s) == 'a', "peek: returns 'a'");
  check(s->cursor == 0, "peek: cursor unchanged");

  // Peek next
  check(string_peek_next(s) == 'b', "peek_next: returns 'b'");
  check(s->cursor == 0, "peek_next: cursor unchanged");

  // Consume
  check(string_consume(s) == 'a', "consume: returns 'a'");
  check(s->cursor == 1, "consume: cursor incremented");

  // Peek after consume
  check(string_peek(s) == 'b', "peek after consume: 'b'");

  // Match success
  check(string_match(s, 'b') == SQOL_TRUE, "match success: true");
  check(s->cursor == 2, "match success: cursor incremented");

  // Match failure
  check(string_match(s, 'x') == SQOL_FALSE, "match failure: false");
  check(s->cursor == 2, "match failure: cursor unchanged");

  // Consume to end
  check(string_consume(s) == 'c', "consume to end: 'c'");
  check(s->cursor == 3, "consume to end: cursor at end");

  // Peek at end
  check(string_peek(s) == '\0', "peek at end: null");
  check(string_peek_next(s) == '\0', "peek_next at end: null");

  // Consume past end
  check(string_consume(s) == '\0', "consume past end: null");
  check(s->cursor == 3, "consume past end: cursor unchanged");

  // NULL args
  check(string_peek(NULL) == SQOL_FAILURE, "peek NULL: failure");
  check(string_peek_next(NULL) == SQOL_FAILURE, "peek_next NULL: failure");
  check(string_consume(NULL) == SQOL_FAILURE, "consume NULL: failure");
  check(string_match(NULL, 'x') == SQOL_FALSE, "match NULL: false");

  // Null internal string
  String *bad = new_string("x");
  SQOL_FREE(bad->string);
  bad->string = NULL;
  check(string_peek(bad) == SQOL_FAILURE, "peek null internal: failure");
  check(string_peek_next(bad) == SQOL_FAILURE,
        "peek_next null internal: failure");
  check(string_consume(bad) == SQOL_FAILURE, "consume null internal: failure");
  check(string_match(bad, 'x') == SQOL_FALSE, "match null internal: false");
  bad->string = (char *)SQOL_MALLOC(1);
  bad->string[0] = '\0';

  delete_string(s);
  delete_string(bad);
}

static void test_string_add_to_arena(void) {
  StringArena *a = new_string_arena(4);
  String *s = new_string("test");

  String *result = string_add_to_arena(s, a);
  check(result != NULL, "add_to_arena: not null");
  check(result == s, "add_to_arena: returns same pointer");
  check(s->arena_owned == SQOL_TRUE, "add_to_arena: marks as owned");
  check(a->count == 1, "add_to_arena: count incremented");

  // NULL args
  check(string_add_to_arena(NULL, a) == SQOL_FAILURE,
        "add_to_arena NULL string: failure");
  check(string_add_to_arena(s, NULL) == SQOL_FAILURE,
        "add_to_arena NULL arena: failure");

  // Don't delete s, arena owns it now
  delete_arena(a);
}

static void test_delete_string(void) {
  // Basic double-call safety is UB by design in C — don't test it.
  // Just verify normal deletion returns success.
  String *s = new_string("bye");
  check(delete_string(s) == SQOL_SUCCESS, "delete: returns success");

  check(delete_string(NULL) == SQOL_FAILURE, "delete NULL: returns failure");
}

static void test_arena_basic(void) {
  StringArena *a = new_string_arena(4);
  check(a != NULL, "new_arena: not null");
  check(a->count == 0, "new_arena: count is 0");
  check(a->cap == 4, "new_arena: cap is 4");

  String *s1 = new_string("one");
  String *s2 = new_string("two");
  String *s3 = new_string("three");

  String *r1 = arena_add_string(a, s1);
  check(r1 != NULL, "arena_add: returns non-null");
  check(a->count == 1, "arena_add: count incremented");
  check(r1 == s1, "arena_add: returns same pointer");

  arena_add_string(a, s2);
  arena_add_string(a, s3);
  check(a->count == 3, "arena_add x3: count is 3");

  // NULL args
  check(arena_add_string(NULL, s1) == NULL,
        "arena_add NULL arena: returns NULL");
  check(arena_add_string(a, NULL) == NULL,
        "arena_add NULL string: returns NULL");

  delete_arena(a);
}

static void test_arena_growth(void) {
  // Start with cap 2, add enough strings to force realloc
  StringArena *a = new_string_arena(2);

  String *strings[8];
  for (int i = 0; i < 8; i++) {
    strings[i] = new_string("x");
    arena_add_string(a, strings[i]);
  }

  check(a->count == 8, "arena growth: count is 8");
  check(a->cap >= 8, "arena growth: cap grew to fit");

  // Verify pointers are intact
  check(a->strings[0] == strings[0], "arena growth: slot 0 intact");
  check(a->strings[7] == strings[7], "arena growth: slot 7 intact");

  delete_arena(a);
}

static void test_arena_default_cap(void) {
  // Cap of 0 should fall back to SQOL_ARENA_DEFAULT_CAP_VALUE
  StringArena *a = new_string_arena(0);
  check(a->cap == SQOL_ARENA_DEFAULT_CAP_VALUE,
        "arena cap=0: uses default cap");
  delete_arena(a);
}

static void test_arena_reset(void) {
  StringArena *a = new_string_arena(4);
  String *s = new_string("temp");
  arena_add_string(a, s);

  check(arena_reset(a, 8) == SQOL_SUCCESS, "arena_reset: success");
  check(a->count == 0, "arena_reset: count zeroed");
  check(a->cap == 8, "arena_reset: new cap applied");

  check(arena_reset(NULL, 4) == SQOL_FAILURE, "arena_reset NULL: failure");

  delete_arena(a);
}

// ---------------------------------------------------------------------------

int main(void) {
  test_new_string();
  test_string_append();
  test_string_append_str();
  test_string_append_string();
  test_string_cpy();
  test_string_replace();
  test_string_reset();
  test_string_compare();
  test_string_compare_string();
  test_string_backspace();
  test_string_peek_consume_match();
  test_string_add_to_arena();
  test_delete_string();
  test_arena_basic();
  test_arena_growth();
  test_arena_default_cap();
  test_arena_reset();

  printf("\n%d passed, %d failed\n", passed, failed);
  return failed > 0 ? 1 : 0;
}