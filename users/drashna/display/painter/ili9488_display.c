// Copyright 2018-2024 Nick Brassel (@tzarc)
// Copyright 2025 Christopher Courtney, aka Drashna Jael're  (@drashna) <drashna@live.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "drashna_runtime.h"
#include "drashna_names.h"
#include "drashna_layers.h"
#include "qp_ili9xxx_opcodes.h"
#include "qp_comms.h"
#include "display/painter/painter.h"
#include "display/painter/ili9488_display.h"
#include "hardware_id_string.h"
#include "version.h"
#ifdef QUANTUM_PAINTER_DRIVERS_ILI9488_SURFACE
#    include "qp_surface.h"
#endif // QUANTUM_PAINTER_DRIVERS_ILI9488_SURFACE
#ifdef SPLIT_KEYBOARD
#    include "split_util.h"
#endif // SPLIT_KEYBOARD
#if defined(RGB_MATRIX_ENABLE)
#    include "rgb/rgb_matrix_stuff.h"
#endif // defined(RGB_MATRIX_ENABLE)
#if defined(RGBLIGHT_ENABLE)
#    include "rgb/rgb_stuff.h"
#endif // defined(RGBLIGHT_ENABLE)
#ifdef COMMUNITY_MODULE_RTC_ENABLE
#    include "rtc.h"
#endif // COMMUNITY_MODULE_RTC_ENABLE
#ifdef COMMUNITY_MODULE_LAYER_MAP_ENABLE
#    include "layer_map.h"
#endif // COMMUNITY_MODULE_LAYER_MAP_ENABLE
#ifdef COMMUNITY_MODULE_KEYBOARD_LOCK_ENABLE
#    include "keyboard_lock.h"
#endif // COMMUNITY_MODULE_KEYBOARD_LOCK_ENABLE
#ifdef COMMUNITY_MODULE_UNICODE_TYPING_ENABLE
#    include "unicode_typing.h"
#endif // COMMUNITY_MODULE_UNICODE_TYPING_ENABLE

static painter_device_t display;

extern painter_image_handle_t windows_logo, apple_logo, linux_logo;
extern painter_image_handle_t mouse_icon, gamepad_icon;
extern painter_image_handle_t akira_explosion;

#ifndef ILI9488_CS_PIN
#    define ILI9488_CS_PIN DISPLAY_CS_PIN
#endif // ILI9488_CS_PIN
#ifndef ILI9488_DC_PIN
#    define ILI9488_DC_PIN DISPLAY_DC_PIN
#endif // ILI9488_DC_PIN
#ifndef ILI9488_RST_PIN
#    ifndef DISPLAY_RST_PIN
#        define ILI9488_RST_PIN NO_PIN
#    else // DISPLAY_RST_PIN
#        define ILI9488_RST_PIN DISPLAY_RST_PIN
#    endif // DISPLAY_RST_PIN
#endif     // ILI9488_RST_PIN
#ifndef ILI9488_SPI_DIVIDER
#    ifndef DISPLAY_SPI_DIVIDER
#        define ILI9488_SPI_DIVIDER 1
#    else // DISPLAY_SPI_DIVIDER
#        define ILI9488_SPI_DIVIDER DISPLAY_SPI_DIVIDER
#    endif // DISPLAY_SPI_DIVIDER
#endif     // ILI9488_SPI_DIVIDER
#ifndef ILI9488_SPI_MODE
#    ifndef DISPLAY_SPI_MODE
#        define ILI9488_SPI_MODE 0
#    else // DISPLAY_SPI_MODE
#        define ILI9488_SPI_MODE DISPLAY_SPI_MODE
#    endif // DISPLAY_SPI_MODE
#endif     // ILI9488_SPI_MODE

#ifdef QUANTUM_PAINTER_DRIVERS_ILI9488_SURFACE
#    if SURFACE_NUM_DEVICES < 2
#        error Not enough surfaces for the ILI9488 display
#    endif
#    pragma message("ILI9488 Surface enabled")
#    define SURFACE_MENU_WIDTH  316
#    define SURFACE_MENU_HEIGHT 146
#    if HAL_USE_SDRAM == TRUE
__attribute__((section(".ram7")))
#    endif
static uint8_t   menu_buffer[SURFACE_REQUIRED_BUFFER_BYTE_SIZE(SURFACE_MENU_WIDTH, SURFACE_MENU_HEIGHT, 24)];
painter_device_t menu_surface;

