// Copyright 2024 Christopher Courtney, aka Drashna Jael're  (@drashna) <drashna@live.com>
// Copyright 2018-2024 Nick Brassel (@tzarc)
// SPDX-License-Identifier: GPL-3.0-or-later

#include "display/painter/graphics/samurai-cyberpunk-minimal-dark-8k-b3-240x320.qgf.h"
#include "drashna.h"
#include "qp.h"
#include "qp_comms.h"
#include "display/display.h"
#include "display/painter/painter.h"
#include "display/painter/ili9341_display.h"
#ifdef CUSTOM_SPLIT_TRANSPORT_SYNC
#    include "split/transport_sync.h"
#endif
#ifdef RTC_ENABLE
#    include "features/rtc/rtc.h"
#endif
#ifdef LAYER_MAP_ENABLE
#    include "features/layer_map.h"
#endif

#include <math.h>
#include <stdio.h>
#include <ctype.h>

painter_device_t ili9341_display;

painter_font_handle_t font_thintel, font_mono, font_oled;

painter_image_handle_t frame;
painter_image_handle_t lock_caps_on, lock_caps_off;
painter_image_handle_t lock_num_on, lock_num_off;
painter_image_handle_t lock_scrl_on, lock_scrl_off;
painter_image_handle_t windows_logo, apple_logo, linux_logo;
painter_image_handle_t mouse_icon;

/**
 * @brief Initializes the display, clears it and sets frame and title
 *
 */
void init_display_ili9341(void) {
    font_thintel = qp_load_font_mem(font_thintel15);
    font_mono    = qp_load_font_mem(font_ProggyTiny15);
    font_oled    = qp_load_font_mem(font_oled_font);

    // ters1 = qp_load_image_mem(gfx_ters1);
    lock_caps_on  = qp_load_image_mem(gfx_lock_caps_ON);
    lock_caps_off = qp_load_image_mem(gfx_lock_caps_OFF);
    lock_num_on   = qp_load_image_mem(gfx_lock_NUM_ON);
    lock_num_off  = qp_load_image_mem(gfx_lock_NUM_OFF);
    lock_scrl_on  = qp_load_image_mem(gfx_lock_scrl_ON);
    lock_scrl_off = qp_load_image_mem(gfx_lock_scrl_OFF);
    // test_anim = qp_load_image_mem(gfx_test_anim);
    // matrix = qp_load_image_mem(gfx_matrix);
    windows_logo = qp_load_image_mem(gfx_windows_logo);
    apple_logo   = qp_load_image_mem(gfx_apple_logo);
    linux_logo   = qp_load_image_mem(gfx_linux_logo);

    mouse_icon = qp_load_image_mem(gfx_mouse_icon);

    ili9341_display =
        qp_ili9341_make_spi_device(240, 320, DISPLAY_CS_PIN, DISPLAY_DC_PIN, DISPLAY_RST_PIN, DISPLAY_SPI_DIVIDER, 0);

    wait_ms(50);

    uint16_t width;
    uint16_t height;

    qp_init(ili9341_display, QP_ROTATION_180);
    // if needs inversion
#ifdef DISPLAY_INVERTED
    qp_comms_start(ili9341_display);
    qp_comms_command(ili9341_display, ILI9XXX_CMD_INVERT_OFF);
    qp_comms_stop(ili9341_display);
#endif
    qp_get_geometry(ili9341_display, &width, &height, NULL, NULL, NULL);
    qp_clear(ili9341_display);
    qp_rect(ili9341_display, 0, 0, width - 1, height - 1, 0, 0, 0, true);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Initial render of frame/logo

    if (is_keyboard_master()) {
        frame = qp_load_image_mem(gfx_frame);
        qp_drawimage_recolor(ili9341_display, 0, 0, frame, 0, 0, 255, 0, 0, 0);

        char title[50] = {0};
        snprintf(title, sizeof(title), "%s", PRODUCT);
        uint8_t title_width = qp_textwidth(font_thintel, title);
        if (title_width > (width - 55)) {
            title_width = width - 55;
        }
        uint8_t title_xpos = (width - title_width) / 2;
        qp_drawtext_recolor(ili9341_display, title_xpos, 2, font_thintel,
                            truncate_text(title, title_width, font_thintel, false, false), 0, 0, 0, 0, 0, 255);
    } else {
        frame = qp_load_image_mem(gfx_samurai_cyberpunk_minimal_dark_8k_b3_240x320);
        qp_drawimage_recolor(ili9341_display, 0, 0, frame, 0, 0, 255, 0, 0, 0);
    }
    qp_close_image(frame);
    qp_power(ili9341_display, true);
}

