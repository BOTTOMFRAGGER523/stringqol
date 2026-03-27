// SPDX-License-Identifier: Unlicense
#ifndef SQOL_TYPES
#define SQOL_TYPES

#include "config.h"

// Types
#define SQOL_STATUS unsigned char
// #define SQOL_BOOL   unsigned char
#define SQOL_TRUE 1
#define SQOL_FALSE 0

#define SQOL_SUCCESS 1
#define SQOL_FAILURE 0

typedef struct String {
  char *string;
  // Note to self: s->string[s->size] should always point to the NULL terminator
  SQOL_SIZE size;
} String;

typedef struct StringArena {
  String **strings;
  // Note to self: a->strings[a->count] should always point to the next slot
  SQOL_SIZE count;
  SQOL_SIZE cap;
} StringArena;

#endif