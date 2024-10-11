// Copyright 2020 Christopher Courtney, aka Drashna Jael're  (@drashna) <drashna@live.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "rgb_stuff.h"
#include "drashna_runtime.h"
#include "drashna_layers.h"
#include <ctype.h>

bool has_initialized;

void rgblight_sethsv_default_helper(uint8_t index) {
    rgblight_sethsv_at(rgblight_get_hue(), rgblight_get_sat(), rgblight_get_val(), index);
}
void rgblight_set_hsv_and_mode(uint8_t hue, uint8_t sat, uint8_t val, uint8_t mode) {
#if defined(RGB_MATRIX_ENABLE) && defined(RGBLIGHT_CUSTOM)
    if (val > rgb_matrix_get_val()) {
        val = rgb_matrix_get_val();
    }
#else  // RGB_MATRIX_ENABLE && RGBLIGHT_CUSTOM
    if (val > rgblight_get_val()) {
        val = rgblight_get_val();
    }
#endif // RGB_MATRIX_ENABLE && RGBLIGHT_CUSTOM
    if (val > RGBLIGHT_LIMIT_VAL) {
        val = RGBLIGHT_LIMIT_VAL;
    }

    rgblight_sethsv_noeeprom(hue, sat, val);
    // wait_us(175);  // Add a slight delay between color and mode to ensure it's processed correctly
    rgblight_mode_noeeprom(mode);
}

bool process_record_user_rgb_light(uint16_t keycode, keyrecord_t *record) {
    return true;
}

#if defined(RGBLIGHT_STARTUP_ANIMATION)
static bool    is_enabled;
static bool    is_rgblight_startup;
static hsv_t   old_hsv;
static uint8_t old_mode;
deferred_token rgb_startup_token;

uint32_t rgb_startup_animation(uint32_t triger_time, void *cb_arg) {
    if (is_rgblight_startup && is_keyboard_master()) {
        static uint8_t counter = 0;
        counter++;
        rgblight_sethsv_noeeprom((counter + old_hsv.h) % 255, 255, old_hsv.v);
        if (counter >= 255) {
            is_rgblight_startup = false;
            if (userspace_config.rgb.layer_change) {
                layer_state_set_rgb_light(layer_state);
            } else {
                rgblight_set_hsv_and_mode(old_hsv.h, old_hsv.s, old_hsv.v, old_mode);
            }
            if (!is_enabled) {
                rgblight_disable_noeeprom();
            }
        }
    }
    return is_rgblight_startup ? 10 : 0;
}
#endif // RGBLIGHT_STARTUP_ANIMATION

bool is_rgblight_startup_running(void) {
#if defined(RGBLIGHT_STARTUP_ANIMATION)
    return is_rgblight_startup && is_keyboard_master();
#else  // RGBLIGHT_STARTUP_ANIMATION
    return false;
#endif // RGBLIGHT_STARTUP_ANIMATION
}

void housekeeping_task_rgb_light(void) {
    // #if defined(RGB_MATRIX_ENABLE) && defined(RGBLIGHT_CUSTOM)
    //     if (is_keyboard_master() && rgblight_get_val() != rgb_matrix_get_val()) {
    //         xprintf("[RGB Val] Matrix: %d, Light: %d\n", rgb_matrix_get_val(), rgblight_get_val());
    //         rgblight_sethsv(rgblight_get_hue(), rgblight_get_sat(), rgb_matrix_get_val());
    //     }
    // #endif // RGB_MATRIX_ENABLE && RGBLIGHT_CUSTOM
}

void keyboard_post_init_rgb_light(void) {
#if defined(RGB_MATRIX_ENABLE) && defined(RGBLIGHT_CUSTOM)
    if (is_keyboard_master() && rgblight_get_val() != rgb_matrix_get_val()) {
        xprintf("[RGB Val] Matrix: %d, Light: %d\n", rgb_matrix_get_val(), rgblight_get_val());
        rgblight_sethsv(rgblight_get_hue(), rgblight_get_sat(), rgb_matrix_get_val());
    }
#endif // RGB_MATRIX_ENABLE && RGBLIGHT_CUSTOM
#if defined(RGBLIGHT_STARTUP_ANIMATION)
    is_enabled = rgblight_is_enabled();
    if (userspace_config.rgb.layer_change) {
        layer_state_set_rgb_light(layer_state);
    }
    old_hsv  = rgblight_get_hsv();
    old_mode = rgblight_get_mode();
    rgblight_mode_noeeprom(RGBLIGHT_MODE_STATIC_LIGHT);
    is_rgblight_startup = true;
    rgb_startup_token   = defer_exec(300, rgb_startup_animation, NULL);
#endif // RGBLIGHT_STARTUP_ANIMATION
    if (userspace_config.rgb.layer_change) {
        layer_state_set_rgb_light(layer_state);
    }
}

