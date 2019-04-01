SPIRIT1_LOCAL_FILE_PATH=$(CONFIG_CUSTOM_PACKAGES_PATH)/spirit1

# Include files
SPIRIT1_LOCAL_INCLUDE_PATH=Inc
$(eval $(call pkg-generic-auto-discover-local-common-srcs,SPIRIT1))

$(eval $(call pkg-generic,SPIRIT1))