#    if defined(WPM_ENABLE) && !defined(WPM_NO_SURFACE)
static uint8_t
    wpm_graph_buffer[SURFACE_REQUIRED_BUFFER_BYTE_SIZE(WPM_PAINTER_GRAPH_WIDTH, WPM_PAINTER_GRAPH_HEIGHT, 24)] = {0};
static painter_device_t wpm_graph_surface;
#    endif // WPM_ENABLE && !WPM_NO_SURFACE

#endif

static bool has_run = false, forced_reinit = false;

void init_display_ili9488_inversion(void) {
    qp_comms_start(display);
    qp_comms_command(display,
                     userspace_config.display.painter.left.inverted ? ILI9XXX_CMD_INVERT_OFF : ILI9XXX_CMD_INVERT_ON);
    qp_comms_stop(display);
    if (has_run) {
        forced_reinit = true;
    }
}

void init_display_ili9488_rotation(void) {
    uint16_t width;
    uint16_t height;

    qp_init(display, userspace_config.display.painter.left.rotation ? QP_ROTATION_180 : QP_ROTATION_0);
    qp_get_geometry(display, &width, &height, NULL, NULL, NULL);
    qp_clear(display);
    qp_rect(display, 0, 0, width - 1, height - 1, 0, 0, 0, true);

    // if needs inversion, run it only after the clear and rect functions or otherwise it won't work
    init_display_ili9488_inversion();
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Initial render of frame/logo

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
void init_display_ili9488(void) {
    display = qp_ili9488_make_spi_device(320, 480, ILI9488_CS_PIN, ILI9488_DC_PIN, ILI9488_RST_PIN, ILI9488_SPI_DIVIDER,
                                         ILI9488_SPI_MODE);

#ifdef QUANTUM_PAINTER_DRIVERS_ILI9488_SURFACE
    menu_surface = qp_make_rgb888_surface(SURFACE_MENU_WIDTH, SURFACE_MENU_HEIGHT, menu_buffer);
#    if defined(WPM_ENABLE) && !defined(WPM_NO_SURFACE)
    wpm_graph_surface = qp_make_rgb888_surface(WPM_PAINTER_GRAPH_WIDTH, WPM_PAINTER_GRAPH_HEIGHT, wpm_graph_buffer);
#    endif // WPM_ENABLE && !WPM_NO_SURFACE
#endif     // QUANTUM_PAINTER_DRIVERS_ILI9488_SURFACE

    wait_ms(50);

#ifdef QUANTUM_PAINTER_DRIVERS_ILI9488_SURFACE
    qp_init(menu_surface, QP_ROTATION_0);
#    if defined(WPM_ENABLE) && !defined(WPM_NO_SURFACE)
    qp_init(wpm_graph_surface, QP_ROTATION_0);
#    endif // WPM_ENABLE && !WPM_NO_SURFACE
#endif     // QUANTUM_PAINTER_DRIVERS_ILI9488_SURFACE

    init_display_ili9488_rotation();
}

void ili9488_display_power(bool on) {
    qp_power(display, on);
}

__attribute__((weak)) void ili9488_draw_user(void) {
    bool hue_redraw = forced_reinit;

    static dual_hsv_t curr_hsv = {0};
    if (memcmp(&curr_hsv, &userspace_config.display.painter.hsv, sizeof(dual_hsv_t)) != 0) {
        curr_hsv   = painter_get_dual_hsv();
        hue_redraw = true;
    }
    const uint8_t disabled_val = curr_hsv.primary.v / 2;
    (void)disabled_val;

    uint16_t width;
    uint16_t height;
    qp_get_geometry(display, &width, &height, NULL, NULL, NULL);

    if (hue_redraw) {
        qp_rect(display, 0, 0, width - 1, height - 1, 0, 0, 0, true);
        painter_render_frame_box(display, curr_hsv.primary, 1, 0, 0, 3, true, false);
        char title[50] = {0};
        snprintf(title, sizeof(title), "%s", PRODUCT);
        uint16_t title_width = qp_textwidth(font_oled, title);
        if (title_width > (width - 55)) {
            title_width = width;
        }
        uint16_t title_xpos = (width - title_width) / 2;
        qp_drawtext_recolor(display, title_xpos, 5, font_oled,
                            truncate_text(title, title_width, font_oled, false, false), 0, 0, 0, curr_hsv.primary.h,
                            curr_hsv.primary.s, curr_hsv.primary.v);

        // horizontal line below scan rate
        qp_line(display, 2, 30, 80, 30, curr_hsv.primary.h, curr_hsv.primary.s, curr_hsv.primary.v);

        // horizontal line below rgb
        qp_line(display, 80, 54, 237, 54, curr_hsv.primary.h, curr_hsv.primary.s, curr_hsv.primary.v);

        // caps lock horizontal line
        qp_line(display, 208, 16, 208, 54, curr_hsv.primary.h, curr_hsv.primary.s, curr_hsv.primary.v);

        // vertical lines next to scan rate + wpm + pointing
        qp_line(display, 80, 16, 80, 106, curr_hsv.primary.h, curr_hsv.primary.s, curr_hsv.primary.v);

        // lines for unicode typing mode and mode
        qp_line(display, 80, 80, 237, 80, curr_hsv.primary.h, curr_hsv.primary.s, curr_hsv.primary.v);

        // qp_line(display, xpos + 180, 80, xpos + 180, 106, curr_hsv.primary.h, curr_hsv.primary.s,
        // curr_hsv.primary.v);

        // lines for mods and OS detection
        qp_line(display, 2, 107, 237, 107, curr_hsv.primary.h, curr_hsv.primary.s, curr_hsv.primary.v);
        qp_line(display, 155, 107, 155, 122, curr_hsv.primary.h, curr_hsv.primary.s, curr_hsv.primary.v);
        // lines for autocorrect and layers
        qp_line(display, 2, 122, 237, 122, curr_hsv.primary.h, curr_hsv.primary.s, curr_hsv.primary.v);
        qp_line(display, 121, 122, 121, 171, curr_hsv.primary.h, curr_hsv.primary.s, curr_hsv.primary.v);
        qp_line(display, 186, 122, 186, 171, curr_hsv.primary.h, curr_hsv.primary.s, curr_hsv.primary.v);

        // line above menu block
        qp_line(display, 2, 304, 317, 304, curr_hsv.primary.h, curr_hsv.primary.s, curr_hsv.primary.v);
        // line above rtc
        qp_line(display, 2, 452, 317, 452, curr_hsv.primary.h, curr_hsv.primary.s, curr_hsv.primary.v);
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
        qp_rect(display, width - mouse_icon->width - 6, 6, width - 6, 6 + mouse_icon->height - 1, 0, 0, 0, true);
        qp_drawimage_recolor(
            display, width - mouse_icon->width - 6, 6, mouse_icon,
            is_keyboard_master() ? curr_hsv.secondary.h : curr_hsv.primary.h,
            is_keyboard_master() ? curr_hsv.secondary.s : curr_hsv.primary.s,
#ifdef SPLIT_KEYBOARD
            is_transport_connected() ? is_keyboard_master() ? curr_hsv.secondary.v : curr_hsv.primary.v : disabled_val,
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
    if (painter_render_side()) {
        ypos = 20;
        xpos = 83;

#if defined(RGB_MATRIX_ENABLE)
        painter_render_rgb(display, font_oled, xpos, ypos, hue_redraw, &curr_hsv,
                           "RGB Matrix Config:", rgb_matrix_get_effect_name, rgb_matrix_get_hsv,
                           rgb_matrix_is_enabled(), RGB_MATRIX_MAXIMUM_BRIGHTNESS);
#endif // RGB_MATRIX_ENABLE

        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // LED Lock indicator(text)

        ypos = 24;
        xpos = 292;
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
#ifdef CAPS_WORD_ENABLE
        static bool caps_words = false;
        if (caps_words != is_caps_word_on()) {
            caps_words           = is_caps_word_on();
            keymap_config_redraw = true;
        }
#endif // CAPS_WORD_ENABLE
        if (hue_redraw || keymap_config_redraw) {
            xpos = 80 + 4;
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
                                    last_keymap_config.oneshot_enable ? curr_hsv.primary.v : disabled_val, 0, 0, 0) +
                5;
#ifdef CAPS_WORD_ENABLE
            xpos += qp_drawtext_recolor(display, xpos, ypos, font_oled, "CAPS",
                                        is_caps_word_on() ? curr_hsv.secondary.h : curr_hsv.primary.h,
                                        is_caps_word_on() ? curr_hsv.secondary.s : curr_hsv.primary.s,
                                        is_caps_word_on() ? curr_hsv.primary.v : disabled_val, 0, 0, 0);
#endif
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Device Config (Audio, Audio Clicky, Host Driver lock, Swap Hands)

        ypos += font_oled->line_height + 4;
        static userspace_runtime_state_t last_user_state = {0};
        if (hue_redraw || memcmp(&userspace_runtime_state, &last_user_state, sizeof(userspace_runtime_state))) {
            memcpy(&last_user_state, &userspace_runtime_state, sizeof(userspace_runtime_state));
            xpos = 80 + 4;
            xpos += qp_drawtext_recolor(display, xpos, ypos, font_oled, "AUDIO",
                                        last_user_state.audio.enable ? curr_hsv.secondary.h : curr_hsv.primary.h,
                                        last_user_state.audio.enable ? curr_hsv.secondary.s : curr_hsv.primary.s,
                                        last_user_state.audio.enable ? curr_hsv.primary.v : disabled_val, 0, 0, 0) +
                    5;
            xpos +=
                qp_drawtext_recolor(display, xpos, ypos, font_oled, "CLCK",
                                    last_user_state.audio.clicky_enable ? curr_hsv.secondary.h : curr_hsv.primary.h,
                                    last_user_state.audio.clicky_enable ? curr_hsv.secondary.s : curr_hsv.primary.s,
                                    last_user_state.audio.clicky_enable ? curr_hsv.primary.v : disabled_val, 0, 0, 0) +
                5;
            xpos += qp_drawtext_recolor(
                        display, xpos, ypos, font_oled, "HOST",
                        !last_user_state.internals.host_driver_disabled ? curr_hsv.secondary.h : curr_hsv.primary.h,
                        !last_user_state.internals.host_driver_disabled ? curr_hsv.secondary.s : curr_hsv.primary.s,
                        !last_user_state.internals.host_driver_disabled ? curr_hsv.primary.v : disabled_val, 0, 0, 0) +
                    5;
            xpos +=
                qp_drawtext_recolor(display, xpos, ypos, font_oled, "SWAP",
                                    last_user_state.internals.swap_hands ? curr_hsv.secondary.h : curr_hsv.primary.h,
                                    last_user_state.internals.swap_hands ? curr_hsv.secondary.s : curr_hsv.primary.s,
                                    last_user_state.internals.swap_hands ? curr_hsv.primary.v : disabled_val, 0, 0, 0);
        }

#ifdef DISPLAY_KEYLOGGER_ENABLE
        painter_render_keylogger(display, font_oled, 84, 84, 150, hue_redraw, &curr_hsv);
#endif // DISPLAY_KEYLOGGER_ENABLE

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Pointing Device CPI

        ypos = 30 + 4;
#if defined(POINTING_DEVICE_ENABLE)
#    if (defined(KEYBOARD_bastardkb_charybdis) || defined(KEYBOARD_handwired_tractyl_manuform))
#        include QMK_KEYBOARD_H

        static uint16_t last_cpi = 0xFFFF;
        if (hue_redraw || last_cpi != charybdis_get_pointer_default_dpi()) {
            last_cpi = charybdis_get_pointer_default_dpi();
            xpos     = 5;
            xpos += qp_drawtext_recolor(display, xpos, ypos, font_oled, "CPI:   ", curr_hsv.primary.h,
                                        curr_hsv.primary.s, curr_hsv.primary.v, 0, 0, 0);
            snprintf(buf, sizeof(buf), "%5u", last_cpi);
            xpos += qp_drawtext_recolor(display, xpos, ypos, font_oled, buf, curr_hsv.secondary.h, curr_hsv.secondary.s,
                                        curr_hsv.secondary.v, 0, 0, 0);
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
            qp_drawtext_recolor(display, xpos, ypos, font_oled, "Auto Layer:", curr_hsv.primary.h, curr_hsv.primary.s,
                                curr_hsv.primary.v, 0, 0, 0);
        }
        ypos += font_oled->line_height + 4;

        static uint8_t last_am_layer = 0xFF;
        if (hue_redraw || last_am_layer != get_auto_mouse_layer() || auto_mouse_redraw) {
            last_am_layer = get_auto_mouse_layer();
            xpos          = 5;
            snprintf(buf, sizeof(buf), "%12s", get_layer_name_string(get_auto_mouse_layer(), false, true));
            qp_drawtext_recolor(display, xpos, ypos, font_oled, truncate_text(buf, 80 - 5 - 2, font_oled, false, false),
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
#    endif // (defined(KEYBOARD_bastardkb_charybdis) || defined(KEYBOARD_handwired_tractyl_manuform))

#    ifdef COMMUNITY_MODULE_POINTING_DEVICE_ACCEL_ENABLE
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Pointing Device Sniping mode
        bool pointing_device_accel_get_enabled(void);

        static bool last_accel_state = false;

        if (hue_redraw || last_accel_state != pointing_device_accel_get_enabled()) {
            last_accel_state = pointing_device_accel_get_enabled();
            xpos             = 5;
            xpos += qp_drawtext_recolor(display, xpos, ypos, font_oled, "Acceleration",
                                        last_accel_state ? curr_hsv.secondary.h : curr_hsv.primary.h,
                                        last_accel_state ? curr_hsv.secondary.s : curr_hsv.primary.s,
                                        last_accel_state ? curr_hsv.primary.v : disabled_val, 0, 0, 0);
        }
        ypos += font_oled->line_height + 4;

#    elif defined(KEYBOARD_bastardkb_charybdis) || defined(KEYBOARD_handwired_tractyl_manuform)
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
        ypos += font_oled->line_height + 4;
#    endif // COMMUNITY_MODULE_POINTING_DEVICE_ACCEL_ENABLE

        static bool last_jiggle_enabled = false;
        if (hue_redraw || last_jiggle_enabled != userspace_config.pointing.mouse_jiggler.enable) {
            last_jiggle_enabled = userspace_config.pointing.mouse_jiggler.enable;
            xpos                = 5;
            xpos += qp_drawtext_recolor(display, xpos, ypos, font_oled, "Jiggler",
                                        last_jiggle_enabled ? curr_hsv.secondary.h : curr_hsv.primary.h,
                                        last_jiggle_enabled ? curr_hsv.secondary.s : curr_hsv.primary.s,
                                        last_jiggle_enabled ? curr_hsv.primary.v : disabled_val, 0, 0, 0);
        }
#endif // POINTING_DEVICE_ENABLE

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Mods

        ypos = 107 + 3;
        xpos = 5;

        painter_render_modifiers(display, font_oled, xpos, ypos, width, hue_redraw || keymap_config_redraw, &curr_hsv,
                                 disabled_val);

#ifdef OS_DETECTION_ENABLE
        ypos = 107 + 4;
        xpos = 159;
        painter_render_os_detection(display, font_oled, xpos, ypos, width, hue_redraw, &curr_hsv);
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
            snprintf(buf, sizeof(buf), "%10s",
                     get_layer_name_string(get_highest_layer(default_layer_state), false, true));
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
            snprintf(buf, sizeof(buf), "%10s", get_layer_name_string(get_highest_layer(temp), false, false));
            qp_drawtext_recolor(display, xpos, ypos, font_oled, buf, curr_hsv.secondary.h, curr_hsv.secondary.s,
                                curr_hsv.secondary.v, 0, 0, 0);
            ypos = 122 + 4;
            xpos = 190;
            qp_drawimage_recolor(display, xpos, ypos, gamepad_icon, curr_hsv.primary.h, curr_hsv.primary.s,
                                 layer_state_cmp(last_layer_state, _GAMEPAD) ? curr_hsv.primary.v : disabled_val, 0, 0,
                                 0);
            qp_drawimage_recolor(display, xpos + gamepad_icon->width + 6, ypos + 4, mouse_icon, curr_hsv.primary.h,
                                 curr_hsv.primary.s,
                                 layer_state_cmp(layer_state, _MOUSE) ? curr_hsv.primary.v : disabled_val, 0, 0, 0);
            ypos += gamepad_icon->height + 2;
            qp_drawtext_recolor(display, xpos, ypos, font_oled, "Diablo",
                                layer_state_cmp(last_layer_state, _DIABLO) ? 0 : curr_hsv.primary.h, curr_hsv.primary.s,
                                layer_state_cmp(last_layer_state, _DIABLO) ? curr_hsv.primary.v : disabled_val, 0, 0,
                                0);
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

        painter_render_autocorrect(display, font_oled, 5, ypos, width, hue_redraw, &curr_hsv);
        ypos += (font_oled->line_height + 4) * 3;

#endif // AUTOCORRECT_ENABLE

        ypos += font_oled->line_height + 1;
        ypos = 80;
        xpos = 238;
#ifdef WPM_ENABLE
        painter_render_wpm(display, font_oled, xpos, ypos, hue_redraw, &curr_hsv);
        ypos += font_oled->line_height + 2 * 4;
#    if defined(QUANTUM_PAINTER_DRIVERS_ILI9488_SURFACE) && !defined(WPM_NO_SURFACE)
        painter_render_wpm_graph(wpm_graph_surface, font_oled, 0, 0, hue_redraw, &curr_hsv);
        qp_surface_draw(wpm_graph_surface, display, xpos, ypos, false);
#    else
        painter_render_wpm_graph(display, font_oled, xpos, ypos, hue_redraw, &curr_hsv);
#    endif
#endif
#ifdef QUANTUM_PAINTER_DRIVERS_ILI9488_SURFACE
        painter_render_menu_block(menu_surface, font_oled, 0, 0, SURFACE_MENU_WIDTH - 1, SURFACE_MENU_HEIGHT,
                                  hue_redraw, &curr_hsv, is_keyboard_left(), true);
        qp_surface_draw(menu_surface, display, 2, 305, false);
#else  // QUANTUM_PAINTER_DRIVERS_ILI9488_SURFACE
        painter_render_menu_block(display, font_oled, 2, 305, 317, 451, hue_redraw, &curr_hsv, is_keyboard_master(),
                                  true);
#endif // QUANTUM_PAINTER_DRIVERS_ILI9341_SURFACE

        // Footer
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        ypos = height - (font_mono->line_height + 2);
        xpos = 27;
        if (hue_redraw) {
            snprintf(buf, sizeof(buf), "SN: %s BD: %10.10s", get_hardware_id_string(), QMK_BUILDDATE);
            xpos += qp_drawtext_recolor(display, xpos, ypos, font_mono, buf, 0, 0, 0, curr_hsv.primary.h,
                                        curr_hsv.primary.s, curr_hsv.primary.v);
        }

        // RTC
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        xpos = 5;
        ypos = height - (16 + font_oled->line_height);
#ifdef COMMUNITY_MODULE_RTC_ENABLE
        static uint16_t last_rtc_time = 0xFFFF;
        painter_render_rtc_time(display, font_oled, xpos, ypos, width, hue_redraw, &last_rtc_time, &curr_hsv.primary);
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
#endif

        forced_reinit = false;
    }

    qp_flush(display);
}

void ili9488_display_shutdown(bool jump_to_bootloader) {
    ili9488_display_power(true);
    painter_render_shutdown(display, jump_to_bootloader);
}
