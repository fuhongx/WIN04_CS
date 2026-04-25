/**
 * ****************************************************************************
 * @file reg_iic.h
 * @author SLC(SC) software team.
 * @brief
 * @version  1.0
 * ****************************************************************************
 *
 * Copyright (c) 2025 SLC Semiconductor all rights reserved.
 *
 * ****************************************************************************
 */

#ifndef __REG_IIC_H__
#define __REG_IIC_H__

typedef struct
{
    __IOM uint32_t IIC_CON;                /*!<0x00:*/
    __IOM uint32_t IIC_TAR;                /*!<0x04:*/
    __IOM uint32_t IIC_SAR;                /*!<0x08:*/
    __IOM uint32_t RES1[1];                /*!<0x0C:*/
    __IOM uint32_t IIC_DATA_CMD;           /*!<0x10:*/
    __IOM uint32_t IIC_SS_SCL_HCNT;        /*!<0x14:*/
    __IOM uint32_t IIC_SS_SCL_LCNT;        /*!<0x18:*/
    __IOM uint32_t IIC_FS_SCL_HCNT;        /*!<0x1C:*/
    __IOM uint32_t IIC_FS_SCL_LCNT;        /*!<0x20:*/
    __IOM uint32_t RES2[2];                /*!<0x24 - 0x28*/
    __IM uint32_t IIC_INTR_STAT;           /*!<0x2C:*/
    __IOM uint32_t IIC_INTR_MASK;          /*!<0x30:*/
    __IM uint32_t IIC_RAW_INTR_STAT;       /*!<0x34:*/
    __IOM uint32_t IIC_RX_TL;              /*!<0x38:*/
    __IOM uint32_t IIC_TX_TL;              /*!<0x3C:*/
    __IM uint32_t IIC_CLR_INTR;            /*!<0x40:*/
    __IM uint32_t IIC_CLR_RX_UNDER;        /*!<0x44:*/
    __IM uint32_t IIC_CLR_RX_OVER;         /*!<0x48:*/
    __IM uint32_t IIC_CLR_TX_OVER;         /*!<0x4C:*/
    __IM uint32_t IIC_CLR_RD_REQ;          /*!<0x50:*/
    __IM uint32_t IIC_CLR_TX_ABRT;         /*!<0x54:*/
    __IM uint32_t IIC_CLR_RX_DONE;         /*!<0x58:*/
    __IM uint32_t IIC_CLR_ACTIVITY;        /*!<0x5C:*/
    __IM uint32_t IIC_CLR_STOP_DET;        /*!<0x60:*/
    __IM uint32_t IIC_CLR_START_DET;       /*!<0x64:*/
    __IM uint32_t IIC_CLR_GEN_CALL;        /*!<0x68:*/
    __IOM uint32_t IIC_ENABLE;             /*!<0x6C:*/
    __IOM uint32_t IIC_STATUS;             /*!<0x70:*/
    __IM uint32_t IIC_TXFLR;               /*!<0x74:*/
    __IM uint32_t IIC_RXFLR;               /*!<0x78:*/
    __IOM uint32_t IIC_SDA_HOLD;           /*!<0x7C:*/
    __IM uint32_t IIC_TX_ABRT_SOURCE;      /*!<0x80:*/
    __IOM uint32_t IIC_SLV_DATA_NACK_ONLY; /*!<0x84:*/
    __IOM uint32_t IIC_DMA_CR;             /*!<0x88:*/
    __IOM uint32_t IIC_DMA_TDLR;           /*!<0x8C:*/
    __IOM uint32_t IIC_DMA_RDLR;           /*!<0x90*/
    __IOM uint32_t IIC_SDA_SETUP;          /*!<0x94:*/
    __IOM uint32_t IIC_ACK_GENERAL_CALL;   /*!<0x98:*/
    __IM uint32_t IIC_ENABLE_STATUS;       /*!<0x9C:*/
    __IOM uint32_t IIC_FS_SPKLEN;          /*!<0xA0:*/
    __IOM uint32_t RES3[1];                /*!<0xA4:*/
    __IOM uint32_t IIC_CLR_RESTART_DET;    /*!<0xA8:*/
} stIicHandle_t, *pstIicHandle_t;

