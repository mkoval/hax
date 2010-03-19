#ifndef BOOL_H_
#define BOOL_H_

#include "compilers.h"

#if defined(MCC18_24)
typedef unsigned char bool;
#define true 1
#define false 0

#else
#error "MCC18 STDBOOL failed"
#endif


#endif
