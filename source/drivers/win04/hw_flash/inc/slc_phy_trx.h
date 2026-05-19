/**
 * ****************************************************************************
 * @file slc_phy_trx.h
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

#ifndef __SLC_PHY_TRX_H__
#define __SLC_PHY_TRX_H__

#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#include "reg_Dev.h"
#include "error_def.h"
#include "utility.h"

#include "slc_phy.h"
#include "slc_rf.h"
#ifdef SLC_FPGA
#include "slc_subg.h"
#endif

typedef struct
{
    uint32_t freq;
    uint16_t sf;        // 5~12, refer to phy_cfg_sf_e
    uint16_t bw;        // refer to phy_cfg_bw_e
    uint16_t flo;       // refer to slc_rf_flo_e
} phy_trx_cfg_param_t;

phy_trx_cfg_param_t *slc_phy_get_trx_config_param(void);
void slc_phy_trx_config(phy_cfg_t *cfg, uint16_t netid, phy_cfg_trx_e trx,
                        slc_rf_flo_e flo, uint32_t freq, phy_cfg_tx_mode_e tx_mode);
void slc_phy_start_trans_tx_data(phy_cfg_tx_mode_e tx_mode, rf_power_e power);
void slc_phy_start_get_rx_data(void);

void slc_phy_single_tone(void);

#endif /* __SLC_PHY_TRX_H__ */
