/**
*  App template
* Sablona ridici aplikace pro UCP
* Interface na uzivatelskou aplikaci.
*
*/
#include "ucp_app.h"
#include "ucp_psd.h"
#include "ucp_swpwm.h"
#include "ucp_hal.h"
#include "ucp_plant.h"

#include "uart.h"

/* Definovat jako 1, pokud se ma pouzit simulovana soustava. Pak je mozno
v simulatoru dat breakpoint do app_on_sample a sledovat vypoctene hodnoty */
#define     SIMULACE        0
                          

/* Regulatory */
/* Parametry regulatoru PSD1 pro simulaci
- odpovidaji parametrum v xls pro srovnani spravnosti vypoctu. */
/*#define     Q0      (0.009)
#define     Q1      (-0.008)
#define     Q2      (0.0)
#define     Q3      (0.0)
  */
  
  // parametry pro PSD pro rizeni modelu tepelne soustavy (BP Babik 2006)
  // Soustava je K = 0.82 (C/%)
  // T1 = 86.6 s  T2 = 18.2 s
#define     Q0      (17.036)
#define     Q1      (-31.94)
#define     Q2      (14.94)
#define     Q3      (0.0)
  
/** min a max hodnoty akcniho zasahu */
#define     MINU     (0.0)
#define     MAXU    (100.0)


UCP_PSD_REG psd1;

#if SIMULACE
    UCP_PLANT plant;
#endif

/** uzivatel zajisti volani po startu aplikace */
void ucp_app_init(void)
{
    ucphal_init();
       
       // TODO: zde inicializovat regulator
    ucp_psd_init(&psd1, Q0, Q1, Q2, Q3 );
    
#if SIMULACE
    ucp_plant_init(&plant, -0.976, 0.01, 0.5, 6.0);
#endif

}

/** Uzivatel zajisti volani s periodou vzorkovani */
void ucp_app_on_sample(void)
{
    float input, setpoint;
    float u;
#if SIMULACE
    static float uk1 = 0;   // pro simulaci - min hodnota akciho zasahu
#endif
    
    /* Namereni vstupu */
    input = ucphal_read_input(1);   /* kanal 1 */

#if SIMULACE    
    /* pro simulacni test ziskame namerenou hodnotu ze simulovane osustavy */
    input = ucp_plant_step(&plant, uk1);
#endif
    
    /* Nacteni aktualni pozadovane hodnoty */
    setpoint = ucphal_read_setpoint(1); /* pro kanal 1 */
    
    /* Vypocet akcnich zasahu pro dany kanal */
    u = ucp_psd_step(&psd1, input, setpoint, MINU, MAXU);
    
    /* Provedeni akcnich zasahu > Zapis vypoctenych hodnot nekam, do
    "registru", odkud je realizuje napr. nezavisly proces */
    ucphal_write_output(1, u );
    
    /* pro simulaci */
#if SIMULACE
    uk1 = u;
#endif

    /* Vypis udaju na UART */    
    uart_printf_float("w=", "%2.1f", setpoint);
    uart_printf_float(" y=", "%2.1f", input);
    uart_printf_float(" u=", "%2.1f",u);
    uart_putchar('\n');
    
}



