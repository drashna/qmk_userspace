////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// User Settings

bool menu_handler_overwatch_mode(menu_input_t input) {
    switch (input) {
        case menu_input_left:
        case menu_input_right:
        case menu_input_enter:
            userspace_config.gaming.is_overwatch = !userspace_config.gaming.is_overwatch;
            eeconfig_update_user_datablock(&userspace_config, 0, EECONFIG_USER_DATA_SIZE);
            return false;
        default:
            return true;
    }
}

__attribute__((weak)) void display_handler_overwatch_mode(char *text_buffer, size_t buffer_len) {
    snprintf(text_buffer, buffer_len - 1, "%s", userspace_config.gaming.is_overwatch ? "on" : "off");
}

bool menu_handler_gamepad_swap(menu_input_t input) {
    switch (input) {
        case menu_input_left:
        case menu_input_right:
        case menu_input_enter:
            userspace_config.gaming.swapped_numbers = !userspace_config.gaming.swapped_numbers;
            eeconfig_update_user_datablock(&userspace_config, 0, EECONFIG_USER_DATA_SIZE);
            return false;
        default:
            return true;
    }
}

__attribute__((weak)) void display_handler_gamepad_swap(char *text_buffer, size_t buffer_len) {
    snprintf(text_buffer, buffer_len - 1, "%s", userspace_config.gaming.swapped_numbers ? "swapped" : "normal");
}

bool menu_handler_clap_trap(menu_input_t input) {
    switch (input) {
        case menu_input_left:
        case menu_input_right:
        case menu_input_enter:
            userspace_config.gaming.clap_trap_enable = !userspace_config.gaming.clap_trap_enable;
            eeconfig_update_user_datablock(&userspace_config, 0, EECONFIG_USER_DATA_SIZE);
            return false;
        default:
            return true;
    }
}

__attribute__((weak)) void display_handler_clap_trap(char *text_buffer, size_t buffer_len) {
    snprintf(text_buffer, buffer_len - 1, "%s", userspace_config.gaming.clap_trap_enable ? "on" : "off");
}

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

menu_entry_t user_settings_option_entries[] = {
#ifdef COMMUNITY_MODULE_DYNAMIC_DEBOUNCE_ENABLE
    MENU_ENTRY_MULTI("Dynamic Debounce", "DYN DB", dynamic_debounce_menu, dynamic_debounce),
#endif
    MENU_ENTRY_CHILD("Overwatch Mode", "OW", overwatch_mode),
    MENU_ENTRY_CHILD("Gamepad 1<->2 Swap", "1-2 SWP", gamepad_swap),
    MENU_ENTRY_CHILD("SOCD Cleaner", "SOCD", clap_trap),
};
