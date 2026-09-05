////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Pointing Device

#ifdef POINTING_DEVICE_ENABLE
#    include "pointing/pointing.h"
#    ifdef COMMUNITY_MODULE_MOUSE_JIGGLER_ENABLE
#        include "mouse_jiggler.h"
#    endif // COMMUNITY_MODULE_MOUSE_JIGGLER_ENABLE

#    ifdef POINTING_DEVICE_AUTO_MOUSE_ENABLE
bool menu_handler_auto_mouse_enable(menu_input_t input) {
    switch (input) {
        case menu_input_left:
        case menu_input_right:
        case menu_input_enter:
            userspace_config.pointing.auto_mouse_layer.enable = !userspace_config.pointing.auto_mouse_layer.enable;
            eeconfig_update_user_datablock(&userspace_config, 0, EECONFIG_USER_DATA_SIZE);
            set_auto_mouse_enable(userspace_config.pointing.auto_mouse_layer.enable);
            return false;
        default:
            return true;
    }
}

__attribute__((weak)) void display_handler_auto_mouse_enable(char *text_buffer, size_t buffer_len) {
    snprintf(text_buffer, buffer_len - 1, "%s",
             userspace_config.pointing.auto_mouse_layer.enable ? "enabled" : "disabled");
}

bool menu_handler_auto_mouse_layer(menu_input_t input) {
    switch (input) {
        case menu_input_left:
            userspace_config.pointing.auto_mouse_layer.layer =
                (userspace_config.pointing.auto_mouse_layer.layer - 1) % MAX_USER_LAYERS;
            set_auto_mouse_layer(userspace_config.pointing.auto_mouse_layer.layer);
            eeconfig_update_user_datablock(&userspace_config, 0, EECONFIG_USER_DATA_SIZE);
            return false;
        case menu_input_right:
        case menu_input_enter:
            userspace_config.pointing.auto_mouse_layer.layer =
                (userspace_config.pointing.auto_mouse_layer.layer + 1) % MAX_USER_LAYERS;
            set_auto_mouse_layer(userspace_config.pointing.auto_mouse_layer.layer);
            eeconfig_update_user_datablock(&userspace_config, 0, EECONFIG_USER_DATA_SIZE);
            return false;
        default:
            return true;
    }
}

__attribute__((weak)) void display_handler_auto_mouse_layer(char *text_buffer, size_t buffer_len) {
    snprintf(text_buffer, buffer_len - 1, "%s",
             get_layer_name_string(userspace_config.pointing.auto_mouse_layer.layer, false, true));
}

bool menu_handler_auto_mouse_timeout(menu_input_t input) {
    switch (input) {
        case menu_input_left:
            userspace_config.pointing.auto_mouse_layer.timeout =
                (userspace_config.pointing.auto_mouse_layer.timeout - 10);
            set_auto_mouse_timeout(userspace_config.pointing.auto_mouse_layer.timeout);
            eeconfig_update_user_datablock(&userspace_config, 0, EECONFIG_USER_DATA_SIZE);
            return false;
        case menu_input_right:
        case menu_input_enter:
            userspace_config.pointing.auto_mouse_layer.timeout =
                (userspace_config.pointing.auto_mouse_layer.timeout + 10);
            set_auto_mouse_timeout(userspace_config.pointing.auto_mouse_layer.timeout);
            eeconfig_update_user_datablock(&userspace_config, 0, EECONFIG_USER_DATA_SIZE);
            return false;
        default:
            return true;
    }
}

__attribute__((weak)) void display_handler_auto_mouse_timeout(char *text_buffer, size_t buffer_len) {
    snprintf(text_buffer, buffer_len - 1, "%d", userspace_config.pointing.auto_mouse_layer.timeout);
}

bool menu_handler_auto_mouse_debounce(menu_input_t input) {
    switch (input) {
        case menu_input_left:
            userspace_config.pointing.auto_mouse_layer.debounce =
                (userspace_config.pointing.auto_mouse_layer.debounce - 1);
            set_auto_mouse_debounce(userspace_config.pointing.auto_mouse_layer.debounce);
            eeconfig_update_user_datablock(&userspace_config, 0, EECONFIG_USER_DATA_SIZE);
            return false;
        case menu_input_right:
        case menu_input_enter:
            userspace_config.pointing.auto_mouse_layer.debounce =
                (userspace_config.pointing.auto_mouse_layer.debounce + 1);
            set_auto_mouse_debounce(userspace_config.pointing.auto_mouse_layer.debounce);
            eeconfig_update_user_datablock(&userspace_config, 0, EECONFIG_USER_DATA_SIZE);
            return false;
        default:
            return true;
    }
}

