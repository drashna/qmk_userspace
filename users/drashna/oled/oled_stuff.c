/* Copyright 2020 Christopher Courtney, aka Drashna Jael're  (@drashna) <drashna@live.com>
 * Copyright 2021 John Ezra - wpm graph
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "drashna.h"
#include <ctype.h>
#include <string.h>
#include <stdio.h>
#include <math.h>
#include "keyboard.h"
#include "lib/lib8tion/lib8tion.h"
#include <math.h>
#include <stdio.h>

#ifdef RTC_ENABLE
#    include "rtc.h"
#    ifdef DS3231_RTC_DRIVER_ENABLE
#        include "ds3231.h"
#    endif
#endif
#ifdef LAYER_MAP_ENABLE
#    include "layer_map.h"
#endif

#ifndef OLED_BRIGHTNESS_STEP
#    define OLED_BRIGHTNESS_STEP 32
#endif

bool is_oled_enabled = true, is_oled_force_off = false, oled_screensaver_enabled = false;

uint32_t               oled_timer                                 = 0;
char                   oled_keylog_str[OLED_KEYLOGGER_LENGTH + 1] = {0};
extern oled_rotation_t oled_rotation;
extern uint8_t         oled_rotation_width;

deferred_token kittoken;

extern uint8_t         oled_buffer[OLED_MATRIX_SIZE];
extern OLED_BLOCK_TYPE oled_dirty;

void oled_pan_section(bool left, uint16_t y_start, uint16_t y_end, uint16_t x_start, uint16_t x_end) {
    uint16_t i = 0;
    for (uint16_t y = y_start; y < y_end; y++) {
        if (left) {
            for (uint16_t x = x_start; x < x_end - 1; x++) {
                i              = y * oled_rotation_width + x;
                oled_buffer[i] = oled_buffer[i + 1];
                oled_dirty |= ((OLED_BLOCK_TYPE)1 << (i / OLED_BLOCK_SIZE));
            }
        } else {
            for (uint16_t x = x_end - 1; x >= x_start; x--) {
                i              = y * oled_rotation_width + x;
                oled_buffer[i] = oled_buffer[i - 1];
                oled_dirty |= ((OLED_BLOCK_TYPE)1 << (i / OLED_BLOCK_SIZE));
            }
        }
    }
}

/**
 * @brief parses pressed keycodes and saves to buffer
 *
 * @param keycode Keycode pressed from switch matrix
 * @param record keyrecord_t data structure
 */
void add_keylog(uint16_t keycode, keyrecord_t *record) {
    if (IS_QK_MOD_TAP(keycode)) {
        if (record->tap.count) {
            keycode = keycode_config(QK_MOD_TAP_GET_TAP_KEYCODE(keycode));
        } else {
            keycode = keycode_config(0xE0 + biton(QK_MOD_TAP_GET_MODS(keycode) & 0xF) +
                                     biton(QK_MOD_TAP_GET_MODS(keycode) & 0x10));
        }
    } else if (IS_QK_LAYER_TAP(keycode) && record->tap.count) {
        keycode = keycode_config(QK_LAYER_TAP_GET_TAP_KEYCODE(keycode));
    } else if (IS_QK_MODS(keycode)) {
        keycode = keycode_config(QK_MODS_GET_BASIC_KEYCODE(keycode));
    } else if (IS_QK_ONE_SHOT_MOD(keycode)) {
        keycode = keycode_config(0xE0 + biton(QK_ONE_SHOT_MOD_GET_MODS(keycode) & 0xF) +
                                 biton(QK_ONE_SHOT_MOD_GET_MODS(keycode) & 0x10));
    } else if (IS_QK_BASIC(keycode)) {
        keycode = keycode_config(keycode);
    }

    if ((keycode == KC_BSPC) && mod_config(get_mods() | get_oneshot_mods()) & MOD_MASK_CTRL) {
        memset(oled_keylog_str, ' ', OLED_KEYLOGGER_LENGTH);
        oled_keylog_str[OLED_KEYLOGGER_LENGTH] = '\0';
        return;
    }

    char code = 0;
    if (keycode < ARRAY_SIZE(code_to_name)) {
        code = pgm_read_byte(&code_to_name[keycode]);
    } else {
        if (keycode == UC_IRNY) {
            code = 0xFD;
        } else if (keycode == UC_CLUE) {
            code = 0xFE;
        }
    }

    if (code != 0) {
        memmove(oled_keylog_str, oled_keylog_str + 1, OLED_KEYLOGGER_LENGTH - 1);
        oled_keylog_str[(OLED_KEYLOGGER_LENGTH - 1)] = code;
    }
}

/**
 * @brief Keycode handler for oled display.
 *
 * This adds pressed keys to buffer, but also resets the oled timer
 *
 * @param keycode Keycode from matrix
 * @param record keyrecord data struture
 * @return true
 * @return false
 */
bool process_record_user_oled(uint16_t keycode, keyrecord_t *record) {
    if (record->event.pressed) {
        add_keylog(keycode, record);
        switch (keycode) {
            case OLED_BRIGHTNESS_INC:
                userspace_config.oled_brightness = qadd8(userspace_config.oled_brightness, OLED_BRIGHTNESS_STEP);
                oled_set_brightness(userspace_config.oled_brightness);
                eeconfig_update_user_config(&userspace_config.raw);
                break;
            case OLED_BRIGHTNESS_DEC:
                userspace_config.oled_brightness = qsub8(userspace_config.oled_brightness, OLED_BRIGHTNESS_STEP);
                oled_set_brightness(userspace_config.oled_brightness);
                eeconfig_update_user_config(&userspace_config.raw);
                break;
            case OLED_LOCK:
                userspace_config.oled_lock = !userspace_config.oled_lock;
                eeconfig_update_user_config(&userspace_config.raw);
                if (userspace_config.oled_lock) {
                    oled_on();
                }
                break;
            case OLED_ROTATE_CW:
                {
                    oled_rotation_t rot_temp;
                    switch (oled_rotation) {
                        case OLED_ROTATION_0:
                            rot_temp = OLED_ROTATION_90;
                            break;
                        case OLED_ROTATION_90:
                            rot_temp = OLED_ROTATION_180;
                            break;
                        case OLED_ROTATION_180:
                            rot_temp = OLED_ROTATION_270;
                            break;
                        default:
                            rot_temp = OLED_ROTATION_0;
                            break;
                    }
                    oled_init(rot_temp);
                    break;
                }
            case OLED_ROTATE_CCW:
                {
                    oled_rotation_t rot_temp;
                    switch (oled_rotation) {
                        case OLED_ROTATION_0:
                            rot_temp = OLED_ROTATION_270;
                            break;
                        case OLED_ROTATION_90:
                            rot_temp = OLED_ROTATION_0;
                            break;
                        case OLED_ROTATION_180:
                            rot_temp = OLED_ROTATION_90;
                            break;
                        default:
                            rot_temp = OLED_ROTATION_180;
                            break;
                    }
                    oled_init(rot_temp);
                    break;
                }
        }
    }
    return true;
}

