#ifndef ARCH_CHECK_H_
#define ARCH_CHECK_H_

/* misc defines */
#ifndef SLOW_US
# error "ARCH: missing SLOW_US"
#endif

/* IX defines */
#ifndef IX_ANALOG
# error "ARCH: missing IX_ANALOG"
#endif

#ifndef IX_DIGITAL
# error "ARCH: missing IX_DIGITAL"
#endif

#ifndef IX_INTERRUPT
# error "ARCH: missing IX_INTERRUPT"
#endif

#ifndef IX_MOTOR
# error "ARCH: missing IX_MOTOR"
#endif

#ifndef IX_OI_GROUP
# error "ARCH: missing IX_OI_GROUP"
#endif

#ifndef IX_OI_BUTTON
# error "ARCH: missing IX_OI_BUTTON"
#endif

/* OI defines */
#ifndef OI_JOY_L_X
# error "ARCH: missing OI_JOY_L_X"
#endif

#ifndef OI_JOY_L_Y
# error "ARCH: missing OI_JOY_L_Y"
#endif

#ifndef OI_JOY_R_X
# error "ARCH: missing OI_JOY_R_X"
#endif

#ifndef OI_JOY_R_Y
# error "ARCH: missing OI_JOY_R_Y"
#endif

#ifndef OI_ROCKER_L
# error "ARCH: missing OI_ROCKER_L"
#endif

#ifndef OI_ROCKER_R
# error "ARCH: missing OI_ROCKER_R"
#endif

#ifndef OI_TRIGGER_L
# error "ARCH: missing OI_TRIGGER_L"
#endif

#ifndef OI_TRIGGER_R
# error "ARCH: missing OI_TRIGGER_R"
#endif

#endif /* ARCH_CHECH_H_ */
