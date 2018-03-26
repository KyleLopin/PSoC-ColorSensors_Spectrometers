/*******************************************************************************
* File Name: LCD.h
* Version 1.20
*
* Description:
*  This header file contains registers and constants associated with the
*  I2C LCD component.
*
* Note:
*
********************************************************************************
* Copyright 2013-2014, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#if !defined(CY_I2C_LCD_LCD_H)
#define CY_I2C_LCD_LCD_H

#include "cytypes.h"
#include "cyfitter.h"

#include "I2C.h"

#if defined(I2C_SCB_MODE)
    #include "I2C_I2C.h"
#endif /* I2C_M_SCB_MODE */


/***************************************
*   Conditional Compilation Parameters
***************************************/

/* This condition checks if I2C Master implemented on SCB
* block. There is mismatch in function names between I2C Master that is
* implemented on SCB and I2C Master that is implemented on UDB.
*/
#if defined(I2C_SCB_MODE)
    /* In this case always returns - "true" */
    #define LCD_IS_SCB_MASTER_USED     (0x01u == 0x01u)
#else
    /* Always "false" */
    #define LCD_IS_SCB_MASTER_USED     (0x01u == 0x00u)
#endif /* I2C_M_SCB_MODE */

#define LCD_CUSTOM_CHAR_SET            (0x00u)

/* Custom character set types */
#define LCD_NONE                       (0u)    /* No Custom Fonts      */
#define LCD_HORIZONTAL_BG              (1u)    /* Horizontal Bar Graph */
#define LCD_VERTICAL_BG                (2u)    /* Vertical Bar Graph   */
#define LCD_USER_DEFINED               (3u)    /* User Defined Fonts   */

#define LCD_COMMAND_FORMAT             (0u)

/* Possible command format values */
#define LCD_NXP_PCF2119X               (0x00u)
#define LCD_CUSTOM                     (0x01u)

/********************
* Generated Code
********************/
/* Indexes of command patterns in the look-up array. Required for a quick 
* access for the command.
*/
#define LCD_WRITE_DATA (0x00u)
#define LCD_DISPLAY_ON_CURSOR_OFF (0x0Cu)
#define LCD_DISPLAY_CURSOR_OFF (0x08u)
#define LCD_DDRAM_0 (0x80u)
#define LCD_CURSOR_HOME (0x02u)
#define LCD_RESET_CURSOR_POSITION (0x03u)
#define LCD_CURSOR_SH_LEFT (0x10u)
#define LCD_CURSOR_SH_RIGHT (0x14u)
#define LCD_DISPLAY_CURSOR_ON (0x0Eu)
#define LCD_CURSOR_WINK (0x0Du)
#define LCD_CURSOR_BLINK (0x0Fu)
#define LCD_CLEAR_DISPLAY (0x01u)
#define LCD_CGRAM_0 (0x40u)
#define LCD_DISPLAY_2_LINES_5X10 (0x24u)
#define LCD_CURSOR_RIGHT (0x06u)
#define LCD_CURSOR_LEFT (0x04u)


/***************************************
*        Function Prototypes
***************************************/
void LCD_Init(void) ;
void LCD_Enable(void) ;
void LCD_Start(void) ;
void LCD_Stop(void) ;
void LCD_WriteControl(uint8 cByte) ;
void LCD_WriteData(uint8 dByte) ;
void LCD_PrintString(char8 const string[]) ;
void LCD_Position(uint8 row, uint8 column) ;
void LCD_PutChar(char8 character) ;
void LCD_SetAddr(uint8 address) ;
void LCD_HandleCustomCommand(uint8 cmdId, uint8 dataLength, uint8 const cmdData[])
                                            ;
void LCD_HandleOneByteCommand(uint8 cmdId, uint8 cmdByte)
                                                        ;
/* ASCII Conversion Routines */
void LCD_PrintInt8(uint8 value) ;
void LCD_PrintInt16(uint16 value) ;
void LCD_PrintNumber(uint16 value) ;

#if((LCD_CUSTOM_CHAR_SET == LCD_VERTICAL_BG) || \
                (LCD_CUSTOM_CHAR_SET == LCD_HORIZONTAL_BG))

    void  LCD_DrawHorizontalBG(uint8 row, uint8 column, uint8 maxCharacters, uint8 value)
                         ;

    void LCD_DrawVerticalBG(uint8 row, uint8 column, uint8 maxCharacters, uint8 value)
                        ;

