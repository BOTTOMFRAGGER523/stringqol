// SPDX-License-Identifier: Unlicense
#define STRING_QOL_IMPL
#define STRING_QOL_CPP_IMPL
#include "stringqol.hpp"

#include <cstdio>
#include <time.h>

#define ITERATIONS 100000
#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)
#define ITER_STR TOSTRING(ITERATIONS)

static double elapsed_ms(struct timespec start, struct timespec end) {
  return (end.tv_sec - start.tv_sec) * 1000.0 +
         (end.tv_nsec - start.tv_nsec) / 1e6;
}

#define BENCH(label, code)                                                     \
  do {                                                                         \
    struct timespec t0, t1;                                                    \
    clock_gettime(CLOCK_MONOTONIC, &t0);                                       \
    code clock_gettime(CLOCK_MONOTONIC, &t1);                                  \
    printf("%-40s %.3f ms\n", label, elapsed_ms(t0, t1));                      \
  } while (0)

static void bench_append_char() {
  BENCH("append char x" ITER_STR, {
    StringQOL::String s((char *)"");
    for (int i = 0; i < ITERATIONS; i++)
      s.append('a');
  });
}

static void bench_append_cstr() {
  BENCH("append cstr x" ITER_STR, {
    StringQOL::String s((char *)"");
    for (int i = 0; i < ITERATIONS; i++)
      s.append((char *)"hello");
  });
}

static void bench_replace_cstr() {
  BENCH("replace cstr x" ITER_STR, {
    StringQOL::String s((char *)"initial");
    for (int i = 0; i < ITERATIONS; i++)
      s.replace("some replacement string");
  });
}

static void bench_replace_string() {
  BENCH("replace string x" ITER_STR, {
    StringQOL::String replacement((char *)"some replacement string");
    StringQOL::String s((char *)"initial");
    for (int i = 0; i < ITERATIONS; i++)
      s.replace(replacement);
  });
}

static void bench_construct_destruct() {
  BENCH("construct+destruct x" ITER_STR, {
    for (int i = 0; i < ITERATIONS; i++) {
      StringQOL::String s((char *)"benchmark string");
      (void)s;
    }
  });
}

static void bench_append_string_object() {
  BENCH("append string object x" ITER_STR, {
    StringQOL::String src((char *)"abcdefghij");
    StringQOL::String dst((char *)"");
    for (int i = 0; i < ITERATIONS; i++) {
      dst.append(src);
      dst.reset();
    }
  });
}

static void bench_copy() {
  StringQOL::String large((char *)"");
  for (int i = 0; i < 1000; i++)
    large.append((char *)"abcdefghij");

  BENCH("copy large string x" ITER_STR, {
    StringQOL::String dst((char *)"");
    for (int i = 0; i < ITERATIONS; i++)
      dst.copy(large);
  });

  BENCH("copy_to large string x" ITER_STR, {
    StringQOL::String dst((char *)"benchmark");
    for (int i = 0; i < ITERATIONS; i++)
      large.copy_to(dst);
  });
}

static void bench_reset() {
  BENCH("reset x" ITER_STR, {
    StringQOL::String s((char *)"abcdefghijklmnopqrstuvwxyz");
    for (int i = 0; i < ITERATIONS; i++) {
      s.reset();
      s.append((char *)"abcdefghijklmnopqrstuvwxyz");
    }
  });
}

static void bench_compare() {
  BENCH("compare literal x" ITER_STR, {
    StringQOL::String s((char *)"benchmark string");
    for (int i = 0; i < ITERATIONS; i++)
      s.compare((char *)"benchmark string");
  });

  BENCH("compare string x" ITER_STR, {
    StringQOL::String s((char *)"benchmark string");
    StringQOL::String other((char *)"benchmark string");
    for (int i = 0; i < ITERATIONS; i++)
      s.compare(other);
  });
}

static void bench_backspace(void) {
  BENCH("backspace x" ITER_STR, {
    StringQOL::String s((char *)"abcdefghijklmnopqrstuvwxyz0123456789");
    for (int i = 0; i < ITERATIONS; i++) {
      s.reset();
      s.replace((char *)"abcdefghijklmnopqrstuvwxyz0123456789");
      for (int j = 0; j < 36; j++)
        s.backspace();
    }
  });
}

static void bench_peek_consume_match(void) {
  BENCH("peek/consume/match x" ITER_STR, {
    StringQOL::String s((char *)"abcdefghijklmnopqrstuvwxyz");
    for (int i = 0; i < ITERATIONS; i++) {
      s.reset();
      s.replace((char *)"abcdefghijklmnopqrstuvwxyz");
      while (s.peek() != '\0')
        s.match(s.peek());
    }
  });
}

static void bench_arena_add_string(void) {
  BENCH("arena add string x" ITER_STR, {
    StringQOL::Arena a(ITERATIONS);
    for (int i = 0; i < ITERATIONS; i++)
      a.add_string((char *)"arena test");
  });
}

static void bench_arena_add_string_object(void) {
  BENCH("arena add string object x" ITER_STR, {
    StringQOL::Arena a(ITERATIONS);
    StringQOL::String src((char *)"arena test");
    for (int i = 0; i < ITERATIONS; i++)
      a.add_string(src);
  });
}

static void bench_arena_reset(void) {
  BENCH("arena reset x" ITER_STR, {
    StringQOL::Arena a(256);
    for (int i = 0; i < ITERATIONS; i++) {
      a.add_string((char *)"arena reset test");
      if ((i & 0xFF) == 0)
        a.reset(256);
    }
  });
}

int main() {
  printf("=== C++ Benchmarks (%d iterations) ===\n\n", ITERATIONS);
  bench_append_char();
  bench_append_cstr();
  bench_append_string_object();
  bench_replace_cstr();
  bench_replace_string();
  bench_construct_destruct();
  bench_copy();
  bench_reset();
  bench_compare();
  bench_backspace();
  bench_peek_consume_match();
  bench_arena_add_string();
  bench_arena_add_string_object();
  bench_arena_reset();
  printf("\n");
  return 0;
}