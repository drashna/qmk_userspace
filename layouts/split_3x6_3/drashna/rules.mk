# Build Options
#   change to "no" to disable the options
#

BOOTMAGIC_ENABLE           = yes # Enable Bootmagic Lite
MOUSEKEY_ENABLE            = no  # Mouse keys
EXTRAKEY_ENABLE             = yes # Audio control and System control
CONSOLE_ENABLE              = no  # Console for debug
COMMAND_ENABLE              = no  # Commands for debug and configuration
NKRO_ENABLE                 = yes # Nkey Rollover - if this doesn't work, see here: https://github.com/tmk/tmk_keyboard/wiki/FAQ#nkro-doesnt-work
BACKLIGHT_ENABLE            = no  # Enable keyboard backlight functionality
AUDIO_ENABLE                = no  # Audio output
RGBLIGHT_ENABLE             = no  # Enable WS2812 RGB underlight.
SWAP_HANDS_ENABLE           = no  # Enable one-hand typing
OS_DETECTION_ENABLE         = no
CUSTOM_UNICODE_ENABLE       = no
CUSTOM_POINTING_DEVICE      = no
CUSTOM_SPLIT_TRANSPORT_SYNC = no
CUSTOM_OLED_DRIVER          = no
CUSTOM_RGB_MATRIX           = no
CUSTOM_RGBLIGHT             = no

ifeq ($(strip $(KEYBOARD)), crkbd/rev1)
    CONVERT_TO             = proton_c
    OLED_ENABLE            = yes
    RGB_MATRIX_ENABLE      = yes
    HAPTIC_ENABLE          = no
endif
ifeq ($(strip $(KEYBOARD)), splitkb/aurora/corne/rev1)
    CONVERT_TO             = proton_c
endif

USE_ARM_CONFIG ?= no
ifeq ($(strip $(CONVERT_TO)), proton_c)
    USE_ARM_CONFIG = yes
    BOOTLOADER     = tinyuf2
endif
ifeq ($(strip $(MCU)), STM32F303)
    USE_ARM_CONFIG = yes
endif
ifeq ($(strip $(MCU)), STM32F401)
    USE_ARM_CONFIG = yes
endif
ifeq ($(strip $(MCU)), STM32F411)
    USE_ARM_CONFIG = yes
endif

ifeq ($(strip $(MCU)), atmega32u4)
    BOOTLOADER             = qmk-hid
    BOOTLOADER_SIZE        = 512
endif

ifeq ($(strip $(USE_ARM_CONFIG)), yes)
    LTO_ENABLE                  = no
    LTO_SUPPORTED               = no
    HAPTIC_ENABLE               = no
    WS2812_DRIVER               = pwm
    SERIAL_DRIVER               = usart
    WPM_ENABLE                  = yes
    AUTOCORRECT_ENABLE          = yes
    CAPS_WORD_ENABLE            = yes
    OS_DETECTION_ENABLE         = yes
    CUSTOM_UNICODE_ENABLE       = yes
    CUSTOM_POINTING_DEVICE      = yes
    CUSTOM_SPLIT_TRANSPORT_SYNC = yes
    CUSTOM_OLED_DRIVER          = yes
    CUSTOM_RGB_MATRIX           = yes
    CUSTOM_RGBLIGHT             = yes
    KEYMAP_JSON                 = $(MAIN_KEYMAP_PATH_5)/keymap_arm.json
endif
