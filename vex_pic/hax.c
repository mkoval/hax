/*
 * Hardware Specific Code,
 * PIC Arch
 */
#include <p18cxxx.h>
#include <usart.h>
#include <adc.h>
#include <delays.h>
#include "hax.h"
#include "pic/master.h"
#include "pic/ifi_lib.h"

/* Slow loop of 18.5 milliseconds (converted to microseconds). */
uint16_t kSlowSpeed = 18500;

/* Checks if the kNumAnalog is valid */
#define NUM_ANALOG_VALID(x) ( (x) < 16 && (x) != 15 )

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
	
	/* Prevent the "slow loop" from executing until data has been received
	 * from the master processor.
	 */
	statusflag.NEW_SPI_DATA = 0;
	
	/* Enable autonomous mode. FIXME: Magic Number (we need an enum of valid "user_cmd"s) */
	/* txdata.user_cmd = 0x02; */
	
	/* Make the master control all PWMs (for now) */
	txdata.pwm_mask.a = 0xFF;
	
	/* Initialize all pins as inputs unless overridden.
	 */
	for (i = 0; i < 16; ++i) {
		pin_set_io( i, kInput);
	}
	
	/* Initialize Serial */
	Open1USART(USART_TX_INT_OFF &
		USART_RX_INT_OFF &
		USART_ASYNCH_MODE &
		USART_EIGHT_BIT &
		USART_CONT_RX &
		USART_BRGH_HIGH,
		kBaud115);   
	Delay1KTCYx( 50 ); /* Settling time (5K Clock ticks) */
	
	/* Init ADC */
	
	/* Setup the number of analog sensors. The PIC defines a series of 15
	 * ADC constants in mcc18/h/adc.h that are all of the form 0b1111xxxx,
	 * where x counts the number of DIGITAL ports. In total, there are
	 * sixteen ports numbered from 0ANA to 15ANA.
	 */
	if ( NUM_ANALOG_VALID(kNumAnalogInputs) && kNumAnalogInputs > 0 ) {
		/* ADC_FOSC: Based on a baud_115 value of 21, given the formula
		 * FOSC/(16(X + 1)) in table 18-1 of the PIC18F8520 doc the FOSC
		 * is 40Mhz.
		 * Also according to the doc, section 19.2, the
		 * ADC Freq needs to be at least 1.6us or 0.625MHz. 40/0.625=64
		 * (Also, see table 19-1 in the chip doc)
		 */
		OpenADC( 
			ADC_FOSC_64 & ADC_RIGHT_JUST &
		       		( 0xF0 | (16 - kNumAnalogInputs) ) ,
			ADC_CH0 & ADC_INT_OFF & ADC_VREFPLUS_VDD &
		       		ADC_VREFMINUS_VSS 
			);
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

Bool new_data_received(void) {
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

void pin_set_io(PinIndex i, PinMode mode) {
	
	/* It will return 1 for all true values */
	uint8_t bit = (mode == kInput);
	
	switch (i) {
	/* For the first 4 pins, the first 4 bits of TRISA control their IO
	 * direction
	 */
	case 0:
	case 1:
	case 2:
	case 3:
		BIT_SET(TRISA, i, bit);
		break;
	
	/* Also in the TRISA register, but the fifth bit (TRISA4) is reserved. */
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

uint16_t analog_get(AnalogInIndex ain) {
	if ( ain > kAnalogSplit ) {
		/* get oi data */
		/* we may not want to trust "ain" */
		return rxdata.oi_analog[ ain - kAnalogSplit ];
	}
	/* kNumAnalogInputs should be checked somewhere else... preferably at
	 * compile time.
	 */
	else if ( ain < kNumAnalogInputs && NUM_ANALOG_VALID(kNumAnalogInputs)  ) {
		/* read ADC */
		SetChanADC(ain);
		Delay10TCYx( 5 ); /* Wait for capacitor to charge */
		ConvertADC();
		while( BusyADC() );
		return ReadADC();
	}
	else {
		return 0xFFFF;
	}
}

void analog_set(AnalogOutIndex aout, int8_t sp) {
	if ( aout < 16 ) {
		uint8_t val = sp + 127;
		
		/* 127 & 128 are treated as the same, apparently. */
		txdata.rc_pwm[aout] = (val > 127) ? val+1 : val;
	}
}

void motor_set(AnalogOutIndex aout, MotorSpeed sp) {
	analog_set(aout,sp);
}

void servo_set(AnalogOutIndex aout, ServoPosition sp) {
	analog_set(aout,sp);
}

/*
 * STREAM IO
 */
void putc(char data) {
	/* From the Microchip C Library Docs */
	while(Busy1USART());
	Write1USART(data);
}
