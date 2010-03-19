# hax/cortex

LDSCRIPT_PATH = cortex/scripts
LD_SCRIPT = STM32F103_384K_64K_FLASH.ld

EASYC_LIB_PATH = cortex/easyclib
EASYC_LIB = cortex/easyclib/easyCRuntimeLib.lib
STARTUP_OBJ = cortex/easyclib/Startup.o
STARTUPI_OBJ = cortex/easyclib/StartupInit.o

ARCH_CFLAGS += -mcpu=cortex-m3 -D_STM32F103VDH6_ -D_STM3x_ -D_STM32x_ -mthumb \
               -fsigned-char -ffunction-sections -Wall -mlittle-endian       \
               -Wno-main 
ARCH_LFLAGS += $(ARCH_CFLAGS) -Wl,-L$(LDSCRIPT_PATH) -Wl,-T$(LD_SCRIPT) \
	      $(STARTUP_OBJ) $(STARTUPI_OBJ) -u _start                 \
	      -Wl,-static,--gc-sections -nostartfiles                  \
              -l:$(EASYC_LIB)

SOURCE += $(ARCH)/hax.c
HEADERS += 

include $(ARCH)/gcc.mk
