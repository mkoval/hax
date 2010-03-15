#ifndef PORTS_NITISH_H_
#define PORTS_NITISH_H_

/* Motor output mappings. */
enum {
    MTR_DRIVE_FR = 0,
    MTR_DRIVE_BR = 1,
    MTR_DRIVE_BL = 2,
    MTR_DRIVE_FL = 3,
    MTR_STRAFE_F = 4,
    MTR_ARM_R    = 5,
    MTR_ARM_L    = 6,
    MTR_RAMP     = 7,
    MTR_NUM
};

/* Analog sensor mappings. */
enum {
    ANA_POT_ARM = 1,
    ANA_IR_F    = 2,
    ANA_NUM
};

/* Hardware interrupt mappings. */
enum {
    INT_ENC_L1 = 0,
    INT_ENC_L2 = 1,
    INT_ENC_R1 = 2,
    INT_ENC_R2 = 3,
    INT_ENC_S1 = 4,
    INT_ENC_S2 = 5,
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
