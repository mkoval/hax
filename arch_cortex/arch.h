#ifndef ARCH_CORTEX_H_
#define ARCH_CORTEX_H_
/* Macros prefixed with '_' are for arch_cortex internal use */

/* Period of the slow loop, in microseconds. */
#define SLOW_US 18200

/* Size of each contiguous I/O index block */
#define CT_ANALOG      8
#define CT_DIGITAL    12
#define CT_MOTOR      10
#define CT_OI_GROUPS_PER_OI     11
#define CT_OI_BUTTONS_PER_GROUP 4 /* Max number of buttons per group */
#define CT_OI         (2 * CT_OI_GROUPS_PER_OI)
/* buttons per oi */
#define CT_OIx_BUTTON (CT_OI_BUTTONS_PER_GROUP * CT_OI_GROUPS_PER_OI)
#define CT_OI_BUTTON  (2 * CT_OIx_BUTTON)

/* IX_OI_BUTTON dir arguments */
#define OI_B_UP 0
#define OI_B_LT 1
#define OI_B_DN 2
#define OI_B_RT 3

/* Offset for each block of contiguous I/O indexes. */
#define OFFSET_ANALOG     0
#define OFFSET_DIGITAL   (OFFSET_ANALOG + CT_ANALOG)
#define OFFSET_MOTOR     (OFFSET_DIGITAL + CT_DIGITAL)
#define OFFSET_OI        (OFFSET_MOTOR + CT_MOTOR)
#define OFFSET_OI_BUTTON (OFFSET_OI + CT_OI)

/* Macros to generate internal I/O indexes. */
#define IX_INTERRUPT(pin) IX_DIGITAL(pin)
#define IX_ANALOG(pin)    (OFFSET_ANALOG + (pin) - 1)
#define IX_DIGITAL(pin)   (OFFSET_DIGITAL + (pin) - 1)
#define IX_MOTOR(n)       (OFFSET_MOTOR + (n) - 1)
#define IX_OI_GROUP(oi, group)  (OFFSET_OI + (((oi) - 1) * \
			CT_OI_GROUPS_PER_OI) + (group) - 1)

#define IX_OI_BUTTON(oi, group, dir) \
	((((oi) - 1) * CT_OIx_BUTTON) \
	 + _IX_OI_BUTTON(group, dir) + OFFSET_OI_BUTTON)
#define _IX_OI_BUTTON(group, dir) \
	((((group) - 1) * CT_OI_BUTTONS_PER_GROUP) + (dir))

/* Inverses to convinient 0 indexed sets */
#define IX_OI_GROUP_INV(ix) _IX_OI_GROUP_INV((ix) - OFFSET_OI)
#define _IX_OI_GROUP_INV(ix) ((ix) >= CT_OI_GROUPS_PER_OI)? \
	((ix) - CT_OI_GROUPS_PER_OI):(ix)

#define IX_OI_OI_INV(ix) _IX_OI_OI_INV((ix) - OFFSET_OI)
#define _IX_OI_OI_INV(ix) (((ix) >= CT_OI_GROUPS_PER_OI)?(1):(0))

#define IX_OI_BUTTON_OI_INV(ix) _IX_OI_BUTTON_OI_INV((ix) - OFFSET_OI_BUTTON)
#define _IX_OI_BUTTON_OI_INV(ix) \
	(((ix) >= CT_OIx_BUTTON)?  \
		(1):(0))

#define IX_OI_BUTTON_GROUP_INV(ix, oi) \
	(IX_OI_BUTTON_INV_BUTTONS(ix, oi) / CT_OI_BUTTONS_PER_GROUP)

#define IX_OI_BUTTON_INV_BUTTONS(ix, oi) \
	(((ix) - (oi) * CT_OIx_BUTTON) - OFFSET_OI_BUTTON)

/* Joysticks */
#define OI_JOY_R_X(oi) IX_OI_GROUP(oi, 1)
#define OI_JOY_R_Y(oi) IX_OI_GROUP(oi, 2)
#define OI_JOY_L_Y(oi) IX_OI_GROUP(oi, 3)
#define OI_JOY_L_X(oi) IX_OI_GROUP(oi, 4)

#define OI_ROCKER_L(oi) IX_OI_GROUP(oi, 5)
#define OI_ROCKER_R(oi) IX_OI_GROUP(oi, 6)
#define OI_TRIGGER_L(oi, dir) IX_OI_BUTTON(oi, 5, dir)
#define OI_TRIGGER_R(oi, dir) IX_OI_BUTTON(oi, 6, dir)

#endif
