// Copyright 2024 Christopher Courtney, aka Drashna Jael're  (@drashna) <drashna@live.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "display/painter/painter.h"
#include <stdio.h>
#include "drashna_names.h"
#include "drashna_runtime.h"
#include "drashna_util.h"
#include "version.h"
#include "hardware_id_string.h"
#include "keyrecords/process_records.h"
#include <lib/lib8tion/lib8tion.h>

#ifdef SPLIT_KEYBOARD
#    include "split_util.h"
#endif // SPLIT_KEYBOARD
#if CUSTOM_QUANTUM_PAINTER_ILI9341
#    include "display/painter/ili9341_display.h"
#endif // CUSTOM_QUANTUM_PAINTER_ILI9341
#if defined(CUSTOM_QUANTUM_PAINTER_ILI9488)
#    include "display/painter/ili9488_display.h"
#endif // CUSTOM_QUANTUM_PAINTER_ILI9488
#if defined(CUSTOM_QUANTUM_PAINTER_ST7789_135X240)
#    include "display/painter/st7789_135x240.h"
#endif // CUSTOM_QUANTUM_PAINTER_ST7789_135X240
#if defined(CUSTOM_QUANTUM_PAINTER_ST7789_170X320)
#    include "display/painter/st7789_170x320.h"
#endif // CUSTOM_QUANTUM_PAINTER_ST7789_170X320
#if defined(CUSTOM_QUANTUM_PAINTER_ST7789_76X284)
#    include "display/painter/st7789_76x284.h"
#endif // CUSTOM_QUANTUM_PAINTER_ST7789_76X284
#ifdef CUSTOM_QUANTUM_PAINTER_SH1107_128X128
#    include "display/painter/sh1107_128x128.h"
#endif // CUSTOM_QUANTUM_PAINTER_SH1107_128X128

#if defined(RGB_MATRIX_ENABLE)
#    include "rgb/rgb_matrix_stuff.h"
#endif // defined(RGB_MATRIX_ENABLE)
#if defined(RGBLIGHT_ENABLE)
#    include "rgb/rgb_stuff.h"
#endif // defined(RGBLIGHT_ENABLE)
#if defined(DISPLAY_KEYLOGGER_ENABLE)
#    include "display/painter/keylogger.h"
#endif // DISPLAY_KEYLOGGER_ENABLE
#ifdef COMMUNITY_MODULE_RTC_ENABLE
#    include "rtc.h"
#endif // COMMUNITY_MODULE_RTC_ENABLE
#ifdef COMMUNITY_MODULE_LAYER_MAP_ENABLE
#    include "layer_map.h"
#endif // COMMUNITY_MODULE_LAYER_MAP_ENABLE
#ifdef COMMUNITY_MODULE_DISPLAY_MENU_ENABLE
#    include "qp_render_menu.h"
#else
void display_menu_set_dirty(bool state) {}
#endif
#ifdef COMMUNITY_MODULE_POINTING_DEVICE_ACCEL_ENABLE
#    include "pointing_device_accel.h"
#endif // COMMUNITY_MODULE_POINTING_DEVICE_ACCEL_ENABLE
#ifdef COMMUNITY_MODULE_QP_HELPERS_ENABLE
#    include "qp_helpers.h"
#endif
#ifdef COMMUNITY_MODULE_QP_HELPERS_ENABLE
#    include "qp_helpers.h"
#endif // COMMUNITY_MODULE_QP_HELPERS_ENABLE

painter_font_handle_t font_thintel, font_mono, font_oled;

painter_image_handle_t lock_caps_on, lock_caps_off;
painter_image_handle_t lock_num_on, lock_num_off;
painter_image_handle_t lock_scrl_on, lock_scrl_off;
painter_image_handle_t windows_logo, apple_logo, linux_logo;
painter_image_handle_t shift_icon, control_icon, alt_icon, command_icon, windows_icon;
painter_image_handle_t mouse_icon;
painter_image_handle_t gamepad_icon;
painter_image_handle_t qmk_banner;
painter_image_handle_t akira_explosion;
painter_image_handle_t nyan_cat;
deferred_token         nyan_token = INVALID_DEFERRED_TOKEN;

#if !defined(PAINTER_SCREENSAVER_NEON_GENESIS) && !defined(PAINTER_SCREENSAVER_CYBERPUNK_2077) && \
    !defined(PAINTER_SCREENSAVER_ANIME)
#    define PAINTER_SCREENSAVER_CYBERPUNK_2077
#endif // !PAINTER_SCREENSAVER_NEON_GENESIS && !PAINTER_SCREENSAVER_CYBERPUNK_2077 && !PAINTER_SCREENSAVER_ANIME

painter_image_array_t screen_saver_image[] = {
#if defined(PAINTER_SCREENSAVER_NEON_GENESIS)
    {gfx_asuka_240x320, "Asuka"},
    {gfx_neon_genesis_evangelion_initial_machine_00_240x320, "Eva Unit 00"},
    {gfx_neon_genesis_evangelion_initial_machine_01_240x320, "Eva Unit 01"},
    {gfx_neon_genesis_evangelion_initial_machine_02_240x320, "Eva Unit 02"},
    {gfx_neon_genesis_evangelion_initial_machine_03_240x320, "Eva Unit 03"},
    {gfx_neon_genesis_evangelion_initial_machine_04_240x320, "Eva Unit 04"},
    {gfx_neon_genesis_evangelion_initial_machine_05_240x320, "Eva Unit 05"},
    {gfx_neon_genesis_evangelion_initial_machine_06_240x320, "Eva Unit 06"},
#endif // PAINTER_SCREENSAVER_NEON_GENESIS
#if defined(PAINTER_SCREENSAVER_CYBERPUNK_2077)
    {gfx_samurai_cyberpunk_minimal_dark_8k_b3_240x320, "Samurai Cyberpunk"},
#endif // PAINTER_SCREENSAVER_CYBERPUNK_2077
#if defined(PAINTER_SCREENSAVER_ANIME)
    {gfx_anime_girl_jacket_240x320, "Anime Girl"},
#endif // PAINTER_SCREENSAVER_ANIME
};

const uint8_t screensaver_image_size = ARRAY_SIZE(screen_saver_image);

void painter_render_menu_block_console(painter_device_t device, painter_font_handle_t font, uint16_t x, uint16_t y,
                                       uint16_t width, uint16_t height, bool force_redraw, dual_hsv_t *curr_hsv) {
    painter_render_console(device, font, x + 2, y + 2, width, force_redraw, &curr_hsv->primary,
                           DISPLAY_CONSOLE_LOG_LINE_START, DISPLAY_CONSOLE_LOG_LINE_NUM);
}

void painter_render_menu_block_fonts(painter_device_t device, painter_font_handle_t font, uint16_t x, uint16_t y,
                                     uint16_t width, uint16_t height, bool force_redraw, dual_hsv_t *curr_hsv) {
    static uint16_t              max_font_xpos[3][4] = {0};
    extern painter_font_handle_t font_thintel, font_mono, font_oled;
    x += 3;
    y += 2;
    render_character_set(device, &x, max_font_xpos[0], &y, font_thintel, curr_hsv->primary.h, curr_hsv->primary.s,
                         curr_hsv->primary.v, 0, 0, 0);
    render_character_set(device, &x, max_font_xpos[1], &y, font_mono, curr_hsv->primary.h, curr_hsv->primary.s,
                         curr_hsv->primary.v, 0, 0, 0);
    render_character_set(device, &x, max_font_xpos[2], &y, font_oled, curr_hsv->primary.h, curr_hsv->primary.s,
                         curr_hsv->primary.v, 0, 0, 0);
}

void painter_render_menu_block_qmk_info(painter_device_t device, painter_font_handle_t font, uint16_t x, uint16_t y,
                                        uint16_t width, uint16_t height, bool force_redraw, dual_hsv_t *curr_hsv) {
    painter_render_qmk_info(device, font, x, y + 5, width, force_redraw, curr_hsv);
}

void painter_render_menu_block_nyan_cat(painter_device_t device, painter_font_handle_t font, uint16_t x, uint16_t y,
                                        uint16_t width, uint16_t height, bool force_redraw, dual_hsv_t *curr_hsv) {
    if (nyan_token == INVALID_DEFERRED_TOKEN) {
        nyan_token =
            qp_animate(device, x + (width - nyan_cat->width) / 2, y + (height - nyan_cat->height) / 2, nyan_cat);
    }
}

void painter_render_menu_block_game_of_life(painter_device_t device, painter_font_handle_t font, uint16_t x, uint16_t y,
                                            uint16_t width, uint16_t height, bool force_redraw, dual_hsv_t *curr_hsv) {
    render_life(device, x, y, curr_hsv, force_redraw);
}

void painter_render_menu_block_layer_map(painter_device_t device, painter_font_handle_t font, uint16_t x, uint16_t y,
                                         uint16_t width, uint16_t height, bool force_redraw, dual_hsv_t *curr_hsv) {
    painter_render_layer_map(device, font, x, y, width, force_redraw, curr_hsv);
}

void painter_render_menu_block_pd_accel_graph(painter_device_t device, painter_font_handle_t font, uint16_t x,
                                              uint16_t y, uint16_t width, uint16_t height, bool force_redraw,
                                              dual_hsv_t *curr_hsv) {
    painter_render_pd_accel_graph(device, x, y, width, height, force_redraw, curr_hsv);
}

painter_display_menu_block_mode_t painter_display_menu_block_modes[] = {
    {painter_render_menu_block_console, "Console"},
    {painter_render_menu_block_fonts, "Fonts"},
    {painter_render_menu_block_qmk_info, "QMK Info"},
    {painter_render_menu_block_nyan_cat, "Nyan Cat"},
    {painter_render_menu_block_game_of_life, "Game of Life"},
#ifdef COMMUNITY_MODULE_LAYER_MAP_ENABLE
    {painter_render_menu_block_layer_map, "Layer Map"},
#endif // COMMUNITY_MODULE_LAYER_MAP_ENABLE
#ifdef COMMUNITY_MODULE_POINTING_DEVICE_ACCEL_ENABLE
    {painter_render_menu_block_pd_accel_graph, "PD Accel Curve"},
#endif // COMMUNITY_MODULE_POINTING_DEVICE_ACCEL_ENABLE
};

const uint8_t painter_display_menu_block_modes_count = ARRAY_SIZE(painter_display_menu_block_modes);

#if defined(RGB_MATRIX_ENABLE) || defined(RGBLIGHT_ENABLE)
bool rgb_redraw = false;
#endif

__attribute__((weak)) bool painter_render_side(void) {
    return !is_keyboard_left();
}

/**
 * @brief Initializes the assets used by the painter module.
 *
 * This function loads various fonts and images into memory for use in the
 * painter module. The assets include fonts, frames, logos, and icons.
 *
 */
void painter_init_assets(void) {
    font_thintel = qp_load_font_mem(font_thintel15);
    font_mono    = qp_load_font_mem(font_ProggyTiny15);
    font_oled    = qp_load_font_mem(font_oled_font);

    windows_logo = qp_load_image_mem(gfx_windows_logo);
    apple_logo   = qp_load_image_mem(gfx_apple_logo);
    linux_logo   = qp_load_image_mem(gfx_linux_logo);

    shift_icon   = qp_load_image_mem(gfx_shift_icon);
    control_icon = qp_load_image_mem(gfx_control_icon);
    alt_icon     = qp_load_image_mem(gfx_alt_icon);
    command_icon = qp_load_image_mem(gfx_command_icon);
    windows_icon = qp_load_image_mem(gfx_windows_icon);

    mouse_icon      = qp_load_image_mem(gfx_mouse_icon);
    gamepad_icon    = qp_load_image_mem(gfx_gamepad_24x24);
    qmk_banner      = qp_load_image_mem(gfx_qmk_powered_by);
    akira_explosion = qp_load_image_mem(gfx_akira_explosion);

    nyan_cat = qp_load_image_mem(gfx_large_nyan_cat);
}

