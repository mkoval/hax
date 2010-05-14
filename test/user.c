#include <hax.h>
#include <stdbool.h>
#include <stdint.h>

#include "user.h"

/* Number of analog inputs. Required for compatability with the PIC, which uses
 * this constant to initialize its ADCs (analog to digital convertors).
 */
uint8_t const kNumAnalogInputs = 0;

/* Called as soon as the processor is initialized. Generally used to initialize
 * digital inputs, analog inputs, digital outputs, and interrupts.
 */
void init(void) {
	/* Register interrupt callback functions for encoders. */
	interrupt_reg_isr(INT_LEFT_A, enc_left_a);
	interrupt_reg_isr(INT_LEFT_B, enc_left_b);
	interrupt_reg_isr(INT_RIGHT_A, enc_right_a);
	interrupt_reg_isr(INT_RIGHT_B, enc_right_b);

	/* Enable an interrupt only after its corresponding callback is set. */
	interrupt_enable(INT_LEFT_A);
	interrupt_enable(INT_LEFT_B);
	interrupt_enable(INT_RIGHT_A);
	interrupt_enable(INT_RIGHT_B);
}

/* Called every time the user processor receives data from the master processor
 * and is in autonomous mode.
 */
void auton_loop(void) {
	static bool done = false;

	if (!done) {
		/* Motor speeds range from -127 (full power, reverse) to +227 (full
		 * power, forward).
		 */
		analog_set(MTR_LEFT,   127);
		analog_set(MTR_RIGHT, -127);

		/* Buttons and limit switches are active-low, meaning that they have
		 * a default value of 1 ("true").
		 */
		done = !digital_get(1);
	}
}

/* Same as auton_loop(), except in user controlled mode. This is controlled by
 * the competition switch (Cortex) or input from the transmitter (PIC).
 */
void telop_loop(void) {
	/* One uses analog_oi_get() and digital_oi_get() to get values from the
	 * transmitter in telop mode.
	 */
	int8_t left  = analog_oi_get(OI_STICK_L_Y);
	int8_t right = analog_oi_get(OI_STICK_R_Y);

	analog_set(MTR_LEFT,   left);
	analog_set(MTR_RIGHT, -right);
}

/* Same as auton_loop() and telop_loop(), except in disabled mode. This is 
 * executed only when the transmitter is off and motor outputs are disabled.
 */
void disable_loop(void) {}

/* These functions share the same behavior as their _loop() counterparts, but
 * execute as quickly as the processor allows. Since inputs and outputs cannot
 * occur this quickly, the _loop() functions should be preferred.
 */
void auton_spin(void) {}
void telop_spin(void) {}
void disable_spin(void) {}

/* Count encoder ticks using interrupts. Note that any variables being modified
 * within an interrupt service routine must be flagged as volatile.
 */
volatile int32_t left  = 0;
volatile int32_t right = 0;

void enc_left_a(bool dig) {
	bool const other = digital_get(INT_LEFT_B);

	if (dig && other)
		--left;
	else if (dig && !other)
		++left;
	else if (!dig && other)
		++left;
	else
		--left;
}

void enc_left_b(bool dig) {
	bool const other = digital_get(INT_LEFT_A);

	if (dig && other)
		--left;
	else if (dig && !other)
		++left;
	else if (!dig && other)
		++left;
	else
		--left;
}

void enc_right_a(bool dig) {
	bool const other = digital_get(INT_RIGHT_B);

	if (dig && other)
		--right;
	else if (dig && !other)
		++right;
	else if (!dig && other)
		++right;
	else
		--right;
}

void enc_right_b(bool dig) {
	bool const other = digital_get(INT_RIGHT_A);

	if (dig && other)
		--right;
	else if (dig && !other)
		++right;
	else if (!dig && other)
		++right;
	else
		--right;
}
