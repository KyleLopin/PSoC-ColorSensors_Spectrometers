/*******************************************************************************
 * File Name: i2c_functions.h
 * Version 0.80
 *
 * Description:
 *  This file provides function prototypes for i2c functions to read and 
 *  write registers easier the the basic APIs.
 *
********************************************************************************
 *
 * Copyright by Kyle Lopin, Naresuan University, 2017
 * All Rights Reserved
 * UNPUBLISHED, LICENSED SOFTWARE.
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION
 * WHICH IS THE PROPERTY OF Naresuan University.
 *
 * ========================================
*/

#if !defined(_I2C_FUNCTIONS_H)
#define _I2C_FUNCTIONS_H
    
#include <project.h>
#include "stdbool.h"
#include "stdio.h"
    
  
/***************************************
*        Function Prototypes
***************************************/   

uint8 sensor_write8(uint8 address, uint8* buffer);
uint8 sensor_write16(uint8 address, uint8* buffer);
uint8 sensor_write_n(uint8 address, uint8* buffer, uint8 num_bytes);
uint8 sensor_read8(uint8 address, uint8* buffer, uint8 _register, uint8* error);  
uint16 sensor_read16(uint8 address, uint8* buffer, uint8 _register, uint8* error);
uint8 sensor_read_n(uint8 address, uint8* buffer, uint8 _register, uint8 num_bytes);

#endif

/* [] END OF FILE */