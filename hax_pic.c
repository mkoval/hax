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


// This is much nicer with inline functions.
#define EXPR_ ( (0xF & x) + '0' )
// If the calculated character is not in the numeric range, push it into the
//  Capital letter range ( 7 away in ascii )
#define H2ASCI(x) (EXPR_>'9')?(EXPR_+7):(EXPR_)
#define PUTH4( x ) putc( H2ASCI( x ) ) 
#define PUTH8( x ) ( PUTH4( (x) >> 4 ) , PUTH4( (x) & 0xF ) )

void puth(uint8_t data) {
	PUTH8(data);
}

void puth2(uint16_t data) {
	PUTH8( data >> 8 );
	PUTH8( data & 0xFF );
}
