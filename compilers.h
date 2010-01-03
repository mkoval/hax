#ifndef COMPILERS_H_
#define COMPILERS_H_

/* MCC18 does not support a __noreturn flag as per Page 104 of the MCC18
 * User Guide.
 */
#if defined(__18CXX)

#define __noreturn 

#else

#error "Unsupported Compiler"

#endif

#endif