void ili9341_display_power(bool on) {
    qp_power(ili9341_display, on);
}

#define NUM_LOG_LINES (10 + 1)
#define MAX_LOG_LINE  38

static bool    needs_redraw  = false;
static uint8_t log_write_idx = 0;
static char    loglines[NUM_LOG_LINES][MAX_LOG_LINE + 2];
static char*   logline_ptrs[NUM_LOG_LINES];

void display_sendchar_hook(uint8_t c) {
    static bool first_setup = false;
    if (!first_setup) {
        memset(loglines, 0, sizeof(loglines));
        for (int i = 0; i < NUM_LOG_LINES; ++i) {
            logline_ptrs[i] = loglines[i];
        }
        first_setup = true;
    }

    if (c == '\n') {
        logline_ptrs[NUM_LOG_LINES - 1][log_write_idx] = 0;
        char* tmp                                      = logline_ptrs[0];
        for (int i = 0; i < NUM_LOG_LINES - 1; ++i) {
            logline_ptrs[i] = logline_ptrs[i + 1];
        }
        logline_ptrs[NUM_LOG_LINES - 1]    = tmp;
        log_write_idx                      = 0;
        logline_ptrs[NUM_LOG_LINES - 1][0] = 0;
        needs_redraw                       = true;
    } else if (log_write_idx >= (MAX_LOG_LINE)) {
        // Ignore.
    } else {
        logline_ptrs[NUM_LOG_LINES - 1][log_write_idx++] = c;
        logline_ptrs[NUM_LOG_LINES - 1][log_write_idx]   = 0;
        needs_redraw                                     = true;
    }
}

