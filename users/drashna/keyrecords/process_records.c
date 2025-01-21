// Copyright 2020 Christopher Courtney, aka Drashna Jael're  (@drashna) <drashna@live.com>
// Copyright 2018-2024 Nick Brassel (@tzarc)
// SPDX-License-Identifier: GPL-3.0-or-later

#include "process_records.h"
#include "drashna.h"
#include "version.h"
#include "drashna_names.h"
#ifdef CUSTOM_DYNAMIC_MACROS_ENABLE
#    include "keyrecords/custom_dynamic_macros.h"
#endif // CUSTOM_DYNAMIC_MACROS_ENABLE
#ifdef DISPLAY_DRIVER_ENABLE
#    include "display/display.h"
#endif // DISPLAY_DRIVER_ENABLE
#if defined(CUSTOM_POINTING_DEVICE)
#    include "pointing/pointing.h"
#endif // CUSTOM_POINTING_DEVICE
#if defined(RGBLIGHT_ENABLE)
#    include "rgb/rgb_stuff.h"
#endif // defined(RGBLIGHT_ENABLE)
#if defined(RGB_MATRIX_ENABLE)
#    include "rgb/rgb_matrix_stuff.h"
#endif // defined(RGB_MATRIX_ENABLE)
#ifdef UNICODE_COMMON_ENABLE
#    include "keyrecords/unicode.h"
#endif // UNICODE_COMMON_ENABLE
#ifdef COMMUNITY_MODULE_I2C_SCANNER_ENABLE
#    include "modules/drashna/i2c_scanner/i2c_scanner.h"
#endif

#if defined(AUDIO_ENABLE) && defined(OS_DETECTION_ENABLE)
#    include "audio.h"
#    ifndef CG_NORM_SONG
#        define CG_NORM_SONG SONG(AG_NORM_SOUND)
#    endif
#    ifndef CG_SWAP_SONG
#        define CG_SWAP_SONG SONG(AG_SWAP_SOUND)
#    endif
static float cg_norm_song[][2] = CG_NORM_SONG;
static float cg_swap_song[][2] = CG_SWAP_SONG;
#endif

uint16_t copy_paste_timer;

__attribute__((weak)) void konami_code_handler(void) {
    dprintf("Konami code entered\n");
    wait_ms(50);
    reset_keyboard();
}

static bool process_record_konami_code(uint16_t keycode, keyrecord_t *record) {
    static uint8_t        konami_index          = 0;
    static const uint16_t konami_code[] PROGMEM = {KC_UP,   KC_UP,    KC_DOWN, KC_DOWN, KC_LEFT, KC_RIGHT,
                                                   KC_LEFT, KC_RIGHT, KC_B,    KC_A,    KC_ENTER};

    if (!record->event.pressed) {
        switch (keycode) {
            case QK_MOMENTARY ... QK_MOMENTARY_MAX:
            case QK_DEF_LAYER ... QK_DEF_LAYER_MAX:
            case QK_TOGGLE_LAYER ... QK_TOGGLE_LAYER_MAX:
            case QK_ONE_SHOT_LAYER ... QK_ONE_SHOT_LAYER_MAX:
            case QK_LAYER_TAP_TOGGLE ... QK_LAYER_TAP_TOGGLE_MAX:
            case KC_BACKSPACE:
            case KC_DELETE:
                // Messing with layers, ignore but don't reset the counter.
                break;
            case QK_MOD_TAP ... QK_MOD_TAP_MAX:
                return process_record_konami_code(QK_MOD_TAP_GET_TAP_KEYCODE(keycode), record);
            case QK_LAYER_TAP ... QK_LAYER_TAP_MAX:
                return process_record_konami_code(QK_LAYER_TAP_GET_TAP_KEYCODE(keycode), record);
            case QK_SWAP_HANDS ... QK_SWAP_HANDS_MAX:
                return process_record_konami_code(QK_SWAP_HANDS_GET_TAP_KEYCODE(keycode), record);
            case KC_KP_ENTER:
            case KC_RETURN:
            case QK_SPACE_CADET_RIGHT_SHIFT_ENTER:
                return process_record_konami_code(KC_ENTER, record);
            case KC_UP:
            case KC_DOWN:
            case KC_LEFT:
            case KC_RIGHT:
            case KC_B:
            case KC_A:
            case KC_ENTER:
                if (keycode == pgm_read_word(&konami_code[konami_index])) {
                    dprintf("Konami code: key released: %s\n", keycode_name(keycode, false));
                    konami_index++;
                    if (konami_index == ARRAY_SIZE(konami_code)) {
                        konami_index = 0;
                        konami_code_handler();
                    }
                } else {
                    if (konami_index) {
                        dprintf("Konami code: reset\n");
                    }
                    konami_index = 0;
                }
                break;
            default:
                if (konami_index) {
                    dprintf("Konami code: reset\n");
                }
                konami_index = 0;
                break;
        }
    }
    return true;
}

