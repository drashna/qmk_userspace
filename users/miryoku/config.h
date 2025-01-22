// Copyright 2019 Manna Harbour
// https://github.com/manna-harbour/miryoku

// This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 2 of the License, or (at your option) any later version. This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details. You should have received a copy of the GNU General Public License along with this program. If not, see <http://www.gnu.org/licenses/>.

#pragma once

#include "custom_config.h"

// default but used in macros
#undef TAPPING_TERM
#define TAPPING_TERM 200


// Enable rapid switch from tap to hold, disables double tap hold auto-repeat.
#define QUICK_TAP_TERM 0

// Auto Shift
#define NO_AUTO_SHIFT_ALPHA
#define AUTO_SHIFT_TIMEOUT TAPPING_TERM
#if defined (VIAL_ENABLE)
  #undef AUTO_SHIFT_NO_SETUP
#endif
#define AUTO_SHIFT_NO_SETUP

// Mouse key speed and acceleration.
#undef MOUSEKEY_DELAY
#define MOUSEKEY_DELAY          0
#undef MOUSEKEY_INTERVAL
#define MOUSEKEY_INTERVAL       16
#undef MOUSEKEY_WHEEL_DELAY
#define MOUSEKEY_WHEEL_DELAY    0
#undef MOUSEKEY_MAX_SPEED
#define MOUSEKEY_MAX_SPEED      6
#undef MOUSEKEY_TIME_TO_MAX
#define MOUSEKEY_TIME_TO_MAX    64

// Thumb Combos
#if defined (MIRYOKU_KLUDGE_THUMBCOMBOS)
  #define COMBO_TERM 200
  #define EXTRA_SHORT_COMBOS
#endif

// If Vial is enabled, replace tap dances with corresponding keys
#if defined (VIAL_ENABLE)
  #define U_TD_KC_BOOT QK_BOOT
  #define U_TD_TAP DF(U_TAP)
  #define U_TD_EXTRA DF(U_EXTRA)
  #define U_TD_BASE DF(U_BASE)
  #define U_TD_FUN DF(U_FUN)
  #define U_TD_MEDIA DF(U_MEDIA)
  #define U_TD_NAV DF(U_NAV)
  #define U_TD_NUM DF(U_NUM)
  #define U_TD_MOUSE DF(U_MOUSE)
  #define U_TD_SYM DF(U_SYM)
#else
  #define U_TD_KC_BOOT TD(U_TD_BOOT)
  #define U_TD_TAP TD(U_TD_U_TAP)
  #define U_TD_EXTRA TD(U_TD_U_EXTRA)
  #define U_TD_BASE TD(U_TD_U_BASE)
  #define U_TD_FUN TD(U_TD_U_FUN)
  #define U_TD_MEDIA TD(U_TD_U_MEDIA)
  #define U_TD_NAV TD(U_TD_U_NAV)
  #define U_TD_NUM TD(U_TD_U_NUM)
  #define U_TD_MOUSE TD(U_TD_U_MOUSE)
  #define U_TD_SYM TD(U_TD_U_SYM)
#endif
