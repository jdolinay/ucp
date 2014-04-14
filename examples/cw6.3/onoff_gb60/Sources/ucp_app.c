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
#include "ucp_onoff.h"
#include "ucp_hal.h"
#include "uart.h"
                       

/** Regulatory */
/* Zde definujte regulator(y) a jejich parametry */
UCP_ONOFF_REG  onoff1;

/* Parametry 2-polohoveho regulatoru pro kanal 1 */
#define     HYSTEREZE_NAHORU       (1.0)
#define     HYSTEREZE_DOLU            (2.0)

/** uzivatel zajisti volani po startu aplikace */
void ucp_app_init(void)
{
    ucphal_init();
       
    // TODO: zde inicializovat regulator
    ucp_onoff_init(&onoff1, HYSTEREZE_NAHORU, HYSTEREZE_DOLU );
}

/** Uzivatel zajisti volani s periodou vzorkovani */
void ucp_app_on_sample(void)
{
    float input, setpoint;
    uint8_t u;
    
    /* Namereni vstupu */
    input = ucphal_read_input(1);   /* kanal 1 */

    
    /* Nacteni aktualni pozadovane hodnoty */
    setpoint = ucphal_read_setpoint(1); /* pro kanal 1 */
    
    /* Vypocet akcnich zasahu pro dany kanal pomoci regulatoru */
    u = ucp_onoff_step(&onoff1, input, setpoint);
    
    /* Provedeni akcniho zasahu  */
    ucphal_write_output(1, u );
        
        
    /* Vypis udaju na UART */    
    uart_printf_float("w=", "%2.1f", setpoint);
    uart_printf_float(" y=", "%2.1f", input);    
    uart_printf16(" u=", "%d", (uint16_t)u);
    //uart_puts(" u =");
    //uart_puts( (u == 0) ? "OFF" : "ON" );
    uart_putchar('\n');    
     
}



