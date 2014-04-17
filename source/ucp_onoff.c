/**
* @file  ucp_onoff.c
* @brief on-off controller (bang-bang controller)
*
*/

#include "ucp_onoff.h"

/** Init 2-polohoveho (on-off)) regulatoru
* @param pReg ukazatel na strukturu alokovanou uzivatelem (globalni promenna)
* @param up_hysteresis hystereze pri "topeni" = pri rostoucim y vypne topeni po 
*   dosazeni/prekroceni (w + up_hysteresis).
* @param down_hysteresis hystereze pri "chladnuti" = pri klesajicim y zapne 
*   topeni po dosazeni/prekroceni (w - down_hysteresis).
* @return error code; (0 = OK)
*/
uint8_t ucp_onoff_init(UCP_ONOFF_REG* pReg, float up_hysteresis, float down_hysteresis)
{
    pReg->up_hysteresis = up_hysteresis;
    pReg->down_hysteresis = down_hysteresis;
    //pReg->yk1 = 0.0f;
    pReg->uk1 = 0;
    
    return UCP_ERROR_OK;  
}

/** Vypocet dalsiho akcniho zasahu. Nutno volat pravidelne. 
* @param pReg ukazatel na strukturu alokovanou uzivatelem (globalni promenna)
* @param y aktualni hodnota regulovane veliciny
* @param setpoint pozadovana hodnota
* @return jaky ma byt akcni zasah; 0 = nic (netopit), 1 = topit
*/
uint8_t ucp_onoff_step(UCP_ONOFF_REG* pReg, float y, float setpoint)
{    
    uint8_t uk, heating;
        
    /* Smer zmeny regulovane veliciny (roste nebo klesa) */
   /* Verze se sledovanim minule hodnoty y je nespolehliva, muze se objevit 
    i pri chladnuti na okamzik vyssi teplota nez v minulem kroku a regulator 
    pak zacne topit...
    Jednodussi a spolehlivejsi je rozhodovat podle minule hodnoty akcniho zasahu. 
    if ( y > pReg->yk1 )
        heating = 1;
    else
        heating = 0;
    */
    /* Jestli jsme minulem kroku topili, pak pouzivame hysterezi pro smer "y roste" */
    if ( pReg->uk1 )
        heating = 1;
    else
        heating = 0;
    
    /* Vlastni vypocet akcniho zasahu */
    if ( heating )
    {
        if ( y > (setpoint + pReg->up_hysteresis) )
            uk = 0;
        else
            uk = 1;
    }    
    else
    {
        if ( y < (setpoint - pReg->down_hysteresis) )
            uk = 1;
        else
            uk = 0;
    }
        
    /* Aktualizace minulych hodnot */    
    pReg->uk1 = uk;
    
    return uk;   
}
