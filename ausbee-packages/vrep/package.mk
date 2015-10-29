CONFIG_VREP_USE_HTTP=y
VREP_VERSION = 3_2_2
VREP_URL=http://coppeliarobotics.com/V-REP_PRO_EDU_V$(VREP_VERSION)_Linux.tar.gz
VREP_ARCHIVE=V-REP_PRO_EDU_V$(VREP_VERSION).tar.gz
VREP_FILE_PATH=vrep-$(VREP_VERSION)
VREP_LOCAL_FILE_PATH=$(CONFIG_CUSTOM_PACKAGES_PATH)/vrep

VREP_SIM_LOCAL_INCLUDE_PATH=utils/FreeRTOS
VREP_SIM_INCLUDE_PATH=V-REP_PRO_EDU_V3_2_2_Linux/programming/remoteApi V-REP_PRO_EDU_V3_2_2_Linux/programming/include

VREP_SIM_LOCAL_SRC_FILES=utils/FreeRTOS/port.c \
               					utils/mcual/adc.c 		\
               					utils/mcual/arch.c 		\
               					utils/mcual/clock.c 	\
               					utils/mcual/gpio.c 		\
               					utils/mcual/timer.c 	\
               					utils/mcual/spi.c 	\
               					utils/mcual/usart.c

VREP_SIM_SRC_FILES=V-REP_PRO_EDU_V3_2_2_Linux/programming/remoteApi/extApi.c V-REP_PRO_EDU_V3_2_2_Linux/programming/remoteApi/extApiPlatform.c

#add LINKER FLAGS
SIM_LDFLAGS+=-lpthread
SIM_CFLAGS+=-DNON_MATLAB_PARSING -DMAX_EXT_API_CONNECTIONS=255

$(eval $(call pkg-generic,VREP))
