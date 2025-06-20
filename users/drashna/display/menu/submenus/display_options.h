
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Display options

#ifdef SPLIT_KEYBOARD
bool menu_handler_display_menu_location(menu_input_t input) {
    switch (input) {
        case menu_input_left:
            userspace_config.display.menu_render_side = (userspace_config.display.menu_render_side - 1) % 4;
            if (userspace_config.display.menu_render_side < 1) {
                userspace_config.display.menu_render_side = 3;
            }
            eeconfig_update_user_datablock_handler(&userspace_config, 0, EECONFIG_USER_DATA_SIZE);
            display_menu_set_dirty(true);
            return false;
        case menu_input_right:
        case menu_input_enter:
            userspace_config.display.menu_render_side = (userspace_config.display.menu_render_side + 1) % 4;
            if (userspace_config.display.menu_render_side < 1) {
                userspace_config.display.menu_render_side = 1;
            }
            display_menu_set_dirty(true);
            eeconfig_update_user_datablock_handler(&userspace_config, 0, EECONFIG_USER_DATA_SIZE);
            return false;
        default:
            return true;
    }
}

__attribute__((weak)) void display_handler_display_menu_location(char *text_buffer, size_t buffer_len) {
    switch (userspace_config.display.menu_render_side) {
        case 1:
            strncpy(text_buffer, "Left", buffer_len - 1);
            return;
        case 2:
            strncpy(text_buffer, "Right", buffer_len - 1);
            return;
        case 3:
            strncpy(text_buffer, "Both", buffer_len - 1);
            return;
    }
    strncpy(text_buffer, "Everywhere", buffer_len);
}
#endif // SPLIT_KEYBOARD

bool menu_handler_display_rotation(menu_input_t input) {
    void display_rotate_screen(bool clockwise);

    switch (input) {
        case menu_input_left:
            display_rotate_screen(false);
            return false;
        case menu_input_right:
        case menu_input_enter:
            display_rotate_screen(true);
            return false;
        default:
            return true;
    }
}

__attribute__((weak)) void display_handler_display_rotation(char *text_buffer, size_t buffer_len) {
#if defined(DISPLAY_FULL_ROTATION_ENABLE)
    switch (userspace_config.display.rotation) {
        case 0:
            strncpy(text_buffer, "0", buffer_len - 1);
            return;
        case 1:
            strncpy(text_buffer, "90", buffer_len - 1);
            return;
        case 2:
            strncpy(text_buffer, "180", buffer_len - 1);
            return;
        case 3:
            strncpy(text_buffer, "270", buffer_len - 1);
            return;
    }
#else
    strncpy(text_buffer, userspace_config.display.rotation ? "Flipped" : "Normal", buffer_len - 1);
    return;
#endif

    strncpy(text_buffer, "Unknown", buffer_len);
}

#ifdef QUANTUM_PAINTER_ILI9341_ENABLE
void init_display_ili9341_inversion(void);
#endif // QUANTUM_PAINTER_ILI9341_ENABLE
#ifdef QUANTUM_PAINTER_ILI9488_ENABLE
void init_display_ili9488_inversion(void);
#endif // QUANTUM_PAINTER_ILI9341_ENABLE

bool menu_handler_display_inverted(menu_input_t input) {
    switch (input) {
        case menu_input_left:
        case menu_input_right:
        case menu_input_enter:
            userspace_config.display.inverted = !userspace_config.display.inverted;
            eeconfig_update_user_datablock_handler(&userspace_config, 0, EECONFIG_USER_DATA_SIZE);
#ifdef QUANTUM_PAINTER_ILI9341_ENABLE
            init_display_ili9341_inversion();
#endif // QUANTUM_PAINTER_ILI9341_ENABLE
#ifdef QUANTUM_PAINTER_ILI9488_ENABLE
            init_display_ili9488_inversion();
#endif // QUANTUM_PAINTER_ILI9341_ENABLE
#ifdef OLED_ENABLE
            void oled_post_init(void);
            oled_post_init();
#endif // OLED_ENABLE;
            return false;
        default:
            return true;
    }
}

__attribute__((weak)) void display_handler_display_inverted(char *text_buffer, size_t buffer_len) {
    strncpy(text_buffer, userspace_config.display.inverted ? "Inverted" : "Normal", buffer_len - 1);
}

#if defined(OLED_ENABLE) && defined(CUSTOM_OLED_DRIVER)
void oled_brightness_increase_step(void);
void oled_brightness_decrease_step(void);

