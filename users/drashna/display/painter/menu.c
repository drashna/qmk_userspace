// Copyright 2018-2024 Nick Brassel (@tzarc)
// Copyright 2024 Drashna (@drashna)
// SPDX-License-Identifier: GPL-2.0-or-later

#include "user_config.h"
#include <printf.h>
#include "display/painter/menu.h"
#include "display/painter/painter.h"
#include "process_keycode/process_unicode_common.h"
#include "unicode.h"

#ifdef AUDIO_ENABLE
#    include "audio.h"

#    ifndef AG_NORM_SONG
#        define AG_NORM_SONG SONG(AG_NORM_SOUND)
#    endif
#    ifndef AG_SWAP_SONG
#        define AG_SWAP_SONG SONG(AG_SWAP_SOUND)
#    endif
#    ifndef CG_NORM_SONG
#        define CG_NORM_SONG SONG(AG_NORM_SOUND)
#    endif
#    ifndef CG_SWAP_SONG
#        define CG_SWAP_SONG SONG(AG_SWAP_SOUND)
#    endif
static float ag_norm_song[][2] = AG_NORM_SONG;
static float ag_swap_song[][2] = AG_SWAP_SONG;
static float cg_norm_song[][2] = CG_NORM_SONG;
static float cg_swap_song[][2] = CG_SWAP_SONG;
#endif
#ifndef DISPLAY_MENU_TIMEOUT
#    define DISPLAY_MENU_TIMEOUT 30000
#endif // !DISPLAY_MENU_TIMEOUT
deferred_token menu_deferred_token = INVALID_DEFERRED_TOKEN;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Display options

static bool menu_handler_display(menu_input_t input) {
    switch (input) {
        case menu_input_left:
            userspace_config.painter.display_mode = (userspace_config.painter.display_mode - 1) % 4;
            if (userspace_config.painter.display_mode > 3) {
                userspace_config.painter.display_mode = 3;
            }
            eeconfig_update_user_datablock(&userspace_config);
            return false;
        case menu_input_right:
            userspace_config.painter.display_mode = (userspace_config.painter.display_mode + 1) % 4;
            if (userspace_config.painter.display_mode > 3) {
                userspace_config.painter.display_mode = 0;
            }
            eeconfig_update_user_datablock(&userspace_config);
            return false;
        default:
            return true;
    }
}

void display_handler_display(char *text_buffer, size_t buffer_len) {
    switch (userspace_config.painter.display_mode) {
        case 0:
            strncpy(text_buffer, "Console", buffer_len - 1);
            return;
        case 1:
            strncpy(text_buffer, "Layer Map", buffer_len - 1);
            return;
        case 2:
            strncpy(text_buffer, "Fonts", buffer_len - 1);
            return;
        case 3:
            strncpy(text_buffer, "QMK Banner", buffer_len - 1);
            return;
    }

    strncpy(text_buffer, "Unknown", buffer_len);
}

static bool menu_handler_slave_image(menu_input_t input) {
    switch (input) {
        case menu_input_left:
            userspace_config.painter.display_logo = (userspace_config.painter.display_logo - 1) % 11;
            if (userspace_config.painter.display_logo > 10) {
                userspace_config.painter.display_logo = 10;
            }
            eeconfig_update_user_datablock(&userspace_config);
            return false;
        case menu_input_right:
            userspace_config.painter.display_logo = (userspace_config.painter.display_logo + 1) % 11;
            if (userspace_config.painter.display_logo > 10) {
                userspace_config.painter.display_logo = 0;
            }
            eeconfig_update_user_datablock(&userspace_config);
            return false;
        default:
            return true;
    }
}

void display_handler_slave_side_image(char *text_buffer, size_t buffer_len) {
    switch (userspace_config.painter.display_logo) {
        case 0:
            strncpy(text_buffer, "Samurai", buffer_len - 1);
            return;
        case 1:
            strncpy(text_buffer, "Anime Girl", buffer_len - 1);
            return;
        case 2:
            strncpy(text_buffer, "Asuka", buffer_len - 1);
            return;
        case 3:
            strncpy(text_buffer, "Eva Unit 00", buffer_len - 1);
            return;
        case 4:
            strncpy(text_buffer, "Eva Unit 01", buffer_len - 1);
            return;
        case 5:
            strncpy(text_buffer, "Eva Unit 02", buffer_len - 1);
            return;
        case 6:
            strncpy(text_buffer, "Eva Unit 03", buffer_len - 1);
            return;
        case 7:
            strncpy(text_buffer, "Eva Unit 04", buffer_len - 1);
            return;
        case 8:
            strncpy(text_buffer, "Eva Unit 05", buffer_len - 1);
            return;
        case 9:
            strncpy(text_buffer, "Eva Unit 06", buffer_len - 1);
            return;
        case 10:
            strncpy(text_buffer, "Console", buffer_len - 1);
            return;
    }

    strncpy(text_buffer, "Unknown", buffer_len);
}

static bool menu_handler_display_rotation(menu_input_t input) {
    void init_display_ili9341_rotation(void);
    switch (input) {
#ifdef DISPLAY_FULL_ROTATION_ENABLE
        case menu_input_left:
            userspace_config.painter.rotation = (userspace_config.painter.rotation - 1) % 4;
            if (userspace_config.painter.rotation > 3) {
                userspace_config.painter.rotation = 3;
            }
            eeconfig_update_user_datablock(&userspace_config);
            return false;
        case menu_input_right:
            userspace_config.painter.rotation = (userspace_config.painter.rotation + 1) % 4;
            if (userspace_config.painter.rotation > 3) {
                userspace_config.painter.rotation = 0;
            }
            eeconfig_update_user_datablock(&userspace_config);
            init_display_ili9341_rotation();
            return false;
#else
        case menu_input_left:
        case menu_input_right:
            userspace_config.painter.rotation ^= 1;
            eeconfig_update_user_datablock(&userspace_config);
            init_display_ili9341_rotation();
            return false;
#endif
        default:
            return true;
    }
}

