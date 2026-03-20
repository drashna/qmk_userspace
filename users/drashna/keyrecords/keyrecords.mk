ifeq ($(strip $(PER_KEY_TAPPING)), yes)
    OPT_DEFS += -DPER_KEY_TAPPING
endif
CONFIG_H += $(USER_PATH)/keyrecords/config.h

UNICODE_ENABLE        := no
UNICODEMAP_ENABLE     := no
UCIS_ENABLE           := no
ifeq ($(strip $(UNICODE_COMMON)), yes)
    OPT_DEFS += -DCUSTOM_UNICODE_ENABLE
    SRC += $(USER_PATH)/keyrecords/unicode.c
endif

ifeq ($(strip $(ENCODER_ENABLE)), yes)
    ifeq ($(strip $(ENCODER_INIT_FIX)), yes)
        OPT_DEFS += -DENCODER_INIT_FIX
    endif
endif

ifeq ($(strip $(TAP_DANCE_ENABLE)), yes)
    CUSTOM_TAP_DANCE_ENABLE ?= yes
endif

KEYLOGGER_ENABLE ?= yes
ifeq ($(strip $(KEYLOGGER_ENABLE)), yes)
    OPT_DEFS += -DKEYLOGGER_ENABLE
endif

KEYRECORD_FEATURES = \
    CUSTOM_TAP_DANCE \

define HANDLE_MY_FEATURE
    # $$(info "Processing: $1_ENABLE $$(USER_PATH)/keyrecords/$2.c")
    SRC += $$(wildcard $$(USER_PATH)/keyrecords/$2.c)
    OPT_DEFS += -D$1_ENABLE
endef

$(foreach F,$(KEYRECORD_FEATURES),\
    $(if $(filter yes, $(strip $($(F)_ENABLE))),\
        $(eval $(call HANDLE_MY_FEATURE,$(F),$(shell echo $(F) | tr '[:upper:]' '[:lower:]'))) \
    ) \
)
