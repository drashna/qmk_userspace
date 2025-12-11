// Copyright 2023 Christopher Courtney, aka Drashna Jael're  (@drashna) <drashna@live.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "drashna.h"

// USB Port Pin Definitions
// black - GND
// red - VCC
// white - D- - A11 - B14
// blue - D+ - A12 - B15
// green - CC1
// yellow - CC2

#if defined(KEYBOARD_handwired_tractyl_manuform_5x6_right_f405)
#    include "keymap_full.h"
#else // KEYBOARD_handwired_tractyl_manuform_5x6_right_f405
#    include "keymap_regular.h"
#endif // KEYBOARD_handwired_tractyl_manuform_5x6_right_f405

// clang-format off
#ifdef COMMUNITY_MODULE_LAYER_MAP_ENABLE
keypos_t layer_remap[LAYER_MAP_ROWS][LAYER_MAP_COLS] = {
    { {   0,   0 }, {   1,   0 }, {   2,   0 }, {   3,   0 }, {   4,   0 }, {   5,   0 }, { 255, 255 }, { 255, 255 }, { 255, 255 }, { 255, 255 }, { 255, 255 }, {   0,   6 }, {   1,   6 }, {   2,   6 }, {   3,   6 }, {   4,   6 }, {   5,   6 } },
    { {   0,   1 }, {   1,   1 }, {   2,   1 }, {   3,   1 }, {   4,   1 }, {   5,   1 }, { 255, 255 }, { 255, 255 }, { 255, 255 }, { 255, 255 }, { 255, 255 }, {   0,   7 }, {   1,   7 }, {   2,   7 }, {   3,   7 }, {   4,   7 }, {   5,   7 } },
    { {   0,   2 }, {   1,   2 }, {   2,   2 }, {   3,   2 }, {   4,   2 }, {   5,   2 }, { 255, 255 }, { 255, 255 }, { 255, 255 }, { 255, 255 }, { 255, 255 }, {   0,   8 }, {   1,   8 }, {   2,   8 }, {   3,   8 }, {   4,   8 }, {   5,   8 } },
    { {   0,   3 }, {   1,   3 }, {   2,   3 }, {   3,   3 }, {   4,   3 }, {   5,   3 }, { 255, 255 }, { 255, 255 }, { 255, 255 }, { 255, 255 }, { 255, 255 }, {   0,   9 }, {   1,   9 }, {   2,   9 }, {   3,   9 }, {   4,   9 }, {   5,   9 } },
    { {   0,   4 }, {   1,   4 }, {   2,   4 }, {   3,   4 }, { 255, 255 }, { 255, 255 }, { 255, 255 }, { 255, 255 }, { 255, 255 }, { 255, 255 }, { 255, 255 }, { 255, 255 }, { 255, 255 }, {   2,  10 }, {   3,  10 }, {   4,  10 }, {   5,  10 } },
    { { 255, 255 }, { 255, 255 }, {   0, 252 }, { 255, 255 }, {   4,   4 }, {   5,   4 }, { 255, 255 }, { 255, 255 }, { 255, 255 }, { 255, 255 }, { 255, 255 }, { 255, 255 }, {   1,  10 }, { 255, 255 }, {   1, 252 }, { 255, 255 }, { 255, 255 } },
    { { 255, 255 }, { 255, 255 }, {   0, 253 }, { 255, 255 }, { 255, 255 }, { 255, 255 }, {   5,   5 }, {   3,   5 }, { 255, 255 }, {   2,  11 }, { 255, 255 }, { 255, 255 }, { 255, 255 }, { 255, 255 }, {   1, 253 }, { 255, 255 }, { 255, 255 } },
    { { 255, 255 }, { 255, 255 }, { 255, 255 }, { 255, 255 }, { 255, 255 }, { 255, 255 }, {   4,   5 }, {   2,   5 }, { 255, 255 }, { 255, 255 }, {   3,  11 }, {   1,  11 }, { 255, 255 }, { 255, 255 }, { 255, 255 }, { 255, 255 }, { 255, 255 } },
};
#endif

#ifdef ENCODER_MAP_ENABLE
const uint16_t PROGMEM encoder_map[][NUM_ENCODERS][NUM_DIRECTIONS] = {
    [_DEFAULT_LAYER_1] = { ENCODER_CCW_CW( KC_UP,   KC_DOWN ), ENCODER_CCW_CW( KC_LEFT, KC_RGHT ) },
    [_DEFAULT_LAYER_2] = { ENCODER_CCW_CW( _______, _______ ), ENCODER_CCW_CW( _______, _______ ) },
    [_DEFAULT_LAYER_3] = { ENCODER_CCW_CW( _______, _______ ), ENCODER_CCW_CW( _______, _______ ) },
    [_DEFAULT_LAYER_4] = { ENCODER_CCW_CW( _______, _______ ), ENCODER_CCW_CW( _______, _______ ) },
    [_GAMEPAD]         = { ENCODER_CCW_CW( _______, _______ ), ENCODER_CCW_CW( _______, _______ ) },
    [_DIABLO]          = { ENCODER_CCW_CW( _______, _______ ), ENCODER_CCW_CW( _______, _______ ) },
    [_MOUSE]           = { ENCODER_CCW_CW( MS_WHLD, MS_WHLU ), ENCODER_CCW_CW( MS_WHLL, MS_WHLR ) },
    [_MEDIA]           = { ENCODER_CCW_CW( _______, _______ ), ENCODER_CCW_CW( _______, _______ ) },
    [_RAISE]           = { ENCODER_CCW_CW( KC_UP,   KC_DOWN ), ENCODER_CCW_CW( KC_LEFT, KC_RGHT ) },
    [_LOWER]           = { ENCODER_CCW_CW( KC_PGUP, KC_PGDN ), ENCODER_CCW_CW( UG_PREV, UG_NEXT ) },
    [_ADJUST]          = { ENCODER_CCW_CW( RM_HUEU, RM_HUED ), ENCODER_CCW_CW( RM_PREV, RM_NEXT ) },
};
#endif
// clang-format on