/**
 * @brief Renders keylogger buffer to oled
 *
 */
void render_keylogger_status(uint8_t col, uint8_t line) {
#ifdef OLED_DISPLAY_VERBOSE
    oled_set_cursor(col, line);
#endif
    oled_write_P(PSTR(OLED_RENDER_KEYLOGGER), false);
    oled_write(oled_keylog_str, false);
}

/**
 * @brief Renders default layer state (aka layout) to oled
 *
 */
void render_default_layer_state(uint8_t col, uint8_t line) {
#ifdef OLED_DISPLAY_VERBOSE
    oled_set_cursor(col, line);
    oled_write_P(PSTR(OLED_RENDER_LAYOUT_NAME), false);

    static char          layer_state_buffer[11] = {0};
    static layer_state_t old_state              = 0;

    if (old_state != default_layer_state) {
        snprintf(layer_state_buffer, sizeof(layer_state_buffer), "%-10s",
                 get_layer_name_string(default_layer_state, false));
        old_state = default_layer_state;
    }
    oled_write(layer_state_buffer, false);
    oled_advance_page(true);
#else
    oled_write_P(PSTR(OLED_RENDER_LAYOUT_NAME), false);
    switch (get_highest_layer(default_layer_state)) {
        case _QWERTY:
            oled_write_P(PSTR(OLED_RENDER_LAYOUT_QWERTY), false);
            break;
        case _COLEMAK_DH:
            oled_write_P(PSTR(OLED_RENDER_LAYOUT_COLEMAK_DH), false);
            break;
        case _COLEMAK:
            oled_write_P(PSTR(OLED_RENDER_LAYOUT_COLEMAK), false);
            break;
        case _DVORAK:
            oled_write_P(PSTR(OLED_RENDER_LAYOUT_DVORAK), false);
            break;
    }
#endif
}

/**
 * @brief Renders the active layers to the OLED
 *
 */
void render_layer_state(uint8_t col, uint8_t line) {
#ifdef OLED_DISPLAY_VERBOSE
    uint8_t layer_is[4] = {0, 4, 4, 4};
    if (layer_state_is(_ADJUST)) {
        layer_is[0] = 3;
    } else if (layer_state_is(_RAISE)) {
        layer_is[0] = 1;
    } else if (layer_state_is(_LOWER)) {
        layer_is[0] = 2;
    }

    if (layer_state_is(_MOUSE)) {
        layer_is[1] = 6;
    }
    if (layer_state_is(_GAMEPAD)) {
        layer_is[2] = 5;
    }

    oled_set_cursor(col, line);
    oled_write_raw_P(tri_layer_image[layer_is[0]][0], sizeof(tri_layer_image[0][0]));
    oled_set_cursor(col + 4, line);
    oled_write_raw_P(tri_layer_image[layer_is[1]][0], sizeof(tri_layer_image[0][0]));
    oled_set_cursor(col + 8, line);
    oled_write_raw_P(tri_layer_image[layer_is[2]][0], sizeof(tri_layer_image[0][0]));
    oled_set_cursor(col + 13, line);
    oled_write_P(PSTR("Diablo2"), layer_state_is(_DIABLOII));
    oled_advance_page(true);

    oled_set_cursor(col, line + 1);
    oled_write_raw_P(tri_layer_image[layer_is[0]][1], sizeof(tri_layer_image[0][0]));
    oled_set_cursor(col + 4, line + 1);
    oled_write_raw_P(tri_layer_image[layer_is[1]][1], sizeof(tri_layer_image[0][0]));
    oled_set_cursor(col + 8, line + 1);
    oled_write_raw_P(tri_layer_image[layer_is[2]][1], sizeof(tri_layer_image[0][0]));
    oled_set_cursor(col + 13, line + 1);
    oled_write_P(PSTR("Diablo3"), layer_state_is(_DIABLO));
    oled_advance_page(true);

    oled_set_cursor(col, line + 2);
    oled_write_raw_P(tri_layer_image[layer_is[0]][2], sizeof(tri_layer_image[0][0]));
    oled_set_cursor(col + 4, line + 2);
    oled_write_raw_P(tri_layer_image[layer_is[1]][2], sizeof(tri_layer_image[0][0]));
    oled_set_cursor(col + 8, line + 2);
    oled_write_raw_P(tri_layer_image[layer_is[2]][2], sizeof(tri_layer_image[0][0]));
    oled_set_cursor(col + 13, line + 2);
    oled_write_P(PSTR("Media"), layer_state_is(_MEDIA));
#else
    oled_write_P(PSTR(OLED_RENDER_LAYER_NAME), false);
    oled_write_P(PSTR(OLED_RENDER_LAYER_LOWER), layer_state_is(_LOWER));
    oled_write_P(PSTR(OLED_RENDER_LAYER_RAISE), layer_state_is(_RAISE));
#endif
    oled_advance_page(true);
}

/**
 * @brief Renders the current lock status to oled
 *
 * @param led_usb_state Current keyboard led state
 */
