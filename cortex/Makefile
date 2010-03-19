# hax/cortex

LDSCRIPT_PATH = ./cortex/easyclib
LD_SCRIPT = scripts/STM32F103_384K_64K_FLASH.ld
EASYC_LIB = cortex/easyclib/easyCRuntimeLib.lib
STARTUP_OBJ = cortex/easyclib/Startup.o
STARTUPI_OBJ = cortex/easyclib/StartupInit.o

ARCH_CFLAGS  += -mcpu=cortex-m3 -D_STM32F103VDH6_ -D_STM3x_ -D_STM32x_ -mthumb \
               -fsigned-char -ffunction-sections -Wall -mlittle-endian        \
               -Wno-main 
ARCH_LFLAGS += -L$(LDSCRIPT_PATH) -Wl,-T,$(LD_SCRIPT)                    \
               $(EASYC_LIB) $(STARTUP_OBJ) $(STARTUPI_OBJ)              \
               -L'cortex/vexlib/Link' -mpu=cortex-m3 -mthumb -u _start  \
               -Wl,-static -Wl,--gc-sections -nostartfiles

SOURCE += $(ARCH)/hax.c
HEADERS += 

include $(ARCH)/gcc.mk
