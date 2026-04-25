/**
 * ****************************************************************************
 * @file gt25q20d.h
 * @author SLC(SC) software team. 
 * @brief This file is a command package file for GT25Q20D, which can be used 
 *        for WIN series chips with Flash_Ctrl.
 * @version  1.0
 * ****************************************************************************
 * Copyright (c) 2024 SLC Semiconductor all rights reserved.
 * ****************************************************************************
 */

#ifndef __GT25Q20D_H__
#define __GT25Q20D_H__

//------------------------------------------------------------------------------
//Flash gerneral setting
//------------------------------------------------------------------------------
#define GT_FLASH_MAX_SIZE_IN_BYTE                          (0x40000)   //256kb
//#define GT_FLASH_PAGE_SIZE_IN_BYTE                         (0x100)     //256byte no page erase
#define GT_FLASH_SECTOR_SIZE_IN_BYTE                       (0x1000)     //4KB
#define GT_FLASH_32KBLOCK_SIZE_IN_BYTE                     (0x8000)    //32kB
#define GT_FLASH_64KBLOCK_SIZE_IN_BYTE                     (0x10000)   //64kB

//------------------------------------------------------------------------------
//Flash Cmds
//------------------------------------------------------------------------------
#define GT_FLASH_CMD_WRITE_ENABLE                       (0x06)
#define GT_FLASH_CMD_WRITE_DISABLE                      (0x04)
#define GT_FLASH_CMD_SR_WRITE_ENABLE                    (0x50)
#define GT_FLASH_CMD_READ_STATUS1                       (0x05)  // Can read all status total
#define GT_FLASH_CMD_READ_STATUS2                       (0x35)
#define GT_FLASH_CMD_READ_STATUS3                       (0x15)
#define GT_FLASH_CMD_WRITE_STATUS1                      (0x01)
#define GT_FLASH_CMD_WRITE_STATUS2                      (0x31)
#define GT_FLASH_CMD_WRITE_STATUS3                      (0x11)
#define GT_FLASH_CMD_READ_DATA                          (0x03)
#define GT_FLASH_CMD_FAST_READ                          (0x0B)
#define GT_FLASH_CMD_DUAL_READ                          (0x3B)
#define GT_FLASH_CMD_QUAD_OUTPUT_READ                   (0x6B)
#define GT_FLASH_CMD_FAST_DUAL_READ                     (0xBB)
#define GT_FLASH_CMD_FAST_QUAD_READ                       (0xEB)
#define GT_FLASH_CMD_PAGE_WRITE                         (0x02)
#define GT_FLASH_CMD_QUAD_PAGE_WRITE                    (0x32)
#define GT_FLASH_CMD_SECTOR_ERASE                       (0x20)
#define GT_FLASH_CMD_32K_ERASE                          (0x52)
#define GT_FLASH_CMD_64K_ERASE                          (0xD8)
#define GT_FLASH_CMD_CHIP_ERASE                         (0xC7)   //or 0x60
#define GT_FLASH_CMD_ENABLE_RESET                       (0x66)
#define GT_FLASH_CMD_RESET                              (0x99)
#define GT_FLASH_CMD_SET_BURST_WRAP                     (0x77)
#define GT_FLASH_CMD_ERASE_SUSPEND                      (0x75)   //the same with write
#define GT_FLASH_CMD_ERASE_RESUME                       (0x7A)
#define GT_FLASH_CMD_DEEP_POWER_DOWN                    (0xB9)
#define GT_FLASH_CMD_RELEASE_DEEP_POWER_DOWN            (0xAB)   //the same with read dev id
#define GT_FLASH_CMD_READ_MANUFACTURER_ID               (0x90)
#define GT_FLASH_CMD_READ_MANUFACTURER_ID_DPI           (0x92)
#define GT_FLASH_CMD_READ_MANUFACTURER_ID_QPI           (0x94)
#define GT_FLASH_CMD_READ_ID                            (0x9F)
#define GT_FLASH_CMD_READ_UNIQUE_ID                     (0x4B)
#define GT_FLASH_CMD_READ_SFDP_REG                      (0x5A)
#define GT_FLASH_CMD_ERASE_SECURITY_REG            (0x44)
#define GT_FLASH_CMD_WRITE_SECURITY_REG            (0x42)
#define GT_FLASH_CMD_READ_SECURITY_REG             (0x48)

