/**
 * ****************************************************************************
 * @file reg_flash_ctrl.h
 * @author SLC(SC) software team. 
 * @brief This file is the Flash_Ctrl module register list of WIN series chips.
 * @version  1.0
 * ****************************************************************************
 * 
 * Copyright (c) 2024 SLC Semiconductor all rights reserved.
 * 
 * ****************************************************************************
 */

#ifndef __REG_FLASH_CTRL_H__
#define __REG_FLASH_CTRL_H__


#include "gt25q20d.h"
#include "p25q21l.h"


//------------------------------------------------------------------------------
//The following code variable names follow the CMSIS format, and if an error 
//occurs try to include "core_cm0plus.h" before calling the file.
//CMSIS is defined as follows:
//#define     __IM     volatile const      /*! Defines 'read only' structure member permissions */
//#define     __OM     volatile            /*! Defines 'write only' structure member permissions */ 
//#define     __IOM    volatile            /*! Defines 'read / write' structure member permissions */
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//Flash_Ctrl Reg list
//------------------------------------------------------------------------------
typedef struct
{
    __IOM uint32_t QSPI_CFG;                //0x00
    __IOM uint32_t READ_INSTRUCT;           //0x04
    __IOM uint32_t WRITE_INSTRUCT;          //0x08
    __IOM uint32_t QSPI_DELAY;              //0x0C
    __IOM uint32_t READ_DATA_CAP;           //0x10
    __IOM uint32_t DEV_SIZE_CFG;            //0x14
    uint32_t Res1;                          //0x18
    __IOM uint32_t INDAC_ADDR_TRIG;         //0x1C
    uint32_t Res2[2];                       //0x20~0x24
    __IOM uint32_t MODE_BIT_CFG;            //0x28
    __IM uint32_t SRAM_FILL_LEVEL;          //0x2C
    uint32_t Res3[4];                       //0x30~0x3C
    __IOM uint32_t INT_STA;                 //0x40
    __IOM uint32_t INT_MASK;                //0x44
    uint32_t Res4[2];                       //0x48~0x4C  
    __IOM uint32_t LOW_WRITE_PROTECT;       //0x50        
    __IOM uint32_t UPPER_WRITE_PROTECT;     //0x54
    __IOM uint32_t WRITE_PROTECT;           //0x58    
    uint32_t Res5;                          //0x5C
    __IOM uint32_t INDAC_READ_CTRL;         //0x60
    __IOM uint32_t INDAC_READ_WATERMARK;    //0x64
    __IOM uint32_t INDAC_READ_START_ADDR;   //0x68
    __IOM uint32_t INDAC_READ_NUM;          //0x6C
    __IOM uint32_t INDAC_WRITE_CTRL;        //0x70
    __IOM uint32_t INDAC_WRITE_WATERMARK;   //0x74
    __IOM uint32_t INDAC_WRITE_START_ADDR;  //0x78
    __IOM uint32_t INDAC_WRITE_NUM;         //0x7C
    __IOM uint32_t INDAC_TRIG_ADDR_RANGE;   //0x80
    uint32_t Res6[3];                       //0x84~8C   
    __IOM uint32_t FLASH_CMD_CRTL;          //0x90
    __IOM uint32_t FLASH_CMD_ADDR;          //0x94
    uint32_t Res7[2];                       //0x98~9C    
    __IM uint32_t FLASH_READ_DATA_LOW;      //0xA0
    __IM uint32_t FLASH_READ_DATA_HI;       //0xA4
    __IOM uint32_t FLASH_WRITE_DATA_LOW;    //0xA8
    __IOM uint32_t FLASH_WRITE_DATA_HI;     //0xAC
    __IOM uint32_t FLASH_POLLING_STA;       //0xB0
}stFlashCtrlHandle_t, *pstFlashCtrlHandle_t;

#define FLASH_CTRL_INIT_DEFAULT(u8ClkDiv)  (((FLASH_CTRL_QSPI_CFG_EN_MASK << FLASH_CTRL_QSPI_CFG_EN_SHIFT)|\
                                    (0x0 << FLASH_CTRL_QSPI_CFG_CLK_POL_SHIFT)|\
                                    (0x0 << FLASH_CTRL_QSPI_CFG_CLK_PHA_SHIFT)|\
                                    (FLASH_CTRL_QSPI_CFG_DAC_EN_MASK << FLASH_CTRL_QSPI_CFG_DAC_SHIFT)|\
                                    (0x0 << FLASH_CTRL_QSPI_CFG_NEXT_READ_XIP_EN_SHIFT)|\
                                    (0x0 << FLASH_CTRL_QSPI_CFG_TURN_XIP_SHIFT)|\
                                    (u8ClkDiv << FLASH_CTRL_QSPI_CFG_CLK_DIV_SHIFT)))


//------------------------------------------------------------------------------
//0x00 QSPI configuration register
//------------------------------------------------------------------------------

//BIT[0] QSPI Enable
#define FLASH_CTRL_QSPI_CFG_EN_MASK                        (0x1)
#define FLASH_CTRL_QSPI_CFG_EN_SHIFT                       (0)

//BIT[1] SPI Clock polarity
#define FLASH_CTRL_QSPI_CFG_CLK_POL_MASK                   (0x1)
#define FLASH_CTRL_QSPI_CFG_CLK_POL_SHIFT                  (1)

//BIT[2] SPI CLock Phase
#define FLASH_CTRL_QSPI_CFG_CLK_PHA_MASK                   (0x1)
#define FLASH_CTRL_QSPI_CFG_CLK_PHA_SHIFT                  (2)

//BIT[3:6] No Use

//BIT[7] DAC Enable
#define FLASH_CTRL_QSPI_CFG_DAC_EN_MASK                    (0x1)
#define FLASH_CTRL_QSPI_CFG_DAC_SHIFT                      (7)

//BIT[8:16] No use

//BIT[17] Next read using XIP
#define FLASH_CTRL_QSPI_CFG_NEXT_READ_XIP_EN_MASK          (0x1)
#define FLASH_CTRL_QSPI_CFG_NEXT_READ_XIP_EN_SHIFT         (17)

//BIT[18] Turn to XIP immediately
#define FLASH_CTRL_QSPI_CFG_TURN_XIP_MASK                  (0x1)
#define FLASH_CTRL_QSPI_CFG_TURN_XIP_SHIFT                 (18)

//BIT[19:22] QSPI CLK FREQ DIV
#define FLASH_CTRL_QSPI_CFG_CLK_DIV_MASK                   (0xF)
#define FLASH_CTRL_QSPI_CFG_CLK_DIV_SHIFT                  (19)

//BIT[23:30] No Use
#define FLASH_CTRL_QSPI_CFG_STA_MASK                       (0x1)
#define FLASH_CTRL_QSPI_CFG_STA_SHIFT                      (31)

#define FLASH_CTRL_QSPI_CFG_REG_MASK       ((FLASH_CTRL_QSPI_CFG_EN_MASK << FLASH_CTRL_QSPI_CFG_EN_SHIFT)|\
                                (FLASH_CTRL_QSPI_CFG_CLK_POL_MASK << FLASH_CTRL_QSPI_CFG_CLK_POL_SHIFT)|\
                                (FLASH_CTRL_QSPI_CFG_CLK_PHA_MASK << FLASH_CTRL_QSPI_CFG_CLK_PHA_SHIFT)|\
                                (FLASH_CTRL_QSPI_CFG_DAC_EN_MASK << FLASH_CTRL_QSPI_CFG_DAC_SHIFT)|\
                                (FLASH_CTRL_QSPI_CFG_NEXT_READ_XIP_EN_MASK << FLASH_CTRL_QSPI_CFG_NEXT_READ_XIP_EN_SHIFT)|\
                                (FLASH_CTRL_QSPI_CFG_TURN_XIP_MASK << FLASH_CTRL_QSPI_CFG_TURN_XIP_SHIFT)|\
                                (FLASH_CTRL_QSPI_CFG_CLK_DIV_MASK << FLASH_CTRL_QSPI_CFG_CLK_DIV_SHIFT)|\
                                (FLASH_CTRL_QSPI_CFG_STA_MASK << FLASH_CTRL_QSPI_CFG_STA_SHIFT))


