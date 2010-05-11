#include "hax.h"
#include "compilers.h"

void main(void) {
	static state_t mode;
	
	setup_1();
	init();
	setup_2();
	
	for(;;) {
		mode = mode_get();
		
		/* The "slow loop", executes once every kSlowSpeed microseconds. */
		if (new_data_received()) {
			loop_1();
			
			switch (mode) {
			case MODE_AUTON:
				auton_loop();
				break;

			case MODE_TELOP:
				telop_loop();
				break;

			default:
				disable_loop();
			}

			loop_2();
		}
		
		/* Executes as fast as the hardware allows. */
		spin();

		switch (mode) {
		case MODE_AUTON:
			auton_spin();
			break;

		case MODE_TELOP:
			telop_spin();
			break;

		default:
			disable_spin();
		}
	}
}
