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
    
    
#define True                1
#define False               0
#define POWER_OFF           0
#define POWER_ON            1


/***************************************
*    Light Sources Setting Structures
***************************************/ 
    
typedef struct {
    uint8 power;
    uint8 power_level;
    uint8 use_flash;
} CAT4004;

typedef struct {
    uint8 power;
    uint8 power_level;
    uint8 use_flash;
} PWM_dimmer;

/***************************************
*        Function Prototypes
***************************************/  

void lighting_commands(uint8 buffer[]);

void turn_led_on(uint8 power_level);
void turn_led_off(void);
void set_led_power_level(uint8 power_level);
void set_led_flash(uint8 use_flash);

void turn_laser_on(uint8 power_level);
void turn_laser_off(void);
void set_laser_power_level(uint8 power_level);
void set_laser_flash(uint8 use_flash);

void turn_external_light_on(uint8 power_level);
void turn_external_light_off(void);
void set_external_light_power_level(uint8 power_level);
void set_external_light_flash(uint8 use_flash);

#endif

/* [] END OF FILE */
