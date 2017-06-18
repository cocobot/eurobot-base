LIBCOCOBOT_LOCAL_FILE_PATH=$(CONFIG_CUSTOM_PACKAGES_PATH)/libcocobot

# Include files
LIBCOCOBOT_LOCAL_INCLUDE_PATH=include/

#find all .c files and populate LIBCOCOBOT_SRC_FILES
$(eval $(call pkg-generic-auto-discover-local-common-srcs,LIBCOCOBOT))

LIBCOCOBOT_TARGET_LOCAL_SRC_FILES+=src/position/encoders.c
LIBCOCOBOT_TEMP_SIM_FILES+=src/position/vrep.c

#remove target and sim sources files to autodiscovered list
LIBCOCOBOT_LOCAL_SRC_FILES:=$(filter-out $(LIBCOCOBOT_TEMP_SIM_FILES),$(LIBCOCOBOT_LOCAL_SRC_FILES))
LIBCOCOBOT_LOCAL_SRC_FILES:=$(filter-out $(LIBCOCOBOT_TARGET_LOCAL_SRC_FILES),$(LIBCOCOBOT_LOCAL_SRC_FILES))

ifeq ($(CONFIG_VREP),y)
LIBCOCOBOT_SIM_LOCAL_SRC_FILES+=src/position/vrep.c
endif

GLOBAL_LDFLAGS+=-lm

$(eval $(call pkg-generic,LIBCOCOBOT))
