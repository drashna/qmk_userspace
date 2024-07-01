BOOTLOADER := tinyuf2

AUDIO_ENABLE          = yes # Audio output
AUDIO_SUPPORTED       = yes # is set to no in kb, needs to be forcibly enabled
AUDIO_DRIVER          = pwm_hardware

BOOTMAGIC_ENABLE      = yes # Enable Bootmagic Lite
MOUSEKEY_ENABLE       = yes # Mouse keys
EXTRAKEY_ENABLE       = yes # Audio control and System control
CONSOLE_ENABLE        = no  # Console for debug
COMMAND_ENABLE        = no  # Commands for debug and configuration
NKRO_ENABLE           = yes # Enable N-Key Rollover
RGBLIGHT_ENABLE       = yes # Enable keyboard RGB underglow
RGBLIGHT_DRIVER       = custom

MOUSE_SHARED_EP       = no

AUTOCORRECT_ENABLE    = yes
CAPS_WORD_ENABLE      = yes
SWAP_HANDS_ENABLE     = yes
TAP_DANCE_ENABLE      = yes
WPM_ENABLE            = yes

PER_KEY_TAPPING = yes

CUSTOM_UNICODE_ENABLE = yes
CUSTOM_POINTING_DEVICE = yes
CUSTOM_SPLIT_TRANSPORT_SYNC = yes

QUANTUM_PAINTER_ENABLE = no
ifeq ($(strip $(QUANTUM_PAINTER_ENABLE)), yes)
	QUANTUM_PAINTER_DRIVERS = ili9341_spi
	DEBUG_MATRIX_SCAN_RATE_ENABLE = api
	BACKLIGHT_ENABLE = yes
	BACKLIGHT_DRIVER = software
endif
