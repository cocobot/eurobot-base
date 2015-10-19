# Set the source path of this platform
PLATFORM_BRAIN_2016_PATH=$(CONFIG_CUSTOM_PLATFORMS_PATH)/brain_2016

# Includes of this platform
PLATFORM_INCLUDES=-I"$(PLATFORM_BRAIN_2016_PATH)"

# Search the source file and generate the object list
PLATFORM_SRC_FILES+=$(PLATFORM_BRAIN_2016_PATH)/platform.c
PLATFORM_OBJ_FILES=$(patsubst ${CONFIG_CUSTOM_PLATFORMS_PATH}/%.c,${OUTPUT_PATH}/%.o,${PLATFORM_SRC_FILES})

#Add the object file list to the global object list
OBJ_FILES+=$(PLATFORM_OBJ_FILES)

$(PLATFORM_OBJ_FILES): $(OUTPUT_PATH)/%.o :$(CONFIG_CUSTOM_PLATFORMS_PATH)/%.c $(TOOLCHAIN_EXTRACTED) MCUAL_LOAD
	@mkdir -p $(dir $@)
	$(HOST_CC) -o $@ $(HOST_CFLAGS) $(MCUAL_INCLUDES) $(SYSTEM_INCLUDES) $(HOST_OPTIMISATION) -c $<

