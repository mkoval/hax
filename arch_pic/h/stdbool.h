#ifndef BOOL_H_
#define BOOL_H_

#include "compilers.h"

#if defined(MCC18)
typedef unsigned char bool;
# define true 1
# define false 0
#elif defined(SDCC)
typedef unsigned char bool;
# define true 1
# define false 0
#else
# error "Alternate STDBOOL failed"
#endif

#endif