void display_handler_display_rotation(char *text_buffer, size_t buffer_len) {
#ifdef DISPLAY_FULL_ROTATION_ENABLE
    switch (userspace_config.painter.rotation) {
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
    strncpy(text_buffer, userspace_config.painter.rotation ? "Flipped" : "Normal", buffer_len - 1);
    return;
#endif

    strncpy(text_buffer, "Unknown", buffer_len);
}

static bool menu_handler_display_inverted(menu_input_t input) {
    void init_display_ili9341_inversion(void);
    switch (input) {
        case menu_input_left:
        case menu_input_right:
            userspace_config.painter.inverted = !userspace_config.painter.inverted;
            eeconfig_update_user_datablock(&userspace_config);
            init_display_ili9341_inversion();
            return false;
        default:
            return true;
    }
}

void display_handler_display_inverted(char *text_buffer, size_t buffer_len) {
    strncpy(text_buffer, userspace_config.painter.inverted ? "Inverted" : "Normal", buffer_len - 1);
}

static bool painter_is_primary = true;

static bool menu_handler_display_hue(menu_input_t input) {
    switch (input) {
        case menu_input_left:
            painter_decrease_hue(painter_is_primary);
            return false;
        case menu_input_right:
            painter_increase_hue(painter_is_primary);
            return false;
        default:
            return true;
    }
}
void display_handler_display_hue(char *text_buffer, size_t buffer_len) {
    snprintf(text_buffer, buffer_len - 1, "%d", painter_get_hue(painter_is_primary));
}

static bool menu_handler_display_sat(menu_input_t input) {
    switch (input) {
        case menu_input_left:
            painter_decrease_sat(painter_is_primary);
            return false;
        case menu_input_right:
            painter_increase_sat(painter_is_primary);
            return false;
        default:
            return true;
    }
}

void display_handler_display_sat(char *text_buffer, size_t buffer_len) {
    snprintf(text_buffer, buffer_len - 1, "%d", painter_get_sat(painter_is_primary));
}

static bool menu_handler_display_val(menu_input_t input) {
    switch (input) {
        case menu_input_left:
            painter_decrease_val(painter_is_primary);
            return false;
        case menu_input_right:
            painter_increase_val(painter_is_primary);
            return false;
        default:
            return true;
    }
}

void display_handler_display_val(char *text_buffer, size_t buffer_len) {
    snprintf(text_buffer, buffer_len - 1, "%d", painter_get_val(painter_is_primary));
}

static bool menu_handler_display_hsv(menu_input_t input) {
    switch (input) {
        case menu_input_left:
        case menu_input_right:
            painter_is_primary = !painter_is_primary;
            return false;
        default:
            return true;
    }
}

void display_handler_display_hsv(char *text_buffer, size_t buffer_len) {
    snprintf(text_buffer, buffer_len - 1, "%s", painter_is_primary ? "Primary" : "Secondary");
}

menu_entry_t display_option_entries[] = {
    {
        .flags                 = menu_flag_is_value,
        .text                  = "Display Option",
        .child.menu_handler    = menu_handler_display,
        .child.display_handler = display_handler_display,
    },
    {
        .flags                 = menu_flag_is_value,
        .text                  = "Slave Side Image",
        .child.menu_handler    = menu_handler_slave_image,
        .child.display_handler = display_handler_slave_side_image,
    },
    {
        .flags                 = menu_flag_is_value,
        .text                  = "Display Rotation",
        .child.menu_handler    = menu_handler_display_rotation,
        .child.display_handler = display_handler_display_rotation,
    },
    {
        .flags                 = menu_flag_is_value,
        .text                  = "Display Inverted",
        .child.menu_handler    = menu_handler_display_inverted,
        .child.display_handler = display_handler_display_inverted,
    },
    {
        .flags                 = menu_flag_is_value,
        .text                  = "Display HSV",
        .child.menu_handler    = menu_handler_display_hsv,
        .child.display_handler = display_handler_display_hsv,
    },
    {
        .flags                 = menu_flag_is_value,
        .text                  = "Display Hue",
        .child.menu_handler    = menu_handler_display_hue,
        .child.display_handler = display_handler_display_hue,
    },
    {
        .flags                 = menu_flag_is_value,
        .text                  = "Display Saturation",
        .child.menu_handler    = menu_handler_display_sat,
        .child.display_handler = display_handler_display_sat,
    },
    {
        .flags                 = menu_flag_is_value,
        .text                  = "Display Value",
        .child.menu_handler    = menu_handler_display_val,
        .child.display_handler = display_handler_display_val,
    },
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Unicode

#ifdef UNICODE_COMMON_ENABLE
static bool menu_handler_unicode(menu_input_t input) {
    switch (input) {
        case menu_input_left:
            unicode_input_mode_step_reverse();
            return false;
        case menu_input_right:
            unicode_input_mode_step();
            return false;
        default:
            return true;
    }
}

void display_handler_unicode(char *text_buffer, size_t buffer_len) {
    switch (get_unicode_input_mode()) {
        case UNICODE_MODE_MACOS:
            strncpy(text_buffer, "macOS", buffer_len - 1);
            return;
        case UNICODE_MODE_LINUX:
            strncpy(text_buffer, "Linux", buffer_len - 1);
            return;
        case UNICODE_MODE_BSD:
            strncpy(text_buffer, "BSD", buffer_len - 1);
            return;
        case UNICODE_MODE_WINDOWS:
            strncpy(text_buffer, "Windows", buffer_len - 1);
            return;
        case UNICODE_MODE_WINCOMPOSE:
            strncpy(text_buffer, "WinCompose", buffer_len - 1);
            return;
        case UNICODE_MODE_EMACS:
            strncpy(text_buffer, "Emacs", buffer_len - 1);
            return;
    }

    strncpy(text_buffer, "Unknown", buffer_len);
}

static bool menu_handler_unicode_typing(menu_input_t input) {
    switch (input) {
        case menu_input_left:
            user_runtime_state.unicode.typing_mode = (user_runtime_state.unicode.typing_mode - 1) % UNCODES_MODE_END;
            if (user_runtime_state.unicode.typing_mode >= UNCODES_MODE_END) {
                user_runtime_state.unicode.typing_mode = UNCODES_MODE_END - 1;
            }
            return false;
        case menu_input_right:
            user_runtime_state.unicode.typing_mode = (user_runtime_state.unicode.typing_mode + 1) % UNCODES_MODE_END;
            if (user_runtime_state.unicode.typing_mode >= UNCODES_MODE_END) {
                user_runtime_state.unicode.typing_mode = 0;
            }
            return false;
        default:
            return true;
    }
}
void display_handler_unicode_typing(char *text_buffer, size_t buffer_len) {
    strncpy(text_buffer, unicode_typing_mode(user_runtime_state.unicode.typing_mode), buffer_len);
}

menu_entry_t unicode_entries[] = {
    {
        .flags                 = menu_flag_is_value,
        .text                  = "Unicode mode",
        .child.menu_handler    = menu_handler_unicode,
        .child.display_handler = display_handler_unicode,
    },
    {
        .flags                 = menu_flag_is_value,
        .text                  = "Unicode Typing Mode",
        .child.menu_handler    = menu_handler_unicode_typing,
        .child.display_handler = display_handler_unicode_typing,
    },
};
#endif // UNICODE_COMMON_ENABLE

#if defined(RGB_MATRIX_ENABLE) || defined(RGBLIGHT_ENABLE)
static bool menu_handler_rgb_layer(menu_input_t input) {
    switch (input) {
        case menu_input_left:
        case menu_input_right:
            rgb_layer_indication_toggle();
            return false;
        default:
            return true;
    }
}

void display_handler_rgb_layer(char *text_buffer, size_t buffer_len) {
    snprintf(text_buffer, buffer_len - 1, "%s", userspace_config.rgb.layer_change ? "on" : "off");
}

#endif // RGB_MATRIX_ENABLE || RGBLIGHT_ENABLE

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// RGB Matrix

#ifdef RGB_MATRIX_ENABLE
static bool menu_handler_rm_enabled(menu_input_t input) {
    switch (input) {
        case menu_input_left:
        case menu_input_right:
            rgb_matrix_toggle();
            return false;
        default:
            return true;
    }
}

void display_handler_rm_enabled(char *text_buffer, size_t buffer_len) {
    snprintf(text_buffer, buffer_len - 1, "%s", rgb_matrix_is_enabled() ? "on" : "off");
}

static bool menu_handler_rm_mode(menu_input_t input) {
    switch (input) {
        case menu_input_left:
            rgb_matrix_step_reverse();
            return false;
        case menu_input_right:
            rgb_matrix_step();
            return false;
        default:
            return true;
    }
}

void display_handler_rm_mode(char *text_buffer, size_t buffer_len) {
    snprintf(text_buffer, buffer_len - 1, "%s", rgb_matrix_get_effect_name());
}

static bool menu_handler_rm_hue(menu_input_t input) {
    switch (input) {
        case menu_input_left:
            rgb_matrix_decrease_hue();
            return false;
        case menu_input_right:
            rgb_matrix_increase_hue();
            return false;
        default:
            return true;
    }
}

void display_handler_rm_hue(char *text_buffer, size_t buffer_len) {
    snprintf(text_buffer, buffer_len - 1, "%d", rgb_matrix_get_hue());
}

static bool menu_handler_rm_sat(menu_input_t input) {
    switch (input) {
        case menu_input_left:
            rgb_matrix_decrease_sat();
            return false;
        case menu_input_right:
            rgb_matrix_increase_sat();
            return false;
        default:
            return true;
    }
}

void display_handler_rm_sat(char *text_buffer, size_t buffer_len) {
    snprintf(text_buffer, buffer_len - 1, "%d", rgb_matrix_get_sat());
}

static bool menu_handler_rm_val(menu_input_t input) {
    switch (input) {
        case menu_input_left:
            rgb_matrix_decrease_val();
#    if defined(RGBLIGHT_CUSTOM)
            rgblight_sethsv_noeeprom(rgblight_get_hue(), rgblight_get_sat(), rgb_matrix_get_val());
#    endif
            return false;
        case menu_input_right:
            rgb_matrix_increase_val();
#    if defined(RGBLIGHT_CUSTOM)
            rgblight_sethsv_noeeprom(rgblight_get_hue(), rgblight_get_sat(), rgb_matrix_get_val());
#    endif
            return false;
        default:
            return true;
    }
}

void display_handler_rm_val(char *text_buffer, size_t buffer_len) {
    snprintf(text_buffer, buffer_len - 1, "%d", rgb_matrix_get_val());
}

static bool menu_handler_rm_speed(menu_input_t input) {
    switch (input) {
        case menu_input_left:
            rgb_matrix_decrease_speed();
            return false;
        case menu_input_right:
            rgb_matrix_increase_speed();
            return false;
        default:
            return true;
    }
}

void display_handler_rm_speed(char *text_buffer, size_t buffer_len) {
    snprintf(text_buffer, buffer_len - 1, "%d", rgb_matrix_get_speed());
}

static bool menu_handler_rgb_idle(menu_input_t input) {
    switch (input) {
        case menu_input_left:
        case menu_input_right:
            rgb_matrix_idle_anim_toggle();
            return false;
        default:
            return true;
    }
}

void display_handler_rgb_idle(char *text_buffer, size_t buffer_len) {
    snprintf(text_buffer, buffer_len - 1, "%s", userspace_config.rgb.idle_anim ? "on" : "off");
}

menu_entry_t rgb_matrix_entries[] = {
    {
        .flags                 = menu_flag_is_value,
        .text                  = "RGB Enabled",
        .child.menu_handler    = menu_handler_rm_enabled,
        .child.display_handler = display_handler_rm_enabled,
    },
    {
        .flags                 = menu_flag_is_value,
        .text                  = "RGB Mode",
        .child.menu_handler    = menu_handler_rm_mode,
        .child.display_handler = display_handler_rm_mode,
    },
    {
        .flags                 = menu_flag_is_value,
        .text                  = "RGB Hue",
        .child.menu_handler    = menu_handler_rm_hue,
        .child.display_handler = display_handler_rm_hue,
    },
    {
        .flags                 = menu_flag_is_value,
        .text                  = "RGB Saturation",
        .child.menu_handler    = menu_handler_rm_sat,
        .child.display_handler = display_handler_rm_sat,
    },
    {
        .flags                 = menu_flag_is_value,
        .text                  = "RGB Value",
        .child.menu_handler    = menu_handler_rm_val,
        .child.display_handler = display_handler_rm_val,
    },
    {
        .flags                 = menu_flag_is_value,
        .text                  = "RGB Speed",
        .child.menu_handler    = menu_handler_rm_speed,
        .child.display_handler = display_handler_rm_speed,
    },
    {
        .flags                 = menu_flag_is_value,
        .text                  = "Layer Indication",
        .child.menu_handler    = menu_handler_rgb_layer,
        .child.display_handler = display_handler_rgb_layer,
    },
    {
        .flags                 = menu_flag_is_value,
        .text                  = "Idle Animation",
        .child.menu_handler    = menu_handler_rgb_idle,
        .child.display_handler = display_handler_rgb_idle,
    },
};
#endif // RGB_MATRIX_ENABLE

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// RGB Light

#ifdef RGBLIGHT_ENABLE
static bool menu_handler_rgbenabled(menu_input_t input) {
    switch (input) {
        case menu_input_left:
        case menu_input_right:
            rgblight_toggle();
            return false;
        default:
            return true;
    }
}

void display_handler_rgbenabled(char *text_buffer, size_t buffer_len) {
    snprintf(text_buffer, buffer_len - 1, "%s", rgblight_is_enabled() ? "on" : "off");
}

static bool menu_handler_rgbmode(menu_input_t input) {
    switch (input) {
        case menu_input_left:
            rgblight_step_reverse();
            return false;
        case menu_input_right:
            rgblight_step();
            return false;
        default:
            return true;
    }
}

void display_handler_rgbmode(char *text_buffer, size_t buffer_len) {
    snprintf(text_buffer, buffer_len - 1, "%s", rgblight_get_effect_name());
}

static bool menu_handler_rgbhue(menu_input_t input) {
    switch (input) {
        case menu_input_left:
            rgblight_decrease_hue();
            return false;
        case menu_input_right:
            rgblight_increase_hue();
            return false;
        default:
            return true;
    }
}

void display_handler_rgbhue(char *text_buffer, size_t buffer_len) {
    snprintf(text_buffer, buffer_len - 1, "%d", rgblight_get_hue());
}

static bool menu_handler_rgbsat(menu_input_t input) {
    switch (input) {
        case menu_input_left:
            rgblight_decrease_sat();
            return false;
        case menu_input_right:
            rgblight_increase_sat();
            return false;
        default:
            return true;
    }
}

void display_handler_rgbsat(char *text_buffer, size_t buffer_len) {
    snprintf(text_buffer, buffer_len - 1, "%d", rgblight_get_sat());
}

static bool menu_handler_rgbval(menu_input_t input) {
#    if defined(RGB_MATRIX_ENABLE) && defined(RGBLIGHT_CUSTOM)
    return menu_handler_rm_val(input);
#    endif
    switch (input) {
        case menu_input_left:
            rgblight_decrease_val();
            return false;
        case menu_input_right:
            rgblight_increase_val();
            return false;
        default:
            return true;
    }
}

void display_handler_rgbval(char *text_buffer, size_t buffer_len) {
#    if defined(RGB_MATRIX_ENABLE) && defined(RGBLIGHT_CUSTOM)
    snprintf(text_buffer, buffer_len - 1, "%d", rgb_matrix_get_val());
#    else
    snprintf(text_buffer, buffer_len - 1, "%d", rgblight_get_val());
#    endif
}

static bool menu_handler_rgbspeed(menu_input_t input) {
    switch (input) {
        case menu_input_left:
            rgblight_decrease_speed();
            return false;
        case menu_input_right:
            rgblight_increase_speed();
            return false;
        default:
            return true;
    }
}

void display_handler_rgbspeed(char *text_buffer, size_t buffer_len) {
    snprintf(text_buffer, buffer_len - 1, "%d", rgblight_get_speed());
}

menu_entry_t rgb_light_entries[] = {
    {
        .flags                 = menu_flag_is_value,
        .text                  = "RGB Enabled",
        .child.menu_handler    = menu_handler_rgbenabled,
        .child.display_handler = display_handler_rgbenabled,
    },
    {
        .flags                 = menu_flag_is_value,
        .text                  = "RGB Mode",
        .child.menu_handler    = menu_handler_rgbmode,
        .child.display_handler = display_handler_rgbmode,
    },
    {
        .flags                 = menu_flag_is_value,
        .text                  = "RGB Hue",
        .child.menu_handler    = menu_handler_rgbhue,
        .child.display_handler = display_handler_rgbhue,
    },
    {
        .flags                 = menu_flag_is_value,
        .text                  = "RGB Saturation",
        .child.menu_handler    = menu_handler_rgbsat,
        .child.display_handler = display_handler_rgbsat,
    },
    {
        .flags                 = menu_flag_is_value,
        .text                  = "RGB Value",
        .child.menu_handler    = menu_handler_rgbval,
        .child.display_handler = display_handler_rgbval,
    },
    {
        .flags                 = menu_flag_is_value,
        .text                  = "RGB Speed",
        .child.menu_handler    = menu_handler_rgbspeed,
        .child.display_handler = display_handler_rgbspeed,
    },
    {
        .flags                 = menu_flag_is_value,
        .text                  = "Layer Indication",
        .child.menu_handler    = menu_handler_rgb_layer,
        .child.display_handler = display_handler_rgb_layer,
    },
};
#endif // RGBLIGHT_ENABLE

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Backlight

#ifdef BACKLIGHT_ENABLE
static bool menu_handler_bl_enabled(menu_input_t input) {
    switch (input) {
        case menu_input_left:
        case menu_input_right:
            backlight_toggle();
            return false;
        default:
            return true;
    }
}

void display_handler_bl_enabled(char *text_buffer, size_t buffer_len) {
    snprintf(text_buffer, buffer_len - 1, "%s", is_backlight_enabled() ? "on" : "off");
}

static bool menu_handler_bl_level(menu_input_t input) {
    switch (input) {
        case menu_input_left:
            backlight_decrease();
            return false;
        case menu_input_right:
            backlight_increase();
            return false;
        default:
            return true;
    }
}

void display_handler_bl_level(char *text_buffer, size_t buffer_len) {
    snprintf(text_buffer, buffer_len - 1, "%d", get_backlight_level());
}

menu_entry_t backlight_entries[] = {
    {
        .flags                 = menu_flag_is_value,
        .text                  = "Backlight Enabled",
        .child.menu_handler    = menu_handler_bl_enabled,
        .child.display_handler = display_handler_bl_enabled,
    },
    {
        .flags                 = menu_flag_is_value,
        .text                  = "Backlight Level",
        .child.menu_handler    = menu_handler_bl_level,
        .child.display_handler = display_handler_bl_level,
    },
};
#endif // BACKLIGHT_ENABLE
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Audio

#ifdef AUDIO_ENABLE
static bool menu_handler_audio_enabled(menu_input_t input) {
    switch (input) {
        case menu_input_left:
        case menu_input_right:
            if (audio_is_on()) {
                audio_off();
            } else {
                audio_on();
            }
            return false;
        default:
            return true;
    }
}

void display_handler_audio_enabled(char *text_buffer, size_t buffer_len) {
    snprintf(text_buffer, buffer_len - 1, "%s", audio_is_on() ? "on" : "off");
}

static bool menu_handler_music_enabled(menu_input_t input) {
    switch (input) {
        case menu_input_left:
        case menu_input_right:
            music_toggle();
            return false;
        default:
            return true;
    }
}

void display_handler_music_enabled(char *text_buffer, size_t buffer_len) {
    snprintf(text_buffer, buffer_len - 1, "%s", is_music_on() ? "on" : "off");
}

static bool menu_handler_audio_clicky_enabled(menu_input_t input) {
    switch (input) {
        case menu_input_left:
        case menu_input_right:
            clicky_toggle();
            return false;
        default:
            return true;
    }
}

void display_handler_audio_clicky_enabled(char *text_buffer, size_t buffer_len) {
    snprintf(text_buffer, buffer_len - 1, "%s", is_clicky_on() ? "on" : "off");
}

static bool menu_handler_audio_clicky_freq(menu_input_t input) {
    switch (input) {
        case menu_input_left:
            clicky_freq_down();
            return false;
        case menu_input_right:
            clicky_freq_up();
            return false;
        default:
            return true;
    }
}

void display_handler_audio_clicky_freq(char *text_buffer, size_t buffer_len) {
    extern float clicky_freq;
    snprintf(text_buffer, buffer_len - 1, "%.2f", (float)clicky_freq);
}

static bool menu_handler_gaming_song_enabled(menu_input_t input) {
    switch (input) {
        case menu_input_left:
        case menu_input_right:
            userspace_config.gaming.song_enable = !userspace_config.gaming.song_enable;
            void set_doom_song(layer_state_t);
            set_doom_song(layer_state);
            return false;
        default:
            return true;
    }
}

void display_handler_gaming_song_enabled(char *text_buffer, size_t buffer_len) {
    snprintf(text_buffer, buffer_len - 1, "%s", userspace_config.gaming.song_enable ? "on" : "off");
}

menu_entry_t audio_entries[] = {
    {
        .flags                 = menu_flag_is_value,
        .text                  = "Audio Enabled",
        .child.menu_handler    = menu_handler_audio_enabled,
        .child.display_handler = display_handler_audio_enabled,
    },
    {
        .flags                 = menu_flag_is_value,
        .text                  = "Music Mode Enabled",
        .child.menu_handler    = menu_handler_music_enabled,
        .child.display_handler = display_handler_music_enabled,
    },
    {
        .flags                 = menu_flag_is_value,
        .text                  = "Clicky Enabled",
        .child.menu_handler    = menu_handler_audio_clicky_enabled,
        .child.display_handler = display_handler_audio_clicky_enabled,
    },
    {
        .flags                 = menu_flag_is_value,
        .text                  = "Clicky Frequency",
        .child.menu_handler    = menu_handler_audio_clicky_freq,
        .child.display_handler = display_handler_audio_clicky_freq,
    },
    {
        .flags                 = menu_flag_is_value,
        .text                  = "Gaming Song Enabled",
        .child.menu_handler    = menu_handler_gaming_song_enabled,
        .child.display_handler = display_handler_gaming_song_enabled,
    },
};
#endif // AUDIO_ENABLE

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Haptic

#ifdef HAPTIC_ENABLE

extern haptic_config_t haptic_config;

static bool menu_handler_haptic_enabled(menu_input_t input) {
    switch (input) {
        case menu_input_left:
        case menu_input_right:
            haptic_toggle();
            return false;
        default:
            return true;
    }
}

void display_handler_haptic_enabled(char *text_buffer, size_t buffer_len) {
    snprintf(text_buffer, buffer_len - 1, "%s", haptic_get_enable() ? "on" : "off");
}

static bool menu_handler_haptic_mode(menu_input_t input) {
    if (!haptic_get_enable()) {
        return false;
    }

    switch (input) {
        case menu_input_left:
            haptic_mode_decrease();
            return false;
        case menu_input_right:
            haptic_mode_increase();
            return false;
        default:
            return true;
    }
}

void display_handler_haptic_mode(char *text_buffer, size_t buffer_len) {
    if (haptic_get_enable()) {
        snprintf(text_buffer, buffer_len - 1, "%s", get_haptic_drv2605l_effect_name(haptic_get_mode()));
    } else {
        snprintf(text_buffer, buffer_len - 1, "off");
    }
}

static bool menu_handler_feedback_mode(menu_input_t input) {
    if (!haptic_get_enable()) {
        return false;
    }
    switch (input) {
        case menu_input_left:
        case menu_input_right:
            haptic_feedback_toggle();
            return false;
        default:
            return true;
    }
}

void display_handler_feedback_mode(char *text_buffer, size_t buffer_len) {
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
static bool menu_handler_haptic_buzz(menu_input_t input) {
    if (!haptic_get_enable()) {
        return false;
    }
    switch (input) {
        case menu_input_left:
        case menu_input_right:
            haptic_buzz_toggle();
            return false;
        default:
            return true;
    }
}

void display_handler_haptic_buzz(char *text_buffer, size_t buffer_len) {
    snprintf(text_buffer, buffer_len - 1, "%d", haptic_get_buzz());
}

static bool menu_handler_haptic_dwell(menu_input_t input) {
    switch (input) {
        case menu_input_left:
            haptic_dwell_decrease();
            return false;
        case menu_input_right:
            haptic_dwell_increase();
            return false;
        default:
            return true;
    }
}

void display_handler_haptic_dwell(char *text_buffer, size_t buffer_len) {
    snprintf(text_buffer, buffer_len - 1, "%d", haptic_get_dwell());
}
#    endif // HAPTIC_SOLENOID

#    ifdef HAPTIC_DRV2605L
static bool menu_handler_haptic_love_mode(menu_input_t input) {
    switch (input) {
        case menu_input_left:
        case menu_input_right:
            haptic_toggle_continuous();
            return false;
        default:
            return true;
    }
}

void display_handler_haptic_love_mode(char *text_buffer, size_t buffer_len) {
    snprintf(text_buffer, buffer_len - 1, "%s", haptic_config.cont ? "Aaah! ;)" : "off");
}

static bool menu_handler_haptic_love_intensity(menu_input_t input) {
    switch (input) {
        case menu_input_left:
            haptic_cont_increase();
            return false;
        case menu_input_right:
            haptic_cont_decrease();
            return false;
        default:
            return true;
    }
}

void display_handler_haptic_love_intensity(char *text_buffer, size_t buffer_len) {
    if (haptic_config.cont) {
        snprintf(text_buffer, buffer_len - 1, "%d", haptic_config.amplitude);
    } else {
        snprintf(text_buffer, buffer_len - 1, "off");
    }
}
#    endif // HAPTIC_DRV2605L

menu_entry_t haptic_entries[] = {
    {
        .flags                 = menu_flag_is_value,
        .text                  = "Enabled",
        .child.menu_handler    = menu_handler_haptic_enabled,
        .child.display_handler = display_handler_haptic_enabled,
    },
    {
        .flags                 = menu_flag_is_value,
        .text                  = "Mode",
        .child.menu_handler    = menu_handler_haptic_mode,
        .child.display_handler = display_handler_haptic_mode,
    },
    {
        .flags                 = menu_flag_is_value,
        .text                  = "Feedback Mode",
        .child.menu_handler    = menu_handler_feedback_mode,
        .child.display_handler = display_handler_feedback_mode,
    },
#    ifdef HAPTIC_SOLENOID
    {
        .flags                 = menu_flag_is_value,
        .text                  = "Buzz",
        .child.menu_handler    = menu_handler_haptic_buzz,
        .child.display_handler = display_handler_haptic_buzz,
    },
    {
        .flags                 = menu_flag_is_value,
        .text                  = "Dwell",
        .child.menu_handler    = menu_handler_haptic_dwell,
        .child.display_handler = display_handler_haptic_dwell,
    },
#    endif // HAPTIC_SOLENOID
#    ifdef HAPTIC_DRV2605L
    {
        .flags                 = menu_flag_is_value,
        .text                  = "Continuous",
        .child.menu_handler    = menu_handler_haptic_love_mode,
        .child.display_handler = display_handler_haptic_love_mode,
    },
    {
        .flags                 = menu_flag_is_value,
        .text                  = "Continuous Amplitude",
        .child.menu_handler    = menu_handler_haptic_love_intensity,
        .child.display_handler = display_handler_haptic_love_intensity,
    },
#    endif // HAPTIC_DRV2605L
};
#endif // HAPTIC_ENABLE

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Pointing Device

#ifdef POINTING_DEVICE_ENABLE

static bool menu_handler_auto_mouse_enable(menu_input_t input) {
    switch (input) {
        case menu_input_left:
        case menu_input_right:
            set_auto_mouse_enable(!get_auto_mouse_enable());
            return false;
        default:
            return true;
    }
}

void display_handler_auto_mouse_enable(char *text_buffer, size_t buffer_len) {
    snprintf(text_buffer, buffer_len - 1, "%s", get_auto_mouse_enable() ? "on" : "off");
}

static bool menu_handler_auto_mouse_layer(menu_input_t input) {
    switch (input) {
        case menu_input_left:
            set_auto_mouse_layer((get_auto_mouse_layer() - 1) % MAX_USER_LAYERS);
            return false;
        case menu_input_right:
            set_auto_mouse_layer((get_auto_mouse_layer() + 1) % MAX_USER_LAYERS);
            return false;
        default:
            return true;
    }
}

void display_handler_auto_mouse_layer(char *text_buffer, size_t buffer_len) {
    snprintf(text_buffer, buffer_len - 1, "%s", layer_name(get_auto_mouse_layer()));
}

extern bool     mouse_jiggler;
extern uint16_t mouse_jiggler_timer;

static bool menu_handler_mouse_jiggler(menu_input_t input) {
    switch (input) {
        case menu_input_left:
        case menu_input_right:
            mouse_jiggler       = !mouse_jiggler;
            mouse_jiggler_timer = timer_read();
            return false;
        default:
            return true;
    }
}

void display_handler_mouse_jiggler(char *text_buffer, size_t buffer_len) {
    snprintf(text_buffer, buffer_len - 1, "%s", mouse_jiggler ? "on" : "off");
}

#    if defined(KEYBOARD_handwired_tractyl_manuform) || defined(KEYBOARD_bastardkb_charybdis)
static bool menu_handler_dpi_config(menu_input_t input) {
    switch (input) {
        case menu_input_left:
            charybdis_cycle_pointer_default_dpi(false);
            return false;
        case menu_input_right:
            charybdis_cycle_pointer_default_dpi(true);
            return false;
        default:
            return true;
    }
}

void display_handler_dpi_config(char *text_buffer, size_t buffer_len) {
    snprintf(text_buffer, buffer_len - 1, "%d", charybdis_get_pointer_default_dpi());
}
#    endif

static bool menu_handler_mouse_accel_toggle(menu_input_t input) {
    switch (input) {
        case menu_input_left:
        case menu_input_right:
            pointing_device_accel_toggle_enabled();
            return false;
        default:
            return true;
    }
}

void display_handler_mouse_accel_toggle(char *text_buffer, size_t buffer_len) {
    snprintf(text_buffer, buffer_len - 1, "%s", pointing_device_accel_get_enabled() ? "on" : "off");
}

static bool menu_handler_mouse_accel_takeoff(menu_input_t input) {
    switch (input) {
        case menu_input_left:
            pointing_device_accel_set_takeoff(pointing_device_accel_get_takeoff() -
                                              pointing_device_accel_get_mod_step(POINTING_DEVICE_ACCEL_TAKEOFF_STEP));
            return false;
        case menu_input_right:
            pointing_device_accel_set_takeoff(pointing_device_accel_get_takeoff() +
                                              pointing_device_accel_get_mod_step(POINTING_DEVICE_ACCEL_TAKEOFF_STEP));
            return false;
        default:
            return true;
    }
}

void display_handler_mouse_accel_takeoff(char *text_buffer, size_t buffer_len) {
    snprintf(text_buffer, buffer_len - 1, "%1.2f", pointing_device_accel_get_takeoff());
}

static bool menu_handler_mouse_accel_growth_rate(menu_input_t input) {
    switch (input) {
        case menu_input_left:
            pointing_device_accel_set_growth_rate(
                pointing_device_accel_get_growth_rate() -
                pointing_device_accel_get_mod_step(POINTING_DEVICE_ACCEL_GROWTH_RATE_STEP));
            return false;
        case menu_input_right:
            pointing_device_accel_set_growth_rate(
                pointing_device_accel_get_growth_rate() +
                pointing_device_accel_get_mod_step(POINTING_DEVICE_ACCEL_GROWTH_RATE_STEP));
            return false;
        default:
            return true;
    }
}

void display_handler_mouse_accel_growth_rate(char *text_buffer, size_t buffer_len) {
    snprintf(text_buffer, buffer_len - 1, "%1.2f", pointing_device_accel_get_growth_rate());
}

static bool menu_handler_mouse_accel_offset(menu_input_t input) {
    switch (input) {
        case menu_input_left:
            pointing_device_accel_set_offset(pointing_device_accel_get_offset() -
                                             pointing_device_accel_get_mod_step(POINTING_DEVICE_ACCEL_OFFSET_STEP));
            return false;
        case menu_input_right:
            pointing_device_accel_set_offset(pointing_device_accel_get_offset() +
                                             pointing_device_accel_get_mod_step(POINTING_DEVICE_ACCEL_OFFSET_STEP));
            return false;
        default:
            return true;
    }
}

void display_handler_mouse_accel_offset(char *text_buffer, size_t buffer_len) {
    snprintf(text_buffer, buffer_len - 1, "%2.2f", pointing_device_accel_get_offset());
}

static bool menu_handler_mouse_accel_limit(menu_input_t input) {
    switch (input) {
        case menu_input_left:
            pointing_device_accel_set_limit(pointing_device_accel_get_limit() -
                                            pointing_device_accel_get_mod_step(POINTING_DEVICE_ACCEL_LIMIT_STEP));
            return false;
        case menu_input_right:
            pointing_device_accel_set_limit(pointing_device_accel_get_limit() +
                                            pointing_device_accel_get_mod_step(POINTING_DEVICE_ACCEL_LIMIT_STEP));
            return false;
        default:
            return true;
    }
}

void display_handler_mouse_accel_limit(char *text_buffer, size_t buffer_len) {
    snprintf(text_buffer, buffer_len - 1, "%1.2f", pointing_device_accel_get_limit());
}

menu_entry_t pointing_acceleration_entries[] = {
    {
        .flags                 = menu_flag_is_value,
        .text                  = "Acceleration",
        .child.menu_handler    = menu_handler_mouse_accel_toggle,
        .child.display_handler = display_handler_mouse_accel_toggle,
    },
    {
        .flags                 = menu_flag_is_value,
        .text                  = "Takeoff",
        .child.menu_handler    = menu_handler_mouse_accel_takeoff,
        .child.display_handler = display_handler_mouse_accel_takeoff,
    },
    {
        .flags                 = menu_flag_is_value,
        .text                  = "Growth Rate",
        .child.menu_handler    = menu_handler_mouse_accel_growth_rate,
        .child.display_handler = display_handler_mouse_accel_growth_rate,
    },
    {
        .flags                 = menu_flag_is_value,
        .text                  = "Offset",
        .child.menu_handler    = menu_handler_mouse_accel_offset,
        .child.display_handler = display_handler_mouse_accel_offset,
    },
    {
        .flags                 = menu_flag_is_value,
        .text                  = "Limit",
        .child.menu_handler    = menu_handler_mouse_accel_limit,
        .child.display_handler = display_handler_mouse_accel_limit,
    },
};

menu_entry_t pointing_entries[] = {
#    if defined(KEYBOARD_handwired_tractyl_manuform) || defined(KEYBOARD_bastardkb_charybdis)
    {
        .flags                 = menu_flag_is_value,
        .text                  = "DPI Config",
        .child.menu_handler    = menu_handler_dpi_config,
        .child.display_handler = display_handler_dpi_config,
    },
#    endif
    {
        .flags                 = menu_flag_is_value,
        .text                  = "Auto Mouse Enable",
        .child.menu_handler    = menu_handler_auto_mouse_enable,
        .child.display_handler = display_handler_auto_mouse_enable,
    },
    {
        .flags                 = menu_flag_is_value,
        .text                  = "Auto Mouse Layer",
        .child.menu_handler    = menu_handler_auto_mouse_layer,
        .child.display_handler = display_handler_auto_mouse_layer,
    },
    {
        .flags                 = menu_flag_is_value,
        .text                  = "Mouse Jiggler",
        .child.menu_handler    = menu_handler_mouse_jiggler,
        .child.display_handler = display_handler_mouse_jiggler,
    },
    {
        .flags              = menu_flag_is_parent,
        .text               = "Mouse Acceleration",
        .parent.children    = pointing_acceleration_entries,
        .parent.child_count = ARRAY_SIZE(pointing_acceleration_entries),
    },
};
#endif // POINTING_DEVICE_ENABLE

static bool menu_handler_keycode_ctrl_caps(menu_input_t input) {
    switch (input) {
        case menu_input_left:
        case menu_input_right:
            keymap_config.swap_control_capslock = !keymap_config.swap_control_capslock;
            eeconfig_update_keymap(keymap_config.raw);
            return false;
        default:
            return true;
    }
}

void display_handler_keycode_ctrl_caps(char *text_buffer, size_t buffer_len) {
    snprintf(text_buffer, buffer_len - 1, "%s", keymap_config.swap_control_capslock ? "swapped" : "normal");
}

static bool menu_handler_keycode_alt_gui(menu_input_t input) {
    switch (input) {
        case menu_input_left:
        case menu_input_right:
            keymap_config.swap_lalt_lgui = keymap_config.swap_ralt_rgui = !keymap_config.swap_lalt_lgui;
#ifdef AUDIO_ENABLE
            if (keymap_config.swap_ralt_rgui) {
                PLAY_SONG(ag_swap_song);
            } else {
                PLAY_SONG(ag_norm_song);
            }
#endif // AUDIO_ENABLE
            eeconfig_update_keymap(keymap_config.raw);
            return false;
        default:
            return true;
    }
}

void display_handler_keycode_alt_gui(char *text_buffer, size_t buffer_len) {
    snprintf(text_buffer, buffer_len - 1, "%s", keymap_config.swap_lalt_lgui ? "swapped" : "normal");
}

static bool menu_handler_keycode_ctrl_gui(menu_input_t input) {
    switch (input) {
        case menu_input_left:
        case menu_input_right:
            keymap_config.swap_lctl_lgui = keymap_config.swap_rctl_rgui = !keymap_config.swap_lctl_lgui;
#ifdef AUDIO_ENABLE
            if (keymap_config.swap_rctl_rgui) {
                PLAY_SONG(cg_swap_song);
            } else {
                PLAY_SONG(cg_norm_song);
            }
#endif // AUDIO_ENABLE
            eeconfig_update_keymap(keymap_config.raw);
            return false;
        default:
            return true;
    }
}

void display_handler_keycode_ctrl_gui(char *text_buffer, size_t buffer_len) {
    snprintf(text_buffer, buffer_len - 1, "%s", keymap_config.swap_lctl_lgui ? "swapped" : "normal");
}

static bool menu_handler_keycode_disable_gui(menu_input_t input) {
    switch (input) {
        case menu_input_left:
        case menu_input_right:
            keymap_config.no_gui = !keymap_config.no_gui;
            eeconfig_update_keymap(keymap_config.raw);
            return false;
        default:
            return true;
    }
}

void display_handler_keycode_disable_gui(char *text_buffer, size_t buffer_len) {
    snprintf(text_buffer, buffer_len - 1, "%s", keymap_config.no_gui ? "enabled" : "disabled");
}

__attribute__((unused)) static bool menu_handler_keycode_grave_esc(menu_input_t input) {
    switch (input) {
        case menu_input_left:
        case menu_input_right:
            keymap_config.swap_grave_esc = !keymap_config.swap_grave_esc;
            eeconfig_update_keymap(keymap_config.raw);
            return false;
        default:
            return true;
    }
}

__attribute__((unused)) void display_handler_keycode_grave_esc(char *text_buffer, size_t buffer_len) {
    snprintf(text_buffer, buffer_len - 1, "%s", keymap_config.swap_grave_esc ? "swapped" : "normal");
}

__attribute__((unused)) static bool menu_handler_keycode_bslash_bspc(menu_input_t input) {
    switch (input) {
        case menu_input_left:
        case menu_input_right:
            keymap_config.swap_backslash_backspace = !keymap_config.swap_backslash_backspace;
            eeconfig_update_keymap(keymap_config.raw);
            return false;
        default:
            return true;
    }
}

__attribute__((unused)) void display_handler_keycode_bslash_bspc(char *text_buffer, size_t buffer_len) {
    snprintf(text_buffer, buffer_len - 1, "%s", keymap_config.swap_backslash_backspace ? "swapped" : "normal");
}

static bool menu_handler_keycode_nkro(menu_input_t input) {
    switch (input) {
        case menu_input_left:
        case menu_input_right:
            keymap_config.nkro = !keymap_config.nkro;
            clear_keyboard(); // clear first buffer to prevent stuck keys
            eeconfig_update_keymap(keymap_config.raw);
            return false;
        default:
            return true;
    }
}

void display_handler_keycode_nkro(char *text_buffer, size_t buffer_len) {
    snprintf(text_buffer, buffer_len - 1, "%s", keymap_config.nkro ? "on" : "off");
}

static bool menu_handler_keycode_oneshot(menu_input_t input) {
    switch (input) {
        case menu_input_left:
        case menu_input_right:
            keymap_config.oneshot_enable = !keymap_config.oneshot_enable;
            eeconfig_update_keymap(keymap_config.raw);
            return false;
        default:
            return true;
    }
}

void display_handler_keycode_oneshot(char *text_buffer, size_t buffer_len) {
    snprintf(text_buffer, buffer_len - 1, "%s", keymap_config.oneshot_enable ? "on" : "off");
}

static bool menu_handler_keycode_autocorrect(menu_input_t input) {
    switch (input) {
        case menu_input_left:
        case menu_input_right:
            keymap_config.autocorrect_enable = !keymap_config.autocorrect_enable;
            eeconfig_update_keymap(keymap_config.raw);
            return false;
        default:
            return true;
    }
}

void display_handler_keycode_autocorrect(char *text_buffer, size_t buffer_len) {
    snprintf(text_buffer, buffer_len - 1, "%s", keymap_config.autocorrect_enable ? "on" : "off");
}

menu_entry_t keymap_config_entries[] = {
    {
        .flags                 = menu_flag_is_value,
        .text                  = "Control <-> Capslock",
        .child.menu_handler    = menu_handler_keycode_ctrl_caps,
        .child.display_handler = display_handler_keycode_ctrl_caps,
    },
    {
        .flags                 = menu_flag_is_value,
        .text                  = "Alt <-> GUI",
        .child.menu_handler    = menu_handler_keycode_alt_gui,
        .child.display_handler = display_handler_keycode_alt_gui,
    },
    {
        .flags                 = menu_flag_is_value,
        .text                  = "Control <-> GUI",
        .child.menu_handler    = menu_handler_keycode_ctrl_gui,
        .child.display_handler = display_handler_keycode_ctrl_gui,
    },
    {
        .flags                 = menu_flag_is_value,
        .text                  = "Disable GUI",
        .child.menu_handler    = menu_handler_keycode_disable_gui,
        .child.display_handler = display_handler_keycode_disable_gui,
    },
    {
        .flags                 = menu_flag_is_value,
        .text                  = "N-Key Roll Over",
        .child.menu_handler    = menu_handler_keycode_nkro,
        .child.display_handler = display_handler_keycode_nkro,
    },
    {
        .flags                 = menu_flag_is_value,
        .text                  = "Oneshot Keys",
        .child.menu_handler    = menu_handler_keycode_oneshot,
        .child.display_handler = display_handler_keycode_oneshot,
    },
    {
        .flags                 = menu_flag_is_value,
        .text                  = "Autocorrect",
        .child.menu_handler    = menu_handler_keycode_autocorrect,
        .child.display_handler = display_handler_keycode_autocorrect,
    },
    // {
    //     .flags                 = menu_flag_is_value,
    //     .text                  = "Grave <-> Escape",
    //     .child.menu_handler    = menu_handler_keycode_grave_esc,
    //     .child.display_handler = display_handler_keycode_grave_esc,
    // },
    // {
    //     .flags                 = menu_flag_is_value,
    //     .text                  = "Backslash <-> Backspace",
    //     .child.menu_handler    = menu_handler_keycode_bslash_bspc,
    //     .child.display_handler = display_handler_keycode_bslash_bspc,
    // },
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// RTC Config

#ifdef RTC_ENABLE
#    include "features/rtc/rtc.h"

void display_handler_rtc_year(char *text_buffer, size_t buffer_len) {
    if (rtc_is_connected()) {
        snprintf(text_buffer, buffer_len - 1, "%04d", rtc_read_time_struct().year);
    } else {
        snprintf(text_buffer, buffer_len - 1, "Not Connected");
    }
}

static bool menu_handler_rtc_year(menu_input_t input) {
    switch (input) {
        case menu_input_left:
            rtc_year_decrease();
            return false;
        case menu_input_right:
            rtc_year_increase();
            return false;
        default:
            return true;
    }
}

void display_handler_rtc_month(char *text_buffer, size_t buffer_len) {
    if (rtc_is_connected()) {
        snprintf(text_buffer, buffer_len - 1, "%02d", rtc_read_time_struct().month);
    } else {
        snprintf(text_buffer, buffer_len - 1, "Not Connected");
    }
}

static bool menu_handler_rtc_month(menu_input_t input) {
    switch (input) {
        case menu_input_left:
            rtc_month_decrease();
            return false;
        case menu_input_right:
            rtc_month_increase();
            return false;
        default:
            return true;
    }
}

void display_handler_rtc_date(char *text_buffer, size_t buffer_len) {
    if (rtc_is_connected()) {
        snprintf(text_buffer, buffer_len - 1, "%02d", rtc_read_time_struct().date);
    } else {
        snprintf(text_buffer, buffer_len - 1, "Not Connected");
    }
}

static bool menu_handler_rtc_date(menu_input_t input) {
    switch (input) {
        case menu_input_left:
            rtc_date_decrease();
            return false;
        case menu_input_right:
            rtc_date_increase();
            return false;
        default:
            return true;
    }
}

void display_handler_rtc_hour(char *text_buffer, size_t buffer_len) {
    if (rtc_is_connected()) {
        rtc_time_t time = rtc_read_time_struct();
        if (time.is_dst) {
            if (time.format == RTC_FORMAT_12H) {
                if (time.hour == 12) {
                    time.hour  = 1;
                    time.am_pm = time.am_pm == RTC_AM ? RTC_PM : RTC_AM;
                } else {
                    time.hour++;
                }
            } else {
                if (time.hour == 23) {
                    time.hour = 0;
                } else {
                    time.hour++;
                }
            }
        }
        snprintf(text_buffer, buffer_len - 1, "%02d", time.hour);
    } else {
        snprintf(text_buffer, buffer_len - 1, "Not Connected");
    }
}

static bool menu_handler_rtc_hour(menu_input_t input) {
    switch (input) {
        case menu_input_left:
            rtc_hour_decrease();
            return false;
        case menu_input_right:
            rtc_hour_increase();
            return false;
        default:
            return true;
    }
}

void display_handler_rtc_minute(char *text_buffer, size_t buffer_len) {
    if (rtc_is_connected()) {
        snprintf(text_buffer, buffer_len - 1, "%02d", rtc_read_time_struct().minute);
    } else {
        snprintf(text_buffer, buffer_len - 1, "Not Connected");
    }
}

static bool menu_handler_rtc_minute(menu_input_t input) {
    switch (input) {
        case menu_input_left:
            rtc_minute_decrease();
            return false;
        case menu_input_right:
            rtc_minute_increase();
            return false;
        default:
            return true;
    }
}

void display_handler_rtc_second(char *text_buffer, size_t buffer_len) {
    if (rtc_is_connected()) {
        snprintf(text_buffer, buffer_len - 1, "%02d", rtc_read_time_struct().second);
    } else {
        snprintf(text_buffer, buffer_len - 1, "Not Connected");
    }
}

static bool menu_handler_rtc_second(menu_input_t input) {
    switch (input) {
        case menu_input_left:
            rtc_second_decrease();
            return false;
        case menu_input_right:
            rtc_second_increase();
            return false;
        default:
            return true;
    }
}

void display_handler_rtc_hour_format(char *text_buffer, size_t buffer_len) {
    if (rtc_is_connected()) {
        snprintf(text_buffer, buffer_len - 1, "%s", rtc_read_time_struct().format == RTC_FORMAT_24H ? "24H" : "12H");
    } else {
        snprintf(text_buffer, buffer_len - 1, "Not Connected");
    }
}

static bool menu_handler_rtc_hour_format(menu_input_t input) {
    switch (input) {
        case menu_input_left:
        case menu_input_right:
            rtc_format_toggle();
            return false;
        default:
            return true;
    }
}

void display_handler_rtc_dst(char *text_buffer, size_t buffer_len) {
    if (rtc_is_connected()) {
        snprintf(text_buffer, buffer_len - 1, "%s", rtc_read_time_struct().is_dst ? "on" : "off");
    } else {
        snprintf(text_buffer, buffer_len - 1, "Not Connected");
    }
}

static bool menu_handler_rtc_dst(menu_input_t input) {
    switch (input) {
        case menu_input_left:
        case menu_input_right:
            rtc_dst_toggle();
            return false;
        default:
            return true;
    }
}

menu_entry_t rtc_config_entries[] = {
    {
        .flags                 = menu_flag_is_value,
        .text                  = "Year",
        .child.menu_handler    = menu_handler_rtc_year,
        .child.display_handler = display_handler_rtc_year,
    },
    {
        .flags                 = menu_flag_is_value,
        .text                  = "Month",
        .child.menu_handler    = menu_handler_rtc_month,
        .child.display_handler = display_handler_rtc_month,
    },
    {
        .flags                 = menu_flag_is_value,
        .text                  = "Date",
        .child.menu_handler    = menu_handler_rtc_date,
        .child.display_handler = display_handler_rtc_date,
    },
    {
        .flags                 = menu_flag_is_value,
        .text                  = "Hour",
        .child.menu_handler    = menu_handler_rtc_hour,
        .child.display_handler = display_handler_rtc_hour,
    },
    {
        .flags                 = menu_flag_is_value,
        .text                  = "Minute",
        .child.menu_handler    = menu_handler_rtc_minute,
        .child.display_handler = display_handler_rtc_minute,
    },
    {
        .flags                 = menu_flag_is_value,
        .text                  = "Second",
        .child.menu_handler    = menu_handler_rtc_second,
        .child.display_handler = display_handler_rtc_second,

    },
    {
        .flags                 = menu_flag_is_value,
        .text                  = "12/24 Hour Format",
        .child.menu_handler    = menu_handler_rtc_hour_format,
        .child.display_handler = display_handler_rtc_hour_format,
    },
    {
        .flags                 = menu_flag_is_value,
        .text                  = "DST",
        .child.menu_handler    = menu_handler_rtc_dst,
        .child.display_handler = display_handler_rtc_dst,
    },
};
#endif // RTC_ENABLE

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// User Settings

static bool menu_handler_overwatch_mode(menu_input_t input) {
    switch (input) {
        case menu_input_left:
        case menu_input_right:
            userspace_config.gaming.is_overwatch = !userspace_config.gaming.is_overwatch;
            eeconfig_update_user_datablock(&userspace_config);
            return false;
        default:
            return true;
    }
}

void display_handler_overwatch_mode(char *text_buffer, size_t buffer_len) {
    snprintf(text_buffer, buffer_len - 1, "%s", userspace_config.gaming.is_overwatch ? "on" : "off");
}

static bool menu_handler_gamepad_swap(menu_input_t input) {
    switch (input) {
        case menu_input_left:
        case menu_input_right:
            userspace_config.gaming.swapped_numbers = !userspace_config.gaming.swapped_numbers;
            eeconfig_update_user_datablock(&userspace_config);
            return false;
        default:
            return true;
    }
}

void display_handler_gamepad_swap(char *text_buffer, size_t buffer_len) {
    snprintf(text_buffer, buffer_len - 1, "%s", userspace_config.gaming.swapped_numbers ? "swapped" : "normal");
}

static bool menu_handler_clap_trap(menu_input_t input) {
    switch (input) {
        case menu_input_left:
        case menu_input_right:
            userspace_config.gaming.clap_trap_enable = !userspace_config.gaming.clap_trap_enable;
            eeconfig_update_user_datablock(&userspace_config);
            return false;
        default:
            return true;
    }
}

void display_handler_clap_trap(char *text_buffer, size_t buffer_len) {
    snprintf(text_buffer, buffer_len - 1, "%s", userspace_config.gaming.clap_trap_enable ? "on" : "off");
}

static bool menu_handler_i2c_scanner(menu_input_t input) {
    switch (input) {
        case menu_input_left:
        case menu_input_right:
            userspace_config.debug.i2c_scanner_enable = !userspace_config.debug.i2c_scanner_enable;
            eeconfig_update_user_datablock(&userspace_config);
            return false;
        default:
            return true;
    }
}

void display_handler_i2c_scanner(char *text_buffer, size_t buffer_len) {
    snprintf(text_buffer, buffer_len - 1, "%s", userspace_config.debug.i2c_scanner_enable ? "on" : "off");
}

static bool menu_handler_scan_rate(menu_input_t input) {
    switch (input) {
        case menu_input_left:
        case menu_input_right:
            userspace_config.debug.matrix_scan_print = !userspace_config.debug.matrix_scan_print;
            eeconfig_update_user_datablock(&userspace_config);
            return false;
        default:
            return true;
    }
}

void display_handler_scan_rate(char *text_buffer, size_t buffer_len) {
    snprintf(text_buffer, buffer_len - 1, "%s", userspace_config.debug.matrix_scan_print ? "on" : "off");
}

menu_entry_t user_settings_option_entries[] = {
    {
        .flags                 = menu_flag_is_value,
        .text                  = "Overwatch Mode",
        .child.menu_handler    = menu_handler_overwatch_mode,
        .child.display_handler = display_handler_overwatch_mode,
    },
    {
        .flags                 = menu_flag_is_value,
        .text                  = "Gamepad 1<->2 Swap",
        .child.menu_handler    = menu_handler_gamepad_swap,
        .child.display_handler = display_handler_gamepad_swap,
    },
    {
        .flags                 = menu_flag_is_value,
        .text                  = "SOCD Cleaner",
        .child.menu_handler    = menu_handler_clap_trap,
        .child.display_handler = display_handler_clap_trap,
    },
    {
        .flags                 = menu_flag_is_value,
        .text                  = "I2C Scanner",
        .child.menu_handler    = menu_handler_i2c_scanner,
        .child.display_handler = display_handler_i2c_scanner,
    },
    {
        .flags                 = menu_flag_is_value,
        .text                  = "Matrix Scan Rate Print",
        .child.menu_handler    = menu_handler_scan_rate,
        .child.display_handler = display_handler_scan_rate,
    },
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Buy More

menu_entry_t buy_more_entries[] = {
#ifdef HAPTIC_ENABLE
    {
        .flags              = menu_flag_is_parent,
        .text               = "Haptic Settings",
        .parent.children    = haptic_entries,
        .parent.child_count = ARRAY_SIZE(haptic_entries),
    },
#endif // HAPTIC_ENABLE
    {
        .flags              = menu_flag_is_parent,
        .text               = "Keymap Config",
        .parent.children    = keymap_config_entries,
        .parent.child_count = ARRAY_SIZE(keymap_config_entries),
    },
#if defined(RTC_ENABLE) && !defined(DISPLAY_MENU_NO_RTC)
    {
        .flags              = menu_flag_is_parent,
        .text               = "RTC Config",
        .parent.children    = rtc_config_entries,
        .parent.child_count = ARRAY_SIZE(rtc_config_entries),
    },
#endif // RTC_ENABLE
    {
        .flags              = menu_flag_is_parent,
        .text               = "User Settings",
        .parent.children    = user_settings_option_entries,
        .parent.child_count = ARRAY_SIZE(user_settings_option_entries),
    },
};

menu_entry_t buy_more_entries_less[] = {
#ifdef HAPTIC_ENABLE
    {
        .flags              = menu_flag_is_parent,
        .text               = "Haptic Settings",
        .parent.children    = haptic_entries,
        .parent.child_count = ARRAY_SIZE(haptic_entries),
    },
#endif // HAPTIC_ENABLE
    {
        .flags              = menu_flag_is_parent,
        .text               = "Keymap Config",
        .parent.children    = keymap_config_entries,
        .parent.child_count = ARRAY_SIZE(keymap_config_entries),
    },
#if defined(RTC_ENABLE) && !defined(DISPLAY_MENU_NO_RTC)
    {
        .flags              = menu_flag_is_parent,
        .text               = "RTC Config",
        .parent.children    = rtc_config_entries,
        .parent.child_count = ARRAY_SIZE(rtc_config_entries),
    },
#endif // RTC_ENABLE
    {
        .flags              = menu_flag_is_parent,
        .text               = "User Settings",
        .parent.children    = user_settings_option_entries,
        .parent.child_count = ARRAY_SIZE(user_settings_option_entries),
    },
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Root menu

#define MENU_ENTRY_PARENT(display_text, child)                  \
    [__COUNTER__] = {.flags              = menu_flag_is_parent, \
                     .text               = display_text,        \
                     .parent.children    = child,               \
                     .parent.child_count = ARRAY_SIZE(child)}

menu_entry_t root_entries[] = {
    MENU_ENTRY_PARENT("Display Option", display_option_entries),
#ifdef UNICODE_COMMON_ENABLE
    MENU_ENTRY_PARENT("Unicode Settings", unicode_entries),
#endif // UNICODE_COMMON_ENABLE
#ifdef RGB_MATRIX_ENABLE
    MENU_ENTRY_PARENT("RGB Matrix Settings", rgb_matrix_entries),
#endif // RGB_MATRIX_ENABLE
#ifdef RGBLIGHT_ENABLE
    MENU_ENTRY_PARENT("RGB Light Settings", rgb_light_entries),
#endif // RGBLIGHT_ENABLE
#ifdef BACKLIGHT_ENABLE
    MENU_ENTRY_PARENT("Backlight Settings", backlight_entries),
#endif // BACKLIGHT_ENABLE
#ifdef AUDIO_ENABLE
    MENU_ENTRY_PARENT("Audio Settings", audio_entries),
#endif // AUDIO_ENABLE
#ifdef POINTING_DEVICE_ENABLE
    MENU_ENTRY_PARENT("Pointing Device Settings", pointing_entries),
#endif // POINTING_DEVICE_ENABLE
#if defined(HAPTIC_ENABLE) && __COUNTER__ < 7
#    define DISPLAY_MENU_NO_HAPTIC
    {
        .flags              = menu_flag_is_parent,
        .text               = "Haptic Settings",
        .parent.children    = haptic_entries,
        .parent.child_count = ARRAY_SIZE(haptic_entries),
    },
#endif
#if __COUNTER__ < 7
#    define DISPLAY_MENU_NO_KEYMAP
    {
        .flags              = menu_flag_is_parent,
        .text               = "Keymap Config",
        .parent.children    = keymap_config_entries,
        .parent.child_count = ARRAY_SIZE(keymap_config_entries),
    },
#endif
#if __COUNTER__ < 7 && defined(RTC_ENABLE)
#    define DISPLAY_MENU_NO_RTC
    {
        .flags              = menu_flag_is_parent,
        .text               = "RTC Config",
        .parent.children    = rtc_config_entries,
        .parent.child_count = ARRAY_SIZE(rtc_config_entries),
    },
#endif
#if __COUNTER__ < 8
    {
        .flags              = menu_flag_is_parent,
        .text               = "User Settings",
        .parent.children    = user_settings_option_entries,
        .parent.child_count = ARRAY_SIZE(user_settings_option_entries),
    },
#endif
#if __COUNTER__ >= 8 && (!defined(DISPLAY_MENU_NO_KEYMAP) || !defined(DISPLAY_MENU_NO_RTC))
    {
        .flags = menu_flag_is_parent,
        .text  = "More...",
#    if defined(DISPLAY_MENU_NO_KEYMAP)
        .parent.children    = buy_more_entries_less,
        .parent.child_count = ARRAY_SIZE(buy_more_entries_less),
#    else
        .parent.children    = buy_more_entries,
        .parent.child_count = ARRAY_SIZE(buy_more_entries),
#    endif
    },
#endif
};

_Static_assert(ARRAY_SIZE(root_entries) <= 8, "Too many root menu entries");

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Root Title

menu_entry_t root = {
    .flags              = menu_flag_is_parent,
    .text               = "Configuration",
    .parent.children    = root_entries,
    .parent.child_count = ARRAY_SIZE(root_entries),
};

menu_entry_t *get_current_menu(void) {
    if (user_runtime_state.menu_state.menu_stack[0] == 0xFF) {
        return &root;
    }

    menu_entry_t *entry = &root;
    for (int i = 0; i < sizeof(user_runtime_state.menu_state.menu_stack); ++i) {
        if (user_runtime_state.menu_state.menu_stack[i] == 0xFF) {
            return entry;
        }
        entry = &entry->parent.children[user_runtime_state.menu_state.menu_stack[i]];
    }

    return entry;
}

menu_entry_t *get_selected_menu_item(void) {
    return &(get_current_menu()->parent.children[user_runtime_state.menu_state.selected_child]);
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
            user_runtime_state.menu_state.is_in_menu = false;
            memset(user_runtime_state.menu_state.menu_stack, 0xFF, sizeof(user_runtime_state.menu_state.menu_stack));
            user_runtime_state.menu_state.selected_child = 0xFF;
            if (cancel_deferred_exec(menu_deferred_token)) {
                menu_deferred_token = INVALID_DEFERRED_TOKEN;
            }
             return false;
        case menu_input_back:
            // Iterate backwards through the stack and remove the last entry
            for (uint8_t i = 0; i < sizeof(user_runtime_state.menu_state.menu_stack); ++i) {
                if (user_runtime_state.menu_state
                        .menu_stack[sizeof(user_runtime_state.menu_state.menu_stack) - 1 - i] != 0xFF) {
                    user_runtime_state.menu_state.selected_child =
                        user_runtime_state.menu_state
                            .menu_stack[sizeof(user_runtime_state.menu_state.menu_stack) - 1 - i];
                    user_runtime_state.menu_state.menu_stack[sizeof(user_runtime_state.menu_state.menu_stack) - 1 - i] =
                        0xFF;
                    break;
                }

                // If we've dropped out of the last entry in the stack, exit the menu
                if (i == sizeof(user_runtime_state.menu_state.menu_stack) - 1) {
                    user_runtime_state.menu_state.is_in_menu     = false;
                    user_runtime_state.menu_state.selected_child = 0xFF;
                }
            }
            return false;
        case menu_input_enter:
            // Only attempt to enter the next menu if we're a parent object
            if (selected->flags & menu_flag_is_parent) {
                // Iterate forwards through the stack and add the selected entry
                for (uint8_t i = 0; i < sizeof(user_runtime_state.menu_state.menu_stack); ++i) {
                    if (user_runtime_state.menu_state.menu_stack[i] == 0xFF) {
                        user_runtime_state.menu_state.menu_stack[i]  = user_runtime_state.menu_state.selected_child;
                        user_runtime_state.menu_state.selected_child = 0;
                        break;
                    }
                }
            }
            return false;
        case menu_input_up:
            user_runtime_state.menu_state.selected_child =
                (user_runtime_state.menu_state.selected_child + menu->parent.child_count - 1) %
                menu->parent.child_count;
            return false;
        case menu_input_down:
            user_runtime_state.menu_state.selected_child =
                (user_runtime_state.menu_state.selected_child + menu->parent.child_count + 1) %
                menu->parent.child_count;
            return false;
        case menu_input_left:
        case menu_input_right:
            if (selected->flags & menu_flag_is_value) {
                user_runtime_state.menu_state.dirty = true;
                return selected->child.menu_handler(input);
            }
            return false;
        default:
            return false;
    }
}

bool process_record_menu(uint16_t keycode, keyrecord_t *record) {
    if (keycode == DISPLAY_MENU && record->event.pressed && !user_runtime_state.menu_state.is_in_menu) {
        user_runtime_state.menu_state.is_in_menu     = true;
        user_runtime_state.menu_state.selected_child = 0;
        menu_deferred_token               = defer_exec(DISPLAY_MENU_TIMEOUT, display_menu_timeout_handler, NULL);
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
    if (user_runtime_state.menu_state.is_in_menu) {
        if (record->event.pressed) {
            switch (keycode) {
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
                case KC_W:
                    return menu_handle_input(menu_input_up);
                case KC_DOWN:
                case KC_S:
                    return menu_handle_input(menu_input_down);
                case KC_LEFT:
                case KC_A:
                    return menu_handle_input(menu_input_left);
                case KC_RIGHT:
                case KC_D:
                    return menu_handle_input(menu_input_right);
                default:
                    return keep_processing;
            }
        }
        return keep_processing;
    }

    return true;
}

extern painter_font_handle_t font_thintel, font_mono, font_oled;

bool render_menu(painter_device_t display, uint16_t start_x, uint16_t start_y, uint16_t width, uint16_t height) {
    static menu_state_t last_state;
    if (memcmp(&last_state, &user_runtime_state.menu_state, sizeof(menu_state_t)) == 0) {
        return user_runtime_state.menu_state.is_in_menu;
    }

    user_runtime_state.menu_state.dirty = false;
    memcpy(&last_state, &user_runtime_state.menu_state, sizeof(menu_state_t));

    uint16_t render_width = width - start_x;

    if (user_runtime_state.menu_state.is_in_menu) {
        qp_rect(display, start_x, start_y, render_width - 1, height - 1, 0, 0, 0, true);

        // uint8_t       hue      = rgb_matrix_get_hue();
        menu_entry_t *menu     = get_current_menu();
        menu_entry_t *selected = get_selected_menu_item();
        dual_hsv_t    hsv      = painter_get_dual_hsv();

        uint16_t y = start_y;
        qp_rect(display, start_x, y, render_width, y + 6 + font_oled->line_height + 2, hsv.primary.h, hsv.primary.s,
                hsv.primary.v, true);
        qp_drawtext_recolor(display, start_x + 4, y + 4, font_oled, menu->text, 0, 0, 0, hsv.primary.h, hsv.primary.s,
                            hsv.primary.v);
        y += font_oled->line_height + 11;
        for (uint8_t i = 0; i < menu->parent.child_count; ++i) {
            menu_entry_t *child = &menu->parent.children[i];
            uint16_t      x     = start_x + 2 + qp_textwidth(font_oled, ">");
            if (child == selected) {
                qp_rect(display, start_x, y - 2, render_width, y + font_oled->line_height + 1, hsv.secondary.h,
                        hsv.secondary.s, hsv.secondary.v, true);
                qp_drawtext_recolor(display, start_x + 1, y, font_oled, ">", 0, 0, 0, hsv.secondary.h, hsv.secondary.s,
                                    hsv.secondary.v);
                x += qp_drawtext_recolor(display, x, y, font_oled,
                                         truncate_text(child->text, render_width, font_oled, false, true), 0, 0, 0,
                                         hsv.secondary.h, hsv.secondary.s, hsv.secondary.v);
            } else {
                x += qp_drawtext_recolor(display, x, y, font_oled,
                                         truncate_text(child->text, render_width, font_oled, false, true),
                                         hsv.primary.h, hsv.primary.s, hsv.primary.v, 0, 255, 0);
            }
            if (child->flags & menu_flag_is_parent) {
                if (child == selected) {
                    qp_drawtext_recolor(display, render_width - (qp_textwidth(font_oled, ">") + 2), y, font_oled, ">",
                                        0, 0, 0, hsv.secondary.h, hsv.secondary.s, hsv.secondary.v);
                } else {
                    qp_drawtext_recolor(display, render_width - (qp_textwidth(font_oled, ">") + 2), y, font_oled, ">",
                                        hsv.primary.h, hsv.primary.s, hsv.primary.v, 0, 0, 0);
                }
            }
            if (child->flags & menu_flag_is_value) {
                char buf[32] = {0}, val[29] = {0};
                child->child.display_handler(val, sizeof(val));
                snprintf(buf, sizeof(buf), ": %s", val);
                if (child == selected) {
                    qp_drawtext_recolor(display, x, y, font_oled, buf, 0, 0, 0, hsv.secondary.h, hsv.secondary.s,
                                        hsv.secondary.v);
                } else {
                    qp_drawtext_recolor(display, x, y, font_oled, buf, hsv.primary.h, hsv.primary.s, hsv.primary.v, 0,
                                        0, 0);
                }
            }
            y += font_oled->line_height + 2;
            qp_rect(display, start_x, y, render_width, y, hsv.primary.h, hsv.primary.s, hsv.primary.v, true);
            y += 3;
        }
        return true;
    }
    return false;
}

void display_menu_set_dirty(void) {
    user_runtime_state.menu_state.dirty = true;
}
