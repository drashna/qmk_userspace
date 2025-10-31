// Copyright 2025 Christopher Courtney, aka Drashna Jael're  (@drashna) <drashna@live.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "qp.h"
#include "drashna_runtime.h"
#include "display/painter/fonts/font_oled.qff.h"
#include "display/painter/fonts/font_proggy_clean.qff.h"
#include "display/painter/fonts/font_proggy_tiny.qff.h"
#include "display/painter/fonts/font_thintel15.qff.h"
#include "display/painter/graphics.qgf.h"
#include "display/painter/graphics/assets.h"
#include "display/display.h"

#ifndef WPM_PAINTER_GRAPH_HEIGHT
#    define WPM_PAINTER_GRAPH_HEIGHT 49
#endif // WPM_PAINTER_GRAPH_HEIGHT
#ifndef WPM_PAINTER_GRAPH_WIDTH
#    define WPM_PAINTER_GRAPH_WIDTH 58
#endif // WPM_PAINTER_GRAPH_WIDTH

typedef struct {
    const uint8_t* data;
    const char*    name;
} painter_image_array_t;
extern painter_image_array_t screen_saver_image[];
extern const uint8_t         screensaver_image_size;

typedef struct {
    void (*painter_function)(painter_device_t device, painter_font_handle_t font, uint16_t x, uint16_t y,
                             uint16_t width, uint16_t height, bool force_redraw, dual_hsv_t* curr_hsv);
    const char* name;
} painter_display_menu_block_mode_t;
extern const uint8_t painter_display_menu_block_modes_count;

extern painter_font_handle_t font_thintel, font_mono, font_oled;

void housekeeping_task_display_menu_user(void);
void keyboard_post_init_quantum_painter(void);
void suspend_power_down_quantum_painter(void);
void suspend_wakeup_init_quantum_painter(void);
void shutdown_quantum_painter(bool jump_to_bootloader);

char* truncate_text(const char* text, uint16_t max_width, painter_font_handle_t font, bool from_start,
                    bool add_ellipses);
void  render_character_set(painter_device_t display, uint16_t* x_offset, uint16_t* max_pos, uint16_t* ypos,
                           painter_font_handle_t font, uint8_t hue_fg, uint8_t sat_fg, uint8_t val_fg, uint8_t hue_bg,
                           uint8_t sat_bg, uint8_t val_bg);

bool painter_render_side(void);
void painter_render_frame_box(painter_device_t device, hsv_t hsv, uint16_t x_buffer, uint16_t y_buffer,
                              int16_t x_offset, int16_t y_offset, bool top_indents, bool side_indents);
void painter_render_frame(painter_device_t device, painter_font_handle_t font_title, bool right_side, uint16_t offset,
                          bool color_side);
void painter_render_rtc_time(painter_device_t device, painter_font_handle_t font, uint16_t x, uint16_t y,
                             uint16_t display_width, bool force_redraw, uint16_t* rtc_timer, hsv_t* hsv);
void painter_render_console(painter_device_t device, painter_font_handle_t font, uint16_t x, uint16_t y,
                            uint16_t display_width, bool force_redraw, hsv_t* hsv, uint8_t start, uint8_t end);
void painter_render_scan_rate(painter_device_t device, painter_font_handle_t font, uint16_t x, uint16_t y,
                              bool force_redraw, dual_hsv_t* curr_hsv);
void painter_render_rgb(painter_device_t device, painter_font_handle_t font, uint16_t x, uint16_t y, bool force_redraw,
                        dual_hsv_t* curr_hsv, const char* title, const char* (*get_rgb_mode)(void),
                        hsv_t (*get_rgb_hsv)(void), bool is_enabled, uint8_t max_val);
void painter_render_lock_state(painter_device_t device, painter_font_handle_t font, uint16_t x, uint16_t y,
                               bool force_redraw, dual_hsv_t* curr_hsv, uint8_t disabled_val);
