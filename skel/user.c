#include <hax.h>
#include <stdio.h>

uint8_t const kNumAnalogInputs = 2;

void init(void) {
	printf("INIT\n");
}

void auton_loop(void) {
	printf("AUTON\n");
}
	
void auton_spin(void) {
}

void telop_loop(void) {
	printf("TELOP\n");
}

void telop_spin(void) {
}

void disable_loop(void) {
	printf("DISAB\n");
}

void disable_spin(void) {
}
