// Copyright 2023 Christopher Courtney, aka Drashna Jael're  (@drashna) <drashna@live.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "drashna.h"
#include "keyboard.h"

// clang-format off
#define LAYOUT_wrapper(...) LAYOUT(__VA_ARGS__)
#define LAYOUT_base( \
    K01, K02, K03, K04, K05, K06, K07, K08, K09, K0A, \
    K11, K12, K13, K14, K15, K16, K17, K18, K19, K1A, K1B, \
    K21, K22, K23, K24, K25, K26, K27, K28, K29, K2A  \
    ) \
    LAYOUT_wrapper ( \
     KC_ESC,  ________________NUMBER_LEFT________________,            ________________NUMBER_RIGHT_______________, UC_CLUE, \
     SH_TT,   K01,    K02,      K03,     K04,     K05,                K06,     K07,     K08,     K09,     K0A,     SH_TT, \
     LALT_T(KC_TAB), K11, K12,  K13,     K14,     K15,                K16,     K17,     K18,     K19,     K1A,     RALT_T(K1B), \
     OS_LSFT,CTL_T(K21),ALT_T(K22),GUI_T(K23),K24,K25,                K26,K27,RGUI_T(K28),RALT_T(K29),RCTL_T(K2A), OS_RSFT, \
                       KC_MUTE, KC_GRV,  KC_SPC,  BK_LWER,    DL_RAIS, RSFT_T(KC_ENT), KC_RALT, TT(_MOUSE) \
    )

#define LAYOUT_base_wrapper(...)       LAYOUT_base(__VA_ARGS__)

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
    [_GAMEPAD] = LAYOUT(
        KC_ESC,  KC_NO,   KC_1,    KC_2,    KC_3,    KC_4,       _______, _______, _______, _______, _______, _______,
        KC_F1,   KC_K,    KC_Q,    KC_W,    KC_E,    KC_R,       _______, _______, _______, _______, _______, _______,
        KC_TAB,  KC_G,    KC_A,    KC_S,    KC_D,    KC_F,       _______, _______, _______, _______, _______, _______,
        KC_LCTL, KC_LSFT, KC_Z,    KC_X,    KC_C,    KC_H,       _______, _______, _______, _______, _______, _______,
                          _______, KC_V,    KC_SPC,  KC_H,       _______, _______, _______, TG_GAME
    ),
    [_DIABLO] = LAYOUT(
        KC_ESC,  KC_V,    KC_D,    KC_LALT, KC_NO,   KC_NO,      KC_F9,   KC_F10,  KC_F11,  KC_F12,  KC_NO,   KC_NO,
        KC_TAB,  KC_S,    KC_I,    KC_F,    KC_M,    KC_T,       _______, _______, _______, _______, _______, _______,
        KC_Q,    KC_1,    KC_2,    KC_3,    KC_4,    KC_G,       _______, _______, _______, _______, _______, _______,
        KC_LCTL, KC_D3_1, KC_D3_2, KC_D3_3, KC_D3_4, KC_Z,       _______, _______, _______, _______, _______, _______,
                          _______, KC_G,    KC_LSFT, KC_LCTL,    _______, _______, TO(_DIABLOII), TG_DBLO
    ),
    [_DIABLOII] = LAYOUT(
        KC_ESC,  KC_1,    KC_2,    KC_3,    KC_4,    KC_GRV,     _______, _______, _______, _______, _______, _______,
        KC_TAB,  KC_A,    KC_T,    KC_Q,    KC_I,    KC_M,       _______, _______, _______, _______, _______, _______,
        KC_S,    KC_F1,   KC_F2,   KC_F3,   KC_F4,   KC_F5,      _______, _______, _______, _______, _______, _______,
        KC_LCTL, KC_F6,   KC_F7,   KC_F8,   KC_F9,   KC_F10,     _______, _______, _______, _______, _______, _______,
                  _______, KC_DIABLO_CLEAR, _______, _______,    _______, _______, _______, TG(_DIABLOII)
    ),
    [_MOUSE] = LAYOUT_wrapper(
        _______, _______, _______, _______, _______, _______,   _______, DPI_RMOD,DPI_MOD, S_D_RMOD,S_D_MOD, PD_JIGGLER,
        _______, ___________________BLANK___________________,   _________________MOUSE_R1__________________, XXXXXXX,
        _______, ___________________BLANK___________________,   _________________MOUSE_R2__________________, XXXXXXX,
        _______, ___________________BLANK___________________,   _________________MOUSE_R3__________________, XXXXXXX,
                          _______, _______, SNIPING, _______,   _______, _______, _______, _______
    ),
    [_LOWER] = LAYOUT_wrapper(
        KC_F12,  _________________FUNC_LEFT_________________,    _________________FUNC_RIGHT________________, KC_F11,
        _______, _________________LOWER_L1__________________,    _________________LOWER_R1__________________, _______,
        _______, _________________LOWER_L2__________________,    _________________LOWER_R2__________________, KC_PIPE,
        _______, _________________LOWER_L3__________________,    _________________LOWER_R3__________________, _______,
                          _______, _______, _______, _______,    _______, AC_TOGG, _______, _______
    ),
    [_RAISE] = LAYOUT_wrapper(
        KC_F12,  _________________FUNC_LEFT_________________,    _________________FUNC_RIGHT________________, KC_F11,
        KC_GRV,  _________________RAISE_L1__________________,    _________________RAISE_R1__________________, _______,
        _______, _________________RAISE_L2__________________,    _________________RAISE_R2__________________, KC_BSLS,
        _______, _________________RAISE_L3__________________,    _________________RAISE_R3__________________, _______,
                          _______, _______, _______, _______,    _______, _______, _______, _______
    ),
    [_ADJUST] = LAYOUT_wrapper(
        QK_MAKE, KC_WIDE,KC_AUSSIE,KC_SCRIPT,KC_ZALGO,KC_SUPER, KC_NOMODE,KC_COMIC,KC_REGIONAL,TG_GAME,TG_DBLO, QK_BOOT,
        VRSN,    _________________ADJUST_L1_________________,    _________________ADJUST_R1_________________, EE_CLR,
        KEYLOCK, _________________ADJUST_L2_________________,    _________________ADJUST_R2_________________, TG_MODS,
        UC_NEXT, _________________ADJUST_L3_________________,    _________________ADJUST_R3_________________, KC_MPLY,
                          DB_TOGG, QK_RBT,  AC_TOGG,_______,    _______, KC_NUKE, OLED_LOCK, _______
    )
};
// clang-format on