bool menu_handler_oled_brightness(menu_input_t input) {
    switch (input) {
        case menu_input_left:
            oled_brightness_decrease_step();
            return false;
        case menu_input_right:
        case menu_input_enter:
            oled_brightness_increase_step();
            return false;
        default:
            return true;
    }
}

__attribute__((weak)) void display_handler_oled_brightness(char *text_buffer, size_t buffer_len) {
    snprintf(text_buffer, buffer_len - 1, "%d", userspace_config.display.oled.brightness);
}

bool menu_handler_oled_lock(menu_input_t input) {
    switch (input) {
        case menu_input_left:
        case menu_input_right:
        case menu_input_enter:
            userspace_config.display.oled.screen_lock = !userspace_config.display.oled.screen_lock;
            eeconfig_update_user_datablock_handler(&userspace_config, 0, EECONFIG_USER_DATA_SIZE);
            return false;
        default:
            return true;
    }
}

__attribute__((weak)) void display_handler_oled_lock(char *text_buffer, size_t buffer_len) {
    strncpy(text_buffer, userspace_config.display.oled.screen_lock ? "Enabled" : "Disabled", buffer_len - 1);
}

bool menu_handler_oled_pet_animation(menu_input_t input) {
    switch (input) {
        case menu_input_left:
            userspace_config.display.oled.pet.index = (userspace_config.display.oled.pet.index - 1) % 3;
            if (userspace_config.display.oled.pet.index > 2) {
                userspace_config.display.oled.pet.index = 2;
            }
            eeconfig_update_user_datablock_handler(&userspace_config, 0, EECONFIG_USER_DATA_SIZE);
            return false;
        case menu_input_right:
        case menu_input_enter:
            userspace_config.display.oled.pet.index = (userspace_config.display.oled.pet.index + 1) % 3;
            if (userspace_config.display.oled.pet.index > 2) {
                userspace_config.display.oled.pet.index = 0;
            }
            eeconfig_update_user_datablock_handler(&userspace_config, 0, EECONFIG_USER_DATA_SIZE);
            return false;
        default:
            return true;
    }
}

__attribute__((weak)) void display_handler_oled_pet_animation(char *text_buffer, size_t buffer_len) {
    switch (userspace_config.display.oled.pet.index) {
        case 0:
            strncpy(text_buffer, "Tora the Cat", buffer_len - 1);
            return;
        case 1:
            strncpy(text_buffer, "Kitty", buffer_len - 1);
            return;
        case 2:
            strncpy(text_buffer, "Luna", buffer_len - 1);
            return;
    }

    strncpy(text_buffer, "Cthulhu", buffer_len);
}

bool menu_handler_oled_pet_sleep_speed(menu_input_t input) {
    switch (input) {
        case menu_input_left:
            userspace_config.display.oled.pet.sleep_speed = (userspace_config.display.oled.pet.sleep_speed - 1) % 100;
            if (userspace_config.display.oled.pet.sleep_speed > 100) {
                userspace_config.display.oled.pet.sleep_speed = 100;
            }
            eeconfig_update_user_datablock_handler(&userspace_config, 0, EECONFIG_USER_DATA_SIZE);
            return false;
        case menu_input_right:
        case menu_input_enter:
            userspace_config.display.oled.pet.sleep_speed = (userspace_config.display.oled.pet.sleep_speed + 1) % 100;
            if (userspace_config.display.oled.pet.sleep_speed > 100) {
                userspace_config.display.oled.pet.sleep_speed = 0;
            }
            eeconfig_update_user_datablock_handler(&userspace_config, 0, EECONFIG_USER_DATA_SIZE);
            return false;
        default:
            return true;
    }
}

__attribute__((weak)) void display_handler_oled_pet_sleep_speed(char *text_buffer, size_t buffer_len) {
    snprintf(text_buffer, buffer_len - 1, "%d", userspace_config.display.oled.pet.sleep_speed);
}

bool menu_handler_oled_pet_kaki_speed(menu_input_t input) {
    switch (input) {
        case menu_input_left:
            userspace_config.display.oled.pet.kaki_speed = (userspace_config.display.oled.pet.kaki_speed - 5) % 200;
            if (userspace_config.display.oled.pet.kaki_speed > 200) {
                userspace_config.display.oled.pet.kaki_speed = 200;
            }
            eeconfig_update_user_datablock_handler(&userspace_config, 0, EECONFIG_USER_DATA_SIZE);
            return false;
        case menu_input_right:
        case menu_input_enter:
            userspace_config.display.oled.pet.kaki_speed = (userspace_config.display.oled.pet.kaki_speed + 5) % 200;
            if (userspace_config.display.oled.pet.kaki_speed > 200) {
                userspace_config.display.oled.pet.kaki_speed = 0;
            }
            eeconfig_update_user_datablock_handler(&userspace_config, 0, EECONFIG_USER_DATA_SIZE);
            return false;
        default:
            return true;
    }
}

