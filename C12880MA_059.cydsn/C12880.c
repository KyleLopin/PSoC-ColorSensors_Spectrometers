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


//#define True                1
//#define False               0

extern CAT4004 laser;
extern CAT4004 led;
extern PWM_dimmer external_light;

enum c12880_read_single_or_multi {
    READ_SINGLE_SET,
    READ_MULTI_SETS
};

static volatile enum c12880_read_single_or_multi read_data_single_or_multi_flag;
static volatile uint8 num_reads;  // global as isr handles this

C12880_settings c12880 = {
    .led_flash = False,
    .laser_flash = False,
    .external_light_flash = False,
    .auto_range = False
};

/************************************************************************************
* IRS Name: c12880_finish_autoread
*************************************************************************************
*
* Summary:
*  ISR to process end of a C12880 read when it is set to auto read, sets a flag for the 
*    main program to process the data and export the data or change integration time 
*    and read again.  Turn off all light sources that were set to flash.
*
* Global variables:
*  C12880_settings c12880: structure that holds c12880 settings
*
*****************************************************************************************/

CY_ISR( c12880_finished_handler ) {
    // check any light sources that may be on and turn them off
    if (led.use_flash) {
        turn_led_off();
    }
    if (laser.use_flash) {
        turn_laser_off();
    }
    if (external_light.use_flash) {
        turn_external_light_off();
    }
    // check if this is a multi or single run
    if ( read_data_single_or_multi_flag == READ_MULTI_SETS ) {
        c12880_process_flag = True;
        num_reads--;  // if multi run 
    }
    else {
        data_read = True;
        currently_reading = False;
    }
}

/************************************************************************************
* IRS Name: c12880_finish_autoread
*************************************************************************************
*
* Summary:
*  ISR to process end of a C12880 read when it is set to auto read, sets a flag for the 
*    main program to process the data and export the data or change integration time 
*    and read again.  Turn off all light sources that were set to flash.
*
* Global variables:
*  C12880_settings c12880: structure that holds c12880 settings
*
*****************************************************************************************/

CY_ISR( c12880_finish_autoread ) {
    currently_reading = False;
    if (led.use_flash) {
        turn_led_off();
    }
    if (laser.use_flash) {
        turn_laser_off();
    }
    if (external_light.use_flash) {
        turn_external_light_off();
    }
}

/***************************************
*        Function Prototypes
***************************************/  

uint16 Convert2Dec(uint8 array[], uint8 len);

/************************************************************************************
* Function Name: c12880_dma_config
*************************************************************************************
*
* Summary:
*  Configure the DMA that handles moving the data from the C12880/ADC to the SRAM
*
* Parameters:
*  None
*
* Return:
*  None
*
* Global variables:
*  C12880Data c12880_data: data union to hold the data coming from ADC attached to C12880
*
*****************************************************************************************/

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

/******************************************************************************
* Function Name: C12880_Commands
*******************************************************************************
*
* Summary:
*  Check input from user to see how they want to configure or run the C12880
*
* Parameters:
*  uint8 buffer[]: array of user inputted chars
*
* Return:
*  None
*
* Global variables:
*  None
*
*******************************************************************************/

