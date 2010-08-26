#ifndef HAX_H_
#define HAX_H_

#include <stdint.h>
#include <stdbool.h>
#include <arch.h>

typedef uint8_t index_t;     /* internal pin index */
typedef void (*isr_t)(void); /* interrupt callback */
typedef enum {
    MODE_TELOP,  /* user-controlled with full OI access (default) */
    MODE_AUTON,  /* autonomous mode with no OI access */
    MODE_DISABLE /* disabled mode with no OI access or output control */
} mode_t;

/*
 * USER CODE
 */
void init(void);       /* called exactly once on initialization */
void loop(void);       /* called once per slow-loop */
void spin(void);       /* called as quickly as possible */
mode_t mode_get(void); /* see mode_t for possible return values */

/*
 * DIGITAL IO
 */
void digital_init(index_t pin, bool output);  /* initialize prior to use */
void digital_set(index_t pin, bool value);    /* must be set as an output */
bool digital_get(index_t pin);                /* must be set as an input */

/*
 * ANALOG IO
 */
void analog_set(index_t pin, int8_t value);   /* control a motor or server */
uint16_t analog_get(index_t pin);             /* returns a 10-bit value */

/*
 * INTERRUPTS
 */
void interrupt_init(index_t pin, isr_t isr);  /* must also be enabled below */
void interrupt_set(index_t pin, bool enable);

#endif
