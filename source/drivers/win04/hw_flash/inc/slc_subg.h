/**
 * ****************************************************************************
 * @file slc_subg.h
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

#ifndef __SLC_SUBG_H__
#define __SLC_SUBG_H__

#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#include "reg_Dev.h"
#include "error_def.h"
#include "utility.h"

/* SUBG2 config func, only for FPGA */
void slc_rf_subg_set_gpio(void);
void slc_rf_subg_disable_trx(void);
void slc_rf_subg_set_tx_en(void);
void slc_rf_subg_set_rx_en(void);
EN_ERR_STA_T slc_rf_subg_afc_cali(void);
void slc_rf_subg_fc_cali(uint32_t freq);
void slc_rf_subg_set_fc(uint32_t freq);
void slc_rf_subg_set_sync_opt_agc_opt(void);

#endif
