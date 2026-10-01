////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Unicode

#ifdef UNICODE_COMMON_ENABLE
#    include "unicode.h"
#    include "keyrecords/unicode.h"
#    ifdef COMMUNITY_MODULE_UNICODE_TYPING_ENABLE
#        include "unicode_typing.h"
#    endif

bool menu_handler_unicode(menu_input_t input) {
    switch (input) {
        case menu_input_left:
            unicode_input_mode_step_reverse();
            return false;
        case menu_input_right:
        case menu_input_enter:
            unicode_input_mode_step();
            return false;
        default:
            return true;
    }
}

__attribute__((weak)) void display_handler_unicode(char *text_buffer, size_t buffer_len) {
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

#    ifdef COMMUNITY_MODULE_UNICODE_TYPING_ENABLE
bool menu_handler_unicode_typing(menu_input_t input) {
    switch (input) {
        case menu_input_left:
            userspace_runtime_state.unicode.typing_mode =
                (userspace_runtime_state.unicode.typing_mode - 1) % UNCODES_MODE_END;
            if (userspace_runtime_state.unicode.typing_mode >= UNCODES_MODE_END) {
                userspace_runtime_state.unicode.typing_mode = UNCODES_MODE_END - 1;
            }
            set_unicode_typing_mode(userspace_runtime_state.unicode.typing_mode);
            return false;
        case menu_input_right:
        case menu_input_enter:
            userspace_runtime_state.unicode.typing_mode =
                (userspace_runtime_state.unicode.typing_mode + 1) % UNCODES_MODE_END;
            if (userspace_runtime_state.unicode.typing_mode >= UNCODES_MODE_END) {
                userspace_runtime_state.unicode.typing_mode = 0;
            }
            set_unicode_typing_mode(userspace_runtime_state.unicode.typing_mode);
            return false;
        default:
            return true;
    }
}
__attribute__((weak)) void display_handler_unicode_typing(char *text_buffer, size_t buffer_len) {
    strncpy(text_buffer, get_unicode_typing_mode_str(get_unicode_typing_mode()), buffer_len);
}
#    endif

menu_entry_t unicode_entries[] = {
    DISPLAY_MENU_ENTRY_CHILD("Unicode mode", "Mode", unicode),
#    ifdef COMMUNITY_MODULE_UNICODE_TYPING_ENABLE
    DISPLAY_MENU_ENTRY_CHILD("Unicode Typing Mode", "Typing", unicode_typing),
#    endif
};
#endif // UNICODE_COMMON_ENABLE
