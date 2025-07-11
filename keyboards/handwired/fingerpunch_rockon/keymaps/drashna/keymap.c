// Copyright 2023 Christopher Courtney, aka Drashna Jael're  (@drashna) <drashna@live.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "drashna.h"

// clang-format off
#define LAYOUT_rockon_bp_wrapper(...) LAYOUT_rockon_bp(__VA_ARGS__)
#define LAYOUT_rockon_bp_base( \
    K01, K02, K03, K04, K05, K06, K07, K08, K09, K0A, \
    K11, K12, K13, K14, K15, K16, K17, K18, K19, K1A, K1B, \
    K21, K22, K23, K24, K25, K26, K27, K28, K29, K2A  \
  ) \
  LAYOUT_rockon_bp_wrapper( \
    KC_ESC,  ________________NUMBER_LEFT________________, UC_FLIP,                      UC_TABL, ________________NUMBER_RIGHT_______________, UC_CLUE, \
    SH_TT,   K01,     K02,     K03,     K04,     K05,     TG_DBLO,                      _______, K06,     K07,     K08,     K09,     K0A,     SH_TT,   \
    LALT_T(KC_TAB), K11, K12,  K13,     K14,     K15,     BK_LWER,                      TG_GAME, K16,     K17,     K18,     K19,     K1A,     RALT_T(K1B), \
    OS_LSFT,CTL_T(K21),K22,    K23,    K24,      K25,     _______,                      _______, K26,     K27,     K28,     K29, RCTL_T(K2A), OS_RSFT, \
                      KC_GRV, OS_LALT, OS_LGUI, SP_LWER, BK_LWER, KC_MUTE,    UC_IRNY, DL_RAIS, KC_ENT,  OS_RGUI, OS_RALT, UC_IRNY\
    )

