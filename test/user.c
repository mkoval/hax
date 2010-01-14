#include "hax.h"

uint8_t kNumAnalogInputs = 2;

void init(void) {
	putc('I');
}

void auton_loop(void) {
	putc('A');
	putc('l');
}
	
void auton_spin(void) {
	putc('A');
	putc('s');
}

void telop_loop(void) {
	putc('T');
	putc('l');
}

void telop_spin(void) {
	putc('T');
	putc('s');
}

