// Copyright 2020 Christopher Courtney, aka Drashna Jael're  (@drashna) <drashna@live.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "rgb_matrix_stuff.h"
#include "drashna_runtime.h"
#include "drashna_layers.h"
#include "keyrecords/process_records.h"
#include "rgb_matrix.h"
#include "debug.h"
#include <ctype.h>
#include "lib/lib8tion/lib8tion.h"
#ifdef RGBLIGHT_ENABLE
#    include "rgblight.h"
#endif

extern led_config_t g_led_config;

static uint32_t hypno_timer;
RGB             rgb_matrix_hsv_to_rgb(HSV hsv);

void rgb_matrix_layer_helper(uint8_t hue, uint8_t sat, uint8_t val, uint8_t mode, uint8_t speed, uint8_t led_type,
                             uint8_t led_min, uint8_t led_max) {
    HSV hsv = {hue, sat, val};
    if (hsv.v > rgb_matrix_get_val()) {
        hsv.v = rgb_matrix_get_val();
    }

    switch (mode) {
        case 1: // breathing
            {
                uint16_t time = scale16by8(g_rgb_timer, speed / 8);
                hsv.v         = scale8(abs8(sin8(time) - 128) * 2, hsv.v);
                RGB rgb       = rgb_matrix_hsv_to_rgb(hsv);
                for (uint8_t i = 0; i < RGB_MATRIX_LED_COUNT; i++) {
                    if (HAS_FLAGS(g_led_config.flags[i], led_type)) {
                        RGB_MATRIX_INDICATOR_SET_COLOR(i, rgb.r, rgb.g, rgb.b);
                    }
                }
                break;
            }
        default: // Solid Color
            {
                RGB rgb = rgb_matrix_hsv_to_rgb(hsv);
                for (uint8_t i = 0; i < RGB_MATRIX_LED_COUNT; i++) {
                    if (HAS_FLAGS(g_led_config.flags[i], led_type)) {
                        RGB_MATRIX_INDICATOR_SET_COLOR(i, rgb.r, rgb.g, rgb.b);
                    }
                }
                break;
            }
    }
}

void housekeeping_task_rgb_matrix(void) {
#if defined(RGB_MATRIX_FRAMEBUFFER_EFFECTS)
    if (userspace_config.rgb.idle_anim && rgb_matrix_get_mode() == RGB_MATRIX_TYPING_HEATMAP &&
        sync_timer_elapsed32(hypno_timer) > 15000) {
        rgb_matrix_mode_noeeprom(RGB_MATRIX_REST_MODE);
    }
#endif // RGB_MATRIX_FRAMEBUFFER_EFFECTS
}

void keyboard_post_init_rgb_matrix(void) {
#if defined(RGB_MATRIX_FRAMEBUFFER_EFFECTS)
    if (userspace_config.rgb.idle_anim) {
        rgb_matrix_mode_noeeprom(RGB_MATRIX_REST_MODE);
    }
#endif // RGB_MATRIX_FRAMEBUFFER_EFFECTS
    if (userspace_config.rgb.layer_change) {
        rgb_matrix_set_flags_noeeprom(LED_FLAG_UNDERGLOW | LED_FLAG_KEYLIGHT | LED_FLAG_INDICATOR);
    } else {
        rgb_matrix_set_flags_noeeprom(LED_FLAG_ALL);
    }
}

bool process_record_user_rgb_matrix(uint16_t keycode, keyrecord_t *record) {
#if defined(RGBLIGHT_ENABLE) && defined(RGBLIGHT_CUSTOM)
    bool shifted = (get_mods() | get_oneshot_mods()) & MOD_MASK_SHIFT;
#endif
#if defined(RGB_MATRIX_FRAMEBUFFER_EFFECTS)
    hypno_timer = sync_timer_read32();
    if (userspace_config.rgb.idle_anim && rgb_matrix_get_mode() == RGB_MATRIX_REST_MODE) {
        rgb_matrix_mode_noeeprom(RGB_MATRIX_TYPING_HEATMAP);
    }
#endif // RGB_MATRIX_FRAMEBUFFER_EFFECTS
    switch (keycode) {
        case RGB_IDL: // This allows me to use underglow as layer indication, or as normal
            if (record->event.pressed) {
                rgb_matrix_idle_anim_toggle();
            }
            break;
#if defined(RGBLIGHT_ENABLE) && defined(RGBLIGHT_CUSTOM)
        case QK_RGB_MATRIX_VALUE_DOWN:
        case QK_UNDERGLOW_VALUE_DOWN:
            if (record->event.pressed) {
                shifted ? rgb_matrix_increase_val() : rgb_matrix_decrease_val();
                rgblight_sethsv(rgblight_get_hue(), rgblight_get_sat(), rgb_matrix_get_val());
            }
            return false;
        case QK_RGB_MATRIX_VALUE_UP:
        case QK_UNDERGLOW_VALUE_UP:
            if (record->event.pressed) {
                shifted ? rgb_matrix_decrease_val() : rgb_matrix_increase_val();
                rgblight_sethsv(rgblight_get_hue(), rgblight_get_sat(), rgb_matrix_get_val());
            }
            return false;
#endif // RGBLIGHT_ENABLE && RGBLIGHT_CUSTOM
    }
    return true;
}