//------------------------------------------------------------------------------
// 0x0 IIC_CON
//------------------------------------------------------------------------------
// BIT[0]
#define IIC_CON_MASTER_MODE_EN_MASK (0x1)
#define IIC_CON_MASTER_MODE_EN_SHIFT (0)

// BIT[1:2]
#define IIC_CON_FAST_MODE_EN_MASK (0x3)
#define IIC_CON_FAST_MODE_EN_SHIFT (1)

// BIT[3]
#define IIC_CON_SLAVE_ADDR_LEN_MASK (0x1)
#define IIC_CON_SLAVE_ADDR_LEN_SHIFT (3)

// BIT[4]
#define IIC_CON_MASTER_ADDR_LEN_MASK (0x1)
#define IIC_CON_MASTER_ADDR_LEN_SHIFT (4)

// BIT[5]
#define IIC_CON_RESTART_EN_MASK (0x1)
#define IIC_CON_RESTART_EN_SHIFT (5)

// BIT[6]
#define IIC_CON_SLAVE_DISABLE_MASK (0x1)
#define IIC_CON_SLAVE_DISABLE_SHIFT (6)

// BIT[7]
#define IIC_CON_STOP_DET_MODE_CFG_MASK (0x1)
#define IIC_CON_STOP_DET_MODE_CFG_SHIFT (7)

// BIT[8]
#define IIC_CON_TX_EMPTY_INT_EN_MASK (0x1)
#define IIC_CON_TX_EMPTY_INT_EN_SHIFT (8)

// BIT[9]
#define IIC_CON_RX_FIFO_FULL_MODE_CFG_MASK (0x1)
#define IIC_CON_RX_FIFO_FULL_MODE_CFG_SHIFT (9)

// BIT[10]
#define IIC_CON_MASTER_ACTIVE_STOP_DET_EN_MASK (0x1)
#define IIC_CON_MASTER_ACTIVE_STOP_DET_EN_SHIFT (10)

#define IIC_CON_REG_MASK ((IIC_CON_MASTER_MODE_EN_MASK << IIC_CON_MASTER_MODE_EN_SHIFT) |               \
                          (IIC_CON_FAST_MODE_EN_MASK << IIC_CON_FAST_MODE_EN_SHIFT) |                   \
                          (IIC_CON_SLAVE_ADDR_LEN_MASK << IIC_CON_SLAVE_ADDR_LEN_SHIFT) |               \
                          (IIC_CON_MASTER_ADDR_LEN_MASK << IIC_CON_MASTER_ADDR_LEN_SHIFT) |             \
                          (IIC_CON_RESTART_EN_MASK << IIC_CON_RESTART_EN_SHIFT) |                       \
                          (IIC_CON_SLAVE_DISABLE_MASK << IIC_CON_SLAVE_DISABLE_SHIFT) |                 \
                          (IIC_CON_STOP_DET_MODE_CFG_MASK << IIC_CON_STOP_DET_MODE_CFG_SHIFT) |         \
                          (IIC_CON_TX_EMPTY_INT_EN_MASK << IIC_CON_TX_EMPTY_INT_EN_SHIFT) |             \
                          (IIC_CON_RX_FIFO_FULL_MODE_CFG_MASK << IIC_CON_RX_FIFO_FULL_MODE_CFG_SHIFT) | \
                          (IIC_CON_MASTER_ACTIVE_STOP_DET_EN_MASK << IIC_CON_MASTER_ACTIVE_STOP_DET_EN_SHIFT))

//------------------------------------------------------------------------------
// 0x04 IIC_TAR
//------------------------------------------------------------------------------
// BIT[0:9]
#define IIC_TAR_TARGET_SLAVE_ADDR_MASK (0x3FF)
#define IIC_TAR_TARGET_SLAVE_ADDR_SHIFT (0)

// BIT[10]
#define IIC_TAR_START_BYTE_MODE_MASK (0x1)
#define IIC_TAR_START_BYTE_MODE_SHIFT (10)

// BIT[11]
#define IIC_TAR_SPECIAL_START_BYTE_MASK (0x1)
#define IIC_TAR_SPECIAL_START_BYTE_SHIFT (11)