void housekeeping_task_keymap(void) {
#ifdef PORTSCAN_MATRIX_ENABLE
    void matrix_wait_for_interrupt(void);
    // Go into low-scan interrupt-based mode if we haven't had any matrix activity in the last 250 milliseconds
    if (last_input_activity_elapsed() > 250) {
        matrix_wait_for_interrupt();
    }
#endif
}

#ifndef PORTSCAN_MATRIX_ENABLE
void matrix_output_select_delay(void) {
    for (uint8_t i = 0; i < 1; i++) {
        __asm__ volatile("nop" ::: "memory");
    }
}
void matrix_output_unselect_delay(uint8_t line, bool key_pressed) {
#    ifdef HEAVY_OPTIMIZATION_ENABLE
    wait_us(1);
#    else
    for (uint8_t i = 0; i < 20; i++) {
        __asm__ volatile("nop" ::: "memory");
    }
#    endif // HEAVY_OPTIMIZATION_ENABLE
}
#endif // PORTSCAN_MATRIX_ENABLE

#ifdef OLED_ENABLE
void render_oled_title(bool side) {
    oled_write_P(side ? PSTR("   Tractyl   ") : PSTR("   Manuform  "), true);
}
#endif
void eeconfig_init_keymap(void) {
    userspace_config.display.oled.rotation = 1; // OLED_ROTATION_180;
}

#if defined(RGB_MATRIX_ENABLE) && defined(RGBLIGHT_ENABLE) && defined(RGBLIGHT_CUSTOM)
const uint8_t led_mapping[RGBLIGHT_LED_COUNT] = {0,  1,  2,  3,  32, 31, 12, 13, 26, 30, 29, 27,
                                                 28, 59, 61, 60, 46, 45, 62, 63, 36, 35, 34, 33};
#endif

#if defined(QUANTUM_PAINTER_ENABLE) && !defined(CUSTOM_QUANTUM_PAINTER_ENABLE)
#    include "qp.h"
#    include "display/painter/graphics/assets.h"

static painter_device_t       display;
static painter_image_handle_t my_image;

void keyboard_post_init_keymap(void) {
    display =
        qp_ili9341_make_spi_device(240, 320, DISPLAY_CS_PIN, DISPLAY_DC_PIN, DISPLAY_RST_PIN, DISPLAY_SPI_DIVIDER, 0);
    qp_init(display, QP_ROTATION_180);
    qp_clear(display);
    qp_power(display, true);
    if (is_keyboard_master()) {
        my_image = qp_load_image_mem(gfx_anime_girl_jacket_240x320);
        qp_drawimage_recolor(display, 0, 0, my_image, true ? 213 : 0, 0, 255, 0, 0, 0);
    } else {
        my_image = qp_load_image_mem(gfx_samurai_cyberpunk_minimal_dark_8k_b3_240x320);
        qp_drawimage(display, 0, 0, my_image);
    }
    qp_flush(display);
    // my_image = qp_load_image_mem(gfx_neon_genesis_evangelion_initial_machine_02_240x320);
    // my_image = qp_load_image_mem(gfx_asuka_240x320);
    qp_close_image(my_image);
}
#endif // QUANTUM_PAINTER_ENABLE &&  !CUSTOM_QUANTUM_PAINTER_ENABLE

#ifdef USE_USB_OTG_HS_PORT
#    pragma message("Using USB OTG HS Port")
void early_hardware_init_post(void) {
    // D-  white
    palSetLineMode(
        B14, PAL_MODE_ALTERNATE(12) | PAL_STM32_OTYPE_PUSHPULL | PAL_STM32_OSPEED_HIGHEST | PAL_STM32_PUPDR_FLOATING);
    // D+  blue
    palSetLineMode(
        B15, PAL_MODE_ALTERNATE(12) | PAL_STM32_OTYPE_PUSHPULL | PAL_STM32_OSPEED_HIGHEST | PAL_STM32_PUPDR_FLOATING);
}
#endif

#ifdef KEYCODE_STRING_ENABLE
// clang-format off
KEYCODE_STRING_NAMES_KB(
    {QK_KB_0, "DPI_INC"},
    {QK_KB_1, "DPI_DEC"},
    {QK_KB_2, "SNI_INC"},
    {QK_KB_3, "SNI_DEC"},
    {QK_KB_4, "SNIP_MO"},
    {QK_KB_5, "SNIP_TG"},
    {QK_KB_6, "DRAG_MO"},
    {QK_KB_7, "DRAG_TG"},
);
// clang-format on
#endif

void execute_user_button_action(bool state) {
    // for the double tap action of the reset pin
    static bool     last_state     = false;
    static uint8_t  tap_counter    = 0;
    static uint16_t reset_timer    = 0;
    static uint16_t debounce_timer = 0;
    bool            reset_pin_fe   = state < last_state && timer_elapsed(debounce_timer) > 50;

    // switch pin on

    if (reset_pin_fe) {
        if (tap_counter == 0) {
            reset_timer = timer_read();
        }
        debounce_timer = timer_read();
        tap_counter++; // count falling edges
    }

    if (timer_elapsed(reset_timer) > 1000) {
        reset_timer = timer_read();
        tap_counter = 0;
    } else {
        if (tap_counter >= 2) {
            dprintf("Bootloader jumps\n");
            if (is_keyboard_master()) {
                reset_keyboard();
            } else {
                soft_reset_keyboard();
            }
        }
    }
    // update last state
    last_state = state;
}