#endif /* ((LCD_CUSTOM_CHAR_SET == LCD_VERTICAL_BG) ||
       * (LCD_CUSTOM_CHAR_SET == LCD_HORIZONTAL_BG))
       */

#if(LCD_CUSTOM_CHAR_SET != LCD_NONE)

    void LCD_LoadCustomFonts(uint8 const customData[])
                            ;

#endif /* ((LCD_CUSTOM_CHAR_SET == LCD_USER_DEFINED) */


/***************************************
*        Macros
***************************************/

/* Clear Macro */
#define LCD_ClearDisplay()            LCD_WriteControl(LCD_CLEAR_DISPLAY)
/* Off Macro */
#define LCD_DisplayOff()              LCD_WriteControl(LCD_DISPLAY_CURSOR_OFF)
/* On Macro */
#define LCD_DisplayOn()               LCD_WriteControl(LCD_DISPLAY_ON_CURSOR_OFF)

/* Function-like macros for handling I2C MAster API names on SCB and UDB */
#if(LCD_IS_SCB_MASTER_USED)
    #define LCD_MasterWriteBuf(slaveAddress, wrData, cnt, mode)  \
                                I2C_I2CMasterWriteBuf(slaveAddress, wrData, cnt, mode)

    #define LCD_MasterReadStatus()     I2C_I2CMasterStatus()
#else
    #define LCD_MasterWriteBuf(slaveAddress, wrData, cnt, mode)  \
                                I2C_MasterWriteBuf(slaveAddress, wrData, cnt, mode)

    #define LCD_MasterReadStatus()     I2C_MasterStatus()
#endif /* LCD_IS_SCB_MASTER_USED */


#define LCD_AddrStart(addr) \
                                                do {\
                                                    LCD_SetAddr(addr);\
                                                    LCD_Start();\
                                                }while(0)

#define LCD_AddrStop(addr) \
                                                do {\
                                                    LCD_SetAddr(addr);\
                                                    LCD_Stop(); \
                                                }while(0)

#define LCD_AddrDisplayOn(addr) \
                                                do {\
                                                    LCD_SetAddr(addr);\
                                                    LCD_DisplayOn();\
                                                }while(0)

#define LCD_AddrDisplayOff(addr) \
                                                do {\
                                                    LCD_SetAddr(addr);\
                                                    LCD_DisplayOff();\
                                                }while(0)

#define LCD_AddrPrintString(addr, str) \
                                                do {\
                                                    LCD_SetAddr(addr);\
                                                    LCD_PrintString(str);\
                                                }while(0)

#define LCD_AddrPutChar(addr, ch) \
                                                do {\
                                                    LCD_SetAddr(addr);\
                                                    LCD_PutChar(ch);\
                                               }while(0)

#define LCD_AddrPosition(addr, row, col) \
                                                do {\
                                                    LCD_SetAddr(addr);\
                                                    LCD_Position(row, col);\
                                                }while(0)

#define LCD_AddrWriteData(addr, dByte) \
                                                do {\
                                                    LCD_SetAddr(addr);\
                                                    LCD_WriteData(dByte);\
                                                }while(0)

#define LCD_AddrWriteControl(addr, cByte) \
                                                do {\
                                                    LCD_SetAddr(addr);\
                                                    LCD_WriteControl(cByte);\
                                                }while(0)

#define LCD_AddrClearDisplay(addr) \
                                                do {\
                                                    LCD_SetAddr(addr);\
                                                    LCD_ClearDisplay();\
                                                }while(0)

#define LCD_AddrEnable(addr) \
                                                do {\
                                                    LCD_SetAddr(addr); \
                                                    LCD_Enable();\
                                                }while(0)

#define LCD_AddrPrintInt8(addr, value) \
                                                do {\
                                                    LCD_SetAddr(addr); \
                                                    LCD_PrintInt8(value);\
                                                }while(0)
                                                
#define LCD_AddrPrintInt16(addr, value) \
                                                do {\
                                                    LCD_SetAddr(addr); \
                                                    LCD_PrintInt16(value);\
                                                }while(0)
                                                
