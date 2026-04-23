ADB_MOUSE_ENABLE                = yes
QUANTUM_PAINTER_ENABLE          = yes
QUANTUM_PAINTER_DRIVERS         += st7789_spi surface
KEYCODE_STRING_ENABLE           = yes
CAPS_WORD_ENABLE                = yes
CUSTOM_UNICODE_ENABLE           = yes
DISPLAY_KEYLOGGER_ENABLE        = yes
BACKLIGHT_ENABLE                = yes
BACKLIGHT_DRIVER                = pwm
DEBUG_MATRIX_SCAN_RATE_ENABLE   = api
WPM_ENABLE                      = yes
OS_DETECTION_ENABLE             = yes
AUDIO_ENABLE                    = yes
AUDIO_DRIVER                    = pwm_hardware

SRC += painter/fonts/font_oled.qff.c painter/graphics.qgf.c painter/keylogger.c
