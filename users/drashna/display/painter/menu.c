// Copyright 2018-2024 Nick Brassel (@tzarc)
// Copyright 2024 Drashna (@drashna)
// SPDX-License-Identifier: GPL-2.0-or-later

#include "drashna_runtime.h"
#include "drashna_names.h"
#include "drashna_layers.h"
#include <printf.h>
#include "display/painter/menu.h"
#include "display/painter/painter.h"
#include "keyrecords/process_records.h"
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

#define MENU_ENTRY_CHILD(display_text, name)             \
    {                                                    \
        .flags                 = menu_flag_is_value,     \
        .text                  = display_text,           \
        .child.menu_handler    = menu_handler_##name,    \
        .child.display_handler = display_handler_##name, \
    }

#define MENU_ENTRY_PARENT(display_text, child)     \
    {                                              \
        .flags              = menu_flag_is_parent, \
        .text               = display_text,        \
        .parent.children    = child,               \
        .parent.child_count = ARRAY_SIZE(child),   \
    }

#define MENU_ENTRY_MULTI(display_text, child, name)                        \
    {                                                                      \
        .flags                 = menu_flag_is_parent | menu_flag_is_value, \
        .text                  = display_text,                             \
        .child.menu_handler    = menu_handler_##name,                      \
        .child.display_handler = display_handler_##name,                   \
        .parent.children       = child,                                    \
        .parent.child_count    = ARRAY_SIZE(child),                        \
    }

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

static bool menu_handler_display_image(menu_input_t input) {
    switch (input) {
        case menu_input_left:
            userspace_config.painter.display_logo = (userspace_config.painter.display_logo - 1) % 10;
            if (userspace_config.painter.display_logo > 9) {
                userspace_config.painter.display_logo = 9;
            }
            eeconfig_update_user_datablock(&userspace_config);
            return false;
        case menu_input_right:
            userspace_config.painter.display_logo = (userspace_config.painter.display_logo + 1) % 10;
            if (userspace_config.painter.display_logo > 9) {
                userspace_config.painter.display_logo = 0;
            }
            eeconfig_update_user_datablock(&userspace_config);
            return false;
        default:
            return true;
    }
}

