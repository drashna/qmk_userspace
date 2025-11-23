// Copyright 2025 Christopher Courtney, aka Drashna Jael're  (@drashna) <drashna@live.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "drashna_runtime.h"

#include "qp.h"
#include "qp_comms.h"
#include "qp_st77xx_opcodes.h"
#include "display/painter/painter.h"
#include "display/painter/st7789_170x320.h"
#ifdef QUANTUM_PAINTER_DRIVERS_ST7789_170X320_SURFACE
#    include "qp_surface.h"
#endif // QUANTUM_PAINTER_DRIVERS_ST7789_170X320_SURFACE
#ifdef COMMUNITY_MODULE_DISPLAY_MENU_ENABLE
#    include "qp_render_menu.h"
#endif // COMMUNITY_MODULE_DISPLAY_MENU_ENABLE

#ifndef ST7789_CS_PIN
#    define ST7789_CS_PIN DISPLAY_CS_PIN
#endif // ST7789_CS_PIN
#ifndef ST7789_DC_PIN
#    define ST7789_DC_PIN DISPLAY_DC_PIN
#endif // ST7789_DC_PIN
#ifndef ST7789_RST_PIN
#    ifndef DISPLAY_RST_PIN
#        define ST7789_RST_PIN NO_PIN
#    else // DISPLAY_RST_PIN
#        define ST7789_RST_PIN DISPLAY_RST_PIN
#    endif // DISPLAY_RST_PIN
#endif     // ST7789_RST_PIN
#ifndef ST7789_SPI_DIVIDER
#    ifndef DISPLAY_SPI_DIVIDER
#        define ST7789_SPI_DIVIDER 1
#    else // DISPLAY_SPI_DIVIDER
#        define ST7789_SPI_DIVIDER DISPLAY_SPI_DIVIDER
#    endif // DISPLAY_SPI_DIVIDER
#endif     // ST7789_SPI_DIVIDER
#ifndef ST7789_SPI_MODE
#    ifndef DISPLAY_SPI_MODE
#        define ST7789_SPI_MODE 3
#    else // DISPLAY_SPI_MODE
#        define ST7789_SPI_MODE DISPLAY_SPI_MODE
#    endif // DISPLAY_SPI_MODE
#endif     // ST7789_SPI_MODE

static painter_device_t st7789_170x320_display;
#ifdef QUANTUM_PAINTER_DRIVERS_ST7789_170X320_SURFACE
static painter_device_t st7789_170x320_surface_display;
static uint8_t          display_buffer[SURFACE_REQUIRED_BUFFER_BYTE_SIZE(170, 320, 16)];
#else
#    define st7789_170x320_surface_display st7789_170x320_display
#endif // QUANTUM_PAINTER_DRIVERS_ST7789_170X320_SURFACE

static bool has_run = false, forced_reinit = false;

void init_display_st7789_170x320_inversion(void) {
    qp_comms_start(st7789_170x320_display);
    qp_comms_command(st7789_170x320_display,
                     userspace_config.display.painter.left.inverted ? ST77XX_CMD_INVERT_OFF : ST77XX_CMD_INVERT_ON);
    qp_comms_stop(st7789_170x320_display);
    if (has_run) {
        forced_reinit = true;
    }
}

void init_display_st7789_170x320_rotation(void) {
    uint16_t width;
    uint16_t height;

    qp_init(st7789_170x320_display, userspace_config.display.painter.left.rotation ? QP_ROTATION_0 : QP_ROTATION_180);
    qp_set_viewport_offsets(st7789_170x320_display, 35, 0);
    qp_get_geometry(st7789_170x320_display, &width, &height, NULL, NULL, NULL);
    qp_clear(st7789_170x320_display);
    qp_rect(st7789_170x320_display, 0, 0, width - 1, height - 1, 0, 0, 0, true);
    // if needs inversion, run it only after the clear and rect functions or otherwise it won't work
    init_display_st7789_170x320_inversion();

    qp_power(st7789_170x320_display, true);
    qp_flush(st7789_170x320_display);
    if (has_run) {
        forced_reinit = true;
    }
    has_run = true;
}

void init_display_st7789_170x320(void) {
    st7789_170x320_display = qp_st7789_make_spi_device(170, 320, ST7789_CS_PIN, ST7789_DC_PIN, ST7789_RST_PIN,
                                                       ST7789_SPI_DIVIDER, ST7789_SPI_MODE);

#ifdef QUANTUM_PAINTER_DRIVERS_ST7789_170X320_SURFACE
    st7789_170x320_surface_display = qp_make_rgb565_surface(170, 320, display_buffer);
#endif

#ifdef QUANTUM_PAINTER_DRIVERS_ST7789_170X320_SURFACE
    qp_init(st7789_170x320_surface_display, QP_ROTATION_0);
#endif

    init_display_st7789_170x320_rotation();
}

void st7789_170x320_display_power(bool on) {
    qp_power(st7789_170x320_display, on);
}

__attribute__((weak)) void st7789_170x320_draw_user(void) {
    uint16_t width;
    uint16_t height;

    qp_get_geometry(st7789_170x320_display, &width, &height, NULL, NULL, NULL);
    // painter_render_frame(st7789_170x320_surface_display, font_thintel, painter_render_side(), 0, true);
    painter_render_frame_box(st7789_170x320_display, (hsv_t){.h = 128, .s = 255, .v = 255}, 0, 0, 0, 0, true, false);

#ifdef QUANTUM_PAINTER_DRIVERS_ST7789_170X320_SURFACE
    /// qp_surface_draw(st7789_170x320_surface_display, st7789_170x320_display, 0, 0, false);
#endif // QUANTUM_PAINTER_DRIVERS_ST7789_170X320_SURFACE

    qp_flush(st7789_170x320_display);
}

void st7789_170x320_display_shutdown(bool jump_to_bootloader) {
    st7789_170x320_display_power(true);
    painter_render_shutdown(st7789_170x320_display, jump_to_bootloader);
}
