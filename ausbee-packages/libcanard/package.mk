LIBCANARD_LOCAL_FILE_PATH=$(CONFIG_CUSTOM_PACKAGES_PATH)/libcanard

# Include files
LIBCANARD_LOCAL_INCLUDE_PATH=libcanard/ libcanard/drivers/stm32/

LIBCANARD_LOCAL_SRC_FILES+=libcanard/canard.c libcanard/drivers/stm32/canard_stm32.c

$(eval $(call pkg-generic,LIBCANARD))
