/*
 * Hardware Specific Code,
 * PIC Arch
 */
#include <adc.h>
#include <delays.h>
#include <hax.h>
#include <p18cxxx.h>
#include <stdio.h>
#include <usart.h>

#include "master.h"
#include "ifi_lib.h"

/* Slow loop of 18.5 milliseconds (converted to microseconds). */
uint16_t kSlowSpeed = 18500;

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

/*
 * INITIALIZATION AND MISC
 */
void setup_1(void) {
	uint8_t i;

	IFI_Initialization();

	/* Initialize serial port communication. */
	statusflag.b.NEW_SPI_DATA = 0;

	Open1USART(USART_TX_INT_OFF &
		USART_RX_INT_OFF &
		USART_ASYNCH_MODE &
		USART_EIGHT_BIT &
		USART_CONT_RX &
		USART_BRGH_HIGH,
		kBaud115);
	Delay1KTCYx( 50 ); 

	/* Make the master control all PWMs (for now) */
	txdata.pwm_mask.a = 0xFF;

	for (i = 0; i < 22; ++i) {
		pin_set_io( i, kInput);
	}

	/* Init ADC */

	/* Setup the number of analog sensors. The PIC defines a series of 15
	 * ADC constants in mcc18/h/adc.h that are all of the form 0b1111xxxx,
	 * where x counts the number of DIGITAL ports. In total, there are
	 * sixteen ports numbered from 0ANA to 15ANA.
	 */
	_puts("[ADC INIT : "); 
	if ( NUM_ANALOG_VALID(kNumAnalogInputs) && kNumAnalogInputs > 0 ) {
		/* ADC_FOSC: Based on a baud_115 value of 21, given the formula
		 * FOSC/(16(X + 1)) in table 18-1 of the PIC18F8520 doc the FOSC
		 * is 40Mhz.
		 * Also according to the doc, section 19.2, the
		 * ADC Freq needs to be at least 1.6us or 0.625MHz. 40/0.625=64
		 * (Also, see table 19-1 in the chip doc)
		 */
		OpenADC( ADC_FOSC_64 & ADC_RIGHT_JUST & 
		                       ( 0xF0 | (16 - kNumAnalogInputs) ) ,
		                       ADC_CH0 & ADC_INT_OFF & ADC_VREFPLUS_VDD &
		       		           ADC_VREFMINUS_VSS );
		_puts("DONE ]\r\n");
	} else { 
		_puts("FAIL ]\r\n");
	}

}

void setup_2(void) {
	User_Proc_Is_Ready();
}

void spin(void) {
}

