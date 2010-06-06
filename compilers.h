#ifndef COMPILERS_H_
#define COMPILERS_H_

/* MCC18 does not support a __noreturn flag as per Page 104 of the MCC18
 * User Guide.
 */
#if defined(__18CXX)
  //FIXME: need to actually detect version.
  #define MCC18 240

  //XXX: Depricated. Version comes from value of MCC18 macro.
  #define MCC18_24 1

  #define __noreturn 
  #define __rom	rom
  #define __overlay overlay

#elif defined(__GNUC__)
  #define GCC 1

  //XXX: Depricated. ARCH and COMPILER are seperate.
  #define GCC_ARM 1

  #define __noreturn __attribute__((noreturn))
  #define __rom
  #define __overlay

#elif defined(SDCC)
  // SDCC is a nice define to check, lets use it.

  #define __rom __code
  #define __noreturn 
  #define __overlay

#else

 #error "Unsupported Compiler"

#endif

#endif /* COMPILERS_H_ */
