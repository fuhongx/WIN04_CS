/**
 * ****************************************************************************
 * @file version.c
 * @author SLC(SC) software team. 
 * @brief 
 * @version  1.0
 * ****************************************************************************
 * 
 * THIS CODE IS LIMITED TO GUIDE APPLICATION DEVELOPMENT AND INCREASE DEVELOPMENT SPEED. 
 * AS A RESULT, SILICONDUCTOR IS NOT RESPONSIBLE FOR ANY CLAIMS ARISING DIRECTLY OR INDIRECTLY FROM SUCH FIRMWARE AND/OR CONTENT.
 * 
 * Copyright (c) 2025 Siliconductor Semiconductor all rights reserved.
 * 
 * ****************************************************************************
 */

#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>

#include "reg_Dev.h"
#include "error_def.h"
#include "utility.h"
#include "version.h"

#ifdef BOOT2PROJECT
//It can only use in Boot2 project //FLASH_BOOT2_CODE_ADDR
const boot2_header_t gstBoot2Version __attribute__((section(".boot2.header"))) = {.u32Boot2Flag = BOOT2CCODE_FLAG ,\
                                                                                    .u32Boot2Ver = BOOT2CODE_VERSION,\
                                                                                    .u32Boot2Len = 0x1000,\
                                                                                    .u32Boot2Crc = 0xF1F3,\
                                                                                    .u32Boot2LoadAddr = RAM_BOOT2_CODE_ADDR,\
                                                                                    .u32Boot2HeaderCrc = 0xabcd};

#else
const fw_header_t gstAppVersion __attribute__((section(".firmware.header"))) = {.u32FirmwareFlag = FIRMWARE_FLAG,\
                                                                                 .u32FirmwareVer = FIRMWARE_VERSION,\
                                                                                 .u32FirmwareLen = 0x1E000,\
                                                                                 .u32FirmwareCrc = 0xF1F2F3F4,\
                                                                                 .u32FirmwareAddr = FLASH_BOOT2_RESET_HANDLER_ADDR,\
                                                                                 .u32FirmwareHeaderCrc = 0
                                                                                 };
#endif