__attribute__((weak)) void display_handler_auto_mouse_debounce(char *text_buffer, size_t buffer_len) {
    snprintf(text_buffer, buffer_len - 1, "%d", userspace_config.pointing.auto_mouse_layer.debounce);
}
#    endif // POINTING_DEVICE_AUTO_MOUSE_ENABLE

#    ifdef COMMUNITY_MODULE_MOUSE_JIGGLER_ENABLE
bool menu_handler_mouse_jiggler(menu_input_t input) {
    switch (input) {
        case menu_input_left:
        case menu_input_right:
        case menu_input_enter:
            jiggler_toggle();
            return false;
        default:
            return true;
    }
}

__attribute__((weak)) void display_handler_mouse_jiggler(char *text_buffer, size_t buffer_len) {
    snprintf(text_buffer, buffer_len - 1, "%s", jiggler_get_enabled() ? "on" : "off");
}

bool menu_handler_mouse_jiggler_timeout(menu_input_t input) {
    switch (input) {
        case menu_input_left:
            jiggler_backoff_decrease();
            return false;
        case menu_input_right:
        case menu_input_enter:
            jiggler_backoff_increase();
            return false;
        default:
            return true;
    }
}

__attribute__((weak)) void display_handler_mouse_jiggler_timeout(char *text_buffer, size_t buffer_len) {
    snprintf(text_buffer, buffer_len - 1, "%d", (uint16_t)jiggler_get_backoff());
}

bool menu_handler_mouse_jiggler_pattern(menu_input_t input) {
    switch (input) {
        case menu_input_left:
            jiggler_pattern_prev();
            return false;
        case menu_input_right:
        case menu_input_enter:
            jiggler_pattern_next();
            return false;
        default:
            return true;
    }
}

__attribute__((weak)) void display_handler_mouse_jiggler_pattern(char *text_buffer, size_t buffer_len) {
    snprintf(text_buffer, buffer_len - 1, "%s", mouse_jiggler_get_name(jiggler_get_pattern()));
}

bool menu_handler_mouse_jiggler_intro(menu_input_t input) {
    switch (input) {
        case menu_input_left:
            jiggler_pattern_intro_prev();
            return false;
        case menu_input_right:
        case menu_input_enter:
            jiggler_pattern_intro_next();
            return false;
        default:
            return true;
    }
}

__attribute__((weak)) void display_handler_mouse_jiggler_intro(char *text_buffer, size_t buffer_len) {
    snprintf(text_buffer, buffer_len - 1, "%s", mouse_jiggler_get_name(jiggler_get_pattern_intro()));
}

bool menu_handler_mouse_jiggler_autostop(menu_input_t input) {
    switch (input) {
        case menu_input_left:
        case menu_input_right:
        case menu_input_enter:
            jiggler_set_autostop(!jiggler_get_autostop());
            return false;
        default:
            return true;
    }
}

__attribute__((weak)) void display_handler_mouse_jiggler_autostop(char *text_buffer, size_t buffer_len) {
    snprintf(text_buffer, buffer_len - 1, "%s", jiggler_get_autostop() ? "on" : "off");
}

bool menu_handler_mouse_jiggler_ending(menu_input_t input) {
    switch (input) {
        case menu_input_left:
            jiggler_pattern_ending_prev();
            return false;
        case menu_input_right:
        case menu_input_enter:
            jiggler_pattern_ending_next();
            return false;
        default:
            return true;
    }
}

__attribute__((weak)) void display_handler_mouse_jiggler_ending(char *text_buffer, size_t buffer_len) {
    snprintf(text_buffer, buffer_len - 1, "%s", mouse_jiggler_get_name(jiggler_get_pattern_ending()));
}

#    endif

#    if defined(COMMUNITY_MODULE_TRACTYL_ENABLE)
#        include "tractyl.h"

bool menu_handler_dpi_config(menu_input_t input) {
    switch (input) {
        case menu_input_left:
            tractyl_cycle_pointer_default_dpi(false);
            return false;
        case menu_input_right:
        case menu_input_enter:
            tractyl_cycle_pointer_default_dpi(true);
            return false;
        default:
            return true;
    }
}

__attribute__((weak)) void display_handler_dpi_config(char *text_buffer, size_t buffer_len) {
    snprintf(text_buffer, buffer_len - 1, "%d", tractyl_get_pointer_default_dpi());
}

bool menu_handler_sniping_dpi_config(menu_input_t input) {
    switch (input) {
        case menu_input_left:
            tractyl_cycle_pointer_sniping_dpi(false);
            return false;
        case menu_input_right:
        case menu_input_enter:
            tractyl_cycle_pointer_sniping_dpi(true);
            return false;
        default:
            return true;
    }
}

