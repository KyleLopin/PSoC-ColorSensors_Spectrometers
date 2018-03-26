/*******************************************************************************
* File Name: helper_functions.c
*
* Description:
*  Functions used convert between data types
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

#include "helper_functions.h"

/******************************************************************************
* Function Name: convert_hex_string
*******************************************************************************
*
* Summary:
*  Convert a string that represents a hexidecimal number into its 
*  corresponding value.
*
* Parameters:
*  uint8[] array:  array that should have 2 bytes with chars of 
*                  valid hex values
*
* Return:
*  uint8:  value the hex string represented
*
*******************************************************************************/

uint8 convert_hex_string(uint8 array[]) {
    uint8 num = hex_value_from_char(array[0]) << 4;
    num |= hex_value_from_char(array[1]);
    return num;
    
}

/******************************************************************************
* Function Name: hex_value_from_char
*******************************************************************************
*
* Summary:
*  Convert a char that represents a hexidecimal number into its 
*  corresponding value.
*
* Parameters:
*  uint8 char:  char of a valid hexidecimal charater to convert to its decimal value
*
* Return:
*  uint8:  value the hex char represented
*
*******************************************************************************/

uint8 hex_value_from_char(uint8 _char) {
    if ( ('0' <= _char) && (_char <= '9') ) {  // char is between 0-9
        return (_char - '0');
    }
    else if ( ('A' <= _char) && (_char <= 'F') ) {  // char is between A-F
        return (_char - ('A' + 10));
    }
    else if ( ('a' <= _char) && (_char <= 'f') ) {  // char is between a-f
        return (_char - ('a' + 10));
    }
    return 0;
}

/******************************************************************************
* Function Name: ConvertDecimal
*******************************************************************************
*
* Summary:
*  Convert an array of char/uint8 of decimal values into the corresponding 
*  value.
*
* Parameters:
*  uint8[] array:  array that should be valid decimal nubers
*
* Return:
*  uint16:  value the string represented
*
*******************************************************************************/

uint16 ConvertDecimal(uint8 array[], uint8 len) {
    uint16 num = 0;
    for (int i = 0; i < len; i++){
        num = num * 10 + (array[i] - '0');
    }
    return num;
}

/* [] END OF FILE */
