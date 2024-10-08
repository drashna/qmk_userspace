// Copyright 2020 Christopher Courtney, aka Drashna Jael're  (@drashna) <drashna@live.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once
#include "quantum.h"

bool          process_record_user_rgb_light(uint16_t keycode, keyrecord_t *record);
void          keyboard_post_init_rgb_light(void);
void          housekeeping_task_rgb_light(void);
layer_state_t layer_state_set_rgb_light(layer_state_t state);
layer_state_t default_layer_state_set_rgb_light(layer_state_t state);
void          rgblight_sethsv_default_helper(uint8_t index);
void          rgblight_shutdown(bool jump_to_bootloader);

const char *rgblight_name(uint8_t effect);
const char *rgblight_get_effect_name(void);
bool        is_rgblight_startup_running(void);
bool        has_rgblight_config_changed(void);
