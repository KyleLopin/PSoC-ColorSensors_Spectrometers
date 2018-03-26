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
*        Spectrometer Data Union
***************************************/

// use a union so it will be parsed when put into the USB as bytes
union C12880Data {
    uint8 data_bytes[576];
    uint16 data[288];
};

union C12880Data c12880_data;

/***************************************
*        External Flags
***************************************/

extern uint8 data_read;
extern 


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
void C12880_Start(void);
void C12880_Read(uint8 use_led, uint8 use_laser);
void C12880_Export_Data(void);

#endif


/* [] END OF FILE */