typedef enum 
{
    EN_FLASH_CTRL_QSPI_DISABLE  =       0,
    EN_FLASH_CTRL_QSPI_ENABLE   =       1,
}EN_FLASH_CTRL_QSPI_EN_T;

typedef enum
{
    EN_FLASH_CTRL_SPI_CLK_PHA_FAILING_EDGE =    0,
    EN_FLASH_CTRL_SPI_CLK_PHA_RISING_EDGE  =    1,
}EN_FLASH_CTRL_SPI_PHA_T;




//------------------------------------------------------------------------------
//0x04 Device read instruction register
//------------------------------------------------------------------------------
//BIT[0:7] Read opcode
#define FLASH_CTRL_READ_INST_READ_OPCODE_MASK                   (0xFF)
#define FLASH_CTRL_READ_INST_READ_OPCODE_SHIFT                  (0)

//BIT[8:9] Opcode type
#define FLASH_CTRL_CMD_TYPE_MASK                                (0x3)
#define FLASH_CTRL_CMD_TYPE_SHIFT                               (8)

//BIT[10:11] No Used

//BIT[12:13] SPI ADDR MODE
#define FLASH_CTRL_READ_INST_SPI_ADDR_MODE_MASK                 (0x3)
#define FLASH_CTRL_READ_INST_SPI_ADDR_MODE_SHIFT                (12)

//BIT[14:15] No Used

//BIT[16:17] SPI DATA MODE
#define FLASH_CTRL_READ_INST_SPI_DATA_MODE_MASK                 (0x3)
#define FLASH_CTRL_READ_INST_SPI_DATA_MODE_SHIFT                (16)

//BIT[18:19] No Used

//BIT[20] Mode Bit EN
#define FLASH_CTRL_READ_INST_MODE_BIT_EN_MASK                   (0x1)
#define FLASH_CTRL_READ_INST_MODE_BIT_EN_SHIFT                  (20)

//BIT[21:23] No Used

//BIT[24:28] Dummy CLK NUM
#define FLASH_CTRL_READ_INST_DUMMY_CLK_NUM_MASK                 (0x1F)
#define FLASH_CTRL_READ_INST_DUMMY_CLK_NUM_SHIFT                (24)

//BIT[29:31] No Used

#define FLASH_CTRL_READ_INST_REG_MASK   ((FLASH_CTRL_READ_INST_READ_OPCODE_MASK << FLASH_CTRL_READ_INST_READ_OPCODE_SHIFT)|\
                                        (FLASH_CTRL_CMD_TYPE_MASK << FLASH_CTRL_CMD_TYPE_SHIFT)|\
                                        (FLASH_CTRL_READ_INST_SPI_ADDR_MODE_MASK << FLASH_CTRL_READ_INST_SPI_ADDR_MODE_SHIFT)|\
                                        (FLASH_CTRL_READ_INST_SPI_DATA_MODE_MASK << FLASH_CTRL_READ_INST_SPI_DATA_MODE_SHIFT)|\
                                        (FLASH_CTRL_READ_INST_MODE_BIT_EN_MASK << FLASH_CTRL_READ_INST_MODE_BIT_EN_SHIFT)|\
                                        (FLASH_CTRL_READ_INST_DUMMY_CLK_NUM_MASK << FLASH_CTRL_READ_INST_DUMMY_CLK_NUM_SHIFT))


#define FLASH_CTRL_READ_INST_QSPI_INIT(u8Mode)   (((u8Mode & FLASH_CTRL_READ_INST_READ_OPCODE_MASK) << FLASH_CTRL_READ_INST_READ_OPCODE_SHIFT)|\
                                        (0x0 << FLASH_CTRL_CMD_TYPE_SHIFT)|\
                                        (0x0 << FLASH_CTRL_READ_INST_SPI_ADDR_MODE_SHIFT)|\
                                        (0x2 << FLASH_CTRL_READ_INST_SPI_DATA_MODE_SHIFT)|\
                                        (0x1 << FLASH_CTRL_READ_INST_MODE_BIT_EN_SHIFT)|\
                                        (0x4 << FLASH_CTRL_READ_INST_DUMMY_CLK_NUM_SHIFT))
										
#define FLASH_CTRL_READ_INST_QSPI_INIT2(u8Mode)   (((u8Mode & FLASH_CTRL_READ_INST_READ_OPCODE_MASK) << FLASH_CTRL_READ_INST_READ_OPCODE_SHIFT)|\
                                        (0x0 << FLASH_CTRL_CMD_TYPE_SHIFT)|\
                                        (0x0 << FLASH_CTRL_READ_INST_SPI_ADDR_MODE_SHIFT)|\
                                        (0x2 << FLASH_CTRL_READ_INST_SPI_DATA_MODE_SHIFT)|\
                                        (0x0 << FLASH_CTRL_READ_INST_MODE_BIT_EN_SHIFT)|\
                                        (0x8 << FLASH_CTRL_READ_INST_DUMMY_CLK_NUM_SHIFT))

#define FLASH_CTRL_READ_INST_SPI_INIT(u8Mode)   (((u8Mode & FLASH_CTRL_READ_INST_READ_OPCODE_MASK) << FLASH_CTRL_READ_INST_READ_OPCODE_SHIFT)|\
                                        (0x0 << FLASH_CTRL_CMD_TYPE_SHIFT)|\
                                        (0x0 << FLASH_CTRL_READ_INST_SPI_ADDR_MODE_SHIFT)|\
                                        (0x2 << FLASH_CTRL_READ_INST_SPI_DATA_MODE_SHIFT)|\
                                        (0x0 << FLASH_CTRL_READ_INST_MODE_BIT_EN_SHIFT)|\
                                        (0x8 << FLASH_CTRL_READ_INST_DUMMY_CLK_NUM_SHIFT))

#define FLASH_CTRL_READ_BY_SPI_CFG  (((GT_FLASH_CMD_READ_DATA & FLASH_CTRL_READ_INST_READ_OPCODE_MASK) << FLASH_CTRL_READ_INST_READ_OPCODE_SHIFT)|\
                                        (0x0 << FLASH_CTRL_CMD_TYPE_SHIFT)|\
                                        (0x0 << FLASH_CTRL_READ_INST_SPI_ADDR_MODE_SHIFT)|\
                                        (0x0 << FLASH_CTRL_READ_INST_SPI_DATA_MODE_SHIFT)|\
                                        (0x0 << FLASH_CTRL_READ_INST_MODE_BIT_EN_SHIFT)|\
                                        (0x0 << FLASH_CTRL_READ_INST_DUMMY_CLK_NUM_SHIFT))

#define FLASH_CTRL_READ_BY_FAST_SPI_CFG  (((GT_FLASH_CMD_FAST_READ & FLASH_CTRL_READ_INST_READ_OPCODE_MASK) << FLASH_CTRL_READ_INST_READ_OPCODE_SHIFT)|\
                                        (0x0 << FLASH_CTRL_CMD_TYPE_SHIFT)|\
                                        (0x0 << FLASH_CTRL_READ_INST_SPI_ADDR_MODE_SHIFT)|\
                                        (0x0 << FLASH_CTRL_READ_INST_SPI_DATA_MODE_SHIFT)|\
                                        (0x0 << FLASH_CTRL_READ_INST_MODE_BIT_EN_SHIFT)|\
                                        (0x8 << FLASH_CTRL_READ_INST_DUMMY_CLK_NUM_SHIFT))
    
