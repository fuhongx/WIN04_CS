/**
 * ****************************************************************************
 * @file qmx_phy.c
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

#include "qmx_hal_delay.h"
#include "qmx_phy.h"
#include "debug.h"

void qmx_set_phy_mode(phy_mode_e mode)
{
    if (mode >= PHY_MODE_INVALID)
        return;

    uint32_t val = PHY_CTRL->CTRL_REG2;

    val &= ~PHY_MODE_MASK;
    val |= PHY_MODE_VAL(mode);
    PHY_CTRL->CTRL_REG2 = val;
}

void qmx_set_phy_tx_mode(phy_cfg_tx_mode_e mode)
{
    if (mode >= PHY_TX_MODE_INVALID)
        return;

    uint32_t val = PHY_CTRL->CTRL_REG1;

    val &= ~PHY_POLAR_SEL_MASK;
    val |= PHY_POLAR_SEL_VAL(mode);
    PHY_CTRL->CTRL_REG1 = val;
}

void qmx_set_monitor_reg_clr(void)
{
    PHY_CTRL->CTRL_REG3 |= PHY_MONITOR_CLR_VAL(1);
    // be sure to wait for clear done
    qmx_hal_nop_delay_us(PHY_MAX_DELAY);
    PHY_CTRL->CTRL_REG3 &= ~PHY_MONITOR_CLR_MASK;
}

void qmx_phy_set_netid(uint16_t netid)
{
    uint32_t val = PHY_CTRL->CTRL_REG3;
    val &= ~PHY_NETID_MASK;
    val |= PHY_NETID_VAL(netid);
    PHY_CTRL->CTRL_REG3 = val;
}

void qmx_phy_sw_reset(void)
{
    PHY_CTRL->CTRL_REG3 &= ~PHY_SRST_MASK;
    qmx_hal_nop_delay_us(PHY_MAX_DELAY);
    PHY_CTRL->CTRL_REG3 |= PHY_SRST_VAL(1);
    qmx_hal_nop_delay_us(PHY_MAX_DELAY);
    PHY_CTRL->CTRL_REG3 &= ~PHY_SRST_MASK;
    qmx_hal_nop_delay_us(PHY_MAX_DELAY);
}

void qmx_rf_dfe_reset(void)
{
    PHY_CTRL->CTRL_REG2 &= ~PHY_RF_DFE_RST_MASK;
    qmx_hal_nop_delay_us(PHY_MAX_DELAY);
    PHY_CTRL->CTRL_REG2 |= PHY_RF_DFE_RST_VAL(1);
    qmx_hal_nop_delay_us(PHY_MAX_DELAY);
    PHY_CTRL->CTRL_REG2 &= ~PHY_RF_DFE_RST_MASK;
    qmx_hal_nop_delay_us(PHY_MAX_DELAY);
}

void qmx_phy_disable_rx(void)
{
    PHY_CTRL->CTRL_REG1 &= ~PHY_RX_EN_MASK;
}

void qmx_phy_set_trx(phy_cfg_trx_e trx_en)
{
    // 所有模式切换和使能尽量一次性配置寄存器，防止中途误切换模式，导致异常
    uint32_t val = PHY_CTRL->CTRL_REG1;
    val &= ~(PHY_TRX_SEL_MASK | PHY_RX_EN_MASK);
    val |= PHY_TRX_SEL_VAL(trx_en);
    if (trx_en == PHY_RX_EN)
        val |= PHY_RX_EN_VAL(1);
    PHY_CTRL->CTRL_REG1 = val;
}

void qmx_phy_cfg_done(bool done)
{
    if (done) {
        // 高脉冲生效，高电平需保持不低于200ns
        PHY_CTRL->CTRL_REG3 &= ~PHY_START_CAL_MASK;
        qmx_hal_nop_delay_us(PHY_MAX_DELAY);
        PHY_CTRL->CTRL_REG3 |= PHY_START_CAL_VAL(1) | PHY_CFG_DONE_VAL(1);
        qmx_hal_nop_delay_us(PHY_MAX_DELAY);
    } else {
        PHY_CTRL->CTRL_REG3 &= ~(PHY_CFG_DONE_MASK | PHY_START_CAL_MASK);
    }
}

void qmx_phy_tx_trigger_single(void)
{
    PHY_CTRL->CTRL_REG3 &= ~PHY_SINGLE_TRING_MASK;
    qmx_hal_nop_delay_us(PHY_MAX_DELAY);
    PHY_CTRL->CTRL_REG3 |= PHY_SINGLE_TRING_VAL(1);
    qmx_hal_nop_delay_us(PHY_MAX_DELAY);
    PHY_CTRL->CTRL_REG3 &= ~PHY_SINGLE_TRING_MASK;
}

void qmx_phy_tx_trigger_repeat(void)
{
    PHY_CTRL->CTRL_REG3 &= ~PHY_REPEAT_TRING_MASK;
    qmx_hal_nop_delay_us(PHY_MAX_DELAY);
    PHY_CTRL->CTRL_REG3 |= PHY_REPEAT_TRING_VAL(1);
    qmx_hal_nop_delay_us(PHY_MAX_DELAY);
    PHY_CTRL->CTRL_REG3 &= ~PHY_REPEAT_TRING_MASK;
}

void qmx_phy_set_sf_reduce_en(bool hsf_en)
{
    if (hsf_en)
        PHY_CTRL->CTRL_REG3 |= PHY_HSF_EN_VAL(1);
    else
        PHY_CTRL->CTRL_REG3 &= ~PHY_HSF_EN_MASK;
}

void qmx_phy_set_timeout(uint32_t timeout)
{
    PHY_CTRL->TIME_OUT_LIMIT = timeout;
}

float qmx_phy_get_bw_hz(uint32_t bw)
{
    float bw_hz = 0.0f;
    switch (bw)
    {
        case PHY_BW_500K:
            bw_hz = 500000.0f;
            break;
        
        case PHY_BW_250K:
            bw_hz = 250000.0f;
            break;

        case PHY_BW_125K:
            bw_hz = 125000.0f;
            break;

        case PHY_BW_62P5K:
            bw_hz = 62500.0f;
            break;

        case PHY_BW_31P25K:
            bw_hz = 31250.0f;
            break;

        case PHY_BW_15P625K:
            bw_hz = 15625.0f;
            break;

        case PHY_BW_7P8125K:
            bw_hz = 7812.5f;
            break;

        default:
            break;
    }

    return bw_hz;
}

void qmx_phy_set_tx_gain(phy_cfg_tx_mode_e tx_mode)
{
    PHY_CTRL->CTRL_REG1 &= ~PHY_TX_MULT_PARAM_MASK;

    if (tx_mode == PHY_TX_IQ)
        PHY_CTRL->CTRL_REG1 |= PHY_TX_MULT_PARAM_VAL(0xc0);
    else
        PHY_CTRL->CTRL_REG1 |= PHY_TX_MULT_PARAM_VAL(0x80);
}

void qmx_phy_set_rx_param_default(uint16_t sf)
{
    uint32_t sfo_track_len = 0;
    uint32_t fd_thld = 0;
    uint32_t fd_mult = 0;
    uint32_t agc_wl_idx = 0;

    switch (sf) {
        case PHY_SF5:
            sfo_track_len = 1;
            fd_thld = 900;
            fd_mult = 22;
            agc_wl_idx = 1;
            break;

        case PHY_SF6:
            sfo_track_len = 1;
            fd_thld = 800;
            fd_mult = 22;
            agc_wl_idx = 1;
            break;

        case PHY_SF7:
            sfo_track_len = 4;
            fd_thld = 1100;
            fd_mult = 18;
            agc_wl_idx = 1;
            break;

        case PHY_SF8:
            sfo_track_len = 4;
            fd_thld = 500;
            fd_mult = 20;
            agc_wl_idx = 1;
            break;

        case PHY_SF9:
        case PHY_SF10:
        case PHY_SF11:
        case PHY_SF12:
            sfo_track_len = 4;
            fd_thld = 500;
            fd_mult = 18;
            if (sf == PHY_SF9)
                agc_wl_idx = 3;
            else
                agc_wl_idx = 4;
            break;

        default:
            return;
    }

    PHY_CTRL->FIND_WIN = (PHY_FD_FIND_WIN_VAL(PHY_FD_FIND_WIN_VAL_DEF) |
                          PHY_SS_FIND_WIN_VAL(PHY_SS_FIND_WIN_VAL_DEF) |
                          PHY_PROTECT_NUM_VAL(PHY_PREAMBLE_PROTECT_NUM_DEF));

    PHY_CTRL->FUNC_REG1 &= ~PHY_FD_RESEARCH_NUM_MASK;
    PHY_CTRL->FUNC_REG1 |= PHY_FD_RESEARCH_NUM_VAL(PHY_FD_RESEARCH_NUM_DEF);

    PHY_CTRL->GAIN &= ~PHY_SYNC_MULT_MASK;
    PHY_CTRL->GAIN |= PHY_SYNC_MULT_VAL(PHY_SYN_MULT_DEF);

    PHY_CTRL->NETID_FILTER = PHY_STOSYN_MULT_VAL(PHY_STOSYN_MULT_DEF) | PHY_DELTAS_VAL(PHY_DELTAS_DEF);

    PHY_CTRL->CTRL_REG3 &= ~PHY_SFO_TRACK_LEN_MASK;
    PHY_CTRL->FRAME_DETECT_THLD &= ~(PHY_FD_THLD_MASK | PHY_FD_MULT_MASK);
    PHY_CTRL->DAGC &= ~PHY_AGC_WL_IDX_MASK;
    PHY_CTRL->CTRL_REG3 |= PHY_SFO_TRACK_LEN_VAL(sfo_track_len);
    PHY_CTRL->FRAME_DETECT_THLD |= (PHY_FD_THLD_VAL(fd_thld) | PHY_FD_MULT_VAL(fd_mult));
    PHY_CTRL->DAGC |= PHY_AGC_WL_IDX_VAL(agc_wl_idx);
}

void qmx_phy_config_common(phy_cfg_t *cfg)
{
    if (cfg == NULL || cfg->sf > PHY_SF_MAX || cfg->sf < PHY_SF_MIN || cfg->bw >= PHY_BW_INVALID)
        return;

    uint32_t val = 0;

    val = PHY_CTRL->CTRL_REG1;
    val &= ~PHY_CFG_MASK;
    val |= (PHY_SF_VAL(cfg->sf) | PHY_BW_VAL(cfg->bw) | PHY_HEADER_EN_VAL(cfg->header) |
            PHY_CRC_EN_VAL(cfg->crc) | PHY_CODERATE_VAL(cfg->coderate) |
            PHY_PREAMBLE_NUM_VAL(cfg->preamble_num));
    PHY_CTRL->CTRL_REG1 = val;

    val = PHY_CTRL->CTRL_REG2;
    val &= ~PHY_PAYLOAD_LEN_MASK;
    val |= PHY_PAYLOAD_LEN_VAL(cfg->payload_len);
    PHY_CTRL->CTRL_REG2 = val;
}

void qmx_phy_cfg_fc(uint32_t freq)
{
    // fc 单位 Hz, 设置值 = floor(fc / 0.95367431640625)
    uint32_t fc_val = floor(freq * ((1 << 20) / 1000000.0));
    PHY_CTRL->CARRIER_FRE = fc_val;
}

void qmx_phy_set_agc_bypass_en(bool agc_bypass)
{
    if (agc_bypass)
        PHY_CTRL->DAGC |= PHY_AGC_BYPASS_VAL(1);
    else
        PHY_CTRL->DAGC &= ~PHY_AGC_BYPASS_MASK;
}

void qmx_phy0_irq_enable(uint32_t irq_mask)
{
    PHY_INT->IRQ0_MASK |= irq_mask & PHY_IRQ_ALL_MASK;
}

void qmx_phy0_irq_disable(uint32_t irq_mask)
{
    PHY_INT->IRQ0_MASK &= ~(irq_mask & PHY_IRQ_ALL_MASK);
}

void qmx_phy1_irq_enable(uint32_t irq_mask)
{
    PHY_INT->IRQ1_MASK |= irq_mask & PHY_IRQ_ALL_MASK;
}

void qmx_phy1_irq_disable(uint32_t irq_mask)
{
    PHY_INT->IRQ1_MASK &= ~(irq_mask & PHY_IRQ_ALL_MASK);
}

uint32_t qmx_phy_irq_get_status(void)
{
    return PHY_INT->IRQ_FLAG;
}

void qmx_phy_irq_clr_status(uint32_t irq_flag)
{
    PHY_INT->IRQ_FLAG = irq_flag & PHY_IRQ_ALL_MASK;
}

uint32_t qmx_phy_monitor_get_rx_det_sta(void)
{
    return PHY_MONITOR->RX_DET_HEADER;
}

void qmx_phy_write_tx_buffer(uint8_t *data, uint32_t len)
{
    if (data == NULL || len == 0 || len > PHY_PAYLOAD_MAX_LEN)
        return;

    uint32_t len_reg = (len + 1) / 2;   // 以16位为单位写入
    uint32_t temp_val = 0;
    uint32_t i = 0;

    for (i = 0; i < len_reg; i++) {
        if (len <= (2*i+1))
            temp_val = data[i * 2] & 0xFF;
        else
            temp_val = (data[i * 2] & 0xFF) | ((data[i * 2 + 1] & 0xFF) << 8);

        write32(ADDR_PHY_TX_BUFFER_BASE + i * 4, temp_val);
    }
}

void qmx_phy_read_rx_buffer(uint8_t *data, uint32_t len)
{
    if (data == NULL || len == 0 || len > PHY_PAYLOAD_MAX_LEN)
        return;

    uint32_t len_reg = (len + 1) / 2;   // 以16位为单位读取
    uint32_t temp_val = 0;
    uint32_t i = 0;

    for (i = 0; i < len_reg; i++) {
        temp_val = read32(ADDR_PHY_RX_BUFFER_BASE + i * 4);
        if (len <= (2*i+1)) {
            data[i * 2] = temp_val & 0xFF;
        } else {
            data[i * 2] = temp_val & 0xFF;
            data[i * 2 + 1] = (temp_val >> 8) & 0xFF;
        }
    }
}
