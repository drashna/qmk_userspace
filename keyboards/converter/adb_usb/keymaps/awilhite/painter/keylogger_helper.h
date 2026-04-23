// Copyright Pablo Martinez (@elpekenin) <elpekenin@elpekenin.dev>
// SPDX-License-Identifier: GPL-2.0-or-later

/**
 * Result/Option types. Expresively express type of operations that can fail or return nothing.
 */

// -- barrier --

#pragma once

#include <quantum/util.h>
#include <stdbool.h>
#include <sys/cdefs.h>

#include "printf/printf.h"

#define _Result(T, E) result___##T##___##E

/**
 * Define a new result type.
 *
 * Args:
 *     T: The type of values.
 *     E: The type of errors.
 */
#define ResultImpl(T, E)    \
    typedef struct PACKED { \
        bool is_ok;         \
        union {             \
            T __value;      \
            E __error;      \
        };                  \
    } Result(T, E)

/**
 * Result type.
 *
 * :c:member:`is_ok`: Whether this result is a value or error.
 */
#define Result(T, E) _Result(T, E)

/**
 * Create a result with the given value.
 */
#define Ok(T, E, v)                    \
    (Result(T, E)) {                   \
        .is_ok = true, .__value = (v), \
    }

/**
 * Create a result instance with the given error.
 */
#define Err(T, E, e)                    \
    (Result(T, E)) {                    \
        .is_ok = false, .__error = (e), \
    }

/**
 * ----
 */

// -- barrier --

#define _Option(T) option___##T

/**
 * Define a new option type.
 *
 * Args:
 *     T: The type of values.
 */
#define OptionImpl(T)       \
    typedef struct PACKED { \
        bool is_some;       \
        T    __value;       \
    } Option(T)

/**
 * Option type.
 *
 * :c:member:`is_some`: Whether this option contains a value.
 */
#define Option(T) _Option(T)

/**
 * Create an option with the given value.
 */
#define Some(T, v)                       \
    (Option(T)) {                        \
        .is_some = true, .__value = (v), \
    }

/**
 * Create an empty option.
 */
#define None(T)           \
    (Option(T)) {         \
        .is_some = false, \
    }

/**
 * ----
 */

// -- barrier --

_Noreturn static inline void raise_error(const char *msg) {
    printf("[ERROR] %s\n", msg);
    while (true) {
    }
}

/**
 * Get the inner value from an `Ok`/`Some` value. Panic if `Err`/`None`.
 */
#define unwrap(v)                                              \
    ({                                                         \
        /* is_ok / is_some */                                  \
        /* relies on them being first element of struct */     \
        bool *ptr = (bool *)&(v);                              \
        if (!(*ptr)) {                                         \
            raise_error("called `unwrap` on `Err` or `None`"); \
        }                                                      \
                                                               \
        (v).__value;                                           \
    })

/**
 * Get the inner value from an `Err`. Panic if `Ok`.
 */
#define unwrap_err(v)                                   \
    ({                                                  \
        if ((v).is_ok) {                                \
            raise_error("called `unwrap_err` on `Ok`"); \
        }                                               \
                                                        \
        (v).__error;                                    \
    })
