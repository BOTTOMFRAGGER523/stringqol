// SPDX-License-Identifier: Unlicense

#define STRING_QOL_IMPL
#define STRING_QOL_CPP_IMPL
#include "stringqol.hpp"

#include <cstdio>
#include <cstring>

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
// String
// ---------------------------------------------------------------------------

static void test_string_construct() {
  StringQOL::String s((char *)"hello");
  check(s.get_internal() != nullptr, "construct: internal not null");
  check(strcmp(s.get_internal()->string, "hello") == 0,
        "construct: content correct");
  check(s.get_internal()->size == 5, "construct: size correct");
  check(s.get_internal()->string[s.get_internal()->size] == '\0',
        "construct: null terminator at size");
}

static void test_string_append_char() {
  StringQOL::String s((char *)"ab");

  check(s.append('c') == SQOL_SUCCESS, "append char: success");
  check(strcmp(s.get_internal()->string, "abc") == 0,
        "append char: content correct");
  check(s.get_internal()->size == 3, "append char: size updated");
  check(s.get_internal()->string[s.get_internal()->size] == '\0',
        "append char: null terminator");

  // Null char should fail
  check(s.append('\0') == SQOL_FAILURE, "append null char: failure");
  check(s.get_internal()->size == 3, "append null char: size unchanged");
}

static void test_string_append_cstr() {
  StringQOL::String s((char *)"foo");

  check(s.append((char *)"bar") == SQOL_SUCCESS, "append cstr: success");
  check(strcmp(s.get_internal()->string, "foobar") == 0,
        "append cstr: content correct");
  check(s.get_internal()->size == 6, "append cstr: size correct");
  check(s.get_internal()->string[s.get_internal()->size] == '\0',
        "append cstr: null terminated");

  // Empty append
  check(s.append((char *)"") == SQOL_SUCCESS, "append empty cstr: success");
  check(strcmp(s.get_internal()->string, "foobar") == 0,
        "append empty cstr: unchanged");
}

static void test_string_append_string() {
  StringQOL::String dst((char *)"hello ");
  StringQOL::String src((char *)"world");

  check(dst.append(src) == SQOL_SUCCESS, "append String: success");
  check(strcmp(dst.get_internal()->string, "hello world") == 0,
        "append String: content");
  check(dst.get_internal()->size == 11, "append String: size");
  check(dst.get_internal()->string[dst.get_internal()->size] == '\0',
        "append String: null term");
}

static void test_string_copy() {
  StringQOL::String dst((char *)"short");
  StringQOL::String src((char *)"much longer string");

  check(dst.copy(src) == SQOL_SUCCESS, "copy: success");
  check(strcmp(dst.get_internal()->string, src.get_internal()->string) == 0,
        "copy: content matches");
  check(dst.get_internal()->size == src.get_internal()->size,
        "copy: size matches");
  check(dst.get_internal()->string[dst.get_internal()->size] == '\0',
        "copy: null terminated");

  // Copy into a larger dst
  StringQOL::String big((char *)"a very long string indeed");
  StringQOL::String small((char *)"hi");

  check(big.copy(small) == SQOL_SUCCESS, "copy into larger: success");
  check(strcmp(big.get_internal()->string, "hi") == 0,
        "copy into larger: content");
  check(big.get_internal()->size == 2, "copy into larger: size");
}

static void test_string_copy_to() {
  StringQOL::String src((char *)"source");
  StringQOL::String dst((char *)"dst");

  // NOTE: copy_to passes dst by value — if you haven't fixed the by-value
  // bug this test will expose it (dst's internal will be freed by the copy's
  // destructor and dst becomes a dangling wrapper).
  check(src.copy_to(dst) == SQOL_SUCCESS, "copy_to: success");
  // After copy_to, dst's internal state is modified via the pointer
  // but only if the by-value bug is fixed. Verify through dst directly:
  check(strcmp(dst.get_internal()->string, "source") == 0,
        "copy_to: dst content updated");
}

