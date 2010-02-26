#ifndef AUTON_H_
#define AUTON_H_

#include "stdint.h"

/* Maximum difference in between the two side sensor readings used for
 * proportional correction.
 */
#define FU_CRUISE_DIST       300
#define FU_SEN_IR_OMEGA_ERR  50
#define FU_SEN_IR_STRAFE_ERR 5

#define FU_TURN_TICKS 48

/* Parameters for the AUTO_CRUISE state, all specified in centimeters. */
#define CRUISE_STOP_CM 200
#define CRUISE_DIST_CM 100

#include "stdbool.h"

/* Macroscopic states that describe the general behavior of the robot. Each of
 * these states corresponds to a single action or a smaller state machine.
 */
typedef enum {
	AUTO_FIELD,
	AUTO_IDLE,
	AUTO_RAISE,
	AUTO_DUMP,
	AUTO_CRUISE
} GlobalState;

/* Sub-states of the AUTO_PICKUP state. */
typedef enum {
	PICKUP_RAISE,
	PICKUP_LOWER
} PickupState;

/* Sub-states of the AUTO_DEPOSIT state. */
typedef enum {
	DEPOSIT_REVERSE,
	DEPOSIT_ARM,
	DEPOSIT_RAISE,
	DEPOSIT_WAIT,
	DEPOSIT_FORWARD,
	DEPOSIT_LOWER
} DepositState;

/* Vertically shifted input scaling. */
uint16_t prop_scale(int8_t minOut, int8_t maxOut, uint16_t maxErr, int16_t err);

/* Convert a raw infrared sensor value into a distance, in centimeters. */
uint16_t ir_to_cm(uint8_t);

/* Drive in a straight line by using the difference between two side-mounted IR
 * sensor readings. Stops far enough from a wall to safely lift the arm.
 */
bool cruise(void);

/* Reverse until a wall is encountered. When a wall is found, all balls in the
 * robot's basket are dumped and the the basket is restored to rest.
 */
bool deposit(void);

/* Pickup all of the balls currently on the arm and desposit them into the rear
 * storage basket.
 */
bool pickup(void);

/* General-purpose autonomous mode function; invoked once per slow loop. */
void auton_do(void);

#endif

