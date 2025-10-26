// Copyright 2024 Christopher Courtney, aka Drashna Jael're  (@drashna) <drashna@live.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <stdio.h>
#include "action.h"
#include "progmem.h"

extern bool layer_map_has_updated;

bool process_record_display_driver(uint16_t keycode, keyrecord_t* record);
void keyboard_post_init_display_driver(void);
void display_rotate_screen(bool clockwise, bool is_left);

#ifndef DISPLAY_CONSOLE_LOG_LINE_NUM
#    if defined(OLED_ENABLE) && !defined(QUANTUM_PAINTER_ENABLE)
#        define DISPLAY_CONSOLE_LOG_LINE_NUM 4
#    else // OLED_ENABLE && !QUANTUM_PAINTER_ENABLE
#        define DISPLAY_CONSOLE_LOG_LINE_NUM 24
#    endif // OLED_ENABLE && !QUANTUM_PAINTER_ENABLE
#endif
#if !defined(DISPLAY_CONSOLE_LOG_LINE_START)
#    if DISPLAY_CONSOLE_LOG_LINE_NUM > 10
#        define DISPLAY_CONSOLE_LOG_LINE_START (DISPLAY_CONSOLE_LOG_LINE_NUM - 10)
#    else // DISPLAY_CONSOLE_LOG_LINE_NUM
#        define DISPLAY_CONSOLE_LOG_LINE_START 0
#    endif // DISPLAY_CONSOLE_LOG_LINE_NUM
#endif     // DISPLAY_CONSOLE_LOG_LINE_START
#ifndef DISPLAY_CONSOLE_LOG_LINE_LENGTH
#    if defined(OLED_ENABLE) && !defined(QUANTUM_PAINTER_ENABLE)
#        define DISPLAY_CONSOLE_LOG_LINE_LENGTH 20
#    else // OLED_ENABLE && !QUANTUM_PAINTER_ENABLE
#        if defined(QUANTUM_PAINTER_ILI9488_SPI_ENABLE)
#            define DISPLAY_CONSOLE_LOG_LINE_LENGTH 52
#        else // QUANTUM_PAINTER_ILI9488_SPI_ENABLE
#            define DISPLAY_CONSOLE_LOG_LINE_LENGTH 38
#        endif // QUANTUM_PAINTER_ILI9488_SPI_ENABLE
#    endif     // OLED_ENABLE && !QUANTUM_PAINTER_ENABLE
#endif         // DISPLAY_CONSOLE_LOG_LINE_LENGTH
extern bool  console_log_needs_redraw, console_has_redrawn;
extern char* logline_ptrs[DISPLAY_CONSOLE_LOG_LINE_NUM + 1];
