// Copyright 2020 Christopher Courtney, aka Drashna Jael're  (@drashna) <drashna@live.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "drashna.h"
#include "drashna_util.h"
#include <string.h>
#ifdef UNICODE_COMMON_ENABLE
#    include "keyrecords/unicode.h"
#endif // UNICODE_COMMON_ENABLE

userspace_config_t userspace_config;

#if defined(__arm__) && HAL_USE_I2C == TRUE
#    include "i2c_master.h"
#    include "debug.h"

#    ifndef I2C_SCANNER_TIMEOUT
#        define I2C_SCANNER_TIMEOUT 50
#    endif // I2C_SCANNER_TIMEOUT

void do_scan(void) {
    if (!userspace_config.debug.i2c_scanner_enable) {
        return;
    }
    uint8_t nDevices = 0;

    xprintf("Scanning for I2C Devices...\n");

    for (uint8_t address = 1; address < 127; address++) {
        // The i2c_scanner uses the return value of
        // i2c_ping_address to see if a device did acknowledge to the address.
        i2c_status_t error = i2c_ping_address(address << 1, I2C_SCANNER_TIMEOUT);
        if (error == I2C_STATUS_SUCCESS) {
            xprintf("  I2C device found at address 0x%02X\n", address);
            nDevices++;
        } else {
            // xprintf("  Unknown error (%u) at address 0x%02X\n", error, address);
        }
    }

    if (nDevices == 0) {
        xprintf("No I2C devices found\n");
    }
}

uint16_t scan_timer = 0;

void housekeeping_task_i2c_scanner(void) {
    if (timer_elapsed(scan_timer) > 5000) {
        do_scan();
        scan_timer = timer_read();
    }
}

void keyboard_post_init_i2c(void) {
    i2c_init();
    scan_timer = timer_read();
}
#endif // HAL_USE_I2C == TRUE

#if defined(AUTOCORRECT_ENABLE)
#    if defined(AUDIO_ENABLE)
#        ifdef USER_SONG_LIST
float autocorrect_song[][2] = SONG(MARIO_GAMEOVER);
#        else // USER_SONG_LIST
float autocorrect_song[][2] = SONG(PLOVER_GOODBYE_SOUND);
#        endif // USER_SONG_LIST
#    endif
// 2 strings, 2q chars each + null terminator. max autocorrect length is 19 chars but 128px/6 supports 21 chars
char autocorrected_str[2][21]     = {"    automatically\0", "      corrected\0"};
char autocorrected_str_raw[2][21] = {"automatically\0", "corrected\0"};
bool autocorrect_str_has_changed  = false;

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
#    if defined(WPM_ENABLE) && defined(WPM_ALLOW_COUNT_REGRESSION)
    for (uint8_t i = 0; i < backspaces; i++) {
        update_wpm(KC_BSPC);
    }
#    endif // WPM_ENABLE

#    if defined(AUDIO_ENABLE)
    audio_play_melody(&autocorrect_song, NOTE_ARRAY_SIZE(autocorrect_song), false);
#    endif // AUDIO_ENABLE
    return true;
}
#endif

#if defined(CAPS_WORD_ENABLE)
bool caps_word_press_user(uint16_t keycode) {
    switch (keycode) {
        // Keycodes that continue Caps Word, with shift applied.
        case KC_MINS:
            if (!keymap_config.swap_lctl_lgui) {
                return true;
            }
        case KC_A ... KC_Z:
            add_weak_mods(MOD_BIT(KC_LSFT)); // Apply shift to next key.
            return true;

        // Keycodes that continue Caps Word, without shifting.
        case KC_1 ... KC_0:
        case KC_BSPC:
        case KC_DEL:
        case KC_UNDS:
            return true;

        default:
            return false; // Deactivate Caps Word.
    }
}

#    if !defined(NO_ACTION_ONESHOT)
void oneshot_locked_mods_changed_user(uint8_t mods) {
    if (mods == MOD_BIT_LSHIFT || mods == MOD_BIT_RSHIFT) {
        del_mods(MOD_MASK_SHIFT);
        set_oneshot_locked_mods(~MOD_MASK_SHIFT & get_oneshot_locked_mods());
        caps_word_on();
    }
}
#    endif // !NO_ACTION_ONESHOT
#endif // CAPS_WORD_ENABLE

#if defined(OS_DETECTION_ENABLE)
typedef struct {
    bool    swap_ctl_gui;
#    ifdef UNICODE_COMMON_ENABLE
    uint8_t unicode_input_mode;
#    endif // UNICODE_COMMON_ENABLE
} os_detection_config_t;

