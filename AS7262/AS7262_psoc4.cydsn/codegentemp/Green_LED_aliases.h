/*******************************************************************************
* File Name: Green_LED.h  
* Version 2.20
*
* Description:
*  This file contains the Alias definitions for Per-Pin APIs in cypins.h. 
*  Information on using these APIs can be found in the System Reference Guide.
*
* Note:
*
********************************************************************************
* Copyright 2008-2015, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions, 
* disclaimers, and limitations in the end user license agreement accompanying 
* the software package with which this file was provided.
*******************************************************************************/

#if !defined(CY_PINS_Green_LED_ALIASES_H) /* Pins Green_LED_ALIASES_H */
#define CY_PINS_Green_LED_ALIASES_H

#include "cytypes.h"
#include "cyfitter.h"
#include "cypins.h"


/***************************************
*              Constants        
***************************************/
#define Green_LED_0			(Green_LED__0__PC)
#define Green_LED_0_PS		(Green_LED__0__PS)
#define Green_LED_0_PC		(Green_LED__0__PC)
#define Green_LED_0_DR		(Green_LED__0__DR)
#define Green_LED_0_SHIFT	(Green_LED__0__SHIFT)
#define Green_LED_0_INTR	((uint16)((uint16)0x0003u << (Green_LED__0__SHIFT*2u)))

#define Green_LED_INTR_ALL	 ((uint16)(Green_LED_0_INTR))


#endif /* End Pins Green_LED_ALIASES_H */


/* [] END OF FILE */
