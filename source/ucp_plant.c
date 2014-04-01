/*
* @file  ucp_plant.c
* @brief Controlled plant for simulations
* 
* Prenos: 
*  b1 * z-1 + b2 * z-2 
*  ---------------------------------
*  1 + a1 * z-1 + a2 * z-2
*
* y(k) = -1 * a1 * y(k-1) - a2 * y(k-2) + b1 * u(k-1) + b2 * u(k-2) 
*/

#include "ucp_plant.h"

/** Init soustavy
* @param pPlant ukazatel na strukturu alokovanou uzivatelem (globalni promenna)
* @return error code; (0 = OK)
*/
uint8_t ucp_plant_init(UCP_PLANT* pPlant, float a1, float a2, float b1, float b2)
{
    pPlant->a1 = a1;
    pPlant->a2 = a2;
    pPlant->b1 = b1;
    pPlant->b2 = b2;
    
    pPlant->yk1 = 0.0f;    
    pPlant->yk2 = 0.0f;    
    pPlant->uk1 = 0.0f;    
    pPlant->uk2 = 0.0f;  
    
    return 0;  
    
}

/** Vypocet dalsiho vystupu soustavy. Nutno volat s periodou vzorkovani! 
* @param pPlant ukazatel na strukturu alokovanou uzivatelem (globalni promenna)
* @param uk aktualni hodnota akcniho zasahu
* @return aktualni vystup soustavy y(k)
*/
float ucp_plant_step(UCP_PLANT* pPlant, float uk)
{
    float yk;
          
    /* y(k) = a1 * y(k-1) + a2 * y(k-2) + b1 * u(k-1) + b2 * u(k-2) */     
    yk = -1.0 * pPlant->a1 * pPlant->yk1 +  pPlant->a2 * pPlant->yk2 + pPlant->b1 * pPlant->uk1 + pPlant->b2 * pPlant->uk2;
      
    /* Aktualizace minulych hodnot */
    pPlant->yk2 = pPlant->yk1;
    pPlant->yk1 = yk;
    pPlant->uk2 = pPlant->uk1;
    pPlant->uk1 = uk;
    
    return yk;   
}