/**
 * @brief Renders RTC Time to display
 *
 * @param device screen to render to
 * @param font font to use
 * @param x x position to start rendering
 * @param y y position to start rendering
 * @param display_width maximum width for rendering
 * @param force_redraw do we need to redraw regardless of time
 * @param rtc_timer timer to check for redraw
 * @param hsv hsv value to render with
 */
void painter_render_rtc_time(painter_device_t device, painter_font_handle_t font, uint16_t x, uint16_t y,
                             uint16_t display_width, bool force_redraw, uint16_t *rtc_timer, hsv_t *hsv) {
#ifdef COMMUNITY_MODULE_RTC_ENABLE

    bool rtc_redraw = false;
    if (timer_elapsed(*rtc_timer) > 125 && rtc_is_connected()) {
        *rtc_timer = timer_read();
        rtc_redraw = true;
    }
    if (force_redraw || rtc_redraw) {
        char buf[40] = {0};
        if (rtc_is_connected()) {
            snprintf(buf, sizeof(buf), "RTC Date/Time: %s", rtc_read_date_time_str());
        } else {
            snprintf(buf, sizeof(buf), "RTC Device Not Connected");
        }

        uint16_t total_width = display_width - 6 - x;
        uint16_t title_width = qp_textwidth(font, buf);
        if (title_width > (total_width)) {
            title_width = total_width;
        }
        uint16_t title_xpos = (total_width - title_width) / 2 + x;

        qp_drawtext_recolor(device, title_xpos, y, font, truncate_text(buf, total_width, font, false, false), hsv->h,
                            hsv->s, hsv->v, 0, 0, 0);
    }
#endif // COMMUNITY_MODULE_RTC_ENABLE
}

/**
 * @brief Render the console log to the display
 *
 * @param device device to render ton
 * @param font font to render with
 * @param x x position to start rendering
 * @param y y position to start rendering
 * @param display_width maximum width for rendering
 * @param force_redraw do we forcibly redraw the console log
 * @param hsv colors to render with
 * @param start start line to render
 * @param end last line to render
 */
void painter_render_console(painter_device_t device, painter_font_handle_t font, uint16_t x, uint16_t y,
                            uint16_t display_width, bool force_redraw, hsv_t *hsv, uint8_t start, uint8_t end) {
    if (console_log_needs_redraw || force_redraw) {
        for (uint8_t i = start; i < end; i++) {
            uint16_t xpos =
                x + qp_drawtext_recolor(device, x, y, font, logline_ptrs[i], hsv->h, hsv->s, hsv->v, 0, 0, 0);
            qp_rect(device, xpos, y, display_width, y + font->line_height, 0, 0, 0, true);
            y += font->line_height + 4;
        }
        console_has_redrawn = true;
    }
}

/**
 * @brief Render the matrix scan rate to the display
 *
 * @param device device to render to
 * @param font font to render with
 * @param x x position to start rendering
 * @param y y position to start rendering
 * @param force_redraw do we forcibly redraw the scan rate
 * @param curr_hsv painter colors
 */
void painter_render_scan_rate(painter_device_t device, painter_font_handle_t font, uint16_t x, uint16_t y,
                              bool force_redraw, dual_hsv_t *curr_hsv) {
    static uint32_t last_scan_rate = 0;
    if (last_scan_rate != get_matrix_scan_rate() || force_redraw) {
        last_scan_rate = get_matrix_scan_rate();
        char buf[6]    = {0};
        x += qp_drawtext_recolor(device, x, y, font, "SCANS: ", curr_hsv->primary.h, curr_hsv->primary.s,
                                 curr_hsv->primary.v, 0, 0, 0);
        snprintf(buf, sizeof(buf), "%5lu", get_matrix_scan_rate());
        qp_drawtext_recolor(device, x, y, font, buf, curr_hsv->secondary.h, curr_hsv->secondary.s,
                            curr_hsv->secondary.v, 0, 0, 0);
    }
}

/**
 * @brief Render the RGB Mode and HSV to the display, with a fancy swatch for the HSV color
 *
 * @param device device to render to
 * @param font font to render with
 * @param x x position to start rendering
 * @param y y position to start rendering
 * @param force_redraw do we forcibly redraw the RGB config
 * @param curr_hsv painter colors
 * @param get_rgb_mode
 * @param get_rgb_hsv
 * @param matrix
 */
void painter_render_rgb(painter_device_t device, painter_font_handle_t font, uint16_t x, uint16_t y, bool force_redraw,
                        dual_hsv_t *curr_hsv, const char *title, const char *(*get_rgb_mode)(void),
                        hsv_t (*get_rgb_hsv)(void), bool is_enabled, uint8_t max_val) {
#if defined(RGB_MATRIX_ENABLE) || defined(RGBLIGHT_ENABLE)
    char buf[22] = {0};
    if (force_redraw || rgb_redraw) {
        hsv_t rgb_hsv = get_rgb_hsv();
        qp_drawtext_recolor(device, x, y, font, title, curr_hsv->primary.h, curr_hsv->primary.s, curr_hsv->primary.v, 0,
                            0, 0);
        y += font->line_height + 4;
        snprintf(buf, sizeof(buf), "%21s", truncate_text(is_enabled ? get_rgb_mode() : "Off", 125, font, false, false));
        qp_drawtext_recolor(device, x + 125 - qp_textwidth(font, buf), y, font, buf, curr_hsv->secondary.h,
                            curr_hsv->secondary.s, curr_hsv->secondary.v, 0, 0, 0);

        y += font->line_height + 4;
        x += qp_drawtext_recolor(device, x, y, font, "HSV: ", curr_hsv->primary.h, curr_hsv->primary.s,
                                 curr_hsv->primary.v, 0, 0, 0);
        if (is_enabled) {
            snprintf(buf, sizeof(buf), "%3d, %3d, %3d", rgb_hsv.h, rgb_hsv.s, rgb_hsv.v);
        } else {
            snprintf(buf, sizeof(buf), "%13s", "Off");
        }
        x += qp_drawtext_recolor(device, x, y, font, buf, curr_hsv->secondary.h, curr_hsv->secondary.s,
                                 curr_hsv->secondary.v, 0, 0, 0);
        qp_rect(device, x + 6, y - 1, x + 6 + 10, y + 9, rgb_hsv.h, rgb_hsv.s,
                is_enabled ? (uint8_t)(rgb_hsv.v * 0xFF / max_val) : 0, true);
    }
#endif // defined(RGB_MATRIX_ENABLE)
}

/**
 * @brief Render the Caps Lock, Scroll Lock and Num Lock states to the display
 *
 * @param device device to render to
 * @param font font to draw with
 * @param x x position to start rendering
 * @param y y position to start rendering
 * @param force_redraw do we forcible redraw the lock states
 * @param curr_hsv painter hsv values
 * @param disabled_val disabled entry render value
 */
void painter_render_lock_state(painter_device_t device, painter_font_handle_t font, uint16_t x, uint16_t y,
                               bool force_redraw, dual_hsv_t *curr_hsv, uint8_t disabled_val) {
    static led_t last_led_state = {0};
    if (force_redraw || last_led_state.raw != host_keyboard_led_state().raw) {
        last_led_state.raw = host_keyboard_led_state().raw;
        qp_drawtext_recolor(device, x, y, font, "CAPS",
                            last_led_state.caps_lock ? curr_hsv->secondary.h : curr_hsv->primary.h,
                            last_led_state.caps_lock ? curr_hsv->secondary.s : curr_hsv->primary.s,
                            last_led_state.caps_lock ? curr_hsv->primary.v : disabled_val, 0, 0, 0);
        y += font->line_height + 2;
        qp_drawtext_recolor(device, x, y, font, "SCRL",
                            last_led_state.scroll_lock ? curr_hsv->secondary.h : curr_hsv->primary.h,
                            last_led_state.scroll_lock ? curr_hsv->secondary.s : curr_hsv->primary.s,
                            last_led_state.scroll_lock ? curr_hsv->primary.v : disabled_val, 0, 0, 0);
        y += font->line_height + 2;
        qp_drawtext_recolor(device, x, y, font, " NUM",
                            last_led_state.num_lock ? curr_hsv->secondary.h : curr_hsv->primary.h,
                            last_led_state.num_lock ? curr_hsv->secondary.s : curr_hsv->primary.s,
                            last_led_state.num_lock ? curr_hsv->primary.v : disabled_val, 0, 0, 0);
    }
}

/**
 * @brief Render the keylogger to the display
 *
 * @param device device to render to
 * @param font font to render with
 * @param x x position to start rendering
 * @param y y position to start rendering
 * @param force_redraw do we forcibly redraw the keylogger
 * @param curr_hsv painter colors
 */
void painter_render_wpm(painter_device_t device, painter_font_handle_t font, uint16_t x, uint16_t y, bool force_redraw,
                        dual_hsv_t *curr_hsv) {
#ifdef WPM_ENABLE
    static wpm_sync_data_t last_wpm_update = {0};
    static char            buf[4]          = {0};
    uint16_t               temp_x = x + 4, temp_y = y + 4;
    if (force_redraw || memcmp(&last_wpm_update, &userspace_runtime_state.wpm, sizeof(wpm_sync_data_t)) != 0) {
        memcpy(&last_wpm_update, &userspace_runtime_state.wpm, sizeof(wpm_sync_data_t));
        temp_x += qp_drawtext_recolor(device, temp_x, temp_y, font, "WPM: ", curr_hsv->primary.h, curr_hsv->primary.s,
                                      curr_hsv->primary.v, 0, 0, 0) +
                  5;
        snprintf(buf, sizeof(buf), "%3u", get_current_wpm());
        qp_drawtext_recolor(device, temp_x, temp_y, font, buf, curr_hsv->secondary.h, curr_hsv->secondary.s,
                            curr_hsv->secondary.v, 0, 0, 0);
    }
#endif // WPM_ENABLE
}

/**
 * @brief Render wpm graph to the display
 *
 * @param device device to render to
 * @param font font to render with
 * @param x x position to start rendering
 * @param y y position to start rendering
 * @param force_redraw do we forcibly redraw the keylogger
 * @param curr_hsv painter colors
 */
void painter_render_wpm_graph(painter_device_t device, painter_font_handle_t font, uint16_t x, uint16_t y,
                              uint16_t width, uint16_t height, bool force_redraw, dual_hsv_t *curr_hsv) {
#if defined(WPM_ENABLE) && defined(COMMUNITY_MODULE_QP_HELPERS_ENABLE)
    static uint16_t wpm_timer = 0;

    if (force_redraw || timer_elapsed(wpm_timer) > 1000) {
        wpm_timer = timer_read();
        extern uint8_t     wpm_graph_samples[WPM_GRAPH_SAMPLES];
        const graph_line_t lines[] = {
            {
                .data      = wpm_graph_samples,
                .color     = curr_hsv->secondary,
                .mode      = LINE,
                .max_value = 120,
            },
            GRAPHS_END,
        };

        const graph_config_t config = {
            .device      = device,
            .start       = {.x = x, .y = y},
            .size        = {.x = width, .y = height},
            .axis        = curr_hsv->primary,
            .background  = {.h = 0, .s = 0, .v = 0},
            .data_points = WPM_GRAPH_SAMPLES,
        };

        qp_draw_graph(&config, lines);
    }
#endif // WPM_ENABLE
}

