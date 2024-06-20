SRC += $(USER_PATH)/drashna.c \
        $(USER_PATH)/callbacks.c \
        $(USER_PATH)/keyrecords/process_records.c \
        $(USER_PATH)/keyrecords/tapping.c \
        $(USER_PATH)/eeconfig_users.c

# TOP_SYMBOLS = yes

DEBOUNCE_TYPE                 ?= asym_eager_defer_pk
DEFERRED_EXEC_ENABLE          ?= yes
OS_DETECTION_ENABLE           ?= yes
GRAVE_ESC_ENABLE              := no
SPACE_CADET_ENABLE            := no
DEBUG_MATRIX_SCAN_RATE_ENABLE := no

ifeq ($(PLATFORM_KEY),chibios)
    # cortex-m4 has DSP+FPU support, so use hack to enable it for lib8tion
    ifeq ($(strip $(MCU)), cortex-m4)
        OPT_DEFS += -DFASTLED_TEENSY3
    endif
    CUSTOM_UNICODE_ENABLE ?= yes

else
    ifneq ($(strip $(LTO_SUPPORTED)), no)
        LTO_ENABLE        = yes
    endif
    SPACE_CADET_ENABLE    ?= no
    GRAVE_ESC_ENABLE      ?= no
endif

-include $(USER_PATH)/../../../qmk_secrets/rules.mk

ifeq ($(strip $(MAKE_BOOTLOADER)), yes)
    OPT_DEFS += -DMAKE_BOOTLOADER
endif

# At least until build.mk or the like drops, this is here to prevent
# VUSB boards from enabling NKRO, as they do not support it. Ideally
# this should be handled per keyboard, but until that happens ...
ifeq ($(strip $(PROTOCOL)), VUSB)
    NKRO_ENABLE       := no
endif

ifeq ($(strip $(I2C_SCANNER_ENABLE)), yes)
    OPT_DEFS += -DI2C_SCANNER_ENABLE
    CONSOLE_ENABLE := yes
endif

CUSTOM_BOOTMAGIC_ENABLE ?= yes
ifeq ($(strip $(CUSTOM_BOOTMAGIC_ENABLE)), yes)
    ifeq ($(strip $(BOOTMAGIC_ENABLE)), yes)
        SRC += bootmagic_better.c
    endif
endif

ifeq ($(strip $(HARDWARE_DEBUG_ENABLE)), yes)
    LTO_ENABLE := no
    OPT := 0
    OPT_DEFS += -g
endif

ifeq ($(strip $(DEBUG_MATRIX_SCAN_RATE_ENABLE)), yes)
    DEBUG_MATRIX_SCAN_RATE_ENABLE := no
    OPT_DEFS += -DDEBUG_MATRIX_SCAN_RATE_ENABLE
endif

include $(USER_PATH)/oled/rules.mk
include $(USER_PATH)/pointing/rules.mk
include $(USER_PATH)/split/rules.mk
include $(USER_PATH)/painter/rules.mk
include $(USER_PATH)/rgb/rules.mk
include $(USER_PATH)/keyrecords/rules.mk
include $(USER_PATH)/features/rules.mk
# Ignore if not found
-include $(KEYMAP_PATH)/post_rules.mk
