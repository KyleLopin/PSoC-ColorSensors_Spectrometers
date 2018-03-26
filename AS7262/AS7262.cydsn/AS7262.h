/*******************************************************************************
* File Name: user_selections.h
*
* Description:
*  Structure, constants and function prototypes to use a AS7262 light sensor
*
**********************************************************************************
 * Copyright Kyle Vitautas Lopin, Naresuan University, Phitsanulok Thailand
*********************************************************************************/

#if !defined(_AS7262_H)
#define _AS7262_H

#include <project.h>
#include "i2c_functions.h"
    
#define True                        1
#define False                       0

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

#define AS7262_ADDRESS              0x49


/***************************************
*      AS7262 register masks
***************************************/ 

#define AS726x_WRITE_REG_MASK       0x80

/***************************************
*      AS7262 Control Registers
***************************************/ 

#define AS726x_REG_DEVICE_TYPE      0x00
#define AS726x_REG_HW_VERSION       0x01
#define AS726x_REG_FW1_VERSION      0x02
#define AS726x_REG_FW2_VERSION      0x03
#define AS726x_REG_CONTROL          0x04
#define AS726x_REG_INT              0x05
#define AS726x_REG_TEMP             0x06
#define AS726x_REG_LED              0x07

/***************************************
*      AS7262 Data Registers
***************************************/ 

#define AS7262_DATA_V               0x08
#define AS7262_DATA_B               0x0A
#define AS7262_DATA_G               0x0C
#define AS7262_DATA_Y               0x0E
#define AS7262_DATA_O               0x10
#define AS7262_DATA_R               0x12
#define AS7262_DATA_V_CAL           0x14
#define AS7262_DATA_B_CAL           0x18
#define AS7262_DATA_G_CAL           0x1C
#define AS7262_DATA_Y_CAL           0x20
#define AS7262_DATA_O_CAL           0x24
#define AS7262_DATA_R_CAL           0x28

#define AS7262_DATA_START_ADDR      0x14


/***************************************
*      AS7262 Device Constants
***************************************/ 

#define AS7262_DEVICE_TYPE          0x40
#define AS7262_HARDWARE_VERSION     0x3E


/***************************************
*      Modes and Settings of AS7262
***************************************/ 

// Bank modes that determine what colors are measured
#define BANK_MODE_0                 0x00
#define BANK_MODE_1                 0x01
#define BANK_MODE_2                 0x02
#define BANK_MODE_3                 0x03

// Gain settings
#define GAIN_SETTING_1X             0x00
#define GAIN_SETTING_3_7X           0x01
#define GAIN_SETTING_16X            0x02
#define GAIN_SETTING_64X            0x03

// LED power settings
#define LED_POWER_12_5_mA           0x00
#define LED_POWER_25_mA             0x01
#define LED_POWER_50_mA             0x02
#define LED_POWER_100_mA            0x03


#define INDICATOR_POWER_1_mA        0X00
#define INDICATOR_POWER_2_mA        0X01
#define INDICATOR_POWER_4_mA        0X02
#define INDICATOR_POWER_8_mA        0X03


/***************************************
*      AS7262 User Commands Constants
***************************************/ 

#define READ_COMMANDS               'R'
#define SET_INTEGRATION_TIME        'I'
#define SET_GAIN                    'G'
#define SET_LED_POWER               'P'
#define LED_ON_OFF                  'L'

extern char USB_str[40];


/***************************************
*        Function Prototypes
***************************************/ 

uint8 AS7262_Init(void);
void AS7262_Start(void);


void AS7262_Commands(uint8 buffer[]);
void AS7262_SingleRead(void);


#endif

/* [] END OF FILE */