/**
 * @brief Render the haptic feedback settings to the display
 *
 * @param device screen to render to
 * @param font font to render with
 * @param x x position to start rendering
 * @param y y position to start rendering
 * @param force_redraw do we forcibly redraw the haptic feedback settings
 * @param curr_hsv colors to render with
 */
void painter_render_haptic(painter_device_t device, painter_font_handle_t font, uint16_t x, uint16_t y,
                           bool force_redraw, dual_hsv_t *curr_hsv) {
#if defined(HAPTIC_ENABLE)
    char                   buf[22]     = {0};
    static haptic_config_t temp_config = {0};
    extern haptic_config_t haptic_config;
    if (force_redraw || haptic_config.raw != temp_config.raw) {
        temp_config.raw = haptic_config.raw;
        uint16_t temp_x = x;
        temp_x += qp_drawtext_recolor(device, temp_x, y, font, "Haptic Feedback: ", curr_hsv->primary.h,
                                      curr_hsv->primary.s, curr_hsv->primary.v, 0, 0, 0) +
                  4;
        if (haptic_get_enable()) {
            switch (haptic_get_feedback()) {
                case 0:
                    snprintf(buf, sizeof(buf), "%8s", "Press");
                    break;
                case 1:
                    snprintf(buf, sizeof(buf), "%8s", "Both");
                    break;
                case 2:
                    snprintf(buf, sizeof(buf), "%8s", "Release");
                    break;
            }
        } else {
            snprintf(buf, sizeof(buf), "%8s", "Off");
        }
        qp_drawtext_recolor(device, temp_x, y, font, buf, curr_hsv->secondary.h, curr_hsv->secondary.s,
                            curr_hsv->secondary.v, 0, 0, 0);

        y += font->line_height + 4;
        temp_x = x +
                 qp_drawtext_recolor(device, x, y, font, "Mode:", curr_hsv->primary.h, curr_hsv->primary.s,
                                     curr_hsv->primary.v, 0, 0, 0) +
                 4;
        snprintf(buf, sizeof(buf), "%20s",
                 truncate_text(haptic_get_enable() ? get_haptic_drv2605l_effect_name(haptic_get_mode()) : "Off", 120,
                               font, true, false));
        qp_drawtext_recolor(device, temp_x, y, font, buf, curr_hsv->secondary.h, curr_hsv->secondary.s,
                            curr_hsv->secondary.v, 0, 0, 0);
    }
#endif // defined(HAPTIC_ENABLE)
}

/**
 * @brief Renders a TOTP (Time-based One-Time Password) on the specified painter device.
 *
 * @param device The painter device to render on.
 * @param font The font handle to use for rendering the text.
 * @param x The x-coordinate where the rendering should start.
 * @param y The y-coordinate where the rendering should start.
 * @param width The width of the area to render the TOTP.
 * @param force_redraw A boolean flag indicating whether to force a redraw of the TOTP.
 * @param curr_hsv A pointer to the current HSV color values.
 * @param wide_load Render as  "name XXXXXX" if false, or "name: XXX XXX" if true
 */
void painter_render_totp(painter_device_t device, painter_font_handle_t font, uint16_t x, uint16_t y, uint16_t width,
                         bool force_redraw, dual_hsv_t *curr_hsv, bool wide_load) {
#if defined(COMMUNITY_MODULE_RTC_ENABLE) && defined(RTC_TOTP_ENABLE) && __has_include("rtc_secrets.h")
#    include "rtc_secrets.h"
    uint32_t    get_totp_code(const uint8_t *hmackey, const uint8_t keylength, const uint32_t timestep);
    static bool is_rtc_connected = false;
    bool        totp_redraw = false, draw_red_redraw = false;
    if (rtc_is_connected() != is_rtc_connected) {
        is_rtc_connected = rtc_is_connected();
        totp_redraw      = true;
    }
    if (rtc_read_time_struct().unixtime % 30 == 1) {
        totp_redraw = true;
    }
    if (rtc_read_time_struct().unixtime % 30 == 23) {
        totp_redraw = draw_red_redraw = true;
    }
    if (force_redraw || totp_redraw) {
        uint16_t temp_x     = x;
        char     buf[20]    = {0};
        uint16_t text_width = qp_textwidth(font, wide_load ? "WWWWWW: WWW WWW" : "WWWWWW WWWWWW") + 10;
        for (uint8_t i = 0; i < ARRAY_SIZE(totp_pairs); i++) {
            uint32_t code = rtc_is_connected()
                                ? get_totp_code(totp_pairs[i].hmacKey, totp_pairs[i].key_length, totp_pairs[i].timestep)
                                : 0;
            snprintf(buf, sizeof(buf), "%6s%s", totp_pairs[i].name, wide_load ? ": " : " ");
            if ((temp_x + text_width) > (width)) {
                temp_x = x;
                y += font->line_height + 3;
            }
            temp_x += qp_drawtext_recolor(device, temp_x, y, font, buf, curr_hsv->primary.h, curr_hsv->primary.s,
                                          curr_hsv->primary.v, 0, 0, 0);

            if (wide_load) {
                snprintf(buf, sizeof(buf), "%03ld %03ld", code / 1000, code % 1000);
            } else {
                snprintf(buf, sizeof(buf), "%06ld", code);
            }
            temp_x +=
                qp_drawtext_recolor(device, temp_x, y, font, buf, draw_red_redraw ? 0 : curr_hsv->secondary.h,
                                    draw_red_redraw ? 170 : curr_hsv->secondary.s, curr_hsv->secondary.v, 0, 0, 0) +
                10;
        }
    }
#endif
}

void painter_render_frame_box(painter_device_t device, hsv_t hsv, uint16_t x_buffer, uint16_t y_buffer,
                              int16_t x_offset, int16_t y_offset, bool top_indents, bool side_indents) {
    uint16_t width = 0, height = 0;
    qp_get_geometry(device, &width, &height, NULL, NULL, NULL);

    width -= (x_buffer + x_offset);
    height -= (y_buffer + y_offset);

    // draw top of frame
    qp_line(device, x_buffer + 7 + x_offset, y_buffer + y_offset, width - 7 + x_offset, y_buffer + y_offset, hsv.h,
            hsv.s, hsv.v);
    // draw angled bits
    qp_line(device, x_buffer, y_buffer + 6 + y_offset, x_buffer + 6 + x_offset, y_buffer + y_offset, hsv.h, hsv.s,
            hsv.v);
    qp_line(device, width - 7 + x_offset, y_buffer + y_offset, width - 1 + x_offset, 6 + y_buffer + y_offset, hsv.h,
            hsv.s, hsv.v);

    if (top_indents) {
        for (uint8_t line = 0; line < 13; line++) {
            qp_line(device, x_buffer + 14 + line + x_offset, y_buffer + line + y_offset, width - 14 - line + x_offset,
                    y_buffer + line + y_offset, hsv.h, hsv.s, hsv.v);
        }
    }

    // // lines for frame sides
    qp_line(device, x_buffer + x_offset, y_buffer + 7 + y_offset, x_buffer + x_offset, height - (7 + 2) + y_offset,
            hsv.h, hsv.s, hsv.v);
    qp_line(device, width - 1 + x_offset, y_buffer + 7 + y_offset, width - 1 + x_offset, height - (7 + 2) + y_offset,
            hsv.h, hsv.s, hsv.v);

    if (side_indents) {
        for (uint8_t line = 0; line < 8; line++) {
            qp_line(device, x_buffer + line + x_offset, y_buffer + 14 + line + y_offset, x_buffer + line + x_offset,
                    height - 14 - line + y_offset, hsv.h, hsv.s, hsv.v);
            qp_line(device, width - 1 - line + x_offset, y_buffer + 14 + line + y_offset, width - 1 - line + x_offset,
                    height - 14 - line + y_offset, hsv.h, hsv.s, hsv.v);
        }
    }

    // draw angled bits
    qp_line(device, x_buffer + 1 + x_offset, height - 6 - 2 + y_offset, x_buffer + 6 + 1 + x_offset,
            height - 2 + y_offset, hsv.h, hsv.s, hsv.v);
    qp_line(device, width - 7 + x_offset, height - 2 + y_offset, width - 1 + x_offset, height - 6 - 2 + y_offset, hsv.h,
            hsv.s, hsv.v);

    if (top_indents) {
        for (uint8_t line = 0; line < 11; line++) {
            qp_line(device, x_buffer + 14 + line + x_offset, height - 3 - line + y_offset,
                    width - 2 - 14 - line + x_offset, height - 3 - line + y_offset, hsv.h, hsv.s, hsv.v);
        }
    }

    // frame bottom
    qp_line(device, x_buffer + 8 + x_offset, height - 2 + y_offset, width - 8 + x_offset, height - 2 + y_offset, hsv.h,
            hsv.s, hsv.v);
}
/**
 * @brief Renders a frame on the painter device.
 *
 * This function is responsible for rendering a frame on the specified painter device.
 * It uses the provided font for the title and can render on either the right or left side
 * of the display, with an optional offset.
 *
 * @param device The painter device to render the frame on.
 * @param font_title The font handle to use for the title.
 * @param right_side A boolean indicating whether to render on the right side (true) or left side (false).
 * @param offset An optional offset value for the rendering position.
 * @param color_side A boolean indicating whether to render the frame in primary (true) or secondary (false) color.
 */
