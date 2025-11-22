// Copyright 2025 Christopher Courtney, aka Drashna Jael're  (@drashna) <drashna@live.com>
// SPDX-License-Identifier: GPL-3.0-or-later

// SPDX-License-Identifier: GPL-2.0-or-later
#include "drashna.h"

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    /*


     ,----.  ,-------------------. ,-------------------. ,-------------------.  ,--------------.
     | Esc|  |  F1|  F2|  F3|  F4| |  F5|  F6|  F7|  F8| |  F9| F10| F11| F12|  | PS | SL | PA |
     `----'  `-------------------' '-------------------' `-------------------'  `--------------'
     ,----------------------------------------------------+-------------------. ,--------------. ,-------------------.
     |  `~|  1!|  2@|  3#|  4$|  5%|  6^|  7&|  8*|  9(|  0)|  [{|  ]}|  Bspc | | In | Hm | PU | |  Nl|  P/|  P*|  P-|
     |------+----+----+----+----+----+----+----+----+----+----+----+----+-----| |----+----+----| |----+----+----+----|
     |   Tab| '" |  ,<|  .>|  P |  Y |  F |  G |  C |  R |  L |  /?|  =+|  \| | | Dl | Ed | PD | |  P7|  P8|  P9|    |
     |--------+----+----+----+----+----+----+----+----+----+----+----+--------| `--------------' |----+----+----+  P+|
     |   MO1  |  A |  O |  E |  U |  I |  D |  H |  T |  N |  S |  -_|  Enter |                  |  P4|  P5|  P6|    |
     |----------+----+----+----+----+----+----+----+----+----+----+-----------|      ,----.      |----+----+----+----|
     |    TD1   |  ;:|  Q |  J |  K |  X |  B |  M |  W |  V |  Z |    TD2    |      | Up |      |  P1|  P2|  P3|    |
     |-----+-----+-----+------------------------------+-----+-----+-----+-----| ,----+----+----. +---------+----+  PE|
     | LC  | LG  | LA  |                              | RA  | RG  | Mn  | RC  | | Lf | Dn |  Rt| |    P0   |  P.|    |
     `------------------------------------------------------------------------' `--------------' `-------------------'

     */

    /*
     * ┌───┐   ┌───┬───┬───┬───┐ ┌───┬───┬───┬───┐ ┌───┬───┬───┬───┐ ┌───┬───┬───┐
     * │Esc│   │F1 │F2 │F3 │F4 │ │F5 │F6 │F7 │F8 │ │F9 │F10│F11│F12│ │PSc│Scr│Pse│
     * └───┘   └───┴───┴───┴───┘ └───┴───┴───┴───┘ └───┴───┴───┴───┘ └───┴───┴───┘
     * ┌───┬───┬───┬───┬───┬───┬───┬───┬───┬───┬───┬───┬───┬───────┐ ┌───┬───┬───┐ ┌───┬───┬───┬───┐
     * │ ` │ 1 │ 2 │ 3 │ 4 │ 5 │ 6 │ 7 │ 8 │ 9 │ 0 │ - │ = │ Backsp│ │Ins│Hom│PgU│ │Num│ / │ * │ - │
     * ├───┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─────┤ ├───┼───┼───┤ ├───┼───┼───┼───┤
     * │ Tab │ Q │ W │ E │ R │ T │ Y │ U │ I │ O │ P │ [ │ ] │  \  │ │Del│End│PgD│ │ 7 │ 8 │ 9 │   │
     * ├─────┴┬──┴┬──┴┬──┴┬──┴┬──┴┬──┴┬──┴┬──┴┬──┴┬──┴┬──┴┬──┴─────┤ └───┴───┴───┘ ├───┼───┼───┤ + │
     * │ Caps │ A │ S │ D │ F │ G │ H │ J │ K │ L │ ; │ ' │  Enter │               │ 4 │ 5 │ 6 │   │
     * ├──────┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴────────┤     ┌───┐     ├───┼───┼───┼───┤
     * │ Shift  │ Z │ X │ C │ V │ B │ N │ M │ , │ . │ / │    Shift │     │ ↑ │     │ 1 │ 2 │ 3 │   │
     * ├────┬───┴┬──┴─┬─┴───┴───┴───┴───┴───┴──┬┴───┼───┴┬────┬────┤ ┌───┼───┼───┐ ├───┴───┼───┤Ent│
     * │Ctrl│GUI │Alt │                        │ Alt│ GUI│Menu│Ctrl│ │ ← │ ↓ │ → │ │   0   │ . │   │
     * └────┴────┴────┴────────────────────────┴────┴────┴────┴────┘ └───┴───┴───┘ └───────┴───┴───┘
     */
    // clang-format off
    [0] = LAYOUT_fullsize_ansi(

        KC_ESC,           KC_F1,   KC_F2,   KC_F3,   KC_F4,   KC_F5,   KC_F6,   KC_F7,   KC_F8,   KC_F9,   KC_F10,  KC_F11,  KC_F12,     KC_PSCR, KC_SCRL, KC_PAUS,

        KC_GRV,  KC_1,    KC_2,    KC_3,    KC_4,    KC_5,    KC_6,    KC_7,    KC_8,    KC_9,    KC_0,    KC_MINS, KC_EQL,  KC_BSPC,    KC_INS,  KC_HOME, KC_PGUP,    KC_NUM,  KC_PSLS, KC_PAST, KC_PMNS,
        KC_TAB,  KC_Q,    KC_W,    KC_E,    KC_R,    KC_T,    KC_Y,    KC_U,    KC_I,    KC_O,    KC_P,    KC_LBRC, KC_RBRC, KC_BSLS,    KC_DEL,  KC_END,  KC_PGDN,    KC_P7,   KC_P8,   KC_P9,   KC_PPLS,
        KC_CAPS, KC_A,    KC_S,    KC_D,    KC_F,    KC_G,    KC_H,    KC_J,    KC_K,    KC_L,    KC_SCLN, KC_QUOT,          KC_ENT,                                   KC_P4,   KC_P5,   KC_P6,
        KC_LSFT,          KC_Z,    KC_X,    KC_C,    KC_V,    KC_B,    KC_N,    KC_M,    KC_COMM, KC_DOT,  KC_SLSH,          KC_RSFT,             KC_UP,               KC_P1,   KC_P2,   KC_P3,   KC_PENT,
        KC_LCTL, KC_LGUI, KC_LALT,                            KC_SPC,                             KC_RALT, KC_RGUI, KC_APP,  KC_RCTL,    KC_LEFT, KC_DOWN, KC_RGHT,    KC_P0,            KC_PDOT
    )
    // clang-format on
};

#ifdef OLED_ENABLE
#    include "display/oled/oled_stuff.h"

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
    // #    if defined(POINTING_DEVICE_ENABLE)
    // render_pointing_dpi_status(
    //     dilemma_get_pointer_sniping_enabled() ? dilemma_get_pointer_sniping_dpi() :
    //     dilemma_get_pointer_default_dpi(), 1, 7, 6);
    // render_mouse_mode(17, 6);
    // #    else
    render_matrix_scan_rate(1, 7, 6);
    // #    endif
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

void keyboard_post_init_keymap(void) {
    // Customise these values to desired behaviour
    userspace_config.display.painter.left.display_logo_cycle = true;
}
