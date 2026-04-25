/**
 * ****************************************************************************
 * @file qmx_phy.h
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

#ifndef __QMX_PHY_H__
#define __QMX_PHY_H__

#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#include "reg_Dev.h"
#include "error_def.h"
#include "utility.h"

#define PHY_MAX_DELAY                   (10)

#define PHY_PAYLOAD_MAX_LEN             (255)
#define PHY_PAYLOAD_MIN_LEN             (1)

#define PHY_PREAMBLE_MAX_LEN            (65520)
#define PHY_PREAMBLE_MIN_LEN            (4)

#define PHY_SF_MAX                      (12)
#define PHY_SF_MIN                      (5)

#define PHY_FD_FIND_WIN_VAL_DEF         (4)
#define PHY_SS_FIND_WIN_VAL_DEF         (5)
#define PHY_PREAMBLE_PROTECT_NUM_DEF    (5)

#define PHY_FD_RESEARCH_NUM_DEF         (2)
#define PHY_SYN_MULT_DEF                (10)

// NETID_FILTER default value
#define PHY_DELTAS_DEF                  (2)
#define PHY_STOSYN_MULT_DEF             (0x100)

#define PHY_CFG_MASK  ( PHY_SF_MASK | PHY_BW_MASK | PHY_HEADER_EN_MASK | \
                        PHY_CRC_EN_MASK | PHY_CODERATE_MASK | PHY_PREAMBLE_NUM_MASK )

#define PHY_IRQ_ALL_MASK                (0x3FFFF)

#define PHY_IRQ_STO_EST_DONE_SHIFT      (17)
#define PHY_IRQ_STO_EST_DONE_MASK       (1U << PHY_IRQ_STO_EST_DONE_SHIFT)
#define PHY_IRQ_STO_EST_DONE_VAL(x)     (((x) << PHY_IRQ_STO_EST_DONE_SHIFT) & PHY_IRQ_STO_EST_DONE_MASK)
#define PHY_IRQ_NETID_ERR_SHIFT         (16)
#define PHY_IRQ_NETID_ERR_MASK          (1U << PHY_IRQ_NETID_ERR_SHIFT)
#define PHY_IRQ_NETID_ERR_VAL(x)        (((x) << PHY_IRQ_NETID_ERR_SHIFT) & PHY_IRQ_NETID_ERR_MASK)
#define PHY_IRQ_SFSEARCH_DONE_SHIFT     (15)
#define PHY_IRQ_SFSEARCH_DONE_MASK      (1U << PHY_IRQ_SFSEARCH_DONE_SHIFT)
#define PHY_IRQ_SFSEARCH_DONE_VAL(x)    (((x) << PHY_IRQ_SFSEARCH_DONE_SHIFT) & PHY_IRQ_SFSEARCH_DONE_MASK)
#define PHY_IRQ_SFSEARCH_FAIL_SHIFT     (14)
#define PHY_IRQ_SFSEARCH_FAIL_MASK      (1U << PHY_IRQ_SFSEARCH_FAIL_SHIFT)
#define PHY_IRQ_SFSEARCH_FAIL_VAL(x)    (((x) << PHY_IRQ_SFSEARCH_FAIL_SHIFT) & PHY_IRQ_SFSEARCH_FAIL_MASK)
#define PHY_IRQ_CAD_DEMOD_ERR_SHIFT     (13)
#define PHY_IRQ_CAD_DEMOD_ERR_MASK      (1U << PHY_IRQ_CAD_DEMOD_ERR_SHIFT)
#define PHY_IRQ_CAD_DEMOD_ERR_VAL(x)    (((x) << PHY_IRQ_CAD_DEMOD_ERR_SHIFT) & PHY_IRQ_CAD_DEMOD_ERR_MASK)
#define PHY_IRQ_CAD_TIMEOUT_SHIFT       (12)
#define PHY_IRQ_CAD_TIMEOUT_MASK        (1U << PHY_IRQ_CAD_TIMEOUT_SHIFT)
#define PHY_IRQ_CAD_TIMEOUT_VAL(x)      (((x) << PHY_IRQ_CAD_TIMEOUT_SHIFT) & PHY_IRQ_CAD_TIMEOUT_MASK)
#define PHY_IRQ_CAD_SYNCAP_SHIFT        (11)
#define PHY_IRQ_CAD_SYNCAP_MASK         (1U << PHY_IRQ_CAD_SYNCAP_SHIFT)
#define PHY_IRQ_CAD_SYNCAP_VAL(x)       (((x) << PHY_IRQ_CAD_SYNCAP_SHIFT) & PHY_IRQ_CAD_SYNCAP_MASK)
#define PHY_IRQ_CSS_DEMOD_DONE_SHIFT    (10)
#define PHY_IRQ_CSS_DEMOD_DONE_MASK     (1U << PHY_IRQ_CSS_DEMOD_DONE_SHIFT)
#define PHY_IRQ_CSS_DEMOD_DONE_VAL(x)   (((x) << PHY_IRQ_CSS_DEMOD_DONE_SHIFT) & PHY_IRQ_CSS_DEMOD_DONE_MASK)
#define PHY_IRQ_RX_SYM_SYNC_SHIFT       (9)
#define PHY_IRQ_RX_SYM_SYNC_MASK        (1U << PHY_IRQ_RX_SYM_SYNC_SHIFT)
#define PHY_IRQ_RX_SYM_SYNC_VAL(x)      (((x) << PHY_IRQ_RX_SYM_SYNC_SHIFT) & PHY_IRQ_RX_SYM_SYNC_MASK)
#define PHY_IRQ_RX_FRAME_DETECT_SHIFT   (8)
#define PHY_IRQ_RX_FRAME_DETECT_MASK    (1U << PHY_IRQ_RX_FRAME_DETECT_SHIFT)
#define PHY_IRQ_RX_FRAME_DETECT_VAL(x)  (((x) << PHY_IRQ_RX_FRAME_DETECT_SHIFT) & PHY_IRQ_RX_FRAME_DETECT_MASK)
#define PHY_IRQ_TIMEOUT_SHIFT           (7)
#define PHY_IRQ_TIMEOUT_MASK            (1U << PHY_IRQ_TIMEOUT_SHIFT)
#define PHY_IRQ_TIMEOUT_VAL(x)          (((x) << PHY_IRQ_TIMEOUT_SHIFT) & PHY_IRQ_TIMEOUT_MASK)
#define PHY_IRQ_RX_MISS_SYNC_SHIFT      (6)
#define PHY_IRQ_RX_MISS_SYNC_MASK       (1U << PHY_IRQ_RX_MISS_SYNC_SHIFT)
#define PHY_IRQ_RX_MISS_SYNC_VAL(x)     (((x) << PHY_IRQ_RX_MISS_SYNC_SHIFT) & PHY_IRQ_RX_MISS_SYNC_MASK)
#define PHY_IRQ_HD_ERR_SHIFT            (5)
#define PHY_IRQ_HD_ERR_MASK             (1U << PHY_IRQ_HD_ERR_SHIFT)
#define PHY_IRQ_HD_ERR_VAL(x)           (((x) << PHY_IRQ_HD_ERR_SHIFT) & PHY_IRQ_HD_ERR_MASK)
#define PHY_IRQ_HD_OK_SHIFT             (4)
#define PHY_IRQ_HD_OK_MASK              (1U << PHY_IRQ_HD_OK_SHIFT)
#define PHY_IRQ_HD_OK_VAL(x)            (((x) << PHY_IRQ_HD_OK_SHIFT) & PHY_IRQ_HD_OK_MASK)
#define PHY_IRQ_CRC_ERR_SHIFT           (3)
#define PHY_IRQ_CRC_ERR_MASK            (1U << PHY_IRQ_CRC_ERR_SHIFT)
#define PHY_IRQ_CRC_ERR_VAL(x)          (((x) << PHY_IRQ_CRC_ERR_SHIFT) & PHY_IRQ_CRC_ERR_MASK)
#define PHY_IRQ_CRC_OK_SHIFT            (2)
#define PHY_IRQ_CRC_OK_MASK             (1U << PHY_IRQ_CRC_OK_SHIFT)
#define PHY_IRQ_CRC_OK_VAL(x)           (((x) << PHY_IRQ_CRC_OK_SHIFT) & PHY_IRQ_CRC_OK_MASK)
#define PHY_IRQ_RX_DONE_SHIFT           (1)
#define PHY_IRQ_RX_DONE_MASK            (1U << PHY_IRQ_RX_DONE_SHIFT)
#define PHY_IRQ_RX_DONE_VAL(x)          (((x) << PHY_IRQ_RX_DONE_SHIFT) & PHY_IRQ_RX_DONE_MASK)
#define PHY_IRQ_TX_DONE_SHIFT           (0)
#define PHY_IRQ_TX_DONE_MASK            (1U << PHY_IRQ_TX_DONE_SHIFT)
#define PHY_IRQ_TX_DONE_VAL(x)          (((x) << PHY_IRQ_TX_DONE_SHIFT) & PHY_IRQ_TX_DONE_MASK)

typedef enum
{
    PHY_NORMAL_MODE = 0,
    PHY_CAD_MODE,
    PHY_TOF_MODE,

    PHY_MODE_INVALID,
} phy_mode_e;

enum  {
    PHY_MODE_NORMAL = 0,
    PHY_MODE_CAD,
    PHY_MODE_DUTY_CYCLE,
};

typedef enum
{
    PHY_BW_500K = 0,
    PHY_BW_250K,
    PHY_BW_125K,
    PHY_BW_62P5K,
    PHY_BW_31P25K,
    PHY_BW_15P625K,
    PHY_BW_7P8125K,

    PHY_BW_INVALID,
} phy_cfg_bw_e;

typedef enum
{
    PHY_SF5 = 5,
    PHY_SF6,
    PHY_SF7,
    PHY_SF8,
    PHY_SF9,
    PHY_SF10,
    PHY_SF11,
    PHY_SF12
} phy_cfg_sf_e;

typedef enum
{
    PHY_RX_EN = 0,
    PHY_TX_EN,
} phy_cfg_trx_e;

typedef enum
{
    PHY_TX_IQ = 0,
    PHY_TX_POLAR,

    PHY_TX_MODE_INVALID,
} phy_cfg_tx_mode_e;

typedef struct
{
    uint16_t sf;            // 5~12, refer to phy_cfg_sf_e
    uint16_t bw;            // refer to phy_cfg_bw_e
    uint16_t header;        // 0: explicit header, 1: implicit header
    uint16_t crc;           // 0: crc off, 1: crc on
    uint16_t coderate;      // 0: 1/1; 1: 4/7;
    uint16_t payload_len;   // 0~255 byte
    uint16_t preamble_num;  // 范围：4～65520
} phy_cfg_t;

/* phy配置接口 */
void qmx_set_phy_mode(phy_mode_e mode);
void qmx_set_phy_tx_mode(phy_cfg_tx_mode_e mode);
float qmx_phy_get_bw_hz(uint32_t bw);
void qmx_set_monitor_reg_clr(void);
void qmx_phy_set_netid(uint16_t netid);
void qmx_phy_sw_reset(void);
void qmx_rf_dfe_reset(void);
void qmx_phy_disable_rx(void);
void qmx_phy_set_trx(phy_cfg_trx_e trx_en);
void qmx_phy_config_common(phy_cfg_t *cfg);
void qmx_phy_cfg_done(bool done);
void qmx_phy_tx_trigger_single(void);
void qmx_phy_tx_trigger_repeat(void);
void qmx_phy_set_sf_reduce_en(bool hsf_en);
void qmx_phy_set_timeout(uint32_t timeout);
void qmx_phy_set_tx_gain(phy_cfg_tx_mode_e tx_mode);
void qmx_phy_set_rx_param_default(uint16_t sf);
void qmx_phy_cfg_fc(uint32_t freq);
void qmx_phy_set_agc_bypass_en(bool agc_bypass);

/* phy trx数据读写接口 */
void qmx_phy_write_tx_buffer(uint8_t *data, uint32_t len);
void qmx_phy_read_rx_buffer(uint8_t *data, uint32_t len);

// 中断类接口 */
void qmx_phy0_irq_enable(uint32_t irq_mask);
void qmx_phy0_irq_disable(uint32_t irq_mask);
void qmx_phy1_irq_enable(uint32_t irq_mask);
void qmx_phy1_irq_disable(uint32_t irq_mask);
uint32_t qmx_phy_irq_get_status(void);
void qmx_phy_irq_clr_status(uint32_t irq_flag);

/* 维测类接口 */
uint32_t qmx_phy_monitor_get_rx_det_sta(void);

#endif
