/*
 * Hardware Specific Code,
 * PIC Arch
 */
#include <adc.h>
#include <hax.h>
#include <stdio.h>
#include <usart.h>

#if defined(MCC18)
#include <p18cxxx.h>
#include <delays.h>
#include "compat_mcc18.h"
#elif defined(SDCC)
#include <pic18fregs.h>
#include <delay.h>
#include "usart_sdcc.h"
#else
#error "Bad compiler"
#endif

#include "master.h"
#include "ifi_lib.h"

/* Slow loop of 18.5 milliseconds (converted to microseconds). */
uint16_t const kSlowSpeed = 18500;

/* Checks if the kNumAnalog is valid */
#define NUM_ANALOG_VALID(_x_) ( (_x_) <= 16 && (_x_) != 15 )
#define kVPMaxMotors 8
#define kVPNumOIInputs 16

/* Variables used for master proc comms by both our code and IFI's lib.
 *  Do not rename.
 */
TxData txdata;
RxData rxdata;
StatusFlags statusflag;

/* Used to save the state of port b before the interrupt handler for ports 3
 * through 6 are called.
 */

typedef enum
{
  kBaud19 = 128,
  kBaud38 = 64,
  kBaud56 = 42,
  kBaud115 = 21
} SerialSpeed;

static state_t mode_s = MODE_AUTON;

/*
 * INITIALIZATION AND MISC
 */
void setup_1(void) {
	uint8_t i;

	IFI_Initialization();

	/* Initialize serial port communication. */
	statusflag.b.NEW_SPI_DATA = 0;

	usart1_open(USART_TX_INT_OFF
	         & USART_RX_INT_OFF
	         & USART_ASYNCH_MODE
	         & USART_EIGHT_BIT
	         & USART_CONT_RX
	         & USART_BRGH_HIGH,
	           kBaud115);
	delay1ktcy(50);

	/* Make the master control all PWMs (for now) */
	txdata.pwm_mask.a = 0xFF;

	for (i = 0; i < 22; ++i) {
		pin_set_io(i, false);
	}

	/* Init ADC */

	/* Setup the number of analog sensors. The PIC defines a series of 15
	 * ADC constants in mcc18/h/adc.h that are all of the form 0b1111xxxx,
	 * where x counts the number of DIGITAL ports. In total, there are
	 * sixteen ports numbered from 0ANA to 15ANA.
	 */
	if ( NUM_ANALOG_VALID(kNumAnalogInputs) && kNumAnalogInputs > 0 ) {
		/* ADC_FOSC: Based on a baud_115 value of 21, given the formula
		 * FOSC/(16(X + 1)) in table 18-1 of the PIC18F8520 doc the 
		 * FOSC is 40Mhz.
		 * Also according to the doc, section 19.2, the
		 * ADC Freq needs to be at least 1.6us or 0.625MHz. 40/0.625=64
		 * (Also, see table 19-1 in the chip doc)
		 */
		#if defined(MCC18)
		OpenADC( ADC_FOSC_64 & ADC_RIGHT_JUST & 
		                       ( 0xF0 | (16 - kNumAnalogInputs) ) ,
		                       ADC_CH0 & ADC_INT_OFF & ADC_VREFPLUS_VDD &
		       		           ADC_VREFMINUS_VSS );
		#elif defined(SDCC)
		adc_open(
			ADC_CHN_0,
			ADC_FOSC_64,
			pcfg,
			ADC_INT_OFF & ADC_FRM_RJUST);
		#else
		#error "Bad Comp"
		#endif
	} else { 
		/* TODO: Handle the error. */
		puts("ADC is disabled");
	}

}

void setup_2(void) {
	User_Proc_Is_Ready();
}

void spin(void) {
}

uint8_t battery_get(void) {
	uint8_t tmp;
	/* 0b1110 is the highest detectable voltage level */
	LVDCON = 0b1110;
	for(;;) {
		PIE2bits.LVDIE = 0;
		LVDCONbits.LVDEN = 1;
#if defined(MCC18)
		while(!LVDCONbits.IRVST);
#elif defined(SDCC)
		while(!LVDCONbits.VRST);
#else
#error "bad compiler"
#endif
		PIR2bits.LVDIF = 0;
	
		tmp = LVDCON & 0xF;
		if (!(PIR2bits.LVDIF)||!tmp) {
			LVDCON = 0;
			return tmp + 1;
		}

		LVDCONbits.LVDEN = 0;
		LVDCON --;
	}
	LVDCON = 0;
}

