// Copyright 2018-2024 Nick Brassel (@tzarc)
// Copyright 2025 Christopher Courtney, aka Drashna Jael're  (@drashna) <drashna@live.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "drashna_runtime.h"
#include "drashna_names.h"
#include "drashna_layers.h"
#include "drashna_util.h"
#include "version.h"
#include "qp_ili9xxx_opcodes.h"
#include "qp_comms.h"
#include "display/painter/painter.h"
#include "display/painter/ili9488_display.h"
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
#    define SURFACE_MENU_WIDTH  236
#    define SURFACE_MENU_HEIGHT 120
static uint8_t   menu_buffer[2][SURFACE_REQUIRED_BUFFER_BYTE_SIZE(SURFACE_MENU_WIDTH, SURFACE_MENU_HEIGHT, 16)];
painter_device_t menu_surface[2];
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

    qp_init(display, userspace_config.display.painter.left.rotation ? QP_ROTATION_0 : QP_ROTATION_180);
    qp_get_geometry(display, &width, &height, NULL, NULL, NULL);
    qp_clear(display);
    qp_rect(display, 0, 0, width - 1, height - 1, 0, 0, 0, true);

    // if needs inversion, run it only after the clear and rect functions or otherwise it won't work
    init_display_ili9488_inversion();
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Initial render of frame/logo

    painter_render_frame(display, font_thintel, true, 0, true);
    painter_render_frame(display, font_thintel, false, 240, false);
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
#endif // QUANTUM_PAINTER_DRIVERS_ILI9488_SURFACE

    wait_ms(50);

#ifdef QUANTUM_PAINTER_DRIVERS_ILI9488_SURFACE
    qp_init(menu_surface, QP_ROTATION_0);
#endif // QUANTUM_PAINTER_DRIVERS_ILI9488_SURFACE

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
        qp_drawimage_recolor(
            display, width - mouse_icon->width - 6, 5, mouse_icon,
            is_keyboard_master() ? curr_hsv.secondary.h : curr_hsv.primary.h,
            is_keyboard_master() ? curr_hsv.secondary.s : curr_hsv.primary.s,
#ifdef SPLIT_KEYBOARD
            is_transport_connected() ? is_keyboard_master() ? curr_hsv.secondary.v : curr_hsv.primary.v : disabled_val,
#else
            curr_hsv.secondary.v,
#endif
            0, 0, 0);
    }

    qp_flush(display);
}

void ili9488_display_shutdown(bool jump_to_bootloader) {
    ili9488_display_power(true);
    painter_render_shutdown(display, jump_to_bootloader);
}
