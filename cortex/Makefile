# hax/cortex

LDSCRIPT_PATH  = cortex/scripts
LD_SCRIPT      = STM32F103_384K_64K_FLASH.ld

EASYC_LIB_PATH = cortex/easyclib
EASYC_LIB      = cortex/easyclib/easyCRuntimeLib.lib
STARTUP_OBJ    = cortex/easyclib/Startup.o
STARTUPI_OBJ   = cortex/easyclib/StartupInit.o

SOURCE  += $(ARCH)/hax.c
HEADERS += 

include $(ARCH)/gcc.mk
