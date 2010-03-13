#ifndef STDINT_H_
#define STDINT_H_

/* As defined in Section 2.1, Page 11 of the MCC18 C18 User Guide */
#if defined(__18CXX)
typedef signed   char int8_t;
typedef unsigned char uint8_t;

typedef signed   int int16_t;
typedef unsigned int uint16_t;

typedef signed   long int32_t;
typedef unsigned long uint32_t;

#elif defined(__ARM_EABI__) && defined(__GNUC__)

#include <stdint.h>

#endif

#endif
