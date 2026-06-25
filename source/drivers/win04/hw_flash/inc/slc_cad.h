/**
 * ****************************************************************************
 * @file slc_cad.h
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

#ifndef __SLC_CAD_H__
#define __SLC_CAD_H__

#include "slc_phy.h"
#include "slc_rf.h"
#include "slc_subg.h"

#define CAD_CYCLE_NUM_MAX 1023
#define CAD_CYCLE_NUM_MIN 1

#define CAD_MATCHADDR_WIDE_MAX 12
#define CAD_MATCHADDR_WIDE_MIN 0

typedef enum
{
    CAD_NORMAL_MODE,
    CAD_DUTY_CYCLE_MODE,
} cad_mode_e;

typedef struct
{
    uint16_t pre_addr;
    uint16_t pre_num;
    uint16_t cycle_num;         // 1~1023
    uint16_t matchaddr_wide;    // 0~12
    uint16_t marker;
    uint16_t nosleep_cycle;
} pre_cad_cfg_t;

void slc_cad_normal_enable(void);
void slc_cad_mode_enable(cad_mode_e mode);
void slc_cad_duty_cycle_enable(void);
void slc_cad_duty_cycle_disable(void);
void slc_cad_cfg(uint16_t check_symbol_num, uint16_t duty_cycle_period, pre_cad_cfg_t *precad_cfg, uint8_t no_demod_data);

#endif
