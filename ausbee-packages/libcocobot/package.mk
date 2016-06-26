LIBCOCOBOT_VERSION = 0.1
LIBCOCOBOT_URL = https://github.com/cocobot/libcocobot/archive/$(LIBCOCOBOT_VERSION).tar.gz
LIBCOCOBOT_ARCHIVE = libcocobot-$(LIBCOCOBOT_VERSION).tar.gz
LIBCOCOBOT_GIT_UPSTREAM_URL = git@github.com:cocobot/libcocobot.git

# File path
ifeq ($(CONFIG_LIBCOCOBOT_USE_GIT),y)
LIBCOCOBOT_FILE_PATH=git
else
LIBCOCOBOT_FILE_PATH=libcocobot-$(LIBCOCOBOT_VERSION)
endif

# Include files
LIBCOCOBOT_INCLUDE_PATH=$(LIBCOCOBOT_FILE_PATH)/include/

#find all .c files and populate LIBCOCOBOT_SRC_FILES
$(eval $(call pkg-generic-auto-discover-common-srcs,LIBCOCOBOT))

LIBCOCOBOT_TARGET_SRC_FILES+=$(LIBCOCOBOT_FILE_PATH)/src/position/encoders.c
LIBCOCOBOT_SIM_SRC_FILES+=$(LIBCOCOBOT_FILE_PATH)/src/position/vrep.c

#remove target and sim sources files to autodiscovered list
LIBCOCOBOT_SRC_FILES:=$(filter-out $(LIBCOCOBOT_SIM_SRC_FILES),$(LIBCOCOBOT_SRC_FILES))
LIBCOCOBOT_SRC_FILES:=$(filter-out $(LIBCOCOBOT_TARGET_SRC_FILES),$(LIBCOCOBOT_SRC_FILES))


GLOBAL_LDFLAGS+=-lm

$(eval $(call pkg-generic,LIBCOCOBOT))
