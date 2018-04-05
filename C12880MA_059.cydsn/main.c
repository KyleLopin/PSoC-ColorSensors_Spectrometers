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
#include "C12880.h"
#include "lighting.h"
#include "usb_protocols.h"

#define True                1
#define False               0


//CY_ISR( SW_Handler ) {
//
////    LCD_Position(1, 0);
////    LCD_PrintString("SW");
//    //C12880_Read();
//    SW1_ClearInterrupt();
//}


char LCD_str[40];
bool Input_Flag = false;
uint8 data_read = False;

extern uint8 DMA_Video_Chan;
extern uint8 DMA_Video_TD[1];

extern union C12880Data c12880_data;

int main(void)
{
//    isr_sw_StartEx( SW_Handler );
    
    CyGlobalIntEnable; /* Enable global interrupts. */
    
//    LCD_Start();
    
    USB_Start();
    C12880_Start();
//    LCD_ClearDisplay();
//    LCD_Position(0, 0);
//    LCD_PrintString(" C12880 Start2");
    
//    uint16 num = 0;
//    uint8 blank = 0;
//    uint8 blank2 = 0;
//    uint8 cystatus = 0;

    for(;;)
    {
        if(USBFS_IsConfigurationChanged()) {  // if the configuration is changed reenable the OUT ENDPOINT
            while(!USBFS_GetConfiguration()) {  // wait for the configuration with windows / controller is updated
            }
            USBFS_EnableOutEP(OUT_ENDPOINT);  // reenable OUT ENDPOINT
        }
        if (Input_Flag == false) {  // make sure any input has already been dealt with

            Input_Flag = USB_CheckInput(OUT_Data_Buffer);  // check if there is a response from the computer
        }
        
//        LCD_Position(1, 0);
////        cystatus = CyDmaTdGetConfiguration( DMA_Video_TD[0], &num, &blank, &blank2 );
//        sprintf(LCD_str, "td: %d", c12880_data.data[10]);
//        LCD_PrintString(LCD_str);
        
        if (Input_Flag == true) {
            switch (OUT_Data_Buffer[0]) {  
            case IDENTIFY: ;  // Identify the PSoC if "ID" inputted or what spectrometers are connected if "ID-Spectrometer" inputted
                if ( OUT_Data_Buffer[3] == IDENTIFY_SPECTROMETER ) {
                    if ( True ) {  // make a C12880 found flag later
                        USB_Export_Data((uint8*)C12880_MESSAGE, LEN_C12880_MESSAGE);
                    }
                    else {
                        USB_Export_Data((uint8*)"None", 4);
                    }
                }
                else {
                    USB_Export_Data((uint8*)TEST_MESSAGE, LEN_TEST_MESSAGE);
                }
                break;
                
            case LIGHTING: ;
                lighting_commands( OUT_Data_Buffer );
                    
                break;
            case C12880_COMMAND: ;
                C12880_Commands(OUT_Data_Buffer);
                break;
                
            }
            for (uint8 i=0; i<20; i++) {
                 OUT_Data_Buffer[i] = '0';  // clear data buffer cause it has been processed
            }
            Input_Flag = false;  // turn off input flag because it has been processed        
        }   
                
        
        
//        if ( data_read ) {
//
//            C12880_Export_Data();
//            // reset the flag
//            data_read = False;
//        }
    }
}

/* [] END OF FILE */
