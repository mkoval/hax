#ifndef COMPILERS_H_
#define COMPILERS_H_

/* MCC18 does not support a __noreturn flag as per Page 104 of the MCC18
 * User Guide.
 */
#if defined(__18CXX)
#define MCC18_24 1

#define __noreturn 
#define __rom	rom
#define __overlay overlay

/* GCC 4.3.2 does not seem to support ROM or overlay attributes. */
#elif defined(_STM32x_) && defined(__GNUC__)
#define GCC_ARM 1

#define __noreturn __attribute__((__noreturn__))
#define __rom
#define __overlay

#else

#error "Unsupported Compiler"

#endif

#endif /* COMPILERS_H_ */
