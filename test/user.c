#include <hax.h>

/* Number of analog inputs. Required for compatability with the PIC, which uses
 * this constant to initialize its ADCs (analog to digital convertors).
 */
uint8_t const kNumAnalogInputs = 0;

/* Called as soon as the processor is initialized. Generally used to initialize
 * digital inputs, analog inputs, digital outputs, and interrupts.
 */
void init(void) {
	
}

/* Called every time the user processor receives data from the master processor
 * and is in autonomous mode.
 */
void auton_loop(void) {
	
}

/* Same as auton_loop(), except in user controlled mode. This is controlled by
 * the competition switch (Cortex) or input from the transmitter (PIC).
 */
void telop_loop(void) {
	
}

/* Same as auton_loop() and telop_loop(), except in disabled mode. This is 
 * executed only when the transmitter is off and motor outputs are disabled.
 */
void disable_loop(void) {
	
}

/* These functions share the same behavior as their _loop() counterparts, but
 * execute as quickly as the processor allows. Since inputs and outputs cannot
 * occur this quickly, the _loop() functions should be preferred.
 */
void auton_spin(void) {}
void telop_spin(void) {}
void disable_spin(void) {}
