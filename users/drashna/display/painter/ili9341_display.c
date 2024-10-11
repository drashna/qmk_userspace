// Copyright 2024 Christopher Courtney, aka Drashna Jael're  (@drashna) <drashna@live.com>
// Copyright 2018-2024 Nick Brassel (@tzarc)
// SPDX-License-Identifier: GPL-3.0-or-later

#include "drashna_runtime.h"
#include "drashna_names.h"
#include "drashna_layers.h"
#include "drashna_util.h"
#include "version.h"
#include "qp_surface.h"
#include "qp_ili9xxx_opcodes.h"
#include "qp_comms.h"
#include "display/painter/painter.h"
#include "display/painter/ili9341_display.h"
#include "display/painter/menu.h"
#include "lib/lib8tion/lib8tion.h"
#ifdef SPLIT_KEYBOARD
#    include "split_util.h"
#endif // SPLIT_KEYBOARD
#if defined(RGB_MATRIX_ENABLE)
#    include "rgb/rgb_matrix_stuff.h"
#endif // defined(RGB_MATRIX_ENABLE)
#if defined(RGBLIGHT_ENABLE)
#    include "rgb/rgb_stuff.h"
#endif // defined(RGBLIGHT_ENABLE)
#ifdef RTC_ENABLE
#    include "features/rtc/rtc.h"
#endif
#ifdef LAYER_MAP_ENABLE
#    include "features/layer_map.h"
#endif
#ifdef KEYBOARD_LOCK_ENABLE
#    include "features/keyboard_lock.h"
#endif // KEYBOARD_LOCK_ENABLE

painter_device_t display, menu_surface;

painter_font_handle_t font_thintel, font_mono, font_oled;

painter_image_handle_t frame_top, frame_bottom;
painter_image_handle_t lock_caps_on, lock_caps_off;
painter_image_handle_t lock_num_on, lock_num_off;
painter_image_handle_t lock_scrl_on, lock_scrl_off;
painter_image_handle_t windows_logo, apple_logo, linux_logo;
painter_image_handle_t shift_icon, control_icon, alt_icon, command_icon, windows_icon;
painter_image_handle_t mouse_icon;
painter_image_handle_t gamepad_icon;
painter_image_handle_t qmk_banner;
painter_image_handle_t screen_saver;

#define SURFACE_MENU_WIDTH  236
#define SURFACE_MENU_HEIGHT 120

uint8_t     menu_buffer[SURFACE_REQUIRED_BUFFER_BYTE_SIZE(SURFACE_MENU_WIDTH, SURFACE_MENU_HEIGHT, 16)];
static bool has_run = false, forced_reinit = false;

/**
 * @brief Renders RTC Time to display
 *
 * @param device screen to render to
 * @param font font to use
 * @param x x position to start rendering
 * @param y y position to start rendering
 * @param display_width maximum width for rendering
 * @param force_redraw do we need to redraw regardless of time
 * @param hsv hsv value to render with
 */
void render_rtc_time(painter_device_t device, painter_font_handle_t font, uint16_t x, uint16_t y,
                     uint16_t display_width, bool force_redraw, hsv_t *hsv) {
#ifdef RTC_ENABLE
    static uint16_t rtc_timer  = 0;
    bool            rtc_redraw = false;
    if (timer_elapsed(rtc_timer) > 125 && rtc_is_connected()) {
        rtc_timer  = timer_read();
        rtc_redraw = true;
    }
    if (force_redraw || rtc_redraw) {
        char buf[40] = {0};
        if (rtc_is_connected()) {
            snprintf(buf, sizeof(buf), "RTC Date/Time: %s", rtc_read_date_time_str());
        } else {
            snprintf(buf, sizeof(buf), "RTC Device Not Connected");
        }

        uint8_t title_width = qp_textwidth(font_oled, buf);
        if (title_width > (display_width - 6)) {
            title_width = display_width - 6;
        }
        uint8_t title_xpos = (display_width - title_width) / 2;

        qp_drawtext_recolor(device, title_xpos, y, font_oled, buf, hsv->h, hsv->s, hsv->v, 0, 0, 0);
    }
#endif // RTC_ENABLE
}

/**
 * @brief Render the console log to the display
 *
 * @param device device to render to
 * @param font font to render with
 * @param x x position to start rendering
 * @param y y position to start rendering
 * @param force_redraw do we forcibly redraw the console log
 * @param hsv colors to render with
 * @param start start line to render
 * @param end last line to render
 */
void painter_render_console(painter_device_t device, painter_font_handle_t font, uint16_t x, uint16_t y,
                            bool force_redraw, hsv_t *hsv, uint8_t start, uint8_t end) {
    static uint16_t max_line_width = 0;
    if (console_log_needs_redraw || force_redraw) {
        for (uint8_t i = start; i < end; i++) {
            uint16_t xpos =
                x + qp_drawtext_recolor(device, x, y, font, logline_ptrs[i], hsv->h, hsv->s, hsv->v, 0, 0, 0);
            if (max_line_width < xpos) {
                max_line_width = xpos;
            }
            qp_rect(device, xpos, y, max_line_width, y + font->line_height, 0, 0, 0, true);
            y += font->line_height + 4;
        }
        console_log_needs_redraw = false;
    }
}
/**
 * @brief Render the pointing device to the display
 *
 * @param device device to render to
 * @param font font to render with
 * @param x x position to start rendering
 * @param y y position to start rendering
 * @param force_redraw do we forcibly redraw the pointing device
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
                        dual_hsv_t *curr_hsv, const char *(*get_rgb_mode)(void), hsv_t (*get_rgb_hsv)(void),
                        bool        matrix) {
#if defined(RGB_MATRIX_ENABLE) || defined(RGBLIGHT_ENABLE)
    char buf[22] = {0};
    if (force_redraw) {
        hsv_t rgb_hsv = get_rgb_hsv();
        qp_drawtext_recolor(device, x, y, font,
                            matrix ? "RGB Matrix Config:" : "RGB Light Config:", curr_hsv->primary.h,
                            curr_hsv->primary.s, curr_hsv->primary.v, 0, 0, 0);
        y += font->line_height + 4;
        snprintf(buf, sizeof(buf), "%21s", truncate_text(get_rgb_mode(), 125, font, false, false));
        qp_drawtext_recolor(device, x + 125 - qp_textwidth(font, buf), y, font, buf, curr_hsv->secondary.h,
                            curr_hsv->secondary.s, curr_hsv->secondary.v, 0, 0, 0);

        y += font->line_height + 4;
        x += qp_drawtext_recolor(device, x, y, font, "HSV: ", curr_hsv->primary.h, curr_hsv->primary.s,
                                 curr_hsv->primary.v, 0, 0, 0);
        snprintf(buf, sizeof(buf), "%3d, %3d, %3d", rgb_hsv.h, rgb_hsv.s, rgb_hsv.v);
        qp_drawtext_recolor(device, x, y, font, buf, curr_hsv->secondary.h, curr_hsv->secondary.s,
                            curr_hsv->secondary.v, 0, 0, 0);
        qp_rect(device, 197, 43, 207, 53, rgb_hsv.h, rgb_hsv.s,
                (uint8_t)(rgb_hsv.v * 0xFF / (matrix ? RGB_MATRIX_MAXIMUM_BRIGHTNESS : RGBLIGHT_LIMIT_VAL)), true);
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
 * @brief Render the current wpm count to the display
 *
 * @param device
 * @param font
 * @param x
 * @param y
 * @param force_redraw
 * @param curr_hsv
 */
