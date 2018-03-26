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
#include "AS7262_psoc4.h"
#include "usb_psoc4.h"

#define True                        1
#define False                       0

// char LCD_str[40];
bool Input_Flag = false;

extern AS7262_settings as7262;
extern uint8 OUT_Data_Buffer[MAX_IN_ENDPOINT_BUFFER_SIZE];
uint8 debug_input[100];

// CY_ISR_PROTO( isr_handler_debug1 );
// CY_ISR_PROTO( isr_handler_basic_read );


int main(void)
{
    CyGlobalIntEnable; /* Enable global interrupts. */

    /* Place your initialization/startup code here (e.g. MyInst_Start()) */
    USB_Start();  // initialize the USB
    //UART_Start();
    //UART_UartPutString("Hello\r\n");
    I2C_Start();
    CyDelay(500);
    // uint8 as7262_found = AS7262_Init();
    Red_LED_Write(1);
    Green_LED_Write(1);
    //LCD_Position(0,0);
    //LCD_PrintString("Hello");
    
    for(;;)
    {
        //UART_UartPutString("Hello\r\n");
        if (Input_Flag == false) {  // make sure any input has already been dealt with

            Input_Flag = USB_CheckInput(OUT_Data_Buffer);  // check if there is a response from the computer
        }
        

        
        if (Input_Flag == true) {
            Red_LED_Write(0);
            switch (OUT_Data_Buffer[0]) {  
            case IDENTIFY: ;  // Identify the PSoC if "ID" inputted or what spectrometers are connected if "ID-Spectrometer" inputted
                if ( OUT_Data_Buffer[3] == IDENTIFY_SPECTROMETER ) {
                    // if ( as7262_found ) {
                    USB_Export_Data((uint8*)AS7262_MESSAGE, LEN_AS7262_MESSAGE);
                    //}
//                    else {
//                        USB_Export_Data((uint8*)"None", 4);
//                    }
                }
                else {
                    USB_Export_Data((uint8*)TEST_MESSAGE, LEN_TEST_MESSAGE);
                }
                break;
            case AS726X_COMMAND: ;  // User wants to set up an AS726X device
                if ( OUT_Data_Buffer[5] == '2' ) {
                    Red_LED_Write(0);
                    AS7262_Commands( OUT_Data_Buffer );
                }
                break;
                
            case 'E': ; // Export what the last input command was
                Red_LED_Write(1);
                UART_UartPutString("Got E");
                USB_Export_Data((uint8*)"Export", 6);
                break;
            }
            
            for (uint8 i=0; i<20; i++) {
                debug_input[i] = OUT_Data_Buffer[i];
                OUT_Data_Buffer[i] = '0';  // clear data buffer cause it has been processed
            }
            
            
            
            
            Input_Flag = false;  // turn off input flag because it has been processed
            //CyDelay(100);
            //Red_LED_Write(1);
        }
        //CyDelay(50);
    }
}

/* [] END OF FILE */
