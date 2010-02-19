#include <stdio.h>
#include "auton.h"
#include "hax.h"
#include "ports.h"
#include "user.h"
#include "util.h"

uint16_t prop_scale(int8_t minOut, int8_t maxOut, uint16_t maxErr, int16_t err) {
	return minOut + (maxOut - minOut) * err / maxErr;
}

uint16_t ir_to_cm(uint8_t sen) {
	uint16_t val = analog_adc_get(sen);

	switch (sen) {
	case SEN_IR_FRONT:
	case SEN_IR_SIDE_F:
	case SEN_IR_SIDE_B:
	default:
		return val;
	}
}

GlobalState turn(void) {
	static uint16_t t = 0u;

	if (t < FU_TURN_TICKS) {
		drive_omni(0, 0, kMotorMax);
		++t;
		return AUTO_TURN;
	} else {
		t = 0;
		return AUTO_IDLE;
	}
}

GlobalState cruise(void) {
	uint16_t sf = ir_to_cm(SEN_IR_SIDE_F);
	uint16_t sb = ir_to_cm(SEN_IR_SIDE_B);
	uint16_t f  = ir_to_cm(SEN_IR_FRONT);

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

		drive_omni(strafe, -kMotorMax, omega);
		return AUTO_CRUISE;
	}
	/* Close enough to start picking up balls... */
	else {
		return AUTO_PICKUP;
	}
}

GlobalState deposit(void) {
	static DepositState state = DEPOSIT_REVERSE;

	switch (state) {
	/* Reverse until both limit switches are depressed. */
	case DEPOSIT_REVERSE:
		drive_omni(0, kMotorMax, 0);

		if (!digital_get(SEN_BUMP_L) && !digital_get(SEN_BUMP_R)) {
			state = DEPOSIT_RAISE;
		}
		return AUTO_DEPOSIT;
	
	/* Raise the basket to its maximum height, dumping it in the process. */
	case DEPOSIT_RAISE:
		if (!lift_basket(+127)) {
			state = DEPOSIT_LOWER;
		}
		return AUTO_DEPOSIT;
	
	/* Lower the basket to its resting height. */
	case DEPOSIT_LOWER:
		if (!lift_basket(-127)) {
			state = DEPOSIT_REVERSE;
			return AUTO_IDLE;
		} else {
			return AUTO_DEPOSIT;
		}
	
	default:
		return AUTO_IDLE;
	}
}

GlobalState pickup(void) {
	static PickupState state = PICKUP_RAISE;
	int16_t pos = analog_adc_get(SEN_POT_ARM);

	switch (state) {
	case PICKUP_RAISE:
		/* Raising the arm. */
		if (!lift_arm(+127)) {
			state = PICKUP_LOWER;
		}
		return AUTO_PICKUP;

	case PICKUP_LOWER:
		/* Lowering the arm. */
		if (lift_arm(-127)) {
			return AUTO_PICKUP;
		}
		/* Just reached the bottom; this action is complete. */
		else {
			state = PICKUP_RAISE;
			return AUTO_IDLE;
		}
	
	/* Safe default... */
	default:
		return AUTO_IDLE;
	}
}

void auton_do(void) {
	static GlobalState state = AUTO_TURN;

	switch (state) {
	/* Move balls from the arm into the basket. */
	case AUTO_PICKUP:
		_puts("[STATE pickup]\n");
		state = pickup();
		break;
	
	/* Back into the nearest wall and dump the basket. */
	case AUTO_DEPOSIT:
		_puts("[STATE DEPOSIT]\n");
		state = deposit();
		break;
	
	/* Follow the wall at a fixed distance. */
	case AUTO_CRUISE:
		_puts("[STATE CRUISE]\n");
		state = cruise();
		break;
	
	/* Turn 90-degrees to the left. */
	case AUTO_TURN:
		_puts("[STATE TURN]\n");
		state = turn();
		break;

	/* Do nothing... */
	case AUTO_IDLE:
	default:
		_puts("[STATE idle]\n");
		break;
	}
}

