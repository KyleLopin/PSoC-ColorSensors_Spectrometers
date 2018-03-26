/*******************************************************************************
* File Name: AS7262_INT.h  
* Version 2.20
*
* Description:
*  This file contains Pin function prototypes and register defines
*
* Note:
*
********************************************************************************
* Copyright 2008-2015, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions, 
* disclaimers, and limitations in the end user license agreement accompanying 
* the software package with which this file was provided.
*******************************************************************************/

#if !defined(CY_PINS_AS7262_INT_H) /* Pins AS7262_INT_H */
#define CY_PINS_AS7262_INT_H

#include "cytypes.h"
#include "cyfitter.h"
#include "cypins.h"
#include "AS7262_INT_aliases.h"

/* APIs are not generated for P15[7:6] */
#if !(CY_PSOC5A &&\
	 AS7262_INT__PORT == 15 && ((AS7262_INT__MASK & 0xC0) != 0))


/***************************************
*        Function Prototypes             
***************************************/    

/**
* \addtogroup group_general
* @{
*/
void    AS7262_INT_Write(uint8 value);
void    AS7262_INT_SetDriveMode(uint8 mode);
uint8   AS7262_INT_ReadDataReg(void);
uint8   AS7262_INT_Read(void);
void    AS7262_INT_SetInterruptMode(uint16 position, uint16 mode);
uint8   AS7262_INT_ClearInterrupt(void);
/** @} general */

/***************************************
*           API Constants        
***************************************/
/**
* \addtogroup group_constants
* @{
*/
    /** \addtogroup driveMode Drive mode constants
     * \brief Constants to be passed as "mode" parameter in the AS7262_INT_SetDriveMode() function.
     *  @{
     */
        #define AS7262_INT_DM_ALG_HIZ         PIN_DM_ALG_HIZ
        #define AS7262_INT_DM_DIG_HIZ         PIN_DM_DIG_HIZ
        #define AS7262_INT_DM_RES_UP          PIN_DM_RES_UP
        #define AS7262_INT_DM_RES_DWN         PIN_DM_RES_DWN
        #define AS7262_INT_DM_OD_LO           PIN_DM_OD_LO
        #define AS7262_INT_DM_OD_HI           PIN_DM_OD_HI
        #define AS7262_INT_DM_STRONG          PIN_DM_STRONG
        #define AS7262_INT_DM_RES_UPDWN       PIN_DM_RES_UPDWN
    /** @} driveMode */
/** @} group_constants */
    
/* Digital Port Constants */
#define AS7262_INT_MASK               AS7262_INT__MASK
#define AS7262_INT_SHIFT              AS7262_INT__SHIFT
#define AS7262_INT_WIDTH              1u

/* Interrupt constants */
#if defined(AS7262_INT__INTSTAT)
/**
* \addtogroup group_constants
* @{
*/
    /** \addtogroup intrMode Interrupt constants
     * \brief Constants to be passed as "mode" parameter in AS7262_INT_SetInterruptMode() function.
     *  @{
     */
        #define AS7262_INT_INTR_NONE      (uint16)(0x0000u)
        #define AS7262_INT_INTR_RISING    (uint16)(0x0001u)
        #define AS7262_INT_INTR_FALLING   (uint16)(0x0002u)
        #define AS7262_INT_INTR_BOTH      (uint16)(0x0003u) 
    /** @} intrMode */
/** @} group_constants */

    #define AS7262_INT_INTR_MASK      (0x01u) 
#endif /* (AS7262_INT__INTSTAT) */


/***************************************
*             Registers        
***************************************/