#define FLASH_CTRL_READ_BY_DPI_CFG  (((GT_FLASH_CMD_DUAL_READ & FLASH_CTRL_READ_INST_READ_OPCODE_MASK) << FLASH_CTRL_READ_INST_READ_OPCODE_SHIFT)|\
                                        (0x0 << FLASH_CTRL_CMD_TYPE_SHIFT)|\
                                        (0x0 << FLASH_CTRL_READ_INST_SPI_ADDR_MODE_SHIFT)|\
                                        (0x1 << FLASH_CTRL_READ_INST_SPI_DATA_MODE_SHIFT)|\
                                        (0x0 << FLASH_CTRL_READ_INST_MODE_BIT_EN_SHIFT)|\
                                        (0x8 << FLASH_CTRL_READ_INST_DUMMY_CLK_NUM_SHIFT))
    
#define FLASH_CTRL_READ_BY_FAST_DPI_CFG  (((GT_FLASH_CMD_FAST_DUAL_READ & FLASH_CTRL_READ_INST_READ_OPCODE_MASK) << FLASH_CTRL_READ_INST_READ_OPCODE_SHIFT)|\
                                        (0x0 << FLASH_CTRL_CMD_TYPE_SHIFT)|\
                                        (0x1 << FLASH_CTRL_READ_INST_SPI_ADDR_MODE_SHIFT)|\
                                        (0x1 << FLASH_CTRL_READ_INST_SPI_DATA_MODE_SHIFT)|\
                                        (0x1 << FLASH_CTRL_READ_INST_MODE_BIT_EN_SHIFT)|\
                                        (0x0 << FLASH_CTRL_READ_INST_DUMMY_CLK_NUM_SHIFT))

#define FLASH_CTRL_READ_BY_FAST_QPI_CFG  (((GT_FLASH_CMD_FAST_QUAD_READ & FLASH_CTRL_READ_INST_READ_OPCODE_MASK) << FLASH_CTRL_READ_INST_READ_OPCODE_SHIFT)|\
                                        (0x0 << FLASH_CTRL_CMD_TYPE_SHIFT)|\
                                        (0x2 << FLASH_CTRL_READ_INST_SPI_ADDR_MODE_SHIFT)|\
                                        (0x2 << FLASH_CTRL_READ_INST_SPI_DATA_MODE_SHIFT)|\
                                        (0x1 << FLASH_CTRL_READ_INST_MODE_BIT_EN_SHIFT)|\
                                        (0x4 << FLASH_CTRL_READ_INST_DUMMY_CLK_NUM_SHIFT))




typedef enum
{
    EN_FLASH_CTRL_CMD_BY_DQ0                = 0,
    EN_FLASH_CTRL_CMD_BY_DQ0_AND_DQ1        = 1,
    EN_FLASH_CTRL_CMD_BY_DQ_ALL             = 2,
}EN_FLASH_CTRL_CMD_TYPE_T;




//------------------------------------------------------------------------------
//0x08 Device write instruction register
//------------------------------------------------------------------------------
//BIT[0:7] Opcode
#define FLASH_CTRL_WRITE_INST_OPCODE_MASK                            (0xFF)
#define FLASH_CTRL_WRITE_INST_OPCODE_SHIFT                           (0)

//BIT[8] WEL enable
#define FLASH_CTRL_WRITE_INST_WEL_DIS_MASK                            (0x1)
#define FLASH_CTRL_WRITE_INST_WEL_DIS_SHIFT                            (8)

//BIT[9:11] No used

//BIT[12:13] SPI addr mode
#define FLASH_CTRL_WRITE_INST_SPI_ADDR_MODE_MASK                      (0x3)
#define FLASH_CTRL_WRITE_INST_SPI_ADDR_MODE_SHIFT                     (12)

//BIT[14:15] No used

//BIT[16:17] SPI data mode
#define FLASH_CTRL_WRITE_INST_SPI_DATA_MODE_MASK                       (0x3)
#define FLASH_CTRL_WRITE_INST_SPI_DATA_MODE_SHIFT                      (16)

//BIT[18:23] No used

//BIT[24:28] Dummy clk
#define FLASH_CTRL_WRITE_INST_DUMMY_CLK_NUM_MASK                        (0x1F)
#define FLASH_CTRL_WRITE_INST_DUMMY_CLK_NUM_SHIFT                       (24)



#define FLASH_CTRL_WRITE_INST_REG_MASK  ((FLASH_CTRL_WRITE_INST_OPCODE_MASK << FLASH_CTRL_WRITE_INST_OPCODE_SHIFT)|\
                                        (0x0 << FLASH_CTRL_WRITE_INST_WEL_DIS_SHIFT)|\
                                        (FLASH_CTRL_WRITE_INST_SPI_ADDR_MODE_MASK << FLASH_CTRL_WRITE_INST_SPI_ADDR_MODE_SHIFT)|\
                                        (FLASH_CTRL_WRITE_INST_SPI_DATA_MODE_MASK << FLASH_CTRL_WRITE_INST_SPI_DATA_MODE_SHIFT)|\
                                        (FLASH_CTRL_WRITE_INST_DUMMY_CLK_NUM_MASK << FLASH_CTRL_WRITE_INST_DUMMY_CLK_NUM_SHIFT))

#define FLASH_CTRL_WRITE_INST_QSPI_INIT(u8Cmd)  (((u8Cmd & FLASH_CTRL_WRITE_INST_OPCODE_MASK) << FLASH_CTRL_WRITE_INST_OPCODE_SHIFT)|\
                                        (0x1 << FLASH_CTRL_WRITE_INST_WEL_DIS_SHIFT)|\
                                        (0x0 << FLASH_CTRL_WRITE_INST_SPI_ADDR_MODE_SHIFT)|\
                                        (0x2 << FLASH_CTRL_WRITE_INST_SPI_DATA_MODE_SHIFT)|\
                                        (0x0 << FLASH_CTRL_WRITE_INST_DUMMY_CLK_NUM_SHIFT))

#define FLASH_CTRL_WRITE_INST_DPI_INIT(u8Cmd)  (((u8Cmd & FLASH_CTRL_WRITE_INST_OPCODE_MASK) << FLASH_CTRL_WRITE_INST_OPCODE_SHIFT)|\
                                        (0x0 << FLASH_CTRL_WRITE_INST_WEL_DIS_SHIFT)|\
                                        (0x0 << FLASH_CTRL_WRITE_INST_SPI_ADDR_MODE_SHIFT)|\
                                        (0x1 << FLASH_CTRL_WRITE_INST_SPI_DATA_MODE_SHIFT)|\
                                        (0x0 << FLASH_CTRL_WRITE_INST_DUMMY_CLK_NUM_SHIFT))  

#define FLASH_CTRL_WRITE_BY_SPI_CFG     (((GT_FLASH_CMD_PAGE_WRITE & FLASH_CTRL_WRITE_INST_OPCODE_MASK) << FLASH_CTRL_WRITE_INST_OPCODE_SHIFT)|\
                                         (0x1 << FLASH_CTRL_WRITE_INST_WEL_DIS_SHIFT)|\
                                         (0x0 << FLASH_CTRL_WRITE_INST_SPI_ADDR_MODE_SHIFT)|\
                                         (0x0 << FLASH_CTRL_WRITE_INST_SPI_DATA_MODE_SHIFT)|\
                                         (0x0 << FLASH_CTRL_WRITE_INST_DUMMY_CLK_NUM_SHIFT))



//------------------------------------------------------------------------------
//0x0C Qspi device delay register
//------------------------------------------------------------------------------
//BIT[0:7] CSSOT time delay
#define FLASH_CTRL_QSPI_DELAY_CSSOT_MASK                                (0xF)
#define FLASH_CTRL_QSPI_DELAY_CSSOT_SHIFT                               (0)

//BIT[8:15] CSEOT time delay
#define FLASH_CTRL_QSPI_DELAY_CSEOT_MASK                                (0xF)
#define FLASH_CTRL_QSPI_DELAY_CSEOT_SHIFT                               (8)

//BIT[16:23] CSDADS time delay
#define FLASH_CTRL_QSPI_DELAY_CSDADS_MASK                               (0xF)
#define FLASH_CTRL_QSPI_DELAY_CSDADS_SHIFT                              (16)