void loop_1(void) {
	Getdata(&rxdata);
	#ifdef DEBUG
	{
	uint8_t i;
	printf((char*)
		   "rxdata:\n"
		   "  packet_num rcmode rcstatusflag: %i %i %i\n"
		   , rxdata.packet_num
		   , rxdata.rcmode.allbits
		   , rxdata.rcstatusflag.allbits);
		
	_puts( "  reserved_1[0..2] : ");
	for(i = 0; i < 3; i++) {
		printf((char*)"%i, ",rxdata.reserved_1[i]);
	}

	_puts( ";"
		   "  reserved_2[0..8] : ");
	for(i = 0; i < 8; i++) {
		printf((char*)"%i, ",rxdata.reserved_2[i]);
	}

	printf((char*) "\n"
		   "  master_version : %i\n", rxdata.master_version);

	printf((char*)
		    "statusflag: 0x%02x\n"
			"  ",statusflag.a);
	for(i = 0; i < 8; i++) {
		printf((char*)"%i, ", (statusflag.a & (1<<i) ) >> i);
	}
	_putc('\n');
	}
	#endif
}

void loop_2(void) {
	Putdata(&txdata);
}

bool new_data_received(void)
{
	return statusflag.b.NEW_SPI_DATA;
}

static bool check_oi(void)
{
	uint8_t i;
	for(i = 0; i < 16; i++) {
		if((rxdata.oi_analog[i] > 0xdf)
		            || (rxdata.oi_analog[i] < 0x1f)) {
			return true;
		}
	}
	return false;
}

state_t mode_get(void)
{
	if (rxdata.rcstatusflag.b.oi_on) {
		if (mode_s != MODE_TELOP) {
			if (check_oi()) {
				mode_s = MODE_TELOP;
			}
		}
		return mode_s;
	} else {
		mode_s = MODE_AUTON;
		return MODE_DISABLE;
	}
}

/*
 * DIGITAL AND ANALOG IO
 */
#define BIT_HI(x, i)     ((x) |=  1 << (i))
#define BIT_LO(x, i)     ((x) &= ~(1 << (i)))
#define BIT_SET(x, i, v) ((v) ? BIT_HI((x), (i)) : BIT_LO((x), (i)))

void pin_set_io(index_t i, bool bit) {
	switch (i) {
	case 1:
	case 2:
	case 3:
	case 4:
		BIT_SET(TRISA, i - 1, bit);
		break;

	case 5:
		BIT_SET(TRISA, 5 - 1, bit);
		break;

	/* Inputs 5 through 11 are stored consecutively in the TRISF register,
	 * starting at bit zero.
	 */
	case 6:
	case 7:
	case 8:
	case 9:
	case 10:
	case 11:
	case 12:
		BIT_SET(TRISF, (i - 5) - 1 , bit);
		break;

	/* The reimaining inputs, 12 through 15, are stored starting at 
	 * bit 4 in the TRISH register.
	 */
	case 13:
	case 14:
	case 15:
	case 16:
		BIT_SET(TRISH, (i - 12) + 4 - 1, bit);
		break;
	
	/* Access the interrupt pins */
	case 17:
	case 18:
	case 19:
	case 20:
	case 21:
	case 22:
		BIT_SET(TRISB, (i - 16) + 2 - 1, bit);
	}
}

#define BIT_GET(_reg_,_index_) ( ( _reg_ & 1 << _index_ ) >> _index_ )

bool digital_get(index_t i) {
	switch (i) {
	case 1:
	case 2:
	case 3:
	case 4:
		return BIT_GET(PORTA, i - 1);

	case 5:
		return BIT_GET(PORTA, 5 - 1);

	/* Inputs 5 through 11 are stored consecutively in the TRISF register,
	 * starting at bit zero.
	 */
	case 6:
	case 7:
	case 8:
	case 9:
	case 10:
	case 11:
	case 12:
		return BIT_GET(PORTF, (i - 5) - 1);

	/* The reimaining inputs, 12 through 15, are stored starting at bit 4 in
	 * the TRISH register.
	 */
	case 13:
	case 14:
	case 15:
	case 16:
		return BIT_GET(PORTH, (i - 12) + 4 - 1);
	
	/* access the interrupt pins */
	case 17:
	case 18:
	case 19:
	case 20:
	case 21:
	case 22:
		return BIT_GET(PORTB, (i - 16) + 2 - 1);

	default:
		ERROR();
		return false;
	}
}

uint16_t analog_adc_get(index_t index) {
	if (1 <= index && index <= kNumAnalogInputs && NUM_ANALOG_VALID(kNumAnalogInputs)) {
		/* Read ADC (0b10000111 = 0x87). */
		uint8_t chan = 0x87 | (index-1) << 3;
		adc_setchannel(chan);
		delay10tcy(5); /* Wait for capacitor to charge */
		adc_conv();
		while(adc_busy());
		return adc_read();
	} else {
		ERROR();
		return 0xFFFF;
	}
}

int8_t analog_oi_get(index_t index) {
	if (1 <= index && index <= kVPNumOIInputs) {
		int8_t v = rxdata.oi_analog[index - 1] - 128;
		return (v < 0) ? v + 1 : v;
	} else {
		ERROR();
		return 0;
	}
}

