#ifndef USER_H_
#define USER_H_

/*!
 * Converts an OI button input into one of three values: -1 (bottom button
 * pressed), 0 (no button pressed), and 1 (top button pressed). Undefined
 * return value if both buttons are pressed.
 *
 * \param io raw IO analog value
 */
int8_t button(int8_t);

/*!
 * Changes the outputs of all drive motors to move in the desired direction.
 * Omega values of +/-127 indicate no forward movement, whereas a spin value
 * of 0 indicates no rotation.
 *
 * \param x side-to-side translation, where positive values are "right"
 * \param y front-to-back translation, where positive values are "forward"
 * \param omega spin value of arbitrary units; zero indicates no rotation
 */
void drive_omni(int8_t, int8_t, int8_t);

/*!
 * Move the arm to the desired location, as specified in potentiometer units
 * that range from SEN_SHOULDER_LOW to SEN_SHOULDER_HIGH.
 *
 * \param angle desired potentiometer reading
 */
void lift_arm(int16_t);

#endif

