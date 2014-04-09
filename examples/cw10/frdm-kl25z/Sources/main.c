/*
 * main implementation: use this 'C' sample to create your own application
 *
 */





#include "derivative.h" /* include peripheral declarations */

// krok 1
#include "ucp_app.h"

// cesta do \support je v include dirs. Pak jsou podslozky podle platformy.
#include "kinetis\uart.h"

void delay(void);
void clock_init(void);
void sci_init(void);
void sci_puts(const char* str);

// from mcg.c
int pll_init(int crystal_val, unsigned char hgo_val, unsigned char erefs_val, signed char prdiv_val, signed char vdiv_val, unsigned char mcgout_select);

// jd: from sysinit.c
/* System clock frequency variables - Represents the current system clock
 * settings
 */
int mcg_clk_hz;
int mcg_clk_khz;
int core_clk_khz;
int periph_clk_khz;
int pll_clk_khz;
int uart0_clk_khz;
uint32 uart0_clk_hz;

// jd: z mcg.h
// Constants for use in pll_init
#define NO_OSCINIT 0
#define OSCINIT 1

#define OSC_0 0
#define OSC_1 1

#define LOW_POWER 0
#define HIGH_GAIN 1

#define CANNED_OSC  0
#define CRYSTAL 1

#define PLL_0 0
#define PLL_1 1

#define PLL_ONLY 0
#define MCGOUT 1


/*
* Input Clock Info
*/
#define CLK0_FREQ_HZ        8000000
#define CLK0_TYPE           CRYSTAL


/*
 * PLL Configuration Info
 */
//#define NO_PLL_INIT  // define this to skip PLL initilaization and stay in default FEI mode

/* The expected PLL output frequency is:
 * PLL out = (((CLKIN/PRDIV) x VDIV) / 2)
 * where the CLKIN is CLK0_FREQ_HZ.
 *
 * For more info on PLL initialization refer to the mcg driver files.
 */

#define PLL0_PRDIV      4       // divider eference by 4 = 2 MHz
#define PLL0_VDIV       24      // multiply reference by 24 = 48 MHz



/* =============================== */
int main(void)
{
	int counter = 0;
	
	// jd: port clock musi byt enabled
	/* Enable all of the port clocks. These have to be enabled to configure
	* pin muxing options, so most code will need all of these on anyway.
	*/
	SIM_SCGC5 |= (SIM_SCGC5_PORTA_MASK
			| SIM_SCGC5_PORTB_MASK
		| SIM_SCGC5_PORTC_MASK
		| SIM_SCGC5_PORTD_MASK
		| SIM_SCGC5_PORTE_MASK );
		        
	/* Enable the pins for the selected UART */
	/* Enable the UART_TXD function on PTA1 */
	PORTA_PCR1 = PORT_PCR_MUX(0x2);		                  
	/* Enable the UART_TXD function on PTA2 */
	PORTA_PCR2 = PORT_PCR_MUX(0x2);
	// jd: toto je potreba jinak clock pro uart0 je disabled by default.
	SIM_SOPT2 |= SIM_SOPT2_UART0SRC(1); // select the PLLFLLCLK as UART0 clock source
	
	//clock_init();		/* set clock to 48 MHz */
	// init SCI
	sci_init();
	
	// krok 2
	ucp_app_init();		
	
	for(;;) {	   
	   	//counter++;
	   	//if ( counter == 0 )
	   	//	ucp_app_on_sample();	// krok 3  (TODO: casovani)
	   	sci_puts("ahoj ");
	   	delay();
	   	delay();
	}
	
	return 0;
}

void delay(void)
{
	uint32_t i, counter;
	for (i = 0; i < 200000; i++ )
			counter++;
}


