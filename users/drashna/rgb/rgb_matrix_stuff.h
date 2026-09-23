// Copyright 2020 Christopher Courtney, aka Drashna Jael're  (@drashna) <drashna@live.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once
#include "quantum.h"

bool process_record_user_rgb_matrix(uint16_t keycode, keyrecord_t *record);
void keyboard_post_init_rgb_matrix(void);
void housekeeping_task_rgb_matrix(void);

void rgb_matrix_set_color_all(uint8_t red, uint8_t green, uint8_t blue);
void rgb_matrix_layer_helper(uint8_t hue, uint8_t sat, uint8_t val, uint8_t mode, uint8_t speed, uint8_t led_type,
                             led_index_t led_min, led_index_t led_max);
void rgb_matrix_indicators_render_layer(led_index_t led_min, led_index_t led_max);

bool rgb_matrix_indicators_advanced_keymap(led_index_t led_min, led_index_t led_max);
bool rgb_matrix_indicators_keymap(void);
void rgb_matrix_shutdown(bool jump_to_bootloader);

const char *rgb_matrix_name(uint8_t effect);
const char *rgb_matrix_get_effect_name(void);
bool        has_rgb_matrix_config_changed(void);
void        rgb_matrix_idle_anim_toggle(void);
