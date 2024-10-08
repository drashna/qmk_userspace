// Copyright 2023 Christopher Courtney, aka Drashna Jael're  (@drashna) <drashna@live.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

/* ws2812 RGB LED */
#if defined(KEYBOARD_fractal)
#    define WS2812_DI_PIN D2
#    undef RGBLED_NUM
#    define RGBLIGHT_EFFECT_BREATHING
#    define RGBLIGHT_EFFECT_RAINBOW_MOOD
#    define RGBLIGHT_EFFECT_RAINBOW_SWIRL
#    define RGBLIGHT_EFFECT_SNAKE
#    define RGBLIGHT_EFFECT_KNIGHT
#    define RGBLIGHT_EFFECT_CHRISTMAS
#    define RGBLIGHT_EFFECT_STATIC_GRADIENT
#    define RGBLIGHT_EFFECT_RGB_TEST
#    define RGBLIGHT_EFFECT_ALTERNATING
#    define RGBLIGHT_EFFECT_TWINKLE
#    define RGBLIGHT_LED_COUNT 29 // Number of LEDs
#    undef RGBLIGHT_HUE_STEP
#    define RGBLIGHT_HUE_STEP 8
#    undef RGBLIGHT_SAT_STEP
#    define RGBLIGHT_SAT_STEP 8
#    undef RGBLIGHT_VAL_STEP
#    define RGBLIGHT_VAL_STEP  8
#    define RGBLIGHT_LIMIT_VAL 175
#    define RGBLIGHT_SLEEP

#    define RGBLIGHT_EFFECT_KNIGHT_OFFSET  3
#    define RGBLIGHT_EFFECT_KNIGHT_LED_NUM 14
#    define AUDIO_PIN                      B7
#    define NO_MUSIC_MODE
#endif