void painter_render_frame(painter_device_t device, painter_font_handle_t font_title, bool right_side, uint16_t offset,
                          bool color_side) {
    const uint16_t max_width = 240;
    uint16_t       xpos      = offset;
    // uint16_t       width     = offset + max_width;
    // uint16_t       height    = 320;

    hsv_t hsv = painter_get_hsv(color_side);

    painter_render_frame_box(device, hsv, 1, 0, xpos, 3, true, false);

    // horizontal line below scan rate
    qp_line(device, xpos + 2, 30, xpos + 80, 30, hsv.h, hsv.s, hsv.v);

    // horizontal line below rgb
    qp_line(device, xpos + 80, 54, xpos + 237, 54, hsv.h, hsv.s, hsv.v);

    // caps lock horizontal line
    qp_line(device, xpos + 208, 16, xpos + 208, 54, hsv.h, hsv.s, hsv.v);

    if (right_side) {
        // vertical lines next to scan rate + wpm + pointing
        qp_line(device, xpos + 80, 16, xpos + 80, 106, hsv.h, hsv.s, hsv.v);

        // lines for unicode typing mode and mode
        qp_line(device, xpos + 80, 80, xpos + 237, 80, hsv.h, hsv.s, hsv.v);

        // qp_line(device, xpos + 180, 80, xpos + 180, 106, hsv.h, hsv.s, hsv.v);

        // lines for mods and OS detection
        qp_line(device, xpos + 2, 107, xpos + 237, 107, hsv.h, hsv.s, hsv.v);
        qp_line(device, xpos + 155, 107, xpos + 155, 122, hsv.h, hsv.s, hsv.v);
        // lines for autocorrect and layers
        qp_line(device, xpos + 2, 122, xpos + 237, 122, hsv.h, hsv.s, hsv.v);
        qp_line(device, xpos + 121, 122, xpos + 121, 171, hsv.h, hsv.s, hsv.v);
        qp_line(device, xpos + 186, 122, xpos + 186, 171, hsv.h, hsv.s, hsv.v);
    } else {
        // vertical line next to nuke block
        qp_line(device, xpos + 80, 16, xpos + 80, 170, hsv.h, hsv.s, hsv.v);

        // lines for haptic feedback block
        qp_line(device, xpos + 80, 80, xpos + 237, 80, hsv.h, hsv.s, hsv.v);

        // horizontal line below wpm
        qp_line(device, xpos + 80, 95, xpos + 138, 95, hsv.h, hsv.s, hsv.v);
        // vertical line next to wpm
        qp_line(device, xpos + 138, 80, xpos + 138, 145, hsv.h, hsv.s, hsv.v);
        // line below last key
        qp_line(device, xpos + 138, 107, xpos + 237, 107, hsv.h, hsv.s, hsv.v);

        qp_line(device, xpos + 80, 145, xpos + 237, 145, hsv.h, hsv.s, hsv.v);
        qp_line(device, xpos + 149, 145, xpos + 149, 171, hsv.h, hsv.s, hsv.v);

        // qp_rect(device, xpos + 81, 81, xpos + 237, 170, hsv.h, hsv.s, hsv.v, true);
    }
    // line above menu block
    qp_line(device, xpos + 2, 171, xpos + 237, 171, hsv.h, hsv.s, hsv.v);
    // line above rtc
    qp_line(device, xpos + 2, 292, xpos + 237, 292, hsv.h, hsv.s, hsv.v);

    // qp_drawimage_recolor(device, xpos + 1, height - frame_bottom->height, frame_bottom, hsv.h, hsv.s, hsv.v, 0, 0,
    // 0);

    char title[50] = {0};
    snprintf(title, sizeof(title), "%s", PRODUCT);
    uint16_t title_width = qp_textwidth(font_title, title);
    if (title_width > (max_width - 55)) {
        title_width = max_width;
    }
    uint16_t title_xpos = (max_width - title_width) / 2;
    qp_drawtext_recolor(device, xpos + title_xpos, 4, font_title,
                        truncate_text(title, title_width, font_title, false, false), 0, 0, 0, hsv.h, hsv.s, hsv.v);
}

/**
 * @brief Renders a pointing device acceleration graph using the painter system
 *
 * This function displays a visual representation of the pointing device acceleration
 * curve as a line graph. The graph shows acceleration values plotted over a fixed
 * number of sample points, helping visualize the current acceleration configuration.
 *
 * @param device The painter device to render the graph on
 * @param x X coordinate for the top-left corner of the graph
 * @param y Y coordinate for the top-left corner of the graph
 * @param width Width of the graph in pixels
 * @param height Height of the graph in pixels
 * @param force_redraw If true, forces a complete redraw of the graph regardless of changes
 * @param curr_hsv Pointer to dual HSV color configuration (primary for axis, secondary for line)
 *
 * @note This function only operates when both COMMUNITY_MODULE_POINTING_DEVICE_ACCEL_ENABLE
 *       and COMMUNITY_MODULE_QP_HELPERS_ENABLE are defined
 * @note The graph automatically redraws when the acceleration configuration changes
 * @note Uses a fixed sample size of 40 data points for the acceleration curve
 */
#ifndef ACCEL_GRAPH_SAMPLES
#    define ACCEL_GRAPH_SAMPLES 40
#endif

void painter_render_pd_accel_graph(painter_device_t device, uint16_t x, uint16_t y, uint16_t width, uint16_t height,
                                   bool force_redraw, dual_hsv_t *curr_hsv) {
#if defined(COMMUNITY_MODULE_POINTING_DEVICE_ACCEL_ENABLE) && defined(COMMUNITY_MODULE_QP_HELPERS_ENABLE)
    static pointing_device_accel_config_t local        = {0};
    bool                                  needs_redraw = false;
    static uint8_t                        graph_samples[ACCEL_GRAPH_SAMPLES];

    if (memcmp(&local, &g_pointing_device_accel_config, sizeof(pointing_device_accel_config_t)) != 0) {
        local        = g_pointing_device_accel_config;
        needs_redraw = true;
        pointing_device_accel_plot_curve(graph_samples, ACCEL_GRAPH_SAMPLES);
    }

    if (force_redraw || needs_redraw) {
        const graph_line_t lines[] = {
            {
                .data      = graph_samples,
                .color     = curr_hsv->secondary,
                .mode      = LINE,
                .max_value = 127,
            },
            GRAPHS_END,
        };

        const graph_config_t config = {
            .device      = device,
            .start       = {.x = x, .y = y},
            .size        = {.x = width, .y = height},
            .axis        = curr_hsv->primary,
            .background  = {.h = 0, .s = 0, .v = 0},
            .data_points = ACCEL_GRAPH_SAMPLES,
        };

        qp_draw_graph(&config, lines);
    }
#endif
}

/**
 * @brief Renders a menu block on the display.
 *
 * @param device The painter device to render on.
 * @param font The font handle to use for rendering text.
 * @param x The x-coordinate of the top-left corner of the menu block.
 * @param y The y-coordinate of the top-left corner of the menu block.
 * @param width The width of the menu block.
 * @param height The height of the menu block.
 * @param force_redraw If true, forces the menu block to be redrawn.
 * @param curr_hsv Pointer to the current HSV color values.
 * @param is_left If true, indicates that the menu block is on the left side rendering.
 */
void painter_render_menu_block(painter_device_t device, painter_font_handle_t font, uint16_t x, uint16_t y,
                               uint16_t width, uint16_t height, bool force_redraw, dual_hsv_t *curr_hsv, bool is_left,
                               bool is_thicc) {
    static bool force_full_block_redraw = false;
#ifdef SPLIT_KEYBOARD
    bool should_render_this_side = userspace_config.display.menu_render_side & (1 << (uint8_t)!is_left);
    (void)should_render_this_side;
    static uint8_t last_menu_side = 0xFF;
    if (last_menu_side != userspace_config.display.menu_render_side) {
        last_menu_side          = userspace_config.display.menu_render_side;
        force_full_block_redraw = true;
    }
    // if slave side can't be detected, we need to force render, just to be safe
    if (!is_transport_connected()) {
        should_render_this_side = true;
    }
#else  // SPLIT_KEYBOARD
    const bool should_render_this_side = true;
    (void)should_render_this_side;
#endif // SPLIT_KEYBOARD

#ifdef COMMUNITY_MODULE_DISPLAY_MENU_ENABLE
    if (should_render_this_side &&
        painter_render_menu(device, font, x, y, width, height, is_thicc, curr_hsv->primary, curr_hsv->secondary)) {
        force_full_block_redraw = true;
        if (nyan_token != INVALID_DEFERRED_TOKEN) {
            qp_stop_animation(nyan_token);
            nyan_token = INVALID_DEFERRED_TOKEN;
        }
    } else
#endif
    {
        bool    block_redraw         = false;
        uint8_t current_display_mode = is_left ? userspace_config.display.painter.left.display_mode
                                               : userspace_config.display.painter.right.display_mode;

        static uint8_t last_display_mode[2] = {0xFF};
        if (last_display_mode[is_left] != current_display_mode) {
            last_display_mode[is_left] = current_display_mode;
            force_full_block_redraw    = true;
        }

        if (force_full_block_redraw || force_redraw) {
            qp_rect(device, x, y, width, height, 0, 0, 0, true);
            force_full_block_redraw = false;
            block_redraw            = true;
        }
        if (nyan_token != INVALID_DEFERRED_TOKEN && current_display_mode != 3) {
            qp_stop_animation(nyan_token);
            nyan_token = INVALID_DEFERRED_TOKEN;
        }

        painter_display_menu_block_modes[current_display_mode].painter_function(device, font, x, y, width, height,
                                                                                force_redraw || block_redraw, curr_hsv);
    }
}

/**
 * @brief Renders the keylogger information on the display.
 *
 * This function renders the keylogger information on the specified device using the given font and coordinates.
 * It checks if the keylogger state has changed or if a forced redraw is requested, and updates the display accordingly.
 *
 * @param device The painter device to render on.
 * @param font The font handle to use for rendering text.
 * @param x The x-coordinate to start rendering.
 * @param y The y-coordinate to start rendering.
 * @param width The width of the area to render within.
 * @param force_redraw A boolean indicating whether to force a redraw regardless of keylogger state change.
 * @param curr_hsv A pointer to the current HSV color values to use for rendering.
 */
void painter_render_keylogger(painter_device_t device, painter_font_handle_t font, uint16_t x, uint16_t y,
                              uint16_t width, bool force_redraw, dual_hsv_t *curr_hsv) {
#ifdef DISPLAY_KEYLOGGER_ENABLE
    if (is_keylogger_dirty() || force_redraw) {
        qp_drawtext_recolor(device, x, y, font, "Keylogger: ", curr_hsv->primary.h, curr_hsv->primary.s,
                            curr_hsv->primary.v, 0, 0, 0);
        y += font->line_height + 4;
        qp_rect(device, x, y, x + width - 1, y + font->line_height + 2, 0, 0, 0, true);
        qp_drawtext_recolor(device, x, y, font, truncate_text(get_keylogger_str(), width, font, true, false),
                            curr_hsv->primary.h, curr_hsv->primary.s, curr_hsv->primary.v, 0, 255, 0);
        keylogger_set_dirty(false);
    }
#endif
}

/**
 * @brief Renders the autocorrected and original text on the display.
 *
 * This function displays the autocorrected text and the original text on the specified device
 * using the provided font. It updates the display if the text has changed or if a forced redraw
 * is requested.
 *
 * @param device The painter device to render on.
 * @param font The font handle to use for rendering text.
 * @param x The x-coordinate where the text rendering starts.
 * @param y The y-coordinate where the text rendering starts.
 * @param width The width of the area to render the text.
 * @param force_redraw A boolean flag indicating whether to force a redraw of the text.
 * @param curr_hsv A pointer to a dual_hsv_t structure containing the primary and secondary HSV colors for text
 * rendering.
 */
void painter_render_autocorrect(painter_device_t device, painter_font_handle_t font, uint16_t x, uint16_t y,
                                uint16_t width, bool force_redraw, dual_hsv_t *curr_hsv) {
#if defined(AUTOCORRECT_ENABLE) || defined(COMMUNITY_MODULE_AUTOCORRECT_ENABLE)
    extern bool autocorrect_str_has_changed;
    extern char autocorrected_str_raw[2][21];
    char        buf[50] = {0};

    if (force_redraw || autocorrect_str_has_changed) {
        qp_drawtext_recolor(device, x, y, font, "Autocorrected: ", curr_hsv->primary.h, curr_hsv->primary.s,
                            curr_hsv->primary.v, 0, 0, 0);
        y += font->line_height + 4;
        snprintf(buf, sizeof(buf), "%19s", autocorrected_str_raw[0]);
        qp_drawtext_recolor(device, x, y, font, buf, curr_hsv->secondary.h, curr_hsv->secondary.s,
                            curr_hsv->secondary.v, 0, 0, 0);

        y += font->line_height + 4;
        qp_drawtext_recolor(device, x, y, font, "Original Text: ", curr_hsv->primary.h, curr_hsv->primary.s,
                            curr_hsv->primary.v, 0, 0, 0);
        y += font->line_height + 4;
        snprintf(buf, sizeof(buf), "%19s", autocorrected_str_raw[1]);

        qp_drawtext_recolor(device, x, y, font, buf, curr_hsv->secondary.h, curr_hsv->secondary.s,
                            curr_hsv->secondary.v, 0, 0, 0);
        autocorrect_str_has_changed = false;
    }
#endif // AUTOCORRECT_ENABLE
}

