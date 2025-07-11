// Copyright 2021 Quentin LEBASTARD <qlebastard@gmail.com>
// Copyright 2023 Christopher Courtney, aka Drashna Jael're  (@drashna) <drashna@live.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "drashna.h"

// clang-format off
#define LAYOUT_charybdis_3x5_wrapper(...) LAYOUT_charybdis_3x5(__VA_ARGS__)
#define LAYOUT_charybdis_3x5_base( \
    K01, K02, K03, K04, K05, K06, K07, K08, K09, K0A, \
    K11, K12, K13, K14, K15, K16, K17, K18, K19, K1A, K1B, \
    K21, K22, K23, K24, K25, K26, K27, K28, K29, K2A  \
    ) \
    LAYOUT_charybdis_3x5 ( \
       K01,    K02,      K03,     K04,     K05,                K06,     K07,     K08,     K09,     K0A, \
       ALT_T(K11), K12,  K13,     K14,     GUI_T(K15),  LGUI_T(K16),    K17,     K18,     K19, LALT_T(K1A), \
       CTL_T(K21), K22,  K23,     K24,     K25,                K26,     K27,     K28,     K29, RCTL_T(K2A), \
                LSFT_T(KC_GRV), KC_SPC,  BK_LWER,              DL_RAIS, RSFT_T(KC_ENT) \
    )
#define LAYOUT_charybdis_3x5_base_wrapper(...)       LAYOUT_charybdis_3x5_base(__VA_ARGS__)

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {

    [_DEFAULT_LAYER_1] = LAYOUT_charybdis_3x5_base_wrapper(
        _________________QWERTY_L1_________________, _________________QWERTY_R1_________________,
        _________________QWERTY_L2_________________, _________________QWERTY_R2_________________,
        _________________QWERTY_L3_________________, _________________QWERTY_R3_________________
    ),

    [_DEFAULT_LAYER_2] = LAYOUT_charybdis_3x5_base_wrapper(
        ______________COLEMAK_MOD_DH_L1____________, ______________COLEMAK_MOD_DH_R1____________,
        ______________COLEMAK_MOD_DH_L2____________, ______________COLEMAK_MOD_DH_R2____________,
        ______________COLEMAK_MOD_DH_L3____________, ______________COLEMAK_MOD_DH_R3____________
    ),
    [_DEFAULT_LAYER_3] = LAYOUT_charybdis_3x5_base_wrapper(
        _________________COLEMAK_L1________________, _________________COLEMAK_R1________________,
        _________________COLEMAK_L2________________, _________________COLEMAK_R2________________,
        _________________COLEMAK_L3________________, _________________COLEMAK_R3________________
    ),

    [_DEFAULT_LAYER_4] = LAYOUT_charybdis_3x5_base_wrapper(
        _________________DVORAK_L1_________________, _________________DVORAK_R1_________________,
        _________________DVORAK_L2_________________, _________________DVORAK_R2_________________,
        _________________DVORAK_L3_________________, _________________DVORAK_R3_________________
    ),

    [_MOUSE] = LAYOUT_charybdis_3x5(
        _______, _______, _______, _______, _______,    MS_WHLU, DPI_RMOD,DPI_MOD, S_D_RMOD,S_D_MOD,
        _______, _______, _______, _______, _______,    MS_WHLD, MS_BTN1, MS_BTN3, MS_BTN2, MS_BTN6,
        _______, _______, _______, _______, _______,    MS_BTN7, MS_BTN4, MS_BTN5, MS_BTN8, _______,
                          _______, SNIPING, _______,    _______, _______
    ),
    [_LOWER] = LAYOUT_charybdis_3x5_wrapper(
        _________________LOWER_L1__________________,    _________________LOWER_R1__________________,
        _________________LOWER_L2__________________,    _________________LOWER_R2__________________,
        _________________LOWER_L3__________________,    _________________LOWER_R3__________________,
                          _______, _______, _______,    _______, AC_TOGG
    ),
    [_RAISE] = LAYOUT_charybdis_3x5_wrapper(
        _________________RAISE_L1__________________,    _________________RAISE_R1__________________,
        _________________RAISE_L2__________________,    _________________RAISE_R2__________________,
        _________________RAISE_L3__________________,    _________________RAISE_R3__________________,
                          QK_RBT,  _______, _______,    _______, _______
    ),
    [_ADJUST] = LAYOUT_charybdis_3x5_wrapper(
        _________________ADJUST_L1_________________,    _________________ADJUST_R1_________________,
        _________________ADJUST_L2_________________,    _________________ADJUST_R2_________________,
        _________________ADJUST_L3_________________,    _________________ADJUST_R3_________________,
                          EE_CLR, KC_NUKE, _______,    _______, QK_BOOT
    ),
};
// clang-format on
