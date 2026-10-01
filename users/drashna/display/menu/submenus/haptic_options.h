
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Haptic

#ifdef HAPTIC_ENABLE
#    include "haptic.h"

extern haptic_config_t haptic_config;

bool menu_handler_haptic_enabled(menu_input_t input) {
    switch (input) {
        case menu_input_left:
        case menu_input_right:
        case menu_input_enter:
            haptic_toggle();
            return false;
        default:
            return true;
    }
}

__attribute__((weak)) void display_handler_haptic_enabled(char *text_buffer, size_t buffer_len) {
    snprintf(text_buffer, buffer_len - 1, "%s", haptic_get_enable() ? "on" : "off");
}

bool menu_handler_haptic_mode(menu_input_t input) {
    if (!haptic_get_enable()) {
        return false;
    }

    switch (input) {
        case menu_input_left:
            haptic_mode_decrease();
            return false;
        case menu_input_right:
        case menu_input_enter:
            haptic_mode_increase();
            return false;
        default:
            return true;
    }
}

__attribute__((weak)) void display_handler_haptic_mode(char *text_buffer, size_t buffer_len) {
    if (haptic_get_enable()) {
        snprintf(text_buffer, buffer_len - 1, "%s", get_haptic_drv2605l_effect_name(haptic_get_mode()));
    } else {
        snprintf(text_buffer, buffer_len - 1, "off");
    }
}

bool menu_handler_feedback_mode(menu_input_t input) {
    if (!haptic_get_enable()) {
        return false;
    }
    switch (input) {
        case menu_input_left:
        case menu_input_right:
        case menu_input_enter:
            haptic_feedback_toggle();
            return false;
        default:
            return true;
    }
}

__attribute__((weak)) void display_handler_feedback_mode(char *text_buffer, size_t buffer_len) {
    if (haptic_get_enable()) {
        switch (haptic_get_feedback()) {
            case 0:
                strncpy(text_buffer, "Press", buffer_len - 1);
                return;
            case 1:
                strncpy(text_buffer, "Both", buffer_len - 1);
                return;
            case 2:
                strncpy(text_buffer, "Release", buffer_len - 1);
                return;
        }
    } else {
        strncpy(text_buffer, "off", buffer_len - 1);
    }
}

#    ifdef HAPTIC_SOLENOID
bool menu_handler_haptic_buzz(menu_input_t input) {
    if (!haptic_get_enable()) {
        return false;
    }
    switch (input) {
        case menu_input_left:
        case menu_input_right:
        case menu_input_enter:
            haptic_buzz_toggle();
            return false;
        default:
            return true;
    }
}

__attribute__((weak)) void display_handler_haptic_buzz(char *text_buffer, size_t buffer_len) {
    snprintf(text_buffer, buffer_len - 1, "%d", haptic_get_buzz());
}

bool menu_handler_haptic_dwell(menu_input_t input) {
    switch (input) {
        case menu_input_left:
            haptic_dwell_decrease();
            return false;
        case menu_input_right:
        case menu_input_enter:
            haptic_dwell_increase();
            return false;
        default:
            return true;
    }
}

__attribute__((weak)) void display_handler_haptic_dwell(char *text_buffer, size_t buffer_len) {
    snprintf(text_buffer, buffer_len - 1, "%d", haptic_get_dwell());
}
#    endif // HAPTIC_SOLENOID

#    ifdef HAPTIC_DRV2605L
bool menu_handler_haptic_love_mode(menu_input_t input) {
    switch (input) {
        case menu_input_left:
        case menu_input_right:
        case menu_input_enter:
            haptic_toggle_continuous();
            return false;
        default:
            return true;
    }
}

__attribute__((weak)) void display_handler_haptic_love_mode(char *text_buffer, size_t buffer_len) {
    snprintf(text_buffer, buffer_len - 1, "%s", haptic_config.cont ? "Aaah! ;)" : "off");
}

bool menu_handler_haptic_love_intensity(menu_input_t input) {
    switch (input) {
        case menu_input_left:
            haptic_cont_increase();
            return false;
        case menu_input_right:
        case menu_input_enter:
            haptic_cont_decrease();
            return false;
        default:
            return true;
    }
}

__attribute__((weak)) void display_handler_haptic_love_intensity(char *text_buffer, size_t buffer_len) {
    if (haptic_config.cont) {
        snprintf(text_buffer, buffer_len - 1, "%d", haptic_config.amplitude);
    } else {
        snprintf(text_buffer, buffer_len - 1, "off");
    }
}
#    endif // HAPTIC_DRV2605L

menu_entry_t haptic_entries[] = {
    DISPLAY_MENU_ENTRY_CHILD("Haptic Enabled", "Enabled", haptic_enabled),
    DISPLAY_MENU_ENTRY_CHILD("Haptic Mode", "Mode", haptic_mode),
    DISPLAY_MENU_ENTRY_CHILD("Feedback Mode", "Key", feedback_mode),
#    ifdef HAPTIC_SOLENOID
    DISPLAY_MENU_ENTRY_CHILD("Buzz", "Buzz", haptic_buzz),
    DISPLAY_MENU_ENTRY_CHILD("Dwell", "Dwell", haptic_dwell),
#    endif // HAPTIC_SOLENOID
#    ifdef HAPTIC_DRV2605L
    DISPLAY_MENU_ENTRY_CHILD("Continuous", "Cont", haptic_love_mode),
    DISPLAY_MENU_ENTRY_CHILD("Continuous Amplitude", "Intensity", haptic_love_intensity),
#    endif // HAPTIC_DRV2605L
};
#endif // HAPTIC_ENABLE
