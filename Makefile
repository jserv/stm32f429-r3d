PROJECT = r3d

EXECUTABLE = $(PROJECT).elf
BIN_IMAGE = $(PROJECT).bin
HEX_IMAGE = $(PROJECT).hex

# set the path to STM32F429I-Discovery firmware package
STDP ?= ../STM32F429I-Discovery_FW_V1.0.1

CROSS_COMPILE = arm-none-eabi-

CC = $(CROSS_COMPILE)gcc
OBJCOPY = $(CROSS_COMPILE)objcopy
OBJDUMP = $(CROSS_COMPILE)objdump
SIZE = $(CROSS_COMPILE)size

LIBS = -lm
CFLAGS = -g -std=c99 -O3 -ffast-math
CFLAGS += -mlittle-endian -mthumb
CFLAGS += -mcpu=cortex-m4 -DSTM32F429_439xx
#CFLAGS += -nostartfiles
#CFLAGS += -ffreestanding
#CFLAGS += -nostdlib
CFLAGS += -Wl,--gc-sections

# FPU
CFLAGS += -mfpu=fpv4-sp-d16 -mfloat-abi=hard
CFLAGS += -DARM_MATH_CM4 -D__FPU_PRESENT

# to run from FLASH
CFLAGS += -DVECT_TAB_FLASH
CFLAGS += -Wl,-T,stm32f429zi_flash.ld

# PROJECT SOURCE
CFLAGS += -I.
SOURCE = *.c

# LIB r3d
CFLAGS += -I libs/r3d/
SOURCE += libs/r3d/*.c

# LIB r3dfb
CFLAGS += -I libs/r3dfb-stm32f429-discovery/
SOURCE += libs/r3dfb-stm32f429-discovery/*.c

# example data
CFLAGS += -I examples/meshes/
CFLAGS += -I examples/textures/
# L3GD20 default callback
# CFLAGS += -DUSE_DEFAULT_TIMEOUT_CALLBACK

# STARTUP FILE
SOURCE += startup_stm32f429_439xx.S

# CMSIS
CFLAGS += -I$(STDP)/Libraries/CMSIS/Device/ST/STM32F4xx/Include
CFLAGS += -I$(STDP)/Libraries/CMSIS/Include
SOURCE += $(STDP)/Libraries/CMSIS/Lib/GCC/libarm_cortexM4lf_math.a

# STemWinLibrary522_4x9i
CFLAGS += -I$(STDP)/Libraries/STemWinLibrary522_4x9i/inc
SOURCE += $(STDP)/Libraries/STemWinLibrary522_4x9i/Lib/STemWin522_4x9i_CM4_OS_GCC.a

# STM32F4xx_StdPeriph_Driver
CFLAGS += -DUSE_STDPERIPH_DRIVER
CFLAGS += -I$(STDP)/Libraries/STM32F4xx_StdPeriph_Driver/inc

# STM32F429I-Discovery Utilities
CFLAGS += -I$(STDP)/Utilities/STM32F429I-Discovery
SOURCE += $(STDP)/Utilities/STM32F429I-Discovery/stm32f429i_discovery.c
SOURCE += $(STDP)/Utilities/STM32F429I-Discovery/stm32f429i_discovery_l3gd20.c
SOURCE += $(STDP)/Utilities/STM32F429I-Discovery/stm32f429i_discovery_lcd.c
SOURCE += $(STDP)/Utilities/STM32F429I-Discovery/stm32f429i_discovery_sdram.c

all: libSTM32F4xx_StdPeriph_Driver.a $(BIN_IMAGE)

$(BIN_IMAGE): $(EXECUTABLE)
	$(OBJCOPY) -O binary $^ $@
	$(OBJCOPY) -O ihex $^ $(HEX_IMAGE)
	$(OBJDUMP) -h -S -D $(EXECUTABLE) > $(PROJECT).lst
	$(SIZE) $(EXECUTABLE)
	
$(EXECUTABLE): $(SOURCE)
	$(CC) $(CFLAGS) $^ -o $@ libSTM32F4xx_StdPeriph_Driver.a $(LIBS)
	
libSTM32F4xx_StdPeriph_Driver.a: 
	$(CC) $(CFLAGS) -c -D"assert_param(expr)=((void)0)" $(STDP)/Libraries/STM32F4xx_StdPeriph_Driver/src/*.c
	@$(AR) cr $@ *.o 
	rm -rf *.o

clean:
	rm -rf $(EXECUTABLE)
	rm -rf $(BIN_IMAGE)
	rm -rf $(HEX_IMAGE)
	rm -f libSTM32F4xx_StdPeriph_Driver.a
	rm -f $(PROJECT).lst

flash:
	openocd -f stm32f429discovery.cfg \
		-c "init" \
		-c "reset init" \
		-c "halt" \
		-c "flash write_image erase $(PROJECT).elf" \
		-c "verify_image $(PROJECT).elf" \
		-c "reset run" -c shutdown
	#st-flash write $(BIN_IMAGE) 0x8000000

.PHONY: clean
