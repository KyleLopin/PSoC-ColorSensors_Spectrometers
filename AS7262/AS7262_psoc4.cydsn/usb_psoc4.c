/*********************************************************************************
* File Name: usb_functions.c
*
* Description:
*  Source code for the protocols used by the USB.
*
*
**********************************************************************************
 * Copyright Naresuan University, Phitsanulok Thailand
*********************************************************************************/


#include <project.h>
#include "usb_psoc4.h"

#define True                        1
#define False                       0


/******************************************************************************
* Function Name: USB_Start
*******************************************************************************
*
* Summary:
*  Power up the USB / UART of the CY8CKIT-044 so 
*  the device can recieve instructions.
*
*******************************************************************************/

void USB_Start(void) {
    UART_Start();
    
}
    

/******************************************************************************
* Function Name: USB_CheckInput
*******************************************************************************
*
* Summary:
*  Check if any incoming USB data and store it to the input buffer
*
* Parameters:
*  uint8 buffer: array of where the data is stored
*
* Return:
*  true (1) if data has been inputed or false (0) if no data
*
* Global variables:
*  OUT_ENDPOINT:  number that is the endpoint coming out of the computer
*
*******************************************************************************/

uint8 USB_CheckInput(uint8 buffer[]) {
//    uint32 rxData;
//    uint8 index = 0;
//    // UART_UartPutString("Hello\r\n");
//    do {
//        rxData = UART_UartGetChar();
//        //UART_UartPutChar(rxData);
//        //buffer[index] = (rxData & 0x000000FF);
//        buffer[index] = (uint8) rxData;
//        index++;
//    } while ( ( rxData != 0 ) | (index >= MAX_IN_ENDPOINT_BUFFER_SIZE) );
//    
//    if ( index > 1 ) {
//        return True;
//    }
//    return False;
    uint32 rxData;
    
    rxData = UART_UartGetChar();
    uint8 index = 0;
    if (rxData != 0x00) {
        Red_LED_Write(0);
        Green_LED_Write(0);
        while ( (rxData != 0x00) && (index < MAX_IN_ENDPOINT_BUFFER_SIZE) && (rxData != 0x2D)) {
            //UART_UartPutChar(rxData);
            buffer[index] = rxData;
            rxData = UART_UartGetChar();
            index++;
        }
        Red_LED_Write(1);
    }
    if (index > 0) {
        return True;
    }
    return False;  
}
    
/******************************************************************************
* Function Name: USB_Export_Data
*************************************************************************************
*
* Summary:
*  Take a buffer as input and export it, the size of bytes to send is also inputted
*
* Parameters:
*  uint8 array array: array of data to export
*  uint16 size: the number of bytes to send in the array
*
* Return:
*  None
*
* Global variables:
*  MAX_BUFFER_SIZE:  the number of bytes the UBS device can hold
*
*******************************************************************************************/

void USB_Export_Data(uint8 array[], uint16 size) {
     for (int i=0; i < size; i++) {
        UART_UartPutChar(array[i]);
    }
    
    //UART_UartPutString((char*) array);
}    
    
/* [] END OF FILE */
