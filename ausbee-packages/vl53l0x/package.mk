VL53L0X_LOCAL_FILE_PATH=$(CONFIG_CUSTOM_PACKAGES_PATH)/vl53l0x

# Include files
VL53L0X_LOCAL_INCLUDE_PATH=include/

#find all .c files and populate VL53L0X_SRC_FILES
$(eval $(call pkg-generic-auto-discover-local-common-srcs,VL53L0X))

#remove target and sim sources files to autodiscovered list
VL53L0X_LOCAL_SRC_FILES:=$(filter-out $(VL53L0X_TEMP_SIM_FILES),$(VL53L0X_LOCAL_SRC_FILES))
VL53L0X_LOCAL_SRC_FILES:=$(filter-out $(VL53L0X_TARGET_LOCAL_SRC_FILES),$(VL53L0X_LOCAL_SRC_FILES))

$(eval $(call pkg-generic,VL53L0X))