__attribute__((weak)) void ili9341_draw_user(void) {
    bool hue_redraw = false;
#if defined(RGBLIGHT_ENABLE) || defined(RGB_MATRIX_ENABLE)
    static uint16_t last_hue = {0xFFFF};
    uint8_t         curr_hue = rgblight_get_hue();
    if (last_hue != curr_hue) {
        last_hue   = curr_hue;
        hue_redraw = true;
    }
#else
    static uint32_t last_update = 0;
    if (timer_elapsed32(last_update) > 125) {
        last_update = timer_read32();
        hue_redraw  = true;
    }
#endif

    uint16_t width;
    uint16_t height;
    qp_get_geometry(ili9341_display, &width, &height, NULL, NULL, NULL);

#if defined(RGB_MATRIX_ENABLE) || defined(RGBLIGHT_ENABLE)
    bool            rgb_effect_redraw = false;
    static uint16_t last_effect       = 0xFFFF;
#    if defined(RGB_MATRIX_ENABLE)
    uint8_t curr_effect = rgb_matrix_get_mode();
#    else
    uint8_t curr_effect = rgblight_get_mode();
#    endif
    if (last_effect != curr_effect) {
        last_effect       = curr_effect;
        rgb_effect_redraw = true;
    }
#endif

    if (is_keyboard_master()) {
        char     buf[50] = {0};
        uint16_t ypos    = 18;
        uint16_t xpos    = 5;

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // LED Lock Indicators

#ifdef QP_LOCK_LOGO_ENABLE
        static led_t last_led_state = {0};
        if (hue_redraw || last_led_state.raw != host_keyboard_led_state().raw) {
            last_led_state.raw = host_keyboard_led_state().raw;
            qp_drawimage_recolor(ili9341_display, xpos, ypos, last_led_state.caps_lock ? lock_caps_on : lock_caps_off,
                                 curr_hue, 255, last_led_state.caps_lock ? 255 : 32, curr_hue, 255, 0);
            xpos += lock_caps_on->width + 4;
            qp_drawimage_recolor(ili9341_display, xpos, ypos, last_led_state.num_lock ? lock_num_on : lock_num_off,
                                 curr_hue, 255, last_led_state.num_lock ? 255 : 32, curr_hue, 255, 0);
            xpos += lock_num_on->width + 4;
            qp_drawimage_recolor(ili9341_display, xpos, ypos, last_led_state.scroll_lock ? lock_scrl_on : lock_scrl_off,
                                 curr_hue, 255, last_led_state.scroll_lock ? 255 : 32, curr_hue, 255, 0);
        }

        ypos += lock_caps_on->height + 4;
#endif // QP_LOCK_LOGO_ENABLE

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        //  WPM

        static uint16_t max_wpm_xpos = 0;
#ifdef WPM_ENABLE

        bool            wpm_redraw      = false;
        static uint32_t last_wpm_update = 0;
        if (timer_elapsed32(last_wpm_update) > 125) {
            last_wpm_update = timer_read32();
            wpm_redraw      = true;
        }

        if (hue_redraw || wpm_redraw) {
            xpos = 5;
            snprintf(buf, sizeof(buf), "WPM: %3u", get_current_wpm());
            xpos +=
                qp_drawtext_recolor(ili9341_display, xpos, ypos, font_oled, buf, curr_hue, 255, 255, curr_hue, 255, 0);
            if (max_wpm_xpos < xpos) {
                max_wpm_xpos = xpos;
            }
            qp_rect(ili9341_display, xpos, ypos, max_wpm_xpos, ypos + font_oled->line_height, 0, 0, 0, true);
        }
#endif // WPM_ENABLE

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Matrix Scan rate

        bool            scan_redraw      = false;
        static uint32_t last_scan_update = 0;
        if (timer_elapsed32(last_scan_update) > 125) {
            last_scan_update = timer_read32();
            scan_redraw      = true;
        }
        static uint16_t max_scans_xpos = 0;
        if (hue_redraw || scan_redraw) {
            xpos = max_wpm_xpos == 0 ? 5 : max_wpm_xpos + 10;
            snprintf(buf, sizeof(buf), "SCANS: %4lu", get_matrix_scan_rate());
            xpos +=
                qp_drawtext_recolor(ili9341_display, xpos, ypos, font_oled, buf, curr_hue, 255, 255, curr_hue, 255, 0);
            if (max_scans_xpos < xpos) {
                max_scans_xpos = xpos;
            }
            qp_rect(ili9341_display, xpos, ypos, max_scans_xpos, ypos + font_oled->line_height, 0, 0, 0, true);
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Pointing Device CPI

#ifdef POINTING_DEVICE_ENABLE
        static uint16_t last_cpi   = {0xFFFF};
        uint16_t        curr_cpi   = charybdis_get_pointer_sniping_enabled() ? charybdis_get_pointer_sniping_dpi()
                                                                             : charybdis_get_pointer_default_dpi();
        bool            cpi_redraw = false;
        if (last_cpi != curr_cpi) {
            last_cpi   = curr_cpi;
            cpi_redraw = true;
        }
        static uint16_t max_cpi_xpos = 0;
        if (hue_redraw || cpi_redraw) {
            xpos = max_scans_xpos + 10;
            snprintf(buf, sizeof(buf), "CPI: %5u", curr_cpi);
            xpos +=
                qp_drawtext_recolor(ili9341_display, xpos, ypos, font_oled, buf, curr_hue, 255, 255, curr_hue, 255, 0);
            if (max_cpi_xpos < xpos) {
                max_cpi_xpos = xpos;
            }
            qp_rect(ili9341_display, xpos, ypos, max_cpi_xpos, ypos + font_oled->line_height, 0, 0, 0, true);
        }
        // if (hue_redraw) {
        //     xpos = 160;
        //     qp_rect(ili9341_display, xpos, ypos, xpos + 10, ypos + 10, 0, 0, 200, true);
        //     qp_drawimage(ili9341_display, xpos, ypos, mouse_icon);
        // }

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Pointing Device Drag Scroll

        bool            ds_state_redraw = false;
        static uint32_t last_ds_state   = 0xFFFFFFFF;
        if (last_ds_state != charybdis_get_pointer_dragscroll_enabled()) {
            last_ds_state   = charybdis_get_pointer_dragscroll_enabled();
            ds_state_redraw = true;
        }

        ypos += font_oled->line_height + 4;
        if (ds_state_redraw) {
            static int max_dss_xpos = 0;
            xpos                    = 5;
            const char* buf         = "DRAG SCROLL";
            xpos += qp_drawtext_recolor(ili9341_display, xpos, ypos, font_oled, buf,
                                        charybdis_get_pointer_dragscroll_enabled() ? 153 : 255, 255, 255,
                                        charybdis_get_pointer_dragscroll_enabled() ? 153 : 255, 255, 0);
            if (max_dss_xpos < xpos) {
                max_dss_xpos = xpos;
            }
            qp_rect(ili9341_display, xpos, ypos, max_dss_xpos, ypos + font_oled->line_height, 0, 0, 0, true);
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Pointing Device Auto Mouse Layer

#    ifdef POINTING_DEVICE_AUTO_MOUSE_ENABLE
        bool           am_state_redraw = false;
        static uint8_t last_am_state   = 0xFF;
        if (last_am_state != get_auto_mouse_enable()) {
            last_am_state   = get_auto_mouse_enable();
            am_state_redraw = true;
        }

        if (am_state_redraw) {
            static int max_ams_xpos = 0;
            xpos += 10;
            const char* buf = "Auto Layer";
            xpos += qp_drawtext_recolor(ili9341_display, xpos, ypos, font_oled, buf, get_auto_mouse_enable() ? 34 : 255,
                                        255, 255, charybdis_get_pointer_sniping_enabled() ? 34 : 255, 255, 0);
            if (max_ams_xpos < xpos) {
                max_ams_xpos = xpos;
            }
            qp_rect(ili9341_display, xpos, ypos, max_ams_xpos, ypos + font_oled->line_height, 0, 0, 0, true);
        }
#    endif

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Pointing Device Sniping mode

        bool           sp_state_redraw = false;
        static uint8_t last_sp_state   = 0xFF;
        if (last_sp_state != charybdis_get_pointer_sniping_enabled()) {
            last_sp_state   = charybdis_get_pointer_sniping_enabled();
            sp_state_redraw = true;
        }
        if (sp_state_redraw) {
            static int max_sps_xpos = 0;
            xpos += 10;
            const char* buf = "Sniping";
            xpos += qp_drawtext_recolor(ili9341_display, xpos, ypos, font_oled, buf,
                                        charybdis_get_pointer_sniping_enabled() ? 153 : 255, 255, 255,
                                        charybdis_get_pointer_sniping_enabled() ? 153 : 255, 255, 0);
            if (max_sps_xpos < xpos) {
                max_sps_xpos = xpos;
            }
            qp_rect(ili9341_display, xpos, ypos, max_sps_xpos, ypos + font_oled->line_height, 0, 0, 0, true);
        }
#endif // POINTING_DEVICE_ENABLE

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Keymap config (nkro, autocorrect, oneshots)

        ypos += font_oled->line_height + 4;
        static keymap_config_t last_keymap_config = {0};
        if (hue_redraw || last_keymap_config.raw != keymap_config.raw) {
            last_keymap_config.raw  = keymap_config.raw;
            static int max_bpm_xpos = 0;
            xpos                    = 5;
            qp_drawimage(ili9341_display, xpos, ypos, last_keymap_config.swap_lctl_lgui ? apple_logo : windows_logo);
            xpos += windows_logo->width + 5;
            xpos += qp_drawtext_recolor(ili9341_display, xpos, ypos, font_oled, (const char*)"NKRO",
                                        last_keymap_config.nkro ? 153 : 255, 255, 255, 0, 0, 0) +
                    5;
            xpos += qp_drawtext_recolor(ili9341_display, xpos, ypos, font_oled, (const char*)"CRCT",
                                        last_keymap_config.autocorrect_enable ? 153 : 255, 255, 255, 0, 0, 0) +
                    5;
            xpos += qp_drawtext_recolor(ili9341_display, xpos, ypos, font_oled, (const char*)"1SHT",
                                        last_keymap_config.oneshot_enable ? 153 : 255, 255, 255, 0, 0, 0);
            if (max_bpm_xpos < xpos) {
                max_bpm_xpos = xpos;
            }
            qp_rect(ili9341_display, xpos, ypos, max_bpm_xpos, ypos + font_oled->line_height, 0, 0, 0, true);
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Device Config (Audio, Audio Clicky, Host Driver lock, Swap Hands)

        ypos += font_oled->line_height + 4;
        static user_runtime_config_t last_user_state = {0};
        if (hue_redraw || last_user_state.raw != user_state.raw) {
            last_user_state.raw          = user_state.raw;
            static uint16_t max_upm_xpos = 0;
            xpos                         = windows_logo->width + 10;
            xpos += qp_drawtext_recolor(ili9341_display, xpos, ypos, font_oled, (const char*)"AUDIO",
                                        last_user_state.audio_enable ? 153 : 255, 255, 255, 0, 0, 0) +
                    5;
            xpos += qp_drawtext_recolor(ili9341_display, xpos, ypos, font_oled, (const char*)"CLCK",
                                        last_user_state.audio_clicky_enable ? 153 : 255, 255, 255, 0, 0, 0) +
                    5;
            xpos += qp_drawtext_recolor(ili9341_display, xpos, ypos, font_oled, (const char*)"HOST",
                                        !last_user_state.host_driver_disabled ? 153 : 255, 255, 255, 0, 0, 0) +
                    5;
            xpos += qp_drawtext_recolor(ili9341_display, xpos, ypos, font_oled, (const char*)"SWAP",
                                        last_user_state.swap_hands ? 153 : 255, 255, 255, 0, 0, 0);
            if (max_upm_xpos < xpos) {
                max_upm_xpos = xpos;
            }
            qp_rect(ili9341_display, xpos, ypos, max_upm_xpos, ypos + font_oled->line_height, 0, 0, 0, true);
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // LED Lock indicator (text)

        ypos += font_oled->line_height + 4;
        static led_t last_led_state = {0};
        if (hue_redraw || last_led_state.raw != host_keyboard_led_state().raw) {
            last_led_state.raw           = host_keyboard_led_state().raw;
            static uint16_t max_led_xpos = 0;
            xpos                         = 5;
            xpos += qp_drawtext_recolor(ili9341_display, xpos, ypos, font_oled, (const char*)"Locks:", 153, 255, 255,
                                        255, 255, 0) +
                    5;
            xpos += qp_drawtext_recolor(ili9341_display, xpos, ypos, font_oled, (const char*)"CAPS",
                                        last_led_state.caps_lock ? 153 : 255, 255, 255, 255, 255, 0) +
                    5;
            xpos += qp_drawtext_recolor(ili9341_display, xpos, ypos, font_oled, (const char*)"SCRL",
                                        last_led_state.scroll_lock ? 153 : 255, 255, 255, 255, 255, 0) +
                    5;
            xpos += qp_drawtext_recolor(ili9341_display, xpos, ypos, font_oled, (const char*)"NUM",
                                        last_led_state.num_lock ? 153 : 255, 255, 255, 255, 255, 0);
            if (max_led_xpos < xpos) {
                max_led_xpos = xpos;
            }
            qp_rect(ili9341_display, xpos, ypos, max_led_xpos, ypos + font_oled->line_height, 0, 0, 0, true);
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Mods

        ypos += font_oled->line_height + 4;
        static uint8_t last_mods    = {0};
        uint8_t        current_mods = get_mods() | get_weak_mods() | get_oneshot_mods();
        if (hue_redraw || last_mods != current_mods) {
            last_mods                    = current_mods;
            static uint16_t max_mod_xpos = 0;
            xpos                         = 5;
            xpos += qp_drawtext_recolor(ili9341_display, xpos, ypos, font_oled, (const char*)"Modifiers:", 153, 255,
                                        255, 255, 255, 0) +
                    5;
            xpos += qp_drawtext_recolor(ili9341_display, xpos, ypos, font_oled, (const char*)"Shift",
                                        last_mods & MOD_MASK_SHIFT ? 153 : 255, 255, 255, 255, 255, 0) +
                    5;
            xpos += qp_drawtext_recolor(ili9341_display, xpos, ypos, font_oled, (const char*)"Control",
                                        last_mods & MOD_MASK_CTRL ? 153 : 255, 255, 255, 255, 255, 0) +
                    5;
            xpos += qp_drawtext_recolor(ili9341_display, xpos, ypos, font_oled, (const char*)"Alt",
                                        last_mods & MOD_MASK_ALT ? 153 : 255, 255, 255, 255, 255, 0) +
                    5;
            xpos += qp_drawtext_recolor(ili9341_display, xpos, ypos, font_oled, (const char*)"GUI",
                                        last_mods & MOD_MASK_GUI ? 153 : 255, 255, 255, 255, 255, 0) +
                    5;
            if (max_mod_xpos < xpos) {
                max_mod_xpos = xpos;
            }
            qp_rect(ili9341_display, xpos, ypos, max_mod_xpos, ypos + font_oled->line_height, 0, 0, 0, true);
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        //  RGB Light Settings

#if defined(RGBLIGHT_ENABLE)
        ypos += font_oled->line_height + 4;
        if (hue_redraw || rgb_effect_redraw) {
            static uint16_t max_rgb_xpos = 0;
            xpos                         = 5;
            snprintf(buf, sizeof(buf), "RGB Light Mode: %s", rgblight_name(curr_effect));
            for (uint16_t i = 16; i < sizeof(buf); ++i) {
                if (buf[i] == 0)
                    break;
                else if (buf[i] == '_')
                    buf[i] = ' ';
                else if (buf[i - 1] == ' ')
                    buf[i] = toupper(buf[i]);
                else if (buf[i - 1] != ' ')
                    buf[i] = tolower(buf[i]);
            }

            xpos +=
                qp_drawtext_recolor(ili9341_display, xpos, ypos, font_oled, buf, curr_hue, 255, 255, curr_hue, 255, 0);
            if (max_rgb_xpos < xpos) {
                max_rgb_xpos = xpos;
            }
            qp_rect(ili9341_display, xpos, ypos, max_rgb_xpos, ypos + font_oled->line_height, 0, 0, 0, true);

            ypos += font_oled->line_height + 4;
            static uint16_t max_hsv_xpos = 0;
            xpos                         = 5;
            snprintf(buf, sizeof(buf), "RGB Light HSV: %3d, %3d, %3d", rgblight_get_hue(), rgblight_get_sat(),
                     rgblight_get_val());
            xpos +=
                qp_drawtext_recolor(ili9341_display, xpos, ypos, font_oled, buf, curr_hue, 255, 255, curr_hue, 255, 0);
            if (max_hsv_xpos < xpos) {
                max_hsv_xpos = xpos;
            }
            qp_rect(ili9341_display, xpos, ypos, max_hsv_xpos, ypos + font_oled->line_height, 0, 0, 0, true);
            qp_rect(ili9341_display, max_hsv_xpos + 5, ypos, max_hsv_xpos + 25, ypos + font_oled->line_height - 1,
                    rgblight_get_hue(), rgblight_get_sat(), (uint8_t)(rgblight_get_val() * 0xFF / RGBLIGHT_LIMIT_VAL),
                    true);
        } else {
            // we called ypos inside the function ... to make sure we don't skip a line on future passes ....
            ypos += font_oled->line_height + 4;
        }
#endif // RGBLIGHT_ENABLE

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // RGB Matrix Settings

#if defined(RGB_MATRIX_ENABLE)
        ypos += font_oled->line_height + 4;
        if (hue_redraw || rgb_effect_redraw) {
            static uint16_t max_rgb_xpos = 0;
            xpos                         = 5;
            snprintf(buf, sizeof(buf), "RGB Matrix Mode: %s", rgb_matrix_name(curr_effect));
            for (uint16_t i = 17; i < sizeof(buf); ++i) {
                if (buf[i] == 0)
                    break;
                else if (buf[i] == '_')
                    buf[i] = ' ';
                else if (buf[i - 1] == ' ')
                    buf[i] = toupper(buf[i]);
                else if (buf[i - 1] != ' ')
                    buf[i] = tolower(buf[i]);
            }

            xpos +=
                qp_drawtext_recolor(ili9341_display, xpos, ypos, font_oled, buf, curr_hue, 255, 255, curr_hue, 255, 0);
            if (max_rgb_xpos < xpos) {
                max_rgb_xpos = xpos;
            }
            qp_rect(ili9341_display, xpos, ypos, max_rgb_xpos, ypos + font_oled->line_height, 0, 0, 0, true);

            ypos += font_oled->line_height + 4;
            static uint16_t max_hsv_xpos = 0;
            xpos                         = 5;
            snprintf(buf, sizeof(buf), "RGB Matrix HSV: %3d, %3d, %3d", rgb_matrix_get_hue(), rgb_matrix_get_sat(),
                     rgb_matrix_get_val());
            xpos +=
                qp_drawtext_recolor(ili9341_display, xpos, ypos, font_oled, buf, curr_hue, 255, 255, curr_hue, 255, 0);
            if (max_hsv_xpos < xpos) {
                max_hsv_xpos = xpos;
            }
            qp_rect(ili9341_display, xpos, ypos, max_hsv_xpos, ypos + font_oled->line_height, 0, 0, 0, true);
            qp_rect(ili9341_display, max_hsv_xpos + 5, ypos, max_hsv_xpos + 25, ypos + font_oled->line_height - 1,
                    rgb_matrix_get_hue(), rgb_matrix_get_sat(),
                    (uint8_t)(rgb_matrix_get_val() * 0xFF / RGB_MATRIX_MAXIMUM_BRIGHTNESS), true);
        } else {
            // we called ypos inside the function ... to make sure we don't skip a line on future passes ....
            ypos += font_oled->line_height + 4;
        }
#endif // RGB_MATRIX_ENABLE

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        //  Default layer state

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

        ypos += font_oled->line_height + 4;
        if (hue_redraw || dl_state_redraw || layer_state_redraw) {
            static int max_layer_xpos = 0;
            xpos                      = 5;
            snprintf(buf, sizeof(buf), "LAYOUT: %s", get_layer_name_string(layer_state, false, true));
            xpos +=
                qp_drawtext_recolor(ili9341_display, xpos, ypos, font_oled, buf, curr_hue, 255, 255, curr_hue, 255, 0);
            if (max_layer_xpos < xpos) {
                max_layer_xpos = xpos;
            }
            qp_rect(ili9341_display, xpos, ypos, max_layer_xpos, ypos + font_oled->line_height, 0, 0, 0, true);
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Layer State

        if (hue_redraw || layer_state_redraw) {
            static int max_layer_xpos = 0;
            xpos                      = 5 + (qp_textwidth(font_oled, "LAYOUT: COLEMAK_DH"));
            snprintf(buf, sizeof(buf), "LAYER: %s", get_layer_name_string(layer_state, false, false));
            xpos +=
                qp_drawtext_recolor(ili9341_display, xpos, ypos, font_oled, buf, curr_hue, 255, 255, curr_hue, 255, 0);
            if (max_layer_xpos < xpos) {
                max_layer_xpos = xpos;
            }
            qp_rect(ili9341_display, xpos, ypos, max_layer_xpos, ypos + font_oled->line_height, 0, 0, 0, true);
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Autocorrection values

#ifdef AUTOCORRECT_ENABLE
        ypos += font_oled->line_height + 4;
        extern bool autocorrect_str_has_changed;
        extern char autocorrected_str_raw[2][21];
        if (hue_redraw || autocorrect_str_has_changed) {
            static int max_klog_xpos = 0;
            xpos                     = 5;
            snprintf(buf, sizeof(buf), "Autocorrected: %s", autocorrected_str_raw[0]);
            xpos +=
                qp_drawtext_recolor(ili9341_display, xpos, ypos, font_oled, buf, curr_hue, 255, 255, curr_hue, 255, 0);

            if (max_klog_xpos < xpos) {
                max_klog_xpos = xpos;
            }
            qp_rect(ili9341_display, xpos, ypos, max_klog_xpos, ypos + font_oled->line_height, 0, 0, 0, true);

            ypos += font_oled->line_height + 4;
            max_klog_xpos = 0;
            xpos          = 5;
            snprintf(buf, sizeof(buf), "Original Text: %s", autocorrected_str_raw[1]);
            xpos +=
                qp_drawtext_recolor(ili9341_display, xpos, ypos, font_oled, buf, curr_hue, 255, 255, curr_hue, 255, 0);
            if (max_klog_xpos < xpos) {
                max_klog_xpos = xpos;
            }
            qp_rect(ili9341_display, xpos, ypos, max_klog_xpos, ypos + font_oled->line_height, 0, 0, 0, true);

            autocorrect_str_has_changed = false;
        } else {
            // we called ypos inside the function ... to make sure we don't skip a line on future passes ....
            ypos += font_oled->line_height + 4;
        }
#endif // AUTOCORRECT_ENABLE

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Console Log

        ypos += (font_oled->line_height + 4);
        static uint32_t last_log_redraw = 0;
        static uint16_t max_line_width  = 0;
        if (hue_redraw || (needs_redraw && timer_elapsed32(last_log_redraw) > 50)) {
            for (uint8_t i = 0; i < NUM_LOG_LINES; ++i) {
                xpos = 5;
                xpos += qp_drawtext_recolor(ili9341_display, xpos, ypos, font_oled, logline_ptrs[i], curr_hue, 255, 255,
                                            curr_hue, 255, 0);
                if (max_line_width < xpos) {
                    max_line_width = xpos;
                }
                qp_rect(ili9341_display, xpos, ypos, max_line_width, ypos + font_oled->line_height, 0, 0, 0, true);
                ypos += font_oled->line_height + 4;
            }
            last_log_redraw = timer_read32();
            needs_redraw    = false;
        }
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Font test

#if 0
        ypos += font_oled->line_height + 4;
        if (hue_redraw) {
            xpos                             = 5;
            static uint16_t max_font_xpos[4] = {0};
            render_character_set(ili9341_display, &xpos, max_font_xpos, &ypos, font_thintel, curr_hue, 255, 255,
                                 curr_hue, 255, 0);
            render_character_set(ili9341_display, &xpos, max_font_xpos, &ypos, font_mono, curr_hue, 255, 255, curr_hue,
                                 255, 0);
            render_character_set(ili9341_display, &xpos, max_font_xpos, &ypos, font_oled, curr_hue, 255, 255, curr_hue,
                                 255, 0);
        }
#endif

        // Keylogger
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifdef DISPLAY_KEYLOGGER_ENABLE // keep at very end
        static uint32_t last_klog_update = 0;
        if (timer_elapsed32(last_klog_update) > 125 || keylogger_has_changed) {
            last_klog_update = timer_read32();
        }

        ypos = height - (font_mono->line_height + 2);
        if (keylogger_has_changed) {
            static int max_klog_xpos = 0;
            xpos                     = 27;
            snprintf(buf, sizeof(buf), "Keylogger: %s", display_keylogger_string);

            xpos += qp_drawtext_recolor(ili9341_display, xpos, ypos, font_mono, buf, 0, 255, 0, 0, 0, 255);

            if (max_klog_xpos < xpos) {
                max_klog_xpos = xpos;
            }
            // qp_rect(ili9341_display, xpos, ypos, max_klog_xpos, ypos + font->line_height, 0, 0, 255, true);
            keylogger_has_changed = false;
        }
#endif // DISPLAY_KEYLOGGER_ENABLE

        // RTC
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifdef RTC_ENABLE
        ypos -= (font_oled->line_height + 4);
        static uint16_t rtc_timer  = 0;
        bool            rtc_redraw = false;
        if (timer_elapsed(rtc_timer) > 125 && rtc_is_connected()) {
            rtc_timer  = timer_read();
            rtc_redraw = true;
        }
        if (hue_redraw || rtc_redraw) {
            static uint16_t max_rtc_xpos = 0;
            xpos                         = 5;
            snprintf(buf, sizeof(buf), "RTC Date/Time: %s", rtc_read_date_time_str());

            uint8_t title_width = qp_textwidth(font_oled, buf);
            if (title_width > (width - 6)) {
                title_width = width - 6;
            }
            uint8_t title_xpos = (width - title_width) / 2;

            xpos += qp_drawtext_recolor(ili9341_display, title_xpos, ypos, font_oled, buf, curr_hue, 255, 255, curr_hue,
                                        255, 0);
            if (max_rtc_xpos < xpos) {
                max_rtc_xpos = xpos;
            }
            qp_rect(ili9341_display, xpos, ypos, max_rtc_xpos, ypos + font_oled->line_height, 0, 0, 0, true);
        }
#endif // RTC_ENABLE

        //  Layer Map render
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifdef LAYER_MAP_ENABLE
        ypos -= (font_oled->line_height + 4) * LAYER_MAP_ROWS;
        if (hue_redraw || layer_map_has_updated) {
            uint16_t temp_ypos = ypos;
            for (uint8_t y = 0; y < LAYER_MAP_ROWS; y++) {
                xpos = 25;
                for (uint8_t x = 0; x < LAYER_MAP_COLS; x++) {
                    uint16_t keycode = extract_basic_keycode(layer_map[y][x], NULL, false);
                    char     code    = 0;
                    if (keycode > 0xFF) {
                        keycode = KC_SPC;
                    }
                    if (keycode < ARRAY_SIZE(code_to_name)) {
                        code = pgm_read_byte(&code_to_name[keycode]);
                    }
                    snprintf(buf, sizeof(buf), "%c", code);
                    xpos += qp_drawtext_recolor(ili9341_display, xpos, temp_ypos, font_oled, buf, curr_hue, 255, 255, 0,
                                                0, peek_matrix_layer_map(y, x) ? 255 : 0) +
                            5;
                }
                temp_ypos += font_oled->line_height + 4;
            }
            layer_map_has_updated = false;
        }

#endif
    }
    qp_flush(ili9341_display);
}
