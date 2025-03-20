////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Pointing Device

#ifdef POINTING_DEVICE_ENABLE
#    include "pointing/pointing.h"

#    ifdef POINTING_DEVICE_AUTO_MOUSE_ENABLE
bool menu_handler_auto_mouse_enable(menu_input_t input) {
    switch (input) {
        case menu_input_left:
        case menu_input_right:
        case menu_input_enter:
            userspace_config.pointing.auto_mouse_layer.enable = !userspace_config.pointing.auto_mouse_layer.enable;
            eeconfig_update_user_datablock_handler(&userspace_config, 0, EECONFIG_USER_DATA_SIZE);
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
            eeconfig_update_user_datablock_handler(&userspace_config, 0, EECONFIG_USER_DATA_SIZE);
            return false;
        case menu_input_right:
        case menu_input_enter:
            userspace_config.pointing.auto_mouse_layer.layer =
                (userspace_config.pointing.auto_mouse_layer.layer + 1) % MAX_USER_LAYERS;
            set_auto_mouse_layer(userspace_config.pointing.auto_mouse_layer.layer);
            eeconfig_update_user_datablock_handler(&userspace_config, 0, EECONFIG_USER_DATA_SIZE);
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
            eeconfig_update_user_datablock_handler(&userspace_config, 0, EECONFIG_USER_DATA_SIZE);
            return false;
        case menu_input_right:
        case menu_input_enter:
            userspace_config.pointing.auto_mouse_layer.timeout =
                (userspace_config.pointing.auto_mouse_layer.timeout + 10);
            set_auto_mouse_timeout(userspace_config.pointing.auto_mouse_layer.timeout);
            eeconfig_update_user_datablock_handler(&userspace_config, 0, EECONFIG_USER_DATA_SIZE);
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
            eeconfig_update_user_datablock_handler(&userspace_config, 0, EECONFIG_USER_DATA_SIZE);
            return false;
        case menu_input_right:
        case menu_input_enter:
            userspace_config.pointing.auto_mouse_layer.debounce =
                (userspace_config.pointing.auto_mouse_layer.debounce + 1);
            set_auto_mouse_debounce(userspace_config.pointing.auto_mouse_layer.debounce);
            eeconfig_update_user_datablock_handler(&userspace_config, 0, EECONFIG_USER_DATA_SIZE);
            return false;
        default:
            return true;
    }
}

__attribute__((weak)) void display_handler_auto_mouse_debounce(char *text_buffer, size_t buffer_len) {
    snprintf(text_buffer, buffer_len - 1, "%d", userspace_config.pointing.auto_mouse_layer.debounce);
}
#    endif // POINTING_DEVICE_AUTO_MOUSE_ENABLE

bool menu_handler_mouse_jiggler(menu_input_t input) {
    switch (input) {
        case menu_input_left:
        case menu_input_right:
        case menu_input_enter:
            pointing_device_mouse_jiggler_toggle();
            return false;
        default:
            return true;
    }
}

__attribute__((weak)) void display_handler_mouse_jiggler(char *text_buffer, size_t buffer_len) {
    snprintf(text_buffer, buffer_len - 1, "%s", userspace_config.pointing.mouse_jiggler.enable ? "on" : "off");
}

bool menu_handler_mouse_jiggler_timeout(menu_input_t input) {
    switch (input) {
        case menu_input_left:
            if (userspace_config.pointing.mouse_jiggler.timeout != 0) {
                userspace_config.pointing.mouse_jiggler.timeout--;
            }
            eeconfig_update_user_datablock_handler(&userspace_config, 0, EECONFIG_USER_DATA_SIZE);
            return false;
        case menu_input_right:
        case menu_input_enter:
            if (userspace_config.pointing.mouse_jiggler.timeout != 255) {
                userspace_config.pointing.mouse_jiggler.timeout++;
            }
            eeconfig_update_user_datablock_handler(&userspace_config, 0, EECONFIG_USER_DATA_SIZE);
            return false;
        default:
            return true;
    }
}

__attribute__((weak)) void display_handler_mouse_jiggler_timeout(char *text_buffer, size_t buffer_len) {
    snprintf(text_buffer, buffer_len - 1, "%d", userspace_config.pointing.mouse_jiggler.timeout);
}

#    if defined(KEYBOARD_handwired_tractyl_manuform) || defined(KEYBOARD_bastardkb_charybdis)
#        include QMK_KEYBOARD_H

bool menu_handler_dpi_config(menu_input_t input) {
    switch (input) {
        case menu_input_left:
            charybdis_cycle_pointer_default_dpi(false);
            return false;
        case menu_input_right:
        case menu_input_enter:
            charybdis_cycle_pointer_default_dpi(true);
            return false;
        default:
            return true;
    }
}

__attribute__((weak)) void display_handler_dpi_config(char *text_buffer, size_t buffer_len) {
    snprintf(text_buffer, buffer_len - 1, "%d", charybdis_get_pointer_default_dpi());
}
#    endif

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
    MENU_ENTRY_CHILD("Acceleration", "Accel", mouse_accel_toggle),
    MENU_ENTRY_CHILD("Takeoff", "Takeoff", mouse_accel_takeoff),
    MENU_ENTRY_CHILD("Growth Rate", "Growth", mouse_accel_growth_rate),
    MENU_ENTRY_CHILD("Offset", "Offset", mouse_accel_offset),
    MENU_ENTRY_CHILD("Limit", "Limit", mouse_accel_limit),
};
#    endif // COMMUNITY_MODULE_POINTING_DEVICE_ACCEL_ENABLE

menu_entry_t pointing_auto_layer_entries[] = {
    MENU_ENTRY_CHILD("Layer", "Layer", auto_mouse_layer),
    MENU_ENTRY_CHILD("Timeout", "Timeout", auto_mouse_timeout),
    MENU_ENTRY_CHILD("Debounce", "Debounce", auto_mouse_debounce),
};

menu_entry_t pointing_entries[] = {
#    ifdef COMMUNITY_MODULE_POINTING_DEVICE_ACCEL_ENABLE
    MENU_ENTRY_MULTI("Mouse Acceleration", "Accel", pointing_acceleration_entries, mouse_accel_toggle),
#    endif // COMMUNITY_MODULE_POINTING_DEVICE_ACCEL_ENABLE
#    if defined(KEYBOARD_handwired_tractyl_manuform) || defined(KEYBOARD_bastardkb_charybdis)
    MENU_ENTRY_CHILD("DPI Config", "DPI", dpi_config),
#    endif // KEYBOARD_handwired_tractyl_manuform || KEYBOARD_bastardkb_charybdis
#    ifdef POINTING_DEVICE_AUTO_MOUSE_ENABLE
    MENU_ENTRY_CHILD("Auto Mouse", "AutoMouse", auto_mouse_enable),
    MENU_ENTRY_MULTI("Auto Mouse Options", "AM Opt", pointing_auto_layer_entries, auto_mouse_layer),
#    endif // POINTING_DEVICE_AUTO_MOUSE_ENABLE
    MENU_ENTRY_CHILD("Mouse Jiggler", "Jiggler", mouse_jiggler),
    MENU_ENTRY_CHILD("Mouse Jiggler Timeout", "JiggleTime", mouse_jiggler_timeout),
#    ifdef AUDIO_ENABLE
    MENU_ENTRY_CHILD("Mouse Clicky", "Clicky", audio_mouse_clicky),
#    endif
};
#endif // POINTING_DEVICE_ENABLE
