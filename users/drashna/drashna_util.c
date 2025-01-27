// Copyright 2024 Christopher Courtney, aka Drashna Jael're  (@drashna) <drashna@live.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "drashna_runtime.h"
#include "drashna_util.h"
#include "action_util.h"
#include "quantum_keycodes.h"

void        tap_code16(uint16_t code);
void        tap_code16_delay(uint16_t code, uint16_t delay);
static bool l_is_suspended = false;

/**
 * @brief Handle registering a keycode, with optional modifier based on timed event
 *
 * @param code keycode to send to host
 * @param mod_code modifier to send with code, if held for tapping term or longer
 * @param pressed the press/release event (can use "record->event.pressed" for this)
 * @return true exits function
 * @return false exits function
 */
bool mod_key_press_timer(uint16_t code, uint16_t mod_code, bool pressed) {
    static uint16_t this_timer = 0;
    mod_key_press(code, mod_code, pressed, &this_timer);
    return false;
}

/**
 * @brief Handle registration of keycode, with optional modifier based on custom timer
 *
 * @param code keycode to send to host
 * @param mod_code modifier keycode to send with code, if held for tapping term or longer
 * @param pressed the press/release event
 * @param this_timer custom timer to use
 * @return true
 * @return false
 */
bool mod_key_press(uint16_t code, uint16_t mod_code, bool pressed, uint16_t *this_timer) {
    if (pressed) {
        *this_timer = timer_read();
    } else {
        if (timer_elapsed(*this_timer) < TAPPING_TERM) {
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
 * @param value the modifier variable (get_mods/get_oneshot_mods/get_weak_mods)
 * @param mask the modifier mask to check for
 * @return true Has the exact modifiers specified
 * @return false Does not have the exact modifiers specified
 */
bool hasAllBitsInMask(uint8_t value, uint8_t mask) {
    value &= 0xF;
    mask &= 0xF;

    return (value & mask) == mask;
}

/** \brief mod_config_8bit
 *
 *  This function checks the mods passed to it against the bootmagic config,
 *  and will remove or replace mods, based on that.
 */

__attribute__((weak)) uint8_t mod_config_8bit(uint8_t mod) {
    /**
     * Note: This function is for the full 8-bit mods, NOT the 5-bit packed mods.
     * More info about the mods can be seen in modifiers.h.
     */
    if (keymap_config.swap_lalt_lgui) {
        /** If both modifiers pressed or neither pressed, do nothing
         * Otherwise swap the values
         */
        if (((mod & MOD_BIT(KC_LALT)) && !(mod & MOD_BIT(KC_LGUI))) ||
            (!(mod & MOD_BIT(KC_LALT)) && (mod & MOD_BIT(KC_LGUI)))) {
            mod ^= (MOD_BIT(KC_LALT) | MOD_BIT(KC_LGUI));
        }
    }
    if (keymap_config.swap_ralt_rgui) {
        if (((mod & MOD_BIT(KC_RALT)) && !(mod & MOD_BIT(KC_RGUI))) ||
            (!(mod & MOD_BIT(KC_RALT)) && (mod & MOD_BIT(KC_RGUI)))) {
            mod ^= (MOD_BIT(KC_RALT) | MOD_BIT(KC_RGUI));
        }
    }
    if (keymap_config.swap_lctl_lgui) {
        if (((mod & MOD_BIT(KC_LCTL)) && !(mod & MOD_BIT(KC_LGUI))) ||
            (!(mod & MOD_BIT(KC_LCTL)) && (mod & MOD_BIT(KC_LGUI)))) {
            mod ^= (MOD_BIT(KC_LCTL) | MOD_BIT(KC_LGUI));
        }
    }
    if (keymap_config.swap_rctl_rgui) {
        if (((mod & MOD_BIT(KC_RCTL)) && !(mod & MOD_BIT(KC_RGUI))) ||
            (!(mod & MOD_BIT(KC_RCTL)) && (mod & MOD_BIT(KC_RGUI)))) {
            mod ^= (MOD_BIT(KC_RCTL) | MOD_BIT(KC_RGUI));
        }
    }
    if (keymap_config.no_gui) {
        mod &= ~MOD_BIT(KC_LGUI);
        mod &= ~MOD_BIT(KC_RGUI);
    }

    return mod;
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

/**
 * @brief hack for snprintf warning
 *
 */
#define snprintf_nowarn(...) (snprintf(__VA_ARGS__) < 0 ? abort() : (void)0)

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
        output[width] = '\0';
        return;
    }

    /* Try to center the TEXT, TODO: Handle Even lengths*/
    uint8_t padlen_l = (width - strlen(text)) / 2;
    uint8_t padlen_r = (padlen_l * 2) + strlen(text) == width ? padlen_l : padlen_l + 1;
    snprintf_nowarn(output, width + 1, "%*s%s%*s", padlen_l, " ", text, padlen_r, " ");
    output[width] = '\0';
}

/**
 * @brief Checks to see if device is suspended
 *
 * @return true
 * @return false
 */
bool is_device_suspended(void) {
    return l_is_suspended;
}

/**
 * @brief Sets the local variable for device suspended
 *
 * @param status
 */
void set_is_device_suspended(bool status) {
#if defined(SPLIT_KEYBOARD) && defined(SPLIT_TRANSACTION_IDS_USER)
    void send_device_suspend_state(bool status);

    send_device_suspend_state(status);
    l_is_suspended = status;
#endif
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

uint16_t extract_non_basic_keycode(uint16_t keycode, keyrecord_t *record, bool check_hold) {
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
        keycode =
            (mod_config(QK_MODS_GET_MODS(keycode)) & 0x1F) << 8 | (keycode_config(QK_MODS_GET_BASIC_KEYCODE(keycode)));
    } else if (IS_QK_ONE_SHOT_MOD(keycode)) {
        keycode = keycode_config(0xE0 + biton(QK_ONE_SHOT_MOD_GET_MODS(keycode) & 0xF) +
                                 biton(QK_ONE_SHOT_MOD_GET_MODS(keycode) & 0x10));
    } else if (IS_QK_BASIC(keycode)) {
        keycode = keycode_config(keycode);
    }

    return keycode;
}

/**
 * @brief shim to work with different versions of the function until PR is merged and lands in master
 *
 * @param data data to be written
 * @param offset offset to write to
 * @param size size of data to be written
 * @return uint32_t
 */
uint32_t eeconfig_update_user_datablock_handler(const void *data, uint8_t offset, uint8_t size) {
    return eeconfig_update_user_datablock(data, offset, size);
}

/**
 * @brief shim to work with different versions of the function until PR is merged and lands in master
 *
 * @param data data to be read into
 * @param offset offset to start read from
 * @param size size of data to be read
 */
uint32_t eeconfig_read_user_datablock_handler(void *data, uint8_t offset, uint8_t size) {
    return eeconfig_read_user_datablock(data, offset, size);
}
