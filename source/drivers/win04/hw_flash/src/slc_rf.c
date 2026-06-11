/**
 * ****************************************************************************
 * @file slc_rf.c
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
#include "slc_private_spi_frame.h"

#include "slc_hal_delay.h"
#include "slc_phy.h"
#include "slc_rf.h"
#include "app_cfg.h"


/* RFFE PMU func */
void slc_rf_set_pmu_lvd_crvt(slc_rf_lvd_crvt_e lvd_val)
{
    slc_rf_spi_reg_or_mask(SLC_RF_SPI_ADDR_PMU(0x18), SLC_PMU_LVD_OUT_POL_MASK);
    slc_rf_spi_reg_update(SLC_RF_SPI_ADDR_PMU(0x18), SLC_PMU_LVD_CRVT_MASK, SLC_PMU_LVD_CRVT_VAL(lvd_val));
}

void slc_rf_set_pmu_lvd_enable(bool enable)
{
    if (enable)
        slc_rf_spi_reg_or_mask(SLC_RF_SPI_ADDR_PMU(0x0C), SLC_PD_PMU_LVD_VAL(1));
    else
        slc_rf_spi_reg_clr_mask(SLC_RF_SPI_ADDR_PMU(0x0C), SLC_PD_PMU_LVD_MASK);
}

void slc_rf_irq_enable(uint32_t irq_mask)
{
    slc_rf_spi_reg_or_mask(SLC_RF_SPI_ADDR_PMU(0x50), (irq_mask & RF_IRQ_ALL_MASK));
}

void slc_rf_irq_disable(uint32_t irq_mask)
{
    slc_rf_spi_reg_clr_mask(SLC_RF_SPI_ADDR_PMU(0x50), (irq_mask & RF_IRQ_ALL_MASK));
}

uint32_t slc_rf_irq_get_sta(void)
{
    return (slc_rf_spi_read32_cmd(SLC_RF_SPI_ADDR_PMU(0x54)) & RF_IRQ_ALL_MASK);
}

void slc_rf_irq_clr_sta(uint32_t irq_sta)
{
    slc_rf_spi_reg_or_mask(SLC_RF_SPI_ADDR_PMU(0x58), (irq_sta & RF_IRQ_ALL_MASK));
}

void slc_rf_enable_xtal32k(bool enable)
{
    if (enable)
        slc_rf_spi_reg_clr_mask(SLC_RF_SPI_ADDR_PMU(0x0C), RF_PMU_PD_XTAL32K_MASK);
    else
        slc_rf_spi_reg_or_mask(SLC_RF_SPI_ADDR_PMU(0x0C), RF_PMU_PD_XTAL32K_VAL(1));
}

void slc_rf_tcxo_bypass(bool bypass)
{
    if (bypass)
        slc_rf_spi_reg_or_mask(SLC_RF_SPI_ADDR_PMU(0x3C), RF_CMU_TCXO_BYPASS_VAL(1));
    else
        slc_rf_spi_reg_clr_mask(SLC_RF_SPI_ADDR_PMU(0x3C), RF_CMU_TCXO_BYPASS_MASK);
}

/* RFFE CTRL func */
void slc_rf_ctrl_clk_gate_enable(slc_rf_ctrl_clk_e clk_gate, bool enable)
{
    if (enable)
        slc_rf_spi_reg_or_mask(SLC_RF_SPI_ADDR_CTRL(0x60), BIT(clk_gate));
    else
        slc_rf_spi_reg_clr_mask(SLC_RF_SPI_ADDR_CTRL(0x60), BIT(clk_gate));
}

