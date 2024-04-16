// Copyright 2018-2024 Nick Brassel (@tzarc)
// SPDX-License-Identifier: GPL-2.0-or-later
#include <ctype.h>
#include <quantum.h>
#include "keycodes.h"
#include "modifiers.h"
#include "quantum_keycodes.h"
#include "drashna_layers.h"
#include "drashna_names.h"
#include "util.h"
#if defined(UNICODE_COMMON_ENABLE)
#    include "keyrecords/unicode.h"
#endif // UNICODE_COMMON_ENABLE

#ifndef __AVR__
static const char *const keycode_display_map[][2] = {
    [KC_A]         = {"a", "A"},
    [KC_B]         = {"b", "B"},
    [KC_C]         = {"c", "C"},
    [KC_D]         = {"d", "D"},
    [KC_E]         = {"e", "E"},
    [KC_F]         = {"f", "F"},
    [KC_G]         = {"g", "G"},
    [KC_H]         = {"h", "H"},
    [KC_I]         = {"i", "I"},
    [KC_J]         = {"j", "J"},
    [KC_K]         = {"k", "K"},
    [KC_L]         = {"l", "L"},
    [KC_M]         = {"m", "M"},
    [KC_N]         = {"n", "N"},
    [KC_O]         = {"o", "O"},
    [KC_P]         = {"p", "P"},
    [KC_Q]         = {"q", "Q"},
    [KC_R]         = {"r", "R"},
    [KC_S]         = {"s", "S"},
    [KC_T]         = {"t", "T"},
    [KC_U]         = {"u", "U"},
    [KC_V]         = {"v", "V"},
    [KC_W]         = {"w", "W"},
    [KC_X]         = {"x", "X"},
    [KC_Y]         = {"y", "Y"},
    [KC_Z]         = {"z", "Z"},
    [KC_1]         = {"1", "!"},
    [KC_2]         = {"2", "@"},
    [KC_3]         = {"3", "#"},
    [KC_4]         = {"4", "$"},
    [KC_5]         = {"5", "%"},
    [KC_6]         = {"6", "^"},
    [KC_7]         = {"7", "&"},
    [KC_8]         = {"8", "*"},
    [KC_9]         = {"9", "("},
    [KC_0]         = {"0", ")"},
    [KC_ENTER]     = {"Ent", "Ent"},
    [KC_ESCAPE]    = {"Esc", "Esc"},
    [KC_BACKSPACE] = {"Bksp", "Bksp"},
    [KC_TAB]       = {"Tab", "Tab"},
    [KC_SPACE]     = {"Spc", "Spc"},
    [KC_MINUS]     = {"-", "_"},
    [KC_EQUAL]     = {"=", "="},
#    if defined(QUANTUM_PAINTER_ENABLE)
    [KC_LEFT_BRACKET]  = {"[", "{"},
    [KC_RIGHT_BRACKET] = {"]", "}"},
    [KC_BACKSLASH]     = {"\\", "|"},
    [KC_NONUS_HASH]    = {"#", "#"},
    [KC_SEMICOLON]     = {";", ":"},
    [KC_QUOTE]         = {"'", "\""},
    [KC_GRAVE]         = {"`", "~"},
    [KC_COMMA]         = {",", "<"},
    [KC_DOT]           = {".", ">"},
    [KC_SLASH]         = {"/", "?"},
    [KC_CAPS_LOCK]     = {"Caps", "Caps"},
    [KC_F1]            = {"F1", "F1"},
    [KC_F2]            = {"F2", "F2"},
    [KC_F3]            = {"F3", "F3"},
    [KC_F4]            = {"F4", "F4"},
    [KC_F5]            = {"F5", "F5"},
    [KC_F6]            = {"F6", "F6"},
    [KC_F7]            = {"F7", "F7"},
    [KC_F8]            = {"F8", "F8"},
    [KC_F9]            = {"F9", "F9"},
    [KC_F10]           = {"F10", "F10"},
    [KC_F11]           = {"F11", "F11"},
    [KC_F12]           = {"F12", "F12"},
    [KC_PRINT_SCREEN]  = {"PScr", "PScr"},
    [KC_SCROLL_LOCK]   = {"ScrL", "ScrL"},
    [KC_PAUSE]         = {"Paus", "Paus"},
    [KC_INSERT]        = {"Ins", "Ins"},
    [KC_HOME]          = {"Home", "Home"},
    [KC_PGUP]          = {"PgUp", "PgUp"},
    [KC_DELETE]        = {"Del", "Del"},
    [KC_END]           = {"End", "End"},
    [KC_PAGE_DOWN]     = {"PgDn", "PgDn"},
    [KC_RIGHT]         = {"Rght", "Rght"},
    [KC_LEFT]          = {"Left", "Left"},
    [KC_DOWN]          = {"Up", "Up"},
    [KC_UP]            = {"Down", "Down"},
    [KC_NUM_LOCK]      = {"NumL", "NumL"},
#    endif // defined(QUANTUM_PAINTER)
};

