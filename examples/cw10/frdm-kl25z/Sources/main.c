/*
 * main implementation: use this 'C' sample to create your own application
 *
 */





#include "derivative.h" /* include peripheral declarations */

// krok 1
#include "ucp_app.h"


int main(void)
{
	int counter = 0;
	
	
	// krok 2
	ucp_app_init();		
	
	for(;;) {	   
	   	counter++;
	   	if ( counter == 0 )
	   		ucp_app_on_sample();	// krok 3  (TODO: casovani)	
	}
	
	return 0;
}
