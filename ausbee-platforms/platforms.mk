ifeq ($(CONFIG_PLATFORM_BRAIN_2019),y)
include $(CONFIG_CUSTOM_PLATFORMS_PATH)/brain_2019/platform.mk
PLATFORM_CLEAN_GOAL+=platform_brain_2019-clean
endif

ifeq ($(CONFIG_PLATFORM_COM_2019),y)
include $(CONFIG_CUSTOM_PLATFORMS_PATH)/com_2019/platform.mk
PLATFORM_CLEAN_GOAL+=platform_com_2019-clean
endif

ifeq ($(CONFIG_PLATFORM_CANON_2019),y)
include $(CONFIG_CUSTOM_PLATFORMS_PATH)/canon_2019/platform.mk
PLATFORM_CLEAN_GOAL+=platform_canon_2019-clean
endif
