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
  // Note: delete_arena does NOT free the strings themselves — that's by design
  // (arena owns the pointers array, not the strings). Free them manually:
  delete_string(s1);
  delete_string(s2);
  delete_string(s3);
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
  for (int i = 0; i < 8; i++)
    delete_string(strings[i]);
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

  delete_string(s);
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
  test_delete_string();
  test_arena_basic();
  test_arena_growth();
  test_arena_default_cap();
  test_arena_reset();

  printf("\n%d passed, %d failed\n", passed, failed);
  return failed > 0 ? 1 : 0;
}