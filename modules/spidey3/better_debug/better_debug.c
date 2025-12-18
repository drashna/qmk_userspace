// Copyright 2025 Joshua Diamond josh@windowoffire.com (@spidey3)
// SPDX-License-Identifier: GPL-2.0-or-later

#include QMK_KEYBOARD_H

#include "version.h"
#include <stdlib.h>

ASSERT_COMMUNITY_MODULES_MIN_API_VERSION(1, 0, 0);

static void __attribute__((noinline)) report_version(void) {
    uprintln(QMK_KEYBOARD "/" QMK_KEYMAP "\n"
             "QMK " QMK_VERSION "\n"
             "Userspace " QMK_USERSPACE_VERSION "\n"
             QMK_BUILDDATE);
}

void keyboard_post_init_better_debug(void) {
    keyboard_post_init_better_debug_kb();

    // Defer reporting the version until console is operational
    if (debug_enable) {
        uint32_t _rv(uint32_t trigger_time, void *cb_arg) {
            report_version();
            return 0;
        }
        defer_exec(3000, _rv, NULL);
    }
}

bool process_record_better_debug(uint16_t keycode, keyrecord_t *record) {
    dprintf("DEBUG: key event: kc: %02X, col: %02u, row: %02u, pressed: %u mods: %08b "
#if !defined(NO_ACTION_ONESHOT)
            "os: %08b "
#endif
            "weak: %08b\n",
            keycode, record->event.key.col, record->event.key.row, record->event.pressed, bitrev(get_mods()),
#if !defined(NO_ACTION_ONESHOT)
            bitrev(get_oneshot_mods()),
#endif
            bitrev(get_weak_mods()));

    if (!process_record_better_debug_kb(keycode, record)) {
        return false;
    }

    if (record->event.pressed) {
        switch (keycode) {
            // Re-implement this here, but smarter, and fix the persistence!
            case QK_DEBUG_TOGGLE:
                if (get_mods() & MOD_MASK_SHIFT) {
                    debug_enable   = 0;
                    debug_keyboard = 0;
                    debug_matrix   = 0;
                } else if (!debug_enable) {
                    debug_enable = 1;
                    report_version();
                } else if (!debug_keyboard) {
                    debug_keyboard = 1;
                } else if (!debug_matrix) {
                    debug_matrix = 1;
                } else {
                    debug_enable   = 0;
                    debug_keyboard = 0;
                    debug_matrix   = 0;
                }
                uprintf("DEBUG: enable=%u, kb=%u, matrix=%u\n", debug_enable, debug_keyboard, debug_matrix);
                eeconfig_update_debug(&debug_config);
                return false;
        }
    }

    return true;
}
