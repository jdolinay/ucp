/*
 *  Sample application for UCP with On-off controller
 *  
 * Vzorovy program pro regulaci s vyuzitim knihovny UCP knihovny  (UTB control package)
 *
 * Author: Jan Dolinay
 *
 * Program obsahuje OnOff regulator a akcni zasah realizuje digitalnim vystupem.
 * Komunikuje pres seriovou linku, virtualni OpenSDA COM port, 115200 bd.
 * 
 *
 *
 */



#include "derivative.h" /* include peripheral declarations */

// krok 1
#include "ucp_app.h"

// cesta do \support je v include dirs. Pak jsou podslozky podle platformy.
#include "kinetis\uart.h"
#include "kinetis\top_kl25.h"

void delay(void);

int main(void)
{	
	char stop = 1;
	char zn;
	
	uart_init(BD_115200);
	
	ucp_app_init();	
	
	for(;;) {	   
			if (!stop )
				ucp_app_on_sample();	// krok 3  (TODO: casovani)
		   		   			   	
		   	/* Rucni ovladani topeni */	   	
		   	if ( uart_data_available() )
		   	{
		   		zn = uart_getc();
		   		if ( zn == '1' )
		   			TopOn();
		   		else if ( zn == '0' )
		   			TopOff();
		   		else if ( zn == 's' )
		   			stop = !stop;	
		   		else
		   			uart_puts("Neznamy prikaz\n");
		   	}
		
		   	
		   	delay();
		   	delay();
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

