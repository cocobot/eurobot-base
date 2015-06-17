LIBCOCOBOT_VERSION = 0.1
LIBCOCOBOT_URL = https://github.com/cocobot/ausbee-libcocobot/archive/$(LIBCOCOBOT_VERSION).tar.gz
LIBCOCOBOT_ARCHIVE = libcocobot-$(LIBCOCOBOT_VERSION).tar.gz


# Include files
LIBCOCOBOT_INCLUDE_PATH=libcocobot-$(LIBCOCOBOT_VERSION)/include/

LIBCOCOBOT_SRC_FILES=

#ifeq ($(CONFIG_LIBCOCOBOT_DRIVERS),y)
#LIBCOCOBOT_SRC_FILES=libcocobot-$(LIBCOCOBOT_VERSION)/src/drivers/lidar.c \
#	libcocobot-$(LIBCOCOBOT_VERSION)/src/drivers/ax12.c \
#	libcocobot-$(LIBCOCOBOT_VERSION)/src/drivers/encoder.c \
#	libcocobot-$(LIBCOCOBOT_VERSION)/src/drivers/l298_driver.c \
#	libcocobot-$(LIBCOCOBOT_VERSION)/src/drivers/servo.c \
#	libcocobot-$(LIBCOCOBOT_VERSION)/src/drivers/gp2.c
#endif
#
#ifeq ($(CONFIG_LIBCOCOBOT_CONTROL_SYSTEM),y)
#LIBCOCOBOT_SRC_FILES+=libcocobot-$(LIBCOCOBOT_VERSION)/src/control_system/control_system_manager.c \
#	libcocobot-$(LIBCOCOBOT_VERSION)/src/control_system/controller/pid.c \
#	libcocobot-$(LIBCOCOBOT_VERSION)/src/control_system/filters/diff.c \
#	libcocobot-$(LIBCOCOBOT_VERSION)/src/control_system/filters/quadramp.c
#endif

$(eval $(call pkg-generic,LIBCOCOBOT))
