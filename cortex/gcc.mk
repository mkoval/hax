TARGET     = $(PROG)-$(ARCH).hex
TARGET_ELF = $(TARGET:.hex=.elf)

PREFIX  = 'arm-elf'

CC      = $(PREFIX)-gcc
LD      = $(PREFIX)-gcc
AS      = $(PREFIX)-as
OBJCOPY = $(PREFIX)-objcopy

OBJECTS = $(SOURCE:=.o)
TRASH   = $(OBJECTS)

.SECONDARY : 
.SUFFIXES:

rebuild : clean all

clean :
	@echo "CLEAN"
	@$(RM) $(OBJECTS) $(TARGET) $(TARGET_ELF) $(TRASH)

$(TARGET_ELF) : $(OBJECTS)
	@echo "LD $(@F)"
	@$(LD) $(ALL_LFLAGS) $^ -o $@

%.hex : %.elf
	@echo "HEX $(@F)"
	@$(OBJCOPY) --target=ihex $< $@ 

%.c.o : %.c $(HEADERS) 
	@echo "CC $(@F)"
	@$(CC) $(ALL_CFLAGS) -c $< -o $@ 

%.s.o : %.s $(HEADERS)
	@echo "AS $(@F)"
	@$(AS) $(ALL_AFLAGS) -c $< -o $@

.PHONY : all clean install rebuild
