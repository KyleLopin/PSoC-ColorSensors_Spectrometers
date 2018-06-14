/*******************************************************************************
 * File Name: AS7262.c
 * Version 0.20
 *
 * Description:
 *  This file provides functions to control an AS7262 color sensor through the I2C.
 *  This file depends on i2c_functions to work
 *
*******************************************************************************
 * Copyright by Kyle Lopin, Naresuan University, 2017
 * All Rights Reserved
 * UNPUBLISHED, LICENSED SOFTWARE.
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION
 * WHICH IS THE PROPERTY OF Naresuan University.
 *
 * ========================================
*/

#include "AS726X.h"
// standard libraries
#include "stdio.h"
// local files
#include "usb_functions.h"

/***************************************
*      Static Function Prototypes
***************************************/

static void Select_Channel(uint8 channel);
uint8 AS726X_Init_CheckPin(AS726X_settings* as726x);

static uint8 update_control_reg_value(AS726X_settings* as726x_ptr);
static uint8 update_LED_reg_value(AS726X_settings* as726x_ptr);

static uint8 virtual_reg_read(uint8 reg_addr);
static uint8 I2C_AS726X_Read(uint8 reg_addr, AS726X_settings* as7262x);
static uint8 I2C_AS726X_ReadRegister(uint8 reg_addr, AS726X_settings* as7262x);
static uint8 I2C_AS726X_Write(uint8 reg_addr, uint8 value);
static uint8 I2C_AS726X_WriteRegister(uint8 reg_addr, uint8 value);
static void AS7262_ReadAllData(void);

/***************************************
*      Initialize Structure to save 
*      settings of AS7262 and AS7263
***************************************/

AS726X_settings as7262 = {
    .measurement_mode = AS726x_BANK_MODE_3,
    .gain = AS726x_GAIN_SETTING_1X,
    .interrupt_on = True,
    .integration_time = 255,
    .LED_power_level = AS726x_LED_POWER_12_5_mA,
    .LED_on = False,
    .indicator_power_level = AS726x_INDICATOR_POWER_4_mA,
    .indicator_on = False
};

AS726X_settings as7263 = {
    .measurement_mode = AS726x_BANK_MODE_3,
    .gain = AS726x_GAIN_SETTING_1X,
    .interrupt_on = True,
    .integration_time = 255,
    .LED_power_level = AS726x_LED_POWER_12_5_mA,
    .LED_on = False,
    .indicator_power_level = AS726x_INDICATOR_POWER_4_mA,
    .indicator_on = False
};

static uint8 read_buffer[2];
static uint8 write_buffer[24];

/******************************************************************************
* Function Name: AS726X_Init
*******************************************************************************
*
* Summary:
*  Check the 2 SDA pins to see if there are AS7262 or AS7263 devices connected to them
*
* Return:
*  true (1) if the AS7262 responds with the correct device type
*
* Global variables:
*  AS7262_settings as7262:  strucutre that stores a local copy of all the parameters it is set to
*
*******************************************************************************/

uint8 AS726X_Init(void) {
    // check if a device is connected to SDA pin 0
    Select_Channel(0);
    // check if a device is found there, first pass it the as7262 setting, and if it is correct leave it
    // and if the as7263 is connected to it, change the structure over
    uint8 device_found_0 = AS726X_Init_CheckPin(&as7262);
    if (device_found_0 == True) {
        // A device was found, se
        if ( as7262.device_type == AS7263_HARDWARE_VERSION ) {
            // the as7263 was found on pin 0 so alias the device name
            as7263 = as7262;
            as7263.I2C_channel = 0;
        }
        else if ( as7262.device_type == AS7262_HARDWARE_VERSION ) {
            // as7262 is on the SDA 0 channel pin
            as7262.I2C_channel = 0;
        }
    }
    
     // check if a device is connected to SDA pin 0
    Select_Channel(0);
    
    
}

static void Select_Channel(uint8 channel) {
    switch (channel) {
        case I2C_BUS0_ACTIVE:
            /* Set drive mode to Digital High-Z to not imapct BUS 0 */ 
            SDA_1_SetDriveMode(PIN_DM_DIG_HIZ);
            SCL_1_SetDriveMode(PIN_DM_DIG_HIZ);
            
            /* Enable BUS 0 to drive the bus */ 
            BusSelect_Write(I2C_BUS0_ACTIVE);
            
            /* Set drive mode to Open Drain Drives Low to drive BUS 0 */ 
            SDA_0_SetDriveMode(PIN_DM_OD_LO);
            SCL_0_SetDriveMode(PIN_DM_OD_LO);
            break;
        case I2C_BUS1_ACTIVE:
            /* Set drive mode to Digital High-Z to not imapct BUS 1 */ 
            SDA_0_SetDriveMode(PIN_DM_DIG_HIZ);
            SCL_0_SetDriveMode(PIN_DM_DIG_HIZ);
            
            /* Enable BUS 1 to drive the bus */ 
            BusSelect_Write(I2C_BUS1_ACTIVE);
            
            /* Set drive mode to Open Drain Drives Low to drive BUS 1 */
            SDA_1_SetDriveMode(PIN_DM_OD_LO);
            SCL_1_SetDriveMode(PIN_DM_OD_LO);
            break;
        default:
            /* Do nothing: incorrect bus index */
            break;
    }
}

