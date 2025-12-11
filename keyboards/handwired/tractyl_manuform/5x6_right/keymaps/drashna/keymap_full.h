// Copyright 2024 Christopher Courtney, aka Drashna Jael're  (@drashna) <drashna@live.com>
// SPDX-License-Identifier: GPL-3.0-or-later

// clang-format off
#define LAYOUT_5x6_full_right_wrapper(...) LAYOUT_5x6_full_right(__VA_ARGS__)
#define LAYOUT_5x6_full_right_base( \
    K01, K02, K03, K04, K05, K06, K07, K08, K09, K0A, \
    K11, K12, K13, K14, K15, K16, K17, K18, K19, K1A, K1B, \
    K21, K22, K23, K24, K25, K26, K27, K28, K29, K2A  \
  ) \
  LAYOUT_5x6_full_right_wrapper( \
     KC_ESC,  ________________NUMBER_LEFT________________,            ________________NUMBER_RIGHT_______________, UC_CLUE, \
     SH_TT,   K01,    K02,      K03,     K04,     K05,                K06,     K07,     K08,     K09,     K0A,     SH_TT, \
     LALT_T(KC_TAB), K11, K12,  K13,     K14,     K15,                K16,     K17,     K18,     K19,     K1A,     RALT_T(K1B), \
     OS_LSFT, CTL_T(K21), K22,  K23,     K24,     K25,                K26,     K27,     K28,     K29, RCTL_T(K2A), OS_RSFT, \
     KC_GRV, DISPLAY_MENU, OS_LGUI, OS_LALT,                                            OS_RALT, OS_RGUI, DISPLAY_MENU, KC_ESC, \
                                KC_MUTE, KC_GRV,                                        MS_BTN3,  \
                                         KC_SPC,  UC_IRNY,                     KC_ENT,  \
                                         BK_LWER, MO(_MOUSE),      TG(_MOUSE), DL_RAIS  \
  )
