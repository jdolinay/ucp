/****************************************************************************
 * @file     stdbool.h
 * @brief    define C99 data type bool
 * @version  1
 * @date     28. Jan 2014
 *
 * @note    Defines some C99 data types for compilers which does not contain this file.
 *
 ******************************************************************************/
#ifndef MSF_STDBOOL_H
#define MSF_STDBOOL_H


/* Our own bool definition
 For C99 compiler it is contained in stdbool.h*/
typedef enum { false, true } bool;
     

#endif /* MSF_STDBOOL_H */