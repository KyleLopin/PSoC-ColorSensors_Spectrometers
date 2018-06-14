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

int main(void)
{
    CyGlobalIntEnable; /* Enable global interrupts. */

    USB_Start();
    I2C_Start();
    
    

    for(;;)
    {
        /* Place your application code here. */
    }
}

/* [] END OF FILE */
