/**
 * @file     flash_prog.h
 * @version  V1.0
 * @date     2020/02/23
 * @history
 * @note
 * @attention
 *
 * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS WITH CODING INFORMATION REGARDING THEIR
 * PRODUCTS IN ORDER FOR THEM TO SAVE TIME. AS A RESULT, MAXSCEND SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
 * INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE
 * USE MADE BY CUSTOMERS OF THE CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
 *
 * Copyright (c) 2016~2023, Maxscend Microelectronics Company Limited.
 * All rights reserved.
 */

#ifndef __FLASH_PROG_H__
#define __FLASH_PROG_H__

/**
 * @brief  Macro definition.
 */
#define FLASH_DEVICE_NAME                  "WIN04_Flash_Algorithm_V01"

#define KBYTE                             (1024)
#define FLASH_BYTES_PER_PAGE              (256)
#define FLASH_BYTES_PER_PAGE_SHIFT_BITS   (8)
#define FLASH_BYTES_PER_SECTOR            (4096) // 4kB
#define FLASH_BYTES_PER_SECTOR_SHIFT_BITS (12)

// Flash base address
#define FLASH_BASE_ADDR                   (0x08000000)
#define MAX_FLASH_SIZE                    (256 * KBYTE)

// RAM base address
#define RAM_BASE_ADDR                     (0x20000000)
#define MAX_RAM_SIZE                      (64 * KBYTE)

/**
 * @brief  Function code enumeration definition.
 */
typedef enum
{
    FUNC_CODE_ERASE  = 1,
    FUNC_CODE_PROGM  = 2,
    FUNC_CODE_VERFIY = 3,

} enFunCode_t;

#endif //__FLASH_PROG_H__
