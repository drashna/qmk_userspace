// Copyright 2020 Christopher Courtney, aka Drashna Jael're  (@drashna) <drashna@live.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

// because layouts seem to not be respecting config.h order atm
#ifdef RGBLIGHT_ENABLE
#    undef RGBLIGHT_EFFECT_BREATHING
#    undef RGBLIGHT_EFFECT_RAINBOW_MOOD
#    undef RGBLIGHT_EFFECT_RAINBOW_SWIRL
#    undef RGBLIGHT_EFFECT_SNAKE
#    undef RGBLIGHT_EFFECT_KNIGHT
#    undef RGBLIGHT_EFFECT_CHRISTMAS
#    undef RGBLIGHT_EFFECT_STATIC_GRADIENT
#    undef RGBLIGHT_EFFECT_RGB_TEST
#    undef RGBLIGHT_EFFECT_ALTERNATING
#    undef RGBLIGHT_EFFECT_TWINKLE
#    if defined(__AVR__) && (!defined(__AVR_AT90USB1286__) && !defined(RGBLIGHT_ALL_ANIMATIONS))
#        define RGBLIGHT_EFFECT_BREATHING
#        define RGBLIGHT_EFFECT_SNAKE
#        define RGBLIGHT_EFFECT_KNIGHT
#    else // defined(__AVR__) && (!defined(__AVR_AT90USB1286__) && !defined(RGBLIGHT_ALL_ANIMATIONS))
#        define RGBLIGHT_EFFECT_BREATHING
#        define RGBLIGHT_EFFECT_RAINBOW_MOOD
#        define RGBLIGHT_EFFECT_RAINBOW_SWIRL
#        define RGBLIGHT_EFFECT_SNAKE
#        define RGBLIGHT_EFFECT_KNIGHT
#        if defined(RGBLIGHT_ALL_ANIMATIONS)
#            define RGBLIGHT_EFFECT_CHRISTMAS
#            define RGBLIGHT_EFFECT_STATIC_GRADIENT
#            define RGBLIGHT_EFFECT_RGB_TEST
#            define RGBLIGHT_EFFECT_ALTERNATING
#        endif
#        define RGBLIGHT_EFFECT_TWINKLE
#    endif // defined(__AVR__) && (!defined(__AVR_AT90USB1286__) && !defined(RGBLIGHT_ALL_ANIMATIONS))
#endif     // RGBLIGHT_ENABLE

