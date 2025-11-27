AUTOCORRECT_ENABLE      = no
AUDIO_ENABLE            = no
ENCODER_ENABLE          = no
CUSTOM_BOOTMAGIC_ENABLE = no
CUSTOM_UNICODE_ENABLE   = no
HAPTIC_ENABLE           = no
OLED_ENABLE             = no
RGBLIGHT_ENABLE         = no
SWAP_HANDS_ENABLE       = no
TAP_DANCE_ENABLE        = no
WPM_ENABLE              = no

MOUSE_SHARED_EP         = no

ifeq ($(strip $(KEYBOARD)), handwired/tractyl_manuform/5x6_right/f411)
    BOOTLOADER        = tinyuf2
    OVERLOAD_FEATURES = yes

    HAPTIC_ENABLE                         = no
    HAPTIC_DRIVER                         = drv2605l
    RTC_DRIVER                            = vendor
    QUANTUM_PAINTER_ENABLE                = yes
    CUSTOM_QUANTUM_PAINTER_ILI9341        = yes
    # This is stupid, and you shouldn't do this
    KEYMAP_JSON       = $(MAIN_KEYMAP_PATH_5)/keymap_f411.json
endif
ifeq ($(strip $(KEYBOARD)), handwired/tractyl_manuform/5x6_right/f405)
    BOOTLOADER                            = tinyuf2
    OVERLOAD_FEATURES                     = yes

    BACKLIGHT_ENABLE                      = yes
    HAPTIC_ENABLE                         = yes
    RGB_MATRIX_ENABLE                     = yes
    RGBLIGHT_DRIVER                       = custom
    QUANTUM_PAINTER_ENABLE                = yes
    CUSTOM_QUANTUM_PAINTER_ILI9341        = yes
    CUSTOM_QUANTUM_PAINTER_ILI9488        = no
    CUSTOM_QUANTUM_PAINTER_ST7789_170X320 = no
    CUSTOM_QUANTUM_PAINTER_ST7789_135X240 = no
    CUSTOM_QUANTUM_PAINTER_ST7789_76X284  = no
    WEAR_LEVELING_DRIVER                  = spi_flash
    USE_USB_OTG_HS_PORT                   = no
    ifeq ($(strip $(PORTSCAN_MATRIX_ENABLE)), yes)
        CUSTOM_MATRIX = lite
        SRC += portscan_matrix.c
        OPT_DEFS += -DPAL_USE_WAIT=TRUE -DPORTSCAN_MATRIX_ENABLE
    endif
endif

ifeq ($(strip $(OVERLOAD_FEATURES)), yes)
    AUDIO_ENABLE                  = yes
    AUTOCORRECT_ENABLE            = yes
    CAPS_WORD_ENABLE              = yes
    CLAP_TRAP_ENABLE              = yes
    CUSTOM_BOOTMAGIC_ENABLE       = yes
    CUSTOM_UNICODE_ENABLE         = yes
    DISPLAY_KEYLOGGER_ENABLE      = yes
    ENCODER_ENABLE                = yes
    ENCODER_MAP_ENABLE            = yes
    LTO_ENABLE                    = yes
    MULTITHREADED_LIGHTING_ENABLE = yes
    MULTITHREADED_PAINTER_ENABLE  = no
    OPT                           = fast
    RGBLIGHT_ENABLE               = yes
    RGBLIGHT_STARTUP_ANIMATION    = no
    RTC_TOTP_ENABLE               = yes
    SWAP_HANDS_ENABLE             = yes
    TAP_DANCE_ENABLE              = yes
    WPM_ENABLE                    = yes
endif

ifeq ($(strip $(USE_USB_OTG_HS_PORT)), yes)
    OPT_DEFS += -DUSE_USB_OTG_HS_PORT
endif
