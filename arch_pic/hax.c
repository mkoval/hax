/*
 * Hardware Specific Code,
 * PIC Arch
 */
#include <stdio.h>
#include <adc.h>
#include <hax.h>
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

#ifndef USER_CT_ANALOG
#define USER_CT_ANALOG 0
#endif

/* Checks if the USER_CT_ANALOG is valid */
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

typedef enum {
	kBaud19 = 128,
	kBaud38 = 64,
	kBaud56 = 42,
	kBaud115 = 21
} SerialSpeed;

static state_t mode_s = MODE_AUTON;

/*
 * INITIALIZATION AND MISC
 */
void setup_1(void)
{
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
	if (NUM_ANALOG_VALID(USER_CT_ANALOG) && USER_CT_ANALOG > 0) {
		/* ADC_FOSC: Based on a baud_115 value of 21, given the formula
		 * FOSC/(16(X + 1)) in table 18-1 of the PIC18F8520 doc the
		 * FOSC is 40Mhz.
		 * Also according to the doc, section 19.2, the
		 * ADC Freq needs to be at least 1.6us or 0.625MHz. 40/0.625=64
		 * (Also, see table 19-1 in the chip doc)
		 */
#if defined(MCC18)
		OpenADC( ADC_FOSC_64 & ADC_RIGHT_JUST &
		                       ( 0xF0 | (16 - USER_CT_ANALOG) ) ,
		                       ADC_CH0 & ADC_INT_OFF & ADC_VREFPLUS_VDD &
				           ADC_VREFMINUS_VSS );
#elif defined(SDCC)
		adc_open(
			ADC_CHN_0,
			ADC_FOSC_64,
			ADC_CFG_16A,
			ADC_FRM_RJUST | ADC_INT_OFF | ADC_VCFG_VDD_VSS );
#else
#error "Bad Comp"
#endif
	} else {
		/* TODO: Handle the error. */
		puts("ADC is disabled");
	}
}

void setup_2(void)
{
	User_Proc_Is_Ready();
}

void spin(void)
{}

uint8_t battery_get(void)
{
	uint8_t tmp;
	uint8_t lvdcon;
	/* 0b1110 is the highest detectable voltage level */
	LVDCON = 0xE; // 0b1110
	for(;;) {
		lvdcon = LVDCON;
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


		tmp = lvdcon & 0xF;
		if (!(PIR2bits.LVDIF)||!tmp) {
			LVDCON = 0;
			return tmp + 1;
		}

		LVDCONbits.LVDEN = 0;

		lvdcon --;
		if ((tmp - 1) == 0) {
			LVDCON = 0;
			return 0;
		}
	}
}

