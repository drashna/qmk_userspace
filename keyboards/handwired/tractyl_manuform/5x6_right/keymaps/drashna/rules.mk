nAUTOCORRECT_ENABLE               = no
AUDIO_ENABLE                     = no
ENCODER_ENABLE                   = no
CUSTOM_BOOTMAGIC_ENABLE          = no
CUSTOM_UNICODE_ENABLE            = no
HAPTIC_ENABLE                    = no
OLED_ENABLE                      = no
RGBLIGHT_ENABLE                  = no
SWAP_HANDS_ENABLE                = no
TAP_DANCE_ENABLE                 = no
WPM_ENABLE                       = no

MOUSE_SHARED_EP                  = no

ifeq ($(strip $(KEYBOARD)), handwired/tractyl_manuform/5x6_right/f411)
    BOOTLOADER                       = tinyuf2
    AUTOCORRECT_ENABLE               = yes
    LTO_SUPPORTED                    = no
    OVERLOAD_FEATURES                = yes
    HAPTIC_ENABLE                    = no
    HAPTIC_DRIVER                    = drv2605l
    RTC_DRIVER                       = ds3231
    WATCHDOG_ENABLE                  = yes
    OLED_ENABLE                      = yes
    RTC_ENABLE                       = yes
endif
ifeq ($(strip $(KEYBOARD)), handwired/tractyl_manuform/5x6_right/f405)
    BOOTLOADER                       = tinyuf2
    AUTOCORRECT_ENABLE               = yes
    OVERLOAD_FEATURES                = yes
    RTC_ENABLE                       = yes
    RTC_DRIVER                       = vendor
    WATCHDOG_ENABLE                  = yes
    OLED_ENABLE                      = no
    RGB_MATRIX_ENABLE                = yes
    RGBLIGHT_DRIVER                  = custom
    LAYER_MAP_ENABLE                 = yes
    HAPTIC_ENABLE                    = yes

    BACKLIGHT_ENABLE                 = yes
    QUANTUM_PAINTER_ENABLE           = yes
    QUANTUM_PAINTER_DRIVERS          += ili9341_spi
    CUSTOM_QUANTUM_PAINTER_ENABLE    = yes
    DISPLAY_KEYLOGGER_ENABLE         = yes
    I2C_DRIVER_REQUIRED              = yes
endif
ifeq ($(strip $(OVERLOAD_FEATURES)), yes)
    AUDIO_ENABLE                     = yes
    CAPS_WORD_ENABLE                 = yes
    CUSTOM_BOOTMAGIC_ENABLE          = yes
    CUSTOM_UNICODE_ENABLE            = yes
    ENCODER_ENABLE                   = yes
    ENCODER_MAP_ENABLE               = yes
    RGBLIGHT_ENABLE                  = yes
    RGBLIGHT_STARTUP_ANIMATION       = no
    TAP_DANCE_ENABLE                 = yes
    SWAP_HANDS_ENABLE                = yes
    WPM_ENABLE                       = yes
    SENTENCE_CASE_ENABLE             = yes
    DISPLAY_KEYLOGGER_ENABLE         = yes
    CLAP_TRAP_ENABLE                 = yes
    LTO_ENABLE                       = yes
    OPT                              = fast
endif
