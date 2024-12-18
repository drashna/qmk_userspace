CUSTOM_QUANTUM_PAINTER_ENABLE ?= no
CUSTOM_OLED_DRIVER ?= yes

ifeq ($(strip $(CUSTOM_QUANTUM_PAINTER_ENABLE)), yes)
    ifeq ($(strip $(QUANTUM_PAINTER_ENABLE)), yes)
        DISPLAY_DRIVER_REQUIRED  = yes
        OPT_DEFS += -DCUSTOM_QUANTUM_PAINTER_ENABLE
        POST_CONFIG_H += $(USER_PATH)/display/painter/config.h

        ifeq ($(strip $(QUANTUM_PAINTER_DRIVERS)),ili9341_spi)
            OPT_DEFS += -DCUSTOM_QUANTUM_PAINTER_ILI9341
            SRC += $(USER_PATH)/display/painter/ili9341_display.c
        endif
        ifeq ($(strip $(QUANTUM_PAINTER_DRIVERS)),ili9488_spi)
            OPT_DEFS += -DCUSTOM_QUANTUM_PAINTER_ILI9488
            SRC += $(USER_PATH)/display/painter/ili9488_display.c
        endif
	    QUANTUM_PAINTER_DRIVERS += surface

        SRC += $(USER_PATH)/display/painter/fonts.qff.c \
            $(USER_PATH)/display/painter/graphics/qmk_logo_220x220.qgf.c \
            $(USER_PATH)/display/painter/graphics/qmk_powered_by.qgf.c \
            $(USER_PATH)/display/painter/graphics/akira_explosion.qgf.c \
            $(USER_PATH)/display/painter/graphics/asuka-240x320.qgf.c \
            $(USER_PATH)/display/painter/graphics/anime-girl-jacket-240x320.qgf.c \
            $(USER_PATH)/display/painter/graphics/neon-genesis-evangelion-initial-machine-240x320.qgf.c \
            $(USER_PATH)/display/painter/graphics/samurai-cyberpunk-minimal-dark-8k-b3-240x320.qgf.c \
            $(USER_PATH)/display/painter/graphics/large_nyan_cat.qgf.c \
            $(USER_PATH)/display/painter/graphics/nyan_cat.qgf.c

        SRC += $(USER_PATH)/display/painter/painter.c \
               $(USER_PATH)/display/painter/graphics.qgf.c \
               $(USER_PATH)/display/painter/qp_render_menu.c

        ifeq ($(strip $(DISPLAY_MENU_ENABLED_DEFAULT)), yes)
            OPT_DEFS += -DDISPLAY_MENU_ENABLED_DEFAULT
        endif
        ifeq ($(strip $(MULTITHREADED_PAINTER_ENABLE)), yes)
            OPT_DEFS += -DMULTITHREADED_PAINTER_ENABLE
        endif
    endif
endif


ifeq ($(strip $(OLED_ENABLE)), yes)
    ifeq ($(strip $(CUSTOM_OLED_DRIVER)), yes)
        DISPLAY_DRIVER_REQUIRED  = yes
        OPT_DEFS += -DCUSTOM_OLED_DRIVER
        SRC += $(USER_PATH)/display/oled/oled_stuff.c \
               $(USER_PATH)/display/oled/screensaver.c
        POST_CONFIG_H += $(USER_PATH)/display/oled/config.h
    endif
    ifeq ($(strip $(OLED_DISPLAY_TEST)), yes)
        OPT_DEFS += -DOLED_DISPLAY_TEST
    endif
    SRC += $(USER_PATH)/display/oled/oled_render_menu.c
endif

ifeq ($(strip $(DISPLAY_DRIVER_REQUIRED)), yes)
    DEFERRED_EXEC_ENABLE = yes
    SRC += $(USER_PATH)/display/display.c \
        $(USER_PATH)/display/menu/menu.c

    OPT_DEFS += -DDISPLAY_DRIVER_ENABLE
    ifeq ($(strip $(DISPLAY_KEYLOGGER_ENABLE)), yes)
        OPT_DEFS += -DDISPLAY_KEYLOGGER_ENABLE
    endif
endif