void render_keylock_status(led_t led_usb_state, uint8_t col, uint8_t line) {
#if defined(OLED_DISPLAY_VERBOSE)
    oled_set_cursor(col, line);
#endif
#ifdef CAPS_WORD_ENABLE
    led_usb_state.caps_lock |= is_caps_word_on();
#endif
    oled_write_P(PSTR(OLED_RENDER_LOCK_NAME), false);
#if !defined(OLED_DISPLAY_VERBOSE)
    oled_write_P(PSTR(" "), false);
#endif
    oled_write_P(PSTR(OLED_RENDER_LOCK_NUML), led_usb_state.num_lock);
    oled_write_P(PSTR(" "), false);
    oled_write_P(PSTR(OLED_RENDER_LOCK_CAPS), led_usb_state.caps_lock);
#if defined(OLED_DISPLAY_VERBOSE)
    oled_write_P(PSTR(" "), false);
    oled_write_ln_P(PSTR(OLED_RENDER_LOCK_SCLK), led_usb_state.scroll_lock);
#endif
}

/**
 * @brief Renders the matrix scan rate to the host system
 *
 */
void render_matrix_scan_rate(uint8_t padding, uint8_t col, uint8_t line) {
    oled_set_cursor(col, line);
    oled_write_P(PSTR("MS:"), false);
    if (padding) {
        for (uint8_t n = padding; n > 0; n--) {
            oled_write_P(PSTR(" "), false);
        }
    }
    oled_write(get_u16_str(get_matrix_scan_rate(), ' '), false);
}

/**
 * @brief Renders the modifier state
 *
 * @param modifiers Modifiers to check against (real, weak, onesheot, etc;)
 */
void render_mod_status(uint8_t modifiers, uint8_t col, uint8_t line) {
    static const char PROGMEM mod_status[5][3] = {
        {0xE8, 0xE9, 0}, {0xE4, 0xE5, 0}, {0xE6, 0xE7, 0}, {0xEA, 0xEB, 0}, {0xEC, 0xED, 0}};
#if defined(OLED_DISPLAY_VERBOSE)
    oled_set_cursor(col, line);
#endif
    bool is_caps = host_keyboard_led_state().caps_lock;
#ifdef CAPS_WORD_ENABLE
    is_caps |= is_caps_word_on();
#endif
    oled_write_P(PSTR(OLED_RENDER_MODS_NAME), false);
#if defined(OLED_DISPLAY_VERBOSE)
    oled_write_P(mod_status[0], (modifiers & MOD_BIT(KC_LSFT)) || is_caps);
    oled_write_P(mod_status[!keymap_config.swap_lctl_lgui ? 3 : 4], (modifiers & MOD_BIT(KC_LGUI)));
    oled_write_P(mod_status[2], (modifiers & MOD_BIT(KC_LALT)));
    oled_write_P(mod_status[1], (modifiers & MOD_BIT(KC_LCTL)));
    oled_write_P(mod_status[1], (modifiers & MOD_BIT(KC_RCTL)));
    oled_write_P(mod_status[2], (modifiers & MOD_BIT(KC_RALT)));
    oled_write_P(mod_status[!keymap_config.swap_lctl_lgui ? 3 : 4], (modifiers & MOD_BIT(KC_RGUI)));
    oled_write_P(mod_status[0], (modifiers & MOD_BIT(KC_RSFT)) || is_caps);
#else
    oled_write_P(mod_status[0], (modifiers & MOD_MASK_SHIFT) || is_caps);
    oled_write_P(mod_status[!keymap_config.swap_lctl_lgui ? 3 : 4], (modifiers & MOD_MASK_GUI));
    oled_write_P(PSTR(" "), false);
    oled_write_P(mod_status[2], (modifiers & MOD_MASK_ALT));
    oled_write_P(mod_status[1], (modifiers & MOD_MASK_CTRL));
#endif
}

#ifdef SWAP_HANDS_ENABLE
extern bool swap_hands;
#endif

void render_bootmagic_status(uint8_t col, uint8_t line) {
    /* Show Ctrl-Gui Swap options */
    static const char PROGMEM logo[][2][3] = {
        {{0x97, 0x98, 0}, {0xb7, 0xb8, 0}},
        {{0x95, 0x96, 0}, {0xb5, 0xb6, 0}},
    };

    bool is_bootmagic_on;
#ifdef OLED_DISPLAY_VERBOSE
    oled_set_cursor(col, line);
    //    oled_set_cursor(7, 3);
    is_bootmagic_on = !keymap_config.swap_lctl_lgui;
#else
    is_bootmagic_on = keymap_config.swap_lctl_lgui;
#endif

#ifdef OLED_DISPLAY_VERBOSE
    if (keymap_config.swap_lctl_lgui)
#else
    oled_write_P(PSTR(OLED_RENDER_BOOTMAGIC_NAME), false);
    oled_write_P(PSTR(" "), false);
#endif
    {
        oled_write_P(logo[1][0], is_bootmagic_on);
#ifdef OLED_DISPLAY_VERBOSE
    } else {
#endif
        oled_write_P(logo[0][0], !is_bootmagic_on);
    }
#ifndef OLED_DISPLAY_VERBOSE
    oled_write_P(PSTR(" "), false);
    oled_write_P(logo[1][1], is_bootmagic_on);
    oled_write_P(logo[0][1], !is_bootmagic_on);
#endif
    oled_write_P(PSTR(" "), false);
    oled_write_P(PSTR(OLED_RENDER_BOOTMAGIC_NKRO), keymap_config.nkro);
    oled_write_P(PSTR(" "), false);
#if defined(AUTOCORRECT_ENABLE)
    oled_write_P(PSTR("CRCT"), autocorrect_is_enabled());
    oled_write_P(PSTR(" "), false);
#else
    oled_write_P(PSTR(OLED_RENDER_BOOTMAGIC_NOGUI), keymap_config.no_gui);
#endif
#ifdef OLED_DISPLAY_VERBOSE
    oled_set_cursor(col, line + 1);
    if (keymap_config.swap_lctl_lgui) {
        oled_write_P(logo[1][1], is_bootmagic_on);
    } else {
        oled_write_P(logo[0][1], !is_bootmagic_on);
    }
#endif
    oled_write_P(PSTR(" "), false);
    oled_write_P(PSTR(OLED_RENDER_BOOTMAGIC_ONESHOT), is_oneshot_enabled());
#ifdef SWAP_HANDS_ENABLE
    oled_write_P(PSTR(" "), false);
    oled_write_P(PSTR(OLED_RENDER_BOOTMAGIC_SWAP), swap_hands);
    oled_write_P(PSTR(" "), false);
#endif
}