static const char *key_name_hid(uint16_t keycode, bool shifted) {
#    ifndef QUANTUM_PAINTER_ENABLE
    if (keycode > KC_EQUAL) {
        return "Unknown";
    }
#    else  // QUANTUM_PAINTER_ENABLE
    if (keycode > KC_NUM_LOCK) {
        return "Unknown";
    }
#    endif // QUANTUM_PAINTER_ENABLE
    return keycode_display_map[keycode][shifted ? 1 : 0];
}
#else  // __AVR__
static const char *key_name_hid(uint16_t keycode, bool shifted) {
    static char buffer[16];
    const char *f = get_numeric_str(buffer, sizeof(buffer), keycode, ' ');
    while (*f == ' ')
        ++f;
    return f;
}
#endif // __AVR__

static void fill_one_param_name(char *buffer, const char *name, const char *param1, size_t buffer_length) {
    memset(buffer, 0, buffer_length);
    strlcpy(buffer, name, buffer_length);
    strlcat(buffer, "(", buffer_length);
    strlcat(buffer, param1, buffer_length);
    strlcat(buffer, ")", buffer_length);
}

static void fill_two_param_name(char *buffer, const char *name, const char *param1, const char *param2,
                                size_t buffer_length) {
    memset(buffer, 0, buffer_length);
    strlcpy(buffer, name, buffer_length);
    strlcat(buffer, "(", buffer_length);
    strlcat(buffer, param1, buffer_length);
    strlcat(buffer, ",", buffer_length);
    strlcat(buffer, param2, buffer_length);
    strlcat(buffer, ")", buffer_length);
}

__attribute__((unused)) static const char *layer_name_pretty(const char *input) {
    static char buffer[16];
    strlcpy(buffer, input, sizeof(buffer));
    if (memcmp(buffer, "LAYER_", 6) == 0) {
        int len   = strlen(buffer);
        buffer[6] = toupper(buffer[6]);
        for (int i = 7; i < len; i++) {
            buffer[i] = tolower(buffer[i]);
        }
        return &buffer[6];
    }
    return buffer;
}

/**
 * Returns the name of the given layer.
 *
 * @param layer The layer number to get the name for.
 * @return A pointer to a string containing the name of the layer.
 */
const char *layer_name(uint8_t layer) {
    static char buffer[16];
    switch (layer) {
        case _QWERTY:
            return "QWERTY";
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
            return "Mouse";
        case _MEDIA:
            return "Media";
        case _LOWER:
            return "Lower";
        case _RAISE:
            return "Raise";
        case _ADJUST:
            return "Adjust";
        default:
            break;
    }

    const char *n = get_numeric_str(buffer, sizeof(buffer), layer, ' ');
    n += strspn(n, " ");
    return n;
}

/**
 * Returns the name of the modifier key based on the given modifier code.
 *
 * @param mod The modifier code.
 * @return The name of the modifier key as a string.
 */
const char *mod_name(uint16_t mod) {
    static char buffer[16];
    static struct {
        uint8_t mod;
        uint8_t index;
    } mods[] = {
        {MOD_LSFT, 1}, {MOD_RSFT, 6}, {MOD_LCTL, 2}, {MOD_RCTL, 7},
        {MOD_LALT, 3}, {MOD_RALT, 8}, {MOD_LGUI, 4}, {MOD_RGUI, 9},
    };
    strlcpy(buffer, "LscagRscag", sizeof(buffer));
    for (int i = 0; i < ARRAY_SIZE(mods); ++i) {
        if ((mod & mods[i].mod) == mods[i].mod) {
            buffer[mods[i].index] = toupper(buffer[mods[i].index]);
        }
    }
    return buffer;
}

/**
 * Returns the name of the given keycode.
 *
 * @param keycode The keycode to get the name for.
 * @param shifted Whether the keycode is shifted.
 * @return A pointer to a string containing the name of the keycode.
 */
