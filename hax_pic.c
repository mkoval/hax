/*
 * Hardware Specific Code,
 * PIC Arch
 */
#include "hax.h"
#include "user_serialdrv.h"

void putc(char data) {
	/* From the IFI Default Code printf_lib.c. */
	TXREG = data;
	Wait4TXEmpty();
}
