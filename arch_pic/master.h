#ifndef MASTER_H_
#define MASTER_H_

/*
 * Structures for comunication with the master proc
 */

#include <stdint.h>
 
/* WarningCode & ErrorCode are sent to the master processor in TxData */
typedef enum {
	/* as detected by SSPCON1.WCOL */
	kWarnSPIWriteCollision = 1,
	
	/* PORTB Pullups disabled or RB Port Chaange interrupt enable */
	kWarnPortB = 2,
	
	/* External Interupt 1 enabled */
	kWarnExInt1En = 3

	/* Also will contain data pretaining to ErrorCodes */
	
} WarningCode;
typedef enum {
	/* set if SPI configuration in SSPCON1 wrong.
	 * set if !(SSPEN == 1 && SSPM2 == 1 && SSPM0 == 1)
	 * warning code = SSPCON1
	 */
	kErrorSSPCON1 = 1,
	
	/* set if SPI hardware configured for I2C mode (some bit in SSPCON2 is set)
	 * warning code = SSPCON2 
	 */
	kErrorSSPCON2 = 2,
	
	/* set if (RCON.IPEN == 0). warn = RCON */
	kErrorIntPriorityDisabled = 3,
	
	/* SPI Interrupt dissabled. warn = PIE1 */
	kErrorSPIIntDisabled = 4,
	kError5 = 5,
	
	/* set if some interrupt other than INT0 is high priority
	 * warn = IPRx
	 */
	kErrorIPR1 = 6,
	kErrorIPR2 = 7,
	kErrorIPR3 = 8,
	
	/* set if INT0 is disabled. warn = INTCON */
	kErrorINTCON = 9,
	
	/* set if some interrupt is high priority or INT0 is not on the falling edge
	 * warn = INTCON2
	 */
	kErrorINTCON2 = 10,
	
	/* INT1 or INT2 high priority. warn = INTCON3 */
	kErrorINT12HighPriority = 11,
	kErrorPinA4NotOutput = 12, /* warn = TRISA */
	kErrorPinB0NotInput = 13, /* warn = TRISB */
	
	/* set if pC1 and pC5 are not outputs or pC2, pC3, and pC4 are not inputs
	 * warning code = TRISC
	 */
	kErrorTRISC = 14,
	
	/* set if PortF.pin7 is not an input
	 * warning code = TRISF
	 */
	kErrorPinF7NotInput = 15

} ErrorCode;
 
typedef struct {
	uint8_t unknown:6;
	uint8_t autonomous:1; /* Autonomous enable/disable flag. */
	uint8_t disabled:1;   /* Robot enable/disable flag. */
} RCModes;

/* This structure defines the contents of the data received from the master
 * processor.
 */
typedef struct {
	uint8_t packet_num;
	
	union {
		RCModes mode;
		uint8_t allbits;
	} rcmode;
	
	union {
		uint8_t a;
		struct {
			uint8_t oi_on:1;
			uint8_t reserved:7;
		} b;
	} rcstatusflag;
	
	uint8_t reserved_1[3];
	uint8_t oi_analog[16]; /* Inputs */
	uint8_t reserved_2[9];
	uint8_t master_version;
} RxData;

/* Indicates master control of a pwm when high */
typedef struct {
	uint8_t pwm1:1;
	uint8_t pwm2:1;
	uint8_t pwm3:1;
	uint8_t pwm4:1;
	uint8_t pwm5:1;
	uint8_t pwm6:1;
	uint8_t pwm7:1;
	uint8_t pwm8:1;
} PwmMasterCtrl;

/* This structure defines the contents of the data transmitted to the master  
 * processor.
 * Note: the use of typedefed enums WarningCode and ErrorCode fit into uint8_t
 *  due to mcc18 using the smallest type capable of representing all values in
 *  the enum.
 */
typedef struct {
	uint8_t reserved_1[4];
	uint8_t rc_pwm[16]; /* Outputs */
	
	/* "user_cmd |= 0x02" gives autonomous mode. */
	uint8_t user_cmd;   /* Reserved for future use. */
	uint8_t cmd_byte1;  /* Reserved for future use. */
	
	union {
		uint8_t a;
		PwmMasterCtrl b;
	} pwm_mask;
	
	union {
		uint8_t a;
		WarningCode e;
	} warning_code;
	uint8_t reserved_2[4];
	union {
		uint8_t a;
		ErrorCode e;
	} error_code;
	uint8_t packetnum;
	uint8_t current_mode;
	uint8_t control;
} TxData;

#endif /* MASTER_H_ */