#define LCD_AddrPrintNumber(addr, value) \
                                                do {\
                                                    LCD_SetAddr(addr); \
                                                    LCD_PrintNumber(value);\
                                                }while(0)

#if(LCD_CUSTOM_CHAR_SET != LCD_NONE)

    #define LCD_AddrLoadCustomFonts(addr, customFont) \
                                                do {\
                                                    LCD_SetAddr(addr);\
                                                    LCD_LoadCustomFonts(customFont);\
                                                }while(0)

#endif /* (LCD_CUSTOM_CHAR_SET != LCD_NONE) */


#if((LCD_CUSTOM_CHAR_SET == LCD_VERTICAL_BG) || \
                (LCD_CUSTOM_CHAR_SET == LCD_HORIZONTAL_BG))

    #define LCD_AddrDrawHorizontalBG(addr, row, column, maxCharacters, value) \
                                                do {\
                                                    LCD_SetAddr(addr);\
                                                    LCD_DrawHorizontalBG(row,column,maxCharacters,value);\
                                                }while(0)


    #define LCD_AddrDrawVerticalBG(addr, row, column, maxCharacters, value) \
                                                do {\
                                                    LCD_SetAddr(addr);\
                                                    LCD_DrawVerticalBG(row, column, maxCharacters, value);\
                                                }while(0)

#endif /* ((LCD_CUSTOM_CHAR_SET == LCD_VERTICAL_BG) ||
       * (LCD_CUSTOM_CHAR_SET == LCD_HORIZONTAL_BG))
       */

#define LCD_AddrHandleCustomCommand(addr, cmdId, dataLength, cmdData) \
                                                do {\
                                                    LCD_SetAddr(addr);\
                                                    LCD_HandleCustomCommand(cmdId, dataLength, cmdData);\
                                                }while(0)

#define LCD_AddrHandleOneByteCommand(addr, cmdId, cmdByte) \
                                                do {\
                                                    LCD_SetAddr(addr);\
                                                    LCD_HandleOneByteCommand(cmdId, cmdByte);\
                                                }while(0)

/********************
* Generated Code
********************/



/***************************************
*           Global Variables
***************************************/

extern uint8 LCD_initVar;
extern uint8 const CYCODE LCD_customFonts[64u];


/***************************************
*           API Constants
***************************************/

#define LCD_ADDRESS_SHIFT              (0x01u)

/* Default I2C address in 7-bit mode */
#define LCD_DEFAULT_I2C_ADDRESS        ((uint8)((uint8)(26u) >> \
                                                                            LCD_ADDRESS_SHIFT))

/* This value should be generated by API customizer */
#define LCD_NUM_COMMANDS               (16u)

/********************
* Generated Code
********************/
/* Legacy names from Character LCD */
#define LCD_WRITE_DATA_IDX (0x00u)
#define LCD_DISPLAY_ON_CURSOR_OFF_IDX (0x01u)
#define LCD_DISPLAY_CURSOR_OFF_IDX (0x02u)
#define LCD_DDRAM_0_IDX (0x03u)
#define LCD_CURSOR_HOME_IDX (0x04u)
#define LCD_RESET_CURSOR_POSITION_IDX (0x05u)
#define LCD_CURSOR_SH_LEFT_IDX (0x06u)
#define LCD_CURSOR_SH_RIGHT_IDX (0x07u)
#define LCD_DISPLAY_CURSOR_ON_IDX (0x08u)
#define LCD_CURSOR_WINK_IDX (0x09u)
#define LCD_CURSOR_BLINK_IDX (0x0Au)
#define LCD_CLEAR_DISPLAY_IDX (0x0Bu)
#define LCD_CGRAM_0_IDX (0x0Cu)
#define LCD_DISPLAY_2_LINES_5X10_IDX (0x0Du)
#define LCD_CURSOR_RIGHT_IDX (0x0Eu)
#define LCD_CURSOR_LEFT_IDX (0x0Fu)

#define LCD_DISPLAY_2_LINES_5x10       (LCD_DISPLAY_2_LINES_5X10)
#define LCD_CURSOR_AUTO_INCR_ON        (LCD_CURSOR_RIGHT)

/********************
* Generated Code
********************/
#define LCD_DDRAM_0_DATA_LEN (0x01u)
#define LCD_CGRAM_0_DATA_LEN (0x09u)