void render_user_status(uint8_t col, uint8_t line) {
#ifdef AUDIO_ENABLE
    bool is_audio_on = false, l_is_clicky_on = false;
#    ifdef SPLIT_KEYBOARD

    is_audio_on = user_state.audio_enable;
#        ifdef AUDIO_CLICKY
    l_is_clicky_on = user_state.audio_clicky_enable;
#        endif
#    else
    is_audio_on = is_audio_on();
#        ifdef AUDIO_CLICKY
    l_is_clicky_on = is_clicky_on();
#        endif
#    endif
#endif
#if defined(OLED_DISPLAY_VERBOSE)
    oled_set_cursor(col, line);
#endif
    oled_write_P(PSTR(OLED_RENDER_USER_NAME), false);
#if !defined(OLED_DISPLAY_VERBOSE)
    oled_write_P(PSTR(" "), false);
#endif
#if defined(RGB_MATRIX_ENABLE)
    oled_write_P(PSTR(OLED_RENDER_USER_ANIM), userspace_config.rgb_matrix_idle_anim);
#    if !defined(OLED_DISPLAY_VERBOSE)
    oled_write_P(PSTR(" "), false);
#    endif
#elif defined(POINTING_DEVICE_ENABLE) && defined(POINTING_DEVICE_AUTO_MOUSE_ENABLE)
    static const char PROGMEM mouse_lock[3] = {0xF2, 0xF3, 0};
    oled_write_P(mouse_lock, get_auto_mouse_toggle());
#endif
#ifdef AUDIO_ENABLE
    static const char PROGMEM audio_status[2][3] = {{0xE0, 0xE1, 0}, {0xE2, 0xE3, 0}};
    oled_write_P(audio_status[is_audio_on], false);

#    ifdef AUDIO_CLICKY
    static const char PROGMEM audio_clicky_status[2][3] = {{0xF4, 0xF5, 0}, {0xF6, 0xF7, 0}};
    oled_write_P(audio_clicky_status[l_is_clicky_on && is_audio_on], false);
#        if !defined(OLED_DISPLAY_VERBOSE)
    oled_write_P(PSTR(" "), false);
#        endif
#    endif
#endif

    static const char PROGMEM rgb_layer_status[2][3] = {{0xEE, 0xEF, 0}, {0xF0, 0xF1, 0}};
    oled_write_P(rgb_layer_status[userspace_config.rgb_layer_change], false);
    static const char PROGMEM cat_mode[3] = {0xF9, 0xFA, 0};
    oled_write_P(cat_mode, get_keyboard_lock());
#if defined(UNICODE_COMMON_ENABLE)
    static const char PROGMEM uc_mod_status[5][3] = {
        {0xEC, 0xED, 0}, {0x20, 0x20, 0}, {0x20, 0x20, 0}, {0x20, 0x20, 0}, {0xEA, 0xEB, 0}};
    oled_write_P(uc_mod_status[get_unicode_input_mode()], false);
#endif
    if (userspace_config.nuke_switch) {
#if !defined(OLED_DISPLAY_VERBOSE)
        oled_write_P(PSTR(" "), false);
#endif
        static const char PROGMEM nukem_good[2] = {0xFB, 0};
        oled_write_P(nukem_good, false);
#if !defined(OLED_DISPLAY_VERBOSE)
        oled_advance_page(true);
#endif
    }
#if defined(OLED_DISPLAY_VERBOSE)
    oled_advance_page(true);
#endif
}

void render_rgb_hsv(uint8_t col, uint8_t line) {
    oled_set_cursor(col, line);
    oled_write_P(PSTR("HSV: "), false);
#ifdef RGB_MATRIX_ENABLE
    oled_write(get_u8_str(rgb_matrix_get_hue(), ' '), false);
    oled_write_P(PSTR(", "), false);
    oled_write(get_u8_str(rgb_matrix_get_sat(), ' '), false);
    oled_write_P(PSTR(", "), false);
    oled_write(get_u8_str(rgb_matrix_get_val(), ' '), false);
#elif RGBLIGHT_ENABLE
    if (is_rgblight_startup_running()) {
        oled_write_P(PSTR("Start Animation"), false);
    } else {
        oled_write(get_u8_str(rgblight_get_hue(), ' '), false);
        oled_write_P(PSTR(", "), false);
        oled_write(get_u8_str(rgblight_get_sat(), ' '), false);
        oled_write_P(PSTR(", "), false);
        oled_write(get_u8_str(rgblight_get_val(), ' '), false);
    }
#endif
}

void render_rgb_mode(uint8_t col, uint8_t line) {
    oled_set_cursor(col, line);
    __attribute__((unused)) static uint8_t mode;
    bool                                   need_update = false;
    static char                            buf[21]     = {0};

#ifdef RGB_MATRIX_ENABLE
    if (mode != rgb_matrix_get_mode()) {
        snprintf(buf, sizeof(buf), "%-20s", rgb_matrix_name(rgb_matrix_get_mode()));
        mode        = rgb_matrix_get_mode();
        need_update = true;
    }
#elif RGBLIGHT_ENABLE
    if (mode != rgblight_get_mode()) {
        snprintf(buf, sizeof(buf), "%-20s", rgblight_name(rgblight_get_mode()));
        mode        = rgblight_get_mode();
        need_update = true;
    }
#endif
    if (need_update) {
        for (uint8_t i = 1; i < sizeof(buf); ++i) {
            if (buf[i] == 0)
                break;
            else if (buf[i] == '_')
                buf[i] = ' ';
            else if (buf[i - 1] == ' ')
                buf[i] = toupper(buf[i]);
            else if (buf[i - 1] != ' ')
                buf[i] = tolower(buf[i]);
        }
    }

    oled_write(buf, false);
}

