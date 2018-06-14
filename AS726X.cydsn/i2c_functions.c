/*******************************************************************************
 * File Name: i2c_functions.c
 * Version 0.80
 *
 * Description:
 *  This file provides higher level I2C functions.
 *  A I2C master component with the name I2C is required
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

#include "i2c_functions.h"

extern char LCD_str[40];


/***************************************
*      Static Function Prototypes
***************************************/ 


static uint8 sensor_read(uint8 address, uint8* buffer, uint8 _register, uint8 num_bytes);
static uint8 sensor_write(uint8 address, uint8* buffer, uint8 num_bytes);

uint8 sensor_write8(uint8 address, uint8* buffer) {   
    return sensor_write(address, buffer, 2);
}

uint8 sensor_write16(uint8 address, uint8* buffer) {   
    return sensor_write(address, buffer, 3);
}

uint8 sensor_write_n(uint8 address, uint8* buffer, uint8 num_bytes) {   
    return sensor_write(address, buffer, num_bytes);
}

uint8 sensor_write(uint8 address, uint8* buffer, uint8 num_bytes) {   
    uint8 status = 0;
    status = I2C_MasterWriteBuf(address, buffer, num_bytes, I2C_MODE_COMPLETE_XFER);
    for(;;) {
        if (0x00 != (I2C_MasterStatus() & I2C_MSTAT_WR_CMPLT)) {
            break;
        }
    }
    return status;
}



uint8 sensor_read8(uint8 address, uint8* buffer, uint8 _register, uint8* error) {
//    LCD_Position(0, 0); 
//    LCD_PrintString("read0010   ");
    *error = sensor_read(address, buffer, _register, 1);
    return buffer[0];
}

uint16 sensor_read16(uint8 address, uint8* buffer, uint8 _register, uint8* error) {
    *error = sensor_read(address, buffer, _register, 2);
    return buffer[0] | (buffer[1] << 8);
}

uint8 sensor_read_n(uint8 address, uint8* buffer, uint8 _register, uint8 num_bytes) {
    return sensor_read(address, buffer, _register, num_bytes);
}

static uint8 sensor_read(uint8 address, uint8* buffer, uint8 _register, uint8 num_bytes) {
    uint8 temp;
//    LCD_Position(0, 0); 
//    LCD_PrintString("read0020   ");
    // I2C_MasterSendStart(address, 0);
    temp = I2C_MasterWriteBuf(address, &_register, 1, I2C_MODE_NO_STOP);
//    LCD_Position(0, 0); 
//    sprintf(LCD_str, "temp stat:%d", temp);
//    LCD_PrintString(LCD_str);
    for(;;) {
        if (0x00 != (I2C_MasterStatus() & I2C_MSTAT_WR_CMPLT)) {
            break;
        }
//        LCD_Position(0, 0); 
//        sprintf(LCD_str, "MS status:%d", I2C_MasterStatus());
        LCD_PrintString(LCD_str);
    }
    if (temp != I2C_MSTR_NO_ERROR ) {
        return temp;
    }
    
    I2C_MasterClearWriteBuf();

    do {
        temp = I2C_MasterReadBuf(address, buffer, num_bytes, I2C_MODE_REPEAT_START);
        for(;;) {
            if (0x00 != (I2C_MasterStatus() & I2C_MSTAT_RD_CMPLT)) {
                break;
            }
        }
    }
    while (temp != I2C_MSTR_NO_ERROR);
    I2C_MasterClearReadBuf();
}



/* [] END OF FILE */