__attribute__((weak)) void display_handler_oled_pet_kaki_speed(char *text_buffer, size_t buffer_len) {
    snprintf(text_buffer, buffer_len - 1, "%d", userspace_config.display.oled.pet.kaki_speed);
}

bool menu_handler_oled_pet_mati_speed(menu_input_t input) {
    switch (input) {
        case menu_input_left:
            userspace_config.display.oled.pet.mati_speed = (userspace_config.display.oled.pet.mati_speed - 5) % 200;
            if (userspace_config.display.oled.pet.mati_speed > 200) {
                userspace_config.display.oled.pet.mati_speed = 200;
            }
            eeconfig_update_user_datablock_handler(&userspace_config, 0, EECONFIG_USER_DATA_SIZE);
            return false;
        case menu_input_right:
        case menu_input_enter:
            userspace_config.display.oled.pet.mati_speed = (userspace_config.display.oled.pet.mati_speed + 5) % 200;
            if (userspace_config.display.oled.pet.mati_speed > 200) {
                userspace_config.display.oled.pet.mati_speed = 0;
            }
            eeconfig_update_user_datablock_handler(&userspace_config, 0, EECONFIG_USER_DATA_SIZE);
            return false;
        default:
            return true;
    }
}

__attribute__((weak)) void display_handler_oled_pet_mati_speed(char *text_buffer, size_t buffer_len) {
    snprintf(text_buffer, buffer_len - 1, "%d", userspace_config.display.oled.pet.mati_speed);
}

#endif // OLED_ENABLE && CUSTOM_OLED_DRIVER

#if defined(QUANTUM_PAINTER_ENABLE) && defined(CUSTOM_QUANTUM_PAINTER_ENABLE)
#    include "display/painter/painter.h"
extern painter_image_array_t screen_saver_image[];
extern const uint8_t         screensaver_image_size;

#    define MAX_MODES 6

static char *display_handler_display_mode(uint8_t mode) {
    switch (mode) {
        case 0:
            return "Console";
        case 1:
            return "Fonts";
        case 2:
            return "QMK Info";
        case 3:
            return "Nyan Cat";
        case 4:
            return "Game of Life";
        case 5:
            return "Layer Map";
        default:
            return "Unknown";
    }
}

__attribute__((weak)) void display_handler_display_mode_left(char *text_buffer, size_t buffer_len) {
    strncpy(text_buffer, display_handler_display_mode(userspace_config.display.painter.display_mode_left),
            buffer_len - 1);
}

__attribute__((weak)) void display_handler_display_mode_right(char *text_buffer, size_t buffer_len) {
    strncpy(text_buffer, display_handler_display_mode(userspace_config.display.painter.display_mode_right),
            buffer_len - 1);
}

static bool menu_handler_display_mode(menu_input_t input, uint8_t *mode, uint8_t max_modes) {
    if (max_modes == 0) {
        return true;
    }

    switch (input) {
        case menu_input_left:
            *mode = (*mode + max_modes - 1) % max_modes;
            return false;
        case menu_input_right:
        case menu_input_enter:
            *mode = (*mode + 1) % max_modes;
            if (*mode > (max_modes - 1)) {
                *mode = 0;
                *mode = (*mode + 1) % max_modes;
            }
            return false;
        default:
            return true;
    }
}

bool menu_handler_display_mode_left(menu_input_t input) {
    uint8_t temp       = userspace_config.display.painter.display_mode_left;
    bool    pass_along = menu_handler_display_mode(input, &temp, MAX_MODES);
    if (userspace_config.display.painter.display_mode_left != temp) {
        userspace_config.display.painter.display_mode_left = temp;
        eeconfig_update_user_datablock_handler(&userspace_config, 0, EECONFIG_USER_DATA_SIZE);
    }
    return pass_along;
}

bool menu_handler_display_mode_right(menu_input_t input) {
    uint8_t temp       = userspace_config.display.painter.display_mode_right;
    bool    pass_along = menu_handler_display_mode(input, &temp, MAX_MODES);
    if (userspace_config.display.painter.display_mode_right != temp) {
        userspace_config.display.painter.display_mode_right = temp;
        eeconfig_update_user_datablock_handler(&userspace_config, 0, EECONFIG_USER_DATA_SIZE);
    }
    return pass_along;
}

