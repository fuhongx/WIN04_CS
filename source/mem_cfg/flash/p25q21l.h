/**
 * ****************************************************************************
 * @file p25q21l.h
 * @author SLC(SC) software team. 
 * @brief This file is a command package file for P25Q21L, which can be used 
 *        for WIN series chips with Flash_Ctrl.
 * @version  1.0
 * ****************************************************************************
 * 
 * Copyright (c) 2024 SLC Semiconductor all rights reserved.
 * 
 * ****************************************************************************
 */
#ifndef __P25Q21L_H__
#define __P25Q21L_H__

#define PY_FLASH_PAGE_SIZE_IN_BYTE                          (0x100)
#define PY_FLASH_SECTOR_SIZE_IN_BYTE                        (0x1000)     //4KB
#define PY_FLASH_32KBLOCK_SIZE_IN_BYTE                      (0x8000)    //32kB
#define PY_FLASH_64KBLOCK_SIZE_IN_BYTE                      (0x10000)   //64kB
//------------------------------------------------------------------------------
//Flash Cmds
//------------------------------------------------------------------------------
#define PY_FLASH_CMD_FREAD                                  (0x0B) //Same with GT
#define PY_FLASH_CMD_READ                                   (0x03) //Same with GT
#define PY_FLASH_CMD_DREAD                                  (0x3B) //Same with GT
#define PY_FLASH_CMD_2READ                                  (0xBB) //Same with GT
#define PY_FLASH_CMD_QREAD                                  (0x6B) //Same with GT
#define PY_FLASH_CMD_4READ                                  (0xEB) //Same with GT
//Program & Erase
#define PY_FLASH_CMD_PE                                     (0x81) //Py only function!!!!!!.
#define PY_FLASH_CMD_SE                                     (0x20) //Same with Gt
#define PY_FLASH_CMD_BE32                                   (0x52) //Same with GT
#define PY_FLASH_CMD_BE64                                   (0xD8) //Same with GT
#define PY_FLASH_CMD_CE                                     (0x60) //Same with GT
#define PY_FLASH_CMD_CE2                                    (0xC7) //Same with GT
#define PY_FLASH_CMD_PP                                     (0x02) //Same with GT
#define PY_FLASH_CMD_2PP                                    (0xA2) //Py only function!!!!!!.
#define PY_FLASH_CMD_QPP                                    (0x32) //Same with GT
#define PY_FLASH_CMD_PES                                    (0x75) //Same with GT
#define PY_FLASH_CMD_PES2                                   (0xB0) //Py only function!!!!!!.
#define PY_FLASH_CMD_PER                                    (0x7A) //Same with GT
#define PY_FLASH_CMD_PER2                                   (0x30) //Py only function!!!!!!.
//Protection
#define PY_FLASH_CMD_WREN                                   (0x06) //Same with GT
#define PY_FLASH_CMD_WRDI                                   (0x04) //Same with GT
#define PY_FLASH_CMD_VWREN                                  (0x50) //Same with GT
#define PY_FLASH_CMD_ERSCUR                                 (0x44) //Same with GT
#define PY_FLASH_CMD_PRSCUR                                 (0x42) //Same with GT
#define PY_FLASH_CMD_PDSCUR                                 (0x48) //Same with GT
//Status
#define PY_FLASH_CMD_RDSR                                   (0x05) //Same with GT Status1
#define PY_FLASH_CMD_RDSR1                                  (0x35) //Same with GT Status2
#define PY_FLASH_CMD_ASI                                    (0x25) //Py only function!!!!!!.
#define PY_FLASH_CMD_WRSR                                   (0x01) //Same with GT Write status1
//Other Cmd
#define PY_FLASH_CMD_RSTEN                                  (0x66) //Same with GT
#define PY_FLASH_CMD_RST                                    (0x99) //Same with GT
#define PY_FLASH_CMD_ENABLE_QPI                             (0x38)
#define PY_FLASH_CMD_RDID                                   (0x9F) //Same with GT
#define PY_FLASH_CMD_REMS                                   (0x90) //Same with GT
#define PY_FLASH_CMD_DREMS                                  (0x92) //Same with GT
#define PY_FLASH_CMD_QREMS                                  (0x94) //Same with GT
#define PY_FLASH_CMD_DP                                     (0xB9) //Same with GT
#define PY_FLASH_CMD_RDP                                    (0xAB) //Same with GT
#define PY_FLASH_CMD_SBL                                    (0x77) //Same with GT
#define PY_FLASH_CMD_RDSFDP                                 (0x5A) //Same with GT
#define PY_FLASH_CMD_RLEASE_READ_ENHANCED                   (0xFF) //Same
#define PY_FLASH_CMD_RUID                                   (0x4B) //Same with GT

//------------------------------------------------------------------------------
//FLash cmd dummy bytes size
//------------------------------------------------------------------------------
#define PY_FLASH_DUMMY_BYTE_DREAD                           (1) //3B
#define PY_FLASH_DUMMY_BYTE_QREAD                           (4) //EB 8clk
#define PY_FLASH_DUMMY_BYTE_4READ                           (2) //EB 4Clk M5-4 != (1,0)
#define PY_FLASH_DUMMY_BYTE_4READ_ENHANCE                   (2) //EB 4Clk M5-4 != (1,0)
#define PY_FLASH_DUMMY_BYTE_RDSCUR                          (1) //48 8Clk
#define PY_FLASH_DUMMY_BYTE_RES                             (3) //AB 24Clk
#define PY_FLASH_DUMMY_BYTE_DREMS                           (2) //92 16Clk
#define PY_FLASH_DUMMY_BYTE_QREMS                           (2) //94 4Clk
#define PY_FLASH_DUMMY_BYTE_RUID                            (4) //4B 32Clk
#define PY_FLASH_DUMMY_BYTE_RDSFDP                          (1) //5A 8Clk


//------------------------------------------------------------------------------
//Max time costs for flash operation
//------------------------------------------------------------------------------
#define PY_FLASH_TIMING_DEEP_POWER_DOWN_US                  (3) //Same with GT
#define PY_FLASH_TIMING_STANDBY_WITHOUT_ID_READ_US          (8) //lease than GT
#define PY_FLASH_TIMING_STANDBY_WITH_ID_READ_US             (8) //lease than GT
#define PY_FLASH_TIMING_WRITE_STATUS_REG_US                 (12000) //more than GT
#define PY_FLASH_TIMING_RESET_US                            (30) //Same with GT
#define PY_FLASH_TIMING_SUSPEND_US                          (30) //more than GT
#define PY_FLASH_TIMING_PROGRAM_RESUME_TO_SUSPEND_US        (100) 
#define PY_FLASH_TIMING_ERASE_RESUME_TO_SUSPEND_US          (200)
#define PY_FLASH_TIMING_PP_US                               (3000) //3ms pageprogram
//typ.=8ms = Gt
#define PY_FLASH_TIMER_PROGRAM_TYPICAL_US                   (8000)
#define PY_FLASH_TIMING_PE_US                               (20000) //20ms Page Erase
#define PY_FLASH_TIMING_SE_US                               (20000) 
#define PY_FLASH_TIMING_BE32_US                             (20000) 
#define PY_FLASH_TIMING_BE64_US                             (20000) 
#define PY_FLASH_TIMING_CE_US                               (20000) 



#endif

