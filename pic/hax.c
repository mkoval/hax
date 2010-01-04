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

/*
 * HARDWARE SPECIFIC DEFINITIONS
 */
#define RND 6
#define RESET_VECTOR 0x800

typedef enum
{
  kBaud19 = 128,
  kBaud38 = 64,
  kBaud56 = 42,
  kBaud115 = 21
} SerialSpeed;

/* This structure contains important system statusflag information. */
typedef struct {
	uint8_t:6;
	uint8_t autonomous:1; /* Autonomous enable/disable flag. */
	uint8_t disabled:1;   /* Robot enable/disable flag. */
} PICModes;

/* This structure allows you to address specific bits of a byte. Changed to
 * an eight-bit integer from IFI's definition avoid reliance upon the MCC18
 * restriction of bitfields to not exceed a byte in length.
 */
typedef struct {
	uint8_t bit0:1;
	uint8_t bit1:1;
	uint8_t bit2:1;
	uint8_t bit3:1;
	uint8_t bit4:1;
	uint8_t bit5:1;
	uint8_t bit6:1;
	uint8_t bit7:1;
} Bits;

/* This structure defines the contents of the data received from the master
 * processor.
 */
typedef struct {
	uint8_t packet_num;
	
	union {
		Bits bitselect;
		PICModes mode;
		uint8_t allbits;
	} rcmode;
	
	union {
		Bits bitselect;
		uint8_t allbits;
	} rcstatusflag;
	
	/* Reserved for future use. */
	uint8_t spare01, spare02, spare03;

	uint8_t oi_analog[16];
	uint8_t reserve[9]; /* Reserved for future use. */
	uint8_t master_version;
} RxData;

/* This structure defines the contents of the data transmitted to the master  
 * microprocessor.
 */
typedef struct {
	/* Reserved for future use. */
	uint8_t spare01, spare02, spare03, spare04;
	
	uint8_t rc_pwm[16];
	
	uint8_t user_cmd;   /* Reserved for future use. */
	uint8_t cmd_byte1;  /* Reserved for future use. */
	uint8_t pwm_mask;
	uint8_t warning_code;
	uint8_t reserve[4]; /* Reserved for future use. */
	uint8_t error_code;
	uint8_t packetnum;
	uint8_t current_mode;
	uint8_t control;
} TxData;

typedef struct {
	uint8_t NEW_SPI_DATA:1;
	uint8_t TX_UPDATED:1;
	uint8_t FIRST_TIME:1;
	uint8_t TX_BUFFSELECT:1;
	uint8_t RX_BUFFSELECT:1;
	uint8_t SPI_SEMAPHORE:1;
	uint8_t:2;
} Packed;

/**
 ** From IFI Lib
 **/
extern TxData txdata;
extern RxData rxdata;
extern Packed statusflag;

/*
 * STARTUP CODE
 * from ifi_startup.c
 */

void _startup(void);
void _do_cinit(void);

#pragma code _entry_scn=RESET_VECTOR
void _entry(void) {
	_asm
	goto _startup
	_endasm
}

#pragma code _startup_scn
void _startup(void) {
	_asm
	/* Initialize the stack pointer. */
	lfsr 1, _stack lfsr 2, _stack clrf TBLPTRU, 0
	/* Initialize rounding flag for floating point libs. */
	bcf FPFLAGS, RND, 0

	/* Initialize the flash memory access configuration. This is harmless for
	 * non-flash devices, so we do it on all parts.
	 */
	bsf 0xa6, 7, 0
	bcf 0xa6, 6, 0
	_endasm

	loop:
		Clear_Memory(); /* Where is this from? */
		/* initialize memory to all zeros (From Kevin Watson's FRC code) *//*
		_asm
		lfsr   0, 0
		movlw  0xF
		clear_loop:
		clrf   POSTINC0, 0
		cpfseq FSR0H, 0
		bra    clear_loop 
		_endasm
		*/
  		_do_cinit ();
		

	main();
	goto loop;
}

/* MPLAB-C18 initialized data memory support, populated by the linker. */
extern far rom struct {
	unsigned short num_init;
	struct _init_entry {
		unsigned long from;
		unsigned long to;
		unsigned long size;
	}
	entries[];
} _cinit;

