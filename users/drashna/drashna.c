// Copyright 2020 Christopher Courtney, aka Drashna Jael're  (@drashna) <drashna@live.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "drashna.h"
#include <string.h>
#include <stdio.h>
#include <ctype.h>

userspace_config_t userspace_config;
static bool        device_suspended = false;

/**
 * @brief Handle registering a keycode, with optional modifer based on timed event
 *
 * @param code keycode to send to host
 * @param mod_code modifier to send with code, if held for tapping term or longer
 * @param pressed the press/release event (can use "record->event.pressed" for this)
 * @return true exits function
 * @return false exits function
 */
bool mod_key_press_timer(uint16_t code, uint16_t mod_code, bool pressed) {
    static uint16_t this_timer;
    mod_key_press(code, mod_code, pressed, this_timer);
    return false;
}

/**
 * @brief Handle registation of keycode, with optional modifier based on custom timer
 *
 * @param code keycode to send to host
 * @param mod_code modifier keycode to send with code, if held for tapping term or longer
 * @param pressed the press/release event
 * @param this_timer custom timer to use
 * @return true
 * @return false
 */
bool mod_key_press(uint16_t code, uint16_t mod_code, bool pressed, uint16_t this_timer) {
    if (pressed) {
        this_timer = timer_read();
    } else {
        if (timer_elapsed(this_timer) < TAPPING_TERM) {
            tap_code(code);
        } else {
            register_code(mod_code);
            tap_code(code);
            unregister_code(mod_code);
        }
    }
    return false;
}

/**
 * @brief Performs exact match for modifier values
 *
 * @param value the modifer varible (get_mods/get_oneshot_mods/get_weak_mods)
 * @param mask the modifier mask to check for
 * @return true Has the exact modifiers specifed
 * @return false Does not have the exact modifiers specified
 */
bool hasAllBitsInMask(uint8_t value, uint8_t mask) {
    value &= 0xF;
    mask &= 0xF;

    return (value & mask) == mask;
}

/**
 * @brief Tap keycode, with no mods
 *
 * @param kc keycode to use
 */
void tap_code16_nomods(uint16_t kc) {
    uint8_t temp_mod = get_mods();
    clear_mods();
    clear_oneshot_mods();
    tap_code16(kc);
    set_mods(temp_mod);
}

#ifdef I2C_SCANNER_ENABLE
#    include "i2c_master.h"
#    include "debug.h"

#    ifndef I2C_SCANNER_TIMEOUT
#        define I2C_SCANNER_TIMEOUT 50
#    endif

void do_scan(void) {
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
            // dprintf("  Unknown error (%u) at address 0x%02X\n", error, address);
        }
    }

    if (nDevices == 0)
        xprintf("No I2C devices found\n");
    else
        xprintf("done\n");
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
#endif

#if defined(AUTOCORRECT_ENABLE)
#    if defined(AUDIO_ENABLE)
#        ifdef USER_SONG_LIST
float autocorrect_song[][2] = SONG(MARIO_GAMEOVER);
#        else
float autocorrect_song[][2] = SONG(PLOVER_GOODBYE_SOUND);
#        endif
#    endif
// 2 strings, 2q chars each + null terminator. max autocorrect length is 19 chars but 128px/6 supports 21 chars
char autocorrected_str[2][21] = {"    automatically\0", "      corrected\0"};
char autocorrected_str_raw[2][21] = {"automatically\0", "corrected\0"};
bool autocorrect_str_has_changed = false;

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
#    endif

#    if defined(AUDIO_ENABLE)
    audio_play_melody(&autocorrect_song, NOTE_ARRAY_SIZE(autocorrect_song), false);
#    endif
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
#    endif
#endif

/**
 * @brief Generates a string of the layer state bitmask
 *
 * @param buffer char string buffer to write to
 * @param state layer state bitmask
 * @param default_state default layer state bitmask (so we can represent default layer differently)
 */
void format_layer_bitmap_string(char *buffer, layer_state_t state, layer_state_t default_state) {
    for (int i = 0; i < 16; i++) {
        if (i == 0 || i == 4 || i == 8 || i == 12) {
            *buffer = ' ';
            ++buffer;
        }

        uint8_t layer = i;
        if ((default_state & ((layer_state_t)1 << layer)) != 0) {
            *buffer = 'D';
        } else if ((state & ((layer_state_t)1 << layer)) != 0) {
            *buffer = '1';
        } else {
            *buffer = '_';
        }
        ++buffer;
    }
    *buffer = 0;
}

#if defined(OS_DETECTION_ENABLE)