__attribute__((weak)) void display_handler_sniping_dpi_config(char *text_buffer, size_t buffer_len) {
    snprintf(text_buffer, buffer_len - 1, "%d", tractyl_get_pointer_sniping_dpi());
}
#    endif // defined(KEYBOARD_handwired_tractyl_manuform) || defined(KEYBOARD_bastardkb_charybdis) ||
           // defined(COMMUNITY_MODULE_TRACTYL_ENABLE)

#    ifdef COMMUNITY_MODULE_POINTING_DEVICE_ACCEL_ENABLE
#        include "pointing_device_accel.h"
bool menu_handler_mouse_accel_toggle(menu_input_t input) {
    switch (input) {
        case menu_input_left:
        case menu_input_right:
        case menu_input_enter:
            pointing_device_accel_toggle_enabled();
            return false;
        default:
            return true;
    }
}

__attribute__((weak)) void display_handler_mouse_accel_toggle(char *text_buffer, size_t buffer_len) {
    snprintf(text_buffer, buffer_len - 1, "%s", pointing_device_accel_get_enabled() ? "on" : "off");
}

bool menu_handler_mouse_accel_takeoff(menu_input_t input) {
    switch (input) {
        case menu_input_left:
            pointing_device_accel_set_takeoff(pointing_device_accel_get_takeoff() -
                                              pointing_device_accel_get_mod_step(POINTING_DEVICE_ACCEL_TAKEOFF_STEP));
            return false;
        case menu_input_right:
        case menu_input_enter:
            pointing_device_accel_set_takeoff(pointing_device_accel_get_takeoff() +
                                              pointing_device_accel_get_mod_step(POINTING_DEVICE_ACCEL_TAKEOFF_STEP));
            return false;
        default:
            return true;
    }
}

__attribute__((weak)) void display_handler_mouse_accel_takeoff(char *text_buffer, size_t buffer_len) {
    snprintf(text_buffer, buffer_len - 1, "%1.2f", pointing_device_accel_get_takeoff());
}

bool menu_handler_mouse_accel_growth_rate(menu_input_t input) {
    switch (input) {
        case menu_input_left:
            pointing_device_accel_set_growth_rate(
                pointing_device_accel_get_growth_rate() -
                pointing_device_accel_get_mod_step(POINTING_DEVICE_ACCEL_GROWTH_RATE_STEP));
            return false;
        case menu_input_right:
        case menu_input_enter:
            pointing_device_accel_set_growth_rate(
                pointing_device_accel_get_growth_rate() +
                pointing_device_accel_get_mod_step(POINTING_DEVICE_ACCEL_GROWTH_RATE_STEP));
            return false;
        default:
            return true;
    }
}

__attribute__((weak)) void display_handler_mouse_accel_growth_rate(char *text_buffer, size_t buffer_len) {
    snprintf(text_buffer, buffer_len - 1, "%1.2f", pointing_device_accel_get_growth_rate());
}

bool menu_handler_mouse_accel_offset(menu_input_t input) {
    switch (input) {
        case menu_input_left:
            pointing_device_accel_set_offset(pointing_device_accel_get_offset() -
                                             pointing_device_accel_get_mod_step(POINTING_DEVICE_ACCEL_OFFSET_STEP));
            return false;
        case menu_input_right:
        case menu_input_enter:
            pointing_device_accel_set_offset(pointing_device_accel_get_offset() +
                                             pointing_device_accel_get_mod_step(POINTING_DEVICE_ACCEL_OFFSET_STEP));
            return false;
        default:
            return true;
    }
}

__attribute__((weak)) void display_handler_mouse_accel_offset(char *text_buffer, size_t buffer_len) {
    snprintf(text_buffer, buffer_len - 1, "%2.2f", pointing_device_accel_get_offset());
}

bool menu_handler_mouse_accel_limit(menu_input_t input) {
    switch (input) {
        case menu_input_left:
            pointing_device_accel_set_limit(pointing_device_accel_get_limit() -
                                            pointing_device_accel_get_mod_step(POINTING_DEVICE_ACCEL_LIMIT_STEP));
            return false;
        case menu_input_right:
        case menu_input_enter:
            pointing_device_accel_set_limit(pointing_device_accel_get_limit() +
                                            pointing_device_accel_get_mod_step(POINTING_DEVICE_ACCEL_LIMIT_STEP));
            return false;
        default:
            return true;
    }
}

__attribute__((weak)) void display_handler_mouse_accel_limit(char *text_buffer, size_t buffer_len) {
    snprintf(text_buffer, buffer_len - 1, "%1.2f", pointing_device_accel_get_limit());
}
menu_entry_t pointing_acceleration_entries[] = {
    DISPLAY_MENU_ENTRY_CHILD("Acceleration", "Accel", mouse_accel_toggle),
    DISPLAY_MENU_ENTRY_CHILD("Takeoff", "Takeoff", mouse_accel_takeoff),
    DISPLAY_MENU_ENTRY_CHILD("Growth Rate", "Growth", mouse_accel_growth_rate),
    DISPLAY_MENU_ENTRY_CHILD("Offset", "Offset", mouse_accel_offset),
    DISPLAY_MENU_ENTRY_CHILD("Limit", "Limit", mouse_accel_limit),
};
#    endif // COMMUNITY_MODULE_POINTING_DEVICE_ACCEL_ENABLE

