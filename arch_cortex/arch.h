#ifndef ARCH_CORTEX_H_
#define ARCH_CORTEX_H_

/* Period of the slow loop, in microseconds. */
#define SLOW_US 18200

/* Counts for IO 'things' */
#define CT_ANALOG      8
#define CT_DIGITAL    12
#define CT_MOTOR      10
#define CT_OIx         8 /* groups per OI */
#define CT_OI_GROUP_SZ 4 /* Max number of buttons per group */
#define CT_OI         (2 * CT_OIx)
#define CT_OIx_BUTTON (CT_OI_GROUP_SZ * CT_OIx)
#define CT_OI_BUTTON  (2 * CT_OIx_BUTTON)

/* PIN_OI_BUTTON dir arguments */
#define OI_B_UP 0
#define OI_B_LT 1
#define OI_B_DN 2
#define OI_B_RT 3

/* Offset for each block of contiguous pins. */
#define OFFSET_ANALOG     0
#define OFFSET_DIGITAL   (OFFSET_ANALOG + CT_ANALOG)
#define OFFSET_MOTOR     (OFFSET_DIGITAL + CT_DIGITAL)
#define OFFSET_OI        (OFFSET_MOTOR + CT_MOTOR)
#define OFFSET_OI_BUTTON (OFFSET_OI + CT_OI)

/* Utility macros for generating internal pin indexes. */
#define PIN_ANALOG(pin)   (OFFSET_ANALOG + (pin) - 1)
#define PIN_DIGITAL(pin)  (OFFSET_DIGITAL + (pin) - 1)
#define PIN_MOTOR(n)      (OFFSET_MOTOR + (n) - 1)
#define PIN_OI(oi, group) (OFFSET_OI + ((oi) * CT_IOx) + (group) - 1)
#define PIN_OI_BUTTON(oi, group, dir) \
	(OFFSET_OI_BUTTON + ((oi) * CT_OIx_BUTTON) + \
	 (((group) - 1) * CT_OI_GROUP_SZ) + (dir))

/* Joysticks */
#define JOY_L_X(oi) PIN_OI(oi, 0)
#define JOY_L_Y(oi) PIN_OI(oi, 1)
#define JOY_R_X(oi) PIN_OI(oi, 2)
#define JOY_R_Y(oi) PIN_OI(oi, 3)

#endif
