#ifndef ARCH_PIC_H_
#define ARCH_PIC_H_

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