void loop_1(void)
{
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

		_puts( "; reserved_2[0..8] : ");
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

void loop_2(void)
{
	Putdata(&txdata);
}

bool new_data_received(void)
{
	return statusflag.b.NEW_SPI_DATA;
}

/* Determines whether any oi input is outside of a middle range
 * 0xff is split into 3 parts, 0xdf and 0x1f are the edges
 */
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

void digital_setup(index_t i, bool output)
{
	switch (i) {
	/* Digitals 1...4, bits 0,1,2,3 in port A */
	case IX_DIGITAL(1):
	case IX_DIGITAL(2):
	case IX_DIGITAL(3):
	case IX_DIGITAL(4):
		BIT_SET(TRISA, i - IX_DIGITAL(1), output);
		break;

	/* Digital 5, bit 4 in port A */
	case IX_DIGITAL(5):
		BIT_SET(TRISA, 4, output);
		break;

	/* Digitals 6...12, bits 0,1... in port F */
	case IX_DIGITAL(6):
	case IX_DIGITAL(7):
	case IX_DIGITAL(8):
	case IX_DIGITAL(9):
	case IX_DIGITAL(10):
	case IX_DIGITAL(11):
	case IX_DIGITAL(12):
		BIT_SET(TRISF, i - IX_DIGITAL(6) , output);
		break;

	/* Digitals 13...16, bits 4,5,6,7 in port H. */
	case IX_DIGITAL(13):
	case IX_DIGITAL(14):
	case IX_DIGITAL(15):
	case IX_DIGITAL(16):
		BIT_SET(TRISH, i - IX_DIGITAL(13) + 4, output);
		break;

	/* Interrupts (all), bits 2,3,4,5,6,7 in port B */
	case IX_INTERRUPT(1):
	case IX_INTERRUPT(2):
	case IX_INTERRUPT(3):
	case IX_INTERRUPT(4):
	case IX_INTERRUPT(5):
	case IX_INTERRUPT(6):
		BIT_SET(TRISB, i - IX_INTERRUPT(1) + 2, output);
	}
}

#define BIT_GET(_reg_,_index_) ( ( _reg_ & 1 << _index_ ) >> _index_ )

bool digital_get(index_t i)
{
	switch (i) {
	/* Digitals 1...4, bits 0,1,2,3 in port A */
	case IX_DIGITAL(1):
	case IX_DIGITAL(2):
	case IX_DIGITAL(3):
	case IX_DIGITAL(4):
		return BIT_GET(PORTA, i - IX_DIGITAL(1) + 0);

	/* Digital 5, bit 4 in port A */
	case IX_DIGITAL(5):
		return BIT_GET(PORTA, 4);

	/* Digitals 6...12, bits 0,1... in port F */
	case IX_DIGITAL(6):
	case IX_DIGITAL(7):
	case IX_DIGITAL(8):
	case IX_DIGITAL(9):
	case IX_DIGITAL(10):
	case IX_DIGITAL(11):
	case IX_DIGITAL(12):
		return BIT_GET(PORTF, i - IX_DIGITAL(6) + 0);

	/* Digitals 13...16, bits 4,5,6,7 in port H. */
	case IX_DIGITAL(13):
	case IX_DIGITAL(14):
	case IX_DIGITAL(15):
	case IX_DIGITAL(16):
		return BIT_GET(PORTH, i - IX_DIGITAL(13) + 4);

	/* Interrupts (all), bits 2,3,4,5,6,7 in port B */
	case IX_INTERRUPT(1):
	case IX_INTERRUPT(2):
	case IX_INTERRUPT(3):
	case IX_INTERRUPT(4):
	case IX_INTERRUPT(5):
	case IX_INTERRUPT(6):
		return BIT_GET(PORTB, i - IX_INTERRUPT(1) + 2);

	/* TODO: OI digitals. */

	default:
		ERROR();
		return false;
	}
}

int8_t oi_group_get(index_t index)
{
	if (IX_OI(1,1) <= index && index <= IX_OI(2, CT_OI_GROUP_SZ)) {
		int8_t v = rxdata.oi_analog[IX_OI_INV(index)] - 128;
		return (v < 0) ? v + 1 : v;
	} else {
		ERROR("index: %d", index);
		return -128;
	}
}

int8_t oi_trigger_get(index_t ix)
{
}

bool oi_button_get(index_t ix)
{
}

uint16_t analog_get(index_t index)
{
	if (IX_ANALOG(1) <= index && index <= IX_ANALOG(USER_CT_ANALOG) && NUM_ANALOG_VALID(USER_CT_ANALOG)) {
		/* Read ADC (0b10000111 = 0x87). */
		uint8_t chan = 0x87 | (IX_ANALOG_INV(index) << 3);
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

void analog_set(index_t index, int8_t sp)
{
	if (IX_MOTOR(1) <= index && index <= IX_MOTOR(CT_MOTOR)) {
		uint8_t val;
		sp = ( sp < 0 && sp != -128) ? sp - 1 : sp;
		val = sp + 128;
		txdata.rc_pwm[IX_MOTOR_INV(index)] = (uint8_t)val;
	} else {
		ERROR("index: %d", index);
	}
}

/*
 * INTERRUPTS
 */
static isr_t isr_callbacks[6];
static isr_t isr_inactive_cb[4];

void interrupt_setup(index_t index, isr_t isr)
{
	if (IX_INTERRUPT(1) <= index && index <= IX_INTERRUPT(CT_INTERRUPT)) {
		uint8_t i = IX_INTERRUPT_INV(index);
		if (i > 1) {
			isr_inactive_cb[i - 2] = isr;
		} else {
			isr_callbacks[i] = isr;
		}
	} else {
		ERROR();
	}
}

#if   defined(MCC18)
  #if MCC18 >= 300
    #pragma interruptlow isr_low nosave=TBLPTRU,TBLPTRH,TBLPTRL,TABLAT
  #else
    #pragma interruptlow isr_low save=PROD,PCLATH,PCLATU,section("MATH_DATA"),section(".tmpdata")
  #endif
#elif defined(SDCC)
  // nada.
#else
  #error "Bad compiler."
#endif

void isr_low(void)
{
	static uint8_t delta, portb_old = 0xFF, portb = 0xFF;

	if (INTCON3bits.INT2IF && INTCON3bits.INT2IE) {
		/* Interrupt 1 */
		INTCON3bits.INT2IF = 0;

		if (isr_callbacks[0]) {
			isr_callbacks[0](INTCON2bits.INTEDG2);
			INTCON2bits.INTEDG2 ^= 1;
		}
	} else if (INTCON3bits.INT3IF && INTCON3bits.INT3IE) {
		/* Interrupt 2 */
		INTCON3bits.INT3IF = 0;

		if (isr_callbacks[1]) {
			isr_callbacks[1](INTCON2bits.INTEDG3);
			INTCON2bits.INTEDG3 ^= 1;
		}
	} else if (INTCONbits.RBIF && INTCONbits.RBIE) {
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
void ivt_low(void) __naked __interrupt 2
{
	__asm
	GOTO _isr_low
	__endasm;
}
#elif defined(MCC18)
#pragma code ivt_low=0x818
void ivt_low(void)
{
	_asm
	GOTO isr_low
	_endasm
}
#pragma code
#else
#error "Bad Compiler."
#endif


static void interrupt_disable(index_t index)
{
	switch (index) {
	case IX_INTERRUPT(1):
		INTCON3bits.INT2IE = 0;
		break;

	case IX_INTERRUPT(2):
		INTCON3bits.INT3IE = 0;
		break;

	case IX_INTERRUPT(3):
	case IX_INTERRUPT(4):
	case IX_INTERRUPT(5):
	case IX_INTERRUPT(6):
		isr_callbacks[IX_INTERRUPT_INV(index) - 2] = NULL;
	}
}

void interrupt_set(index_t index, bool enable)
{
	if (!enable) {
		interrupt_disable(index);
		return;
	}

	switch (index) {
	case IX_INTERRUPT(1):
		TRISBbits.TRISB2    = 1;
		INTCON3bits.INT2IP  = 0;
		INTCON3bits.INT2IF  = 0;
		INTCON2bits.INTEDG2 = 1;
		INTCON3bits.INT2IE  = 1;
		break;

	case IX_INTERRUPT(2):
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

	case IX_INTERRUPT(3):
	case IX_INTERRUPT(4):
	case IX_INTERRUPT(5):
	case IX_INTERRUPT(6): {
		uint8_t i = IX_INTERRUPT_INV(index);
		isr_callbacks[i] = isr_inactive_cb[i - 2];
		TRISBbits.TRISB4 = 1;
		TRISBbits.TRISB5 = 1;
		TRISBbits.TRISB6 = 1;
		TRISBbits.TRISB7 = 1;
		INTCON2bits.RBIP = 0;
		INTCONbits.RBIF  = 0;
		INTCONbits.RBIE  = 1;
		break;
	}

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
