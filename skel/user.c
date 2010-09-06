#include <hax.h>
#include <stdio.h>
#include "user.h"

void init(void) {
	printf("INIT\n");
}

void auton_loop(void) {
	printf("AUTON\n");
}

void auton_spin(void) {
	/* Not recomended to print constantly */
}

void telop_loop(void) {
	printf("TELOP\n");
}

void telop_spin(void) {
	/* Not recomended to print constantly */
}

void disable_loop(void) {
	printf("DISAB\n");
}

void disable_spin(void) {
	/* Not recomended to print constantly */
}
