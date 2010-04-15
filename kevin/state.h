#ifndef STATE_KEVIN_H_
#define STATE_KEVIN_H_

#include <compilers.h>

#include "auton.h"

STATE_START()
STATE(done, AUTO_WAIT(0), auto_none_init, auto_none_loop, done, done, false)
STATE_DONE()

#endif
