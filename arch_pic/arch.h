#ifndef ARCH_PIC_H_
#define ARCH_PIC_H_

/* Slow loop of 18.5 milliseconds (converted to microseconds). */
#define SLOW_US 18500

/* Counts for IO 'things' */
#define CT_DIGITAL    16
#define CT_INTERRUPT   6
#define CT_MOTOR       8
#define CT_OIx         6
#define CT_OI         (2 * CT_OIx)
#define CT_OIx_BUTTON  8
#define CT_OI_GROUP_SZ 2
#define CT_OI_BUTTON  (2 * CT_OIx_BUTTON)

/* PIN_OI_BUTTON dir arguments */
#define OI_B_UP 0
#define OI_B_DN 1

/* Offset for each block of contiguous pins. */
#define OFFSET_DIGITAL      0
#define OFFSET_INTERRUPT   (OFFSET_DIGITAL + CT_DIGITAL)
#define OFFSET_MOTOR       (OFFSET_INTERRUPT + CT_INTERRUPT)
#define OFFSET_OI          (OFFSET_MOTOR + CT_MOTOR)
#define OFFSET_OI_BUTTON   (OFFSET_OI + CT_OI)

/* Utility macros for generating internal pin indexes. */
#define PIN_DIGITAL(pin)      ((pin) + OFFSET_DIGITAL)
#define PIN_INTERRUPT(pin)    ((pin) + OFFSET_INTERRUPT)
#define PIN_MOTOR(n)          ((n) + OFFSET_MOTOR)
#define PIN_OI(oi, group)     (OFFSET_OI + ((oi) * CT_IOx) + (group))
#define PIN_OI_BUTTON(io, group, dir) \
	(OFFSET_OI_BUTTON + ((oi) * CT_IOx) + \
	 ((group) * CT_OI_GROUP_SZ) + dir)

/* Joysticks */
#define JOY_L_X(oi) PIN_OI(oi, 0)
#define JOY_L_Y(oi) PIN_OI(oi, 1)
#define JOY_R_X(oi) PIN_OI(oi, 2)
#define JOY_R_Y(oi) PIN_OI(oi, 3)

#endif
