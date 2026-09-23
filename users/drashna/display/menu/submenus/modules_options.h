
#if defined(COMMUNITY_MODULE_AUTOCORRECT_ENABLE)
#    include "autocorrect.h"

bool menu_handler_keycode_autocorrect(menu_input_t input) {
    switch (input) {
        case menu_input_left:
        case menu_input_right:
        case menu_input_enter:
            autocorrect_toggle();
            return false;
        default:
            return true;
    }
}

__attribute__((weak)) void display_handler_keycode_autocorrect(char *text_buffer, size_t buffer_len) {
    snprintf(text_buffer, buffer_len - 1, "%s", autocorrect_is_enabled() ? "on" : "off");
}

bool menu_handler_keycode_autocorrect_dict(menu_input_t input) {
    switch (input) {
        case menu_input_left:
            autocorrect_dict_cycle(false);
            return false;
        case menu_input_right:
            autocorrect_dict_cycle(true);
            return false;
        default:
            return true;
    }
}

__attribute__((weak)) void display_handler_keycode_autocorrect_dict(char *text_buffer, size_t buffer_len) {
    uint8_t dict_index = autocorrect_get_current_dictionary();
    snprintf(text_buffer, buffer_len - 1, "Dict %d/%d", dict_index + 1, autocorrect_get_number_of_dictionaries());
}
#endif //  COMMUNITY_MODULE_AUTOCORRECT_ENABLE

#ifdef COMMUNITY_MODULE_CONSOLE_KEYLOGGING_ENABLE
#    include "console_keylogging.h"
bool menu_handler_keylogger(menu_input_t input) {
    switch (input) {
        case menu_input_left:
        case menu_input_right:
            console_keylogging_set_enabled(!console_keylogging_get_enabled());
            return false;
        default:
            return true;
    }
}

__attribute__((weak)) void display_handler_keylogger(char *text_buffer, size_t buffer_len) {
    snprintf(text_buffer, buffer_len - 1, "%s", console_keylogging_get_enabled() ? "on" : "off");
}
#endif // COMMUNITY_MODULE_CONSOLE_KEYLOGGING_ENABLE

#ifdef COMMUNITY_MODULE_DYNAMIC_DEBOUNCE_ENABLE
#    include "dynamic_debounce.h"

bool menu_handler_dynamic_debounce(menu_input_t input) {
    switch (input) {
        case menu_input_left:
            dynamic_debounce_previous_algorithm();
            return false;
        case menu_input_right:
        case menu_input_enter:
            dynamic_debounce_next_algorithm();
            return false;
        default:
            return true;
    }
}

__attribute__((weak)) void display_handler_dynamic_debounce(char *text_buffer, size_t buffer_len) {
    snprintf(text_buffer, buffer_len - 1, "%s", dynamic_debounce_get_algorithm_name(dynamic_debounce_get_algorithm()));
}