void painter_render_wpm(painter_device_t device, painter_font_handle_t font, uint16_t x, uint16_t y, bool force_redraw,
                        dual_hsv_t *curr_hsv) {
#ifdef WPM_ENABLE
    static uint32_t last_wpm_update = 0;
    static char     buf[4]          = {0};
    if (force_redraw || last_wpm_update != get_current_wpm()) {
        last_wpm_update = get_current_wpm();
        x += qp_drawtext_recolor(device, x, y, font, "WPM:     ", curr_hsv->primary.h, curr_hsv->primary.s,
                                 curr_hsv->primary.v, 0, 0, 0);
        snprintf(buf, sizeof(buf), "%3u", get_current_wpm());
        qp_drawtext_recolor(device, x, y, font, buf, curr_hsv->secondary.h, curr_hsv->secondary.s,
                            curr_hsv->secondary.v, 0, 0, 0);
    }
#endif // WPM_ENABLE
}

/**
 * @brief Draws the initial frame on the screen
 *
 * @param display
 */
void render_frame(painter_device_t device) {
    uint16_t width;
    uint16_t height;
    qp_get_geometry(device, &width, &height, NULL, NULL, NULL);

    hsv_t hsv = painter_get_hsv(true);
    // frame top
    qp_drawimage_recolor(device, 1, 2, frame_top, hsv.h, hsv.s, hsv.v, 0, 0, 0);
    // lines for frame sides
    qp_line(device, 1, frame_top->height, 1, height - frame_bottom->height, hsv.h, hsv.s, hsv.v);
    qp_line(device, width - 2, frame_top->height, width - 2, height - frame_bottom->height, hsv.h, hsv.s, hsv.v);

    // horizontal line below rgb
    qp_line(device, 80, 54, width - 3, 54, hsv.h, hsv.s, hsv.v);

    // caps lock horizontal line
    qp_line(device, 208, 16, 208, 54, hsv.h, hsv.s, hsv.v);

    if (is_keyboard_master()) {
        // vertical lines next to scan rate + wpm + pointing
        qp_line(device, 80, 16, 80, 106, hsv.h, hsv.s, hsv.v);
        // horizontal line below scan rate + wpm
        qp_line(device, 2, 43, 80, 43, hsv.h, hsv.s, hsv.v);

        // lines for unicode typing mode and mode
        qp_line(device, 80, 80, width - 3, 80, hsv.h, hsv.s, hsv.v);
        qp_line(device, 149, 80, 149, 106, hsv.h, hsv.s, hsv.v);

        // lines for mods and OS detection
        qp_line(device, 2, 107, width - 3, 107, hsv.h, hsv.s, hsv.v);
        qp_line(device, 155, 107, 155, 122, hsv.h, hsv.s, hsv.v);
        // lines for autocorrect and layers
        qp_line(device, 2, 122, width - 3, 122, hsv.h, hsv.s, hsv.v);
        qp_line(device, 121, 122, 121, 171, hsv.h, hsv.s, hsv.v);
        qp_line(device, 186, 122, 186, 171, hsv.h, hsv.s, hsv.v);
    } else {
        // horizontal line below scan rate + wpm
        qp_line(device, 2, 43, 80, 43, hsv.h, hsv.s, hsv.v);
        // vertical line next to pointing device block
        qp_line(device, 80, 16, 80, 54, hsv.h, hsv.s, hsv.v);
    }
    // line above menu block
    qp_line(device, 2, 171, width - 3, 171, hsv.h, hsv.s, hsv.v);
    // line above rtc
    qp_line(device, 2, 292, width - 3, 292, hsv.h, hsv.s, hsv.v);
    // frame bottom
    qp_drawimage_recolor(device, 1, height - frame_bottom->height, frame_bottom, hsv.h, hsv.s, hsv.v, 0, 0, 0);

    char title[50] = {0};
    snprintf(title, sizeof(title), "%s", PRODUCT);
    uint8_t title_width = qp_textwidth(font_thintel, title);
    if (title_width > (width - 55)) {
        title_width = width - 55;
    }
    uint8_t title_xpos = (width - title_width) / 2;
    qp_drawtext_recolor(device, title_xpos, 4, font_thintel,
                        truncate_text(title, title_width, font_thintel, false, false), 0, 0, 0, hsv.h, hsv.s, hsv.v);
}

void init_display_ili9341_inversion(void) {
    qp_comms_start(display);
    qp_comms_command(display, userspace_config.painter.inverted ? ILI9XXX_CMD_INVERT_ON : ILI9XXX_CMD_INVERT_OFF);
    qp_comms_stop(display);
    if (has_run) {
        forced_reinit = true;
    }
}

void init_display_ili9341_rotation(void) {
    uint16_t width;
    uint16_t height;

    qp_init(display, userspace_config.painter.rotation ? QP_ROTATION_0 : QP_ROTATION_180);
    qp_get_geometry(display, &width, &height, NULL, NULL, NULL);
    qp_clear(display);
    qp_rect(display, 0, 0, width - 1, height - 1, 0, 0, 0, true);

    // if needs inversion, run it only afetr the clear and rect functions or otherwise it won't work
    init_display_ili9341_inversion();
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Initial render of frame/logo
    frame_top    = qp_load_image_mem(gfx_frame_top);
    frame_bottom = qp_load_image_mem(gfx_frame_bottom);
    render_frame(display);

    qp_power(display, true);
    qp_flush(display);
    if (has_run) {
        forced_reinit = true;
    }
    has_run = true;
}

/**
 * @brief Initializes the display, clears it and sets frame and title
 *
 */
void init_display_ili9341(void) {
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

    mouse_icon   = qp_load_image_mem(gfx_mouse_icon);
    gamepad_icon = qp_load_image_mem(gfx_gamepad_24x24);
    qmk_banner   = qp_load_image_mem(gfx_qmk_powered_by);

    display =
        qp_ili9341_make_spi_device(240, 320, DISPLAY_CS_PIN, DISPLAY_DC_PIN, DISPLAY_RST_PIN, DISPLAY_SPI_DIVIDER, 0);
    menu_surface = qp_make_rgb565_surface(SURFACE_MENU_WIDTH, SURFACE_MENU_HEIGHT, menu_buffer);

    wait_ms(50);

    qp_init(menu_surface, QP_ROTATION_0);
    init_display_ili9341_rotation();
}