#define IIC_TAR_REG_MASK ((IIC_TAR_TARGET_SLAVE_ADDR_MASK << IIC_TAR_TARGET_SLAVE_ADDR_SHIFT) | \
                          (IIC_TAR_START_BYTE_MODE_MASK << IIC_TAR_START_BYTE_MODE_SHIFT) |     \
                          (IIC_TAR_SPECIAL_START_BYTE_MASK << IIC_TAR_SPECIAL_START_BYTE_SHIFT))

//------------------------------------------------------------------------------
// 0x08 IIC_SAR
//------------------------------------------------------------------------------
#define IIC_SAR_REG_MASK (0x3FF)

//------------------------------------------------------------------------------
// 0x10 IIC_DATA_CMD
//------------------------------------------------------------------------------
// BIT[0:7]
#define IIC_DATA_CMD_DATA_MASK (0xFF)
#define IIC_DATA_CMD_DATA_SHIFT (0)

// BIT[8]
#define IIC_DATA_CMD_READ_EN_MASK (0x1)
#define IIC_DATA_CMD_READ_EN_SHIFT (8)

// BIT[9]
#define IIC_DATA_CMD_STOP_TRANS_MASK (0x1)
#define IIC_DATA_CMD_STOP_TRANS_SHIFT (9)

// BIT[10]
#define IIC_DATA_CMD_RESTART_MASK (0x1)
#define IIC_DATA_CMD_RESTART_SHIFT (10)

#define IIC_DATA_CMD_REG_MASK ((IIC_DATA_CMD_DATA_MASK << IIC_DATA_CMD_DATA_SHIFT) |              \
                               (IIC_DATA_CMD_READ_EN_MASK << IIC_DATA_CMD_READ_EN_SHIFT) |        \
                               (IIC_DATA_CMD_STOP_TRANS_MASK << IIC_DATA_CMD_STOP_TRANS_SHIFTv) | \
                               (IIC_DATA_CMD_RESTART_MASK << IIC_DATA_CMD_RESTART_SHIFT))

//------------------------------------------------------------------------------
// 0x14 IIC_SS_SCL_HCNT
//------------------------------------------------------------------------------
#define IIC_SS_SCL_HCNT_REG_MASK (0xFFFF)

//------------------------------------------------------------------------------
// 0x18 IIC_SS_SCL_LCNT
//------------------------------------------------------------------------------
#define IIC_SS_SCL_LCNT_REG_MASK (0xFFFF)

//------------------------------------------------------------------------------
// 0x1C IIC_FS_SCL_HCNT
//------------------------------------------------------------------------------
#define IIC_FS_SCL_HCNT_REG_MASK (0xFFFF)

//------------------------------------------------------------------------------
// 0x20 IIC_FS_SCL_LCNT
//------------------------------------------------------------------------------
#define IIC_FS_SCL_LCNT_REG_MASK (0xFFFF)

//------------------------------------------------------------------------------
// 0x2C IIC_INTR_STAT
//------------------------------------------------------------------------------
#define IIC_INTR_MAX_INT_NUM (13)
// BIT[0]
#define IIC_INTR_STAT_RX_UNDER_FLOW_INT_MASK (0x1)
#define IIC_INTR_STAT_RX_UNDER_FLOW_INT_SHIFT (0)

// BIT[1]
#define IIC_INTR_STAT_RX_OVER_FLOW_INT_MASK (0x1)
#define IIC_INTR_STAT_RX_OVER_FLOW_INT_SHIFT (1)

// BIT[2]
#define IIC_INTR_STAT_RX_FIFO_FULL_INT_MASK (0x1)
#define IIC_INTR_STAT_RX_FIFO_FULL_INT_SHIFT (2)

// BIT[3]
#define IIC_INTR_STAT_TX_OVER_FLOW_INT_MASK (0x1)
#define IIC_INTR_STAT_TX_OVER_FLOW_INT_SHIFT (3)

// BIT[4]
#define IIC_INTR_STAT_TX_EMPTY_INT_MASK (0x1)
#define IIC_INTR_STAT_TX_EMPTY_INT_SHIFT (4)

