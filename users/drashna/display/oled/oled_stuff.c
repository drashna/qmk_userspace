// Copyright 2020 Christopher Courtney, aka Drashna Jael're  (@drashna) <drashna@live.com>
// Copyright 2021 John Ezra - wpm graph
// SPDX-License-Identifier: GPL-3.0-or-later

#include "oled_stuff.h"
#include "drashna.h"
#include <string.h>
#include <stdio.h>
#include <math.h>
#include "keyboard.h"
#include "lib/lib8tion/lib8tion.h"
#include "progmem.h"

#ifdef COMMUNITY_MODULE_RTC_ENABLE
#    include "rtc.h"
#    ifdef DS3231_RTC_DRIVER_ENABLE
#        include "drivers/ds3231.h"
#    endif
#endif
#ifdef COMMUNITY_MODULE_LAYER_MAP_ENABLE
#    include "layer_map.h"
#endif // COMMUNITY_MODULE_LAYER_MAP_ENABLE
#ifdef COMMUNITY_MODULE_KEYBOARD_LOCK_ENABLE
#    include "keyboard_lock.h"
#endif // COMMUNITY_MODULE_KEYBOARD_LOCK_ENABLE
#ifdef COMMUNITY_MODULE_UNICODE_TYPING_ENABLE
#    include "unicode_typing.h"
#endif // COMMUNITY_MODULE_UNICODE_TYPING_ENABLE
#if defined(RGBLIGHT_ENABLE)
#    include "rgb/rgb_stuff.h"
#endif // defined(RGBLIGHT_ENABLE)
#if defined(RGB_MATRIX_ENABLE)
#    include "rgb/rgb_matrix_stuff.h"
#endif // defined(RGB_MATRIX_ENABLE)
#if defined(COMMUNITY_MODULE_DISPLAY_MENU_ENABLE)
#    include "oled_render_menu.h"
#endif // COMMUNITY_MODULE_DISPLAY_MENU_ENABLE
#ifndef OLED_BRIGHTNESS_STEP
#    define OLED_BRIGHTNESS_STEP 32
#endif

bool is_oled_enabled = true, is_oled_force_off = false, oled_screensaver_enabled = false;

uint32_t               oled_timer = 0;
extern oled_rotation_t oled_rotation;
extern uint8_t         oled_rotation_width;
deferred_token         kittoken;

extern uint8_t         oled_buffer[OLED_MATRIX_SIZE];
extern OLED_BLOCK_TYPE oled_dirty;

#ifdef DISPLAY_KEYLOGGER_ENABLE
static bool keylogger_has_changed                               = true;
static char display_keylogger_string[OLED_KEYLOGGER_LENGTH + 1] = {
    [0 ... OLED_KEYLOGGER_LENGTH - 1] = '_',
    [OLED_KEYLOGGER_LENGTH]           = '\0',
};

