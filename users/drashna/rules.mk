SRC += $(USER_PATH)/drashna.c \
        $(USER_PATH)/callbacks.c \
        $(USER_PATH)/keyrecords/process_records.c \
        $(USER_PATH)/keyrecords/tapping.c \
        $(USER_PATH)/drashna_names.c \
        $(USER_PATH)/drashna_util.c

# TOP_SYMBOLS = yes

DEBOUNCE_TYPE                 ?= asym_eager_defer_pk
DEFERRED_EXEC_ENABLE          ?= yes
OS_DETECTION_ENABLE           ?= yes
GRAVE_ESC_ENABLE              := no
SPACE_CADET_ENABLE            := no
DEBUG_MATRIX_SCAN_RATE_ENABLE := no

ifeq ($(PLATFORM_KEY),chibios)
    # If MCU has FPU support, use hack to enable it for lib8tion
    ifeq ($(strip $(USE_FPU)), yes)
        OPT_DEFS += -DFASTLED_TEENSY3
    endif
    CUSTOM_UNICODE_ENABLE ?= yes
    KEYCODE_STRING_ENABLE ?= yes
    SRC += $(USER_PATH)/hardware/hardware_id.c
    VPATH += $(USER_PATH)/hardware
    ifeq ($(strip $(MCU_FAMILY)), STM32)
        OPT_DEFS += -DSERIAL_NUMBER_LENGTH=12
    endif
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

CUSTOM_BOOTMAGIC_ENABLE ?= yes
ifeq ($(strip $(CUSTOM_BOOTMAGIC_ENABLE)), yes)
    ifeq ($(strip $(BOOTMAGIC_ENABLE)), yes)
        SRC += bootmagic_better.c
    endif
endif

ifeq ($(strip $(WPM_ENABLE)), yes)
    SRC += $(USER_PATH)/wpm.c
endif

ifeq ($(strip $(AUTOCORRECT_ENABLE)), yes)
    SRC += $(USER_PATH)/autocorrect.c
endif

ifeq ($(strip $(HARDWARE_DEBUG_ENABLE)), yes)
    LTO_ENABLE := no
    OPT := 0
    OPT_DEFS += -g -O0
    SEGGER_RTT_DRIVER_REQUIRED = yes
    TOP_SYMBOLS = 200
    EXTRALDFLAGS = -Xlinker -Map=$(BUILD_DIR)/$(TARGET).map
    ifeq ($(PLATFORM_KEY),chibios)
        EXTRAFLAGS = -fstack-usage
        EXTRALDFLAGS += -Wl,--print-memory-usage
    endif
endif

ifeq ($(strip $(HEAVY_OPTIMIZATION_ENABLE)), yes)
    OPT_DEFS += -DHEAVY_OPTIMIZATION_ENABLE -ffast-math -funroll-all-loops \
                -fno-tree-vectorize -fno-signed-zeros -fno-math-errno \
                -fno-common -fomit-frame-pointer -fno-exceptions -fno-unwind-tables
endif

SRC += $(USER_PATH)/sendchar.c

ifeq ($(strip $(DEBUG_MATRIX_SCAN_RATE_ENABLE)), yes)
    DEBUG_MATRIX_SCAN_RATE_ENABLE := no
    OPT_DEFS += -DDEBUG_MATRIX_SCAN_RATE_ENABLE
endif

include $(USER_PATH)/display/display.mk
include $(USER_PATH)/pointing/pointing.mk
include $(USER_PATH)/split/split.mk
include $(USER_PATH)/rgb/rgb.mk
include $(USER_PATH)/keyrecords/keyrecords.mk
include $(USER_PATH)/features/common.mk
# Ignore if not found
-include $(KEYMAP_PATH)/post_rules.mk