// BIT[5]
#define IIC_INTR_STAT_READ_REQ_INT_MASK (0x1)
#define IIC_INTR_STAT_READ_REQ_INT_SHIFT (5)

// BIT[6]
#define IIC_INTR_STAT_TX_ABORT_INT_MASK (0x1)
#define IIC_INTR_STAT_TX_ABORT_INT_SHIFT (6)

// BIT[7]
#define IIC_INTR_STAT_RX_DONE_INT_MASK (0x1)
#define IIC_INTR_STAT_RX_DONE_INT_SHIFT (7)

// BIT[8]
#define IIC_INTR_STAT_ACTIVITY_INT_MASK (0x1)
#define IIC_INTR_STAT_ACTIVITY_INT_SHIFT (8)

// BIT[9]
#define IIC_INTR_STAT_STOP_DET_INT_MASK (0x1)
#define IIC_INTR_STAT_STOP_DET_INT_SHIFT (9)

// BIT[10]
#define IIC_INTR_STAT_START_DET_INT_MASK (0x1)
#define IIC_INTR_STAT_START_DET_INT_SHIFT (10)

// BIT[11]
#define IIC_INTR_STAT_CALL_DET_INT_MASK (0x1)
#define IIC_INTR_STAT_CALL_DET_INT_SHIFT (11)

// BIT[12]
#define IIC_INTR_STAT_RESTART_DET_INT_MASK (0x1)
#define IIC_INTR_STAT_RESTART_DET_INT_SHIFT (12)

#define IIC_INTR_STAT_REG_MASK ((IIC_INTR_STAT_RX_UNDER_FLOW_INT_MASK << IIC_INTR_STAT_RX_UNDER_FLOW_INT_SHIFT) | \
                                (IIC_INTR_STAT_RX_OVER_FLOW_INT_MASK << IIC_INTR_STAT_RX_OVER_FLOW_INT_SHIFT) |   \
                                (IIC_INTR_STAT_RX_FIFO_FULL_INT_MASK << IIC_INTR_STAT_RX_FIFO_FULL_INT_SHIFT) |   \
                                (IIC_INTR_STAT_TX_OVER_FLOW_INT_MASK << IIC_INTR_STAT_TX_OVER_FLOW_INT_SHIFT) |   \
                                (IIC_INTR_STAT_TX_EMPTY_INT_MASK << IIC_INTR_STAT_TX_EMPTY_INT_SHIFT) |           \
                                (IIC_INTR_STAT_READ_REQ_INT_MASK << IIC_INTR_STAT_READ_REQ_INT_SHIFT) |           \
                                (IIC_INTR_STAT_TX_ABORT_INT_MASK << IIC_INTR_STAT_TX_ABORT_INT_SHIFT) |           \
                                (IIC_INTR_STAT_RX_DONE_INT_MASK << IIC_INTR_STAT_RX_DONE_INT_SHIFT) |             \
                                (IIC_INTR_STAT_ACTIVITY_INT_MASK << IIC_INTR_STAT_ACTIVITY_INT_SHIFT) |           \
                                (IIC_INTR_STAT_STOP_DET_INT_MASK << IIC_INTR_STAT_STOP_DET_INT_SHIFT) |           \
                                (IIC_INTR_STAT_START_DET_INT_MASK << IIC_INTR_STAT_START_DET_INT_SHIFT) |         \
                                (IIC_INTR_STAT_CALL_DET_INT_MASK << IIC_INTR_STAT_CALL_DET_INT_SHIFT) |           \
                                (IIC_INTR_STAT_RESTART_DET_INT_MASK << IIC_INTR_STAT_RESTART_DET_INT_SHIFT))

//------------------------------------------------------------------------------
// 0x30 IIC_INTR_MASK
//------------------------------------------------------------------------------
#define IIC_INTR_MASK_REG_MASK (IIC_INTR_STAT_REG_MASK)

//------------------------------------------------------------------------------
// 0x34 IIC_RAW_INTR_STAT
//------------------------------------------------------------------------------
#define IIC_RAW_INTR_STAT_REG_MASK (IIC_INTR_STAT_REG_MASK)

//------------------------------------------------------------------------------
// 0x38 IIC_RX_TL
//------------------------------------------------------------------------------
#define IIC_RX_TL_REG_MASK (0xFF)

