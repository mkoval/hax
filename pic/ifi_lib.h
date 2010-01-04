
/**
 ** IFI LIBRARY CODE
 ** methods defined in ifi_library.lib
 **/
 
extern TxData txdata;
extern RxData rxdata;
extern Packed statusflag;
 
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
