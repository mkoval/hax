/*
 * Hardware Specific Code,
 * PIC Arch
 */
#include <p18cxxx.h>
#include <usart.h>
#include <adc.h>
#include <delays.h>
#include "hax.h"
#include "vex_pic/master.h"
#include "vex_pic/ifi_lib.h"

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

	statusflag.NEW_SPI_DATA = 0;

	Open1USART(USART_TX_INT_OFF &
		USART_RX_INT_OFF &
		USART_ASYNCH_MODE &
		USART_EIGHT_BIT &
		USART_CONT_RX &
		USART_BRGH_HIGH,
		kBaud115);
	Delay1KTCYx( 50 ); 

	/* Enable autonomous mode. FIXME: Magic Number (we need an enum of valid "user_cmd"s) */
	/* txdata.user_cmd = 0x02; */

	/* Make the master control all PWMs (for now) */
	txdata.pwm_mask.a = 0xFF;

	for (i = 0; i < 16; ++i) {
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
		_puts("DONE ]\n");
	} else { 
		_puts("FAIL ]\n");
	}

}

void setup_2(void) {
	User_Proc_Is_Ready();
}

void spin(void) {

}

void loop_1(void) {
	Getdata(&rxdata);
}

void loop_2(void) {
	Putdata(&txdata);
}

bool new_data_received(void) {
	return statusflag.NEW_SPI_DATA;
}

CtrlMode get_mode(void) {
	return (rxdata.rcmode.mode.autonomous) ? kAuton : kTelop;
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
	}
}

#define BIT_GET(_reg_,_index_) ( ( _reg_ & 1 << _index_ ) >> _index_ )

int8_t digital_get(PinIx i) {

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
		uint8_t val;
		sp = ( sp < 0 && sp != 128) ? sp - 1 : sp;
		val = sp + 128;

		/* 127 & 128 are treated as the same, apparently. */
		txdata.rc_pwm[aout] = val; //(val > 127) ? val+1 : val;
	}
}

void motor_set(AnalogOutIx aout, MotorSpeed sp) {
	analog_set(aout,sp);
}

void servo_set(AnalogOutIx aout, ServoPosition sp) {
	analog_set(aout,sp);
}

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
