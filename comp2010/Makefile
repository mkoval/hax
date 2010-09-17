arch     = cortex
prog     = comp2010
ifndef ROBOT
	ROBOT = KEVIN
endif

SOURCE = auton.c   \
	encoder.c  \
	robot.c    \
	user.c     \
	state.c    \
	ru_ir.c    \
	main.c

CFLAGS = -DROBOT_$(ROBOT)

include ../build.mk