#    define DEBOUNCE_ALGORITHM(name, ...)                                                                              \
        bool menu_handler_dynamic_debounce_##name(menu_input_t input) {                                                \
            switch (input) {                                                                                           \
                case menu_input_enter:                                                                                 \
                    dynamic_debounce_set_algorithm(DYNAMIC_DEBOUNCE_##name);                                           \
                    menu_handle_input(menu_input_back);                                                                \
                    return false;                                                                                      \
                default:                                                                                               \
                    return true;                                                                                       \
            }                                                                                                          \
        }                                                                                                              \
        __attribute__((weak)) void display_handler_dynamic_debounce_##name(char *text_buffer, size_t buffer_len) {     \
            snprintf(text_buffer, buffer_len - 1, "%s", dynamic_debounce_get_algorithm_name(DYNAMIC_DEBOUNCE_##name)); \
        }

#    include "debounce.inc"
#    undef DEBOUNCE_ALGORITHM

menu_entry_t dynamic_debounce_algo[] = {
#    define DEBOUNCE_ALGORITHM(name) MENU_ENTRY_CHILD(#name, #name, dynamic_debounce_##name),
#    include "debounce.inc"
#    undef DEBOUNCE_ALGORITHM
};

bool menu_handler_dynamic_debounce_time(menu_input_t input) {
    switch (input) {
        case menu_input_left:
            dynamic_debounce_decrease_time();
            return false;
        case menu_input_right:
        case menu_input_enter:
            dynamic_debounce_increase_time();
            return false;
        default:
            return true;
    }
}

__attribute__((weak)) void display_handler_dynamic_debounce_time(char *text_buffer, size_t buffer_len) {
    snprintf(text_buffer, buffer_len - 1, "%u ms", dynamic_debounce_get_time());
}

menu_entry_t dynamic_debounce_menu[] = {
    MENU_ENTRY_MULTI("Debounce Algorithm", "ALGO", dynamic_debounce_algo, dynamic_debounce),
    MENU_ENTRY_CHILD("Debounce Time", "TIME", dynamic_debounce_time),
};
#endif

#ifdef COMMUNITY_MODULE_GUARDED_RESET_ENABLE
#include "guarded_reset.h"
bool menu_handler_guarded_reset(menu_input_t input) {
    switch (input) {
        case menu_input_left:
            guarded_reset_hold_ms_step(false);
            return false;
        case menu_input_right:
        case menu_input_enter:
            guarded_reset_hold_ms_step(true);
            return false;
        default:
            return true;
    }
}

__attribute__((weak)) void display_handler_guarded_reset(char *text_buffer, size_t buffer_len) {
    snprintf(text_buffer, buffer_len - 1, "%d ms", get_guarded_reset_hold_ms());
}
#endif // COMMUNITY_MODULE_GUARDED_RESET_ENABLE

#ifdef COMMUNITY_MODULE_I2C_SCANNER_ENABLE
#    include "i2c_scanner.h"

bool menu_handler_i2c_scanner(menu_input_t input) {
    switch (input) {
        case menu_input_left:
        case menu_input_right:
        case menu_input_enter:
            i2c_scanner_set_enabled(!i2c_scanner_get_enabled());
            return false;
        default:
            return true;
    }
}

__attribute__((weak)) void display_handler_i2c_scanner(char *text_buffer, size_t buffer_len) {
    snprintf(text_buffer, buffer_len - 1, "%s", i2c_scanner_get_enabled() ? "on" : "off");
}
#endif

menu_entry_t community_modules_settings_option_entries[] = {
#if defined(COMMUNITY_MODULE_AUTOCORRECT_ENABLE)
    MENU_ENTRY_CHILD("Autocorrect", "AutoCorr", keycode_autocorrect),
    MENU_ENTRY_CHILD("Autocorrect Dictionary", "AutoDict", keycode_autocorrect_dict),
#endif // AUTOCORRECT_ENABLE || COMMUNITY_MODULE_AUTOCORRECT_ENABLE
#ifdef COMMUNITY_MODULE_DYNAMIC_DEBOUNCE_ENABLE
    MENU_ENTRY_MULTI("Dynamic Debounce", "DYN DB", dynamic_debounce_menu, dynamic_debounce),
#endif
#ifdef COMMUNITY_MODULE_CONSOLE_KEYLOGGING_ENABLE
    MENU_ENTRY_CHILD("Console Keylogger", "Keylogger", keylogger),
#endif // COMMUNITY_MODULE_CONSOLE_KEYLOGGING_ENABLE
#ifdef COMMUNITY_MODULE_GUARDED_RESET_ENABLE
    MENU_ENTRY_CHILD("Guarded Reset Timeout", "GReset", guarded_reset),
#endif // COMMUNITY_MODULE_GUARDED_RESET_ENABLE
#ifdef COMMUNITY_MODULE_I2C_SCANNER_ENABLE
    MENU_ENTRY_CHILD("I2C Scanner", "I2C Scan", i2c_scanner),
#endif // COMMUNITY_MODULE_I2C_SCANNER_ENABLE
#ifdef COMMUNITY_MODULE_POINTING_DEVICE_ACCEL_ENABLE
    MENU_ENTRY_MULTI("Mouse Acceleration", "Accel", pointing_acceleration_entries, mouse_accel_toggle),
#endif // COMMUNITY_MODULE_POINTING_DEVICE_ACCEL_ENABLE
#ifdef POINTING_DEVICE_MOUSE_JIGGLER_ENABLE
    MENU_ENTRY_CHILD("Mouse Jiggler", "Jiggler", mouse_jiggler),
    MENU_ENTRY_CHILD("Mouse Jiggler Timeout", "JiggleTime", mouse_jiggler_timeout),
#endif // POINTING_DEVICE_MOUSE_JIGGLER_ENABLE
#    ifdef COMMUNITY_MODULE_POINTING_DEVICE_SMOOTHING_ENABLE
    MENU_ENTRY_CHILD("Mouse Smoothing", "Smoothing", mouse_smoothing),
#    endif // COMMUNITY_MODULE_POINTING_DEVICE_SMOOTHING_ENABLE
#if defined(COMMUNITY_MODULE_RTC_ENABLE)
    MENU_ENTRY_PARENT("RTC Settings", "RTC", rtc_config_entries),
#endif // COMMUNITY_MODULE_RTC_ENABLE
#    ifdef COMMUNITY_MODULE_UNICODE_TYPING_ENABLE
    MENU_ENTRY_CHILD("Unicode Typing Mode", "Typing", unicode_typing),
#    endif
};
