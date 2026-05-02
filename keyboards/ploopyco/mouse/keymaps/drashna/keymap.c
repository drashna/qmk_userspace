// Copyright 2023 Christopher Courtney, aka Drashna Jael're  (@drashna) <drashna@live.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include QMK_KEYBOARD_H

// clang-format off
const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [0] = LAYOUT(
                 C(KC_C), MS_BTN1, MS_BTN3, MS_BTN2, MO(1), MS_BTN4, MS_BTN5, DPI_CONFIG),
    [1] = LAYOUT(
                 UG_HUEU, UG_NEXT, UG_TOGG, UG_PREV, MO(1), KC_VOLU, KC_VOLD, QK_BOOT)
};
// clang-format on

#ifdef ENCODER_MAP_ENABLE
const uint16_t PROGMEM encoder_map[][NUM_ENCODERS][NUM_DIRECTIONS] = {
    [0] = {ENCODER_CCW_CW(MS_WHLU, MS_WHLU)},
    [1] = {ENCODER_CCW_CW(UG_HUEU, UG_HUED)},
};
#endif

#ifdef RGBLIGHT_ENABLE
void eeconkfig_init_user(void) {
    rgblight_enable();
#    ifdef RGBLIGHT_EFFECT_TWINKLE
    rgblight_mode(RGBLIGHT_MODE_TWINKLE + 5);
#    else
    rgblight_mode(RGBLIGHT_MODE_BREATHING + 5);
#    endif
    rgblight_sethsv(HSV_MAGENTA);
}
#endif
