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

static painter_device_t st7789_display;
painter_image_handle_t  screen_saver;

static bool has_run = false, forced_reinit = false;

void init_display_st7789_170x320_inversion(void) {
    qp_comms_start(st7789_display);
    qp_comms_command(st7789_display,
                     userspace_config.display.painter.left.inverted ? ST77XX_CMD_INVERT_OFF : ST77XX_CMD_INVERT_ON);
    qp_comms_stop(st7789_display);
    if (has_run) {
        forced_reinit = true;
    }
}

void init_display_st7789_170x320_rotation(void) {
    uint16_t width;
    uint16_t height;

    qp_init(st7789_display, userspace_config.display.painter.left.rotation ? QP_ROTATION_0 : QP_ROTATION_180);
    // qp_set_viewport_offsets(st7789_display, 35, 0);
    qp_get_geometry(st7789_display, &width, &height, NULL, NULL, NULL);
    qp_clear(st7789_display);
    qp_rect(st7789_display, 0, 0, width - 1, height - 1, 0, 0, 0, true);

    // if needs inversion, run it only after the clear and rect functions or otherwise it won't work
    init_display_st7789_170x320_inversion();

    qp_power(st7789_display, true);
    qp_flush(st7789_display);
    if (has_run) {
        forced_reinit = true;
    }
    has_run = true;
}

void init_display_st7789_170x320(void) {
    st7789_display = qp_st7789_make_spi_device(240, 320, ST7789_CS_PIN, ST7789_DC_PIN, ST7789_RST_PIN,
                                               ST7789_SPI_DIVIDER, ST7789_SPI_MODE);
    init_display_st7789_170x320_rotation();

    qp_rect(st7789_display, 0, 0, 240 - 1, 320 - 1, 0, 0, 0, true);

    st7789_170x320_draw_user();
}

void st7789_170x320_display_power(bool on) {
    qp_power(st7789_display, on);
}

__attribute__((weak)) void st7789_170x320_draw_user(void) {
    uint16_t width;
    uint16_t height;
    qp_get_geometry(st7789_display, &width, &height, NULL, NULL, NULL);

    static bool force_redraw = false;
#ifdef COMMUNITY_MODULE_DISPLAY_MENU_ENABLE
    if (painter_render_menu(st7789_display, font_oled, 0, 0, width, height, false,
                            userspace_config.display.painter.hsv.primary,
                            userspace_config.display.painter.hsv.secondary)) {
        force_redraw = true;
    } else
#endif // COMMUNITY_MODULE_DISPLAY_MENU_ENABLE
    {
        static uint16_t screen_saver_timer  = 0;
        static bool     screen_saver_redraw = true;
        static uint8_t  display_mode_ref    = 0;
        uint8_t         display_logo_index  = 0;
        bool            display_logo_cycle  = false;
        if (is_keyboard_left()) {
            display_logo_index = userspace_config.display.painter.left.display_logo;
            display_logo_cycle = userspace_config.display.painter.left.display_logo_cycle;
        } else {
            display_logo_index = userspace_config.display.painter.right.display_logo;
            display_logo_cycle = userspace_config.display.painter.right.display_logo_cycle;
        }

        if (display_logo_cycle && timer_elapsed(screen_saver_timer) > 5000) {
            static uint8_t last_display_mode = 0;
            if (last_display_mode != display_logo_index) {
                last_display_mode = display_logo_index;
                display_mode_ref  = 0; // reset the reference
            } else {
                display_mode_ref++;
            }
            screen_saver_redraw = true;
            screen_saver_timer  = timer_read();
        } else if (!display_logo_cycle) {
            display_mode_ref = 0; // reset the reference
            // xprintf("Screen saver: %d, reset at %u\n", display_mode_ref, screen_saver_timer);
        }
        if (force_redraw || screen_saver_redraw) {
            screen_saver_redraw = force_redraw = false;
            screen_saver                       = qp_load_image_mem(
                screen_saver_image[(display_logo_index + display_mode_ref) % screensaver_image_size].data);
            if (screen_saver != NULL) {
                qp_drawimage(st7789_display, 0, 0, screen_saver);
                qp_close_image(screen_saver);
            } else {
                qp_rect(st7789_display, 0, 0, width - 1, height - 1, 0, 0, 0, true);
            }
        }
    }

    qp_flush(st7789_display);
}

void st7789_170x320_display_shutdown(bool jump_to_bootloader) {
    st7789_170x320_display_power(true);
    painter_render_shutdown(st7789_display, jump_to_bootloader);
}
