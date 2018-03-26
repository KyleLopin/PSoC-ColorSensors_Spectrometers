/*******************************************************************************
* File Name: user_selections.h
*
* Description:
*  Structure, constants and function prototypes to use a AS7262 light sensor
*
**********************************************************************************
 * Copyright by Kyle Lopin, Naresuan University, 2017-2018
 * All Rights Reserved
 * UNPUBLISHED, LICENSED SOFTWARE.
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION
 * WHICH IS THE PROPERTY OF Naresuan University.
 *
 * ========================================
*/

#if !defined(_AS726X_H)
#define _AS726X_H

#include <project.h>
#include "i2c_functions.h"
    
#define True                                1
#define False                               0

/***************************************
*      Structures
***************************************/ 

typedef struct {
    uint8 measurement_mode;
    uint8 gain;
    uint8 interrupt_on;
    uint8 integration_time;
    uint8 LED_power_level;
    uint8 LED_on;
    uint8 indicator_power_level;
    uint8 indicator_on;
    uint8 device_type;
    uint8 hw_version;
    uint16 fw_version;
    uint8* comm_error;
    uint8 control_reg_value;
    uint8 LED_control_reg_value;
} AS7262_settings;

/***************************************
*      I2C AS7262 Constants
***************************************/ 

#define AS726x_ADDRESS                      0x49


/***************************************
*      AS7262 register masks
***************************************/ 

#define AS726x_WRITE_REG_MASK               0x80

/***************************************
*      AS7262 Control Registers
***************************************/ 

#define AS726x_REG_DEVICE_TYPE              0x00
#define AS726x_REG_HW_VERSION               0x01
#define AS726x_REG_FW1_VERSION              0x02
#define AS726x_REG_FW2_VERSION              0x03
#define AS726x_REG_CONTROL                  0x04
#define AS726x_REG_INT                      0x05
#define AS726x_REG_TEMP                     0x06
#define AS726x_REG_LED                      0x07

/***************************************
*      AS7262 Data Registers
***************************************/ 

#define AS726x_DATA_1                       0x08
#define AS726x_DATA_2                       0x0A
#define AS726x_DATA_3                       0x0C
#define AS726x_DATA_4                       0x0E
#define AS726x_DATA_5                       0x10
#define AS726x_DATA_6                       0x12
#define AS726x_DATA_1_CAL                   0x14
#define AS726x_DATA_2_CAL                   0x18
#define AS726x_DATA_3_CAL                   0x1C
#define AS726x_DATA_4_CAL                   0x20
#define AS726x_DATA_5_CAL                   0x24
#define AS726x_DATA_6_CAL                   0x28

#define AS726x_DATA_START_ADDR              0x14


/***************************************
*      AS7262 Device Constants
***************************************/ 

#define AS726x_DEVICE_TYPE                  0x40
#define AS7262_HARDWARE_VERSION             0x3E
#define AS7263_HARDWARE_VERSION             0x3F

/***************************************
*      Modes and Settings of AS7262
***************************************/ 

// Bank modes that determine what colors are measured
#define AS726x_BANK_MODE_0                  0x00
#define AS726x_BANK_MODE_1                  0x01
#define AS726x_BANK_MODE_2                  0x02
#define AS726x_BANK_MODE_3                  0x03

// Gain settings
#define AS726x_GAIN_SETTING_1X              0x00
#define AS726x_GAIN_SETTING_3_7X            0x01
#define AS726x_GAIN_SETTING_16X             0x02
#define AS726x_GAIN_SETTING_64X             0x03

// LED power settings
#define AS726x_LED_POWER_12_5_mA            0x00
#define AS726x_LED_POWER_25_mA              0x01
#define AS726x_LED_POWER_50_mA              0x02
#define AS726x_LED_POWER_100_mA             0x03


#define AS726x_INDICATOR_POWER_1_mA         0X00
#define AS726x_INDICATOR_POWER_2_mA         0X01
#define AS726x_INDICATOR_POWER_4_mA         0X02
#define AS726x_INDICATOR_POWER_8_mA         0X03


/***************************************
*      AS7262 User Commands Constants
***************************************/ 

#define AS726x_READ_COMMANDS                'R'
#define AS726x_SET_INTEGRATION_TIME         'I'
#define AS726x_SET_GAIN                     'G'
#define AS726x_SET_LED_POWER                'P'
#define AS726x_LED_ON_OFF                   'L'

extern char USB_str[40];


/***************************************
*        Function Prototypes
***************************************/ 

uint8 AS726x_Init(void);
void AS726x_Start(void);


void AS726x_Commands(uint8 buffer[]);
void AS726x_SingleRead(void);

#endif

/* [] END OF FILE */