static void test_string_replace_cstr() {
  StringQOL::String s((char *)"old");

  check(s.replace("new content") == SQOL_SUCCESS, "replace cstr: success");
  check(strcmp(s.get_internal()->string, "new content") == 0,
        "replace cstr: content");
  check(s.get_internal()->size == 11, "replace cstr: size");
  check(s.get_internal()->string[s.get_internal()->size] == '\0',
        "replace cstr: null term");

  // Replace with empty
  check(s.replace("") == SQOL_SUCCESS, "replace empty: success");
  check(s.get_internal()->size == 0, "replace empty: size 0");
  check(s.get_internal()->string[0] == '\0', "replace empty: null term");
}

static void test_string_replace_string() {
  StringQOL::String s((char *)"old");
  StringQOL::String src((char *)"replacement");

  check(s.replace(src) == SQOL_SUCCESS, "replace String: success");
  check(strcmp(s.get_internal()->string, "replacement") == 0,
        "replace String: content");
  check(s.get_internal()->size == 11, "replace String: size");
}

static void test_string_reset() {
  StringQOL::String s((char *)"original");

  check(s.reset() == SQOL_SUCCESS, "reset: success");
  check(s.get_internal()->size == 0, "reset: size 0");
  check(s.get_internal()->cursor == 0, "reset: cursor 0");
  check(s.get_internal()->string[0] == '\0', "reset: null terminated");
  check(s.get_internal()->cap == SQOL_ARENA_DEFAULT_CAP_VALUE,
        "reset: cap default");
}

static void test_string_compare_cstr() {
  StringQOL::String s((char *)"hello");

  check(s.compare("hello") == SQOL_SUCCESS, "compare cstr equal: success");
  check(s.compare("world") == SQOL_FAILURE, "compare cstr unequal: failure");
  check(s.compare("") == SQOL_FAILURE, "compare cstr empty: failure");
}

static void test_string_compare_string() {
  StringQOL::String s1((char *)"test");
  StringQOL::String s2((char *)"test");
  StringQOL::String s3((char *)"different");

  check(s1.compare(s2) == SQOL_SUCCESS, "compare String equal: success");
  check(s1.compare(s3) == SQOL_FAILURE, "compare String unequal: failure");
}

static void test_string_backspace() {
  StringQOL::String s((char *)"hello");

  check(s.backspace() == SQOL_SUCCESS, "backspace: success");
  check(strcmp(s.get_internal()->string, "hell") == 0, "backspace: content");
  check(s.get_internal()->size == 4, "backspace: size");

  // Backspace on empty
  StringQOL::String empty((char *)"");
  check(empty.backspace() == SQOL_FAILURE, "backspace empty: failure");
}

static void test_string_peek_consume_match() {
  StringQOL::String s((char *)"abc");

  // Initial
  check(s.get_internal()->cursor == 0, "cursor initial: 0");

  // Peek
  check(s.peek() == 'a', "peek: 'a'");
  check(s.get_internal()->cursor == 0, "peek: cursor unchanged");

  // Peek next
  check(s.peek_next() == 'b', "peek_next: 'b'");
  check(s.get_internal()->cursor == 0, "peek_next: cursor unchanged");

  // Consume
  check(s.consume() == 'a', "consume: 'a'");
  check(s.get_internal()->cursor == 1, "consume: cursor incremented");

  // Match success
  check(s.match('b') == SQOL_TRUE, "match success: true");
  check(s.get_internal()->cursor == 2, "match success: cursor incremented");

  // Match failure
  check(s.match('x') == SQOL_FALSE, "match failure: false");
  check(s.get_internal()->cursor == 2, "match failure: cursor unchanged");

  // Consume to end
  check(s.consume() == 'c', "consume to end: 'c'");
  check(s.get_internal()->cursor == 3, "consume to end: cursor at end");

  // Peek at end
  check(s.peek() == '\0', "peek at end: null");
  check(s.peek_next() == '\0', "peek_next at end: null");

  // Consume past end
  check(s.consume() == '\0', "consume past end: null");
  check(s.get_internal()->cursor == 3, "consume past end: cursor unchanged");
}