bool process_detected_host_os_user(os_variant_t detected_os) {
    if (is_keyboard_master()) {
        bool is_mac = (detected_os == OS_MACOS) || (detected_os == OS_IOS);
        if (keymap_config.swap_lctl_lgui != is_mac) {
            keymap_config.swap_lctl_lgui = keymap_config.swap_rctl_rgui = is_mac;
        }
#    ifdef UNICODE_COMMON_ENABLE
        set_unicode_input_mode_soft(keymap_config.swap_lctl_lgui ? UNICODE_MODE_MACOS : UNICODE_MODE_WINCOMPOSE);
#    endif
        switch (detected_os) {
            case OS_UNSURE:
                xprintf("unknown OS Detected\n");
                break;
            case OS_LINUX:
#    ifdef UNICODE_COMMON_ENABLE
                set_unicode_input_mode_soft(UNICODE_MODE_LINUX);
#    endif // UNICODE_COMMON_ENABLE
                xprintf("Linux Detected\n");
                break;
            case OS_WINDOWS:
#    ifdef UNICODE_COMMON_ENABLE
                set_unicode_input_mode_soft(UNICODE_MODE_WINCOMPOSE);
#    endif // UNICODE_COMMON_ENABLE
                xprintf("Windows Detected\n");
                break;
            case OS_WINDOWS_UNSURE:
#        ifdef UNICODE_COMMON_ENABLE
                set_unicode_input_mode_soft(UNICODE_MODE_WINCOMPOSE);
#        endif // UNICODE_COMMON_ENABLE
                xprintf("Windows? Detected\n");
                break;
            case OS_MACOS:
#    ifdef UNICODE_COMMON_ENABLE
                set_unicode_input_mode_soft(UNICODE_MODE_MACOS);
#    endif // UNICODE_COMMON_ENABLE
                xprintf("MacOS Detected\n");
                break;
            case OS_IOS:
#    ifdef UNICODE_COMMON_ENABLE
                set_unicode_input_mode_soft(UNICODE_MODE_MACOS);
#    endif // UNICODE_COMMON_ENABLE
                xprintf("iOS Detected\n");
                break;
            case OS_PS5:
#        ifdef UNICODE_COMMON_ENABLE
                set_unicode_input_mode_soft(UNICODE_MODE_LINUX);
#        endif // UNICODE_COMMON_ENABLE
                xprintf("PlayStation 5 Detected\n");
                break;
            case OS_HANDHELD:
#        ifdef UNICODE_COMMON_ENABLE
                set_unicode_input_mode_soft(UNICODE_MODE_LINUX);
#        endif // UNICODE_COMMON_ENABLE
                xprintf("Nintend Switch/Quest 2 Detected\n");
                break;
            default:
                xprintf("Unknown OS Detected\n");
                break;
        }
    }

    return true;
}
#endif // OS_DETECTION_ENABLE

static host_driver_t *host_driver          = 0;
static bool           host_driver_disabled = false;

/**
 * @brief Mount/Umount the keyboard USB driver
 */
void set_keyboard_lock(bool status) {
    if (!status && !host_get_driver()) {
        host_set_driver(host_driver);
    } else if (status && host_get_driver()) {
        host_driver = host_get_driver();
        clear_keyboard();
        host_set_driver(0);
    } else if (status) {
        clear_keyboard();
    }

    host_driver_disabled = status;
}

/**
 * @brief Toggles the keyboard lock status
 *
 */
void toggle_keyboard_lock(void) {
    set_keyboard_lock(!host_driver_disabled);
}

/**
 * @brief Get the keyboard lock status
 *
 * @return true
 * @return false
 */
bool get_keyboard_lock(void) {
    return host_driver_disabled;
}

/**
 * @brief Get the layer name string object
 *
 * @param state layer state bitmask
 * @param alt_name Use altname?
 * @param is_default do we want the default layer's name?
 * @return const char* Layer name in string format
 */
const char *get_layer_name_string(layer_state_t state, bool alt_name, bool is_default) {
    switch (get_highest_layer(state)) {
        case _QWERTY:
            return alt_name ? "Num Pad" : is_default ? "QWERTY" : "Base";
        case _COLEMAK:
            return "Colemak";
        case _COLEMAK_DH:
            return "Colemak-DH";
        case _DVORAK:
            return "Dvorak";
        case _GAMEPAD:
            return "Gamepad";
        case _DIABLO:
            return "Diablo";
        case _DIABLOII:
            return "Diablo II";
        case _MOUSE:
            return alt_name ? "Macros" : "Mouse";
        case _MEDIA:
            return "Media";
        case _LOWER:
            return "Lower";
        case _RAISE:
            return "Raise";
        case _ADJUST:
            return "Adjust";
        default:
            return "Unknown";
    }
}

/**
 * @brief hack for snprintf warning
 *
 */
