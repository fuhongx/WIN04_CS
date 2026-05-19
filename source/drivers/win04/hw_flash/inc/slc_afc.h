/**
 * ****************************************************************************
 * @file slc_afc.h
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

#ifndef __SLC_AFC_H__
#define __SLC_AFC_H__

#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>

#include "reg_Dev.h"
#include "error_def.h"
#include "utility.h"

#include "slc_phy.h"
#include "slc_rf.h"
#include "slc_subg.h"

typedef enum
{
    SLC_PHY_PARAM,
    SLC_MAC_PARAM,
    SLC_PHY_MAC_PARAM,
} afc_farrow_type_e;

void slc_afc_adjust(phy_cfg_trx_e trx_type, phy_cfg_tx_mode_e tx_mode);
void slc_afc_set_sw_flag(bool flag);
bool slc_afc_get_sw_flag(void);

#endif