void slc_txrf_set_power_ctrl(slc_rf_flo_e flo, rf_power_e power)
{
    rf_power_e power_tmp = power;
    if((power < RF_PA_GAIN_N20DB) || (power > RF_PA_GAIN_P20DB)) {
        power_tmp = RF_PA_GAIN_P16DB;
    }
    const uint16_t flo_230M_val[15][4] = {
        {0x0DB, 0x00, 0xDD, 0x01},  // Pout=-20dBm
        {0x06B, 0x00, 0xDD, 0x01},  // Pout=-15dBm
        {0x0AB, 0x01, 0xDD, 0x01},  // Pout=-10dBm
        {0x16B, 0x01, 0xDD, 0x01},  // Pout=-5dBm
        {0x16B, 0x03, 0xDD, 0x01},  // Pout=0dBm
        {0x1FB, 0x03, 0xDD, 0x01},  // Pout=2dBm
        {0x30B, 0x03, 0xDD, 0x01},  // Pout=4dBm
        {0x48B, 0x03, 0xDD, 0x01},  // Pout=6dBm
        {0x48B, 0x06, 0xDD, 0x01},  // Pout=8dBm
        {0x4BB, 0x07, 0xDD, 0x01},  // Pout=10dBm
        {0x243, 0x1F, 0x76, 0x06},  // Pout=12dBm
        {0x3D3, 0x1F, 0x76, 0x06},  // Pout=14dBm
        {0x5D3, 0x1F, 0x76, 0x06},  // Pout=16dBm
        {0x853, 0x1F, 0x76, 0x06},  // Pout=18dBm
        {0xC83, 0x1F, 0x76, 0x06},  // Pout=20dBm
    };

    const uint16_t flo_490M_val[15][4] = {
        {0x01BB, 0x0000, 0x00DD, 0x0001},  // 0: Pout=-20dBm
        {0x04FB, 0x0000, 0x00DD, 0x0001},  // 1: Pout=-15dBm
        {0x007B, 0x0001, 0x00DD, 0x0001},  // 2: Pout=-10dBm
        {0x011B, 0x0002, 0x00DD, 0x0001},  // 3: Pout=-5dBm
        {0x013B, 0x0003, 0x00DD, 0x0001},  // 4: Pout=0dBm
        {0x036B, 0x0003, 0x00DD, 0x0001},  // 5: Pout=2dBm
        {0x05AB, 0x0003, 0x00DD, 0x0001},  // 6: Pout=4dBm
        {0x093B, 0x0003, 0x00DD, 0x0001},  // 7: Pout=6dBm
        {0x05FB, 0x0006, 0x00DD, 0x0001},  // 8: Pout=8dBm
        {0x06EB, 0x0007, 0x00DD, 0x0001},  // 9: Pout=10dBm
        {0x0443, 0x001F, 0x0076, 0x0006},  // 10: Pout=12dBm
        {0x0533, 0x001F, 0x0076, 0x0006},  // 11: Pout=14dBm
        {0x06C3, 0x001F, 0x0076, 0x0006},  // 12: Pout=16dBm
        {0x0903, 0x001F, 0x0076, 0x0006},  // 13: Pout=18dBm
        {0x0C83, 0x001F, 0x0076, 0x0006},  // 14: Pout=20dBm
    };

    const uint16_t flo_1080M_val[15][4] = {
        {0x10B, 0x00, 0xDD, 0x01},  // Pout=-20dBm
        {0x06B, 0x00, 0xDD, 0x01},  // Pout=-15dBm
        {0x0DB, 0x01, 0xDD, 0x01},  // Pout=-10dBm
        {0x12B, 0x03, 0xDD, 0x01},  // Pout=-5dBm
        {0x14B, 0x05, 0xDD, 0x01},  // Pout=0dBm
        {0x1FB, 0x05, 0xDD, 0x01},  // Pout=2dBm
        {0x32B, 0x05, 0xDD, 0x01},  // Pout=4dBm
        {0x4EB, 0x06, 0xDD, 0x01},  // Pout=6dBm
        {0x4BB, 0x07, 0xDD, 0x01},  // Pout=8dBm
        {0x4BB, 0x08, 0xDD, 0x01},  // Pout=10dBm
        {0x1E3, 0x1F, 0x76, 0x06},  // Pout=12dBm
        {0x363, 0x1F, 0x76, 0x06},  // Pout=14dBm
        {0x553, 0x1F, 0x76, 0x06},  // Pout=16dBm
        {0x7C3, 0x1F, 0x76, 0x06},  // Pout=18dBm
        {0xBE3, 0x1F, 0x76, 0x06},  // Pout=20dBm
    };

    const uint16_t (*flo_table_ptr[3])[15][4] = {
        &flo_230M_val,
        &flo_490M_val,
        &flo_1080M_val
    };

    slc_rf_spi_reg_update(SLC_RF_SPI_ADDR_CTRL(0x78), SLC_RFCTRL_POWER_CNTL_LDO_MASK, SLC_RFCTRL_POWER_CNTL_LDO_VAL((*flo_table_ptr[flo])[power_tmp][0]));

    slc_rf_spi_reg_update(SLC_RF_SPI_ADDR_CTRL(0x80),
                          (SLC_RFCTRL_POWER_CNTL_PPA_H_MASK | SLC_RFCTRL_POWER_CNTL_PPA_L_MASK),
                          (SLC_RFCTRL_POWER_CNTL_PPA_H_VAL((*flo_table_ptr[flo])[power_tmp][1]) |
                           SLC_RFCTRL_POWER_CNTL_PPA_L_VAL((*flo_table_ptr[flo])[power_tmp][2])));

    slc_rf_spi_reg_update(SLC_RF_SPI_ADDR_PMU(0x1C), SLC_RFCTRL_POWER_CNTL_RESERVE_MASK, SLC_RFCTRL_POWER_CNTL_RESERVE_VAL((*flo_table_ptr[flo])[power_tmp][3]));

}