#ifdef ENCODER_MAP_ENABLE
// clang-format off
const uint16_t PROGMEM encoder_map[][NUM_ENCODERS][NUM_DIRECTIONS] = {
    [_DEFAULT_LAYER_1] = { ENCODER_CCW_CW( KC_VOLD, KC_VOLU ), ENCODER_CCW_CW( MS_WHLD, MS_WHLD ) },
    [_DEFAULT_LAYER_2] = { ENCODER_CCW_CW( _______, _______ ), ENCODER_CCW_CW( _______, _______ ) },
    [_DEFAULT_LAYER_3] = { ENCODER_CCW_CW( _______, _______ ), ENCODER_CCW_CW( _______, _______ ) },
    [_DEFAULT_LAYER_4] = { ENCODER_CCW_CW( _______, _______ ), ENCODER_CCW_CW( _______, _______ ) },
    [_GAMEPAD]         = { ENCODER_CCW_CW( _______, _______ ), ENCODER_CCW_CW( _______, _______ ) },
    [_DIABLO]          = { ENCODER_CCW_CW( _______, _______ ), ENCODER_CCW_CW( _______, _______ ) },
    [_MOUSE]           = { ENCODER_CCW_CW( _______, _______ ), ENCODER_CCW_CW( _______, _______ ) },
    [_MEDIA]           = { ENCODER_CCW_CW( _______, _______ ), ENCODER_CCW_CW( _______, _______ ) },
    [_RAISE]           = { ENCODER_CCW_CW( OL_BINC, OL_BDEC ), ENCODER_CCW_CW( KC_PGDN, KC_PGUP ) },
    [_LOWER]           = { ENCODER_CCW_CW( RM_NEXT, RM_PREV ), ENCODER_CCW_CW( RM_HUEU, RM_HUED ) },
    [_ADJUST]          = { ENCODER_CCW_CW( CK_UP,   CK_DOWN ), ENCODER_CCW_CW( _______, _______ ) },
};
// clang-format on
#endif // ENCODER_MAP_ENABLE

