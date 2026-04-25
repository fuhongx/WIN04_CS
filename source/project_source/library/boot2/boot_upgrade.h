/**
 * ****************************************************************************
 * @file boot_upgrade.h
 * @author SLC(SC) software team. 
 * @brief 
 * @version  1.0
 * ****************************************************************************
 * 
 * THIS CODE IS LIMITED TO GUIDE THE APPLICATION AND IMPROVE THE EFFICIENCY OF RESEARCH AND DEVELOPMENT.
 * AS A RESULT, SILICONDUCTOR IS NOT RESPONSIBLE FOR ANY CLAIMS ARISING DIRECTLY OR INDIRECTLY FROM SUCH FIRMWARE AND/OR CONTENT.
 * 
 * Copyright (c) 2025 Siliconductor Semiconductor all rights reserved.
 * 
 * ****************************************************************************
 */

#ifndef __BOOT_UPGRADE_H__
#define __BOOT_UPGRADE_H__

#include "reg_Dev.h"
#include "error_def.h"
#include "version.h"

bool boot_upgrade_update_boot2_code(boot2_header_t *pstBoot2);
bool boot_upgrade_update_app_code(fw_header_t *pstFirm);

#endif
