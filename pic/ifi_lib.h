
/**
 ** IFI LIBRARY CODE
 ** methods defined in ifi_library.lib
 **/
 
 
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
		uint8_t allbits; /* ??? */
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
	
	uint8_t warning_code;
	uint8_t reserved_2[4];
	uint8_t error_code;
	uint8_t packetnum;
	uint8_t current_mode;
	uint8_t control;
} TxData;
 
 typedef struct {
	uint8_t NEW_SPI_DATA:1;
	uint8_t TX_UPDATED:1;
	uint8_t FIRST_TIME:1;
	uint8_t TX_BUFFSELECT:1;
	uint8_t RX_BUFFSELECT:1;
	uint8_t SPI_SEMAPHORE:1;
	uint8_t unknown:2;
} StatusFlags;
 
extern TxData txdata;
extern RxData rxdata;
extern StatusFlags statusflag;
 
/* Vector jumps to the appropriate high priority interrupt handler. Called
 * from the high priority interrupt vector.
 */
void InterruptHandlerHigh(void);


/* Configure registers and initializes the SPI RX/TX buffers. Called from the
 * setup() HAX function during robot initalization.
 */
void Initialize_Registers(void);
void IFI_Initialization(void);

/* Informs the master processor that all user initialization is complete. Must
 * be the last function call in setup().
 */
void User_Proc_Is_Ready(void);

/* Fill the transmit buffer with data in the supplied struct to send to the
 * master processor. Completes in 23 microseconds.
 */
void Putdata(TxData *);

/* Retreive data from the SPI receive buffer (from the master processor),
 * reading the results into the supplied structure. Completes in 14.8
 * microseconds.
 */
void Getdata(RxData *);

/* FIXME: Conflicting Documentation */
/* Sets the output type of PWM's 13, 14, 15, and 16. Each argument is either
 * IFI_PWM for a PWM output or USER_CCP for a timer.
 */
/* EIGHTH: Set your PWM output type.  Only applies if USER controls PWM 1, 2, 3, or 4. */
/*   Choose from these parameters for PWM 1-4 respectively:                          */
/*     IFI_PWM  - Standard IFI PWM output generated with Generate_Pwms(...)          */
/*     USER_CCP - User can use PWM pin as digital I/O or CCP pin.                    */
void Setup_PWM_Output_Type(int, int, int, int);

/* From ifi_utilities.h
 */
void Hex_output(unsigned char temp);
void Generate_Pwms(unsigned char pwm_1,unsigned char pwm_2,
                   unsigned char pwm_3,unsigned char pwm_4,
                   unsigned char pwm_5,unsigned char pwm_6,
                   unsigned char pwm_7,unsigned char pwm_8);
				   
/* Called in initialization to zero all memory
 */
extern void Clear_Memory(void);