/**
 * @brief Renders the detected operating system on the display.
 *
 * This function checks the currently detected operating system and displays it on the specified device.
 *
 * @param device The painter device to render on.
 * @param font The font handle to use for rendering text.
 * @param x The x-coordinate to start rendering.
 * @param y The y-coordinate to start rendering.
 * @param width The width of the area to render.
 * @param force_redraw A boolean flag to force redraw even if the OS has not changed.
 * @param curr_hsv A pointer to a dual_hsv_t structure containing the current HSV color values for primary and secondary
 * colors.
 */
void painter_render_os_detection(painter_device_t device, painter_font_handle_t font, uint16_t x, uint16_t y,
                                 uint16_t width, bool force_redraw, dual_hsv_t *curr_hsv) {
#ifdef OS_DETECTION_ENABLE
    static os_variant_t last_detected_os    = {0};
    char                buf[50]             = {0};
    os_variant_t        current_detected_os = detected_host_os();
    if (force_redraw || last_detected_os != current_detected_os) {
        last_detected_os = current_detected_os;

        x += qp_drawtext_recolor(device, x, y, font, "OS: ", curr_hsv->primary.h, curr_hsv->primary.s,
                                 curr_hsv->primary.v, 0, 0, 0);

        snprintf(buf, sizeof(buf), "%9s", os_variant_to_string(current_detected_os));
        qp_drawtext_recolor(device, x, y, font, buf, curr_hsv->secondary.h, curr_hsv->secondary.s,
                            curr_hsv->secondary.v, 0, 0, 0);
    }
#endif
}

/**
 * @brief Renders the modifier icons on the display.
 *
 * This function draws the current state of the modifier keys (Shift, Control, Alt, Command, Windows) on the specified
 * device using the provided font and coordinates. It recolors the icons based on whether the modifiers are active or
 * not.
 *
 * @param device The painter device to render on.
 * @param font The font handle to use for rendering text.
 * @param x The x-coordinate to start rendering.
 * @param y The y-coordinate to start rendering.
 * @param width The width of the area to render.
 * @param force_redraw If true, forces the redraw of the modifiers regardless of state change.
 * @param curr_hsv The current HSV color values to use for active and inactive states.
 * @param disabled_val The value to use for the disabled state of the modifiers.
 */

void painter_render_modifiers(painter_device_t device, painter_font_handle_t font, uint16_t x, uint16_t y,
                              uint16_t width, bool force_redraw, dual_hsv_t *curr_hsv, uint8_t disabled_val) {
    extern painter_image_handle_t shift_icon, control_icon, alt_icon, command_icon, windows_icon;
    static uint8_t                last_mods    = 0;
    uint8_t                       current_mods = get_mods() | get_weak_mods() | get_oneshot_mods();
    if (force_redraw || last_mods != current_mods) {
        last_mods = current_mods;
        x += qp_drawtext_recolor(device, x, y + 1, font, "Modifiers:", curr_hsv->primary.h, curr_hsv->primary.s,
                                 curr_hsv->primary.v, 0, 0, 0) +
             2;

        if (qp_drawimage_recolor(
                device, x, y, shift_icon, last_mods & MOD_BIT_LSHIFT ? curr_hsv->secondary.h : curr_hsv->primary.h,
                curr_hsv->primary.s, last_mods & MOD_BIT_LSHIFT ? curr_hsv->primary.v : disabled_val, 0, 0, 0)) {
            x += shift_icon->width + 2;
        }
        if (qp_drawimage_recolor(device, x, y, keymap_config.swap_lctl_lgui ? command_icon : windows_icon,
                                 last_mods & MOD_BIT_LGUI ? curr_hsv->secondary.h : curr_hsv->primary.h,
                                 last_mods & MOD_BIT_LGUI ? curr_hsv->secondary.s : curr_hsv->primary.s,
                                 last_mods & MOD_BIT_LGUI ? curr_hsv->primary.v : disabled_val, 0, 0, 0)) {
            x += windows_icon->width + 2;
        }
        if (qp_drawimage_recolor(device, x, y, alt_icon,
                                 last_mods & MOD_BIT_LALT ? curr_hsv->secondary.h : curr_hsv->primary.h,
                                 last_mods & MOD_BIT_LALT ? curr_hsv->secondary.s : curr_hsv->primary.s,
                                 last_mods & MOD_BIT_LALT ? curr_hsv->primary.v : disabled_val, 0, 0, 0)) {
            x += alt_icon->width + 2;
        }
        if (qp_drawimage_recolor(device, x, y, control_icon,
                                 last_mods & MOD_BIT_LCTRL ? curr_hsv->secondary.h : curr_hsv->primary.h,
                                 last_mods & MOD_BIT_LCTRL ? curr_hsv->secondary.s : curr_hsv->primary.s,
                                 last_mods & MOD_BIT_LCTRL ? curr_hsv->primary.v : disabled_val, 0, 0, 0)) {
            x += control_icon->width + 2;
        }
        if (qp_drawimage_recolor(device, x, y, control_icon,
                                 last_mods & MOD_BIT_RCTRL ? curr_hsv->secondary.h : curr_hsv->primary.h,
                                 last_mods & MOD_BIT_RCTRL ? curr_hsv->secondary.s : curr_hsv->primary.s,
                                 last_mods & MOD_BIT_RCTRL ? curr_hsv->primary.v : disabled_val, 0, 0, 0)) {
            x += control_icon->width + 2;
        }
        if (qp_drawimage_recolor(device, x, y, alt_icon,
                                 last_mods & MOD_BIT_RALT ? curr_hsv->secondary.h : curr_hsv->primary.h,
                                 last_mods & MOD_BIT_RALT ? curr_hsv->secondary.s : curr_hsv->primary.s,
                                 last_mods & MOD_BIT_RALT ? curr_hsv->primary.v : disabled_val, 0, 0, 0)) {
            x += alt_icon->width + 2;
        }
        if (qp_drawimage_recolor(device, x, y, keymap_config.swap_rctl_rgui ? command_icon : windows_icon,
                                 last_mods & MOD_BIT_RGUI ? curr_hsv->secondary.h : curr_hsv->primary.h,
                                 last_mods & MOD_BIT_RGUI ? curr_hsv->secondary.s : curr_hsv->primary.s,
                                 last_mods & MOD_BIT_RGUI ? curr_hsv->primary.v : disabled_val, 0, 0, 0)) {
            x += windows_icon->width + 2;
        }
        if (qp_drawimage_recolor(device, x, y, shift_icon,
                                 last_mods & MOD_BIT_RSHIFT ? curr_hsv->secondary.h : curr_hsv->primary.h,
                                 last_mods & MOD_BIT_RSHIFT ? curr_hsv->secondary.s : curr_hsv->primary.s,
                                 last_mods & MOD_BIT_RSHIFT ? curr_hsv->primary.v : disabled_val, 0, 0, 0)) {
            x += shift_icon->width + 2;
        }
    }
}

/**
 * @brief Renders QMK information on the display.
 *
 * This function displays the build date, QMK version, and compiler version on the specified device.
 * It also draws a QMK banner image.
 *
 * @param device        The painter device to render on.
 * @param font          The font handle to use for text rendering.
 * @param x             The x-coordinate to start rendering.
 * @param y             The y-coordinate to start rendering.
 * @param width         The width of the rendering area.
 * @param force_redraw  A flag to force redraw.
 * @param curr_hsv      Pointer to a dual_hsv_t structure containing the current HSV color values.
 */
void painter_render_qmk_info(painter_device_t device, painter_font_handle_t font, uint16_t x, uint16_t y,
                             uint16_t width, bool force_redraw, dual_hsv_t *curr_hsv) {
    char     buf[50] = {0};
    uint16_t xpos    = x + 5;

    if (force_redraw) {
        snprintf(buf, sizeof(buf), "%s", QMK_BUILDDATE);
        xpos += qp_drawtext_recolor(device, xpos, y, font, "Built on: ", curr_hsv->primary.h, curr_hsv->primary.s,
                                    curr_hsv->primary.v, 0, 0, 0);
        qp_drawtext_recolor(device, xpos, y, font, buf, curr_hsv->secondary.h, curr_hsv->secondary.s,
                            curr_hsv->secondary.v, 0, 0, 0);
        xpos = x + 5;
        y += font->line_height + 4;
        snprintf(buf, sizeof(buf), "%s", QMK_VERSION);
        xpos += qp_drawtext_recolor(device, xpos, y, font, "Built from: ", curr_hsv->primary.h, curr_hsv->primary.s,
                                    curr_hsv->primary.v, 0, 0, 0);
        qp_drawtext_recolor(device, xpos, y, font, buf, curr_hsv->secondary.h, curr_hsv->secondary.s,
                            curr_hsv->secondary.v, 0, 0, 0);
        xpos = x + 5;
        y += font->line_height + 4;
        xpos += qp_drawtext_recolor(device, xpos, y, font, "Built with: ", curr_hsv->primary.h, curr_hsv->primary.s,
                                    curr_hsv->primary.v, 0, 0, 0);
        xpos += qp_drawtext_recolor(device, xpos, y, font, __VERSION__, curr_hsv->secondary.h, curr_hsv->secondary.s,
                                    curr_hsv->secondary.v, 0, 0, 0) +
                10;
        xpos += qp_drawtext_recolor(device, xpos, y, font, "Newlib: ", curr_hsv->primary.h, curr_hsv->primary.s,
                                    curr_hsv->primary.v, 0, 0, 0);
        qp_drawtext_recolor(device, xpos, y, font, _NEWLIB_VERSION, curr_hsv->secondary.h, curr_hsv->secondary.s,
                            curr_hsv->secondary.v, 0, 0, 0);
        xpos = x + 5;
        y += font->line_height + 4;
        xpos += qp_drawtext_recolor(device, xpos, y, font, "Serial: ", curr_hsv->primary.h, curr_hsv->primary.s,
                                    curr_hsv->primary.v, 0, 0, 0);
        snprintf(buf, sizeof(buf), "%s", get_hardware_id_string());
        qp_drawtext_recolor(device, xpos, y, font, buf, curr_hsv->secondary.h, curr_hsv->secondary.s,
                            curr_hsv->secondary.v, 0, 0, 0);
        y += font->line_height + 4;

        qp_drawimage(device, x, y, qmk_banner);
    }
}

/**
 * @brief Renders the layer map on the display.
 *
 * This function renders the current keymap based on the active layers on the specified painter device.
 *
 * @param device The display device to render on.
 * @param font The font handle to use for rendering text.
 * @param x The x-coordinate to start rendering from.
 * @param y The y-coordinate to start rendering from.
 * @param width The width of the area to render within.
 * @param force_redraw A boolean indicating whether to force a redraw of the layer map.
 * @param curr_hsv A pointer to a dual_hsv_t structure containing the primary and secondary HSV color values to use for
 * rendering.
 */
