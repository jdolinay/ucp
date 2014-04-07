/*
* @file  ucp_app.h
*
* @brief Main application
*
* @note Predpokladane pouziti: 
* uzivatel na zacatku main() vola ucp_app_init() a pak
* pravidelne s periodou vzokovani vola ucp_app_on_sample().
* V souboru ucp_app.c definuje svoje regulatory a upravi kod podle potreby.
*
*/
#ifndef  UCP_APP_H
    #define  UCP_APP_H

/* Vlastni definice datovych typu, jako podle C99 */
#include "ucp_stdint.h"
#include "ucp_stdbool.h"

/** The interface used by the user application */
void ucp_app_init(void);
void ucp_app_on_sample(void);


/* ------------- end of file ------------- */
#endif         /* UCP_APP_H */