void sci_init(void)
{
	//uart0_init (UART0_BASE_PTR, uart0_clk_khz, TERMINAL_BAUD);
	uart_init(BD_115200);
	
	// jd: from sysinit.c 
	/* UART0 is clocked asynchronously to the core clock, but all other UARTs are
	         * clocked from the peripheral clock. So we have to determine which clock
	         * to send to the UART_init function.
	         */
	/*        
	if (TERM_PORT_NUM == 0)
	            uart0_init (UART0_BASE_PTR, uart0_clk_khz, TERMINAL_BAUD);
	        else if (TERM_PORT_NUM == 1)
	  	    uart_init (UART1_BASE_PTR, periph_clk_khz, TERMINAL_BAUD);
	        else if (TERM_PORT_NUM == 2)
	            uart_init (UART2_BASE_PTR, periph_clk_khz, TERMINAL_BAUD);
	        else
	          while(1);
	          */
}

void sci_puts(const char* str)
{
	while ( *str )
	{
		uart0_putchar (UART0_BASE_PTR, *str);
		str++;
	}
}

// jd: ze sample code pro FRDM-KL25Z, ze sysinit.c
// Podle me nastavuje CPU na 48 MHZ (crystal = 8 MHz / 4 * pll (24) = 48.
// UART0 freq je polovina PLL freq > 24 MHz.

void clock_init(void)
{
	/* Enable all of the port clocks. These have to be enabled to configure
	         * pin muxing options, so most code will need all of these on anyway.
	         */
	        SIM_SCGC5 |= (SIM_SCGC5_PORTA_MASK
	                      | SIM_SCGC5_PORTB_MASK
	                      | SIM_SCGC5_PORTC_MASK
	                      | SIM_SCGC5_PORTD_MASK
	                      | SIM_SCGC5_PORTE_MASK );
	        
	        // Release hold with ACKISO:  Only has an effect if recovering from VLLS1, VLLS2, or VLLS3
	        // if ACKISO is set you must clear ackiso before calling pll_init 
	        //    or pll init hangs waiting for OSC to initialize
	        // if osc enabled in low power modes - enable it first before ack
	        // if I/O needs to be maintained without glitches enable outputs and modules first before ack.
	        if (PMC_REGSC &  PMC_REGSC_ACKISO_MASK)
	        	PMC_REGSC |= PMC_REGSC_ACKISO_MASK;
	        
	        /* Ramp up the system clock */
	               /* Set the system dividers */
	               /* NOTE: The PLL init will not configure the system clock dividers,
	                * so they must be configured appropriately before calling the PLL
	                * init function to ensure that clocks remain in valid ranges.
	                */  
	        SIM_CLKDIV1 = ( 0
	                                | SIM_CLKDIV1_OUTDIV1(0)
	                                | SIM_CLKDIV1_OUTDIV4(1) );
	        /* Initialize PLL */
	        /* PLL will be the source for MCG CLKOUT so the core, system, and flash clocks are derived from it */ 
	        mcg_clk_hz = pll_init(CLK0_FREQ_HZ,  /* CLKIN0 frequency */
	                                     LOW_POWER,     /* Set the oscillator for low power mode */
	                                     CLK0_TYPE,     /* Crystal or canned oscillator clock input */
	                                     PLL0_PRDIV,    /* PLL predivider value */
	                                     PLL0_VDIV,     /* PLL multiplier */
	                                     MCGOUT);       /* Use the output from this PLL as the MCGOUT */
	        
	        /* Check the value returned from pll_init() to make sure there wasn't an error */
	               if (mcg_clk_hz < 0x100)
	                 while(1);
	               
	               SIM_SOPT2 |= SIM_SOPT2_PLLFLLSEL_MASK; // set PLLFLLSEL to select the PLL for this clock source
	               
	               uart0_clk_khz = ((mcg_clk_hz / 2) / 1000); // UART0 clock frequency will equal half the PLL frequency
	      
	        	/*
	                 * Use the value obtained from the pll_init function to define variables
	        	 * for the core clock in kHz and also the peripheral clock. These
	        	 * variables can be used by other functions that need awareness of the
	        	 * system frequency.
	        	 */
	            mcg_clk_khz = mcg_clk_hz / 1000;
	          	core_clk_khz = mcg_clk_khz / (((SIM_CLKDIV1 & SIM_CLKDIV1_OUTDIV1_MASK) >> 28)+ 1);
	            periph_clk_khz = core_clk_khz / (((SIM_CLKDIV1 & SIM_CLKDIV1_OUTDIV4_MASK) >> 16)+ 1);
	                 
	                
	            if (TERM_PORT_NUM == 0)
	            {
	          	  /* Enable the pins for the selected UART */

	                  /* Enable the UART_TXD function on PTA1 */
	              PORTA_PCR1 = PORT_PCR_MUX(0x2);
	                  
	                  /* Enable the UART_TXD function on PTA2 */
	              PORTA_PCR2 = PORT_PCR_MUX(0x2);
	                
	              SIM_SOPT2 |= SIM_SOPT2_UART0SRC(1); // select the PLLFLLCLK as UART0 clock source
	            }
	          	if (TERM_PORT_NUM == 1)
	          	{
	                        /* Enable the UART_TXD function on PTC4 */
	          		PORTC_PCR4 = PORT_PCR_MUX(0x3); // UART1 is alt3 function for this pin
	          		
	          		/* Enable the UART_RXD function on PTC3 */
	          		PORTC_PCR3 = PORT_PCR_MUX(0x3); // UART1 is alt3 function for this pin
	          	}
	                
	            if (TERM_PORT_NUM == 2)
	          	{
	                         /* Enable the UART_TXD function on PTD3 */
	          		PORTD_PCR3 = PORT_PCR_MUX(0x3); // UART2 is alt3 function for this pin
	          		
	          		/* Enable the UART_RXD function on PTD2 */
	          		PORTD_PCR2 = PORT_PCR_MUX(0x3); // UART2 is alt3 function for this pin
	          	}
	                	          
}