void slc_txrf_set_pa_ppa_upc_ctrl(slc_rf_flo_e flo)
{
    uint32_t val = slc_rf_spi_read32_cmd(SLC_RF_SPI_ADDR_CTRL(0x7C));
    uint32_t tx_ups_hrm_ii_trim, tx_ups_hrm_i_trim;

    switch (flo) {
        case RF_FLO_230M:
            tx_ups_hrm_ii_trim = 5;
            tx_ups_hrm_i_trim = 3;
            break;
        case RF_FLO_490M:
            tx_ups_hrm_ii_trim = 7;
            tx_ups_hrm_i_trim = 5;
            break;
        case RF_FLO_1080M:
            tx_ups_hrm_ii_trim = 1;
            tx_ups_hrm_i_trim = 1;
            break;
        default:
            tx_ups_hrm_ii_trim = 7;
            tx_ups_hrm_i_trim = 5;
            break;
    }

    slc_rf_spi_write32_cmd(SLC_RF_SPI_ADDR_CTRL(0x78), ( SLC_RFCTRL_PPA_CNTL_LDO_VAL(PPA_LDO_1P100) |
                                SLC_RFCTRL_PA_CNTL_LDO_MO_VAL(0xA4) |
                                SLC_RFCTRL_PA_CNTL_LDO_ME_VAL(1) ));

    val &= ~(SLC_RFCTRL_TX_UPC_HRM_II_TRIM_MASK | SLC_RFCTRL_TX_UPC_HRM_I_TRIM_MASK);
    val |= ( SLC_RFCTRL_TX_UPC_HRM_II_TRIM_VAL(tx_ups_hrm_ii_trim) |
             SLC_RFCTRL_TX_UPC_HRM_I_TRIM_VAL(tx_ups_hrm_i_trim) );
    slc_rf_spi_write32_cmd(SLC_RF_SPI_ADDR_CTRL(0x7C), val);

    slc_rf_spi_reg_update(SLC_RF_SPI_ADDR_CTRL(0x80), SLC_RFCTRL_TX_PA_VB_CS_P_MASK, SLC_RFCTRL_TX_PA_VB_CS_P_VAL(0x2));
}

void slc_tx_dac_lpf_enable(slc_rf_me_e me_sel)
{
    uint32_t mo = (me_sel == RF_MO) ? 1 : 0;
    uint32_t me = (me_sel == RF_ME) ? 1 : 0;
    slc_rf_spi_write32_cmd(SLC_RF_SPI_ADDR_CTRL(0x4C), (RF_TXDAC_LPF_MO_SEL(mo) | RF_TXDAC_LPF_ME_SEL(me)));

    slc_rf_spi_reg_or_mask(SLC_RF_SPI_ADDR_CTRL(0x6C), (RFCTRL_TXDAC_RSTN_ME_VAL(1) | RFCTRL_TXDAC_RSTN_MO_VAL(1)));
}

void slc_tx_dac_lpf_ctrl(slc_rf_tx_dac_lpf_ctrl_t *ctrl)
{
    uint32_t val = slc_rf_spi_read32_cmd(SLC_RF_SPI_ADDR_CTRL(0x6C));
    val &= ~(RFCTRL_TXDAC_GAIN_MASK | RFCTRL_TXDAC_CLK_PHASE_MASK);
    val |= (RFCTRL_TXDAC_GAIN_VAL(ctrl->txdac_gain) | RFCTRL_TXDAC_CLK_PHASE_VAL(ctrl->txdac_clk_phase));
    slc_rf_spi_write32_cmd(SLC_RF_SPI_ADDR_CTRL(0x6C), val);

    val = slc_rf_spi_read32_cmd(SLC_RF_SPI_ADDR_CTRL(0x70));
    val &= ~(RFCTRL_TXLPF_BWMODE_MASK | RFCTRL_TXLPF_GAIN_MASK);
    val |= (RFCTRL_TXLPF_BWMODE_VAL(ctrl->txlpf_bw) | RFCTRL_TXLPF_GAIN_VAL(ctrl->txlpf_gain));
    slc_rf_spi_write32_cmd(SLC_RF_SPI_ADDR_CTRL(0x70), val);

    val = slc_rf_spi_read32_cmd(SLC_RF_SPI_ADDR_CTRL(0x74));
    val &= ~(RFCTRL_TXLPF_BIAS_TRIM_OP1_MASK | RFCTRL_TXLPF_BIAS_TRIM_OP2_MASK |
             RFCTRL_TXLPF_BIAS_TRIM_OP3_MASK | RFCTRL_TXLPF_BIAS_TRIM_OP4_MASK |
             RFCTRL_TXLPF_VCM_TRIM_MASK);
    val |= (RFCTRL_TXLPF_BIAS_TRIM_OP1_VAL(ctrl->txlpf_bias_op1) | RFCTRL_TXLPF_BIAS_TRIM_OP2_VAL(ctrl->txlpf_bias_op2) |
            RFCTRL_TXLPF_BIAS_TRIM_OP3_VAL(ctrl->txlpf_bias_op3) | RFCTRL_TXLPF_BIAS_TRIM_OP4_VAL(ctrl->txlpf_bias_op4) |
            RFCTRL_TXLPF_VCM_TRIM_VAL(ctrl->txlpf_vcm));
    slc_rf_spi_write32_cmd(SLC_RF_SPI_ADDR_CTRL(0x74), val);
}

