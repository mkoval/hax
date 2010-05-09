# HAX #

## Getting Started ##

1. Things you need
    * Some type of POSIX-y environment, we developed this using 
      Cygwin (on Windows), MacOSX, and Linux.
    * A build environment. 
        * For 'vex_pic', this means installing
          mcc18-2.4 (subject to change) and the wrapper
          scripts (http://github.com/jmesmon/mcc18-wrapper).
        * For 'vex_cortex' you will need binutils, gcc, and
          newlib for the arm-none-eabi architecture (arm-elf
          may also work, but is untested)

2. Look at the 'skel' directory and the (forthcoming) 'test' directory
   to get an idea of how the functions work, etc.

3. For everything hax defines, look at 'hax.h'.

## Other Stuff ##

### Included User Code ###
* 'comp2010' is code for both RU Vex 2010 
* 'skel' shows the minimal amount of implimentation neccisarry.
* 'test' is the code we are using to test HAX

### Archs ###
* 'vex_pic' is the IFI setup of a pair of 18F8520 processors, one of which we
	have easy access to.
* 'vex_cortex' is the new Vex 2.0 processor released as a beta test to college
	teams in 2010. Also the excuse for this entire project.

### Unimplemented or Untested Features ###
* A generic timer/rtc API
