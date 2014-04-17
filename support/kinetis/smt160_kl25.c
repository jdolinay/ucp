/**
 * @file stm160_kl25.c
 * @brief driver for temperature sensor SMT 160-30 for Freedom board FRDM-KL25Z
 * 
 * @note
 * For input we can use pins:
 * PTA12 (TMP1_CH0) - digital pin 3 on Arduino
 * ** not used - PTA13 (TMP1_CH1) - digital pin 8 on Arduino 
 * 
 * Typical signal is 3 kHz (period is about 330 us) 
 * and the pulse is about 150 us for temperature 20 C
 * so there is about 50% duty. 
 * With CPU clock 20 MHz the timer tick is 0.05 us, max value is
 * reached in about 3.3 ms and we get value about 3000 for pulse
 * and 6000 for period. This seems OK. 
 * With max CPU 40 MHz the values would be 6000 and 12000, still OK.
 * For e.g. 5 MHz CPU the tick would be 0.2 us and we get 750 for
 * pulse - still ok.
 *  
 * 
*/

#include "derivative.h"	// jd
#include "smt160_kl25.h"

// TODO: move to generic header!
/*!< Macro to enable all interrupts. */
#define EnableInterrupts __asm ("CPSIE  i")

/*!< Macro to disable all interrupts. */
#define DisableInterrupts __asm ("CPSID  i")


/* Maximum values for signal period - for sanity check 
  the freq. is between 1 kHz and 4 kHz, so the period is
  between 1 ms and 250 us. 
  We should not set strict limits if we do not know the CPU speed
  but it is reasonable to assume the timer clock is may 40 Mhz and
  min 1 MHz (1 us to 0.025 us per timer tick 
  Min period: 250 us --> SMT160_MIN_PERIOD = 250 for 1 us tick 
  Max period: 1 ms --> SMT160_MAX_PERIOD = 40000 */
#define		SMT160_MAX_PERIOD	(40000)
#define		SMT160_MIN_PERIOD	(250)

/* how many samples (periods) we sum before computing average */
#define		SMT160_SUM_NUMBER	(20)

volatile uint16_t gsmt_start;	/* time of pulse start */
volatile uint16_t gsmt_pulse;	/* averaged pulse length in ticks */
volatile uint16_t gsmt_period;	/* averaged signal period */

volatile uint16_t gsmt_tmp_pulse;	/* pulse length in ticks */
volatile uint32_t gsmt_pulses;	/* sum of pulses */
volatile uint32_t gsmt_period_sum;	/* sum of periods */
volatile uint32_t gsmt_sumcnt;		/* sum counter */



/** init the timer TPM1 for measuring temperature using input capture
 * 
 */
