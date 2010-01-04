#include "hax.h"
#include "compilers.h"

void main() {
	static CtrlMode mode;
	
	setup_1();
	init();
	setup_2();
	
	for(;;) {
		mode = get_mode();
		
		/* The "slow loop", executes once every kSlowSpeed microseconds. */
		if (new_data_received()) {
			loop_1();
			
			if (mode == kAuton) {
				auton_loop();
			} else {
				telop_loop();
			}
			
			loop_2();
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
