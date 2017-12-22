/*********************************************************************************
* File Name: usb_protocols.h
*
* Description:
*  This file contains the function prototypes and constants used for
*  the USB protocols.
*
*
**********************************************************************************
 * Copyright Naresuan University, Phitsanulok Thailand
 * Released under Creative Commons Attribution-ShareAlike  3.0 (CC BY-SA 3.0 US)
*********************************************************************************/


#if !defined(USB_PROTOCOLS_H)
#define USB_PROTOCOLS_H
    
#include <project.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

/**************************************
*      Constants
**************************************/
    
#define IN_ENDPOINT                 0X01
#define OUT_ENDPOINT                0x02   
    
#define MAX_IN_ENDPOINT_BUFFER_SIZE 40

    
/**************************************
*      User Options
**************************************/    

#define IDENTIFY                    'I'
#define IDENTIFY_SPECTROMETER       'S'
#define AS726X_COMMAND              'A'
#define IDENTIFY                    'I'

#define TEST_MESSAGE                "PSoC-Spectrometer" 
#define LEN_TEST_MESSAGE            17
#define AS7262_MESSAGE              "AS7262" 
#define LEN_AS7262_MESSAGE          6
    
    
/********************************************
*        Variable to store input
********************************************/  
   
uint8 OUT_Data_Buffer[MAX_IN_ENDPOINT_BUFFER_SIZE];
char USB_str[40];  // holder for making strings to export 
    
/* External variable of the device address located in USBFS.h */
extern uint8 USB_deviceAdress;
    
extern char LCD_str[];  // for debug
    
/***************************************
*        Function Prototypes
***************************************/  

void USB_Start(void);
uint8 USB_CheckInput(uint8 buffer[]);
void USB_Export_Data(uint8 array[], uint16 size);
void USB_Export_Ctrl_Info(uint8* message, uint16 size);

#endif

/* [] END OF FILE */