#define POLAR_TX_RESERVE_LV_VAL 0xED
#define POLAR_TX_RESERVE_HV_VAL 0x80
#define PMU_TX_RESERVE_VAL      0x1

void slc_rf_tx_ctrl(phy_cfg_tx_mode_e mode, slc_rf_flo_e flo)
{
    uint8_t tx_reserve_lv_val, tx_reserve_hv_val, tx_upc_ibias_trim_val, pa_cntl_ldo_mo_val;

    slc_rf_spi_reg_clr_mask(SLC_RF_SPI_ADDR_CTRL(0x8C), (SLC_RFCTRL_TX_RESERVE_LV_MASK | SLC_RFCTRL_TX_RESERVE_HV_MASK));
    slc_rf_spi_reg_update(SLC_RF_SPI_ADDR_PMU(0x1C), SLC_PMU_RESERVE_MASK, SLC_PMU_RESERVE_VAL(PMU_TX_RESERVE_VAL));

    switch (flo)
    {
        case RF_FLO_230M:
            tx_reserve_lv_val = 0xB1;
            tx_reserve_hv_val = 0x9C;
            tx_upc_ibias_trim_val = 0x5;
            pa_cntl_ldo_mo_val = 0xC8;
            break;
        case RF_FLO_490M:
            tx_reserve_lv_val = 0xF5;
            tx_reserve_hv_val = 0xA7;
            tx_upc_ibias_trim_val = 0x3;
            pa_cntl_ldo_mo_val = 0xC8;
            break;
        case RF_FLO_1080M:
            tx_reserve_lv_val = 0xB9;
            tx_reserve_hv_val = 0xBE;
            tx_upc_ibias_trim_val = 0x3;
            pa_cntl_ldo_mo_val = 0xBE;
            break;
        default:
            tx_reserve_lv_val = 0xF5;
            tx_reserve_hv_val = 0xA7;
            tx_upc_ibias_trim_val = 0x3;
            pa_cntl_ldo_mo_val = 0xC8;
            break;
    }

    if (mode == PHY_TX_POLAR) {
        tx_reserve_lv_val = POLAR_TX_RESERVE_LV_VAL;
        tx_reserve_hv_val = POLAR_TX_RESERVE_HV_VAL;
    }
    slc_rf_spi_reg_or_mask(SLC_RF_SPI_ADDR_CTRL(0x8C), (SLC_RFCTRL_TX_RESERVE_LV_VAL(tx_reserve_lv_val) | SLC_RFCTRL_TX_RESERVE_HV_VAL(tx_reserve_hv_val)));
    slc_rf_spi_reg_update(SLC_RF_SPI_ADDR_CTRL(0x7C), (SLC_RFCTRL_TX_UPC_IBIAS_TRIM_MASK | SLC_RFCTRL_TX_BAND_SEL_MASK), (SLC_RFCTRL_TX_UPC_IBIAS_TRIM_VAL(tx_upc_ibias_trim_val) | SLC_RFCTRL_TX_BAND_SEL_VAL(flo)));

    slc_rf_spi_reg_update(SLC_RF_SPI_ADDR_CTRL(0x78), SLC_RFCTRL_PA_CNTL_LDO_MO_MASK, SLC_RFCTRL_PA_CNTL_LDO_MO_VAL(pa_cntl_ldo_mo_val));

    // RXADC误开启问题的bugfix
    if (mode == PHY_TX_IQ) {
        slc_rf_spi_reg_update(SLC_RF_SPI_ADDR_CTRL(0x54), (SLC_RFCTRL_PD_RXADC_COM_MO_MASK | SLC_RFCTRL_PD_RXADC_COM_ME_MASK), (SLC_RFCTRL_PD_RXADC_COM_MO_VAL(1) | SLC_RFCTRL_PD_RXADC_COM_ME_VAL(1)));
    } else {
        slc_rf_spi_reg_update(SLC_RF_SPI_ADDR_CTRL(0x54), (SLC_RFCTRL_PD_RXADC_DIGLDO_MO_MASK | SLC_RFCTRL_PD_RXADC_DIGLDO_ME_MASK), (SLC_RFCTRL_PD_RXADC_DIGLDO_MO_VAL(0) | SLC_RFCTRL_PD_RXADC_DIGLDO_ME_VAL(1)));
    }

    slc_rf_spi_write32_cmd(SLC_RF_SPI_ADDR_CTRL(0x68), ( SLC_RFCTRL_TX_RAMP_STEP_VAL(0xA5) | SLC_RFCTRL_TX_RAMP_CYCLE_VAL(0xFF) |
                                 SLC_RFCTRL_DFE_DELAY_VAL(120)));
}

