#ifndef HAX_H_
#define HAX_H_

#include <stdint.h>
#include <stdbool.h>

#include <arch.h> /* $(ARCH)/arch.h */
#include <prog_conf.h> /* $(PROG)/prog_conf.h */
#include "compilers.h"

#include "arch_check.h"
#include "prog_check.h"

#define ARRAY_SIZE(x) ((size_t)sizeof(x)/sizeof(*(x)))

#define WARN_IX(ix) WARN("idx: %d", ix)

typedef uint8_t index_t;     /* internal pin index */
typedef void (*isr_t)(bool pin_state); /* interrupt callback */
typedef enum {
	MODE_TELOP,  /* user-controlled with full OI access (default) */
	MODE_AUTON,  /* autonomous mode with no OI access */
	MODE_DISABLE /* disabled mode with no OI access or output control */
} ctrl_mode_t;

/*
 * ARCH provided upkeep functions
 */
void arch_init_1(void); /* call prior to user initialization */
void arch_init_2(void); /* call following user initialization */
void arch_loop_1(void); /* call on entry to the slow loop */
void arch_loop_2(void); /* call on exit from the slow loop */
void arch_spin(void); /* call in the fast loop (as quickly as possible) */

bool do_slow_loop(void); /* if true, a slow loop should be executed */
ctrl_mode_t ctrl_mode_get(void); /* see ctrl_mode_t for possible return vals */

/*
 * DIGITAL IO
 */
#define DIGITAL_OUT true
#define DIGITAL_IN false
void digital_setup(index_t pin, bool output); /* initialize prior to use */
void digital_set(index_t pin, bool value);    /* must be set as an output */
bool digital_get(index_t pin);                /* must be set as an input */

/*
 * OI [Operator Interface] Input
 */
bool   oi_button_get(index_t ix);
int8_t oi_group_get(index_t ix);  /* constrained from -127 to 127 */
int8_t oi_rocker_get(index_t ix); /* Rocker/Trigger reading {-1, 0, 1} */

/* MOTORs */
void motor_set(index_t pin, int8_t value);   /* control a motor or server */


/* ANALOG (adc) IN */
uint16_t analog_get(index_t pin); /* returns a 10-bit value read via adc */

/*
 * INTERRUPTS
 */
void interrupt_setup(index_t pin, isr_t isr); /* must also be enabled below */
void interrupt_set(index_t pin, bool enable);

#endif
