// Copyright 2018-2024 Nick Brassel (@tzarc)
// Copyright 2024 Drashna (@drashna)
// SPDX-License-Identifier: GPL-2.0-or-later

#include "drashna_runtime.h"
#include "drashna_names.h"
#include "drashna_layers.h"
#include <printf.h>
#include "display/menu/menu.h"
#include "keyrecords/process_records.h"
#include "unicode.h"
#include "deferred_exec.h"

#include "menu.inc"

#ifndef DISPLAY_MENU_TIMEOUT
#    define DISPLAY_MENU_TIMEOUT 30000
#endif // !DISPLAY_MENU_TIMEOUT
deferred_token menu_deferred_token = INVALID_DEFERRED_TOKEN;

menu_entry_t *get_current_menu(void) {
    if (userspace_runtime_state.menu_state.menu_stack[0] == 0xFF) {
        return &root;
    }

    menu_entry_t *entry = &root;
    for (int i = 0; i < sizeof(userspace_runtime_state.menu_state.menu_stack); ++i) {
        if (userspace_runtime_state.menu_state.menu_stack[i] == 0xFF) {
            return entry;
        }
        entry = &entry->parent.children[userspace_runtime_state.menu_state.menu_stack[i]];
    }

    return entry;
}

menu_entry_t *get_selected_menu_item(void) {
    return &(get_current_menu()->parent.children[userspace_runtime_state.menu_state.selected_child]);
}

uint32_t display_menu_timeout_handler(uint32_t trigger_time, void *cb_arg) {
    /* do something */
    menu_handle_input(menu_input_exit);
    return 0;
}

bool menu_handle_input(menu_input_t input) {
    menu_entry_t *menu     = get_current_menu();
    menu_entry_t *selected = get_selected_menu_item();
    if (menu_deferred_token != INVALID_DEFERRED_TOKEN && input != menu_input_exit) {
        extend_deferred_exec(menu_deferred_token, DISPLAY_MENU_TIMEOUT);
    }
    switch (input) {
        case menu_input_exit:
            userspace_runtime_state.menu_state.is_in_menu = false;
            memset(userspace_runtime_state.menu_state.menu_stack, 0xFF,
                   sizeof(userspace_runtime_state.menu_state.menu_stack));
            userspace_runtime_state.menu_state.selected_child = 0xFF;
            if (cancel_deferred_exec(menu_deferred_token)) {
                menu_deferred_token = INVALID_DEFERRED_TOKEN;
            }
            return false;
        case menu_input_back:
            // Iterate backwards through the stack and remove the last entry
            for (uint8_t i = 0; i < sizeof(userspace_runtime_state.menu_state.menu_stack); ++i) {
                if (userspace_runtime_state.menu_state
                        .menu_stack[sizeof(userspace_runtime_state.menu_state.menu_stack) - 1 - i] != 0xFF) {
                    userspace_runtime_state.menu_state.selected_child =
                        userspace_runtime_state.menu_state
                            .menu_stack[sizeof(userspace_runtime_state.menu_state.menu_stack) - 1 - i];
                    userspace_runtime_state.menu_state
                        .menu_stack[sizeof(userspace_runtime_state.menu_state.menu_stack) - 1 - i] = 0xFF;
                    break;
                }

                // If we've dropped out of the last entry in the stack, exit the menu
                if (i == sizeof(userspace_runtime_state.menu_state.menu_stack) - 1) {
                    userspace_runtime_state.menu_state.is_in_menu     = false;
                    userspace_runtime_state.menu_state.selected_child = 0xFF;
                }
            }
            return false;
        case menu_input_enter:
            // Only attempt to enter the next menu if we're a parent object
            if (selected->flags & menu_flag_is_parent) {
                // Iterate forwards through the stack and add the selected entry
                for (uint8_t i = 0; i < sizeof(userspace_runtime_state.menu_state.menu_stack); ++i) {
                    if (userspace_runtime_state.menu_state.menu_stack[i] == 0xFF) {
                        userspace_runtime_state.menu_state.menu_stack[i] =
                            userspace_runtime_state.menu_state.selected_child;
                        userspace_runtime_state.menu_state.selected_child = 0;
                        break;
                    }
                }
            } else if (selected->flags & menu_flag_is_value) {
                userspace_runtime_state.menu_state.dirty = true;
                return selected->child.menu_handler(menu_input_right);
            }

            return false;
        case menu_input_up:
            userspace_runtime_state.menu_state.selected_child =
                (userspace_runtime_state.menu_state.selected_child + menu->parent.child_count - 1) %
                menu->parent.child_count;
            return false;
        case menu_input_down:
            userspace_runtime_state.menu_state.selected_child =
                (userspace_runtime_state.menu_state.selected_child + menu->parent.child_count + 1) %
                menu->parent.child_count;
            return false;
        case menu_input_left:
        case menu_input_right:
            if (selected->flags & menu_flag_is_value) {
                userspace_runtime_state.menu_state.dirty = true;
                return selected->child.menu_handler(input);
            }
            return false;
        default:
            return false;
    }
}

