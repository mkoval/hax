#include "hax.h"
#include "compilers.h"

#include "user.h"

__noreturn void main(void) {
	static ctrl_mode_t mode;

	arch_init_1();
	init();
	arch_init_2();

	for(;;) {
		mode = ctrl_mode_get();

		/* The "slow loop", executes once every SLOW_US microseconds. */
		if (do_slow_loop()) {
			arch_loop_1();

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

			arch_loop_2();
		}

		/* Executes as fast as the hardware allows. */
		arch_spin();

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