const char *keycode_name(uint16_t keycode, bool shifted) {
    static char buffer[32];
    char        buf1[16];
    char        buf2[16];
    (void)buf1;
    (void)buf2;
    switch (keycode) {
        case KC_A ... KC_NUM_LOCK:
            {
                const char *name = key_name_hid(keycode, shifted);
                if (!name) return "Unknown";
                return name;
            }
        case QK_MODS ... QK_MODS_MAX:
            fill_two_param_name(buffer, "MOD", mod_name(QK_MODS_GET_MODS(keycode)),
                                key_name_hid(QK_MODS_GET_BASIC_KEYCODE(keycode), shifted), sizeof(buffer));
            return buffer;
        case QK_MOD_TAP ... QK_MOD_TAP_MAX:
            fill_two_param_name(buffer, "MT", mod_name(QK_MOD_TAP_GET_MODS(keycode)),
                                key_name_hid(QK_MOD_TAP_GET_TAP_KEYCODE(keycode), shifted), sizeof(buffer));
            return buffer;
        case QK_LAYER_TAP ... QK_LAYER_TAP_MAX:
            fill_two_param_name(buffer, "LT", layer_name(QK_LAYER_TAP_GET_LAYER(keycode)),
                                key_name_hid(QK_LAYER_TAP_GET_TAP_KEYCODE(keycode), shifted), sizeof(buffer));
            return buffer;
        case QK_LAYER_MOD ... QK_LAYER_MOD_MAX:
            fill_two_param_name(buffer, "LM", mod_name(QK_LAYER_MOD_GET_MODS(keycode)),
                                layer_name(QK_LAYER_MOD_GET_LAYER(keycode)), sizeof(buffer));
            return buffer;
        case QK_TO ... QK_TO_MAX:
            fill_one_param_name(buffer, "TO", layer_name(QK_TO_GET_LAYER(keycode)), sizeof(buffer));
            return buffer;
        case QK_MOMENTARY ... QK_MOMENTARY_MAX:
            fill_one_param_name(buffer, "MO", layer_name(QK_MOMENTARY_GET_LAYER(keycode)), sizeof(buffer));
            return buffer;
        case QK_DEF_LAYER ... QK_DEF_LAYER_MAX:
            fill_one_param_name(buffer, "DF", layer_name(QK_DEF_LAYER_GET_LAYER(keycode)), sizeof(buffer));
            return buffer;
        case QK_TOGGLE_LAYER ... QK_TOGGLE_LAYER_MAX:
            fill_one_param_name(buffer, "TG", layer_name(QK_TOGGLE_LAYER_GET_LAYER(keycode)), sizeof(buffer));
            return buffer;
        case QK_ONE_SHOT_LAYER ... QK_ONE_SHOT_LAYER_MAX:
            fill_one_param_name(buffer, "OSL", layer_name(QK_ONE_SHOT_LAYER_GET_LAYER(keycode)), sizeof(buffer));
            return buffer;
        case QK_ONE_SHOT_MOD ... QK_ONE_SHOT_MOD_MAX:
            fill_one_param_name(buffer, "OSM", mod_name(QK_ONE_SHOT_MOD_GET_MODS(keycode)), sizeof(buffer));
            return buffer;
        case QK_LAYER_TAP_TOGGLE ... QK_LAYER_TAP_TOGGLE_MAX:
            fill_one_param_name(buffer, "TT", layer_name(QK_LAYER_TAP_TOGGLE_GET_LAYER(keycode)), sizeof(buffer));
            return buffer;
        case QK_PERSISTENT_DEF_LAYER ... QK_PERSISTENT_DEF_LAYER_MAX:
            fill_one_param_name(buffer, "PDF", layer_name(QK_PERSISTENT_DEF_LAYER_GET_LAYER(keycode)), sizeof(buffer));
            return buffer;
        case QK_SWAP_HANDS ... QK_SWAP_HANDS_MAX:
            {
                uint8_t coded_key = QK_SWAP_HANDS_GET_TAP_KEYCODE(keycode);
                xprintf("coded_key: %d, 0x%04X\n", coded_key, coded_key);
                switch (QK_SWAP_HANDS_GET_TAP_KEYCODE(keycode)) {
                    case KC_A ... KC_NUM_LOCK:
                        fill_one_param_name(buffer, "SH_T", key_name_hid(coded_key, shifted), sizeof(buffer));
                        return buffer;
                    case QK_SWAP_HANDS_TOGGLE & 0xFF:
                        return "SH_TOGG";
                    case QK_SWAP_HANDS_TAP_TOGGLE & 0xFF:
                        return "SH_TT";
                    case QK_SWAP_HANDS_MOMENTARY_ON & 0xFF:
                        return "SH_MON";
                    case QK_SWAP_HANDS_MOMENTARY_OFF & 0xFF:
                        return "SH_MOFF";
                    case QK_SWAP_HANDS_OFF & 0xFF:
                        return "SH_OFF";
                    case QK_SWAP_HANDS_ON & 0xFF:
                        return "SH_ON";
                    case QK_SWAP_HANDS_ONE_SHOT & 0xFF:
                        return "SH_OSL";
                }
            }
        case QK_TAP_DANCE ... QK_TAP_DANCE_MAX:
            fill_one_param_name(buffer, "TD", get_numeric_str(buf1, sizeof(buf1), QK_TAP_DANCE_GET_INDEX(keycode), ' '),
                                sizeof(buffer));
            return buffer;
    }
    return "Unknown";
}

