#include "hax.h"
#include "user_serialdrv.h"

void putc(char data) {
	/* From the IFI Default Code printf_lib.c. */
	TXREG = data;
	Wait4TXEmpty();
}

void puth(uint16_t data) {
	uint8_t i, digit;
	
	putc('0');
	putc('x');
	
	for (i = 0; i < 4; ++i) {
		/* Each four-bit chunk of a binary number is a hex digit. */
		digit  = (uint8_t) ((data & 0xF000) >> 12);
		data <<= 4;
		
		if (digit < 10) {
			putc('0' + digit);
		} else {
			putc('A' + digit - 10);
		}
	}
}

void puti(uint16_t data) {
	char buf[6];
	uint8_t i;
	
	/* Generate the output in reverse order (i.e. the left-most digit is the
	 * most significant digit).
	 */
	for (i = 0; data > 0; ++i) {
		buf[i] = '0' + (data % 10);
		data  /= 10;
	}
	
	/* Reverse the string prior to sending it to the serial port. */
	for (++i; i > 0; --i) {
		putc(buf[i - 1]);
	}
}

void putf(float data) {
	/* TODO: Essentially implement ftoa() and send the results to putc(). */
}

void puts(char *data) {
	for (; *data; ++data) {
		putc(*data);
	}
}