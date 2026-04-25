#########################################################################################################
#
# Default Location of GCC Compile Ref Files
#########################################################################################################


#########################################################################################################
###                                Different Configuration For Different Project
#########################################################################################################

## Set Vars by Input Value
# target name
    
CHIP_TYPE :=  0
BOOT2SIZE := 0x1000

ROOT_DIR          = ../../..
MAKE_TARGET_NAME := gcc_example

## Set default compile ref files
LINK_SCRIPT     = ./gcc_example.ld

LIB_FILE  =  $(ROOT_DIR)/library/win04_rom_gcc_lib.lib

GCC_STARTUP_ASM_FILE =  $(ROOT_DIR)/source/device/ARMCM0plus/startup_gcc/startup_ARMCM0plus.s

PRJ_C_MICRO_DEFINES :=   \
WIN04  \
QMX_FPGA \
NT_SHELL \


PRJ_ASM_INCLUDE_PATH :=  \


PRJ_ASM_MICRO_DEFINES :=   \

########################################## Here add C file ##############################################
PRJ_C_SRC_FILES:=\
$(ROOT_DIR)/source/device/ARMCM0plus/startup_gcc/system_ARMCM0plus.c \
$(ROOT_DIR)/source/device/win04/version.c \
$(ROOT_DIR)/library/boot2.c \
$(ROOT_DIR)/source/component/debug/debug.c \
$(ROOT_DIR)/source/component/crypto/aes/sw_aes.c \
$(ROOT_DIR)/source/component/crypto/crc/sw_crc.c \
$(ROOT_DIR)/source/component/private_spi/qmx_private_spi_frame.c \
$(ROOT_DIR)/source/component/nt_shell/nttop.c \
$(ROOT_DIR)/source/component/nt_shell/core/src/ntlibc.c \
$(ROOT_DIR)/source/component/nt_shell/core/src/ntshell.c \
$(ROOT_DIR)/source/component/nt_shell/core/src/text_editor.c \
$(ROOT_DIR)/source/component/nt_shell/core/src/text_history.c \
$(ROOT_DIR)/source/component/nt_shell/core/src/vtrecv.c \
$(ROOT_DIR)/source/component/nt_shell/core/src/vtsend.c \
$(ROOT_DIR)/source/component/nt_shell/util/src/ntopt.c \
$(ROOT_DIR)/source/component/nt_shell/util/src/ntstdio.c \
$(ROOT_DIR)/source/drivers/win04/hw_flash/src/qmx_afc.c \
$(ROOT_DIR)/source/drivers/win04/hw_flash/src/qmx_cad.c \
$(ROOT_DIR)/source/drivers/win04/hw_flash/src/qmx_phy_trx.c \
$(ROOT_DIR)/source/drivers/win04/hw_flash/src/qmx_phy.c \
$(ROOT_DIR)/source/drivers/win04/hw_flash/src/qmx_rf.c \
$(ROOT_DIR)/source/drivers/win04/hw_flash/src/qmx_sf_search.c \
$(ROOT_DIR)/source/drivers/win04/hw_flash/src/qmx_subg.c \
$(ROOT_DIR)/source/drivers/win04/hw_flash/src/qmx_cali.c \
$(ROOT_DIR)/source/drivers/win04/hw_flash/src/qmx_debug_ila.c \
$(ROOT_DIR)/source/drivers/win04/hw_flash/src/qmx_lpwr_ctrl.c \
$(ROOT_DIR)/source/drivers/win04/hw_flash/src/qmx_intc.c \
$(ROOT_DIR)/source/drivers/win04/hw_flash/src/qmx_systick.c \
$(ROOT_DIR)/source/drivers/win04/patch/src/qmx_aes.c \
$(ROOT_DIR)/source/drivers/win04/patch/src/qmx_dma.c \
$(ROOT_DIR)/source/drivers/win04/patch/src/qmx_gpio.c \
$(ROOT_DIR)/source/drivers/win04/patch/src/qmx_pmu.c \
$(ROOT_DIR)/source/drivers/win04/patch/src/qmx_rtc.c \
$(ROOT_DIR)/source/drivers/win04/patch/src/qmx_uart.c \
$(ROOT_DIR)/source/hal/src/qmx_hal_gpio.c \
$(ROOT_DIR)/source/hal/src/qmx_hal_intc.c \
$(ROOT_DIR)/source/hal/src/qmx_hal_uart.c \
$(ROOT_DIR)/source/hal/src/qmx_hal_iwdt.c \
$(ROOT_DIR)/source/hal/src/qmx_hal_wdt.c \
$(ROOT_DIR)/source/hal/src/qmx_hal_pmu.c \
$(ROOT_DIR)/source/hal/src/qmx_hal_rtc.c \
$(ROOT_DIR)/source/hal/src/qmx_hal_sysctrl.c \
./main.c

