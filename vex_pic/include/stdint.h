#ifndef STDINT_H_
#define STDINT_H_

#include "compilers.h"

/* As defined in Section 2.1, Page 11 of the MCC18 C18 User Guide */
#if defined(MCC18_24)
typedef signed   char int8_t;
typedef unsigned char uint8_t;

typedef signed   int int16_t;
typedef unsigned int uint16_t;

typedef signed   short long int24_t;
typedef unsigned short long uint24_t;

typedef signed   long int32_t;
typedef unsigned long uint32_t;

#else

#error "MCC18 STDINT failed"

#endif

#endif
