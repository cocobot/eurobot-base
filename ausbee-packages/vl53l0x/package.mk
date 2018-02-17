VL53L0X_LOCAL_FILE_PATH=$(CONFIG_CUSTOM_PACKAGES_PATH)/vl53l0x

# Include files
VL53L0X_LOCAL_INCLUDE_PATH=include/

#find all .c files and populate VL53L0X_SRC_FILES
$(eval $(call pkg-generic-auto-discover-local-common-srcs,VL53L0X))

$(eval $(call pkg-generic,VL53L0X))
