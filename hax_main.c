#include "hax.h"

#ifdef NOTDEF
void main(void) __noreturn;
void main() {
	Mode mode;
	
	setup();
	
	for(;;) {
		mode = get_mode();
		
		/* The "slow loop", executes once every kSlowSpeed milliseconds. */
		if (new_data_received()) {
			loop();
			
			if (mode == kAuton) {
				auton_loop();
			} else {
				telop_loop();
			}
		}
		
		/* Executes as fast as the hardware allows. */
		spin();
		if (mode == kAuton) {
			auton_spin();
		}
		else {
			telop_spin();
		}
	}
}
#endif