#define LAYOUT_base_wrapper(...)       LAYOUT_5x6_full_right_base(__VA_ARGS__)

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

    [_GAMEPAD] = LAYOUT_5x6_full_right(
        KC_ESC,  KC_NO,   KC_1,    KC_2,    KC_3,    KC_4,                           _______, _______, _______, _______, _______, _______,
        KC_F1,   KC_K,    KC_Q,    KC_W,    KC_E,    KC_R,                           _______, _______, _______, _______, _______, _______,
        KC_TAB,  KC_G,    KC_A,    KC_S,    KC_D,    KC_F,                           _______, _______, _______, _______, _______, _______,
        KC_LCTL, KC_LSFT, KC_Z,    KC_X,    KC_C,    KC_H,                           _______, _______, _______, _______, _______, _______,
        _______, _______, KC_I,    KC_T,                                                               TG_GAME, KC_NO,   _______, _______,
                                            KC_V,    KC_O,                                    _______,
                                                     KC_SPC,  KC_P,                  _______,
                                                     KC_H,    KC_LGUI,      _______, _______
    ),
    [_DIABLO] = LAYOUT_5x6_full_right(
        KC_ESC,  KC_V,    KC_D,    KC_LALT, KC_NO,   KC_NO,                          KC_F9,   KC_F10,  KC_F11,  KC_F12,  KC_NO,   KC_NO,
        KC_TAB,  KC_S,    KC_I,    KC_F,    KC_M,    KC_T,                           _______, _______, _______, _______, _______, _______,
        KC_Q,    KC_1,    KC_2,    KC_3,    KC_4,    KC_G,                           _______, _______, _______, _______, _______, _______,
        KC_LCTL, KC_D3_1, KC_D3_2, KC_D3_3, KC_D3_4, KC_Z,                           _______, _______, _______, _______, _______, _______,
        _______, _______, KC_F,    KC_L,                                                               KC_NO,   TG_DBLO, _______, _______,
                                    KC_DIABLO_CLEAR, KC_F,                                    _______,
                                               SFT_T(KC_SPC), KC_J,                  _______,
                                               ALT_T(KC_Q),   KC_LGUI,      _______, _______
    ),
    [_DIABLOII] = LAYOUT_5x6_full_right(
        KC_ESC,  KC_1,    KC_2,    KC_3,    KC_4,    KC_GRV,                         _______, _______, _______, _______, _______, _______,
        KC_TAB,  KC_A,    KC_T,    KC_Q,    KC_I,    KC_M,                           _______, _______, _______, _______, _______, _______,
        KC_S,    KC_F1,   KC_F2,   KC_F3,   KC_F4,   KC_F5,                          _______, _______, _______, _______, _______, _______,
        KC_LCTL, KC_F6,   KC_F7,   KC_F8,   KC_F9,   KC_F10,                         _______, _______, _______, _______, _______, _______,
        _______, _______, KC_F,    KC_L,                                                           KC_NO, TG(_DIABLOII), _______, _______,
                                   _______, KC_G,                                             _______,
                                               KC_LSFT, _______,                     _______,
                                               KC_LCTL, KC_V,               _______, _______
    ),
    [_MOUSE] = LAYOUT_5x6_full_right_wrapper(
        _______, _______, _______, _______, _______, _______,                        XXXXXXX, DPI_RMOD,DPI_MOD, S_D_RMOD,S_D_MOD, PD_JIGG,
        _______, ___________________BLANK___________________,                        _________________MOUSE_R1__________________, XXXXXXX,
        _______, ___________________BLANK___________________,                        _________________MOUSE_R2__________________, XXXXXXX,
        _______, ___________________BLANK___________________,                        _________________MOUSE_R3__________________, XXXXXXX,
        _______, _______, _______, _______,                                                            _______, _______, _______, _______,
                                            _______, _______,                                 MS_BTN3,
                                                     _______, _______,               _______,
                                                     _______, _______,      _______, _______
    ),
    [_LOWER] = LAYOUT_5x6_full_right_wrapper(
        KC_F12,  _________________FUNC_LEFT_________________,                        _________________FUNC_RIGHT________________, KC_F11,
        _______, _________________LOWER_L1__________________,                        _________________LOWER_R1__________________, _______,
        _______, _________________LOWER_L2__________________,                        _________________LOWER_R2__________________, KC_PIPE,
        _______, _________________LOWER_L3__________________,                        _________________LOWER_R3__________________, _______,
        _______, _______, _______, _______,                                                            _______, _______, _______, _______,
                                            _______, _______,                                 _______,
                                                     _______, _______,               _______,
                                                     _______, _______,      _______, _______
    ),
    [_RAISE] = LAYOUT_5x6_full_right_wrapper(
        KC_F12,  _________________FUNC_LEFT_________________,                        _________________FUNC_RIGHT________________, KC_F11,
        KC_GRV,  _________________RAISE_L1__________________,                        _________________RAISE_R1__________________, _______,
        _______, _________________RAISE_L2__________________,                        _________________RAISE_R2__________________, KC_BSLS,
        _______, _________________RAISE_L3__________________,                        _________________RAISE_R3__________________, _______,
        _______, _______, STORE_SETUPS, PRINT_SETUPS,                                                  _______, _______, _______, _______,
                                            OL_LOCK, _______,                                 _______,
                                                     _______, _______,               _______,
                                                     _______, _______,      _______, _______
    ),
    [_ADJUST] = LAYOUT_5x6_full_right_wrapper(
        QK_MAKE, KC_WIDE,KC_AUSSIE,KC_SCRIPT,KC_ZALGO,KC_NOMODE,                 KC_NOMODE,KC_BLOCKS,KC_REGIONAL,_______,_______, QK_BOOT,
        VRSN,    _________________ADJUST_L1_________________,                        _________________ADJUST_R1_________________, KC_SEC6,
        KEYLOCK, _________________ADJUST_L2_________________,                        _________________ADJUST_R2_________________, TG_MODS,
        UC_NEXT, _________________ADJUST_L3_________________,                        _________________ADJUST_R3_________________, KC_MPLY,
        KC_PSCR, _______, TG(_DIABLOII), AC_TOGG,                                                      TG_GAME, TG_DBLO, _______, _______,
                                            _______, QK_RBT,                                  KC_NUKE,
                                                     HF_TOGG, _______,               US_MSRP,
                                                     _______, _______,      KC_NUKE, _______
    ),
};
