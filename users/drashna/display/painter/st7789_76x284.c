// Copyright 2025 Christopher Courtney, aka Drashna Jael're  (@drashna) <drashna@live.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "drashna_runtime.h"
#include "qp.h"
#include "qp_comms.h"
#include <qp_st77xx_opcodes.h>
#include "display/painter/painter.h"
#include "display/painter/st7789_76x284.h"
#ifdef COMMUNITY_MODULE_DISPLAY_MENU_ENABLE
#    include "qp_render_menu.h"
#endif
#ifdef QUANTUM_PAINTER_DRIVERS_ST7789_76X284_SURFACE
#    include "qp_surface.h"
#endif // QUANTUM_PAINTER_DRIVERS_ST7789_76X284_SURFACE

#ifndef ST7789_THIN_CS_PIN
#    define ST7789_THIN_CS_PIN DISPLAY_CS_PIN
#endif // ST7789_THIN_CS_PIN
#ifndef ST7789_THIN_DC_PIN
#    define ST7789_THIN_DC_PIN DISPLAY_DC_PIN
#endif // ST7789_THIN_DC_PIN
#ifndef ST7789_THIN_RST_PIN
#    ifndef DISPLAY_RST_PIN
#        define ST7789_THIN_RST_PIN NO_PIN
#    else // DISPLAY_RST_PIN
#        define ST7789_THIN_RST_PIN DISPLAY_RST_PIN
#    endif // DISPLAY_RST_PIN
#endif     // ST7789_THIN_RST_PIN
#ifndef ST7789_THIN_SPI_DIVIDER
#    ifndef DISPLAY_SPI_DIVIDER
#        define ST7789_THIN_SPI_DIVIDER 1
#    else // DISPLAY_SPI_DIVIDER
#        define ST7789_THIN_SPI_DIVIDER DISPLAY_SPI_DIVIDER
#    endif // DISPLAY_SPI_DIVIDER
#endif     // ST7789_THIN_SPI_DIVIDER
#ifndef ST7789_THIN_SPI_MODE
#    ifndef DISPLAY_SPI_MODE
#        define ST7789_THIN_SPI_MODE 3
#    else // DISPLAY_SPI_MODE
#        define ST7789_THIN_SPI_MODE DISPLAY_SPI_MODE
#    endif // DISPLAY_SPI_MODE
#endif     // ST7789_THIN_SPI_MODE

painter_device_t st7789_76x284_display;
#ifdef QUANTUM_PAINTER_DRIVERS_ST7789_76X284_SURFACE
static painter_device_t st7789_76x284_surface_display;
#    if HAL_USE_SDRAM == TRUE
__attribute__((section(".ram7")))
#    endif
static uint8_t display_buffer[SURFACE_REQUIRED_BUFFER_BYTE_SIZE(76, 284, 16)];
#else
#    define st7789_76x284_surface_display st7789_76x284_display
#endif // QUANTUM_PAINTER_DRIVERS_ST7789_76X284_SURFACE

static bool has_run = false, forced_reinit = false;

void init_display_st7789_76x284_inversion(void) {
    qp_comms_start(st7789_76x284_display);
    qp_comms_command(st7789_76x284_display, (is_keyboard_left() ? userspace_config.display.painter.left.inverted
                                                                : userspace_config.display.painter.right.inverted)
                                                ? ST77XX_CMD_INVERT_ON
                                                : ST77XX_CMD_INVERT_OFF);
    qp_comms_stop(st7789_76x284_display);
    if (has_run) {
        forced_reinit = true;
    }
}

void init_display_st7789_76x284_rotation(void) {
    painter_rotation_t rotation = (is_keyboard_left() ? userspace_config.display.painter.left.rotation
                                                      : userspace_config.display.painter.right.rotation)
                                      ? QP_ROTATION_0
                                      : QP_ROTATION_180;

    qp_init(st7789_76x284_display, rotation);

    // Configure the rotation (i.e. the ordering and direction of memory writes in GRAM)
    const uint8_t madctl[] = {
        [QP_ROTATION_0]   = ST77XX_MADCTL_BGR,
        [QP_ROTATION_90]  = ST77XX_MADCTL_BGR | ST77XX_MADCTL_MX | ST77XX_MADCTL_MV,
        [QP_ROTATION_180] = ST77XX_MADCTL_BGR | ST77XX_MADCTL_MX | ST77XX_MADCTL_MY,
        [QP_ROTATION_270] = ST77XX_MADCTL_BGR | ST77XX_MADCTL_MV | ST77XX_MADCTL_MY,
    };
    qp_comms_start(st7789_76x284_display);
    qp_comms_command_databyte(st7789_76x284_display, ST77XX_SET_MADCTL, madctl[rotation]);
    qp_comms_stop(st7789_76x284_display);

    qp_set_viewport_offsets(st7789_76x284_display, 82, 18); // for qp_rotation_0

    qp_clear(st7789_76x284_display);
    qp_rect(st7789_76x284_surface_display, 0, 0, 76 - 1, 284 - 1, HSV_BLACK, true);

    init_display_st7789_76x284_inversion();

    painter_render_frame_box(st7789_76x284_surface_display, painter_get_hsv(false), 1, 1, 0, 2, true, true);
    painter_render_frame_box(st7789_76x284_surface_display, painter_get_hsv(true), 0, 0, 0, 2, true, true);

    qp_power(st7789_76x284_display, true);
    qp_flush(st7789_76x284_display);
    if (has_run) {
        forced_reinit = true;
    }
    has_run = true;
}

void init_display_st7789_76x284(void) {
    st7789_76x284_display =
        qp_st7789_make_spi_device(76, 284, ST7789_THIN_CS_PIN, ST7789_THIN_DC_PIN, ST7789_THIN_RST_PIN,
                                  ST7789_THIN_SPI_DIVIDER, ST7789_THIN_SPI_MODE);

#ifdef QUANTUM_PAINTER_DRIVERS_ST7789_76X284_SURFACE
    st7789_76x284_surface_display = qp_make_rgb565_surface(76, 284, display_buffer);
    qp_init(st7789_76x284_surface_display, QP_ROTATION_0);
#endif

    init_display_st7789_76x284_rotation();

    st7789_76x284_draw_user();
}

void st7789_76x284_display_power(bool on) {
    qp_power(st7789_76x284_display, on);
}

void st7789_76x284_display_shutdown(bool jump_to_bootloader) {}

__attribute__((weak)) void st7789_76x284_draw_user(void) {
    // static uint16_t last_activity = UINT16_MAX - 9999;
    // qp_rect(st7789_76x284_display, 0, 0, 240, 320 - 1, HSV_MAGENTA, true);

#ifdef QUANTUM_PAINTER_DRIVERS_ST7789_76X284_SURFACE
    qp_surface_draw(st7789_76x284_surface_display, st7789_76x284_display, 0, 0, false);
#endif // QUANTUM_PAINTER_DRIVERS_ST7789_76X284_SURFACE
    qp_flush(st7789_76x284_display);
}