void smt160_init()
{
	int prio = 2;	// priorita interruptu; 0 az 3, 0 = max
	
	/* Initialize our global variables */
	gsmt_start = 0;
	gsmt_pulse = 0;
	gsmt_period = 0;
	gsmt_tmp_pulse = 0;
	gsmt_pulses = 0;
	gsmt_period_sum = 0;	
	gsmt_sumcnt = 0;
	
	/* Enable Port A clock (pins used for input capture) */
	SIM_SCGC5 |= SIM_SCGC5_PORTA_MASK;
	
	/* enable input pins for TPM1 channels 0 and 1 
	  The timer channel is Alt 3 function of the pins */
	PORTA_PCR12 = PORT_PCR_MUX(3);		/* PTA12 = channel 0 for TPM1 */
	//PORTA_PCR13 = PORT_PCR_MUX(3);
	
	/* Enable clock for timer TMP1 */
	SIM_SCGC6 |= SIM_SCGC6_TPM1_MASK;
	/* and clock source */
	SIM_SOPT2 |= SIM_SOPT2_TPMSRC(1); // select the PLLFLLCLK as TPM clock source
	
	/*  Set timer to input capture mode etc. */
	TPM1_SC = TPM_SC_CMOD(1) | TPM_SC_PS(0);	//0x08;	// CMOD = 01, prescaler = 1
	
	/* test TOF interrupt
	TPM1_SC |= TPM_SC_TOIE_MASK;
	TPM1_MOD = 10000;
	*/

#if 0
	// Input capture config
	// Version with 2 input pins used: channel 0 detects pulses, channel 1
	// detects period.
	TPM1_C0SC = 0;	/* clear any pending interrupt and set all values to default */
	TPM1_C0SC |= TPM_CnSC_ELSA_MASK | TPM_CnSC_ELSB_MASK;	/* input capture on falling and rising edges */
	TPM1_C0SC |= TPM_CnSC_CHIE_MASK;	/* enable channel interrupt */


	TPM1_C1SC = 0;	/* clear any pending interrupt and set all values to default */
	TPM1_C1SC |= TPM_CnSC_ELSA_MASK;	/* input capture on rising edges only */
	TPM1_C1SC |= TPM_CnSC_CHIE_MASK;	/* enable channel interrupt */
#endif
	
	// Input capture config
	// Version with 1 pin used; wait for rising edge
	TPM1_C0SC = 0;	/* clear any pending interrupt and set all values to default */
	TPM1_C0SC |= TPM_CnSC_ELSA_MASK;	/* input capture on rising edge */
	TPM1_C0SC |= TPM_CnSC_CHIE_MASK;	/* enable channel interrupt */
	
	// Enable the interrupt
	// Note: sample code package provides function for this in arm_cm0.c; (enable_irq(interrupt);)
	// CMSIS compatible environment should implement NVIC_EnableIRQ((IRQn_Type)interrupt);
	// In NVIC there are 32-bit registers where each bit corresponds to 1 interrupt
	// The number of the interrupt used in NVIC is "non-core" int number, which is the 
	// absolute number - 16. (there are 16 core interrupts which are not controlled by NVIC)
	// ICPR register = clear pending flag (writing 1 clears pending interrupt)
	// ISER register = enable interrupt by writing 1 to its bit
	NVIC_ICPR |= (1 << (INT_TPM1 - 16) );	// clear possibly pending interrupt
	NVIC_ISER |= (1 << (INT_TPM1 - 16) );	// enable the interrupt
	
	// Set priority for the interrupt
	// Bit field starting location = 8 * (IRQ mod 4) + 6 = 
	// Zde IRQ = 18 (cislo TPM1 int v MKL25Z4.h - 16)
	// start = 8 * 18 mod 4 + 6 = 8 * 2 + 6 = 22
	NVIC_IPR4 = ((prio & 0x03) << 22);
	
	EnableInterrupts;
	
}

/** Get current temperature in C x 100
 *  For example: temperature 22.5 C is returned as 2250.
 *  @returns temperature in C x 100 (22.5 C is returned as 2250)
 */
uint16_t smt160_get_temp()
{
	uint32_t pulse, period, duty ;
	
	/* Get the values with disabled timer interrupt */
	TPM1_C0SC &= ~TPM_CnSC_CHIE_MASK;	/* disable interrupt */
	pulse = gsmt_pulse;
	period = gsmt_period;
	
	/* We might have missed some pulses, restart the measuring to state of waiting for 
	  new pulse */
	gsmt_start = 0;	
	gsmt_tmp_pulse = 0;
	TPM1_C0SC |= TPM_CnSC_CHIE_MASK;	/* enable interrupt again */
	
	/* Compute temperature */
	/* DC = 0.32 + 0.0047 x T 
	  -->  T = (DC - 0.32) / 0.0047 (with DC 0.5 is 50%)
	  T =  DC_PERCENT / 0.47 - 68.0851	
	  100T = 100* (DC_PERCENT / 0.47) - 6809
	  100T = 100* (DC_PERCENT*100 / 47) - 6809 
	  100T = 10000 * DC_PERCENT / 47 - 6809 
	  
	  Example: DC = 0.32 which is T = 0C
	  100T = 100 * ((32*100) / 47) - 6809 =  0
	  DC = 0,93 which is T = 130 °C
	  100T = 100 * ((93*100)/47) - 6909 = 12978 which is 129.78 °C :)
	 */
	if ( pulse < period )
	{		
		duty = (pulse * 100) / period;
		
		/* using pulse to store temperature */
		pulse = 10000 * duty / 47 - 6809;		
	}
	else
		pulse = 0;	/* invalid pulse or period */
	
	return (uint16_t)pulse;
}