//BIT[24:31] CSDA time delay
#define FLASH_CTRL_QSPI_DELAY_CSDA_MASK                                 (0xF)
#define FLASH_CTRL_QSPI_DELAY_CSDA_SHIFT                                (24)

#define FLASH_CTRL_QSPI_DELAY_REG_MASK  ((FLASH_CTRL_QSPI_DELAY_CSSOT_MASK << FLASH_CTRL_QSPI_DELAY_CSSOT_SHIFT)|\
                                        (FLASH_CTRL_QSPI_DELAY_CSEOT_MASK << FLASH_CTRL_QSPI_DELAY_CSEOT_SHIFT)|\
                                        (FLASH_CTRL_QSPI_DELAY_CSDADS_MASK << FLASH_CTRL_QSPI_DELAY_CSDADS_SHIFT)|\
                                        (FLASH_CTRL_QSPI_DELAY_CSDA_MASK << FLASH_CTRL_QSPI_DELAY_CSDA_SHIFT))

//------------------------------------------------------------------------------
//0x10 Read date capture register
//------------------------------------------------------------------------------
//BIT[0] Bypass cycle enable
#define FLASH_CTRL_READ_CAP_BYPASS_CYCLE_EN_MASK                           (0x1)
#define FLASH_CTRL_READ_CAP_BYPASS_CYCLE_EN_SHIFT                          (0)

//BIT[1:4] Sampling delay
#define FLASH_CTRL_READ_CAP_SAMPLING_DELAY_MASK                             (0xF)
#define FLASH_CTRL_READ_CAP_SAMPLING_DELAY_SHIFT                            (1)

//BIT[5] Sampling edge
#define FLASH_CTRL_READ_CAP_SAMPLING_EDGE_MASK                              (0x1)
#define FLASH_CTRL_READ_CAP_SAMPLING_EDGE_SHIFT                             (5)

//BIT[6:31] No used



#define FLASH_CTRL_READ_CAP_REG_MASK    ((FLASH_CTRL_READ_CAP_BYPASS_CYCLE_EN_MASK << FLASH_CTRL_READ_CAP_BYPASS_CYCLE_EN_SHIFT)|\
                                        (FLASH_CTRL_READ_CAP_SAMPLING_DELAY_MASK << FLASH_CTRL_READ_CAP_SAMPLING_DELAY_SHIFT)|\
                                        (FLASH_CTRL_READ_CAP_SAMPLING_EDGE_MASK << FLASH_CTRL_READ_CAP_SAMPLING_EDGE_SHIFT))

//------------------------------------------------------------------------------
//0x14 Device Size Configuration Register
//------------------------------------------------------------------------------
//BIT[0:3] Addr bytes cfg
#define FLASH_CTRL_DEV_SIZE_CFG_ADDR_SIZE_MASK                       (0xF)
#define FLASH_CTRL_DEV_SIZE_CFG_ADDR_SIZE_SHIFT                      (0)

//BIT[4:15] Page bytes cfg
#define FLASH_CTRL_DEV_SIZE_CFG_PAGE_SIZE_MASK                       (0xFFF)
#define FLASH_CTRL_DEV_SIZE_CFG_PAGE_SIZE_SHIFT                      (4)

//BIT[16:20] Block bytes
#define FLASH_CTRL_DEV_SIZE_CFG_BLOCK_SIZE_MASK                      (0x1F)
#define FLASH_CTRL_DEV_SIZE_CFG_BLOCK_SIZE_SHIFT                     (16)

//BIT[21:31] No used

#define FLASH_CTRL_DEV_SIZE_CFG_REG_MASK    ((FLASH_CTRL_DEV_SIZE_CFG_ADDR_SIZE_MASK << FLASH_CTRL_DEV_SIZE_CFG_ADDR_SIZE_SHIFT)|\
                                            (FLASH_CTRL_DEV_SIZE_CFG_PAGE_SIZE_MASK << FLASH_CTRL_DEV_SIZE_CFG_PAGE_SIZE_SHIFT)|\
                                            (FLASH_CTRL_DEV_SIZE_CFG_BLOCK_SIZE_MASK << FLASH_CTRL_DEV_SIZE_CFG_BLOCK_SIZE_SHIFT))

//------------------------------------------------------------------------------
//0x1C Indirect ahb address trigger Register
//------------------------------------------------------------------------------
//Waiting add



//------------------------------------------------------------------------------
//0x28 Mode bit configuration register
//------------------------------------------------------------------------------
//BIT[0:7] Mode bits
#define FLASH_CTRL_MODE_BIT_MASK                                    (0xFF)
#define FLASH_CTRL_MODE_BIT_SHIFT                                   (0)

//BIT[8:31] No used

#define FLASH_CTRL_MODE_BIT_CFG_REG_MASK    ((FLASH_CTRL_MODE_BIT_MASK << FLASH_CTRL_MODE_BIT_SHIFT))

//------------------------------------------------------------------------------
//0x2C SRAM fill level Register
//------------------------------------------------------------------------------
//BIT[0:15] Sram read status
#define FLASH_CTRL_SRAM_FILL_LEVEL_READ_STA_MASK                    (0xFFFF)
#define FLASH_CTRL_SRAM_FILL_LEVEL_READ_STA_SHIFT                   (0)

//BIT[16:31] Sram write status
#define FLASH_CTRL_SRAM_FILL_LEVEL_WRITE_STA_MASK                   (0xFFFF)
#define FLASH_CTRL_SRAM_FILL_LEVEL_WRITE_STA_SHIFT                  (16)

#define FLASH_CTRL_SRAM_FILL_LEVEL_REG_MASK ((FLASH_CTRL_SRAM_FILL_LEVEL_READ_STA_MASK << FLASH_CTRL_SRAM_FILL_LEVEL_WRITE_STA_SHIFT)|\
                                            (FLASH_CTRL_SRAM_FILL_LEVEL_WRITE_STA_MASK << FLASH_CTRL_SRAM_FILL_LEVEL_WRITE_STA_SHIFT))

//------------------------------------------------------------------------------
//0x40 Interrupt status register
//------------------------------------------------------------------------------
//BIT[0] No used

//BIT[1] Underflow detected int
#define FLASH_CTRL_INT_STA_UNDERFLOW_DETECT_MASK                    (0x1)
#define FLASH_CTRL_INT_STA_UNDERFLOW_DETECT_SHIFT                   (1)

//BIT[2] INDAC finish int
#define FLASH_CTRL_INT_STA_INDAC_FINISH_MASK                        (0x1)
#define FLASH_CTRL_INT_STA_INDAC_FINISH_SHIFT                       (2)

//BIT[3] INDAC can't accept operation
#define FLASH_CTRL_INT_STA_INDAC_REJECT_MASK                        (0x1)
#define FLASH_CTRL_INT_STA_INDAC_REJECT_SHIFT                       (3)

//BIT[4] Write protection
#define FLASH_CTRL_INT_STA_WRITE_PROTECT_MASK                       (0x1)
#define FLASH_CTRL_INT_STA_WRITE_PROTECT_SHIFT                      (4)

//BIT[5] Illegal AHB Access Detected
#define FLASH_CTRL_INT_STA_ILLEGAL_AHB_ACCESS_MASK                  (0x1)
#define FLASH_CTRL_INT_STA_ILLEGAL_AHB_ACCESS_SHIFT                 (5)

//BIT[6] Reach INDAC water
#define FLASH_CTRL_INT_STA_REACH_INDAC_WATERMARK_MASK               (0x1)
#define FLASH_CTRL_INT_STA_REACH_INDAC_WATERMARK_SHIFT              (6)

//BIT[7:11] No used

//BIT[12] SRAM Full
#define FLASH_CTRL_INT_STA_SRAM_FULL_MASK                           (0x1)
#define FLASH_CTRL_INT_STA_SRAM_FULL_SHIFT                          (12)

