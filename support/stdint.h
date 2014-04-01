/****************************************************************************
 * @file     stdint.h
 * @brief    define some C99 data types 
 * @version  1
 * @date     28. Jan 2014
 *
 * @note    Defines some C99 data types for CW 6.3 compiler which does not contain this file.
 *
 ******************************************************************************/
#ifndef MSF_STDINT_H
#define MSF_STDINT_H


typedef unsigned char   uint8_t;
typedef unsigned int    uint16_t;
typedef unsigned long   uint32_t;

/* Our own bool definition
 For C99 compiler it is contained in stdbool.h*/
typedef enum { false, true } bool;
     

#endif /* MSF_STDINT_H */