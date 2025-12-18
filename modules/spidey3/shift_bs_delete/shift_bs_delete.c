// Copyright 2025 Joshua Diamond josh@windowoffire.com (@spidey3)
// SPDX-License-Identifier: GPL-2.0-or-later

#include QMK_KEYBOARD_H

bool process_record_shift_bs_delete(uint16_t keycode, keyrecord_t *record) {
    if (!process_record_shift_bs_delete_kb(keycode, record)) {
        return false;
    }

    uint8_t mods = get_mods();
#ifndef NO_ACTION_ONESHOT
    uint8_t osm = get_oneshot_mods();
#else
    uint8_t osm = 0;
#endif

    switch (keycode) {
        case KC_BSPC: {
            static bool delkey_registered;
            if (record->event.pressed) {
                if ((mods | osm) & MOD_MASK_SHIFT) {
                    del_mods(MOD_MASK_SHIFT);
#ifndef NO_ACTION_ONESHOT
                    clear_oneshot_mods();
#endif
                    register_code(KC_DEL);
                    delkey_registered = true;
                    set_mods(mods);
                    return false;
                }
            } else { // on release of KC_BSPC
                // In case KC_DEL is still being sent even after the release of KC_BSPC
                if (delkey_registered) {
                    unregister_code(KC_DEL);
                    delkey_registered = false;
                    return false;
                }
            }
        }
    }

    return true;
}
