// Copyright Pablo Martinez (@elpekenin) <elpekenin@elpekenin.dev>
// SPDX-License-Identifier: GPL-2.0-or-later

#include "elpekenin/indicators.h"

#include <quantum/quantum.h>
#include "rgb_matrix.h"

static bool should_draw_indicator(const indicator_t *indicator, const indicator_fn_args_t *args) {
    if (indicator->flags & LAYER && indicator->conditions.layer != args->layer) {
        return false;
    }

    if (indicator->flags & KEYCODE && indicator->conditions.keycode != args->keycode) {
        return false;
    }

    if (indicator->flags & MODS && !(indicator->conditions.mods & args->mods)) {
        return false;
    }

    if (indicator->flags & KC_GT_THAN && indicator->conditions.keycode >= args->keycode) {
        return false;
    }

    return true;
}

bool rgb_matrix_indicators_advanced_indicators(uint8_t led_min, uint8_t led_max) {
    if (!rgb_matrix_indicators_advanced_indicators_kb(led_min, led_max)) {
        return false;
    }

    uint8_t mods  = get_mods();
    uint8_t layer = get_highest_layer(layer_state);

    indicator_fn_args_t args = {
        .mods  = mods,
        .layer = layer,
    };

    // iterate all keys
    for (int8_t row = 0; row < MATRIX_ROWS; ++row) {
        for (int8_t col = 0; col < MATRIX_COLS; ++col) {
            uint8_t index = g_led_config.matrix_co[row][col];

            // early exit if out of range
            if (index < led_min || index >= led_max) {
                continue;
            }

            args.led_index = index;
            args.keycode   = keymap_key_to_keycode(layer, (keypos_t){col, row});

            // iterate all indicators
            for (int8_t i = 0; i < indicators_count(); ++i) {
                const indicator_t indicator = get_indicator(i);

                if (should_draw_indicator(&indicator, &args)) {
                    rgb_matrix_set_color(args.led_index, indicator.color.r, indicator.color.g, indicator.color.b);
                }
            }
        }
    }

    return false;
}
