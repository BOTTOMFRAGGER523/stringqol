// SPDX-License-Identifier: Unlicense
// Default settings
#ifndef SQOL_CONFIG
#define SQOL_CONFIG

// Sets memory functions
#define SQOL_MALLOC malloc
#define SQOL_FREE free
#define SQOL_REALLOC realloc
#define SQOL_MEMCPY memcpy

// Use either the std lib or non-std lib
#define SQOL_USE_STD_LIB
// #define USE_NONSTD_LIB

// SQOL configurations for arena
#define SQOL_ARENA_INCLUDED // Includes the arena

// clang-format off
#ifdef SQOL_ARENA_INCLUDED
    #define SQOL_ARENA_CAN_INCREASE_CAP                 // Configures whether or not the arena can increase the cap
    #define SQOL_ARENA_DEFAULT_CAP_VALUE 16             // The default cap value if left unspecified
    #ifdef SQOL_ARENA_CAN_INCREASE_CAP
        #define SQOL_ARENA_DEFAULT_CAP_INCREMENT 2      // How much the arena increments the cap by
            #if defined(SQOL_ARENA_DEFAULT_CAP_INCREMENT) &&                               \
                SQOL_ARENA_DEFAULT_CAP_INCREMENT < 1
            #error "SQOL_ARENA_DEFAULT_CAP_INCREMENT IS BELOW 1!"
        #endif
    #endif
#endif
// clang-format on

// Std lib includes (don't touch)
#ifdef SQOL_USE_STD_LIB
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

// define SQOL_SIZE
#define SQOL_SIZE size_t
#endif

// Nonstd lib includes go here
#ifdef SQOL_USE_NONSTD_LIB
#ifndef SQOL_SIZE
#error "Define SQOL_SIZE!"
#endif
// Includes and the definition of SQOL_SIZE go here

#endif
#endif