/* Main Port Registers */
/* Pin State */
#define AS7262_INT_PS                     (* (reg8 *) AS7262_INT__PS)
/* Data Register */
#define AS7262_INT_DR                     (* (reg8 *) AS7262_INT__DR)
/* Port Number */
#define AS7262_INT_PRT_NUM                (* (reg8 *) AS7262_INT__PRT) 
/* Connect to Analog Globals */                                                  
#define AS7262_INT_AG                     (* (reg8 *) AS7262_INT__AG)                       
/* Analog MUX bux enable */
#define AS7262_INT_AMUX                   (* (reg8 *) AS7262_INT__AMUX) 
/* Bidirectional Enable */                                                        
#define AS7262_INT_BIE                    (* (reg8 *) AS7262_INT__BIE)
/* Bit-mask for Aliased Register Access */
#define AS7262_INT_BIT_MASK               (* (reg8 *) AS7262_INT__BIT_MASK)
/* Bypass Enable */
#define AS7262_INT_BYP                    (* (reg8 *) AS7262_INT__BYP)
/* Port wide control signals */                                                   
#define AS7262_INT_CTL                    (* (reg8 *) AS7262_INT__CTL)
/* Drive Modes */
#define AS7262_INT_DM0                    (* (reg8 *) AS7262_INT__DM0) 
#define AS7262_INT_DM1                    (* (reg8 *) AS7262_INT__DM1)
#define AS7262_INT_DM2                    (* (reg8 *) AS7262_INT__DM2) 
/* Input Buffer Disable Override */
#define AS7262_INT_INP_DIS                (* (reg8 *) AS7262_INT__INP_DIS)
/* LCD Common or Segment Drive */
#define AS7262_INT_LCD_COM_SEG            (* (reg8 *) AS7262_INT__LCD_COM_SEG)
/* Enable Segment LCD */
#define AS7262_INT_LCD_EN                 (* (reg8 *) AS7262_INT__LCD_EN)
/* Slew Rate Control */
#define AS7262_INT_SLW                    (* (reg8 *) AS7262_INT__SLW)

/* DSI Port Registers */
/* Global DSI Select Register */
#define AS7262_INT_PRTDSI__CAPS_SEL       (* (reg8 *) AS7262_INT__PRTDSI__CAPS_SEL) 
/* Double Sync Enable */
#define AS7262_INT_PRTDSI__DBL_SYNC_IN    (* (reg8 *) AS7262_INT__PRTDSI__DBL_SYNC_IN) 
/* Output Enable Select Drive Strength */
#define AS7262_INT_PRTDSI__OE_SEL0        (* (reg8 *) AS7262_INT__PRTDSI__OE_SEL0) 
#define AS7262_INT_PRTDSI__OE_SEL1        (* (reg8 *) AS7262_INT__PRTDSI__OE_SEL1) 
/* Port Pin Output Select Registers */
#define AS7262_INT_PRTDSI__OUT_SEL0       (* (reg8 *) AS7262_INT__PRTDSI__OUT_SEL0) 
#define AS7262_INT_PRTDSI__OUT_SEL1       (* (reg8 *) AS7262_INT__PRTDSI__OUT_SEL1) 
/* Sync Output Enable Registers */
#define AS7262_INT_PRTDSI__SYNC_OUT       (* (reg8 *) AS7262_INT__PRTDSI__SYNC_OUT) 

/* SIO registers */
#if defined(AS7262_INT__SIO_CFG)
    #define AS7262_INT_SIO_HYST_EN        (* (reg8 *) AS7262_INT__SIO_HYST_EN)
    #define AS7262_INT_SIO_REG_HIFREQ     (* (reg8 *) AS7262_INT__SIO_REG_HIFREQ)
    #define AS7262_INT_SIO_CFG            (* (reg8 *) AS7262_INT__SIO_CFG)
    #define AS7262_INT_SIO_DIFF           (* (reg8 *) AS7262_INT__SIO_DIFF)
#endif /* (AS7262_INT__SIO_CFG) */

/* Interrupt Registers */
#if defined(AS7262_INT__INTSTAT)
    #define AS7262_INT_INTSTAT            (* (reg8 *) AS7262_INT__INTSTAT)
    #define AS7262_INT_SNAP               (* (reg8 *) AS7262_INT__SNAP)
    
	#define AS7262_INT_0_INTTYPE_REG 		(* (reg8 *) AS7262_INT__0__INTTYPE)
#endif /* (AS7262_INT__INTSTAT) */

#endif /* CY_PSOC5A... */

#endif /*  CY_PINS_AS7262_INT_H */


/* [] END OF FILE */
