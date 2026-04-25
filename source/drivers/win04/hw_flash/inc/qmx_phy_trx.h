/**
 * ****************************************************************************
 * @file qmx_phy_trx.h
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

#ifndef __QMX_PHY_TRX_H__
#define __QMX_PHY_TRX_H__

#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#include "reg_Dev.h"
#include "error_def.h"
#include "utility.h"

#include "qmx_phy.h"
#include "qmx_rf.h"
#ifdef QMX_FPGA
#include "qmx_subg.h"
#endif

typedef struct
{
    uint32_t freq;
    uint16_t sf;        // 5~12, refer to phy_cfg_sf_e
    uint16_t bw;        // refer to phy_cfg_bw_e
    uint16_t flo;       // refer to qmx_rf_flo_e
} phy_trx_cfg_param_t;

phy_trx_cfg_param_t *qmx_phy_get_trx_config_param(void);
void qmx_phy_trx_config(phy_cfg_t *cfg, uint16_t netid, phy_cfg_trx_e trx,
                        qmx_rf_flo_e flo, uint32_t freq, phy_cfg_tx_mode_e tx_mode);
void qmx_phy_start_trans_tx_data(phy_cfg_tx_mode_e tx_mode, rf_power_e power);
void qmx_phy_start_get_rx_data(void);

void qmx_phy_single_tone(void);

#endif /* __QMX_PHY_TRX_H__ */
