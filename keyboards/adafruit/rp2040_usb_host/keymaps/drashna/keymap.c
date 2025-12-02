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
#    ifdef COMMUNITY_MODULE_LAYER_MAP_ENABLE
#        include "layer_map.h"
#    endif // COMMUNITY_MODULE_LAYER_MAP_ENABLE
#    ifdef COMMUNITY_MODULE_DISPLAY_MENU_ENABLE
#        include "oled_render_menu.h"
#    endif // COMMUNITY_MODULE_DISPLAY_MENU_ENABLE

extern bool is_oled_enabled;

static const char PROGMEM code_to_name[256] = {
    // clang-format off
//   0    1    2    3    4    5    6    7    8    9    A    B    c    D    E    F
    ' ', ' ', ' ', ' ', 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l',  // 0x
    'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z', '1', '2',  // 1x
    '3', '4', '5', '6', '7', '8', '9', '0',  20,  19,0xC6,  26,  22, '-', '=', '[',  // 2x
    ']','\\', '#', ';','\'', '`', ',', '.', '/',0xC9,0xD5,0xD6,0xD7,0xD8,0xD9,0xDA,  // 3x
   0xDB,0xDC,0xDD,0xDE,0XDF,0xFC, 'P',0xCB,  19, 'I',  17,  30,0xC7,  16,  31,  26,  // 4x
     27,  25,  24, 'N', '/', '*', '-', '+',  23, '1', '2', '3', '4', '5', '6', '7',  // 5x
    '8', '9', '0', '.','\\',0xC8,   0, '=', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',  // 6x
    ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',0xCD,  // 7x
   0xCE,0xCF,0xC9,0xCA,0xCB, ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',  // 8x
    ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',  // 9x
    ' ', ' ', ' ', ' ', ' ',   0, ' ', ' ',0xCD,0xCE,0xCF,0xD0,0xD1,0xD2,0xD3, ' ',  // Ax
    ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',  // Bx
    ' ',0x9E,0x9E, ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',0x80,0x80,0x80,0x80,  // Cx
   0x80,0x81,0x82,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,  // Dx
    'C', 'S', 'A', 'G', 'C', 'S', 'A', 'G', ' ', ' ', ' ', ' ', ' ',  24,  26,  24,  // Ex
     25, ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',  24,  25,  27,  26, ' ', ' ', ' '   // Fx
    // clang-format on
};

bool oled_task_keymap(void) {
    oled_write_raw_P(header_image, sizeof(header_image));
    oled_set_cursor(4, 0);
    oled_write_P(PSTR("USB Converter"), true);

    render_pet(0, 1);
    render_wpm(1, 7, 1);
    render_matrix_scan_rate(1, 7, 2);
    // #    endif
    render_bootmagic_status(7, 3);
    render_user_status(1, 5);

    render_mod_status(get_mods() | get_oneshot_mods(), 1, 6);
    render_keylock_status(host_keyboard_led_state(), 1, 7);
    render_os(1, 8);

#    if defined(COMMUNITY_MODULE_DISPLAY_MENU_ENABLE)
    if (!oled_render_menu(0, 7, 8, true))
#    endif
    {
#    ifdef COMMUNITY_MODULE_LAYER_MAP_ENABLE

        for (uint8_t i = 0; i < LAYER_MAP_ROWS; i++) {
            oled_set_cursor(1, 9 + i);
            for (uint8_t j = 0; j < LAYER_MAP_COLS; j++) {
                uint16_t keycode = extract_basic_keycode(layer_map[i][j], NULL, false);
                if (j > oled_max_chars() - 2) {
                    break;
                }
                char code = 0;
                if (keycode > 0xFF) {
                    if (keycode == UC_IRNY) {
                        code = 0xFD;
                    } else if (keycode == UC_CLUE) {
                        code = 0xFE;
                    } else if (keycode == DISPLAY_MENU) {
                        code = 0xC8;
                    } else {
                        keycode = 0;
                    }
                }
                if (keycode < ARRAY_SIZE(code_to_name)) {
                    code = pgm_read_byte(&code_to_name[keycode]);
                }

                oled_write_char(code, peek_matrix_layer_map(i, j));
            }
        }
#    else
        render_autocorrected_info(1, 9);
#    endif
    }
    // nrender_autocorrected_info(1, 9);

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
    oled_write(get_oled_keylogger_str(), true);
#    endif
    return false;
}
#endif

