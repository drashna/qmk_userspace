
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Backlight

#ifdef BACKLIGHT_ENABLE
bool menu_handler_bl_enabled(menu_input_t input) {
    switch (input) {
        case menu_input_left:
        case menu_input_right:
        case menu_input_enter:
            backlight_toggle();
            return false;
        default:
            return true;
    }
}

__attribute__((weak)) void display_handler_bl_enabled(char *text_buffer, size_t buffer_len) {
    snprintf(text_buffer, buffer_len - 1, "%s", is_backlight_enabled() ? "on" : "off");
}

bool menu_handler_bl_level(menu_input_t input) {
    switch (input) {
        case menu_input_left:
            backlight_decrease();
            return false;
        case menu_input_right:
        case menu_input_enter:
            backlight_increase();
            return false;
        default:
            return true;
    }
}

__attribute__((weak)) void display_handler_bl_level(char *text_buffer, size_t buffer_len) {
    snprintf(text_buffer, buffer_len - 1, "%d", get_backlight_level());
}

#    ifdef BACKLIGHT_BREATHING
bool menu_handler_breathing(menu_input_t input) {
    switch (input) {
        case menu_input_left:
        case menu_input_right:
        case menu_input_enter:
            backlight_toggle_breathing();
            return false;
        default:
            return true;
    }
}

__attribute__((weak)) void display_handler_breathing(char *text_buffer, size_t buffer_len) {
    snprintf(text_buffer, buffer_len - 1, "%s", is_backlight_breathing() ? "on" : "off");
}
#    endif

menu_entry_t backlight_entries[] = {
    DISPLAY_MENU_ENTRY_CHILD("Backlight Enabled", "Enabled", bl_enabled),
    DISPLAY_MENU_ENTRY_CHILD("Backlight Level", "Level", bl_level),
#    ifdef BACKLIGHT_BREATHING
    DISPLAY_MENU_ENTRY_CHILD("Backlight Breathing", "Breathing", breathing),
#    endif // BACKLIGHT_BREATHING
};
#endif // BACKLIGHT_ENABLE