void render_wpm(uint8_t padding, uint8_t col, uint8_t line) {
#ifdef WPM_ENABLE
    oled_set_cursor(col, line);
    oled_write_P(PSTR(OLED_RENDER_WPM_COUNTER), false);
    if (padding) {
        for (uint8_t n = padding; n > 0; n--) {
            oled_write_P(PSTR(" "), false);
        }
    }
    oled_write(get_u8_str(get_current_wpm(), ' '), false);
#endif
}

//=============  USER CONFIG PARAMS  ===============
// wpm graph originally designed by john-ezra

// start_offset              = 3;
// cutoff                    = 125;
// for 128x128:
// max_lines_graph          = 54;
// vertical_offset          = 64;
// for 128x64:
// max_lines_graph          = 64;
// vertical_offset          = 0;

/**
 * @brief Renders dynamic WPM graph on screen
 *
 * @param start_offset horizontal pixel location (from left side of screen) to start rendering graph
 * @param cutoff horizontal pixel location (from left side of screen) to stop rendering graph
 * @param max_lines_graph vertical pixel height of graph
 * @param vertical_offset vertial pixel location (from top) to start rendering graph
 */
void render_wpm_graph(uint8_t start_offset, uint8_t cutoff, uint8_t max_lines_graph, uint8_t vertical_offset) {
#ifdef WPM_ENABLE
    static uint16_t timer   = 0;
    static uint8_t  x       = OLED_DISPLAY_HEIGHT - 1;
    uint8_t         currwpm = get_current_wpm();
    float           max_wpm = OLED_WPM_GRAPH_MAX_WPM;

    if (timer_elapsed(timer) >
        OLED_WPM_GRAPH_REFRESH_INTERVAL) { // check if it's been long enough before refreshing graph
        x = (max_lines_graph - 1) -
            ((currwpm / max_wpm) * (max_lines_graph - 1));                       // main calculation to plot graph line
        for (uint8_t i = 0; i <= OLED_WPM_GRAPH_GRAPH_LINE_THICKNESS - 1; i++) { // first draw actual value line
            oled_write_pixel(start_offset, x + i + vertical_offset, true);
        }
#    ifdef OLED_WPM_GRAPH_VERTICAL_LINE
        static uint8_t vert_count = 0;
        if (vert_count == OLED_WPM_GRAPH_VERTCAL_LINE_INTERVAL) {
            vert_count = 0;
            while (x <= (max_lines_graph - 1)) {
                oled_write_pixel(start_offset, x + vertical_offset, true);
                x++;
            }
        } else {
            for (uint8_t i = (max_lines_graph - 1); i > x; i--) {
                if (i % OLED_WPM_GRAPH_AREA_FILL_INTERVAL == 0) {
                    oled_write_pixel(start_offset, i + vertical_offset, true);
                }
            }
            vert_count++;
        }
#    else
        for (int i = (max_lines_graph - 1); i > x; i--) {
            if (i % OLED_WPM_GRAPH_AREA_FILL_INTERVAL == 0) {
                oled_write_pixel(start_offset, i + vertical_offset, true);
            }
        }
#    endif
        uint8_t y_start  = ceil(vertical_offset / 8);
        uint8_t y_length = y_start + ceil(max_lines_graph / 8);
        oled_pan_section(false, y_start, y_length, start_offset,
                         cutoff); // then move the entire graph one pixel to the right
        timer = timer_read();     // refresh the timer for the next iteration
    }
#endif
}

#if defined(POINTING_DEVICE_ENABLE)
void render_pointing_dpi_status(uint16_t cpi, uint8_t padding, uint8_t col, uint8_t line) {
    oled_set_cursor(col, line);
    oled_write_P(PSTR("CPI:"), false);
    if (padding) {
        for (uint8_t n = padding - 1; n > 0; n--) {
            oled_write_P(PSTR(" "), false);
        }
    }

    oled_write(get_u16_str(cpi, ' '), false);
}
#endif

// WPM-responsive animation stuff here
#ifndef OLED_SLEEP_SPEED
#    define OLED_SLEEP_SPEED 10
#endif
#ifndef OLED_KAKI_SPEED
#    define OLED_KAKI_SPEED 40
#endif
#ifndef OLED_MATI_SPEED
#    define OLED_MATI_SPEED 60
#endif
#ifndef OLED_PET_ARRAY
#    define OLED_PET_ARRAY tora_the_cat_animation
#endif

// #define ANIM_FRAME_DURATION 500 // how long each frame lasts in ms
//  #define SLEEP_TIMER 60000 // should sleep after this period of 0 wpm, needs fixing
#if (OLED_SLEEP_FRAMES > OLED_ANIM_MAX_FRAMES) || (OLED_WAKE_FRAMES > OLED_ANIM_MAX_FRAMES) || \
    (OLED_KAKI_FRAMES > OLED_ANIM_MAX_FRAMES) || (OLED_RTOGI_FRAMES > OLED_ANIM_MAX_FRAMES)
#    error frame size too large
#endif

static uint8_t animation_frame = 0;
static uint8_t animation_type  = 0;

void render_pet(uint8_t col, uint8_t line) {
    for (uint8_t i = 0; i < 4; i++) {
        oled_set_cursor(col, line + i);
        oled_write_raw_P(pet_animiations[1][animation_type][animation_frame][i], OLED_ANIM_SIZE);
    }
}

uint32_t pet_animation_phases(uint32_t triger_time, void *cb_arg) {
    static uint32_t anim_frame_duration = 500;
#if defined(POINTING_DEVICE_ENABLE) && defined(POINTING_DEVICE_AUTO_MOUSE_ENABLE)
    if (get_auto_mouse_toggle()) {
        animation_frame     = (animation_frame + 1) % OLED_RTOGI_FRAMES;
        animation_type      = 4;
        anim_frame_duration = 300;
    } else
#endif
    {
#ifdef WPM_ENABLE
        if (get_current_wpm() <= OLED_SLEEP_SPEED) {
#endif
            animation_frame     = (animation_frame + 1) % OLED_SLEEP_FRAMES;
            animation_type      = 0;
            anim_frame_duration = 500;
#ifdef WPM_ENABLE
        } else if (get_current_wpm() <= OLED_KAKI_SPEED) {
            animation_frame     = (animation_frame + 1) % OLED_KAKI_FRAMES;
            animation_type      = 1;
            anim_frame_duration = 500;
        } else if (get_current_wpm() <= OLED_MATI_SPEED) {
            animation_frame     = (animation_frame + 1) % OLED_MATI_FRAMES;
            animation_type      = 2;
            anim_frame_duration = 500;
        } else {
            animation_frame     = (animation_frame + 1) % OLED_AWAKE_FRAMES;
            animation_type      = 3;
            anim_frame_duration = 300;
        }
#endif
    }
    return anim_frame_duration;
}

