// Copyright 2023 Christopher Courtney, aka Drashna Jael're  (@drashna) <drashna@live.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "drashna.h"

/*
 * The `LAYOUT_kyria_base` macro is a template to allow the use of identical
 * modifiers for the default layouts (eg QWERTY, Colemak, Dvorak, etc), so
 * that there is no need to set them up for each layout, and modify all of
 * them if I want to change them.  This helps to keep consistency and ease
 * of use. K## is a placeholder to pass through the individual keycodes
 */

// clang-format off
#define LAYOUT_wrapper(...)            LAYOUT(__VA_ARGS__)
#define LAYOUT_kyria_base( \
    K01, K02, K03, K04, K05, K06, K07, K08, K09, K0A, \
    K11, K12, K13, K14, K15, K16, K17, K18, K19, K1A, K1B, \
    K21, K22, K23, K24, K25, K26, K27, K28, K29, K2A  \
  ) \
  LAYOUT_wrapper( \
  SH_T(KC_ESC), K01,    K02,     K03,     K04,     K05,                                             K06,     K07,     K08,     K09,     K0A,     SH_T(KC_MINS), \
   LALT_T(KC_TAB), K11, K12,     K13,     K14,     K15,                                             K16,     K17,     K18,     K19,     K1A, RALT_T(K1B), \
      OS_LSFT, CTL_T(K21), K22,  K23,     K24,     K25,  TG_GAME, MEH(KC_MINS),   TG_DBLO, KC_CAPS, K26,     K27,     K28,     K29, RCTL_T(K2A), OS_RSFT, \
                                 KC_MUTE, OS_LALT, KC_GRV,  KC_SPC,  BK_LWER,     DL_RAIS, KC_ENT,  OS_RGUI, UC(0x03A8), UC(0x2E2E) \
    )
/* Re-pass though to allow templates to be used */
#define LAYOUT_base_wrapper(...)       LAYOUT_kyria_base(__VA_ARGS__)

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
        KC_ESC,  KC_K,    KC_Q,    KC_W,    KC_E,    KC_R,                                          _______, _______, _______, _______, _______, _______,
        KC_TAB,  KC_G,    KC_A,    KC_S,    KC_D,    KC_F,                                          _______, _______, _______, _______, _______, _______,
        KC_LCTL, KC_LSFT, KC_Z,    KC_X,    KC_C,    KC_V,    _______, _______,   _______, LALT(KC_PSCR), _______, _______, _______, _______, _______, _______,
                                  _______, NK_TOGG, KC_V,     KC_SPC,  KC_H,   _______, _______, _______, _______, _______
    ),

    [_DIABLO] = LAYOUT_wrapper(
        KC_ESC,  KC_S,    KC_I,    KC_F,    KC_M,    KC_T,                                          KC_F9,   KC_F10,  KC_F11,  KC_F12,  KC_NO,   KC_NO,
        KC_TAB,  KC_1,    KC_2,    KC_3,    KC_4,    KC_G,                                          KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,
        KC_LCTL, KC_D3_1, KC_D3_2, KC_D3_3, KC_D3_4, KC_Z,    KC_J,    KC_L,        TG_DBLO, KC_NO, KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,
                           KC_DIABLO_CLEAR, KC_J,    KC_NO, SFT_T(KC_SPACE), ALT_T(KC_Q),   KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO
    ),
    [_LOWER] = LAYOUT_wrapper(
        KC_F12,  _________________LOWER_L1__________________,                                       _________________LOWER_R1__________________, KC_F11,
        _______, _________________LOWER_L2__________________,                                       _________________LOWER_R2__________________, KC_PIPE,
        _______, _________________LOWER_L3__________________, _______, _______,   _______, _______, _________________LOWER_R3__________________, _______,
                                   _______, _______, _______, _______, _______,   _______, _______, _______, _______, _______
    ),
    [_RAISE] = LAYOUT_wrapper(
        KC_F12,  _________________RAISE_L1__________________,                                       _________________RAISE_R1__________________, KC_F11,
        _______, _________________RAISE_L2__________________,                                       _________________RAISE_R2__________________, KC_BSLS,
        _______, _________________RAISE_L3__________________, _______, _______,   _______, _______, _________________RAISE_R3__________________, _______,
                                   _______, _______, _______, _______, _______,   _______, _______, _______, _______, _______
    ),
    [_ADJUST] = LAYOUT_wrapper(
        QK_MAKE, _________________ADJUST_L1_________________,                                       _________________ADJUST_R1_________________, QK_BOOT,
        VRSN,    _________________ADJUST_L2_________________,                                       _________________ADJUST_R2_________________, EE_CLR,
        UC_NEXT, _________________ADJUST_L3_________________, HF_TOGG, HF_FDBK,   NK_TOGG, UC_NEXT, _________________ADJUST_R3_________________, TG_MODS,
                                   QK_RBT,  _______, KEYLOCK, KC_NUKE, _______,   _______, _______, _______, _______, AC_TOGG
    ),