ifneq ($(findstring QMX_SOCTEST,$(PRJ_C_MICRO_DEFINES)),)
PRJ_C_SRC_FILES += \
$(ROOT_DIR)/source/project_source/soc_test/algo/qmx_aes_test.c \
$(ROOT_DIR)/source/project_source/soc_test/algo/qmx_crc_test.c \
$(ROOT_DIR)/source/project_source/soc_test/algo/qmx_rng_test.c \
$(ROOT_DIR)/source/project_source/soc_test/gpio/qmx_gpio_test.c \
$(ROOT_DIR)/source/project_source/soc_test/NVIC/qmx_nvic_test.c \
$(ROOT_DIR)/source/project_source/soc_test/RTC/qmx_rtc_test.c \
$(ROOT_DIR)/source/project_source/soc_test/uart/qmx_uart_test.c \
$(ROOT_DIR)/source/project_source/soc_test/WDT/qmx_wdt_test.c \
$(ROOT_DIR)/source/project_source/soc_test/DMA/qmx_dma_test.c \
$(ROOT_DIR)/source/project_source/soc_test/flash/qmx_flash_test.c \
$(ROOT_DIR)/source/project_source/soc_test/IIC/qmx_iic_test.c \
$(ROOT_DIR)/source/project_source/soc_test/IWDT/qmx_iwdt_test.c \
$(ROOT_DIR)/source/project_source/soc_test/lowpower/qmx_low_power_test.c \
$(ROOT_DIR)/source/project_source/soc_test/mem/qmx_mem_test.c \
$(ROOT_DIR)/source/project_source/soc_test/phy/qmx_phy_test.c \
$(ROOT_DIR)/source/project_source/soc_test/soc_test/qmx_soc_test.c \
$(ROOT_DIR)/source/project_source/soc_test/soc_test/qmx_test_master_slave_common.c \
$(ROOT_DIR)/source/project_source/soc_test/soc_test/qmx_test_master.c \
$(ROOT_DIR)/source/project_source/soc_test/soc_test/qmx_test_slave.c \
$(ROOT_DIR)/source/project_source/soc_test/soc_test/qmx_uart_cmd_process.c \
$(ROOT_DIR)/source/project_source/soc_test/systick/qmx_systick_test.c \
$(ROOT_DIR)/source/project_source/soc_test/timer/qmx_timer_test.c \
$(ROOT_DIR)/source/project_source/soc_test/timer/qmx_pwm_test.c
endif

########################################## Here add h index ##############################################
PRJ_C_INCLUDE_PATH := \
$(ROOT_DIR)/source/device/ARMCM0plus/Include \
$(ROOT_DIR)/source/device/CMSIS_Core/Include \
$(ROOT_DIR)/source/device/reg_device \
$(ROOT_DIR)/source/device/win04 \
$(ROOT_DIR)/source/component/error_def    \
$(ROOT_DIR)/source/component/utility    \
$(ROOT_DIR)/source/component/debug \
$(ROOT_DIR)/source/component/crypto/aes \
$(ROOT_DIR)/source/component/crypto/crc \
$(ROOT_DIR)/source/component/nt_shell/core/inc \
$(ROOT_DIR)/source/component/nt_shell/util/inc \
$(ROOT_DIR)/source/component/nt_shell \
$(ROOT_DIR)/source/component/private_spi \
$(ROOT_DIR)/source/drivers/win04/register \
$(ROOT_DIR)/source/drivers/win04/hw_rom/inc \
$(ROOT_DIR)/source/drivers/win04/hw_flash/inc \
$(ROOT_DIR)/source/drivers/win04/patch/inc \
$(ROOT_DIR)/source/mem_cfg/flash \
$(ROOT_DIR)/source/mem_cfg/win04 \
$(ROOT_DIR)/source/hal/inc \
./config