#if defined(RGBLIGHT_ENABLE) && defined(RGBLIGHT_CUSTOM)
#    include "rgblight_drivers.h"

rgb_led_t led_array[RGBLIGHT_LED_COUNT] = {0};

extern uint8_t led_mapping[RGBLIGHT_LED_COUNT];

void init(void) {}

void set_color_custom(int index, uint8_t red, uint8_t green, uint8_t blue) {
    led_array[index].r = red;
    led_array[index].g = green;
    led_array[index].b = blue;
}
void set_color_all_custom(uint8_t red, uint8_t green, uint8_t blue) {
    for (int i = 0; i < RGBLIGHT_LED_COUNT; i++) {
        set_color_custom(i, red, green, blue);
    }
}

const rgblight_driver_t rgblight_driver = {
    .init          = init,
    .set_color_all = set_color_all_custom,
    .set_color     = set_color_custom,
    .flush         = init,
};
#endif // RGBLIGHT_ENABLE && RGBLIGHT_CUSTOM

__attribute__((weak)) bool rgb_matrix_indicators_advanced_keymap(uint8_t led_min, uint8_t led_max) {
    return true;
}
bool rgb_matrix_indicators_advanced_user(uint8_t led_min, uint8_t led_max) {
    if (!rgb_matrix_indicators_advanced_keymap(led_min, led_max)) {
        return false;
    }

    if (userspace_config.rgb.layer_change) {
#if defined(RGBLIGHT_ENABLE) && defined(RGBLIGHT_CUSTOM)
#    if defined(SPLIT_KEYBOARD) && defined(SPLIT_LAYER_STATE_ENABLE)
        static layer_state_t old_layer_state = (layer_state_t)~0, old_default_layer_state = (layer_state_t)~0;
        if (!is_keyboard_master()) {
            if (layer_state != old_layer_state) {
                old_layer_state = layer_state_set_rgb_light(layer_state);
            }
            if (default_layer_state != old_default_layer_state) {
                old_default_layer_state = default_layer_state_set_rgb_light(default_layer_state);
            }
        }
#    endif // SPLIT_KEYBOARD && SPLIT_LAYER_STATE_ENABLE
        for (uint8_t i = 0; i < RGBLIGHT_LED_COUNT; i++) {
            RGB_MATRIX_INDICATOR_SET_COLOR(led_mapping[i], led_array[i].r, led_array[i].g, led_array[i].b);
        }
#else  // RGBLIGHT_ENABLE && RGBLIGHT_CUSTOM
        switch (get_highest_layer(layer_state & ~((layer_state_t)1 << _MOUSE))) {
            case _GAMEPAD:
                rgb_matrix_layer_helper(HSV_ORANGE, 1, rgb_matrix_config.speed, LED_FLAG_MODIFIER, led_min, led_max);
                break;
            case _DIABLO:
                rgb_matrix_layer_helper(HSV_RED, 1, rgb_matrix_config.speed * 8, LED_FLAG_MODIFIER, led_min, led_max);
                break;
            case _RAISE:
                rgb_matrix_layer_helper(HSV_YELLOW, 1, rgb_matrix_config.speed, LED_FLAG_MODIFIER, led_min, led_max);
                break;
            case _LOWER:
                rgb_matrix_layer_helper(HSV_GREEN, 1, rgb_matrix_config.speed, LED_FLAG_MODIFIER, led_min, led_max);
                break;
            case _ADJUST:
                rgb_matrix_layer_helper(HSV_RED, 1, rgb_matrix_config.speed, LED_FLAG_MODIFIER, led_min, led_max);
                break;
            default:
                if (layer_state_is(_MOUSE)) {
                    rgb_matrix_layer_helper(HSV_PURPLE, 1, rgb_matrix_config.speed, LED_FLAG_MODIFIER, led_min,
                                            led_max);
                } else {
                    switch (get_highest_layer(default_layer_state)) {
                        case _DEFAULT_LAYER_1:
                            rgb_matrix_layer_helper(DEFAULT_LAYER_1_HSV, 0, rgb_matrix_config.speed, LED_FLAG_MODIFIER,
                                                    led_min, led_max);
                            break;
                        case _DEFAULT_LAYER_2:
                            rgb_matrix_layer_helper(DEFAULT_LAYER_2_HSV, 0, rgb_matrix_config.speed, LED_FLAG_MODIFIER,
                                                    led_min, led_max);
                            break;
                        case _DEFAULT_LAYER_3:
                            rgb_matrix_layer_helper(DEFAULT_LAYER_3_HSV, 0, rgb_matrix_config.speed, LED_FLAG_MODIFIER,
                                                    led_min, led_max);
                            break;
                        case _DEFAULT_LAYER_4:
                            rgb_matrix_layer_helper(DEFAULT_LAYER_4_HSV, 0, rgb_matrix_config.speed, LED_FLAG_MODIFIER,
                                                    led_min, led_max);
                            break;
                    }
                }
                break;
        }
#endif // RGBLIGHT_ENABLE && RGBLIGHT_CUSTOM
    }
    return false;
}