void painter_render_layer_map(painter_device_t device, painter_font_handle_t font, uint16_t x, uint16_t y,
                              uint16_t width, bool force_redraw, dual_hsv_t *curr_hsv) {
#ifdef COMMUNITY_MODULE_LAYER_MAP_ENABLE
    if (force_redraw || get_layer_map_has_updated()) {
        y += font->line_height + 4;
        uint16_t xpos = x, ypos = y;
        for (uint8_t lm_y = 0; lm_y < LAYER_MAP_ROWS; lm_y++) {
            xpos = x + 20;
            for (uint8_t lm_x = 0; lm_x < LAYER_MAP_COLS; lm_x++) {
                uint16_t keycode = extract_non_basic_keycode(layer_map[lm_y][lm_x], NULL, false);
#    ifdef LAYER_MAP_REMAPPING
                keypos_t key = layer_remap[lm_y][lm_x];
#    else  // LAYER_MAP_REMAPPING
                keypos_t key = {.row = lm_y, .col = lm_x};
#    endif // LAYER_MAP_REMAPPING

                xpos += MAX(qp_drawtext_recolor(device, xpos, ypos, font_oled, get_keyode_character(keycode, &key),
                                                curr_hsv->primary.h, curr_hsv->primary.s,
                                                peek_matrix_layer_map(lm_y, lm_x) ? 0 : curr_hsv->primary.v,
                                                curr_hsv->secondary.h, curr_hsv->secondary.s,
                                                peek_matrix_layer_map(lm_y, lm_x) ? curr_hsv->secondary.v : 0),
                            6);
                xpos += qp_drawtext_recolor(device, xpos, ypos, font, " ", 0, 0, 0, 0, 0, 0);
            }
            ypos += font->line_height + 4;
        }
        set_layer_map_has_updated(false);
    }
#endif // COMMUNITY_MODULE_LAYER_MAP_ENABLE
}

/**
 * @brief Renders the shutdown screen for the painter device.
 *
 * This function is responsible for rendering the shutdown screen on the specified
 * painter device. It can also optionally jump to the bootloader if specified.
 *
 * @param device The painter device to render the shutdown screen on.
 * @param jump_to_bootloader If true, the device will jump to the bootloader after rendering the shutdown screen.
 * @return true if the shutdown screen was rendered successfully, false otherwise.
 */
bool painter_render_shutdown(painter_device_t device, bool jump_to_bootloader) {
    painter_font_handle_t  font_proggy = qp_load_font_mem(font_proggy_clean_15);
    painter_image_handle_t logo        = qp_load_image_mem(gfx_qmk_logo_220x220);
    uint16_t               width, height, ypos = 240;
    qp_get_geometry(device, &width, &height, NULL, NULL, NULL);
    qp_rect(device, 0, 0, width - 1, height - 1, 0, 0, 0, true);
    qp_drawimage_recolor(device, (width - logo->width) / 2, 10, logo, 0, 0, 255, 0, 0, 0);

    char title[50] = {0};
    snprintf(title, sizeof(title), "%s", "Please Stand By...");
    uint16_t title_width = qp_textwidth(font_proggy, title);
    uint16_t title_xpos  = (width - title_width) / 2;
    qp_drawtext_recolor(device, title_xpos, ypos, font_proggy,
                        truncate_text(title, title_width, font_proggy, false, false), 0, 0, 255, 0, 0, 0);
    ypos += font_proggy->line_height + 4;
    snprintf(title, sizeof(title), "%s", jump_to_bootloader ? "Jumping to Bootloader..." : "Shutting Down...");
    title_width = qp_textwidth(font_proggy, title);
    title_xpos  = (width - title_width) / 2;
    qp_drawtext_recolor(device, title_xpos, ypos, font_proggy,
                        truncate_text(title, title_width, font_proggy, false, false), 0, 0, 255, 0, 0, 0);
    ypos += font_proggy->line_height + 4;
    if (!eeconfig_is_enabled()) {
        snprintf(title, sizeof(title), "%s", "Reinitialiing EEPROM...");
        title_width = qp_textwidth(font_proggy, title);
        title_xpos  = (width - title_width) / 2;
        qp_drawtext_recolor(device, title_xpos, ypos, font_proggy,
                            truncate_text(title, title_width, font_proggy, false, false), 0, 0, 255, 0, 0, 0);
        ypos += font_proggy->line_height + 4;
    }
    qp_flush(device);
    return false;
}

/**
 * @brief Truncates text to fit within a certain width
 *
 * @param text original text
 * @param max_width max width in pixels
 * @param font font being used
 * @param from_start truncate from start or end
 * @param add_ellipses add ellipses to truncated text
 * @return char* truncated text
 */
char *truncate_text(const char *text, uint16_t max_width, painter_font_handle_t font, bool from_start,
                    bool add_ellipses) {
    static char truncated_text[50];
    strncpy(truncated_text, text, sizeof(truncated_text) - 1);
    truncated_text[sizeof(truncated_text) - 1] = '\0';

    uint16_t text_width = qp_textwidth(font, truncated_text);
    if (text_width <= max_width) {
        return truncated_text;
    }

    size_t      len            = strlen(truncated_text);
    const char *ellipses       = "...";
    uint16_t    ellipses_width = add_ellipses ? qp_textwidth(font, ellipses) : 0;

    if (from_start) {
        size_t start_index = 0;
        while (start_index < len && text_width > max_width - ellipses_width) {
            start_index++;
            text_width = qp_textwidth(font, truncated_text + start_index);
        }

        if (add_ellipses) {
            char temp[75];
            snprintf(temp, sizeof(temp), "%s%s", ellipses, truncated_text + start_index);
            strncpy(truncated_text, temp, sizeof(truncated_text) - 1);
            truncated_text[sizeof(truncated_text) - 1] = '\0';
        } else {
            memmove(truncated_text, truncated_text + start_index, len - start_index + 1);
        }
    } else {
        while (len > 0 && text_width > max_width - ellipses_width) {
            len--;
            truncated_text[len] = '\0';
            text_width          = qp_textwidth(font, truncated_text);
        }

        if (add_ellipses) {
            snprintf(truncated_text + len, sizeof(truncated_text) - len, "%s", ellipses);
        }
    }

    return truncated_text;
}

/**
 * @brief Renders full character set of characters that can be displayed in 4 lines:
 *
 *   abcdefghijklmnopqrstuvwxyz
 *   ABCDEFGHIJKLMNOPQRSTUVWXYZ
 *   01234567890 !@#$%^&*()
 *   __+-=[]{}\\|;:'",.<>/?
 *
 * @param display quantum painter device to write to
 * @param x_offset x offset to start rendering at
 * @param max_pos array to store max x position of each line for clearing after
 * rerendering
 * @param ypos y position to start rendering at
 * @param font font to use
 * @param hue_fg text hue
 * @param sat_fg text saturation
 * @param val_fg text value
 * @param hue_bg background hue
 * @param sat_bg background saturation
 * @param val_bg background value
 */
static const char *test_text[] = {
    // did intentionally skip PROGMEM here :)
    "abcdefghijklmnopqrstuvwxyz  1234567890",
    "ABCDEFGHIJKLMNOPQRSTUVWXYZ  !@#$%^&*()",
    "__+-=[]{}\\|;:'\",.<>/?¶‽⸮«»˚‣⇤↕↓↲←▼→⇥▲♪",
};

void render_character_set(painter_device_t display, uint16_t *x_offset, uint16_t *max_pos, uint16_t *ypos,
                          painter_font_handle_t font, uint8_t hue_fg, uint8_t sat_fg, uint8_t val_fg, uint8_t hue_bg,
                          uint8_t sat_bg, uint8_t val_bg) {
    for (uint8_t i = 0; i < ARRAY_SIZE(test_text); ++i) {
        uint16_t width = qp_drawtext_recolor(display, *x_offset, *ypos, font, test_text[i], hue_fg, sat_fg, val_fg,
                                             hue_bg, sat_bg, val_bg);

        // clean up after the line (?)
        if (width < max_pos[i]) {
            qp_rect(display, *x_offset + width, *ypos, *x_offset + width + max_pos[i], *ypos + font->line_height, 0, 0,
                    0, true);
        }

        // move down for next line
        *ypos += font->line_height;
    }
}

#define GRID_WIDTH   47
#define GRID_HEIGHT  24
#define CELL_SIZE    4 // Cell size excluding outline
#define OUTLINE_SIZE 1

// Define the probability factor for initial alive cells
#define INITIAL_ALIVE_PROBABILITY 0.5 // 20% chance of being alive

static const hsv_t color_array[] = {
    {.h = 0, .s = 0, .v = 160},    {.h = 23, .s = 89, .v = 255},  {.h = 43, .s = 71, .v = 255},
    {.h = 0, .s = 82, .v = 255},   {.h = 77, .s = 64, .v = 255},  {.h = 176, .s = 77, .v = 255},
    {.h = 131, .s = 99, .v = 255}, {.h = 154, .s = 94, .v = 255},
};

void render_life(painter_device_t display, uint16_t xpos, uint16_t ypos, dual_hsv_t *curr_hsv, bool force_redraw) {
    static bool grid[GRID_HEIGHT][GRID_WIDTH], new_grid[GRID_HEIGHT][GRID_WIDTH], changed_grid[GRID_HEIGHT][GRID_WIDTH];
    static uint8_t color_value = 0;

    if (force_redraw) {
        // Initialize the grid with random values
        for (uint8_t y = 0; y < GRID_HEIGHT; y++) {
            for (uint8_t x = 0; x < GRID_WIDTH; x++) {
                grid[y][x] = (rand() < INITIAL_ALIVE_PROBABILITY * RAND_MAX); // Use probability factor
            }
        }
        color_value = rand() % ARRAY_SIZE(color_array);
    }
    static uint8_t i = 0;
    if (i++ % 10 != 0) {
        return;
    }
    for (uint8_t y = 0; y < GRID_HEIGHT; y++) {
        for (uint8_t x = 0; x < GRID_WIDTH; x++) {
            if (changed_grid[y][x]) { // Only update changed cells
                uint16_t left   = xpos + x * (CELL_SIZE + OUTLINE_SIZE);
                uint16_t top    = ypos + y * (CELL_SIZE + OUTLINE_SIZE);
                uint16_t right  = xpos + left + CELL_SIZE + OUTLINE_SIZE;
                uint16_t bottom = ypos + top + CELL_SIZE + OUTLINE_SIZE;

                // Draw the outline
                qp_rect(display, left, top, right, bottom, curr_hsv->primary.h, curr_hsv->primary.s,
                        curr_hsv->primary.v / 4, false);
                qp_rect(display, left + 1, top + 1, right - 1, bottom - 1, 0, 0, 0, true);

                // Draw the filled cell inside the outline if it's alive
                if (grid[y][x]) {
                    qp_rect(display, left + OUTLINE_SIZE, top + OUTLINE_SIZE, right - OUTLINE_SIZE,
                            bottom - OUTLINE_SIZE, color_array[color_value].h, color_array[color_value].s,
                            color_array[color_value].v, true);
                }
            }
        }
    }
    for (uint8_t y = 0; y < GRID_HEIGHT; y++) {
        for (uint8_t x = 0; x < GRID_WIDTH; x++) {
            uint8_t alive_neighbors = 0;

            // Count alive neighbors
            for (int8_t dy = -1; dy <= 1; dy++) {
                for (int8_t dx = -1; dx <= 1; dx++) {
                    if (dy == 0 && dx == 0) continue; // Skip the current cell
                    int8_t ny = y + dy;
                    int8_t nx = x + dx;
                    if (ny >= 0 && ny < GRID_HEIGHT && nx >= 0 && nx < GRID_WIDTH) {
                        alive_neighbors += grid[ny][nx];
                    }
                }
            }

            // Apply the rules of the Game of Life
            if (grid[y][x]) {
                // Any live cell with two or three live neighbours survives.
                new_grid[y][x] = (alive_neighbors == 2 || alive_neighbors == 3);
            } else {
                // Any dead cell with exactly three live neighbours becomes a live cell.
                new_grid[y][x] = (alive_neighbors == 3);
            }

            // Track changed cells
            changed_grid[y][x] = (grid[y][x] != new_grid[y][x]);
        }
    }

    // Copy new grid state to current grid
    for (uint8_t y = 0; y < GRID_HEIGHT; y++) {
        for (uint8_t x = 0; x < GRID_WIDTH; x++) {
            grid[y][x] = new_grid[y][x];
        }
    }

    static uint32_t last_tick = 0;
    uint32_t        now = (last_matrix_activity_time() > last_encoder_activity_time()) ? last_matrix_activity_time()
                                                                                       : last_encoder_activity_time();
    if (last_tick != now) {
        uint8_t cluster_size = 3; // Size of the cluster (3x3)
        uint8_t x            = rand() % (GRID_WIDTH - cluster_size);
        uint8_t y            = rand() % (GRID_HEIGHT - cluster_size);

        for (uint8_t dy = 0; dy < cluster_size; dy++) {
            for (uint8_t dx = 0; dx < cluster_size; dx++) {
                bool is_alive                = rand() % 2; // Randomly choose between 0 and 1
                grid[y + dy][x + dx]         = is_alive;   // Set the cell to be alive
                changed_grid[y + dy][x + dx] = true;       // Mark the cell as changed
            }
        }
        color_value = rand() % ARRAY_SIZE(color_array);
        last_tick   = now;
    }
}
// Scale graph data to fit graph_height
static inline uint8_t scale_value(uint8_t value, uint8_t from, uint8_t to) {
    return (value * from / to);
}

