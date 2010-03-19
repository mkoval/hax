#include <stdio.h>
#include "auton.h"
#include "hax.h"
#include "ir.h"
#include "ports.h"
#include "user.h"
#include "util.h"
#include "stdbool.h"

uint16_t prop_scale(int8_t minOut, int8_t maxOut, uint16_t maxErr, int16_t err) {
	return (int32_t) minOut + ( maxOut - minOut) * MIN(err, maxErr) / maxErr;
}

uint16_t ir_to_cm(uint8_t sen) {
	uint16_t val = analog_adc_get(sen);
	uint16_t cal = ir_long_to_in10(val);

	switch (sen) {
	case SEN_IR_FRONT:
	case SEN_IR_SIDE_F:
	case SEN_IR_SIDE_B:
		return cal;
	
	default:
		return 0;
	}
}

bool turn(void) {
	static uint16_t t = 0u;

	if (t < FU_TURN_TICKS) {
		drive_omni(0, 0, kMotorMax);
		++t;
		return true;
	} else {
		t = 0;
		return false;
	}
}

/* Follow a wall using side-mounted IR sensors. */
#if 0
bool cruise(void) {
	uint16_t sf = ir_to_cm(SEN_IR_SIDE_F);
	uint16_t sb = ir_to_cm(SEN_IR_SIDE_B);
	uint16_t f  = ir_to_cm(SEN_IR_FRONT);

	f = 1000;

	/* Not close enough to a wall to start picking up balls yet. */
	if (f > CRUISE_STOP_CM) {
		int8_t strafe = 0, omega = 0;
		int16_t err_omega = sf - sb;
		/* This is non-optimal and should be replaced with a more accurate
		 * approximation.
		 */
		int16_t err_dist  = (sf + sb) / 2 - FU_CRUISE_DIST;

		/* Correct for the robot's facing angle with respect to the wall. */
		omega  = prop_scale(0, kMotorMax, FU_SEN_IR_OMEGA_ERR, ABS(err_omega));
		omega *= SIGN(err_omega); /* Restore the sign on the motor value. */

		/* Correct the robot's distance from the wall. */
		strafe  = prop_scale(0, kMotorMax, FU_SEN_IR_STRAFE_ERR, ABS(err_dist));
		strafe *= SIGN(err_dist);

		printf((char *)"F:%4u B:%4u D:%4d O:%4d\n", sf, sb, err_omega, omega);

		drive_omni(strafe, kMotorMin, omega);
		return true;
	}
	/* Close enough to start picking up balls... */
	else {
		return false;
	}
}
#endif

void auton_do(void) {
	static GlobalState state = AUTO_IDLE;
	static GlobalState prev  = AUTO_IDLE;

	/* Debug state change message. */
	if (state != prev) {
		printf((char *)"State: %d\n", state);
		prev = state;
	}
}