//BIT[13] Polling timeout
#define FLASH_CTRL_INT_STA_POLLING_TIMEOUT_MASK                     (0x1)
#define FLASH_CTRL_INT_STA_POLLING_TIMEOUT_SHIFT                    (13)

//BIT[14:31] No used

#define FLASH_CTRL_INT_STA_REG_MASK ((FLASH_CTRL_INT_STA_UNDERFLOW_DETECT_MASK << FLASH_CTRL_INT_STA_UNDERFLOW_DETECT_SHIFT)|\
                                    (FLASH_CTRL_INT_STA_INDAC_FINISH_MASK << FLASH_CTRL_INT_STA_INDAC_FINISH_SHIFT)|\
                                    (FLASH_CTRL_INT_STA_INDAC_REJECT_MASK << FLASH_CTRL_INT_STA_INDAC_REJECT_SHIFT)|\
                                    (FLASH_CTRL_INT_STA_WRITE_PROTECT_MASK << FLASH_CTRL_INT_STA_WRITE_PROTECT_SHIFT)|\
                                    (FLASH_CTRL_INT_STA_ILLEGAL_AHB_ACCESS_MASK << FLASH_CTRL_INT_STA_ILLEGAL_AHB_ACCESS_SHIFT)|\
                                    (FLASH_CTRL_INT_STA_REACH_INDAC_WATERMARK_MASK << FLASH_CTRL_INT_STA_REACH_INDAC_WATERMARK_SHIFT)|\
                                    (FLASH_CTRL_INT_STA_SRAM_FULL_MASK << FLASH_CTRL_INT_STA_SRAM_FULL_SHIFT)|\
                                    (FLASH_CTRL_INT_STA_POLLING_TIMEOUT_MASK << FLASH_CTRL_INT_STA_POLLING_TIMEOUT_SHIFT))

//------------------------------------------------------------------------------
//0x44 Interrupt mask Register
//------------------------------------------------------------------------------
#define FLASH_CTRL_INT_MASK_REG_MASK    (FLASH_CTRL_INT_STA_REG_MASK)

//------------------------------------------------------------------------------
//0x50 Lower write protection register
//------------------------------------------------------------------------------
//BIT[0:31] Lower write protect addr
#define FLASH_CTRL_LOW_WRITE_PROTECT_MASK                           (0xFFFFFFFF)
#define FLASH_CTRL_LOW_WRITE_PROTECT_SHIFT                          (0)

#define FLASH_CTRL_LOW_WRITE_PROTECT_REG_MASK   ((FLASH_CTRL_LOW_WRITE_PROTECT_MASK << FLASH_CTRL_LOW_WRITE_PROTECT_SHIFT))

//------------------------------------------------------------------------------
//0x54 Upper write protection register
//------------------------------------------------------------------------------
#define FLASH_CTRL_UPPER_WRITE_PROTECT_MASK                           (0xFFFFFFFF)
#define FLASH_CTRL_UPPER_WRITE_PROTECT_SHIFT                          (0)

#define FLASH_CTRL_UPPER_WRITE_PROTECT_REG_MASK   ((FLASH_CTRL_UPPER_WRITE_PROTECT_MASK << FLASH_CTRL_UPPER_WRITE_PROTECT_SHIFT))

//------------------------------------------------------------------------------
//0x58 write protection register
//------------------------------------------------------------------------------
//BIT[0] Inverse protect
#define FLASH_CTRL_WRITE_PROTECT_REVERSE_MASK                           (0x1)
#define FLASH_CTRL_WRITE_PROTECT_REVERSE_SHIFT                          (0)

//BIT[1] Write protect enable
#define FLASH_CTRL_WRITE_PROTECT_EN_MASK                                (0x1)
#define FLASH_CTRL_WRITE_PROTECT_EN_SHIFT                               (1)

//BIT[2:31] No used

#define FLASH_CTRL_WRITE_PROTECT_REG_MASK   ((FLASH_CTRL_WRITE_PROTECT_REVERSE_MASK << FLASH_CTRL_WRITE_PROTECT_REVERSE_SHIFT)|\
                                            (FLASH_CTRL_WRITE_PROTECT_EN_MASK << FLASH_CTRL_WRITE_PROTECT_EN_SHIFT))

//------------------------------------------------------------------------------
//0x60 indirect read transfer control register
//------------------------------------------------------------------------------
//Waiting add

//------------------------------------------------------------------------------
//0x64 indirect read transfer watermark register
//------------------------------------------------------------------------------
//Waiting add

//------------------------------------------------------------------------------
//0x68 indirect read transfer start address register
//------------------------------------------------------------------------------
//Waiting add

//------------------------------------------------------------------------------
//0x6C indirect read transfer number byte register
//------------------------------------------------------------------------------
//Waiting add

//------------------------------------------------------------------------------
//0x70 indirect write transfer control register
//------------------------------------------------------------------------------
//Waiting add

//------------------------------------------------------------------------------
//0x74 Indirect Write Transfer Watermark Register
//------------------------------------------------------------------------------
//Waiting add

//------------------------------------------------------------------------------
//0x78 Indirect Write Transfer Start Address Register
//------------------------------------------------------------------------------
//Waiting add

//------------------------------------------------------------------------------
//0x7C Indirect Write Transfer Number Bytes Register
//------------------------------------------------------------------------------
//Waiting add

//------------------------------------------------------------------------------
//0x80 Indirect Trigger Address Range Register
//------------------------------------------------------------------------------
//Waiting add

//------------------------------------------------------------------------------
//0x90 Flash Command Control Register (Using STIG)
//------------------------------------------------------------------------------
//BIT[0] Execute current command
#define FLASH_CTRL_FLASH_CMD_CTRL_EXECUTE_MASK                      (0x1)
#define FLASH_CTRL_FLASH_CMD_CTRL_EXECUTE_SHIFT                     (0)

//BIT[1] Execute status
#define FLASH_CTRL_FLASH_CMD_CTRL_EXECUTE_STA_MASK                  (0x1)
#define FLASH_CTRL_FLASH_CMD_CTRL_EXECUTE_STA_SHIFT                 (1)

//BIT[2:6] No used

//BIT[7:11] Dummy nums
#define FLASH_CTRL_FLASH_CMD_CTRL_DUMMY_BYTES_NUM_MASK              (0x1F)
#define FLASH_CTRL_FLASH_CMD_CTRL_DUMMY_BYTES_NUM_SHIFT             (7)

//BIT[12:14] Write Data num
#define FLASH_CTRL_FLASH_CMD_CTRL_WRITE_NUM_MASK                    (0x7)
#define FLASH_CTRL_FLASH_CMD_CTRL_WRITE_NUM_SHIFT                   (12)

//BIT[15] Write enable
#define FLASH_CTRL_FLASH_CMD_CTRL_WRITE_EN_MASK                     (0x1)
#define FLASH_CTRL_FLASH_CMD_CTRL_WRITE_EN_SHIFT                    (15)

//BIT[16:17]
#define FLASH_CTRL_FLASH_CMD_CTRL_ADDR_BYTES_NUM_MASK               (0x3)
#define FLASH_CTRL_FLASH_CMD_CTRL_ADDR_BYTES_NUM_SHIFT              (16)

//BIT[18] Mode bit enable
#define FLASH_CTRL_FLASH_CMD_CTRL_MODE_BIT_EN_MASK                  (0x1)
#define FLASH_CTRL_FLASH_CMD_CTRL_MODE_BIT_EN_SHIFT                 (18)

//BIT[19] CMD addr en
#define FLASH_CTRL_FLASH_CMD_CTRL_CMD_ADDR_EN_MASK                  (0x1)
#define FLASH_CTRL_FLASH_CMD_CTRL_CMD_ADDR_EN_SHIFT                 (19)

//BIT[20:22] Read num
#define FLASH_CTRL_FLASH_CMD_CTRL_READ_NUM_MASK                     (0x3)
#define FLASH_CTRL_FLASH_CMD_CTRL_READ_NUM_SHIFT                    (20)

