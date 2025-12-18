// Copyright 2025 Joshua Diamond josh@windowoffire.com (@spidey3)
// SPDX-License-Identifier: GPL-2.0-or-later

#include QMK_KEYBOARD_H

#include <lib/lib8tion/lib8tion.h>

extern rgblight_config_t rgblight_config;
extern rgblight_status_t rgblight_status;

#define STARTUP_ANIMATION_SATURATION 200
#define STARTUP_ANIMATION_VALUE 255
#define STARTUP_ANIMATION_FADE_STEP 5
#define STARTUP_ANIMATION_CYCLE_STEP 2
#define STARTUP_ANIMATION_RAMP_TO_STEPS 70
#define STARTUP_ANIMATION_STEP_TIME 10
#define STARTUP_ANIMATION_INITIAL_DELAY 5 // milliseconds, must be > 0 and < 255 * STEP_TIME

// clang-format off
typedef enum {
    START,
    FADE_OLD,
    FADE_IN,
    CYCLE,
    RAMP_DOWN,
    RAMP_TO,
    CLEAN_UP,
    DONE
} startup_animation_state_t;
// clang-format on

static rgblight_config_t         old_config;
static uint8_t                   old_base_mode;
static startup_animation_state_t startup_animation_state = START;

uint32_t startup_animation_cb(uint32_t trigger_time, void *cb_arg) {
    static uint8_t counter;

    switch (startup_animation_state) {
        case START:
#ifdef STARTUP_ANIMATION_DEBUG
            dprintln("sua START");
#endif
            startup_animation_state = FADE_OLD;
            counter                 = old_config.val;
            // No break! Just roll into FADE_OLD in the same iteration...

        case FADE_OLD:
#ifdef STARTUP_ANIMATION_DEBUG
            dprintf("sua FADE_OLD counter=%u\n", counter);
#endif
            if (counter >= STARTUP_ANIMATION_FADE_STEP) {
                rgblight_sethsv_noeeprom(old_config.hue, old_config.sat, counter);
                counter -= STARTUP_ANIMATION_FADE_STEP;
            } else {
                counter                 = 0;
                startup_animation_state = FADE_IN;
                rgblight_mode_noeeprom(RGBLIGHT_MODE_STATIC_LIGHT);
            }
            break;

        case FADE_IN:
#ifdef STARTUP_ANIMATION_DEBUG
            dprintf("sua FADE_IN counter=%u\n", counter);
#endif
            if (counter < STARTUP_ANIMATION_VALUE) {
                rgblight_sethsv_noeeprom(old_config.hue, STARTUP_ANIMATION_SATURATION, counter);
                counter += STARTUP_ANIMATION_FADE_STEP;
            } else {
                counter                 = 255;
                startup_animation_state = CYCLE;
            }
            break;

        case CYCLE:
#ifdef STARTUP_ANIMATION_DEBUG
            dprintf("sua CYCLE counter=%u\n", counter);
#endif
            if (counter >= STARTUP_ANIMATION_CYCLE_STEP) {
                rgblight_sethsv_noeeprom((counter + old_config.hue) % 255, STARTUP_ANIMATION_SATURATION, STARTUP_ANIMATION_VALUE);
                counter -= STARTUP_ANIMATION_CYCLE_STEP;
            } else {
                if (
#ifdef RGBLIGHT_EFFECT_BREATHING
                    (old_base_mode == RGBLIGHT_MODE_BREATHING) ||
#endif
#ifdef RGBLIGHT_EFFECT_SNAKE
                    (old_base_mode == RGBLIGHT_MODE_SNAKE) ||
#endif
#ifdef RGBLIGHT_EFFECT_KNIGHT
                    (old_base_mode == RGBLIGHT_MODE_KNIGHT) ||
#endif
#ifdef RGBLIGHT_EFFECT_TWINKLE
                    (old_base_mode == RGBLIGHT_MODE_TWINKLE) ||
#endif
                    !old_config.enable) {
                    counter                 = STARTUP_ANIMATION_VALUE;
                    startup_animation_state = RAMP_DOWN;
                } else if (
#ifdef RGBLIGHT_EFFECT_STATIC_GRADIENT
                    (old_base_mode == RGBLIGHT_MODE_STATIC_GRADIENT) ||
#endif
#ifdef RGBLIGHT_EFFECT_RAINBOW_MOOD
                    (old_base_mode == RGBLIGHT_MODE_RAINBOW_MOOD) ||
#endif
#ifdef RGBLIGHT_EFFECT_RAINBOW_SWIRL
                    (old_base_mode == RGBLIGHT_MODE_RAINBOW_SWIRL) ||
#endif
#ifdef RGBLIGHT_EFFECT_RAINBOW_CHRISTMAS
                    (old_base_mode == RGBLIGHT_MODE_CHRISTMAS) ||
#endif
#ifdef RGBLIGHT_EFFECT_RAINBOW_RGB_TEST
                    (old_base_mode == RGBLIGHT_MODE_RGB_TEST) ||
#endif
                    (old_base_mode == RGBLIGHT_MODE_STATIC_LIGHT)) {
                    counter                 = 0;
                    startup_animation_state = RAMP_TO;
                } else {
                    startup_animation_state = CLEAN_UP;
                }
            }
            break;

        case RAMP_DOWN:
#ifdef STARTUP_ANIMATION_DEBUG
            dprintf("sua RAMP_DOWN counter=%u\n", counter);
#endif
            rgblight_sethsv_noeeprom(old_config.hue, STARTUP_ANIMATION_SATURATION, counter);
            if (counter >= STARTUP_ANIMATION_FADE_STEP) {
                counter -= STARTUP_ANIMATION_FADE_STEP;
            } else {
                startup_animation_state = CLEAN_UP;
            }
            break;

        case RAMP_TO: {
#ifdef STARTUP_ANIMATION_DEBUG
            dprintf("sua RAMP_TO s=%u, v=%u, counter=%u\n", old_config.sat, old_config.val, counter);
#endif
            uint8_t steps = STARTUP_ANIMATION_RAMP_TO_STEPS;
            if (counter < steps) {
                uint8_t s = STARTUP_ANIMATION_SATURATION + counter * (((float)old_config.sat - STARTUP_ANIMATION_SATURATION) / (float)steps);
                uint8_t v = STARTUP_ANIMATION_VALUE + counter * (((float)old_config.val - STARTUP_ANIMATION_VALUE) / (float)steps);
                rgblight_sethsv_noeeprom(old_config.hue, s, v);
                counter++;
            } else {
                startup_animation_state = CLEAN_UP;
            }
        } break;

        case CLEAN_UP:
#ifdef STARTUP_ANIMATION_DEBUG
            dprintln("sua CLEAN_UP");
#endif
            rgblight_reload_from_eeprom();
            startup_animation_state = DONE;
#ifdef STARTUP_ANIMATION_DEBUG
            dprintln("sua DONE");
#endif
            return 0; // no repeat

        default:
            break;
    }

    return STARTUP_ANIMATION_STEP_TIME;
}

void keyboard_post_init_startup_rgb_animation(void) {
    if (is_keyboard_master()) {
        old_config.raw = rgblight_config.raw;
        old_base_mode  = rgblight_status.base_mode;
        if (!old_config.enable) rgblight_enable_noeeprom();
        defer_exec(STARTUP_ANIMATION_INITIAL_DELAY, startup_animation_cb, NULL);
    }
}

bool shutdown_startup_rgb_animation(bool jump_to_bootloader) {
    if (jump_to_bootloader) {
#ifdef RGBLIGHT_LAYERS
        for (uint8_t i = 0; i < RGBLIGHT_MAX_LAYERS; i++) {
            rgblight_set_layer_state(i, false);
        }
#endif
        rgblight_enable_noeeprom();
        rgblight_mode_noeeprom(RGBLIGHT_MODE_STATIC_LIGHT);
        for (int i = 0; i < RGBLIGHT_LED_COUNT; i++) {
            rgblight_setrgb_at(0xFF, 0x80 * (i % 2), 0, i);
        }
    } else {
        rgblight_disable_noeeprom();
    }
    return true;
}
