// ----------------------------------------------------------------------
// SPDX-License-Identifier: Unlicense
// C++ Wrapper around the C API to be more OOP oriented.
// ----------------------------------------------------------------------
// Unless you don't have access to the C++ standard library, Please use
// std::string instead.
// ----------------------------------------------------------------------

#ifndef STRING_QOL_CPP
#define STRING_QOL_CPP
#ifndef __cplusplus
#error "Include stringqol.h instead!"
#endif

#include "config.h"
#include "stringqol.h"
#include "types.h"

namespace StringQOL {
// Forward declarations because of a mutual dependency
class String;
#ifdef SQOL_ARENA_INCLUDED
class Arena;
#endif

class String {
private:
  ::String *internal;

public:
  String(const char *str);
  SQOL_STATUS append(char ch);
  SQOL_STATUS append(char *str);
  SQOL_STATUS append(StringQOL::String &string);
  SQOL_STATUS copy(StringQOL::String &src);
  SQOL_STATUS copy_to(StringQOL::String &dst);
  SQOL_STATUS replace(const char *str);
  SQOL_STATUS replace(StringQOL::String &src);
#ifdef SQOL_ARENA_INCLUDED
  StringQOL::String add_to_arena(StringQOL::Arena &a);
#endif

  // Do not touch unless you want to access the internal state itself!
  ::String *get_internal();
  ~String();
};

#ifdef SQOL_ARENA_INCLUDED
class Arena {
private:
  ::StringArena *internal;

public:
  Arena(SQOL_SIZE cap);
  Arena();
  StringQOL::String add_string(StringQOL::String &str);
  StringQOL::String add_string(const char *str);
  SQOL_STATUS reset(SQOL_SIZE new_cap);
  SQOL_STATUS reset();

  // Do not touch unless you want to access the internal state itself!
  ::StringArena *get_internal();
  ~Arena();
};
#endif
} // namespace StringQOL

// Should only be defined here if you're editing/developing this file
// #define STRING_QOL_CPP_IMPL

// Single header implementation
#ifdef STRING_QOL_CPP_IMPL

// String class
StringQOL::String::String(const char *str) { internal = new_string(str); }

SQOL_STATUS StringQOL::String::append(char ch) {
  return string_append(internal, ch);
}

SQOL_STATUS StringQOL::String::append(char *str) {
  return string_append_str(internal, str);
}

SQOL_STATUS StringQOL::String::append(StringQOL::String &src) {
  return string_append_string(internal, src.get_internal());
}

SQOL_STATUS StringQOL::String::copy(StringQOL::String &src) {
  return string_cpy(internal, src.get_internal());
}

SQOL_STATUS StringQOL::String::copy_to(StringQOL::String &dst) {
  return string_cpy(dst.get_internal(), internal);
}

SQOL_STATUS StringQOL::String::replace(const char *str) {
  return string_replace(internal, str);
}

SQOL_STATUS StringQOL::String::replace(StringQOL::String &dst) {
  return string_replace(internal, dst.get_internal()->string);
}

#ifdef SQOL_ARENA_INCLUDED
StringQOL::String StringQOL::String::add_to_arena(StringQOL::Arena &a) {
  ::String *str = string_add_to_arena(internal, a.get_internal());

  if (!str) {
    return nullptr;
  }

  return StringQOL::String(str->string);
}
#endif

::String *StringQOL::String::get_internal() { return internal; }

StringQOL::String::~String() { delete_string(internal); }

// Arena class

StringQOL::Arena::Arena(SQOL_SIZE cap) {
  if (cap < 1) {
    cap = SQOL_ARENA_DEFAULT_CAP_VALUE;
  }
  internal = new_string_arena(cap);
}

StringQOL::Arena::Arena() {
  internal = new_string_arena(SQOL_ARENA_DEFAULT_CAP_VALUE);
}

StringQOL::String StringQOL::Arena::add_string(StringQOL::String &str) {
  return StringQOL::String(
      arena_add_string(internal, str.get_internal())->string);
}

StringQOL::String StringQOL::Arena::add_string(const char *str) {
  return StringQOL::String(arena_add_string(internal, new_string(str))->string);
}

SQOL_STATUS StringQOL::Arena::reset(SQOL_SIZE cap) {
  return arena_reset(internal, cap);
}

SQOL_STATUS StringQOL::Arena::reset() {
  return arena_reset(internal, SQOL_ARENA_DEFAULT_CAP_VALUE);
}

::StringArena *StringQOL::Arena::get_internal() { return internal; }

StringQOL::Arena::~Arena() { delete_arena(internal); }
#endif
#endif