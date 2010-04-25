#include <compilers.h>

#include "auton.h"

#include "state.h"

#if defined(ROBOT_KEVIN)

STATE_START(start)
STATE(start,      1,    AUTO_WAIT(),             auto_none_init,     auto_none_loop,     deploy,     done,       auto_none_isdone)
STATE(deploy,     200,  AUTO_DEPLOY(),           auto_deploy_init,   auto_deploy_loop,   init_ram,   done,       auto_none_isdone)
STATE(init_ram,   6000, AUTO_RAM(127),           auto_straight_init, auto_straight_loop, init_arm,   init_arm,   auto_ram_isdone)
STATE(init_arm,   4000, AUTO_ARM(0, -127),       auto_arm_init,      auto_arm_loop,      init_dump1, init_dump1, auto_none_isdone)
STATE(init_dump1, 4000, AUTO_RAMP(100, 127),     auto_ramp_init,     auto_ramp_loop,     init_dump2, init_dump2, auto_ramp_isdone)
STATE(init_dump2, 4000, AUTO_WAIT(),             auto_none_init,     auto_none_loop,     init_dump3, init_dump3, auto_none_isdone)

/* Collect the white ball. */
STATE(init_dump3, 4000, AUTO_RAMP(0, -127),      auto_ramp_init,     auto_ramp_loop,     big_turn,   big_turn,   auto_ramp_isdone)
STATE(big_turn,   500,  AUTO_TURN(9, 127),       auto_turn_init,     auto_turn_loop,     big_drive,  big_drive,  auto_turn_isdone)
STATE(big_drive,  6000, AUTO_STRAIGHT(500,-127), auto_straight_init, auto_straight_loop, big_jig,    big_jig,    auto_straight_isdone)
STATE(big_jig,    2000, AUTO_TURN(9, -127),      auto_turn_init,     auto_turn_loop,     big_shove,  big_shove,  auto_turn_isdone)
STATE(big_shove,  1000, AUTO_DRIVE(-127),        auto_drive_init,    auto_drive_loop,    big_pickup, big_pickup, auto_none_isdone)
STATE(big_pickup, 5000, AUTO_PICKUP(70),         auto_pickup_init,   auto_pickup_loop,   big_ram,    big_ram,    auto_pickup_isdone)

/* Dump the previously harvested white ball. */
STATE(big_ram,    8000, AUTO_RAM(127),           auto_straight_init, auto_straight_loop, big_arm,    big_arm,    auto_ram_isdone)
STATE(big_arm,    4000, AUTO_ARM(0, -127),       auto_arm_init,      auto_arm_loop,      big_dump1,  big_dump1,  auto_arm_isdone)
STATE(big_dump1,  4000, AUTO_RAMP(100, 127),     auto_ramp_init,     auto_ramp_loop,     big_dump2,  big_dump2,  auto_ramp_isdone)
STATE(big_dump2,  4000, AUTO_WAIT(),             auto_none_init,     auto_none_loop,     big_dump3,  big_dump3,  auto_none_isdone)
STATE(big_dump3,  4000, AUTO_RAMP(0, -127),      auto_ramp_init,     auto_ramp_loop,     big_catch,  big_catch,  auto_ramp_isdone)

/* Prepare for telop. */
STATE(big_catch,  500,  AUTO_STRAIGHT(54, 127),  auto_straight_init, auto_straight_loop, done,       done,       auto_straight_isdone)
STATE(done,       1,    AUTO_WAIT(),             auto_none_init,     auto_none_loop,     done,       done,       auto_none_isdone)

#elif defined(ROBOT_NITISH)

STATE_START(start)
/* Dump the preloaded balls. */
STATE(start,      1,    AUTO_WAIT(),             auto_none_init,     auto_none_loop,     init_ram,   done,       auto_none_isdone)
STATE(init_ram,   8000, AUTO_RAM(127),           auto_straight_init, auto_straight_loop, init_arm,   init_arm,   auto_ram_isdone)
STATE(init_arm,   5000, AUTO_ARM(0, -127),       auto_arm_init,      auto_arm_loop,      init_back,  init_back,  auto_none_isdone)
STATE(init_back,  3000, AUTO_STRAIGHT(20, -127), auto_straight_init, auto_straight_loop, init_dump1, init_dump1, auto_straight_isdone)
STATE(init_dump1, 5000, AUTO_RAMP(100, 127),     auto_ramp_init,     auto_ramp_loop,     init_dump2, init_dump2, auto_ramp_isdone)
STATE(init_dump2, 5000, AUTO_WAIT(),             auto_none_init,     auto_none_loop,     init_dump3, done,       auto_none_isdone)

/* Collect the white ball. */
STATE(init_dump3, 5000, AUTO_RAMP(0, -127),      auto_ramp_init,     auto_ramp_loop,     big_turn,   big_turn,   auto_ramp_isdone)
STATE(big_turn,   2000, AUTO_TURN(22, -127),     auto_turn_init,     auto_turn_loop,     big_drive,  big_drive,  auto_turn_isdone)
/* Prevent the arm from digging into the foam mat. */
STATE(big_drive,  8000, AUTO_STRAIGHT(500,-127), auto_straight_init, auto_straight_loop, big_jig,    big_jig,    auto_straight_isdone)
STATE(big_jig,    2000, AUTO_TURN(15, 127),      auto_turn_init,     auto_turn_loop,     big_shove,  big_shove,  auto_turn_isdone)
STATE(big_shove,  1000, AUTO_DRIVE(-127),        auto_drive_init,    auto_drive_loop,    big_pickup, big_pickup, auto_none_isdone)
STATE(big_pickup, 5000, AUTO_PICKUP(80),         auto_pickup_init,   auto_pickup_loop,   big_ram,    big_ram,    auto_pickup_isdone)

/* Dump the previously harvested white ball. */
STATE(big_ram,    9000, AUTO_RAM(127),           auto_straight_init, auto_straight_loop, big_arm,    big_arm,   auto_ram_isdone)
STATE(big_arm,    4000, AUTO_ARM(0, -127),       auto_arm_init,      auto_arm_loop,      big_back,   big_back,  auto_arm_isdone)
STATE(big_back,   8000, AUTO_STRAIGHT(20, -127), auto_straight_init, auto_straight_loop, big_dump1,  big_dump1, auto_straight_isdone)
STATE(big_dump1,  5000, AUTO_RAMP(100, 127),     auto_ramp_init,     auto_ramp_loop,     big_dump2,  big_dump2, auto_ramp_isdone)
STATE(big_dump2,  5000, AUTO_WAIT(),             auto_none_init,     auto_none_loop,     big_dump3,  big_dump3, auto_none_isdone)
STATE(big_dump3,  5000, AUTO_RAMP(0, -127),      auto_ramp_init,     auto_ramp_loop,     big_catch,  big_catch, auto_ramp_isdone)

/* Prepare for telop. */
STATE(big_catch,  5000, AUTO_RAM(127),           auto_straight_init, auto_straight_loop, done,       done,      auto_ram_isdone)
STATE(done,       1,    AUTO_WAIT(),             auto_none_init,     auto_none_loop,     done,       done,      auto_none_isdone)

#endif