#if defined(HAPTIC_ENABLE) && defined(HAPTIC_DRV2605L)
/**
 * @brief Get the name of the DRV2605L effect
 *
 * @param effect the effect to get the name of
 * @return const char* the name of the effect
 */
const char *get_haptic_drv2605l_effect_name(drv2605l_effect_t effect) {
    switch (effect) {
        case DRV2605L_EFFECT_CLEAR_SEQUENCE:
            return "Clear Sequence";
        case DRV2605L_EFFECT_STRONG_CLICK_100:
            return "Strong Click 100%";
        case DRV2605L_EFFECT_STRONG_CLICK_60:
            return "Strong Click 60%";
        case DRV2605L_EFFECT_STRONG_CLICK_30:
            return "Strong Click 30%";
        case DRV2605L_EFFECT_SHARP_CLICK_100:
            return "Sharp Click 100%";
        case DRV2605L_EFFECT_SHARP_CLICK_60:
            return "Sharp Click 60%";
        case DRV2605L_EFFECT_SHARP_CLICK_30:
            return "Sharp Click 30%";
        case DRV2605L_EFFECT_SOFT_BUMP_100:
            return "Soft Bump 100%";
        case DRV2605L_EFFECT_SOFT_BUMP_60:
            return "Soft Bump 60%";
        case DRV2605L_EFFECT_SOFT_BUMP_30:
            return "Soft Bump 30%";
        case DRV2605L_EFFECT_DOUBLE_CLICK_100:
            return "Double Click 100%";
        case DRV2605L_EFFECT_DOUBLE_CLICK_60:
            return "Double Click 60%";
        case DRV2605L_EFFECT_TRIPLE_CLICK_100:
            return "Triple Click 100%";
        case DRV2605L_EFFECT_SOFT_FUZZ_60:
            return "Soft Fuzz 60%";
        case DRV2605L_EFFECT_STRONG_BUZZ_100:
            return "Strong Buzz 100%";
        case DRV2605L_EFFECT_750_MS_ALERT_100:
            return "750ms Alert 100%";
        case DRV2605L_EFFECT_1000_MS_ALERT_100:
            return "1000ms Alert 100%";
        case DRV2605L_EFFECT_STRONG_CLICK_1_100:
            return "Strong Click 1 100%";
        case DRV2605L_EFFECT_STRONG_CLICK_2_80:
            return "Strong Click 2 80%";
        case DRV2605L_EFFECT_STRONG_CLICK_3_60:
            return "Strong Click 3 60%";
        case DRV2605L_EFFECT_STRONG_CLICK_4_30:
            return "Strong Click 4 30%";
        case DRV2605L_EFFECT_MEDIUM_CLICK_1_100:
            return "Medium Click 1 100%";
        case DRV2605L_EFFECT_MEDIUM_CLICK_2_80:
            return "Medium Click 2 80%";
        case DRV2605L_EFFECT_MEDIUM_CLICK_3_60:
            return "Medium Click 3 60%";
        case DRV2605L_EFFECT_SHARP_TICK_1_100:
            return "Sharp Tick 1 100%";
        case DRV2605L_EFFECT_SHARP_TICK_2_80:
            return "Sharp Tick 2 80%";
        case DRV2605L_EFFECT_SHARP_TICK_3_60:
            return "Sharp Tick 3 60%";
        case DRV2605L_EFFECT_SHORT_DOUBLE_CLICK_STRONG_1_100:
            return "Short Double Click Strong 1 100%";
        case DRV2605L_EFFECT_SHORT_DOUBLE_CLICK_STRONG_2_80:
            return "Short Double Click Strong 2 80%";
        case DRV2605L_EFFECT_SHORT_DOUBLE_CLICK_STRONG_3_60:
            return "Short Double Click Strong 3 60%";
        case DRV2605L_EFFECT_SHORT_DOUBLE_CLICK_STRONG_4_30:
            return "Short Double Click Strong 4 30%";
        case DRV2605L_EFFECT_SHORT_DOUBLE_CLICK_MEDIUM_1_100:
            return "Short Double Click Medium 1 100%";
        case DRV2605L_EFFECT_SHORT_DOUBLE_CLICK_MEDIUM_2_80:
            return "Short Double Click Medium 2 80%";
        case DRV2605L_EFFECT_SHORT_DOUBLE_CLICK_MEDIUM_3_60:
            return "Short Double Click Medium 3 60%";
        case DRV2605L_EFFECT_SHORT_DOUBLE_SHARP_TICK_1_100:
            return "Short Double Sharp Tick 1 100%";
        case DRV2605L_EFFECT_SHORT_DOUBLE_SHARP_TICK_2_80:
            return "Short Double Sharp Tick 2 80%";
        case DRV2605L_EFFECT_SHORT_DOUBLE_SHARP_TICK_3_60:
            return "Short Double Sharp Tick 3 60%";
        case DRV2605L_EFFECT_LONG_DOUBLE_SHARP_CLICK_STRONG_1_100:
            return "Long Double Sharp Click Strong 1 100%";
        case DRV2605L_EFFECT_LONG_DOUBLE_SHARP_CLICK_STRONG_2_80:
            return "Long Double Sharp Click Strong 2 80%";
        case DRV2605L_EFFECT_LONG_DOUBLE_SHARP_CLICK_STRONG_3_60:
            return "Long Double Sharp Click Strong 3 60%";
        case DRV2605L_EFFECT_LONG_DOUBLE_SHARP_CLICK_STRONG_4_30:
            return "Long Double Sharp Click Strong 4 30%";
        case DRV2605L_EFFECT_LONG_DOUBLE_SHARP_CLICK_MEDIUM_1_100:
            return "Long Double Sharp Click Medium 1 100%";
        case DRV2605L_EFFECT_LONG_DOUBLE_SHARP_CLICK_MEDIUM_2_80:
            return "Long Double Sharp Click Medium 2 80%";
        case DRV2605L_EFFECT_LONG_DOUBLE_SHARP_CLICK_MEDIUM_3_60:
            return "Long Double Sharp Click Medium 3 60%";
        case DRV2605L_EFFECT_LONG_DOUBLE_SHARP_TICK_1_100:
            return "Long Double Sharp Tick 1 100%";
        case DRV2605L_EFFECT_LONG_DOUBLE_SHARP_TICK_2_80:
            return "Long Double Sharp Tick 2 80%";
        case DRV2605L_EFFECT_LONG_DOUBLE_SHARP_TICK_3_60:
            return "Long Double Sharp Tick 3 60%";
        case DRV2605L_EFFECT_BUZZ_1_100:
            return "Buzz 1 100%";
        case DRV2605L_EFFECT_BUZZ_2_80:
            return "Buzz 2 80%";
        case DRV2605L_EFFECT_BUZZ_3_60:
            return "Buzz 3 60%";
        case DRV2605L_EFFECT_BUZZ_4_40:
            return "Buzz 4 40%";
        case DRV2605L_EFFECT_BUZZ_5_20:
            return "Buzz 5 20%";
        case DRV2605L_EFFECT_PULSING_STRONG_1_100:
            return "Pulsing Strong 1 100%";
        case DRV2605L_EFFECT_PULSING_STRONG_2_60:
            return "Pulsing Strong 2 60%";
        case DRV2605L_EFFECT_PULSING_MEDIUM_1_100:
            return "Pulsing Medium 1 100%";
        case DRV2605L_EFFECT_PULSING_MEDIUM_2_60:
            return "Pulsing Medium 2 60%";
        case DRV2605L_EFFECT_PULSING_SHARP_1_100:
            return "Pulsing Sharp 1 100%";
        case DRV2605L_EFFECT_PULSING_SHARP_2_60:
            return "Pulsing Sharp 2 60%";
        case DRV2605L_EFFECT_TRANSITION_CLICK_1_100:
            return "Transition Click 1 100%";
        case DRV2605L_EFFECT_TRANSITION_CLICK_2_80:
            return "Transition Click 2 80%";
        case DRV2605L_EFFECT_TRANSITION_CLICK_3_60:
            return "Transition Click 3 60%";
        case DRV2605L_EFFECT_TRANSITION_CLICK_4_40:
            return "Transition Click 4 40%";
        case DRV2605L_EFFECT_TRANSITION_CLICK_5_20:
            return "Transition Click 5 20%";
        case DRV2605L_EFFECT_TRANSITION_CLICK_6_10:
            return "Transition Click 6 10%";
        case DRV2605L_EFFECT_TRANSITION_HUM_1_100:
            return "Transition Hum 1 100%";
        case DRV2605L_EFFECT_TRANSITION_HUM_2_80:
            return "Transition Hum 2 80%";
        case DRV2605L_EFFECT_TRANSITION_HUM_3_60:
            return "Transition Hum 3 60%";
        case DRV2605L_EFFECT_TRANSITION_HUM_4_40:
            return "Transition Hum 4 40%";
        case DRV2605L_EFFECT_TRANSITION_HUM_5_20:
            return "Transition Hum 5 20%";
        case DRV2605L_EFFECT_TRANSITION_HUM_6_10:
            return "Transition Hum 6 10%";
        case DRV2605L_EFFECT_TRANSITION_RAMP_DOWN_LONG_SMOOTH_1_100:
            return "Transition Ramp Down Long Smooth 1 100%";
        case DRV2605L_EFFECT_TRANSITION_RAMP_DOWN_LONG_SMOOTH_2_100:
            return "Transition Ramp Down Long Smooth 2 100%";
        case DRV2605L_EFFECT_TRANSITION_RAMP_DOWN_MEDIUM_SMOOTH_1_100:
            return "Transition Ramp Down Medium Smooth 1 100%";
        case DRV2605L_EFFECT_TRANSITION_RAMP_DOWN_MEDIUM_SMOOTH_2_100:
            return "Transition Ramp Down Medium Smooth 2 100%";
        case DRV2605L_EFFECT_TRANSITION_RAMP_DOWN_SHORT_SMOOTH_1_100:
            return "Transition Ramp Down Short Smooth 1 100%";
        case DRV2605L_EFFECT_TRANSITION_RAMP_DOWN_SHORT_SMOOTH_2_100:
            return "Transition Ramp Down Short Smooth 2 100%";
        case DRV2605L_EFFECT_TRANSITION_RAMP_DOWN_LONG_SHARP_1_100:
            return "Transition Ramp Down Long Sharp 1 100%";
        case DRV2605L_EFFECT_TRANSITION_RAMP_DOWN_LONG_SHARP_2_100:
            return "Transition Ramp Down Long Sharp 2 100%";
        case DRV2605L_EFFECT_TRANSITION_RAMP_DOWN_MEDIUM_SHARP_1_100:
            return "Transition Ramp Down Medium Sharp 1 100%";
        case DRV2605L_EFFECT_TRANSITION_RAMP_DOWN_MEDIUM_SHARP_2_100:
            return "Transition Ramp Down Medium Sharp 2 100%";
        case DRV2605L_EFFECT_TRANSITION_RAMP_DOWN_SHORT_SHARP_1_100:
            return "Transition Ramp Down Short Sharp 1 100%";
        case DRV2605L_EFFECT_TRANSITION_RAMP_DOWN_SHORT_SHARP_2_100:
            return "Transition Ramp Down Short Sharp 2 100%";
        case DRV2605L_EFFECT_TRANSITION_RAMP_UP_LONG_SMOOTH_1_100:
            return "Transition Ramp Up Long Smooth 1 100%";
        case DRV2605L_EFFECT_TRANSITION_RAMP_UP_LONG_SMOOTH_2_100:
            return "Transition Ramp Up Long Smooth 2 100%";
        case DRV2605L_EFFECT_TRANSITION_RAMP_UP_MEDIUM_SMOOTH_1_100:
            return "Transition Ramp Up Medium Smooth 1 100%";
        case DRV2605L_EFFECT_TRANSITION_RAMP_UP_MEDIUM_SMOOTH_2_100:
            return "Transition Ramp Up Medium Smooth 2 100%";
        case DRV2605L_EFFECT_TRANSITION_RAMP_UP_SHORT_SMOOTH_1_100:
            return "Transition Ramp Up Short Smooth 1 100%";
        case DRV2605L_EFFECT_TRANSITION_RAMP_UP_SHORT_SMOOTH_2_100:
            return "Transition Ramp Up Short Smooth 2 100%";
        case DRV2605L_EFFECT_TRANSITION_RAMP_UP_LONG_SHARP_1_100:
            return "Transition Ramp Up Long Sharp 1 100%";
        case DRV2605L_EFFECT_TRANSITION_RAMP_UP_LONG_SHARP_2_100:
            return "Transition Ramp Up Long Sharp 2 100%";
        case DRV2605L_EFFECT_TRANSITION_RAMP_UP_MEDIUM_SHARP_1_100:
            return "Transition Ramp Up Medium Sharp 1 100%";
        case DRV2605L_EFFECT_TRANSITION_RAMP_UP_MEDIUM_SHARP_2_100:
            return "Transition Ramp Up Medium Sharp 2 100%";
        case DRV2605L_EFFECT_TRANSITION_RAMP_UP_SHORT_SHARP_1_100:
            return "Transition Ramp Up Short Sharp 1 100%";
        case DRV2605L_EFFECT_TRANSITION_RAMP_UP_SHORT_SHARP_2_100:
            return "Transition Ramp Up Short Sharp 2 100%";
        case DRV2605L_EFFECT_TRANSITION_RAMP_DOWN_LONG_SMOOTH_1_50:
            return "Transition Ramp Down Long Smooth 1 50%";
        case DRV2605L_EFFECT_TRANSITION_RAMP_DOWN_LONG_SMOOTH_2_50:
            return "Transition Ramp Down Long Smooth 2 50%";
        case DRV2605L_EFFECT_TRANSITION_RAMP_DOWN_MEDIUM_SMOOTH_1_50:
            return "Transition Ramp Down Medium Smooth 1 50%";
        case DRV2605L_EFFECT_TRANSITION_RAMP_DOWN_MEDIUM_SMOOTH_2_50:
            return "Transition Ramp Down Medium Smooth 2 50%";
        case DRV2605L_EFFECT_TRANSITION_RAMP_DOWN_SHORT_SMOOTH_1_50:
            return "Transition Ramp Down Short Smooth 1 50%";
        case DRV2605L_EFFECT_TRANSITION_RAMP_DOWN_SHORT_SMOOTH_2_50:
            return "Transition Ramp Down Short Smooth 2 50%";
        case DRV2605L_EFFECT_TRANSITION_RAMP_DOWN_LONG_SHARP_1_50:
            return "Transition Ramp Down Long Sharp 1 50%";
        case DRV2605L_EFFECT_TRANSITION_RAMP_DOWN_LONG_SHARP_2_50:
            return "Transition Ramp Down Long Sharp 2 50%";
        case DRV2605L_EFFECT_TRANSITION_RAMP_DOWN_MEDIUM_SHARP_1_50:
            return "Transition Ramp Down Medium Sharp 1 50%";
        case DRV2605L_EFFECT_TRANSITION_RAMP_DOWN_MEDIUM_SHARP_2_50:
            return "Transition Ramp Down Medium Sharp 2 50%";
        case DRV2605L_EFFECT_TRANSITION_RAMP_DOWN_SHORT_SHARP_1_50:
            return "Transition Ramp Down Short Sharp 1 50%";
        case DRV2605L_EFFECT_TRANSITION_RAMP_DOWN_SHORT_SHARP_2_50:
            return "Transition Ramp Down Short Sharp 2 50%";
        case DRV2605L_EFFECT_TRANSITION_RAMP_UP_LONG_SMOOTH_1_50:
            return "Transition Ramp Up Long Smooth 1 50%";
        case DRV2605L_EFFECT_TRANSITION_RAMP_UP_LONG_SMOOTH_2_50:
            return "Transition Ramp Up Long Smooth 2 50%";
        case DRV2605L_EFFECT_TRANSITION_RAMP_UP_MEDIUM_SMOOTH_1_50:
            return "Transition Ramp Up Medium Smooth 1 50%";
        case DRV2605L_EFFECT_TRANSITION_RAMP_UP_MEDIUM_SMOOTH_2_50:
            return "Transition Ramp Up Medium Smooth 2 50%";
        case DRV2605L_EFFECT_TRANSITION_RAMP_UP_SHORT_SMOOTH_1_50:
            return "Transition Ramp Up Short Smooth 1 50%";
        case DRV2605L_EFFECT_TRANSITION_RAMP_UP_SHORT_SMOOTH_2_50:
            return "Transition Ramp Up Short Smooth 2 50%";
        case DRV2605L_EFFECT_TRANSITION_RAMP_UP_LONG_SHARP_1_50:
            return "Transition Ramp Up Long Sharp 1 50%";
        case DRV2605L_EFFECT_TRANSITION_RAMP_UP_LONG_SHARP_2_50:
            return "Transition Ramp Up Long Sharp 2 50%";
        case DRV2605L_EFFECT_TRANSITION_RAMP_UP_MEDIUM_SHARP_1_50:
            return "Transition Ramp Up Medium Sharp 1 50%";
        case DRV2605L_EFFECT_TRANSITION_RAMP_UP_MEDIUM_SHARP_2_50:
            return "Transition Ramp Up Medium Sharp 2 50%";
        case DRV2605L_EFFECT_TRANSITION_RAMP_UP_SHORT_SHARP_1_50:
            return "Transition Ramp Up Short Sharp 1 50%";
        case DRV2605L_EFFECT_TRANSITION_RAMP_UP_SHORT_SHARP_2_50:
            return "Transition Ramp Up Short Sharp 2 50%";
        case DRV2605L_EFFECT_LONG_BUZZ_FOR_PROGRAMMATIC_STOPPING:
            return "Long Buzz for Programmatic Stopping";
        case DRV2605L_EFFECT_SMOOTH_HUM_1_50:
            return "Smooth Hum 1 50%";
        case DRV2605L_EFFECT_SMOOTH_HUM_2_40:
            return "Smooth Hum 2 40%";
        case DRV2605L_EFFECT_SMOOTH_HUM_3_30:
            return "Smooth Hum 3 30%";
        case DRV2605L_EFFECT_SMOOTH_HUM_4_20:
            return "Smooth Hum 4 20%";
        case DRV2605L_EFFECT_SMOOTH_HUM_5_10:
            return "Smooth Hum 5 10%";
        case DRV2605L_EFFECT_COUNT:
            return "Unknown";
    }
    return "Unknown";
}
#endif // HAPTIC_ENABLE && HAPTIC_DRV2605L