//------------------------------------------------------------------------------
// 0x3C IIC_TX_TL
//------------------------------------------------------------------------------
#define IIC_TX_TL_REG_MASK (0xFF)

//------------------------------------------------------------------------------
// 0x40 IIC_CLR_INTR ~ 0x68 IIC_CLR_GEN_CALL
//------------------------------------------------------------------------------
#define IIC_INTR_CLR_REG_MASK (0x1)

//------------------------------------------------------------------------------
// 0x6C IIC_ENABLE
//------------------------------------------------------------------------------
// BIT[0]
#define IIC_ENABLE_EN_MASK (0x1)
#define IIC_ENABLE_EN_SHIFT (0)

// BIT[1]
#define IIC_ENABLE_ABORT_MASK (0x1)
#define IIC_ENABLE_ABORT_SHIFT (1)

#define IIC_ENABLE_REG_MASK ((IIC_ENABLE_EN_MASK << IIC_ENABLE_EN_SHIFT) | \
                             (IIC_ENABLE_ABORT_MASK << IIC_ENABLE_ABORT_SHIFT))

//------------------------------------------------------------------------------
// 0x70 IIC_STATUS
//------------------------------------------------------------------------------
// BIT[0]
#define IIC_STATUS_ACTIVITY_MASK (0x1)
#define IIC_STATUS_ACTIVITY_SHIFT (0)

// BIT[1]
#define IIC_STATUS_TX_FIFO_NOT_FULL_MASK (0x1)
#define IIC_STATUS_TX_FIFO_NOT_FULL_SHIFT (1)

// BIT[2]
#define IIC_STATUS_TX_FIFO_EMPTY_MASK (0x1)
#define IIC_STATUS_TX_FIFO_EMPTY_SHIFT (2)

// BIT[3]
#define IIC_STATUS_RX_FIFO_NOT_EMPTY_MASK (0x1)
#define IIC_STATUS_RX_FIFO_NOT_EMPTY_SHIFT (3)

// BIT[4]
#define IIC_STATUS_RX_FIFO_FULL_MASK (0x1)
#define IIC_STATUS_RX_FIFO_FULL_SHIFT (4)

// BIT[5]
#define IIC_STATUS_MASTER_BUSY_MASK (0x1)
#define IIC_STATUS_MASTER_BUSY_SHIFT (5)

// BIT[6]
#define IIC_STATUS_SLAVE_BUSY_MASK (0x1)
#define IIC_STATUS_SLAVE_BUSY_SHIFT (6)

#define IIC_STATUS_REG_MASK ((IIC_STATUS_ACTIVITY_MASK << IIC_STATUS_ACTIVITY_SHIFT) |                   \
                             (IIC_STATUS_TX_FIFO_NOT_FULL_MASK << IIC_STATUS_TX_FIFO_NOT_FULL_SHIFT) |   \
                             (IIC_STATUS_TX_FIFO_EMPTY_MASK << IIC_STATUS_TX_FIFO_EMPTY_SHIFT) |         \
                             (IIC_STATUS_RX_FIFO_NOT_EMPTY_MASK << IIC_STATUS_RX_FIFO_NOT_EMPTY_SHIFT) | \
                             (IIC_STATUS_RX_FIFO_FULL_MASK << IIC_STATUS_RX_FIFO_FULL_SHIFT) |           \
                             (IIC_STATUS_MASTER_BUSY_MASK << IIC_STATUS_MASTER_BUSY_SHIFT) |             \
                             (IIC_STATUS_SLAVE_BUSY_MASK << IIC_STATUS_SLAVE_BUSY_SHIFT))

//------------------------------------------------------------------------------
// 0x74 IIC_TXFLR
//------------------------------------------------------------------------------
#define IIC_TXFLR_REG_MASK (0xF)

//------------------------------------------------------------------------------
// 0x78 IIC_RXFLR
//------------------------------------------------------------------------------
#define IIC_RXFLR_REG_MASK (0xF)