#ifdef COMMUNITY_MODULE_LAYER_MAP_ENABLE
// clang-format off
keypos_t layer_remap[LAYER_MAP_ROWS][LAYER_MAP_COLS] = {
    { { .row =   5, .col =   1 }, { .row =   7, .col =   2 }, { .row =   7, .col =   3 }, { .row =   7, .col =   4 }, { .row =   7, .col =   5 }, { .row = 255, .col = 255 }, { .row =   7, .col =   6 }, { .row =   7, .col =   7 }, { .row =   8, .col =   0 }, { .row =   8, .col =   1 }, { .row = 255, .col = 255 }, { .row =   8, .col =   2 }, { .row =   8, .col =   3 }, { .row =   8, .col =   4 }, { .row =   8, .col =   5 }, { .row = 255, .col = 255 }, { .row =   8, .col =   6 }, { .row =   8, .col =   7 }, { .row =   9, .col =   0 }, { .row = 255, .col = 255 }, { .row = 255, .col = 255 }, { .row = 255, .col = 255 }, { .row = 255, .col = 255 }, { .row = 255, .col = 255 } },
    { { .row =   6, .col =   5 }, { .row =   3, .col =   6 }, { .row =   3, .col =   7 }, { .row =   4, .col =   0 }, { .row =   4, .col =   1 }, { .row =   4, .col =   2 }, { .row =   4, .col =   3 }, { .row =   4, .col =   4 }, { .row =   4, .col =   5 }, { .row =   4, .col =   6 }, { .row =   4, .col =   7 }, { .row =   5, .col =   5 }, { .row =   5, .col =   6 }, { .row =   5, .col =   2 }, { .row =   5, .col =   2 }, { .row = 255, .col = 255 }, { .row =   9, .col =   1 }, { .row =   9, .col =   2 }, { .row =   9, .col =   3 }, { .row = 255, .col = 255 }, { .row =  10, .col =   3 }, { .row =  10, .col =   4 }, { .row =  10, .col =   5 }, { .row =  10, .col =   6 } },
    { { .row =   5, .col =   3 }, { .row =   2, .col =   4 }, { .row =   3, .col =   2 }, { .row =   1, .col =   0 }, { .row =   2, .col =   5 }, { .row =   2, .col =   7 }, { .row =   3, .col =   4 }, { .row =   3, .col =   0 }, { .row =   1, .col =   4 }, { .row =   2, .col =   2 }, { .row =   2, .col =   3 }, { .row =   5, .col =   7 }, { .row =   6, .col =   0 }, { .row =   6, .col =   1 }, { .row =   6, .col =   1 }, { .row = 255, .col = 255 }, { .row =   9, .col =   4 }, { .row =   9, .col =   5 }, { .row =   9, .col =   6 }, { .row = 255, .col = 255 }, { .row =  11, .col =   7 }, { .row =  12, .col =   0 }, { .row =  12, .col =   1 }, { .row =  10, .col =   7 } },
    { { .row =   7, .col =   1 }, { .row =   0, .col =   4 }, { .row =   2, .col =   6 }, { .row =   0, .col =   7 }, { .row =   1, .col =   1 }, { .row =   1, .col =   2 }, { .row =   1, .col =   3 }, { .row =   1, .col =   4 }, { .row =   1, .col =   5 }, { .row =   1, .col =   6 }, { .row =   1, .col =   7 }, { .row =   6, .col =   3 }, { .row =   6, .col =   4 }, { .row =   5, .col =   0 }, { .row =   5, .col =   0 }, { .row = 255, .col = 255 }, { .row = 255, .col = 255 }, { .row = 255, .col = 255 }, { .row = 255, .col = 255 }, { .row = 255, .col = 255 }, { .row =  11, .col =   4 }, { .row =  11, .col =   5 }, { .row =  11, .col =   6 }, { .row =  10, .col =   7 } },
    { { .row =  28, .col =   1 }, { .row =  28, .col =   1 }, { .row =   3, .col =   5 }, { .row =   3, .col =   3 }, { .row =   0, .col =   6 }, { .row =   3, .col =   1 }, { .row =   0, .col =   5 }, { .row =   2, .col =   1 }, { .row =   2, .col =   0 }, { .row =   6, .col =   6 }, { .row =   6, .col =   7 }, { .row =   7, .col =   0 }, { .row =  28, .col =   5 }, { .row =  28, .col =   5 }, { .row =  28, .col =   5 }, { .row = 255, .col = 255 }, { .row = 255, .col = 255 }, { .row =  10, .col =   2 }, { .row = 255, .col = 255 }, { .row = 255, .col = 255 }, { .row =  11, .col =   1 }, { .row =  11, .col =   2 }, { .row =  11, .col =   3 }, { .row =  11, .col =   0 } },
    { { .row =  28, .col =   0 }, { .row =  28, .col =   3 }, { .row =  28, .col =   2 }, { .row =   5, .col =   4 }, { .row =   5, .col =   4 }, { .row =   5, .col =   4 }, { .row =   5, .col =   4 }, { .row =   5, .col =   4 }, { .row =   5, .col =   4 }, { .row =   5, .col =   4 }, { .row =   5, .col =   4 }, { .row =  28, .col =   6 }, { .row =  28, .col =   7 }, { .row =  12, .col =   5 }, { .row =  28, .col =   4 }, { .row = 255, .col = 255 }, { .row =  10, .col =   0 }, { .row =  10, .col =   1 }, { .row =   9, .col =   7 }, { .row = 255, .col = 255 }, { .row =  12, .col =   2 }, { .row =  12, .col =   2 }, { .row =  12, .col =   3 }, { .row =  11, .col =   0 } },
};
#endif

void keyboard_post_init_keymap(void) {
    // Customise these values to desired behaviour
    userspace_config.display.painter.left.display_logo_cycle = true;
}
