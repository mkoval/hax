/*
 * Hardware Specific Code,
 * PIC Arch
 */
#include <p18cxxx.h>
#include <usart.h>
#include "hax.h"
#include "ifi_lib.h"

/* Slow loop of 18.5 milliseconds (converted to microseconds). */
uint16_t kSlowSpeed = 18500;

/* Checks if the kNumAnalog is valid */
#define NUM_ANALOG_VALID(x) ( (x) < 16 && (x) != 15 )

typedef enum
{
  kBaud19 = 128,
  kBaud38 = 64,
  kBaud56 = 42,
  kBaud115 = 21
} SerialSpeed;

typedef struct {
	uint8_t unknown:6;
	uint8_t autonomous:1; /* Autonomous enable/disable flag. */
	uint8_t disabled:1;   /* Robot enable/disable flag. */
} RCModes;


/* This structure defines the contents of the data received from the master
 * processor.
 */
typedef struct {
	uint8_t packet_num;
	
	union {
		RCModes mode;
		uint8_t allbits;
	} rcmode;
	
	union {
		uint8_t allbits; /* ??? */
	} rcstatusflag;
	
	uint8_t reserved_1[3];
	uint8_t oi_analog[16]; /* Inputs */
	uint8_t reserved_2[9];
	uint8_t master_version;
} RxData;

/* Indicates master control of a pwm when high */
typedef struct {
	uint8_t pwm1:1;
	uint8_t pwm2:1;
	uint8_t pwm3:1;
	uint8_t pwm4:1;
	uint8_t pwm5:1;
	uint8_t pwm6:1;
	uint8_t pwm7:1;
	uint8_t pwm8:1;
} PwmMasterCtrl;

/* This structure defines the contents of the data transmitted to the master  
 * processor.
 */
typedef struct {
	uint8_t reserved_1[4];
	uint8_t rc_pwm[16]; /* Outputs */
	
	/* "user_cmd |= 0x02" gives autonomous mode. */
	uint8_t user_cmd;   /* Reserved for future use. */
	uint8_t cmd_byte1;  /* Reserved for future use. */
	
	union {
		uint8_t a;
		PwmMasterCtrl b;
	} pwm_mask;
	
	uint8_t warning_code;
	uint8_t reserved_2[4];
	uint8_t error_code;
	uint8_t packetnum;
	uint8_t current_mode;
	uint8_t control;
} TxData;

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
	txdata.user_cmd = 0x02;
	
	/* Make the master control all PWMs (for now) */
	txdata.pwm_mask = 0xFF;
	
	/* Initialize all pins as inputs unless overridden.
	 */
	for (i = 0; i <= 15; ++i) {
		pin_set_io( i, kDigitalInput);
	}
	
	/* Initialize Serial */
	OpenUSART(USART_TX_INT_OFF &
		USART_RX_INT_OFF &
		USART_ASYNCH_MODE &
		USART_EIGHT_BIT &
		USART_CONT_RX &
		USART_BRGH_HIGH,
		kBaud115);   
	Delay1KTCYx( 50 ); /* Settling time */
	
	/* Init ADC */
	
	/* Setup the number of analog sensors. The PIC defines a series of 15
	 * ADC constants in mcc18/h/adc.h that are all of the form 0b1111xxxx,
	 * where x counts the number of DIGITAL ports. In total, there are
	 * sixteen ports numbered from 0ANA to 15ANA.
	 */
	if ( NUM_ANALOG_VALID(kNumAnalogInputs) && kNumAnalogInputs > 0 ) {
		/* ADC_FOSC: Based on a baud_115 value of 21, given the formula
		 * FOSC/(16(X + 1)) in table 18-1 of the PIC18F8520 doc, the FOSC
		 * is 40Mhz.
		 * According to the doc, section 19.2, the
		 * ADC Freq needs to be at least 1.6us or 0.625MHz. 40/0.625=64
		 * (Also, see table 19-1 in the chip doc)
		 */
		OpenADC( ADC_FOSC_64 & ADC_RIGHT_JUST & ( xF0 | (15 - kNumAnalogInputs) ) ,
			ADC_CH0 & ADC_INT_OFF & ADC_VREFPLUS_VDD & ADC_VREFMINUS_VSS );
	}
}

void setup_2(void) {
	User_Proc_Is_Ready();
}

void spin(void) {

}

void loop_1(void) {
	GetData(&rxdata);
}

void loop_2(void) {
	PutData(&txdata);
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
		BIT_SET(TRISF, (i - 7) , bit);
		break;
	
	/* The reimaining inputs, 12 through 15, are stored starting at bit 4 in
	 * the TRISH register.
	 */
	case 12:
	case 13:
	case 14:
	case 15:
		BIT_SET(TRISH, (i - 15) + 4, bit);
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

void motor_set(AnalogOutIndex aout, MotorSpeed sp) {
	analog_set(aout,sp);
}

void motor_set(AnalogOutIndex aout, ServoSetpoint sp) {
	analog_set(aout,sp);
}

void analog_set(AnalogOutIndex aout, int8_t sp) {
	if ( aout < 16 ) {
		uint8_t val = sp + 127;
		
		/* 127 & 128 are treated as the same, apparently. */
		txdata.rc_pwm[aout] = (val > 127) ? val+1 : val;
	}
}

/*
 * STREAM IO
 */
void putc(char data) {
	/* From the Microchip C Library Docs */
	while(Busy1USART());
	Write1USART(data);
}
