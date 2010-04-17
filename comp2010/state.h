#ifndef STATE_COMP2010_H_
#define STATE_COMP2010_H_

#include <compilers.h>

#include "auton.h"

static bool kFalse = 0;

#if defined(ROBOT_KEVIN)

STATE_START()
STATE(deploy,      AUTO_ARM(250, 100, 127),       auto_arm_init,      auto_arm_loop,      init_ram,    init_ram, kFalse)
STATE(init_ram,    AUTO_RAM(2000, 127),           auto_straight_init, auto_straight_loop, init_arm,    done,     auto_ram_isdone(state->data))
STATE(init_arm,    AUTO_ARM(500, 0,   127),       auto_arm_init,      auto_arm_loop,      init_dump1,  done,     auto_arm_isdone(state->data))
STATE(init_dump1,  AUTO_RAMP(500, 100, 127),      auto_ramp_init,     auto_ramp_loop,     init_dump2,  done,     auto_ramp_isdone(state->data)) 
STATE(init_dump2,  AUTO_WAIT(1000),               auto_none_init,     auto_none_loop,     init_dump3,  done,     auto_none_isdone(state->data))
STATE(init_dump3,  AUTO_RAMP(500, 0, 127),        auto_ramp_init,     auto_ramp_loop,     coll_drive1, done,     auto_ramp_isdone(state->data))
STATE(coll_drive1, AUTO_STRAIGHT(1000, 120, 127), auto_straight_init, auto_straight_loop, coll_turn1,  done,     auto_straight_isdone(state->data))
STATE(coll_turn1,  AUTO_TURN(500, 90, 127),       auto_turn_init,     auto_turn_loop,     coll_drive2, done,     auto_turn_isdone(state->data))
STATE(coll_drive2, AUTO_STRAIGHT(3000, 480, 127), auto_straight_init, auto_straight_loop, coll_turn2,  done,     auto_straight_isdone(state->data))
STATE(coll_turn2,  AUTO_TURN(500, 90, 127),       auto_turn_init,     auto_turn_loop,     coll_dump1,  done,     auto_turn_isdone(state->data))
STATE(coll_dump1,  AUTO_RAMP(500, 100, 127),      auto_ramp_init,     auto_ramp_loop,     coll_dump2,  done,     auto_ramp_isdone(state->data))
STATE(coll_dump2,  AUTO_WAIT(1000),               auto_none_init,     auto_none_loop,     coll_dump3,  done,     auto_none_isdone(state->data))
STATE(coll_dump3,  AUTO_RAMP(500, 0, 127),        auto_ramp_init,     auto_ramp_loop,     done,        done,     auto_ramp_isdone(state->data))
STATE(done,        AUTO_WAIT(1),                  auto_none_init,     auto_none_loop,     done,        done,     kFalse)
STATE_DONE()

transition_t auto_cbs[_st_end - _st_start - 1] = {
	STATE_LOOKUP(deploy),
	STATE_LOOKUP(init_ram),
	STATE_LOOKUP(init_arm),
	STATE_LOOKUP(init_dump1),
	STATE_LOOKUP(init_dump2),
	STATE_LOOKUP(init_dump3),
	STATE_LOOKUP(coll_drive1),
	STATE_LOOKUP(coll_turn1),
	STATE_LOOKUP(coll_drive2),
	STATE_LOOKUP(coll_turn2),
	STATE_LOOKUP(coll_dump1),
	STATE_LOOKUP(coll_dump2),
	STATE_LOOKUP(coll_dump3),
	STATE_LOOKUP(done)
};

#elif defined(ROBOT_NITISH)

STATE_START()
STATE(init_ram,    AUTO_RAM(2000, 127),           auto_straight_init, auto_straight_loop, init_arm,    done, auto_ram_isdone(state->data))
STATE(init_arm,    AUTO_ARM(5000, 0, -127),       auto_arm_init,      auto_arm_loop,      init_dump1,  done, auto_arm_isdone(state->data))
STATE(init_dump1,  AUTO_RAMP(5000, 100, 127),     auto_ramp_init,     auto_ramp_loop,     init_dump2,  done, auto_ramp_isdone(state->data)) 
STATE(init_dump2,  AUTO_WAIT(2000),               auto_none_init,     auto_none_loop,     init_dump3,  done, auto_none_isdone(state->data))
STATE(init_dump3,  AUTO_RAMP(5000, 0, -127),      auto_ramp_init,     auto_ramp_loop,     done,        done, auto_ramp_isdone(state->data))
STATE(done,        AUTO_WAIT(1),                  auto_none_init,     auto_none_loop,     done,        done, kFalse)
STATE_DONE()

transition_t auto_cbs[_st_end - _st_start - 1] = {
	STATE_LOOKUP(init_ram),
	STATE_LOOKUP(init_arm),
	STATE_LOOKUP(init_dump1),
	STATE_LOOKUP(init_dump2),
	STATE_LOOKUP(init_dump3),
	STATE_LOOKUP(done)
};

#endif

#endif
