// Copyright 2024 Christopher Courtney, aka Drashna Jael're  (@drashna) <drashna@live.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "drashna.h"
#include "display.h"

#if defined(OLED_ENABLE) && defined(CUSTOM_OLED_DRIVER)
#    include "display/oled/oled_stuff.h"
#endif // OLED_ENABLE && CUSTOM_OLED_DRIVER
#if defined(QUANTUM_PAINTER_ENABLE) && defined(CUSTOM_QUANTUM_PAINTER_ENABLE)
#    include "qp.h"
#    if defined(DISPLAY_KEYLOGGER_ENABLE)
#        include "display/painter/keylogger.h"
#    endif
#    ifdef CUSTOM_QUANTUM_PAINTER_ILI9341
#        include "display/painter/ili9341_display.h"
#    endif // CUSTOM_QUANTUM_PAINTER_ILI9341
#    ifdef CUSTOM_QUANTUM_PAINTER_ILI9488
#        include "display/painter/ili9488_display.h"
#    endif // CUSTOM_QUANTUM_PAINTER_ILI9488
#endif     // QUANTUM_PAINTER_ENABLE && CUSTOM_QUANTUM_PAINTER_ENABLE
#ifdef COMMUNITY_MODULE_LAYER_MAP_ENABLE
#    include "layer_map.h"
#endif // COMMUNITY_MODULE_LAYER_MAP_ENABLE

void housekeeping_task_display(void) {
#if defined(COMMUNITY_MODULE_DISPLAY_MENU_ENABLE) && defined(SPLIT_KEYBOARD)
    if (is_keyboard_master()) {
        extern menu_state_t         menu_state;
        extern menu_state_runtime_t menu_state_runtime;

        if (memcmp(&menu_state, &userspace_runtime_state.display.menu_state, sizeof(menu_state_t)) != 0) {
            memcpy(&userspace_runtime_state.display.menu_state, &menu_state, sizeof(menu_state_t));
        }
        if (memcmp(&menu_state_runtime, &userspace_runtime_state.display.menu_state_runtime,
                   sizeof(menu_state_runtime_t)) != 0) {
            memcpy(&userspace_runtime_state.display.menu_state_runtime, &menu_state_runtime,
                   sizeof(menu_state_runtime_t));
        }
    }
#endif // COMMUNITY_MODULE_DISPLAY_MENU_ENABLE && SPLIT_KEYBOARD

#ifdef CUSTOM_OLED_DRIVER
    housekeeping_task_oled();
#endif // CUSTOM_OLED_DRIVER
#ifdef COMMUNITY_MODULE_LAYER_MAP_ENABLE
    static matrix_row_t last_matrix[MATRIX_ROWS] = {0};
    for (uint8_t i = 0; i < MATRIX_ROWS; i++) {
        if (last_matrix[i] != matrix_get_row(i)) {
            matrix_row_t row = matrix_get_row(i);
            last_matrix[i]   = row;
            set_layer_map_has_updated(true);
        }
    }
#endif // COMMUNITY_MODULE_LAYER_MAP_ENABLE
#if !defined(COMMUNITY_MODULE_DISPLAY_MENU_ENABLE) && defined(CUSTOM_QUANTUM_PAINTER_ENABLE)
    void housekeeping_task_display_menu_user(void);
    housekeeping_task_display_menu_user();
#endif // CUSTOM_QUANTUM_PAINTER_ENABLE
}

/**
 * @brief Keycode handler for oled display.
 *
 * This adds pressed keys to buffer, but also resets the oled timer
 *
 * @param keycode Keycode from matrix
 * @param record keyrecord data structure
 * @return true
 * @return false
 */
bool process_record_display_driver(uint16_t keycode, keyrecord_t* record) {
    userspace_runtime_state.last_keycode   = keycode;
    userspace_runtime_state.last_key_event = record->event;
#if defined(CUSTOM_QUANTUM_PAINTER_ENABLE) && defined(DISPLAY_KEYLOGGER_ENABLE)
    keylogger_process(keycode, record);
#endif // CUSTOM_QUANTUM_PAINTER_ENABLE
    if (record->event.pressed) {
#ifdef OLED_ENABLE
        if (!process_record_user_oled(keycode, record)) {
            return false;
        }
#endif // OLED_ENABLE
    }

    return true;
}

/**
 * @brief
 *
 */

