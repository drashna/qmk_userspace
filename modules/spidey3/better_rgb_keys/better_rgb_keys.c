// Copyright 2025 Joshua Diamond josh@windowoffire.com (@spidey3)
// SPDX-License-Identifier: GPL-2.0-or-later

#include QMK_KEYBOARD_H

#include <lib/lib8tion/lib8tion.h>

// flags. 0 = no change, 1 = increment, -1 = decrement.
int8_t change_hue = 0;
int8_t change_sat = 0;
int8_t change_val = 0;

// timer to control color change speed
const uint16_t change_tick = 15;

uint32_t change_cb(uint32_t trigger_time, void *cb_arg) {
    if (change_hue != 0 || change_val != 0 || change_sat != 0) {
        HSV hsv = rgblight_get_hsv();
        hsv.h += change_hue;
        hsv.s = change_sat > 0 ? qadd8(hsv.s, (uint8_t)change_sat) : qsub8(hsv.s, (uint8_t)-change_sat);
        hsv.v = change_val > 0 ? qadd8(hsv.v, (uint8_t)change_val) : qsub8(hsv.v, (uint8_t)-change_val);
        rgblight_sethsv_noeeprom(hsv.h, hsv.s, hsv.v);
    }
    return change_tick;
}

static deferred_token change_token = 0;

void start_change(uint8_t hue, uint8_t sat, uint8_t val) {
    change_token = defer_exec(change_tick, change_cb, NULL);
    change_hue   = hue;
    change_sat   = sat;
    change_val   = val;
}

bool process_record_better_rgb_keys(uint16_t keycode, keyrecord_t *record) {
    if (record->event.pressed) {
        switch (keycode) {
                // clang-format off
            case UG_HUEU: start_change( 1, 0, 0); return false;
            case UG_HUED: start_change(-1, 0, 0); return false;
            case UG_SATU: start_change( 0, 1, 0); return false;
            case UG_SATD: start_change( 0,-1, 0); return false;
            case UG_VALU: start_change( 0, 0, 1); return false;
            case UG_VALD: start_change( 0, 0,-1); return false;
                // clang-format on
        }
    } else {
        switch (keycode) {
            case UG_HUEU ... UG_VALD:
                if (change_token) {
                    cancel_deferred_exec(change_token);
                    change_token = 0;
                    eeconfig_update_rgblight_current();
                }
                return false;
        }
    }

    return true;
}
