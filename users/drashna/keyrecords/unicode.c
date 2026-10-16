// Copyright 2020 @ridingqwerty
// Copyright 2020 @tzarc
// Copyright 2021 Christopher Courtney, aka Drashna Jael're  (@drashna) <drashna@live.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "quantum/unicode/unicode.h"
#include "unicode.h"
#include "process_records.h"
#include "process_unicode_common.h"
#include "drashna_util.h"
#if defined(COMMUNITY_MODULE_DISPLAY_KEYLOGGER_ENABLE)
#    include "display_keylogger.h"
void keylog_append(const char *str);

#endif

enum {
    ST_IDLE,
    ST_SLASH,
    ST_SLASH_S,
    ST_QUES,
};
static uint8_t irony_state = ST_IDLE;

/**
 * @brief Main handler for unicode input
 *
 * @param keycode Keycode from switch matrix
 * @param record keyrecord_t data structure
 * @return true Send keycode from matrix to host
 * @return false Stop processing and do not send to host
 */

bool process_record_unicode(uint16_t keycode, keyrecord_t *record) {
    if (!record->event.pressed) return true;

    switch (keycode) {
        case UC_FLIP: // (ノಠ痊ಠ)ノ彡┻━┻
            send_unicode_string("(ノಠ痊ಠ)ノ彡┻━┻");
            return true;

        case UC_TABL: // ┬─┬ノ( º _ ºノ)
            send_unicode_string("┬─┬ノ( º _ ºノ)");
            return true;

        case UC_SHRG: // ¯\_(ツ)_/¯
            send_unicode_string("¯\\_(ツ)_/¯");
            return true;

        case UC_DISA: // ಠ_ಠ
            send_unicode_string("ಠ_ಠ");
            return true;

        case UC_IRNY: // ⸮
            register_unicode(0x2E2E);
#if defined(COMMUNITY_MODULE_DISPLAY_KEYLOGGER_ENABLE)
            display_keylogger_append("⸮");
#endif // COMMUNITY_MODULE_DISPLAY_KEYLOGGER_ENABLE
            return true;
        case UC_CLUE: // ‽
            register_unicode(0x203D);
#if defined(COMMUNITY_MODULE_DISPLAY_KEYLOGGER_ENABLE)
            display_keylogger_append("‽");
#endif // COMMUNITY_MODULE_DISPLAY_KEYLOGGER_ENABLE
            return true;
    }

    // Ignore bare modifier presses so they don't reset the sequence
    if (keycode >= KC_LCTL && keycode <= KC_RGUI) return true;
    if (IS_QK_LAYER_TAP(keycode)) return true;

    // Unwrap mod-tap / layer-tap keys when tapped
    uint16_t code = extract_non_basic_keycode(keycode, record, false);
    uint8_t  mods = get_mods() | get_oneshot_mods();
    if (IS_QK_MODS(code)) {
        mods |= QK_MODS_GET_MODS(code);
    }
    bool    shifted = mods & MOD_MASK_SHIFT;
    uint8_t prev    = irony_state;
    irony_state     = ST_IDLE;

    // Any non-shift modifier (ctrl/alt/gui) cancels the sequence
    if (mods & ~MOD_MASK_SHIFT) return true;

    // "?!" -> ‽
    if (prev == ST_QUES && ((code == KC_1 && shifted) || code == KC_EXLM)) {
        tap_code16_nomods(KC_BSPC); // remove the "?"
        register_unicode(0x203D);
#if defined(COMMUNITY_MODULE_DISPLAY_KEYLOGGER_ENABLE)
        display_keylogger_shift_right();
        display_keylogger_shift_right();
        display_keylogger_append("‽");
#endif // COMMUNITY_MODULE_DISPLAY_KEYLOGGER_ENABLE
        return false;
    }

    // "/s" + word boundary -> ⸮
    bool word_boundary = code == KC_SPC || code == KC_ENT || code == KC_TAB || code == KC_DOT || code == KC_COMM ||
                         code == KC_SCLN || code == KC_QUOT || (shifted && (code == KC_1 || code == KC_SLSH));
    if (prev == ST_SLASH_S && word_boundary) {
        tap_code16_nomods(KC_BSPC); // remove "s"
        tap_code16_nomods(KC_BSPC); // remove "/"
        register_unicode(0x2E2E);
#if defined(COMMUNITY_MODULE_DISPLAY_KEYLOGGER_ENABLE)
        display_keylogger_shift_right();
        display_keylogger_shift_right();
        display_keylogger_shift_right();
        display_keylogger_append("⸮");
        add_keycode_to_display_keylogger_str(keycode, mods);
#endif // DISPLAY_KEYLOGGER_ENABLE && CUSTOM_QUANTUM_PAINTER_ENABLE
        return true;
    }
    if (code == KC_SLSH) {
        irony_state = shifted ? ST_QUES : ST_SLASH; // "?" vs "/"
    } else if (code == KC_S && !shifted && prev == ST_SLASH) {
        irony_state = ST_SLASH_S;
    }

    return true;
}

/**
 * @brief Initialize the default unicode mode on firmware startup
 *
 */
void keyboard_post_init_unicode(void) {
    unicode_input_mode_init();
}

/**
 * @brief Set the unicode input mode without extra functionality
 *
 * @param input_mode
 */
void set_unicode_input_mode_soft(uint8_t input_mode) {
    unicode_config.input_mode = input_mode;
    unicode_input_mode_set_kb(input_mode);
}

#ifdef COMMUNITY_MODULE_UNICODE_TYPING_ENABLE
#    include "drashna_runtime.h"
#    include "unicode_typing.h"
void post_process_record_unicode_typing_user(uint16_t keycode, keyrecord_t *record) {
    if (record->event.pressed) {
        userspace_runtime_state.unicode.typing_mode = get_unicode_typing_mode();
    }
}
#endif
