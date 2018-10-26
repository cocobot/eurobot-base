LIBCANARD_LOCAL_FILE_PATH=$(CONFIG_CUSTOM_PACKAGES_PATH)/libcanard

# Include files
LIBCANARD_LOCAL_INCLUDE_PATH=libcanard/include/

LIBCANARD_LOCAL_SRC_FILES+=libcanard/canard.c

$(eval $(call pkg-generic,LIBCANARD))
