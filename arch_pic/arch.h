#ifndef ARCH_PIC_H_
#define ARCH_PIC_H_

/* Slow loop of 18.5 milliseconds (converted to microseconds). */
#define SLOW_US 18500

#define JOY1_L_X PIN_OI(0)
#define JOY1_L_Y PIN_OI(1)
#define JOY1_R_X PIN_OI(2)
#define JOY1_R_Y PIN_OI(3)

#define JOY2_L_X PIN_OI(8)
#define JOY2_L_Y PIN_OI(9)
#define JOY2_R_X PIN_OI(10)
#define JOY2_R_Y PIN_OI(11)

/* Offset for each block of contiguous pins. */
#define OFFSET_DIGITAL      0
#define OFFSET_INTERRUPT   16
#define OFFSET_MOTOR       22
#define OFFSET_OI          41

/* Utility macros for generating internal pin indexes. */
#define PIN_DIGITAL(_x_)   ((_x_) + OFFSET_DIGITAL)
#define PIN_INTERRUPT(_x_) ((_x_) + OFFSET_INTERRUPT)
#define PIN_MOTOR(_x_)     ((_x_) + OFFSET_MOTOR)
#define PIN_OI(_x_)        ((_x_) + OFFSET_OI)

#endif
