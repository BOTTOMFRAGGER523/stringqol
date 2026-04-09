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

// Creates a new string
// @param str String literal
String *new_string(const char *str);

// Appends a char to the string
// @param s String that's the one getting appended
// @param ch Char that is to be appended to `s`
SQOL_STATUS string_append(String *s, char ch);

// Appends a string literal to `s`
// @param s String that's the one getting appended
// @param str String literal that is to be appended to `s`
SQOL_STATUS string_append_str(String *s, const char *str);

// Appends `src` to `dst`
// @param dst Destination string
// @param src Source string
SQOL_STATUS string_append_string(String *dst, String *src);

// Copies a string from `src` to `dst`
// @param dst Destination string
// @param src Source string
SQOL_STATUS string_cpy(String *dst, String *src);

// Replaces `s` with a string literal
// @param s String to be replaced
// @param new_str String literal that is replacing
SQOL_STATUS string_replace(String *s, const char *new_str);

// Resets `s` to a clean slate
// @param s String to be reset
SQOL_STATUS string_reset(String *s);

// Compares `s` with a string literal
// @param s String to be compared against `str`
// @param str String literal to be compared against `s`
SQOL_BOOL string_compare(String *s, const char *str);

// Compares `s` with another string object
// @param s String to be compared against `string`
// @param string String to be compared against `s`
SQOL_BOOL string_compare_string(String *s, String *string);

// Removes the top character of `s`
// @param s String to have the top character removed
SQOL_STATUS string_backspace(String *s);

// Peeks the current character and returns it.
// @param s String to be peeked
char string_peek(String *s);

// Peeks the next character and returns it.
// If out of bounds then it returns \0
// @param s String to be peeked
char string_peek_next(String *s);

// Consumes/Advances to the next character and returns the current character
// before consuming/advancing, returns \0 if out of bounds
// @param s String to be consumed/advanced
char string_consume(String *s);

// Matches the next character, if both match it consumes/advances and returns
// true if not it returns false and doesn't consume/advance
// @param s String to be matched against `ch`
// @param ch Char to be matched against the next character in `s`
SQOL_BOOL string_match(String *s, char ch);
#ifdef SQOL_ARENA_INCLUDED
// Adds `str` to `arena` and transfers ownership the the arena. Should not call
// `delete_string` after calling this
// @param s String to be added to `arena`
// @param arena Arena to be getting `s`
String *string_add_to_arena(String *s, StringArena *arena);
#endif

// Deletes the string
// @param string String to be deleted
SQOL_STATUS delete_string(String *string);

// Arena Interface
#ifdef SQOL_ARENA_INCLUDED
// Creates a new String Arena
// @param cap The starting cap/limit of the arena
StringArena *new_string_arena(SQOL_SIZE cap);

// Adds a string to the String Arena
// @param a The arena to be getting a string
// @param str The string to be added to the string arena
String *arena_add_string(StringArena *a, String *str);

// Frees all the string and resets the arena
// @param a The arena to be resetted
// @param new_cap The new cap/limit of the arena
SQOL_STATUS arena_reset(StringArena *a, SQOL_SIZE new_cap);

// Deletes the arena and all its strings.
// @param a The arena to be deleted
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
static SQOL_SIZE sqol_strlen(const char *str) {
  NULL_CHECK(str, SQOL_FAILURE)

  SQOL_SIZE i = 0;
  while (str[i] != '\0') {
    i++;
  }

  return i;
}

static inline size_t next_power_of_2(size_t x) {
#if SIZE_MAX == UINT64_MAX
  if (x == 0)
    return 1; // edge case
  x--;        // subtract 1 to handle exact powers
  x |= x >> 1;
  x |= x >> 2;
  x |= x >> 4;
  x |= x >> 8;
  x |= x >> 16;
  x |= x >> 32;
  x++; // now x is next power of 2 ≥ original
  return x;
#elif SIZE_MAX == UINT32_MAX
  if (x == 0)
    return 1; // edge case
  x--;        // subtract 1 to handle exact powers
  x |= x >> 1;
  x |= x >> 2;
  x |= x >> 4;
  x |= x >> 8;
  x |= x >> 16;
  x++; // now x is next power of 2 >= original
  return x;
#else
#error Unsupported SIZE_MAX
#endif
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
  s->cap = next_power_of_2(len);
  s->arena_owned = SQOL_FALSE;
  s->cursor = 0;
  return s;
}

SQOL_STATUS string_append(String *s, char ch) {
  NULL_CHECK(s, SQOL_FAILURE)
  if (ch == '\0')
    return SQOL_FAILURE;

#ifndef SQOL_SHOULD_INCREMENT
  if (s->size + 1 >= s->cap) {
    s->cap *= 2;
    s->cap = next_power_of_2(s->cap);
#else
  s->cap += 2;
#endif
    char *temp = (char *)SQOL_REALLOC(s->string, s->cap);
    NULL_CHECK(temp, SQOL_FAILURE)
    s->string = temp;
#ifndef SQOL_SHOULD_INCREMENT
  }
#endif

  s->string[s->size] = ch;
  s->size++;
  s->string[s->size] = '\0';

  return SQOL_SUCCESS;
}

