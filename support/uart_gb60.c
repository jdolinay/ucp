/****************************************************************************
 * @file     uart.c
 * @brief    UART functions of the framework for S08
 * @version  1
 * @date     07. Mar 2014
 *
 *  Vyextrahovano z MSF pro pouziti pri testech "regulators" na GB60
 ******************************************************************************/
   
#include <hidef.h> /* for EnableInterrupts macro */
#include "derivative.h" /* include peripheral declarations */
#include <stdio.h>   /* for sprintf */

#include "uart.h"


#define LF 0x0A						// Line Feed ASCII code
#define CR 0x0D						// Carriage Return ASCII code

/* -------- Prototypes of internal functions   -------- */

/* -------- Implementation of public functions   -------- */

/** Init the SCI interface
 * @param speed the baudrate; one of the constants defined in <device>.h
 * @return 0 if init was ok; error code otherwise. For now it always returns OK.
 * @note It expects F_CPU constant exist which is the CPU frequency in Hz. 
 **/
uint8_t uart_init(UART_speed_t speed)
{
    SCI1BD = speed;
    SCI1C1 = 0;	// 8,N,1
    SCI1C2 = 0x0C;	// SCI Tx/Rx enabled

    return 0;
}

/** Write single byte to SCI
 * @param byte of data to write
 **/
void uart_write(uint8_t data)
{
    // wait for ready to send
    // Note: we do not reset watchdog assuming it will never restart in
    // normal situation but will restart in some hw error 
    while ( (SCI1S1 & 0x80) == 0 )
        ;  //MSF_RESET_WATCHDOG();
        
	SCI1D = data;							// send data byte on COM1
	while(!SCI1S1_TC);					// wait for Tx Complete flag to set
}

/** Read byte form SCI; wait for data to arrive!
 * @return byte received from SCI.
 **/
uint8_t uart_read(void)
{
    // wait for data to arrive
    while (SCI1S1_RDRF == 0 )
        __RESET_WATCHDOG();
    
    return (uint8_t)SCI1D;    
}


/** Read one character from SCI.
 * @return the character read.
 * @note blocks the caller untill character is received! 
 **/
char uart_getc()               
{
    return (char)uart_read();   
}

/** Check if data are available for reading
 * @return true if there are data
 *  
 **/
bool uart_data_available(void)
{
    return (SCI1S1_RDRF != 0);
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

/** @} */