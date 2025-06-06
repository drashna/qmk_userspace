// Copyright 2025 Christopher Courtney, aka Drashna Jael're  (@drashna) <drashna@live.com>
// SPDX-License-Identifier: GPL-3.0-or-later

RGB_MATRIX_EFFECT(alpha_mod_homerow)
RGB_MATRIX_EFFECT(alpha_mod_homerow_color_shift)

#ifdef RGB_MATRIX_CUSTOM_EFFECT_IMPLS

static bool alpha_mod_homerow_runner(effect_params_t* params, bool shift) {
    RGB_MATRIX_USE_LIMITS(led_min, led_max);

    HSV hsv = rgb_matrix_config.hsv;

    // homerow color is based on saturation, grab it and then override it with 100
    HSV hsv_for_homerow = rgb_matrix_config.hsv;
    hsv_for_homerow.h += ((rgb_matrix_config.speed + hsv.s + 100) % 255);

    // Now set the saturation to 100 for both
    hsv.s             = 255;
    hsv_for_homerow.s = 255;

    RGB rgb1 = rgb_matrix_hsv_to_rgb(hsv);
    hsv.h += rgb_matrix_config.speed;
    RGB rgb2 = rgb_matrix_hsv_to_rgb(hsv);
    RGB rgb3 = rgb_matrix_hsv_to_rgb(hsv_for_homerow);

    for (uint8_t i = led_min; i < led_max; i++) {
        RGB_MATRIX_TEST_LED_FLAGS();
        if ((HAS_FLAGS(g_led_config.flags[i], LED_FLAG_KEYLIGHT) &&
             HAS_FLAGS(g_led_config.flags[i], LED_FLAG_INDICATOR)) ||
            (HAS_FLAGS(g_led_config.flags[i], LED_FLAG_MODIFIER) &&
             HAS_FLAGS(g_led_config.flags[i], LED_FLAG_INDICATOR))) {
            rgb_matrix_set_color(i, rgb3.r, rgb3.g, rgb3.b);
        } else if (HAS_FLAGS(g_led_config.flags[i], LED_FLAG_MODIFIER)) {
            rgb_matrix_set_color(i, rgb2.r, rgb2.g, rgb2.b);
        } else {
            rgb_matrix_set_color(i, rgb1.r, rgb1.g, rgb1.b);
        }
    }

    return rgb_matrix_check_finished_leds(led_max);
}

static bool alpha_mod_homerow(effect_params_t* params) {
    return alpha_mod_homerow_runner(params, false);
}

static bool alpha_mod_homerow_color_shift(effect_params_t* params) {
    return alpha_mod_homerow_runner(params, true);
}

#endif // RGB_MATRIX_CUSTOM_EFFECT_IMPLS