int16_t get_battery(void) {
	uint8_t tmp;
	LVDCON = 0b1110;
	for(;;) {
		PIE2bits.LVDIE = 0;
		LVDCONbits.LVDEN = 1;
		while(!LVDCONbits.IRVST);
		PIR2bits.LVDIF = 0;
		
		if ( !PIR2bits.LVDIF || !(LVDCON & 0xF) ) {
			LVDCON = 0;
			return (LVDCON & 0xF) + 1;
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

bool new_data_received(void) {
	return statusflag.b.NEW_SPI_DATA;
}

static CtrlMode mode_s = kAuton;

uint8_t check_oi(void) {
	uint8_t i;
	for(i = 0; i < 16; i++) {
		if ( (rxdata.oi_analog[i] > 0xdf) || (rxdata.oi_analog[i] < 0x1f) ) {
			return 1;
		}
	}
	return 0;
}

CtrlMode mode_get(void) {
	if (rxdata.rcstatusflag.b.oi_on) {
		if (mode_s != kTelop) {
			if (check_oi()) {
				mode_s = kTelop;
			}
		}
		return mode_s;
	} else {
		mode_s = kAuton;
		return kDisable;
	}
}

void mode_set(CtrlMode mode) {
}

/*
 * DIGITAL AND ANALOG IO
 */

#define BIT_HI(x, i)     ((x) |=  1 << (i))
#define BIT_LO(x, i)     ((x) &= ~(1 << (i)))
#define BIT_SET(x, i, v) ((v) ? BIT_HI((x), (i)) : BIT_LO((x), (i)))

void pin_set_io(PinIx i, PinMode mode) {
	uint8_t bit = (mode == kInput);

	switch (i) {
	
	case 0:
	case 1:
	case 2:
	case 3:
		BIT_SET(TRISA, i, bit);
		break;

	case 4:
		BIT_SET(TRISA, 5, bit);
		break;

	/* Inputs 5 through 11 are stored consecutively in the TRISF register,
	 * starting at bit zero.
	 */
	case 5:
	case 6:
	case 7:
	case 8:
	case 9:
	case 10:
	case 11:
		BIT_SET(TRISF, (i - 5) , bit);
		break;

	/* The reimaining inputs, 12 through 15, are stored starting at bit 4 in
	 * the TRISH register.
	 */
	case 12:
	case 13:
	case 14:
	case 15:
		BIT_SET(TRISH, (i - 12) + 4, bit);
		break;
	
	/* access the interrupt pins */
	case 16:
	case 17:
	case 18:
	case 19:
	case 20:
	case 21:
		BIT_SET(TRISB, (i - 16) + 2, bit);
	}
}

#define BIT_GET(_reg_,_index_) ( ( _reg_ & 1 << _index_ ) >> _index_ )

bool digital_get(PinIx i) {

	switch (i) {
	
	case 0:
	case 1:
	case 2:
	case 3:
		return BIT_GET(PORTA,i);

	case 4:
		return BIT_GET(PORTA, 5);

	/* Inputs 5 through 11 are stored consecutively in the TRISF register,
	 * starting at bit zero.
	 */
	case 5:
	case 6:
	case 7:
	case 8:
	case 9:
	case 10:
	case 11:
		return BIT_GET(PORTF, (i - 5));

	/* The reimaining inputs, 12 through 15, are stored starting at bit 4 in
	 * the TRISH register.
	 */
	case 12:
	case 13:
	case 14:
	case 15:
		return BIT_GET(PORTH, (i - 12) + 4);

	
	/* access the interrupt pins */
	case 16:
	case 17:
	case 18:
	case 19:
	case 20:
	case 21:
		return BIT_GET(PORTB, (i - 16) + 2);

	default:
		return -1;
	}
}

uint16_t analog_adc_get(PinIx ain) {
	if ( ain < kNumAnalogInputs && NUM_ANALOG_VALID(kNumAnalogInputs)  ) {
		/* 0 <= ain < 16 */
		/* read ADC (0b10000111 = 0x87) */
		uint8_t chan = 0x87 | ain << 3;
		SetChanADC(chan);
		Delay10TCYx( 5 ); /* Wait for capacitor to charge */
		ConvertADC();
		while( BusyADC() );
		return ReadADC();
	}
	else {
		/* ain is not valid */
		return 0xFFFF;
	}
}

int8_t analog_oi_get(OIIx ain) {
	if ( ain >= kAnalogSplit && ain < (kAnalogSplit + kVPNumOIInputs) ) {
		/* 127 <= ain < (127 + 16) */
		/* ain refers to one of the off robot inputs */
		int8_t v = rxdata.oi_analog[ ain - kAnalogSplit ] - 128;
		return (v < 0) ? v + 1 : v;
	}
	return 0;
}

void analog_set(AnalogOutIx aout, AnalogOut sp) {

	if ( aout < kVPMaxMotors ) {
#if defined(MIKE_WHAT____)
		int16_t val = (int16_t)sp + 127;

		/* Constrain the value to fit in a uint8. */
		if (val > 255) {
			val = 255;
		} else if (val < 0) {
			val = 0;
		}
#else
		uint8_t val;
		sp = ( sp < 0 && sp != -128) ? sp - 1 : sp;
		val = sp + 128;
#endif
		txdata.rc_pwm[aout] = (uint8_t)val;
	}
}

void motor_set(AnalogOutIx aout, MotorSpeed sp) {
	analog_set(aout,sp);
}

void servo_set(AnalogOutIx aout, ServoPosition sp) {
	analog_set(aout,sp);
}

/*
 * INTERRUPTS
 */
InterruptServiceRoutine isr_callbacks[6] = { 0 };

void interrupt_reg_isr(InterruptIx index, InterruptServiceRoutine isr) {
	isr_callbacks[index] = isr;
}

bool interrupt_get(InterruptIx index) {
	/* There are 16 digital pins, so the first interrupt is pin 16. */
	return digital_get(16 + index);
}

#if   defined(MCC18_30)
#pragma interruptlow interrupt_handler nosave=section(".tmpdata"),TBLPTRU,TBLPTRH,TBLPTRL,TABLAT,PCLATH,PCLATU
#elif defined(MCC18_24)
#pragma interruptlow interrupt_handler save=PROD,section("MATH_DATA"),section(".tmpdata")
#else
#error Interrupts are unsuported with this compiler.
#endif
void interrupt_handler(void) {
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

#pragma code interrupt_vector=0x818
void interrupt_vector(void) {
	/* There's not much space for this function... */
	_asm
	goto interrupt_handler
	_endasm
}
#pragma code

/* TODO Implement interrupt_disable(). */

void interrupt_enable(InterruptIx index) {
	switch (index) {
	case 0:
		TRISBbits.TRISB2    = 1;
		INTCON3bits.INT2IP  = 0;
		INTCON3bits.INT2IF  = 0;
		INTCON2bits.INTEDG2 = 1;
		INTCON3bits.INT2IE  = 1;
		break;
	
	case 1:
		TRISBbits.TRISB3    = 1;
		INTCON2bits.INT3IP  = 0;
		INTCON2bits.INTEDG3 = 1;
		INTCON3bits.INT3IF  = 0;
		INTCON3bits.INT3IE  = 1;
		break;
	
	case 2:
	case 3:
	case 4:
	case 5:
		TRISBbits.TRISB4 = 1;
		TRISBbits.TRISB5 = 1;
		TRISBbits.TRISB6 = 1;
		TRISBbits.TRISB7 = 1;
  		INTCON2bits.RBIP = 0;
		INTCONbits.RBIF  = 0;
		INTCONbits.RBIE  = 1;
		break;
	}
}

/*
 * TIMERS
 */
#if 0
InterruptServiceRoutine isr_callbacks[5] = { 0, 0, 0, 0, 0 };

void timer_set(TimerIx index, bool enabled) {
	uint8_t flags = TO_16BIT & TO_PS_1_256 & 
	switch (index) {
	case 0:
		OpenTimer0(TO_16BIT & TO_PS_1_256 
}
#endif

/*
 * STREAM IO
 */
void _putc(char data) {
	/* From the Microchip C Library Docs */
	while(Busy1USART());
	Write1USART(data);
}
 
/* IFI lib uses this. (IT BURNNNNSSSS) */
void Wait4TXEmpty(void) {
	while(Busy1USART());
}