bool digital_io_get(index_t index) {
	/* All ports on the old transmitter are analog, including the
	 * buttons.
	 * FIXME: the buttons aren't reallly.
	 */
	ERROR();
	return false;
}

void analog_set(index_t index, int8_t sp)
{
	if (1 <= index && index <= kVPMaxMotors) {
		uint8_t val;
		sp = ( sp < 0 && sp != -128) ? sp - 1 : sp;
		val = sp + 128;
		txdata.rc_pwm[index - 1] = (uint8_t)val;
	} else {
		ERROR();
	}
}

/*
 * INTERRUPTS
 */
static isr_t isr_callbacks[6];

void interrupt_reg_isr(index_t index, isr_t isr)
{
	if (17 <= index && index <= 22) {
		isr_callbacks[index - 17] = isr;
	} else {
		ERROR();
	}
}

#if   defined(MCC18)
  #if MCC18 >= 300
    #pragma interruptlow interrupt_handler nosave=TBLPTRU,TBLPTRH,TBLPTRL,TABLAT
  #else
    #pragma interruptlow interrupt_handler save=PROD,PCLATH,PCLATU,section("MATH_DATA"),section(".tmpdata")
  #endif
#elif defined(SDCC)
  // nada.
#else
 #error "Bad compiler."
#endif

static void interrupt_handler(void)
{
	static uint8_t delta, portb_old = 0xFF, portb = 0xFF;

	/* Interrupt 1 */
	if (INTCON3bits.INT2IF && INTCON3bits.INT2IE) { 
		INTCON3bits.INT2IF = 0;

		if (isr_callbacks[0]) {
			isr_callbacks[0](INTCON2bits.INTEDG2);
			INTCON2bits.INTEDG2 ^= 1;
		}
	}
	/* Interrupt 2 */
	else if (INTCON3bits.INT3IF && INTCON3bits.INT3IE) {
		INTCON3bits.INT3IF = 0;

		if (isr_callbacks[1]) {
			isr_callbacks[1](INTCON2bits.INTEDG3);
			INTCON2bits.INTEDG3 ^= 1;
		}
	}
	else if (INTCONbits.RBIF && INTCONbits.RBIE) {
		/* Remove the "mismatch condition" by reading port b. */
		portb           = PORTB;
		INTCONbits.RBIF = 0;
		delta           = portb ^ portb_old;
		portb_old       = portb;
	 
		/* Interrupt 3 */
		if((delta & 0x10) && isr_callbacks[2]) {
			isr_callbacks[2]((portb & (1<<4)) >> 4);
		}

		/* Interrupt 4 */
		if((delta & 0x20) && isr_callbacks[3]) {
			isr_callbacks[3]((portb & (1<<5)) >> 5);
		}

		/* Interrupt 5 */
		if((delta & 0x40) && isr_callbacks[4]) {
			isr_callbacks[4]((portb & (1<<6)) >> 6);
		}

		/* Interrupt 6 */
		if((delta & 0x80) && isr_callbacks[5]) {
			isr_callbacks[5]((portb & (1<<7)) >> 7);
		}
	}
}

#if defined(SDCC)
void interrupt_vector(void) __naked __interrupt 2
{
	__asm
	goto _interrupt_handler
	__endasm;
}
#elif defined(MCC18)
#pragma code interrupt_vector=0x818
void interrupt_vector(void)
{
	_asm
	goto interrupt_handler
	_endasm
}
#pragma code
#else
#error "Bad Compiler."
#endif


/* TODO: Implement interrupt_disable(). */

void interrupt_enable(index_t index)
{
	switch (index) {
	case 17:
		TRISBbits.TRISB2    = 1;
		INTCON3bits.INT2IP  = 0;
		INTCON3bits.INT2IF  = 0;
		INTCON2bits.INTEDG2 = 1;
		INTCON3bits.INT2IE  = 1;
		break;
	
	case 18:
		TRISBbits.TRISB3    = 1;
#if defined(MCC18)
		INTCON2bits.INT3IP  = 0;
#elif defined(SDCC)
		INTCON2bits.INT3P   = 0;
#endif
		INTCON2bits.INTEDG3 = 1;
		INTCON3bits.INT3IF  = 0;
		INTCON3bits.INT3IE  = 1;
		break;
	
	case 19:
	case 20:
	case 21:
	case 22:
		TRISBbits.TRISB4 = 1;
		TRISBbits.TRISB5 = 1;
		TRISBbits.TRISB6 = 1;
		TRISBbits.TRISB7 = 1;
  		INTCON2bits.RBIP = 0;
		INTCONbits.RBIF  = 0;
		INTCONbits.RBIE  = 1;
		break;
	
	default:
		ERROR();
	}
}

/*
 * STREAM IO
 */

/* used in ifi_util.asm */
void usart1_busywait(void)
{
	while(usart1_busy());
}

void _putc(char c)
{
	usart1_busywait();
	usart1_putc(c);
}
