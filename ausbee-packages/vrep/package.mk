VREP_VERSION = 3_2_2
VREP_URL=http://coppeliarobotics.com/V-REP_SOURCE_V$(VREP_VERSION).zip
VREP_ARCHIVE=V-REP_SOURCE_V$(VREP_VERSION).zip

VREP_AUTOGEN_INCLUDES=-I"$(CURDIR)"

VREP_DEPENDENCIES=

# File path
VREP_FILE_PATH=vrep-$(VREP_VERSION)

# Include files
VREP_INCLUDE_PATH=

VREP_SRC_FILES=

$(eval $(call pkg-generic,VREP))

ifneq ($(findstring vrep-sim,$(MAKECMDGOALS)),)
HOST_AS=as
HOST_AR=ar
HOST_CC=gcc
HOST_CXX=g++
HOST_LD=ld
HOST_OBJCPY=objcopy
HOST_SIZE=size
HOST_STRIP=strip

#remove AVR/ARM flags
remove-in-list = $(foreach v,$2,$(if $(findstring $1,$v),,$v))
HOST_COMMON_FLAGS := $(call remove-in-list,mthumb,$(HOST_COMMON_FLAGS))
HOST_COMMON_FLAGS := $(call remove-in-list,mcpu,$(HOST_COMMON_FLAGS))
HOST_COMMON_FLAGS := $(call remove-in-list,mfloat-abi,$(HOST_COMMON_FLAGS))
HOST_COMMON_FLAGS := $(call remove-in-list,mfpu,$(HOST_COMMON_FLAGS))

OBJ_FILES := $(filter-out $(SYSTEM_OBJ_C_FILES),$(OBJ_FILES))
OBJ_FILES := $(filter-out $(SYSTEM_OBJ_S_FILES),$(OBJ_FILES))
OBJ_FILES := $(filter-out $(SYSTEM_OBJ_S_FILES),$(OBJ_FILES))
OBJ_FILES := $(call remove-in-list,ARM_CM4F,$(OBJ_FILES))
OBJ_FILES := $(call remove-in-list,mcual,$(OBJ_FILES))


OPERATING_SYSTEMS_INCLUDES := $(call remove-in-list,ARM_CM4F,$(OPERATING_SYSTEMS_INCLUDES))
OPERATING_SYSTEMS_INCLUDES += -I"$(CONFIG_CUSTOM_PACKAGES_PATH)/vrep/utils/FreeRTOS"
endif


vrep-help:
	$(ECHO_E) "List of vrep-* commands:"
	$(ECHO_E) " - vrep-sim-build: build project simulation"

vrep-sim-debug: 
	$(ECHO_E) $(OPERATING_SYSTEMS_INCLUDES)
	$(ECHO_E)
	$(ECHO_E) $(FREERTOS_SRC_PORT_PATH)
	$(ECHO_E)
	$(ECHO_E) $(OBJ_FILES)
	$(ECHO_E) $(HOST_CC)

vrep-sim-build: vrep-sim-debug all