void painter_render_wpm(painter_device_t device, painter_font_handle_t font, uint16_t x, uint16_t y, bool force_redraw,
                        dual_hsv_t* curr_hsv);
void painter_render_wpm_graph(painter_device_t device, painter_font_handle_t font, uint16_t x, uint16_t y,
                              uint16_t width, uint16_t height, bool force_redraw, dual_hsv_t* curr_hsv);
void painter_render_haptic(painter_device_t device, painter_font_handle_t font, uint16_t x, uint16_t y,
                           bool force_redraw, dual_hsv_t* curr_hsv);
void painter_render_totp(painter_device_t device, painter_font_handle_t font, uint16_t x, uint16_t y, uint16_t width,
                         bool force_redraw, dual_hsv_t* curr_hsv, bool wide_load);
void painter_render_menu_block(painter_device_t device, painter_font_handle_t font, uint16_t x, uint16_t y,
                               uint16_t width, uint16_t height, bool force_redraw, dual_hsv_t* curr_hsv, bool side,
                               bool is_thicc);
void painter_render_keylogger(painter_device_t device, painter_font_handle_t font, uint16_t x, uint16_t y,
                              uint16_t width, bool force_redraw, dual_hsv_t* curr_hsv);
void painter_render_autocorrect(painter_device_t device, painter_font_handle_t font, uint16_t x, uint16_t y,
                                uint16_t width, bool force_redraw, dual_hsv_t* curr_hsv);
void painter_render_os_detection(painter_device_t device, painter_font_handle_t font, uint16_t x, uint16_t y,
                                 uint16_t width, bool force_redraw, dual_hsv_t* curr_hsv);
void painter_render_modifiers(painter_device_t device, painter_font_handle_t font, uint16_t x, uint16_t y,
                              uint16_t width, bool force_redraw, dual_hsv_t* curr_hsv, uint8_t disabled_val);
void painter_render_qmk_info(painter_device_t device, painter_font_handle_t font, uint16_t x, uint16_t y,
                             uint16_t width, bool force_redraw, dual_hsv_t* curr_hsv);
void painter_render_layer_map(painter_device_t device, painter_font_handle_t font, uint16_t x, uint16_t y,
                              uint16_t width, bool force_redraw, dual_hsv_t* curr_hsv);
bool painter_render_shutdown(painter_device_t device, bool jump_to_bootloader);
void render_life(painter_device_t display, uint16_t xpos, uint16_t ypos, dual_hsv_t* curr_hsv, bool force_redraw);
bool qp_draw_graph_l(painter_device_t device, uint16_t graph_x, uint16_t graph_y, uint16_t graph_width,
                     uint16_t graph_height, dual_hsv_t* curr_hsv, uint8_t* graph_data, uint8_t graph_segments,
                     uint8_t scale_to);
void painter_render_pd_accel_graph(painter_device_t device, uint16_t x, uint16_t y, uint16_t width, uint16_t height,
                                   bool force_redraw, dual_hsv_t* curr_hsv);

dual_hsv_t painter_get_dual_hsv(void);
void       painter_sethsv(uint8_t hue, uint8_t sat, uint8_t val, bool primary);
void       painter_sethsv_noeeprom(uint8_t hue, uint8_t sat, uint8_t val, bool primary);
hsv_t      painter_get_hsv(bool primary);
uint8_t    painter_get_hue(bool primary);
uint8_t    painter_get_sat(bool primary);
uint8_t    painter_get_val(bool primary);
void       painter_increase_hue(bool primary);
void       painter_increase_hue_noeeprom(bool primary);
void       painter_decrease_hue(bool primary);
void       painter_decrease_hue_noeeprom(bool primary);
void       painter_increase_sat(bool primary);
void       painter_increase_sat_noeeprom(bool primary);
void       painter_decrease_sat(bool primary);
void       painter_decrease_sat_noeeprom(bool primary);
void       painter_increase_val(bool primary);
void       painter_increase_val_noeeprom(bool primary);
void       painter_decrease_val(bool primary);
void       painter_decrease_val_noeeprom(bool primary);
