/**
 * ****************************************************************************
 * @file slc_rf.h
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

#ifndef __SLC_RF_H__
#define __SLC_RF_H__

#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#include "reg_Dev.h"
#include "error_def.h"
#include "utility.h"
#include "slc_phy.h"

typedef enum {
    RF_DFE_TX = 0,
    RF_DFE_RX,
    RF_DFE_CALI,
    RF_DFE_GPADC
} slc_rf_dfe_mode_e;

typedef enum {
    RF_DFE_REG_CTRL = 0,
    RF_DFE_AUTO_CTRL,
} slc_rf_dfe_ctrl_e;

typedef enum {
    RF_CLK_MO = 0,
    RF_XTAL_25M_REF_CLK,
    RF_DCDC_1M_TUNE_CLK,
    RF_RC32K_TUNE_CLK,
    RF_RC50M_TUNE_CLK,
    RF_RC_CALIB_CLK,
    RF_RF_FSM_CLK,
    RF_RX_AGC_CLK,
    RF_DCOC_LUT_CLK,

    RF_CLK_ME = 16,

    RF_CLK_MAX,
} slc_rf_ctrl_clk_e;

typedef enum
{
    RF_BW_500K = 0,
    RF_BW_250K,
    RF_BW_125K,
    RF_BW_62P5K,
    RF_BW_31P25K,
    RF_BW_15P625K,
    RF_BW_7P8125K,

    RF_BW_INVALID,
} slc_rf_bw_e;

typedef enum
{
    RF_FLO_230M = 0,
    RF_FLO_490M = 1,
    RF_FLO_1080M = 2,

    RF_FLO_INVALID,
} slc_rf_flo_e;

typedef enum
{
    RF_PA_GAIN_N20DB = 0,
    RF_PA_GAIN_N15DB,
    RF_PA_GAIN_N10DB,
    RF_PA_GAIN_N05DB,
    RF_PA_GAIN_P00DB,
    RF_PA_GAIN_P02DB,
    RF_PA_GAIN_P04DB,
    RF_PA_GAIN_P06DB,
    RF_PA_GAIN_P08DB,
    RF_PA_GAIN_P10DB,
    RF_PA_GAIN_P12DB,
    RF_PA_GAIN_P14DB,
    RF_PA_GAIN_P16DB,
    RF_PA_GAIN_P18DB,
    RF_PA_GAIN_P20DB,
} rf_power_e;

typedef enum
{
    RF_MO = 0,
    RF_ME = 1,
} slc_rf_me_e;

typedef enum
{
    RF_TXDAC_GAIN_9P16 = 0, // 9.16uA
    RF_TXDAC_GAIN_11,       // 11uA
    RF_TXDAC_GAIN_13P75,    // 13.75uA
    RF_TXDAC_GAIN_18P33,    // 18.33uA
} slc_rf_txdac_gain_e;

typedef enum
{
    RF_TXLPF_112P5K = 0,    // BW: 125K
    RF_TXLPF_225K,          // BW: 250K
    RF_TXLPF_450K,          // BW: 500K
    RF_TXLPF_900K,          // IQ CALI
    RF_TXLPF_1800K,         // CIM3
} slc_rf_txlpf_bw_e;

typedef enum
{
    RF_RXABB_IF_0K = 0,
    RF_RXABB_IF_RESV,
    RF_RXABB_IF_250K,
    RF_RXABB_IF_500K,
} slc_rf_rxabb_if_e;

typedef enum
{
    RF_RXABB_125K = 0,      // BW: 125K
    RF_RXABB_250K,          // BW: 250K
    RF_RXABB_500K,          // BW: 500K
} slc_rf_rxabb_bw_e;

typedef enum
{
    RF_IF250K = 0,
    RF_IF500K,
} slc_rf_tia_if_e;

typedef enum
{
    RF_TXLPF_9DB = 0,   // -9dB
    RF_TXLPF_6DB,       // -6dB
    RF_TXLPF_3DB,       // -3dB
    RF_TXLPF_0DB,       // 0dB
} slc_rf_txlpf_gain_e;

typedef enum
{
    RF_TXLPF_VCM_700 = 0,   // 700mV
    RF_TXLPF_VCM_675,       // 675mV
    RF_TXLPF_VCM_650,       // 650mV
    RF_TXLPF_VCM_RESERVE,   // Reserve
    RF_TXLPF_VCM_625,       // 625mV [default]
    RF_TXLPF_VCM_600,       // 600mV
    RF_TXLPF_VCM_575,       // 575mV
    RF_TXLPF_VCM_550,       // 550mV
} slc_rf_txlpf_vcm_e;

typedef enum
{
    RF_TXLPF_BIAS_35 = 0,   // 35uA
    RF_TXLPF_BIAS_30,       // 30uA
    RF_TXLPF_BIAS_25,       // 25uA
    RF_TXLPF_BIAS_20,       // 20uA
    RF_TXLPF_BIAS_15,       // 15uA
    RF_TXLPF_BIAS_10,       // 10uA [default]
    RF_TXLPF_BIAS_5,        // 5uA
    RF_TXLPF_BIAS_0,        // 0uA
} slc_rf_txlpf_bias_e;

typedef enum {
    RF_LVD_1P8 = 0,
    RF_LVD_1P9,
    RF_LVD_2P0,
    RF_LVD_2P1,
    RF_LVD_2P2,
    RF_LVD_2P3,
    RF_LVD_2P4,
    RF_LVD_2P5,
    RF_LVD_2P6,
    RF_LVD_2P7,
    RF_LVD_2P8,
    RF_LVD_2P9,
    RF_LVD_3P0,
    RF_LVD_3P1,
    RF_LVD_3P2,
    RF_LVD_3P3,

    RF_LVD_MAX,
} slc_rf_lvd_crvt_e;

typedef enum {
    PPA_LDO_1P232 = 0,
    PPA_LDO_1P188,
    PPA_LDO_1P144,
    PPA_LDO_1P100,
    PPA_LDO_1P056,
    PPA_LDO_1P012,
    PPA_LDO_0P968,
    PPA_LDO_0P924,
} slc_rf_ppa_ldo_e;

typedef struct
{
    slc_rf_txdac_gain_e txdac_gain; // refer to slc_rf_txdac_gain_e
    slc_rf_txlpf_gain_e txlpf_gain; // refer to slc_rf_txlpf_gain_e
    slc_rf_txlpf_bw_e txlpf_bw;     // refer to slc_rf_txlpf_bw_e
    slc_rf_txlpf_vcm_e txlpf_vcm;   // refer to slc_rf_txlpf_vcm_e
    slc_rf_txlpf_bias_e txlpf_bias_op1; // refer to slc_rf_txlpf_bias_e
    slc_rf_txlpf_bias_e txlpf_bias_op2; // refer to slc_rf_txlpf_bias_e
    slc_rf_txlpf_bias_e txlpf_bias_op3; // refer to slc_rf_txlpf_bias_e
    slc_rf_txlpf_bias_e txlpf_bias_op4; // refer to slc_rf_txlpf_bias_e
    uint8_t txdac_clk_phase;        // 0:输入CLK取反，内部采用下降沿取输入code;
                                    // 1:输入CLK不取反，内部采用上升沿取输入code;
} slc_rf_tx_dac_lpf_ctrl_t;

typedef struct
{
    uint8_t en_polar_me;    // 0: 自动; 1: 手动
    uint8_t band_sel;       // 0: 自动; 1: 手动
    uint8_t mode_sel;       // 0: 自动; 1: 手动
    uint8_t en_polar_mo;    // 0: TX上变频模式，1: TX POLAR模式
    uint8_t band_mult;      // 00:*2, 01:*4, 10:*8, 11:*16
    uint8_t div_band_sel;   /* 00:/4,01:/8,10:/16,11:/2, TX(POLAR)
                               00:/2,01:/4,10:/8,11:/16, TX(MIXER)
                               00:/2, 01:/4, 10:/8, 11: 1 (TX 輸出會再除2) */
    uint8_t mode;           // 0: disable; 1: RX; 2: TX
} slc_rf_pll_lo_ctrl_t;

