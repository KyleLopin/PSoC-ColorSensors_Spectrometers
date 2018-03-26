/* ========================================
 *
 * Copyright YOUR COMPANY, THE YEAR
 * All Rights Reserved
 * UNPUBLISHED, LICENSED SOFTWARE.
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION
 * WHICH IS THE PROPERTY OF your company.
 *
 * ========================================
*/
#include "project.h"

// standard libraries
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
// project files
#include "AS7262.h"
#include "usb_functions.h"

#define True                        1
#define False                       0

char LCD_str[40];
bool Input_Flag = false;

extern AS7262_settings as7262;
extern uint8 OUT_Data_Buffer[MAX_IN_ENDPOINT_BUFFER_SIZE];

CY_ISR_PROTO( isr_handler_debug1 );
CY_ISR_PROTO( isr_handler_basic_read );

int main(void)
{
    CyGlobalIntEnable; /* Enable global interrupts. */
    LCD_Start();
    LCD_ClearDisplay();
    LCD_Position(1, 0); 
    LCD_PrintString("check1   ");
    /* Place your initialization/startup code here (e.g. MyInst_Start()) */
    USB_Start();  // initialize the USB
    
    I2C_Start();
    CyDelay(500);
    uint8 as7262_found = AS7262_Init();
    //CyDelay(2000);
    LCD_ClearDisplay();
    LCD_Position(1, 0); 
    LCD_PrintString("check3   ");
    isr_as7262_int_StartEx( isr_handler_basic_read );
    isr_as7262_pin_StartEx( isr_handler_debug1 );
    LED_3_Write( 1 );
    LED_4_Write( 1 );
    //isr_as7262_int_StartEx( isr_handler_basic_read );
    //isr_as7262_pin_StartEx( isr_handler_debug1 );
    for(;;)
    {
        LCD_Position(0, 0);
        LCD_PrintString("main loop ");
        
        if(USBFS_IsConfigurationChanged()) {  // if the configuration is changed reenable the OUT ENDPOINT
            while(!USBFS_GetConfiguration()) {  // wait for the configuration with windows / controller is updated
            }
            USBFS_EnableOutEP(OUT_ENDPOINT);  // reenable OUT ENDPOINT
        }
        if (Input_Flag == false) {  // make sure any input has already been dealt with

            Input_Flag = USB_CheckInput(OUT_Data_Buffer);  // check if there is a response from the computer
        }

        // LED_4_Write( AS7262_INT_Read() );
        
        if (Input_Flag == true) {
            switch (OUT_Data_Buffer[0]) {  
            case IDENTIFY: ;  // Identify the PSoC if "ID" inputted or what spectrometers are connected if "ID-Spectrometer" inputted
                if ( OUT_Data_Buffer[3] == IDENTIFY_SPECTROMETER ) {
                    if ( as7262_found ) {
                        USB_Export_Data((uint8*)AS7262_MESSAGE, LEN_AS7262_MESSAGE);
                    }
                    else {
                        USB_Export_Data((uint8*)"None", 4);
                    }
                }
                else {
                    USB_Export_Data((uint8*)TEST_MESSAGE, LEN_TEST_MESSAGE);
                }
                break;
            case AS726X_COMMAND: ;  // User wants to set up an AS726X device
                if ( OUT_Data_Buffer[5] == '2' ) {
                    AS7262_Commands( OUT_Data_Buffer );
                }
                break;
            }
            
            for (uint8 i=0; i<20; i++) {
                 OUT_Data_Buffer[i] = '0';  // clear data buffer cause it has been processed
            }
            Input_Flag = false;  // turn off input flag because it has been processed
        }
        
        
        //LCD_Position(1, 0); 
        //LCD_PrintString("check    ");
        
//        LCD_Position(0, 0); 
//        sprintf(LCD_str, "as: %x|%x|%d", as7262.device_type, as7262.hw_version, as7262.fw_version);
        //LCD_PrintString(LCD_str);
        CyDelay(100);
    }
}

/* [] END OF FILE */
