ifeq ($(CONFIG_LIBCOCOBOT),y)
include $(CONFIG_CUSTOM_PACKAGES_PATH)/libcocobot/package.mk
endif

ifeq ($(CONFIG_MCUAL),y)
include $(CONFIG_CUSTOM_PACKAGES_PATH)/mcual/package.mk
endif

ifeq ($(CONFIG_VL53L0X),y)
include $(CONFIG_CUSTOM_PACKAGES_PATH)/vl53l0x/package.mk
endif

ifeq ($(CONFIG_VREP),y)
include $(CONFIG_CUSTOM_PACKAGES_PATH)/vrep/package.mk
endif
