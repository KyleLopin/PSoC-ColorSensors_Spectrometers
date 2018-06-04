/*******************************************************************************
 * File Name: C12880.h
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


#if !defined(_C12880_H)
#define _C12880_H

#include <project.h>
#include "lighting.h"
#include "usb_protocols.h"
    
/***************************************
*         Constants and Enums
***************************************/  
    
#define True                            1
#define False                           0
    
//#define C12880_READ                     0x02
//#define C12880_BACKGROUND               0x01
    
//#define C12880_SINGLE_READ              0x01
//#define C12880_MULTI_READ               0x02
    
#define NUM_DATA_PTS                    288
#define NUM_SINGLE_READ_DATA_BYTES      576
#define NUM_MULTI_READ_DATA_BYTES       1152
    
/***************************************
*     DMA Constants and variables
***************************************/  
    
/* Defines for DMA_Video */
#define DMA_Video_BYTES_PER_BURST 2
#define DMA_Video_REQUEST_PER_BURST 1
#define DMA_Video_SRC_BASE (CYDEV_PERIPH_BASE)
#define DMA_Video_DST_BASE (CYDEV_SRAM_BASE)

/* Variable declarations for DMA_Video */
/* Move these variable declarations to the top of the function */
uint8 DMA_Video_Chan;
uint8 DMA_Video_TD[1];


/***************************************
*      C12880 Status Enums and Defines
***************************************/ 

//enum _c12880_status_ {
//    C12880_READY = 0,
//    C12880_DATA_READY = 1,
//    C12880_SINGLE_READ_RUNNING = 2,
//    C12880_AUTO_READ_RUNNING = 3,
//    
//    C12880_ERROR_READING_NO_EOS_SIGNAL = 10
//};
enum c12880_status_ {
    C12880_READY = 0,
    C12880_READING = 1,
    C12880_DATA_READY = 2,
    C12880_PROCESSES_MULTI_READ = 3,
    C12880_PROCESS_AUTO_READ = 4
};

enum c12880_status_ c12880_status;

//struct c12880_status {
//    uint8 status_code;
//    char* message;
//} C12880_Status[] = {
//    { C12880_READY, "READY" },
//    { C12880_DATA_READY, "DATA READY" },
//    { C12880_SINGLE_READ_RUNNING, "READING SINGLE" },
//    { C12880_AUTO_READ_RUNNING, "READING AUTO" },
//    
//    { C12880_ERROR_READING_NO_EOS_SIGNAL, "EOS ERROR" }
//};



/***************************************
*      C12880 Setting Structure
***************************************/ 

typedef struct {
    uint32 integration_time;
    uint8 led_flash;
    uint8 laser_flash;
    uint8 external_light_flash;
    uint8 auto_range;
} C12880_settings;

bool c12880_process_flag;

/***************************************
*        Spectrometer Data Union
***************************************/

// use a union so it will be parsed when put into the USB as bytes
union C12880Data {
    uint8 data_bytes[NUM_SINGLE_READ_DATA_BYTES];
    uint16 data[NUM_DATA_PTS];
};

union C12880MultiData {
    uint8 data_bytes[NUM_MULTI_READ_DATA_BYTES];
    uint32 data[NUM_DATA_PTS];
};

union C12880Data c12880_data;
union C12880MultiData c12880_multi_read_data;

/***************************************
*    Data Structure and Union for Debug
***************************************/

typedef struct C12880_debugger {
    uint16 TRG_value;
    uint16 CLK_value;
    uint16 ST_value;
    uint16 ISR_PWM_value;
    uint16 dma_transfer_count;
    uint8 EoS_status;
} C12880_Debug;

union C12880DebugData {
    uint8 data_bytes[24];
    C12880_Debug data_debug;
};
union C12880DebugData c12880_debug_data;

/***************************************
*        External Flags
***************************************/

extern uint8 data_read;
uint8 currently_reading;


/***************************************
*  Interrupt Service Routine Prototypes
***************************************/  

CY_ISR_PROTO( c12880_finished_handler );

/***************************************
*        Function Prototypes
***************************************/   

void c12880_dma_config(void);

void C12880_Lighting(uint8 buffer[]);

void C12880_Commands(uint8 buffer[]);
void C12880_Process_Commands(void);
void C12880_Start(void);
void C12880_Read_Multi(bool initial_call);
void C12880_Read(void);
void C12880_Export_Data(uint8 data_type_flag);
void Export_C12880_State(void);

#endif


/* [] END OF FILE */
