// Copyright 2021 Christopher Courtney, aka Drashna Jael're  (@drashna) <drashna@live.com>
// Copyright 2024 burkfers (@burkfers)
// Copyright 2024 Wimads (@wimads)
// SPDX-License-Identifier: GPL-3.0-or-later

#include "pointing_device_auto_mouse.h"
#include "keyrecords/process_records.h"
#include "action_util.h"
#include "drashna_layers.h"
#include "drashna_runtime.h"
#include "drashna_util.h"
#include "pointing.h"
#include "math.h"
#include <stdlib.h>
#ifdef AUDIO_ENABLE
#    include "audio.h"
#    ifdef AUDIO_CLICKY
#        include "process_clicky.h"
#    endif // AUDIO_CLICKY
#endif     // AUDIO_ENABLE

static uint16_t mouse_debounce_timer = 0;

#ifdef TAPPING_TERM_PER_KEY
#    define TAP_CHECK get_tapping_term(QK_MOUSE_BUTTON_1, NULL)
#else // TAPPING_TERM_PER_KEY
#    ifndef TAPPING_TERM
#        define TAPPING_TERM 200
#    endif
#    define TAP_CHECK TAPPING_TERM
#endif // TAPPING_TERM_PER_KEY

#ifndef MOUSE_JIGGLER_THRESHOLD
#    define MOUSE_JIGGLER_THRESHOLD 20
#endif // MOUSE_JIGGLER_THRESHOLD
#ifndef MOUSE_JIGGLER_INTERVAL_MS
#    define MOUSE_JIGGLER_INTERVAL_MS 16
#endif // MOUSE_JIGGLER_INTERVAL_MS

#define _CONSTRAIN(amt, low, high) ((amt) < (low) ? (low) : ((amt) > (high) ? (high) : (amt)))
#define CONSTRAIN_REPORT(val)      (mouse_xy_report_t) _CONSTRAIN(val, XY_REPORT_MIN, XY_REPORT_MAX)

static uint16_t     mouse_jiggler_timer          = 0;
static uint32_t     mouse_jiggler_debounce_timer = 0;
static const int8_t deltas[32]                   = {0, -1, -2, -2, -3, -3, -4, -4, -4, -4, -3, -3, -2, -2, -1, 0,
                                                    0, 1,  2,  2,  3,  3,  4,  4,  4,  4,  3,  3,  2,  2,  1,  0};
typedef struct {
    mouse_xy_report_t x;
    mouse_xy_report_t y;
    mouse_hv_report_t v;
    mouse_hv_report_t h;
} mouse_movement_t;
mouse_movement_t total_mouse_movement = {0, 0, 0, 0};

#ifdef AUDIO_ENABLE
// brackets: 1st is number of buttons, 2nd is number of notes, 3rd is number of octaves
// Increase the first 2,if you want more than 2 notes here
float fp_mouse_sounds[][2][2] = {
    SONG(FP_MOUSE_SOUND_1),
    SONG(FP_MOUSE_SOUND_2),
    SONG(FP_MOUSE_SOUND_3),
};

_Static_assert(ARRAY_SIZE(fp_mouse_sounds) <= 8, "Too many mouse sounds defined");

uint8_t pointing_device_handle_buttons(uint8_t buttons, bool pressed, pointing_device_buttons_t button) {
    uint8_t button_mask = 1 << button;

    if ((buttons & button_mask) != (pressed ? button_mask : 0) && pressed && button <= ARRAY_SIZE(fp_mouse_sounds) &&
        !is_clicky_on() && userspace_config.pointing.audio_mouse_clicky) {
        PLAY_SONG(fp_mouse_sounds[button]);
    }

    if (pressed) {
        buttons |= button_mask;
    } else {
        buttons &= ~button_mask;
    }

    return buttons;
}
#endif // AUDIO_ENABLE

__attribute__((weak)) void pointing_device_init_keymap(void) {}

