/**
 * ****************************************************************************
 * @file qmx_afc.h
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

#ifndef __QMX_AFC_H__
#define __QMX_AFC_H__

#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>

#include "reg_Dev.h"
#include "error_def.h"
#include "utility.h"

#include "qmx_phy.h"
#include "qmx_rf.h"
#include "qmx_subg.h"

typedef enum
{
    QMX_PHY_PARAM,
    QMX_MAC_PARAM,
    QMX_PHY_MAC_PARAM,
} afc_farrow_type_e;

void qmx_afc_adjust(phy_cfg_trx_e trx_type, phy_cfg_tx_mode_e tx_mode);
void qmx_afc_set_sw_flag(bool flag);
bool qmx_afc_get_sw_flag(void);

#endif