/* TMP1 interrupt handler
 * We need to read TPM registers to find out which TMP1 interrupt this is;
 *  
 */
void FTM1_IRQHandler()
{
	volatile uint16_t tmp;
	
	// Channel interrupt?
	if ( (TPM1_C0SC & TPM_CnSC_CHF_MASK) != 0 )
	{
		// channel 0 interrupt occurred
		TPM1_C0SC |= TPM_CnSC_CHF_MASK;		// clear the flag
		
		// is it rising or falling edge?
		// ELSB == 0 means falling
		if ( (TPM1_C1SC & TPM_CnSC_ELSB_MASK) == 0 )
		{
			/* RISING edge detected */
			
			if ( gsmt_start == 0 )
			{
				/* start of pulse - first after our init  */
				gsmt_start = TPM_CnV_VAL(TPM1_C0V);				
			}
			else
			{
				/* end of period */
				tmp = TPM_CnV_VAL(TPM1_C0V);
				if ( tmp > gsmt_start )
					tmp = tmp - gsmt_start;
				else	/* overflow of counter while measuring */
					tmp = (0xffff - gsmt_start) + tmp;
				
				//gsmt_period = tmp;	/* save period */
				/* save to sums if valid */
				if ( tmp > SMT160_MIN_PERIOD && tmp < SMT160_MAX_PERIOD
					&& gsmt_tmp_pulse > SMT160_MIN_PERIOD && gsmt_tmp_pulse < SMT160_MAX_PERIOD )
				{
					// pokusne vyrazeno prumerovani...
					gsmt_period = tmp;
					gsmt_pulse = gsmt_tmp_pulse;
					/*
					gsmt_period_sum += tmp;
					gsmt_pulses += gsmt_tmp_pulse;
					gsmt_sumcnt++;
					if ( gsmt_sumcnt >= SMT160_SUM_NUMBER )
					{
						gsmt_period = gsmt_period_sum / gsmt_sumcnt;
						gsmt_pulse = gsmt_pulses / gsmt_sumcnt;
						gsmt_sumcnt = 0;
						gsmt_period_sum = 0;
						gsmt_pulses = 0;
					}
					*/
				}
				
				/* end of period means also start of a new period */
				gsmt_start = TPM_CnV_VAL(TPM1_C0V);			
			}
			
			/* switch to falling edge  > wait for end of pulse */
			TPM1_C1SC &= ~TPM_CnSC_ELSA_MASK;
			TPM1_C1SC |= TPM_CnSC_ELSB_MASK;	
		}
		else
		{
			/* falling edge detected */
			/* = end of pulse. Wait for rising edge to measure the period */
			if ( gsmt_start != 0 )
			{
				/* end of pulse */
				tmp = TPM_CnV_VAL(TPM1_C0V);
				if ( tmp > gsmt_start )
					tmp = tmp - gsmt_start;
				else	/* overflow of counter while measuring */
					tmp = (0xffff - gsmt_start) + tmp;
				gsmt_tmp_pulse = tmp;
				//gsmt_period = 0;	/* invalidate the period */				
			}
			
			/* switch to rising edge  > wait for start of next pulse which is
			  also end of this period */
			TPM1_C1SC &= ~TPM_CnSC_ELSB_MASK;
			TPM1_C1SC |= TPM_CnSC_ELSA_MASK;
		}
		
	}
	
	/* Chanel 1 input capture interrupt
	if ( (TPM1_C1SC & TPM_CnSC_CHF_MASK) != 0 )
	{
		TPM1_C1SC |= TPM_CnSC_CHF_MASK;		
	}*/
	
	
	/* test for TOF
	if ( (TPM1_SC & TPM_SC_TOF_MASK) != 0 )
	{
		// TOF occurred
		TPM1_SC |= TPM_SC_TOF_MASK;	// clear TOF by writing 1
	}*/		
}

