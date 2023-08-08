TARGET = f103
DEBUG = 1

BUILD_DIR = build
TN_DIR = ../../../src

C_SOURCES = \
Src/main.c \
$(TN_DIR)/tn_app_check.c \
$(TN_DIR)/core/tn_dqueue.c \
$(TN_DIR)/core/tn_eventgrp.c \
$(TN_DIR)/core/tn_fmem.c \
$(TN_DIR)/core/tn_list.c \
$(TN_DIR)/core/tn_mutex.c \
$(TN_DIR)/core/tn_sem.c \
$(TN_DIR)/core/tn_sys.c \
$(TN_DIR)/core/tn_tasks.c \
$(TN_DIR)/core/tn_timer.c \
$(TN_DIR)/core/tn_timer_dyn.c \
$(TN_DIR)/core/tn_timer_static.c \
$(TN_DIR)/arch/cortex_m/tn_arch_cortex_m_c.c

ASM_SOURCES = \
Src/startup_stm32f103xb.S \
$(TN_DIR)/arch/cortex_m/tn_arch_cortex_m.S

PREFIX = arm-none-eabi-
# The gcc compiler bin path can be either defined in make command via GCC_PATH variable (> make GCC_PATH=xxx)
# either it can be added to the PATH environment variable.
ifdef GCC_PATH
CC = $(GCC_PATH)/$(PREFIX)gcc
AS = $(GCC_PATH)/$(PREFIX)gcc -x assembler-with-cpp
CP = $(GCC_PATH)/$(PREFIX)objcopy
SZ = $(GCC_PATH)/$(PREFIX)size
else
CC = $(PREFIX)gcc
AS = $(PREFIX)gcc -x assembler-with-cpp
CP = $(PREFIX)objcopy
SZ = $(PREFIX)size
endif
HEX = $(CP) -O ihex
BIN = $(CP) -O binary -S

CPU = -mcpu=cortex-m3
FPU =
FLOAT-ABI = -mfloat-abi=soft
MCU = $(CPU) -mthumb $(FPU) $(FLOAT-ABI)

# AS defines
AS_DEFS =

# C defines
C_DEFS = \
-D STM32F103xB

# AS includes
AS_INCLUDES = \
-I Inc \
-I ${TN_DIR} \
-I ${TN_DIR}/core

# C includes
C_INCLUDES = \
-I Inc \
-I ${TN_DIR} \
-I ${TN_DIR}/core \
-I ${TN_DIR}/arch \
-I ${TN_DIR}/core/internal

ifeq ($(DEBUG), 1)
OPT = -O0 -g3 -gdwarf-5
else
OPT = -O2 -g0
endif

COMMON_FLAGS = -Wall -Wextra -fdata-sections -ffunction-sections
CSTD = -std=c99 -Wpedantic

ASFLAGS = $(MCU) $(AS_DEFS) $(AS_INCLUDES) $(OPT) $(COMMON_FLAGS)
CFLAGS = $(MCU) $(C_DEFS) $(C_INCLUDES) $(OPT) $(CSTD) $(COMMON_FLAGS) -Wsign-conversion -Wconversion

# Generate dependency information
CFLAGS += -MMD -MP -MF"$(@:%.o=%.d)"

LDSCRIPT = Src/STM32F103C8Tx_FLASH.ld

# libraries
LIBS = -lc -lm
LDFLAGS = --specs=nano.specs $(MCU) -T$(LDSCRIPT) $(LIBS) -Wl,-Map=$(BUILD_DIR)/$(TARGET).map,--cref -Wl,--gc-sections

# default action: build all
all: $(BUILD_DIR)/$(TARGET).elf $(BUILD_DIR)/$(TARGET).hex $(BUILD_DIR)/$(TARGET).bin


#######################################
# build the application
#######################################
# list of objects
OBJECTS = $(addprefix $(BUILD_DIR)/,$(notdir $(C_SOURCES:.c=.o)))
vpath %.c $(sort $(dir $(C_SOURCES)))
# list of ASM program objects
OBJECTS += $(addprefix $(BUILD_DIR)/,$(notdir $(ASM_SOURCES:.S=.o)))
vpath %.S $(sort $(dir $(ASM_SOURCES)))

$(BUILD_DIR)/%.o: %.c Makefile | $(BUILD_DIR)
	$(CC) -c $(CFLAGS) -Wa,-a,-ad,-alms=$(BUILD_DIR)/$(notdir $(<:.c=.lst)) $< -o $@

$(BUILD_DIR)/%.o: %.S Makefile | $(BUILD_DIR)
	$(AS) -c $(ASFLAGS) $< -o $@

$(BUILD_DIR)/$(TARGET).elf: $(OBJECTS)
	$(CC) $(OBJECTS) $(LDFLAGS) -o $@
	$(SZ) $@

$(BUILD_DIR)/%.hex: $(BUILD_DIR)/%.elf | $(BUILD_DIR)
	$(HEX) $< $@

$(BUILD_DIR)/%.bin: $(BUILD_DIR)/%.elf | $(BUILD_DIR)
	$(BIN) $< $@

$(BUILD_DIR):
	mkdir $@


#######################################
# clean up
#######################################
clean:
	-rm -fR $(BUILD_DIR)


#######################################
# dependencies
#######################################
-include $(wildcard $(BUILD_DIR)/*.d)