bool process_detected_host_os_user(os_variant_t detected_os) {
    if (is_keyboard_master()) {
        os_detection_config_t os_detection_config = {
            .swap_ctl_gui = false,
#    ifdef UNICODE_COMMON_ENABLE
            .unicode_input_mode = UNICODE_MODE_WINCOMPOSE,
#    endif // UNICODE_COMMON_ENABLE
        };
        switch (detected_os) {
            case OS_UNSURE:
                xprintf("unknown OS Detected\n");
                break;
            case OS_LINUX:
                xprintf("Linux Detected\n");
#    ifdef UNICODE_COMMON_ENABLE
                os_detection_config.unicode_input_mode = UNICODE_MODE_LINUX;
#    endif // UNICODE_COMMON_ENABLE
                break;
            case OS_WINDOWS:
                xprintf("Windows Detected\n");
                break;
#    if 0
            case OS_WINDOWS_UNSURE:
                xprintf("Windows? Detected\n");
                break;
#    endif
            case OS_MACOS:
                xprintf("MacOS Detected\n");
                os_detection_config = (os_detection_config_t){
                    .swap_ctl_gui = true,
#    ifdef UNICODE_COMMON_ENABLE
                    .unicode_input_mode = UNICODE_MODE_MACOS,
#    endif // UNICODE_COMMON_ENABLE
                };
                userspace_config.pointing.accel.enabled = false;
                break;
            case OS_IOS:
                xprintf("iOS Detected\n");
                os_detection_config = (os_detection_config_t){
                    .swap_ctl_gui = true,
#    ifdef UNICODE_COMMON_ENABLE
                    .unicode_input_mode = UNICODE_MODE_MACOS,
#    endif // UNICODE_COMMON_ENABLE
                };
                userspace_config.pointing.accel.enabled = false;
                break;
#    if 0
            case OS_PS5:
                xprintf("PlayStation 5 Detected\n");
#        ifdef UNICODE_COMMON_ENABLE
                os_detection_config.unicode_input_mode = UNICODE_MODE_LINUX;
#        endif // UNICODE_COMMON_ENABLE
                break;
            case OS_HANDHELD:
                xprintf("Nintend Switch/Quest 2 Detected\n");
#        ifdef UNICODE_COMMON_ENABLE
                os_detection_config.unicode_input_mode = UNICODE_MODE_LINUX;
#        endif
                break;
#    endif
            default:
                xprintf("Unknown OS Detected\n");
                break;
        }
        keymap_config.swap_lctl_lgui = keymap_config.swap_rctl_rgui = os_detection_config.swap_ctl_gui;
#    ifdef UNICODE_COMMON_ENABLE
        set_unicode_input_mode_soft(os_detection_config.unicode_input_mode);
#    endif // UNICODE_COMMON_ENABLE
    }

    return true;
}
#endif // OS_DETECTION_ENABLE

static uint32_t last_matrix_scan_count = 0;
/**
 * @brief Get the matrix scan rate value
 *
 * @return uint32_t scans per second
 */
uint32_t get_matrix_scan_rate(void) {
    return last_matrix_scan_count;
}

/**
 * @brief Task to monitor and print the matrix scan rate
 */
void matrix_scan_rate_task(void) {
    static uint32_t matrix_timer      = 0;
    static uint32_t matrix_scan_count = 0;

    matrix_scan_count++;

    if (timer_elapsed32(matrix_timer) >= 1000) {
#ifndef NO_PRINT
        if (userspace_config.debug.matrix_scan_print) {
            xprintf("matrix scan frequency: %lu\n", matrix_scan_count);
        }
#endif // NO_PRINT
        last_matrix_scan_count = matrix_scan_count;
        matrix_timer           = timer_read32();
        matrix_scan_count      = 0;
    }
}

#ifdef AUDIO_ENABLE
float doom_song[][2] = SONG(E1M1_DOOM);
extern audio_config_t audio_config;

void set_doom_song(layer_state_t state) {
    static bool is_gaming_layer_active = false, is_doom_song_active = false;

    if (userspace_config.gaming.song_enable != is_doom_song_active ||
        is_gaming_layer_active != layer_state_cmp(state, _GAMEPAD)) {
        is_doom_song_active    = userspace_config.gaming.song_enable;
        is_gaming_layer_active = layer_state_cmp(state, _GAMEPAD);
        if (is_gaming_layer_active && is_doom_song_active) {
            PLAY_LOOP(doom_song);
            audio_config.clicky_enable = false;
        } else {
            audio_stop_all();
            audio_config.raw = eeconfig_read_audio();
        }
    }
}
#endif // AUDIO_ENABLE