#define RF_IRQ_LVD_SHIFT            (0)
#define RF_IRQ_LVD_MASK             (1U << RF_IRQ_LVD_SHIFT)
#define RF_IRQ_DCDC_1M_SHIFT        (4)
#define RF_IRQ_DCDC_1M_MASK         (1U << RF_IRQ_DCDC_1M_SHIFT)
#define RF_IRQ_RC32K_SHIFT          (5)
#define RF_IRQ_RC32K_MASK           (1U << RF_IRQ_RC32K_SHIFT)
#define RF_IRQ_RC50M_SHIFT          (6)
#define RF_IRQ_RC50M_MASK           (1U << RF_IRQ_RC50M_SHIFT)
#define RF_IRQ_RXABB_RCCAL_SHIFT    (7)
#define RF_IRQ_RXABB_RCCAL_MASK     (1U << RF_IRQ_RXABB_RCCAL_SHIFT)
#define RF_IRQ_PMU_RCAL_SHIFT       (8)
#define RF_IRQ_PMU_RCAL_MASK        (1U << RF_IRQ_PMU_RCAL_SHIFT)
#define RF_IRQ_AGC_MAX_ENTRY_SHIFT  (9)
#define RF_IRQ_AGC_MAX_ENTRY_MASK   (1U << RF_IRQ_AGC_MAX_ENTRY_SHIFT)
#define RF_IRQ_AGC_MAX_EXIT_SHIFT   (10)
#define RF_IRQ_AGC_MAX_EXIT_MASK    (1U << RF_IRQ_AGC_MAX_EXIT_SHIFT)
#define RF_IRQ_RFPLL_AFC_SHIFT      (11)
#define RF_IRQ_RFPLL_AFC_MASK       (1U << RF_IRQ_RFPLL_AFC_SHIFT)
#define RF_IRQ_RFPLL_DAC_SHIFT      (12)
#define RF_IRQ_RFPLL_DAC_MASK       (1U << RF_IRQ_RFPLL_DAC_SHIFT)