//------------------------------------------------------------------------------
//FLash cmd dummy bytes size
//------------------------------------------------------------------------------
#define GT_FLASH_DUMMY_BYTE_QUAD_OUTPUT_READ            (1)     //6BH
#define GT_FLASH_DUMMY_BYTE_QUAD_FAST_READ              (2)     //EBH 4clk
#define GT_FLASH_DUMMY_BYTE_READ_DEV_SEQ                (1)
#define GT_FLASH_DUMMY_BYTE_QUAD_READ_MANUFACTURE_ID    (2)     //4clk
#define GT_FLASH_DUMMY_BYTE_READ_UNIQUE_ID              (4)
#define GT_FLASH_DYMMY_BYTE_READ_SFPD_REG               (1)
#define GT_FLASH_DYMMY_BYTE_READ_SECURITY_REG           (1)

//------------------------------------------------------------------------------
//Max time costs for flash operation
//------------------------------------------------------------------------------
#define GT_FLASH_TIMING_DEEP_POWER_DOWN_US              (3)
#define GT_FLASH_TIMING_STANDBY_WITHOUT_ID_READ_US      (10)    //Release deep power down
#define GT_FLASH_TIMING_STANDBY_WITH_ID_READ_US         (10)
#define GT_FLASH_TIMING_SUSPEND_US                      (20)
#define GT_FLASH_TIMING_RESET_US                        (30)
#define GT_FLASH_TIMING_CHIP_ERASE_AFTER_RESET_US       (150)
#define GT_FLASH_TIMING_WRITE_STATUS_REG_US             (5000)
#define GT_FLASH_TIMING_FIRST_BYTE_PROGRAM_US           (150)
#define GT_FLASH_TIMING_PAGE_PROGRAM_US                 (2500)
#define GT_FLASH_TIMING_SECTOR_ERASE_US                 (8000)
#define GT_FLASH_TIMING_32K_ERASE_US                    (8000)
#define GT_FLASH_TIMING_64K_ERASE_US                    (8000)
#define GT_FLASH_TIMING_CHIP_ERASE_US                   (14000)

//------------------------------------------------------------------------------
//Status register 1 bit field
//------------------------------------------------------------------------------
typedef enum
{
    EN_FLASH_STA1_BUSY                                   = (1 << 0), //S0
    EN_FLASH_STA1_WEL                                    = (1 << 1), //S1 write enable latch
    EN_FLASH_STA1_BP0                                    = (1 << 2), //S2 block0 protect
    EN_FLASH_STA1_BP1                                    = (1 << 3), //S3 block1 protect
    EN_FLASH_STA1_BP2                                    = (1 << 4), //S4 block2 protect
    EN_FLASH_STA1_TB                                     = (1 << 5), //S5 top/bottom protect
    EN_FLASH_STA1_SEC                                    = (1 << 6), //S6 sector protect
    EN_FLASH_STA1_SRP0                                   = (1 << 7), //S7 status register protect
}EN_FLASH_STATUS1_T;

//------------------------------------------------------------------------------
//Status register 2 bit field
//------------------------------------------------------------------------------
typedef enum
{
    EN_FLASH_STA2_SRP1                                   = (1 << 0), //S0 status register lock
    EN_FLASH_STA2_QE                                     = (1 << 1), //S1 quad enable Non-Volatile Writable
    EN_FLASH_STA2_LB                                     = (1 << 2), //S2 security register lock bits Volatile/Non-Volatile OTP Writable
    //bit[3:5] res.
    EN_FLASH_STA2_CMP                                    = (1 << 6), //S6 complement protect
    EN_FLASH_STA2_SUS                                    = (1 << 7), //S7 suspend status
}EN_FLASH_STATUS2_T;

//------------------------------------------------------------------------------
//Status register 3 bit field
//------------------------------------------------------------------------------
typedef enum
{
    //bit[0:4] res.
    EN_FLASH_STA3_DRV0                                   = (1 << 5), //S5 output driver strength bit0
    EN_FLASH_STA3_DRV1                                   = (1 << 6), //S6 output driver strength bit1
}EN_FLASH_STATUS3_T;


#endif



