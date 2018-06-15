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

union CalibratedData {
    uint8 data_bytes[24];
    float32 calibrated_data[6];
};

union CalibratedData all_as7262_data;
union CalibratedData all_as7263_data;
union CalibratedData all_as726x_data;

/***************************************
*      Static Function Prototypes
***************************************/

static void AS726X_ReadColorData(AS726X_settings* as726x, uint8 flash);
static void Select_Channel(uint8 channel);
uint8 AS726X_Init_CheckPin(AS726X_settings* as726x);

static uint8 update_control_reg_value(AS726X_settings* as726x_ptr);
static uint8 update_LED_reg_value(AS726X_settings* as726x_ptr);

static uint8 virtual_reg_read(uint8 reg_addr);
static uint8 I2C_AS726X_Read(uint8 reg_addr, AS726X_settings* as726x);
static uint8 I2C_AS726X_ReadRegister(uint8 reg_addr, AS726X_settings* as726x);
static uint8 I2C_AS726X_Write(uint8 reg_addr, uint8 value, AS726X_settings* as726x);
static uint8 I2C_AS726X_WriteRegister(uint8 reg_addr, uint8 value);
static void AS726X_ReadData(union CalibratedData data, AS726X_settings* as726x);

/***************************************
*      State to tell what sensors are found
***************************************/

static uint8 found_sensors = NO_SENSORS;

/***************************************
*      Buffers the I2C can use for communication
***************************************/

static uint8 read_buffer[2];
static uint8 write_buffer[24];

/******************************************************************************
* Function Name: AS726X_Commands
*******************************************************************************
*
* Summary:
*  Take in User inputs that deal with the AS726x and peform the proper action.
*
* Parameters:
*  uint8 buffer: array of the chars the user inputted
*
* Global variables:
*  AS726X_settings as7262 and as7263:  strucutre that stores a local copy 
*  of all the parameters the color sensors are set to
*
*******************************************************************************/

void AS726x_Commands(uint8 buffer[]) {
    switch ( buffer[7] ) {
//        case AS726x_READ_COMMANDS: ;  // a command to read 'READ' data is given, or register commands 'REG'
//            if ( buffer[9] == 'G' ) {  // Register read or write
//                if ( buffer[11] == 'W' ) {  // Write a register value
//                    uint8 reg_to_write = convert_hex_string(&buffer[19]);
//                    uint8 value_to_write = convert_hex_string(&buffer[24]);
//                    I2C_AS726x_Write(reg_to_write, value_to_write);
//                }
//                else if ( buffer[11] == 'R' ) {  // Read a value from a register
//                    uint8 reg_to_read = convert_hex_string(&buffer[18]);
//                    uint8 reg_value = I2C_AS726x_Read(reg_to_read);
//                    sprintf(USB_str, "REG_VALUE|0x%02X", reg_value);
//                    USB_Export_Data((uint8*)USB_str, 14);
//                }
//            }
        case AS726x_READ_COMMANDS:  // a command to 'READ' data was given
            switch ( buffer[12] ) {
                if ( buffer[17] == '2' ) {  // AS726X|READ_AS7262|FLASH, read the as7262
                    Select_Channel( as7262.I2C_channel );
                    uint8 use_flash = False;
                    if ( buffer[19] == 'F') {
                        use_flash = True;
                    }
                    AS726X_ReadColorData(&as7262, use_flash);
                }  // not DRY, refactor
                else if ( buffer[17] == '3' ) {  // AS726X|READ_AS7263|FLASH, read the as7262
                    Select_Channel( as7263.I2C_channel );
                    uint8 use_flash = False;
                    if ( buffer[19] == 'F') {
                        use_flash = True;
                    }
                    AS726X_ReadColorData(&as7263, use_flash);
                }
                
                
            }
//            switch (  ) {
//                
//            }
    }
}

static void AS726X_ReadColorData(AS726X_settings* as726x, uint8 flash) {
    if (flash == True) {
        // turn LED on
        uint8 led_reg_hold = as7262.LED_control_reg_value;
        uint8 led_reg_on = led_reg_hold;
        led_reg_on |= 0x08;
        
        I2C_AS726X_Write( AS726x_REG_LED, led_reg_on, as726x );
        
        uint8 led_reg = I2C_AS726X_Read( AS726x_REG_LED, as726x );

        if (led_reg != led_reg_on)  {  // LED is not flashing properly so just return
            // figure out what to do here
        }
        // run a read of AS7262
        AS726X_ReadData( all_as7262_data, as726x );
        
        I2C_AS726X_Write( AS726x_REG_LED, led_reg_hold, as726x );
    }
    else {
        AS726X_ReadData( all_as7262_data, as726x );
    }
}

void AS726X_ReadData(union CalibratedData data, AS726X_settings* as726x) {
    for (uint8 i=0; i < 24; i++ ) {
        data.data_bytes[i] = I2C_AS726X_Read(AS726x_DATA_START_ADDR + i, as726x);
    }
}

