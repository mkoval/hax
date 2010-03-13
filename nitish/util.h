#ifndef UTIL_H_
#define UTIL_H_

#define ABS(x)    ((x) > 0 ? (x) : -(x))
#define CONSTRAIN (x, min, max) (((x) < (min)) ? (min) : (((x) > (max)) ? (max) : (x)))
#define MAX(x, y) (((x) > (y)) ? (x) : (y))
#define MIN(x, y) (((x) < (y)) ? (x) : (y))
#define SIGN(x)   (((x) < 0) ? -1 : +1)

#endif

