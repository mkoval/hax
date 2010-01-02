#include "hax.h"
#include "user_serialdrv.h"

void putc(char data) {
	/* From the IFI Default Code printf_lib.c. */
	TXREG = data;
	Wait4TXEmpty();
}

void puti(uint16_t data, uint8_t radix) {
	char buf[6];
	uint8_t i;
	
	/* Generate the output in reverse order (i.e. the left-most digit is the
	 * most significant digit).
	 */
	for (i = 0; data > 0; ++i) {
		buf[i] = '0' + (data % radix);
		data  /= radix;
	}
	
	/* Reverse the string prior to sending it to the serial port. */
	for (++i; i > 0; --i) {
		putc(buf[i - 1]);
	}
}

void puth(uint16_t data) {
	putc(data, '0');
	putc(data, 'x');
	puti(data, 16);
}