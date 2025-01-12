// Copyright 2025 Christopher Courtney, aka Drashna Jael're  (@drashna) <drashna@live.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "drashna_runtime.h"

#include "qp.h"
#include "qp_comms.h"
#include "qp_st77xx_opcodes.h"
#include "display/painter/painter.h"
#include "display/painter/st7789_135x240.h"
#ifdef COMMUNITY_MODULE_DISPLAY_MENU_ENABLE
#    include "qp_render_menu.h"
#endif
#ifdef QUANTUM_PAINTER_DRIVERS_ST7789_135X240_SURFACE
#    include "qp_surface.h"
#endif // QUANTUM_PAINTER_DRIVERS_ST7789_135X240_SURFACE

#ifndef ST7789_MINI_CS_PIN
#    define ST7789_MINI_CS_PIN DISPLAY_CS_PIN
#endif // ST7789_MINI_CS_PIN
#ifndef ST7789_MINI_DC_PIN
#    define ST7789_MINI_DC_PIN DISPLAY_DC_PIN
#endif // ST7789_MINI_DC_PIN
#ifndef ST7789_MINI_RST_PIN
#    ifndef DISPLAY_RST_PIN
#        define ST7789_MINI_RST_PIN NO_PIN
#    else // DISPLAY_RST_PIN
#        define ST7789_MINI_RST_PIN DISPLAY_RST_PIN
#    endif // DISPLAY_RST_PIN
#endif     // ST7789_MINI_RST_PIN
#ifndef ST7789_MINI_SPI_DIVIDER
#    ifndef DISPLAY_SPI_DIVIDER
#        define ST7789_MINI_SPI_DIVIDER 1
#    else // DISPLAY_SPI_DIVIDER
#        define ST7789_MINI_SPI_DIVIDER DISPLAY_SPI_DIVIDER
#    endif // DISPLAY_SPI_DIVIDER
#endif     // ST7789_MINI_SPI_DIVIDER
#ifndef ST7789_MINI_SPI_MODE
#    ifndef DISPLAY_SPI_MODE
#        define ST7789_MINI_SPI_MODE 3
#    else // DISPLAY_SPI_MODE
#        define ST7789_MINI_SPI_MODE DISPLAY_SPI_MODE
#    endif // DISPLAY_SPI_MODE
#endif     // ST7789_MINI_SPI_MODE

painter_device_t st7789_135x240_display;
#ifdef QUANTUM_PAINTER_DRIVERS_ST7789_135X240_SURFACE
static painter_device_t st7789_135x240_surface_display;
#    if HAL_USE_SDRAM == TRUE
__attribute__((section(".ram7")))
#    endif
static uint8_t display_buffer[SURFACE_REQUIRED_BUFFER_BYTE_SIZE(135, 240, 16)];
#else
#    define st7789_135x240_surface_display st7789_135x240_display
#endif // QUANTUM_PAINTER_DRIVERS_ST7789_135X240_SURFACE

static bool has_run = false, forced_reinit = false;

void init_display_st7789_135x240_inversion(void) {
    qp_comms_start(st7789_135x240_display);
    qp_comms_command(st7789_135x240_display,
                     userspace_config.display.inverted ? ST77XX_CMD_INVERT_OFF : ST77XX_CMD_INVERT_ON);
    qp_comms_stop(st7789_135x240_display);
    if (has_run) {
        forced_reinit = true;
    }
}

void init_display_st7789_135x240_rotation(void) {
    qp_init(st7789_135x240_display, userspace_config.display.rotation ? QP_ROTATION_0 : QP_ROTATION_180);
    // qp_set_viewport_offsets(st7789_135x240_display, 52, 40);
    qp_clear(st7789_135x240_display);
    qp_rect(st7789_135x240_display, 0, 0, 240 - 1, 320 - 1, 0, 0, 0, true);

    // #ifdef COMMUNITY_MODULE_DISPLAY_MENU_ENABLE
    //     extern menu_state_t menu_state;
    //     menu_state.is_in_menu     = true;
    //     menu_state.selected_child = 0;
    // #endif
    // if needs inversion, run it only after the clear and rect functions or otherwise it won't work
    init_display_st7789_135x240_inversion();

    qp_power(st7789_135x240_display, true);
    qp_flush(st7789_135x240_display);
    if (has_run) {
        forced_reinit = true;
    }
    has_run = true;
}

