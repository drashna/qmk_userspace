// Copyright 2024 Christopher Courtney, aka Drashna Jael're  (@drashna) <drashna@live.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "action.h"
#include "action_util.h"

bool     mod_key_press_timer(uint16_t code, uint16_t mod_code, bool pressed);
bool     mod_key_press(uint16_t code, uint16_t mod_code, bool pressed, uint16_t *this_timer);
bool     hasAllBitsInMask(uint8_t value, uint8_t mask);
uint8_t  mod_config_8bit(uint8_t mod);
void     tap_code16_nomods(uint16_t kc);
void     center_text(const char *text, char *output, uint8_t width);
bool     is_device_suspended(void);
void     set_is_device_suspended(bool status);
uint16_t extract_basic_keycode(uint16_t keycode, keyrecord_t *record, bool check_hold);
uint16_t extract_non_basic_keycode(uint16_t keycode, keyrecord_t *record, bool check_hold);
mod_t    get_mods_state_from_keycode(uint16_t keycode);
uint8_t  get_mods_from_keycode(uint16_t keycode);