static void test_string_add_to_arena() {
  StringQOL::Arena a(4);
  StringQOL::String s((char *)"test");

  StringQOL::String result = s.add_to_arena(a);
  check(result.get_internal() != nullptr, "add_to_arena: not null");
  check(result.get_internal() == s.get_internal(),
        "add_to_arena: same internal");
  check(s.get_internal()->arena_owned == SQOL_TRUE,
        "add_to_arena: marked owned");
  check(a.get_internal()->count == 1, "add_to_arena: count 1");
}

// ---------------------------------------------------------------------------
// Arena
// ---------------------------------------------------------------------------

static void test_arena_default_construct() {
  StringQOL::Arena a;
  check(a.get_internal() != nullptr, "arena default: not null");
  check(a.get_internal()->cap == SQOL_ARENA_DEFAULT_CAP_VALUE,
        "arena default: cap correct");
  check(a.get_internal()->count == 0, "arena default: count 0");
}

static void test_arena_sized_construct() {
  StringQOL::Arena a(8);
  check(a.get_internal() != nullptr, "arena sized: not null");
  check(a.get_internal()->cap == 8, "arena sized: cap correct");
  check(a.get_internal()->count == 0, "arena sized: count 0");
}

static void test_arena_construct_zero_cap() {
  // cap < 1 currently leaves internal uninitialized — this test documents
  // the bug. Once fixed (e.g. clamp to default), internal should not be null.
  StringQOL::Arena a(0);
  check(a.get_internal() != nullptr,
        "arena cap=0: internal not null (bug if fails)");
}

static void test_arena_add_string_obj() {
  StringQOL::Arena a(4);
  StringQOL::String s((char *)"hello");

  // NOTE: if the by-value bug isn't fixed, s.get_internal() will be
  // freed inside add_string and this will crash or corrupt.
  StringQOL::String r = a.add_string(s);

  check(r.get_internal() != nullptr, "arena add_string obj: not null");
  check(strcmp(r.get_internal()->string, "hello") == 0,
        "arena add_string obj: content");
  check(a.get_internal()->count == 1, "arena add_string obj: count 1");
}

static void test_arena_add_string_cstr() {
  StringQOL::Arena a(4);

  StringQOL::String r = a.add_string("world");
  check(r.get_internal() != nullptr, "arena add cstr: not null");
  check(strcmp(r.get_internal()->string, "world") == 0,
        "arena add cstr: content");
  check(a.get_internal()->count == 1, "arena add cstr: count 1");
}

static void test_arena_growth() {
  StringQOL::Arena a(2);

  for (int i = 0; i < 6; i++) {
    a.add_string("x");
  }

  check(a.get_internal()->count == 6, "arena growth: count 6");
  check(a.get_internal()->cap >= 6, "arena growth: cap grew");
}

static void test_arena_reset_sized() {
  StringQOL::Arena a(4);
  a.add_string("one");
  a.add_string("two");

  check(a.reset(8) == SQOL_SUCCESS, "arena reset sized: success");
  check(a.get_internal()->count == 0, "arena reset sized: count zeroed");
  check(a.get_internal()->cap == 8, "arena reset sized: new cap");
}

static void test_arena_reset_default() {
  StringQOL::Arena a(4);
  a.add_string("one");

  check(a.reset() == SQOL_SUCCESS, "arena reset default: success");
  check(a.get_internal()->count == 0, "arena reset default: count 0");
  check(a.get_internal()->cap == SQOL_ARENA_DEFAULT_CAP_VALUE,
        "arena reset default: cap");
}

// ---------------------------------------------------------------------------

int main() {
  test_string_construct();
  test_string_append_char();
  test_string_append_cstr();
  test_string_append_string();
  test_string_copy();
  test_string_copy_to();
  test_string_replace_cstr();
  test_string_replace_string();
  test_string_reset();
  test_string_compare_cstr();
  test_string_compare_string();
  test_string_backspace();
  test_string_peek_consume_match();
  test_string_add_to_arena();
  test_arena_default_construct();
  test_arena_sized_construct();
  test_arena_construct_zero_cap();
  test_arena_add_string_obj();
  test_arena_add_string_cstr();
  test_arena_growth();
  test_arena_reset_sized();
  test_arena_reset_default();

  printf("\n%d passed, %d failed\n", passed, failed);
  return failed > 0 ? 1 : 0;
}