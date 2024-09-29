-include $(KEYMAP_PATH)/post_rules.mk


ifeq ($(strip $(I2C_DRIVER_REQUIRED)), yes)
    OPT_DEFS += -DI2C_DRIVER_REQUIRED
endif
