// Copyright 2018-2024 Nick Brassel (@tzarc)
// Copyright 2025 Christopher Courtney, aka Drashna Jael're  (@drashna) <drashna@live.com>
// SPDX-License-Identifier: GPL-3.0-or-later

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

#ifdef KEYCODE_STRING_ENABLE
#    include "keyrecords/process_records.h"
// clang-format off
KEYCODE_STRING_NAMES_USER(
    {VRSN, "VRSN"},
    {KC_DIABLO_CLEAR, "DIAB_CLR"},
    {KC_RGB_T, "RGB_T"},
    {RGB_IDL, "RGB_IDL"},
    {KC_SECRET_1, "SECRET_1"},
    {KC_SECRET_2, "SECRET_2"},
    {KC_SECRET_3, "SECRET_3"},
    {KC_SECRET_4, "SECRET_4"},
    {KC_SECRET_5, "SECRET_5"},
    {KC_SECRET_6, "SECRET_6"},
    {KC_NUKE, "NUKE"},
    {UC_FLIP, "UC_FLIP"},
    {UC_TABL, "UC_TABL"},
    {UC_SHRG, "UC_SHRG"},
    {UC_DISA, "UC_DISA"},
    {UC_IRNY, "UC_IRNY"},
    {UC_CLUE, "UC_CLUE"},
    {KEYLOCK, "KEYLOCK"},
    {KC_NOMODE, "UT_NOMODE"},
    {KC_WIDE, "UT_WIDE"},
    {KC_SCRIPT, "UT_SCRIPT"},
    {KC_BLOCKS, "UT_BLOCKS"},
    {KC_REGIONAL, "UT_REGIONAL"},
    {KC_AUSSIE, "UT_AUSSIE"},
    {KC_ZALGO, "UT_ZALGO"},
    {KC_SUPER, "UT_SUPER"},
    {KC_COMIC, "UT_COMIC"},
    {OLED_LOCK, "OLED_LOCK"},
    {OLED_BRIGHTNESS_INC, "OLED_BRIGHTNESS_INC"},
    {OLED_BRIGHTNESS_DEC, "OLED_BRIGHTNESS_DEC"},
    {OLED_ROTATE_CW, "OLED_ROTATE_CW"},
    {OLED_ROTATE_CCW, "OLED_ROTATE_CCW"},
    {STORE_SETUPS, "STORE_SETUPS"},
    {PRINT_SETUPS, "PRINT_SETUPS"},
    {PD_JIGGLER, "PD_JIGGLER"},
    {US_MATRIX_SCAN_RATE_PRINT, "SCAN_RATE"},
    {US_SELECT_WORD, "SELECT_WORD"},
    {LAYER_LOCK, "LAYER_LOCK"},
    {DISPLAY_MENU, "DISPLAY_MENU"},
    {CLAP_TRAP_ON, "SOCD_ON"},
    {CLAP_TRAP_OFF, "SOCD_OFF"},
    {CLAP_TRAP_TOGGLE, "SOCD_TG"},
    {US_I2C_SCAN_ENABLE, "I2C_SCAN"},
    {US_GAMING_SCAN_TOGGLE, "GAME_MODE"},
    {UC_NEXT, "UC_NEXT"},
    {UC_PREV, "UC_PREV"},
);
// clang-format on
#endif

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
#    include "unicode_typing.h"

/**
 * Returns a string representing the Unicode typing mode.
 *
 * @param mode The mode to be converted to a string.
 * @return A constant character pointer to the string representing the Unicode typing mode.
 */
const char *get_unicode_typing_mode_str(uint8_t mode) {
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
        case UCTM_SCREAM_CYPHER:
            // AÃA̱ĂẠǍA̧ĂAẢÁǍA̱AaÅÃẢA̋A̓ÅẢǍAA̱d
            return "XKCD Scream";
        default:
            // Normal
            return "Unknown";
    }
}
#endif // CUSTOM_UNICODE_ENABLE

/**
 * @brief Generates a string of the layer state bitmask
 *
 * @param buffer char string buffer to write to
 * @param state layer state bitmask
 * @param default_state default layer state bitmask (so we can represent default layer differently)
 */
void format_layer_bitmap_string(char *buffer, layer_state_t state, layer_state_t default_state) {
    for (uint8_t i = 0; i < 16; i++) {
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
    *buffer = '\0';
}

/**
 * @brief Get the layer name string object
 *
 * @param state layer state bitmask
 * @param alt_name Use altname?
 * @param is_default do we want the default layer's name?
 * @return const char* Layer name in string format
 */
const char *get_layer_name_string(uint8_t layer, bool alt_name, bool is_default) {
    switch (layer) {
        case _QWERTY ... _DVORAK:
            if (alt_name) {
                return "Alt";
            }
            if (is_default) {
                switch (layer) {
                    case _QWERTY:
                        return "QWERTY";
                    case _COLEMAK:
                        return "Colemak";
                    case _COLEMAK_DH:
                        return "Colemak-DH";
                    case _DVORAK:
                        return "Dvorak";
                }
            }
            return "Base";
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
 * @brief Checks to see if one or more gaming layers are active
 *
 * @param state layer state bitmask
 * @return true A gaming layer is active
 * @return false No gaming layers active
 */

bool is_gaming_layer_active(layer_state_t state) {
    return ((state & (1 << _GAMEPAD)) != 0) || ((state & (1 << _DIABLO)) != 0) || ((state & (1 << _DIABLOII)) != 0);
}
