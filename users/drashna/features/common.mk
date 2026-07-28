-include $(USER_PATH)/features/rtc/rtc.mk
VPATH += $(USER_PATH)/features/

CUSTOM_SEND_STRING_ENABLE ?= yes
ifeq ($(strip $(CUSTOM_SEND_STRING_ENABLE)), yes)
    OPT_DEFS += -DCUSTOM_SEND_STRING_ENABLE -DSEND_STRING_ENABLE
    override SEND_STRING_ENABLE = no
    VPATH += $(USER_PATH)/features/custom_send_string/
    SRC += $(USER_PATH)/features/custom_send_string/send_string.c
endif