__attribute__((weak)) bool rgb_matrix_indicators_keymap(void) {
    return true;
}

bool rgb_matrix_indicators_user(void) {
    return rgb_matrix_indicators_keymap();
}

void rgb_matrix_shutdown(bool jump_to_bootloader) {
    void rgb_matrix_update_pwm_buffers(void);
    rgb_matrix_set_color_all(rgb_matrix_get_val(), 0x00, 0x00);
    rgb_matrix_update_pwm_buffers();
}

//----------------------------------------------------------
// RGB Matrix naming
#include <rgb_matrix.h>

#if defined(RGB_MATRIX_EFFECT)
#    undef RGB_MATRIX_EFFECT
#endif // defined(RGB_MATRIX_EFFECT)

#define RGB_MATRIX_EFFECT(x) RGB_MATRIX_EFFECT_##x,
enum {
    RGB_MATRIX_EFFECT_NONE,
#include "rgb_matrix_effects.inc"
#ifdef RGB_MATRIX_CUSTOM_KB
#    include "rgb_matrix_kb.inc"
#endif // RGB_MATRIX_CUSTOM_KB
#ifdef RGB_MATRIX_CUSTOM_USER
#    include "rgb_matrix_user.inc"
#endif // RGB_MATRIX_CUSTOM_USER
#undef RGB_MATRIX_EFFECT
};

#define RGB_MATRIX_EFFECT(x)    \
    case RGB_MATRIX_EFFECT_##x: \
        return #x;
const char *rgb_matrix_name(uint8_t effect) {
    switch (effect) {
        case RGB_MATRIX_EFFECT_NONE:
            return "NONE";
#include "rgb_matrix_effects.inc"
#ifdef RGB_MATRIX_CUSTOM_KB
#    include "rgb_matrix_kb.inc"
#endif // RGB_MATRIX_CUSTOM_KB
#ifdef RGB_MATRIX_CUSTOM_USER
#    include "rgb_matrix_user.inc"
#endif // RGB_MATRIX_CUSTOM_USER
#undef RGB_MATRIX_EFFECT
        default:
            return "UNKNOWN";
    }
}

const char *rgb_matrix_get_effect_name(void) {
    static char    buf[32]     = {0};
    static uint8_t last_effect = 0;
    if (last_effect != rgb_matrix_get_mode()) {
        last_effect = rgb_matrix_get_mode();
        snprintf(buf, sizeof(buf), "%s", rgb_matrix_name(rgb_matrix_get_mode()));
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

extern rgb_config_t rgb_matrix_config;

bool has_rgb_matrix_config_changed(void) {
    static rgb_config_t last_config = {0};
    bool                has_changed = memcmp(&last_config, &rgb_matrix_config, sizeof(rgb_config_t));
    if (has_changed) {
        memcpy(&last_config, &rgb_matrix_config, sizeof(rgb_config_t));
    }
    return has_changed;
}

void rgb_matrix_idle_anim_toggle(void) {
#if defined(RGB_MATRIX_ENABLE) && defined(RGB_MATRIX_FRAMEBUFFER_EFFECTS)
    userspace_config.rgb.idle_anim ^= 1;
    dprintf("RGB Matrix Idle Animation [EEPROM]: %u\n", userspace_config.rgb.idle_anim);
    eeconfig_update_user_datablock(&userspace_config);
    if (userspace_config.rgb.idle_anim) {
        rgb_matrix_mode_noeeprom(RGB_MATRIX_TYPING_HEATMAP);
    }
#endif // RGB_MATRIX_ENABLE && RGB_MATRIX_FRAMEBUFFER_EFFECTS
}
