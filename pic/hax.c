#include <usart.h>
#include "hax.h"

/* Slow loop of 18.5 milliseconds (converted to microseconds). */
uint16_t kSlowSpeed = 18500;

/*
 * HARDWARE SPECIFIC DEFINITIONS
 */
#define RND 6
#define RESET_VECTOR 0x800

/* Registers required for the IFI code to function. */
extern volatile near uint8_t TXSTA1;
extern volatile near struct {
	uint8_t TX9D:1;
	uint8_t TRMT:1;
	uint8_t BRGH:1;
	uint8_t:1;
	uint8_t SYNC:1;
	uint8_t TXEN:1;
	uint8_t TX9:1;
	uint8_t CSRC:1;
} TXSTA1bits;

extern volatile near unsigned char       PIR1;
extern volatile near union {
	struct {
		unsigned TMR1IF:1;
		unsigned TMR2IF:1;
		unsigned CCP1IF:1;
		unsigned SSPIF:1;     /* Reserved - Do not use */
		unsigned TX1IF:1;
		unsigned RC1IF:1;
		unsigned ADIF:1;
		unsigned PSPIF:1;     /* Reserved - Do not use */
	};
	struct {
		unsigned :4;
		unsigned TXIF:1;
		unsigned RCIF:1;
	};
} PIR1bits;


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

	uint8_t oi_analog01, oi_analog02, oi_analog03, oi_analog04;
	uint8_t oi_analog05, oi_analog06, oi_analog07, oi_analog08;
	uint8_t oi_analog09, oi_analog10, oi_analog11, oi_analog12;
	uint8_t oi_analog13, oi_analog14, oi_analog15, oi_analog16;
	uint8_t reserve[9]; /* Reserved for future use. */
	uint8_t master_version;
} RxData;

/* This structure defines the contents of the data transmitted to the master  
 * microprocessor.
 */
typedef struct {
	/* Reserved for future use. */
	uint8_t spare01, spare02, spare03, spare04;
	
	uint8_t rc_pwm01, rc_pwm02, rc_pwm03, rc_pwm04;
	uint8_t rc_pwm05, rc_pwm06, rc_pwm07, rc_pwm08;
	uint8_t rc_pwm09, rc_pwm10, rc_pwm11, rc_pwm12;
	uint8_t rc_pwm13, rc_pwm14, rc_pwm15, rc_pwm16;
	
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

extern volatile near unsigned long short TBLPTR;
extern near unsigned FSR0;
extern near char FPFLAGS;

TxData txdata;
RxData rxdata;
Packed statusflag;

/*
 * STARTUP CODE - DO NOT MODIFY
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


/*
 * IFI MASTER PROCESSOR CODE - DO NOT MODIFY
 * from ifi_startup.c; methods defined in ifi_library.lib
 */
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

void Wait4TXEmpty() {
	while (!PIR1bits.TXIF);
}

/*
 * INITIALIZATION AND MISC
 */
void setup(void) {
	IFI_Initialization();
	
	/* Prevent the "slow loop" from executing until data has been received
	 * from the master processor.
	 */
	statusflag.NEW_SPI_DATA = 0;
	
	/* Enable autonomous mode. */
	txdata.user_cmd = 0x02;
}

void spin(void) {
	if (rxdata.rcmode.mode.autonomous) {
		auton_spin();
	} else {
		telop_spin();
	}
}

void loop(void) {
	if (rxdata.rcmode.mode.autonomous) {
		auton_loop();
	} else {
		telop_loop();
	}
}

Bool new_data_received(void) {
	return statusflag.NEW_SPI_DATA;
}

CtrlMode get_mode(void) {
	return (rxdata.rcmode.mode.autonomous) ? kAuton : kTelop;
}

/*
 * STREAM IO
 */
void putb(uint8_t data) {
	while(Busy1USART());
	Write1USART(data);
}

void putc(char data) {
	/* From the Microchip C Library Docs */
	while(Busy1USART());
	Write1USART(data);
}
