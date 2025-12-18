// Copyright 2025 Joshua Diamond josh@windowoffire.com (@spidey3)
// SPDX-License-Identifier: GPL-2.0-or-later

#include QMK_KEYBOARD_H

bool chromeos_gflock = false;

bool process_gflock(uint16_t keycode, keyrecord_t *record) {
    if (!chromeos_gflock) {
        return true;
    }

    if (record->event.pressed) {
        register_code16(G(keycode));
    } else {
        unregister_code16(G(keycode));
    }
    return false;
}

bool process_record_chromeos(uint16_t keycode, keyrecord_t *record) {
    if (!process_record_chromeos_kb(keycode, record)) {
        return false;
    }

    if (record->event.pressed) {
        switch (keycode) {
            case CHROMEOS_SUSPEND:
                tap_code16(LGUI(LSFT(KC_L)));
                return true;

            case CHROMEOS_GUI_FKEY_LOCK:
                chromeos_gflock = !chromeos_gflock;
                break;
        }
    }

    switch (keycode) {
        case KC_F1 ... KC_F12:
            return process_gflock(keycode, record);
    }

    return true;
}