static const char PROGMEM code_to_name[256] = {
    // clang-format off
//   0    1    2    3    4    5    6    7    8    9    A    B    c    D    E    F
    ' ', ' ', ' ', ' ', 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l',  // 0x
    'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z', '1', '2',  // 1x
    '3', '4', '5', '6', '7', '8', '9', '0',  20,  19,  27,  26,  22, '-', '=', '[',  // 2x
    ']','\\', '#', ';','\'', '`', ',', '.', '/',0xC9,0xD5,0xD6,0xD7,0xD8,0xD9,0xDA,  // 3x
   0xDB,0xDC,0xDD,0xDE,0XDF,0xFB, 'P',0xCB,  19, ' ',  17,  30,  16,  16,  31,  26,  // 4x
     27,  25,  24, 'N', '/', '*', '-', '+',  23, '1', '2', '3', '4', '5', '6', '7',  // 5x
    '8', '9', '0', '.','\\', 'A',   0, '=', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',  // 6x
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

/**
 * @brief parses pressed keycodes and saves to buffer
 *
 * @param keycode Keycode pressed from switch matrix
 * @param record keyrecord_t data structure
 */
__attribute__((unused)) static void add_keylog(uint16_t keycode, keyrecord_t *record, char *str, uint8_t length) {
    userspace_runtime_state.last_keycode   = keycode;
    userspace_runtime_state.last_key_event = record->event;
    keycode                                = extract_basic_keycode(keycode, record, true);

    if ((keycode == KC_BSPC) && mod_config(get_mods() | get_oneshot_mods()) & MOD_MASK_CTRL) {
        memset(str, '_', length);
        str[length - 1] = 0x00;
        return;
    }
    if (record->tap.count) {
        keycode &= 0xFF;
    } else if (keycode > 0xFF) {
        return;
    }

    memmove(str, str + 1, length - 2);

    if (keycode < ARRAY_SIZE(code_to_name)) {
        str[(length - 2)] = pgm_read_byte(&code_to_name[keycode]);
    }
}

const char *get_oled_keylogger_str(void) {
    return display_keylogger_string;
}

void split_sync_oled_keylogger_str(const uint8_t *data, uint8_t size) {
    if (memcmp(data, display_keylogger_string, size) != 0) {
        memcpy(display_keylogger_string, data, size);
        keylogger_has_changed = true;
    }
}
#endif // DISPLAY_KEYLOGGER_ENABLE

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
 * @brief Keycode handler for oled display.
 *
 * This adds pressed keys to buffer, but also resets the oled timer
 *
 * @param keycode Keycode from matrix
 * @param record keyrecord data structure
 * @return true
 * @return false
 */
bool process_record_user_oled(uint16_t keycode, keyrecord_t *record) {
#ifdef DISPLAY_KEYLOGGER_ENABLE
    keylogger_has_changed = true;
#endif // DISPLAY_KEYLOGGER_ENABLE

    if (record->event.pressed) {
#ifdef DISPLAY_KEYLOGGER_ENABLE
        add_keylog(keycode, record, display_keylogger_string, ARRAY_SIZE(display_keylogger_string));
#endif // DISPLAY_KEYLOGGER_ENABLE

        switch (keycode) {
            case OLED_BRIGHTNESS_INC:
                oled_brightness_increase_step();
                break;
            case OLED_BRIGHTNESS_DEC:
                oled_brightness_decrease_step();
                break;
            case OLED_LOCK:
                userspace_config.display.oled.screen_lock = !userspace_config.display.oled.screen_lock;
                eeconfig_update_user_datablock(&userspace_config, 0, EECONFIG_USER_DATA_SIZE);
                if (userspace_config.display.oled.screen_lock) {
                    oled_on();
                }
                break;
            case OLED_ROTATE_CW:
                display_rotate_screen(true, true);
                break;
            case OLED_ROTATE_CCW:
                display_rotate_screen(false, true);
                break;
        }
    }
    return true;
}

/**
 * @brief Renders keylogger buffer to oled
 *
 */
void render_keylogger_status(uint8_t col, uint8_t line) {
#ifdef DISPLAY_KEYLOGGER_ENABLE
#    ifdef OLED_DISPLAY_VERBOSE
    oled_set_cursor(col, line);
#    endif
    oled_write_P(PSTR(OLED_RENDER_KEYLOGGER), false);
    oled_write(display_keylogger_string, false);
#endif // DISPLAY_KEYLOGGER_ENABLE
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
                 get_layer_name_string(default_layer_state, false, true));
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
#if defined(OLED_DISPLAY_VERBOSE)
    oled_set_cursor(col, line);
#endif
    oled_write_P(PSTR(OLED_RENDER_USER_NAME), false);
#if !defined(OLED_DISPLAY_VERBOSE)
    oled_write_P(PSTR(" "), false);
#endif
#if defined(RGB_MATRIX_ENABLE)
    oled_write_P(PSTR(OLED_RENDER_USER_ANIM), userspace_config.rgb.idle_anim);
#    if !defined(OLED_DISPLAY_VERBOSE)
    oled_write_P(PSTR(" "), false);
#    endif
#elif defined(POINTING_DEVICE_ENABLE) && defined(POINTING_DEVICE_AUTO_MOUSE_ENABLE)
    static const char PROGMEM mouse_lock[3] = {0xF2, 0xF3, 0};
    oled_write_P(mouse_lock, get_auto_mouse_toggle());
#endif
#ifdef AUDIO_ENABLE
    static const char PROGMEM audio_status[2][3] = {{0xE0, 0xE1, 0}, {0xE2, 0xE3, 0}};
    oled_write_P(audio_status[userspace_runtime_state.audio.enable], false);

#    ifdef AUDIO_CLICKY
    static const char PROGMEM audio_clicky_status[2][3] = {{0xF4, 0xF5, 0}, {0xF6, 0xF7, 0}};
    oled_write_P(
        audio_clicky_status[userspace_runtime_state.audio.clicky_enable && userspace_runtime_state.audio.enable],
        false);
#        if !defined(OLED_DISPLAY_VERBOSE)
    oled_write_P(PSTR(" "), false);
#        endif
#    endif
#endif

    static const char PROGMEM rgb_layer_status[2][3] = {{0xEE, 0xEF, 0}, {0xF0, 0xF1, 0}};
    oled_write_P(rgb_layer_status[userspace_config.rgb.layer_change], false);
#ifdef COMMUNITY_MODULE_KEYBOARD_LOCK_ENABLE
    static const char PROGMEM cat_mode[3] = {0xF9, 0xFA, 0};
    oled_write_P(cat_mode, get_keyboard_lock());
#endif // COMMUNITY_MODULE_KEYBOARD_LOCK_ENABLE
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
        oled_write_ln(get_u8_str(rgblight_get_val(), ' '), false);
    }
