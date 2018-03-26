/*******************************************************************************
 * File Name: C12880.c
 * Version 0.20
 *
 * Description:
 *  This file provides functions to control an C12880MA through a series of digital periphirals
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

#include "lighting.h"
#include <stdio.h>

#define True                1
#define False               0
#define POWER_OFF           0
#define POWER_ON            1
#define CAT4004_DELAY       1
#define DIMMING_LEVELS      32

extern char LCD_str[40];
uint16 Convert2Dec2(uint8 array[], uint8 len);


typedef struct {
    uint8 power;
    uint8 power_level;
} CAT4004;

CAT4004 laser = {
    .power = POWER_OFF,
    .power_level = 0
};

CAT4004 led = {
    .power = POWER_OFF,
    .power_level = 0
};


void lighting_commands(uint8 buffer[]) {
    switch( buffer[1] ) {
        case 'E': ;  // LED commands
            if ( buffer[5] == 'N' ) {  // turn led on
                //uint8 power_level = buffer[7] - '0';
                uint8 power_level = Convert2Dec2(buffer[7], 2);
                turn_led_on(power_level);
            }
            else if ( buffer[5] == 'F' ) {  // turn led off
                turn_led_off();
            }
            else if ( buffer[5] == 'O' ) {  // change led power level
                uint8 power_level = buffer[10] - '0';
                set_led_power_level( power_level );
            }
        break;
        
        case 'A': ;  // Laser commands
            if ( buffer[7] == 'N' ) {  // turn led on
                //uint8 power_level = buffer[9] - '0';
                uint8 power_level = Convert2Dec2(buffer[9], 2);
                turn_laser_on(power_level);
            }
            else if ( buffer[7] == 'F' ) {  // turn led off
                turn_laser_off();
            }
            else if ( buffer[7] == 'O' ) {  // change led power level
                uint8 power_level = buffer[12] - '0';
                set_laser_power_level( power_level );
            }
        break;
        
    }
}


void turn_led_on(uint8 power_level) {
    if (power_level == 64) {  // hack
        power_level = led.power_level;
    }
//    LCD_Position(1,0);
//    sprintf(LCD_str, "n tr: %d", power_level);
//    LCD_PrintString(LCD_str);
    
    LED_Write(1);
    CyDelayUs(20);
    for ( uint8 i=0; i < power_level; i++ ) {
        LED_Write(0);
        CyDelayUs(CAT4004_DELAY);
        LED_Write(1);
        CyDelayUs(CAT4004_DELAY);
    }
    led.power = POWER_ON;
    led.power_level = power_level;
}

void turn_led_off(void) {
    led.power = POWER_OFF;
    LED_Write(0);
}

void set_led_power_level(uint8 power_level) {
    if (led.power == POWER_ON) {
        uint8 num_transition = ( (power_level - led.power_level) + DIMMING_LEVELS ) % DIMMING_LEVELS;
//        
//
//        LCD_Position(1,0);
//        sprintf(LCD_str, "n tr: %d", num_transition);
//        LCD_PrintString(LCD_str);
        
        
        for (uint8 i = 0; i < num_transition; i++) {
            LED_Write(0);
            CyDelayUs(CAT4004_DELAY);
            LED_Write(1);
            CyDelayUs(CAT4004_DELAY);
        }
    }
    led.power_level = power_level;
}

void turn_laser_on(uint8 power_level) {
    if (power_level == 64) {  // hack
        power_level = laser.power_level;
    }
    Laser_Write(1);
    CyDelayUs(20);
    for ( uint8 i=0; i < power_level; i++ ) {
        Laser_Write(0);
        CyDelayUs(CAT4004_DELAY);
        Laser_Write(1);
        CyDelayUs(CAT4004_DELAY);
    }
    laser.power = POWER_ON;
    laser.power_level = power_level;
}

void turn_laser_off(void) {
    laser.power = POWER_OFF;
    Laser_Write(0);
}

void set_laser_power_level(uint8 power_level) {
    if (laser.power == POWER_ON) {
        uint8 num_transition = ( (power_level - laser.power_level) + DIMMING_LEVELS ) % DIMMING_LEVELS;
//        
//
//        LCD_Position(1,0);
//        sprintf(LCD_str, "n tr: %d", num_transition);
//        LCD_PrintString(LCD_str);
//               
        for (uint8 i = 0; i < num_transition; i++) {
            Laser_Write(0);
            CyDelayUs(CAT4004_DELAY);
            Laser_Write(1);
            CyDelayUs(CAT4004_DELAY);
        }
    }
    laser.power_level = power_level;
}

uint16 Convert2Dec2(uint8 array[], uint8 len){
    uint16 num = 0;
    for (int i = 0; i < len; i++){
        num = num * 10 + (array[i] - '0');
    }
    return num;
}


/* [] END OF FILE */
