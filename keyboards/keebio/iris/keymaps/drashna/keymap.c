// Copyright 2023 Christopher Courtney, aka Drashna Jael're  (@drashna) <drashna@live.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "drashna.h"

/*
 * The `LAYOUT_iris_base` macro is a template to allow the use of identical
 * modifiers for the default layouts (eg QWERTY, Colemak, Dvorak, etc), so
 * that there is no need to set them up for each layout, and modify all of
 * them if I want to change them.  This helps to keep consistency and ease
 * of use. K## is a placeholder to pass through the individual keycodes
 */

// clang-format off
#define LAYOUT_wrapper(...)            LAYOUT(__VA_ARGS__)
#define LAYOUT_iris_base( \
    K01, K02, K03, K04, K05, K06, K07, K08, K09, K0A, \
    K11, K12, K13, K14, K15, K16, K17, K18, K19, K1A, K1B, \
    K21, K22, K23, K24, K25, K26, K27, K28, K29, K2A  \
  ) \
  LAYOUT_wrapper( \
     KC_ESC,  ________________NUMBER_LEFT________________,                       ________________NUMBER_RIGHT_______________, KC_MINS, \
     LALT_T(KC_TAB), K01, K02, K03,      K04,     K05,                           K06,     K07,     K08,     K09,     K0A,     KC_BSLS, \
     KC_C1R3, K11,    K12,     K13,      K14,     K15,                           K16,     K17,     K18,     K19,     K1A, RALT_T(K1B), \
     OS_LSFT, CTL_T(K21), K22, K23,      K24,     K25,     OS_LALT,     OS_RGUI, K26,     K27,     K28,     K29, RCTL_T(K2A), OS_RSFT, \
                             KC_GRV, KC_SPC,  LT(_LOWER,KC_BSPC),         LT(_RAISE,KC_DEL),  KC_ENT,  RAISE                           \
  )
#define LAYOUT_base_wrapper(...)       LAYOUT_iris_base(__VA_ARGS__)


const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [_DEFAULT_LAYER_1] = LAYOUT_base_wrapper(
        _________________QWERTY_L1_________________, _________________QWERTY_R1_________________,
        _________________QWERTY_L2_________________, _________________QWERTY_R2_________________,
        _________________QWERTY_L3_________________, _________________QWERTY_R3_________________
    ),

    [_DEFAULT_LAYER_2] = LAYOUT_base_wrapper(
        ______________COLEMAK_MOD_DH_L1____________, ______________COLEMAK_MOD_DH_R1____________,
        ______________COLEMAK_MOD_DH_L2____________, ______________COLEMAK_MOD_DH_R2____________,
        ______________COLEMAK_MOD_DH_L3____________, ______________COLEMAK_MOD_DH_R3____________
    ),
    [_DEFAULT_LAYER_3] = LAYOUT_base_wrapper(
        _________________COLEMAK_L1________________, _________________COLEMAK_R1________________,
        _________________COLEMAK_L2________________, _________________COLEMAK_R2________________,
        _________________COLEMAK_L3________________, _________________COLEMAK_R3________________
    ),

    [_DEFAULT_LAYER_4] = LAYOUT_base_wrapper(
        _________________DVORAK_L1_________________, _________________DVORAK_R1_________________,
        _________________DVORAK_L2_________________, _________________DVORAK_R2_________________,
        _________________DVORAK_L3_________________, _________________DVORAK_R3_________________
    ),

  [_GAMEPAD] = LAYOUT_wrapper(
     KC_ESC,  KC_NO,   KC_1,    KC_2,    KC_3,    KC_4,                          _______, _______, _______, _______, _______, _______,
     KC_F1,   KC_K,    KC_Q,    KC_W,    KC_E,    KC_R,                          _______, _______, _______, _______, _______, _______,
     KC_TAB,  KC_G,    KC_A,    KC_S,    KC_D,    KC_F,                          _______, _______, _______, _______, _______, _______,
     KC_LCTL, KC_LSFT, KC_Z,    KC_X,    KC_C,    KC_H,    TG_GAME,     _______, _______, _______, _______, _______, _______, _______,
                                       KC_GRV,  KC_V,    KC_SPC,          _______, _______, _______
  ),


  [_LOWER] = LAYOUT_wrapper(
     KC_F12,  _________________FUNC_LEFT_________________,                       _________________FUNC_RIGHT________________, KC_F11,
     KC_TILD, _________________LOWER_L1__________________,                       _________________LOWER_R1__________________, _______,
     _______, ___________________BLANK___________________,                       _________________LOWER_R2__________________, KC_PIPE,
     _______, ___________________BLANK___________________, _______,     _______, _________________LOWER_R3__________________, _______,
                                       _______, _______, _______,         _______, _______, _______
  ),

  [_RAISE] = LAYOUT_wrapper(
      KC_F12,  _________________FUNC_LEFT_________________,                      _________________FUNC_RIGHT________________, KC_F11,
      KC_GRV,  _________________RAISE_L1__________________,                      _________________RAISE_R1__________________, _______,
      _______, _________________RAISE_L2__________________,                      _________________RAISE_R2__________________, KC_BSLS,
      _______, _________________RAISE_L3__________________, _______,    _______, _________________RAISE_R3__________________, _______,
                                        _______, _______, _______,        _______, _______, _______
   ),

  [_ADJUST] = LAYOUT_wrapper(
      QK_MAKE, _______, _______, _______, _______, _______,                      _________________ADJUST_R1_________________, QK_BOOT,
      VRSN,    _________________ADJUST_L1_________________,                      KC_NUKE, _______, _______, _______, _______, EE_CLR,
      _______, _________________ADJUST_L2_________________,                      _________________ADJUST_R2_________________, TG_MODS,
      _______, _________________ADJUST_L3_________________, TG_GAME,    _______, _________________ADJUST_R3_________________, KC_MPLY,
                                        _______, _______, _______,        _______, _______, _______
   )

};
// clang-format on

void keyboard_post_init_keymap(void) {
#if BACKLIGHT_ENABLE
    backlight_enable();
    backlight_level(5);
#    ifdef BACKLIGHT_BREATHING
    breathing_enable();
#    endif
#endif
}