#if defined(OLED_ENABLE) && defined(OLED_DISPLAY_128X128)
#    ifdef UNICODE_COMMON_ENABLE
#        include "process_unicode_common.h"
#        include "keyrecords/unicode.h"
#    endif

extern const char PROGMEM display_border[3 + 1];

extern bool is_oled_enabled;

bool oled_task_keymap(void) {
    // No right side oled, so just exit.
    if (!is_keyboard_left()) {
        return false;
    }

    oled_write_raw_P(header_image, sizeof(header_image));
    oled_set_cursor(4, 0);
    oled_write_P(PSTR(" Dilemma Max"), true);

    render_default_layer_state(1, 1);
    render_layer_state(1, 2);
    render_pet(0, 5);
    render_wpm(1, 7, 5);
#    if defined(POINTING_DEVICE_ENABLE)
    render_pointing_dpi_status(
        dilemma_get_pointer_sniping_enabled() ? dilemma_get_pointer_sniping_dpi() : dilemma_get_pointer_default_dpi(),
        1, 7, 6);
    render_mouse_mode(17, 6);
#    else
    render_matrix_scan_rate(1, 7, 6);
#    endif
    render_bootmagic_status(7, 7);
    render_user_status(1, 9);

    render_mod_status(get_mods() | get_oneshot_mods(), 1, 10);
    render_keylock_status(host_keyboard_led_state(), 1, 11);
    render_unicode_mode(1, 12);

    render_os(1, 13);
    render_rgb_mode(1, 14);

    for (uint8_t i = 1; i < 15; i++) {
        oled_set_cursor(0, i);
        oled_write_raw_P(display_border, sizeof(display_border));
        oled_set_cursor(21, i);
        oled_write_raw_P(display_border, sizeof(display_border));
    }

    oled_set_cursor(0, 15);
    oled_write_raw_P(footer_image2, sizeof(footer_image2));
#    ifdef DISPLAY_KEYLOGGER_ENABLE
    oled_set_cursor(4, 15);
    oled_write(display_keylogger_string, true);
#    endif
    return false;
}
#endif

#ifdef RGB_MATRIX_ENABLE
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
    if (userspace_config.rgb.layer_change) {
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
            default:
                {
                    check_default_layer(0, LED_FLAG_UNDERGLOW, led_min, led_max);
                    break;
                }
        }
        check_default_layer(0, LED_FLAG_MODIFIER, led_min, led_max);
    }
    return false;
}
#endif

uint16_t get_tapping_term_keymap(uint16_t keycode, keyrecord_t *record) {
    if ((record->event.key.row == 4 || record->event.key.row == 9) && record->event.key.col == 3) {
        return 400;
    }
    return TAPPING_TERM;
}

#ifdef QUANTUM_PAINTER_ENABLE
#    include "qp.h"
#    include "bkb_logo_mod.qgf.h"

static painter_device_t       display;
static painter_image_handle_t my_image;
#endif // QUANTUM_PAINTER_ENABLE

void keyboard_post_init_keymap(void) {
#ifdef QUANTUM_PAINTER_ENABLE
    if (is_keyboard_left()) {
        display = qp_gc9a01_make_spi_device(240, 240, DISPLAY_CS_PIN, DISPLAY_DC_PIN, DISPLAY_RST_PIN, 4, 0);

        qp_init(display, QP_ROTATION_0);
        qp_clear(display);
        my_image = qp_load_image_mem(gfx_bkb_logo_mod);
        qp_drawimage(display, 0, 0, my_image);
        qp_flush(display);
    }
#endif // QUANTUM_PAINTER_ENABLE
#ifdef RGB_MATRIX_ENABLE
    g_led_config.flags[48] = g_led_config.flags[50] = g_led_config.flags[51] = g_led_config.flags[100] =
        g_led_config.flags[101] = g_led_config.flags[102] = g_led_config.flags[103] = LED_FLAG_MODIFIER;
#endif
}
