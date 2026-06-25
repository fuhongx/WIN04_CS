/**
 * ****************************************************************************
 * @file reg_phy.h
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

#ifndef __REG_PHY_H__
#define __REG_PHY_H__

typedef struct
{
    __IOM uint32_t CTRL_REG1;           // 0x00
    __IOM uint32_t CTRL_REG2;           // 0x04
    __IOM uint32_t CTRL_REG3;           // 0x08
    __IOM uint32_t CTRL_REG4;           // 0x0C
    __IOM uint32_t FIND_WIN;            // 0x10
    __IOM uint32_t GAIN;                // 0x14
    __IOM uint32_t CARRIER_FRE;         // 0x18
    __IOM uint32_t FUNC_REG1;           // 0x1C
    __IOM uint32_t TIMER_SET_VAL;       // 0x20
    __IOM uint32_t TX_TIME;             // 0x24
    __IOM uint32_t TIME_OUT_LIMIT;      // 0x28
    __IOM uint32_t FRAME_DETECT_THLD;   // 0x2C
    __IOM uint32_t STATE_SHIFT_DELAY;   // 0x30
    __IOM uint32_t FARROWPARAM1;        // 0x34
    __IOM uint32_t FARROWPARAM2;        // 0x38
    __IOM uint32_t DAGC;                // 0x3C
    __IOM uint32_t IIR_FILTER;          // 0x40
    __IOM uint32_t NETID_FILTER;        // 0x44
    __IOM uint32_t CFO_VALUE;           // 0x48
    __IOM uint32_t RESV0[3];            // 0x4C ~ 0x54
    __IOM uint32_t CAD1;                // 0x58
    __IOM uint32_t CAD2;                // 0x5C
    __IOM uint32_t CAD3;                // 0x60
    __IOM uint32_t CAD_WAIT_TIME;       // 0x64
    __IOM uint32_t RESV;                // 0x68
    __IOM uint32_t RANGING1;            // 0x6C
    __IOM uint32_t RANGING2;            // 0x70
    __IOM uint32_t RESV2[1];            // 0x74 ~ 0x74
    __IOM uint32_t SF_SEARCH1;           // 0x78
    __IOM uint32_t SF_SEARCH2;          // 0x7C
}stPhyCtrlHandle_t, *pstPhyCtrlHandle_t;

typedef struct
{
    __IOM uint32_t IRQ_FLAG;        //0x00
    __IOM uint32_t IRQ0_MASK;       //0x04
    __IOM uint32_t IRQ1_MASK;       //0x08
}stPhyIntHandle_t, *pstPhyIntHandle_t;

typedef struct
{
    __IOM uint32_t VERSION;             // 0x00
    __IOM uint32_t HEADER_GOOD;         // 0x04
    __IOM uint32_t HEADER_ERROR;        // 0x08
    __IOM uint32_t HEADER_ALL0;         // 0x0C
    __IOM uint32_t PAYLOAD_GOOD;        // 0x10
    __IOM uint32_t PAYLOAD_ERROR;       // 0x14
    __IOM uint32_t PAYLOAD_ALL0;        // 0x18
    __IOM uint32_t PAYLOAD_CRC;         // 0x1C
    __IOM uint32_t RX_FRAME_DET_CNT;    // 0x20
    __IOM uint32_t RSSI;                // 0x24
    __IOM uint32_t PNOISE;              // 0x28
    __IOM uint32_t PSIGNAL;             // 0x2C
    __IOM uint32_t DNOISE;              // 0x30
    __IOM uint32_t DSIGNAL;             // 0x34
    __IOM uint32_t TIMER_VALUE;         // 0x38
    __IOM uint32_t RXDONE_TIMER_VALUE;  // 0x3C
    __IOM uint32_t RX_DET_HEADER;       // 0x40
    __IOM uint32_t RF_GAIN_IDX;         // 0x44
    __IOM uint32_t RX_CTO_PHY;          // 0x48
    __IOM uint32_t RX_STO_PHY;          // 0x4C
    __IOM uint32_t RX_SFO_PHY;          // 0x50
    __IOM uint32_t RX_SYMBL_SYNC_CNT;   // 0x54
    __IOM uint32_t RX_RANG_1STSTO;      // 0x58
    __IOM uint32_t RX_RANG_2NDSTO;      // 0x5C
    __IOM uint32_t RX_MISS_SYNC_CNT;    // 0x60
}stPhyMoniHandle_t, *pstPhyMoniHandle_t;

typedef struct
{
    __IOM uint32_t SIGNAL_SEL;      //0x00
    __IOM uint32_t CONTROL2_REG;    //0x04
}stPhyDebugHandle_t, *pstPhyDebugHandle_t;

// CTRL_REG1 bitfield define
#define PHY_PREAMBLE_NUM_SHIFT      (0)
#define PHY_PREAMBLE_NUM_MASK       (0xFFFFU << PHY_PREAMBLE_NUM_SHIFT)
#define PHY_PREAMBLE_NUM_VAL(x)     (((uint32_t)(x) << PHY_PREAMBLE_NUM_SHIFT) & PHY_PREAMBLE_NUM_MASK)
#define PHY_SF_SHIFT                (16)
#define PHY_SF_MASK                 (0xFU << PHY_SF_SHIFT)
#define PHY_SF_VAL(x)               (((uint32_t)(x) << PHY_SF_SHIFT) & PHY_SF_MASK)
#define PHY_BW_SHIFT                (20)
#define PHY_BW_MASK                 (0xFU << PHY_BW_SHIFT)
#define PHY_BW_VAL(x)               (((uint32_t)(x) << PHY_BW_SHIFT) & PHY_BW_MASK)
#define PHY_HEADER_EN_SHIFT         (24)
#define PHY_HEADER_EN_MASK          (0x1U << PHY_HEADER_EN_SHIFT)
#define PHY_HEADER_EN_VAL(x)        (((uint32_t)(x) << PHY_HEADER_EN_SHIFT) & PHY_HEADER_EN_MASK)
#define PHY_CODERATE_SHIFT          (26)
#define PHY_CODERATE_MASK           (0x1U << PHY_CODERATE_SHIFT)
#define PHY_CODERATE_VAL(x)         (((uint32_t)(x) << PHY_CODERATE_SHIFT) & PHY_CODERATE_MASK)
#define PHY_CRC_EN_SHIFT            (28)
#define PHY_CRC_EN_MASK             (0x1U << PHY_CRC_EN_SHIFT)
#define PHY_CRC_EN_VAL(x)           (((uint32_t)(x) << PHY_CRC_EN_SHIFT) & PHY_CRC_EN_MASK)
#define PHY_POLAR_SEL_SHIFT         (29)
#define PHY_POLAR_SEL_MASK          (0x1U << PHY_POLAR_SEL_SHIFT)
#define PHY_POLAR_SEL_VAL(x)        (((uint32_t)(x) << PHY_POLAR_SEL_SHIFT) & PHY_POLAR_SEL_MASK)
#define PHY_TRX_SEL_SHIFT           (30)
#define PHY_TRX_SEL_MASK            (0x1U << PHY_TRX_SEL_SHIFT)
#define PHY_TRX_SEL_VAL(x)          (((uint32_t)(x) << PHY_TRX_SEL_SHIFT) & PHY_TRX_SEL_MASK)
#define PHY_RX_EN_SHIFT             (31)
#define PHY_RX_EN_MASK              (0x1U << PHY_RX_EN_SHIFT)
#define PHY_RX_EN_VAL(x)            (((uint32_t)(x) << PHY_RX_EN_SHIFT) & PHY_RX_EN_MASK)

// CTRL_REG2 bitfield define
#define PHY_PAYLOAD_LEN_SHIFT       (0)
#define PHY_PAYLOAD_LEN_MASK        (0x3FFU << PHY_PAYLOAD_LEN_SHIFT)
#define PHY_PAYLOAD_LEN_VAL(x)      (((uint32_t)(x) << PHY_PAYLOAD_LEN_SHIFT) & PHY_PAYLOAD_LEN_MASK)
#define PHY_RF_DFE_RST_SHIFT        (11)
#define PHY_RF_DFE_RST_MASK         (0x1U << PHY_RF_DFE_RST_SHIFT)
#define PHY_RF_DFE_RST_VAL(x)       (((uint32_t)(x) << PHY_RF_DFE_RST_SHIFT) & PHY_RF_DFE_RST_MASK)
#define PHY_MODE_SHIFT              (12)
#define PHY_MODE_MASK               (0xFU << PHY_MODE_SHIFT)
#define PHY_MODE_VAL(x)             (((uint32_t)(x) << PHY_MODE_SHIFT) & PHY_MODE_MASK)
#define PHY_TX_DONE_DLY_SHIFT       (16)
#define PHY_TX_DONE_DLY_MASK        (0xFFFFU << PHY_TX_DONE_DLY_SHIFT)
#define PHY_TX_DONE_DLY_VAL(x)      (((uint32_t)(x) << PHY_TX_DONE_DLY_SHIFT) & PHY_TX_DONE_DLY_MASK)

// CTRL_REG3 bitfield define
#define PHY_SINGLE_TRING_SHIFT      (0)
#define PHY_SINGLE_TRING_MASK       (0x1U << PHY_SINGLE_TRING_SHIFT)
#define PHY_SINGLE_TRING_VAL(x)     (((uint32_t)(x) << PHY_SINGLE_TRING_SHIFT) & PHY_SINGLE_TRING_MASK)
#define PHY_REPEAT_TRING_SHIFT      (1)
#define PHY_REPEAT_TRING_MASK       (0x1U << PHY_REPEAT_TRING_SHIFT)
#define PHY_REPEAT_TRING_VAL(x)     (((uint32_t)(x) << PHY_REPEAT_TRING_SHIFT) & PHY_REPEAT_TRING_MASK)
#define PHY_START_CAL_SHIFT         (2)
#define PHY_START_CAL_MASK          (0x1U << PHY_START_CAL_SHIFT)
#define PHY_START_CAL_VAL(x)        (((uint32_t)(x) << PHY_START_CAL_SHIFT) & PHY_START_CAL_MASK)
#define PHY_CFG_DONE_SHIFT          (3)
#define PHY_CFG_DONE_MASK           (0x1U << PHY_CFG_DONE_SHIFT)
#define PHY_CFG_DONE_VAL(x)         (((uint32_t)(x) << PHY_CFG_DONE_SHIFT) & PHY_CFG_DONE_MASK)
#define PHY_NETID_SHIFT             (4)
#define PHY_NETID_MASK              (0xFFFU << PHY_NETID_SHIFT)
#define PHY_NETID_VAL(x)            (((uint32_t)(x) << PHY_NETID_SHIFT) & PHY_NETID_MASK)
#define PHY_HSF_EN_SHIFT            (16)
#define PHY_HSF_EN_MASK             (0x1U << PHY_HSF_EN_SHIFT)
#define PHY_HSF_EN_VAL(x)           (((uint32_t)(x) << PHY_HSF_EN_SHIFT) & PHY_HSF_EN_MASK)
#define PHY_SRST_SHIFT              (18)
#define PHY_SRST_MASK               (0x1U << PHY_SRST_SHIFT)
#define PHY_SRST_VAL(x)             (((uint32_t)(x) << PHY_SRST_SHIFT) & PHY_SRST_MASK)
#define PHY_MONITOR_CLR_SHIFT       (20)
#define PHY_MONITOR_CLR_MASK        (0x1U << PHY_MONITOR_CLR_SHIFT)
#define PHY_MONITOR_CLR_VAL(x)      (((uint32_t)(x) << PHY_MONITOR_CLR_SHIFT) & PHY_MONITOR_CLR_MASK)
#define PHY_SFO_TRACK_LEN_SHIFT     (24)
#define PHY_SFO_TRACK_LEN_MASK      (0xFU << PHY_SFO_TRACK_LEN_SHIFT)
#define PHY_SFO_TRACK_LEN_VAL(x)    (((uint32_t)(x) << PHY_SFO_TRACK_LEN_SHIFT) & PHY_SFO_TRACK_LEN_MASK)

// CTRL_REG4 bitfield define
#define PHY_TX_REPEAT_NUM_SHIFT     (0)
#define PHY_TX_REPEAT_NUM_MASK      (0x3FFU << PHY_TX_REPEAT_NUM_SHIFT)
#define PHY_TX_REPEAT_NUM_VAL(x)    (((uint32_t)(x) << PHY_TX_REPEAT_NUM_SHIFT) & PHY_TX_REPEAT_NUM_MASK)
#define PHY_TX_REPEAT_INTERAL_SHIFT (10)
#define PHY_TX_REPEAT_INTERAL_MASK  (0x3FFFFFU << PHY_TX_REPEAT_INTERAL_SHIFT)
#define PHY_TX_REPEAT_INTERAL_VAL(x) (((uint32_t)(x) << PHY_TX_REPEAT_INTERAL_SHIFT) & PHY_TX_REPEAT_INTERAL_MASK)

// FIND_WIN bitfield define
#define PHY_FD_FIND_WIN_SHIFT       (0)
#define PHY_FD_FIND_WIN_MASK        (0xFU << PHY_FD_FIND_WIN_SHIFT)
#define PHY_FD_FIND_WIN_VAL(x)      (((uint32_t)(x) << PHY_FD_FIND_WIN_SHIFT) & PHY_FD_FIND_WIN_MASK)
#define PHY_SS_FIND_WIN_SHIFT       (4)
#define PHY_SS_FIND_WIN_MASK        (0xFU << PHY_SS_FIND_WIN_SHIFT)
#define PHY_SS_FIND_WIN_VAL(x)      (((uint32_t)(x) << PHY_SS_FIND_WIN_SHIFT) & PHY_SS_FIND_WIN_MASK)
#define PHY_PROTECT_NUM_SHIFT       (16)
#define PHY_PROTECT_NUM_MASK        (0xFFFFU << PHY_PROTECT_NUM_SHIFT)
#define PHY_PROTECT_NUM_VAL(x)      (((uint32_t)(x) << PHY_PROTECT_NUM_SHIFT) & PHY_PROTECT_NUM_MASK)

// GAIN_REG bitfield define
#define PHY_PEAK_JUD_MULT_SHIFT     (0)
#define PHY_PEAK_JUD_MULT_MASK      (0xFFU << PHY_PEAK_JUD_MULT_SHIFT)
#define PHY_PEAK_JUD_MULT_VAL(x)    (((uint32_t)(x) << PHY_PEAK_JUD_MULT_SHIFT) & PHY_PEAK_JUD_MULT_MASK)
#define PHY_SYNC_MULT_SHIFT         (8)
#define PHY_SYNC_MULT_MASK          (0x3FU << PHY_SYNC_MULT_SHIFT)
#define PHY_SYNC_MULT_VAL(x)        (((uint32_t)(x) << PHY_SYNC_MULT_SHIFT) & PHY_SYNC_MULT_MASK)
#define PHY_TX_MULT_PARAM_SHIFT     (16)
#define PHY_TX_MULT_PARAM_MASK      (0x7FFU << PHY_TX_MULT_PARAM_SHIFT)
#define PHY_TX_MULT_PARAM_VAL(x)    (((uint32_t)(x) << PHY_TX_MULT_PARAM_SHIFT) & PHY_TX_MULT_PARAM_MASK)

// FUNC_REG1 bitfield define
#define PHY_STATE_STA_SHIFT         (0)
#define PHY_STATE_STA_MASK          (0xFFU << PHY_STATE_STA_SHIFT)
#define PHY_STATE_STA_VAL(x)        (((uint32_t)(x) << PHY_STATE_STA_SHIFT) & PHY_STATE_STA_MASK)
#define PHY_FUNC_SEL_SHIFT          (8)
#define PHY_FUNC_SEL_MASK           (0x3U << PHY_FUNC_SEL_SHIFT)
#define PHY_FUNC_SEL_VAL(x)         (((uint32_t)(x) << PHY_FUNC_SEL_SHIFT) & PHY_FUNC_SEL_MASK)
#define PHY_EXIT_TIM_TRANS_SHIFT    (10)
#define PHY_EXIT_TIM_TRANS_MASK     (0x1U << PHY_EXIT_TIM_TRANS_SHIFT)
#define PHY_EXIT_TIM_TRANS_VAL(x)   (((uint32_t)(x) << PHY_EXIT_TIM_TRANS_SHIFT) & PHY_EXIT_TIM_TRANS_MASK)
#define PHY_FD_SCHEME_SEL_SHIFT     (11)
#define PHY_FD_SCHEME_SEL_MASK      (0x1U << PHY_FD_SCHEME_SEL_SHIFT)
#define PHY_FD_SCHEME_SEL_VAL(x)    (((uint32_t)(x) << PHY_FD_SCHEME_SEL_SHIFT) & PHY_FD_SCHEME_SEL_MASK)
#define PHY_FD_RESEARCH_NUM_SHIFT   (12)
#define PHY_FD_RESEARCH_NUM_MASK    (0xFU << PHY_FD_RESEARCH_NUM_SHIFT)
#define PHY_FD_RESEARCH_NUM_VAL(x)  (((uint32_t)(x) << PHY_FD_RESEARCH_NUM_SHIFT) & PHY_FD_RESEARCH_NUM_MASK)
#define PHY_SNR_SYM_NUM_SHIFT       (16)
#define PHY_SNR_SYM_NUM_MASK        (0xFFFFU << PHY_SNR_SYM_NUM_SHIFT)
#define PHY_SNR_SYM_NUM_VAL(x)      (((uint32_t)(x) << PHY_SNR_SYM_NUM_SHIFT) & PHY_SNR_SYM_NUM_MASK)

// FRAME_DETECT_THLD bitfield define
#define PHY_FD_THLD_SHIFT           (0)
#define PHY_FD_THLD_MASK            (0xFFFFU << PHY_FD_THLD_SHIFT)
#define PHY_FD_THLD_VAL(x)          (((uint32_t)(x) << PHY_FD_THLD_SHIFT) & PHY_FD_THLD_MASK)
#define PHY_FD_MULT_SHIFT           (16)
#define PHY_FD_MULT_MASK            (0xFFU << PHY_FD_MULT_SHIFT)
#define PHY_FD_MULT_VAL(x)          (((uint32_t)(x) << PHY_FD_MULT_SHIFT) & PHY_FD_MULT_MASK)

// DAGC bitfield define
#define PHY_AGC_WL_IDX_SHIFT        (0)
#define PHY_AGC_WL_IDX_MASK         (0x7U << PHY_AGC_WL_IDX_SHIFT)
#define PHY_AGC_WL_IDX_VAL(x)       (((uint32_t)(x) << PHY_AGC_WL_IDX_SHIFT) & PHY_AGC_WL_IDX_MASK)
#define PHY_TX_FARROW_EN_SHIFT      (9)
#define PHY_TX_FARROW_EN_MASK       (0x1U << PHY_TX_FARROW_EN_SHIFT)
#define PHY_TX_FARROW_EN_VAL(x)     (((uint32_t)(x) << PHY_TX_FARROW_EN_SHIFT) & PHY_TX_FARROW_EN_MASK)
#define PHY_FARROW_SEL_SHIFT        (10)
#define PHY_FARROW_SEL_MASK         (0x3U << PHY_FARROW_SEL_SHIFT)
#define PHY_FARROW_SEL_VAL(x)       (((uint32_t)(x) << PHY_FARROW_SEL_SHIFT) & PHY_FARROW_SEL_MASK)
#define PHY_CFO_CFG_SHIFT           (13)
#define PHY_CFO_CFG_MASK            (0x1U << PHY_CFO_CFG_SHIFT)
#define PHY_CFO_CFG_VAL(x)          (((uint32_t)(x) << PHY_CFO_CFG_SHIFT) & PHY_CFO_CFG_MASK)
#define PHY_AGC_BYPASS_SHIFT        (14)
#define PHY_AGC_BYPASS_MASK         (0x1U << PHY_AGC_BYPASS_SHIFT)
#define PHY_AGC_BYPASS_VAL(x)       (((uint32_t)(x) << PHY_AGC_BYPASS_SHIFT) & PHY_AGC_BYPASS_MASK)

// NETID_FILTER bitfield define
#define PHY_STOSYN_MULT_SHIFT       (0)
#define PHY_STOSYN_MULT_MASK        (0xFFFFU << PHY_STOSYN_MULT_SHIFT)
#define PHY_STOSYN_MULT_VAL(x)      (((uint32_t)(x) << PHY_STOSYN_MULT_SHIFT) & PHY_STOSYN_MULT_MASK)
#define PHY_DELTAS_SHIFT            (16)
#define PHY_DELTAS_MASK             (0xFFFFU << PHY_DELTAS_SHIFT)
#define PHY_DELTAS_VAL(x)           (((uint32_t)(x) << PHY_DELTAS_SHIFT) & PHY_DELTAS_MASK)

// CAD1 bitfield define
#define PHY_CAD_MODEL_EN_SHIFT      (0)
#define PHY_CAD_MODEL_EN_MASK       (0x1U << PHY_CAD_MODEL_EN_SHIFT)
#define PHY_CAD_MODEL_EN_VAL(x)     (((uint32_t)(x) << PHY_CAD_MODEL_EN_SHIFT) & PHY_CAD_MODEL_EN_MASK)
#define PHY_CAD_DUTY_EN_SHIFT       (1)
#define PHY_CAD_DUTY_EN_MASK        (0x1U << PHY_CAD_DUTY_EN_SHIFT)
#define PHY_CAD_DUTY_EN_VAL(x)      (((uint32_t)(x) << PHY_CAD_DUTY_EN_SHIFT) & PHY_CAD_DUTY_EN_MASK)
#define PHY_CAD_EXIT_DUTY_EN_SHIFT  (2)
#define PHY_CAD_EXIT_DUTY_EN_MASK   (0x1U << PHY_CAD_EXIT_DUTY_EN_SHIFT)
#define PHY_CAD_EXIT_DUTY_EN_VAL(x) (((uint32_t)(x) << PHY_CAD_EXIT_DUTY_EN_SHIFT) & PHY_CAD_EXIT_DUTY_EN_MASK)
#define PHY_PRECAD_EN_SHIFT         (3)
#define PHY_PRECAD_EN_MASK          (0x1U << PHY_PRECAD_EN_SHIFT)
#define PHY_PRECAD_EN_VAL(x)        (((uint32_t)(x) << PHY_PRECAD_EN_SHIFT) & PHY_PRECAD_EN_MASK)
#define PHY_PRECAD_NOSLEEP_SHIFT    (4)
#define PHY_PRECAD_NOSLEEP_MASK     (0x3FFU << PHY_PRECAD_NOSLEEP_SHIFT)
#define PHY_PRECAD_NOSLEEP_VAL(x)   (((uint32_t)(x) << PHY_PRECAD_NOSLEEP_SHIFT) & PHY_PRECAD_NOSLEEP_MASK)
#define PHY_CAD_DUTY_CYCLE_SHIFT    (16)
#define PHY_CAD_DUTY_CYCLE_MASK     (0xFFFFU << PHY_CAD_DUTY_CYCLE_SHIFT)
#define PHY_CAD_DUTY_CYCLE_VAL(x)   (((uint32_t)(x) << PHY_CAD_DUTY_CYCLE_SHIFT) & PHY_CAD_DUTY_CYCLE_MASK)

// CAD2 bitfield define
#define PHY_CAD_PRE_NUM_SHIFT       (0)
#define PHY_CAD_PRE_NUM_MASK        (0xFU << PHY_CAD_PRE_NUM_SHIFT)
#define PHY_CAD_PRE_NUM_VAL(x)      (((uint32_t)(x) << PHY_CAD_PRE_NUM_SHIFT) & PHY_CAD_PRE_NUM_MASK)
#define PHY_CAD_PREADDR_SHIFT       (4)
#define PHY_CAD_PREADDR_MASK        (0xFFFU << PHY_CAD_PREADDR_SHIFT)
#define PHY_CAD_PREADDR_VAL(x)      (((uint32_t)(x) << PHY_CAD_PREADDR_SHIFT) & PHY_CAD_PREADDR_MASK)
#define PHY_CAD_CHECK_SYM_NUM_SHIFT (16)
#define PHY_CAD_CHECK_SYM_NUM_MASK  (0xFFFFU << PHY_CAD_CHECK_SYM_NUM_SHIFT)
#define PHY_CAD_CHECK_SYM_NUM_VAL(x) (((uint32_t)(x) << PHY_CAD_CHECK_SYM_NUM_SHIFT) & PHY_CAD_CHECK_SYM_NUM_MASK)

// CAD3 bitfield define
#define PHY_CAD_MARKER_SHIFT        (0)
#define PHY_CAD_MARKER_MASK         (0xFFFU << PHY_CAD_MARKER_SHIFT)
#define PHY_CAD_MARKER_VAL(x)       (((uint32_t)(x) << PHY_CAD_MARKER_SHIFT) & PHY_CAD_MARKER_MASK)
#define PHY_MATCHADDR_WIDE_SHIFT    (12)
#define PHY_MATCHADDR_WIDE_MASK     (0xFU << PHY_MATCHADDR_WIDE_SHIFT)
#define PHY_MATCHADDR_WIDE_VAL(x)   (((uint32_t)(x) << PHY_MATCHADDR_WIDE_SHIFT) & PHY_MATCHADDR_WIDE_MASK)
#define PHY_CAD_CYCLE_NUM_SHIFT     (16)
#define PHY_CAD_CYCLE_NUM_MASK      (0x3FFU << PHY_CAD_CYCLE_NUM_SHIFT)
#define PHY_CAD_CYCLE_NUM_VAL(x)    (((uint32_t)(x) << PHY_CAD_CYCLE_NUM_SHIFT) & PHY_CAD_CYCLE_NUM_MASK)
#define PHY_CAD_NO_DATA_SHIFT       (26)
#define PHY_CAD_NO_DATA_MASK        (0x1U << PHY_CAD_NO_DATA_SHIFT)
#define PHY_CAD_NO_DATA_VAL(x)      (((uint32_t)(x) << PHY_CAD_NO_DATA_SHIFT) & PHY_CAD_NO_DATA_MASK)


#define PHY_RX_DET_PL_CRC_SHIFT     (0)
#define PHY_RX_DET_PL_CRC_MASK      (0x1U << PHY_RX_DET_PL_CRC_SHIFT)
#define PHY_RX_DET_PL_CRC_VAL(x)    (((uint32_t)(x) << PHY_RX_DET_PL_CRC_SHIFT) & PHY_RX_DET_PL_CRC_MASK)
#define PHY_RX_DET_PL_CR_SHIFT      (1)
#define PHY_RX_DET_PL_CR_MASK       (0x1U << PHY_RX_DET_PL_CR_SHIFT)
#define PHY_RX_DET_PL_CR_VAL(x)     (((uint32_t)(x) << PHY_RX_DET_PL_CR_SHIFT) & PHY_RX_DET_PL_CR_MASK)
#define PHY_RX_PAYLOAD_LEN_SHIFT    (2)
#define PHY_RX_PAYLOAD_LEN_MASK     (0x3FFU << PHY_RX_PAYLOAD_LEN_SHIFT)
#define PHY_RX_PAYLOAD_LEN_VAL(x)   (((uint32_t)(x) << PHY_RX_PAYLOAD_LEN_SHIFT) & PHY_RX_PAYLOAD_LEN_MASK)

// SF_SEARCH bitfield define
#define PHY_SF_SEARCH_MULTI1_SHIFT  (0)
#define PHY_SF_SEARCH_MULTI1_MASK   (0x3FU << PHY_SF_SEARCH_MULTI1_SHIFT)
#define PHY_SF_SEARCH_MULTI1_VAL(x) (((uint32_t)(x) << PHY_SF_SEARCH_MULTI1_SHIFT) & PHY_SF_SEARCH_MULTI1_MASK)
#define PHY_SF_SEARCH_MULTI2_SHIFT  (6)
#define PHY_SF_SEARCH_MULTI2_MASK   (0x3FU << PHY_SF_SEARCH_MULTI2_SHIFT)
#define PHY_SF_SEARCH_MULTI2_VAL(x) (((uint32_t)(x) << PHY_SF_SEARCH_MULTI2_SHIFT) & PHY_SF_SEARCH_MULTI2_MASK)
#define PHY_SF_SEARCH_MULTI3_SHIFT  (12)
#define PHY_SF_SEARCH_MULTI3_MASK   (0x3FU << PHY_SF_SEARCH_MULTI3_SHIFT)
#define PHY_SF_SEARCH_MULTI3_VAL(x) (((uint32_t)(x) << PHY_SF_SEARCH_MULTI3_SHIFT) & PHY_SF_SEARCH_MULTI3_MASK)
#define PHY_SF_SEARCH_NUM_SHIFT     (18)
#define PHY_SF_SEARCH_NUM_MASK      (0x3FU << PHY_SF_SEARCH_NUM_SHIFT)
#define PHY_SF_SEARCH_NUM_VAL(x)    (((uint32_t)(x) << PHY_SF_SEARCH_NUM_SHIFT) & PHY_SF_SEARCH_NUM_MASK)
#define PHY_SF_SEARCH_EN_SHIFT      (31)
#define PHY_SF_SEARCH_EN_MASK       (0x1U << PHY_SF_SEARCH_EN_SHIFT)
#define PHY_SF_SEARCH_EN_VAL(x)     (((uint32_t)(x) << PHY_SF_SEARCH_EN_SHIFT) & PHY_SF_SEARCH_EN_MASK)

#define PHY_SF_SEARCH_LEN_SHIFT      (24)
#define PHY_SF_SEARCH_LEN_MASK       (0x3FU << PHY_SF_SEARCH_LEN_SHIFT)
#define PHY_SF_SEARCH_LEN_VAL(x)     (((uint32_t)(x) << PHY_SF_SEARCH_LEN_SHIFT) & PHY_SF_SEARCH_LEN_MASK)

#define PHY_SF_SEARCH4_SHIFT      (28)
#define PHY_SF_SEARCH4_MASK       (0xFU << PHY_SF_SEARCH4_SHIFT)
#define PHY_SF_SEARCH4_VAL(x)     (((uint32_t)(x) << PHY_SF_SEARCH4_SHIFT) & PHY_SF_SEARCH4_MASK)

#define PHY_SF_SEARCH3_SHIFT      (24)
#define PHY_SF_SEARCH3_MASK       (0xFU << PHY_SF_SEARCH3_SHIFT)
#define PHY_SF_SEARCH3_VAL(x)     (((uint32_t)(x) << PHY_SF_SEARCH3_SHIFT) & PHY_SF_SEARCH3_MASK)

#define PHY_SF_SEARCH2_SHIFT      (20)
#define PHY_SF_SEARCH2_MASK       (0xFU << PHY_SF_SEARCH2_SHIFT)
#define PHY_SF_SEARCH2_VAL(x)     (((uint32_t)(x) << PHY_SF_SEARCH2_SHIFT) & PHY_SF_SEARCH2_MASK)

#define PHY_SF_SEARCH1_SHIFT      (16)
#define PHY_SF_SEARCH1_MASK       (0xFU << PHY_SF_SEARCH1_SHIFT)
#define PHY_SF_SEARCH1_VAL(x)     (((uint32_t)(x) << PHY_SF_SEARCH1_SHIFT) & PHY_SF_SEARCH1_MASK)

#define PHY_SF_SEARCH_ADDR_SHIFT      (0)
#define PHY_SF_SEARCH_ADDR_MASK       (0xFU << PHY_SF_SEARCH_ADDR_SHIFT)
#define PHY_SF_SEARCH_ADDR_VAL(x)     (((uint32_t)(x) << PHY_SF_SEARCH_ADDR_SHIFT) & PHY_SF_SEARCH_ADDR_MASK)


#endif
