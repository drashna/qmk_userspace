// Copyright 2021 Christopher Courtney, aka Drashna Jael're  (@drashna) <drashna@live.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "quantum.h"

void          keyboard_post_init_keymap(void);
void          matrix_init_secret(void);
bool          shutdown_keymap(bool jump_to_bootloader);
void          suspend_power_down_keymap(void);
void          suspend_wakeup_init_keymap(void);
void          matrix_scan_keymap(void);
void          matrix_scan_secret(void);
layer_state_t layer_state_set_keymap(layer_state_t state);
layer_state_t default_layer_state_set_keymap(layer_state_t state);
void          led_set_keymap(uint8_t usb_led);
void          eeconfig_init_keymap(void);
void          housekeeping_task_keymap(void);

#ifdef SPLIT_KEYBOARD
void matrix_slave_scan_keymap(void);
#endif // SPLIT_KEYBOARD