//     [_LAYERINDEX] = LAYOUT_wrapper(
//       _______, _______, _______, _______, _______, _______,                                       _______, _______, _______, _______, _______, _______,
//       _______, _______, _______, _______, _______, _______,                                       _______, _______, _______, _______, _______, _______,
//       _______, _______, _______, _______, _______, _______, _______, _______,   _______, _______, _______, _______, _______, _______, _______, _______,
//                                  _______, _______, _______, _______, _______,   _______, _______, _______, _______, _______
//     ),
};

#ifdef ENCODER_MAP_ENABLE
const uint16_t PROGMEM encoder_map[][NUM_ENCODERS][NUM_DIRECTIONS] = {
    [_DEFAULT_LAYER_1] = { ENCODER_CCW_CW( KC_VOLD, KC_VOLU ), ENCODER_CCW_CW( KC_WH_D, KC_WH_U ) },
    [_DEFAULT_LAYER_2] = { ENCODER_CCW_CW( _______, _______ ), ENCODER_CCW_CW( _______, _______ ) },
    [_DEFAULT_LAYER_2] = { ENCODER_CCW_CW( _______, _______ ), ENCODER_CCW_CW( _______, _______ ) },
    [_DEFAULT_LAYER_2] = { ENCODER_CCW_CW( _______, _______ ), ENCODER_CCW_CW( _______, _______ ) },
    [_GAMEPAD]         = { ENCODER_CCW_CW( _______, _______ ), ENCODER_CCW_CW( _______, _______ ) },
    [_DIABLO]          = { ENCODER_CCW_CW( _______, _______ ), ENCODER_CCW_CW( _______, _______ ) },
    [_MOUSE]           = { ENCODER_CCW_CW( _______, _______ ), ENCODER_CCW_CW( KC_WH_D, KC_WH_U ) },
    [_MEDIA]           = { ENCODER_CCW_CW( _______, _______ ), ENCODER_CCW_CW( _______, _______ ) },
    [_RAISE]           = { ENCODER_CCW_CW( _______, _______ ), ENCODER_CCW_CW( KC_PGDN, KC_PGUP ) },
    [_LOWER]           = { ENCODER_CCW_CW( RGB_MOD, RGB_RMOD), ENCODER_CCW_CW( RGB_HUD, RGB_HUI ) },
    [_ADJUST]          = { ENCODER_CCW_CW( CK_DOWN, CK_UP   ), ENCODER_CCW_CW( _______, _______ ) },
};
#endif
// clang-format on

#ifdef OLED_ENABLE

void render_oled_title(bool side) {
    oled_write_P(side ? PSTR("   Splitkb   ") : PSTR("    Kyria    "), true);
}

oled_rotation_t oled_init_keymap(oled_rotation_t rotation, bool has_run) {
    if (has_run) return rotation;
#    ifdef OLED_DISPLAY_128X128
    return OLED_ROTATION_0;
#    else
    return OLED_ROTATION_180;
#    endif
}
#endif

#if defined(RGBLIGHT_ENABLE) && defined(RGBLIGHT_LAYERS)
const rgblight_segment_t PROGMEM shift_layers[] =
    RGBLIGHT_LAYER_SEGMENTS({8, 1, 120, 255, 255}, {18, 1, 120, 255, 255});
const rgblight_segment_t PROGMEM control_layers[] = RGBLIGHT_LAYER_SEGMENTS({6, 1, 0, 255, 255}, {16, 1, 0, 255, 255});
const rgblight_segment_t PROGMEM alt_layers[] = RGBLIGHT_LAYER_SEGMENTS({2, 1, 240, 255, 255}, {17, 1, 250, 255, 255});
const rgblight_segment_t PROGMEM gui_layers[] = RGBLIGHT_LAYER_SEGMENTS({7, 1, 51, 255, 255}, {12, 1, 51, 255, 255});

