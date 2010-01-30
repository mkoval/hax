#include "hax.h"

uint8_t kNumAnalogInputs = 16;

void init(void) {
	puts("Initialization\n");
}

void auton_loop(void) {
}
	
void auton_spin(void) {
}

void telop_loop(void) {
	int i;

	puts("Digitals : ");
	for (i = 0; i <= 16; ++i) {
		_puth(i);
		putc(':');
		_puth(digital_get(i));
		putc(':');
		_puth2(analog_get(i));
		puts(" ; ");
	}

	/* Test motor output. */
	for (i = 0; i < 7; ++i) {
		motor_set(i, kMotorMax);
	}
}

void telop_spin(void) {
}