//------------------------------------------------------------------------------
// 0x7C IIC_SDA_HOLD, unit apb0 sys clk
//------------------------------------------------------------------------------
// BIT[0:15]
#define IIC_SDA_HOLD_TX_SDA_HOLD_TIME_MASK (0xFFFF)
#define IIC_SDA_HOLD_TX_SDA_HOLD_TIME_SHIFT (0)

// BIT[16:23]
#define IIC_SDA_HOLD_RX_SDA_HOLD_TIME_MASK (0xFF)
#define IIC_SDA_HOLD_RX_SDA_HOLD_TIME_SHIFT (16)

#define IIC_SDA_HOLD_REG_MASK ((IIC_SDA_HOLD_TX_SDA_HOLD_TIME_MASK << IIC_SDA_HOLD_TX_SDA_HOLD_TIME_SHIFT) | \
                               (IIC_SDA_HOLD_RX_SDA_HOLD_TIME_MASK << IIC_SDA_HOLD_RX_SDA_HOLD_TIME_SHIFT))

//------------------------------------------------------------------------------
// 0x80 IIC_TX_ABRT_SOURCE
//------------------------------------------------------------------------------
// BIT[0]
#define IIC_TX_ABRT_SOURCE_7BIT_ADDR_NO_ACK_MASK (0x1)
#define IIC_TX_ABRT_SOURCE_7BIT_ADDR_NO_ACK_SHIFT (0)

// BIT[1]
#define IIC_TX_ABRT_SOURCE_10BIT_ADDR1_NO_ACK_MASK (0x1)
#define IIC_TX_ABRT_SOURCE_10BIT_ADDR1_NO_ACK_SHIFT (1)

// BIT[2]
#define IIC_TX_ABRT_SOURCE_10BIT_ADDR2_NO_ACK_MASK (0x1)
#define IIC_TX_ABRT_SOURCE_10BIT_ADDR2_NO_ACK_SHIFT (2)

// BIT[3]
#define IIC_TX_ABRT_SOURCE_TX_DATA_NO_ACK_MASK (0x1)
#define IIC_TX_ABRT_SOURCE_TX_DATA_NO_ACK_SHIFT (3)

// BIT[4]
#define IIC_TX_ABRT_SOURCE_CALL_NO_ACK_MASK (0x1)
#define IIC_TX_ABRT_SOURCE_CALL_NO_ACK_SHIFT (4)

// BIT[5]
#define IIC_TX_ABRT_SOURCE_CALL_READ_MASK (0x1)
#define IIC_TX_ABRT_SOURCE_CALL_READ_SHIFT (5)

// BIT[6]
#define IIC_TX_ABRT_SOURCE_HISPEED_ACK_DET_MASK (0x1)
#define IIC_TX_ABRT_SOURCE_HISPEED_ACK_DET_SHIFT (6)

// BIT[7]
#define IIC_TX_ABRT_SOURCE_START_ACK_MASK (0x1)
#define IIC_TX_ABRT_SOURCE_START_ACK_SHIFT (7)

// BIT[8]
#define IIC_TX_ABRT_SOURCE_HISPEED_RESTART_ERROR_MASK (0x1)
#define IIC_TX_ABRT_SOURCE_HISPEED_RESTART_ERROR_SHIFT (8)

// BIT[9]
#define IIC_TX_ABRT_SOURCE_RESTART_ERROR_MASK (0x1)
#define IIC_TX_ABRT_SOURCE_RESTART_ERROR_SHIFT (9)

// BIT[10]
#define IIC_TX_ABRT_SOURCE_10BIT_READ_RESTART_ERROR_MASK (0x1)
#define IIC_TX_ABRT_SOURCE_10BIT_READ_RESTART_ERROR_SHIFT (10)

// BIT[11]
#define IIC_TX_ABRT_SOURCE_MASTER_DISABLE_MASK (0x1)
#define IIC_TX_ABRT_SOURCE_MASTER_DISABLE_SHIFT (11)

// BIT[12]
#define IIC_TX_ABRT_SOURCE_ARB_LOST_MASK (0x1)
#define IIC_TX_ABRT_SOURCE_ARB_LOST_SHIFT (12)

// BIT[13]
#define IIC_TX_ABRT_SOURCE_SLAVE_TX_FIFO_EMPTY_MASK (0x1)
#define IIC_TX_ABRT_SOURCE_SLAVE_TX_FIFO_EMPTY_SHIFT (13)

