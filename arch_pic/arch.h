#ifndef ARCH_PIC_H_
#define ARCH_PIC_H_

/* Slow loop of 18.5 milliseconds (converted to microseconds). */
#define SLOW_US 18500

/* Counts for IO 'things' */
#define CT_DIGITAL    16
#define CT_INTERRUPT   6
#define CT_MOTOR       8
#define CT_OIx         6 /* groups per OI */
#define CT_OI_GROUP_SZ 2 /* Max number of buttons per group */
#define CT_OI         (2 * CT_OIx)
#define CT_OIx_BUTTON (CT_OI_GROUP_SZ * CT_OIx)
#define CT_OI_BUTTON  (2 * CT_OIx_BUTTON)

/* IX_OI_BUTTON dir arguments */
#define OI_B_UP 0
#define OI_B_DN 1

/* Offset for each block of contiguous pins. */
#define OFFSET_DIGITAL      0
#define OFFSET_INTERRUPT   (OFFSET_DIGITAL + CT_DIGITAL)
#define OFFSET_MOTOR       (OFFSET_INTERRUPT + CT_INTERRUPT)
#define OFFSET_OI          (OFFSET_MOTOR + CT_MOTOR)
#define OFFSET_OI_BUTTON   (OFFSET_OI + CT_OI)

/* Macros to generate internal I/O indexes. */
#define IX_ANALOG(pin)    IX_DIGITAL(pin)
#define IX_DIGITAL(pin)   (OFFSET_DIGITAL + (pin) - 1)
#define IX_INTERRUPT(pin) (OFFSET_INTERRUPT + (pin) - 1)
#define IX_MOTOR(n)       (OFFSET_MOTOR + (n) - 1)
#define IX_OI(oi, group)  (OFFSET_OI + (((oi) - 1) * CT_IOx) + (group) - 1)
#define IX_OI_BUTTON(io, group, dir) \
	(OFFSET_OI_BUTTON + (((oi) - 1 ) * CT_IOx_BUTTON) + \
	 (((group) - 1) * CT_OI_GROUP_SZ) + (dir))

/* Joysticks */
#define JOY_L_X(oi) IX_OI(oi, 0)
#define JOY_L_Y(oi) IX_OI(oi, 1)
#define JOY_R_X(oi) IX_OI(oi, 2)
#define JOY_R_Y(oi) IX_OI(oi, 3)

#endif
