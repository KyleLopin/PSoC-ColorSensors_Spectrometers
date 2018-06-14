/*******************************************************************************
 * File Name: AS7262.c
 * Version 0.50
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

#include "AS7262.h"
// standard libraries
#include "stdio.h"
// local files
#include "usb_functions.h"

#define AS72XX_SLAVE_STATUS_REG         0x00
#define AS72XX_SLAVE_WRITE_REG          0x01
#define AS72XX_SLAVE_READ_REG           0x02
#define AS72XX_SLAVE_TX_VALID           0x02
#define AS72XX_SLAVE_RX_VALID           0x01
uint8 read_buffer[24];
uint8 write_buffer[2];

/***************************************
*      Function Prototypes
***************************************/

uint8 update_control_reg_value(AS7262_settings* as7262_ptr);
uint8 update_LED_reg_value(AS7262_settings* as7262_ptr);

uint8 virtual_reg_read(uint8 reg_addr);
uint8 I2C_AS7262_Read(uint8 reg_addr);
uint8 I2C_AS7262_ReadRegister(uint8 reg_addr);
uint8 I2C_AS7262_Write(uint8 reg_addr, uint8 value);
uint8 I2C_AS7262_WriteRegister(uint8 reg_addr, uint8 value);
void AS7262_ReadAllData(void);
uint8 convert_hex_string(uint8 array[]);
uint8 hex_value_from_char(uint8 _char);
uint16 ConvertDecimal(uint8 array[], uint8 len);


/***************************************
*      Initialize Structure to save 
*      settings of AS7262
***************************************/

AS7262_settings as7262 = {
    .measurement_mode = BANK_MODE_3,
    .gain = GAIN_SETTING_1X,
    .interrupt_on = True,
    .integration_time = 255,
    .LED_power_level = LED_POWER_12_5_mA,
    .LED_on = False,
    .indicator_power_level = INDICATOR_POWER_4_mA,
    .indicator_on = False
};



union CalibratedData {
    uint8 data_bytes[24];
    float32 calibrated_data[6];
};

union CalibratedData all_calibrated_data;

float32 color_data[6];
extern char LCD_str[40];


CY_ISR( isr_handler_basic_read ) {
    // isr_as7262_int_Disable();
    AS7262_INT_ClearInterrupt();
}

CY_ISR( isr_handler_debug1 ) {
    LED_3_Write( 0 );
    LCD_Position(1, 0);
    LCD_PrintString("debug1 ");
    //AS7262_INT_ClearInterrupt();
}

/******************************************************************************
* Function Name: AS7262_Init
*******************************************************************************
*
* Summary:
*  Set up the AS7262 with the default configuration.
*
* Return:
*  true (1) if the AS7262 responds with the correct device type
*
* Global variables:
*  AS7262_settings as7262:  strucutre that stores a local copy of all the parameters it is set to
*
*******************************************************************************/

uint8 AS7262_Init(void) {
    // get the hardware and firmware data and save it to the struct
    uint8 data = I2C_AS7262_Read(AS726x_REG_DEVICE_TYPE);
    as7262.device_type = data;
    data = I2C_AS7262_Read(AS726x_REG_HW_VERSION);
    as7262.hw_version = data;
    data = I2C_AS7262_Read(AS726x_REG_FW1_VERSION);
    as7262.fw_version = ( data << 8 );
    data = I2C_AS7262_Read(AS726x_REG_FW2_VERSION);
    as7262.fw_version += data;
    
    // update the control,led, and integration time registers to their default starting values
    update_control_reg_value( &as7262 );
    update_LED_reg_value( &as7262 );
    I2C_AS7262_Write(AS726x_REG_INT, as7262.integration_time);
    
    // if the device responeded with the correct device type return true to symbolize the device is connected
    if ( as7262.device_type == AS7262_DEVICE_TYPE ) {
        return true;
    }
    return false;
}

/******************************************************************************
* Function Name: AS7262_Commands
*******************************************************************************
*
* Summary:
*  Take in User inputs that deal with the AS7262 and peform the proper action.
*
* Parameters:
*  uint8 buffer: array of the chars the user inputted
*
* Global variables:
*  AS7262_settings as7262:  strucutre that stores a local copy of all the parameters it is set to
*
*******************************************************************************/

