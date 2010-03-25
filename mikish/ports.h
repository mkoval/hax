#ifndef PORTS_NITISH_H_
#define PORTS_NITISH_H_

#include "../hax.h"

/* PWM Motor Outputs */
enum {
	MTR_DRIVE_F = 0,
	MTR_DRIVE_L,
	MTR_DRIVE_B,
	MTR_DRIVE_R,
	MTR_ARM_L,
	MTR_ARM_R,
	MTR_SCISSOR_L,
	MTR_SCISSOR_R,
    MTR_NUM
};

/* Operator Interface Analog Inputs */
enum {
	OI_R_X = kAnalogSplit,
	OI_R_Y,
	OI_L_Y,
	OI_L_X,
	OI_L_B,
	OI_R_B
};

/* Analog Sensors */
enum {
    ANA_POT_ARM = 0,
    SEN_POT_SCISSOR_L,
    SEN_POT_SCISSOR_R,
    ANA_NUM
};

/* Digital Sensors */
enum {
	/* Rear bump sensors. */
	DIG_BUT_BL = ANA_NUM,
	DIG_BUT_BR,
	DIG_BUT_F
};

/* Hardware interrupt mappings. */
enum {
    INT_ENC_L1,
    INT_ENC_L2,
    INT_ENC_R1,
    INT_ENC_R2,
    INT_ENC_S1,
    INT_ENC_S2,
    INT_NUM
};

/* Arbitrary constants used to interface with the encoder API. */
enum {
	ENC_L,
	ENC_R,
	ENC_S,
	ENC_NUM
};
/* Scissor lift potentiometers. */
#define SEN_POT_SCISSOR_L_LOW  164
#define SEN_POT_SCISSOR_L_HIGH 420
#define SEN_POT_SCISSOR_R_LOW  560
#define SEN_POT_SCISSOR_R_HIGH 800

/* Garbage-truck like arm potentiometers. */
#define ANA_POT_ARM_LOW        971
#define ANA_POT_ARM_HIGH       300
#define ANA_POT_ARM_RAM        300

#define ENC_PER_IN  25

/* Robot dimensions, used to refine drive distances. */
#define ROB_WIDTH_IN           24
#define ROB_LENGTH_IN          18
#define ROB_HEIGHT_IN          18
#define ROB_ARM_IN             20

/* Field dimensions; used to calculate drive distances. */
#define FIELD_WIDTH_IN         (12 * 12)
#define FIELD_LENGTH_IN        (6  * 12)

#endif