void ili9341_display_power(bool on) {
    qp_power(display, on);
}

__attribute__((weak)) bool screen_saver_sanity_checks(void) {
    static uint32_t last_tick = 0;
    uint32_t        now       = last_input_activity_elapsed();
    uint32_t        diff      = abs8(now - last_tick);
    last_tick                 = now;
    // if the last activity has been more than 45 days ago... doubt. Likely corruption.
    if (now > 4000000000) {
        return false;
    }
    // if time since firmware start has been less than 10 seconds, doubt.
    if (timer_elapsed32(0) < 10000) {
        return false;
    }
    // if the difference between the last tick and this one is more than 10 times the throttle, doubt.
    if (diff > ((QUANTUM_PAINTER_TASK_THROTTLE) * 10)) {
        return false;
    }
    // if last activity has been been a third of the timeout, believe.
    if (now > ((QUANTUM_PAINTER_DISPLAY_TIMEOUT) / 3)) {
        return true;
    }

    return false;
}

__attribute__((weak)) void ili9341_draw_user(void) {
    bool            hue_redraw          = forced_reinit;
    static bool     screen_saver_redraw = false;
    static uint32_t last_tick           = 0;
    uint32_t        now                 = timer_read32();
    if (TIMER_DIFF_32(now, last_tick) < (QUANTUM_PAINTER_TASK_THROTTLE)) {
        return;
    }

    static dual_hsv_t curr_hsv = {0};
    if (memcmp(&curr_hsv, &userspace_config.painter.hsv, sizeof(dual_hsv_t)) != 0) {
        curr_hsv   = painter_get_dual_hsv();
        hue_redraw = true;
    }
    const uint8_t disabled_val = curr_hsv.primary.v / 2;
    uint16_t      width;
    uint16_t      height;
    qp_get_geometry(display, &width, &height, NULL, NULL, NULL);

#if defined(RGB_MATRIX_ENABLE) || defined(RGBLIGHT_ENABLE)
    bool rgb_redraw = false;
#    if defined(RGB_MATRIX_ENABLE)
    if (has_rgb_matrix_config_changed()) {
        display_menu_set_dirty();
        rgb_redraw = true;
    }
#    endif
#    if defined(RGBLIGHT_ENABLE)
    if (has_rgblight_config_changed()) {
        display_menu_set_dirty();
        rgb_redraw = true;
    }
#    endif
#endif
    if (screen_saver_sanity_checks()) {
        if (!screen_saver_redraw) {
            dprintf("Screen saver: %lu\n", last_input_activity_elapsed());
        }
        static uint8_t display_mode = 0xFF;
        if (display_mode != userspace_config.painter.display_mode || screen_saver_redraw == false) {
            display_mode        = userspace_config.painter.display_mode;
            screen_saver_redraw = true;
            switch (userspace_config.painter.display_logo) {
                case 0:
                    screen_saver = qp_load_image_mem(gfx_samurai_cyberpunk_minimal_dark_8k_b3_240x320);
                    break;
                case 1:
                    screen_saver = qp_load_image_mem(gfx_anime_girl_jacket_240x320);
                    break;
                case 2:
                    screen_saver = qp_load_image_mem(gfx_asuka_240x320);
                    break;
                case 3:
                    screen_saver = qp_load_image_mem(gfx_neon_genesis_evangelion_initial_machine_00_240x320);
                    break;
                case 4:
                    screen_saver = qp_load_image_mem(gfx_neon_genesis_evangelion_initial_machine_01_240x320);
                    break;
                case 5:
                    screen_saver = qp_load_image_mem(gfx_neon_genesis_evangelion_initial_machine_02_240x320);
                    break;
                case 6:
                    screen_saver = qp_load_image_mem(gfx_neon_genesis_evangelion_initial_machine_03_240x320);
                    break;
                case 7:
                    screen_saver = qp_load_image_mem(gfx_neon_genesis_evangelion_initial_machine_04_240x320);
                    break;
                case 8:
                    screen_saver = qp_load_image_mem(gfx_neon_genesis_evangelion_initial_machine_05_240x320);
                    break;
                case 9:
                    screen_saver = qp_load_image_mem(gfx_neon_genesis_evangelion_initial_machine_06_240x320);
                    break;
            }
            if (screen_saver != NULL) {
                qp_drawimage(display, 0, 0, screen_saver);
                qp_close_image(screen_saver);
            }
        }
    } else {
        if (screen_saver_redraw) {
            hue_redraw = true;
            qp_rect(display, 0, 0, width - 1, height - 1, 0, 0, 0, true);
            render_frame(display);
        }
        bool transport_icon_redraw = false;
#ifdef SPLIT_KEYBOARD
        static bool transport_connected = true;
        if (transport_connected != is_transport_connected()) {
            transport_connected   = is_transport_connected();
            transport_icon_redraw = true;
        }
#endif
        if (hue_redraw || transport_icon_redraw) {
            qp_rect(display, width - mouse_icon->width - 6, 5, width - 6, 5 + mouse_icon->height - 1, 0, 0, 0, true);
            qp_drawimage_recolor(display, width - mouse_icon->width - 6, 5, mouse_icon,
                                 is_keyboard_master() ? curr_hsv.secondary.h : curr_hsv.primary.h,
                                 is_keyboard_master() ? curr_hsv.secondary.s : curr_hsv.primary.s,
#ifdef SPLIT_KEYBOARD
                                 is_transport_connected()
                                     ? is_keyboard_master() ? curr_hsv.secondary.v : curr_hsv.primary.v
                                     : disabled_val,
#else
                                 curr_hsv.secondary.v,
#endif
                                 0, 0, 0);
        }
        char     buf[50] = {0};
        uint16_t ypos    = 20;
        uint16_t xpos    = 5;

        painter_render_scan_rate(display, font_oled, xpos, ypos, hue_redraw, &curr_hsv);
        ypos += font_oled->line_height + 4;
#ifdef WPM_ENABLE
        painter_render_wpm(display, font_oled, 5, ypos, hue_redraw, &curr_hsv);
#endif // WPM_ENABLE

        if (is_keyboard_master()) {
            ypos = 20;
            xpos = 83;
#if defined(RGB_MATRIX_ENABLE)
            painter_render_rgb(display, font_oled, xpos, ypos, hue_redraw || rgb_redraw, &curr_hsv,
                               rgb_matrix_get_effect_name, rgb_matrix_get_hsv, true);
#endif // RGB_MATRIX_ENABLE

            //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            // LED Lock indicator(text)

            ypos = 24;
            xpos = 212;
            painter_render_lock_state(display, font_oled, xpos, ypos, hue_redraw, &curr_hsv, disabled_val);

            ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            // Keymap config (nkro, autocorrect, oneshots)

            ypos                                        = 54 + 4;
            static keymap_config_t last_keymap_config   = {0};
            bool                   keymap_config_redraw = false;
            if (last_keymap_config.raw != keymap_config.raw) {
                last_keymap_config.raw = keymap_config.raw;
                keymap_config_redraw   = true;
            }
            if (hue_redraw || keymap_config_redraw) {
                xpos = 80 + 4;
                qp_drawimage(display, xpos, ypos + 2, last_keymap_config.swap_lctl_lgui ? apple_logo : windows_logo);
                xpos += windows_logo->width + 5;
                xpos += qp_drawtext_recolor(display, xpos, ypos, font_oled, "NKRO",
                                            last_keymap_config.nkro ? curr_hsv.secondary.h : curr_hsv.primary.h,
                                            last_keymap_config.nkro ? curr_hsv.secondary.s : curr_hsv.primary.s,
                                            last_keymap_config.nkro ? curr_hsv.primary.v : disabled_val, 0, 0, 0) +
                        5;
                xpos += qp_drawtext_recolor(
                            display, xpos, ypos, font_oled, "CRCT",
                            last_keymap_config.autocorrect_enable ? curr_hsv.secondary.h : curr_hsv.primary.h,
                            last_keymap_config.autocorrect_enable ? curr_hsv.secondary.s : curr_hsv.primary.s,
                            last_keymap_config.autocorrect_enable ? curr_hsv.primary.v : disabled_val, 0, 0, 0) +
                        5;
                xpos +=
                    qp_drawtext_recolor(display, xpos, ypos, font_oled, "1SHT",
                                        last_keymap_config.oneshot_enable ? curr_hsv.secondary.h : curr_hsv.primary.h,
                                        last_keymap_config.oneshot_enable ? curr_hsv.secondary.s : curr_hsv.primary.s,
                                        last_keymap_config.oneshot_enable ? curr_hsv.primary.v : disabled_val, 0, 0, 0);
            }

            ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            // Device Config (Audio, Audio Clicky, Host Driver lock, Swap Hands)

            ypos += font_oled->line_height + 4;
            static user_runtime_config_t last_user_state = {0};
            if (hue_redraw || memcmp(&user_runtime_state, &last_user_state, sizeof(user_runtime_state))) {
                memcpy(&last_user_state, &user_runtime_state, sizeof(user_runtime_state));
                xpos = 80 + 4 + windows_logo->width + 5;
                xpos += qp_drawtext_recolor(display, xpos, ypos, font_oled, "AUDIO",
                                            last_user_state.audio.enable ? curr_hsv.secondary.h : curr_hsv.primary.h,
                                            last_user_state.audio.enable ? curr_hsv.secondary.s : curr_hsv.primary.s,
                                            last_user_state.audio.enable ? curr_hsv.primary.v : disabled_val, 0, 0, 0) +
                        5;
                xpos += qp_drawtext_recolor(
                            display, xpos, ypos, font_oled, "CLCK",
                            last_user_state.audio.clicky_enable ? curr_hsv.secondary.h : curr_hsv.primary.h,
                            last_user_state.audio.clicky_enable ? curr_hsv.secondary.s : curr_hsv.primary.s,
                            last_user_state.audio.clicky_enable ? curr_hsv.primary.v : disabled_val, 0, 0, 0) +
                        5;
                xpos +=
                    qp_drawtext_recolor(
                        display, xpos, ypos, font_oled, "HOST",
                        last_user_state.internals.host_driver_disabled ? curr_hsv.secondary.h : curr_hsv.primary.h,
                        last_user_state.internals.host_driver_disabled ? curr_hsv.secondary.s : curr_hsv.primary.s,
                        last_user_state.internals.host_driver_disabled ? curr_hsv.primary.v : disabled_val, 0, 0, 0) +
                    5;
                xpos += qp_drawtext_recolor(
                    display, xpos, ypos, font_oled, "SWAP",
                    last_user_state.internals.swap_hands ? curr_hsv.secondary.h : curr_hsv.primary.h,
                    last_user_state.internals.swap_hands ? curr_hsv.secondary.s : curr_hsv.primary.s,
                    last_user_state.internals.swap_hands ? curr_hsv.primary.v : disabled_val, 0, 0, 0);
            }
            ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            // Pointing Device CPI

            ypos = 43 + 4;
#if defined(POINTING_DEVICE_ENABLE)
#    if (defined(KEYBOARD_bastardkb_charybdis) || defined(KEYBOARD_handwired_tractyl_manuform))
#        include QMK_KEYBOARD_H

            static uint16_t last_cpi = {0xFFFF};
            uint16_t        curr_cpi = charybdis_get_pointer_sniping_enabled() ? charybdis_get_pointer_sniping_dpi()
                                                                               : charybdis_get_pointer_default_dpi();
            if (hue_redraw || last_cpi != curr_cpi) {
                last_cpi = curr_cpi;
                xpos     = 5;
                xpos += qp_drawtext_recolor(display, xpos, ypos, font_oled, "CPI:   ", curr_hsv.primary.h,
                                            curr_hsv.primary.s, curr_hsv.primary.v, 0, 0, 0);
                snprintf(buf, sizeof(buf), "%5u", curr_cpi);
                xpos += qp_drawtext_recolor(display, xpos, ypos, font_oled, buf, curr_hsv.secondary.h,
                                            curr_hsv.secondary.s, curr_hsv.secondary.v, 0, 0, 0);
            }
            ypos += font_oled->line_height + 4;
#    endif
            ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            // Pointing Device Auto Mouse Layer

#    ifdef POINTING_DEVICE_AUTO_MOUSE_ENABLE
            static uint8_t last_am_state     = 0xFF;
            bool           auto_mouse_redraw = false;
            if (last_am_state != get_auto_mouse_enable()) {
                last_am_state     = get_auto_mouse_enable();
                auto_mouse_redraw = true;
            }
            if (hue_redraw || auto_mouse_redraw) {
                xpos = 5;
                qp_drawtext_recolor(display, xpos, ypos, font_oled, "Auto Layer:", curr_hsv.primary.h,
                                    curr_hsv.primary.s, curr_hsv.primary.v, 0, 0, 0);
            }
            ypos += font_oled->line_height + 4;

            static uint8_t last_am_layer = 0xFF;
            if (hue_redraw || last_am_layer != get_auto_mouse_layer() || auto_mouse_redraw) {
                last_am_state = get_auto_mouse_layer();
                xpos          = 5;
                snprintf(buf, sizeof(buf), "%12s", layer_name(get_auto_mouse_layer()));
                qp_drawtext_recolor(display, xpos, ypos, font_oled,
                                    truncate_text(buf, 80 - 5 - 2, font_oled, false, false),
                                    get_auto_mouse_enable() ? curr_hsv.secondary.h : curr_hsv.primary.h,
                                    get_auto_mouse_enable() ? curr_hsv.secondary.s : curr_hsv.primary.s,
                                    get_auto_mouse_enable() ? curr_hsv.primary.v : disabled_val, 0, 0, 0);
            }
            ypos += font_oled->line_height + 4;

#    endif

            ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            // Pointing Device Drag Scroll

#    if (defined(KEYBOARD_bastardkb_charybdis) || defined(KEYBOARD_handwired_tractyl_manuform))
            static uint32_t last_ds_state = 0xFFFFFFFF;
            if (hue_redraw || last_ds_state != charybdis_get_pointer_dragscroll_enabled()) {
                last_ds_state = charybdis_get_pointer_dragscroll_enabled();
                xpos          = 5;
                xpos += qp_drawtext_recolor(
                    display, xpos, ypos, font_oled, "Drag-Scroll",
                    charybdis_get_pointer_dragscroll_enabled() ? curr_hsv.secondary.h : curr_hsv.primary.h,
                    charybdis_get_pointer_dragscroll_enabled() ? curr_hsv.secondary.s : curr_hsv.primary.s,
                    charybdis_get_pointer_dragscroll_enabled() ? curr_hsv.primary.v : disabled_val, 0, 0, 0);
            }
            ypos += font_oled->line_height + 4;

            ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            // Pointing Device Sniping mode

            static uint8_t last_sp_state = 0xFF;

            if (hue_redraw || last_sp_state != charybdis_get_pointer_sniping_enabled()) {
                last_sp_state = charybdis_get_pointer_sniping_enabled();
                xpos          = 5;
                xpos += qp_drawtext_recolor(
                    display, xpos, ypos, font_oled, "Sniping",
                    charybdis_get_pointer_sniping_enabled() ? curr_hsv.secondary.h : curr_hsv.primary.h,
                    charybdis_get_pointer_sniping_enabled() ? curr_hsv.secondary.s : curr_hsv.primary.s,
                    charybdis_get_pointer_sniping_enabled() ? curr_hsv.primary.v : disabled_val, 0, 0, 0);
            }
#    endif
#endif // POINTING_DEVICE_ENABLE

#ifdef CUSTOM_UNICODE_ENABLE
            ypos                             = 80 + 4;
            static uint8_t last_unicode_mode = UNICODE_MODE_COUNT;
            if (hue_redraw || last_unicode_mode != get_unicode_input_mode()) {
                last_unicode_mode   = get_unicode_input_mode();
                xpos                = 80 + 4;
                uint8_t xpos_offset = xpos +
                                      qp_drawtext_recolor(display, xpos, ypos, font_oled, "Unicode", curr_hsv.primary.h,
                                                          curr_hsv.primary.s, curr_hsv.primary.v, 0, 0, 0) +
                                      4;
                switch (last_unicode_mode) {
                    case UNICODE_MODE_WINCOMPOSE:
                    case UNICODE_MODE_WINDOWS:
                        qp_drawimage(display, xpos_offset, ypos + 2, windows_logo);
                        break;
                    case UNICODE_MODE_MACOS:
                        qp_drawimage(display, xpos_offset, ypos + 2, apple_logo);
                        break;
                    case UNICODE_MODE_LINUX:
                    case UNICODE_MODE_BSD:
                    case UNICODE_MODE_EMACS:
                        qp_drawimage(display, xpos_offset, ypos + 2, linux_logo);
                        break;
                }
                ypos += font_oled->line_height + 4;
                qp_drawtext_recolor(display, xpos + 8, ypos, font_oled, "Mode", curr_hsv.primary.h, curr_hsv.primary.s,
                                    curr_hsv.primary.v, 0, 0, 0);
            }

            ypos                                    = 80 + 4;
            static uint8_t last_unicode_typing_mode = 0;
            if (hue_redraw || last_unicode_typing_mode != user_runtime_state.unicode.typing_mode) {
                last_unicode_typing_mode = user_runtime_state.unicode.typing_mode;
                xpos                     = 149 + 4;
                qp_drawtext_recolor(display, xpos, ypos, font_oled, "Typing Mode:", curr_hsv.primary.h,
                                    curr_hsv.primary.s, curr_hsv.primary.v, 0, 0, 0);
                ypos += font_oled->line_height + 4;
                snprintf(buf, sizeof(buf), "%14s", unicode_typing_mode(last_unicode_typing_mode));
                qp_drawtext_recolor(display, xpos, ypos, font_oled, buf, curr_hsv.secondary.h, curr_hsv.secondary.s,
                                    curr_hsv.secondary.v, 0, 0, 0);
            }
#endif // CUSTOM_UNICODE_ENABLE

            ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            // Mods

            ypos                        = 107 + 3;
            static uint8_t last_mods    = {0};
            uint8_t        current_mods = get_mods() | get_weak_mods() | get_oneshot_mods();
            if (hue_redraw || last_mods != current_mods || keymap_config_redraw) {
                last_mods = current_mods;
                xpos      = 5;
                xpos += qp_drawtext_recolor(display, xpos, ypos + 1, font_oled, "Modifiers:", curr_hsv.primary.h,
                                            curr_hsv.primary.s, curr_hsv.primary.v, 0, 0, 0) +
                        2;

                if (qp_drawimage_recolor(display, xpos, ypos, shift_icon,
                                         last_mods & MOD_BIT_LSHIFT ? curr_hsv.secondary.h : curr_hsv.primary.h,
                                         curr_hsv.primary.s,
                                         last_mods & MOD_BIT_LSHIFT ? curr_hsv.primary.v : disabled_val, 0, 0, 0)) {
                    xpos += shift_icon->width + 2;
                }
                if (qp_drawimage_recolor(display, xpos, ypos,
                                         keymap_config.swap_lctl_lgui ? command_icon : windows_icon,
                                         last_mods & MOD_BIT_LGUI ? curr_hsv.secondary.h : curr_hsv.primary.h,
                                         last_mods & MOD_BIT_LGUI ? curr_hsv.secondary.s : curr_hsv.primary.s,
                                         last_mods & MOD_BIT_LGUI ? curr_hsv.primary.v : disabled_val, 0, 0, 0)) {
                    xpos += windows_icon->width + 2;
                }
                if (qp_drawimage_recolor(display, xpos, ypos, alt_icon,
                                         last_mods & MOD_BIT_LALT ? curr_hsv.secondary.h : curr_hsv.primary.h,
                                         last_mods & MOD_BIT_LALT ? curr_hsv.secondary.s : curr_hsv.primary.s,
                                         last_mods & MOD_BIT_LALT ? curr_hsv.primary.v : disabled_val, 0, 0, 0)) {
                    xpos += alt_icon->width + 2;
                }
                if (qp_drawimage_recolor(display, xpos, ypos, control_icon,
                                         last_mods & MOD_BIT_LCTRL ? curr_hsv.secondary.h : curr_hsv.primary.h,
                                         last_mods & MOD_BIT_LCTRL ? curr_hsv.secondary.s : curr_hsv.primary.s,
                                         last_mods & MOD_BIT_LCTRL ? curr_hsv.primary.v : disabled_val, 0, 0, 0)) {
                    xpos += control_icon->width + 2;
                }
                if (qp_drawimage_recolor(display, xpos, ypos, control_icon,
                                         last_mods & MOD_BIT_RCTRL ? curr_hsv.secondary.h : curr_hsv.primary.h,
                                         last_mods & MOD_BIT_RCTRL ? curr_hsv.secondary.s : curr_hsv.primary.s,
                                         last_mods & MOD_BIT_RCTRL ? curr_hsv.primary.v : disabled_val, 0, 0, 0)) {
                    xpos += control_icon->width + 2;
                }
                if (qp_drawimage_recolor(display, xpos, ypos, alt_icon,
                                         last_mods & MOD_BIT_RALT ? curr_hsv.secondary.h : curr_hsv.primary.h,
                                         last_mods & MOD_BIT_RALT ? curr_hsv.secondary.s : curr_hsv.primary.s,
                                         last_mods & MOD_BIT_RALT ? curr_hsv.primary.v : disabled_val, 0, 0, 0)) {
                    xpos += alt_icon->width + 2;
                }
                if (qp_drawimage_recolor(display, xpos, ypos,
                                         keymap_config.swap_rctl_rgui ? command_icon : windows_icon,
                                         last_mods & MOD_BIT_RGUI ? curr_hsv.secondary.h : curr_hsv.primary.h,
                                         last_mods & MOD_BIT_RGUI ? curr_hsv.secondary.s : curr_hsv.primary.s,
                                         last_mods & MOD_BIT_RGUI ? curr_hsv.primary.v : disabled_val, 0, 0, 0)) {
                    xpos += windows_icon->width + 2;
                }
                if (qp_drawimage_recolor(display, xpos, ypos, shift_icon,
                                         last_mods & MOD_BIT_RSHIFT ? curr_hsv.secondary.h : curr_hsv.primary.h,
                                         last_mods & MOD_BIT_RSHIFT ? curr_hsv.secondary.s : curr_hsv.primary.s,
                                         last_mods & MOD_BIT_RSHIFT ? curr_hsv.primary.v : disabled_val, 0, 0, 0)) {
                    xpos += shift_icon->width + 2;
                }
            }

#ifdef OS_DETECTION_ENABLE
            ypos                                    = 107 + 4;
            xpos                                    = 159;
            static os_variant_t last_detected_os    = {0};
            os_variant_t        current_detected_os = detected_host_os();
            if (hue_redraw || last_detected_os != current_detected_os) {
                last_detected_os = current_detected_os;

                xpos += qp_drawtext_recolor(display, xpos, ypos, font_oled, "OS: ", curr_hsv.primary.h,
                                            curr_hsv.primary.s, curr_hsv.primary.v, 0, 0, 0);

                snprintf(buf, sizeof(buf), "%9s", os_variant_to_string(current_detected_os));
                qp_drawtext_recolor(display, xpos, ypos, font_oled, buf, curr_hsv.secondary.h, curr_hsv.secondary.s,
                                    curr_hsv.secondary.v, 0, 0, 0);
            }
#endif // OS_DETECTION_ENABLE

            ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            //  Default layer state

            ypos                                    = 122 + 4;
            xpos                                    = 125;
            bool                 layer_state_redraw = false, dl_state_redraw = false;
            static layer_state_t last_layer_state = 0, last_dl_state = 0;
            if (last_layer_state != layer_state) {
                last_layer_state   = layer_state;
                layer_state_redraw = true;
            }
            if (last_dl_state != default_layer_state) {
                last_dl_state   = default_layer_state;
                dl_state_redraw = true;
            }

            if (hue_redraw || dl_state_redraw || layer_state_redraw) {
                qp_drawtext_recolor(display, xpos, ypos, font_oled, "Layout: ", curr_hsv.primary.h, curr_hsv.primary.s,
                                    curr_hsv.primary.v, 0, 0, 0);
                ypos += font_oled->line_height + 4;
                snprintf(buf, sizeof(buf), "%10s", get_layer_name_string(default_layer_state, false, true));
                qp_drawtext_recolor(display, xpos, ypos, font_oled, buf, curr_hsv.secondary.h, curr_hsv.secondary.s,
                                    curr_hsv.secondary.v, 0, 0, 0);
            } else {
                ypos += font_oled->line_height + 4;
            }
            ypos += font_oled->line_height + 4;

            ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            // Layer State

            if (hue_redraw || layer_state_redraw) {
                qp_drawtext_recolor(display, xpos, ypos, font_oled, "Layer: ", curr_hsv.primary.h, curr_hsv.primary.s,
                                    curr_hsv.primary.v, 0, 0, 0);
                ypos += font_oled->line_height + 4;
                layer_state_t temp = last_layer_state;
                if (is_gaming_layer_active(last_layer_state)) {
                    temp = last_layer_state & ~((layer_state_t)1 << _MOUSE);
                }
                snprintf(buf, sizeof(buf), "%10s", get_layer_name_string(temp, false, false));
                qp_drawtext_recolor(display, xpos, ypos, font_oled, buf, curr_hsv.secondary.h, curr_hsv.secondary.s,
                                    curr_hsv.secondary.v, 0, 0, 0);
                ypos = 122 + 4;
                xpos = 190;
                qp_drawimage_recolor(display, xpos, ypos, gamepad_icon, curr_hsv.primary.h, curr_hsv.primary.s,
                                     layer_state_cmp(last_layer_state, _GAMEPAD) ? curr_hsv.primary.v : disabled_val, 0,
                                     0, 0);
                qp_drawimage_recolor(display, xpos + gamepad_icon->width + 6, ypos + 4, mouse_icon, curr_hsv.primary.h,
                                     curr_hsv.primary.s,
                                     layer_state_cmp(layer_state, _MOUSE) ? curr_hsv.primary.v : disabled_val, 0, 0, 0);
                ypos += gamepad_icon->height + 2;
                qp_drawtext_recolor(
                    display, xpos, ypos, font_oled, "Diablo",
                    layer_state_cmp(last_layer_state, _DIABLO) ? 0 : curr_hsv.primary.h, curr_hsv.primary.s,
                    layer_state_cmp(last_layer_state, _DIABLO) ? curr_hsv.primary.v : disabled_val, 0, 0, 0);
                ypos += font_oled->line_height + 2;
                qp_drawtext_recolor(
                    display, xpos, ypos, font_oled, "Diablo 2",
                    layer_state_cmp(last_layer_state, _DIABLOII) ? 0 : curr_hsv.primary.h, curr_hsv.primary.s,
                    layer_state_cmp(last_layer_state, _DIABLOII) ? curr_hsv.primary.v : disabled_val, 0, 0, 0);
            }

            ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            // Autocorrection values

#ifdef AUTOCORRECT_ENABLE
            ypos = 122 + 4;
            extern bool     autocorrect_str_has_changed;
            extern char     autocorrected_str_raw[2][21];
            static uint32_t autocorrect_timer = 0;
            if (timer_elapsed(autocorrect_timer) > 125) {
                autocorrect_timer           = timer_read();
                autocorrect_str_has_changed = true;
            }

            if (hue_redraw || autocorrect_str_has_changed) {
                xpos = 5;
                qp_drawtext_recolor(display, xpos, ypos, font_oled, "Autocorrected: ", curr_hsv.primary.h,
                                    curr_hsv.primary.s, curr_hsv.primary.v, 0, 0, 0);
                ypos += font_oled->line_height + 4;
                snprintf(buf, sizeof(buf), "%19s", autocorrected_str_raw[0]);
                qp_drawtext_recolor(display, xpos, ypos, font_oled, buf, curr_hsv.secondary.h, curr_hsv.secondary.s,
                                    curr_hsv.secondary.v, 0, 0, 0);

                ypos += font_oled->line_height + 4;
                qp_drawtext_recolor(display, xpos, ypos, font_oled, "Original Text: ", curr_hsv.primary.h,
                                    curr_hsv.primary.s, curr_hsv.primary.v, 0, 0, 0);
                ypos += font_oled->line_height + 4;
                snprintf(buf, sizeof(buf), "%19s", autocorrected_str_raw[1]);

                qp_drawtext_recolor(display, xpos, ypos, font_oled, buf, curr_hsv.secondary.h, curr_hsv.secondary.s,
                                    curr_hsv.secondary.v, 0, 0, 0);
                autocorrect_str_has_changed = false;
            } else {
                // we called ypos inside the function ... to make sure we don't skip a line on future passes ....
                ypos += (font_oled->line_height + 4) * 3;
            }
#endif // AUTOCORRECT_ENABLE

            ypos += font_oled->line_height + 1;

            // Keylogger
            ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifdef DISPLAY_KEYLOGGER_ENABLE // keep at very end
            static uint32_t last_klog_update = 0;
            if (timer_elapsed32(last_klog_update) > 125 || keylogger_has_changed) {
                last_klog_update      = timer_read32();
                keylogger_has_changed = true;
            }

            ypos = height - (font_mono->line_height + 2);
            if (keylogger_has_changed) {
                static int max_klog_xpos = 0;
                xpos                     = 27;
                snprintf(buf, sizeof(buf), "Keylogger: %s", display_keylogger_string);

                xpos += qp_drawtext_recolor(display, xpos, ypos, font_mono, buf, 0, 255, 0, curr_hsv.primary.h,
                                            curr_hsv.primary.s, curr_hsv.primary.v);

                if (max_klog_xpos < xpos) {
                    max_klog_xpos = xpos;
                }
                // qp_rect(display, xpos, ypos, max_klog_xpos, ypos + font->line_height, 0, 0, 255, true);
                keylogger_has_changed = false;
            }
#endif // DISPLAY_KEYLOGGER_ENABLE

            // RTC
            ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

            ypos -= (font_oled->line_height + 4);
#ifdef RTC_ENABLE
            render_rtc_time(display, font_oled, 5, ypos, width, hue_redraw, &curr_hsv.primary);
#else
            if (hue_redraw) {
                snprintf(buf, sizeof(buf), "Built on: %s", QMK_BUILDDATE);

                uint8_t title_width = qp_textwidth(font_oled, buf);
                if (title_width > (width - 6)) {
                    title_width = width - 6;
                }
                uint8_t title_xpos = (width - title_width) / 2;

                xpos += qp_drawtext_recolor(display, title_xpos, ypos, font_oled, buf, curr_hsv.primary.h,
                                            curr_hsv.primary.s, curr_hsv.primary.v, 0, 0, 0);
            }

#endif // RTC_ENABLE

            static bool force_full_block_redraw = false;
            ypos                                = 172;
#if !defined(SPLIT_KEYBOARD)
            if (render_menu(menu_surface, 0, 0, SURFACE_MENU_WIDTH, SURFACE_MENU_HEIGHT)) {
                force_full_block_redraw = true;
            } else
#else
            // force set the dirty flag to false since we aren't actually rendering the menu on this side.
            user_runtime_state.menu_state.dirty = false;
#endif
            {
                bool     block_redraw = false;
                uint16_t surface_ypos = 2, surface_xpos = 3;

                static uint8_t last_display_mode = 0xFF;
                if (last_display_mode != userspace_config.painter.display_mode) {
                    last_display_mode       = userspace_config.painter.display_mode;
                    force_full_block_redraw = true;
                }

                if (force_full_block_redraw || screen_saver_redraw) {
                    qp_rect(menu_surface, 0, 0, SURFACE_MENU_WIDTH - 1, SURFACE_MENU_HEIGHT - 1, 0, 0, 0, true);
                    force_full_block_redraw = false;
                    block_redraw            = true;
                }

                switch (userspace_config.painter.display_mode) {
                    case 0:
                        painter_render_console(menu_surface, font_oled, 2, surface_ypos, hue_redraw || block_redraw,
                                               &curr_hsv.primary, DISPLAY_CONSOLE_LOG_LINE_START,
                                               DISPLAY_CONSOLE_LOG_LINE_NUM);
                        break;
                    case 1:
                        //  Layer Map render
                        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifdef LAYER_MAP_ENABLE
                        if (hue_redraw || block_redraw || layer_map_has_updated) {
                            surface_ypos += font_oled->line_height + 4;
                            uint16_t temp_ypos = surface_ypos;
                            for (uint8_t y = 0; y < LAYER_MAP_ROWS; y++) {
                                surface_xpos = 20;
                                for (uint8_t x = 0; x < LAYER_MAP_COLS; x++) {
                                    uint16_t keycode = extract_basic_keycode(layer_map[y][x], NULL, false);
                                    wchar_t  code[2] = {0};

                                    // if (keycode == UC_IRNY) {
                                    //     code[0] = L'⸮';
                                    // } else if (keycode == UC_CLUE) {
                                    //     code[0] = L'‽'
                                    // } else
                                    if (keycode > 0xFF) {
                                        keycode = KC_SPC;
                                    }
                                    if (keycode < ARRAY_SIZE(code_to_name)) {
                                        code[0] = pgm_read_byte(&code_to_name[keycode]);
                                    }
                                    surface_xpos += qp_drawtext_recolor(
                                        menu_surface, surface_xpos, temp_ypos, font_oled, (char *)code,
                                        curr_hsv.primary.h, curr_hsv.primary.s,
                                        peek_matrix_layer_map(y, x) ? 0 : curr_hsv.primary.v, curr_hsv.secondary.h,
                                        curr_hsv.secondary.s, peek_matrix_layer_map(y, x) ? curr_hsv.secondary.v : 0);
                                    surface_xpos += qp_drawtext_recolor(menu_surface, surface_xpos, temp_ypos,
                                                                        font_oled, " ", 0, 0, 0, 0, 0, 0);
                                }
                                temp_ypos += font_oled->line_height + 4;
                            }
                            layer_map_has_updated = false;
                        }
                        break;
#endif
                    case 2:
                        if (hue_redraw || block_redraw) {
                            static uint16_t max_font_xpos[3][4] = {0};
                            render_character_set(menu_surface, &surface_xpos, max_font_xpos[0], &surface_ypos,
                                                 font_thintel, curr_hsv.primary.h, curr_hsv.primary.s,
                                                 curr_hsv.primary.v, 0, 0, 0);
                            render_character_set(menu_surface, &surface_xpos, max_font_xpos[1], &surface_ypos,
                                                 font_mono, curr_hsv.primary.h, curr_hsv.primary.s, curr_hsv.primary.v,
                                                 0, 0, 0);
                            render_character_set(menu_surface, &surface_xpos, max_font_xpos[2], &surface_ypos,
                                                 font_oled, curr_hsv.primary.h, curr_hsv.primary.s, curr_hsv.primary.v,
                                                 0, 0, 0);
                        }
                        break;
                    case 3:
                        if (hue_redraw || block_redraw) {
                            surface_xpos = 5;
                            surface_ypos = 5;
                            snprintf(buf, sizeof(buf), "%s", QMK_BUILDDATE);
                            surface_xpos += qp_drawtext_recolor(menu_surface, surface_xpos, surface_ypos, font_oled,
                                                                "Built on: ", curr_hsv.primary.h, curr_hsv.primary.s,
                                                                curr_hsv.primary.v, 0, 0, 0);
                            qp_drawtext_recolor(menu_surface, surface_xpos, surface_ypos, font_oled, buf,
                                                curr_hsv.secondary.h, curr_hsv.secondary.s, curr_hsv.secondary.v, 0, 0,
                                                0);
                            surface_xpos = 5;
                            surface_ypos += font_oled->line_height + 4;
                            snprintf(buf, sizeof(buf), "%s", QMK_VERSION);
                            surface_xpos += qp_drawtext_recolor(menu_surface, surface_xpos, surface_ypos, font_oled,
                                                                "Built from: ", curr_hsv.primary.h, curr_hsv.primary.s,
                                                                curr_hsv.primary.v, 0, 0, 0);
                            qp_drawtext_recolor(menu_surface, surface_xpos, surface_ypos, font_oled, buf,
                                                curr_hsv.secondary.h, curr_hsv.secondary.s, curr_hsv.secondary.v, 0, 0,
                                                0);
                            surface_ypos += font_oled->line_height + 4;
                            surface_xpos = 5;
                            surface_xpos += qp_drawtext_recolor(menu_surface, surface_xpos, surface_ypos, font_oled,
                                                                "Built with: ", curr_hsv.primary.h, curr_hsv.primary.s,
                                                                curr_hsv.primary.v, 0, 0, 0);
                            qp_drawtext_recolor(menu_surface, surface_xpos, surface_ypos, font_oled, __VERSION__,
                                                curr_hsv.secondary.h, curr_hsv.secondary.s, curr_hsv.secondary.v, 0, 0,
                                                0);

                            qp_drawimage_recolor(menu_surface, 0, (SURFACE_MENU_HEIGHT - qmk_banner->height) - 3,
                                                 qmk_banner, curr_hsv.primary.h, curr_hsv.primary.s, curr_hsv.primary.v,
                                                 0, 0, 0);
                        }
                        break;
                    default:
                        break;
                }
            }
#ifdef SPLIT_KEYBOARD
        } else {
            static bool force_full_block_redraw = false;
            if (!is_transport_connected()) {
                force_full_block_redraw = true;
                return;
            }
#    if defined(RGBLIGHT_ENABLE)
            ypos = 20;
            xpos = 83;
            painter_render_rgb(display, font_oled, xpos, ypos, hue_redraw || rgb_redraw, &curr_hsv,
                               rgblight_get_effect_name, rgblight_get_hsv, false);
#    endif // RGBLIGHT_ENABLE

            if (render_menu(menu_surface, 0, 0, SURFACE_MENU_WIDTH, SURFACE_MENU_HEIGHT)) {
                force_full_block_redraw = true;
            } else {
                ypos = 3;
                if (force_full_block_redraw || screen_saver_redraw) {
                    qp_rect(menu_surface, 0, 0, SURFACE_MENU_WIDTH - 1, SURFACE_MENU_HEIGHT - 1, 0, 0, 0, true);
                }
                painter_render_console(menu_surface, font_oled, 2, ypos,
                                       hue_redraw || force_full_block_redraw || screen_saver_redraw, &curr_hsv.primary,
                                       DISPLAY_CONSOLE_LOG_LINE_START, DISPLAY_CONSOLE_LOG_LINE_NUM);

                force_full_block_redraw = false;
            }
            ypos = height - (16 + font_oled->line_height);
            render_rtc_time(display, font_oled, 5, ypos, width, hue_redraw, &curr_hsv.primary);
        }
        qp_surface_draw(menu_surface, display, 2, 172, screen_saver_redraw);

#endif // SPLIT_KEYBOARD
        forced_reinit       = false;
        screen_saver_redraw = false;
    }
    qp_flush(display);
    last_tick = now;
}

