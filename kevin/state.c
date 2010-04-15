#include <stdbool.h>

#include "auton.h"
#include "state.h"

STATE_START()
STATE(done, AUTO_WAIT(0), auto_none_init, auto_none_loop, done, done, false)
STATE_DONE()

