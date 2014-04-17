/*
 * @File        uart.c
 * @brief     Provide common uart routines for Kinetis MCU; for UART0 only!
 *
 * @Note  
 * Initialises pins Tx = PTA1, Rx = PTA2 and clock source for UART to PLLFLLCLK
 * 
 * The UART0 clock source is configured in SIM_SOPT2 register; by default
 * the CPU clock is 20.9 MHz and if PLL-FLL is selected as UART0 clock source, 
 * the UART clock will be also 20.9 MHz.
 * If PLL is used as CPU clock source, then the UART0 clock is PLL_clock/ 2 !
 * The UART0 clock source is set to PLL/FLL by this driver (this is one, the same option)
 * it depends on the user code what is set as CPU clock.
 *
 * Zaklad kodu je z FRDM-KL25 sample code package     
 *              
 */

//#include "common.h"
#include "derivative.h"	// jd
#include "uart.h"

#define LF 0x0A						// Line Feed ASCII code
#define CR 0x0D						// Carriage Return ASCII code

 
/********************************************************************/
/** Initialize UART0 with given baudrate
 * 
 * @param speed baudrate constant, e.g. BD_115200 
 *
 * @note Initialize the uart for 8N1 operation, interrupts disabled, and
 * no hardware flow-control
 *
 */
 uint8_t uart_init(UART_speed_t speed)
{
    uint32_t osr_val;
    uint32_t sbr_val;
    uint32_t reg_temp = 0;
        
    /* Enable clock for PORTA needed for Tx, Rx pins */
    SIM_SCGC5 |= SIM_SCGC5_PORTA_MASK;
    		
    /* Enable the UART_TXD function on PTA1 */
    PORTA_PCR1 = PORT_PCR_MUX(0x2);		                  
    /* Enable the UART_TXD function on PTA2 */
    PORTA_PCR2 = PORT_PCR_MUX(0x2);
    /* set clock for UART0 */
    SIM_SOPT2 |= SIM_SOPT2_UART0SRC(1); // select the PLLFLLCLK as UART0 clock source
    
    
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
    UART0_BDL = (uint8_t)(sbr_val & UART0_BDL_SBR_MASK);
        
    /* Enable receiver and transmitter */
    UART0_C2 |= (UART0_C2_TE_MASK | UART0_C2_RE_MASK );
    
    return 0;	/* OK*/
       
}
 
 /** Write single byte to SCI
  * @param byte of data to write
  **/
 void uart_write(uint8_t data)
 {
	 /* Wait until space is available in the FIFO */
	 while(!(UART0_S1_REG(UART0_BASE_PTR) & UART0_S1_TDRE_MASK));
	    
	 /* Send the character */
	 UART0_D_REG(UART0_BASE_PTR) = data;
 }
 
 /** Read byte form SCI; wait for data to arrive!
  * @return byte received from SCI.
  **/
 uint8_t uart_read(void)
 {
	 /* Wait until character has been received */
	 while (!(UART0_S1_REG(UART0_BASE_PTR) & UART0_S1_RDRF_MASK));
	     
	 /* Return the 8-bit data from the receiver */
	 return UART0_D_REG(UART0_BASE_PTR);
 }
 
 /** Check if data are available for reading
  * @return true if there are data 
 **/
 bool uart_data_available(void)
 {
	 return ((UART0_S1_REG(UART0_BASE_PTR) & UART0_S1_RDRF_MASK) != 0);
 }
 
 
 /** Read one character from SCI.
  * @return the character read.
  * @note blocks the caller until character is received! 
  **/
 char uart_getc()               
 {
     return (char)uart_read();   
 }
 
 /** Send one character to SCI. If the char is '\n', send CR + LF
  * @param char to send
  **/
 void uart_putchar(char c)        
 {
     if(c == '\n')
     {	 				
 		uart_write(CR);
 		uart_write(LF);
 	} 
     else 
     {
 		uart_write(c);					
 	}
 }

 /** Send null-terminated string to SCI. 
  * @param pointer to string to send
  * @note If the string contains '\n', CR + LF are sent. 
  **/
 void uart_puts(const char* str)     
 {
     while(*str) 
     {	
 		uart_putchar(*str);
 		str++;
 	}
 }


 /* Convenience global functions for output to UART */
 // TODO: should be somehow possible to define where the output goes ? - which UART...
 /** print line of text without new line
  */
 void uart_print(const char* str)    // print string
 {
     uart_puts(str);
 } 
 /** print line of text with new line
  */
 void uart_println(const char* str)   // print string with line end
 {
     uart_puts(str);
     uart_putchar('\n');
 }

 /** print string with one formatted 16-bit number 
 * @note max 8 chars for the resulting element in str!
 */
 void uart_printf16(const char* str, const char* format, uint16_t data)  
 {
     char buffer[9];
     sprintf(buffer, format, data);
     uart_puts(str);
     uart_putchar(' ');
     uart_puts(buffer);    
 }
 /** print string with one formatted 32-bit number 
 * @note max 8 chars for the resulting element in str!
 */
 void uart_printf32(const char* str, const char* format, uint32_t data)  
 {
     char buffer[12];
     sprintf(buffer, format, data);
     uart_puts(str);
     uart_putchar(' ');
     uart_puts(buffer);    
 } 
 
 void uart_printf_float(const char* str, const char* format, float data)
 {
     char buffer[12];
     sprintf(buffer, format, data);
     uart_puts(str);
     uart_putchar(' ');
     uart_puts(buffer);    
 }
 
 
 /* puvodni kod z FRDM--KL25Z sample code package */
#if 0 
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
#endif
