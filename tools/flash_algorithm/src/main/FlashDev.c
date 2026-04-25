/***********************************************************************/
/*  This file is part of the ARM Toolchain package                     */
/*  Copyright (c) 2010 Keil - An ARM Company. All rights reserved.     */
/***********************************************************************/
/*                                                                     */
/*  FlashDev.C:  Device Description for New Device Flash               */
/*                                                                     */
/***********************************************************************/

#include "FlashOS.H" // FlashOS Structures
#include "flash_prog.h"

struct FlashDevice const FlashDevice = {
    FLASH_DRV_VERS,       // Driver Version, do not modify!
    FLASH_DEVICE_NAME,    // Device Name
    ONCHIP,               // Device Type
    FLASH_BASE_ADDR,      // Device Start Address
    MAX_FLASH_SIZE,       // Device Size in Bytes (1024kB)
    FLASH_BYTES_PER_PAGE, // Programming Page Size
    0,                    // Reserved, must be 0
    0xFF,                 // Initial Content of Erased Memory
    3000,                 // Program Page Timeout 100 mSec
    3000,                 // Erase Sector Timeout 3000 mSec

    // Specify Size and Address of Sectors
    FLASH_BYTES_PER_SECTOR,
    0x000000,
    SECTOR_END,
};