bool ili9341_display_shutdown(bool jump_to_bootloader) {
    painter_font_handle_t font_proggy = qp_load_font_mem(font_proggy_clean_15);
    uint16_t              width, height, ypos = 240;
    qp_get_geometry(display, &width, &height, NULL, NULL, NULL);
    painter_image_handle_t frame = NULL;
    qp_rect(display, 0, 0, width - 1, height - 1, 0, 0, 0, true);
    frame = qp_load_image_mem(gfx_qmk_logo_220x220);
    qp_drawimage_recolor(display, 10, 10, frame, 0, 0, 255, 0, 0, 0);

    char title[50] = {0};
    snprintf(title, sizeof(title), "%s", "Please Stand By...");
    uint8_t title_width = qp_textwidth(font_proggy, title);
    uint8_t title_xpos  = (width - title_width) / 2;
    qp_drawtext_recolor(display, title_xpos, ypos, font_proggy,
                        truncate_text(title, title_width, font_proggy, false, false), 0, 0, 255, 0, 0, 0);
    ypos += font_proggy->line_height + 4;
    snprintf(title, sizeof(title), "%s", jump_to_bootloader ? "Jumping to Bootloader..." : "Shutting Down...");
    title_width = qp_textwidth(font_proggy, title);
    title_xpos  = (width - title_width) / 2;
    qp_drawtext_recolor(display, title_xpos, ypos, font_proggy,
                        truncate_text(title, title_width, font_proggy, false, false), 0, 0, 255, 0, 0, 0);
    ypos += font_proggy->line_height + 4;
    if (!eeconfig_is_enabled()) {
        snprintf(title, sizeof(title), "%s", "Reinitialiing EEPROM...");
        title_width = qp_textwidth(font_proggy, title);
        title_xpos  = (width - title_width) / 2;
        qp_drawtext_recolor(display, title_xpos, ypos, font_proggy,
                            truncate_text(title, title_width, font_proggy, false, false), 0, 0, 255, 0, 0, 0);
        ypos += font_proggy->line_height + 4;
    }
    qp_flush(display);
    return false;
}
