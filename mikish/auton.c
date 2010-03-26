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
	
	/* Ram the middle wall, the distance to which is specified in extra, while
	 * raising the arm. This pushes the green balls through the slot, knocks
	 * down the low football, and prepares to hit the high footballs.
	 */
	case AUTO_FWDRAM: {
		int32_t dist  = drive_straight(kMotorMax);
		bool    close = !digital_get(DIG_BUT_F);

		/* TODO Raise the arm to knock down the high balls. */

		if (close) {
			advance = true;
		}
		break;
	}

    /* Reverse until the robot hits a wall. Useful for getting in position to
     * dump balls over the wall.
     */
    case AUTO_REVRAM: {
        bool close = !digital_get(DIG_BUT_BL) && !digital_get(DIG_BUT_BR);

        if (!close) {
            drive_straight(kMotorMin);
        } else {
            advance = true;
        }
        break;
    }

	/* Strafe a distance (specified in extra) left or right with the arm in the
	 * raised position. Used to knock down the high footballs.
	 */
	case AUTO_STRAFE: {
		int32_t dist = encoder_get(ENC_S);

		drive_raw(0, SIGN(cur.extra) * kMotorMax, 0);


		if (ABS(dist) >= ABS(cur.extra) * ENC_PER_IN / 10) {
			advance = true;
		}
		break;
	}
	
	/* Drive forward or reverse a distance specified in extra. Positive values
	 * are forward and negative values are reverse.
	 */
	case AUTO_DRIVE: {
		int32_t dist = drive_straight(SIGN(cur.extra) * kMotorMax);

		if (ABS(dist) >= ABS(cur.extra) * ENC_PER_IN / 10) {
			advance = true;
		}
		break;
    }

    case AUTO_TURN: {
        /* TODO Implement this using encoders... */
		advance = true;
		break;
    }

    /* Raise (positive values of cur.extra) or lower (negative values of
     * cur.extra) the ramp at the desired speed.
     */
    case AUTO_RAMP:
        /* Move the ramp until it hits a software stop, measured with a pot. */
        if (ramp_raw(cur.extra)) {
            advance = true;
        }
        break;
    }
		
	/* Advance to the next state if the current state set the correct flag. */
	if (advance) {
		cur = auton_dequeue(queue);

		printf((char *)"[STATE %d, PARAM %d]\r\n", (int)cur.state, (int)cur.extra);

		/* Reset encoder ticks to avoid contaminating the next state. */
		encoder_reset_all();
	}
}

