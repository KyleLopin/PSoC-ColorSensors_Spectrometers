/*******************************************************************************
 * File Name: lighting.h
 * Version 0.20
 *
 * Description:
 *  This file provides functions to control leds or lasers 
 *  through a CAT4004 for the C12880 breakout board
 *
*******************************************************************************
 * Copyright by Kyle Lopin, Naresuan University, 2018
 * All Rights Reserved
 * UNPUBLISHED, LICENSED SOFTWARE.
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION
 * WHICH IS THE PROPERTY OF Naresuan University.
 *
 * ========================================
*/

#if !defined(_LIGHTING_H)
#define _LIGHTING_H

#include <project.h>











/***************************************
*        Function Prototypes
***************************************/  

void lighting_commands(uint8 buffer[]);
void turn_led_on(uint8 power_level);
void turn_led_off(void);
void set_led_power_level(uint8 power_level);

void turn_laser_on(uint8 power_level);
void turn_laser_off(void);
void set_laser_power_level(uint8 power_level);

#endif

/* [] END OF FILE */