//BIT[23] Read enable
#define FLASH_CTRL_FLASH_CMD_CTRL_READ_EN_MASK                      (0x1)
#define FLASH_CTRL_FLASH_CMD_CTRL_READ_EN_SHIFT                     (23)

//BIT[24:31] CMD opcode
#define FLASH_CTRL_FLASH_CMD_CTRL_CMD_OPCODE_MASK                   (0xFF)
#define FLASH_CTRL_FLASH_CMD_CTRL_CMD_OPCODE_SHIFT                  (24)

#define FLASH_CTRL_FLASH_CMD_CTRL_REG_MASK  ((FLASH_CTRL_FLASH_CMD_CTRL_EXECUTE_MASK << FLASH_CTRL_FLASH_CMD_CTRL_EXECUTE_SHIFT)|\
                                            (FLASH_CTRL_FLASH_CMD_CTRL_EXECUTE_STA_MASK << FLASH_CTRL_FLASH_CMD_CTRL_EXECUTE_STA_SHIFT)|\
                                            (FLASH_CTRL_FLASH_CMD_CTRL_DUMMY_BYTES_NUM_MASK << FLASH_CTRL_FLASH_CMD_CTRL_DUMMY_BYTES_NUM_SHIFT)|\
                                            (FLASH_CTRL_FLASH_CMD_CTRL_WRITE_NUM_MASK << FLASH_CTRL_FLASH_CMD_CTRL_WRITE_NUM_SHIFT)|\
                                            (FLASH_CTRL_FLASH_CMD_CTRL_WRITE_EN_MASK << FLASH_CTRL_FLASH_CMD_CTRL_WRITE_EN_SHIFT)|\
                                            (FLASH_CTRL_FLASH_CMD_CTRL_ADDR_BYTES_NUM_MASK << FLASH_CTRL_FLASH_CMD_CTRL_ADDR_BYTES_NUM_SHIFT)|\
                                            (FLASH_CTRL_FLASH_CMD_CTRL_MODE_BIT_EN_MASK << FLASH_CTRL_FLASH_CMD_CTRL_MODE_BIT_EN_SHIFT)|\
                                            (FLASH_CTRL_FLASH_CMD_CTRL_CMD_ADDR_EN_MASK << FLASH_CTRL_FLASH_CMD_CTRL_CMD_ADDR_EN_SHIFT)|\
                                            (FLASH_CTRL_FLASH_CMD_CTRL_READ_NUM_MASK << FLASH_CTRL_FLASH_CMD_CTRL_READ_NUM_SHIFT)|\
                                            (FLASH_CTRL_FLASH_CMD_CTRL_READ_EN_MASK << FLASH_CTRL_FLASH_CMD_CTRL_READ_EN_SHIFT)|\
                                            (FLASH_CTRL_FLASH_CMD_CTRL_CMD_OPCODE_MASK << FLASH_CTRL_FLASH_CMD_CTRL_CMD_OPCODE_SHIFT))


#define FLASH_CTRL_CMD_NOADDR_NODATA_INIT(u8Cmd) (((0x1 & FLASH_CTRL_FLASH_CMD_CTRL_EXECUTE_MASK) << FLASH_CTRL_FLASH_CMD_CTRL_EXECUTE_SHIFT)|\
                                            ((0x0 & FLASH_CTRL_FLASH_CMD_CTRL_EXECUTE_STA_MASK) << FLASH_CTRL_FLASH_CMD_CTRL_EXECUTE_STA_SHIFT)|\
                                            ((0x0 & FLASH_CTRL_FLASH_CMD_CTRL_DUMMY_BYTES_NUM_MASK) << FLASH_CTRL_FLASH_CMD_CTRL_DUMMY_BYTES_NUM_SHIFT)|\
                                            ((0x0 & FLASH_CTRL_FLASH_CMD_CTRL_WRITE_NUM_MASK) << FLASH_CTRL_FLASH_CMD_CTRL_WRITE_NUM_SHIFT)|\
                                            ((0x0 & FLASH_CTRL_FLASH_CMD_CTRL_WRITE_EN_MASK) << FLASH_CTRL_FLASH_CMD_CTRL_WRITE_EN_SHIFT)|\
                                            ((0x0 & FLASH_CTRL_FLASH_CMD_CTRL_ADDR_BYTES_NUM_MASK) << FLASH_CTRL_FLASH_CMD_CTRL_ADDR_BYTES_NUM_SHIFT)|\
                                            ((0x0 & FLASH_CTRL_FLASH_CMD_CTRL_MODE_BIT_EN_MASK) << FLASH_CTRL_FLASH_CMD_CTRL_MODE_BIT_EN_SHIFT)|\
                                            ((0x0 & FLASH_CTRL_FLASH_CMD_CTRL_CMD_ADDR_EN_MASK) << FLASH_CTRL_FLASH_CMD_CTRL_CMD_ADDR_EN_SHIFT)|\
                                            ((0x0 & FLASH_CTRL_FLASH_CMD_CTRL_READ_NUM_MASK) << FLASH_CTRL_FLASH_CMD_CTRL_READ_NUM_SHIFT)|\
                                            ((0x0 & FLASH_CTRL_FLASH_CMD_CTRL_READ_EN_MASK) << FLASH_CTRL_FLASH_CMD_CTRL_READ_EN_SHIFT)|\
                                            ((u8Cmd & FLASH_CTRL_FLASH_CMD_CTRL_CMD_OPCODE_MASK) << FLASH_CTRL_FLASH_CMD_CTRL_CMD_OPCODE_SHIFT))

#define FLASH_CTRL_CMD_ADDR_NODATA_INIT(u8Cmd) (((0x1 & FLASH_CTRL_FLASH_CMD_CTRL_EXECUTE_MASK) << FLASH_CTRL_FLASH_CMD_CTRL_EXECUTE_SHIFT)|\
                                            ((0x0 & FLASH_CTRL_FLASH_CMD_CTRL_EXECUTE_STA_MASK) << FLASH_CTRL_FLASH_CMD_CTRL_EXECUTE_STA_SHIFT)|\
                                            ((0x0 & FLASH_CTRL_FLASH_CMD_CTRL_DUMMY_BYTES_NUM_MASK) << FLASH_CTRL_FLASH_CMD_CTRL_DUMMY_BYTES_NUM_SHIFT)|\
                                            ((0x0 & FLASH_CTRL_FLASH_CMD_CTRL_WRITE_NUM_MASK) << FLASH_CTRL_FLASH_CMD_CTRL_WRITE_NUM_SHIFT)|\
                                            ((0x0 & FLASH_CTRL_FLASH_CMD_CTRL_WRITE_EN_MASK) << FLASH_CTRL_FLASH_CMD_CTRL_WRITE_EN_SHIFT)|\
                                            ((0x2 & FLASH_CTRL_FLASH_CMD_CTRL_ADDR_BYTES_NUM_MASK) << FLASH_CTRL_FLASH_CMD_CTRL_ADDR_BYTES_NUM_SHIFT)|\
                                            ((0x0 & FLASH_CTRL_FLASH_CMD_CTRL_MODE_BIT_EN_MASK) << FLASH_CTRL_FLASH_CMD_CTRL_MODE_BIT_EN_SHIFT)|\
                                            ((0x1 & FLASH_CTRL_FLASH_CMD_CTRL_CMD_ADDR_EN_MASK) << FLASH_CTRL_FLASH_CMD_CTRL_CMD_ADDR_EN_SHIFT)|\
                                            ((0x0 & FLASH_CTRL_FLASH_CMD_CTRL_READ_NUM_MASK) << FLASH_CTRL_FLASH_CMD_CTRL_READ_NUM_SHIFT)|\
                                            ((0x0 & FLASH_CTRL_FLASH_CMD_CTRL_READ_EN_MASK) << FLASH_CTRL_FLASH_CMD_CTRL_READ_EN_SHIFT)|\
                                            ((u8Cmd & FLASH_CTRL_FLASH_CMD_CTRL_CMD_OPCODE_MASK) << FLASH_CTRL_FLASH_CMD_CTRL_CMD_OPCODE_SHIFT))

