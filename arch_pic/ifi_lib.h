
/**
 ** IFI LIBRARY CODE
 ** methods defined in ifi_library.lib
 **/
 
#include "master.h"
 
typedef union {
	struct {
		uint8_t NEW_SPI_DATA:1;
		uint8_t TX_UPDATED:1;
		uint8_t FIRST_TIME:1;
		uint8_t TX_BUFFSELECT:1;
		uint8_t RX_BUFFSELECT:1;
		uint8_t SPI_SEMAPHORE:1;
		uint8_t unknown:2;
	} b;
	uint8_t a;
} StatusFlags;


/* These need to be defined somewhere in our code.
 */
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

/* When any of the pwms 1 to 4 are controlled by the user proc, set whether 
 * Generate_Pwms generates their outputs.
 */
#define IFI_PWM 0
#define USER_CCP 1
void Setup_PWM_Output_Type(int pwm1, int pwm2, int pwm3, int pwm4);

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
