/*
 * Hardware Specific Code,
 * PIC Arch
 */
#include "hax.h"
#include <usart.h>

void putc(char data) {
	/* From the Microchip C Library Docs */
	while(Busy1USART());
	Write1USART(data);
}
