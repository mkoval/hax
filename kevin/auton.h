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

/* Constructors used to wrap the initialization of a data_t. */
#define AUTO_STRAIGHT(_timeout_, _dist_, _vel_) { (_timeout_), (_dist_)* ENC_PER_IN,  (_vel_), 0, 0 }
#define AUTO_RAM(_timeout_, _vel_)              { (_timeout_), 0,                     (_vel_), 0, 0 }
#define AUTO_TURN(_timeout_, _ang_, _vel_)      { (_timeout_), (_ang_) * ENC_PER_DEG, (_vel_), 0, 0 }
#define AUTO_ARM(_timeout_, _pos_, _vel_)       { (_timeout_), (_pos_),               (_vel_), 0, 0 }
#define AUTO_RAMP(_timeout_, _pos_, _vel_)      { (_timeout_), (_pos_),               (_vel_), 0, 0 }
#define AUTO_WAIT(_timeout_)                    { (_timeout_), 0,                     0,       0, 0 }

/* Define a transition function with name _name_ that transitions into the
 * _ntime_ state if auto_istimeout() is true, _ncond_ if _cond_ evaluates to
 * true, otherwise the current state.
 */
#define STATE_START() enum { _st_start = __LINE__ };
#define STATE_DONE()  enum { _st_end   = __LINE__ };
#define STATE_NUM     (_st_end - _st_start - 1)

#define STATE(_name_, _data_, _cbinit_, _cbloop_, _stsuc_, _stfail_, _cond_)  \
enum { _st_##_name_##_line = __LINE__ };                                      \
state_t const __rom * _st_##_name_##_transition(state_t const __rom *state) { \
	static data_t              _st_##_name_##_data  = _data_;                 \
	static state_t const __rom _st_##_name_##_state = {                       \
		&_st_##_name_##_data, _cbinit_, _cbloop_, _st_##_name_##_transition   \
	};                                                                        \
	if (!state)                                                               \
		return &_st_##_name_##_state; /* For initialization code. */          \
	else if (!state->data->timeout)                                           \
		return _stfail_;              /* Timeout condition. */                \
	else if (_cond_)                                                          \
		return _stsuc_;               /* Success condition. */                \
	else                                                                      \
		return cstate;                /* No transition. */                    \
}

typedef union {
	/* Hack to enable shorthand union initalization. */
	struct {
		uint16_t unnamed1;
		uint16_t unnamed2;
		int8_t   unnamed3;
		int32_t  unnamed4;
		int32_t  unnamed5;
	} unused;

	/* Simulate polymorphism using clever struct alignment. */
	uint16_t timeout;

	/* Control the robot's position and orientation using a timeout and encoder
	 * ticks. Applies to both forward movement and turning, although encoder
	 * tick calculations are performed slightly differently.
	 */
	struct {
		uint16_t timeout;
		uint16_t ticks;
		int8_t   vel;
		int32_t  enc_left;
		int32_t  enc_right;
	} move;

	/* Control the pose of a posable part of the robot (arm or ramp) using a
	 * potentiometer value and a timeout.
	 */
	struct {
		uint16_t timeout;
		uint16_t pos;     /* potentiometer value */
		int8_t   vel;
	} pose;
} data_t;

/* Forward declaration is required to avoid a recursive type reference. */
typedef struct state_s state_t;

/* Generalized callback that does not trigger a state transition. */
typedef void (*callback_t)(data_t *);

/* Callback specifically for transitioning states. */
typedef state_t *(*transition_t)(state_t const __rom *);

/* All necessary information to execute and transition from a state. */
struct state_s {
	data_t      *data;
	callback_t   cb_init;
	callback_t   cb_loop;
	transition_t cb_next;
};

/* Initialize the data field for a state_t. */
data_t auto_straight_create(uint16_t, uint16_t, int8_t);
data_t auto_turn_create(uint16_t, uint16_t, int8_t);
data_t auto_ram_create(uint16_t, int8_t);
data_t auto_arm_create(uint16_t, uint16_t, int8_t);
data_t auto_ramp_create(uint16_t, uint16_t, int8_t);

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
void auto_none_init(data_t *);
void auto_none_loop(data_t *);
bool auto_none_isdone(data_t *);

bool auto_ram_isdone(data_t *);

#endif