#define FLASH_CTRL_FLASH_CMD_WRITE_NO_ADDR_INIT(u8Cmd, u8Num, u8DummyNum)    ((0x1 << FLASH_CTRL_FLASH_CMD_CTRL_EXECUTE_SHIFT)|\
                                            (0x0 << FLASH_CTRL_FLASH_CMD_CTRL_EXECUTE_STA_SHIFT)|\
                                            (u8DummyNum << FLASH_CTRL_FLASH_CMD_CTRL_DUMMY_BYTES_NUM_SHIFT)|\
                                            (u8Num << FLASH_CTRL_FLASH_CMD_CTRL_WRITE_NUM_SHIFT)|\
                                            (FLASH_CTRL_FLASH_CMD_CTRL_WRITE_EN_MASK << FLASH_CTRL_FLASH_CMD_CTRL_WRITE_EN_SHIFT)|\
                                            (0x0 << FLASH_CTRL_FLASH_CMD_CTRL_ADDR_BYTES_NUM_SHIFT)|\
                                            (0x0 << FLASH_CTRL_FLASH_CMD_CTRL_MODE_BIT_EN_SHIFT)|\
                                            (0x0 << FLASH_CTRL_FLASH_CMD_CTRL_CMD_ADDR_EN_SHIFT)|\
                                            (0x0 << FLASH_CTRL_FLASH_CMD_CTRL_READ_NUM_SHIFT)|\
                                            (0x0 << FLASH_CTRL_FLASH_CMD_CTRL_READ_EN_SHIFT)|\
                                            (u8Cmd << FLASH_CTRL_FLASH_CMD_CTRL_CMD_OPCODE_SHIFT))

#define FLASH_CTRL_FLASH_CMD_READ_NO_ADDR_INIT(u8Cmd, u8Num, u8DummyNum)    ((0x1 << FLASH_CTRL_FLASH_CMD_CTRL_EXECUTE_SHIFT)|\
                                            (0x0 << FLASH_CTRL_FLASH_CMD_CTRL_EXECUTE_STA_SHIFT)|\
                                            (u8DummyNum << FLASH_CTRL_FLASH_CMD_CTRL_DUMMY_BYTES_NUM_SHIFT)|\
                                            (0x0 << FLASH_CTRL_FLASH_CMD_CTRL_WRITE_NUM_SHIFT)|\
                                            (0x0 << FLASH_CTRL_FLASH_CMD_CTRL_WRITE_EN_SHIFT)|\
                                            (0x2 << FLASH_CTRL_FLASH_CMD_CTRL_ADDR_BYTES_NUM_SHIFT)|\
                                            (0x0 << FLASH_CTRL_FLASH_CMD_CTRL_MODE_BIT_EN_SHIFT)|\
                                            (0x0 << FLASH_CTRL_FLASH_CMD_CTRL_CMD_ADDR_EN_SHIFT)|\
                                            (u8Num << FLASH_CTRL_FLASH_CMD_CTRL_READ_NUM_SHIFT)|\
                                            (FLASH_CTRL_FLASH_CMD_CTRL_READ_EN_MASK << FLASH_CTRL_FLASH_CMD_CTRL_READ_EN_SHIFT)|\
                                            (u8Cmd << FLASH_CTRL_FLASH_CMD_CTRL_CMD_OPCODE_SHIFT))

#define FLASH_CTRL_FLASH_CMD_READ_UID   ((0x1 << FLASH_CTRL_FLASH_CMD_CTRL_EXECUTE_SHIFT)|\
                                            (0x0 << FLASH_CTRL_FLASH_CMD_CTRL_EXECUTE_STA_SHIFT)|\
                                            (0x0 << FLASH_CTRL_FLASH_CMD_CTRL_DUMMY_BYTES_NUM_SHIFT)|\
                                            (0x0 << FLASH_CTRL_FLASH_CMD_CTRL_WRITE_NUM_SHIFT)|\
                                            (0x0 << FLASH_CTRL_FLASH_CMD_CTRL_WRITE_EN_SHIFT)|\
                                            (0x3 << FLASH_CTRL_FLASH_CMD_CTRL_ADDR_BYTES_NUM_SHIFT)|\
                                            (0x0 << FLASH_CTRL_FLASH_CMD_CTRL_MODE_BIT_EN_SHIFT)|\
                                            (0x1 << FLASH_CTRL_FLASH_CMD_CTRL_CMD_ADDR_EN_SHIFT)|\
                                            (0x7 << FLASH_CTRL_FLASH_CMD_CTRL_READ_NUM_SHIFT)|\
                                            (FLASH_CTRL_FLASH_CMD_CTRL_READ_EN_MASK << FLASH_CTRL_FLASH_CMD_CTRL_READ_EN_SHIFT)|\
                                            (GT_FLASH_CMD_READ_UNIQUE_ID << FLASH_CTRL_FLASH_CMD_CTRL_CMD_OPCODE_SHIFT))

#define FLASH_READ_CMD_INIT(u8Cmd, u8Num, u8DummyNum)    ((0x1 << FLASH_CTRL_FLASH_CMD_CTRL_EXECUTE_SHIFT)|\
                                            (0x0 << FLASH_CTRL_FLASH_CMD_CTRL_EXECUTE_STA_SHIFT)|\
                                            (u8DummyNum << FLASH_CTRL_FLASH_CMD_CTRL_DUMMY_BYTES_NUM_SHIFT)|\
                                            (0x0 << FLASH_CTRL_FLASH_CMD_CTRL_WRITE_NUM_SHIFT)|\
                                            (0x0 << FLASH_CTRL_FLASH_CMD_CTRL_WRITE_EN_SHIFT)|\
                                            (0x0 << FLASH_CTRL_FLASH_CMD_CTRL_ADDR_BYTES_NUM_SHIFT)|\
                                            (0x0 << FLASH_CTRL_FLASH_CMD_CTRL_MODE_BIT_EN_SHIFT)|\
                                            (0x0 << FLASH_CTRL_FLASH_CMD_CTRL_CMD_ADDR_EN_SHIFT)|\
                                            (u8Num << FLASH_CTRL_FLASH_CMD_CTRL_READ_NUM_SHIFT)|\
                                            (FLASH_CTRL_FLASH_CMD_CTRL_READ_EN_MASK << FLASH_CTRL_FLASH_CMD_CTRL_READ_EN_SHIFT)|\
                                            (u8Cmd << FLASH_CTRL_FLASH_CMD_CTRL_CMD_OPCODE_SHIFT))

//------------------------------------------------------------------------------
//0x94 Flash Command Address Register
//------------------------------------------------------------------------------
#define FLASH_CTRL_FLASH_CMD_ADDR_MASK                              (0xFFFFFFFF)
#define FLASH_CTRL_FLASH_CMD_ADDR_SHIFT                             (0)

#define FLASH_CTRL_FLASH_CMD_ADDR_REG_MASK  (FLASH_CTRL_FLASH_CMD_ADDR_MASK << FLASH_CTRL_FLASH_CMD_ADDR_SHIFT)

//------------------------------------------------------------------------------
//0xA0 flash Command Read Data Register(lower)
//------------------------------------------------------------------------------
#define FLASH_CTRL_FLASH_READ_DATA_MAX_SIZE_ONCE                    (8)
#define FLASH_CTRL_FLASH_READ_DATA_LOW_REG_MASK                     (0xFFFFFFFF)

//------------------------------------------------------------------------------
//0xA4 flash Command Read Data Register(upper)
//------------------------------------------------------------------------------
#define FLASH_CTRL_FLASH_READ_DATA_HI_REG_MASK                      (0xFFFFFFFF)

//------------------------------------------------------------------------------
//0xA8 flash Command Write Data Register(Lower)
//------------------------------------------------------------------------------
#define FLASH_CTRL_FLASH_WRITE_DATA_MAX_SIZE_ONCE                    (8)
#define FLASH_CTRL_FLASH_WRITE_DATA_LOW_REG_MASK                    (0xFFFFFFFF)