bool qp_draw_graph_l(painter_device_t device, uint16_t graph_x, uint16_t graph_y, uint16_t graph_width,
                     uint16_t graph_height, dual_hsv_t *curr_hsv, uint8_t *graph_data, uint8_t graph_segments,
                     uint8_t scale_to) {
    uint8_t graph_starting_index = 0;
    // if there are more segments than the graph width is wide in pixels, then set up things to only render the last
    // graph_width segments of the array.
    if (graph_segments >= graph_width) {
        graph_starting_index = graph_segments - graph_width;
        graph_segments       = graph_width;
    }
    // clear the graph area for redrawing
    if (!qp_rect(device, graph_x, graph_y, graph_x + graph_width, graph_y + graph_height, 0, 0, 0, true)) {
        return false;
    }

    // Draw graph axes
    if (!qp_line(device, graph_x, graph_y, graph_x, graph_y + graph_height, curr_hsv->primary.h, curr_hsv->primary.s,
                 curr_hsv->primary.v)) {
        return false;
    }
    if (!qp_line(device, graph_x, graph_y + graph_height, graph_x + graph_width, graph_y + graph_height,
                 curr_hsv->primary.h, curr_hsv->primary.s, curr_hsv->primary.v)) {
        return false;
    }

    uint8_t spacing   = graph_width / (graph_segments);
    uint8_t remainder = graph_width - (graph_segments * spacing);

    // Plot graph data
    uint8_t offset = 0;
    for (uint8_t i = graph_starting_index; i < graph_segments; i++) {
        offset += (remainder != 0 && (i - graph_starting_index) % (graph_segments / remainder) == 0) ? 1 : 0;
        uint16_t x1 = graph_x + ((i - graph_starting_index) * spacing) + offset;
        uint16_t y1 = graph_y + graph_height - scale_value(graph_data[i], graph_height - 1, scale_to) - 1;
        uint16_t x2 = graph_x + (((i - graph_starting_index) + 1) * spacing) + offset;
        uint16_t y2 = graph_y + graph_height - scale_value(graph_data[i + 1], graph_height - 1, scale_to) - 1;
        if (!qp_line(device, x1, y1, x2, y2, curr_hsv->secondary.h, curr_hsv->secondary.s, curr_hsv->secondary.v)) {
            return false;
        }
    }

    // Add markers on the y-axis for every 10 units, scaled to scale_to
    for (uint16_t i = 0; i <= scale_to; i += 10) {
        uint16_t marker_y = graph_y + graph_height - scale_value(i, graph_height - 1, scale_to);
        if (!qp_line(device, graph_x, marker_y, graph_x + 2, marker_y, curr_hsv->primary.h, curr_hsv->primary.s,
                     curr_hsv->primary.v)) {
            return false;
        }
    }

    return true;
}

#ifdef BACKLIGHT_ENABLE
static uint8_t last_backlight = 255;
#endif

void qp_backlight_enable(void) {
#ifdef BACKLIGHT_ENABLE
    if (last_backlight != 255) {
        backlight_level_noeeprom(last_backlight);
        last_backlight = 255;
    }
#elif defined(BACKLIGHT_PIN)
    gpio_write_pin_high(BACKLIGHT_PIN);
#endif // BACKLIGHT_ENABLE
}

void qp_backlight_disable(void) {
#ifdef BACKLIGHT_ENABLE
    if (last_backlight == 255) {
        last_backlight = get_backlight_level();
        backlight_level_noeeprom(0);
    }
#elif defined(BACKLIGHT_PIN)
    gpio_write_pin_low(BACKLIGHT_PIN);
#endif // BACKLIGHT_ENABLE
}

void painter_display_power(bool enable) {
#ifdef CUSTOM_QUANTUM_PAINTER_ILI9341
    ili9341_display_power(enable);
#endif // CUSTOM_QUANTUM_PAINTER_ILI9341
#ifdef CUSTOM_QUANTUM_PAINTER_ILI9488
    ili9488_display_power(enable);
#endif // CUSTOM_QUANTUM_PAINTER_ILI9488
#if defined(CUSTOM_QUANTUM_PAINTER_ST7789_135X240)
    st7789_135x240_display_power(enable);
#endif // CUSTOM_QUANTUM_PAINTER_ST7789_135X240
#if defined(CUSTOM_QUANTUM_PAINTER_ST7789_170X320)
    st7789_170x320_display_power(enable);
#endif // CUSTOM_QUANTUM_PAINTER_ST7789_170X320
#if defined(CUSTOM_QUANTUM_PAINTER_ST7789_76X284)
    st7789_76x284_display_power(enable);
#endif // CUSTOM_QUANTUM_PAINTER_ST7789_76X284
#ifdef CUSTOM_QUANTUM_PAINTER_SH1107_128X128
    sh1107_128x128_display_power(enable);
#endif // CUSTOM_QUANTUM_PAINTER_SH1107_128X128
}

void painter_init_user(void) {
#ifdef CUSTOM_QUANTUM_PAINTER_ILI9341
    init_display_ili9341();
#endif // CUSTOM_QUANTUM_PAINTER_ILI9341
#ifdef CUSTOM_QUANTUM_PAINTER_ILI9488
    init_display_ili9488();
#endif // CUSTOM_QUANTUM_PAINTER_ILI9488
#if defined(CUSTOM_QUANTUM_PAINTER_ST7789_135X240)
    init_display_st7789_135x240();
#endif // CUSTOM_QUANTUM_PAINTER_ST7789_135X240
#if defined(CUSTOM_QUANTUM_PAINTER_ST7789_170X320)
    init_display_st7789_170x320();
#endif // CUSTOM_QUANTUM_PAINTER_ST7789_170X320
#if defined(CUSTOM_QUANTUM_PAINTER_ST7789_76X284)
    init_display_st7789_76x284();
#endif // CUSTOM_QUANTUM_PAINTER_ST7789_76X284
#ifdef CUSTOM_QUANTUM_PAINTER_SH1107_128X128
    init_display_sh1107_128x128();
#endif // CUSTOM_QUANTUM_PAINTER_SH1107_128X128
}

void painter_render_user(void) {
#ifdef CUSTOM_QUANTUM_PAINTER_ILI9341
    ili9341_draw_user();
#endif // CUSTOM_QUANTUM_PAINTER_ILI9341
#ifdef CUSTOM_QUANTUM_PAINTER_ILI9488
    ili9488_draw_user();
#endif // CUSTOM_QUANTUM_PAINTER_ILI9488
#if defined(CUSTOM_QUANTUM_PAINTER_ST7789_135X240)
    st7789_135x240_draw_user();
#endif // CUSTOM_QUANTUM_PAINTER_ST7789_135X240
#if defined(CUSTOM_QUANTUM_PAINTER_ST7789_170X320)
    st7789_170x320_draw_user();
#endif // CUSTOM_QUANTUM_PAINTER_ST7789_170X320
#if defined(CUSTOM_QUANTUM_PAINTER_ST7789_76X284)
    st7789_76x284_draw_user();
#endif // CUSTOM_QUANTUM_PAINTER_ST7789_76X284
#ifdef CUSTOM_QUANTUM_PAINTER_SH1107_128X128
    sh1107_128x128_draw_user();
#endif // CUSTOM_QUANTUM_PAINTER_SH1107_128X128

#if defined(RGB_MATRIX_ENABLE) || defined(RGBLIGHT_ENABLE)
    rgb_redraw = false;
#endif
}

void housekeeping_task_display_menu_user(void) {
#ifdef SPLIT_KEYBOARD
    if (!is_keyboard_master()) {
        static bool suspended = false;
        bool        is_device_suspended(void);
        if (suspended != is_device_suspended()) {
            suspended = is_device_suspended();
            painter_display_power(!suspended);
        }
    }
#endif
#if defined(RGB_MATRIX_ENABLE)
    if (has_rgb_matrix_config_changed()) {
        display_menu_set_dirty(true);
        rgb_redraw = true;
    }
#endif
#if defined(RGBLIGHT_ENABLE)
    if (has_rgblight_config_changed()) {
        display_menu_set_dirty(true);
        rgb_redraw = true;
    }
#endif
    static uint32_t last_tick = 0;
    uint32_t        now       = timer_read32();
    if (TIMER_DIFF_32(now, last_tick) >= (QUANTUM_PAINTER_TASK_THROTTLE)) {
        painter_render_user();
        last_tick = now;
    }
#if (QUANTUM_PAINTER_DISPLAY_TIMEOUT) > 0
    if (is_keyboard_master() && (last_input_activity_elapsed() > QUANTUM_PAINTER_DISPLAY_TIMEOUT)) {
        qp_backlight_disable();
    } else {
        qp_backlight_enable();
    }
#endif
    if (console_has_redrawn) {
        console_log_needs_redraw = false;
    }
}

