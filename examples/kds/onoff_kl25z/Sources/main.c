/*
 Sample project for UCP library for Kinetis Design Studio IDE
 On-Off controller.

 Howto setup a project with UCP:

 1) Add #include "ucp_app.h" to your main.c

 2) Call ucp_app_init(); somewhere near the beginning of main() function

 3) Repeatedly call ucp_app_on_sample(); in the main loop of the program.
  It should be called as often as needed, e.g. for on/off control of a heater once per
  10 seconds can be enough. For discrete PID controller it should be called with the sampling period,
  which was used to determine the parameters of the controller.

 4) Add to your project files ucp_app.h and ucp_app.c from ucp/template.
  COPY these files to your project. DO NOT LINK!
  you can drag and drop the files from Windows explorer to the Sources folder in KDS > your project.

 5) Add to your project files ucp_hal.h and ucp_hal.c from ucp/template.
  COPY these files to your project. DO NOT LINK!

 6) Edit the ucp_app.c file. There is example code in comments in this file.
   Define variable for your controller:  UCP_ONOFF_REG controller;
   Initialize the controller: ucp_onoff_init(&regulator, 0, 1);
   In ucp_app_on_sample(void) call
   u = ucp_onoff_step(&regulator, input, setpoint);
   to obtain desired output of the controller (u).

 7) Edit the file ucp_hal.c.


 */

#include "MKL25Z4.h"
#include "ucp_app.h"

static int i = 0;

int main(void)
{

	ucp_app_init();

    /* This for loop should be replaced. By default this loop allows a single stepping. */
    for (;;) {
        i++;
    }
    /* Never leave main */
    return 0;
}
////////////////////////////////////////////////////////////////////////////////
// EOF
////////////////////////////////////////////////////////////////////////////////
