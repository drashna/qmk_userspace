// Copyright 2024 Christopher Courtney, aka Drashna Jael're  (@drashna) <drashna@live.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "qp.h"
#include "display/painter/painter.h"
#include <stdio.h>
#if defined(QUANTUM_PAINTER_ILI9341_ENABLE) && defined(CUSTOM_QUANTUM_PAINTER_ILI9341)
#    include "display/painter/ili9341_display.h"
#endif // QUANTUM_PAINTER_ILI9341_ENABLE && CUSTOM_QUANTUM_PAINTER_ILI9341

/**
 * @brief Truncates text to fit within a certain width
 *
 * @param text original text
 * @param max_width max width in pixels
 * @param font font being used
 * @return char* truncated text
 */
char* truncate_text(const char* text, uint16_t max_width, painter_font_handle_t font, bool from_start,
                    bool add_ellipses) {
    static char truncated_text[50];
    strncpy(truncated_text, text, sizeof(truncated_text) - 1);
    truncated_text[sizeof(truncated_text) - 1] = '\0';

    uint16_t text_width = qp_textwidth(font, truncated_text);
    if (text_width <= max_width) {
        return truncated_text;
    }

    size_t      len            = strlen(truncated_text);
    const char* ellipses       = "...";
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
 * @param max_pos array to store max x position of each line for clearing after rerendering
 * @param ypos y position to start rendering at
 * @param font font to use
 * @param hue_fg text hue
 * @param sat_fg text saturation
 * @param val_fg text value
 * @param hue_bg background hue
 * @param sat_bg background saturation
 * @param val_bg background value
 */
static const char* test_text[] = {
    // did intentionally skip PROGMEM here :)
    "abcdefghijklmnopqrstuvwxyz",
    "ABCDEFGHIJKLMNOPQRSTUVWXYZ",
    "01234567890 !@#$%%^&*()",
    "__+-=[]{}\\|;:'\",.<>/?",
};

void render_character_set(painter_device_t display, uint16_t* x_offset, uint16_t* max_pos, uint16_t* ypos,
                          painter_font_handle_t font, uint8_t hue_fg, uint8_t sat_fg, uint8_t val_fg, uint8_t hue_bg,
                          uint8_t sat_bg, uint8_t val_bg) {
    for (uint8_t i = 0; i < 4; ++i) {
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
void housekeeping_task_quantum_painter(void) {
#ifdef QUANTUM_PAINTER_ILI9341_ENABLE
    ili9341_draw_user();
#endif // QUANTUM_PAINTER_ILI9341_ENABLE
#if (QUANTUM_PAINTER_DISPLAY_TIMEOUT) > 0
    if (is_keyboard_master() && (last_input_activity_elapsed() > QUANTUM_PAINTER_DISPLAY_TIMEOUT)) {
        qp_backlight_disable();
    } else {
        qp_backlight_enable();
    }
#endif
}
void keyboard_post_init_quantum_painter(void) {
#ifdef BACKLIGHT_ENABLE
    backlight_level_noeeprom(BACKLIGHT_LEVELS);
#elif defined(BACKLIGHT_PIN)
    gpio_set_pin_output_push_pull(BACKLIGHT_PIN);
    gpio_write_pin_high(BACKLIGHT_PIN);
#endif
    wait_ms(150);
#ifdef QUANTUM_PAINTER_ILI9341_ENABLE
    init_display_ili9341();
#endif // QUANTUM_PAINTER_ILI9341_ENABLE
}

void suspend_power_down_quantum_painter(void) {
    qp_backlight_disable();
#ifdef QUANTUM_PAINTER_ILI9341_ENABLE
    ili9341_display_power(false);
#endif // QUANTUM_PAINTER_ILI9341_ENABLE
}

void suspend_wakeup_init_quantum_painter(void) {
#ifdef QUANTUM_PAINTER_ILI9341_ENABLE
    ili9341_display_power(true);
#endif // QUANTUM_PAINTER_ILI9341_ENABLE
    qp_backlight_enable();
}

void shutdown_quantum_painter(void) {
#ifdef BACKLIGHT_ENABLE
    qp_backlight_disable();
#elif defined(BACKLIGHT_PIN)
    gpio_write_pin_low(BACKLIGHT_PIN);
#endif
#ifdef QUANTUM_PAINTER_ILI9341_ENABLE
    ili9341_display_power(false);
#endif // QUANTUM_PAINTER_ILI9341_ENABLE
}
