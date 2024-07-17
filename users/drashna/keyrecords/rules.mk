ifeq ($(strip $(PER_KEY_TAPPING)), yes)
    OPT_DEFS += -DPER_KEY_TAPPING
endif
CONFIG_H += $(USER_PATH)/keyrecords/config.h

UNICODE_ENABLE        := no
UNICODEMAP_ENABLE     := no
UCIS_ENABLE           := no
ifeq ($(strip $(CUSTOM_UNICODE_ENABLE)), yes)
    UNICODE_COMMON        := yes
    OPT_DEFS += -DCUSTOM_UNICODE_ENABLE
    SRC += $(USER_PATH)/keyrecords/unicode.c
else
    UNICODE_COMMON        := no
endif

ifeq ($(strip $(TAP_DANCE_ENABLE)), yes)
    CUSTOM_TAP_DANCE_ENABLE ?= yes
endif
ifeq ($(strip $(KEY_CANCELLATION_ENABLE)), yes)
    CUSTOM_KEY_CANCELLATION_ENABLE ?= yes
endif

KEYLOGGER_ENABLE ?= no
ifeq ($(strip $(KEYLOGGER_ENABLE)), yes)
    OPT_DEFS += -DKEYLOGGER_ENABLE
endif

KEYRECORD_FEATURES = \
    ACHORDION \
    CUSTOM_SHIFT_KEYS \
    CUSTOM_TAP_DANCE \
    CUSTOM_DYNAMIC_MACROS \
    CUSTOM_KEY_CANCELLATION \
    SELECT_WORD \
    SENTENCE_CASE

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
