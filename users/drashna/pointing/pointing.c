// Copyright 2024 burkfers (@burkfers)
// Copyright 2024 Wimads (@wimads)
// Copyright 2025 Christopher Courtney, aka Drashna Jael're  (@drashna) <drashna@live.com>
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
#include <string.h>
#ifdef AUDIO_ENABLE
#    include "audio.h"
#    ifdef AUDIO_CLICKY
#        include "process_clicky.h"
#    endif // AUDIO_CLICKY
#endif     // AUDIO_ENABLE
#ifdef COMMUNITY_MODULE_MOUSE_JIGGLER_ENABLE
#    include "mouse_jiggler.h"
#endif // COMMUNITY_MODULE_MOUSE_JIGGLER_ENABLE

static uint16_t mouse_debounce_timer = 0;

#ifdef TAPPING_TERM_PER_KEY
#    define TAP_CHECK get_tapping_term(QK_MOUSE_BUTTON_1, NULL)
#else // TAPPING_TERM_PER_KEY
#    ifndef TAPPING_TERM
#        define TAPPING_TERM 200
#    endif
#    define TAP_CHECK TAPPING_TERM
#endif // TAPPING_TERM_PER_KEY

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

#ifdef COMMUNITY_MODULE_MOUSE_JIGGLER_ENABLE
    if (userspace_config.pointing.mouse_jiggler.enable) {
        jiggler_enable();
    } else {
        jiggler_disable();
    }
    jiggler_set_backoff(userspace_config.pointing.mouse_jiggler.timeout);
#endif // COMMUNITY_MODULE_MOUSE_JIGGLER_ENABLE
    pointing_device_init_keymap();
}

__attribute__((weak)) report_mouse_t pointing_device_task_keymap(report_mouse_t mouse_report) {
    return mouse_report;
}

report_mouse_t pointing_device_task_user(report_mouse_t mouse_report) {
    if (timer_elapsed(mouse_debounce_timer) < TAP_CHECK) {
        mouse_report.x = 0;
        mouse_report.y = 0;
    }

    return pointing_device_task_keymap(mouse_report);
}

bool process_record_pointing(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
        default:
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

bool has_mouse_report_changed(report_mouse_t *new_report, report_mouse_t *old_report) {
    // memcmp doesn't work here because of the `report_id` field when using
    // shared mouse endpoint
    bool changed = ((new_report->buttons != old_report->buttons) ||
#ifdef MOUSE_EXTENDED_REPORT
                    (new_report->boot_x != 0 && new_report->boot_x != old_report->boot_x) ||
                    (new_report->boot_y != 0 && new_report->boot_y != old_report->boot_y) ||
#endif
                    (new_report->x != 0 && new_report->x != old_report->x) ||
                    (new_report->y != 0 && new_report->y != old_report->y) ||
                    (new_report->h != 0 && new_report->h != old_report->h) ||
                    (new_report->v != 0 && new_report->v != old_report->v));

    return changed;
}

#if defined(POINTING_DEVICE_AUTO_MOUSE_ENABLE)
__attribute__((weak)) bool is_mouse_record_keymap(uint16_t keycode, keyrecord_t *record) {
    return false;
}

bool is_mouse_record_user(uint16_t keycode, keyrecord_t *record) {
    if (is_mouse_record_keymap(keycode, record)) {
        return true;
    }
    switch (keycode) {
        case MOUSE_KEYCODE_RANGE:
            return true;
#    if defined(KEYBOARD_ploopy)
        case DPI_CONFIG:
#    elif (defined(KEYBOARD_bastardkb_charybdis) || defined(KEYBOARD_handwired_tractyl_manuform)) && \
            !defined(NO_CHARYBDIS_KEYCODES) ||                                                       \
        (defined(KEYBOARD_bastardkb_dilemma) && !defined(NO_DILEMMA_KEYCODES))
        case QK_KB ... QK_KB_MAX:
#    endif
#    ifdef COMMUNITY_MODULE_MOUSE_JIGGLER_ENABLE
        case COMMUNITY_MODULE_MOUSE_JIGGLER_TOGGLE ... COMMUNITY_MODULE_MOUSE_JIGGLER_AUTOSTOP:
#    endif // COMMUNITY_MODULE_MOUSE_JIGGLER_ENABLE
            return true;
    }
    return false;
}
#endif

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
