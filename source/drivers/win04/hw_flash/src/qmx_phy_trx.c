/**
 * ****************************************************************************
 * @file qmx_phy_trx.c
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

#include <string.h>
#include <math.h>
#include "qmx_cali.h"
#include "qmx_phy_trx.h"
#include "qmx_hal_delay.h"
#include "qmx_hal_gpio.h"
#include "debug.h"
#include "compiler.h"

phy_trx_cfg_param_t g_phy_trx_config_param = {0};

phy_trx_cfg_param_t *qmx_phy_get_trx_config_param(void)
{
    return &g_phy_trx_config_param;
}

void qmx_phy_start_trans_tx_data(phy_cfg_tx_mode_e tx_mode, rf_power_e power)
{
    RF_PLL->PK_CTRL &= ~RFPLL_PEAKDET_CON_VREF_1P1_MASK;
    RF_PLL->PK_CTRL |= RFPLL_PEAKDET_CON_VREF_1P1_VAL(QMX_PEAKDET_VREF_1P1_TX_CODE);
    qmx_phy_set_trx(PHY_TX_EN);

#ifndef QMX_FPGA
    /* DFE config */
    qmx_rf_ctrl_clk_gate_enable(RF_CLK_ME, true);
    qmx_rf_dfe_set_link_ctrl_mode(RF_DFE_REG_CTRL);
    qmx_rf_dfe_set_line_ctrl_bw(RF_DFE_AUTO_CTRL);
    // qmx_rf_dfe_set_bw(g_phy_trx_config_param.bw);
    if (tx_mode == PHY_TX_IQ) {
        qmx_rf_dfe_bypass_fir59(true, 0, RF_DFE_AUTO_CTRL);
    } else {
        qmx_rf_dfe_bypass_fir59(false, 1, RF_DFE_AUTO_CTRL);
        qmx_rf_dfe_set_fir59(g_phy_trx_config_param.sf);
    }
#endif
    qmx_rf_dfe_set_trx_mode(RF_DFE_TX);

    qmx_rf_dfe_reset();

#ifdef QMX_FPGA
    qmx_rf_subg_disable_trx();
    qmx_rf_subg_set_tx_en();
#else
    qmx_txrf_set_pa_ppa_upc_ctrl(g_phy_trx_config_param.flo);
    qmx_tx_dac_lpf_enable(RF_ME);

    qmx_rf_tx_dac_lpf_ctrl_t dac_lpf_ctrl;
    dac_lpf_ctrl.txdac_gain = RF_TXDAC_GAIN_11;
    dac_lpf_ctrl.txdac_clk_phase = 0;
    dac_lpf_ctrl.txlpf_gain = RF_TXLPF_6DB;
    dac_lpf_ctrl.txlpf_bw = qmx_rf_dfe_get_txlpf_bw(g_phy_trx_config_param.bw);
    dac_lpf_ctrl.txlpf_vcm = RF_TXLPF_VCM_625;
    dac_lpf_ctrl.txlpf_bias_op1 = RF_TXLPF_BIAS_10;
    dac_lpf_ctrl.txlpf_bias_op2 = RF_TXLPF_BIAS_10;
    dac_lpf_ctrl.txlpf_bias_op3 = RF_TXLPF_BIAS_10;
    dac_lpf_ctrl.txlpf_bias_op4 = RF_TXLPF_BIAS_10;
    qmx_tx_dac_lpf_ctrl(&dac_lpf_ctrl);

    // qmx_rf_pll_lo_ctrl_t lo_ctrl;
    // lo_ctrl.en_polar_me = 1;
    // lo_ctrl.band_sel = 1;
    // lo_ctrl.mode_sel = 0;
    // lo_ctrl.band_mult = 0;
    // lo_ctrl.en_polar_mo = tx_mode;
    // lo_ctrl.div_band_sel = 0x2; // 8分频
    // lo_ctrl.mode = 2; // TX
    // qmx_rf_pll_set_lo_ctrl(&lo_ctrl);

    qmx_rf_tx_ctrl(tx_mode, g_phy_trx_config_param.flo);
    qmx_txrf_set_power_ctrl(g_phy_trx_config_param.flo, power);
#endif
}

void qmx_phy_start_get_rx_data(void)
{
    qmx_rf_tx_disable();

    RF_PLL->PK_CTRL &= ~RFPLL_PEAKDET_CON_VREF_1P1_MASK;
    RF_PLL->PK_CTRL |= RFPLL_PEAKDET_CON_VREF_1P1_VAL(QMX_PEAKDET_VREF_1P1_RX_CODE);

#ifdef QMX_FPGA
    // 中频1M，默认采样率5M
    qmx_rf_dfe_set_ddc_fcw(1000000, 5000000, false);
    qmx_rf_dfe_set_trx_mode(RF_DFE_RX);
    qmx_rf_dfe_set_link_ctrl_mode(RF_DFE_REG_CTRL);
    qmx_rf_dfe_set_rmdc_bypass_en(true, true);
    qmx_rf_dfe_reset();

    qmx_phy_set_agc_bypass_en(false);
#else
    qmx_rf_ctrl_clk_gate_enable(RF_CLK_ME, true);
    qmx_rf_dfe_set_trx_mode(RF_DFE_RX);
    qmx_rf_dfe_set_link_ctrl_mode(RF_DFE_REG_CTRL);
    qmx_rf_dfe_set_rmdc_bypass_en(true, true);
    qmx_rf_dfe_reset();
    qmx_rf_dfe_set_ddc_fcw((g_phy_trx_config_param.bw == PHY_BW_500K) ? 500000 : 250000, 5000000, false);
    qmx_rf_rx_tia_ctrl((g_phy_trx_config_param.bw == PHY_BW_500K) ? RF_IF500K : RF_IF250K);
    qmx_rf_rx_cbpf_ctrl(g_phy_trx_config_param.bw);

    qmx_rf_rx_adc_ctrl();

    // qmx_rf_pll_lo_ctrl_t lo_ctrl;
    // lo_ctrl.en_polar_me = 1;
    // lo_ctrl.band_sel = 1;
    // lo_ctrl.mode_sel = 0;
    // lo_ctrl.band_mult = 0;
    // lo_ctrl.en_polar_mo = 0;
    // lo_ctrl.div_band_sel = 0x0; // 4分频
    // lo_ctrl.mode = 1; // TX
    // qmx_rf_pll_set_lo_ctrl(&lo_ctrl);

    qmx_rf_rx_agc_ctrl();
#endif

    qmx_phy_set_trx(PHY_RX_EN);
#ifdef QMX_FPGA
    qmx_rf_subg_set_sync_opt_agc_opt();
    qmx_rf_subg_disable_trx();
    qmx_rf_subg_set_rx_en();
#endif
}

