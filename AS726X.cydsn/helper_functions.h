/*******************************************************************************
* File Name: helper_functions.h
*
* Description:
*  Functions prototypes and their variables used by main.
*
**********************************************************************************
 * Copyright by Kyle Lopin, Naresuan University, 2017
 * All Rights Reserved
 * UNPUBLISHED, LICENSED SOFTWARE.
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION
 * WHICH IS THE PROPERTY OF Naresuan University.
 *
 * ========================================
*/

#if !defined(HELPER_FUNCTIONS_H)
#define HELPER_FUNCTIONS_H

#include <project.h>
#include "cytypes.h"

/***************************************
*        Function Prototypes
***************************************/ 
    
uint8 convert_hex_string(uint8 array[]);
uint8 hex_value_from_char(uint8 _char);
uint16 ConvertDecimal(uint8 array[], uint8 len);
    
#endif

/* [] END OF FILE */