#endif
}

void render_rgb_mode(uint8_t col, uint8_t line) {
    oled_set_cursor(col, line);
#if defined(RGB_MATRIX_ENABLE) || defined(RGBLIGHT_ENABLE)
    static uint8_t mode;
    static char    buf[21] = {0};

#    ifdef RGB_MATRIX_ENABLE
    if (mode != rgb_matrix_get_mode()) {
        snprintf(buf, sizeof(buf), "%-20s", rgb_matrix_get_effect_name());
        mode = rgb_matrix_get_mode();
    }
#    elif RGBLIGHT_ENABLE
    if (mode != rgblight_get_mode()) {
        snprintf(buf, sizeof(buf), "%-20s", rgblight_get_effect_name());
        mode = rgblight_get_mode();
    }
#    endif
    oled_write(buf, false);
#endif
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
 * @param vertical_offset vertical pixel location (from top) to start rendering graph
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
        oled_write_raw_P(pet_animiations[userspace_config.display.oled.pet.index][animation_type][animation_frame][i],
                         OLED_ANIM_SIZE);
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
        if (get_current_wpm() <= userspace_config.display.oled.pet.sleep_speed) {
#endif
            animation_frame     = (animation_frame + 1) % OLED_SLEEP_FRAMES;
            animation_type      = 0;
            anim_frame_duration = 500;
#ifdef WPM_ENABLE
        } else if (get_current_wpm() <= userspace_config.display.oled.pet.kaki_speed) {
            animation_frame     = (animation_frame + 1) % OLED_KAKI_FRAMES;
            animation_type      = 1;
            anim_frame_duration = 500;
        } else if (get_current_wpm() <= userspace_config.display.oled.pet.mati_speed) {
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
    char buf[13] = {0};
    snprintf(buf, sizeof(buf), "%12s", get_unicode_typing_mode_str(userspace_runtime_state.unicode.typing_mode));
    oled_write(buf, false);
#endif
}

void render_unicode_mode_small(uint8_t col, uint8_t line, bool invert) {
#if defined(CUSTOM_UNICODE_ENABLE) && defined(UNICODE_COMMON_ENABLE)
    oled_set_cursor(col, line);
    oled_write_P(PSTR("UC"), invert);
    char buf[13] = {0};
    snprintf(buf, sizeof(buf), "%12s", get_unicode_typing_mode_str(userspace_runtime_state.unicode.typing_mode));
    oled_write(buf, invert);
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

void render_console_output(uint8_t col, uint8_t line) {
    for (uint8_t i = 0; i < DISPLAY_CONSOLE_LOG_LINE_NUM; i++) {
        oled_set_cursor(col, line + i);
        oled_write(logline_ptrs[i], false);
    }
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
    if (timer_elapsed(timer) > 50) {
        frame++;
        if (frame == 10) {
            frame = 0;
        }
        timer = timer_read();
    }
    oled_set_cursor(col, line);
    oled_write_raw_P(train_animation[frame], sizeof(train_animation[0]));
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
        case OS_WINDOWS_UNSURE:
            oled_write_ln_P(PSTR("Windows?"), false);
            break;
        case OS_PS5:
            oled_write_ln_P(PSTR("Sony"), false);
            break;
        case OS_HANDHELD:
            oled_write_ln_P(PSTR("Handheld"), false);
            break;
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
#ifdef COMMUNITY_MODULE_RTC_ENABLE
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
#ifdef COMMUNITY_MODULE_RTC_ENABLE
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

#    if defined(COMMUNITY_MODULE_DISPLAY_MENU_ENABLE)
        if (!oled_render_menu(0, 8, 7, userspace_config.display.menu_render_side))
#    endif
        {
            render_arasaka_logo(0, 8);
            render_wpm_graph(23, 107, 25, 96);
            oled_render_mario(1, 11);
        }
    } else {
        // oled_advance_page(true);
#    if defined(COMMUNITY_MODULE_DISPLAY_MENU_ENABLE)
        if (!oled_render_menu(0, 7, 8, userspace_config.display.menu_render_side))
#    endif
        {
#    ifdef COMMUNITY_MODULE_LAYER_MAP_ENABLE
            oled_set_cursor(1, 7);

            for (uint8_t i = 0; i < LAYER_MAP_ROWS; i++) {
                oled_set_cursor(1 + 1, 7 + i);
                for (uint8_t j = 0; j < LAYER_MAP_COLS; j++) {
                    uint16_t keycode = extract_basic_keycode(layer_map[i][j], NULL, false);

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
            render_autocorrected_info(1, 7);
            render_os(1, 11);
            render_unicode_mode(1, 12);
            oled_render_time(1, 13);
#    endif
        }
    }
}
#endif

__attribute__((weak)) void render_oled_title(bool side) {
    oled_write_P(side ? PSTR("     Left    ") : PSTR("    Right    "), true);
    // oled_write_P(PSTR(    "1234567890123"         "1234567890123"), true);
}

__attribute__((weak)) bool oled_init_keymap(oled_rotation_t *rotation, bool has_run) {
    return true;
}

oled_rotation_t oled_init_user(oled_rotation_t rotation) {
    static bool has_run = false;
    if (!oled_init_keymap(&rotation, has_run)) {
        return rotation;
    }
    rotation = userspace_config.display.oled.rotation;

    if (has_run) {
        oled_set_brightness(userspace_config.display.oled.brightness);
        return rotation;
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

#    ifdef DISPLAY_KEYLOGGER_ENABLE
    if (is_keyboard_left()) {
        oled_set_cursor(4, num_of_rows);
        oled_write(display_keylogger_string, true);
    } else
#    endif // DISPLAY_KEYLOGGER_ENABLE
    {
        render_unicode_mode_small(4, num_of_rows, true);
    }
#endif

    return false;
}

void housekeeping_task_oled(void) {
    is_oled_enabled = oled_screensaver_enabled = false;

    if (is_device_suspended() || is_oled_force_off) {
        is_oled_enabled = false;
    } else if (userspace_config.display.oled.screen_lock) {
        is_oled_enabled = true;
    } else if (last_input_activity_elapsed() < (10 * 60 * 1000)) {
        is_oled_enabled = true;
    }
    if (is_oled_enabled && last_input_activity_elapsed() > (1 * 60 * 1000)) {
        oled_screensaver_enabled = true;
    }

    if (oled_get_brightness() != userspace_config.display.oled.brightness) {
        oled_set_brightness(userspace_config.display.oled.brightness);
    }
}

void keyboard_post_init_oled(void) {
    oled_invert(userspace_config.display.oled.inverted);
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

void oled_write_compressed(compressed_oled_frame_t frame) {
    uint16_t block_index = 0;
    for (uint16_t i = 0; i < frame.data_len; i++) {
        uint8_t bit          = i % 8;
        uint8_t map_index    = i / 8;
        uint8_t _block_map   = frame.block_map[map_index];
        uint8_t nonzero_byte = (_block_map & (1 << bit));
        if (nonzero_byte) {
            const char data = frame.block_list[block_index++];
            oled_write_raw_byte(data, i);
        } else {
            const char data = (const char)0x00;
            oled_write_raw_byte(data, i);
        }
    }
}
#ifndef pgm_read_byte_near
#    define pgm_read_byte_near(addr) pgm_read_byte(addr)
#endif

void oled_write_compressed_P(compressed_oled_frame_t frame) {
    uint16_t block_index = 0;
    for (uint16_t i = 0; i < frame.data_len; i++) {
        uint8_t bit          = i % 8;
        uint8_t map_index    = i / 8;
        uint8_t _block_map   = (uint8_t)pgm_read_byte_near(frame.block_map + map_index);
        uint8_t nonzero_byte = (_block_map & (1 << bit));
        if (nonzero_byte) {
            const char data = (const char)pgm_read_byte_near(frame.block_list + block_index++);
            oled_write_raw_byte(data, i);
        } else {
            const char data = (const char)0x00;
            oled_write_raw_byte(data, i);
        }
    }
}
/**
 * @brief Re-initializes the OLED display with the specified rotation and inversion settings.
 *
 * This function is called after the OLED display has been initialized. It sets the rotation
 * and inversion settings based on the user's configuration.
 *
 * @note If DISPLAY_FULL_ROTATION_ENABLE is defined, the rotation is set directly from the
 *       user's configuration. Otherwise, the rotation is set to either 0 or 180 degrees
 *       based on the user's configuration.
 */

void oled_post_init(void) {
#if defined(DISPLAY_FULL_ROTATION_ENABLE)
    oled_init(userspace_config.display.rotation);
#else  // DISPLAY_FULL_ROTATION_ENABLE
    oled_init(userspace_config.display.oled.rotation ? OLED_ROTATION_180 : OLED_ROTATION_0);
#endif // DISPLAY_FULL_ROTATION_ENABLE
    oled_invert(userspace_config.display.oled.inverted);
}

/**
 * @brief Increases the OLED display brightness by a predefined step.
 *
 * This function increments the current OLED brightness by a constant value
 * defined by OLED_BRIGHTNESS_STEP. It then updates the OLED display with the
 * new brightness value and saves the updated configuration to persistent storage.
 */
void oled_brightness_increase_step(void) {
    userspace_config.display.oled.brightness = qadd8(userspace_config.display.oled.brightness, OLED_BRIGHTNESS_STEP);
    oled_set_brightness(userspace_config.display.oled.brightness);
    eeconfig_update_user_datablock(&userspace_config, 0, EECONFIG_USER_DATA_SIZE);
}

/**
 * @brief Decreases the OLED display brightness by a predefined step.
 *
 * This function reduces the current brightness level of the OLED display
 * by a constant value defined by `OLED_BRIGHTNESS_STEP`. It then updates
 * the display brightness and saves the new configuration to persistent storage.
 */
void oled_brightness_decrease_step(void) {
    userspace_config.display.oled.brightness = qsub8(userspace_config.display.oled.brightness, OLED_BRIGHTNESS_STEP);
    oled_set_brightness(userspace_config.display.oled.brightness);
    eeconfig_update_user_datablock(&userspace_config, 0, EECONFIG_USER_DATA_SIZE);
}