void slc_rf_tx_disable(void)
{
    /*
     * 1. TX IQ模式RXADC误开启问题的bugfix，离开TX IQ需要还原配置
     * 2. TX polar模式下无法关掉TXABB问题的bugfix，离开TX POLAR时需要的配置
     */
    slc_rf_spi_reg_update(SLC_RF_SPI_ADDR_CTRL(0x54), (SLC_RFCTRL_PD_RXADC_COM_ME_MASK | SLC_RFCTRL_PD_RXADC_DIGLDO_ME_MASK), SLC_RFCTRL_PD_RXADC_DIGLDO_MO_VAL(1));

    /* 复位reserve控制字 */
    slc_rf_spi_reg_update(SLC_RF_SPI_ADDR_CTRL(0x8C), (SLC_RFCTRL_TX_RESERVE_LV_MASK | SLC_RFCTRL_TX_RESERVE_HV_MASK), (SLC_RFCTRL_TX_RESERVE_LV_VAL(0x00) | SLC_RFCTRL_TX_RESERVE_HV_VAL(0x80)));

    slc_rf_spi_reg_update(SLC_RF_SPI_ADDR_PMU(0x1C), SLC_PMU_RESERVE_MASK, SLC_PMU_RESERVE_VAL(0x5));

    slc_rf_spi_reg_update(SLC_RF_SPI_ADDR_CTRL(0x7C), SLC_RFCTRL_TX_UPC_IBIAS_TRIM_MASK, SLC_RFCTRL_TX_UPC_IBIAS_TRIM_VAL(0x3));
}

void slc_rf_rx_tia_ctrl(slc_rf_tia_if_e tia_if)
{
    slc_rf_spi_reg_update(SLC_RF_SPI_ADDR_CTRL(0xB4), SLC_RFCTRL_RXTIA_MODE_CTRL_MASK, SLC_RFCTRL_RXTIA_MODE_CTRL_VAL(tia_if));
}

void slc_rf_rx_cbpf_ctrl(phy_cfg_bw_e bw)
{
    slc_rf_spi_reg_update(SLC_RF_SPI_ADDR_CTRL(0x94),
                          (RFCTRL_RXABB_CBPF_IFMODE_MASK | RFCTRL_RXABB_CBPF_BWMODE_MASK),
                          (RFCTRL_RXABB_CBPF_IFMODE_VAL(slc_rf_get_rxabb_ifmode_bw(bw)) |
                           RFCTRL_RXABB_CBPF_BWMODE_VAL(slc_rf_get_rxabb_bwmode_bw(bw))));
}

void slc_rf_rx_adc_ctrl(void)
{
#if 0
    slc_rf_spi_reg_update(SLC_RF_SPI_ADDR_CTRL(0x54),
                          (SLC_RFCTRL_PD_RXADC_ME_MASK | SLC_RFCTRL_PD_RXADC_COM_ME_MASK |
                           SLC_RFCTRL_PD_RXADC_DIGLDO_ME_MASK | SLC_RFCTRL_PD_RXADC_ANALDO_ME_MASK |
                           SLC_RFCTRL_PD_RXADC_MO_MASK | SLC_RFCTRL_PD_RXADC_COM_MO_MASK |
                           SLC_RFCTRL_PD_RXADC_DIGLDO_MO_MASK | SLC_RFCTRL_PD_RXADC_ANALDO_MO_MASK),
                          (SLC_RFCTRL_PD_RXADC_ME_VAL(1) | SLC_RFCTRL_PD_RXADC_COM_ME_VAL(1) |
                           SLC_RFCTRL_PD_RXADC_DIGLDO_ME_VAL(1) | SLC_RFCTRL_PD_RXADC_ANALDO_ME_VAL(1) |
                           SLC_RFCTRL_PD_RXADC_MO_VAL(0) | SLC_RFCTRL_PD_RXADC_COM_MO_VAL(0) |
                           SLC_RFCTRL_PD_RXADC_DIGLDO_MO_VAL(0) | SLC_RFCTRL_PD_RXADC_ANALDO_MO_VAL(0)));
    slc_rf_spi_reg_update(SLC_RF_SPI_ADDR_CTRL(0xA0), (SLC_RFCTRL_RXADC_RST_ME_MASK | SLC_RFCTRL_RXADC_RST_MO_MASK), SLC_RFCTRL_RXADC_RST_ME_VAL(1) | SLC_RFCTRL_RXADC_RST_MO_VAL(0));
    __DSB();
#endif

    slc_rf_spi_reg_update(SLC_RF_SPI_ADDR_CTRL(0xA0),
                          (SLC_RFCTRL_RXADC_CNT_EN_MASK | SLC_RFCTRL_RXADC_DEM_EN_MASK | SLC_RFCTRL_RXADC_NSPATH_EN_MASK),
                          (SLC_RFCTRL_RXADC_CNT_EN_VAL(1) | SLC_RFCTRL_RXADC_DEM_EN_VAL(1) |
                           SLC_RFCTRL_RXADC_NSPATH_EN_VAL(1)));
}

