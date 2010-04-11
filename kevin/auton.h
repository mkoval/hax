#ifndef AUTON_MIKISH_H_
#define AUTON_MIKISH_H_

#include <hax.h>
#include <stdbool.h>
#include <stdint.h>

#include "ports.h"
#include "util.h"

#define STRAIGHT_ERROR 100
#define TURN_ERROR     ((int16_t)30 * ENC_PER_DEG)
#define TURN_THRESHOLD ((int16_t)5  * ENC_PER_DEG)

/* Generate a transition function with name AUTO_TRANS_LOOKUP(_name_). */
#define AUTO_TRANS_CREATE(_name_, _trans_) \
void _name_##tr_(state_t state) {          \
	_trans_                                \
}

/* Lookup the name of a previously-defined transition function. */
#define AUTO_TRANS_LOOKUP(_name_) _name_##tr_

/* Maximum number of actions that can be present in the autonomous queue. */
#define AUTON_QUEUE_MAX 20

typedef union {
	/* Simulate polymorphism using clever struct alignment. */
	uint16_t timeout;

	/* Control the robot's position and orientation using a timeout and encoder
	 * ticks. Applies to both forward movement and turning, although encoder
	 * tick calculations are performed slightly differently.
	 */
	struct {
		uint16_t timeout;
		uint16_t ticks;
		int32_t  enc_left;
		int32_t  enc_right;
		int8_t   vel;
	} move;

	/* Control the pose of a posable part of the robot (arm or ramp) using a
	 * potentiometer value and a timeout.
	 */
	struct {
		uint16_t timeout;
		uint16_t pos;     /* potentiometer value */
		int8_t   vel;
	} pose;

	/* Do nothing for a specified duration. */
	struct {
		uint16_t timeout;
	} noop;
} data_t;

/* Forward declaration is required to avoid a recursive type reference. */
typedef struct state_s state_t;

/* Generalized callback that does not trigger a state transition. */
typedef void (*callback_t)(data_t *);

/* Callback specifically for transitioning states. */
typedef state_t *(*transition_t)(data_t *);

/* All necessary information to execute and transition from a state. */
struct state_s {
	data_t       data;
	callback_t   cb_init;
	callback_t   cb_loop;
	callback_t   cb_spin;
	transition_t cb_next;
};

/* Initialize the data field for a state_t. */
data_t auto_straight_create(uint16_t, uint16_t, int8_t);
data_t auto_turn_create(uint16_t, uint16_t, int8_t);
data_t auto_ram_create(uint16_t, int8_t);

/* Drive straight for the specified distance.*/
void auto_straight_init(data_t *);
void auto_straight_loop(data_t *);
bool auto_straight_isdone(data_t *);

/* Turn through the specified number of degrees. */
void auto_turn_init(data_t *);
void auto_turn_loop(data_t *);
bool auto_turn_isdone(data_t *);

/* Rotate the arm into the specified position. */
void auto_arm_init(data_t *);
void auto_arm_loop(data_t *);
bool auto_arm_isdone(data_t *);

/* Raise or lower the ramp into the specified position. */
void auto_ramp_init(data_t *);
void auto_ramp_loop(data_t *);
bool auto_ramp_isdone(data_t *);

/* Do nothing until the state times out. */
void auto_wait_init(data_t *);
void auto_wait_loop(data_t *);
bool auto_wait_isdone(data_t *);

#endif