SQOL_STATUS string_append_str(String *s, const char *str) {
  NULL_CHECK(s, SQOL_FAILURE)
  NULL_CHECK(str, SQOL_FAILURE)
  SQOL_SIZE append_len = sqol_strlen(str);

#ifndef SQOL_SHOULD_INCREMENT
  if (s->size + append_len >= s->cap) {
    s->cap *= 2;
    while (s->cap <= s->size + append_len) {
      s->cap *= 2;
    }
    s->cap = next_power_of_2(s->cap);
#else
  s->cap += append_len + 2;
#endif
    char *temp = (char *)SQOL_REALLOC(s->string, s->cap);
    NULL_CHECK(temp, SQOL_FAILURE)
    s->string = temp;
#ifndef SQOL_SHOULD_INCREMENT
  }
#endif

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
  if (dst->cap <= src->size) {
#ifndef SQOL_SHOULD_INCREMENT
    dst->cap *= 2;
    // Edge case
    while (dst->cap <= src->size) {
      dst->cap *= 2;
    }

    dst->cap = next_power_of_2(dst->cap);
#else
    dst->cap += 1 + src->cap;
#endif
    char *temp = (char *)SQOL_REALLOC(dst->string, dst->cap);
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

SQOL_STATUS string_reset(String *s) {
  NULL_CHECK(s, SQOL_FAILURE)

  // Free old string if not arena managed
  if (s->string && !s->arena_owned) {
    SQOL_FREE(s->string);
  }

  // Allocate 256 bytes by default
  char *temp = (char *)SQOL_MALLOC(SQOL_ARENA_DEFAULT_CAP_VALUE);
  NULL_CHECK(temp, SQOL_FAILURE)

  s->string = temp;
  s->size = 0;
  s->cap = SQOL_ARENA_DEFAULT_CAP_VALUE;
  s->cursor = 0;
  return SQOL_SUCCESS;
}

SQOL_STATUS string_compare(String *s, const char *str) {
  NULL_CHECK(s, SQOL_FAILURE)
  NULL_CHECK(s->string, SQOL_FAILURE)
  NULL_CHECK(str, SQOL_FAILURE)

  if (strcmp(s->string, str) == 0) {
    return SQOL_SUCCESS;
  }

  return SQOL_FAILURE;
}

SQOL_STATUS string_compare_string(String *s, String *string) {
  NULL_CHECK(s, SQOL_FAILURE)
  NULL_CHECK(s->string, SQOL_FAILURE)
  NULL_CHECK(string, SQOL_FAILURE)

  if (strcmp(s->string, string->string) == 0) {
    return SQOL_SUCCESS;
  }

  return SQOL_FAILURE;
}

SQOL_STATUS string_backspace(String *s) {
  NULL_CHECK(s, SQOL_FAILURE)
  NULL_CHECK(s->string, SQOL_FAILURE)
  if (s->size == 0)
    return SQOL_FAILURE;
  s->size--;
  s->string[s->size] = '\0';
  return SQOL_SUCCESS;
}

char string_peek(String *s) {
  NULL_CHECK(s, SQOL_FAILURE)
  NULL_CHECK(s->string, SQOL_FAILURE)
  return s->string[s->cursor];
}

char string_peek_next(String *s) {
  NULL_CHECK(s, SQOL_FAILURE)
  NULL_CHECK(s->string, SQOL_FAILURE)
  if (s->cursor < s->size) {
    return s->string[s->cursor + 1];
  }

  return '\0';
}

char string_consume(String *s) {
  NULL_CHECK(s, SQOL_FAILURE)
  NULL_CHECK(s->string, SQOL_FAILURE)
  if (s->cursor < s->size) {
    return s->string[s->cursor++];
  }

  return '\0';
}

SQOL_BOOL string_match(String *s, char ch) {
  NULL_CHECK(s, SQOL_FAILURE)
  NULL_CHECK(s->string, SQOL_FAILURE)
  if (string_peek(s) == ch) {
    string_consume(s);
    return SQOL_TRUE;
  }

  return SQOL_FALSE;
}

#ifdef SQOL_ARENA_INCLUDED
String *string_add_to_arena(String *s, StringArena *a) {
  return arena_add_string(a, s);
}
#endif

SQOL_STATUS delete_string(String *s) {
  NULL_CHECK(s, SQOL_FAILURE)

  if (!s->arena_owned) {
    if (s->string)
      SQOL_FREE(s->string);
    SQOL_FREE(s);

    return SQOL_SUCCESS;
  }

  return SQOL_FAILURE;
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

  s->arena_owned = SQOL_TRUE;
  return a->strings[a->count - 1];
}

SQOL_STATUS arena_reset(StringArena *a, SQOL_SIZE cap) {
  NULL_CHECK(a, SQOL_FAILURE)
  NULL_CHECK(a->strings, SQOL_FAILURE);

  for (size_t i = 0; i < a->count; i++) {
    if (a->strings[i]) {
      SQOL_FREE(a->strings[i]->string);
      SQOL_FREE(a->strings[i]);
    }
  }

  SQOL_FREE(a->strings);
  a->strings = (String **)SQOL_MALLOC(sizeof(String *) * cap);
  a->cap = cap;
  a->count = 0;

  return SQOL_SUCCESS;
}

SQOL_STATUS delete_arena(StringArena *a) {
  NULL_CHECK(a, SQOL_FAILURE)
  NULL_CHECK(a->strings, SQOL_FAILURE)

  for (size_t i = 0; i < a->count; i++) {
    if (a->strings[i]) {
      SQOL_FREE(a->strings[i]->string);
      SQOL_FREE(a->strings[i]);
    }
  }

  SQOL_FREE(a->strings);
  SQOL_FREE(a);

  return SQOL_SUCCESS;
}

#ifdef __cplusplus
}
#endif

#endif

#endif