uint8 AS726X_Init_CheckPin(AS726X_settings* as726x) {
    // get the hardware and firmware data and save it to the struct
    uint8 data = I2C_AS726X_Read(AS726x_REG_DEVICE_TYPE, as726x);
    
    // Check if the device responded correctly for a AS7262 or AS7263
    if ( data == AS726x_DEVICE_TYPE  ) {
        return False;
    }
    
    // The device is AS726X so get the infomation
    as726x->device_type = data;
    data = I2C_AS726X_Read(AS726x_REG_HW_VERSION, as726x);
    as726x->hw_version = data;
    data = I2C_AS726X_Read(AS726x_REG_FW1_VERSION, as726x);
    as726x->fw_version = ( data << 8 );
    data = I2C_AS726X_Read(AS726x_REG_FW2_VERSION, as726x);
    as726x->fw_version += data;
    
    // update the control,led, and integration time registers to their default starting values
    update_control_reg_value( as726x );
    update_LED_reg_value( as726x );
    I2C_AS726X_Write(AS726x_REG_INT, as7262.integration_time);
    
    // if the device responeded with the correct device type return true to symbolize the device is connected
    return True;

}


/******************************************************************************
* Function Name: I2C_AS7262_Read
*******************************************************************************
*
* Summary:
*  Read a register from a AS7262 light sensor.  This function implements the
*  virtual register I2C scheme used in the data sheet. 
*
* Parameters:
*  uint8 reg_addr:  register address to read
*
* Return:
*  uint8:  data that was stored in the register requested
*
*******************************************************************************/

uint8 I2C_AS726X_Read(uint8 reg_addr, AS726X_settings* as7262x) {
    uint8 status;
//    LCD_Position(1, 0); 
//    LCD_PrintString("read0   ");
    status = I2C_AS726X_ReadRegister(AS72XX_SLAVE_STATUS_REG, as726x);
//    LCD_Position(1, 0); 
//    LCD_PrintString("read0b   ");
    if ( ( status & AS72XX_SLAVE_RX_VALID ) != 0 ) {  // data is trying ready to read, but it was not read before
        uint8 data = I2C_AS726X_ReadRegister(reg_addr);
    }
//    LCD_Position(1, 0); 
//    LCD_PrintString("read1   ");
//    
//    I2C_AS7262_WriteRegister(AS72XX_SLAVE_WRITE_REG, ( reg_addr | 0x80 ) );
    
    // Wait for the write flag to clear
    while(1) {
        status = I2C_AS726X_ReadRegister( AS72XX_SLAVE_STATUS_REG );
        if( ( status & AS72XX_SLAVE_TX_VALID ) == 0 ) {
            break;
        }
        CyDelay(1);
    }
    I2C_AS726X_WriteRegister( AS72XX_SLAVE_WRITE_REG, reg_addr );
//    LCD_Position(1, 0); 
//    LCD_PrintString("read2   ");
    // Wait for read flag to be set
    while(1) {
        status = I2C_AS726X_ReadRegister(AS72XX_SLAVE_STATUS_REG);
        // Check if the status register has the data read bit set
        if ( ( status & AS72XX_SLAVE_RX_VALID ) != 0 ) {  
            break;
        }
        CyDelay(1);
//        LCD_Position(0, 0); 
//        sprintf(LCD_str, "Code9: %d", status);
//        LCD_PrintString(LCD_str);
    }

    return I2C_AS726X_ReadRegister(AS72XX_SLAVE_READ_REG);
}

/******************************************************************************
* Function Name: I2C_AS7262_ReadRegister
*******************************************************************************
*
* Summary:
*  Read an actual register from a AS7262 light sensor, not a virtual register.  
*  Should only be used for STATUS or READ register.  Uses the sensor_read8 
*  function of the i2c_functions.c to work.
*
* Parameters:
*  uint8 reg_addr:  register address to read, should be STATUS (0x00)
*                   or READ (0x02)
*
* Global variables:
*  uint8[] read_buffer:  array used by the i2c_functions to store the data.
* 
* Return:
*  uint8:  data that was stored in the register requested
*
*******************************************************************************/

uint8 I2C_AS726X_ReadRegister(uint8 reg_addr, AS726X_settings* as7262x) {
    uint8 status;
    status = sensor_read8(AS726X_ADDRESS, read_buffer, reg_addr, &as726x.comm_error);
    return read_buffer[0];
}





/* [] END OF FILE */
