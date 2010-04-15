#include <stdbool.h>

#include "state.h"

static bool kFalse = false;

AUTO_LEAVE(0,  &auto_state[1],  &auto_state[1],  kFalse)
AUTO_LEAVE(1,  AUTO_DONE,       &auto_state[2],  auto_ram_isdone(cur->data))
AUTO_LEAVE(2,  AUTO_DONE,       &auto_state[3],  auto_arm_isdone(cur->data))
AUTO_LEAVE(3,  AUTO_DONE,       &auto_state[4],  auto_ramp_isdone(cur->data))
AUTO_LEAVE(4,  &auto_state[5],  AUTO_DONE,       kFalse)
AUTO_LEAVE(5,  AUTO_DONE,       &auto_state[6],  auto_ramp_isdone(cur->data))

AUTO_LEAVE(6,  AUTO_DONE,       &auto_state[7],  auto_straight_isdone(cur->data))
AUTO_LEAVE(7,  AUTO_DONE,       &auto_state[8],  auto_turn_isdone(cur->data))
AUTO_LEAVE(8,  AUTO_DONE,       &auto_state[9],  auto_straight_isdone(cur->data))
AUTO_LEAVE(9,  AUTO_DONE,       &auto_state[10], auto_arm_isdone(cur->data))
AUTO_LEAVE(10, &auto_state[11], AUTO_DONE,       kFalse)
AUTO_LEAVE(11, AUTO_DONE,       &auto_state[12], auto_arm_isdone(cur->data))

AUTO_LEAVE(12, AUTO_DONE,       &auto_state[13], auto_turn_isdone(cur->data))
AUTO_LEAVE(13, AUTO_DONE,       &auto_state[14], auto_ram_isdone(cur->data))
AUTO_LEAVE(14, AUTO_DONE,       &auto_state[15], auto_ramp_isdone(cur->data))
AUTO_LEAVE(15, &auto_state[16], AUTO_DONE,       kFalse)
AUTO_LEAVE(16, AUTO_DONE,       AUTO_DONE,       auto_ramp_isdone(cur->data))

AUTO_LEAVE(17, AUTO_DONE,       AUTO_DONE,       kFalse)

char const __rom *const __rom auto_name[] = {
	"deploy",
	"ram wall",
	"lower arm",
	"raise ramp",
	"wait",
	"lower ramp",

	"drive straight",
	"turn 90 degrees",
	"drive straight",
	"raise arm",
	"wait",
	"lower arm",

	"turn 90 degrees",
	"ram wall",
	"raise ramp",
	"wait",
	"lower ramp",
	"done"
};

#define UNUSED 0

data_t auto_data[] = {
	/* Dump preloaded balls. */
	AUTO_ARM(250, 0, kMotorMax),
	AUTO_RAM(4000, kMotorMax),
	AUTO_ARM(1000, 0, kMotorMin),
	AUTO_RAMP(500, UNUSED, kMotorMax),
	AUTO_WAIT(1000),
	AUTO_RAMP(500, UNUSED, kMotorMin),

	/* Collect the first three footballs. */
	AUTO_STRAIGHT(500, 60, kMotorMax),
	AUTO_TURN(500, 90, kMotorMax),
	AUTO_STRAIGHT(500, 480, kMotorMax),
	AUTO_ARM(500, UNUSED, kMotorMax),
	AUTO_WAIT(100),
	AUTO_ARM(500, UNUSED, kMotorMin),
	
	/* Dump the freshly-harvested balls. */
	AUTO_TURN(500, 90, kMotorMin),
	AUTO_RAM(500, kMotorMax),
	AUTO_RAMP(250, UNUSED, kMotorMax),
	AUTO_WAIT(1000),
	AUTO_RAMP(250, UNUSED, kMotorMin),

	/* Do nothing for the remainder of autonomous. */
	AUTO_WAIT(0)
};

state_t const __rom auto_state[] = {
	/* Dump preloaded balls. */
	{ &auto_data[0],  auto_arm_init,      auto_arm_loop,      AUTO_LOOKUP(0)  },
	{ &auto_data[1],  auto_straight_init, auto_straight_loop, AUTO_LOOKUP(1)  },
	{ &auto_data[2],  auto_arm_init,      auto_arm_loop,      AUTO_LOOKUP(2)  },
	{ &auto_data[3],  auto_ramp_init,     auto_ramp_loop,     AUTO_LOOKUP(3)  },
	{ &auto_data[4],  auto_none_init,     auto_none_loop,     AUTO_LOOKUP(4)  },
	{ &auto_data[5],  auto_ramp_init,     auto_ramp_loop,     AUTO_LOOKUP(5)  },

	/* Collect the first three footballs. */
	{ &auto_data[6],  auto_straight_init, auto_straight_loop, AUTO_LOOKUP(6)  },
	{ &auto_data[7],  auto_turn_init,     auto_turn_loop,     AUTO_LOOKUP(7)  },
	{ &auto_data[8],  auto_straight_init, auto_straight_loop, AUTO_LOOKUP(8)  },
	{ &auto_data[9],  auto_arm_init,      auto_arm_loop,      AUTO_LOOKUP(9)  },
	{ &auto_data[10], auto_none_init,     auto_none_loop,     AUTO_LOOKUP(10) },
	{ &auto_data[11], auto_arm_init,      auto_arm_loop,      AUTO_LOOKUP(11) },
	
	/* Dump the freshly-harvested balls. */
	{ &auto_data[12], auto_turn_init,     auto_turn_loop,     AUTO_LOOKUP(12) },
	{ &auto_data[13], auto_straight_init, auto_straight_loop, AUTO_LOOKUP(13) },
	{ &auto_data[14], auto_ramp_init,     auto_ramp_loop,     AUTO_LOOKUP(14) },
	{ &auto_data[15], auto_none_init,     auto_none_loop,     AUTO_LOOKUP(15) },
	{ &auto_data[16], auto_ramp_init,     auto_ramp_loop,     AUTO_LOOKUP(16) },

	/* Do nothing for the remainder of autonomous. */
	{ &auto_data[17], auto_none_init,     auto_none_loop,     AUTO_LOOKUP(17) }
};

