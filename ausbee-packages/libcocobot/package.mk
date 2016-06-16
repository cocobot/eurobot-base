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

LIBCOCOBOT_SRC_FILES= $(LIBCOCOBOT_FILE_PATH)/src/console/console.c
LIBCOCOBOT_SRC_FILES+=$(LIBCOCOBOT_FILE_PATH)/src/lcd/lcd.c
LIBCOCOBOT_SRC_FILES+=$(LIBCOCOBOT_FILE_PATH)/src/trajectory/trajectory.c
LIBCOCOBOT_SRC_FILES+=$(LIBCOCOBOT_FILE_PATH)/src/asserv/asserv.c
LIBCOCOBOT_SRC_FILES+=$(LIBCOCOBOT_FILE_PATH)/src/asserv/ramp.c
LIBCOCOBOT_SRC_FILES+=$(LIBCOCOBOT_FILE_PATH)/src/asserv/pid.c
LIBCOCOBOT_SRC_FILES+=$(LIBCOCOBOT_FILE_PATH)/src/action_scheduler/action_scheduler.c
LIBCOCOBOT_SRC_FILES+=$(LIBCOCOBOT_FILE_PATH)/src/position/position.c
LIBCOCOBOT_SRC_FILES+=$(LIBCOCOBOT_FILE_PATH)/src/game_state/game_state.c
LIBCOCOBOT_SRC_FILES+=$(LIBCOCOBOT_FILE_PATH)/src/opponent_detection/opponent_detection.c
LIBCOCOBOT_SRC_FILES+=$(LIBCOCOBOT_FILE_PATH)/src/pathfinder/pathfinder.c
LIBCOCOBOT_SRC_FILES+=$(LIBCOCOBOT_FILE_PATH)/src/pathfinder/pathfinder_internal.c
LIBCOCOBOT_SRC_FILES+=$(LIBCOCOBOT_FILE_PATH)/src/pathfinder/pathfinder_table.c
LIBCOCOBOT_SRC_FILES+=$(LIBCOCOBOT_FILE_PATH)/src/pathfinder/pathfinder_douglas_peucker.c

LIBCOCOBOT_TARGET_SRC_FILES+=$(LIBCOCOBOT_FILE_PATH)/src/position/encoders.c

LIBCOCOBOT_SIM_SRC_FILES+=$(LIBCOCOBOT_FILE_PATH)/src/position/vrep.c

GLOBAL_LDFLAGS+=-lm

$(eval $(call pkg-generic,LIBCOCOBOT))
