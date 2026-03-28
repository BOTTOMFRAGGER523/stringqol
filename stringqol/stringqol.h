// -------------------------------------------------------------------------
// SPDX-License-Identifier: Unlicense
// -------------------------------------------------------------------------
// A simple & minimal String QoL (Quality of Life) single header library
// designed to be memory safe while abstracting all the unsafe and tedious
// string work commonly found in C.
// -------------------------------------------------------------------------

#ifndef STRING_QOL
#define STRING_QOL

#include "config.h"
#include "types.h"

#ifdef __cplusplus
extern "C" {
#ifndef STRING_QOL_CPP
#warning stringqol.h: Include stringqol.hpp instead!
#endif
#endif

// Interface

String *new_string(const char *str);
SQOL_STATUS string_append(String *s, char ch);
SQOL_STATUS string_append_str(String *s, char *str);
SQOL_STATUS string_append_string(String *dst, String *src);
SQOL_STATUS string_cpy(String *dst, String *src);
SQOL_STATUS string_replace(String *s, const char *new_str);
#ifdef SQOL_ARENA_INCLUDED
String *string_add_to_arena(String *str, StringArena *arena);
#endif
SQOL_STATUS delete_string(String *string);

// Arena Interface
#ifdef SQOL_ARENA_INCLUDED
StringArena *new_string_arena(SQOL_SIZE cap);
String *arena_add_string(StringArena *a, String *str);
SQOL_STATUS arena_reset(StringArena *a, SQOL_SIZE new_cap);
SQOL_STATUS delete_arena(StringArena *a);
#endif

#ifdef __cplusplus
}
#endif

// Should only be uncommented when editing/developing
// #define STRING_QOL_IMPL

// Implementation
#ifdef STRING_QOL_IMPL