#pragma code _cinit_scn
void _do_cinit(void) {
	/* We'll make the assumption in the following code that these statics
	 * will be allocated into the same bank.
	 */
	static short long prom;
	static unsigned short curr_byte;
	static unsigned short curr_entry;
	static short long data_ptr;

	/* Initialized data... */
	TBLPTR = (short long) &_cinit;
	
	_asm
	movlb data_ptr
	tblrdpostinc
	movf  TABLAT, 0, 0
	movwf curr_entry, 1
	tblrdpostinc
	movf  TABLAT, 0, 0
	movwf curr_entry+1, 1
	_endasm
	
	test:
		_asm
		bnz 3
		tstfsz curr_entry, 1
		bra 1
		_endasm
	
	goto done;
	
	/* Count down so we only have to look up the data in _cinit once. At
	 * this point we know that TBLPTR points to the top of the current entry
	 * in _cinit, so we can just start reading the from, to, and size
	 * values.
	 */
	_asm
	/* Read the source address. */
	tblrdpostinc
	movf  TABLAT, 0, 0
	movwf prom, 1
	tblrdpostinc
	movf  TABLAT, 0, 0
	movwf prom+1, 1
	tblrdpostinc
	movf  TABLAT, 0, 0
	movwf prom+2, 1
	
	/* Skip a byte since it's stored as a 32bit int. */
	tblrdpostinc
	
	/* Read the destination address directly into FSR0. */
	tblrdpostinc
	movf  TABLAT, 0, 0
	movwf FSR0L, 0
	tblrdpostinc
	movf  TABLAT, 0, 0
	movwf FSR0H, 0
	
	/* Skip two bytes since it's stored as a 32bit int. */
	tblrdpostinc
	tblrdpostinc
	
	/* Read the destination address directly into FSR0. */
	tblrdpostinc
	movf  TABLAT, 0, 0
	movwf curr_byte, 1
	tblrdpostinc
	movf  TABLAT, 0, 0
	movwf curr_byte+1, 1
	
	/* Skip two bytes since it's stored as a 32bit int. */
	tblrdpostinc
	tblrdpostinc
	_endasm

	/* The table pointer now points to the next entry. Save it off since we'll
	 * be using the table pointer to do the copying for the entry.
	 */
	data_ptr = TBLPTR;

	/* now assign the source address to the table pointer */
	TBLPTR = prom;

	/* Do the copy loop. */
	_asm
	/* Determine if we have any more bytes to copy. */
	movlb curr_byte
	movf  curr_byte, 1, 1
	
	copy_loop:
		bnz 2 /* Copy one byte. */
		movf  curr_byte + 1, 1, 1
		bz 7  /* Done copying. */

	copy_one_byte:
		tblrdpostinc
		movf  TABLAT, 0, 0
		movwf POSTINC0, 0

	/* Decrement byte counter. */
	decf  curr_byte, 1, 1
	bc -8 /* Copy loop. */
	decf  curr_byte + 1, 1, 1
	bra -7 /* Copy one byte. */

	done_copying:
	_endasm
	
    /* Restore the table pointer for the next entry. */
	TBLPTR = data_ptr;
	
	/* Next entry... */
	curr_entry--;
	goto test;
	
	done:
		;
}

/*
 * INITIALIZATION AND MISC
 */
 
 #if 0
 static void Setup_Who_Controls_Pwms(int pwmSpec1, int pwmSpec2, int pwmSpec3,
		int pwmSpec4, int pwmSpec5,int pwmSpec6,int pwmSpec7,int pwmSpec8)
{
  txdata.pwm_mask = 0xFF;         /* Default to master controlling all PWMs. */
  if (pwmSpec1 == USER)           /* If User controls PWM1 then clear bit0. */
    txdata.pwm_mask &= 0xFE;      
  if (pwmSpec2 == USER)           /* If User controls PWM2 then clear bit1. */
    txdata.pwm_mask &= 0xFD;
  if (pwmSpec3 == USER)           /* If User controls PWM3 then clear bit2. */
    txdata.pwm_mask &= 0xFB;
  if (pwmSpec4 == USER)           /* If User controls PWM4 then clear bit3. */
    txdata.pwm_mask &= 0xF7;
  if (pwmSpec5 == USER)           /* If User controls PWM5 then clear bit4. */
    txdata.pwm_mask &= 0xEF;
  if (pwmSpec6 == USER)           /* If User controls PWM6 then clear bit5. */
    txdata.pwm_mask &= 0xDF;
  if (pwmSpec7 == USER)           /* If User controls PWM7 then clear bit6. */
    txdata.pwm_mask &= 0xBF;
  if (pwmSpec8 == USER)           /* If User controls PWM8 then clear bit7. */
    txdata.pwm_mask &= 0x7F;
 }
 #endif
 
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