// Defines actions tor my global custom keycodes. Defined in drashna.h file
// Then runs the _keymap's record handier if not processed here

__attribute__((weak)) bool pre_process_record_keymap(uint16_t keycode, keyrecord_t *record) {
    return true;
}

bool pre_process_record_user(uint16_t keycode, keyrecord_t *record) {
    return pre_process_record_keymap(keycode, record);
}

/**
 * @brief Keycode handler for keymaps
 *
 * This handles the keycodes at the keymap level, useful for keyboard specific customization
 */
__attribute__((weak)) bool process_record_keymap(uint16_t keycode, keyrecord_t *record) {
    return true;
}
__attribute__((weak)) bool process_record_secrets(uint16_t keycode, keyrecord_t *record) {
    return true;
}

/**
 * @brief Main user keycode handler
 *
 * This handles all of the keycodes for the user, including calling feature handlers.
 *
 * @param keycode Keycode from matrix
 * @param record keyrecord_t data structure
 * @return true Continue processing keycode and send to host
 * @return false Stop process keycode and do not send to host
 */
bool process_record_user(uint16_t keycode, keyrecord_t *record) {
#if defined(ENCODER_ENABLE) && defined(SPLIT_KEYBOARD) // some debouncing for weird issues
    if (IS_ENCODEREVENT(record->event)) {
        static bool ignore_next = true;
        if (ignore_next) {
            if (timer_elapsed32(0) < 500) {
                return false;
            } else {
                ignore_next = false;
            }
        }
    }
#endif // ENCODER_ENABLE && SPLIT_KEYBOARD

    // If console is enabled, it will print the matrix position and status of each key pressed
#ifdef KEYLOGGER_ENABLE
    if (userspace_config.debug.console_keylogger) {
        xprintf("KL: %s, kc: 0x%04X, col: %2u, row: %2u, pressed: %1d, time: %5u, int: %1d, count: %u\n",
                get_keycode_string(keycode), keycode, record->event.key.col, record->event.key.row,
                record->event.pressed, record->event.time, record->tap.interrupted, record->tap.count);
    }
#endif // KEYLOGGER_ENABLE

#ifdef ACHORDION_ENABLE
    if (!process_achordion(keycode, record)) {
        return false;
    }
#endif // ACHORDION_ENABLE

    if (!process_record_konami_code(keycode, record)) {
        return false;
    }

    if (!(process_record_keymap(keycode, record) && process_record_secrets(keycode, record)
#ifdef DISPLAY_DRIVER_ENABLE
          && process_record_display_driver(keycode, record)
#endif // DISPLAY_DRIVER_ENABLE
#ifdef CUSTOM_RGB_MATRIX
          && process_record_user_rgb_matrix(keycode, record)
#endif // CUSTOM_RGB_MATRIX
#ifdef CUSTOM_RGBLIGHT
          && process_record_user_rgb_light(keycode, record)
#endif // CUSTOM_RGBLIGHT
#ifdef CUSTOM_UNICODE_ENABLE
          && process_record_unicode(keycode, record)
#endif // CUSTOM_UNICODE_ENABLE
#if defined(CUSTOM_POINTING_DEVICE)
          && process_record_pointing(keycode, record)
#endif // CUSTOM_POINTING_DEVICE
#ifdef CUSTOM_DYNAMIC_MACROS_ENABLE
          && process_record_dynamic_macro(keycode, record)
#endif // CUSTOM_DYNAMIC_MACROS_ENABLE
          && true)) {
        return false;
    }

    switch (keycode) {
        case VRSN: // Prints firmware version
            if (record->event.pressed) {
                send_string_with_delay_P(
                    PSTR(QMK_KEYBOARD "/" QMK_KEYMAP " @ " QMK_VERSION ", Built on: " QMK_BUILDDATE), TAP_CODE_DELAY);
            }
            break;

        case KC_DIABLO_CLEAR: // reset all Diablo timers, disabling them
#ifdef CUSTOM_TAP_DANCE_ENABLE
            if (record->event.pressed) {
                for (uint8_t index = 0; index < 4; index++) {
                    diablo_timer[index].key_interval = 0;
                }
            }
#endif // CUSTOM_TAP_DANCE_ENABLE
            break;

        case KC_CCCV: // One key copy/paste
            if (record->event.pressed) {
                copy_paste_timer = timer_read();
            } else {
                if (timer_elapsed(copy_paste_timer) > TAPPING_TERM) { // Hold, copy
                    tap_code16(LCTL(KC_C));
                } else { // Tap, paste
                    tap_code16(LCTL(KC_V));
                }
            }
            break;
        case KC_RGB_T: // This allows me to use underglow as layer indication, or as normal
#if defined(CUSTOM_RGBLIGHT) || defined(CUSTOM_RGB_MATRIX)
            if (record->event.pressed) {
                rgb_layer_indication_toggle();
            }
#endif // CUSTOM_RGBLIGHT || CUSTOM_RGB_MATRIX
            break;

#if defined(CUSTOM_RGBLIGHT) || defined(CUSTOM_RGB_MATRIX)
        case QK_RGB_MATRIX_TOGGLE:
        case QK_UNDERGLOW_TOGGLE:
            // Split keyboards need to trigger on key-up for edge-case issue
#    ifndef SPLIT_KEYBOARD
            if (record->event.pressed) {
#    else  // SPLIT_KEYBOARD
            if (!record->event.pressed) {
#    endif // SPLIT_KEYBOARD
#    if defined(CUSTOM_RGBLIGHT)
                rgblight_toggle();
#    endif // CUSTOM_RGBLIGHT
#    if defined(CUSTOM_RGB_MATRIX)
                rgb_matrix_toggle();
#    endif // CUSTOM_RGB_MATRIX
            }
            return false;
            break;
        case QK_UNDERGLOW_MODE_NEXT:
        case QK_UNDERGLOW_MODE_PREVIOUS:
            if (record->event.pressed) {
                bool is_eeprom_updated = false;
#    if defined(CUSTOM_RGBLIGHT)
                // This disables layer indication, as it's assumed that if you're changing this ... you want that
                // disabled
                if (userspace_config.rgb.layer_change) {
                    userspace_config.rgb.layer_change = false;
                    dprintf("rgblight layer change [EEPROM]: %u\n", userspace_config.rgb.layer_change);
                    is_eeprom_updated = true;
                }
#    endif // CUSTOM_RGBLIGHT
#    if defined(CUSTOM_RGB_MATRIX) && defined(RGB_MATRIX_FRAMEBUFFER_EFFECTS)
                if (userspace_config.rgb.idle_anim) {
                    userspace_config.rgb.idle_anim = false;
                    dprintf("RGB Matrix Idle Animation [EEPROM]: %u\n", userspace_config.rgb.idle_anim);
                    is_eeprom_updated = true;
                }
#    endif // CUSTOM_RGB_MATRIX && RGB_MATRIX_FRAMEBUFFER_EFFECTS
                if (is_eeprom_updated) {
                    eeconfig_update_user_datablock_handler(&userspace_config, 0, EECONFIG_USER_DATA_SIZE);
                }
            }
            break;
        case QK_RGB_MATRIX_MODE_NEXT:
        case QK_RGB_MATRIX_MODE_PREVIOUS:
            if (record->event.pressed) {
                bool is_eeprom_updated = false;
#    if defined(CUSTOM_RGB_MATRIX) && defined(RGB_MATRIX_FRAMEBUFFER_EFFECTS)
                if (userspace_config.rgb.idle_anim) {
                    userspace_config.rgb.idle_anim = false;
                    dprintf("RGB Matrix Idle Animation [EEPROM]: %u\n", userspace_config.rgb.idle_anim);
                    is_eeprom_updated = true;
                }
#    endif // CUSTOM_RGB_MATRIX && RGB_MATRIX_FRAMEBUFFER_EFFECTS
                if (is_eeprom_updated) {
                    eeconfig_update_user_datablock_handler(&userspace_config, 0, EECONFIG_USER_DATA_SIZE);
                }
            }
            break;

#endif // CUSTOM_RGBLIGHT || CUSTOM_RGB_MATRIX
#if defined(OS_DETECTION_ENABLE) && defined(OS_DETECTION_DEBUG_ENABLE)
        case STORE_SETUPS:
            if (record->event.pressed) {
                store_setups_in_eeprom();
            }
            return false;
        case PRINT_SETUPS:
            if (record->event.pressed) {
                print_stored_setups();
            }
            return false;
#endif // OS_DETECTION_ENABLE && OS_DETECTION_DEBUG_ENABLE
        case US_MATRIX_SCAN_RATE_PRINT:
            if (record->event.pressed) {
                userspace_config.debug.matrix_scan_print ^= 1;
                eeconfig_update_user_datablock_handler(&userspace_config, 0, EECONFIG_USER_DATA_SIZE);
            }
            break;
        case US_I2C_SCAN_ENABLE:
#ifdef COMMUNITY_MODULE_I2C_SCANNER_ENABLE
            if (record->event.pressed) {
                userspace_config.debug.i2c_scanner_enable = !userspace_config.debug.i2c_scanner_enable;
                i2c_scanner_set_enabled(userspace_config.debug.i2c_scanner_enable);
                eeconfig_update_user_datablock_handler(&userspace_config, 0, EECONFIG_USER_DATA_SIZE);
            }
#endif
            break;
        case US_GAMING_SCAN_TOGGLE:
#ifdef AUDIO_ENABLE
            if (record->event.pressed) {
                userspace_config.gaming.song_enable = !userspace_config.gaming.song_enable;
                eeconfig_update_user_datablock_handler(&userspace_config, 0, EECONFIG_USER_DATA_SIZE);
                set_doom_song(layer_state);
            }
#endif // AUDIO_ENABLE
            break;
#if defined(OS_DETECTION_ENABLE)
        case QK_MAGIC_SWAP_LCTL_LGUI:
            if (record->event.pressed) {
                keymap_config.swap_lctl_lgui = true;
                clear_keyboard();
#    if defined(UNICODE_COMMON_ENABLE)
                set_unicode_input_mode_soft(keymap_config.swap_lctl_lgui ? UNICODE_MODE_MACOS
                                                                         : UNICODE_MODE_WINCOMPOSE);
#    endif
            }
            return false;
        case QK_MAGIC_SWAP_RCTL_RGUI:
            if (record->event.pressed) {
                keymap_config.swap_rctl_rgui = true;
                clear_keyboard();
            }
            return false;
        case QK_MAGIC_SWAP_CTL_GUI:
            if (record->event.pressed) {
                keymap_config.swap_lctl_lgui = keymap_config.swap_rctl_rgui = true;
#    ifdef AUDIO_ENABLE
                PLAY_SONG(cg_swap_song);
#    endif
                clear_keyboard();
#    if defined(UNICODE_COMMON_ENABLE)
                set_unicode_input_mode_soft(keymap_config.swap_lctl_lgui ? UNICODE_MODE_MACOS
                                                                         : UNICODE_MODE_WINCOMPOSE);
#    endif
            }
            return false;
        case QK_MAGIC_UNSWAP_LCTL_LGUI:
            if (record->event.pressed) {
                keymap_config.swap_lctl_lgui = false;
                clear_keyboard();
#    if defined(UNICODE_COMMON_ENABLE)
                set_unicode_input_mode_soft(keymap_config.swap_lctl_lgui ? UNICODE_MODE_MACOS
                                                                         : UNICODE_MODE_WINCOMPOSE);
#    endif
            }
            return false;
        case QK_MAGIC_UNSWAP_RCTL_RGUI:
            if (record->event.pressed) {
                keymap_config.swap_rctl_rgui = false;
                clear_keyboard();
            }
            return false;
        case QK_MAGIC_UNSWAP_CTL_GUI:
            if (record->event.pressed) {
                keymap_config.swap_lctl_lgui = keymap_config.swap_rctl_rgui = false;
#    ifdef AUDIO_ENABLE
                PLAY_SONG(cg_norm_song);
#    endif
                clear_keyboard();
#    if defined(UNICODE_COMMON_ENABLE)
                set_unicode_input_mode_soft(keymap_config.swap_lctl_lgui ? UNICODE_MODE_MACOS
                                                                         : UNICODE_MODE_WINCOMPOSE);
#    endif
            }
            return false;
        case QK_MAGIC_TOGGLE_CTL_GUI:
            if (record->event.pressed) {
                keymap_config.swap_lctl_lgui = !keymap_config.swap_lctl_lgui;
                keymap_config.swap_rctl_rgui = keymap_config.swap_lctl_lgui;
#    ifdef AUDIO_ENABLE
                if (keymap_config.swap_rctl_rgui) {
                    PLAY_SONG(cg_swap_song);
                } else {
                    PLAY_SONG(cg_norm_song);
                }
#    endif
                clear_keyboard();
#    if defined(UNICODE_COMMON_ENABLE)
                set_unicode_input_mode_soft(keymap_config.swap_lctl_lgui ? UNICODE_MODE_MACOS
                                                                         : UNICODE_MODE_WINCOMPOSE);
#    endif
            }
            return false;
#endif
    }
    return true;
}

__attribute__((weak)) void post_process_record_keymap(uint16_t keycode, keyrecord_t *record) {}
void                       post_process_record_user(uint16_t keycode, keyrecord_t *record) {
    post_process_record_keymap(keycode, record);
}

void rgb_layer_indication_toggle(void) {
    userspace_config.rgb.layer_change ^= 1;
    dprintf("rgblight layer change [EEPROM]: %u\n", userspace_config.rgb.layer_change);
    eeconfig_update_user_datablock_handler(&userspace_config, 0, EECONFIG_USER_DATA_SIZE);
    if (userspace_config.rgb.layer_change) {
#if defined(CUSTOM_RGB_MATRIX)
        rgb_matrix_set_flags(LED_FLAG_UNDERGLOW | LED_FLAG_KEYLIGHT | LED_FLAG_INDICATOR);
#    if defined(CUSTOM_RGBLIGHT)
        rgblight_enable_noeeprom();
#    endif                            // CUSTOM_RGBLIGHT
#endif                                // CUSTOM_RGB_MATRIX
        layer_state_set(layer_state); // This is needed to immediately set the layer color (looks better)
#if defined(CUSTOM_RGB_MATRIX)
    } else {
        rgb_matrix_set_flags(LED_FLAG_ALL);
#    if defined(CUSTOM_RGBLIGHT)
        rgblight_disable_noeeprom();
#    endif // CUSTOM_RGBLIGHT
#endif     // CUSTOM_RGB_MATRIX
    }
}
