#include "../hax.h"

#include "auton.h"
#include "encoder.h"
#include "ports.h"
#include "robot.h"

#include "user.h"

/* Physical and electronic robot configuration is specified in ports.h. */
uint8_t kNumAnalogInputs = ANA_NUM;

/* Autonomous queue; used to build a specific autonomous mode from the building
 * blocks provided in the state machine defined in auton.c.
 */
static AutonQueue queue;

void init(void) {
	/* Pre-define all of autonomous mode as a giant state machine.
	 * 1. Ram the center wall, dislodging the orange football and pushing the
	 *      four green balls under the wall.
	 * 2. Strafe left to dislodge the left football from its high perch.
	 * 3. Strafe right to dislodge the right football from its high perch.
	 * 4. Reverse to a safe distance from the wall to lower the arm.
	 * 5. Turn around in preparation for dumping.
	 * 6. Reverse into the wall, preparing to dump the preloaded balls.
	 * 7. Lift the ramp, deposit the balls, and lower the ramp.
	 */
	/* TODO Replace this with something simplier and better thought-out. */
	auton_enqueue(&queue, AUTO_FWDRAM,  FIELD_LENGTH_IN / 2 - ROB_LENGTH_IN);
	auton_enqueue(&queue, AUTO_STRAFE,  NONE);
	auton_enqueue(&queue, AUTO_STRAFE,  NONE);
	auton_enqueue(&queue, AUTO_REVERSE, ROB_ARM_IN);
	auton_enqueue(&queue, AUTO_TURN,    180);
	auton_enqueue(&queue, AUTO_REVRAM,  NONE);
	auton_enqueue(&queue, AUTO_DEPOSIT, NONE);
	auton_enqueue(&queue, AUTO_DONE,    NONE);

	/* Initialize the encoder API; from now on we can use the logical mappings
	 * ENC_L, ENC_R, and ENC_S without worrying about the wiring of the robot.
	 */
	encoder_init(ENC_L, INT_ENC_L1, INT_ENC_L2); /* Left   */
	encoder_init(ENC_R, INT_ENC_R1, INT_ENC_R2); /* Right  */
	encoder_init(ENC_S, INT_ENC_S1, INT_ENC_S2); /* Strafe */
}

void disable_loop(void) {
}

void disable_spin(void) {
}

void auton_loop(void) {
	auton_do(&queue);
}

void auton_spin(void) {
}

void telop_loop(void) {
	uint16_t forward = analog_oi_get(OI_L_Y);
	uint16_t strafe  = analog_oi_get(OI_R_X);
	uint16_t rotate  = analog_oi_get(OI_L_X);
	uint16_t arm     = analog_oi_get(OI_L_B);
	uint16_t ramp    = analog_oi_get(OI_R_B);

	drive_raw(forward, strafe, rotate);
	arm_raw(arm);
	ramp_raw(ramp);
}

void telop_spin(void) {
}