void C12880_Commands(uint8 buffer[]) {
    switch ( buffer[7] ) {
        case 'R': ;  // C12880|READ_SINGLE,  C12880|READ_MULTI
            if ( buffer[12] == 'S' ) {
                read_data_single_or_multi_flag = READ_SINGLE_SET;
                C12880_Read();
            }
            else if ( buffer[12] == 'M' ) {  // C12880|READ_MULTI|XXX
                read_data_single_or_multi_flag = READ_MULTI_SETS;
                //c12880_status = C12880_PROCESSES_MULTI_READ;
                num_reads = Convert2Dec( &buffer[18], 3 );
                // clear any previous reads
                for (uint16 i=0; i < 288; i++) {
                    c12880_multi_read_data.data[i] = 0;
                }
                c12880_process_flag = False;  // the isr will set this to true
                C12880_Read_Multi(True);
            }
            break;
        
        case 'B': ;  // C12880|BACKGROUND
//            read_data_or_background_flag = READ_C12880_BACKGROUND;
//            read_data_single_or_multi_flag = READ_MULTI_SETS;
//            num_reads = 10;
//            // clear any previous reads
//            for (uint16 i=0; i < 288; i++) {
//                    c12880_multi_read_data.data[i] = 0;
//                }
//            c12880_process_flag = False;  // the isr will set this to true
//            
//            C12880_Read_Multi(True);
            for ( uint16 j=0; j < 288; j++ ) {
                    c12880_multi_read_data.data[j] = 0;
            }
            
            for ( uint8 i=0; i < 10; i++ ) {
                for ( uint16 j=0; j < 288; j++ ) {
                    ADC_StartConvert();
                    ADC_IsEndConversion( ADC_WAIT_FOR_RESULT );
                    c12880_multi_read_data.data[j] += ADC_GetResult16();
                    //c12880_multi_read_data.data[j] += j;
                }
            }
            ADC_StopConvert();
            
            break;
            
        case 'I': ;  // C12880|INTEGRATION|XXX
            // DEPRECATED
            uint16 integration_time = Convert2Dec(&buffer[19], 3);
            uint16 integration_cycles = integration_time*500 - 48;
            
            C12880_ST_WritePeriod(integration_cycles + 2);
            C12880_ST_WriteCounter(integration_cycles + 2);
            C12880_ST_WriteCompare(integration_cycles);
        
            break;
        case 'S': ;  // C12880|ST_DIVIDER|XXXXX or C12880|ST_PERIOD|XXXXX
            
            
            if ( buffer[10] == 'D' ) {  // C12880|ST_DIVIDER|XXXXX
                uint16 clock_divider = Convert2Dec(&buffer[18], 5);
                Clock_ST_SetDividerValue(clock_divider);
            }
            else if ( buffer[10] == 'P' ) {  //C12880|ST_PERIOD|XXXXX
                uint16 pwm_cycles = Convert2Dec(&buffer[17], 5);
                C12880_ST_WritePeriod(pwm_cycles + 2);
                C12880_ST_WriteCounter(pwm_cycles + 2);
                C12880_ST_WriteCompare(pwm_cycles);
            }
            // Sync the C12880 clock to the start signal
            C12880_ST_Stop();
            C12880_ST_Start();
            
            break;
            
            
        case 'Q': ;  // C12880|QUERY_RUN
            if (currently_reading == True) {
                USB_Export_Data((uint8*)"NOT DONE ", 9);
            }
            else if (data_read == True) {
                USB_Export_Data((uint8*)"READ DONE", 9);
            }
            else {
                USB_Export_Data((uint8*)"NO DATA  ", 9);
            }
            break;
            
        case 'E': ;  // C12880|EXPORT_DATA|SINGLE or C12880|EXPORT_DATA|MULTI
            if ( buffer[19] == 'S' ) { 
                C12880_Export_Data( READ_SINGLE_SET );
            }
            else if ( buffer[19] == 'M' ) { 
                C12880_Export_Data( READ_MULTI_SETS );
            }
            data_read = False;
            break;
            
        case 'D': ;  // C12880|DEBUG
            
            Export_C12880_State();
            break;
    }
}

void C12880_Process_Commands(void) {
//    switch ( c12880_status ) {  // read_data_single_or_multi_flag = READ_MULTI_SETS
//        case C12880_PROCESSES_MULTI_READ: ; 
//            C12880_Read_Multi(False);
//            break;
//        
//    }
    if ( read_data_single_or_multi_flag == READ_MULTI_SETS ) {
        CyDelay(10);
        C12880_Read_Multi(False);
    }
}

void C12880_Start(void) {
    C12880_TRG_Start();
    C12880_CLK_Start();
    C12880_ST_Start();
    //Opamp_video_Start();
    ADC_Start();
    PWM_EoC_Debug_Start();
    c12880_dma_config();
    isr_read_complete_StartEx( c12880_finished_handler );
    currently_reading = False;
}

void C12880_Read_Autorange() {
    
}

void C12880_Read_Multi(bool initial_call) {
    if ( initial_call == False ) {
        for (uint16 i=0; i < 288; i++) {  // this is not needed the first time, 
            c12880_multi_read_data.data[i] += c12880_data.data[i];
        }
    }
    
    if ( num_reads > 0 ) {
        C12880_Read();
    }
    else {
        data_read = True;
        currently_reading = False;
        c12880_process_flag = False;
    }
}



void C12880_Read(void) {
    currently_reading = True;
    uint8 flash = False;
    if (led.use_flash) {
        turn_led_on(64);
        flash = True;
    }
    if (laser.use_flash) {
        turn_laser_on(64);
        flash = True;
    }
    if (external_light.use_flash) {
        turn_external_light_on(external_light.power_level);
        flash = True;
    }
    if ( flash ) {
        CyDelay(20);
    }
    
    CyDmaChEnable(DMA_Video_Chan, 1);
    C12880_ctrl_reg_Write( 1 );
    C12880_ctrl_reg_Write( 0 );
    
}

void C12880_Export_Data(uint8 data_type_flag) {
    if ( data_type_flag == READ_SINGLE_SET ) {
        for (uint16 i=0; i < NUM_SINGLE_READ_DATA_BYTES; i=i+48) {
            USB_Export_Data(&c12880_data.data_bytes[i], 48);
        }
    }
    else if ( data_type_flag == READ_MULTI_SETS ) {
        for (uint16 i=0; i < NUM_MULTI_READ_DATA_BYTES; i=i+48) {
            USB_Export_Data(&c12880_multi_read_data.data_bytes[i], 48);
        }
    } 
}

void C12880_Measure_Background(void) {
    
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