#define snprintf_nowarn(...)                                       \
    __extension__({                                                \
        _Pragma("GCC diagnostic push");                            \
        _Pragma("GCC diagnostic ignored \"-Wformat-truncation\""); \
        const int _snprintf_nowarn = snprintf(__VA_ARGS__);        \
        _Pragma("GCC diagnostic pop");                             \
        _snprintf_nowarn;                                          \
    })

/**
 * @brief Center text in a string. Useful for monospaced font rendering such as oled display feature.
 *
 * @param text pointer to input string to center
 * @param output pointer to output string to write to
 * @param width total width of the output string
 */
void center_text(const char *text, char *output, uint8_t width) {
    /* If string is bigger than the available width, trim it */
    if (strlen(text) > width) {
        memcpy(output, text, width);
        return;
    }

    /* Try to center the TEXT, TODO: Handle Even lengths*/
    uint8_t padlen_l = (width - strlen(text)) / 2;
    uint8_t padlen_r = (padlen_l * 2) + strlen(text) == width ? padlen_l : padlen_l + 1;
    snprintf_nowarn(output, width, "%*s%s%*s", padlen_l, " ", text, padlen_r, " ");
}

/**
 * @brief Checks to see if device is suspended
 *
 * @return true
 * @return false
 */
bool is_device_suspended(void) {
    return device_suspended;
}

/**
 * @brief Sets the local variable for device suspended
 *
 * @param status
 */
void set_is_device_suspended(bool status) {
#if defined(SPLIT_KEYBOARD) && defined(SPLIT_TRANSACTION_IDS_USER)
    send_device_suspend_state(status);
#endif
    device_suspended = status;
}

/**
 * @brief Checks to see if one or more gaming layers are active
 *
 * @param state layer state bitmask
 * @return true A gaming layer is active
 * @return false No gaming layers active
 */

bool is_gaming_layer_active(layer_state_t state) {
    return (state & (1 << _GAMEPAD)) || (state & (1 << _DIABLO)) || (state & (1 << _DIABLOII));
}

/**
 * @brief Grabs the basic keycode from a quantum keycode
 *
 * @param keycode extended keycode to extract from
 * @param record keyrecord used for checking tap count/hold
 * @param check_hold do we check for hold keycode status
 * @return uint16_t returns the basic keycode
 */
uint16_t extract_basic_keycode(uint16_t keycode, keyrecord_t *record, bool check_hold) {
    if (IS_QK_MOD_TAP(keycode)) {
        if (record->tap.count || !check_hold) {
            keycode = keycode_config(QK_MOD_TAP_GET_TAP_KEYCODE(keycode));
        } else {
            keycode = keycode_config(0xE0 + biton(QK_MOD_TAP_GET_MODS(keycode) & 0xF) +
                                     biton(QK_MOD_TAP_GET_MODS(keycode) & 0x10));
        }
    } else if (IS_QK_LAYER_TAP(keycode) && (record->tap.count || !check_hold)) {
        keycode = keycode_config(QK_LAYER_TAP_GET_TAP_KEYCODE(keycode));
    } else if (IS_QK_MODS(keycode)) {
        keycode = keycode_config(QK_MODS_GET_BASIC_KEYCODE(keycode));
    } else if (IS_QK_ONE_SHOT_MOD(keycode)) {
        keycode = keycode_config(0xE0 + biton(QK_ONE_SHOT_MOD_GET_MODS(keycode) & 0xF) +
                                 biton(QK_ONE_SHOT_MOD_GET_MODS(keycode) & 0x10));
    } else if (IS_QK_BASIC(keycode)) {
        keycode = keycode_config(keycode);
    }

    return keycode;
}

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
        if (userspace_config.matrix_scan_print) {
            xprintf("matrix scan frequency: %lu\n", matrix_scan_count);
        }
#endif
        last_matrix_scan_count = matrix_scan_count;
        matrix_timer           = timer_read32();
        matrix_scan_count      = 0;
    }
}

#if 0
#    include "hardware_id.h"
void get_serial_number(void) {
    uint8_t uid[12];
    for (uint8_t index = 0; index < 12; index++) {
        uid[index] = (uint8_t)(get_hardware_id().data[index / 4] >> ((index % 4) * 8));
    }

    uint16_t serial[6];
    serial[0] = uid[11];
    serial[1] = uid[10] + uid[2];
    serial[2] = uid[9];
    serial[3] = uid[8] + uid[0];
    serial[4] = uid[7];
    serial[5] = uid[6];

    xprintf("%02X%02X%02X%02X%02X%02X", serial[0], serial[1], serial[2], serial[3], serial[4], serial[5]);
}
#endif
