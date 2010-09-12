#ifndef AUTON_COMP2010_H_
#define AUTON_COMP2010_H_

#include <hax.h>
#include <stdbool.h>
#include <stdint.h>

#include "ports.h"
#include "util.h"

#define STRAIGHT_ERROR 100
#define TURN_ERROR     ((int16_t)10 * ENC_PER_DEG)
#define TURN_THRESHOLD ((int16_t)5  * ENC_PER_DEG)

#define MS_TO_LOOPS(_x_)   (((uint32_t)(_x_) * (uint32_t)1000 / SLOW_US))
#define IN10_TO_TICKS(_x_) ((uint32_t)(_x_) * ENC_PER_IN / 10)
#define DEG_TO_TICKS(_x_)  ((uint32_t)(_x_) * ENC_PER_DEG)

/* Constructors used to wrap the initialization of a data_t. */
#define AUTO_STRAIGHT(_dist_, _vel_) {{ IN10_TO_TICKS(_dist_), (_vel_)         }}
#define AUTO_DRIVE(_vel_)            {{ 0,                     (_vel_)         }}
#define AUTO_DEPLOY()                {{ 0,                     LIFT_DEPLOY_VEL }}
#define AUTO_RAM(_vel_)              {{ 0,                     (_vel_)         }}
#define AUTO_TURN(_ang_, _vel_)      {{ DEG_TO_TICKS(_ang_),   (_vel_)         }}
#define AUTO_ARM(_pos_, _vel_)       {{ (_pos_),               (_vel_)         }}
#define AUTO_PICKUP(_vel_)           {{ 0,                     (_vel_)         }}
#define AUTO_RAMP( _pos_, _vel_)     {{ (_pos_),               (_vel_)         }}
#define AUTO_WAIT()                  {{ 0,                     0               }}

/* Define a transition function with name _name_ that transitions into the
 * _ntime_ state if auto_istimeout() is true, _ncond_ if _cond_ evaluates to
 * true, otherwise the current state.
 */
#define STATE_START(_name_)    \
extern state_t const __rom _st_##_name_##_state; \
state_t const __rom *auto_current = &_st_##_name_##_state; \
mutable_t            auto_mutable = { 0 };

#define STATE(_name_, _timeout_, _data_, _cbinit_, _cbloop_, _stsuc_,         \
              _stfail_, _cond_)                                               \
/* Forward declare the states that are transitioned into. */                  \
extern state_t const __rom _st_##_stsuc_##_state;                             \
extern state_t const __rom _st_##_stfail_##_state;                            \
state_t const __rom *_st_##_name_##_transition(state_t const __rom *,         \
                                               mutable_t *);                  \
                                                                              \
/* State structure and data used by this state. */                            \
data_t  const __rom _st_##_name_##_data  = _data_;                            \
state_t const __rom _st_##_name_##_state = {                                  \
	MS_TO_LOOPS(_timeout_), #_name_, &_st_##_name_##_data,                    \
	_cbinit_, _cbloop_, _st_##_name_##_transition                             \
};                                                                            \
                                                                              \
state_t const __rom *_st_##_name_##_transition(state_t const __rom *state,    \
                                               mutable_t *mut) {              \
	if (_cond_(state, mut)) {                                                 \
		return &_st_##_stsuc_##_state;  /* Success condition. */              \
	} else if (mut->timer >= state->timeout) {                                \
		return &_st_##_stfail_##_state; /* Timeout condition. */              \
	} else {                                                                  \
		return state;                   /* No transition. */                  \
	}                                                                         \
}

typedef union {
	/* Control the robot's position and orientation using a timeout and encoder
	 * ticks. Applies to both forward movement and turning, although encoder
	 * tick calculations are performed slightly differently.
	 */
	struct {
		uint16_t ticks;
		int8_t   vel;
	} move;

	/* Control the pose of a posable part of the robot (arm or ramp) using a
	 * potentiometer value and a timeout.
	 */
	struct {
		uint16_t pos;     /* potentiometer value */
		int8_t   vel;
	} pose;
} data_t;

/* Mutable data used to keep track of the internal state of a state_t. */
typedef struct {
	uint32_t timer;
	uint32_t enc_left;
	uint32_t enc_right;
} mutable_t;

/* Forward declaration is required to avoid a recursive type reference. */
typedef struct state_s state_t;

/* Generalized callback that does not trigger a state transition. */
typedef void (*callback_t)(state_t const __rom *, mutable_t *);

/* Callback specifically for transitioning states. */
typedef state_t const __rom *(*transition_t)(state_t const __rom *, mutable_t *);

/* All necessary information to execute and transition from a state. */
struct state_s {
	uint32_t            timeout;
	char const __rom   *name;
	data_t const __rom *data;
	callback_t          cb_init;
	callback_t          cb_loop;
	transition_t        cb_next;
};

void auto_deploy_init(state_t const __rom *, mutable_t *);
void auto_deploy_loop(state_t const __rom *, mutable_t *);

/* Drive forward until the the terminating condition is true. */
void auto_drive_init(state_t const __rom *, mutable_t *);
void auto_drive_loop(state_t const __rom *, mutable_t *);

/* Drive straight for the specified distance.*/
void auto_straight_init(state_t const __rom *, mutable_t *);
void auto_straight_loop(state_t const __rom *, mutable_t *);
bool auto_straight_isdone(state_t const __rom *, mutable_t *);

/* Drive back while lifting the arm to collect balls. */
void auto_pickup_init(state_t const __rom *, mutable_t *);
void auto_pickup_loop(state_t const __rom *, mutable_t *);
bool auto_pickup_isdone(state_t const __rom *, mutable_t *);

/* Turn through the specified number of degrees. */
void auto_turn_init(state_t const __rom *, mutable_t *);
void auto_turn_loop(state_t const __rom *, mutable_t *);
bool auto_turn_isdone(state_t const __rom *, mutable_t *);

/* Rotate the arm into the specified position. */
void auto_arm_init(state_t const __rom *, mutable_t *);
void auto_arm_loop(state_t const __rom *, mutable_t *);
bool auto_arm_isdone(state_t const __rom *, mutable_t *);

/* Raise or lower the ramp into the specified position. */
void auto_ramp_init(state_t const __rom *, mutable_t *);
void auto_ramp_loop(state_t const __rom *, mutable_t *);
bool auto_ramp_isdone(state_t const __rom *, mutable_t *);

/* Do nothing until the state times out. */
void auto_none_init(state_t const __rom *, mutable_t *);
void auto_none_loop(state_t const __rom *, mutable_t *);
bool auto_none_isdone(state_t const __rom *, mutable_t *);

bool auto_ram_isdone(state_t const __rom *, mutable_t *);

#endif
