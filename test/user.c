#include <hax.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include "user.h"

/* Called as soon as the processor is initialized. Generally used to initialize
 * digital inputs, analog inputs, digital outputs, and interrupts.
 */
void init(void) {
	/* Register interrupt callback functions for encoders. */
	interrupt_setup(IX_INTERRUPT(1), enc_left_a);
	interrupt_setup(IX_INTERRUPT(2), enc_left_b);
	interrupt_setup(IX_INTERRUPT(3), enc_right_a);
	interrupt_setup(IX_INTERRUPT(4), enc_right_b);

	/* Enable an interrupt only after its corresponding callback is set. */
	interrupt_set(IX_INTERRUPT(1), 1);
	interrupt_set(IX_INTERRUPT(2), 1);
	interrupt_set(IX_INTERRUPT(3), 1);
	interrupt_set(IX_INTERRUPT(4), 1);
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
		motor_set(IX_MOTOR(2),   127);
		motor_set(IX_MOTOR(3), -127);

		/* Buttons and limit switches are active-low, meaning that they have
		 * a default value of 1 ("true").
		 */
		done = !digital_get(IX_DIGITAL(5));
	}
}

/* Same as auton_loop(), except in user controlled mode. This is controlled by
 * the competition switch (Cortex) or input from the transmitter (PIC).
 */
void telop_loop(void) {
	/* One uses analog_oi_get() and digital_oi_get() to get values from the
	 * transmitter in telop mode.
	 */
	int8_t left  = oi_group_get(IX_OI_GROUP(1, 3));
	int8_t right = oi_group_get(IX_OI_GROUP(1, 2));

	printf("left: %d, right: %d", left, right);

	bool oi_thing = oi_button_get(IX_OI_BUTTON(1, 7, OI_B_UP));

	motor_set(IX_MOTOR(4), 127*oi_thing);

	motor_set(IX_MOTOR(2),   left);
	motor_set(IX_MOTOR(3), -right);
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
	bool const other = digital_get(IX_INTERRUPT(2));

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
	bool const other = digital_get(IX_INTERRUPT(1));

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
	bool const other = digital_get(IX_INTERRUPT(4));

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
	bool const other = digital_get(IX_INTERRUPT(3));

	if (dig && other)
		--right;
	else if (dig && !other)
		++right;
	else if (!dig && other)
		++right;
	else
		--right;
}
