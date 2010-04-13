#ifndef STATE_KEVIN_H_
#define STATE_KEVIN_H_

#include <compilers.h>

#include "auton.h"

/* Define transition functions using a helper macro. */
#define AUTO_DONE            &auto_state[17]
#define STATE_NUM(_state_)   ((((int)(_state_) - (int)&auto_state[0]))/sizeof(state_t))
#define STATE_NAME(_state_)  auto_name[STATE_NUM(_state_)]

extern data_t auto_data[];
extern state_t const __rom auto_state[];
extern char const __rom * const __rom auto_name[];

#endif
