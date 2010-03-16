#ifndef PORTS_NITISH_H_
#define PORTS_NITISH_H_

#include "../hax.h"

/* OI inputs are read as if they are any other analog pin. */
enum {
	OI_R_X = kAnalogSplit,
	OI_R_Y,
	OI_L_Y,
	OI_L_X,
	OI_L_B,
	OI_R_B
};

/* Motor output mappings. */
enum {
    MTR_DRIVE_FR,
    MTR_DRIVE_BR,
    MTR_DRIVE_BL,
    MTR_DRIVE_FL,
    MTR_STRAFE_F,
    MTR_ARM_R,
    MTR_ARM_L,
    MTR_RAMP,
    MTR_NUM
};

/* Analog sensor mappings. */
enum {
    ANA_POT_ARM,
    ANA_IR_F,
    ANA_NUM
};

/* Digital sensor mappings. */
enum {
	DIG_LIM_LIFT_L = ANA_NUM,
	DIG_LIM_LIFT_H,
	SEN_NUM
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

/* Robot-specific sensor calibration fudge constants. LOW indicates that the
 * constants holds the lower of the two analog bounds, not that it is
 * physically lower than the HIGH value.
 */
#define ENC_PER_IN       14
#define ANA_POT_ARM_LOW  0
#define ANA_POT_ARM_HIGH 1024
#define ANA_POT_ARM_RAM  1024

/* Robot dimensions, used to refine drive distances. */
#define ROB_WIDTH_IN  18
#define ROB_LENGTH_IN 24
#define ROB_HEIGHT_IN 18
#define ROB_ARM_IN    20

/* Field dimensions; used to calculate drive distances. */
#define FIELD_WIDTH_IN  (12 * 12)
#define FIELD_LENGTH_IN (6  * 12)


#endif
