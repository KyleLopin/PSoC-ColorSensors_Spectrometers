/*******************************************************************************
 * File Name: C12880.c
 * Version 0.20
 *
 * Description:
 *  This file provides functions to control an C12880MA through a series of digital periphirals
 *
*******************************************************************************
 * Copyright by Kyle Lopin, Naresuan University, 2018
 * All Rights Reserved
 * UNPUBLISHED, LICENSED SOFTWARE.
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION
 * WHICH IS THE PROPERTY OF Naresuan University.
 *
 * ========================================
*/

#include "C12880.h"


#define True                1
#define False               0

uint8 use_led_flash = False;
uint8 use_laser_flash = False;


CY_ISR( c12880_finished_handler ) {
    data_read = True;
    if (use_led_flash) {
        turn_led_off();
    }
    if (use_laser_flash) {
        turn_laser_off();
    }
//    LCD_Position(0,0);
//    LCD_PrintString("C12880 finish");
}


uint16 Convert2Dec(uint8 array[], uint8 len);

void c12880_dma_config(void) {
    /* DMA Configuration for DMA_Video */
    DMA_Video_Chan = DMA_Video_DmaInitialize(DMA_Video_BYTES_PER_BURST, DMA_Video_REQUEST_PER_BURST, 
        HI16(DMA_Video_SRC_BASE), HI16(DMA_Video_DST_BASE));
    DMA_Video_TD[0] = CyDmaTdAllocate();
    //CyDmaTdSetConfiguration(DMA_Video_TD[0], 576, DMA_Video_TD[0], DMA_Video__TD_TERMOUT_EN | CY_DMA_TD_INC_DST_ADR);
    CyDmaTdSetConfiguration(DMA_Video_TD[0], 576, CY_DMA_DISABLE_TD, DMA_Video__TD_TERMOUT_EN | CY_DMA_TD_INC_DST_ADR);
    CyDmaTdSetAddress(DMA_Video_TD[0], LO16((uint32)ADC_SAR_WRK0_PTR), LO16((uint32)c12880_data.data));
    CyDmaChSetInitialTd(DMA_Video_Chan, DMA_Video_TD[0]);
    //CyDmaChEnable(DMA_Video_Chan, 1);
}


void C12880_Commands(uint8 buffer[]) {
    switch ( buffer[7] ) {
        case 'R': ;  // C12880|READ
//        LCD_Position(0,0);
//        LCD_PrintString("USB start");
            use_led_flash = buffer[19] - '0';
            use_laser_flash = buffer[21] - '0';
            
            C12880_Read(use_led_flash, use_laser_flash);
            break;
        
        case 'I': ;
            uint16 integration_time = Convert2Dec(&buffer[19], 3);
            uint16 integration_cycles = integration_time*500 - 48;
            
            C12880_ST_WritePeriod(integration_cycles + 2);
            C12880_ST_WriteCounter(integration_cycles + 2);
            C12880_ST_WriteCompare(integration_cycles);
        
            break;
            
        case 'D': ;
            Export_C12880_State();
            break;
    }
}


void C12880_Start(void) {
    C12880_TRG_Start();
    C12880_CLK_Start();
    C12880_ST_Start();
    Opamp_video_Start();
    ADC_Start();
    PWM_EoC_Debug_Start();
    c12880_dma_config();
    isr_read_complete_StartEx( c12880_finished_handler );
}

void C12880_Read(uint8 use_led, uint8 use_laser) {
    if (use_led) {
        turn_led_on(64);
        CyDelay(100);
//        LCD_Position(0,0);
//        LCD_PrintString("flash LEd");
    }
    if (use_laser) {
        turn_laser_on(64);
        CyDelay(100);
    }
    // NOTE THIS IS A LINE CHANGE THAT COULD CAUSE PROBLEMS ON MARCH 30TH 2018
    
    CyDmaChEnable(DMA_Video_Chan, 1);
    C12880_ctrl_reg_Write( 1 );
    //C12880_ctrl_reg_Write( 0 );
    
    CyDelayUs(50);
    C12880_ctrl_reg_Write( 0 );
    
}

void C12880_Export_Data(void) {
    for (uint16 i=0; i < 576; i=i+48) {
        USB_Export_Data(&c12880_data.data_bytes[i], 48);
    }
}

void Export_C12880_State(void) {
    c12880_debug_data.data_debug.TRG_value = C12880_TRG_ReadCounter();
    c12880_debug_data.data_debug.CLK_value = C12880_CLK_ReadCounter();
    c12880_debug_data.data_debug.ST_value = C12880_ST_ReadCounter();
    c12880_debug_data.data_debug.ISR_PWM_value = PWM_EoC_Debug_ReadCounter();
    c12880_debug_data.data_debug.EoS_status = Status_Reg_1_Read();
    CyDmaTdGetConfiguration(DMA_Video_TD[0], &c12880_debug_data.data_debug.dma_transfer_count, 
                            NULL, NULL);
    USB_Export_Data(&c12880_debug_data.data_bytes[0], 11);
}


uint16 Convert2Dec(uint8 array[], uint8 len){
    uint16 num = 0;
    for (int i = 0; i < len; i++){
        num = num * 10 + (array[i] - '0');
    }
    return num;
}


/* [] END OF FILE */
