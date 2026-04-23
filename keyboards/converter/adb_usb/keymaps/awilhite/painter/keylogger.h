// Copyright Pablo Martinez (@elpekenin) <elpekenin@elpekenin.dev>
// SPDX-License-Identifier: GPL-2.0-or-later

/**
 * Utility to track last keys pressed in a string.
 *
 * This could later be shown on a screen, for example.
 */

// -- barrier --

#pragma once

#ifndef DISPLAY_KEYLOGGER_LENGTH
#    define DISPLAY_KEYLOGGER_LENGTH 25
#endif

#include <quantum/quantum.h>

/**
 * Hook into :c:func:`process_record_user` that performs the tracking.
 */
void keylogger_process(uint16_t keycode, keyrecord_t *record);

/**
 * Read the current state of the keylog.
 */
const char *get_keylogger_str(void);

const int32_t *get_keylogger_str_raw(void);

/**
 * Whether anything has been written since keylog was last fetched.
 */
bool is_keylogger_dirty(void);

/**
 * Set the keylog dirty state.
 *
 * This is used to force a redraw of the keylogger string.
 */
void keylogger_set_dirty(bool dirty);

/**
 * Takes a basic string representation of a keycode and
 * replace it with a prettier one. Eg: ``KC_A`` becomes ``A``
 */
void keycode_repr(const char **str, const uint8_t mods);

void        split_sync_keylogger_str(const uint8_t *data, uint8_t size);
const char *get_keyode_character(uint16_t keycode, keypos_t *key);
void        add_keycode_to_keylogger_str(uint8_t keycode, uint8_t mods);
void        keylog_shift_right(void);

uint16_t extract_basic_keycode(uint16_t keycode, keyrecord_t *record, bool check_hold);
uint16_t extract_non_basic_keycode(uint16_t keycode, keyrecord_t *record, bool check_hold);
