#ifndef HAX_H_
#define HAX_H_

#include <stdint.h>
#include <stdbool.h>

typedef uint8_t index_t;               /* internal pin index */
typedef void (*isr_t)(bool pin_level); /* interrupt service routine  */
typedef enum {
	MODE_AUTON,  /* competition autonomous mode; no OI access */
	MODE_TELOP,  /* user-control mode; full OI access */
	MODE_DISABLE /* competition disabled mode; no output control */
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
void digital_init(index_t pin, bool output); /* initialize a digital pin */
void digital_set(index_t pin, bool value);   /* sets a digital output */
bool digital_get(index_t pin);               /* reads a digital input */

/*
 * ANALOG IO
 */
uint16_t analog_get(index_t pin);            /* reads an analog input */
void analog_set(index_t pin, int8_t value);  /* control a motor or server */

/*
 * INTERRUPT SERVICE ROUTINE FUNCTIONS
 */
void interrupt_init(index_t pin, isr_t isr); /* register an interrupt ISR */
void interrupt_set(bool enable);             /* enable or disable an ISR */

#endif
