/*********************************************************************************
* File Name: usb_functions.c
*
* Description:
*  Source code for the protocols used by the USB.
*
*
**********************************************************************************
 * Copyright Naresuan University, Phitsanulok Thailand
 * Released under Creative Commons Attribution-ShareAlike  3.0 (CC BY-SA 3.0 US)
*********************************************************************************/

#include <project.h>
#include "usb_functions.h"


/******************************************************************************
* Function Name: USB_Start
*******************************************************************************
*
* Summary:
*  Power up the USB get the configuration and enable to the OUT ENDPOINT so 
*  the device can recieve instructions.
*
*******************************************************************************/

void USB_Start(void) {
    
    USBFS_Start(0, USBFS_DWR_VDDD_OPERATION);  // initialize the USB
    LCD_Position(1, 0); 
    LCD_PrintString("check1a   ");
    while(!USBFS_bGetConfiguration());  //Wait till it the usb gets its configuration from the PC
    LCD_Position(1, 0); 
    LCD_PrintString("check1b   ");
    USBFS_EnableOutEP(OUT_ENDPOINT);
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
    
    if(USBFS_GetEPState(OUT_ENDPOINT) == USBFS_OUT_BUFFER_FULL) {
        
        /* There is data coming in, get the number of bytes*/
        uint8 OUT_COUNT = USBFS_GetEPCount(OUT_ENDPOINT);
        /* Read the OUT endpoint and store data in OUT_Data_buffer */
        USBFS_ReadOutEP(OUT_ENDPOINT, buffer, OUT_COUNT);
        /* Re-enable OUT endpoint */
        USBFS_EnableOutEP(OUT_ENDPOINT);
//        LCD_Position(1,0);
//        //sprintf(LCD_str, "%c%c%c%c%c%c%c%c", buffer[16], buffer[17],buffer[18], buffer[19], buffer[20],buffer[21], buffer[22], buffer[23]);
//        sprintf(LCD_str, "Out count:%d", OUT_COUNT);
//        LCD_PrintString((char*)buffer);
//        LCD_PrintString(LCD_str);
        
        return true;
    }
    
    return false;
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
//    LCD_Position(1,0);
//    LCD_PrintString("Export_in1");
    for (int i=0; i < size; i=i+MAX_IN_ENDPOINT_BUFFER_SIZE) {
        //LCD_Position(1,0);
        //LCD_PrintString("Export_in2");
//        LCD_Position(1,0);
//        LCD_PrintString((char*)array);
        
        while(USBFS_GetEPState(IN_ENDPOINT) != USBFS_IN_BUFFER_EMPTY)
        {
        }
        //LCD_Position(1,0);
        //LCD_PrintString("Export_in3");
        uint16 size_to_send = size - i;
        if (size_to_send > MAX_IN_ENDPOINT_BUFFER_SIZE) {
            size_to_send = MAX_IN_ENDPOINT_BUFFER_SIZE;
        }
        if(USBFS_GetEPState(IN_ENDPOINT) == USBFS_IN_BUFFER_EMPTY){
            USBFS_LoadInEP(IN_ENDPOINT, &array[i], size_to_send);  // TODO: Fix this
            USBFS_EnableOutEP(OUT_ENDPOINT);
        }
        //LCD_Position(1,0);
        //LCD_PrintString("Export_in4");
    }
}


/* [] END OF FILE */