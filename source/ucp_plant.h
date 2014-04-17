/**
* @file  ucp_plant.h
* @brief Simulate controlled plant
*
*/
#ifndef  UCP_PLANT_H
    #define  UCP_PLANT_H

/* Vlastni definice datovych typu, jako podle C99 */
#include "ucp_stdint.h"
#include "ucp_stdbool.h"


/** Data pro rizenou soustavu
*/
struct plant_data
{
    float a1;        /**>koeficient regulatoru A1 */
    float a2;
    float b1;
    float b2;
    
    float yk1;      /**> minula hodnota vystupu soustavy */    
    float yk2;  
    float uk1       /**> minula hodnota akcniho zasahu */;
    float uk2;            
            
};

typedef struct plant_data  UCP_PLANT;


uint8_t ucp_plant_init(UCP_PLANT* pReg, float a1, float a2, float b1, float b2);
float ucp_plant_step(UCP_PLANT* pReg, float uk);


/* ------------- end of file ------------- */
#endif