void pointing_device_init_user(void) {
    set_auto_mouse_enable(userspace_config.pointing.auto_mouse_layer.enable);
    set_auto_mouse_layer(userspace_config.pointing.auto_mouse_layer.layer);
    set_auto_mouse_timeout(userspace_config.pointing.auto_mouse_layer.timeout);
    set_auto_mouse_debounce(userspace_config.pointing.auto_mouse_layer.debounce);

    pointing_device_init_keymap();
}

bool mouse_movement_threshold_check(report_mouse_t* mouse_report, mouse_movement_t* movement, uint16_t threshold) {
    movement->x += mouse_report->x;
    movement->y += mouse_report->y;
    movement->h += mouse_report->h;
    movement->v += mouse_report->v;
    return abs(movement->x) > threshold || abs(movement->y) > threshold || abs(movement->h) > threshold ||
           abs(movement->v) > threshold;
}

void mouse_jiggler_check(report_mouse_t* mouse_report) {
    static mouse_movement_t jiggler_threshold = {0, 0, 0, 0};
    if (mouse_movement_threshold_check(mouse_report, &jiggler_threshold, MOUSE_JIGGLER_THRESHOLD)) {
        userspace_runtime_state.pointing.mouse_jiggler.running = false;
        jiggler_threshold                                      = (mouse_movement_t){.x = 0, .y = 0, .h = 0, .v = 0};
    }
    if (userspace_config.pointing.mouse_jiggler.enable &&
        timer_elapsed(mouse_jiggler_timer) > MOUSE_JIGGLER_INTERVAL_MS) {
        if (userspace_runtime_state.pointing.mouse_jiggler.running) {
            static uint8_t phase = 0;
            mouse_report->x += deltas[phase];
            mouse_report->y += deltas[(phase + 8) & 31];
            phase = (phase + 1) & 31;
        } else if (timer_elapsed32(mouse_jiggler_debounce_timer) >
                       (userspace_config.pointing.mouse_jiggler.timeout * 1000) &&
                   !is_device_suspended()) {
            userspace_runtime_state.pointing.mouse_jiggler.running = true;
        }
        jiggler_threshold   = (mouse_movement_t){.x = 0, .y = 0, .h = 0, .v = 0};
        mouse_jiggler_timer = timer_read();
    }
}

__attribute__((weak)) report_mouse_t pointing_device_task_keymap(report_mouse_t mouse_report) {
    return mouse_report;
}

report_mouse_t pointing_device_task_user(report_mouse_t mouse_report) {
    static report_mouse_t last_mouse_report = {0};
    mouse_jiggler_check(&mouse_report);

    if (memcmp(&mouse_report, &last_mouse_report, sizeof(report_mouse_t)) != 0) {
        last_mouse_report            = mouse_report;
        mouse_jiggler_debounce_timer = timer_read32();
    }

    if (timer_elapsed(mouse_debounce_timer) < TAP_CHECK) {
        mouse_report.x = 0;
        mouse_report.y = 0;
    }

    return pointing_device_task_keymap(mouse_report);
}

bool process_record_pointing(uint16_t keycode, keyrecord_t* record) {
    switch (keycode) {
        case PD_JIGGLER:
            if (record->event.pressed) {
                pointing_device_mouse_jiggler_toggle();
            }
            break;
        default:
            if (!IS_MOUSE_KEYCODE(keycode)) {
                mouse_debounce_timer         = timer_read();
                mouse_jiggler_debounce_timer = timer_read32();
                if (userspace_runtime_state.pointing.mouse_jiggler.running && record->event.pressed) {
                    userspace_runtime_state.pointing.mouse_jiggler.running = false;
                }
            }
            break;
    }
    return true;
}

layer_state_t layer_state_set_pointing(layer_state_t state) {
    if (layer_state_cmp(state, _GAMEPAD) || layer_state_cmp(state, _DIABLO) || layer_state_cmp(state, _DIABLOII)) {
        state |= ((layer_state_t)1 << _MOUSE);
        set_auto_mouse_enable(false); // auto mouse can be disabled any time during run time
    } else {
        set_auto_mouse_enable(userspace_config.pointing.auto_mouse_layer.enable);
    }
    return state;
}

