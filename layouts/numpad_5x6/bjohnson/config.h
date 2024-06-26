// Copyright 2023 Christopher Courtney, aka Drashna Jael're  (@drashna) <drashna@live.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#if defined(RGBLIGHT_ENABLE) && !defined(RGBLED_NUM)
#    define WS2812_DI_PIN B7
#    define RGBLED_NUM    13 // Number of LEDs
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

#    define RGBLIGHT_HUE_STEP              12
#    define RGBLIGHT_SAT_STEP              12
#    define RGBLIGHT_VAL_STEP              12
#    define RGBLIGHT_EFFECT_KNIGHT_LENGTH  2
#    define RGBLIGHT_EFFECT_SNAKE_LENGTH   2
#    define RGBLIGHT_EFFECT_BREATHE_CENTER 1

#    define RGBLIGHT_SLEEP
#endif // RGBLIGHT_ENABLE

#ifdef AUDIO_ENABLE
#    ifdef RGBLIGHT_ENABLE
#        define NO_MUSIC_MODE
#    endif // RGBLIGHT_ENABLE
#endif     // AUDIO_ENABLE
#define TAP_CODE_DELAY 10
