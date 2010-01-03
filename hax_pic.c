#include <usart.h>
#include "hax.h"

void putb(uint8_t data) {
	while(Busy1USART());
	Write1USART(data);
}

void putc(char data) {
	/* From the Microchip C Library Docs */
	while(Busy1USART());
	Write1USART(data);
}