/* RFFE AGC func */
void slc_rf_rx_agc_ctrl(void)
{
    slc_rf_spi_reg_update(SLC_RF_SPI_ADDR_AGC(0x10), (AGC_GET_PRXADC_TIME_MASK), AGC_GET_PRXADC_TIME_VAL(0x7));

    slc_rf_spi_reg_update(SLC_RF_SPI_ADDR_AGC(0x08), (AGC_AGC_OPT_MASK | AGC_BHF_AGC_PFS_STEP_MASK), (AGC_AGC_OPT_VAL(1) | AGC_BHF_AGC_PFS_STEP_VAL(0x6)));
}

/* RFFE PLL func */
void slc_rf_pll_set_lo_ctrl(slc_rf_pll_lo_ctrl_t *ctrl)
{
    slc_rf_spi_write32_cmd(SLC_RF_SPI_ADDR_PLL(0xB8), ( PLL_LO_MODE_SEL_VAL(ctrl->mode) | PLL_LO_DIV_BAND_VAL(ctrl->div_band_sel) |
                        PLL_LO_BAND_MULT_VAL(ctrl->band_mult) | PLL_LO_EN_POLAR_MO_VAL(ctrl->en_polar_mo) |
                        PLL_LO_DIV_MODE_SEL_VAL(ctrl->mode_sel) | PLL_LO_BAND_SEL_VAL(ctrl->band_sel) |
                        PLL_LO_EN_POLAR_ME_VAL(ctrl->en_polar_me)));
}

/* RFFE DFE func */
// 在发送状态下，1：表示信号完整输出；0：信号删除前端和尾部点数输出；
void slc_rf_dfe_tx_del_keep_pnt_bypass(bool enable)
{
    slc_rf_spi_write32_cmd(SLC_RF_SPI_ADDR_DFE(0xB4), enable);
}

void slc_rf_dfe_set_trx_mode(slc_rf_dfe_mode_e mode)
{
    uint32_t val = slc_rf_spi_read32_cmd(SLC_RF_SPI_ADDR_DFE(0x00));
    val &= ~DFE_WORK_MODE_MASK;
    val |= DFE_WORK_MODE_VAL(mode);
    slc_rf_spi_write32_cmd(SLC_RF_SPI_ADDR_DFE(0x00), val);
    // slc_hal_nop_delay_us(PHY_MAX_DELAY);
}

void slc_rf_dfe_set_link_ctrl_mode(slc_rf_dfe_ctrl_e mode)
{
    uint32_t val = slc_rf_spi_read32_cmd(SLC_RF_SPI_ADDR_DFE(0x00));
    val &= ~DFE_LINK_CTRL_MODE_MASK;
    val |= DFE_LINK_CTRL_MODE_VAL(mode);
    slc_rf_spi_write32_cmd(SLC_RF_SPI_ADDR_DFE(0x00), val);
    // slc_hal_nop_delay_us(PHY_MAX_DELAY);
}

void slc_rf_dfe_set_ddc_fcw(uint32_t freq, uint32_t fs, bool ddc_bypass)
{
    uint32_t val = slc_rf_spi_read32_cmd(SLC_RF_SPI_ADDR_DFE(0xB8));
    uint32_t ddc_fce_reg = 0;
    int32_t ddc_fce = 0;

    ddc_fce = (int32_t)round(-2.0 * PI * freq * 16777216.0 / fs);
    // 将计算的值改为补码格式
    ddc_fce_reg = (1 << 27) + ddc_fce;

    val &= ~(DFE_DDC_FCW_MASK | DFE_DDC_BYPASS_MASK);
    val |= DFE_DDC_FCW_VAL(ddc_fce_reg) | DFE_DDC_BYPASS_VAL(ddc_bypass);
    slc_rf_spi_write32_cmd(SLC_RF_SPI_ADDR_DFE(0xB8), val);
}

void slc_rf_dfe_set_rmdc_bypass_en(bool dc0_bypass, bool dc1_bypass)
{
    if (dc0_bypass)
        slc_rf_spi_reg_or_mask(SLC_RF_SPI_ADDR_DFE(0xE4), DFE_RMDC0_BYPASS_VAL(1));
    else
        slc_rf_spi_reg_clr_mask(SLC_RF_SPI_ADDR_DFE(0xE4), DFE_RMDC0_BYPASS_MASK);

    if (dc1_bypass)
        slc_rf_spi_reg_or_mask(SLC_RF_SPI_ADDR_DFE(0xF0), DFE_RMDC1_BYPASS_VAL(1));
    else
        slc_rf_spi_reg_clr_mask(SLC_RF_SPI_ADDR_DFE(0xF0), DFE_RMDC1_BYPASS_MASK);
}

void slc_rf_dfe_set_bw(slc_rf_bw_e bw)
{
    uint32_t val = slc_rf_spi_read32_cmd(SLC_RF_SPI_ADDR_DFE(0x08));
    val &= ~DFE_BW_MASK;
    val |= DFE_BW_VAL(bw);
    slc_rf_spi_write32_cmd(SLC_RF_SPI_ADDR_DFE(0x08), val);
    // slc_hal_nop_delay_us(PHY_MAX_DELAY);
}