/*********************************************************************************************/
/* Functon name : pll_init
 *
 * Mode transition: Option to move from FEI to PEE mode or to just initialize the PLL
 *
 * This function initializess PLL0. Either OSC0 is selected for the
 * reference clock source. The oscillators can be configured to use a crystal or take in an
 * external square wave clock.
 * Using the function parameter names the PLL frequency is calculated as follows:
 * PLL freq = ((crystal_val / prdiv_val) * vdiv_val)
 * Refer to the readme file in the mcg driver directory for examples of pll_init configurations.
 * All parameters must be provided, for example crystal_val must be provided even if the
 * oscillator associated with that parameter is already initialized.
 * The various passed parameters are checked to ensure they are within the allowed range. If any
 * of these checks fail the driver will exit and return a fail/error code. An error code will
 * also be returned if any error occurs during the PLL initialization sequence. Refer to the
 * readme file in the mcg driver directory for a list of all these codes.
 *
 * Parameters: crystal_val - external clock frequency in Hz either from a crystal or square
 *                           wave clock source
 *             hgo_val     - selects whether low power or high gain mode is selected
 *                           for the crystal oscillator. This has no meaning if an
 *                           external clock is used.
 *             erefs_val   - selects external clock (=0) or crystal osc (=1)
 *             prdiv_val   - value to divide the external clock source by to create the desired
 *                           PLL reference clock frequency
 *             vdiv_val    - value to multiply the PLL reference clock frequency by
 *             mcgout_select  - 0 if the PLL is just to be enabled, non-zero if the PLL is used
 *                              to provide the MCGOUT clock for the system.
 *
 * Return value : PLL frequency (Hz) or error code
 */