#    ifdef POINTING_DEVICE_AUTO_MOUSE_ENABLE
menu_entry_t pointing_auto_layer_entries[] = {
    DISPLAY_MENU_ENTRY_CHILD("Layer", "Layer", auto_mouse_layer),
    DISPLAY_MENU_ENTRY_CHILD("Timeout", "Timeout", auto_mouse_timeout),
    DISPLAY_MENU_ENTRY_CHILD("Debounce", "Debounce", auto_mouse_debounce),
};
#    endif

#    ifdef COMMUNITY_MODULE_POINTING_DEVICE_SMOOTHING_ENABLE
#        include "pointing_device_smoothing.h"
bool menu_handler_mouse_smoothing(menu_input_t input) {
    switch (input) {
        case menu_input_left:
        case menu_input_right:
        case menu_input_enter:
            pointing_device_smoothing_toggle_enabled();
            return false;
        default:
            return true;
    }
}

__attribute__((weak)) void display_handler_mouse_smoothing(char *text_buffer, size_t buffer_len) {
    snprintf(text_buffer, buffer_len - 1, "%s", pointing_device_smoothing_get_enabled() ? "on" : "off");
}
#    endif

#    ifdef COMMUNITY_MODULE_MOUSE_JIGGLER_ENABLE
menu_entry_t pointing_mouse_jiggler[] = {
    DISPLAY_MENU_ENTRY_CHILD("Enabled:", "Jiggler", mouse_jiggler),
    DISPLAY_MENU_ENTRY_CHILD("Pattern", "Pattern", mouse_jiggler_pattern),
    DISPLAY_MENU_ENTRY_CHILD("Timeout", "Timeout", mouse_jiggler_timeout),
    DISPLAY_MENU_ENTRY_CHILD("Autostop", "Autostop", mouse_jiggler_autostop),
    DISPLAY_MENU_ENTRY_CHILD("Intro Pattern", "Intro", mouse_jiggler_intro),
    DISPLAY_MENU_ENTRY_CHILD("Ending Pattern", "Ending", mouse_jiggler_ending),
};
#    endif // COMMUNITY_MODULE_MOUSE_JIGGLER_ENABLE

menu_entry_t pointing_entries[] = {
#    ifdef COMMUNITY_MODULE_POINTING_DEVICE_ACCEL_ENABLE
    DISPLAY_MENU_ENTRY_MULTI("Mouse Acceleration", "Accel", pointing_acceleration_entries, NULL, mouse_accel_toggle),
#    endif // COMMUNITY_MODULE_POINTING_DEVICE_ACCEL_ENABLE
#    ifdef COMMUNITY_MODULE_MOUSE_JIGGLER_ENABLE
    DISPLAY_MENU_ENTRY_MULTI("Mouse Jiggler", "Jiggler", pointing_mouse_jiggler, NULL, mouse_jiggler),
#    endif // COMMUNITY_MODULE_MOUSE_JIGGLER_ENABLE
#    if defined(COMMUNITY_MODULE_TRACTYL_ENABLE)
    DISPLAY_MENU_ENTRY_CHILD("DPI Config", "DPI", dpi_config),
    DISPLAY_MENU_ENTRY_CHILD("Sniping DPI Config", "Sniping DPI", sniping_dpi_config),
#    endif // KEYBOARD_handwired_tractyl_manuform || KEYBOARD_bastardkb_charybdis
#    ifdef POINTING_DEVICE_AUTO_MOUSE_ENABLE
    DISPLAY_MENU_ENTRY_CHILD("Auto Mouse", "AutoMouse", auto_mouse_enable),
    DISPLAY_MENU_ENTRY_MULTI("Auto Mouse Options", "AM Opt", pointing_auto_layer_entries, NULL, auto_mouse_layer),
#    endif // POINTING_DEVICE_AUTO_MOUSE_ENABLE
#    ifdef AUDIO_ENABLE
    DISPLAY_MENU_ENTRY_CHILD("Mouse Clicky", "Clicky", audio_mouse_clicky),
#    endif
#    ifdef COMMUNITY_MODULE_POINTING_DEVICE_SMOOTHING_ENABLE
    DISPLAY_MENU_ENTRY_CHILD("Mouse Smoothing", "Smoothing", mouse_smoothing),
#    endif // COMMUNITY_MODULE_POINTING_DEVICE_SMOOTHING_ENABLE
};
#endif // POINTING_DEVICE_ENABLE
