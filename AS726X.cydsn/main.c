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

// project files
#include "AS726X.h"
#include "usb_functions.h"


#define True                        1
#define False                       0

char LCD_str[40];
uint8 Input_Flag = False;
extern uint8 OUT_Data_Buffer[MAX_IN_ENDPOINT_BUFFER_SIZE];
uint8 device_attached = False;

int main(void)
{
    CyGlobalIntEnable; /* Enable global interrupts. */

    USB_Start();
    I2C_Start();
    CyDelay(20);
    
    LCD_Start();
    LCD_ClearDisplay();
    LCD_Position(1, 0); 
    LCD_PrintString("check0   ");
    device_attached = AS726x_Init();
    LCD_Position(0, 0); 
    LCD_PrintString("check1   ");

    for(;;)
    {
        if(USBFS_IsConfigurationChanged()) {  // if the configuration is changed reenable the OUT ENDPOINT
            while(!USBFS_GetConfiguration()) {  // wait for the configuration with windows / controller is updated
            }
            USBFS_EnableOutEP(OUT_ENDPOINT);  // reenable OUT ENDPOINT
        }
        if (Input_Flag == False) {  // make sure any input has already been dealt with

            Input_Flag = USB_CheckInput(OUT_Data_Buffer);  // check if there is a response from the computer
        }
        
        if (Input_Flag == True) {
            switch (OUT_Data_Buffer[0]) {  
            case IDENTIFY: ;  // Identify the PSoC if "ID" inputted or what spectrometers are connected if "ID-Spectrometer" inputted
                if ( OUT_Data_Buffer[3] == IDENTIFY_SPECTROMETER ) {
                    AS726x_Identify();
                }
                else {
                    USB_Export_Data((uint8*)TEST_MESSAGE, LEN_TEST_MESSAGE);
                }
                break;
            case AS726X_COMMAND:
                AS726x_Commands( OUT_Data_Buffer );
                break;
            }
            
        }
        
    }
}

/* [] END OF FILE */
