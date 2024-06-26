// Copyright 2023 Christopher Courtney, aka Drashna Jael're  (@drashna) <drashna@live.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#ifdef RGBLIGHT_ENABLE
#    undef RGBLIGHT_SAT_STEP
#    define RGBLIGHT_SAT_STEP 12

#    define RGBLIGHT_EFFECT_KNIGHT_LENGTH 3
#    define RGBLIGHT_EFFECT_SNAKE_LENGTH  3
#    ifdef RGB_MATRIX_ENABLE
#        define RGBLIGHT_DISABLE_KEYCODES
#    endif
#endif // RGBLIGHT_ENABLE

#undef PRODUCT
#define PRODUCT "DrashnaDox - Hacked ErgoDox EZ Hybrid Monstrosity"

#undef DEBOUNCE
#define DEBOUNCE 20

#define TAPPING_TERM_PER_KEY

#define ERGODOX_LED_30

#ifdef RGB_MATRIX_ENABLE
#    undef RGB_MATRIX_LED_PROCESS_LIMIT
#    undef RGB_MATRIX_LED_FLUSH_LIMIT
#    if defined(RGBLIGHT_ENABLE) && defined(RGB_MATRIX_ENABLE)
#        define RGBLIGHT_LIMIT_VAL 100
#    endif
#endif

#ifdef TAPPING_TERM
#    undef TAPPING_TERM
#endif
#define TAPPING_TERM 185
