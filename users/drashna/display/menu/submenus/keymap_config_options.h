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
#endif // AUDIO_ENABLE

bool menu_handler_keycode_ctrl_caps(menu_input_t input) {
    switch (input) {
        case menu_input_left:
        case menu_input_right:
        case menu_input_enter:
            keymap_config.swap_control_capslock = !keymap_config.swap_control_capslock;
            eeconfig_update_keymap(&keymap_config);
            return false;
        default:
            return true;
    }
}

__attribute__((weak)) void display_handler_keycode_ctrl_caps(char *text_buffer, size_t buffer_len) {
    snprintf(text_buffer, buffer_len - 1, "%s", keymap_config.swap_control_capslock ? "swapped" : "normal");
}

bool menu_handler_keycode_alt_gui(menu_input_t input) {
    switch (input) {
        case menu_input_left:
        case menu_input_right:
        case menu_input_enter:
            keymap_config.swap_lalt_lgui = keymap_config.swap_ralt_rgui = !keymap_config.swap_lalt_lgui;
#ifdef AUDIO_ENABLE
            if (keymap_config.swap_ralt_rgui) {
                PLAY_SONG(ag_swap_song);
            } else {
                PLAY_SONG(ag_norm_song);
            }
#endif // AUDIO_ENABLE
            eeconfig_update_keymap(&keymap_config);
            return false;
        default:
            return true;
    }
}

__attribute__((weak)) void display_handler_keycode_alt_gui(char *text_buffer, size_t buffer_len) {
    snprintf(text_buffer, buffer_len - 1, "%s", keymap_config.swap_lalt_lgui ? "swapped" : "normal");
}

bool menu_handler_keycode_ctrl_gui(menu_input_t input) {
    switch (input) {
        case menu_input_left:
        case menu_input_right:
        case menu_input_enter:
            keymap_config.swap_lctl_lgui = keymap_config.swap_rctl_rgui = !keymap_config.swap_lctl_lgui;
#ifdef AUDIO_ENABLE
            if (keymap_config.swap_rctl_rgui) {
                PLAY_SONG(cg_swap_song);
            } else {
                PLAY_SONG(cg_norm_song);
            }
#endif // AUDIO_ENABLE
            eeconfig_update_keymap(&keymap_config);
            return false;
        default:
            return true;
    }
}

__attribute__((weak)) void display_handler_keycode_ctrl_gui(char *text_buffer, size_t buffer_len) {
    snprintf(text_buffer, buffer_len - 1, "%s", keymap_config.swap_lctl_lgui ? "swapped" : "normal");
}

bool menu_handler_keycode_disable_gui(menu_input_t input) {
    switch (input) {
        case menu_input_left:
        case menu_input_right:
        case menu_input_enter:
            keymap_config.no_gui = !keymap_config.no_gui;
            eeconfig_update_keymap(&keymap_config);
            return false;
        default:
            return true;
    }
}

__attribute__((weak)) void display_handler_keycode_disable_gui(char *text_buffer, size_t buffer_len) {
    snprintf(text_buffer, buffer_len - 1, "%s", keymap_config.no_gui ? "disabled" : "enabled");
}

__attribute__((unused)) bool menu_handler_keycode_grave_esc(menu_input_t input) {
    switch (input) {
        case menu_input_left:
        case menu_input_right:
        case menu_input_enter:
            keymap_config.swap_grave_esc = !keymap_config.swap_grave_esc;
            eeconfig_update_keymap(&keymap_config);
            return false;
        default:
            return true;
    }
}

__attribute__((unused, weak)) void display_handler_keycode_grave_esc(char *text_buffer, size_t buffer_len) {
    snprintf(text_buffer, buffer_len - 1, "%s", keymap_config.swap_grave_esc ? "swapped" : "normal");
}

__attribute__((unused)) bool menu_handler_keycode_bslash_bspc(menu_input_t input) {
    switch (input) {
        case menu_input_left:
        case menu_input_right:
        case menu_input_enter:
            keymap_config.swap_backslash_backspace = !keymap_config.swap_backslash_backspace;
            eeconfig_update_keymap(&keymap_config);
            return false;
        default:
            return true;
    }
}

__attribute__((unused, weak)) void display_handler_keycode_bslash_bspc(char *text_buffer, size_t buffer_len) {
    snprintf(text_buffer, buffer_len - 1, "%s", keymap_config.swap_backslash_backspace ? "swapped" : "normal");
}

bool menu_handler_keycode_nkro(menu_input_t input) {
    switch (input) {
        case menu_input_left:
        case menu_input_right:
        case menu_input_enter:
            keymap_config.nkro = !keymap_config.nkro;
            clear_keyboard(); // clear first buffer to prevent stuck keys
            eeconfig_update_keymap(&keymap_config);
            return false;
        default:
            return true;
    }
}

__attribute__((weak)) void display_handler_keycode_nkro(char *text_buffer, size_t buffer_len) {
    snprintf(text_buffer, buffer_len - 1, "%s", keymap_config.nkro ? "on" : "off");
}

bool menu_handler_keycode_oneshot(menu_input_t input) {
    switch (input) {
        case menu_input_left:
        case menu_input_right:
        case menu_input_enter:
            keymap_config.oneshot_enable = !keymap_config.oneshot_enable;
            eeconfig_update_keymap(&keymap_config);
            return false;
        default:
            return true;
    }
}

__attribute__((weak)) void display_handler_keycode_oneshot(char *text_buffer, size_t buffer_len) {
    snprintf(text_buffer, buffer_len - 1, "%s", keymap_config.oneshot_enable ? "on" : "off");
}

#if defined(AUTOCORRECT_ENABLE) || defined(COMMUNITY_MODULE_AUTOCORRECT_ENABLE)
#    include "autocorrect.h"

bool menu_handler_keycode_autocorrect(menu_input_t input) {
    switch (input) {
        case menu_input_left:
        case menu_input_right:
        case menu_input_enter:
            autocorrect_enable();
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
#endif // AUTOCORRECT_ENABLE || COMMUNITY_MODULE_AUTOCORRECT_ENABLE

menu_entry_t keymap_config_entries[] = {
    MENU_ENTRY_CHILD("Control <-> Capslock", "Ctl-Caps", keycode_ctrl_caps),
    MENU_ENTRY_CHILD("Alt <-> GUI", "Alt-GUI", keycode_alt_gui),
    MENU_ENTRY_CHILD("Control <-> GUI", "Ctl-GUI", keycode_ctrl_gui),
    MENU_ENTRY_CHILD("Grave <-> Escape", "Grv-Esc", keycode_grave_esc),
    MENU_ENTRY_CHILD("Backslash <-> Backspace", "/-Bspc", keycode_bslash_bspc),
    MENU_ENTRY_CHILD("GUI", "GUI", keycode_disable_gui),
    MENU_ENTRY_CHILD("N-Key Roll Over", "NKRO", keycode_nkro),
    MENU_ENTRY_CHILD("Oneshot Keys", "1SHOT", keycode_oneshot),
#if defined(AUTOCORRECT_ENABLE) || defined(COMMUNITY_MODULE_AUTOCORRECT_ENABLE)
    MENU_ENTRY_CHILD("Autocorrect", "AutoCorr", keycode_autocorrect),
    MENU_ENTRY_CHILD("Autocorrect Dictionary", "AutoDict", keycode_autocorrect_dict),
#endif // AUTOCORRECT_ENABLE || COMMUNITY_MODULE_AUTOCORRECT_ENABLE
};