void render_unicode_mode(uint8_t col, uint8_t line) {
#if defined(CUSTOM_UNICODE_ENABLE) && defined(UNICODE_COMMON_ENABLE)
    oled_set_cursor(col, line);
    oled_write_P(PSTR("Unicode:"), false);
    oled_write_P(unicode_mode_str[unicode_typing_mode], false);
#endif
}

void render_unicode_mode_small(uint8_t col, uint8_t line, bool invert) {
#if defined(CUSTOM_UNICODE_ENABLE) && defined(UNICODE_COMMON_ENABLE)
    oled_set_cursor(col, line);
    oled_write_P(PSTR("UC"), invert);
    oled_write_P(unicode_mode_str[unicode_typing_mode], invert);
#endif
}

void render_mouse_mode(uint8_t col, uint8_t line) {
#if (defined(KEYBOARD_bastardkb_charybdis) || defined(KEYBOARD_handwired_tractyl_manuform)) && \
    defined(POINTING_DEVICE_ENABLE)
    // credit and thanks to jaspertandy on discord for these images
    uint8_t image_index = 0;
#    ifdef OLED_DISPLAY_TEST
    image_index = animation_frame;
#    else
    if (charybdis_get_pointer_sniping_enabled()) {
        image_index = 1;
    } else if (charybdis_get_pointer_dragscroll_enabled()) {
        image_index = 2;
    }
#    endif

    oled_set_cursor(col, line);
    oled_write_raw_P(mouse_logo[image_index][0], 16);
    oled_set_cursor(col, line + 1);
    oled_write_raw_P(mouse_logo[image_index][1], 16);
#endif
}

void render_status_right(void) {
#if defined(OLED_DISPLAY_VERBOSE)
    render_default_layer_state(1, 1);
#else
    render_default_layer_state(0, 0);
#endif

    /* Show Keyboard Layout  */
    render_layer_state(1, 2);
    render_mod_status(get_mods() | get_oneshot_mods(), 1, 5);
#if !defined(OLED_DISPLAY_VERBOSE) && defined(WPM_ENABLE) && !defined(STM32F303xC)
    render_wpm(2, 7, 1);
#endif
    render_keylock_status(host_keyboard_led_state(), 1, 6);
}

void render_status_left(void) {
#if defined(OLED_DISPLAY_VERBOSE)
    render_pet(0, 1);

#    if defined(WPM_ENABLE)
    render_wpm(1, 7, 1);
#    else
    render_matrix_scan_rate(1, 7, 1);
#    endif
#    if (defined(KEYBOARD_bastardkb_charybdis) || defined(KEYBOARD_handwired_tractyl_manuform)) && \
        defined(POINTING_DEVICE_ENABLE)
    render_pointing_dpi_status(charybdis_get_pointer_sniping_enabled() ? charybdis_get_pointer_sniping_dpi()
                                                                       : charybdis_get_pointer_default_dpi(),
                               1, 7, 2);
    render_mouse_mode(17, 1);
#    elif defined(WPM_ENABLE)
    render_matrix_scan_rate(1, 7, 2);
#    endif
    /* Show Keyboard Layout  */
    render_bootmagic_status(7, 3);
    render_user_status(1, 5);
#else
    render_default_layer_state(0, 0);
    /* Show Keyboard Layout  */
    render_bootmagic_status(7, 3);
    render_user_status(1, 5);

    render_keylogger_status(1, 6);
#endif
}

void render_autocorrected_info(uint8_t col, uint8_t line) {
#ifdef AUTOCORRECT_ENABLE
    extern char autocorrected_str[2][21];

    oled_set_cursor(col, line);
    oled_write_ln_P(PSTR("Autocorrected:"), false);
    oled_set_cursor(col, line + 1);
    oled_write_ln(autocorrected_str[0], false);
    oled_set_cursor(col, line + 2);
    oled_write_ln_P(PSTR("to:"), false);
    oled_set_cursor(col, line + 3);
    oled_write_ln(autocorrected_str[1], false);
#endif
}

void render_cyberpunk_logo(uint8_t col, uint8_t line) {
    oled_set_cursor(col, line);
    oled_write_raw_P(cyberpunk_logo, sizeof(cyberpunk_logo));
}

void render_arasaka_logo_small(uint8_t col, uint8_t line) {
    for (uint8_t i = 0; i < 4; i++) {
        oled_set_cursor(col, line + i);
        oled_write_raw_P(arasaka_logo[i], ARRAY_SIZE(arasaka_logo[i]));
    }
}

