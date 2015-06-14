ifeq ($(CONFIG_PLATFORM_BRAIN_2015),y)
include $(CONFIG_CUSTOM_PLATFORMS_PATH)/brain_2015/platform.mk
PLATFORM_CLEAN_GOAL+=platform_brain_2015-clean
endif

ifeq ($(CONFIG_PLATFORM_BRAIN_2016),y)
include $(CONFIG_CUSTOM_PLATFORMS_PATH)/brain_2016/platform.mk
PLATFORM_CLEAN_GOAL+=platform_brain_2016-clean
endif