void slc_rf_dfe_set_line_ctrl_bw(slc_rf_dfe_ctrl_e mode)
{
    uint32_t val = slc_rf_spi_read32_cmd(SLC_RF_SPI_ADDR_DFE(0x08));
    val &= ~DFE_LINE_CTRL_BW_MASK;
    val |= DFE_LINE_CTRL_BW_VAL(mode);
    slc_rf_spi_write32_cmd(SLC_RF_SPI_ADDR_DFE(0x08), val);
    // slc_hal_nop_delay_us(PHY_MAX_DELAY);
}

void slc_rf_dfe_bypass_fir59(bool bypass, uint8_t pll_gen_flag, slc_rf_dfe_ctrl_e line_ctrl_pll_gen_flag)
{
    slc_rf_spi_write32_cmd(SLC_RF_SPI_ADDR_DFE(0x0C), (DFE_TX_FIR59_BYPASS_VAL(bypass) |
                                DFE_FIR59_PLL_GEN_VAL(pll_gen_flag) |
                                DFE_FIR59_LINE_CTRL_PLL_GEN_VAL(line_ctrl_pll_gen_flag)));
}

void slc_rf_dfe_set_fir59(phy_cfg_sf_e sf)
{
    if(sf <= PHY_SF8) {
        slc_rf_spi_write32_cmd(SLC_RF_SPI_ADDR_DFE(0x10), 0x00000FFF);
        slc_rf_spi_write32_cmd(SLC_RF_SPI_ADDR_DFE(0x14), 0x00020001);
        slc_rf_spi_write32_cmd(SLC_RF_SPI_ADDR_DFE(0x18), 0x0FFF0001);
        slc_rf_spi_write32_cmd(SLC_RF_SPI_ADDR_DFE(0x1C), 0x0FFC0FFD);
        slc_rf_spi_write32_cmd(SLC_RF_SPI_ADDR_DFE(0x20), 0x00020FFD);
        slc_rf_spi_write32_cmd(SLC_RF_SPI_ADDR_DFE(0x24), 0x000A0007);
        slc_rf_spi_write32_cmd(SLC_RF_SPI_ADDR_DFE(0x28), 0x0FFD0007);
        slc_rf_spi_write32_cmd(SLC_RF_SPI_ADDR_DFE(0x2C), 0x0FEA0FF1);
        slc_rf_spi_write32_cmd(SLC_RF_SPI_ADDR_DFE(0x30), 0x00020FEF);
        slc_rf_spi_write32_cmd(SLC_RF_SPI_ADDR_DFE(0x34), 0x002B001A);
        slc_rf_spi_write32_cmd(SLC_RF_SPI_ADDR_DFE(0x38), 0x00050025);
        slc_rf_spi_write32_cmd(SLC_RF_SPI_ADDR_DFE(0x3C), 0x0FAA0FD3);
        slc_rf_spi_write32_cmd(SLC_RF_SPI_ADDR_DFE(0x40), 0x0FE20FA7);
        slc_rf_spi_write32_cmd(SLC_RF_SPI_ADDR_DFE(0x44), 0x00F4005A);
        slc_rf_spi_write32_cmd(SLC_RF_SPI_ADDR_DFE(0x48), 0x01DB0184);
    } else {
        slc_rf_spi_write32_cmd(SLC_RF_SPI_ADDR_DFE(0x10), 0x00000000);
        slc_rf_spi_write32_cmd(SLC_RF_SPI_ADDR_DFE(0x14), 0x00000000);
        slc_rf_spi_write32_cmd(SLC_RF_SPI_ADDR_DFE(0x18), 0x00000000);
        slc_rf_spi_write32_cmd(SLC_RF_SPI_ADDR_DFE(0x1C), 0x00000000);
        slc_rf_spi_write32_cmd(SLC_RF_SPI_ADDR_DFE(0x20), 0x0FFF0000);
        slc_rf_spi_write32_cmd(SLC_RF_SPI_ADDR_DFE(0x24), 0x00020FFF);
        slc_rf_spi_write32_cmd(SLC_RF_SPI_ADDR_DFE(0x28), 0x0FFD0003);
        slc_rf_spi_write32_cmd(SLC_RF_SPI_ADDR_DFE(0x2C), 0x00040FFA);
        slc_rf_spi_write32_cmd(SLC_RF_SPI_ADDR_DFE(0x30), 0x0FFB000B);
        slc_rf_spi_write32_cmd(SLC_RF_SPI_ADDR_DFE(0x34), 0x00050FEB);
        slc_rf_spi_write32_cmd(SLC_RF_SPI_ADDR_DFE(0x38), 0x00000024);
        slc_rf_spi_write32_cmd(SLC_RF_SPI_ADDR_DFE(0x3C), 0x0FF20FC6);
        slc_rf_spi_write32_cmd(SLC_RF_SPI_ADDR_DFE(0x40), 0x002D005C);
        slc_rf_spi_write32_cmd(SLC_RF_SPI_ADDR_DFE(0x44), 0x0F7D0F67);
        slc_rf_spi_write32_cmd(SLC_RF_SPI_ADDR_DFE(0x48), 0x036D0156);
    }
}

