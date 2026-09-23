#define MENU_ENTRY_CHILD(display_text, short_display, name) \
    {                                                       \
        .flags                 = menu_flag_is_value,        \
        .text                  = display_text,              \
        .short_text            = short_display,             \
        .child.menu_handler    = menu_handler_##name,       \
        .child.display_handler = display_handler_##name,    \
    }

#define MENU_ENTRY_PARENT(display_text, short_display, child) \
    {                                                         \
        .flags              = menu_flag_is_parent,            \
        .text               = display_text,                   \
        .short_text         = short_display,                  \
        .parent.children    = child,                          \
        .parent.child_count = ARRAY_SIZE(child),              \
    }

#define MENU_ENTRY_MULTI(display_text, short_display, child_item, name)    \
    {                                                                      \
        .flags                 = menu_flag_is_parent | menu_flag_is_value, \
        .text                  = display_text,                             \
        .short_text            = short_display,                            \
        .child.menu_handler    = menu_handler_##name,                      \
        .child.display_handler = display_handler_##name,                   \
        .parent.children       = child_item,                               \
        .parent.child_count    = ARRAY_SIZE(child_item),                   \
    }

#include "drashna_util.h"
#include "drashna_runtime.h"
#include "drashna_names.h"
#include "drashna_layers.h"

#include "submenus/display_options.h"
#include "submenus/unicode_options.h"
#include "submenus/rgb_options.h"
#include "submenus/backlight_options.h"
#include "submenus/audio_options.h"
#include "submenus/haptic_options.h"
#include "submenus/pointing_device_options.h"
#include "submenus/keymap_config_options.h"
#include "submenus/rtc_options.h"
#include "submenus/user_settings_options.h"
#include "submenus/debugging_options.h"
#include "submenus/modules_options.h"
#ifdef SECRET_MENU_ENABLE
#    include "secrets_menu.h"
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Root menu

menu_entry_t root_entries[] = {
#ifdef QUANTUM_PAINTER_ENABLE
    MENU_ENTRY_PARENT("Display Option", "Display", display_option_entries),
#else // QUANTUM_PAINTER_ENABLE
    MENU_ENTRY_PARENT("Display Options", "Display", display_option_entries),
#endif // QUANTUM_PAINTER_ENABLE
#ifdef UNICODE_COMMON_ENABLE
#    ifdef COMMUNITY_MODULE_UNICODE_TYPING_ENABLE
    MENU_ENTRY_MULTI("Unicode Settings", "Unicode", unicode_entries, unicode),
#    else
    MENU_ENTRY_MULTI("Unicode Settings", "Unicode", unicode_entries, unicode_typing),
#    endif
#endif // UNICODE_COMMON_ENABLE
#ifdef RGB_MATRIX_ENABLE
    MENU_ENTRY_MULTI("RGB Matrix Settings", "RGB Matrix", rgb_matrix_entries, rm_enabled),
#endif // RGB_MATRIX_ENABLE
#ifdef RGBLIGHT_ENABLE
    MENU_ENTRY_MULTI("RGB Light Settings", "RGB Light", rgb_light_entries, rgbenabled),
#endif // RGBLIGHT_ENABLE
#ifdef BACKLIGHT_ENABLE
    MENU_ENTRY_MULTI("Backlight Settings", "Backlight", backlight_entries, bl_level),
#endif // BACKLIGHT_ENABLE
#ifdef AUDIO_ENABLE
    MENU_ENTRY_MULTI("Audio Settings", "Audio", audio_entries, audio_enabled),
#endif // AUDIO_ENABLE
#ifdef POINTING_DEVICE_ENABLE
    MENU_ENTRY_PARENT("Pointing Device Settings", "Pointing", pointing_entries),
#endif // POINTING_DEVICE_ENABLE
#if defined(HAPTIC_ENABLE)
    MENU_ENTRY_MULTI("Haptic Settings", "Haptics", haptic_entries, haptic_enabled),
#endif // HAPTIC_ENABLE
    MENU_ENTRY_PARENT("Keymap Settings", "Keymap", keymap_config_entries),
    MENU_ENTRY_PARENT("User Settings", "User", user_settings_option_entries),
    MENU_ENTRY_PARENT("Community Modules", "Modules", community_modules_settings_option_entries),
#ifdef SECRET_MENU_ENABLE
    MENU_ENTRY_PARENT("Secret Settings", "Secret", secret_entries),
#endif
    MENU_ENTRY_PARENT("Debug Settings", "Debug", debug_entries),
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Root Title

menu_entry_t root = {
    .flags              = menu_flag_is_parent,
    .text               = "Configuration",
    .parent.children    = root_entries,
    .parent.child_count = ARRAY_SIZE(root_entries),
};
