# HAX #

## Getting Started ##

1. Things you need
    * Some type of POSIX-y environment, we developed this using 
      Cygwin (on Windows), MacOSX, and Linux.
    * A build environment. 
        * For 'pic', this means installing
          - \>=mcc18-3.35 and the wrapper scripts
	    (http://github.com/jmesmon/mcc18-wrapper) or
          - sdcc.
          - a bootloader communicator ("programmer")
            + rigel
            + ifi-picloader
            + roboctl
            + ifi loader
        * For 'cortex' you will need 
          - compiled for arm-none-eabi (may be possible to use arm-elf)
            + binutils, 
            + gcc,
            + and newlib
          - a "programmer" / chip flasher:
	    + stm32loader : included in ./arch_cortex/jtag/stm32lodaer.py 
		     (no, it isn`t jtag)
	    + stm32flash http://code.google.com/p/stm32flash/
	    + stm32ld https://github.com/jsnyder/stm32ld

2. Look at the 'skel' directory and the (forthcoming) 'test' directory
   to get an idea of how the functions work, etc. Files containing code
   specific to a particular architecture are placed in folder prefixed by 
   `arch_` followed by the arch name (presently, 'pic' or 'cortex').

3. For everything hax defines, look at 'hax.h'.

4. To build some user code, cd to a user program directory (for example,
  'skel') and type make.

## Other Stuff ##

### Included User Code ###
* 'comp2010' is code for both RU Vex 2010 
* 'skel' shows the minimal amount of implimentation neccisarry.
* 'test' is the code we are using to test HAX

### Archs ###
* 'arch_pic' is the IFI setup of a pair of 18F8520 processors, one of which we
	have easy access to.
* 'arch_cortex' is the new Vex 2.0 processor released as a beta test to college
	teams in 2010. Also the excuse for this entire project.

### Unimplemented or Untested Features ###
* A generic timer/rtc API