__attribute__((weak)) bool process_record_menu_user(uint16_t keycode, bool keep_processing) {
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

bool process_record_menu(uint16_t keycode, keyrecord_t *record) {
    if (keycode == DISPLAY_MENU && record->event.pressed && !userspace_runtime_state.menu_state.is_in_menu) {
        userspace_runtime_state.menu_state.is_in_menu     = true;
        userspace_runtime_state.menu_state.selected_child = 0;
        menu_deferred_token = defer_exec(DISPLAY_MENU_TIMEOUT, display_menu_timeout_handler, NULL);
        return false;
    }

    bool keep_processing = false;

    switch (keycode) {
        case QK_TO ... QK_TO_MAX:
        case QK_MOMENTARY ... QK_MOMENTARY_MAX:
        case QK_DEF_LAYER ... QK_DEF_LAYER_MAX:
        case QK_TOGGLE_LAYER ... QK_TOGGLE_LAYER_MAX:
        case QK_ONE_SHOT_LAYER ... QK_ONE_SHOT_LAYER_MAX:
        case QK_LAYER_TAP_TOGGLE ... QK_LAYER_TAP_TOGGLE_MAX:
        case QK_SWAP_HANDS ... QK_SWAP_HANDS_MAX:
            keep_processing = true;
            break;
        case QK_LAYER_TAP ... QK_LAYER_TAP_MAX:
            // Exclude hold keycode
            if (!record->tap.count) {
                keep_processing = true;
                break;
            }
            keycode = QK_LAYER_TAP_GET_TAP_KEYCODE(keycode);
            break;
        case QK_MOD_TAP ... QK_MOD_TAP_MAX:
            // Exclude hold keycode
            if (!record->tap.count) {
                keep_processing = false;
                break;
            }
            keycode = QK_MOD_TAP_GET_TAP_KEYCODE(keycode);
            break;
#if defined(POINTING_DEVICE_ENABLE)
        default:
#    if defined(POINTING_DEVICE_AUTO_MOUSE_ENABLE)
            if (IS_MOUSE_KEYCODE(keycode) || is_mouse_record_kb(keycode, record)) {
                keep_processing = true;
            }
#    else  // POINTING_DEVICE_AUTO_MOUSE_ENABLE
            keep_processing = IS_MOUSE_KEYCODE(keycode);
#    endif // POINTING_DEVICE_AUTO_MOUSE_ENABLE
            break;
#endif // POINTING_DEVICE_ENABLE
    }
    if (userspace_runtime_state.menu_state.is_in_menu) {
        if (record->event.pressed) {
            return process_record_menu_user(keycode, keep_processing);
        }
        return keep_processing;
    }

    return true;
}

uint8_t get_menu_scroll_offset(menu_entry_t *menu, uint8_t visible_entries) {
    static uint8_t l_scroll_offset = 0;

    // If the number of entries exceeds the number of visible entries, we need to scroll
    if (menu->parent.child_count > visible_entries) {
        // If the selected child is is at the end of the visible list but we still have entries to scroll from,
        // don't actually select the last one and increase the scroll offset
        if (userspace_runtime_state.menu_state.selected_child >= l_scroll_offset + visible_entries - 1 &&
            userspace_runtime_state.menu_state.selected_child < menu->parent.child_count - 1) {
            l_scroll_offset = userspace_runtime_state.menu_state.selected_child - visible_entries + 2;
        } else if (userspace_runtime_state.menu_state.selected_child < l_scroll_offset + 1) {
            // If the selected child is at the start of the visible list but we still have entries to scroll to,
            // don't actually select the first one and decrease the scroll offset
            if (userspace_runtime_state.menu_state.selected_child != 0) {
                l_scroll_offset = userspace_runtime_state.menu_state.selected_child - 1;
            } else {
                // if first entry is selected, reset scroll offset
                l_scroll_offset = 0;
            }
            // If the selected child is at the end of the visible list and we don't have any more entries to scroll
            // to, then don't increase, but ensure ofset is at the end (for wrapping)
        } else if (userspace_runtime_state.menu_state.selected_child == menu->parent.child_count - 1) {
            l_scroll_offset = menu->parent.child_count - visible_entries;
        }
    } else {
        l_scroll_offset = 0;
    }
    return l_scroll_offset;
}

void display_menu_set_dirty(void) {
    userspace_runtime_state.menu_state.dirty = true;
}
