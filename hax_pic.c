#include <user_serialdrv.h>
#include "hax.h"

/* Slow loop of 18.5 milliseconds (converted to microseconds). */
uint16_t kSlowSpeed = 18500;

/*
 * HARDWARE SPECIFIC DEFINITIONS
 */
#define RND 6

#define DATA_SIZE        30
#define SPI_TRAILER_SIZE 2
#define SPI_XFER_SIZE    (DATA_SIZE + SPI_TRAILER_SIZE)

#define RESET_VECTOR    0x800
#define HIGH_INT_VECTOR 0x808
#define LOW_INT_VECTOR  0x818

/* This structure contains important system status information. */
typedef struct {
  unsigned int :6;
  unsigned int autonomous:1; /* Autonomous enable/disable flag. */
  unsigned int disabled:1;   /* Robot enable/disable flag. */
} modebits;

/* This structure allows you to address specific bits of a byte.*/
typedef struct {
	unsigned int bit0:1;
	unsigned int bit1:1;
	unsigned int bit2:1;
	unsigned int bit3:1;
	unsigned int bit4:1;
	unsigned int bit5:1;
	unsigned int bit6:1;
	unsigned int bit7:1;
} bitid;

/* This structure defines the contents of the data received from the Master
 * microprocessor. */
typedef struct {
	unsigned char packet_num;
	
	union {
		bitid bitselect;
		modebits mode;
		unsigned char allbits;
	} rc_mode_byte;
	
	union {
		bitid bitselect;
		unsigned char allbits;
	} rc_receiver_status_byte;
	
	unsigned char spare01,spare02,spare03; /* Reserved for future use. */

	unsigned char oi_analog01, oi_analog02, oi_analog03, oi_analog04;
	unsigned char oi_analog05, oi_analog06, oi_analog07, oi_analog08;
	unsigned char oi_analog09, oi_analog10, oi_analog11, oi_analog12;
	unsigned char oi_analog13, oi_analog14, oi_analog15, oi_analog16;
	unsigned char reserve[9]; /* Reserved for future use. */
	unsigned char master_version;
} RxData;

/* This structure defines the contents of the data transmitted to the master  
 * microprocessor.
 */
typedef struct {
	/* Reserved for future use. */
	unsigned char spare01,spare02,spare03,spare04;
	
	unsigned char rc_pwm01, rc_pwm02, rc_pwm03, rc_pwm04;
	unsigned char rc_pwm05, rc_pwm06, rc_pwm07, rc_pwm08;
	unsigned char rc_pwm09, rc_pwm10, rc_pwm11, rc_pwm12;
	unsigned char rc_pwm13, rc_pwm14, rc_pwm15, rc_pwm16;
	
	unsigned char user_cmd;   /* Reserved for future use. */
	unsigned char cmd_byte1;  /* Reserved for future use. */
	unsigned char pwm_mask;
	unsigned char warning_code;
	unsigned char reserve[4]; /* Reserved for future use. */
	unsigned char error_code;
	unsigned char packetnum;
	unsigned char current_mode;
	unsigned char control;
} TxData;

typedef struct {
	unsigned int NEW_SPI_DATA:1;
	unsigned int TX_UPDATED:1;
	unsigned int FIRST_TIME:1;
	unsigned int TX_BUFFSELECT:1;
	unsigned int RX_BUFFSELECT:1;
	unsigned int SPI_SEMAPHORE:1;
	unsigned int:2;
} Packed;

extern volatile near unsigned long short TBLPTR;
extern near unsigned FSR0;
extern near char FPFLAGS;

extern TxData txdata;
extern RxData rxdata;
extern Packed status;

/*
 * STARTUP CODE - DO NOT MODIFY
 * from ifi_startup.c
 */
extern void Clear_Memory (void);
extern void main (void);

void _entry(void);
void _startup(void);
void _do_cinit(void);

#pragma code _entry_scn=RESET_VECTOR
void _entry(void) {
	_asm goto _startup _endasm
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
	if (autonomous_mode) {
		auton_spin();
	} else {
		telop_spin();
	}
}

void loop(void) {
	if (autonomous_mode) {
		auton_loop();
	} else {
		telop_loop();
	}
}

Bool new_data_received(void) {
	return statusflag.NEW_SPI_DATA;
}

CtrlMode get_mode(void) {
	return (autonomous_mode) ? kAuton : kTelop;
}

/*
 * STREAM IO
 */
void putb(uint8_t data) {
	TXREG = data;
	Wait4TXEmpty();
}

void putc(char data) {
	/* From the IFI Default Code printf_lib.c. */
	TXREG = data;
	Wait4TXEmpty();
}

void puth(uint16_t data) {
	uint8_t i, digit;
	
	putc('0');
	putc('x');
	
	for (i = 0; i < 4; ++i) {
		/* Each four-bit chunk of a binary number is a hex digit. */
		digit  = (uint8_t) ((data & 0xF000) >> 12);
		data <<= 4;
		
		if (digit < 10) {
			putc('0' + digit);
		} else {
			putc('A' + digit - 10);
		}
	}
}

void puti(uint16_t data) {
	char buf[6];
	uint8_t i;
	
	/* Generate the output in reverse order (i.e. the left-most digit is the
	 * most significant digit).
	 */
	for (i = 0; data > 0; ++i) {
		buf[i] = '0' + (data % 10);
		data  /= 10;
	}
	
	/* Reverse the string prior to sending it to the serial port. */
	for (++i; i > 0; --i) {
		putc(buf[i - 1]);
	}
}

void putf(float data) {
	/* TODO: Essentially implement ftoa() and send the results to putc(). */
}

void puts(char *data) {
	for (; *data; ++data) {
		putc(*data);
	}
}