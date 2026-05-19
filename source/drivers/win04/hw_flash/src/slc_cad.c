/**
 * ****************************************************************************
 * @file slc_cad.c
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
#include "slc_hal_delay.h"
#include "slc_cad.h"

void slc_cad_normal_enable(void)
{
    PHY_CTRL->CAD1 &= ~PHY_CAD_MODEL_EN_MASK;
    slc_hal_nop_delay_us(1);
    PHY_CTRL->CAD1 |= PHY_CAD_MODEL_EN_VAL(1);
    slc_hal_nop_delay_us(1);
    PHY_CTRL->CAD1 &= ~PHY_CAD_MODEL_EN_MASK;
}

void slc_cad_duty_cycle_enable(void)
{
    PHY_CTRL->CAD1 &= ~PHY_CAD_DUTY_EN_MASK;
    slc_hal_nop_delay_us(1);
    PHY_CTRL->CAD1 |= PHY_CAD_DUTY_EN_VAL(1);
    slc_hal_nop_delay_us(1);
    PHY_CTRL->CAD1 &= ~PHY_CAD_DUTY_EN_MASK;
}

void slc_cad_duty_cycle_disable(void)
{
    PHY_CTRL->CAD1 &= ~PHY_CAD_EXIT_DUTY_EN_MASK;
    slc_hal_nop_delay_us(1);
    PHY_CTRL->CAD1 |= PHY_CAD_EXIT_DUTY_EN_VAL(1);
    slc_hal_nop_delay_us(1);
    PHY_CTRL->CAD1 &= ~PHY_CAD_EXIT_DUTY_EN_MASK;
}

void slc_cad_mode_enable(cad_mode_e mode)
{
    if (mode == CAD_NORMAL_MODE) {
        slc_cad_normal_enable();
    } else {
        slc_cad_duty_cycle_enable();
    }
}

void slc_cad_cfg(uint16_t check_symbol_num, uint16_t duty_cycle_period, pre_cad_cfg_t *precad_cfg)
{
    uint32_t val = 0;
    uint8_t precad_en = 0;

    val = PHY_CTRL->CAD2;
    val &= ~PHY_CAD_CHECK_SYM_NUM_MASK;
    val |= PHY_CAD_CHECK_SYM_NUM_VAL(check_symbol_num);
    PHY_CTRL->CAD2 = val;

    val = PHY_CTRL->CAD1;
    val &= ~PHY_CAD_DUTY_CYCLE_MASK;
    val |= PHY_CAD_DUTY_CYCLE_VAL(duty_cycle_period);
    PHY_CTRL->CAD1 = val;

    if (precad_cfg == NULL) {
        precad_en = 0;
        goto set_mode;
    }

    if (precad_cfg->matchaddr_wide > CAD_MATCHADDR_WIDE_MAX ||
        precad_cfg->cycle_num > CAD_CYCLE_NUM_MAX || precad_cfg->pre_num < CAD_CYCLE_NUM_MIN) {
        return;
    }

    precad_en = 1;

    val = PHY_CTRL->FIND_WIN;
    val &= ~(PHY_PROTECT_NUM_MASK);
    val |= PHY_PROTECT_NUM_VAL(20000);
    PHY_CTRL->FIND_WIN = val;

    val = PHY_CTRL->CAD2;
    val &= ~(PHY_CAD_PRE_NUM_MASK | PHY_CAD_PREADDR_MASK);
    val |= (PHY_CAD_PRE_NUM_VAL(precad_cfg->pre_num) |
           PHY_CAD_PREADDR_VAL(precad_cfg->pre_addr));
    PHY_CTRL->CAD2 = val;

    val = PHY_CTRL->CAD3;
    val &= ~(PHY_CAD_MARKER_MASK | PHY_MATCHADDR_WIDE_MASK | PHY_CAD_CYCLE_NUM_MASK);
    val |= (PHY_CAD_MARKER_VAL(precad_cfg->marker) |
           PHY_MATCHADDR_WIDE_VAL(precad_cfg->matchaddr_wide) |
           PHY_CAD_CYCLE_NUM_VAL(precad_cfg->cycle_num));
    PHY_CTRL->CAD3 = val;

    val = PHY_CTRL->CAD1;
    val &= ~PHY_PRECAD_NOSLEEP_MASK;
    val |= PHY_PRECAD_NOSLEEP_VAL(precad_cfg->nosleep_cycle);
    PHY_CTRL->CAD1 = val;

set_mode:
    PHY_CTRL->CAD1 &= ~PHY_PRECAD_EN_MASK;
    PHY_CTRL->CAD1 |= PHY_PRECAD_EN_VAL(precad_en);

    slc_set_phy_mode(PHY_CAD_MODE);
    slc_phy_sw_reset();
}