bool menu_handler_display_image_left(menu_input_t input) {
    if (screensaver_image_size == 0) {
        return true;
    }
    switch (input) {
        case menu_input_left:
            userspace_config.display.painter.display_logo_left =
                (userspace_config.display.painter.display_logo_left - 1) % screensaver_image_size;
            if (userspace_config.display.painter.display_logo_left > (screensaver_image_size - 1)) {
                userspace_config.display.painter.display_logo_left = (screensaver_image_size - 1);
            }
            eeconfig_update_user_datablock_handler(&userspace_config, 0, EECONFIG_USER_DATA_SIZE);
            return false;
        case menu_input_right:
        case menu_input_enter:
            userspace_config.display.painter.display_logo_left =
                (userspace_config.display.painter.display_logo_left + 1) % screensaver_image_size;
            if (userspace_config.display.painter.display_logo_left > (screensaver_image_size - 1)) {
                userspace_config.display.painter.display_logo_left = 0;
            }
            eeconfig_update_user_datablock_handler(&userspace_config, 0, EECONFIG_USER_DATA_SIZE);
            return false;
        default:
            return true;
    }
}

__attribute__((weak)) void display_handler_display_image_left(char *text_buffer, size_t buffer_len) {
    if (screensaver_image_size == 0) {
        strncpy(text_buffer, "No Images", buffer_len - 1);
        return;
    }
    strncpy(text_buffer, screen_saver_image[userspace_config.display.painter.display_logo_left].name, buffer_len - 1);
}

bool menu_handler_display_image_right(menu_input_t input) {
    if (screensaver_image_size == 0) {
        return true;
    }
    switch (input) {
        case menu_input_left:
            userspace_config.display.painter.display_logo_right =
                (userspace_config.display.painter.display_logo_right - 1) % screensaver_image_size;
            if (userspace_config.display.painter.display_logo_right > (screensaver_image_size - 1)) {
                userspace_config.display.painter.display_logo_right = (screensaver_image_size - 1);
            }
            eeconfig_update_user_datablock_handler(&userspace_config, 0, EECONFIG_USER_DATA_SIZE);
            return false;
        case menu_input_right:
        case menu_input_enter:
            userspace_config.display.painter.display_logo_right =
                (userspace_config.display.painter.display_logo_right + 1) % screensaver_image_size;
            if (userspace_config.display.painter.display_logo_right > (screensaver_image_size - 1)) {
                userspace_config.display.painter.display_logo_right = 0;
            }
            eeconfig_update_user_datablock_handler(&userspace_config, 0, EECONFIG_USER_DATA_SIZE);
            return false;
        default:
            return true;
    }
}

__attribute__((weak)) void display_handler_display_image_right(char *text_buffer, size_t buffer_len) {
    if (screensaver_image_size == 0) {
        strncpy(text_buffer, "No Images", buffer_len - 1);
        return;
    }
    strncpy(text_buffer, screen_saver_image[userspace_config.display.painter.display_logo_right].name, buffer_len - 1);
}

bool menu_handler_display_hue(menu_input_t input, bool painter_is_primary) {
    switch (input) {
        case menu_input_left:
            painter_decrease_hue(painter_is_primary);
            return false;
        case menu_input_right:
        case menu_input_enter:
            painter_increase_hue(painter_is_primary);
            return false;
        default:
            return true;
    }
}

__attribute__((weak)) void display_handler_display_hue(char *text_buffer, size_t buffer_len, bool painter_is_primary) {
    snprintf(text_buffer, buffer_len - 1, "%d", painter_get_hue(painter_is_primary));
}

bool menu_handler_display_sat(menu_input_t input, bool painter_is_primary) {
    switch (input) {
        case menu_input_left:
            painter_decrease_sat(painter_is_primary);
            return false;
        case menu_input_right:
        case menu_input_enter:
            painter_increase_sat(painter_is_primary);
            return false;
        default:
            return true;
    }
}

__attribute__((weak)) void display_handler_display_sat(char *text_buffer, size_t buffer_len, bool painter_is_primary) {
    snprintf(text_buffer, buffer_len - 1, "%d", painter_get_sat(painter_is_primary));
}

bool menu_handler_display_val(menu_input_t input, bool painter_is_primary) {
    switch (input) {
        case menu_input_left:
            painter_decrease_val(painter_is_primary);
            return false;
        case menu_input_right:
        case menu_input_enter:
            painter_increase_val(painter_is_primary);
            return false;
        default:
            return true;
    }
}

__attribute__((weak)) void display_handler_display_val(char *text_buffer, size_t buffer_len, bool painter_is_primary) {
    snprintf(text_buffer, buffer_len - 1, "%d", painter_get_val(painter_is_primary));
}

