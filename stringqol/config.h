// ---------------------------------------------------------------------------------------------------------------------------------------
// clang-format off
// ---------------------------------------------------------------------------------------------------------------------------------------
// Configuration file for StringQOL.
//
// Note: Settings marked with an asterisk at the end should be
//       edited if the environment is freestanding (aka no stdlib)
//       unless you have a very specific reason not to.
//
// Note: Settings indented should be considered a sub-setting
//       of the parent setting and if the parent setting is
//       disabled then consider every sub-setting of the parent disabled.
//
// Configurations:
//      SQOL_USE_STD_LIB: Flag that configures whether or not StringQOL uses the stdlib.
//      SQOL_ARENA_INCLUDED: Flag that configures whether or not the arena is included.
//          SQOL_ARENA_CAN_INCREASE_CAP: Flag that configures whether or not the arena can increase the cap/limit of allocations.
//              SQOL_ARENA_DEFAULT_CAP_INCREMENT: Sets how much the arena increments the cap each time it hits its limit.
//          SQOL_ARENA_DEFAULT_CAP_VALUE: Sets the default cap value for the arena if left to zero or undefined.
//      SQOL_SIZE*: Sets the macro for the type. It should be a size_t equivalent
//      SQOL_MALLOC*: Sets the function name for malloc().
//      SQOL_FREE*: Sets the function name for free().
//      SQOL_REALLOC*: Sets the function name for realloc().
//
// ---------------------------------------------------------------------------------------------------------------------------------------
// clang-format on
// ---------------------------------------------------------------------------------------------------------------------------------------

#ifndef SQOL_CONFIG
#define SQOL_CONFIG

// If defined it will use the standard library.
// clang-format off
#ifndef SQOL_USE_STD_LIB
    #define SQOL_USE_STD_LIB
#endif // !SQOL_USE_STDLIB

// Configurations for arena
#ifndef SQOL_ARENA_INCLUDED
    #define SQOL_ARENA_INCLUDED // Includes the arena
#endif // !SQOL_ARENA_INCLUDED
// clang-format on

// clang-format off
#ifdef SQOL_ARENA_INCLUDED
    #ifndef SQOL_ARENA_CAN_INCREASE_CAP
        #define SQOL_ARENA_CAN_INCREASE_CAP
    #endif // !SQOL_ARENA_CAN_INCREASE_CAP

    #ifndef SQOL_ARENA_DEFAULT_CAP_VALUE
        #define SQOL_ARENA_DEFAULT_CAP_VALUE 16
    #endif // !SQOL_ARENA_DEFAULT_CAP_VALUE

    #ifdef SQOL_ARENA_CAN_INCREASE_CAP
        #ifndef SQOL_ARENA_DEFAULT_CAP_INCREMENT
            #define SQOL_ARENA_DEFAULT_CAP_INCREMENT 5
        #endif // !SQOL_ARENA_DEFAULT_CAP_INCREMENT

        #if SQOL_ARENA_DEFAULT_CAP_INCREMENT < 1
            #error "SQOL_ARENA_DEFAULT_CAP_INCREMENT is below 1!"
        #endif // SQOL_ARENA_DEFAULT_CAP_INCREMENT < 1
    #endif // SQOL_ARENA_CAN_INCREASE_CAP
#endif // SQOL_ARENA_INCLUDED
// clang-format on

// Freestanding library includes go here
// clang-format off
#ifndef SQOL_USE_STD_LIB
#if !defined(SQOL_SIZE) || !defined(SQOL_MALLOC) || !defined(SQOL_FREE) ||     \
    !defined(SQOL_REALLOC) || !defined(SQOL_MEMCPY)
#error "Did you forget to define the macros for freestanding?"
#endif // !SQOL_SIZE or !SQOL_MALLOC or !SQOL_FREE or !SQOL_REALLOC or !SQOL_MEMCPY

// Includes and the definitions of macros go here

#endif
// clang-format on

// Standard library includes and default definitions (don't touch)
#ifdef SQOL_USE_STD_LIB
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

// Definitions
#define SQOL_SIZE size_t
#define SQOL_MALLOC malloc
#define SQOL_FREE free
#define SQOL_REALLOC realloc
#define SQOL_MEMCPY memcpy
#endif

#endif