MCUAL_VERSION = 0.1
MCUAL_URL = https://github.com/cocobot/mcual/archive/$(MCUAL_VERSION).tar.gz
MCUAL_ARCHIVE =$(MCUAL_FILE_PATH).tar.gz
MCUAL_GIT_UPSTREAM_URL = git@github.com:cocobot/mcual.git

MCUAL_DEPENDENCIES=system

# File path
ifeq ($(CONFIG_MCUAL_USE_GIT),y)
MCUAL_FILE_PATH=git
else
MCUAL_FILE_PATH=mcual-$(MCUAL_VERSION)
endif

# Include files
MCUAL_INCLUDE_PATH=$(MCUAL_FILE_PATH)/include/

MCUAL_SRC_FILES=$(MCUAL_FILE_PATH)/src/clock_arm.c

$(eval $(call pkg-generic,MCUAL))

