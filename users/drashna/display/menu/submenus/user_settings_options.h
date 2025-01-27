////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// User Settings

bool menu_handler_overwatch_mode(menu_input_t input) {
    switch (input) {
        case menu_input_left:
        case menu_input_right:
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

menu_entry_t user_settings_option_entries[] = {
    MENU_ENTRY_CHILD("Overwatch Mode", "OW", overwatch_mode),
    MENU_ENTRY_CHILD("Gamepad 1<->2 Swap", "1-2 SWP", gamepad_swap),
    MENU_ENTRY_CHILD("SOCD Cleaner", "SOCD", clap_trap),
};
