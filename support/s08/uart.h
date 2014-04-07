/****************************************************************************
 * @file     uart.h
 * @brief    UART driver; generic definitions for all platforms 
 * @version  1
 * @date     7. Mar 2014
 *
 * @note  Vyextrahovano z MSF pro pouziti pri testech "regulators" na GB60  
 *
 ******************************************************************************/
#ifndef MSF_UART_H
#define MSF_UART_H

#ifdef __cplusplus
extern "C" {
#endif

#include "ucp_stdint.h"
#include "ucp_stdbool.h"

/* baudrate constants for 20 MHz bus clock */
typedef enum 
 {
        BD_INVALID = 0,
        /*BD2400 = 521,
        BD4800 = 260,*/
        BD9600 =  130, 
        BD19200 =  65,
        BD38400 = 33,       
        BD57600 = 22,
        BD115200 = 11,
 } UART_baudrate_type; 
 

/* UART_speed_t 
 @brief The data type for baud rate for UART (SCI).
 @note Note that C compiler will not even warn when any number is passed in 
 instead of the enum value. But there are still advantages to enum:
 - C++ compiler will catch wrong use
 - we can let user know that some speed is not supported (enum value does not exist).
*/   
typedef enum UART_baudrate_type  UART_speed_t;

/* Public functions */
uint8_t uart_init(UART_speed_t speed);  
bool uart_data_available(void);
void uart_write(uint8_t data);
uint8_t uart_read(void);
void uart_putchar(char c);        // send one character
void uart_puts(const char* str);      // send null-terminated string
char uart_getc();               // read one character (blocking)

/* Convenience global functions for output to UART */
void uart_printf16(const char* str, const char* format, uint16_t data);
void uart_printf32(const char* str, const char* format, uint32_t data);  
void uart_printf_float(const char* str, const char* format, float data);


#ifdef __cplusplus
}
#endif
/* ----------- end of file -------------- */
#endif /* MSF_UART_H */