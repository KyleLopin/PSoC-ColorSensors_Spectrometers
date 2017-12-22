/*******************************************************************************
 * File Name: AS7262.c
 * Version 0.20
 *
 * Description:
 *  This file provides functions to control an AS7262 color sensor through the I2C.
 *  This file depends on i2c_functions to worik
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


#define AS72XX_SLAVE_STATUS_REG 0x00
#define AS72XX_SLAVE_WRITE_REG 0x01
#define AS72XX_SLAVE_READ_REG 0x02
#define AS72XX_SLAVE_TX_VALID   0x02
#define AS72XX_SLAVE_RX_VALID   0x01
uint8 read_buffer[24];
uint8 write_buffer[2];

/***************************************
*      Function Prototypes
***************************************/

uint8 virtual_reg_read(uint8 reg_addr);
uint8 I2C_AS7262_Read(uint8 reg_addr);
uint8 I2C_AS7262_ReadRegister(uint8 reg_addr);
uint8 I2C_AS7262_Write(uint8 reg_addr, uint8 value);
uint8 I2C_AS7262_WriteRegister(uint8 reg_addr, uint8 value);
void AS7262_ReadAllData(void);
uint8 convert_hex_string(uint8 array[]);
uint8 hex_value_from_char(uint8 _char);


/***************************************
*      Initialize Structure to save 
*      settings of AS7262
***************************************/

AS7262_settings as7262 = {
    .measurement_mode = BANK_MODE_0,
    .gain = GAIN_SETTING_1X,
    .integration_time = 1,
    .LED_power_level = LED_POWER_12_5_mA,
    .LED_on = False
};



union CalibratedData {
    uint8 data_bytes[24];
    float32 calibrated_data[6];
};

union CalibratedData all_calibrated_data;

float32 color_data[6];
extern char LCD_str[40];


CY_ISR( isr_handler_basic_read ) {
    LCD_Position(1, 0);
    LCD_PrintString("SingleRead3a ");
    isr_as7262_int_Disable();
    AS7262_INT_ClearInterrupt();
    LCD_Position(1, 0);
    LCD_PrintString("SingleRead3b ");
    AS7262_ReadAllData();
    LCD_Position(1, 0);
    LCD_PrintString("SingleRead4 ");
    USB_Export_Data(all_calibrated_data.data_bytes, 24);
}


/******************************************************************************
* Function Name: as7262_Init
*******************************************************************************
*
* Summary:
*  Set up the AS7262 with the default configuration.
*
*******************************************************************************/

uint8 AS7262_Init(void) {
    LCD_Position(1, 0); 
    LCD_PrintString("check4bpre   ");
    uint8 data = I2C_AS7262_Read(AS726x_REG_DEVICE_TYPE);
    as7262.device_type = data;
    LCD_Position(1, 0); 
    LCD_PrintString("check4b   ");
    data = I2C_AS7262_Read(AS726x_REG_HW_VERSION);
    as7262.hw_version = data;
    data = I2C_AS7262_Read(AS726x_REG_FW1_VERSION);
    as7262.fw_version = ( data << 8 );
    data = I2C_AS7262_Read(AS726x_REG_FW2_VERSION);
    as7262.fw_version += data;
    I2C_AS7262_Write( AS726x_REG_CONTROL, 0x4C );
    
    AS7262_ReadAllData();
    
    data = I2C_AS7262_Read(AS726x_REG_CONTROL);
    LCD_Position(0, 0); 
    sprintf(LCD_str, "Cod2: 0x%02x", data);
    LCD_PrintString(LCD_str);
    
    if ( as7262.device_type == AS7262_DEVICE_TYPE ) {
        return true;
    }
    return false;
}

void AS7262_Commands(uint8 buffer[]) {
    switch ( buffer[7] ) {
        case READ_COMMANDS: ;  // a command to read 'READ' data is given, or register commands 'REG'
            if ( buffer[9] == 'G' ) {  // Register read or write
                if ( buffer[11] == 'W' ) {  // Write a register value
                    uint8 reg_to_write = convert_hex_string(&buffer[19]);
                    uint8 value_to_write = convert_hex_string(&buffer[24]);
                    I2C_AS7262_Write(reg_to_write, value_to_write);
                    LCD_Position(0, 0); 
                    sprintf(LCD_str, "R:0x%02x|v:0x%02x  ", reg_to_write, value_to_write);
                    LCD_PrintString(LCD_str);
                    LCD_Position(1, 0); 
                    sprintf(LCD_str, "B:%c|v:%c  ", buffer[24], buffer[25]);
                    LCD_PrintString(LCD_str);
                    
                }
                else if ( buffer[11] == 'R' ) {  // Read a value from a register
                    uint8 reg_to_read = convert_hex_string(&buffer[18]);
                    uint8 reg_value = I2C_AS7262_Read(reg_to_read);
                    sprintf(USB_str, "REG_VALUE|0x%02X", reg_value);
                    LCD_Position(0, 0); 
                    sprintf(LCD_str, "RegR:0x%02x|v:0x%02x  ", reg_to_read, reg_value);
                    // sprintf(LCD_str, "RegR:0x%d|v:0x%d  ", buffer[18], buffer[19]);
                    LCD_PrintString(LCD_str);
                    USB_Export_Data((uint8*)USB_str, 14);
                }
            }
            if ( buffer[12] == 'S' ) {  // get a single data point
                LCD_Position(0, 0);
                LCD_PrintString("Single Read");
                AS7262_SingleRead();
            }
            break;
    }
}


