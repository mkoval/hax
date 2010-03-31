# hax/cortex

LDSCRIPT_PATH  = $(ARCH)/scripts
LD_SCRIPT      = STM32F103_384K_64K_FLASH.ld

EASYC_LIB_PATH = $(ARCH)/easyclib
EASYC_LIB      = $(ARCH)/easyclib/easyCRuntimeLib.lib
STARTUP_OBJ    = $(ARCH)/easyclib/Startup.o
STARTUPI_OBJ   = $(ARCH)/easyclib/StartupInit.o

SOURCE  += $(ARCH)/hax.c
HEADERS += 

include $(ARCH)/gcc.mk
