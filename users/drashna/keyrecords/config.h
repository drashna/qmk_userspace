// Copyright 2020 Christopher Courtney, aka Drashna Jael're  (@drashna) <drashna@live.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#if defined(WPM_ENABLE)
// #    define WPM_LAUNCH_CONTROL
// #    define WPM_UNFILTERED
#    define WPM_ALLOW_COUNT_REGRESSION
#    define WPM_SAMPLE_SECONDS      10
#    define WPM_SAMPLE_PERIODS      50
#    define WPM_ESTIMATED_WORD_SIZE 5
#endif

#ifdef IS_COMMAND
#    undef IS_COMMAND
#endif // IS_COMMAND
#define IS_COMMAND() (((get_mods() | get_oneshot_mods()) & MOD_MASK_SHIFT) == MOD_MASK_SHIFT)

#define UNICODE_SELECTED_MODES UNICODE_MODE_WINCOMPOSE, UNICODE_MODE_MACOS, UNICODE_MODE_LINUX
#define UNICODE_CYCLE_PERSIST  false

#ifndef ONESHOT_TAP_TOGGLE
#    define ONESHOT_TAP_TOGGLE 2
#endif // !ONESHOT_TAP_TOGGLE

#ifndef ONESHOT_TIMEOUT
#    define ONESHOT_TIMEOUT 3000
#endif // !ONESHOT_TIMEOUT

#if defined(PER_KEY_TAPPING)
#    define PERMISSIVE_HOLD_PER_KEY
#    define QUICK_TAP_TERM_PER_KEY
#    define HOLD_ON_OTHER_KEY
#    define RETRO_TAPPING_PER_KEY
#    define HOLD_ON_OTHER_KEY_PRESS_PER_KEY
#    define TAPPING_TERM_PER_KEY
#elif defined(COMMUNITY_MODULE_ACHORDION_ENABLE)
#    define PERMISSIVE_HOLD
#    define QUICK_TAP_TERM_PER_KEY
#endif // PER_KEY_TAPPING

#ifndef TAPPING_TOGGLE
#    define TAPPING_TOGGLE 1
#endif // !TAPPING_TOGGLE
#define TAP_CODE_DELAY 5

/* Disable unused and unneeded features to reduce on firmware size */
#ifdef LOCKING_SUPPORT_ENABLE
#    undef LOCKING_SUPPORT_ENABLE
#endif // LOCKING_SUPPORT_ENABLE
#ifdef LOCKING_RESYNC_ENABLE
#    undef LOCKING_RESYNC_ENABLE
#endif // LOCKING_RESYNC_ENABLE

#define ENABLE_COMPILE_KEYCODE

#define BOTH_SHIFTS_TURNS_ON_CAPS_WORD
