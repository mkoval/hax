#ifndef STATE_COMP2010_H_
#define STATE_COMP2010_H_

#include "auton.h"

extern transition_t const __rom auto_cbs[];
extern state_t const __rom     *auto_current;
extern mutable_t                auto_mutable;

#endif
