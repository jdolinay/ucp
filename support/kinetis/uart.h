/****************************************************************************
 * @file     uart.h
 * @brief    UART driver; generic definitions for all platforms 
 * @version  1
 * @date     8. Apr 2014
 *
 * @note    
 *
 ******************************************************************************/
#ifndef MSF_UART_H
#define MSF_UART_H

#ifdef __cplusplus
extern "C" {
#endif

#include "ucp_stdint.h"
#include "ucp_stdbool.h"


#include "stdio.h"
/********************************************************************/

#define F_CPU (20900000)
/* baudrate constants for 24 MHz UART clock (48 MHz F_CPU) */
/**
 * Baud rate is:
 * baud = uart_clock / ((OSR+1)xBR)
 * BR is baud rate divisor set in baud register UART_BDH (1 - 8191)
 * OSR is oversampling set in UART_C4 register ( 4 - 32)
 * We define the enum value as the BD in lower word and OSR in upper word 
 * */
/** macro which creates 32-bit value by combining OSR and BR values.
 * osr is the real OSR value; the register C4 stores osr-1 (value %11 (3) in OSR means real osr = 4) */
#define		UART_MAKE_BDVAL(osr, br)	((br & 0x0000FFFF) | ((osr) << 16))
#define		UART_GET_BR(baud_val)	(baud_val & 0x00001FFF)		/* the BR is actually only 13-bit long*/
#define		UART_GET_OSR(baud_val)	((baud_val >> 16) & 0x0000001F)		/* OSR is only 5-bits */

#if F_CPU == 48000000
 enum UART_baudrate_type 
 {
        BD_INVALID = 0,
        BD_9600 = UART_MAKE_BDVAL(20, 125),
        BD_19200 = UART_MAKE_BDVAL(13, 96),
        BD_57600 = UART_MAKE_BDVAL(13, 32),
        BD_115200 = UART_MAKE_BDVAL(13, 16), 
       
 };
#elif F_CPU == 20900000		/* default for frdm-kl25z */
 enum UART_baudrate_type 
 {
        BD_INVALID = 0,
        BD_9600 = UART_MAKE_BDVAL(11, 198),
        BD_19200 = UART_MAKE_BDVAL(11, 99),
        BD_57600 = UART_MAKE_BDVAL(11, 33),
        BD_115200 = UART_MAKE_BDVAL(13, 14), 
       
 };
#else
	#error The CPU clock defined by F_CPU is not supported.
#endif 
 
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



#if 0
// jd: cislo SCI interface 
#define TERM_PORT_NUM       0
#define TERMINAL_BAUD       115200

// TODO: predelat v kodu na typy z ucp_stdint.h 
/*
 * The basic data types
 */
typedef unsigned char		uint8;  /*  8 bits */
typedef unsigned short int	uint16; /* 16 bits */
typedef unsigned long int	uint32; /* 32 bits */

typedef char			    int8;   /*  8 bits */
typedef short int	        int16;  /* 16 bits */
typedef int		            int32;  /* 32 bits */

typedef volatile int8		vint8;  /*  8 bits */
typedef volatile int16		vint16; /* 16 bits */
typedef volatile int32		vint32; /* 32 bits */

typedef volatile uint8		vuint8;  /*  8 bits */
typedef volatile uint16		vuint16; /* 16 bits */
typedef volatile uint32		vuint32; /* 32 bits */


/*
//void uart_init (UART_MemMapPtr uartch, int sysclk, int baud);
char uart_getchar (UART_MemMapPtr channel);
void uart_putchar (UART_MemMapPtr channel, char ch);
int uart_getchar_present (UART_MemMapPtr channel);
void uart0_init (UART0_MemMapPtr uartch, int sysclk, int baud);
char uart0_getchar (UART0_MemMapPtr channel);
void uart0_putchar (UART0_MemMapPtr channel, char ch);
int uart0_getchar_present (UART0_MemMapPtr channel);
*/
/********************************************************************/
#endif

#ifdef __cplusplus
}
#endif
/* ----------- end of file -------------- */
#endif /* MSF_UART_H */
