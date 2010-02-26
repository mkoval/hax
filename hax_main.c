#include "hax.h"
#include "compilers.h"

void main() {
	static CtrlMode mode;
	
	setup_1();
	init();
	setup_2();
	
	for(;;) {
		mode = mode_get();
		
		/* The "slow loop", executes once every kSlowSpeed microseconds. */
		if (new_data_received()) {
			loop_1();
			
			if (mode == kAuton) {
				auton_loop();
			} else if (mode == kTelop) {
				telop_loop();
			} else if (mode == kDisable) {
				disable_loop();
			}
			
			loop_2();
		}
		
		/* Executes as fast as the hardware allows. */
		spin();

		if (mode == kAuton) {
			auton_spin();
		} else if (mode == kTelop) {
			telop_spin();
		} else if (mode == kDisable) {
			disable_spin();
		}
	}
}