#define RF_IRQ_ALL_MASK             (RF_IRQ_LVD_MASK | RF_IRQ_DCDC_1M_MASK | RF_IRQ_RC32K_MASK | RF_IRQ_RC50M_MASK | \
                                     RF_IRQ_RXABB_RCCAL_MASK | RF_IRQ_PMU_RCAL_MASK | RF_IRQ_AGC_MAX_ENTRY_MASK | \
                                     RF_IRQ_AGC_MAX_EXIT_MASK | RF_IRQ_RFPLL_AFC_MASK | RF_IRQ_RFPLL_DAC_MASK)

#define RF_TXDAC_LPF_MO_SEL(val)    (RFCTRL_PD_I_TXDAC_MO_VAL(val) | RFCTRL_PD_Q_TXDAC_MO_VAL(val) | \
                                     RFCTRL_PD_TXDAC_IBIAS_I_MO_VAL(val) | RFCTRL_PD_TXDAC_IBIAS_Q_MO_VAL(val) | \
                                     RFCTRL_PD_TXLPF_MO_VAL(val) | RFCTRL_PD_TXLPF_IBIAS_MO_VAL(val) | \
                                     RFCTRL_PD_TXLPF_TIA_MO_VAL(val))

#define RF_TXDAC_LPF_ME_SEL(val)    (RFCTRL_PD_I_TXDAC_ME_VAL(val) | RFCTRL_PD_Q_TXDAC_ME_VAL(val) | \
                                     RFCTRL_PD_TXDAC_IBIAS_I_ME_VAL(val) | RFCTRL_PD_TXDAC_IBIAS_Q_ME_VAL(val) | \
                                     RFCTRL_PD_TXLPF_ME_VAL(val) | RFCTRL_PD_TXLPF_IBIAS_ME_VAL(val) | \
                                     RFCTRL_PD_TXLPF_TIA_ME_VAL(val))

void slc_rf_dfe_tx_del_keep_pnt_bypass(bool enable);
void slc_rf_dfe_set_trx_mode(slc_rf_dfe_mode_e mode);
void slc_rf_dfe_set_link_ctrl_mode(slc_rf_dfe_ctrl_e mode);
void slc_rf_dfe_set_ddc_fcw(uint32_t freq, uint32_t fs, bool ddc_bypass);
void slc_rf_dfe_set_rmdc_bypass_en(bool dc0_bypass, bool dc1_bypass);
void slc_rf_dfe_set_bw(slc_rf_bw_e bw);
void slc_rf_dfe_set_line_ctrl_bw(slc_rf_dfe_ctrl_e mode);
void slc_rf_dfe_bypass_fir59(bool bypass, uint8_t pll_gen_flag, slc_rf_dfe_ctrl_e line_ctrl_pll_gen_flag);
void slc_rf_dfe_set_fir59(phy_cfg_sf_e sf);
void slc_rf_dfe_sw_reset(void);

void slc_rf_enable_xtal32k(bool enable);
void slc_rf_tcxo_bypass(bool bypass);
void slc_rf_set_pmu_lvd_crvt(slc_rf_lvd_crvt_e lvd_val);
void slc_rf_set_pmu_lvd_enable(bool enable);

void slc_rf_ctrl_clk_gate_enable(slc_rf_ctrl_clk_e clk_gate, bool enable);
void slc_txrf_set_pa_ppa_upc_ctrl(slc_rf_flo_e flo);
void slc_txrf_set_power_ctrl(slc_rf_flo_e flo, rf_power_e power);
void slc_tx_dac_lpf_enable(slc_rf_me_e me_sel);
void slc_tx_dac_lpf_ctrl(slc_rf_tx_dac_lpf_ctrl_t *ctrl);
void slc_rf_tx_ctrl(phy_cfg_tx_mode_e mode, slc_rf_flo_e flo);
void slc_rf_tx_disable(void);
slc_rf_txlpf_bw_e slc_rf_dfe_get_txlpf_bw(phy_cfg_bw_e bw);
slc_rf_rxabb_bw_e slc_rf_get_rxabb_bwmode_bw(phy_cfg_bw_e bw);
slc_rf_rxabb_if_e slc_rf_get_rxabb_ifmode_bw(phy_cfg_bw_e bw);
void slc_rf_rx_tia_ctrl(slc_rf_tia_if_e tia_if);
void slc_rf_rx_cbpf_ctrl(phy_cfg_bw_e bw);
void slc_rf_rx_adc_ctrl(void);

void slc_rf_rx_agc_ctrl(void);

void slc_rf_pll_set_lo_ctrl(slc_rf_pll_lo_ctrl_t *ctrl);

void slc_rf_irq_enable(uint32_t irq_mask);
void slc_rf_irq_disable(uint32_t irq_mask);
uint32_t slc_rf_irq_get_sta(void);
void slc_rf_irq_clr_sta(uint32_t irq_sta);

#endif
