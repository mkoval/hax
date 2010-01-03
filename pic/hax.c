/*
 * Hardware Specific Code,
 * PIC Arch
 */
#include <p18cxxx.h>
#include <usart.h>
#include "hax.h"

/* Slow loop of 18.5 milliseconds (converted to microseconds). */
uint16_t kSlowSpeed = 18500;

/* Bitmask applied to the input of OpenADC to specify the number of analog
 * ports.
 */
uint8_t picAnalogMask;

/*
 * HARDWARE SPECIFIC DEFINITIONS
 */
#define RND 6
#define RESET_VECTOR 0x800

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

TxData txdata;
RxData rxdata;
Packed statusflag;

/*
 * STARTUP CODE
 * from ifi_startup.c
 */
extern void Clear_Memory(void);
extern void main(void);

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
		Clear_Memory();
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


/**
 ** IFI LIBRARY CODE
 ** methods defined in ifi_library.lib
 **/
/* Vector jumps to the appropriate high priority interrupt handler. Called
 * from the high priority interrupt vector.
 */
void InterruptHandlerHigh(void);


/* Configure registers and initializes the SPI RX/TX buffers. Called from the
 * setup() HAX function during robot initalization.
 */
void Initialize_Registers(void);
void IFI_Initialization(void);

/* Informs the master processor that all user initialization is complete. Must
 * be the last function call in setup().
 */
void User_Proc_Is_Ready(void);

/* Fill the transmit buffer with data in the supplied struct to send to the
 * master processor. Completes in 23 microseconds.
 */
void Putdata(TxData *);

/* Retreive data from the SPI receive buffer (from the master processor),
 * reading the results into the supplied structure. Completes in 14.8
 * microseconds.
 */
void Getdata(RxData *);

/* Sets the output type of PWM's 13, 14, 15, and 16. Each argument is either
 * IFI_PWM for a PWM output or USER_CCP for a timer.
 */
void Setup_PWM_Output_Type(int, int, int, int);

/**
 ** END IFI LIB CODE
 **/



/*
 * INITIALIZATION AND MISC
 */
void setup(void) {
	uint8_t i;
	
	IFI_Initialization();
	
	/* Prevent the "slow loop" from executing until data has been received
	 * from the master processor.
	 */
	statusflag.NEW_SPI_DATA = 0;
	
	/* Enable autonomous mode. FIXME: Magic Number (we need an enum of valid "user_cmd"s) */
	txdata.user_cmd = 0x02;
	
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
		baud_115);   
	Delay1KTCYx( 50 ); /* Settling time */
	
	/* Init ADC */
	
	/* Setup the number of analog sensors. The PIC defines a series of 15
	 * ADC constants in mcc18/h/adc.h that are all of the form 0b1111xxxx,
	 * where x counts the number of DIGITAL ports. In total, there are
	 * sixteen ports numbered from 0ANA to 15ANA.
	 */
	if ( kNumAnalog > 0 ) {
		OpenADC( ADC_FOSC_RC & ADC_RIGHT_JUST & ( xF0 | (15 - kNumAnalogInputs) ) ,
			ADC_CH0 & ADC_INT_OFF & ADC_VREFPLUS_VDD & ADC_VREFMINUS_VSS );
	}
	
	User_Proc_Is_Ready();
}

void spin(void) {

}

void loop(void) {
	PutData(&txdata);
	GetData(&rxdata);
}

Bool new_data_received(void) {
	return statusflag.NEW_SPI_DATA;
}

CtrlMode get_mode(void) {
	return (rxdata.rcmode.mode.autonomous) ? kAuton : kTelop;
}

/*
 * DIGITAL AND ANALOG INPUTS
 */

#define BIT_HI(x, i)     ((x) |=  1 << (i))
#define BIT_LO(x, i)     ((x) &= ~(1 << (i)))
#define BIT_SET(x, i, v) ((v) ? BIT_HI((x), (i)) : BIT_LO((x), (i)))

void pin_set_io(PinIndex i, PinMode mode) {
	/* The comparison operator may not return "1" for all true values. This
	 * is potentialy dangerous as it is being used as a bitmask.
	 */
	uint8_t bit = (mode == kInput);
	
	switch (i) {
	/* The first four inputs are consecutively numbered starting at zero in
	 * the TRISA register.
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

/*
 * STREAM IO
 */
void putc(char data) {
	/* From the Microchip C Library Docs */
	while(Busy1USART());
	Write1USART(data);
}