const rgblight_segment_t* const PROGMEM my_rgb_layers[] =
    RGBLIGHT_LAYERS_LIST(shift_layers, control_layers, alt_layers, gui_layers);

void keyboard_post_init_keymap(void) {
    rgblight_layers = my_rgb_layers;
}

void housekeeping_task_keymap(void) {
    uint8_t mods = mod_config(get_mods() | get_oneshot_mods());
    rgblight_set_layer_state(0, mods & MOD_MASK_SHIFT);
    rgblight_set_layer_state(1, mods & MOD_MASK_CTRL);
    rgblight_set_layer_state(2, mods & MOD_MASK_ALT);
    rgblight_set_layer_state(3, mods & MOD_MASK_GUI);
}
#elif defined(RGB_MATRIX_ENABLE) && defined(KEYBOARD_splitkb_kyria_rev3)
void keyboard_post_init_keymap(void) {
    extern led_config_t g_led_config;
    g_led_config.flags[30] = g_led_config.flags[24] = g_led_config.flags[18] = g_led_config.flags[12] =
        g_led_config.flags[11] = g_led_config.flags[10] = g_led_config.flags[9] = g_led_config.flags[8] =
            g_led_config.flags[7] = g_led_config.flags[6] = g_led_config.flags[37] = g_led_config.flags[38] =
                g_led_config.flags[39] = g_led_config.flags[40] = g_led_config.flags[41] = g_led_config.flags[42] =
                    g_led_config.flags[43] = g_led_config.flags[49] = g_led_config.flags[55] = g_led_config.flags[61] =
                        LED_FLAG_MODIFIER;
}

void check_default_layer(uint8_t mode, uint8_t type, uint8_t led_min, uint8_t led_max) {
    switch (get_highest_layer(default_layer_state)) {
        case _QWERTY:
            rgb_matrix_layer_helper(DEFAULT_LAYER_1_HSV, mode, rgb_matrix_config.speed, type, led_min, led_max);
            break;
        case _COLEMAK_DH:
            rgb_matrix_layer_helper(DEFAULT_LAYER_2_HSV, mode, rgb_matrix_config.speed, type, led_min, led_max);
            break;
        case _COLEMAK:
            rgb_matrix_layer_helper(DEFAULT_LAYER_3_HSV, mode, rgb_matrix_config.speed, type, led_min, led_max);
            break;
        case _DVORAK:
            rgb_matrix_layer_helper(DEFAULT_LAYER_4_HSV, mode, rgb_matrix_config.speed, type, led_min, led_max);
            break;
    }
}

bool rgb_matrix_indicators_advanced_keymap(uint8_t led_min, uint8_t led_max) {
    if (userspace_config.rgb_layer_change) {
        switch (get_highest_layer(layer_state)) {
            case _GAMEPAD:
                rgb_matrix_layer_helper(HSV_ORANGE, 0, rgb_matrix_config.speed, LED_FLAG_UNDERGLOW, led_min, led_max);
                break;
            case _DIABLO:
                rgb_matrix_layer_helper(HSV_RED, 0, rgb_matrix_config.speed, LED_FLAG_UNDERGLOW, led_min, led_max);
                break;
            case _RAISE:
                rgb_matrix_layer_helper(HSV_YELLOW, 0, rgb_matrix_config.speed, LED_FLAG_UNDERGLOW, led_min, led_max);
                break;
            case _LOWER:
                rgb_matrix_layer_helper(HSV_GREEN, 0, rgb_matrix_config.speed, LED_FLAG_UNDERGLOW, led_min, led_max);
                break;
            case _ADJUST:
                rgb_matrix_layer_helper(HSV_RED, 0, rgb_matrix_config.speed, LED_FLAG_UNDERGLOW, led_min, led_max);
                break;
            case _MOUSE:
                rgb_matrix_layer_helper(HSV_PURPLE, 1, rgb_matrix_config.speed, LED_FLAG_UNDERGLOW, led_min, led_max);
                break;
            default:
                check_default_layer(0, LED_FLAG_UNDERGLOW, led_min, led_max);
                break;
        }
        check_default_layer(0, LED_FLAG_MODIFIER, led_min, led_max);
    }
    return false;
}
#endif
