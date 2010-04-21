#ifndef USER_COMP2010_H_
#define USER_COMP2010_H_

typedef enum {
	CAL_MODE_NONE = 0,
	CAL_MODE_DRIVE,
	CAL_MODE_TURN,
	CAL_MODE_PRINT
} CalibrationMode;

/* Avoid including string.h since it causes the linker to fail. */
void memset(void *, int, size_t);

#endif
