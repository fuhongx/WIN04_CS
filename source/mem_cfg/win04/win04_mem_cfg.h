/**
 * ****************************************************************************
 * @file win04_mem_cfg.h
 * @author SLC(SC) software team. 
 * @brief 
 * @version  1.0
 * ****************************************************************************
 * 
 * Copyright (c) 2024 SLC Semiconductor all rights reserved.
 * 
 * ****************************************************************************
 */

#ifndef __WIN04_MEM_CFG_H__
#define __WIN04_MEM_CFG_H__

#ifdef __cplusplus
extern "C"
{
#endif


//Base unit for flash or mem
#define UNIT_1K                                     (0x400)
#define UNIT_PAGE                                   (256)
#define UNIT_SECTOR                                 (4 * UNIT_1K)

//Base WIN04 mem cfg
#define ROM_LIB_BASE_ADDR                           (0x0)
#define ROM_LIB_MAX_SIZE                            (32 * UNIT_1K)
#define ROM_LIB_MAX_ADDR                            (ROM_LIB_BASE_ADDR + ROM_LIB_MAX_SIZE)

//Flash ADDR
#define FLASH_BASE_ADDR                             (0x08000000)

//------------------------------------------------------------------------------
//Flash Area with 256KB
//------------------------------------------------------------------------------
#define FLASH_SIZE_MAX                              (256 * UNIT_1K)
#define FLASH_MAX_ADDR                              (FLASH_BASE_ADDR + FLASH_SIZE_MAX)


//WIN04 Direct boot addr
#define FLASH_RESET_HANDLER_ADDR                    (FLASH_BASE_ADDR + 0x04)           //From 0x8000004

//WIN04 Boot2 Addr
#define FLASH_BOOT2_HEADER_ADDR                     (FLASH_BASE_ADDR)
#define FLASH_BOOT2_HEADER_SIZE                     (120)


#define FLASH_BOOT2_HEADER_MAX                      (FLASH_BOOT2_HEADER_ADDR + FLASH_BOOT2_HEADER_SIZE)

#define FLASH_BOOT2_CODE_ADDR                       (FLASH_BOOT2_HEADER_MAX)
#define FLASH_BOOT2_SIZE                            (4 * UNIT_1K)
#define FLASH_BOOT2_CODE_MAX                        (FLASH_BOOT2_HEADER_ADDR + FLASH_BOOT2_SIZE)

//------------------------------------------------------------------------------
//Code Addr start
//------------------------------------------------------------------------------
#define FLASH_BOOT2_FIRMWARE_HEADER_ADDR            (FLASH_BOOT2_CODE_MAX)
#define FLASH_BOOT2_FIRMWARE_HEADER_SIZE            (120)

#define FLASH_CODE_ADDR_BASE                        (FLASH_BOOT2_FIRMWARE_HEADER_ADDR + FLASH_BOOT2_FIRMWARE_HEADER_SIZE)

#define FLASH_BOOT2_RESET_HANDLER_ADDR              (FLASH_CODE_ADDR_BASE + 0x4)

#define FLASH_CODE_BLOCK_A_SIZE                     (124 * UNIT_1K)
#define FLASH_CODE_BLOCK_A_MAX_ADDR                 (FLASH_BASE_ADDR + FLASH_CODE_BLOCK_A_SIZE)
#if (FLASH_CODE_BLOCK_A_MAX_ADDR > FLASH_MAX_ADDR)
    #error "Code A Over Flow!"
#endif

//Upgrade Boot2 Info
#define FLASH_FIRM_UP_BOOT2_HEADER_ADDR             (FLASH_BASE_ADDR + FLASH_CODE_BLOCK_A_SIZE)
#define FLASH_FIRM_UP_BOOT2_CODE_ADDR               (FLASH_FIRM_UP_BOOT2_HEADER_ADDR + FLASH_BOOT2_HEADER_SIZE)
#define FLASH_FIRM_UP_BOOT2_ADDR_MAX                (FLASH_FIRM_UP_BOOT2_HEADER_ADDR + FLASH_BOOT2_SIZE)


#define FLASH_FIRM_UP_FIRMWARE_HEADER_ADDR          (FLASH_FIRM_UP_BOOT2_ADDR_MAX)


#define FLASH_FIRM_UP_FIRMWARE_HEADER_MAX_ADDR      (FLASH_FIRM_UP_FIRMWARE_HEADER_ADDR + FLASH_BOOT2_FIRMWARE_HEADER_SIZE)

#define FLASH_FIRM_UP_CODE_UPDATE_ADDR              (FLASH_FIRM_UP_FIRMWARE_HEADER_MAX_ADDR)

// User data area, for save data or config.
#define FLASH_USERER_DATA_SIZE                      (8 * UNIT_1K)
#define FLASH_USERER_DATA_ADDR                      (FLASH_MAX_ADDR - FLASH_USERER_DATA_SIZE)
#define FLASH_USERER_DATA_MAX_ADDR                  (FLASH_MAX_ADDR)


#define IS_INVALID_FLASH_ADDR(u32Addr)              ( ((u32Addr) > FLASH_BASE_ADDR) && ((u32Addr) < FLASH_MAX_ADDR) )


//------------------------------------------------------------------------------
// RAM MAP
//------------------------------------------------------------------------------
/* RAM base addr & total size */
#define RAM_BASE_ADDR                               (0x20000000)
#define RAM_SIZE_MAX                                (64 * UNIT_1K)
#define RAM_ADDR_MAX                                (RAM_BASE_ADDR + RAM_SIZE_MAX)

/* retention ram, for save data or config. */
#define RAM_RETENTION_BASE_ADDR                     (RAM_BASE_ADDR)
#define RAM_RETENTION_SIZE                          (16 * UNIT_1K)
#define RAM_RETENTION_MAX_ADDR                      (RAM_RETENTION_BASE_ADDR + RAM_RETENTION_SIZE)

/* RAM in ROM, this part use in rom lib, not use in app. */
// rom lib data shared with app
#define RAM_ROM_DATA_SHARED_ADDR                    (RAM_RETENTION_MAX_ADDR)
#define RAM_ROM_DATA_SHARED_SIZE_MAX                (0x200) // 512byte
#define RAM_ROM_DATA_SHARED_ADDR_MAX                (RAM_ROM_DATA_SHARED_ADDR + RAM_ROM_DATA_SHARED_SIZE_MAX)
// rom lib data addr
#define RAM_ROM_DATA_ADDR                           (RAM_ROM_DATA_SHARED_ADDR_MAX)
#define RAM_ROM_DATA_SIZE_MAX                       (0x600) // 1536byte, for .bss, .data, etc.
#define RAM_ROM_DATA_ADDR_MAX                       (RAM_ROM_DATA_ADDR + RAM_ROM_DATA_SIZE_MAX)

// rom STACK and HEAP
#define RAM_ROM_STACK_SIZE_MAX                      (8 * UNIT_1K) // @ref startup_ARMCM0plus.s
#define RAM_ROM_HEAP_SIZE_MAX                       (8 * UNIT_1K) // @ref startup_ARMCM0plus.s
#define RAM_ROM_STACK_HEAP_ADDR                     (RAM_ADDR_MAX - RAM_ROM_STACK_SIZE_MAX - RAM_ROM_HEAP_SIZE_MAX)
#define RAM_ROM_STACK_HEAP_SIZE_MAX                 (RAM_ROM_STACK_SIZE_MAX + RAM_ROM_HEAP_SIZE_MAX)
#define RAM_ROM_STACK_HEAP_MAX_ADDR                 (RAM_ADDR_MAX)

/* RAM in boot2 */
#define RAM_BOOT2_HEADER_ADDR                       (RAM_ROM_DATA_ADDR_MAX)
#define RAM_BOOT2_HEADER_SIZE                       (FLASH_BOOT2_HEADER_SIZE)
#define RAM_BOOT2_HEADER_MAX_ADDR                   (RAM_BOOT2_HEADER_ADDR + RAM_BOOT2_HEADER_SIZE)
#define RAM_BOOT2_CODE_ADDR                         (RAM_BOOT2_HEADER_MAX_ADDR)
#define RAM_BOOT2_CODE_MAX_ADDR                     (RAM_BOOT2_HEADER_ADDR + FLASH_BOOT2_SIZE)

/* RAM in App */
// RAM App vector addr
#define RAM_VECTOR_ADDR                             (RAM_RETENTION_BASE_ADDR)
#define RAM_VECTOR_SIZE_MAX                         (512)
#define RAM_VECTOR_MAX_ADDR                         (RAM_VECTOR_ADDR + RAM_VECTOR_SIZE_MAX)

// RAM App retention data & code
#define RAM_APP_RETENTION_ADDR                      (RAM_VECTOR_MAX_ADDR)
#define RAM_APP_RETENTION_SIZE                      (RAM_RETENTION_SIZE - RAM_VECTOR_SIZE_MAX)
#define RAM_APP_RETENTION_MAX_ADDR                  (RAM_APP_RETENTION_ADDR + RAM_APP_RETENTION_SIZE)

// stack is in retention ram
#define RAM_APP_STACK_SIZE_MAX                      (8 * UNIT_1K) //@ref startup_ARMCM0plus.s
#define RAM_APP_STACK_ADDR                          (RAM_RETENTION_MAX_ADDR - RAM_APP_STACK_SIZE_MAX)

// RAM App code
#define RAM_APP_CODE_ADDR                           (RAM_ROM_DATA_SHARED_ADDR_MAX)
#define RAM_APP_CODE_SIZE_MAX                       ((4 * UNIT_1K) + RAM_ROM_DATA_SIZE_MAX)
#define RAM_APP_CODE_MAX_ADDR                       (RAM_APP_CODE_ADDR + RAM_APP_CODE_SIZE_MAX)

// RAM App data, such as .bss, .data, etc.
#define RAM_APP_DATA_ADDR                           (RAM_APP_CODE_MAX_ADDR)
#define RAM_APP_DATA_SIZE_MAX                       (34 * UNIT_1K)
#define RAM_APP_DATA_MAX_ADDR                       (RAM_APP_DATA_ADDR + RAM_APP_DATA_SIZE_MAX)

// stack & heap 
#define RAM_APP_HEAP_SIZE_MAX                       (8 * UNIT_1K)
#define RAM_APP_HEAP_ADDR                           (RAM_ADDR_MAX - RAM_APP_HEAP_SIZE_MAX)

#ifdef __cplusplus
}
#endif


#endif
