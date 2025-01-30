BOOTMAGIC_ENABLE   = yes      # Enable Bootmagic Lite
TAP_DANCE_ENABLE   = no
COMMAND_ENABLE     = no  # Commands for debug and configuration
CONSOLE_ENABLE     = no

ifneq ($(filter $(KEYBOARD), ergodox_ez/base ergodox_ez/glow ergodox_ez/shine),)
    RGBLIGHT_ENABLE            = yes
    RGB_MATRIX_ENABLE          = yes
    INDICATOR_LIGHTS           = no
    RGBLIGHT_STARTUP_ANIMATION = no
    NO_SECRETS                 = yes
    #OS_DETECTION_ENABLE        = no
    DEBOUNCE_TYPE              = sym_defer_g
    DEFERRED_EXEC_ENABLE       = no
    KEYBOARD_LOCK_ENABLE       = no
    KEYLOGGER_ENABLE           = no
endif

UNICODE_ENABLE        = no
UNICDOEMAP_ENABLE     = no
CUSTOM_UNICODE_ENABLE = no

ifeq ($(strip $(KEYBOARD)), hotdox76v2)
    OLED_ENABLE = no
    RGB_MATRIX_ENABLE = no
endif
