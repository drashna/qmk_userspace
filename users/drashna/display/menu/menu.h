// Copyright 2018-2024 Nick Brassel (@tzarc)
// Copyright 2024 Drashna (@drashna)
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "action.h"

typedef enum _menu_flags_t {
    menu_flag_is_parent = (1 << 0),
    menu_flag_is_value  = (1 << 1),
} menu_flags_t;

typedef enum _menu_input_t {
    menu_input_exit,
    menu_input_back,
    menu_input_enter,
    menu_input_up,
    menu_input_down,
    menu_input_left,
    menu_input_right,
} menu_input_t;

typedef struct _menu_entry_t {
    menu_flags_t flags;
    const char  *text;
    const char  *short_text;
    struct {
        struct _menu_entry_t *children;
        size_t                child_count;
    } parent;
    struct {
        bool (*menu_handler)(menu_input_t input);
        void (*display_handler)(char *text_buffer, size_t buffer_len);
    } child;
} menu_entry_t;

menu_entry_t *get_current_menu(void);
menu_entry_t *get_selected_menu_item(void);
bool          menu_handle_input(menu_input_t input);
bool          process_record_menu(uint16_t keycode, keyrecord_t *record);
void          display_menu_set_dirty(bool state);
uint8_t       get_menu_scroll_offset(menu_entry_t *menu, uint8_t visible_entries);
void          keyboard_task_display_menu_pre(void);
void          keyboard_task_display_menu_post(void);
