#include "../hax.h"

#include "vexlib/API.h"

void IO_Initialization(void) {
    /* Enable VexNET control in the EasyC library. */
	SetCompetitionMode(-1, -1);

    /* ??? */
	DefineControllerIO(1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0);
}

void setup_1(void) {
    /* Handled by the EasyC library. */
}

void setup_2(void) {
    /* Handled by the EasyC library. */
}

void spin(void) {
}

void loop_1(void) {
    /* Handled by the EasyC library. */
}

void loop_2(void) {
    /* Handled by the EasyC library. */
}

CtrlMode mode_get(void) {
	return 0;
}

bool new_data_received(void) {
	return false;
}
