// Copyright 2025 Christopher Courtney, aka Drashna Jael're  (@drashna) <drashna@live.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "drashna_runtime.h"
#include "qp.h"
#include "qp_comms.h"
#include "display/painter/painter.h"
#include "display/painter/sh1107_128x128.h"

#ifdef COMMUNITY_MODULE_DISPLAY_MENU_ENABLE
#    include "qp_render_menu.h"
#endif

#ifdef CUSTOM_QUANTUM_PAINTER_SH1107_128X128_SPI
#    ifndef OLED_CS_PIN
#        define OLED_CS_PIN DISPLAY_CS_PIN
#    endif // OLED_CS_PIN
#    ifndef OLED_DC_PIN
#        define OLED_DC_PIN DISPLAY_DC_PIN
#    endif // OLED_DC_PIN
#    ifndef OLED_RST_PIN
#        ifndef DISPLAY_RST_PIN
#            define OLED_RST_PIN NO_PIN
#        else // DISPLAY_RST_PIN
#            define OLED_RST_PIN DISPLAY_RST_PIN
#        endif // DISPLAY_RST_PIN
#    endif     // OLED_RST_PIN
#    ifndef OLED_SPI_DIVIDER
#        ifndef DISPLAY_SPI_DIVIDER
#            define OLED_SPI_DIVIDER 1
#        else // DISPLAY_SPI_DIVIDER
#            define OLED_SPI_DIVIDER DISPLAY_SPI_DIVIDER
#        endif // DISPLAY_SPI_DIVIDER
#    endif     // OLED_SPI_DIVIDER
#    ifndef OLED_SPI_MODE
#        ifndef DISPLAY_SPI_MODE
#            define OLED_SPI_MODE 3
#        else // DISPLAY_SPI_MODE
#            define OLED_SPI_MODE DISPLAY_SPI_MODE
#        endif // DISPLAY_SPI_MODE
#    endif     // OLED_SPI_MODE
#elif defined(CUSTOM_QUANTUM_PAINTER_SH1107_128X128_I2C)
#    ifndef OLED_I2C_ADDRESS
#        define OLED_I2C_ADDRESS 0x3C
#    endif // OLED_I2C_ADDRESS
#endif

painter_device_t sh1107_128x128_display;
static bool      has_run = false, forced_reinit = false;

void init_display_sh1107_128x128_inversion(void) {
    qp_comms_start(sh1107_128x128_display);
    qp_comms_command(sh1107_128x128_display, (is_keyboard_left() ? userspace_config.display.painter.left.inverted
                                                                 : userspace_config.display.painter.right.inverted)
                                                 ? 0xA7
                                                 : 0xA6);
    qp_comms_stop(sh1107_128x128_display);
    if (has_run) {
        forced_reinit = true;
    }
}

void init_display_sh1107_128x128_rotation(void) {
    painter_rotation_t rotation = (is_keyboard_left() ? userspace_config.display.painter.left.rotation
                                                      : userspace_config.display.painter.right.rotation)
                                      ? QP_ROTATION_0
                                      : QP_ROTATION_180;

    qp_init(sh1107_128x128_display, rotation);
    qp_clear(sh1107_128x128_display);
    qp_rect(sh1107_128x128_display, 0, 0, 128 - 1, 128 - 1, HSV_BLACK, true);

    init_display_sh1107_128x128_inversion();

    painter_render_frame_box(sh1107_128x128_display, (hsv_t){.h = 0, .s = 255, .v = 255}, 0, 0, 0, 2, true, false);

    qp_power(sh1107_128x128_display, true);
    qp_flush(sh1107_128x128_display);
    if (has_run) {
        forced_reinit = true;
    }
    has_run = true;
}

void init_display_sh1107_128x128(void) {
#if defined(CUSTOM_QUANTUM_PAINTER_SH1107_128X128_SPI)
    sh1107_128x128_display =
        qp_sh1107_make_spi_device(128, 128, OLED_CS_PIN, OLED_DC_PIN, OLED_RST_PIN, OLED_SPI_DIVIDER, OLED_SPI_MODE);
#elif defined(CUSTOM_QUANTUM_PAINTER_SH1107_128X128_I2C)
    sh1107_128x128_display = qp_sh1107_make_i2c_device(128, 128, OLED_I2C_ADDRESS);
#endif
    init_display_sh1107_128x128_rotation();

    sh1107_128x128_draw_user();
}

void sh1107_128x128_display_power(bool on) {
    qp_power(sh1107_128x128_display, on);
}

void sh1107_128x128_display_shutdown(bool jump_to_bootloader) {}

__attribute__((weak)) void sh1107_128x128_draw_user(void) {
    // static uint16_t last_activity = UINT16_MAX - 9999;
    // qp_rect(sh1107_128x128_display, 0, 0, 240, 320 - 1, HSV_MAGENTA, true);

    qp_flush(sh1107_128x128_display);
}
