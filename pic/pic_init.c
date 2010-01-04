/*
 * STARTUP CODE
 * from ifi_startup.c
 */

#include <p18cxxx.h>
#include "ifi_lib.h"
#include "hax.h"
 
#define RND 6 /* Rounding Flag for math libs */ 
#define RESET_VECTOR 0x800

 
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
