USER_NAME := not_drashna

ifeq ($(strip $(KEYBOARD)), ploopyco/mouse/rev1_002)
    BOOTLOADER = qmk-hid
    RGBLIGHT_ENABLE = yes
    CONSOLE_ENABLE  = no
    LTO_ENABLE      = yes
endif
