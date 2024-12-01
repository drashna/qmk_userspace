BOOTMAGIC_ENABLE = yes      # Enable Bootmagic Lite
MOUSEKEY_ENABLE            = no       # Mouse keys
EXTRAKEY_ENABLE            = yes      # Audio control and System control
CONSOLE_ENABLE             = no       # Console for debug
COMMAND_ENABLE             = no       # Commands for debug and configuration
NKRO_ENABLE                = yes      # USB Nkey Rollover
OLED_ENABLE                = yes
ENCODER_ENABLE             = yes      # Enables the use of one or more encoders
RGBLIGHT_ENABLE            = no       # Enable keyboard RGB underglow
KEY_LOCK_ENABLE            = no
WPM_ENABLE                 = yes

ifeq ($(strip $(KEYBOARD)), splitkb/kyria/rev1/proton_c)
    OVERLOAD_FEATURES = yes
    BOOTLOADER        = tinyuf2
endif
ifeq ($(strip $(KEYBOARD)), splitkb/kyria/rev3)
    OVERLOAD_FEATURES = yes
    CONVERT_TO        = proton_c
    WS2812_DRIVER     = pwm
    SERIAL_DRIVER     = usart

endif

ifeq ($(strip $(OVERLOAD_FEATURES)), yes)
    RGB_MATRIX_ENABLE        = yes
    CONSOLE_ENABLE           = yes      # Console for debug
    MOUSEKEY_ENABLE          = yes      # Mouse keys
    TAP_DANCE_ENABLE         = yes
    SWAP_HANDS_ENABLE        = yes
    LTO_ENABLE               = yes
    OPT                      = s
    ENCODER_MAP_ENABLE       = yes
    AUTOCORRECT_ENABLE       = no
    CAPS_WORD_ENABLE         = yes
    AUDIO_ENABLE             = no
    LAYER_MAP_ENABLE         = yes
    DISPLAY_KEYLOGGER_ENABLE = yes
else
    LTO_ENABLE               = yes
    BOOTLOADER               = qmk-hid
    BOOTLOADER_SIZE          = 512
    OS_DETECTION_ENABLE      = no
    CUSTOM_UNICODE_ENABLE    = no
endif