void keyboard_post_init_display_driver(void) {
#if defined(COMMUNITY_MODULE_DISPLAY_MENU_ENABLE)
    userspace_runtime_state.display.menu_state = (menu_state_t){
#    ifdef DISPLAY_MENU_ENABLED_DEFAULT
        .is_in_menu     = true,
        .selected_child = 0x01,
#    else
        .is_in_menu     = false,
        .selected_child = 0xFF,
#    endif // DISPLAY_MENU_ENABLED_DEFAULT
        .menu_stack = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF},
    };
    if (userspace_config.display.menu_render_side == 0) {
        userspace_config.display.menu_render_side = 3;
    }
#endif // COMMUNITY_MODULE_DISPLAY_MENU_ENABLE
#if defined(OLED_ENABLE) && defined(CUSTOM_OLED_DRIVER)
    keyboard_post_init_oled();
#endif // OLED_ENABLE && CUSTOM_OLED_DRIVER
#if defined(QUANTUM_PAINTER_ENABLE) && defined(CUSTOM_QUANTUM_PAINTER_ENABLE)
    void keyboard_post_init_quantum_painter(void);
    keyboard_post_init_quantum_painter();
#endif // QUANTUM_PAINTER_ENABLE && CUSTOM_QUANTUM_PAINTER_ENABLE
}

bool           console_log_needs_redraw = false, console_has_redrawn = false;
static uint8_t log_write_idx = 0;
static char    loglines[DISPLAY_CONSOLE_LOG_LINE_NUM + 1][DISPLAY_CONSOLE_LOG_LINE_LENGTH + 2];
char*          logline_ptrs[DISPLAY_CONSOLE_LOG_LINE_NUM + 1];

#if defined(OLED_ENABLE) && !defined(QUANTUM_PAINTER_ENABLE)
_Static_assert(DISPLAY_CONSOLE_LOG_LINE_LENGTH <= (OLED_DISPLAY_WIDTH / OLED_FONT_WIDTH),
               "DISPLAY_CONSOLE_LOG_LINE_LENGTH must be lower than oled character limit");
#endif

/**
 * @brief Function for capturing console log messages.
 *
 * @param c
 */
void display_sendchar_hook(uint8_t c) {
    static bool first_setup = false;
    if (!first_setup) {
        memset(loglines, 0, sizeof(loglines));
        for (int i = 0; i < (DISPLAY_CONSOLE_LOG_LINE_NUM + 1); ++i) {
            logline_ptrs[i] = loglines[i];
        }
        first_setup = true;
    }

    if (c == '\n') {
        logline_ptrs[DISPLAY_CONSOLE_LOG_LINE_NUM][log_write_idx] = 0;
        char* tmp                                                 = logline_ptrs[0];
        for (int i = 0; i < DISPLAY_CONSOLE_LOG_LINE_NUM; ++i) {
            logline_ptrs[i] = logline_ptrs[i + 1];
        }
        logline_ptrs[DISPLAY_CONSOLE_LOG_LINE_NUM]    = tmp;
        log_write_idx                                 = 0;
        logline_ptrs[DISPLAY_CONSOLE_LOG_LINE_NUM][0] = 0;
        console_log_needs_redraw                      = true;
        console_has_redrawn                           = false;
    } else if (log_write_idx >= (DISPLAY_CONSOLE_LOG_LINE_LENGTH)) {
        // Ignore.
    } else {
        logline_ptrs[DISPLAY_CONSOLE_LOG_LINE_NUM][log_write_idx++] = c;
        logline_ptrs[DISPLAY_CONSOLE_LOG_LINE_NUM][log_write_idx]   = 0;
        console_log_needs_redraw                                    = true;
        console_has_redrawn                                         = false;
    }
}

