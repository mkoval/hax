#ifndef COMPILERS_H_
#define COMPILERS_H_

#if defined(__18CXX)
//FIXME: need to actually detect version.
# define MCC18 300
# define __noreturn
# define __rom	rom
# define __overlay overlay
#elif defined(__GNUC__)
# define GCC 1
# define __noreturn __attribute__((noreturn))
# define __rom
# define __overlay
#elif defined(SDCC)
# define __rom __code
# define __noreturn
# define __overlay
#else
# error "Unsupported Compiler"
#endif

#endif /* COMPILERS_H_ */