ifneq ($(findstring QMX_SOCTEST,$(PRJ_C_MICRO_DEFINES)),)
PRJ_C_INCLUDE_PATH += \
$(ROOT_DIR)/source/project_source/soc_test/algo \
$(ROOT_DIR)/source/project_source/soc_test/gpio \
$(ROOT_DIR)/source/project_source/soc_test/NVIC \
$(ROOT_DIR)/source/project_source/soc_test/RTC\
$(ROOT_DIR)/source/project_source/soc_test/uart \
$(ROOT_DIR)/source/project_source/soc_test/WDT\
$(ROOT_DIR)/source/project_source/soc_test/DMA\
$(ROOT_DIR)/source/project_source/soc_test/flash\
$(ROOT_DIR)/source/project_source/soc_test/IIC \
$(ROOT_DIR)/source/project_source/soc_test/IWDT \
$(ROOT_DIR)/source/project_source/soc_test/lowpower \
$(ROOT_DIR)/source/project_source/soc_test/mem \
$(ROOT_DIR)/source/project_source/soc_test/phy \
$(ROOT_DIR)/source/project_source/soc_test/soc_test \
$(ROOT_DIR)/source/project_source/soc_test/systick \
$(ROOT_DIR)/source/project_source/soc_test/timer
endif


# Set echo cmd
ECHO = @echo

#########################################################################################################
###                                   Common Configuration Area, Change carefully
#########################################################################################################

## Set Compiler (CC/ASM use same compile cmd for now)
CROSS_COMPILE   = arm-none-eabi-
CC              = $(CROSS_COMPILE)gcc
ASM             = $(CROSS_COMPILE)gcc
CPP             = $(CROSS_COMPILE)cpp
LINK            = $(CROSS_COMPILE)gcc
OBJCOPY         = $(CROSS_COMPILE)objcopy
OBJDUMP         = $(CROSS_COMPILE)objdump
NM              = $(CROSS_COMPILE)nm  	# 添加这一行，定义nm工具
SIZE            = $(CROSS_COMPILE)size  # 可选：添加size工具用于显示段大小信息

## Set Common Flags for C/ASM
COMMON_COMPILE_FLAGS += -std=gnu99 --inline -O1
COMMON_COMPILE_FLAGS += -ggdb3 -specs=nano.specs -u _printf_float
COMMON_COMPILE_FLAGS += -ffunction-sections -fdata-sections
COMMON_COMPILE_FLAGS += -mfloat-abi=softfp -mapcs-frame -mthumb-interwork -mthumb -mcpu=cortex-m0plus
COMMON_COMPILE_FLAGS += -gdwarf-2 -MD

## Set LDFLAGS
LDFLAGS += -Wl,--gc-sections

## Set CFLAGS
# Set include path
CFLAGS += $(foreach inc,$(PRJ_C_INCLUDE_PATH),-I $(inc))
# Set macro-defines Flags
CFLAGS += $(foreach md,$(PRJ_C_MICRO_DEFINES),-D$(md))
CFLAGS += $(COMMON_COMPILE_FLAGS)


## Set ASMFLAGS
ASMFLAGS += $(foreach inc,$(PRJ_ASM_INCLUDE_PATH),-I $(inc))
# Set macro-defines Flags
ASMFLAGS += $(foreach md,$(PRJ_ASM_MICRO_DEFINES),-D$(md))
ASMFLAGS += $(COMMON_COMPILE_FLAGS)

## Set compile output directory
BUILD       ?= ./Build
BUILD_OBJ   ?= ./Objects
BUILD_LST   ?= ./Objects


## Set source files and objects
SRC_C   := $(PRJ_C_SRC_FILES)
SRC_ASM := $(GCC_STARTUP_ASM_FILE) $(PRJ_ASM_SRC_FILES)
OBJ_C   := $(SRC_C:.c=.o)
OBJ_ASM := $(SRC_ASM:.s=.o)

OBJ     := $(OBJ_C) $(OBJ_ASM)
OBJ_ADJUST 	= $(patsubst %.o,$(BUILD_OBJ)/%.o,$(notdir $(OBJ)))


## verbosity switch
V ?= 1
ifeq ($(V),0)
    V_CC = @echo " $(CC) " $<;
    V_ASM = @echo " $(ASM) " $<;
    V_CPP = @echo " $(CPP) " $<;
    V_LINK = @echo " $(LINK) " $<;
    V_OBJCOPY = @echo " $(OBJCOPY) " $<;
else
    V_OPT = '-v'
endif


