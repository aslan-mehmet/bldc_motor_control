PROJ_NAME=buzzer

LIBRARY_DIR=./lib
BUILD_DIR=build

SRC_DIRS=$(LIBRARY_DIR)/std_periph/src

# + makes space separated
INC_DIRS=$(LIBRARY_DIR)/cmsis/inc
INC_DIRS+=$(LIBRARY_DIR)/std_periph/inc
INC_DIRS+= .

DEFS=-DUSE_STDPERIPH_DRIVER
# target device
DEFS+=-DSTM32F401xx
DEFS+=-DHSE_VALUE=16000000

CC=arm-none-eabi-gcc
OBJCOPY=arm-none-eabi-objcopy

INCLUDE=$(addprefix -I, $(INC_DIRS))

CFLAGS=-std=c99
CFLAGS+=-Wall -Wextra -Warray-bounds -Wno-unused-parameter
CFLAGS+=-Wno-unused-variable -Wno-strict-aliasing
CFLAGS+=-mlittle-endian -mthumb -mcpu=cortex-m4 -mthumb-interwork
CFLAGS+=-mfloat-abi=softfp -mfpu=fpv4-sp-d16
# only use if necessary. also w/ -g problem
#CFLAGS+=-ffunction-sections -fdata-sections -Og -g
CFLAGS+=-Og -g

# nosys.specs solved linking error
LFLAGS=-Tstm32f401re.ld --specs=nosys.specs
LFLAGS+=-Wl,-Map=$(BUILD_DIR)/$(PROJ_NAME).map
LFLAGS+=-Wl,--cref
#LFLAGS+=-Wl,--gc-sections
#LFLAGS+=-Wl,-print-gc-sections

# if source code not found in current dir, go search for
vpath %.c $(SRC_DIRS)
# stm32f4xx_fmc.c
# stm32f4xx_fsmc.c
# gives ton of errors

# all the c and s files in current directory
SRCS=$(wildcard *.c)
SRCS+=$(wildcard *.s)

# at first thought why bother add all
# creates huge binary file
# for optimization only choose which used
SRCS+=misc.c
SRCS+=stm32f4xx_adc.c
#SRCS+=stm32f4xx_can.c
#SRCS+=stm32f4xx_cec.c
#SRCS+=stm32f4xx_crc.c
#SRCS+=stm32f4xx_cryp_aes.c
#SRCS+=stm32f4xx_cryp.c
#SRCS+=stm32f4xx_cryp_des.c
#SRCS+=stm32f4xx_cryp_tdes.c
#SRCS+=stm32f4xx_dac.c
#SRCS+=stm32f4xx_dbgmcu.c
#SRCS+=stm32f4xx_dcmi.c
#SRCS+=stm32f4xx_dfsdm.c
#SRCS+=stm32f4xx_dma2d.c
#SRCS+=stm32f4xx_dma.c
#SRCS+=stm32f4xx_dsi.c
SRCS+=stm32f4xx_exti.c
#SRCS+=stm32f4xx_flash.c
#SRCS+=stm32f4xx_flash_ramfunc.c
#SRCS+=stm32f4xx_fmc.c
#SRCS+=stm32f4xx_fmpi2c.c
#SRCS+=stm32f4xx_fsmc.c
SRCS+=stm32f4xx_gpio.c
#SRCS+=stm32f4xx_hash.c
#SRCS+=stm32f4xx_hash_md5.c
#SRCS+=stm32f4xx_hash_sha1.c
#SRCS+=stm32f4xx_i2c.c
#SRCS+=stm32f4xx_iwdg.c
#SRCS+=stm32f4xx_lptim.c
#SRCS+=stm32f4xx_ltdc.c
#SRCS+=stm32f4xx_pwr.c
#SRCS+=stm32f4xx_qspi.c
SRCS+=stm32f4xx_rcc.c
#SRCS+=stm32f4xx_rng.c
#SRCS+=stm32f4xx_rtc.c
#SRCS+=stm32f4xx_sai.c
#SRCS+=stm32f4xx_sdio.c
#SRCS+=stm32f4xx_spdifrx.c
#SRCS+=stm32f4xx_spi.c
SRCS+=stm32f4xx_syscfg.c
SRCS+=stm32f4xx_tim.c
SRCS+=stm32f4xx_usart.c
#SRCS+=stm32f4xx_wwdg.c


all: program size
# SRCS WORKS WITH VPATH
# VPATH only works with prerequisites
program:$(SRCS)
	@mkdir -p $(BUILD_DIR)
	$(CC) $^ $(INCLUDE) $(DEFS) $(CFLAGS) $(LFLAGS) -o $(BUILD_DIR)/$(PROJ_NAME).elf
	$(OBJCOPY) -O ihex $(BUILD_DIR)/$(PROJ_NAME).elf   $(BUILD_DIR)/$(PROJ_NAME).hex
	$(OBJCOPY) -O binary $(BUILD_DIR)/$(PROJ_NAME).elf $(BUILD_DIR)/$(PROJ_NAME).bin

clean:
	rm -f $(BUILD_DIR)/*

flash:
	st-flash write $(BUILD_DIR)/$(PROJ_NAME).bin 0x8000000

size:
	arm-none-eabi-size --format=berkeley $(BUILD_DIR)/$(PROJ_NAME).elf

debug:
	gnome-terminal --command="st-util"
	arm-none-eabi-gdb -q $(BUILD_DIR)/$(PROJ_NAME).elf -ex "tar extended-remote :4242" -ex "load"

erase:
	st-flash erase

rebuild: clean all
reflash: erase flash
