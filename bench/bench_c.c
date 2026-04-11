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

static void bench_append_string(void) {
  BENCH("append string x" ITER_STR, {
    String *src = new_string("abcdefghij");
    String *dst = new_string("");
    for (int i = 0; i < ITERATIONS; i++) {
      string_append_string(dst, src);
      string_reset(dst);
    }
    delete_string(dst);
    delete_string(src);
  });
}

static void bench_replace(void) {
  BENCH("replace literal x" ITER_STR, {
    String *s = new_string("initial");
    for (int i = 0; i < ITERATIONS; i++)
      string_replace(s, "some replacement string");
    delete_string(s);
  });
}

static void bench_replace_string(void) {
  BENCH("replace string x" ITER_STR, {
    String *source = new_string("some replacement string");
    String *s = new_string("initial");
    for (int i = 0; i < ITERATIONS; i++)
      string_replace(s, source->string);
    delete_string(s);
    delete_string(source);
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

static void bench_reset(void) {
  BENCH("reset x" ITER_STR, {
    String *s = new_string("abcdefghijklmnopqrstuvwxyz");
    for (int i = 0; i < ITERATIONS; i++) {
      string_reset(s);
      string_append_str(s, "abcdefghijklmnopqrstuvwxyz");
    }
    delete_string(s);
  });
}

static void bench_compare(void) {
  BENCH("compare literal x" ITER_STR, {
    String *s = new_string("benchmark string");
    for (int i = 0; i < ITERATIONS; i++)
      string_compare(s, "benchmark string");
    delete_string(s);
  });

  BENCH("compare string x" ITER_STR, {
    String *s = new_string("benchmark string");
    String *other = new_string("benchmark string");
    for (int i = 0; i < ITERATIONS; i++)
      string_compare_string(s, other);
    delete_string(s);
    delete_string(other);
  });
}

static void bench_backspace(void) {
  BENCH("backspace x" ITER_STR, {
    String *s = new_string("abcdefghijklmnopqrstuvwxyz0123456789");
    for (int i = 0; i < ITERATIONS; i++) {
      string_reset(s);
      string_replace(s, "abcdefghijklmnopqrstuvwxyz0123456789");
      for (int j = 0; j < 36; j++)
        string_backspace(s);
    }
    delete_string(s);
  });
}

static void bench_peek_consume_match(void) {
  BENCH("peek/consume/match x" ITER_STR, {
    String *s = new_string("abcdefghijklmnopqrstuvwxyz");
    for (int i = 0; i < ITERATIONS; i++) {
      string_reset(s);
      string_replace(s, "abcdefghijklmnopqrstuvwxyz");
      while (string_peek(s) != '\0')
        string_match(s, string_peek(s));
    }
    delete_string(s);
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

static void bench_arena(void) {
  BENCH("arena add x" ITER_STR, {
    StringArena *a = new_string_arena(ITERATIONS);
    for (int i = 0; i < ITERATIONS; i++) {
      String *s = new_string("arena test");
      arena_add_string(a, s);
    }
    delete_arena(a);
  });

  BENCH("arena reset x" ITER_STR, {
    StringArena *a = new_string_arena(256);
    for (int i = 0; i < ITERATIONS; i++) {
      String *s = new_string("arena reset test");
      arena_add_string(a, s);
      if ((i & 0xFF) == 0)
        arena_reset(a, 256);
    }
    delete_arena(a);
  });

  BENCH("arena add-to-arena x" ITER_STR, {
    StringArena *a = new_string_arena(ITERATIONS);
    for (int i = 0; i < ITERATIONS; i++) {
      String *s = new_string("arena test");
      string_add_to_arena(s, a);
    }
    delete_arena(a);
  });
}

int main(void) {
  printf("=== C Benchmarks (%d iterations) ===\n\n", ITERATIONS);
  bench_append_char();
  bench_append_cstr();
  bench_append_string();
  bench_replace();
  bench_replace_string();
  bench_new_delete();
  bench_reset();
  bench_compare();
  bench_backspace();
  bench_peek_consume_match();
  bench_copy();
  bench_arena();
  printf("\n");
  return 0;
}