#ifdef OS_DETECTION_ENABLE
/**
 * Converts the given OS variant to its corresponding string representation.
 *
 * @param os_detected The detected OS variant of type `os_variant_t`.
 * @return A string representing the name of the OS variant.
 *         Possible return values are:
 *         - "Windows" for OS_WINDOWS
 *         - "MacOS" for OS_MACOS
 *         - "iOS" for OS_IOS
 *         - "Linux" for OS_LINUX
 *         - "Unknown" for any other value
 *
 * Note: Some cases are conditionally compiled and may not be included
 *       depending on the compilation flags.
 */
const char *os_variant_to_string(os_variant_t os_detected) {
    switch (os_detected) {
        case OS_WINDOWS:
            return "Windows";
        case OS_WINDOWS_UNSURE:
            return "Windows?";
        case OS_MACOS:
            return "MacOS";
        case OS_IOS:
            return "iOS";
        case OS_PS5:
            return "PS5";
        case OS_HANDHELD:
            return "Handheld";
        case OS_LINUX:
            return "Linux";
        default:
            return "Unknown";
    }
}
#endif // OS_DETECTION_ENABLE

#ifdef COMMUNITY_MODULE_UNICODE_TYPING_ENABLE
#    include "modules/drashna/unicode_typing/unicode_typing.h"

