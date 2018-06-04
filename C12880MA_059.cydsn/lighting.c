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

/***************************************
*        Debug variables
***************************************/ 

extern char LCD_str[40];

/***************************************
*        Function Prototypes
***************************************/ 

uint16 Convert2Dec2(uint8 array[], uint8 len);

/***************************************
*    Initialize Setting Structures
***************************************/ 

CAT4004 laser = {
    .power = POWER_OFF,
    .power_level = 0,
    .use_flash = False
};

CAT4004 led = {
    .power = POWER_OFF,
    .power_level = 0,
    .use_flash = False
};

PWM_dimmer external_light = {
    .power = POWER_OFF,
    .power_level = 32,
    .use_flash = False
};

/******************************************************************************
* Function Name: lighting_commands
*******************************************************************************
*
* Summary:
*  Check input from user to see how they want to change lighting settings
*
* Parameters:
*  uint8 buffer[]: array of user inputted chars
*
* Return:
*  None
*
* Global variables:
*  None
*
*******************************************************************************/

void lighting_commands(uint8 buffer[]) {
    
    switch( buffer[1] ) {
        
        case 'E': ;  // LED commands: LED|....
            if ( buffer[5] == 'N' ) {  // turn led on: LED|ON|0
                //uint8 power_level = buffer[7] - '0';
                uint8 power_level = Convert2Dec2(&buffer[7], 2);
                turn_led_on(power_level);
            }
            else if ( buffer[5] == 'F' ) {  // turn led off: LED|OFF
                turn_led_off();
            }
            else if ( buffer[5] == 'O' ) {  // change led power level: LED|POWER|5
                uint8 power_level = buffer[10] - '0';
                set_led_power_level( power_level );
            }
            else if (buffer[4] == 'F' ) {  // set light flash source: LED|Flash|0
                led.use_flash = buffer[10] - '0';
            }
        break;
        
        case 'a': ;  // Laser commands: Laser|....
            if ( buffer[7] == 'N' ) {  // turn led on: Laser|ON|0
                //uint8 power_level = buffer[9] - '0';
                uint8 power_level = Convert2Dec2(&buffer[9], 2);
                turn_laser_on(power_level);
            }
            else if ( buffer[7] == 'F' ) {  // turn led off: Laser|OFF
                turn_laser_off();
            }
            else if ( buffer[7] == 'O' ) {  // change led power level: Laser|POWER|2
                uint8 power_level = buffer[12] - '0';
                set_laser_power_level( power_level );
            }
            else if (buffer[6] == 'F' ) {  // set light flash source: Laser|Flash|0
                laser.use_flash = buffer[12] - '0';
            }
        break;
            
        case 'i': ; // External light commands: Light 1|...
            if ( buffer[6] == '1' ) {  // commands for the first light source: LIGHT 1|
                if ( buffer[9] == 'N' ) {  // turn external light 1 on: LIGHT 1|ON|XXX
                    uint8 power_level = Convert2Dec2(&buffer[11], 3);
                    turn_external_light_on(power_level);
                }
                else if ( buffer[9] == 'F' ) {  // turn external light 1 off: LIGHT 1|OFF
                    turn_external_light_off();
                }
                else if ( buffer[9] == 'O' ) {  // set external light 1 power: LIGHT 1|POWER|XXX
                    uint8 power_level = Convert2Dec2(&buffer[14], 3);
                    set_external_light_power_level(power_level);
                }
                else if ( buffer[8] == 'F') {  // set light flash source: Light 1|Flash|0
                    external_light.use_flash = buffer[14] - '0'; 
                }
            }
        break;
    }
}

/******************************************************************************
* Function Name: turn_led_on
*******************************************************************************
*
* Summary:
*  Turn LED, that is connected to a CAT4004 LED driver, on
*
* Parameters:
*  uint8 power_level: integer 1-6 that sets the power level of led using the CAT4004
*                     current applied to LED is max current/2^power_level 
*
* Return:
*  None
*
* Global variables:
*  CAT4004 led: data structure holding current state of the LED
*
*******************************************************************************/

