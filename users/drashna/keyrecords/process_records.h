// Copyright 2020 Christopher Courtney, aka Drashna Jael're  (@drashna) <drashna@live.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "action.h"

#if __has_include("keymap.h")
#    include "keymap.h"
#else
enum userspace_custom_keycodes {
    VRSN = QK_USER,  // Prints QMK Firmware and board info
    KC_DIABLO_CLEAR, // Clears all Diablo Timers
    KC_RGB_T,        // Toggles RGB Layer Indication mode
    RGB_IDL,         // RGB Idling animations
    KC_SECRET_1,     // test1
    KC_SECRET_2,     // test2
    KC_SECRET_3,     // test3
    KC_SECRET_4,     // test4
    KC_SECRET_5,     // test5
    KC_SECRET_6,     // test6
    KC_NUKE,         // NUCLEAR LAUNCH DETECTED!!!
    UC_FLIP,         // (ಠ痊ಠ)┻━┻
    UC_TABL,         // ┬─┬ノ( º _ ºノ)
    UC_SHRG,         // ¯\_(ツ)_/¯
    UC_DISA,         // ಠ_ಠ
    UC_IRNY,
    UC_CLUE,

    OLED_LOCK,
    OLED_BRIGHTNESS_INC,
    OLED_BRIGHTNESS_DEC,
    OLED_ROTATE_CW,
    OLED_ROTATE_CCW,

    STORE_SETUPS,
    PRINT_SETUPS,

    US_MATRIX_SCAN_RATE_PRINT,

    US_SELECT_WORD,

    LAYER_LOCK,

    US_I2C_SCAN_ENABLE,
    US_GAMING_SCAN_TOGGLE,
    USER_SAFE_RANGE,
};

#    define KC_SEC1 KC_SECRET_1
#    define KC_SEC2 KC_SECRET_2
#    define KC_SEC3 KC_SECRET_3
#    define KC_SEC4 KC_SECRET_4
#    define KC_SEC5 KC_SECRET_5
#    define KC_SEC6 KC_SECRET_6

#    define OL_LOCK OLED_LOCK
#    define OL_BINC OLED_BRIGHTNESS_INC
#    define OL_BDEC OLED_BRIGHTNESS_DEC
#    define OL_CW   OLED_ROTATE_CW
#    define OL_CCW  OLED_ROTATE_CCW

#    define US_MSRP US_MATRIX_SCAN_RATE_PRINT
#    define US_SELW US_SELECT_WORD

#endif // _hash_include("keymap.h")

bool process_record_secrets(uint16_t keycode, keyrecord_t *record);
bool pre_process_record_keymap(uint16_t keycode, keyrecord_t *record);
bool process_record_keymap(uint16_t keycode, keyrecord_t *record);
void post_process_record_keymap(uint16_t keycode, keyrecord_t *record);
#ifdef CUSTOM_UNICODE_ENABLE
bool process_record_unicode(uint16_t keycode, keyrecord_t *record);
#endif // CUSTOM_UNICODE_ENABLE
void rgb_layer_indication_toggle(void);

#define LOWER   MO(_LOWER)
#define RAISE   MO(_RAISE)
#define ADJUST  MO(_ADJUST)
#define TG_MODS OS_TOGG
#define TG_GAME TG(_GAMEPAD)
#define TG_DBLO TG(_DIABLO)
#define OS_LWR  OSL(_LOWER)
#define OS_RSE  OSL(_RAISE)

#define QWERTY  PDF(_QWERTY)
#define CLMKDH  PDF(_COLEMAK_DH)
#define COLEMAK PDF(_COLEMAK)
#define DVORAK  PDF(_DVORAK)

#ifdef SWAP_HANDS_ENABLE
#    define KC_C1R3 SH_T(KC_TAB)
#elif defined(DRASHNA_LP)
#    define KC_C1R3 TG(_GAMEPAD)
#else // SWAP_HANDS_ENABLE
#    define KC_C1R3 LALT_T(KC_TAB)
#endif // SWAP_HANDS_ENABLE

#define BK_LWER LT(_LOWER, KC_BSPC)
#define SP_LWER LT(_LOWER, KC_SPC)
#define DL_RAIS LT(_RAISE, KC_DEL)
#define ET_RAIS LT(_RAISE, KC_ENTER)

/* OSM keycodes, to keep things clean and easy to change */
#define KC_MLSF OSM(MOD_LSFT)
#define KC_MRSF OSM(MOD_RSFT)

#define ALT_APP ALT_T(KC_APP)

/*
Custom Keycodes for Diablo 3 layer
But since TD() doesn't work when tap dance is disabled
We use custom codes here, so we can substitute the right stuff
*/
#ifdef CUSTOM_TAP_DANCE_ENABLE
#    define KC_D3_1 TD(TD_D3_1)
#    define KC_D3_2 TD(TD_D3_2)
#    define KC_D3_3 TD(TD_D3_3)
#    define KC_D3_4 TD(TD_D3_4)
#else // CUSTOM_TAP_DANCE_ENABLE
#    define KC_D3_1 KC_1
#    define KC_D3_2 KC_2
#    define KC_D3_3 KC_3
#    define KC_D3_4 KC_4
#endif // CUSTOM_TAP_DANCE_ENABLE

#ifndef COMMUNITY_MODULE_DISPLAY_MENU_ENABLE
#    define DISPLAY_MENU KC_NO
#endif // COMMUNITY_MODULE_DISPLAY_MENU_ENABLE
#ifndef COMMUNITY_MODULE_KEYBOARD_LOCK_ENABLE
#    define KEYLOCK KC_NO
#endif // COMMUNITY_MODULE_KEYBOARD_LOCK_ENABLE
#ifndef COMMUNITY_MODULE_UNICODE_TYPING_ENABLE
#    define KC_NOMODE   KC_NO
#    define KC_WIDE     KC_NO
#    define KC_SCRIPT   KC_NO
#    define KC_BLOCKS   KC_NO
#    define KC_REGIONAL KC_NO
#    define KC_AUSSIE   KC_NO
#    define KC_ZALGO    KC_NO
#    define KC_SUPER    KC_NO
#    define KC_COMIC    KC_NO
#endif // COMMUNITY_MODULE_UNICODE_TYPING_ENABLE
