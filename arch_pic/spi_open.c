#ifndef SDCC
#error "Only for sdcc"
#endif /* SDCC */

#include <pic18fregs.h>
#include "spi.h"

void spi_open(unsigned char sync_mode, unsigned char bus_mode,
	      unsigned char smp_phase)
{
	SSPSTAT &= 0x3F; // power on state 
	SSPCON1 = 0x00;	 // power on state
	SSPCON1 |= sync_mode; // select serial mode 
	SSPSTAT |= smp_phase; // select data input sample phase
	switch (bus_mode) {
	case 0:	// SPI bus mode 0,0
		SSPSTATbits.CKE = 1; // data transmitted on falling edge
		break;
	case 2:	// SPI bus mode 1,0
		SSPSTATbits.CKE = 1; // data transmitted on rising edge
		SSPCON1bits.CKP = 1; // clock idle state high
		break;
	case 3:	// SPI bus mode 1,1
		SSPCON1bits.CKP = 1; // clock idle state high
		break;
	default: // default SPI bus mode 0,1
		break;
	}
	switch (sync_mode) {
	case 4:	// slave mode w /SS enable
		TRISFbits.TRISF7 = 1; // define /SS pin as input
		TRISCbits.TRISC3 = 1; // define clock pin as input
		break;
	case 5:	// slave mode w/o /SS enable
		TRISCbits.TRISC3 = 1; // define clock pin as input        
		break;
	default:// master mode, define clock pin as output
		TRISCbits.TRISC3 = 0; // define clock pin as input        
		break;
	}
	TRISCbits.TRISC4 = 1; // define SDI pin as input
	TRISCbits.TRISC5 = 0; // define SDO pin as output
	SSPCON1 |= SSPENB;    // enable synchronous serial port 
}