void turn_led_on(uint8 power_level) {
    if (power_level == 64) {  // hack
        power_level = led.power_level;
    }
    
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

/******************************************************************************
* Function Name: turn_led_off
*******************************************************************************
*
* Summary:
*  Turn LED, that is connected to a CAT4004 LED driver, off
*
* Parameters:
*  None
*
* Return:
*  None
*
* Global variables:
*  CAT4004 led: data structure holding current state of the LED
*
*******************************************************************************/

void turn_led_off(void) {
    led.power = POWER_OFF;
    LED_Write(0);
}

/******************************************************************************
* Function Name: set_led_power_level
*******************************************************************************
*
* Summary:
*  Set the power level of the LED that is connect to a CAT4004 LED driver, if the 
*    LED is on, the power will automatically update.
*
* Parameters:
*  uint8 power_level: integer 1-6 that sets the power level of led using the CAT4004
*                     current applied to LED is max current/2^power_level 
*
* Return:
*  None
*
* Global variables:
*  CAT4004 led: data structure holding current state of the LED
*
*******************************************************************************/

void set_led_power_level(uint8 power_level) {
    if (led.power == POWER_ON) {  // power is on so change CAT4004 state now
        // check if the power is set to a higher level you have to go to zero then to 100% and down to desired level
        uint8 num_transition = ( (power_level - led.power_level) + DIMMING_LEVELS ) % DIMMING_LEVELS;

        for (uint8 i = 0; i < num_transition; i++) {
            LED_Write(0);
            CyDelayUs(CAT4004_DELAY);
            LED_Write(1);
            CyDelayUs(CAT4004_DELAY);
        }
    }
    led.power_level = power_level;
}

/******************************************************************************
* Function Name: set_led_flash
*******************************************************************************
*
* Summary:
*  Set flag of LED to use a flash during reading.
*
* Parameters:
*  uint8 use_flash:  Boolean if flash should be used
*
* Return:
*  None
*
* Global variables:
*  CAT4004 led: data structure holding current state of the LED
*
*******************************************************************************/

void set_led_flash(uint8 use_flash) {
    led.use_flash = use_flash;
}

/******************************************************************************
* Function Name: turn_laser_on
*******************************************************************************
*
* Summary:
*  Turn a laser, that is connected to a CAT4004 LED driver, on
*
* Parameters:
*  uint8 power_level: integer 1-6 that sets the power level of led using the CAT4004
*                     current applied to the laser is max current/2^power_level 
*
* Return:
*  None
*
* Global variables:
*  CAT4004 laser: data structure holding current state of the laser
*
*******************************************************************************/

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

/******************************************************************************
* Function Name: turn_laser_off
*******************************************************************************
*
* Summary:
*  Turn a laser, that is connected to a CAT4004 LED driver, off
*
* Parameters:
*  None
*
* Return:
*  None
*
* Global variables:
*  CAT4004 laser: data structure holding current state of the laser
*
*******************************************************************************/

void turn_laser_off(void) {
    laser.power = POWER_OFF;
    Laser_Write(0);
}

/******************************************************************************
* Function Name: set_laser_power_level
*******************************************************************************
*
* Summary:
*  Set the power level of a laser that is connect to a CAT4004 LED driver, if the 
*    laser is on, the power will automatically update.
*
* Parameters:
*  uint8 power_level: integer 1-6 that sets the power level of led using the CAT4004
*                     current applied to the laser is max current/2^power_level 
*
* Return:
*  None
*
* Global variables:
*  CAT4004 laser: data structure holding current state of the laser
*
*******************************************************************************/

void set_laser_power_level(uint8 power_level) {
    if (laser.power == POWER_ON) {
        uint8 num_transition = ( (power_level - laser.power_level) + DIMMING_LEVELS ) % DIMMING_LEVELS;    
        for (uint8 i = 0; i < num_transition; i++) {
            Laser_Write(0);
            CyDelayUs(CAT4004_DELAY);
            Laser_Write(1);
            CyDelayUs(CAT4004_DELAY);
        }
    }
    laser.power_level = power_level;
}

/******************************************************************************
* Function Name: set_laser_flash
*******************************************************************************
*
* Summary:
*  Set flag of laser to use a flash during reading.
*
* Parameters:
*  uint8 use_flash:  Boolean if flash should be used
*
* Return:
*  None
*
* Global variables:
*  CAT4004 laser: data structure holding current state of the laser
*
*******************************************************************************/

void set_laser_flash(uint8 use_flash) {
    led.use_flash = use_flash;
}

/******************************************************************************
* Function Name: turn_external_light_on
*******************************************************************************
*
* Summary:
*  Turn an external light source on, the light source is controlled through 
*    an N-channel MOSFET with the gate connected to a PSoC PWM
*
* Parameters:
*  uint8 power_level: duty cycle of PWM to control the light source brightness
*
* Return:
*  None
*
* Global variables:
*  PWM_dimmer external_light: data structure holding current state 
*    of the external_light source
*
*******************************************************************************/

void turn_external_light_on(uint8 power_level) {
    LED_Dimmer_Start();
    LED_Dimmer_WriteCompare(power_level);
    external_light.power = POWER_ON;
}

/******************************************************************************
* Function Name: turn_external_light_off
*******************************************************************************
*
* Summary:
*  Turn an external light source off, the light source is controlled through 
*    an N-channel MOSFET with the gate connected to a PSoC PWM
*
* Parameters:
*  None
*
* Return:
*  None
*
* Global variables:
*  PWM_dimmer external_light: data structure holding current state 
*    of the external_light source
*
*******************************************************************************/

void turn_external_light_off(void) {
    external_light.power = POWER_OFF;
    LED_Dimmer_Stop();
}

/******************************************************************************
* Function Name: set_external_light_power_level
*******************************************************************************
*
* Summary:
*  Set the power of an external light source, the light source is controlled through 
*    an N-channel MOSFET with the gate connected to a PSoC PWM
*
* Parameters:
*  uint8 power_level: duty cycle of PWM to control the light source brightness
*
* Return:
*  None
*
* Global variables:
*  PWM_dimmer external_light: data structure holding current state 
*    of the external_light source
*
*******************************************************************************/

void set_external_light_power_level(uint8 power_level) {
    if (external_light.power == POWER_ON) {
        LED_Dimmer_WriteCompare(power_level);
    }
    else {
        LED_Dimmer_Start();
        LED_Dimmer_WriteCompare(power_level);
        LED_Dimmer_Stop();
    }
    external_light.power_level = power_level;
}

/******************************************************************************
* Function Name: set_external_light_flash
*******************************************************************************
*
* Summary:
*  Set flag of external light to use a flash during reading.
*
* Parameters:
*  uint8 use_flash:  Boolean if flash should be used
*
* Return:
*  None
*
* Global variables:
*  CAT4004 laser: data structure holding current state of the external light
*
*******************************************************************************/

void set_external_light_flash(uint8 use_flash) {
    external_light.use_flash = use_flash;
}

/******************************************************************************
* Function Name: Convert2Dec2
*******************************************************************************
*
* Summary:
*  THIS FUNCTION NEEDS TO BE MOVED.
*  Convert in inputted char array into the corresponding integer it represents
*
* Parameters:
*  uint8 array[]: array of chars
*  uint8 len: number of chars to read to convert to the integer number
*
* Return:
*  uint16: number represented in the array
*
*******************************************************************************/

uint16 Convert2Dec2(uint8 array[], uint8 len){
    uint16 num = 0;
    for (int i = 0; i < len; i++){
        num = num * 10 + (array[i] - '0');
    }
    return num;
}

/* [] END OF FILE */