SRC_PATH += $(dir $(SRC_C))
SRC_PATH += $(dir $(SRC_ASM))
MAKE_PATH = $(foreach n,$(SRC_PATH),:$(n))
vpath %.c ./$(MAKE_PATH)
vpath %.s ./$(MAKE_PATH)
## default make goal
all: mkdir $(BUILD)/$(MAKE_TARGET_NAME).bin $(BUILD)/$(MAKE_TARGET_NAME).hex $(BUILD)/$(MAKE_TARGET_NAME).s $(BUILD)/$(MAKE_TARGET_NAME).nm $(BUILD)/$(MAKE_TARGET_NAME).dis $(BUILD)/$(MAKE_TARGET_NAME).size makefirm

##  compile C & asm files
$(BUILD_OBJ)/%.o : %.c
	$(V_CC) $(CC) $(CFLAGS) -c $< -o $@

## how to compile assembly files
$(BUILD_OBJ)/%.o : %.s
	$(V_ASM) $(ASM) $(ASMFLAGS) -c $< -o $@

## make depends
$(BUILD)/$(MAKE_TARGET_NAME).hex: $(BUILD_LST)/$(MAKE_TARGET_NAME).elf
#	$(ECHO) "compile hex file ..."
	$(V_OBJCOPY) $(OBJCOPY) -O ihex $< $@

$(BUILD)/$(MAKE_TARGET_NAME).bin: $(BUILD_LST)/$(MAKE_TARGET_NAME).elf
#	$(ECHO) "compile binary file ..."
	$(V_OBJCOPY) $(OBJCOPY) -O binary $< $@

$(BUILD)/$(MAKE_TARGET_NAME).s: $(BUILD_LST)/$(MAKE_TARGET_NAME).elf
#	$(ECHO) "compile asm file ..."
	$(OBJDUMP) -l -S $(BUILD_LST)/$(MAKE_TARGET_NAME).elf > $(BUILD_LST)/$(MAKE_TARGET_NAME).s

$(BUILD_LST)/$(MAKE_TARGET_NAME).elf: $(OBJ_ADJUST)
#	$(ECHO) "compile .elf file ..."
	$(V_LINK) $(LINK) $(CFLAGS) -T $(LINK_SCRIPT) $(LIB_FILE) $(OBJ_ADJUST) $(LDFLAGS) -Wl,-Map=$(BUILD_LST)/$(MAKE_TARGET_NAME).map -o $@

# 生成符号表文件(.nm)
$(BUILD)/$(MAKE_TARGET_NAME).nm: $(BUILD_LST)/$(MAKE_TARGET_NAME).elf
	$(ECHO) "generate nm file ..."
	$(NM) -n -S $< > $@

# 生成反汇编文件(.dis)
$(BUILD)/$(MAKE_TARGET_NAME).dis: $(BUILD_LST)/$(MAKE_TARGET_NAME).elf
	$(ECHO) "generate dis file ..."
	$(OBJDUMP) -h -d -S $< > $@

# 可选：添加生成段大小信息文件(.size)
$(BUILD)/$(MAKE_TARGET_NAME).size: $(BUILD_LST)/$(MAKE_TARGET_NAME).elf
	$(ECHO) "generate size file ..."
	$(SIZE) $<

$(OBJECT_DIR):
	@mkdir -p $@

#########################################################################################################
# OS Detection and Command Adaptation
#########################################################################################################

# Detect OS
ifeq ($(OS),Windows_NT)
    DETECTED_OS := Windows
else
    DETECTED_OS := $(shell uname -s)
endif

# Define OS-specific commands
ifeq ($(DETECTED_OS),Windows)
    # Windows commands (using standard cmd syntax)
    # Note: We use 'if exist' instead of 'rm -rf' because 'rm' might not be in PATH
    RM_CMD = if exist $(subst /,\,$(1)) rmdir /s /q $(subst /,\,$(1))
    MKDIR_CMD = if not exist $(subst /,\,$(1)) mkdir $(subst /,\,$(1))
else
    # Linux/macOS commands
    RM_CMD = rm -rf $(1)
    MKDIR_CMD = mkdir -p $(1)
endif

#########################################################################################################
# Targets
#########################################################################################################
.PHONY: clean makefirm

makefirm: $(BUILD)/$(MAKE_TARGET_NAME).bin
	python ./format_firmware_header_gcc.py $(BUILD)/$(MAKE_TARGET_NAME) $(BOOT2SIZE)

clean:
	$(call RM_CMD,$(BUILD))
	$(call RM_CMD,$(BUILD_OBJ))

mkdir:
	$(call MKDIR_CMD,$(BUILD))
	$(call MKDIR_CMD,$(BUILD_OBJ))

-include $(wildcard $(OBJECT_DIR)/*.d)
