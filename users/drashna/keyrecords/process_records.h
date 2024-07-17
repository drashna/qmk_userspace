// Copyright 2020 Christopher Courtney, aka Drashna Jael're  (@drashna) <drashna@live.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once
#include "drashna.h"
#ifdef ACHORDION_ENABLE
#    include "keyrecords/achordion.h"
#endif
#ifdef CUSTOM_KEY_CANCELLATION_ENABLE
#    include "keyrecords/custom_key_cancellation.h"
#endif
#ifdef CUSTOM_TAP_DANCE_ENABLE
#    include "keyrecords/custom_tap_dance.h"
#endif // CUSTOM_TAP_DANCE_ENABLE
#ifdef CUSTOM_SHIFT_KEYS_ENABLE
#    include "keyrecords/custom_shift_keys.h"
#endif
#ifdef SELECT_WORD_ENABLE
#    include "keyrecords/select_word.h"
#endif
#ifdef SENTENCE_CASE_ENABLE
#    include "keyrecords/sentence_case.h"
#endif
#ifdef UNICODE_COMMON_ENABLE
#    include "keyrecords/unicode.h"
#endif

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
    KC_CCCV,         // Hold to copy, tap to paste
    KC_NUKE,         // NUCLEAR LAUNCH DETECTED!!!
    UC_FLIP,         // (ಠ痊ಠ)┻━┻
    UC_TABL,         // ┬─┬ノ( º _ ºノ)
    UC_SHRG,         // ¯\_(ツ)_/¯
    UC_DISA,         // ಠ_ಠ
    UC_IRNY,
    UC_CLUE,
    KEYLOCK, // Locks keyboard by unmounting driver
    KC_NOMODE,
    KC_WIDE,
    KC_SCRIPT,
    KC_BLOCKS,
    KC_REGIONAL,
    KC_AUSSIE,
    KC_ZALGO,
    KC_SUPER,
    KC_COMIC,
    KC_ACCEL,

    OLED_LOCK,
    OLED_BRIGHTNESS_INC,
    OLED_BRIGHTNESS_DEC,
    OLED_ROTATE_CW,
    OLED_ROTATE_CCW,

    STORE_SETUPS,
    PRINT_SETUPS,

    PD_JIGGLER,

    DYN_MACRO_PROG,
    DYN_MACRO_KEY00,
    DYN_MACRO_KEY01,
    DYN_MACRO_KEY02,
    DYN_MACRO_KEY03,
    DYN_MACRO_KEY04,
    DYN_MACRO_KEY05,
    DYN_MACRO_KEY06,
    DYN_MACRO_KEY07,
    DYN_MACRO_KEY08,
    DYN_MACRO_KEY09,
    DYN_MACRO_KEY10,
    DYN_MACRO_KEY11,
    DYN_MACRO_KEY12,
    DYN_MACRO_KEY13,
    DYN_MACRO_KEY14,
    DYN_MACRO_KEY15,

    US_MATRIX_SCAN_RATE_PRINT,

    US_SELECT_WORD,

    OM_DBLS,
    OM_SEL1,
    OM_SEL2,
    OM_SEL3,
    OM_SEL4,
    OM_SEL5,
    OM_SEL6,
    OM_SEL7,
    OM_SEL8,

    LAYER_LOCK,

    USER_SAFE_RANGE,
};

bool process_record_secrets(uint16_t keycode, keyrecord_t *record);
bool pre_process_record_keymap(uint16_t keycode, keyrecord_t *record);
bool process_record_keymap(uint16_t keycode, keyrecord_t *record);
void post_process_record_keymap(uint16_t keycode, keyrecord_t *record);
#ifdef CUSTOM_UNICODE_ENABLE
bool process_record_unicode(uint16_t keycode, keyrecord_t *record);
#endif

#define LOWER   MO(_LOWER)
#define RAISE   MO(_RAISE)
#define ADJUST  MO(_ADJUST)
#define TG_MODS OS_TOGG
#define TG_GAME TG(_GAMEPAD)
#define TG_DBLO TG(_DIABLO)
#define OS_LWR  OSL(_LOWER)
#define OS_RSE  OSL(_RAISE)

#define KC_SEC1 KC_SECRET_1
#define KC_SEC2 KC_SECRET_2
#define KC_SEC3 KC_SECRET_3
#define KC_SEC4 KC_SECRET_4
#define KC_SEC5 KC_SECRET_5

#define KC_QWERTY     DF(_QWERTY)
#define KC_COLEMAK_DH DF(_COLEMAK_DH)
#define KC_COLEMAK    DF(_COLEMAK)
#define KC_DVORAK     DF(_DVORAK)

#define QWERTY  KC_QWERTY
#define DVORAK  KC_DVORAK
#define COLEMAK KC_COLEMAK
#define CLMKDH  KC_COLEMAK_DH

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

#define OS_LGUI OSM(MOD_LGUI)
#define OS_RGUI OSM(MOD_RGUI)
#define OS_LSFT OSM(MOD_LSFT)
#define OS_RSFT OSM(MOD_RSFT)
#define OS_LCTL OSM(MOD_LCTL)
#define OS_RCTL OSM(MOD_RCTL)
#define OS_LALT OSM(MOD_LALT)
#define OS_RALT OSM(MOD_RALT)
#define OS_MEH  OSM(MOD_MEH)
#define OS_HYPR OSM(MOD_HYPR)

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

#define OL_LOCK OLED_LOCK
#define OL_BINC OLED_BRIGHTNESS_INC
#define OL_BDEC OLED_BRIGHTNESS_DEC
#define OL_CW   OLED_ROTATE_CW
#define OL_CCW  OLED_ROTATE_CCW

#define US_MSRP US_MATRIX_SCAN_RATE_PRINT
#define US_SELW US_SELECT_WORD
#define PD_JIGG PD_JIGGLER

#define OM_U    KC_MS_U
#define OM_D    KC_MS_D
#define OM_L    KC_MS_L
#define OM_R    KC_MS_R
#define OM_W_U  KC_WH_U
#define OM_W_D  KC_WH_D
#define OM_W_L  KC_WH_L
#define OM_W_R  KC_WH_R
#define OM_BTN1 KC_BTN1
#define OM_BTN2 KC_BTN2
#define OM_BTN3 KC_BTN3
#define OM_BTN4 KC_BTN4
#define OM_BTN5 KC_BTN5
#define OM_BTN6 KC_BTN6
#define OM_BTN7 KC_BTN7
#define OM_BTN8 KC_BTN8
#define OM_BTNS KC_ACL0
#define OM_HLDS KC_ACL1
#define OM_RELS KC_ACL2
