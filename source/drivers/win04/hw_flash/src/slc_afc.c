/**
 * ****************************************************************************
 * @file slc_afc.c
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
#include <math.h>

#include "slc_hal_delay.h"
#include "slc_afc.h"
#include "slc_cali.h"
#include "slc_phy_trx.h"
#include "debug.h"

#define PPM_THRESHOLD   10.0f

void slc_afc_set_farrow(afc_farrow_type_e farrow_type)
{
    uint32_t val = PHY_CTRL->DAGC;
    val &= ~PHY_FARROW_SEL_MASK;
    val |= PHY_FARROW_SEL_VAL(farrow_type);
    PHY_CTRL->DAGC = val;
    slc_hal_nop_delay_us(PHY_MAX_DELAY);
}

void slc_afc_tx_farrow_enable(bool enable)
{
    PHY_CTRL->DAGC &= ~PHY_TX_FARROW_EN_MASK;
    slc_hal_nop_delay_us(PHY_MAX_DELAY);

    if (enable) {
        PHY_CTRL->DAGC |= PHY_TX_FARROW_EN_VAL(1);
        slc_hal_nop_delay_us(PHY_MAX_DELAY);
    }
}

void slc_afc_cfo_enable(bool enable)
{
    if (enable)
        PHY_CTRL->DAGC |= PHY_CFO_CFG_MASK;
    else
        PHY_CTRL->DAGC &= ~PHY_CFO_CFG_MASK;
}

volatile bool g_afc_flag = 0;
void slc_afc_set_sw_flag(bool flag)
{
    g_afc_flag = flag;
}

bool slc_afc_get_sw_flag(void)
{
    return g_afc_flag;
}

void slc_afc_adjust(phy_cfg_trx_e trx_type, phy_cfg_tx_mode_e tx_mode)
{
    phy_trx_cfg_param_t *trx_cfg = slc_phy_get_trx_config_param();
    int32_t cfo_reg = (int32_t)PHY_MONITOR->RX_CTO_PHY;
    int32_t sfo_reg = (int32_t)PHY_MONITOR->RX_SFO_PHY;
    uint32_t bw = (PHY_CTRL->CTRL_REG1 & PHY_BW_MASK) >> PHY_BW_SHIFT;
    uint32_t sf = (PHY_CTRL->CTRL_REG1 & PHY_SF_MASK) >> PHY_SF_SHIFT;
    uint32_t freq_reg = PHY_CTRL->CARRIER_FRE;
    float cfo_hz = 0.0f;
    float bw_hz = 0.0f;
    float ppm = 0.0f;
    float freq_hz = 0.0f;
    uint32_t freq = 0;
    static int32_t sfo_value = 0;
    static float freq_first = 0.0f;
    float total_ppm = 0.0f;
    int ret = 0;

    if (slc_afc_get_sw_flag() == false) {
        if (trx_type == PHY_TX_EN) {
            slc_afc_set_farrow(SLC_MAC_PARAM);
            slc_afc_tx_farrow_enable(true);
        } else {
            slc_afc_cfo_enable(false);
            slc_afc_set_farrow(SLC_PHY_MAC_PARAM);
        }

        return;
    }

    freq_hz = (freq_reg * (1000000.0 / (1 << 20)));
    // 记录初始本振，用作后续SFO配置参数的计算
    if (freq_first == 0.0f)
        freq_first = freq_hz;

    slc_phy_disable_rx();

    bw_hz = slc_phy_get_bw_hz(bw);
    cfo_hz = cfo_reg * bw_hz / (1 << (sf+18));
    ppm = cfo_hz / (freq_hz / 1000000.0f);
    // PRINTF("current freq: %.2fHz, CFO: %.2fHz, ppm: %.2f, CFO_reg: 0x%X, SFO_reg: 0x%X\n",
    //         freq_hz, cfo_hz, ppm, cfo_reg, sfo_reg);

    slc_afc_cfo_enable(false);
    slc_afc_tx_farrow_enable(false);

    if (trx_type == PHY_RX_EN)
        slc_afc_set_farrow(SLC_PHY_MAC_PARAM);
    else
        slc_afc_set_farrow(SLC_MAC_PARAM);

    // 此处的ppm都是基于当前本振计算
    if (fabsf(ppm) >= PPM_THRESHOLD) {
        freq = (trx_type == PHY_RX_EN) ? (uint32_t)(freq_hz + cfo_hz) : (uint32_t)(freq_hz - cfo_hz);

        // 调整本振后需要重新进行FC校准和AFC校准
        slc_phy_cfg_fc(freq);
        slc_rf_dfe_tx_del_keep_pnt_bypass(true);

#ifdef SLC_FPGA
        /* subg config */
        slc_rf_subg_set_gpio();
        slc_rf_subg_disable_trx();
        if (slc_rf_subg_afc_cali() != EN_ERROR_STA_OK) {
            PRINTF("AFC cali error!, timeout\n");
            return;
        }

        slc_rf_subg_fc_cali(freq);
        slc_rf_subg_set_fc(freq);
#else
        trx_cfg->freq = freq;
        ret += slc_afc_cali(freq, ((trx_cfg->bw == PHY_BW_500K) ? -500000 : -250000),
                            true, (tx_mode == PHY_TX_POLAR) ? true: false);
        ret += slc_afc_cali(freq, ((trx_cfg->bw == PHY_BW_500K) ? -500000 : -250000),
                            false, false);
        if (ret != 0) {
            PRINTF("afc cali failed, ret = %d\n", ret);
            return;
        }

        ret = slc_kdac_cali();
        if (ret != 0) {
            PRINTF("kdac cali failed, ret = %d\n", ret);
            return;
        }
#endif

        // 基于初始本振计算SFO，SFO = (与初始本振相比的频偏ppm) * 2^24 / 1000000
        total_ppm = (freq_hz - freq_first + cfo_hz) / (freq_first / 1000000.0f);
        sfo_reg = (int32_t)(-round(total_ppm * 16.777216));  // 16.777216 = (1 << 24) / 1000000.0f
        PHY_CTRL->FARROWPARAM1 = sfo_reg;
        sfo_value = sfo_reg;
        PRINTF("after AFC, current_ppm: %.2f, total_ppm: %.2f, freq: %dHz, sfo_reg: 0x%X\n",
                ppm, total_ppm, freq, PHY_CTRL->FARROWPARAM1);
    } else {
        PHY_CTRL->FARROWPARAM1 = sfo_value;
        PRINTF("after AFC, current_ppm: %.2f, sfo_reg: 0x%X\n", ppm, PHY_CTRL->FARROWPARAM1);
    }

    if (trx_type == PHY_TX_EN)
        slc_afc_tx_farrow_enable(true);

    // 修改PHY参数后需要复位phy并触发内部参数计算，确保配置的值生效
    slc_phy_sw_reset();
    slc_phy_cfg_done(true);

    slc_afc_set_sw_flag(false);
}