void init_display_st7789_135x240(void) {
    st7789_135x240_display =
        qp_st7789_make_spi_device(240, 320, ST7789_MINI_CS_PIN, ST7789_MINI_DC_PIN, ST7789_MINI_RST_PIN,
                                  ST7789_MINI_SPI_DIVIDER, ST7789_MINI_SPI_MODE);
#ifdef QUANTUM_PAINTER_DRIVERS_ST7789_135X240_SURFACE
    st7789_135x240_surface_display = qp_make_rgb565_surface(135, 240, display_buffer);
#endif // QUANTUM_PAINTER_DRIVERS_ST7789_135X240_SURFACE

    init_display_st7789_135x240_rotation();

#ifdef QUANTUM_PAINTER_DRIVERS_ST7789_135X240_SURFACE
    qp_init(st7789_135x240_surface_display, QP_ROTATION_0);

    qp_rect(st7789_135x240_surface_display, 0, 0, 135 - 1, 240 - 1, HSV_BLACK, true);
#endif // QUANTUM_PAINTER_DRIVERS_ST7789_135X240_SURFACE

    st7789_135x240_draw_user();
}

void st7789_135x240_display_power(bool on) {
    qp_power(st7789_135x240_display, on);
}

__attribute__((weak)) void st7789_135x240_draw_user(void) {
    static uint16_t last_activity = UINT16_MAX - 9999;

#ifdef COMMUNITY_MODULE_DISPLAY_MENU_ENABLE
#    ifdef QUANTUM_PAINTER_DRIVERS_ST7789_135X240_SURFACE
    if (!painter_render_menu(st7789_135x240_surface_display, font_oled, 0, 0, 135, 240, false,
                             userspace_config.display.painter.hsv.primary,
                             userspace_config.display.painter.hsv.secondary))
#    else  // QUANTUM_PAINTER_DRIVERS_ST7789_135X240_SURFACE
    if (!painter_render_menu(st7789_135x240_display, font_oled, 50, 40, 240, 320, false,
                             userspace_config.display.painter.hsv.primary,
                             userspace_config.display.painter.hsv.secondary))
#    endif // QUANTUM_PAINTER_DRIVERS_ST7789_135X240_SURFACE
#endif     // COMMUNITY_MODULE_DISPLAY_MENU_ENABLE
    {
        extern painter_image_array_t screen_saver_image[];
        extern const uint8_t         screensaver_image_size;
        painter_image_handle_t       screen_saver = NULL;

        static uint8_t screensaver = 0;

        if (timer_elapsed(last_activity) > 1000) {
            screensaver++;
            if (screensaver >= screensaver_image_size) {
                screensaver = 0;
            }

            screen_saver = qp_load_image_mem(screen_saver_image[screensaver].data);

            if (screen_saver != NULL) {
                qp_drawimage(st7789_135x240_display, 0, 0, screen_saver);
                qp_close_image(screen_saver);
                last_activity = timer_read();
            }
        }
    } else {
#ifdef QUANTUM_PAINTER_DRIVERS_ST7789_135X240_SURFACE
        qp_surface_draw(st7789_135x240_surface_display, st7789_135x240_display, 52, 40, 0);
#endif
        // last_activity = UINT16_MAX - 9999;
    }

    qp_flush(st7789_135x240_display);
}

void st7789_135x240_display_shutdown(bool jump_to_bootloader) {
    st7789_135x240_display_power(true);
    painter_render_shutdown(st7789_135x240_display, jump_to_bootloader);
}
