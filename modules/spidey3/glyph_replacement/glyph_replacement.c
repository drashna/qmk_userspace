// Copyright 2025 Joshua Diamond josh@windowoffire.com (@spidey3)
// SPDX-License-Identifier: GPL-2.0-or-later

#include QMK_KEYBOARD_H

uint16_t glyph_replace_mode = GLYPH_REPLACE_MODE_NORMAL;

static uint32_t math_glyph_exceptions(const uint16_t keycode, const bool shifted) {
    bool caps = host_keyboard_led_state().caps_lock;
    if (shifted != caps) {
        switch (keycode) {
            // clang-format off
            case KC_C: return 0x2102;
            case KC_H: return 0x210D;
            case KC_N: return 0x2115;
            case KC_P: return 0x2119;
            case KC_Q: return 0x211A;
            case KC_R: return 0x211D;
            case KC_Z: return 0x2124;
                // clang-format on
        }
    }
    return 0;
}

static bool replace_glyph(uint16_t keycode, keyrecord_t *record, uint32_t baseAlphaLower, uint32_t baseAlphaUpper, uint32_t zeroGlyph, uint32_t baseNumberOne, uint32_t spaceGlyph, uint32_t (*exceptions)(const uint16_t keycode, const bool shifted), uint8_t temp_mod, uint8_t temp_osm) {
    void _register(uint32_t codepoint) {
        unicode_input_start();
        register_hex32(codepoint);
        unicode_input_finish();
    }

    if ((((temp_mod | temp_osm) & (MOD_MASK_CTRL | MOD_MASK_ALT | MOD_MASK_GUI))) == 0) {
        bool shifted = ((temp_mod | temp_osm) & MOD_MASK_SHIFT);
        if (exceptions) {
            uint32_t res = exceptions(keycode, shifted);
            if (res) {
                if (record->event.pressed) {
                    _register(res);
                }
                return false;
            }
        }
        switch (keycode) {
            case KC_A ... KC_Z:
                if (record->event.pressed) {
                    clear_mods();
#ifndef NO_ACTION_ONESHOT
                    clear_oneshot_mods();
#endif

                    bool     caps = host_keyboard_led_state().caps_lock;
                    uint32_t base = ((shifted == caps) ? baseAlphaLower : baseAlphaUpper);
                    _register(base + (keycode - KC_A));
                    set_mods(temp_mod);
                }
                return false;
            case KC_0:
                if (shifted) { // skip shifted numbers, so that we can still use symbols etc.
                    return true;
                }
                if (record->event.pressed) {
                    _register(zeroGlyph);
                }
                return false;
            case KC_1 ... KC_9:
                if (shifted) { // skip shifted numbers, so that we can still use symbols etc.
                    return true;
                }
                if (record->event.pressed) {
                    _register(baseNumberOne + (keycode - KC_1));
                }
                return false;
            case KC_SPACE:
                if (record->event.pressed) {
                    _register(spaceGlyph); // em space
                }
                return false;
        }
    }

    return true;
}

bool process_record_glyph_replacement(uint16_t keycode, keyrecord_t *record) {
    if (!process_record_glyph_replacement_kb(keycode, record)) {
        return false;
    }

    uint8_t mods = get_mods();
#ifndef NO_ACTION_ONESHOT
    uint8_t osm = get_oneshot_mods();
#else
    uint8_t osm = 0;
#endif

    if (record->event.pressed) {
        switch (keycode) {
            case GLYPH_REPLACE_MODE_NORMAL ... GLYPH_REPLACE_MODE_MATH:
                glyph_replace_mode = (glyph_replace_mode == keycode) ? GLYPH_REPLACE_MODE_NORMAL : keycode;
                break;
        }
    }

    switch (keycode) {
        case KC_A ... KC_0:
        case KC_SPACE:
            switch (glyph_replace_mode) {
                case GLYPH_REPLACE_MODE_WIDE:
                    return replace_glyph(keycode, record, 0xFF41, 0xFF21, 0xFF10, 0xFF11, 0x2003, NULL, mods, osm);
                case GLYPH_REPLACE_MODE_SCRIPT:
                    return replace_glyph(keycode, record, 0x1D4EA, 0x1D4D0, 0x1D7CE, 0x1D7CF, 0x2002, NULL, mods, osm);
                case GLYPH_REPLACE_MODE_BLOCKS:
                    return replace_glyph(keycode, record, 0x1F170, 0x1F170, '0', '1', 0x2002, NULL, mods, osm);
                case GLYPH_REPLACE_MODE_CIRCLE:
                    return replace_glyph(keycode, record, 0x1F150, 0x1F150, '0', '1', 0x2002, NULL, mods, osm);
                case GLYPH_REPLACE_MODE_SQUARE:
                    return replace_glyph(keycode, record, 0x1F130, 0x1F130, '0', '1', 0x2002, NULL, mods, osm);
                case GLYPH_REPLACE_MODE_PARENS:
                    return replace_glyph(keycode, record, 0x1F110, 0x1F110, '0', '1', 0x2002, NULL, mods, osm);
                case GLYPH_REPLACE_MODE_FRAKTR:
                    return replace_glyph(keycode, record, 0x1D586, 0x1D56C, '0', '1', 0x2002, NULL, mods, osm);
                case GLYPH_REPLACE_MODE_BOLD:
                    return replace_glyph(keycode, record, 0x1D41A, 0x1D400, '0', '1', 0x2002, NULL, mods, osm);
                case GLYPH_REPLACE_MODE_MATH:
                    return replace_glyph(keycode, record, 0x1D552, 0x1D538, '0', '1', 0x2002, &math_glyph_exceptions, mods, osm);
            }
    }

    return true;
}
