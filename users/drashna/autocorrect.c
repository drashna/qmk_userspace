
// Copyright 2025 Christopher Courtney, aka Drashna Jael're  (@drashna) <drashna@live.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#if defined(AUTOCORRECT_ENABLE) || defined(COMMUNITY_MODULE_AUTOCORRECT_ENABLE)
#    include <quantum.h>
#    include "drashna_names.h"
#    include "drashna_util.h"
#    include "drashna_runtime.h"

#    if defined(AUDIO_ENABLE)
#        ifdef USER_SONG_LIST
float autocorrect_song[][2] = SONG(MARIO_GAMEOVER);
#        else  // USER_SONG_LIST
float autocorrect_song[][2] = SONG(PLOVER_GOODBYE_SOUND);
#        endif // USER_SONG_LIST
#    endif

// 2 strings, 2q chars each + null terminator. max autocorrect length is 19 chars but 128px/6 supports 21 chars
char autocorrected_str[2][21]     = {"    automatically\0", "      corrected\0"};
char autocorrected_str_raw[2][21] = {"automatically\0", "corrected\0"};
bool autocorrect_str_has_changed  = false;

#    if defined(DISPLAY_KEYLOGGER_ENABLE) && defined(CUSTOM_QUANTUM_PAINTER_ENABLE)
#        include "users/drashna/display/painter/keylogger.h"
#        include <send_string.h>
#        include <ctype.h>

#        define PGM_LOADBIT(mem, pos) ((pgm_read_byte(&((mem)[(pos) / 8])) >> ((pos) % 8)) & 0x01)
char send_string_get_next_ram(void *arg);

typedef struct send_string_memory_state_t {
    const char *string;
} send_string_memory_state_t;

void add_autocorrect_char_to_keylogger_str(char ascii_code) {
    if (ascii_code == '\a') { // BEL
        return;
    }

    uint8_t keycode    = pgm_read_byte(&ascii_to_keycode_lut[(uint8_t)ascii_code]);
    bool    is_shifted = PGM_LOADBIT(ascii_to_shift_lut, (uint8_t)ascii_code);
    bool    is_altgred = PGM_LOADBIT(ascii_to_altgr_lut, (uint8_t)ascii_code);

    uint8_t mods = 0;

    if (is_shifted) {
        mods |= MOD_BIT(KC_LEFT_SHIFT);
    }

    if (is_altgred) {
        mods |= MOD_BIT(KC_RIGHT_ALT);
    }

    add_keycode_to_keylogger_str(keycode, mods);
}

static void update_keylogger_string(char (*getter)(void *), void *arg) {
    while (1) {
        char ascii_code = getter(arg);
        if (!ascii_code) break;
        if (ascii_code == SS_QMK_PREFIX) {
            ascii_code = getter(arg);

            if (ascii_code == SS_TAP_CODE) {
                // tap
                uint8_t keycode = getter(arg);
                add_autocorrect_char_to_keylogger_str(keycode);
            } else if (ascii_code == SS_DOWN_CODE) {
                // down
                uint8_t keycode = getter(arg);
                add_autocorrect_char_to_keylogger_str(keycode);
            } else if (ascii_code == SS_UP_CODE) {
                // up
                getter(arg);
            } else if (ascii_code == SS_DELAY_CODE) {
                // delay
                int ms     = 0;
                ascii_code = getter(arg);

                while (isdigit(ascii_code)) {
                    ms *= 10;
                    ms += ascii_code - '0';
                    ascii_code = getter(arg);
                }
            }

            // if we had a delay that terminated with a null, we're done
            if (ascii_code == 0) break;
        } else {
            add_autocorrect_char_to_keylogger_str(ascii_code);
        }
    }
}
#    endif // DISPLAY_KEYLOGGER_ENABLE && CUSTOM_QUANTUM_PAINTER_ENABLE

bool apply_autocorrect(uint8_t backspaces, const char *str, char *typo, char *correct) {
    if (is_gaming_layer_active(layer_state)) {
        return false;
    }

    strncpy(autocorrected_str_raw[0], typo, sizeof(autocorrected_str_raw[0]) - 1);
    strncpy(autocorrected_str_raw[1], correct, sizeof(autocorrected_str_raw[1]) - 1);

    center_text(typo, autocorrected_str[0], sizeof(autocorrected_str[0]) - 1);
    center_text(correct, autocorrected_str[1], sizeof(autocorrected_str[1]) - 1);
    // printf("Autocorrected %s to %s (original: %s)\n", typo, correct, str);
    autocorrect_str_has_changed = true;
    for (uint8_t i = 0; i <= backspaces; ++i) {
#    if defined(WPM_ENABLE) && defined(WPM_ALLOW_COUNT_REGRESSION)
        update_wpm(KC_BSPC);
#    endif // WPM_ENABLE

#    if defined(DISPLAY_KEYLOGGER_ENABLE) && defined(CUSTOM_QUANTUM_PAINTER_ENABLE)
        keylog_shift_right();
#    endif // DISPLAY_KEYLOGGER_ENABLE && CUSTOM_QUANTUM_PAINTER_ENABLE
    }

#    if defined(DISPLAY_KEYLOGGER_ENABLE) && defined(CUSTOM_QUANTUM_PAINTER_ENABLE)
    send_string_memory_state_t state = {str};
    update_keylogger_string(send_string_get_next_ram, &state);

    if (userspace_runtime_state.last_keycode == KC_SPC) {
        // If the last keycode was space, we need to add a space to the keylogger string
        // so that it doesn't look like the autocorrected word is the first word in the sentence.
        add_autocorrect_char_to_keylogger_str(' ');
    }
#    endif // DISPLAY_KEYLOGGER_ENABLE && CUSTOM_QUANTUM_PAINTER_ENABLE

#    if defined(AUDIO_ENABLE)
    audio_play_melody(&autocorrect_song, NOTE_ARRAY_SIZE(autocorrect_song), false);
#    endif // AUDIO_ENABLE

    return true;
}
#endif // AUTOCORRECT_ENABLE || COMMUNITY_MODULE_AUTOCORRECT_ENABLE
