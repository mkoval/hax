#ifndef USER_H_
#define USER_H_

#include <stdbool.h>
#include <stdint.h>

/* Constants specific to the PIC microcontroller. */
#if defined(ARCH_PIC)

/* Input mappings for the old radio-transmitter joystick. */
enum {
	OI_STICK_R_X = kAnalogSplit,
	OI_STICK_R_Y,
	OI_STICK_L_Y,
	OI_STICK_L_X,
	OI_BUT_L,
	OI_BUT_B
};

enum {
	MTR_LEFT  = 1,
	MTR_RIGHT = 2
};

/* Interrupts on the PIC are numbered starting at pin 17. */
enum {
	INT_LEFT_A  = 17,
	INT_LEFT_B  = 18,
	INT_RIGHT_A = 19,
	INT_RIGHT_B = 20
};

/* Constants specific to the Cortex microcontroller. */
#elif defined(ARCH_CORTEX)

/* Input mappings for the new VexNET joystick. */
enum {
	OI_STICK_L_X,
	OI_STICK_L_Y,
	OI_STICK_R_X,
	OI_STICK_R_Y,
	OI_BUT_L_U,
	OI_BUT_L_D,
	OI_BUT_L_L,
	OI_BUT_L_R,
	OI_BUT_R_U,
	OI_BUT_R_D,
	OI_BUT_R_L,
	OI_BUT_R_R,
	OI_TRIG_L_U,
	OI_TRIG_L_D,
	OI_TRIG_R_U,
	OI_TRIG_R_D
};

enum {
	/* Ports 1 and 10 are two-wire "high strength" motors. */
	MTR_LEFT  = 2,
	MTR_RIGHT = 3
};

enum {
	/* Most digital inputs on the Cortex double as interrupts. */
	INT_LEFT_A  = 1,
	INT_LEFT_B  = 2,
	INT_RIGHT_A = 3,
	INT_RIGHT_B = 4
};

#else
#error Unsupported architecture
#endif

enum {
	DIG_BUMPER = 1
};

#endif

extern volatile int32_t enc_left;
extern volatile int32_t enc_right;

void enc_left_a(bool);
void enc_left_b(bool);
void enc_right_a(bool);
void enc_right_b(bool);