#ifdef __cplusplus
extern "C" {
#endif

#define NULL_CHECK(x, y)                                                       \
  if (!x)                                                                      \
    return y;

// Note: rvalue of sqol_strlen == s->size
// Note: if 0 is returned either the string is empty or STR is NULL
SQOL_SIZE sqol_strlen(const char *str) {
  NULL_CHECK(str, SQOL_FAILURE)

  SQOL_SIZE i = 0;
  while (str[i] != '\0') {
    i++;
  }

  return i;
}

// Implementation of strings
String *new_string(const char *str) {
  NULL_CHECK(str, SQOL_FAILURE)

  String *s = (String *)SQOL_MALLOC(sizeof(String));
  NULL_CHECK(s, NULL)

  SQOL_SIZE len = sqol_strlen(str);
  s->string = (char *)SQOL_MALLOC(len + 1);
  SQOL_MEMCPY(s->string, (void *)str, len);
  s->string[len] = '\0';

  // Note to self: s->string[s->size] should always point to the NULL terminator
  s->size = len;
  return s;
}

SQOL_STATUS string_append(String *s, char ch) {
  NULL_CHECK(s, SQOL_FAILURE)
  if (ch == '\0')
    return SQOL_FAILURE;

  char *temp = (char *)SQOL_REALLOC(s->string, s->size + 2);
  NULL_CHECK(temp, SQOL_FAILURE)
  s->string = temp;

  s->string[s->size] = ch;
  s->size++;
  s->string[s->size] = '\0';

  return SQOL_SUCCESS;
}

SQOL_STATUS string_append_str(String *s, char *str) {
  NULL_CHECK(s, SQOL_FAILURE)
  NULL_CHECK(str, SQOL_FAILURE)

  SQOL_SIZE append_len = sqol_strlen(str);
  char *temp = (char *)SQOL_REALLOC(s->string, s->size + append_len + 1);
  NULL_CHECK(temp, SQOL_FAILURE)
  s->string = temp;

  SQOL_MEMCPY(s->string + s->size, str, append_len);
  s->size += append_len;
  s->string[s->size] = '\0';

  return SQOL_SUCCESS;
}

SQOL_STATUS string_append_string(String *dst, String *src) {
  NULL_CHECK(dst, SQOL_FAILURE)
  NULL_CHECK(src, SQOL_FAILURE)

  return string_append_str(dst, src->string);
}

SQOL_STATUS string_cpy(String *dst, String *src) {
  NULL_CHECK(dst, SQOL_FAILURE)
  NULL_CHECK(src, SQOL_FAILURE)
  if (dst->size < src->size) {
    char *temp = (char *)SQOL_REALLOC(dst->string, src->size + 1);
    NULL_CHECK(temp, SQOL_FAILURE);
    dst->string = temp;
  }

  SQOL_MEMCPY(dst->string, src->string, src->size);
  dst->size = src->size;
  dst->string[dst->size] = 0;

  return SQOL_SUCCESS;
}

SQOL_STATUS string_replace(String *s, const char *new_str) {
  NULL_CHECK(s, SQOL_FAILURE)
  NULL_CHECK(new_str, SQOL_FAILURE)

  // We dont really care if s->string is NULL. That actually makes our job
  // easier
  if (s->string)
    SQOL_FREE(s->string);

  SQOL_SIZE len = sqol_strlen(new_str);
  s->size = len;
  s->string = (char *)SQOL_MALLOC(len + 1);
  SQOL_MEMCPY(s->string, (void *)new_str, s->size);
  s->string[s->size] = '\0';

  return SQOL_SUCCESS;
}

#ifdef SQOL_ARENA_INCLUDED
String *string_add_to_arena(String *s, StringArena *a) {
  return arena_add_string(a, s);
}
#endif

SQOL_STATUS delete_string(String *s) {
  NULL_CHECK(s, SQOL_FAILURE)

  SQOL_FREE(s->string);
  SQOL_FREE(s);

  return SQOL_SUCCESS;
}

// Arena implementation
StringArena *new_string_arena(SQOL_SIZE cap) {
  StringArena *a = (StringArena *)SQOL_MALLOC(sizeof(StringArena));
  NULL_CHECK(a, NULL)
  if (!cap) {
    cap = SQOL_ARENA_DEFAULT_CAP_VALUE;
  }
  a->strings = (String **)SQOL_MALLOC(sizeof(String *) * cap);
  NULL_CHECK(a->strings, NULL)
  // Note to self: a->strings[a->count] should always point to the next slot
  a->count = 0; // none yet.
  a->cap = cap;

  return a;
}

String *arena_add_string(StringArena *a, String *s) {
  NULL_CHECK(a, SQOL_FAILURE)
  NULL_CHECK(s, SQOL_FAILURE)

  if (a->count < a->cap) {
    a->count++;
    a->strings[a->count - 1] = s;
  } else {
    a->cap += SQOL_ARENA_DEFAULT_CAP_INCREMENT;

    // Edge case
    while (a->count + 1 > a->cap) {
      a->cap += SQOL_ARENA_DEFAULT_CAP_INCREMENT;
    }

    String **temp =
        (String **)SQOL_REALLOC(a->strings, sizeof(String *) * a->cap);
    NULL_CHECK(temp, SQOL_FAILURE)
    a->strings = temp;

    a->count++;
    a->strings[a->count - 1] = s;
  }

  return a->strings[a->count - 1];
}

SQOL_STATUS arena_reset(StringArena *a, SQOL_SIZE cap) {
  NULL_CHECK(a, SQOL_FAILURE)
  NULL_CHECK(a->strings, SQOL_FAILURE);

  SQOL_FREE(a->strings);
  a->strings = (String **)SQOL_MALLOC(sizeof(String *) * cap);
  a->cap = cap;
  a->count = 0;

  return SQOL_SUCCESS;
}

SQOL_STATUS delete_arena(StringArena *a) {
  NULL_CHECK(a, SQOL_FAILURE)
  NULL_CHECK(a->strings, SQOL_FAILURE)

  SQOL_FREE(a->strings);
  SQOL_FREE(a);

  return SQOL_SUCCESS;
}

#ifdef __cplusplus
}
#endif

#endif

#endif