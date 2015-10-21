VREP_VERSION = 3_2_2
VREP_URL=http://coppeliarobotics.com/V-REP_SOURCE_V$(VREP_VERSION).zip
VREP_ARCHIVE=V-REP_SOURCE_V$(VREP_VERSION).zip
VREP_FILE_PATH=vrep-$(VREP_VERSION)
VREP_LOCAL_FILE_PATH=$(CONFIG_CUSTOM_PACKAGES_PATH)/vrep

VREP_SIM_LOCAL_INCLUDE_PATH=utils/FreeRTOS

VREP_SIM_LOCAL_SRC_FILES=utils/FreeRTOS/port.c \
               					utils/mcual/adc.c 		\
               					utils/mcual/arch.c 		\
               					utils/mcual/clock.c 	\
               					utils/mcual/gpio.c 		\
               					utils/mcual/timer.c 	\
               					utils/mcual/usart.c

#add LINKER FLAGS
SIM_LDFLAGS+=-lpthread

$(eval $(call pkg-generic,VREP))
