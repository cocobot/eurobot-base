ifeq ($(CONFIG_PLATFORM_BRAIN_2015),y)
include $(CONFIG_CUSTOM_PLATFORMS_PATH)/brain_2015/platform.mk
PLATFORM_CLEAN_GOAL+=platform_brain_2015-clean
endif

ifeq ($(CONFIG_PLATFORM_BRAIN_2016),y)
include $(CONFIG_CUSTOM_PLATFORMS_PATH)/brain_2016/platform.mk
PLATFORM_CLEAN_GOAL+=platform_brain_2016-clean
endif

ifeq ($(CONFIG_PLATFORM_DISCOVERY_F4),y)
include $(CONFIG_CUSTOM_PLATFORMS_PATH)/discovery_f4/platform.mk
PLATFORM_CLEAN_GOAL+=platform_discovery_f4-clean
endif

ifeq ($(CONFIG_PLATFORM_NUCLEO_401),y)
include $(CONFIG_CUSTOM_PLATFORMS_PATH)/nucleo_401/platform.mk
PLATFORM_CLEAN_GOAL+=platform_nucleo_401-clean
endif

ifeq ($(CONFIG_PLATFORM_PRISM_V1),y)
include $(CONFIG_CUSTOM_PLATFORMS_PATH)/prism_v1/platform.mk
PLATFORM_CLEAN_GOAL+=prism_v1-clean
endif
