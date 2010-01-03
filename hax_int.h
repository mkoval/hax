#ifndef HAXINT_H_
#define HAXINT_H_

/* As defined in Section 2.1, Page 11 of the MCC18 C18 User Guide */
#ifdef __18CXX
typedef char int8_t;
typedef unsigned char uint8_t;

typedef int int16_t;
typedef unsigned int uint16_t;

typedef long int32_t;
typedef unsigned long uint32_t;
#endif

#endif
