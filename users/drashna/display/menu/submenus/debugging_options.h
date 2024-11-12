
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Debugging

bool menu_handler_debugging_enable(menu_input_t input) {
    switch (input) {
        case menu_input_left:
        case menu_input_right:
        case menu_input_enter:
            debug_enable = !debug_enable;
            return false;
        default:
            return true;
    }
}

__attribute__((weak)) void display_handler_debugging_enable(char *text_buffer, size_t buffer_len) {
    snprintf(text_buffer, buffer_len - 1, "%s", debug_enable ? "enabled" : "disabled");
}

bool menu_handler_keyboard_debugging(menu_input_t input) {
    switch (input) {
        case menu_input_left:
        case menu_input_right:
        case menu_input_enter:
            debug_keyboard = !debug_keyboard;
            return false;
        default:
            return true;
    }
}

__attribute__((weak)) void display_handler_keyboard_debugging(char *text_buffer, size_t buffer_len) {
    snprintf(text_buffer, buffer_len - 1, "%s", debug_keyboard ? "enabled" : "disabled");
}

bool menu_handler_matrix_debugging(menu_input_t input) {
    switch (input) {
        case menu_input_left:
        case menu_input_right:
        case menu_input_enter:
            debug_matrix = !debug_matrix;
            return false;
        default:
            return true;
    }
}

__attribute__((weak)) void display_handler_matrix_debugging(char *text_buffer, size_t buffer_len) {
    snprintf(text_buffer, buffer_len - 1, "%s", debug_matrix ? "enabled" : "disabled");
}

bool menu_handler_mouse_debugging(menu_input_t input) {
    switch (input) {
        case menu_input_left:
        case menu_input_right:
        case menu_input_enter:
            debug_mouse = !debug_mouse;
            return false;
        default:
            return true;
    }
}

__attribute__((weak)) void display_handler_mouse_debugging(char *text_buffer, size_t buffer_len) {
    snprintf(text_buffer, buffer_len - 1, "%s", debug_mouse ? "enabled" : "disabled");
}

bool menu_handler_pointing_debugging(menu_input_t input) {
    switch (input) {
        case menu_input_left:
        case menu_input_right:
        case menu_input_enter:
            debug_pointing = !debug_pointing;
            return false;
        default:
            return true;
    }
}

__attribute__((weak)) void display_handler_pointing_debugging(char *text_buffer, size_t buffer_len) {
    snprintf(text_buffer, buffer_len - 1, "%s", debug_pointing ? "enabled" : "disabled");
}

bool menu_handler_action_debugging(menu_input_t input) {
    switch (input) {
        case menu_input_left:
        case menu_input_right:
        case menu_input_enter:
            debug_action = !debug_action;
            return false;
        default:
            return true;
    }
}

__attribute__((weak)) void display_handler_action_debugging(char *text_buffer, size_t buffer_len) {
    snprintf(text_buffer, buffer_len - 1, "%s", debug_action ? "enabled" : "disabled");
}

bool menu_handler_split_serial_debugging(menu_input_t input) {
    switch (input) {
        case menu_input_left:
        case menu_input_right:
        case menu_input_enter:
            debug_serial = !debug_serial;
            return false;
        default:
            return true;
    }
}

__attribute__((weak)) void display_handler_split_serial_debugging(char *text_buffer, size_t buffer_len) {
    snprintf(text_buffer, buffer_len - 1, "%s", debug_serial ? "enabled" : "disabled");
}

bool menu_handler_quantum_painter_debugging(menu_input_t input) {
    switch (input) {
        case menu_input_left:
        case menu_input_right:
        case menu_input_enter:
            debug_quantum_painter = !debug_quantum_painter;
            return false;
        default:
            return true;
    }
}

__attribute__((weak)) void display_handler_quantum_painter_debugging(char *text_buffer, size_t buffer_len) {
    snprintf(text_buffer, buffer_len - 1, "%s", debug_quantum_painter ? "enabled" : "disabled");
}

