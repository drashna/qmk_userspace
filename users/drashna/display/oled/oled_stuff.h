// Copyright 2020 Christopher Courtney, aka Drashna Jael're  (@drashna) <drashna@live.com>
// Copyright 2021 John Ezra - wpm graph
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "quantum.h"
#include "oled_driver.h"
#include "oled_assets.h"
#include "display/display.h"
#ifdef DEFFERED_EXEC_ENABLE
extern deferred_token kittoken;
#endif

typedef struct {
    const uint16_t data_len;
    const char*    block_map;
    const char*    block_list;
} compressed_oled_frame_t;

void oled_driver_render_logo(void);
bool process_record_user_oled(uint16_t keycode, keyrecord_t* record);
bool oled_init_keymap(oled_rotation_t* rotation, bool has_run);
void oled_timer_reset(void);
void render_keylogger_status(uint8_t col, uint8_t line);
void render_default_layer_state(uint8_t col, uint8_t line);
void render_layer_state(uint8_t col, uint8_t line);
void render_keylock_status(led_t led_usb_state, uint8_t col, uint8_t line);
void render_matrix_scan_rate(uint8_t padding, uint8_t col, uint8_t line);
void render_mod_status(uint8_t modifiers, uint8_t col, uint8_t line);
void render_bootmagic_status(uint8_t col, uint8_t line);
void render_user_status(uint8_t col, uint8_t line);
void oled_driver_render_logo(void);
void render_wpm(uint8_t padding, uint8_t col, uint8_t line);
void render_pointing_dpi_status(uint16_t cpi, uint8_t padding, uint8_t col, uint8_t line);
void oled_driver_render_logo_left(void);
void oled_driver_render_logo_right(void);
void oled_render_large_display(bool side);
void oled_render_mario(uint8_t col, uint8_t line);
void render_pet(uint8_t col, uint8_t line);
void render_unicode_mode(uint8_t col, uint8_t line);
void render_unicode_mode_small(uint8_t col, uint8_t line, bool invert);
void render_rgb_hsv(uint8_t col, uint8_t line);
void render_rgb_mode(uint8_t col, uint8_t line);
void render_mouse_mode(uint8_t col, uint8_t line);
void render_autocorrected_info(uint8_t col, uint8_t line);
void render_cyberpunk_logo(uint8_t col, uint8_t line);
void render_arasaka_logo(uint8_t col, uint8_t line);
void housekeeping_task_oled(void);
void keyboard_post_init_oled(void);
void render_oled_title(bool side);
void oled_shutdown(bool jump_to_bootloader);
void oled_render_time(uint8_t col, uint8_t line);
void render_os(uint8_t col, uint8_t line);
void oled_write_compressed(compressed_oled_frame_t frame);
void oled_write_compressed_P(compressed_oled_frame_t frame);
void oled_post_init(void);
void oled_brightness_increase_step(void);
void oled_brightness_decrease_step(void);

void        render_wpm_graph(uint8_t start_offset, uint8_t cutoff, uint8_t max_lines_graph, uint8_t vertical_offset);
void        oled_pan_section(bool left, uint16_t y_start, uint16_t y_end, uint16_t x_start, uint16_t x_end);
const char* get_oled_keylogger_str(void);
void        split_sync_oled_keylogger_str(const uint8_t* data, uint8_t size);

#if defined(OLED_DISPLAY_128X128) || defined(OLED_DISPLAY_128X64)
#    define OLED_DISPLAY_VERBOSE

#    define OLED_RENDER_KEYLOGGER "Keylogger: "
#    ifndef OLED_KEYLOGGER_LENGTH
#        define OLED_KEYLOGGER_LENGTH 14
#    endif
#    define OLED_RENDER_LAYOUT_NAME       "Layout: "
#    define OLED_RENDER_LAYOUT_QWERTY     "Qwerty"
#    define OLED_RENDER_LAYOUT_COLEMAK_DH "Colemak DH"
#    define OLED_RENDER_LAYOUT_COLEMAK    "Colemak"
#    define OLED_RENDER_LAYOUT_DVORAK     "Dvorak"
#    define OLED_RENDER_LAYOUT_WORKMAN    "Workman"
#    define OLED_RENDER_LAYOUT_NORMAN     "Norman"
#    define OLED_RENDER_LAYOUT_MALTRON    "Matron"
#    define OLED_RENDER_LAYOUT_EUCALYN    "Eucalyn"
#    define OLED_RENDER_LAYOUT_CARPLAX    "Carplax"

#    define OLED_RENDER_LAYER_NAME   "Layer:"
#    define OLED_RENDER_LAYER_LOWER  "Lower"
#    define OLED_RENDER_LAYER_RAISE  "Raise"
#    define OLED_RENDER_LAYER_ADJUST "Adjust"
#    define OLED_RENDER_LAYER_MODS   "Mods"

#    define OLED_RENDER_LOCK_NAME "Lock: "
#    define OLED_RENDER_LOCK_NUML "NUM"
#    define OLED_RENDER_LOCK_CAPS "CAPS"
#    define OLED_RENDER_LOCK_SCLK "SCLK"