void render_arasaka_logo(uint8_t col, uint8_t line) {
    static bool     glitch      = true;
    static uint8_t  frame_count = 15;
    static uint16_t arasaka_timer;

    oled_set_cursor(col, line);

    uint16_t timer        = timer_elapsed(arasaka_timer);
    bool     can_be_dirty = true;

    if (timer < 150) {
        oled_write_raw_P(text_glitch_dirty[rand() % text_glitch_dirty_count], sizeof(text_glitch_dirty[0]));
        return;
    }

    if (timer < 250) {
        uint8_t frame =
            can_be_dirty ? rand() % (text_glitch_count + text_glitch_dirty_count) : rand() % text_glitch_count;

        if (frame < text_glitch_count) {
            oled_write_raw_P(text_glitch[frame], sizeof(text_glitch[0]));

            return;
        }

        oled_write_raw_P(text_glitch_dirty[rand() % text_glitch_dirty_count], sizeof(text_glitch_dirty[0]));

        return;
    }

    if (timer > 9750 && timer < 9850) {
        uint8_t frame =
            can_be_dirty ? rand() % (text_glitch_count + text_glitch_dirty_count) : rand() % text_glitch_count;

        if (frame < text_glitch_count) {
            oled_write_raw_P(text_glitch[frame], sizeof(text_glitch[0]));

            return;
        }

        oled_write_raw_P(text_glitch_dirty[rand() % text_glitch_dirty_count], sizeof(text_glitch_dirty[0]));
        return;
    }

    if (timer > 9850 && timer < 10000) {
        oled_write_raw_P(text_glitch_dirty[rand() % text_glitch_dirty_count], sizeof(text_glitch_dirty[0]));
        return;
    }

    if (timer > 10000) {
        arasaka_timer = timer_read();
    }

    if (glitch && 0 != frame_count) {
        frame_count--;
        uint8_t frame =
            can_be_dirty ? rand() % (text_glitch_count + text_glitch_dirty_count) : rand() % text_glitch_count;

        if (frame < text_glitch_count) {
            oled_write_raw_P(text_glitch[frame], sizeof(text_glitch[0]));

            return;
        }

        oled_write_raw_P(text_glitch_dirty[rand() % text_glitch_dirty_count], sizeof(text_glitch_dirty[0]));
        return;
    }

    glitch = false;

    oled_write_raw_P(text_clean, frame_size);

    if (1 == rand() % 60) {
        glitch      = true;
        frame_count = 1 + rand() % 4;
        return;
    }

    if (1 == rand() % 60) {
        glitch      = true;
        frame_count = 1 + rand() % 10;
    }
}

void render_train_animation(uint8_t col, uint8_t line) {
    static uint16_t timer = 0;
    static uint8_t  frame = 0;
    if (timer_elapsed(timer) > 500) {
        frame++;
        if (frame == 10) {
            frame = 0;
        }
        timer = timer_read();
    }
    oled_set_cursor(col, line);
    oled_write_raw_P(train_animation[frame][i], sizeof(train_animation[0][0]));
}

void oled_render_mario(uint8_t col, uint8_t line) {
    static uint16_t timer = 0;
    static uint8_t  frame = 0;
    if (timer_elapsed(timer) > 500) {
        frame++;
        if (frame == 3) {
            frame = 0;
        }
        timer = timer_read();
    }
    for (uint8_t i = 0; i < 4; i++) {
        oled_set_cursor(col, line + i);
        oled_write_raw_P(mario_animation[frame][i], sizeof(mario_animation[0][0]));
    }
}

void render_os(uint8_t col, uint8_t line) {
#ifdef OS_DETECTION_ENABLE
    oled_set_cursor(col, line);
    oled_write_P(PSTR("OS: "), false);
    os_variant_t os_type = detected_host_os();
    switch (os_type) {
        case OS_LINUX:
            oled_write_ln_P(PSTR("Linux"), false);
            break;
        case OS_WINDOWS:
            oled_write_ln_P(PSTR("Windows"), false);
            break;
        case OS_MACOS:
            oled_write_ln_P(PSTR("MacOS"), false);
            break;
        case OS_IOS:
            oled_write_ln_P(PSTR("iOS"), false);
            break;
#    if 0
        case OS_WINDOWS_UNSURE:
            oled_write_ln_P(PSTR("Windows?"), false);
            break;
        case OS_PS5:
            oled_write_ln_P(PSTR("Sony"), false);
            break;
        case OS_HANDHELD:
            oled_write_ln_P(PSTR("Handheld"), false);
            break;
#    endif
        case OS_UNSURE:
            oled_write_ln_P(PSTR("Unsure"), false);
            break;
        default:
            oled_write_ln(get_u8_str(os_type, ' '), false);
            break;
    }
#endif
}

void oled_render_time(uint8_t col, uint8_t line) {
#ifdef RTC_ENABLE
    oled_set_cursor(col, line);
    if (rtc_is_connected()) {
#    ifdef DS3231_RTC_DRIVER_ENABLE
        oled_write_P(PSTR("RTC Temp: "), false);
        oled_write(ds3231_read_temp_imperial_str(), false);
        oled_write_char(0xF8, false);
        oled_write_ln_P(PSTR("F "), false);
#    else
        oled_write_ln_P(PSTR("RTC Temp: N/A"), false);
#    endif
        oled_set_cursor(col, line + 1);
        oled_write_ln(rtc_read_date_time_str(), false);
    } else {
        oled_write_ln_P(PSTR("RTC not found"), false);
        oled_advance_page(true);
    }
#endif
}

void oled_render_time_small(uint8_t col, uint8_t line, uint8_t padding) {
#ifdef RTC_ENABLE
    oled_set_cursor(col, line);
    if (rtc_is_connected()) {
#    ifdef DS3231_RTC_DRIVER_ENABLE
        oled_write_P(PSTR("RTC Temp: "), false);
        oled_write(get_u8_str((uint8_t)(ds3231_read_temp()), ' '), false);
        oled_write_char(0xF8, false);
#    else
        oled_write_ln_P(PSTR("RTC Temp: N/A"), false);
#    endif
        oled_set_cursor(col + padding, line + 1);
        oled_write(rtc_read_date_str(), false);
        oled_set_cursor(col + padding + 2, line + 2);
        oled_write(rtc_read_time_str(), false);
    } else {
        oled_write_ln_P(PSTR("RTC not found"), false);
        oled_set_cursor(col, line + 1);
        oled_advance_page(true);
        oled_set_cursor(col, line + 2);
        oled_advance_page(true);
    }
#endif
}

