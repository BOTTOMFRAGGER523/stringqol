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

static void bench_construct_destruct() {
  BENCH("construct+destruct x" ITER_STR, {
    for (int i = 0; i < ITERATIONS; i++) {
      StringQOL::String s((char *)"benchmark string");
      (void)s;
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
}

static void bench_arena_add_cstr() {
  BENCH("arena add cstr x" ITER_STR, {
    StringQOL::Arena a(ITERATIONS);
    for (int i = 0; i < ITERATIONS; i++)
      a.add_string("arena test");
  });
}

int main() {
  printf("=== C++ Benchmarks (%d iterations) ===\n\n", ITERATIONS);
  bench_append_char();
  bench_append_cstr();
  bench_replace_cstr();
  bench_construct_destruct();
  bench_copy();
  bench_arena_add_cstr();
  printf("\n");
  return 0;
}