#if(LCD_IS_SCB_MASTER_USED)
    #define LCD_MODE_COMPLETE_XFER         (I2C_I2C_MODE_COMPLETE_XFER)
    #define LCD_WRITE_COMPLETE             (I2C_I2C_MSTAT_WR_CMPLT)
#else
    #define LCD_MODE_COMPLETE_XFER         (I2C_MODE_COMPLETE_XFER)
    #define LCD_WRITE_COMPLETE             (I2C_MSTAT_WR_CMPLT)
#endif /* LCD_IS_SCB_MASTER_USED */

/* Constants for PrintNumber routine */
#define LCD_NUMBER_OF_REMAINDERS           (0x05u)
#define LCD_TEN                            (0x0Au)

/* Nibble Offset and Mask */
#define LCD_NIBBLE_SHIFT                   (0x04u)
#define LCD_NIBBLE_MASK                    (0x0Fu)
#define LCD_BUFF_SIZE                      (0x100u)
#define LCD_LENGTH_2_BYTES                 (0x02u)
#define LCD_COMMAND_BYTE_INDEX             (0x02u)

/* Delay value required for some commands to execute */
#define LCD_CMOMMAND_DELAY                 (1500u)

/* LCD Module RAM Address Constants */
#if (LCD_COMMAND_FORMAT == LCD_NXP_PCF2119X)
    #define LCD_ROW_0_START                    (0x80u)
    #define LCD_ROW_1_START                    (0xC0u)
    #define LCD_ROW_2_START                    (0x94u)
    #define LCD_ROW_3_START                    (0xD4u)
#else
    #define LCD_ROW_0_START                    (0x00u)
    #define LCD_ROW_1_START                    (0x40u)
    #define LCD_ROW_2_START                    (0x14u)
    #define LCD_ROW_3_START                    (0x54u)
#endif /* (LCD_COMMAND_FORMAT == LCD_NXP_PCF2119X)  */

/* Custom character handling related constants */
#if(LCD_CUSTOM_CHAR_SET != LCD_NONE)
    #define LCD_CHARACTER_WIDTH                (0x05u)
    #define LCD_CHARACTER_HEIGHT               (0x08u)
    #define LCD_CUSTOM_CHAR_SET_LEN            (0x40u)
#endif /* (LCD_CUSTOM_CHAR_SET != LCD_NONE) */

#define LCD_CUSTOM_CHAR_NUM                (0x08u)

#define LCD_ONE_BYTE_LEN                   (0x01u)

/* Other constants */
#define LCD_BYTE_UPPER_NIBBLE_SHIFT        (0x04u)
#define LCD_BYTE_LOWER_NIBBLE_MASK         (0x0Fu)
#define LCD_U16_UPPER_BYTE_SHIFT           (0x08u)
#define LCD_U16_LOWER_BYTE_MASK            (0xFFu)
                                                    
/* Custom Character References */
#define LCD_CUSTOM_0                       (0x00u)
#define LCD_CUSTOM_1                       (0x01u)
#define LCD_CUSTOM_2                       (0x02u)
#define LCD_CUSTOM_3                       (0x03u)
#define LCD_CUSTOM_4                       (0x04u)
#define LCD_CUSTOM_5                       (0x05u)
#define LCD_CUSTOM_6                       (0x06u)
#define LCD_CUSTOM_7                       (0x07u)

#if (LCD_COMMAND_FORMAT == LCD_NXP_PCF2119X)
    
    /* Command send opcode */
    #define LCD_COMMAND_SEND               (0x00u) 
    
    #define LCD_SET_EXTENDED_FUNC          (0x35u)      
    #define LCD_SPLIT_SCREEN               (0x02u)
    #define LCD_DISP_CONF                  (0x04u)
    
    #define LCD_ICON_CTL                   (0x08u)  
    #define LCD_VLCD_SET                   (0xA0u)
    #define LCD_FUNCTION_SET_STANDARD      (0x34u)  /* Select Standard Instruction set, 2x16 display */

    /* This is mask for character set "S" that is used in NXP
    * compatible LCD Module.
    */
    #define LCD_CHARACTER_SET_S_MASK       ((uint8) (0x80u))
    
#endif /* (LCD_COMMAND_FORMAT == LCD_NXP_PCF2119X) */


#endif /* CY_I2C_LCD_LCD_H */


/* [] END OF FILE */
