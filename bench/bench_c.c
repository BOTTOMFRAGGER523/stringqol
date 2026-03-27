// SPDX-License-Identifier: Unlicense
#define STRING_QOL_IMPL
#include "stringqol.h"
#include <stdio.h>
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

static void bench_append_char(void) {
  BENCH("append char x" ITER_STR, {
    String *s = new_string("");
    for (int i = 0; i < ITERATIONS; i++)
      string_append(s, 'a');
    delete_string(s);
  });
}

static void bench_append_cstr(void) {
  BENCH("append cstr x" ITER_STR, {
    String *s = new_string("");
    for (int i = 0; i < ITERATIONS; i++)
      string_append_str(s, "hello");
    delete_string(s);
  });
}

static void bench_replace(void) {
  BENCH("replace x" ITER_STR, {
    String *s = new_string("initial");
    for (int i = 0; i < ITERATIONS; i++)
      string_replace(s, "some replacement string");
    delete_string(s);
  });
}

static void bench_new_delete(void) {
  BENCH("new+delete x" ITER_STR, {
    for (int i = 0; i < ITERATIONS; i++) {
      String *s = new_string("benchmark string");
      delete_string(s);
    }
  });
}

static void bench_copy(void) {
  String *large = new_string("");
  for (int i = 0; i < 1000; i++)
    string_append_str(large, "abcdefghij");

  BENCH("copy large string x" ITER_STR, {
    String *dst = new_string("");
    for (int i = 0; i < ITERATIONS; i++)
      string_cpy(dst, large);
    delete_string(dst);
  });

  delete_string(large);
}

static void bench_arena_vs_manual(void) {
  BENCH("manual new+delete x" ITER_STR, {
    String *strings[ITERATIONS];
    for (int i = 0; i < ITERATIONS; i++)
      strings[i] = new_string("arena test");
    for (int i = 0; i < ITERATIONS; i++)
      delete_string(strings[i]);
  });

  BENCH("arena add x" ITER_STR, {
    StringArena *a = new_string_arena(ITERATIONS);
    for (int i = 0; i < ITERATIONS; i++) {
      String *s = new_string("arena test");
      arena_add_string(a, s);
    }
    for (SQOL_SIZE i = 0; i < a->count; i++)
      delete_string(a->strings[i]);
    delete_arena(a);
  });
}

int main(void) {
  printf("=== C Benchmarks (%d iterations) ===\n\n", ITERATIONS);
  bench_append_char();
  bench_append_cstr();
  bench_replace();
  bench_new_delete();
  bench_copy();
  bench_arena_vs_manual();
  printf("\n");
  return 0;
}