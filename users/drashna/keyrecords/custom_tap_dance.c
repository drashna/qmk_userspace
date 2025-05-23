// Copyright 2020 Christopher Courtney, aka Drashna Jael're  (@drashna) <drashna@live.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "custom_tap_dance.h"
#include "util.h"
#include "action_layer.h"
#include "drashna_layers.h"

#define NUM_OF_DIABLO_KEYS 4
// define diablo macro timer variables
diablo_timer_t diablo_timer[NUM_OF_DIABLO_KEYS];

// Set the default intervals.  Always start with 0 so that it will disable on first hit.
// Otherwise, you will need to hit a bunch of times, or hit the "clear" command
uint8_t diablo_times[] = {0, 1, 3, 5, 10, 30};

/**
 * @brief Main function for handling disable related tap dances.
 *
 * @param state Main data struction containing information about events
 * @param user_data Local data for the dance. Allows customization to be passed on to function
 */
void diablo_tapdance_master(tap_dance_state_t *state, void *user_data) {
    diable_keys_t *diablo_keys = (diable_keys_t *)user_data;
    // Sets the keycode based on the index
    diablo_timer[diablo_keys->index].keycode = diablo_keys->keycode;

    // if the tapdance is hit more than the number of elemints in the array, reset
    if (state->count >= ARRAY_SIZE(diablo_times)) {
        diablo_timer[diablo_keys->index].key_interval = 0;
        reset_tap_dance(state);
    } else { // else set the interval (tapdance count starts at 1, array starts at 0, so offset by one)
        diablo_timer[diablo_keys->index].key_interval = diablo_times[state->count - 1];
    }
}

// clang-format off
// One function to rule them all!!  Where the Magic Sauce lies
#define ACTION_TAP_DANCE_DIABLO(index, keycode) {  \
    .fn = { NULL, (void *)diablo_tapdance_master, NULL }, \
    .user_data = (void *)&((diable_keys_t) { index, keycode }),  \
  }
// clang-format on

// Tap Dance Definitions, sets the index and the keycode.
tap_dance_action_t tap_dance_actions[] = {
    // tap once to disable, and more to enable timed micros
    [TD_D3_1] = ACTION_TAP_DANCE_DIABLO(0, KC_1),
    [TD_D3_2] = ACTION_TAP_DANCE_DIABLO(1, KC_2),
    [TD_D3_3] = ACTION_TAP_DANCE_DIABLO(2, KC_3),
    [TD_D3_4] = ACTION_TAP_DANCE_DIABLO(3, KC_4),
};

/**
 * @brief Runs check to see if timer has elapsed for each dance, and sends keycodes, if it has.
 *
 */
void run_diablo_macro_check(void) {
    for (uint8_t index = 0; index < NUM_OF_DIABLO_KEYS; index++) {
        // if key_interval is 0, it's disabled, so only run if it's set.  If it's set, check the timer.
        if (diablo_timer[index].key_interval &&
            timer_elapsed(diablo_timer[index].timer) > (diablo_timer[index].key_interval * 1000)) {
            // reset the timer, since enough time has passed
            diablo_timer[index].timer = timer_read();
            // send keycode ONLY if we're on the diablo layer.
            if (layer_state_is(_DIABLO)) {
                tap_code(diablo_timer[index].keycode);
            }
        }
    }
}