#ifdef COMMUNITY_MODULE_I2C_SCANNER_ENABLE
#    include "i2c_scanner.h"

bool menu_handler_i2c_scanner(menu_input_t input) {
    switch (input) {
        case menu_input_left:
        case menu_input_right:
        case menu_input_enter:
            userspace_config.debug.i2c_scanner_enable = !userspace_config.debug.i2c_scanner_enable;
            i2c_scanner_set_enabled(userspace_config.debug.i2c_scanner_enable);
            eeconfig_update_user_datablock(&userspace_config, 0, EECONFIG_USER_DATA_SIZE);

            return false;
        default:
            return true;
    }
}

__attribute__((weak)) void display_handler_i2c_scanner(char *text_buffer, size_t buffer_len) {
    snprintf(text_buffer, buffer_len - 1, "%s", userspace_config.debug.i2c_scanner_enable ? "on" : "off");
}
#endif

bool menu_handler_scan_rate(menu_input_t input) {
    switch (input) {
        case menu_input_left:
        case menu_input_right:
        case menu_input_enter:
            userspace_config.debug.matrix_scan_print = !userspace_config.debug.matrix_scan_print;
            eeconfig_update_user_datablock(&userspace_config, 0, EECONFIG_USER_DATA_SIZE);
            return false;
        default:
            return true;
    }
}

__attribute__((weak)) void display_handler_scan_rate(char *text_buffer, size_t buffer_len) {
    snprintf(text_buffer, buffer_len - 1, "%s", userspace_config.debug.matrix_scan_print ? "on" : "off");
}

#ifdef COMMUNITY_MODULE_CONSOLE_KEYLOGGING_ENABLE
#    include "console_keylogging.h"
bool menu_handler_keylogger(menu_input_t input) {
    switch (input) {
        case menu_input_left:
        case menu_input_right:
            userspace_config.debug.console_keylogger = !userspace_config.debug.console_keylogger;
            eeconfig_update_user_datablock(&userspace_config, 0, EECONFIG_USER_DATA_SIZE);
            console_keylogger_set_enabled(userspace_config.debug.console_keylogger);
            return false;
        default:
            return true;
    }
}

__attribute__((weak)) void display_handler_keylogger(char *text_buffer, size_t buffer_len) {
    snprintf(text_buffer, buffer_len - 1, "%s", console_keylogger_get_enabled() ? "on" : "off");
}
#endif // COMMUNITY_MODULE_CONSOLE_KEYLOGGING_ENABLE

menu_entry_t debug_entries[] = {
    MENU_ENTRY_CHILD("Debugging", "Enabled", debugging_enable),
    MENU_ENTRY_CHILD("Keyboard Debugging", "Keeb", keyboard_debugging),
    MENU_ENTRY_CHILD("Matrix Debugging", "Matrix", matrix_debugging),
    MENU_ENTRY_CHILD("Mouse Debugging", "Mouse", mouse_debugging),
    MENU_ENTRY_CHILD("Pointing Device Debugging", "Pointing", pointing_debugging),
    MENU_ENTRY_CHILD("Action Debugging", "Action", action_debugging),
    MENU_ENTRY_CHILD("Split Serial Debugging", "Split", split_serial_debugging),
    MENU_ENTRY_CHILD("Quantum Painter Debugging", "QP????", quantum_painter_debugging),
#ifdef COMMUNITY_MODULE_I2C_SCANNER_ENABLE
    MENU_ENTRY_CHILD("I2C Scanner", "I2C Scan", i2c_scanner),
#endif // COMMUNITY_MODULE_I2C_SCANNER_ENABLE
    MENU_ENTRY_CHILD("Matrix Scan Rate Print", "Scan Rate", scan_rate),
#ifdef COMMUNITY_MODULE_CONSOLE_KEYLOGGING_ENABLE
    MENU_ENTRY_CHILD("Console Keylogger", "Keylogger", keylogger),
#endif // COMMUNITY_MODULE_CONSOLE_KEYLOGGING_ENABLE
};
