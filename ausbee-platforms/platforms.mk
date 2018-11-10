ifeq ($(CONFIG_PLATFORM_BRAIN_2019),y)
include $(CONFIG_CUSTOM_PLATFORMS_PATH)/brain_2019/platform.mk
PLATFORM_CLEAN_GOAL+=platform_brain_2019-clean
endif
