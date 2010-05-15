TARGET=libstm32.a
SOURCE=$(wildcard $(srcdir)/fwlib/src/*.c)     \
       $(wildcard $(srcdir)/*.c)               \
       $(srcdir)/startup/startup_stm32f10x_hd.s

srcdir=.
#VPATH=$(srcdir)/fwlib/src $(srcdir)

HEADER=$(wildcard $(srcdir)/*.h) \
       $(wildcard $(srcdir)/fwlib/inc/*.h)
OBJ=$(SOURCE:=.o)

GCC_PATH=
GCC_PREFIX=$(GCC_PATH)arm-none-eabi-

FIND=find
CC=$(GCC_PREFIX)gcc
AS=$(GCC_PREFIX)gcc
OBJCOPY=$(GCC_PREFIX)objcopy

CC_INC=-I$(srcdir) -I$(srcdir)/fwlib/inc

# When changing boards, modify STMPROC, HSE_VALUE, and the
#  startup asm code.

STMPROC=STM32F10X_HD
HSE_VALUE=8000000

ALL_CFLAGS=-MD -D$(STMPROC) -DHSE_VALUE=$(HSE_VALUE) \
           -mthumb -mcpu=cortex-m3 -Wall -g -O2      \
           -Wno-main -DUSE_STDPERIPH_DRIVER          \
           $(CC_INC) $(CFLAGS)

ALL_ASFLAGS=$(ALL_CFLAGS)

.SUFFIXES:

all: $(TARGET)
	
.SECONDARY:

%.s.o: %.s $(HEADER)
	$(AS) $(ALL_ASFLAGS) -c -o $@ $<

%.c.o: %.c $(HEADER)
	$(CC) $(ALL_CFLAGS) -c -o $@ $<
	
$(TARGET) : $(OBJ)
	$(AR) $(ARFLAGS) $@ $^

clean:
	@$(FIND) . -regex '.*\.\([oda]\|elf\|hex\|bin\|map\)'\
		-printf 'RM %P\n' -delete

.PHONY: clean all