#ifdef OLED_DISPLAY_128X128
__attribute__((weak)) void oled_render_large_display(bool side) {
    if (side) {
        render_rgb_hsv(1, 6);
        render_rgb_mode(1, 7);

        render_arasaka_logo(0, 8);
        render_wpm_graph(23, 107, 25, 96);
        oled_render_mario(1, 11);
    } else {
        // oled_advance_page(true);
#    if defined(LAYER_MAP_ENABLE)
        oled_set_cursor(1, 7);

        for (uint8_t i = 0; i < LAYER_MAP_ROWS; i++) {
            oled_set_cursor(1 + 1, 7 + i);
            for (uint8_t j = 0; j < LAYER_MAP_COLS; j++) {
                uint16_t keycode = layer_map[i][j];
                if (IS_QK_MOD_TAP(keycode)) {
                    keycode = keycode_config(QK_MOD_TAP_GET_TAP_KEYCODE(keycode));
                } else if (IS_QK_LAYER_TAP(keycode)) {
                    keycode = keycode_config(QK_LAYER_TAP_GET_TAP_KEYCODE(keycode));
                } else if (IS_QK_MODS(keycode)) {
                    keycode = keycode_config(QK_MODS_GET_BASIC_KEYCODE(keycode));
                } else if (IS_QK_ONE_SHOT_MOD(keycode)) {
                    keycode = keycode_config(0xE0 + biton(QK_ONE_SHOT_MOD_GET_MODS(keycode) & 0xF) +
                                             biton(QK_ONE_SHOT_MOD_GET_MODS(keycode) & 0x10));
                } else if (IS_QK_BASIC(keycode)) {
                    keycode = keycode_config(keycode);
                }

                char code = 0;
                if (keycode > 0xFF) {
                    if (keycode == UC_IRNY) {
                        code = 0xFD;
                    } else if (keycode == UC_CLUE) {
                        code = 0xFE;
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
        render_autocorrected_info(1, 7);
        render_os(1, 11);
        render_unicode_mode(1, 12);
        oled_render_time(1, 13);
#    endif
    }
}
#endif

__attribute__((weak)) void render_oled_title(bool side) {
    oled_write_P(side ? PSTR("     Left    ") : PSTR("    Right    "), true);
    // oled_write_P(PSTR(    "1234567890123"         "1234567890123"), true);
}

__attribute__((weak)) oled_rotation_t oled_init_keymap(oled_rotation_t rotation, bool has_run) {
    return rotation;
}

oled_rotation_t oled_init_user(oled_rotation_t rotation) {
    static bool has_run = false;
    rotation            = oled_init_keymap(rotation, has_run);

    if (has_run) {
        return rotation;
    }

    if (is_keyboard_master()) {
        memset(oled_keylog_str, ' ', OLED_KEYLOGGER_LENGTH);
        oled_keylog_str[OLED_KEYLOGGER_LENGTH] = '\0';
    }

    kittoken = defer_exec(3000, pet_animation_phases, NULL);

    oled_clear();
    oled_render_dirty(true);
    has_run = true;
    return rotation;
}

__attribute__((weak)) bool oled_task_keymap(void) {
    return true;
}

__attribute__((weak)) void render_oled_screensaver(void) {
    oled_off();
}

bool oled_task_user(void) {
    static bool was_screensaver_enabled = false;
#ifndef OLED_DISPLAY_TEST
    if (!is_oled_enabled) {
        oled_off();
        return false;
    } else
#endif
    {
        oled_on();
    }

    if (oled_screensaver_enabled) {
        was_screensaver_enabled = true;
        render_oled_screensaver();
        return false;
    }
    if (was_screensaver_enabled) {
        was_screensaver_enabled = false;
        oled_clear();
    }

    if (!oled_task_keymap()) {
        return false;
    }

#if defined(OLED_DISPLAY_VERBOSE)
    oled_write_raw_P(header_image, sizeof(header_image));
    oled_set_cursor(4, 0);
    render_oled_title(is_keyboard_left());
#endif

#ifndef OLED_DISPLAY_TEST
    if (is_keyboard_left()) {
#endif
        render_status_left();
#if defined(OLED_DISPLAY_128X128)
        oled_render_large_display(true);
#endif
#ifndef OLED_DISPLAY_TEST
    } else {
        render_status_right();
#    if defined(OLED_DISPLAY_128X128)
        oled_render_large_display(false);
#    endif
    }
#endif

#if defined(OLED_DISPLAY_VERBOSE)
    uint8_t num_of_rows;
#    if defined(OLED_DISPLAY_128X128)
    num_of_rows = 15;
#    else
    num_of_rows = 7;
#    endif
    for (uint8_t i = 1; i < num_of_rows; i++) {
        oled_set_cursor(0, i);
        oled_write_raw_P(display_border, sizeof(display_border));
        oled_set_cursor(21, i);
        oled_write_raw_P(display_border, sizeof(display_border));
    }

    oled_set_cursor(0, num_of_rows);
    oled_write_raw_P(footer_image2, sizeof(footer_image2));

    if (is_keyboard_left()) {
        oled_set_cursor(4, num_of_rows);
        oled_write(oled_keylog_str, true);
    } else {
        render_unicode_mode_small(4, num_of_rows, true);
    }
#endif

    return false;
}

void housekeeping_task_oled(void) {
    is_oled_enabled = oled_screensaver_enabled = false;

    if (is_device_suspended() || is_oled_force_off) {
        is_oled_enabled = false;
    } else if (userspace_config.oled_lock) {
        is_oled_enabled = true;
    } else if (last_input_activity_elapsed() < (10 * 60 * 1000)) {
        is_oled_enabled = true;
    }
    if (is_oled_enabled && last_input_activity_elapsed() > (1 * 60 * 1000)) {
        oled_screensaver_enabled = true;
    }

    if (oled_get_brightness() != userspace_config.oled_brightness) {
        oled_set_brightness(userspace_config.oled_brightness);
    }
}

void oled_shutdown(bool jump_to_bootloader) {
    oled_clear();
#if defined(OLED_DISPLAY_128X128)
    oled_set_cursor(0, 4);
    oled_write_raw_P(qmk_large_logo, sizeof(qmk_large_logo));
    oled_set_cursor(0, 15);
#else
    oled_set_cursor(0, 0);
#endif
    if (jump_to_bootloader) {
        oled_write_P(PSTR("Jumping to bootloader"), false);
    } else {
        oled_write_P(PSTR("   Please stand by   "), false);
    }
    oled_render_dirty(true);
}
