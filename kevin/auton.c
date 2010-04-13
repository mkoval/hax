#include <hax.h>
#include <stdio.h>

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

	if (!queue->num) {
		out.state = AUTO_DONE;
		out.extra = 0;
		return out;
	}

	out = queue->actions[0];

	--(queue->num);

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
	
	/* Jerk the arm to deploy the ramp. */
	case AUTO_DEPLOY:
		if (cur.extra) {
			arm_raw(kMotorMax);
			--cur.extra;
		} else {
			advance = true;
		}
		break;

    /* Reverse until the robot hits a wall. Useful for getting in position to
     * dump balls over the wall.
     */
    case AUTO_REVRAM: {
        bool close = !digital_get(BUT_B);

        if (!close) {
            drive_straight(kMotorMin);
        } else {
            advance = true;
        }
        break;
    }

	/* Drive forward or reverse a distance specified in extra. Positive values
	 * are forward and negative values are reverse.
	 */
	case AUTO_DRIVE: {
		int32_t dist = drive_straight(SIGN(cur.extra) * kMotorMax);

		/* Convert between inches and tenth-inches. */
		if (ABS(dist) >= ABS(cur.extra) * ENC_PER_IN / 10) {
			advance = true;
		}
		break;
    }

	/* Turn the specified number of degrees. */
    case AUTO_TURN:
		advance = drive_turn(cur.extra);
		break;

	/* Move the arm until it is at an extreme. */
	case AUTO_ARM: {
		bool done = arm_smart(cur.extra);

		if (done) {
			advance = true;
		}
		break;
	}

    /* Raise (positive values of cur.extra) or lower (negative values of
     * cur.extra) the ramp at the desired speed.
     */
    case AUTO_RAMP:
        /* Move the ramp until it hits a software stop, measured with a pot. */
        if (ramp_smart(cur.extra)) {
            advance = true;
        }
        break;
	
	/* Do nothing for the specified number of slow loops. */
	case AUTO_WAIT:
		if (cur.extra) {
			--cur.extra;
		} else {
			advance = true;
		}
		break;
	
	default:
		advance = true;
    }
		
	/* Advance to the next state if the current state set the correct flag. */
	if (advance) {
		cur = auton_dequeue(queue);

		printf((char *)"[STATE %d, PARAM %d]\r\n", (int)cur.state, (int)cur.extra);

		/* Reset encoder ticks to avoid contaminating the next state. */
		encoder_reset_all();
	}
}
