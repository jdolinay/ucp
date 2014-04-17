/**
* @file  ucp_app.c
* App template
* Sablona ridici aplikace pro knihovnu UCP
*
* Tento soubor pridejte do projektu a implementujte zde celou funcnost aplikace.
* Na zacatku main() zavolejte ucp_app_init() a pak
* pravidelne s periodou vzokovani volejte ucp_app_on_sample().
*
*/

#include "ucp_app.h"
//#include "ucp_psd.h"
#include "ucp_onoff.h"
//#include "ucp_swpwm.h"
#include "ucp_hal.h"

#include "uart.h"
                       

/** Regulatory */
/* Zde definujte regulator(y) a jejich parametry */
/* Priklad pro PSD regulator  */
/* Parametry PSD regulatoru 1 */
/*
#define     Q0      (17.036)
#define     Q1      (-31.94)
#define     Q2      (14.94)
#define     Q3      (0.0)
*/
/* Promenna definujici regulator */
/*
UCP_PSD_REG psd1;
*/
  
/* Zde definujte min a max hodnoty akcniho zasahu pro regulator */
/*
#define     MINU     (0.0)
#define     MAXU    (100.0)
*/

UCP_ONOFF_REG regulator;


/** uzivatel zajisti volani po startu aplikace */
void ucp_app_init(void)
{
    ucphal_init();
       
    // TODO: zde inicializovat regulator
    // Priklad pro PSD regulator: 
    // ucp_psd_init(&psd1, Q0, Q1, Q2, Q3 );
    // On-Off regulator
    ucp_onoff_init(&regulator, 0, 1);	
}

/** Uzivatel zajisti volani s periodou vzorkovani */
void ucp_app_on_sample(void)
{
    float input, setpoint;
   // float u;
    uint8_t u;
    
    /* Namereni vstupu */
    input = ucphal_read_input(1);   /* kanal 1 */

    
    /* Nacteni aktualni pozadovane hodnoty */
    setpoint = ucphal_read_setpoint(1); /* pro kanal 1 */
    
    /* Vypocet akcnich zasahu pro dany kanal pomoci regulatoru */
    // Priklad pro PSD regulator   
    //u = ucp_psd_step(&psd1, input, setpoint, MINU, MAXU);
    // On-off regulator
    u = ucp_onoff_step(&regulator, input, setpoint);
    
    /* Provedeni akcniho zasahu  */
    //ucphal_write_output(1, u );
    // Priklad pokud budu chtit jen on off muzu volat
    ucphal_write_pin(1, u );
    
    /* Vypis udaju na UART */    
    uart_printf_float("w=", "%2.1f", setpoint);
    uart_printf_float(" y=", "%2.1f", input);    
    uart_printf16(" u=", "%d", (uint16_t)u);
    //uart_puts(" u =");
    //uart_puts( (u == 0) ? "OFF" : "ON" );
    uart_putchar('\n');   
        
}



