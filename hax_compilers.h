#ifndef HAX_COMPILERS_H_
#define HAX_COMPILERS_H_

/* MCC18 does not support a __noreturn flag as per Page 104 of the MCC18
 * User Guide.
 */
#ifdef __18CXX
#define __noreturn 
#endif

#endif