#define LAYOUT_base_wrapper(...) LAYOUT_rockon_bp_base(__VA_ARGS__)

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
    [_GAMEPAD] = LAYOUT_rockon_bp(
        KC_ESC,  XXXXXXX, KC_1,    KC_2,    KC_3,    KC_4, HYPR(KC_Q),                       _______, KC_F9,   KC_F10,  KC_F11,  KC_F12,  XXXXXXX, KC_NO,
        KC_F1,   KC_K,    KC_Q,    KC_W,    KC_E,    KC_R,    KC_T,                          UC_SHRG, UC_DISA, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, KC_NO,
        KC_TAB,  KC_G,    KC_A,    KC_S,    KC_D,    KC_F,    _______,                       _______, KC_I,    KC_O,    XXXXXXX, XXXXXXX, XXXXXXX, KC_NO,
        KC_LCTL, KC_LSFT, KC_Z,    KC_X,    KC_C,    KC_V,    _______,                       _______, KC_N,    KC_M,    XXXXXXX, XXXXXXX, XXXXXXX, KC_NO,
                          _______, _______, KC_V,    KC_SPC,  KC_H,    _______,     _______, _______, _______, _______, _______, _______
    ),
    [_DIABLO] = LAYOUT_rockon_bp(
        KC_ESC,  KC_V,    KC_D,    KC_LALT, XXXXXXX, XXXXXXX, XXXXXXX,                       _______, KC_F9,   KC_F10,   KC_F11,  KC_F12,  XXXXXXX, KC_NO,
        KC_TAB,  KC_S,    KC_I,    KC_F,    KC_M,    KC_T,    _______,                       _______, XXXXXXX, XXXXXXX,  XXXXXXX, XXXXXXX, XXXXXXX, KC_NO,
        XXXXXXX, KC_1,    KC_2,    KC_3,    KC_4,    KC_G,    XXXXXXX,                       _______, XXXXXXX, XXXXXXX,  XXXXXXX, XXXXXXX, XXXXXXX, KC_NO,
        KC_LCTL, KC_D3_1, KC_D3_2, KC_D3_3, KC_D3_4, KC_Z,    _______,                       _______, KC_N,    KC_M,     XXXXXXX, XXXXXXX, XXXXXXX, KC_NO,
                          _______, _______, KC_DIABLO_CLEAR,  SFT_T(KC_SPACE),  ALT_T(KC_Q), _______,     _______, _______, _______, _______, _______, _______
    ),
    [_DIABLOII] = LAYOUT_rockon_bp(
        KC_ESC,  KC_1,    KC_2,    KC_3,    KC_4,    KC_GRV,  XXXXXXX,                       _______, KC_F9,   KC_F10,   KC_F11,  KC_F12,  XXXXXXX, KC_NO,
        KC_TAB,  KC_A,    KC_T,    KC_Q,    KC_I,    KC_M,    TG(_DIABLOII),                 _______, XXXXXXX, XXXXXXX,  XXXXXXX, XXXXXXX, XXXXXXX, KC_NO,
        KC_S,    KC_F1,   KC_F2,   KC_F3,   KC_F4,   KC_F5,   XXXXXXX,                       _______, XXXXXXX, XXXXXXX,  XXXXXXX, XXXXXXX, XXXXXXX, KC_NO,
        KC_LCTL, KC_F6,   KC_F7,   KC_F8,   KC_F9,   KC_F10,  _______,                       _______, KC_N,    KC_M,     XXXXXXX, XXXXXXX, XXXXXXX, KC_NO,
                          _______, _______, KC_G,    KC_LSFT, KC_LCTL, _______,     _______, _______, _______, _______, _______, _______
    ),
    [_MOUSE] = LAYOUT_rockon_bp(
        _______, _______, _______, _______, _______, _______, _______,                       _______, _______, _______, _______, _______, _______, _______,
        _______, _______, _______, _______, _______, _______, _______,                       _______, MS_WH_U, _______, _______, _______, _______, _______,
        _______, _______, _______, _______, _______, _______, _______,                       _______, MS_WH_D, MS_BTN1, MS_BTN3, MS_BTN2, MS_BTN6, _______,
        _______, _______, _______, _______, _______, _______, _______,                       _______, MS_BTN7, MS_BTN4, MS_BTN5, MS_BTN8, _______, _______,
                          _______, _______, _______, _______, _______, _______,     MS_BTN3, _______, _______, _______, _______, _______
    ),
    [_LOWER] = LAYOUT_rockon_bp_wrapper(
        KC_F12,  _________________FUNC_LEFT_________________, _______,                       _______, _________________FUNC_RIGHT________________, KC_F11,
        _______, _________________LOWER_L1__________________, _______,                       _______, _________________LOWER_R1__________________, KC_PIPE,
        _______, _________________LOWER_L2__________________, _______,                       _______, _________________LOWER_R2__________________, KC_DQUO,
        _______, _________________LOWER_L3__________________, _______,                       _______, _________________LOWER_R3__________________, KC_PSCR,
                          _______, _______, _______, _______, _______, _______,     _______, _______, _______, _______, _______, _______
    ),
    [_RAISE] = LAYOUT_rockon_bp_wrapper(
        KC_F12,  _________________FUNC_LEFT_________________, _______,                       _______, _________________FUNC_RIGHT________________, KC_F11,
        _______, _________________RAISE_L1__________________, _______,                       _______, _________________RAISE_R1__________________, KC_BSLS,
        _______, _________________RAISE_L2__________________, _______,                       _______, _________________RAISE_R2__________________, KC_QUOT,
        _______, _________________RAISE_L3__________________, _______,                       _______, _________________RAISE_R3__________________, KC_PSCR,
                          _______, _______, _______, _______, _______, _______,     _______, _______, _______, _______, _______, _______
    ),
    [_ADJUST] = LAYOUT_rockon_bp_wrapper(
        QK_MAKE, KC_WIDE,KC_AUSSIE,KC_SCRIPT,KC_ZALGO,KC_NOMODE,_______,                     KC_NUKE,KC_NOMODE,KC_BLOCKS,KC_REGIONAL,_______,_______, QK_BOOT,
        VRSN,    _________________ADJUST_L1_________________, TG(_DIABLOII),                 _______, _________________ADJUST_R1_________________, EE_CLR,
        KEYLOCK, _________________ADJUST_L2_________________, _______,                       _______, _________________ADJUST_R2_________________, RGB_IDL,
        UC_NEXT, _________________ADJUST_L3_________________, _______,                       _______, _________________ADJUST_R3_________________, TG_MODS,
                          _______, _______, QK_RBT,  _______, _______, _______,     _______, _______, _______, KC_NUKE, _______, AC_TOGG
    ),
};
// clang-format on