void keyboard_post_init_quantum_painter(void) {
    painter_init_assets();
#if defined(BACKLIGHT_ENABLE)
    if (!is_backlight_enabled()) {
        backlight_enable();
        backlight_level_noeeprom(get_backlight_level());
    }
    if (get_backlight_level() == 0) {
        backlight_level(BACKLIGHT_LEVELS);
    }
#elif !defined(BACKLIGHT_ENABLE) && defined(BACKLIGHT_PIN)
    gpio_set_pin_output_push_pull(BACKLIGHT_PIN);
    gpio_write_pin_high(BACKLIGHT_PIN);
#endif
    painter_init_user();
    if (userspace_config.display.painter.left.display_logo >= screensaver_image_size) {
        userspace_config.display.painter.left.display_logo = 0;
        eeconfig_update_user_datablock(&userspace_config, 0, EECONFIG_USER_DATA_SIZE);
    }
#ifdef SPLIT_KEYBOARD
    if (userspace_config.display.painter.right.display_logo >= screensaver_image_size) {
        userspace_config.display.painter.right.display_logo = 0;
        eeconfig_update_user_datablock(&userspace_config, 0, EECONFIG_USER_DATA_SIZE);
    }
#endif
}

void suspend_power_down_quantum_painter(void) {
    qp_backlight_disable();
    painter_display_power(false);
}

void suspend_wakeup_init_quantum_painter(void) {
    painter_display_power(true);
    qp_backlight_enable();
}

void shutdown_quantum_painter(bool jump_to_bootloader) {
#ifdef CUSTOM_QUANTUM_PAINTER_ILI9341
    ili9341_display_shutdown(jump_to_bootloader);
#endif // CUSTOM_QUANTUM_PAINTER_ILI9341
#ifdef CUSTOM_QUANTUM_PAINTER_ILI9488
    ili9488_display_shutdown(jump_to_bootloader);
#endif // CUSTOM_QUANTUM_PAINTER_ILI9488
#if defined(CUSTOM_QUANTUM_PAINTER_ST7789_135X240)
    st7789_135x240_display_shutdown(jump_to_bootloader);
#endif // CUSTOM_QUANTUM_PAINTER_ST7789_135X240
#if defined(CUSTOM_QUANTUM_PAINTER_ST7789_170X320)
    st7789_170x320_display_shutdown(jump_to_bootloader);
#endif // CUSTOM_QUANTUM_PAINTER_ST7789_170X320
#if defined(CUSTOM_QUANTUM_PAINTER_ST7789_76X284)
    st7789_76x284_display_shutdown(jump_to_bootloader);
#endif // CUSTOM_QUANTUM_PAINTER_ST7789_76X284
#ifdef CUSTOM_QUANTUM_PAINTER_SH1107_128X128
    sh1107_128x128_display_shutdown(jump_to_bootloader);
#endif // CUSTOM_QUANTUM_PAINTER_SH1107_128X128

#ifdef BACKLIGHT_ENABLE
    qp_backlight_enable();
#elif defined(BACKLIGHT_PIN)
    gpio_write_pin_low(BACKLIGHT_PIN);
#endif
}

#ifndef PAINTER_HUE_STEP
#    define PAINTER_HUE_STEP 4
#endif // PAINTER_HUE_STEP

#ifndef PAINTER_SAT_STEP
#    define PAINTER_SAT_STEP 16
#endif // PAINTER_SAT_STEP

#ifndef PAINTER_VAL_STEP
#    define PAINTER_VAL_STEP 16
#endif // PAINTER_VAL_STEP

#ifndef PAINTER_SPD_STEP
#    define PAINTER_SPD_STEP 16
#endif // PAINTER_SPD_STEP
#include <lib/lib8tion/lib8tion.h>

/**
 * @brief Get the current HSV values for the painter
 *
 * @return HSV
 */
dual_hsv_t painter_get_dual_hsv(void) {
    return userspace_config.display.painter.hsv;
}

/**
 * @brief Set the HSV for painter rendering
 *
 * @param hue hue value to use (0-255, qmk's modified hue range)
 * @param sat satuartion value to use
 * @param val value/brightness value to use
 * @param write_to_eeprom save changes to eeprom?
 */
void painter_sethsv_eeprom_helper(uint8_t hue, uint8_t sat, uint8_t val, bool write_to_eeprom, bool primary) {
    hsv_t *hsv =
        primary ? &userspace_config.display.painter.hsv.primary : &userspace_config.display.painter.hsv.secondary;
    hsv->h = hue;
    hsv->s = sat;
    hsv->v = val;
    if (write_to_eeprom) {
        eeconfig_update_user_datablock(&userspace_config, 0, EECONFIG_USER_DATA_SIZE);
    }
    dprintf("painter set %s hsv [%s]: %u,%u,%u\n", primary ? "primary" : "secondary",
            write_to_eeprom ? "EEPROM" : "NOEEPROM", hsv->h, hsv->s, hsv->v);
}
/**
 * @brief Sets the HSV for painter rendering, without saving to eeprom
 *
 * @param hue hue value to use (0-255, qmk's modified hue range)
 * @param sat satuartion value to use
 * @param val value/brightness value to use
 */
void painter_sethsv_noeeprom(uint8_t hue, uint8_t sat, uint8_t val, bool primary) {
    painter_sethsv_eeprom_helper(hue, sat, val, false, primary);
}
/**
 * @brief Sets the HSV for painter rendering, and saves to eeprom
 *
 * @param hue hue value to use (0-255, qmk's modified hue range)
 * @param sat satuartion value to use
 * @param val value/brightness value to use
 */
void painter_sethsv(uint8_t hue, uint8_t sat, uint8_t val, bool primary) {
    painter_sethsv_eeprom_helper(hue, sat, val, true, primary);
}

/**
 * @brief Get the current HSV values for the painter
 *
 * @return HSV
 */
hsv_t painter_get_hsv(bool primary) {
    return primary ? userspace_config.display.painter.hsv.primary : userspace_config.display.painter.hsv.secondary;
}

/**
 * @brief Get the current hue value for the painter
 *
 * @return uint8_t
 */
uint8_t painter_get_hue(bool primary) {
    return primary ? userspace_config.display.painter.hsv.primary.h : userspace_config.display.painter.hsv.secondary.h;
}

/**
 * @brief Get the current saturation value for the painter
 *
 * @return uint8_t
 */
uint8_t painter_get_sat(bool primary) {
    return primary ? userspace_config.display.painter.hsv.primary.s : userspace_config.display.painter.hsv.secondary.s;
}

/**
 * @brief Get the current value/brightness value for the painter
 *
 * @return uint8_t
 */
uint8_t painter_get_val(bool primary) {
    return primary ? userspace_config.display.painter.hsv.primary.v : userspace_config.display.painter.hsv.secondary.v;
}

/**
 * @brief Increments hue up by the step value
 *
 * @param write_to_eeprom Save to eeprom?
 */
void painter_increase_hue_helper(bool write_to_eeprom, bool primary) {
    hsv_t *hsv =
        primary ? &userspace_config.display.painter.hsv.primary : &userspace_config.display.painter.hsv.secondary;
    painter_sethsv_eeprom_helper(qadd8(hsv->h, PAINTER_HUE_STEP), hsv->s, hsv->v, write_to_eeprom, primary);
}

/**
 * @brief Increments hue up by the step value, without saving to eeprom
 */
void painter_increase_hue_noeeprom(bool primary) {
    painter_increase_hue_helper(false, primary);
}

/**
 * @brief Increments hue up by the step value, and saves to eeprom
 */
void painter_increase_hue(bool primary) {
    painter_increase_hue_helper(true, primary);
}

/**
 * @brief Decrements hue down by the step value
 *
 * @param write_to_eeprom Save to eeprom?
 */
void painter_decrease_hue_helper(bool write_to_eeprom, bool primary) {
    hsv_t *hsv =
        primary ? &userspace_config.display.painter.hsv.primary : &userspace_config.display.painter.hsv.secondary;
    painter_sethsv_eeprom_helper(qsub8(hsv->h, PAINTER_HUE_STEP), hsv->s, hsv->v, write_to_eeprom, primary);
}

/**
 * @brief Decrements hue down by the step value, without saving to eeprom
 */
void painter_decrease_hue_noeeprom(bool primary) {
    painter_decrease_hue_helper(false, primary);
}

/**
 * @brief Decrements hue down by the step value, and saves to eeprom
 */
void painter_decrease_hue(bool primary) {
    painter_decrease_hue_helper(true, primary);
}

/**
 * @brief Increments saturation up by the step value
 *
 * @param write_to_eeprom Save to eeprom?
 */
void painter_increase_sat_helper(bool write_to_eeprom, bool primary) {
    hsv_t *hsv =
        primary ? &userspace_config.display.painter.hsv.primary : &userspace_config.display.painter.hsv.secondary;
    painter_sethsv_eeprom_helper(hsv->h, qadd8(hsv->s, PAINTER_SAT_STEP), hsv->v, write_to_eeprom, primary);
}

/**
 * @brief Increments saturation up by the step value, without saving to eeprom
 */
void painter_increase_sat_noeeprom(bool primary) {
    painter_increase_sat_helper(false, primary);
}

/**
 * @brief Increments saturation up by the step value, and saves to eeprom
 */
void painter_increase_sat(bool primary) {
    painter_increase_sat_helper(true, primary);
}

/**
 * @brief Decrements saturation down by the step value
 *
 * @param write_to_eeprom Save to eeprom?
 */
void painter_decrease_sat_helper(bool write_to_eeprom, bool primary) {
    hsv_t *hsv =
        primary ? &userspace_config.display.painter.hsv.primary : &userspace_config.display.painter.hsv.secondary;
    painter_sethsv_eeprom_helper(hsv->h, qsub8(hsv->s, PAINTER_SAT_STEP), hsv->v, write_to_eeprom, primary);
}

/**
 * @brief Decrements saturation down by the step value, without saving to eeprom
 */
void painter_decrease_sat_noeeprom(bool primary) {
    painter_decrease_sat_helper(false, primary);
}

/**
 * @brief Decrements saturation down by the step value, and saves to eeprom
 */
void painter_decrease_sat(bool primary) {
    painter_decrease_sat_helper(true, primary);
}

/**
 * @brief Increments value/brightness up by the step value
 *
 * @param write_to_eeprom Save to eeprom?
 */
void painter_increase_val_helper(bool write_to_eeprom, bool primary) {
    hsv_t *hsv =
        primary ? &userspace_config.display.painter.hsv.primary : &userspace_config.display.painter.hsv.secondary;
    painter_sethsv_eeprom_helper(hsv->h, hsv->s, qadd8(hsv->v, PAINTER_VAL_STEP), write_to_eeprom, primary);
}

/**
 * @brief Increments value/brightness up by the step value, without saving to eeprom
 */
void painter_increase_val_noeeprom(bool primary) {
    painter_increase_val_helper(false, primary);
}

/**
 * @brief Increments value/brightness up by the step value, and saves to eeprom
 */
void painter_increase_val(bool primary) {
    painter_increase_val_helper(true, primary);
}

/**
 * @brief Decrements value/brightness down by the step value
 *
 * @param write_to_eeprom Save to eeprom?
 */
void painter_decrease_val_helper(bool write_to_eeprom, bool primary) {
    hsv_t *hsv =
        primary ? &userspace_config.display.painter.hsv.primary : &userspace_config.display.painter.hsv.secondary;
    painter_sethsv_eeprom_helper(hsv->h, hsv->s, qsub8(hsv->v, PAINTER_VAL_STEP), write_to_eeprom, primary);
}

/**
 * @brief Decrements value/brightness down by the step value, without saving to eeprom
 */
void painter_decrease_val_noeeprom(bool primary) {
    painter_decrease_val_helper(false, primary);
}

/**
 * @brief Decrements value/brightness down by the step value, and saves to eeprom
 */
void painter_decrease_val(bool primary) {
    painter_decrease_val_helper(true, primary);
}