//------------------------------------------------------------------------------
//0xAC flash Command Write Data Register(upper)
//------------------------------------------------------------------------------
#define FLASH_CTRL_FLASH_WRITE_DATA_HI_REG_MASK                     (0xFFFFFFFF)

//------------------------------------------------------------------------------
//0xB0 Polling Flash Status Register
//------------------------------------------------------------------------------
//BIT[0:7] Flash status
#define FLASH_CTRL_FLASH_POLLING_STA_MASK                           (0xFF)
#define FLASH_CTRL_FLASH_POLLING_STA_SHIFT                          (0)

//BIT[8] Polling status effect
#define FLASH_CTRL_FLASH_POLLING_STA_EFFECT_MASK                    (0x1)
#define FLASH_CTRL_FLASH_POLLING_STA_EFFECT_SHIFT                   (8)


#define FLASH_CTRL_FLASH_POLLING_STA_REG_MASK   ((FLASH_CTRL_FLASH_POLLING_STA_MASK << FLASH_CTRL_FLASH_POLLING_STA_SHIFT)|\
                                                (FLASH_CTRL_FLASH_POLLING_STA_EFFECT_MASK << FLASH_CTRL_FLASH_POLLING_STA_EFFECT_SHIFT))



//------------------------------------------------------------------------------
//Cmd to flash
//------------------------------------------------------------------------------
#define FLASH_CTRL_CHIP_ERASE_CMD       (FLASH_CTRL_CMD_NOADDR_NODATA_INIT(GT_FLASH_CMD_CHIP_ERASE))

#define FLASH_CTRL_RELEASE_DEEP_POWER_DOWN  (FLASH_CTRL_CMD_NOADDR_NODATA_INIT(GT_FLASH_CMD_RELEASE_DEEP_POWER_DOWN))
#define FLASH_CTRL_RELEASE_READ_ENHANCED    (FLASH_CTRL_CMD_NOADDR_NODATA_INIT(PY_FLASH_CMD_RLEASE_READ_ENHANCED))
#define FLASH_CTRL_ENABLE_FLASH_RESET       (FLASH_CTRL_CMD_NOADDR_NODATA_INIT(GT_FLASH_CMD_ENABLE_RESET))
#define FLASH_CTRL_FLASH_RESET              (FLASH_CTRL_CMD_NOADDR_NODATA_INIT(GT_FLASH_CMD_RESET))


//Status 1 & 2 Can use 01h write.
#define FLASH_CTRL_WRITE_STATUS_QSPI_VALUE  ((EN_FLASH_STA2_QE ) )
#define FLASH_CTRL_WRITE_STATUS_SPI_VALUE   (0x0 )
#define FLASH_CTRL_WRITE_STATUS_QSPI_CMD    (FLASH_CTRL_FLASH_CMD_WRITE_NO_ADDR_INIT(GT_FLASH_CMD_WRITE_STATUS2, 0, 0))

#define FLASH_CTRL_READ_SECURITY_REG_CMD(u8Num)    (((0x1 << FLASH_CTRL_FLASH_CMD_CTRL_EXECUTE_SHIFT)|\
                                            (0x0 << FLASH_CTRL_FLASH_CMD_CTRL_EXECUTE_STA_SHIFT)|\
                                            (0x1 << FLASH_CTRL_FLASH_CMD_CTRL_DUMMY_BYTES_NUM_SHIFT)|\
                                            (0x0 << FLASH_CTRL_FLASH_CMD_CTRL_WRITE_NUM_SHIFT)|\
                                            (0x0 << FLASH_CTRL_FLASH_CMD_CTRL_WRITE_EN_SHIFT)|\
                                            (0x2 << FLASH_CTRL_FLASH_CMD_CTRL_ADDR_BYTES_NUM_SHIFT)|\
                                            (0x0 << FLASH_CTRL_FLASH_CMD_CTRL_MODE_BIT_EN_SHIFT)|\
                                            (0x1 << FLASH_CTRL_FLASH_CMD_CTRL_CMD_ADDR_EN_SHIFT)|\
                                            (u8Num << FLASH_CTRL_FLASH_CMD_CTRL_READ_NUM_SHIFT)|\
                                            (FLASH_CTRL_FLASH_CMD_CTRL_READ_EN_MASK << FLASH_CTRL_FLASH_CMD_CTRL_READ_EN_SHIFT)|\
                                            (GT_FLASH_CMD_READ_SECURITY_REG << FLASH_CTRL_FLASH_CMD_CTRL_CMD_OPCODE_SHIFT)))

#define FLASH_CTRL_WRITE_SECURITY_REG_CMD(u8Num)    (((0x1 << FLASH_CTRL_FLASH_CMD_CTRL_EXECUTE_SHIFT)|\
                                            (0x0 << FLASH_CTRL_FLASH_CMD_CTRL_EXECUTE_STA_SHIFT)|\
                                            (0x0 << FLASH_CTRL_FLASH_CMD_CTRL_DUMMY_BYTES_NUM_SHIFT)|\
                                            ((u8Num) << FLASH_CTRL_FLASH_CMD_CTRL_WRITE_NUM_SHIFT)|\
                                            (0x1 << FLASH_CTRL_FLASH_CMD_CTRL_WRITE_EN_SHIFT)|\
                                            (0x2 << FLASH_CTRL_FLASH_CMD_CTRL_ADDR_BYTES_NUM_SHIFT)|\
                                            (0x0 << FLASH_CTRL_FLASH_CMD_CTRL_MODE_BIT_EN_SHIFT)|\
                                            (0x1 << FLASH_CTRL_FLASH_CMD_CTRL_CMD_ADDR_EN_SHIFT)|\
                                            (0x0 << FLASH_CTRL_FLASH_CMD_CTRL_READ_NUM_SHIFT)|\
                                            (0x0 << FLASH_CTRL_FLASH_CMD_CTRL_READ_EN_SHIFT)|\
                                            (GT_FLASH_CMD_WRITE_SECURITY_REG << FLASH_CTRL_FLASH_CMD_CTRL_CMD_OPCODE_SHIFT)))

#define FLASH_CTRL_ERASE_SECURITY_REG_CMD    (((0x1 << FLASH_CTRL_FLASH_CMD_CTRL_EXECUTE_SHIFT)|\
                                            (0x0 << FLASH_CTRL_FLASH_CMD_CTRL_EXECUTE_STA_SHIFT)|\
                                            (0x0 << FLASH_CTRL_FLASH_CMD_CTRL_DUMMY_BYTES_NUM_SHIFT)|\
                                            (0x0 << FLASH_CTRL_FLASH_CMD_CTRL_WRITE_NUM_SHIFT)|\
                                            (0x0 << FLASH_CTRL_FLASH_CMD_CTRL_WRITE_EN_SHIFT)|\
                                            (0x2 << FLASH_CTRL_FLASH_CMD_CTRL_ADDR_BYTES_NUM_SHIFT)|\
                                            (0x0 << FLASH_CTRL_FLASH_CMD_CTRL_MODE_BIT_EN_SHIFT)|\
                                            (0x1 << FLASH_CTRL_FLASH_CMD_CTRL_CMD_ADDR_EN_SHIFT)|\
                                            (0x0 << FLASH_CTRL_FLASH_CMD_CTRL_READ_NUM_SHIFT)|\
                                            (0x0 << FLASH_CTRL_FLASH_CMD_CTRL_READ_EN_SHIFT)|\
                                            (GT_FLASH_CMD_ERASE_SECURITY_REG << FLASH_CTRL_FLASH_CMD_CTRL_CMD_OPCODE_SHIFT)))


#define FLASH_CTRL_WRITE_FLASH_CTRL_REG_CMD(opCode) (FLASH_CTRL_FLASH_CMD_WRITE_NO_ADDR_INIT(opCode, 0, 0))

#endif
