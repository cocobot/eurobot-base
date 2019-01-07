ifeq ($(CONFIG_LIBCOCOBOT),y)
include $(CONFIG_CUSTOM_PACKAGES_PATH)/libcocobot/package.mk
endif

ifeq ($(CONFIG_MCUAL),y)
include $(CONFIG_CUSTOM_PACKAGES_PATH)/mcual/package.mk
endif

ifeq ($(CONFIG_VREP),y)
include $(CONFIG_CUSTOM_PACKAGES_PATH)/vrep/package.mk
endif

ifeq ($(CONFIG_LIBCANARD),y)
include $(CONFIG_CUSTOM_PACKAGES_PATH)/libcanard/package.mk
endif

ifeq ($(CONFIG_SPIRIT1),y)
include $(CONFIG_CUSTOM_PACKAGES_PATH)/spirit1/package.mk
endif
