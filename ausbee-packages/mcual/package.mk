MCUAL_DEPENDENCIES=system operating_systems mcual_autogen

ifeq ($(CONFIG_XMEGA_CORE),y)
MCUAL_ARCH=avr
else
MCUAL_ARCH=arm
endif

MCUAL_LOCAL_FILE_PATH=$(CONFIG_CUSTOM_PACKAGES_PATH)/mcual

# Include files
MCUAL_LOCAL_INCLUDE_PATH=include

MCUAL_TARGET_LOCAL_SRC_FILES=src/$(MCUAL_ARCH)/clock.c \
														 src/$(MCUAL_ARCH)/gpio.c	 \
														 src/$(MCUAL_ARCH)/adc.c	 \
														 src/$(MCUAL_ARCH)/usart.c \
														 src/$(MCUAL_ARCH)/timer.c \
														 src/$(MCUAL_ARCH)/spi.c \
														 src/$(MCUAL_ARCH)/can.c \
														 src/$(MCUAL_ARCH)/i2c.c \
														 src/$(MCUAL_ARCH)/loader.c \
														 src/$(MCUAL_ARCH)/arch.c

MCUAL_SIM_LOCAL_SRC_FILES=src/sim/clock.c \
													src/sim/gpio.c	 \
													src/sim/adc.c	 \
													src/sim/usart.c \
													src/sim/timer.c \
													src/sim/spi.c \
													src/sim/i2c.c \
													src/sim/loader.c \
													src/sim/can.c \
													src/sim/arch.c



$(eval $(call pkg-generic,MCUAL))


#create loader rule
loader: $(OUTPUT_TARGET_HEX)
	python2 $(MCUAL_LOCAL_FILE_PATH)/scripts/mcual_loader.py $(PROGRAM_SERIAL_INTERFACE) $(OUTPUT_TARGET_HEX)
