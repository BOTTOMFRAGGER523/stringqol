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

// Check if its not a freestanding environment and warn the user to use
// std::string instead
#if defined(__STDC_HOSTED__) && __STDC_HOSTED__ == 1 ||                        \
    defined(__FREESTANDING__)
#warning "Use std::string instead!"
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
  // Creates a string object
  // @param str The string literal to be added to the string object
  String(const char *str);

  // Appends a char to the string object
  // @param ch The char to be appended
  SQOL_STATUS append(char ch);

  // Appends a string literal to the string object
  // @param str The string literal to be appended
  SQOL_STATUS append(char *str);

  // Appends another string object to the current string object
  // @param string The string to be appended
  SQOL_STATUS append(StringQOL::String &string);

  // Copies `src` to the current string object
  // @param src The string to be copied to the current string object
  SQOL_STATUS copy(StringQOL::String &src);

  // Copies the current string object to `dst`
  // @param dst The destination/target of the copy
  SQOL_STATUS copy_to(StringQOL::String &dst);

  // Replaces the current string with a string literal
  // @param str The string literal to be replacing
  SQOL_STATUS replace(const char *str);

  // Replaces the current string object with another string object
  // @param src The string object to be replacing
  SQOL_STATUS replace(StringQOL::String &src);
#ifdef SQOL_ARENA_INCLUDED
  // Adds the current string object to the arena object `a` and transfering
  // ownership to the arena
  // @param a The arena object receiving the string object
  StringQOL::String add_to_arena(StringQOL::Arena &a);
#endif

  // Returns the internal String C state.
  // Do not touch unless you want to access the internal C state itself!
  ::String *get_internal();

  // Deletes the string object
  ~String();
};

#ifdef SQOL_ARENA_INCLUDED
class Arena {
private:
  ::StringArena *internal;

public:
  // Creates an Arena object
  // @param cap The cap/limit of the arena
  Arena(SQOL_SIZE cap);

  // Creates an Arena object with the default cap value
  Arena();

  // Adds a string to the arena
  // @param str The string to be added to the arena
  StringQOL::String add_string(StringQOL::String &str);

  // Adds a string literal to the arena
  // @param str The string literal to be added to the arena
  StringQOL::String add_string(const char *str);

  // Resets the arena and sets the cap value to `new_cap`
  // @param new_cap The new cap value to be set as the cap value
  SQOL_STATUS reset(SQOL_SIZE new_cap);

  // Resets the arena and uses the default cap value
  SQOL_STATUS reset();

  // Do not touch unless you want to access the internal C state itself!
  ::StringArena *get_internal();

  // Deletes the arena object
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