void AS7262_Commands(uint8 buffer[]) {
    switch ( buffer[7] ) {
        case READ_COMMANDS: ;  // a command to read 'READ' data is given, or register commands 'REG'
            if ( buffer[9] == 'G' ) {  // Register read or write
                if ( buffer[11] == 'W' ) {  // Write a register value
                    uint8 reg_to_write = convert_hex_string(&buffer[19]);
                    uint8 value_to_write = convert_hex_string(&buffer[24]);
                    I2C_AS7262_Write(reg_to_write, value_to_write);
                }
                else if ( buffer[11] == 'R' ) {  // Read a value from a register
                    uint8 reg_to_read = convert_hex_string(&buffer[18]);
                    uint8 reg_value = I2C_AS7262_Read(reg_to_read);
                    sprintf(USB_str, "REG_VALUE|0x%02X", reg_value);
                    USB_Export_Data((uint8*)USB_str, 14);
                }
            }
            if ( buffer[12] == 'S' ) {  // get a single data point
                
                if (  buffer[19] == 'F' ) { 
                    // turn on LED
                    uint8 led_reg_hold = as7262.LED_control_reg_value;
                    uint led_reg_on = led_reg_hold;
                    led_reg_on |= 0x08;
                    I2C_AS7262_Write( AS726x_REG_LED, led_reg_on );
                    
                    uint8 led_reg = I2C_AS7262_Read( AS726x_REG_LED );
                    
                    if (led_reg != led_reg_on)  {  // LED is not flashing properly so just return
                        break;
                    }
                    // run the read
                    AS7262_SingleRead();
                    
                    I2C_AS7262_Write( AS726x_REG_LED, led_reg_hold );
                }
                else if ( buffer[19] == 'N' ) { 
                    AS7262_SingleRead();
                }
            }
            break;
        case SET_INTEGRATION_TIME: ; // the command to set the integration time
            uint8 new_integration_time = ConvertDecimal(&buffer[22],3);
            as7262.integration_time = new_integration_time;
            I2C_AS7262_Write( AS726x_REG_INT, as7262.integration_time );
            break;
            
        case SET_GAIN: ; // change the gain setting of the device
            as7262.gain = ( ConvertDecimal(&buffer[12], 1) & 0x03 );  // only allow values from 0x00-0x03
            update_control_reg_value( &as7262 );
            
            break;
        case SET_LED_POWER: ; // set the current through the main light source
            as7262.LED_power_level = ( ConvertDecimal(&buffer[19], 1) & 0x03 );  // only allow values from 0x00-0x03
            update_LED_reg_value( &as7262 );
            break;
        case LED_ON_OFF: ;  //  set if the LED should be on or off
            if ( (buffer[17] == 'N') || (buffer[17] == 'n') ) {
                as7262.LED_on = True;
            }
            else if ( (buffer[17] == 'F') || (buffer[17] == 'f') ) {
                as7262.LED_on = False;
            }
            update_LED_reg_value( &as7262 );
            break;
    }
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

uint8 update_control_reg_value(AS7262_settings* as7262_ptr) {
    uint8 control_reg = (as7262_ptr->interrupt_on << 6) | 
                        (as7262_ptr->gain << 4) | 
                        (as7262_ptr->measurement_mode << 2);
    I2C_AS7262_Write( AS726x_REG_CONTROL, control_reg );
    as7262_ptr->control_reg_value = control_reg;
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

uint8 update_LED_reg_value(AS7262_settings* as7262_ptr) {
    uint8 led_control_reg = (as7262_ptr->LED_power_level << 4) | 
                            (as7262_ptr->LED_on << 3) | 
                            (as7262_ptr->indicator_power_level << 1) |
                            (as7262_ptr->indicator_on);
    I2C_AS7262_Write( AS726x_REG_LED, led_control_reg );
    as7262_ptr->LED_control_reg_value = led_control_reg;
    return led_control_reg;
}

/******************************************************************************
* Function Name: AS7262_SingleRead
*******************************************************************************
*
* Summary:
*  Read all 6 calibrated data points from a AS7262 device.  This function writes
*  to the control register so the device will start reading, waits for  
*  the integration time needed for the mode the device is set in, then reads 
*  the device and exports the data.
*
* Parameters:
*  
*
* Global variables:
*  AS7262_settings as7262:  strucutre that stores a local copy of all 
*  the parameters it is set to
*
*******************************************************************************/

void AS7262_SingleRead(void) {
    // send command to run a single shot read
    I2C_AS7262_Write( AS726x_REG_CONTROL, as7262.control_reg_value );
    uint pre_data = I2C_AS7262_Read( AS726x_REG_CONTROL );
    
    if ( pre_data != as7262.control_reg_value ) {  // the setting were not set correctly so send back 0's to indicate error
        for (uint8 i=0; i < 6; i++) {
            all_calibrated_data.calibrated_data[i] = 0.0;
        }
        USB_Export_Data(all_calibrated_data.data_bytes, 24);
        return;
    }

    float integ_time;
    if ( as7262.measurement_mode == BANK_MODE_0 || as7262.measurement_mode == BANK_MODE_1) {
        integ_time = 2.8;  // milliseconds
    }
    else {
        integ_time = 5.8;  // milliseconds
    }
    CyDelay( (integ_time * as7262.integration_time) );
    uint8 data;
    uint8 failed_tries=0;
    do {
        CyDelay(1);
        data = I2C_AS7262_Read( AS726x_REG_CONTROL );
        failed_tries++;
        if ( failed_tries > 200 ) {
            USB_Export_Data((uint8*)"FAILED", 6);
        }
    } while ( (data & 0x02) == 0x00 );
    
    AS7262_ReadAllData();
   
    USB_Export_Data(all_calibrated_data.data_bytes, 24);

}

/******************************************************************************
* Function Name: AS7262_ReadAllData
*******************************************************************************
*
* Summary:
*  Read all 6 calibrated data points from a AS7262 device.  This function should
*  only be called if the device has already been started and the data is ready for
*  export.
*
* Parameters:
*  
*
* Global variables:
*  AS7262_settings as7262:  strucutre that stores a local copy of all 
*  the parameters it is set to
*
*******************************************************************************/

void AS7262_ReadAllData(void) {
    for (uint8 i=0; i < 24; i++ ) {
        all_calibrated_data.data_bytes[i] = I2C_AS7262_Read(AS7262_DATA_START_ADDR + i);
    }
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

uint8 I2C_AS7262_Read(uint8 reg_addr) {
    uint8 status;
    status = I2C_AS7262_ReadRegister(AS72XX_SLAVE_STATUS_REG);

    if ( ( status & AS72XX_SLAVE_RX_VALID ) != 0 ) {  // data is trying ready to read, but it was not read before
        I2C_AS7262_ReadRegister(reg_addr);
    }
    // Wait for the write flag to clear
    while(1) {
        status = I2C_AS7262_ReadRegister( AS72XX_SLAVE_STATUS_REG );
        if( ( status & AS72XX_SLAVE_TX_VALID ) == 0 ) {
            break;
        }
        CyDelay(1);
    }
    I2C_AS7262_WriteRegister( AS72XX_SLAVE_WRITE_REG, reg_addr );

    // Wait for read flag to be set
    while(1) {
        status = I2C_AS7262_ReadRegister(AS72XX_SLAVE_STATUS_REG);
        // Check if the status register has the data read bit set
        if ( ( status & AS72XX_SLAVE_RX_VALID ) != 0 ) {  
            break;
        }
        CyDelay(1);
    }

    return I2C_AS7262_ReadRegister(AS72XX_SLAVE_READ_REG);
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

uint8 I2C_AS7262_ReadRegister(uint8 reg_addr) {
    uint8 status;
    status = sensor_read8(AS7262_ADDRESS, read_buffer, reg_addr, &as7262.comm_error);
    return status;
}

/******************************************************************************
* Function Name: I2C_AS7262_Write
*******************************************************************************
*
* Summary:
*  Write a register from a AS7262 light sensor.  This function implements the
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

uint8 I2C_AS7262_Write(uint8 reg_addr, uint8 value) {
    uint8 status;
    // Check to see if write buffer is ready
    while(1) {
        status = I2C_AS7262_ReadRegister( AS72XX_SLAVE_STATUS_REG );
        if( ( status & AS72XX_SLAVE_TX_VALID ) == 0 ) {
            break;  // no data inbound so it can be written to
        }
    }
    // Send the virtual register address and set bit 7 to indicate a pending write
    I2C_AS7262_WriteRegister( AS72XX_SLAVE_WRITE_REG, ( reg_addr | AS726x_WRITE_REG_MASK ) );
    // Wait for the write flag to clear
    while(1) {
        status = I2C_AS7262_ReadRegister( AS72XX_SLAVE_STATUS_REG );
        if( ( status & AS72XX_SLAVE_TX_VALID ) == 0 ) {
            break;  // no data inbound so it can be written to
        }
    }
    
    return I2C_AS7262_WriteRegister(AS72XX_SLAVE_WRITE_REG, value);
}

/******************************************************************************
* Function Name: I2C_AS7262_ReadRegister
*******************************************************************************
*
* Summary:
*  Read an actual register from a AS7262 light sensor, not a virtual register.  
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

uint8 I2C_AS7262_WriteRegister(uint8 reg_addr, uint8 value) {
    write_buffer[0] = reg_addr;
    write_buffer[1] = value;
    return sensor_write8(AS7262_ADDRESS, write_buffer);
}

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