void display_rotate_screen(bool clockwise, bool is_left) {
#ifdef QUANTUM_PAINTER_ENABLE
    uint8_t temp_rotation =
        is_left ? userspace_config.display.painter.left.rotation : userspace_config.display.painter.right.rotation;
#else
    uint8_t temp_rotation = userspace_config.display.oled.rotation;
#endif // QUANTUM_PAINTER_ENABLE
#if defined(COMMUNITY_MODULE_DISPLAY_MENU_ENABLE)
    display_menu_set_dirty(true);
#endif // COMMUNITY_MODULE_DISPLAY_MENU_ENABLE
#if defined(DISPLAY_FULL_ROTATION_ENABLE)
    if (clockwise) {
        temp_rotation = (temp_rotation + 1) % 4;
        if (temp_rotation > 3) {
            temp_rotation = 0;
        }
    } else {
        temp_rotation = (temp_rotation - 1) % 4;
        if (temp_rotation > 3) {
            temp_rotation = 3;
        }
    }
#else
    temp_rotation = !temp_rotation;
#endif
#ifdef QUANTUM_PAINTER_ENABLE
    if (is_left) {
        userspace_config.display.painter.left.rotation = temp_rotation;
    } else {
        userspace_config.display.painter.right.rotation = temp_rotation;
    }
#else
    userspace_config.display.oled.rotation = temp_rotation;
#endif // QUANTUM_PAINTER_ENABLE
    eeconfig_update_user_datablock(&userspace_config, 0, EECONFIG_USER_DATA_SIZE);
#ifdef QUANTUM_PAINTER_ILI9341_ENABLE
    init_display_ili9341_rotation();
#endif // QUANTUM_PAINTER_ILI9341_ENABLE
#ifdef QUANTUM_PAINTER_ILI9488_ENABLE
    init_display_ili9488_rotation();
#endif // QUANTUM_PAINTER_ILI9341_ENABLE
#ifdef QUANTUM_PAINTER_SH1107_128X128_ENABLE
    init_display_sh1107_128x128_rotation();
#endif // QUANTUM_PAINTER_SH1107_128X128_ENABLE
#ifdef OLED_ENABLE
    oled_post_init();
#endif // OLED_ENABLE;
}

bool display_menu_set_dirty_user(bool state) {
    userspace_runtime_state.display.menu_state_runtime.dirty        = state;
    userspace_runtime_state.display.menu_state_runtime.has_rendered = !state;
    return true;
}

#if defined(COMMUNITY_MODULE_DISPLAY_MENU_ENABLE)
bool process_record_display_menu_handling_user(uint16_t keycode, bool keep_processing) {
    const bool is_qwerty  = get_highest_layer(default_layer_state) == _QWERTY,
               is_dvorak  = get_highest_layer(default_layer_state) == _DVORAK,
               is_colemak = get_highest_layer(default_layer_state) == _COLEMAK ||
                            get_highest_layer(default_layer_state) == _COLEMAK_DH;

    switch (keycode) {
        case KC_D:
            if (is_qwerty) {
                return menu_handle_input(menu_input_down);
            }
            return keep_processing;
        case KC_E:
            if (is_qwerty) {
                return menu_handle_input(menu_input_up);
            } else if (is_dvorak) {
                return menu_handle_input(menu_input_down);
            }
            return keep_processing;
        case KC_F:
            if (is_qwerty) {
                return menu_handle_input(menu_input_right);
            } else if (is_colemak) {
                return menu_handle_input(menu_input_up);
            }
            return keep_processing;
        case KC_O:
            if (is_dvorak) {
                return menu_handle_input(menu_input_left);
            }
            return keep_processing;
        case KC_R:
            if (is_colemak) {
                return menu_handle_input(menu_input_left);
            }
            return keep_processing;
        case KC_S:
            if (is_qwerty) {
                return menu_handle_input(menu_input_left);
            } else if (is_colemak) {
                return menu_handle_input(menu_input_down);
            }
            return keep_processing;
        case KC_T:
            if (is_colemak) {
                return menu_handle_input(menu_input_right);
            }
            return keep_processing;
        case KC_U:
            if (is_dvorak) {
                return menu_handle_input(menu_input_right);
            }
            return keep_processing;
        case KC_DOT:
            if (is_dvorak) {
                return menu_handle_input(menu_input_right);
            }
            return keep_processing;
        case DISPLAY_MENU:
            return menu_handle_input(menu_input_exit);
        case KC_ESC:
        case KC_BSPC:
        case KC_DEL:
            return menu_handle_input(menu_input_back);
        case KC_SPACE:
        case KC_ENTER:
        case KC_RETURN:
            return menu_handle_input(menu_input_enter);
        case KC_UP:
            return menu_handle_input(menu_input_up);
        case KC_DOWN:
            return menu_handle_input(menu_input_down);
        case KC_LEFT:
            return menu_handle_input(menu_input_left);
        case KC_RIGHT:
            return menu_handle_input(menu_input_right);
        default:
            return keep_processing;
    }
}
#endif