#ifdef ENCODER_MAP_ENABLE
const uint16_t PROGMEM encoder_map[][NUM_ENCODERS][NUM_DIRECTIONS] = {
    [_DEFAULT_LAYER_1] = {{KC_VOLD, KC_VOLU}, {KC_WH_D, KC_WH_U}},
    [_DEFAULT_LAYER_2] = {{_______, _______}, {_______, _______}},
    [_DEFAULT_LAYER_3] = {{_______, _______}, {_______, _______}},
    [_DEFAULT_LAYER_4] = {{_______, _______}, {_______, _______}},
    [_GAMEPAD]         = {{_______, _______}, {_______, _______}},
    [_DIABLO]          = {{_______, _______}, {_______, _______}},
    [_MOUSE]           = {{_______, _______}, {KC_WH_D, KC_WH_U}},
    [_MEDIA]           = {{_______, _______}, {_______, _______}},
    [_RAISE]           = {{_______, _______}, {KC_PGDN, KC_PGUP}},
    [_LOWER]           = {{RGB_MOD, RGB_RMOD}, {RGB_HUD, RGB_HUI}},
    [_ADJUST]          = {{CK_DOWN, CK_UP}, {_______, _______}},
};
#endif

#if defined(OLED_ENABLE) && defined(OLED_DISPLAY_128X128)
#    ifdef UNICODE_COMMON_ENABLE
#        include "process_unicode_common.h"
#        include "keyrecords/unicode.h"
#    endif

void render_rgb_mode(uint8_t col, uint8_t line);

bool oled_task_keymap(void) {
    oled_write_raw_P(header_image, sizeof(header_image));
    oled_set_cursor(7, 0);
    oled_write_P(PSTR("Rock On"), true);

    render_default_layer_state(1, 1);
    render_layer_state(1, 2);
    render_pet(0, 5);
    render_wpm(1, 7, 5);
#    if defined(POINTING_DEVICE_ENABLE)
    render_pointing_dpi_status(charybdis_get_pointer_sniping_enabled() ? charybdis_get_pointer_sniping_dpi()
                                                                       : charybdis_get_pointer_default_dpi(),
                               1, 7, 6);
    render_mouse_mode(17, 6);
#    elif defined(WPM_ENABLE) && defined(DEBUG_MATRIX_SCAN_RATE)
    render_matrix_scan_rate(1, 7, 6);
#    endif
    render_bootmagic_status(7, 7);
    render_user_status(1, 9);

    render_mod_status(get_mods() | get_oneshot_mods(), 1, 10);
    render_keylock_status(host_keyboard_led_state(), 1, 11);

    render_rgb_hsv(1, 12);
    render_rgb_mode(1, 13);

    render_os(1, 14);

    for (uint8_t i = 1; i < 15; i++) {
        oled_set_cursor(0, i);
        oled_write_raw_P(display_border, sizeof(display_border));
        oled_set_cursor(21, i);
        oled_write_raw_P(display_border, sizeof(display_border));
    }

    oled_set_cursor(0, 15);
    oled_write_raw_P(footer_image2, sizeof(footer_image2));

    oled_set_cursor(4, 15);
    oled_write(oled_keylog_str, true);

    return false;
}
#endif