#    define OLED_RENDER_MODS_NAME "Mods"
#    define OLED_RENDER_MODS_SFT  "Sft"
#    define OLED_RENDER_MODS_CTL  "Ctl"
#    define OLED_RENDER_MODS_ALT  "Alt"
#    define OLED_RENDER_MODS_GUI  "GUI"

#    define OLED_RENDER_BOOTMAGIC_NAME    "Boot  "
#    define OLED_RENDER_BOOTMAGIC_NKRO    "NKRO"
#    define OLED_RENDER_BOOTMAGIC_NOGUI   "nGUI"
#    define OLED_RENDER_BOOTMAGIC_GRV     "GRV"
#    define OLED_RENDER_BOOTMAGIC_ONESHOT "1SHT"
#    define OLED_RENDER_BOOTMAGIC_SWAP    "SWAP"
#    define OLED_RENDER_BOOTMAGIC_CAPS    "CAPS"

#    define OLED_RENDER_USER_NAME "USER:"
#    define OLED_RENDER_USER_ANIM "Anim"
#    define OLED_RENDER_USER_LAYR "Layr"
#    define OLED_RENDER_USER_NUKE "Nuke"

#    define OLED_RENDER_WPM_COUNTER "WPM: "
#else
#    define OLED_RENDER_KEYLOGGER "KLogr"
#    ifndef OLED_KEYLOGGER_LENGTH
#        define OLED_KEYLOGGER_LENGTH 5
#    endif

#    define OLED_RENDER_LAYOUT_NAME       "Lyout"
#    define OLED_RENDER_LAYOUT_QWERTY     " QRTY"
#    define OLED_RENDER_LAYOUT_COLEMAK_DH " cmDH"
#    define OLED_RENDER_LAYOUT_COLEMAK    " COLE"
#    define OLED_RENDER_LAYOUT_DVORAK     " DVRK"
#    define OLED_RENDER_LAYOUT_WORKMAN    " WKMN"
#    define OLED_RENDER_LAYOUT_NORMAN     " NORM"
#    define OLED_RENDER_LAYOUT_MALTRON    " MLTN"
#    define OLED_RENDER_LAYOUT_EUCALYN    " ECLN"
#    define OLED_RENDER_LAYOUT_CARPLAX    " CRPX"

#    define OLED_RENDER_LAYER_NAME   "LAYER"
#    define OLED_RENDER_LAYER_LOWER  "Lower"
#    define OLED_RENDER_LAYER_RAISE  "Raise"
#    define OLED_RENDER_LAYER_ADJUST "Adjst"
#    define OLED_RENDER_LAYER_MODS   " Mods"

#    define OLED_RENDER_LOCK_NAME "Lock:"
#    define OLED_RENDER_LOCK_NUML "NumL"
#    define OLED_RENDER_LOCK_CAPS "CapL"
#    define OLED_RENDER_LOCK_SCLK "ScrL"

#    define OLED_RENDER_MODS_NAME "Mods: "
#    define OLED_RENDER_MODS_SFT  "Shft"
#    define OLED_RENDER_MODS_CTL  "Ctrl"
#    define OLED_RENDER_MODS_ALT  "Alt\n"
#    define OLED_RENDER_MODS_GUI  "GUI\n"

#    define OLED_RENDER_BOOTMAGIC_NAME    "BTMGK"
#    define OLED_RENDER_BOOTMAGIC_NKRO    "NKRO"
#    define OLED_RENDER_BOOTMAGIC_NOGUI   "nGUI"
#    define OLED_RENDER_BOOTMAGIC_GRV     "GRV"
#    define OLED_RENDER_BOOTMAGIC_ONESHOT "1SHT"
#    define OLED_RENDER_BOOTMAGIC_SWAP    "SWAP"
#    define OLED_RENDER_BOOTMAGIC_CAPS    "CAPS"

#    define OLED_RENDER_USER_NAME "USER:"
#    define OLED_RENDER_USER_ANIM "Anim"
#    define OLED_RENDER_USER_LAYR "Layr"
#    define OLED_RENDER_USER_NUKE "Nuke"

#    define OLED_RENDER_WPM_COUNTER "WPM: "
#endif

#ifndef OLED_WPM_GRAPH_MAX_WPM
#    define OLED_WPM_GRAPH_MAX_WPM 120
#endif
#ifndef OLED_WPM_GRAPH_REFRESH_INTERVAL
#    define OLED_WPM_GRAPH_REFRESH_INTERVAL 300
#endif
#ifndef OLED_WPM_GRAPH_AREA_FILL_INTERVAL
#    define OLED_WPM_GRAPH_AREA_FILL_INTERVAL 3
#endif
#ifndef OLED_WPM_GRAPH_VERTCAL_LINE_INTERVAL
#    define OLED_WPM_GRAPH_VERTCAL_LINE_INTERVAL 3
#endif
#ifndef OLED_WPM_GRAPH_GRAPH_LINE_THICKNESS
#    define OLED_WPM_GRAPH_GRAPH_LINE_THICKNESS 2
#endif
