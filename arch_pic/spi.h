#ifndef SPI_H__
#define SPI_H__
#include <stdint.h>
/* SSPSTAT REGISTER */
// Master SPI mode only

#define   SMPEND        0b10000000     // Input data sample at end of data out
#define   SMPMID        0b00000000     // Input data sample at middle of data out

#define   MODE_00       0b00000000     // Setting for SPI bus Mode 0,0
//CKE           0x40                   // SSPSTAT register 
//CKP           0x00                   // SSPCON1 register 

#define   MODE_01       0b00000001     // Setting for SPI bus Mode 0,1
//CKE           0x00                   // SSPSTAT register 
//CKP           0x00                   // SSPCON1 register

#define   MODE_10       0b00000010     // Setting for SPI bus Mode 1,0
//CKE           0x40                   // SSPSTAT register
//CKP           0x10                   // SSPCON1 register

#define   MODE_11       0b00000011     // Setting for SPI bus Mode 1,1
//CKE           0x00                   // SSPSTAT register
//CKP           0x10                   // SSPCON1 register

/* SSPCON1 REGISTER */
#define   SSPENB        0b00100000	// enable serial port 
					// and config SCK, SDO, SDI
#define   SPI_FOSC_4    0b00000000	// SPI Master mode, clock = Fosc/4
#define   SPI_FOSC_16   0b00000001	// SPI Master mode, clock = Fosc/16
#define   SPI_FOSC_64   0b00000010	// SPI Master mode, clock = Fosc/64
#define   SPI_FOSC_TMR2 0b00000011	// SPI Master mode, clock = TMR2 output/2
#define   SLV_SSON      0b00000100	// SPI Slave mode, /SS pin control enabled
#define   SLV_SSOFF     0b00000101	// SPI Slave mode, /SS pin control disabled

void spi_open(unsigned char sync_mode, unsigned char bus_mode,
	      unsigned char smp_phase);
unsigned char spi_write(unsigned char data_out);
unsigned char spi_read(void);

#endif				/* SPI_H_ */
