SWAP_HANDS_ENABLE     = yes
AUTOCORRECT_ENABLE    = yes
CAPS_WORD_ENABLE      = yes
CONSOLE_ENABLE        = yes
WPM_ENABLE            = yes
OLED_ENABLE           = no
OLED_TRANSPORT        = spi
ENCODER_MAP_ENABLE    = yes
PER_KEY_TAPPING       = yes
RTC_ENABLE            = yes
RTC_DRIVER            = pcf8523

QUANTUM_PAINTER_ENABLE = yes
QUANTUM_PAINTER_DRIVERS += gc9a01_spi surface

SRC += bkb_logo_mod.qgf.c
