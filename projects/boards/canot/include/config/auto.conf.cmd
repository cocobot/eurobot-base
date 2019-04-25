deps_config := \
	/home/damien/prog/cocobot/projects/boards/canot/../../../ausbee-platforms/canivo_2019/Kconfig \
	/home/damien/prog/cocobot/projects/boards/canot/../../../ausbee-platforms/com_2019/Kconfig \
	/home/damien/prog/cocobot/projects/boards/canot/../../../ausbee-platforms/option.Kconfig \
	/home/damien/prog/cocobot/projects/boards/canot/../../../ausbee-platforms/Kconfig \
	../../../ausbee/Platforms/Kconfig \
	../../../ausbee/Operating_Systems/libc/Kconfig \
	../../../ausbee/Operating_Systems/FreeRTOS/Kconfig \
	../../../ausbee/Operating_Systems/Kconfig \
	/home/damien/prog/cocobot/projects/boards/canot/../../../ausbee-packages/libcanard/Kconfig \
	/home/damien/prog/cocobot/projects/boards/canot/../../../ausbee-packages/vrep/Kconfig \
	/home/damien/prog/cocobot/projects/boards/canot/../../../ausbee-packages/libcocobot/Kconfig \
	/home/damien/prog/cocobot/projects/boards/canot/../../../ausbee-packages/mcual/Kconfig \
	/home/damien/prog/cocobot/projects/boards/canot/../../../ausbee-packages/Kconfig \
	../../../ausbee/Packages/libausbee/Kconfig \
	../../../ausbee/Packages/stm32f4xx_stdperiph_driver/Kconfig \
	../../../ausbee/Packages/stm32f10x_stdperiph_driver/Kconfig \
	../../../ausbee/Packages/Kconfig \
	../../../ausbee/AVRToolchain/Kconfig \
	../../../ausbee/Toolchain/Kconfig \
	../../../ausbee/Kconfig

include/config/auto.conf: \
	$(deps_config)

ifneq "$(AUSBEE_CUSTOM_PLATFORMS_PATH)" "/home/damien/prog/cocobot/projects/boards/canot/../../../ausbee-platforms"
include/config/auto.conf: FORCE
endif
ifneq "$(AUSBEE_CUSTOM_PLATFORMS_OPTION_KCONFIG_PATH)" "/home/damien/prog/cocobot/projects/boards/canot/../../../ausbee-platforms/option.Kconfig"
include/config/auto.conf: FORCE
endif
ifneq "$(AUSBEE_CUSTOM_PLATFORMS_KCONFIG_PATH)" "/home/damien/prog/cocobot/projects/boards/canot/../../../ausbee-platforms/Kconfig"
include/config/auto.conf: FORCE
endif
ifneq "$(AUSBEE_CUSTOM_PACKAGES_PATH)" "/home/damien/prog/cocobot/projects/boards/canot/../../../ausbee-packages"
include/config/auto.conf: FORCE
endif
ifneq "$(AUSBEE_CUSTOM_PACKAGES_KCONFIG_PATH)" "/home/damien/prog/cocobot/projects/boards/canot/../../../ausbee-packages/Kconfig"
include/config/auto.conf: FORCE
endif
ifneq "$(AUSBEE_DIR)" "../../../ausbee"
include/config/auto.conf: FORCE
endif

$(deps_config): ;
