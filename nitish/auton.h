#ifndef AUTON_H_
#define AUTON_H_

#include <stdbool.h>
#include <stdint.h>

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

#define PROP_SCALE(maxOut, maxErr, err) \
    ((maxOut) * MIN((err), (maxErr)) / (maxErr))

/* Macroscopic states that describe the general behavior of the robot. Each of
 * these states corresponds to a single action or a smaller state machine.
 */
typedef enum {
	AUTO_IDLE
} GlobalState;

/* Vertically shifted input scaling. */
uint16_t prop_scale(int8_t minOut, int8_t maxOut, uint16_t maxErr, int16_t err);

/* Convert a raw infrared sensor value into a distance, in centimeters. */
uint16_t ir_to_cm(uint8_t);

/* General-purpose autonomous mode function; invoked once per slow loop. */
void auton_do(void);

#endif

