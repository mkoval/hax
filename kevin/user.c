#include <hax.h>
#include <stdbool.h>
#include <stdio.h>

#include "auton.h"
#include "encoder.h"
#include "ports.h"
#include "robot.h"
#include "state.h"

#include "user.h"

/* Physical and electronic robot configuration is specified in ports.h. */
uint8_t kNumAnalogInputs = ANA_NUM;

/* Jumper-enabled calibration modes. */
static CalibrationMode cal_mode = CAL_MODE_NONE;

/* User-override for arm and ramp potentiometers. */
static bool override = true;

/* Current state of autonomous mode. Meaningless if in telop mode. */
state_t const __rom *auto_states[STATE_NUM];
state_t const __rom *auto_current;

void init(void) {
	uint8_t i;

	/* Enable the calibration mode specified by the jumpers. */
	cal_mode = (!digital_get(JUMP_CAL_MODE1)     )
	         | (!digital_get(JUMP_CAL_MODE2) << 1)
	         | (!digital_get(JUMP_CAL_MODE3) << 2);

	printf((char *)"[CALIB %d]\r\n", cal_mode);

	/* Initialize autonomous mode. */
	for (i = 0; i < STATE_NUM; ++i) {
		auto_states[i] = auto_cbs[i](NULL);
	}
	auto_current = auto_states[0];

	/* Initialize the encoder API; from now on we can use the logical mappings
	 * ENC_L, ENC_R, and ENC_S without worrying about the wiring of the robot.
	 */
	encoder_init(ENC_L, INT_ENC_L1, INT_ENC_L2); /* Left  */
	encoder_init(ENC_R, INT_ENC_R1, INT_ENC_R2); /* Right */
}

void disable_loop(void) {
}

void disable_spin(void) { 
}

void auton_loop(void) {
	state_t const __rom *next = auto_current;
	uint8_t i;

	/* Start each autonomous slow loop with a clean slate. */
	for (i = 0; i < MTR_NUM; ++i) {
		motor_set(i, 0);
	}

	/* Update the current state. */
	auto_current->cb_loop(auto_current->data);

	/* We just changed states and need to call the initialization routine for
	 * the new state. Additionally, printf() an alert.
	 */
	next = auto_current->cb_next(auto_current);

	/* Update the current state using the loop() callback. */
	if (auto_current != next) {
		printf((char *)"TRANSITION\n\r");
		next->cb_init(auto_current->data);
	}
	auto_current = next;

	/* Count down to a potential timeout. This property is shared amongst all
	 * states.
	 */
	--auto_current->data->timeout;
}

void auton_spin(void) {
}

void telop_loop(void) {
	uint16_t left    = analog_oi_get(OI_L_Y);
	uint16_t right   = analog_oi_get(OI_R_Y);
	uint16_t arm     = analog_oi_get(OI_L_B);
	uint16_t ramp    = analog_oi_get(OI_R_B);
	bool     done    = false;

	switch (cal_mode) {
	/* Calibrate the ENC_PER_IN constant. */
	case CAL_MODE_DRIVE: {
		int32_t dist = drive_straight(kMotorMax);
		done = dist > CAL_ENC_DRIVE;
		break;
	}

	/* Calibrate the ENC_PER_DEG constant. */
	case CAL_MODE_TURN: {
		done = drive_turn(90);
		break;
	}
	
	/* Calibrate the POT_ARM_LOW and POT_ARM_HIGH constants. */
	case CAL_MODE_PRINT:
#if defined(ROBOT_KEVIN)
		printf((char *)"ARM %4d  LIFT %4d  ENCL %5d  ENCR %5d\n\r",
		       (int)analog_adc_get(POT_ARM),
		       (int)analog_adc_get(POT_LIFT),
		       (int)encoder_get(ENC_L),
		       (int)encoder_get(ENC_R));
#elif defined(ROBOT_NITISH)
		printf((char *)"ARM %4d  LIFTL %4d  LIFTR %4d  ENCL %5d  ENCR %5d\n\r",
		       (int)analog_adc_get(POT_ARM),
		       (int)analog_adc_get(POT_LIFT_L),
		       (int)analog_adc_get(POT_LIFT_R),
		       (int)encoder_get(ENC_L),
		       (int)encoder_get(ENC_R));
#endif
		/* Fall through to allow normal telop control. */
	
	/* Normal user-controlled telop mode. */
	default:
		/* Disable ramp and arm potientiometer checks. */
		if (override) {
			drive_raw(left, right);
			arm_raw(arm);
			ramp_raw(ramp, ramp);
		}
		/* Prevent dangerous ramp and arm movement in software. */
		else {
			drive_raw(left, right);
			arm_smart(arm);
			ramp_smart(ramp);
		}
	}

#if defined(ROBOT_KEVIN)
	printf((char *)"ARM %4d  LIFT %4d  ENCL %5d  ENCR %5d\n\r",
	       (int)analog_adc_get(POT_ARM),
	       (int)analog_adc_get(POT_LIFT),
	       (int)encoder_get(ENC_L),
	       (int)encoder_get(ENC_R));
#elif defined(ROBOT_NITISH)
	printf((char *)"ARM %4d  LIFTL %4d  LIFTR %4d  ENCL %5d  ENCR %5d\n\r",
	       (int)analog_adc_get(POT_ARM),
	       (int)analog_adc_get(POT_LIFT_L),
	       (int)analog_adc_get(POT_LIFT_R),
	       (int)encoder_get(ENC_L),
	       (int)encoder_get(ENC_R));
#endif


	/* End calibration mode when the routine is complete. */
	cal_mode *= !done;
}

void telop_spin(void) {
}