layer_state_t layer_state_set_rgb_light(layer_state_t state) {
#ifdef RGBLIGHT_ENABLE
    if (userspace_config.rgb.layer_change) {
        switch (get_highest_layer(state & ~((layer_state_t)1 << _MOUSE))) {
            case _MEDIA:
                rgblight_set_hsv_and_mode(HSV_CHARTREUSE, RGBLIGHT_MODE_KNIGHT + 1);
                break;
            case _GAMEPAD:
                rgblight_set_hsv_and_mode(HSV_ORANGE, RGBLIGHT_MODE_SNAKE + 2);
                break;
            case _DIABLO:
            case _DIABLOII:
                rgblight_set_hsv_and_mode(HSV_RED, RGBLIGHT_MODE_BREATHING + 3);
                break;
            case _RAISE:
                rgblight_set_hsv_and_mode(HSV_YELLOW, RGBLIGHT_MODE_BREATHING + 3);
                break;
            case _LOWER:
                rgblight_set_hsv_and_mode(HSV_GREEN, RGBLIGHT_MODE_BREATHING + 3);
                break;
            case _ADJUST:
                rgblight_set_hsv_and_mode(HSV_RED, RGBLIGHT_MODE_KNIGHT + 2);
                break;
            default:
                if (layer_state_cmp(state, _MOUSE)) {
#    if defined(RGBLIGHT_EFFECT_TWINKLE)
                    rgblight_set_hsv_and_mode(HSV_CHARTREUSE, RGBLIGHT_MODE_TWINKLE + 5);
#    else  // RGBLIGHT_EFFECT_TWINKLE
                    rgblight_set_hsv_and_mode(HSV_CHARTREUSE, RGBLIGHT_MODE_BREATHING + 3);
#    endif // RGBLIGHT_EFFECT_TWINKLE
                } else {
                    default_layer_state_set_rgb_light(default_layer_state);
                }
        }
#endif // RGBLIGHT_ENABLE
    }
    return state;
}

layer_state_t default_layer_state_set_rgb_light(layer_state_t state) {
    switch (get_highest_layer(state)) {
        case _DEFAULT_LAYER_1:
            rgblight_set_hsv_and_mode(DEFAULT_LAYER_1_HSV, RGBLIGHT_MODE_STATIC_LIGHT);
            break;
        case _DEFAULT_LAYER_2:
            rgblight_set_hsv_and_mode(DEFAULT_LAYER_2_HSV, RGBLIGHT_MODE_STATIC_LIGHT);
            break;
        case _DEFAULT_LAYER_3:
            rgblight_set_hsv_and_mode(DEFAULT_LAYER_3_HSV, RGBLIGHT_MODE_STATIC_LIGHT);
            break;
        case _DEFAULT_LAYER_4:
            rgblight_set_hsv_and_mode(DEFAULT_LAYER_4_HSV, RGBLIGHT_MODE_STATIC_LIGHT);
            break;
    }
    return state;
}

#define _RGBM_SINGLE_STATIC(sym) \
    case RGBLIGHT_MODE_##sym:    \
        return #sym;
#define _RGBM_SINGLE_DYNAMIC(sym) \
    case RGBLIGHT_MODE_##sym:     \
        return #sym;
#define _RGBM_MULTI_STATIC(sym) \
    case RGBLIGHT_MODE_##sym:   \
        return #sym;
#define _RGBM_MULTI_DYNAMIC(sym) \
    case RGBLIGHT_MODE_##sym:    \
        return #sym;
#define _RGBM_TMP_STATIC(sym, msym) \
    case RGBLIGHT_MODE_##sym:       \
        return #msym;
#define _RGBM_TMP_DYNAMIC(sym, msym) \
    case RGBLIGHT_MODE_##sym:        \
        return #msym;

const char *rgblight_name(uint8_t effect) {
    switch (effect) {
#include "rgblight_modes.h"
        case 0:
            return "Off";
        default:
            return "UNKNOWN";
    }
}

const char *rgblight_get_effect_name(void) {
    static char    buf[32]     = {0};
    static uint8_t last_effect = 0;
    if (last_effect != rgblight_get_mode()) {
        last_effect = rgblight_get_mode();
        snprintf(buf, sizeof(buf), "%s", rgblight_name(rgblight_get_mode()));
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
    return buf;
}

void rgblight_shutdown(bool jump_to_bootloader) {
    rgblight_enable_noeeprom();
    rgblight_mode_noeeprom(1);
    rgblight_setrgb(rgblight_get_val(), 0x00, 0x00);
}

extern rgblight_config_t rgblight_config;

bool has_rgblight_config_changed(void) {
    static rgblight_config_t last_config = {0};
    bool                     has_changed = memcmp(&last_config, &rgblight_config, sizeof(rgblight_config_t));
    if (has_changed) {
        memcpy(&last_config, &rgblight_config, sizeof(rgblight_config_t));
    }
    return has_changed;
}
