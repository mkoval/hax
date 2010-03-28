FWLIB_PATH    = ./$(ARCH)/fwlib
LDSCRIPT_PATH = ./$(ARCH)/scripts
LD_SCRIPT     = STM32F103_384K_64K_FLASH.ld

SOURCE  += $(ARCH)/hax.c                     \
           $(ARCH)/vexlib/cortexm3_macro.s   \
           $(ARCH)/vexlib/ddt_support.c      \
           $(ARCH)/vexlib/stm32f10x_it.c     \
           $(ARCH)/vexlib/stm32f10x_vector.c \
           $(ARCH)/vexlib/utilities.c

HEADERS += $(ARCH)/vexlib/platform_config.h  \
           $(ARCH)/vexlib/stm32f10x_conf.h   \
           $(ARCH)/vexlib/stm32f10x_it.h     \
           $(ARCH)/vexlib/stm32f10x_vector.c

include $(ARCH)/gcc.mk