void slc_rf_dfe_sw_reset(void)
{
    slc_rf_spi_reg_or_mask(SLC_RF_SPI_ADDR_PMU(0x08), RF_PMU_RFFE_DFE_RSTN_VAL(1));
}

slc_rf_txlpf_bw_e slc_rf_dfe_get_txlpf_bw(phy_cfg_bw_e bw)
{
    switch (bw) {
        case PHY_BW_500K:
            return RF_TXLPF_450K;
        case PHY_BW_250K:
            return RF_TXLPF_225K;
        case PHY_BW_125K:
        case PHY_BW_62P5K:
        case PHY_BW_31P25K:
        case PHY_BW_15P625K:
        case PHY_BW_7P8125K:
            return RF_TXLPF_112P5K;
        default:
            return RF_TXLPF_450K;
    }
}

slc_rf_rxabb_if_e slc_rf_get_rxabb_ifmode_bw(phy_cfg_bw_e bw)
{
    switch (bw) {
        case PHY_BW_500K:
            return RF_RXABB_IF_500K;
        case PHY_BW_250K:
        case PHY_BW_125K:
        case PHY_BW_62P5K:
        case PHY_BW_31P25K:
        case PHY_BW_15P625K:
        case PHY_BW_7P8125K:
            return RF_RXABB_IF_250K;
        default:
            return RF_RXABB_IF_0K;
    }
}

slc_rf_rxabb_bw_e slc_rf_get_rxabb_bwmode_bw(phy_cfg_bw_e bw)
{
    switch (bw) {
        case PHY_BW_500K:
            return RF_RXABB_500K;
        case PHY_BW_250K:
            return RF_RXABB_250K;
        case PHY_BW_125K:
        case PHY_BW_62P5K:
        case PHY_BW_31P25K:
        case PHY_BW_15P625K:
        case PHY_BW_7P8125K:
            return RF_RXABB_125K;
        default:
            return RF_RXABB_500K;
    }
}

void slc_rf_fpga_set(phy_cfg_bw_e bw, phy_cfg_trx_e trx)
{
    slc_rf_spi_reg_update(SLC_RF_SPI_ADDR_CTRL(0x60), (RF_CTRL_CLK_ME_MASK | RF_CTRL_CLK_ME_MASK), (RF_CTRL_CLK_ME_VAL(1) | RF_CTRL_CLK_MO_VAL(1)));
    slc_rf_spi_reg_update(SLC_RF_SPI_ADDR_DFE(0x00), DFE_CTRL_WORK_MODE_MASK, DFE_CTRL_WORK_MODE_VAL(0));
    
    if (trx == PHY_RX_EN){
        slc_rf_spi_reg_update(SLC_RF_SPI_ADDR_DFE(0x00), DFE_WORK_MODE_MASK, DFE_WORK_MODE_VAL(1));
    }
    else{
        slc_rf_spi_reg_update(SLC_RF_SPI_ADDR_DFE(0x00), DFE_WORK_MODE_MASK, DFE_WORK_MODE_VAL(0));
    }

    slc_rf_spi_reg_update(SLC_RF_SPI_ADDR_DFE(0x08), (DFE_CFG_CTRL_BW_MASK | DFE_CFG_BW_MASK), (DFE_CFG_CTRL_BW_VAL(0) | DFE_CFG_BW_VAL((uint32_t)bw)));
    
    uint32_t reeg = slc_rf_spi_read32_cmd(SLC_RF_SPI_ADDR_CTRL(0x60));
    PRINTF("RF CTRL 0X60 = %08x, %08x!\n",reeg,SLC_RF_SPI_ADDR_CTRL(0x60));
    reeg = slc_rf_spi_read32_cmd(SLC_RF_SPI_ADDR_CTRL(0x00));
    PRINTF("RF CTRL 0X00 = %08x!\n",reeg);
    reeg = slc_rf_spi_read32_cmd(SLC_RF_SPI_ADDR_DFE(0x00));
    PRINTF("RF DFE 0X00 = %08x!\n",reeg);
    reeg = slc_rf_spi_read32_cmd(SLC_RF_SPI_ADDR_DFE(0x08));
    PRINTF("RF DFE 0X08 = %08x!\n",reeg);
    
}

void tx_polar_en(void)
{
    slc_rf_spi_reg_update(SLC_RF_SPI_ADDR_CTRL(0x00), (RF_CTRL_TXMODE_ME_MASK | RF_CTRL_TXMODE_MO_MASK), (RF_CTRL_TXMODE_ME_VAL(1) | RF_CTRL_TXMODE_MO_VAL(1)));
}



