/*
 * File:        uart.c
 * Purpose:     Provide common uart routines for serial IO
 *
 * Notes:  
 * The UART0 clock source is configured in SIM_SOPT2 register; by default
 * the CPU clock is 20.9 MHz and if PLL-FLL is selected as clock source, the
 * UART0 clock will be the same. (default is FLL mode, which feeds the same clock
 * to UART. If PLL mode is used, then the UART0 clock is PLL_clock/ 2 !
 *
 * Zaklad kodu je z FRDM-KL25 sample code package     
 *              
 */

//#include "common.h"
#include "derivative.h"	// jd
#include "uart.h"
#include <stdarg.h>

// TODO: resit povoleni pinu pro UART viz note u init nize
 
/********************************************************************/
/*
 * Initialize the uart for 8N1 operation, interrupts disabled, and
 * no hardware flow-control
 *
 * NOTE: Since the uarts are pinned out in multiple locations on most
 *       Kinetis devices, this driver does not enable uart pin functions.
 *       The desired pins should be enabled before calling this init function.
 *
 
 */
 uint8_t uart_init(UART_speed_t speed)
{
    uint32 osr_val;
    uint32 sbr_val;
    uint32 reg_temp = 0;
    
    
    osr_val = UART_GET_OSR(speed);
    sbr_val = UART_GET_BR(speed);
    
    SIM_SCGC4 |= SIM_SCGC4_UART0_MASK;
    
    // Disable UART0 before changing registers
    UART0_C2 &= ~(UART0_C2_TE_MASK | UART0_C2_RE_MASK);
      
    
    // If the OSR is between 4x and 8x then both
    // edge sampling MUST be turned on.  
    if ((osr_val >3) && (osr_val < 9))
    	UART0_C5|= UART0_C5_BOTHEDGE_MASK;
        
    // Setup OSR value 
    reg_temp = UART0_C4;
    reg_temp &= ~UART0_C4_OSR_MASK;
    reg_temp |= UART0_C4_OSR(osr_val-1);
    
    // Write reg_temp to C4 register
    UART0_C4 = reg_temp;
        
    //reg_temp = (reg_temp & UART0_C4_OSR_MASK) + 1;
        
    /* Save current value of uartx_BDH except for the SBR field */
    reg_temp = UART0_BDH & ~(UART0_BDH_SBR(0x1F));
   
    UART0_BDH = reg_temp |  UART0_BDH_SBR(((sbr_val & 0x1F00) >> 8));
    UART0_BDL = (uint8)(sbr_val & UART0_BDL_SBR_MASK);
        
    /* Enable receiver and transmitter */
    UART0_C2 |= (UART0_C2_TE_MASK | UART0_C2_RE_MASK );
    
    return 0;	/* OK*/
       
}
 
/********************************************************************/
/*
 * Wait for a character to be received on the specified uart
 *
 * Parameters:
 *  channel      uart channel to read from
 *
 * Return Values:
 *  the received character
 */
char uart0_getchar (UART0_MemMapPtr channel)
{
      /* Wait until character has been received */
      while (!(UART0_S1_REG(channel) & UART0_S1_RDRF_MASK));
    
      /* Return the 8-bit data from the receiver */
      return UART0_D_REG(channel);
}
/********************************************************************/
/*
 * Wait for space in the uart Tx FIFO and then send a character
 *
 * Parameters:
 *  channel      uart channel to send to
 *  ch			 character to send
 */ 
void uart0_putchar (UART0_MemMapPtr channel, char ch)
{
      /* Wait until space is available in the FIFO */
      while(!(UART0_S1_REG(channel) & UART0_S1_TDRE_MASK));
    
      /* Send the character */
      UART0_D_REG(channel) = (uint8)ch;
    
 }
/********************************************************************/
/*
 * Check to see if a character has been received
 *
 * Parameters:
 *  channel      uart channel to check for a character
 *
 * Return values:
 *  0       No character received
 *  1       Character has been received
 */
int uart0_getchar_present (UART0_MemMapPtr channel)
{
    return (UART0_S1_REG(channel) & UART0_S1_RDRF_MASK);
}
/********************************************************************/