// BIT[14]
#define IIC_TX_ABRT_SOURCE_SLAVE_BUS_ERROR_MASK (0x1)
#define IIC_TX_ABRT_SOURCE_SLAVE_BUS_ERROR_SHIFT (14)

// BIT[15]
#define IIC_TX_ABRT_SOURCE_SLAVE_READ_CMD_ERROR_MASK (0x1)
#define IIC_TX_ABRT_SOURCE_SLAVE_READ_CMD_ERROR_SHIFT (15)

// BIT[16]
#define IIC_TX_ABRT_SOURCE_USER_CLOSE_MASK (0x1)
#define IIC_TX_ABRT_SOURCE_USER_CLOSE_SHIFT (16)

// BIT[23:31]
#define IIC_TX_ABRT_SOURCE_TX_FLUSH_CNT_MASK (0xFF)
#define IIC_TX_ABRT_SOURCE_TX_FLUSH_CNT_SHIFT (23)

#define IIC_TX_ABRT_SOURCE_REG_MASK ((IIC_TX_ABRT_SOURCE_7BIT_ADDR_NO_ACK_MASK << IIC_TX_ABRT_SOURCE_7BIT_ADDR_NO_ACK_SHIFT) |                 \
                                     (IIC_TX_ABRT_SOURCE_10BIT_ADDR1_NO_ACK_MASK << IIC_TX_ABRT_SOURCE_10BIT_ADDR1_NO_ACK_SHIFT) |             \
                                     (IIC_TX_ABRT_SOURCE_10BIT_ADDR2_NO_ACK_MASK << IIC_TX_ABRT_SOURCE_10BIT_ADDR2_NO_ACK_SHIFT) |             \
                                     (IIC_TX_ABRT_SOURCE_TX_DATA_NO_ACK_MASK << IIC_TX_ABRT_SOURCE_TX_DATA_NO_ACK_SHIFT) |                     \
                                     (IIC_TX_ABRT_SOURCE_CALL_NO_ACK_MASK << IIC_TX_ABRT_SOURCE_CALL_NO_ACK_SHIFT) |                           \
                                     (IIC_TX_ABRT_SOURCE_CALL_READ_MASK << IIC_TX_ABRT_SOURCE_CALL_READ_SHIFT) |                               \
                                     (IIC_TX_ABRT_SOURCE_HISPEED_ACK_DET_MASK << IIC_TX_ABRT_SOURCE_HISPEED_ACK_DET_SHIFT) |                   \
                                     (IIC_TX_ABRT_SOURCE_START_ACK_MASK << IIC_TX_ABRT_SOURCE_START_ACK_SHIFT) |                               \
                                     (IIC_TX_ABRT_SOURCE_HISPEED_RESTART_ERROR_MASK << IIC_TX_ABRT_SOURCE_HISPEED_RESTART_ERROR_SHIFT) |       \
                                     (IIC_TX_ABRT_SOURCE_RESTART_ERROR_MASK << IIC_TX_ABRT_SOURCE_RESTART_ERROR_SHIFT) |                       \
                                     (IIC_TX_ABRT_SOURCE_10BIT_READ_RESTART_ERROR_MASK << IIC_TX_ABRT_SOURCE_10BIT_READ_RESTART_ERROR_SHIFT) | \
                                     (IIC_TX_ABRT_SOURCE_MASTER_DISABLE_MASK << IIC_TX_ABRT_SOURCE_MASTER_DISABLE_SHIFT) |                     \
                                     (IIC_TX_ABRT_SOURCE_ARB_LOST_MASK << IIC_TX_ABRT_SOURCE_ARB_LOST_SHIFT) |                                 \
                                     (IIC_TX_ABRT_SOURCE_SLAVE_TX_FIFO_EMPTY_MASK << IIC_TX_ABRT_SOURCE_SLAVE_TX_FIFO_EMPTY_SHIFT) |           \
                                     (IIC_TX_ABRT_SOURCE_SLAVE_BUS_ERROR_MASK << IIC_TX_ABRT_SOURCE_SLAVE_BUS_ERROR_SHIFT) |                   \
                                     (IIC_TX_ABRT_SOURCE_SLAVE_READ_CMD_ERROR_MASK << IIC_TX_ABRT_SOURCE_SLAVE_READ_CMD_ERROR_SHIFT) |         \
                                     (IIC_TX_ABRT_SOURCE_USER_CLOSE_MASK << IIC_TX_ABRT_SOURCE_USER_CLOSE_SHIFT) |                             \
                                     (IIC_TX_ABRT_SOURCE_TX_FLUSH_CNT_MASK << IIC_TX_ABRT_SOURCE_TX_FLUSH_CNT_SHIFT))