/**
 * Returns a string representing the Unicode typing mode.
 *
 * @param mode The mode to be converted to a string.
 * @return A constant character pointer to the string representing the Unicode typing mode.
 */
const char *unicode_typing_mode(uint8_t mode) {
    switch (mode) {
        case UCTM_NO_MODE:
            // Normal
            return "Normal";
        case UCTM_WIDE:
            // ｗｉｄｅ
            return "Wide";
        case UCTM_SCRIPT:
            // 𝒮𝒸𝓇𝒾𝓅𝓉
            return "Script";
        case UCTM_BLOCKS:
            //  🅱🅻🅾🅲🅺🆂
            return "Blocks";
        case UCTM_REGIONAL:
            //  ‌‌🇷‌‌🇪‌‌🇬‌‌🇮‌‌🇴‌‌🇳‌‌🇦‌‌🇱‌‌
            return "Regional";
        case UCTM_AUSSIE:
            // ǝᴉssnɐ
            return "Aussie";
        case UCTM_ZALGO:
            // z͚ͭ̐a̪̒͋ͤl͇̼̥̖̃͟͡g̸ͥỡ̳ͬ
            return "Zalgo";
        case UCTM_SUPER:
            // ˢᵘᵖᵉʳˢᶜʳᶦᵖᵗ
            return "Super Script";
        case UCTM_COMIC:
            // ƈơɱıƈ
            return "Comic";
        case UCTM_FRAKTUR:
            // 𝔉𝔯𝔞𝔨𝔱𝔲𝔯
            return "Fraktur";
        case UCTM_DOUBLE_STRUCK:
            // 𝕯𝖔𝖚𝖇𝖑𝖊 𝕾𝖙𝖗𝖚𝖈𝖐
            return "DoubleStruck";
        default:
            // Normal
            return "Unknown";
    }
}
#endif // CUSTOM_UNICODE_ENABLE