#ifdef MOUSEKEY_ENABLE
// mouse movement config
#    ifdef MK_3_SPEED
#        undef MK_3_SPEED
#    endif // MK_3_SPEED
#    define MK_KINETIC_SPEED
#    ifdef MK_KINETIC_SPEED
#        ifndef MOUSEKEY_DELAY
#            define MOUSEKEY_DELAY 8
#        endif // MOUSEKEY_DELAY
#        ifndef MOUSEKEY_INTERVAL
#            define MOUSEKEY_INTERVAL 20
#        endif // MOUSEKEY_INTERVAL
#        ifndef MOUSEKEY_MOVE_DELTA
#            define MOUSEKEY_MOVE_DELTA 25
#        endif // MOUSEKEY_MOVE_DELTA
#    else      // MK_KINETIC_SPEED
#        ifndef MOUSEKEY_DELAY
#            define MOUSEKEY_DELAY 300
#        endif // MOUSEKEY_DELAY
#        ifndef MOUSEKEY_INTERVAL
#            define MOUSEKEY_INTERVAL 50
#        endif // MOUSEKEY_INTERVAL
#        ifndef MOUSEKEY_MOVE_DELTA
#            define MOUSEKEY_MOVE_DELTA 5
#        endif // MOUSEKEY_MOVE_DELTA
#    endif     // MK_KINETIC_SPEED
#    ifndef MOUSEKEY_MAX_SPEED
#        define MOUSEKEY_MAX_SPEED 7
#    endif // MOUSEKEY_MAX_SPEED
#    ifndef MOUSEKEY_TIME_TO_MAX
#        define MOUSEKEY_TIME_TO_MAX 60
#    endif // MOUSEKEY_TIME_TO_MAX
#    ifndef MOUSEKEY_INITIAL_SPEED
#        define MOUSEKEY_INITIAL_SPEED 100
#    endif // MOUSEKEY_INITIAL_SPEED
#    ifndef MOUSEKEY_BASE_SPEED
#        define MOUSEKEY_BASE_SPEED 1000
#    endif // MOUSEKEY_BASE_SPEED
#    ifndef MOUSEKEY_DECELERATED_SPEED
#        define MOUSEKEY_DECELERATED_SPEED 400
#    endif // MOUSEKEY_DECELERATED_SPEED
#    ifndef MOUSEKEY_ACCELERATED_SPEED
#        define MOUSEKEY_ACCELERATED_SPEED 3000
#    endif // MOUSEKEY_ACCELERATED_SPEED
// mouse scroll config
#    ifndef MOUSEKEY_WHEEL_DELAY
#        define MOUSEKEY_WHEEL_DELAY 15
#    endif // MOUSEKEY_WHEEL_DELAY
#    ifndef MOUSEKEY_WHEEL_DELTA
#        define MOUSEKEY_WHEEL_DELTA 1
#    endif // MOUSEKEY_WHEEL_DELTA
#    ifndef MOUSEKEY_WHEEL_INTERVAL
#        define MOUSEKEY_WHEEL_INTERVAL 50
#    endif // MOUSEKEY_WHEEL_INTERVAL
#    ifndef MOUSEKEY_WHEEL_MAX_SPEED
#        define MOUSEKEY_WHEEL_MAX_SPEED 8
#    endif // MOUSEKEY_WHEEL_MAX_SPEED
#    ifndef MOUSEKEY_WHEEL_TIME_TO_MAX
#        define MOUSEKEY_WHEEL_TIME_TO_MAX 80
#    endif // MOUSEKEY_WHEEL_TIME_TO_MAX
// mouse scroll kinetic config
#    ifndef MOUSEKEY_WHEEL_INITIAL_MOVEMENTS
#        define MOUSEKEY_WHEEL_INITIAL_MOVEMENTS 8
#    endif // MOUSEKEY_WHEEL_INITIAL_MOVEMENTS
#    ifndef MOUSEKEY_WHEEL_BASE_MOVEMENTS
#        define MOUSEKEY_WHEEL_BASE_MOVEMENTS 48
#    endif // MOUSEKEY_WHEEL_BASE_MOVEMENTS
#    ifndef MOUSEKEY_WHEEL_ACCELERATED_MOVEMENTS
#        define MOUSEKEY_WHEEL_ACCELERATED_MOVEMENTS 48
#    endif // MOUSEKEY_WHEEL_ACCELERATED_MOVEMENTS
#    ifndef MOUSEKEY_WHEEL_DECELERATED_MOVEMENTS
#        define MOUSEKEY_WHEEL_DECELERATED_MOVEMENTS 8
#    endif // MOUSEKEY_WHEEL_DECELERATED_MOVEMENTS
#endif     // MOUSEKEY_ENABLE

#ifndef TAPPING_TERM
#    define TAPPING_TERM 175
#endif // TAPPING_TERM

#if !defined(DYNAMIC_KEYMAP_LAYER_COUNT) && (defined(XAP_ENABLE) || defined(VIA_ENABLE)) && !defined(__AVR__)
#    define DYNAMIC_KEYMAP_LAYER_COUNT 12
#endif

#ifndef EECONFIG_USER_DATA_SIZE
#    define EECONFIG_USER_DATA_SIZE 64
#endif // EECONFIG_USER_DATA_SIZE
#ifdef SPLIT_KEYBOARD
#    if EECONFIG_USER_DATA_SIZE > RPC_M2S_BUFFER_SIZE
#        error "EECONFIG_USER_DATA_SIZE is too large!"
#    endif // EECONFIG_USER_DATA_SIZE > 1000
#else      // SPLIT_KEYBOARD
#    if EECONFIG_USER_DATA_SIZE > 1000
#        error "EECONFIG_USER_DATA_SIZE is too large!"
#    endif // EECONFIG_USER_DATA_SIZE > 1000
#endif     // SPLIT_KEYBOARD
#ifndef EECONFIG_USER_DATA_VERSION
#    define EECONFIG_USER_DATA_VERSION (0x13373A7D + EECONFIG_USER_DATA_SIZE)
#endif // EECONFIG_USER_DATA_VERSION

#ifdef WPM_ENABLE
#    ifndef WPM_GRAPH_SAMPLES
#        define WPM_GRAPH_SAMPLES 20
#    endif // WPM_GRAPH_SAMPLES
#endif     // WPM_ENABLE