//------------------------------------------------------------------------------
// 0x84 IIC_SLV_DATA_NACK_ONLY
//------------------------------------------------------------------------------
#define IIC_SLV_DATA_NACK_ONLY_REG_MASK (0x1)

//------------------------------------------------------------------------------
// 0x88 IIC_DMA_CR
//------------------------------------------------------------------------------
// BIT[0]
#define IIC_DMA_RX_DMA_EN_MASK  (0x1)
#define IIC_DMA_RX_DMA_EN_SHIFT (0)

// BIT[1]
#define IIC_DMA_TX_DMA_EN_MASK  (0x1)
#define IIC_DMA_TX_DMA_EN_SHIFT (1)

#define IIC_DMA_CR_REG_MASK ((IIC_DMA_RX_DMA_EN_MASK << IIC_DMA_RX_DMA_EN_SHIFT) | \
                             (IIC_DMA_TX_DMA_EN_MASK << IIC_DMA_TX_DMA_EN_SHIFT))

//------------------------------------------------------------------------------
// 0x8C IIC_DMA_TDLR
//------------------------------------------------------------------------------
#define IIC_DMA_TDLR_REG_MASK           (0xF)

//------------------------------------------------------------------------------
// 0x90 IIC_DMA_RDLR
//------------------------------------------------------------------------------
#define IIC_DMA_RDLR_REG_MASK           (0xF)

//------------------------------------------------------------------------------
// 0x94 IIC_SDA_SETUP
//------------------------------------------------------------------------------
#define IIC_SDA_SETUP_REG_MASK          (0xFF)

//------------------------------------------------------------------------------
// 0x98 IIC_ACK_GENERAL_CALL
//------------------------------------------------------------------------------
#define IIC_ACK_GENERAL_CALL_REG_MASK   (0x1)

//------------------------------------------------------------------------------
// 0x9C IIC_ENABLE_STATUS
//------------------------------------------------------------------------------
// BIT[0]
#define IIC_ENABLE_STATUS_IIC_EN_MASK           (0x1)
#define IIC_ENABLE_STATUS_IIC_EN_SHIFT          (0)

// BIT[1]
#define IIC_ENABLE_STATUS_SLAVE_DISABLE_MASK    (0x1)
#define IIC_ENABLE_STATUS_SLAVE_DISABLE_SHIFT   (1)

#define IIC_ENABLE_STATUS_SLAVE_RX_LOST_MASK    (0x1)
#define IIC_ENABLE_STATUS_SLAVE_RX_LOST_SHIFT   (2)

#define IIC_ENABLE_STATUS_REG_MASK ((IIC_ENABLE_STATUS_IIC_EN_MASK << IIC_ENABLE_STATUS_IIC_EN_SHIFT) |               \
                                    (IIC_ENABLE_STATUS_SLAVE_DISABLE_MASK << IIC_ENABLE_STATUS_SLAVE_DISABLE_SHIFT) | \
                                    (IIC_ENABLE_STATUS_SLAVE_RX_LOST_MASK << IIC_ENABLE_STATUS_SLAVE_RX_LOST_SHIFT))

//------------------------------------------------------------------------------
// 0xa0 IIC_FS_SPKLEN
//------------------------------------------------------------------------------
#define IIC_FS_SPKLEN_REG_MASK          (0xFF)

//------------------------------------------------------------------------------
// 0xa8 IIC_CLR_RESTART_DET
//------------------------------------------------------------------------------
#define IIC_CLR_RESTART_DET_REG_MASK    (0x1)

#endif