int pll_init(int crystal_val, unsigned char hgo_val, unsigned char erefs_val, signed char prdiv_val, signed char vdiv_val, unsigned char mcgout_select)
{
  unsigned char frdiv_val;
  unsigned char temp_reg;
  unsigned char prdiv, vdiv;
  short i;
  int ref_freq;
  int pll_freq;

  // check if in FEI mode
  if (!((((MCG_S & MCG_S_CLKST_MASK) >> MCG_S_CLKST_SHIFT) == 0x0) && // check CLKS mux has selcted FLL output
      (MCG_S & MCG_S_IREFST_MASK) &&                                  // check FLL ref is internal ref clk
      (!(MCG_S & MCG_S_PLLST_MASK))))                                 // check PLLS mux has selected FLL
  {
    return 0x1;                                                     // return error code
  }

  // check external frequency is less than the maximum frequency
  if  (crystal_val > 50000000) {return 0x21;}

  // check crystal frequency is within spec. if crystal osc is being used as PLL ref
  if (erefs_val)
  {
    if ((crystal_val < 3000000) || (crystal_val > 32000000)) {return 0x22;} // return 1 if one of the available crystal options is not available
  }

  // make sure HGO will never be greater than 1. Could return an error instead if desired.
  if (hgo_val > 0)
  {
    hgo_val = 1; // force hgo_val to 1 if > 0
  }

  // Check PLL divider settings are within spec.
  if ((prdiv_val < 1) || (prdiv_val > 25)) {return 0x41;}
  if ((vdiv_val < 24) || (vdiv_val > 50)) {return 0x42;}

  // Check PLL reference clock frequency is within spec.
  ref_freq = crystal_val / prdiv_val;
  if ((ref_freq < 2000000) || (ref_freq > 4000000)) {return 0x43;}

  // Check PLL output frequency is within spec.
  pll_freq = (crystal_val / prdiv_val) * vdiv_val;
  if ((pll_freq < 48000000) || (pll_freq > 100000000)) {return 0x45;}

  // configure the MCG_C2 register
  // the RANGE value is determined by the external frequency. Since the RANGE parameter affects the FRDIV divide value
  // it still needs to be set correctly even if the oscillator is not being used
      
  temp_reg = MCG_C2;
  temp_reg &= ~(MCG_C2_RANGE0_MASK | MCG_C2_HGO0_MASK | MCG_C2_EREFS0_MASK); // clear fields before writing new values
    
  if (crystal_val <= 8000000)
  {
    temp_reg |= (MCG_C2_RANGE0(1) | (hgo_val << MCG_C2_HGO0_SHIFT) | (erefs_val << MCG_C2_EREFS0_SHIFT));
  }
  else
  {
    temp_reg |= (MCG_C2_RANGE0(2) | (hgo_val << MCG_C2_HGO0_SHIFT) | (erefs_val << MCG_C2_EREFS0_SHIFT));
  }
  MCG_C2 = temp_reg;
  
  // determine FRDIV based on reference clock frequency
  // since the external frequency has already been checked only the maximum frequency for each FRDIV value needs to be compared here.
  if (crystal_val <= 1250000) {frdiv_val = 0;}
  else if (crystal_val <= 2500000) {frdiv_val = 1;}
  else if (crystal_val <= 5000000) {frdiv_val = 2;}
  else if (crystal_val <= 10000000) {frdiv_val = 3;}
  else if (crystal_val <= 20000000) {frdiv_val = 4;}
  else {frdiv_val = 5;}

  // Select external oscillator and Reference Divider and clear IREFS to start ext osc
  // If IRCLK is required it must be enabled outside of this driver, existing state will be maintained
  // CLKS=2, FRDIV=frdiv_val, IREFS=0, IRCLKEN=0, IREFSTEN=0
  temp_reg = MCG_C1;
  temp_reg &= ~(MCG_C1_CLKS_MASK | MCG_C1_FRDIV_MASK | MCG_C1_IREFS_MASK); // Clear values in these fields
  temp_reg |= (MCG_C1_CLKS(2) | MCG_C1_FRDIV(frdiv_val)); // Set the required CLKS and FRDIV values
  MCG_C1 = temp_reg;

  // if the external oscillator is used need to wait for OSCINIT to set
  if (erefs_val)
  {
    for (i = 0 ; i < 20000 ; i++)
    {
      if (MCG_S & MCG_S_OSCINIT0_MASK) break; // jump out early if OSCINIT sets before loop finishes
    }
  if (!(MCG_S & MCG_S_OSCINIT0_MASK)) return 0x23; // check bit is really set and return with error if not set
  }

  // wait for Reference clock Status bit to clear
  for (i = 0 ; i < 2000 ; i++)
  {
    if (!(MCG_S & MCG_S_IREFST_MASK)) break; // jump out early if IREFST clears before loop finishes
  }
  if (MCG_S & MCG_S_IREFST_MASK) return 0x11; // check bit is really clear and return with error if not set

  // Wait for clock status bits to show clock source is ext ref clk
  for (i = 0 ; i < 2000 ; i++)
  {
    if (((MCG_S & MCG_S_CLKST_MASK) >> MCG_S_CLKST_SHIFT) == 0x2) break; // jump out early if CLKST shows EXT CLK slected before loop finishes
  }
  if (((MCG_S & MCG_S_CLKST_MASK) >> MCG_S_CLKST_SHIFT) != 0x2) return 0x1A; // check EXT CLK is really selected and return with error if not

  // Now in FBE
  // It is recommended that the clock monitor is enabled when using an external clock as the clock source/reference.
  // It is enabled here but can be removed if this is not required.
  MCG_C6 |= MCG_C6_CME0_MASK;
  
  // Configure PLL
  // Configure MCG_C5
  // If the PLL is to run in STOP mode then the PLLSTEN bit needs to be OR'ed in here or in user code.
  temp_reg = MCG_C5;
  temp_reg &= ~MCG_C5_PRDIV0_MASK;
  temp_reg |= MCG_C5_PRDIV0(prdiv_val - 1);    //set PLL ref divider
  MCG_C5 = temp_reg;

  // Configure MCG_C6
  // The PLLS bit is set to enable the PLL, MCGOUT still sourced from ext ref clk
  // The loss of lock interrupt can be enabled by seperately OR'ing in the LOLIE bit in MCG_C6
  temp_reg = MCG_C6; // store present C6 value
  temp_reg &= ~MCG_C6_VDIV0_MASK; // clear VDIV settings
  temp_reg |= MCG_C6_PLLS_MASK | MCG_C6_VDIV0(vdiv_val - 24); // write new VDIV and enable PLL
  MCG_C6 = temp_reg; // update MCG_C6

  // wait for PLLST status bit to set
  for (i = 0 ; i < 2000 ; i++)
  {
    if (MCG_S & MCG_S_PLLST_MASK) break; // jump out early if PLLST sets before loop finishes
  }
  if (!(MCG_S & MCG_S_PLLST_MASK)) return 0x16; // check bit is really set and return with error if not set

  // Wait for LOCK bit to set
  for (i = 0 ; i < 4000 ; i++)
  {
    if (MCG_S & MCG_S_LOCK0_MASK) break; // jump out early if LOCK sets before loop finishes
  }
  if (!(MCG_S & MCG_S_LOCK0_MASK)) return 0x44; // check bit is really set and return with error if not set

  // Use actual PLL settings to calculate PLL frequency
  prdiv = ((MCG_C5 & MCG_C5_PRDIV0_MASK) + 1);
  vdiv = ((MCG_C6 & MCG_C6_VDIV0_MASK) + 24);

  // now in PBE

  MCG_C1 &= ~MCG_C1_CLKS_MASK; // clear CLKS to switch CLKS mux to select PLL as MCG_OUT

  // Wait for clock status bits to update
  for (i = 0 ; i < 2000 ; i++)
  {
    if (((MCG_S & MCG_S_CLKST_MASK) >> MCG_S_CLKST_SHIFT) == 0x3) break; // jump out early if CLKST = 3 before loop finishes
  }
  if (((MCG_S & MCG_S_CLKST_MASK) >> MCG_S_CLKST_SHIFT) != 0x3) return 0x1B; // check CLKST is set correctly and return with error if not

  // Now in PEE
  
  return ((crystal_val / prdiv) * vdiv); //MCGOUT equals PLL output frequency
} // pll_init