void AS726x_Identify(void) {
    switch ( found_sensors ) {
        case NO_SENSORS:
            USB_Export_Data((uint8*)"No sensor", 10);
            break;
        case ONLY_AS7262:
            USB_Export_Data((uint8*)"AS7262", 8);
            break;
        case ONLY_AS7263:
            USB_Export_Data((uint8*)"AS7263", 8);
            break;
        case BOTH_AS726X:
            USB_Export_Data((uint8*)"Both AS726X", 13);
            break;
    }
}

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

uint8 AS726x_Init(void) {
    // go through both pins and assign them the as7262 and as7263 randomly and then change it if it is wrong later
    LCD_Position(1, 0); 
    LCD_PrintString("checka   ");
    Select_Channel(0);
    uint8 device_found_0 = AS726X_Init_CheckPin(&as7262);
    LCD_Position(1, 0); 
    LCD_PrintString("checkb   ");
    Select_Channel(1);
    uint8 device_found_1 = AS726X_Init_CheckPin(&as7263);
    LCD_Position(1, 0); 
    LCD_PrintString("checkc   ");
    if ( device_found_0 == True && device_found_1 == True ) {  // 2 devices were find, figure out of they are set correct
        if ( as7262.device_type == AS7262_HARDWARE_VERSION ) {
            if ( as7263.device_type == AS7263_HARDWARE_VERSION ) {
                // everything is assigned correctly
                found_sensors = BOTH_AS726X;
                as7262.I2C_channel = 0;
                as7263.I2C_channel = 1;
            }
        else if ( as7262.device_type == AS7263_HARDWARE_VERSION ) {
            if ( as7263.device_type == AS7262_HARDWARE_VERSION ) {
                // assignments are backwards so flip then
                AS726X_settings hold;
                
                hold = as7262;
                as7262 = as7263;
                as7263 = hold;
                
                found_sensors = BOTH_AS726X;
                as7262.I2C_channel = 1;
                as7263.I2C_channel = 0;
                }
            }
        }
        return True;
    }
    else if( device_found_0 == True ) {  //  as7262 was given to device 0
        if ( as7262.device_type == AS7262_HARDWARE_VERSION ) {  // everything is correct
            found_sensors = ONLY_AS7262;
            as7262.I2C_channel = 0;
        }
        else if ( as7262.device_type == AS7263_HARDWARE_VERSION ) {  
            // channel 0 has an as7263 attached to it
            found_sensors = ONLY_AS7263;
            as7263 = as7262;
            as7263.I2C_channel = 0;
        }
        return True;
    }
    else if ( device_found_1 == True ) {  //  as7263 was given to device 0
        if ( as7263.device_type == AS7263_HARDWARE_VERSION ) {  // everything is correct
            found_sensors = ONLY_AS7263;
            as7263.I2C_channel = 1;
        }
        else if ( as7263.device_type == AS7262_HARDWARE_VERSION ) {  
            // channel 0 has an as7263 attached to it
            found_sensors = ONLY_AS7262;
            as7262 = as7263;
            as7262.I2C_channel = 1;
        }
        return True;
    }
    else {
        found_sensors = NO_SENSORS;
        return False;
    } 
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
    LCD_Position(1, 0); 
    LCD_PrintString("checka1   ");
    uint8 data = I2C_AS726X_Read(AS726x_REG_DEVICE_TYPE, as726x);
    LCD_Position(1, 0); 
    LCD_PrintString("checka2   ");
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
    I2C_AS726X_Write(AS726x_REG_INT, as7262.integration_time, as726x);
    
    // if the device responeded with the correct device type return true to symbolize the device is connected
    return True;
}

/******************************************************************************
* Function Name: update_control_reg_value
*******************************************************************************
*
* Summary:
*  Update the control register of a AS7262, save a copy of its state to the
*  settings that are passed in and return a local copy of what the 
*  register state is.
*
* Parameters:
*  AS7262_settings* as7262_ptr: pointer to the settings of the light sensor (AS7262)
*
* Return:
*  uint8:  state the control register was set to
*
*******************************************************************************/

uint8 update_control_reg_value(AS726X_settings* as726x_ptr) {
    uint8 control_reg = (as726x_ptr->interrupt_on << 6) | 
                        (as726x_ptr->gain << 4) | 
                        (as726x_ptr->measurement_mode << 2);
    I2C_AS726X_Write( AS726x_REG_CONTROL, control_reg, as726x_ptr );
    as726x_ptr->control_reg_value = control_reg;
    return control_reg;
}

/******************************************************************************
* Function Name: update_LED_reg_value
*******************************************************************************
*
* Summary:
*  Update the register of a AS7262 that controls the LED, save a copy of its 
*  state to the settings that are passed in and return a local copy of what the 
*  register state is.  The LED register sets the amount of current the AS7262
*  should sink.
*
* Parameters:
*  AS7262_settings* as7262_ptr: pointer to the settings of the light sensor (AS7262)
*
* Return:
*  uint8:  state the LED register was set to
*
*******************************************************************************/

