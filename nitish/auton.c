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
	static AutonState  prev = AUTO_START;
	static AutonAction cur  = { AUTO_START, 0 };
	bool advance = false;

	/* Print state change messages for debugging purposes. */
	if (cur.state != prev) {
		/* TODO Print a readable, useful message. */
		prev = cur.state;
	}

	switch (cur.state) {
	/* Dummy state used to initialize autonomous mode. */
	case AUTO_START:
		advance = true;
		break;
	
	/* Ram the middle wall, pushing the green balls through the slot. */
	case AUTO_FWDRAM: {
		int32_t left  = encoder_get(ENC_L);
		int32_t right = encoder_get(ENC_R);
		int32_t dist  = MIN(ABS(left), ABS(right));

		/* Get the arm into position for knocking down the high footballs. */
		arm_set(ANA_POT_ARM_RAM);

		/* Drive straight until the robot hits the balls under the wall. */
		if (dist * ENC_PER_IN < cur.extra) {
			drive_straight(kMotorMax);
		}
		/* Back up a safe distance from the wall, lift the arm in a position
		 * appropriate for knocking down footballs, and do so.
		 */
		else {
			advance = true;
		}
		break;
	}
	}
	
	/* Advance to the next state if the current state set the correct flag. */
	if (advance) {
		cur = auton_dequeue(queue);

		/* Reset encoder ticks to avoid contaminating the next state. */
		encoder_reset_all();
	}
}

