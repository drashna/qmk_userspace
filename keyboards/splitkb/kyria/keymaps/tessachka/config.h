/* Copyright 2019 Thomas Baart <thomas@splitkb.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

#ifdef OLED_ENABLE
#    undef OLED_DISPLAY_128X64
#    define OLED_DISPLAY_128X128
#    define OLED_PRE_CHARGE_PERIOD 0x22
#    define OLED_VCOM_DETECT       0x35
#    define OLED_FONT_H   "oled_font.h"
#    define OLED_FONT_END 255
#endif

#ifdef RGBLIGHT_ENABLE
#    define RGBLIGHT_EFFECT_BREATHING
#    define RGBLIGHT_EFFECT_RAINBOW_MOOD
#    define RGBLIGHT_EFFECT_RAINBOW_SWIRL
#    define RGBLIGHT_EFFECT_SNAKE
#    define RGBLIGHT_EFFECT_KNIGHT
// #define RGBLIGHT_EFFECT_CHRISTMAS
// #define RGBLIGHT_EFFECT_STATIC_GRADIENT
// #define RGBLIGHT_EFFECT_RGB_TEST
// #define RGBLIGHT_EFFECT_ALTERNATING
#    define RGBLIGHT_EFFECT_TWINKLE
#    define RGBLIGHT_HUE_STEP 8
#    define RGBLIGHT_SAT_STEP 8
#    define RGBLIGHT_VAL_STEP 8
#    define RGBLIGHT_DEFAULT_MODE RGBLIGHT_MODE_STATIC_LIGHT
#    define RGBLIGHT_DEFAULT_HUE 130
#    define RGBLIGHT_SLEEP
#endif

#undef LOCKING_SUPPORT_ENABLE
/* Locking resynchronize hack */
#undef LOCKING_RESYNC_ENABLE

#ifndef SPLIT_KEYBOARD
#    undef MATRIX_ROWS
#    define MATRIX_ROWS 4

#    ifdef RGBLIGHT_ENABLE
#        undef RGBLED_SPLIT
#        undef RGBLIGHT_LED_COUNT

#        ifdef KEYBOARD_splitkb_kyria_rev3
#            define RGBLIGHT_LED_COUNT 31
#        else
#            define RGBLIGHT_LED_COUNT 10
#        endif
#    endif
#endif
