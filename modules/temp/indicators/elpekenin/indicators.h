// Copyright Pablo Martinez (@elpekenin) <elpekenin@elpekenin.dev>
// SPDX-License-Identifier: GPL-2.0-or-later

/**
 * Declaratively define RGB indicators.
 *
 * Your configuration would look like:
 *
 * .. code-block:: c
 *
 *     const indicator_t PROGMEM indicators[] = {
 *        LAYER_INDICATOR(UTILS, RGB_OFF),
 *        KEYCODE_IN_LAYER_INDICATOR(QK_BOOT, UTILS, RGB_RED),
 *     };
 *
 * Since indicators are checked (and applied) in the same order that you define them, this config is:
 *   * On the layer ``UTILS``, all LEDs will be off except for ``QK_BOOT`` which will be red
 *   * No indicator to be drawn on other layers (running effect left unchanged)
 */

// -- barrier --

#pragma once

#include <quantum/quantum.h>

/**
 * .. hint::
 *    In general, you will use the :ref:`convenience macros <indicator-macros>`.
 *    However, you might need to manually instantiate these data structures to define custom conditions.
 */

/**
 * State about the LED being checked.
 *
 * Used to tell whether the indicator's color should be applied.
 */
typedef struct {
    /**
     * Index of the LED.
     */
    uint8_t led_index;
    /**
     * Highest active layer.
     */
    uint8_t layer;
    /**
     * Keycode currently mapped to the key where LED belongs.
     */
    uint16_t keycode;
    /**
     * Active modifiers (bitmask).
     */
    uint8_t mods;
} indicator_fn_args_t;

/**
 * Different conditions to be checked
 */
typedef enum {
    /**
     * Highest active layer is ``<X>``.
     */
    LAYER = (1 << 0),
    /**
     * Keycode is exactly ``<X>``.
     */
    KEYCODE = (1 << 1),
    /**
     * Modifiers ``<X>`` are active (not an exact match, others can be active too).
     */
    MODS = (1 << 2),
    /**
     * Keycode is greater than ``<X>``.
     */
    KC_GT_THAN = (1 << 3),
} indicator_flags_t;

/**
 * An indicator's specification:
 */
typedef struct PACKED {
    /**
     * Color to be applied if conditions are fulfilled.
     */
    rgb_t color;
    /**
     * Which conditions have to be checked.
     */
    indicator_flags_t flags;
    /**
     * Values used to check (the ``<X>``\s above).
     */
    indicator_fn_args_t conditions;
} indicator_t;

/**
 * .. _indicator-macros:
 *
 * Indicator on any key mapped to the given keycode.
 *
 * Args:
 *     _keycode: Value of the keycode.
 *     rgb: Color to be applied.
 */
#define KEYCODE_INDICATOR(_keycode, rgb)  \
    (indicator_t) {                       \
        .color = {rgb}, .flags = KEYCODE, \
        .conditions = {                   \
            .keycode = _keycode,          \
        },                                \
    }

/**
 * Indicator for all LEDs in the given layer.
 *
 * Args:
 *     _layer: Where the indicator should fire.
 *     rgb: Color to be applied.
 */
#define LAYER_INDICATOR(_layer, rgb)    \
    (indicator_t) {                     \
        .color = {rgb}, .flags = LAYER, \
        .conditions = {                 \
            .layer = _layer,            \
        },                              \
    }

/**
 * Indicator on any key mapped to the given keycode in the given layer.
 *
 * Args:
 *     _keycode: Value of the keycode.
 *     _layer: Where the indicator should fire.
 *     rgb: Color to be applied.
 */
#define KEYCODE_IN_LAYER_INDICATOR(_keycode, _layer, rgb) \
    (indicator_t) {                                       \
        .color = {rgb}, .flags = KEYCODE | LAYER,         \
        .conditions = {                                   \
            .keycode = _keycode,                          \
            .layer   = _layer,                            \
        },                                                \
    }

/**
 * Indicator on any key that has been mapped in the given layer (ie: is not ``KC_NO`` nor ``KC_TRNS``) .
 *
 * Args:
 *     _layer: Where the indicator should fire.
 *     rgb: Color to be applied.
 */
#define ASSIGNED_KEYCODE_IN_LAYER_INDICATOR(_layer, rgb) \
    (indicator_t) {                                      \
        .color = {rgb}, .flags = LAYER | KC_GT_THAN,     \
        .conditions = {                                  \
            .keycode = KC_TRNS,                          \
            .layer   = _layer,                           \
        },                                               \
    }

/**
 * Indicator on any key mapped to the given keycode while mods are active.
 *
 * Args:
 *     _keycode: Value of the keycode.
 *     mod_mask: Bitmask of the modifiers that must be active.
 *     rgb: Color to be applied.
 */
#define KEYCODE_WITH_MOD_INDICATOR(_keycode, mod_mask, rgb) \
    (indicator_t) {                                         \
        .color = {rgb}, .flags = KEYCODE | MODS,            \
        .conditions = {                                     \
            .keycode = _keycode,                            \
            .mods    = mod_mask,                            \
        },                                                  \
    }

/**
 * Indicator on any key mapped to a custom keycode in the given layer.
 *
 * Args:
 *     _layer: Where the indicator should fire.
 *     rgb: Color to be applied.
 */
#define CUSTOM_KEYCODE_IN_LAYER_INDICATOR(_layer, rgb) \
    (indicator_t) {                                    \
        .color = {rgb}, .flags = LAYER | KC_GT_THAN,   \
        .conditions = {                                \
            .keycode = QK_USER,                        \
            .layer   = _layer,                         \
        },                                             \
    }

/**
 * Check conditions, drawing the indicator if they met, for all leds in the given range.
 *
 * Args:
 *     led_min: First LED to be drawn.
 *     led_max: Last LED to be drawn.
 *
 * .. warning::
 *    Since QMK does not (yet?) allow to hook modules into the RGB system,
 *    you need to call this function from within ``rgb_matrix_indicators_advanced_user``
 */
bool draw_indicators(uint8_t led_min, uint8_t led_max);

// Not intended to be used by users -> no docstring
uint8_t indicators_count(void);

indicator_t get_indicator(uint8_t i);