void qmx_phy_trx_config(phy_cfg_t *cfg, uint16_t netid, phy_cfg_trx_e trx,
                        qmx_rf_flo_e flo, uint32_t freq, phy_cfg_tx_mode_e tx_mode)
{
    int ret = 0;
    uint8_t afc_cali_en __MAYBE_UNUSED = 0;
    uint8_t kadc_cali_en __MAYBE_UNUSED = 0;

    if ((netid < 3) || (netid > ((1 << cfg->sf) - 3))) {
        PRINTF("netid error, must in [3 ~ 2^sf-3]! netid = %d\n", netid);
        return;
    }

    // polar模式下，频点变换或者BW在500K/250K/125K之间切换时才需要做KVCO2校准
    if (tx_mode == PHY_TX_POLAR) {
        if (((g_phy_trx_config_param.bw != cfg->bw) && (cfg->bw <= PHY_BW_125K)) ||
            (g_phy_trx_config_param.freq != freq)) {
            kadc_cali_en = 1;
        }
    }
    g_phy_trx_config_param.freq = freq;
    g_phy_trx_config_param.sf = cfg->sf;
    g_phy_trx_config_param.bw = cfg->bw;
    g_phy_trx_config_param.flo = flo;

    /* phy config */
    qmx_phy_set_timeout(0xFFFFFFFF);
    qmx_phy_set_sf_reduce_en(true);
    qmx_set_phy_mode(PHY_NORMAL_MODE);
    qmx_set_phy_tx_mode(tx_mode);

    qmx_phy_cfg_done(false);

    qmx_phy_config_common(cfg);
    qmx_phy_set_netid(netid);
    if (trx == PHY_RX_EN)
        qmx_phy_set_rx_param_default(cfg->sf);

    qmx_set_monitor_reg_clr();
    qmx_phy_cfg_fc(freq);
    qmx_rf_dfe_tx_del_keep_pnt_bypass(true);

    qmx_phy_cfg_done(true);
    qmx_phy_sw_reset();

#ifdef QMX_FPGA
    /* subg config */
    qmx_rf_subg_set_gpio();
    qmx_rf_subg_disable_trx();
    ret = qmx_rf_subg_afc_cali();
    if (ret != EN_ERROR_STA_OK) {
        PRINTF("AFC cali error! ret = %d\n", ret);
        return;
    }

    qmx_rf_subg_fc_cali(freq);
    qmx_rf_subg_set_fc(freq);
#else
    ret += qmx_afc_cali(g_phy_trx_config_param.freq, ((g_phy_trx_config_param.bw == PHY_BW_500K) ? -500000 : -250000),
                        true, ((tx_mode == PHY_TX_POLAR) ? true: false));
    ret += qmx_afc_cali(g_phy_trx_config_param.freq, ((g_phy_trx_config_param.bw == PHY_BW_500K) ? -500000 : -250000),
                        false, false);

    if (ret != EN_ERROR_STA_OK) {
        PRINTF("afc %s cali failed, ret = %d\n", (trx == PHY_TX_EN) ? "tx" : "rx", ret);
        return;
    }

    if (kadc_cali_en) {
        qmx_kdac_cali();
    }
#endif
}

void qmx_phy_single_tone(void)
{
    uint32_t sine_fcw = 0;
    int32_t fft_fcw = 0;
    qmx_phy_set_trx(PHY_TX_EN);
    PHY_CTRL->CTRL_REG3 = PHY_TRX_SEL_VAL(1);
    qmx_rf_ctrl_clk_gate_enable(RF_CLK_ME, true);
    qmx_rf_dfe_set_link_ctrl_mode(RF_DFE_REG_CTRL);
    qmx_rf_dfe_set_line_ctrl_bw(RF_DFE_AUTO_CTRL);
    qmx_rf_dfe_bypass_fir59(true, 0, RF_DFE_AUTO_CTRL);
    qmx_rf_dfe_set_trx_mode(RF_DFE_CALI);
    RF_DFE->DFE_CALIB_PARAMETER &= ~DFE_SINE_DC_SEL_MASK;  // 0: sine, 1: DC

    sine_fcw = (uint32_t)round(2.0 * PI * 100000 * 16777216.0 / 25000000.0);
    RF_DFE->DFE_CALIB_SIN_FCW = DFE_CAL_SEND_SINE_FCW_VAL(sine_fcw);

    RF_DFE->DFE_CALIB_WAIT_TIME &= ~DFE_CAL_SEND_DURATION_MASK;
    RF_DFE->DFE_CALIB_WAIT_TIME |= DFE_CAL_SEND_DURATION_VAL(0);

    qmx_rf_dfe_reset();

    RF_DFE->DFE_CALIB_START |= DFE_CAL_SEND_START_VAL(1);
}