void display_handler_display_image(char *text_buffer, size_t buffer_len) {
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

static bool menu_handler_display_hue(menu_input_t input, bool painter_is_primary) {
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
void display_handler_display_hue(char *text_buffer, size_t buffer_len, bool painter_is_primary) {
    snprintf(text_buffer, buffer_len - 1, "%d", painter_get_hue(painter_is_primary));
}

static bool menu_handler_display_sat(menu_input_t input, bool painter_is_primary) {
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

void display_handler_display_sat(char *text_buffer, size_t buffer_len, bool painter_is_primary) {
    snprintf(text_buffer, buffer_len - 1, "%d", painter_get_sat(painter_is_primary));
}

static bool menu_handler_display_val(menu_input_t input, bool painter_is_primary) {
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

void display_handler_display_val(char *text_buffer, size_t buffer_len, bool painter_is_primary) {
    snprintf(text_buffer, buffer_len - 1, "%d", painter_get_val(painter_is_primary));
}

static bool menu_handler_display_hue_primary(menu_input_t input) {
    return menu_handler_display_hue(input, true);
}

void display_handler_display_hue_primary(char *text_buffer, size_t buffer_len) {
    display_handler_display_hue(text_buffer, buffer_len, true);
}

static bool menu_handler_display_sat_primary(menu_input_t input) {
    return menu_handler_display_sat(input, true);
}

void display_handler_display_sat_primary(char *text_buffer, size_t buffer_len) {
    display_handler_display_sat(text_buffer, buffer_len, true);
}

static bool menu_handler_display_val_primary(menu_input_t input) {
    return menu_handler_display_val(input, true);
}

void display_handler_display_val_primary(char *text_buffer, size_t buffer_len) {
    display_handler_display_val(text_buffer, buffer_len, true);
}

static bool menu_handler_display_hue_secondary(menu_input_t input) {
    return menu_handler_display_hue(input, false);
}

void display_handler_display_hue_secondary(char *text_buffer, size_t buffer_len) {
    display_handler_display_hue(text_buffer, buffer_len, false);
}

static bool menu_handler_display_sat_secondary(menu_input_t input) {
    return menu_handler_display_sat(input, false);
}

void display_handler_display_sat_secondary(char *text_buffer, size_t buffer_len) {
    display_handler_display_sat(text_buffer, buffer_len, false);
}

static bool menu_handler_display_val_secondary(menu_input_t input) {
    return menu_handler_display_val(input, false);
}

void display_handler_display_val_secondary(char *text_buffer, size_t buffer_len) {
    display_handler_display_val(text_buffer, buffer_len, false);
}

menu_entry_t display_option_entries[] = {
    MENU_ENTRY_CHILD("Display Option", display),
    MENU_ENTRY_CHILD("Image", display_image),
    MENU_ENTRY_CHILD("Rotation", display_rotation),
    MENU_ENTRY_CHILD("Inverted", display_inverted),
    MENU_ENTRY_CHILD("Primary Hue", display_hue_primary),
    MENU_ENTRY_CHILD("Primary Saturation", display_sat_primary),
    MENU_ENTRY_CHILD("Primary Value", display_val_primary),
    MENU_ENTRY_CHILD("Secondary Hue", display_hue_secondary),
    MENU_ENTRY_CHILD("Secondary Saturation", display_sat_secondary),
    MENU_ENTRY_CHILD("Secondary Value", display_val_secondary),
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
    MENU_ENTRY_CHILD("Unicode mode", unicode),
    MENU_ENTRY_CHILD("Unicode Typing Mode", unicode_typing),
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
#    include "rgb/rgb_matrix_stuff.h"

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
            rgblight_sethsv(rgblight_get_hue(), rgblight_get_sat(), rgb_matrix_get_val());
#    endif
            return false;
        case menu_input_right:
            rgb_matrix_increase_val();
#    if defined(RGBLIGHT_CUSTOM)
            rgblight_sethsv(rgblight_get_hue(), rgblight_get_sat(), rgb_matrix_get_val());
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
    MENU_ENTRY_CHILD("RGB Enabled", rm_enabled),
    MENU_ENTRY_CHILD("RGB Mode", rm_mode),
    MENU_ENTRY_CHILD("RGB Hue", rm_hue),
    MENU_ENTRY_CHILD("RGB Saturation", rm_sat),
    MENU_ENTRY_CHILD("RGB Value", rm_val),
    MENU_ENTRY_CHILD("RGB Speed", rm_speed),
    MENU_ENTRY_CHILD("Layer Indication", rgb_layer),
    MENU_ENTRY_CHILD("Idle Animation", rgb_idle),
};
#endif // RGB_MATRIX_ENABLE

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// RGB Light

#ifdef RGBLIGHT_ENABLE
#    include "rgb/rgb_stuff.h"

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
    snprintf(text_buffer, buffer_len - 1, "%d", rgblight_get_val());
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
    MENU_ENTRY_CHILD("RGB Enabled", rgbenabled),
    MENU_ENTRY_CHILD("RGB Mode", rgbmode),
    MENU_ENTRY_CHILD("RGB Hue", rgbhue),
    MENU_ENTRY_CHILD("RGB Saturation", rgbsat),
    MENU_ENTRY_CHILD("RGB Value", rgbval),
    MENU_ENTRY_CHILD("RGB Speed", rgbspeed),
    MENU_ENTRY_CHILD("Layer Indication", rgb_layer),
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
    MENU_ENTRY_CHILD("Backlight Enabled", bl_enabled),
    MENU_ENTRY_CHILD("Backlight Level", bl_level),
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
    snprintf(text_buffer, buffer_len - 1, "%s", is_clicky_on() ? "enabled" : "disabled");
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
            eeconfig_update_user_datablock(&userspace_config);
            void set_doom_song(layer_state_t);
            set_doom_song(layer_state);
            return false;
        default:
            return true;
    }
}

void display_handler_gaming_song_enabled(char *text_buffer, size_t buffer_len) {
    snprintf(text_buffer, buffer_len - 1, "%s", userspace_config.gaming.song_enable ? "enabled" : "disabled");
}

static bool menu_handler_audio_mouse_clicky(menu_input_t input) {
    switch (input) {
        case menu_input_left:
        case menu_input_right:
            userspace_config.audio_mouse_clicky = !userspace_config.audio_mouse_clicky;
            eeconfig_update_user_datablock(&userspace_config);
            return false;
        default:
            return true;
    }
}

void display_handler_audio_mouse_clicky(char *text_buffer, size_t buffer_len) {
    snprintf(text_buffer, buffer_len - 1, "%s", userspace_config.audio_mouse_clicky ? "enabled" : "disabled");
}

menu_entry_t audio_entries[] = {
    MENU_ENTRY_CHILD("Audio", audio_enabled),
    MENU_ENTRY_CHILD("Music Mode", music_enabled),
    MENU_ENTRY_CHILD("Clicky", audio_clicky_enabled),
    MENU_ENTRY_CHILD("Clicky Frequency", audio_clicky_freq),
    MENU_ENTRY_CHILD("Gaming Song", gaming_song_enabled),
#    ifdef POINTING_DEVICE_ENABLE
    MENU_ENTRY_CHILD("Mouse Clicky", audio_mouse_clicky),
#    endif
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
    MENU_ENTRY_CHILD("Haptic Enabled", haptic_enabled),
    MENU_ENTRY_CHILD("Haptic Mode", haptic_mode),
    MENU_ENTRY_CHILD("Feedback Mode", feedback_mode),
#    ifdef HAPTIC_SOLENOID
    MENU_ENTRY_CHILD("Buzz", haptic_buzz),
    MENU_ENTRY_CHILD("Dwell", haptic_dwell),
#    endif // HAPTIC_SOLENOID
#    ifdef HAPTIC_DRV2605L
    MENU_ENTRY_CHILD("Continuous", haptic_love_mode),
    MENU_ENTRY_CHILD("Continuous Amplitude", haptic_love_intensity),
#    endif // HAPTIC_DRV2605L
};
#endif // HAPTIC_ENABLE

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Pointing Device

#ifdef POINTING_DEVICE_ENABLE
#    include "pointing/pointing.h"

static bool menu_handler_auto_mouse_enable(menu_input_t input) {
    switch (input) {
        case menu_input_left:
        case menu_input_right:
            userspace_config.pointing.auto_mouse_layer_enable = !userspace_config.pointing.auto_mouse_layer_enable;
            eeconfig_update_user_datablock(&userspace_config);
            set_auto_mouse_enable(userspace_config.pointing.auto_mouse_layer_enable);
            return false;
        default:
            return true;
    }
}

void display_handler_auto_mouse_enable(char *text_buffer, size_t buffer_len) {
    snprintf(text_buffer, buffer_len - 1, "%s",
             userspace_config.pointing.auto_mouse_layer_enable ? "enabled" : "disabled");
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
#        include QMK_KEYBOARD_H

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
    MENU_ENTRY_CHILD("Acceleration", mouse_accel_toggle),
    MENU_ENTRY_CHILD("Takeoff", mouse_accel_takeoff),
    MENU_ENTRY_CHILD("Growth Rate", mouse_accel_growth_rate),
    MENU_ENTRY_CHILD("Offset", mouse_accel_offset),
    MENU_ENTRY_CHILD("Limit", mouse_accel_limit),
};

menu_entry_t pointing_entries[] = {
    MENU_ENTRY_PARENT("Mouse Acceleration", pointing_acceleration_entries),
#    if defined(KEYBOARD_handwired_tractyl_manuform) || defined(KEYBOARD_bastardkb_charybdis)
    MENU_ENTRY_CHILD("DPI Config", dpi_config),
#    endif // KEYBOARD_handwired_tractyl_manuform || KEYBOARD_bastardkb_charybdis
    MENU_ENTRY_CHILD("Auto Mouse", auto_mouse_enable),
    MENU_ENTRY_CHILD("Auto Mouse Layer", auto_mouse_layer),
    MENU_ENTRY_CHILD("Mouse Jiggler", mouse_jiggler),
#    ifdef AUDIO_ENABLE
    MENU_ENTRY_CHILD("Mouse Clicky", audio_mouse_clicky),
#    endif
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
    snprintf(text_buffer, buffer_len - 1, "%s", keymap_config.no_gui ? "disabled" : "enabled");
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
    MENU_ENTRY_CHILD("Control <-> Capslock", keycode_ctrl_caps),
    MENU_ENTRY_CHILD("Alt <-> GUI", keycode_alt_gui),
    MENU_ENTRY_CHILD("Control <-> GUI", keycode_ctrl_gui),
    MENU_ENTRY_CHILD("Grave <-> Escape", keycode_grave_esc),
    MENU_ENTRY_CHILD("Backslash <-> Backspace", keycode_bslash_bspc),
    MENU_ENTRY_CHILD("GUI", keycode_disable_gui),
    MENU_ENTRY_CHILD("N-Key Roll Over", keycode_nkro),
    MENU_ENTRY_CHILD("Oneshot Keys", keycode_oneshot),
    MENU_ENTRY_CHILD("Autocorrect", keycode_autocorrect),
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
    MENU_ENTRY_CHILD("Year", rtc_year),
    MENU_ENTRY_CHILD("Month", rtc_month),
    MENU_ENTRY_CHILD("Date", rtc_date),
    MENU_ENTRY_CHILD("Hour", rtc_hour),
    MENU_ENTRY_CHILD("Minute", rtc_minute),
    MENU_ENTRY_CHILD("Second", rtc_second),
    MENU_ENTRY_CHILD("12/24 Hour Format", rtc_hour_format),
    MENU_ENTRY_CHILD("DST", rtc_dst),
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
    MENU_ENTRY_CHILD("Overwatch Mode", overwatch_mode),
    MENU_ENTRY_CHILD("Gamepad 1<->2 Swap", gamepad_swap),
    MENU_ENTRY_CHILD("SOCD Cleaner", clap_trap),
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Debugging

static bool menu_handler_debugging_enable(menu_input_t input) {
    switch (input) {
        case menu_input_left:
        case menu_input_right:
            debug_enable = !debug_enable;
            return false;
        default:
            return true;
    }
}

void display_handler_debugging_enable(char *text_buffer, size_t buffer_len) {
    snprintf(text_buffer, buffer_len - 1, "%s", debug_enable ? "enabled" : "disabled");
}

static bool menu_handler_keyboard_debugging(menu_input_t input) {
    switch (input) {
        case menu_input_left:
        case menu_input_right:
            debug_keyboard = !debug_keyboard;
            return false;
        default:
            return true;
    }
}

void display_handler_keyboard_debugging(char *text_buffer, size_t buffer_len) {
    snprintf(text_buffer, buffer_len - 1, "%s", debug_keyboard ? "enabled" : "disabled");
}

static bool menu_handler_matrix_debugging(menu_input_t input) {
    switch (input) {
        case menu_input_left:
        case menu_input_right:
            debug_matrix = !debug_matrix;
            return false;
        default:
            return true;
    }
}

void display_handler_matrix_debugging(char *text_buffer, size_t buffer_len) {
    snprintf(text_buffer, buffer_len - 1, "%s", debug_matrix ? "enabled" : "disabled");
}

static bool menu_handler_mouse_debugging(menu_input_t input) {
    switch (input) {
        case menu_input_left:
        case menu_input_right:
            debug_mouse = !debug_mouse;
            return false;
        default:
            return true;
    }
}

void display_handler_mouse_debugging(char *text_buffer, size_t buffer_len) {
    snprintf(text_buffer, buffer_len - 1, "%s", debug_mouse ? "enabled" : "disabled");
}

static bool menu_handler_pointing_debugging(menu_input_t input) {
    switch (input) {
        case menu_input_left:
        case menu_input_right:
            debug_pointing = !debug_pointing;
            return false;
        default:
            return true;
    }
}

void display_handler_pointing_debugging(char *text_buffer, size_t buffer_len) {
    snprintf(text_buffer, buffer_len - 1, "%s", debug_pointing ? "enabled" : "disabled");
}

static bool menu_handler_action_debugging(menu_input_t input) {
    switch (input) {
        case menu_input_left:
        case menu_input_right:
            debug_action = !debug_action;
            return false;
        default:
            return true;
    }
}

void display_handler_action_debugging(char *text_buffer, size_t buffer_len) {
    snprintf(text_buffer, buffer_len - 1, "%s", debug_action ? "enabled" : "disabled");
}

static bool menu_handler_split_serial_debugging(menu_input_t input) {
    switch (input) {
        case menu_input_left:
        case menu_input_right:
            debug_serial = !debug_serial;
            return false;
        default:
            return true;
    }
}

void display_handler_split_serial_debugging(char *text_buffer, size_t buffer_len) {
    snprintf(text_buffer, buffer_len - 1, "%s", debug_serial ? "enabled" : "disabled");
}

static bool menu_handler_quantum_painter_debugging(menu_input_t input) {
    switch (input) {
        case menu_input_left:
        case menu_input_right:
            debug_quantum_painter = !debug_quantum_painter;
            return false;
        default:
            return true;
    }
}

void display_handler_quantum_painter_debugging(char *text_buffer, size_t buffer_len) {
    snprintf(text_buffer, buffer_len - 1, "%s", debug_quantum_painter ? "enabled" : "disabled");
}

menu_entry_t debug_entries[] = {
    MENU_ENTRY_CHILD("Debugging", debugging_enable), // force formatting
    MENU_ENTRY_CHILD("Keyboard Debugging", keyboard_debugging),
    MENU_ENTRY_CHILD("Matrix Debugging", matrix_debugging),
    MENU_ENTRY_CHILD("Mouse Debugging", mouse_debugging),
    MENU_ENTRY_CHILD("Pointing Device Debugging", pointing_debugging),
    MENU_ENTRY_CHILD("Action Debugging", action_debugging),
    MENU_ENTRY_CHILD("Split Serial Debugging", split_serial_debugging),
    MENU_ENTRY_CHILD("Quantum Painter Debugging", quantum_painter_debugging),
    MENU_ENTRY_CHILD("I2C Scanner", i2c_scanner),
    MENU_ENTRY_CHILD("Matrix Scan Rate Print", scan_rate),
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Root menu

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
#if defined(HAPTIC_ENABLE)
    MENU_ENTRY_PARENT("Haptic Settings", haptic_entries),
#endif // HAPTIC_ENABLE
    MENU_ENTRY_PARENT("Keymap Settings", keymap_config_entries),
#if defined(RTC_ENABLE)
    MENU_ENTRY_PARENT("RTC Settings", rtc_config_entries),
#endif // RTC_ENABLE
    MENU_ENTRY_PARENT("User Settings", user_settings_option_entries),
    MENU_ENTRY_PARENT("Debug Settings", debug_entries),
};

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
            if (selected->flags & menu_flag_is_value) {
                user_runtime_state.menu_state.dirty = true;
                return selected->child.menu_handler(menu_input_right);
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

__attribute__((weak)) bool process_record_menu_user(uint16_t keycode, bool keep_processing) {
    const bool is_qwerty  = get_highest_layer(default_layer_state) == _QWERTY,
               is_dvorak  = get_highest_layer(default_layer_state) == _DVORAK,
               is_colemak = get_highest_layer(default_layer_state) == _COLEMAK ||
                            get_highest_layer(default_layer_state) == _COLEMAK_DH;

    switch (keycode) {
        case KC_D:
            if (is_qwerty) {
                return menu_handle_input(menu_input_down);
            }
            return keep_processing;
        case KC_E:
            if (is_qwerty) {
                return menu_handle_input(menu_input_up);
            } else if (is_dvorak) {
                return menu_handle_input(menu_input_down);
            }
            return keep_processing;
        case KC_F:
            if (is_qwerty) {
                return menu_handle_input(menu_input_right);
            } else if (is_colemak) {
                return menu_handle_input(menu_input_up);
            }
            return keep_processing;
        case KC_O:
            if (is_dvorak) {
                return menu_handle_input(menu_input_left);
            }
            return keep_processing;
        case KC_R:
            if (is_colemak) {
                return menu_handle_input(menu_input_left);
            }
            return keep_processing;
        case KC_S:
            if (is_qwerty) {
                return menu_handle_input(menu_input_left);
            } else if (is_colemak) {
                return menu_handle_input(menu_input_down);
            }
            return keep_processing;
        case KC_T:
            if (is_colemak) {
                return menu_handle_input(menu_input_right);
            }
            return keep_processing;
        case KC_U:
            if (is_dvorak) {
                return menu_handle_input(menu_input_right);
            }
            return keep_processing;
        case KC_DOT:
            if (is_dvorak) {
                return menu_handle_input(menu_input_right);
            }
            return keep_processing;
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
            return menu_handle_input(menu_input_up);
        case KC_DOWN:
            return menu_handle_input(menu_input_down);
        case KC_LEFT:
            return menu_handle_input(menu_input_left);
        case KC_RIGHT:
            return menu_handle_input(menu_input_right);
        default:
            return keep_processing;
    }
}

bool process_record_menu(uint16_t keycode, keyrecord_t *record) {
    if (keycode == DISPLAY_MENU && record->event.pressed && !user_runtime_state.menu_state.is_in_menu) {
        user_runtime_state.menu_state.is_in_menu     = true;
        user_runtime_state.menu_state.selected_child = 0;
        menu_deferred_token = defer_exec(DISPLAY_MENU_TIMEOUT, display_menu_timeout_handler, NULL);
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
            return process_record_menu_user(keycode, keep_processing);
        }
        return keep_processing;
    }

    return true;
}

extern painter_font_handle_t font_thintel, font_mono, font_oled;
bool render_menu(painter_device_t display, uint16_t start_x, uint16_t start_y, uint16_t width, uint16_t height) {
    static menu_state_t last_state;
    static uint8_t      scroll_offset = 0;

    if (memcmp(&last_state, &user_runtime_state.menu_state, sizeof(menu_state_t)) == 0) {
        return user_runtime_state.menu_state.is_in_menu;
    }

    user_runtime_state.menu_state.dirty = false;
    memcpy(&last_state, &user_runtime_state.menu_state, sizeof(menu_state_t));

    uint16_t render_width = width - start_x;

    if (user_runtime_state.menu_state.is_in_menu) {
        qp_rect(display, start_x, start_y, render_width - 1, height - 1, 0, 0, 0, true);

        menu_entry_t *menu     = get_current_menu();
        menu_entry_t *selected = get_selected_menu_item();
        dual_hsv_t    hsv      = painter_get_dual_hsv();

        uint16_t y = start_y;
        qp_rect(display, start_x, y, render_width, y + 6 + font_oled->line_height + 2, hsv.primary.h, hsv.primary.s,
                hsv.primary.v, true);
        qp_drawtext_recolor(display, start_x + 4, y + 4, font_oled, menu->text, 0, 0, 0, hsv.primary.h, hsv.primary.s,
                            hsv.primary.v);
        y += font_oled->line_height + 11;

        uint8_t visible_entries = (height - y) / (font_oled->line_height + 5);
        if (menu->parent.child_count > visible_entries) {
            if (user_runtime_state.menu_state.selected_child >= scroll_offset + visible_entries &&
                user_runtime_state.menu_state.selected_child < menu->parent.child_count - 1) {
                scroll_offset = user_runtime_state.menu_state.selected_child - visible_entries + 1;
            } else if (user_runtime_state.menu_state.selected_child < scroll_offset + 1) {
                if (user_runtime_state.menu_state.selected_child != 0) {
                    scroll_offset = user_runtime_state.menu_state.selected_child - 1;
                } else {
                    scroll_offset = 0;
                }
            } else if (user_runtime_state.menu_state.selected_child == menu->parent.child_count - 1) {
                scroll_offset = menu->parent.child_count - visible_entries - 1;
            }
        } else {
            scroll_offset = 0;
        }

        for (uint8_t i = scroll_offset; i < menu->parent.child_count && i <= (scroll_offset + visible_entries); ++i) {
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
                if ((i == scroll_offset && scroll_offset > 0) ||
                    (i == scroll_offset + visible_entries &&
                     scroll_offset + visible_entries + 1 < menu->parent.child_count)) {
                    qp_drawtext_recolor(display, start_x + 1, y, font_oled, "+", hsv.primary.h, hsv.primary.s,
                                        hsv.primary.v, 0, 255, 0);
                }
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
