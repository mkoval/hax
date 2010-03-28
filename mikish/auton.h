#ifndef AUTON_MIKISH_H_
#define AUTON_MIKISH_H_

#include <stdbool.h>
#include <stdint.h>

/* Maximum number of actions that can be present in the autonomous queue. */
#define AUTON_QUEUE_MAX 20

typedef enum {
	AUTO_START,
	AUTO_FWDRAM,  /* extra = distance to move forward (in tenth-inches) */
	AUTO_STRAFE,  /* extra = signed distance to strafe (in tenth-inches) */
	AUTO_DRIVE,   /* extra = distance (in tenth-inches), negative is reverse */
	AUTO_RAMP,    /* extra = motor speed */
	AUTO_TURN,    /* extra = number of degrees to turn */
	AUTO_REVRAM,  /* extra = none */
	AUTO_DONE
} AutonState;

/* FIFO data structure used to generalize the FSA autonomous code. */
typedef struct {
	AutonState state;
	int16_t    extra;
} AutonAction;

typedef struct {
	uint8_t     num;
	AutonAction actions[AUTON_QUEUE_MAX];
} AutonQueue;

/* Meaningless keyword to make a long series of auton_enqueue() calls look
 * pretty.
 */
#define NONE 0

/* Add a state to the end of the autonomous queue. */
void auton_enqueue(AutonQueue *, AutonState, int16_t);

/* Remove and return the first element from the autonomous queue. */
AutonAction auton_dequeue(AutonQueue *);

/* Perform all of the actions in the autonomous queue until the AUTON_DONE
 * state is reached, the queue is empty, or autonomous mode ends.
 */
void auton_do(AutonQueue *);

#endif