bool has_mouse_report_changed(report_mouse_t* new_report, report_mouse_t* old_report) {
    return ((new_report->buttons != old_report->buttons) || (new_report->x != 0 && new_report->x != old_report->x) ||
            (new_report->y != 0 && new_report->y != old_report->y) ||
            (new_report->h != 0 && new_report->h != old_report->h) ||
            (new_report->v != 0 && new_report->v != old_report->v));
}

#if defined(POINTING_DEVICE_AUTO_MOUSE_ENABLE)
__attribute__((weak)) bool is_mouse_record_keymap(uint16_t keycode, keyrecord_t* record) {
    return false;
}

bool is_mouse_record_user(uint16_t keycode, keyrecord_t* record) {
    if (is_mouse_record_keymap(keycode, record)) {
        return true;
    }
    switch (keycode) {
#    if defined(KEYBOARD_ploopy)
        case DPI_CONFIG:
#    elif (defined(KEYBOARD_bastardkb_charybdis) || defined(KEYBOARD_handwired_tractyl_manuform)) && \
            !defined(NO_CHARYBDIS_KEYCODES) ||                                                       \
        (defined(KEYBOARD_bastardkb_dilemma) && !defined(NO_DILEMMA_KEYCODES))
        case QK_KB ... QK_KB_MAX:
#    endif
        case PD_JIGGLER:
            return true;
    }
    return false;
}
#endif

void pointing_device_mouse_jiggler_toggle(void) {
    mouse_jiggler_timer          = timer_read();
    mouse_jiggler_debounce_timer = timer_read32() + (userspace_config.pointing.mouse_jiggler.timeout - 5) * 1000;
    userspace_config.pointing.mouse_jiggler.enable = !userspace_config.pointing.mouse_jiggler.enable;
    eeconfig_update_user_datablock_handler(&userspace_config, 0, EECONFIG_USER_DATA_SIZE);
}

#ifdef POINTING_MODE_MAP_ENABLE
enum keymap_pointing_mode_ids {
    PM_BROW = POINTING_MODE_MAP_START, // BROWSER TAB Manipulation                      [mode id  6]
    PM_APP,                            // Open App browsing                             [mode id  7]
    POSTMAP_PM_SAFE_RANGE              // To avoid overlap when adding additional modes [mode id  8]
};
// (optional) enum to make things easier to read (index numbers can be used directly)
// Must be in the same order as the above modes
enum keymap_pointing_mode_maps_index {
    _PM_BROW, // first mode map  [index  0]
    _PM_APP   // second mode map [index  1]
};

const uint16_t PROGMEM pointing_mode_maps[POINTING_MODE_MAP_COUNT][POINTING_NUM_DIRECTIONS] = {
    [_PM_BROW] = POINTING_MODE_LAYOUT(C(S(KC_PGUP)), C(S(KC_TAB)), C(KC_TAB), C(S(KC_PGDN))),
    [_PM_APP]  = POINTING_MODE_LAYOUT(KC_NO, A(S(KC_TAB)), A(KC_TAB), KC_NO)};
#endif // POINTING_MODE_MAP_ENABLE

#ifdef COMMUNITY_MODULE_POINTING_DEVICE_ACCEL_ENABLE
#    include "pointing_device_accel.h"

void pointing_device_config_read(pointing_device_accel_config_t* config) {
    if (memcmp(config, &userspace_config.pointing.accel, sizeof(pointing_device_accel_config_t)) != 0) {
        memcpy(config, &userspace_config.pointing.accel, sizeof(pointing_device_accel_config_t));
    }
}

void pointing_device_config_update(pointing_device_accel_config_t* config) {
    if (memcmp(config, &userspace_config.pointing.accel, sizeof(pointing_device_accel_config_t)) != 0) {
        memcpy(&userspace_config.pointing.accel, config, sizeof(pointing_device_accel_config_t));
        eeconfig_update_user_datablock_handler(&userspace_config, 0, EECONFIG_USER_DATA_SIZE);
    }
}
#endif
