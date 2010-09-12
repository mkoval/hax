#ifndef ROBOT_COMP2010_H_
#define ROBOT_COMP2010_H_

#include <hax.h>
#include <stdbool.h>

#define ARM_SET_ERRMAX        10
#define DRIVE_STRAIGHT_ERRMAX 100
#define DRIVE_TURN_ERRMAX     ((int16_t)30 * ENC_PER_DEG)
#define DRIVE_TURN_ERRMIN     ((int16_t)5  * ENC_PER_DEG)

/* Control the speed of the drive motors to get a desired forward movement
 * and rotation. Positive values indicate forward movement and counter-
 * clockwise rotation.
 */
void drive_raw(int8_t l, int8_t r);
void drive_smart(int8_t l, int8_t r);

/* Control the speed of the arm motors to raise or lower the arm. Positive
 * values indicate upward movement. Both extremes are checked in software using
 * a potentiometer.
 */
void arm_raw(int8_t motor);
bool arm_smart(int8_t motor);

/* Control the speed of the ramp motor(s) to raise or lower the ramp. Positive
 * values indicate upward movement. Both extremes are checked in software using
 * limit switches.
 */
void ramp_raw(int8_t, int8_t);
bool ramp_smart(int8_t);

/* Drive forward or reverse in a straight line, using encoders to correct for
 * small errors. Returns the distance traveled in inches.
 */
int32_t drive_straight(int8_t);

/* Turn about the robot's center, using encoders to measure the distance
 * travelled and correct for errors.
 */
bool drive_turn(int16_t);

#endif