bool menu_handler_display_hue_primary(menu_input_t input) {
    return menu_handler_display_hue(input, true);
}

__attribute__((weak)) void display_handler_display_hue_primary(char *text_buffer, size_t buffer_len) {
    display_handler_display_hue(text_buffer, buffer_len, true);
}

bool menu_handler_display_sat_primary(menu_input_t input) {
    return menu_handler_display_sat(input, true);
}

__attribute__((weak)) void display_handler_display_sat_primary(char *text_buffer, size_t buffer_len) {
    display_handler_display_sat(text_buffer, buffer_len, true);
}

bool menu_handler_display_val_primary(menu_input_t input) {
    return menu_handler_display_val(input, true);
}

__attribute__((weak)) void display_handler_display_val_primary(char *text_buffer, size_t buffer_len) {
    display_handler_display_val(text_buffer, buffer_len, true);
}

bool menu_handler_display_hue_secondary(menu_input_t input) {
    return menu_handler_display_hue(input, false);
}

__attribute__((weak)) void display_handler_display_hue_secondary(char *text_buffer, size_t buffer_len) {
    display_handler_display_hue(text_buffer, buffer_len, false);
}

bool menu_handler_display_sat_secondary(menu_input_t input) {
    return menu_handler_display_sat(input, false);
}

__attribute__((weak)) void display_handler_display_sat_secondary(char *text_buffer, size_t buffer_len) {
    display_handler_display_sat(text_buffer, buffer_len, false);
}

bool menu_handler_display_val_secondary(menu_input_t input) {
    return menu_handler_display_val(input, false);
}

__attribute__((weak)) void display_handler_display_val_secondary(char *text_buffer, size_t buffer_len) {
    display_handler_display_val(text_buffer, buffer_len, false);
}
#endif // QUANTUM_PAINTER_ENABLE

menu_entry_t oled_pets_entries[] = {
#if defined(OLED_ENABLE) && defined(CUSTOM_OLED_DRIVER)
    MENU_ENTRY_CHILD("Pet Animation", "Pet", oled_pet_animation),
    MENU_ENTRY_CHILD("Sleep WPM (Max)", "Sleep WPM", oled_pet_sleep_speed),
    MENU_ENTRY_CHILD("Kaki WPM (Min)", "Kaki WPM", oled_pet_kaki_speed),
    MENU_ENTRY_CHILD("Mati WPM (Min)", "Mati WPM", oled_pet_mati_speed),
#endif
};

menu_entry_t display_option_entries[] = {
#ifdef SPLIT_KEYBOARD
    MENU_ENTRY_CHILD("Menu Location", "Side", display_menu_location),
#endif // SPLIT_KEYBOARD
    MENU_ENTRY_CHILD("Rotation", "Rotation", display_rotation),
    MENU_ENTRY_CHILD("Inverted", "Inverted", display_inverted),
#if defined(OLED_ENABLE) && defined(CUSTOM_OLED_DRIVER)
    MENU_ENTRY_CHILD("Brightness", "Brightness", oled_brightness),
    MENU_ENTRY_CHILD("Screen Lock", "Lock", oled_lock),
    MENU_ENTRY_MULTI("Pet Animation", "Pet", oled_pets_entries, oled_pet_animation),
#endif // OLED_ENABLE && CUSTOM_OLED_DRIVER
#if defined(QUANTUM_PAINTER_ENABLE) && defined(CUSTOM_QUANTUM_PAINTER_ENABLE)
#    ifdef SPLIT_KEYBOARD
    MENU_ENTRY_CHILD("Display (Left)", "Left", display_mode_left),
    MENU_ENTRY_CHILD("Display (Right)", "Right", display_mode_right),
#    else  // SPLIT_KEYBOARD
    MENU_ENTRY_CHILD("Display", "Display", display_mode_left),
#    endif // SPLIT_KEYBOARD
    MENU_ENTRY_CHILD("Left Image", "Image(L)", display_image_left),
    MENU_ENTRY_CHILD("Right Image", "Image(R)", display_image_right),
    MENU_ENTRY_CHILD("Primary Hue", "P Hue", display_hue_primary),
    MENU_ENTRY_CHILD("Primary Saturation", "P Sat", display_sat_primary),
    MENU_ENTRY_CHILD("Primary Value", "P Val", display_val_primary),
    MENU_ENTRY_CHILD("Secondary Hue", "S Hue", display_hue_secondary),
    MENU_ENTRY_CHILD("Secondary Saturation", "S Sat", display_sat_secondary),
    MENU_ENTRY_CHILD("Secondary Value", "S Val", display_val_secondary),
#endif // QUANTUM_PAINTER_ENABLE
};
