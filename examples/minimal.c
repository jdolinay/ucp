/* Minimal version of UCP application MAIN.C file */

#include "ucp_app.h"

void main()
{
	ucp_app_init();		
    	
	for(;;) {	
            ucp_app_on_sample();   
	    delay();
	}
}
