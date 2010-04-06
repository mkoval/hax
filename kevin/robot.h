#ifndef ROBOT_MIKISH_H_
#define ROBOT_MIKISH_H_

#include <hax.h>
#include <stdbool.h>

#define ARM_SET_ERRMAX        10
#define DRIVE_STRAIGHT_ERRMAX 100

/* Control the speed of the drive motors to get a desired forward movement
 * and rotation. Positive values indicate forward movement and counter-
 * clockwise rotation.
 */
void drive_raw(AnalogOut, AnalogOut);

/* Control the speed of the arm motors to raise or lower the arm. Positive
 * values indicate upward movement. Both extremes are checked in software using
 * a potentiometer.
 */
bool arm_raw(AnalogOut);

/* Control the speed of the ramp motor(s) to raise or lower the ramp. Positive
 * values indicate upward movement. Both extremes are checked in software using
 * limit switches.
 */
bool ramp_raw(AnalogOut);

/* Drive forward or reverse in a straight line, using encoders to correct for
 * small errors. Returns the distance traveled in inches.
 */
int32_t drive_straight(AnalogOut);

/* Turn about the robot's center, using encoders to measure the distance
 * travelled and correct for errors.
 */
int32_t drive_turn(AnalogOut);

#endif