uint8 update_LED_reg_value(AS726X_settings* as726x_ptr) {
    uint8 led_control_reg = (as726x_ptr->LED_power_level << 4) | 
                            (as726x_ptr->LED_on << 3) | 
                            (as726x_ptr->indicator_power_level << 1) |
                            (as726x_ptr->indicator_on);
    I2C_AS726X_Write( AS726x_REG_LED, led_control_reg, as726x_ptr );
    as726x_ptr->LED_control_reg_value = led_control_reg;
    return led_control_reg;
}

/******************************************************************************
* Function Name: I2C_AS726x_Write
*******************************************************************************
*
* Summary:
*  Write a register from a AS726x light sensor.  This function implements the
*  virtual register I2C scheme used in the data sheet. 
*
* Parameters:
*  uint8 reg_addr:  register address to write
*
* Return:
*  uint8:  status of the write request, see error messages of PSoC
*          I2C_MasterSendStart in I2C datasheet for details
*
*******************************************************************************/

uint8 I2C_AS726X_Write(uint8 reg_addr, uint8 value, AS726X_settings* as726x) {
    uint8 status;
    // Check to see if write buffer is ready
    while(1) {
        status = I2C_AS726X_ReadRegister( AS72XX_SLAVE_STATUS_REG, as726x );
        if( ( status & AS72XX_SLAVE_TX_VALID ) == 0 ) {
            break;  // no data inbound so it can be written to
        }
    }
    // Send the virtual register address and set bit 7 to indicate a pending write
    I2C_AS726X_WriteRegister( AS72XX_SLAVE_WRITE_REG, ( reg_addr | AS726X_WRITE_REG_MASK ) );
    // Wait for the write flag to clear
    while(1) {
        status = I2C_AS726X_ReadRegister( AS72XX_SLAVE_STATUS_REG, as726x );
        if( ( status & AS72XX_SLAVE_TX_VALID ) == 0 ) {
            break;  // no data inbound so it can be written to
        }
    }
    
    return I2C_AS726X_WriteRegister(AS72XX_SLAVE_WRITE_REG, value);
}

/******************************************************************************
* Function Name: I2C_AS726x_WriteRegister
*******************************************************************************
*
* Summary:
*  Read an actual register from a AS726x light sensor, not a virtual register.  
*  Should only be used for WRITE register.  Uses the sensor_write8 
*  function of the i2c_functions.c to work.
*
* Parameters:
*  uint8 reg_addr:  register address to read, should be WRITE (0x01)
*
* Global variables:
*  uint8[] write_buffer:  array used by the i2c_functions to store data.
* 
* Return:
*  uint8:  status of the write request, see error messages of PSoC
*          I2C_MasterSendStart in I2C datasheet for details
*
*******************************************************************************/

uint8 I2C_AS726X_WriteRegister(uint8 reg_addr, uint8 value) {
    write_buffer[0] = reg_addr;
    write_buffer[1] = value;
    return sensor_write8(AS726X_ADDRESS, write_buffer);
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

uint8 I2C_AS726X_Read(uint8 reg_addr, AS726X_settings* as726x) {
    uint8 status;
    LCD_Position(1, 0); 
    LCD_PrintString("checkb1   ");
    status = I2C_AS726X_ReadRegister(AS72XX_SLAVE_STATUS_REG, as726x);
    LCD_Position(1, 0); 
    LCD_PrintString("checkb2   ");
    if ( ( status & AS72XX_SLAVE_RX_VALID ) != 0 ) {  // data is trying read, but it was not read before
        I2C_AS726X_ReadRegister(reg_addr, as726x);
    }  
//    I2C_AS7262_WriteRegister(AS72XX_SLAVE_WRITE_REG, ( reg_addr | 0x80 ) );
    
    // Wait for the write flag to clear
    while(1) {
        status = I2C_AS726X_ReadRegister( AS72XX_SLAVE_STATUS_REG, as726x);
        if( ( status & AS72XX_SLAVE_TX_VALID ) == 0 ) {
            break;
        }
        CyDelay(1);
    }
    I2C_AS726X_WriteRegister( AS72XX_SLAVE_WRITE_REG, reg_addr );

    // Wait for read flag to be set
    while(1) {
        status = I2C_AS726X_ReadRegister(AS72XX_SLAVE_STATUS_REG, as726x);
        // Check if the status register has the data read bit set
        if ( ( status & AS72XX_SLAVE_RX_VALID ) != 0 ) {  
            break;
        }
        CyDelay(1);
    }

    return I2C_AS726X_ReadRegister(AS72XX_SLAVE_READ_REG, as726x);
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

uint8 I2C_AS726X_ReadRegister(uint8 reg_addr, AS726X_settings* as726x) {
    uint8 status;
    status = sensor_read8(AS726X_ADDRESS, read_buffer, reg_addr, &as726x->comm_error);
    return status;
}

/* [] END OF FILE */