void AS7262_SingleRead(void) {
    // send command to run a single shot read
    LCD_ClearDisplay();
    LCD_Position(0, 0);
    LCD_PrintString("SingleRead  ");
    I2C_AS7262_Write( AS726x_REG_CONTROL, 0x4C );
    LCD_Position(0, 0);
    LCD_PrintString("SingleRead2  ");
    
    CyDelay(1600);
    uint8 data = I2C_AS7262_Read(AS726x_REG_CONTROL);
    LCD_Position(1, 0); 
    sprintf(LCD_str, "Code: 0x%02x  ", data);
    LCD_PrintString(LCD_str);
    //isr_as7262_int_StartEx( isr_handler_basic_read );
    //CyDelay(6);
    AS7262_ReadAllData();
    USB_Export_Data(all_calibrated_data.data_bytes, 24);
}

void AS7262_ReadAllData(void) {
    for (uint8 i=0; i < 24; i++ ) {
        all_calibrated_data.data_bytes[i] = I2C_AS7262_Read(AS7262_DATA_START_ADDR + i);
    }
    LCD_Position(0, 0);
    sprintf(LCD_str, "C:0x%02x|0x%02x  ", all_calibrated_data.data_bytes[0], all_calibrated_data.data_bytes[1]);
    LCD_PrintString(LCD_str);
    
}


uint8 I2C_AS7262_Read(uint8 reg_addr) {
    uint8 status;
//    LCD_Position(1, 0); 
//    LCD_PrintString("read0   ");
    status = I2C_AS7262_ReadRegister(AS72XX_SLAVE_STATUS_REG);
//    LCD_Position(1, 0); 
//    LCD_PrintString("read0b   ");
    if ( ( status & AS72XX_SLAVE_RX_VALID ) != 0 ) {  // data is trying ready to read, but it was not read before
        uint8 data = I2C_AS7262_ReadRegister(reg_addr);
    }
//    LCD_Position(1, 0); 
//    LCD_PrintString("read1   ");
//    
//    I2C_AS7262_WriteRegister(AS72XX_SLAVE_WRITE_REG, ( reg_addr | 0x80 ) );
    
    // Wait for the write flag to clear
    while(1) {
        status = I2C_AS7262_ReadRegister( AS72XX_SLAVE_STATUS_REG );
        if( ( status & AS72XX_SLAVE_TX_VALID ) == 0 ) {
            break;
        }
        CyDelay(1);
    }
    I2C_AS7262_WriteRegister( AS72XX_SLAVE_WRITE_REG, reg_addr );
//    LCD_Position(1, 0); 
//    LCD_PrintString("read2   ");
    // Wait for read flag to be set
    while(1) {
        status = I2C_AS7262_ReadRegister(AS72XX_SLAVE_STATUS_REG);
        // Check if the status register has the data read bit set
        if ( ( status & AS72XX_SLAVE_RX_VALID ) != 0 ) {  
            break;
        }
        CyDelay(1);
//        LCD_Position(0, 0); 
//        sprintf(LCD_str, "Code9: %d", status);
//        LCD_PrintString(LCD_str);
    }

    return I2C_AS7262_ReadRegister(AS72XX_SLAVE_READ_REG);
}

uint8 I2C_AS7262_ReadRegister(uint8 reg_addr) {
    uint8 status;
//    LCD_Position(0, 0); 
//    LCD_PrintString("read00   ");
    status = sensor_read8(AS7262_ADDRESS, read_buffer, reg_addr, &as7262.comm_error);
//    LCD_Position(0, 0); 
//    LCD_PrintString("read01   ");
    return read_buffer[0];
}

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


uint8 I2C_AS7262_WriteRegister(uint8 reg_addr, uint8 value) {
    uint8 status;
    write_buffer[0] = reg_addr;
    write_buffer[1] = value;
    return sensor_write8(AS7262_ADDRESS, write_buffer);
}

uint8 convert_hex_string(uint8 array[]) {
    uint8 num = hex_value_from_char(array[0]) << 4;
    num |= hex_value_from_char(array[1]);
    return num;
    
}

uint8 hex_value_from_char(uint8 _char) {
    if ( ('0' <= _char) && (_char <= '9') ) {  // char is between 0-9
        return (_char - '0');
    }
    else if ( ('A' <= _char) && (_char <= 'F') ) {  // char is between A-F
        return (10 + _char - 'A');
    }
    else if ( ('a' <= _char) && (_char <= 'f') ) {  // char is between a-f
        return (10 + _char - 'a');
    }
    return 0;
}


/* [] END OF FILE */