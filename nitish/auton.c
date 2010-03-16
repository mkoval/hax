#include "../hax.h"

#include "encoder.h"
#include "ports.h"
#include "robot.h"
#include "util.h"

#include "auton.h"

void auton_enqueue(AutonQueue *queue, AutonState state, int16_t extra) {
	queue->actions[queue->num].state = state;
	queue->actions[queue->num].extra = extra;
	++queue->num;
}

AutonAction auton_dequeue(AutonQueue *queue) {
	AutonAction out;
	uint8_t i;

	out = queue->actions[0];

	--queue->num;

	/* Shift all of the remaining elements up by one. */
	for (i = 0; i < queue->num; ++i) {
		queue->actions[i] = queue->actions[i + 1];
	}
	return out;
}

void auton_do(AutonQueue *queue) {
	static AutonAction cur = { AUTO_START, 0 };
	bool advance = false;

	switch (cur.state) {
	/* Dummy state used to initialize autonomous mode. */
	case AUTO_START:
		advance = true;
		break;
	
	/* Ram the middle wall, the distance to which is specified in extra, while
	 * raising the arm. This pushes the green balls through the slot, knocks
	 * down the low football, and prepares to hit the high footballs.
	 */
	case AUTO_FWDRAM: {
		int32_t dist  = drive_straight(kMotorMax);
		bool    armup = arm_set(ANA_POT_ARM_RAM);
		bool    close = dist * ENC_PER_10IN >= cur.extra;

		/* Stop driving if we're at the wall and still waiting on the arm. */
		if (close) {
			drive_raw(0, 0, 0);
		}

		/* Keep the arm in position for knocking down the high footballs. */
		arm_set(ANA_POT_ARM_RAM);

		if (armup && close) {
			advance = true;
		}
		break;
	}

	/* Strafe a distance (specified in extra) left or right with the arm in the
	 * raised position. Used to knock down the high footballs.
	 */
	case AUTO_STRAFE: {
		int32_t dist = encoder_get(ENC_S) * ENC_PER_10IN;

		drive_raw(0, SIGN(cur.extra) * kMotorMax, 0);

		if (ABS(dist) >= ABS(cur.extra)) {
			advance = true;
		}
		break;
	}
	
	/* Drive forward or reverse a distance specified in extra. Positive values
	 * are forward and negative values are reverse.
	 */
	case AUTO_DRIVE: {
		int32_t dist = drive_straight(SIGN(cur.extra) * kMotorMax);

		if (ABS(dist) >= ABS(cur.extra)) {
			advance = true;
		}
		break;
	}
	}
	
	/* Advance to the next state if the current state set the correct flag. */
	if (advance) {
		cur = auton_dequeue(queue);

		/* TODO Print out a readable state-change message for debugging. */

		/* Reset encoder ticks to avoid contaminating the next state. */
		encoder_reset_all();
	}
}

