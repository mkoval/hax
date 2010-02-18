#ifndef PORTS_H_
#define PORTS_H_

/* PWM Motor Outputs */
enum {
	MTR_DRIVE_F = 0,
	MTR_DRIVE_L,
	MTR_DRIVE_B,
	MTR_DRIVE_R,
	MTR_ARM_L,
	MTR_ARM_R,
	MTR_SCISSOR_L,
	MTR_SCISSOR_R
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

/* Analog and Digital Sensors */
enum {
	/* Potentiometers */
	SEN_POT_ARM = 0,
	SEN_POT_SCISSOR_L,
	SEN_POT_SCISSOR_R,
	/* Infrared Rangers */
	SEN_IR_FRONT,
	SEN_IR_SIDE_F,
	SEN_IR_SIDE_B,
	/* Rear bump sensors. */
	SEN_BUMP_L = 10,
	SEN_BUMP_R = 11
};

#define SEN_POT_SCISSOR_L_LOW  164
#define SEN_POT_SCISSOR_L_HIGH 420
#define SEN_POT_SCISSOR_R_LOW  560
#define SEN_POT_SCISSOR_R_HIGH 800
#define SEN_POT_ARM_LOW        971
#define SEN_POT_ARM_HIGH       300

/* Digital Interrupts */
enum {
	/* Encoders on the left, right, and back wheels */ 
	INT_ENC_L1 = 0,
	INT_ENC_L2,
	INT_ENC_R1,
	INT_ENC_R2,
	INT_ENC_B1,
	INT_ENC_B2
};

#endif

