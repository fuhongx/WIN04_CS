/**
 * ****************************************************************************
 * @file hw_dma.h
 * @author SLC(SC) software team.
 * @brief
 * @version  1.0
 * ****************************************************************************
 *
 * Copyright (c) 2024 SLC Semiconductor all rights reserved.
 *
 * ****************************************************************************
 */

#ifndef __HW_DMA_H__
#define __HW_DMA_H__

#define DMA_CH_STA_MASK(ch)     (0xF << (ch*4))
#define DMA_ABORT_TIMEOUT_US    (5000)

#define DMA_CH_STA_IDLE         (0x1 << 0)
#define DMA_CH_STA_READ_SRC     (0x1 << 1)
#define DMA_CH_STA_WRITE_DST    (0x1 << 2)
#define DMA_CH_STA_DONE         (0x1 << 3)

typedef enum
{
    EN_DMA_DATA_TYPE_BYTE = 0x0,
    EN_DMA_DATA_TYPE_HALF_WORD = 0x1,
    EN_DMA_DATA_TYPE_WORD = 0x2,
} EN_DMA_DATA_TYPE_T;

typedef enum
{
    EN_DMA_REQ_DISABLE   = 0x0, // 5'b00000
    EN_DMA_REQ_UART0_RX  = 0x1, // 5'b00001
    EN_DMA_REQ_UART0_TX  = 0x2, // 5'b00010
    EN_DMA_REQ_UART1_RX  = 0x3, // 5'b00011
    EN_DMA_REQ_UART1_TX  = 0x4, // 5'b00100
    EN_DMA_REQ_SPI0_RX   = 0x5, // 5'b00101
    EN_DMA_REQ_SPI0_TX   = 0x6, // 5'b00110
    EN_DMA_REQ_LPUART_RX = 0x7, // 5'b00111
    EN_DMA_REQ_LPUART_TX = 0x8, // 5'b01000
    EN_DMA_REQ_SPI1_RX   = 0x9, // 5'b01001
    EN_DMA_REQ_SPI1_TX   = 0xA, // 5'b01010
    EN_DMA_REQ_I2C0_RX   = 0xB, // 5'b01011
    EN_DMA_REQ_I2C0_TX   = 0xC, // 5'b01100
    EN_DMA_REQ_I2C1_RX   = 0xD, // 5'b01101
    EN_DMA_REQ_I2C1_TX   = 0xE, // 5'b01110
} EN_DMA_REQ_T;

typedef enum
{
    EN_DMA_CH0 = 0,
    EN_DMA_CH1 = 1,
    EN_DMA_CH2 = 2,
    EN_DMA_CH3 = 3,
    EN_DMA_CH4 = 4,
    EN_DMA_CH5 = 5,
    EN_DMA_CH6 = 6,
    EN_DMA_CH7 = 7,
} EN_DMA_CHANNEL_T;

typedef union
{
    struct
    {
        uint32_t ErrorIntEn : 1;    /* BIT 0, CHx config error int(suggest to set 0) */
        uint32_t FinishIntEn : 1;   /* BIT 1, CHx transfer done int */
        uint32_t SrcAddrRiseEn : 1; /* BIT 2 */
        uint32_t DstAddrRiseEn : 1; /* BIT 3 */
        uint32_t SrcDataType : 2;   /* BIT 4-5 */
        uint32_t DstDataType : 2;   /* BIT 6-7 */
        uint32_t MutiTrigEn : 1;    /* BIT 8 */
        uint32_t TimeoutEn : 1;     /* BIT 9 */
        uint32_t Res : 6;           /* BIT 10-15 */
        uint32_t TimeoutVal : 16;   /* BIT 16-31, actual = TimeoutVal*16*(ahb_clk) */
    } stDMAChCfg;
    uint32_t u32Cfg;
} unDMAChCfg_t;

typedef struct
{
    EN_DMA_CHANNEL_T enDmaCh;
    unDMAChCfg_t unChCfg;
    EN_DMA_REQ_T enReqType;
    uint32_t u32SrcAddr;
    uint32_t u32DstAddr;
    uint16_t u16PackByte;   // The length of single package. The unit is byte
    uint16_t u16TrigNum;    // 1: single transfer, other: multi transfer
    bool bSoftReqEn;        // true: soft. false: peripherals.
    bool bHiPriority;       // true: high. false: low.
} stDMAInit_t;

typedef enum
{
    EN_DMA_INT_CH0_TRANS_DONE = 0x1 << DMA_INT_CH0_DONE_SHIFT,
    EN_DMA_INT_CH1_TRANS_DONE = 0x1 << DMA_INT_CH1_DONE_SHIFT,
    EN_DMA_INT_CH2_TRANS_DONE = 0x1 << DMA_INT_CH2_DONE_SHIFT,
    EN_DMA_INT_CH3_TRANS_DONE = 0x1 << DMA_INT_CH3_DONE_SHIFT,
    EN_DMA_INT_CH4_TRANS_DONE = 0x1 << DMA_INT_CH4_DONE_SHIFT,
    EN_DMA_INT_CH5_TRANS_DONE = 0x1 << DMA_INT_CH5_DONE_SHIFT,
    EN_DMA_INT_CH6_TRANS_DONE = 0x1 << DMA_INT_CH6_DONE_SHIFT,
    EN_DMA_INT_CH7_TRANS_DONE = 0x1 << DMA_INT_CH7_DONE_SHIFT,

    EN_DMA_INT_CH0_CFG_ERR = 0x1 << DMA_INT_CH0_CFG_ERR_SHIFT,
    EN_DMA_INT_CH1_CFG_ERR = 0x1 << DMA_INT_CH1_CFG_ERR_SHIFT,
    EN_DMA_INT_CH2_CFG_ERR = 0x1 << DMA_INT_CH2_CFG_ERR_SHIFT,
    EN_DMA_INT_CH3_CFG_ERR = 0x1 << DMA_INT_CH3_CFG_ERR_SHIFT,
    EN_DMA_INT_CH4_CFG_ERR = 0x1 << DMA_INT_CH4_CFG_ERR_SHIFT,
    EN_DMA_INT_CH5_CFG_ERR = 0x1 << DMA_INT_CH5_CFG_ERR_SHIFT,
    EN_DMA_INT_CH6_CFG_ERR = 0x1 << DMA_INT_CH6_CFG_ERR_SHIFT,
    EN_DMA_INT_CH7_CFG_ERR = 0x1 << DMA_INT_CH7_CFG_ERR_SHIFT,

    EN_DMA_INT_FALT = 0x1 << DMA_INT_FALT_SHIFT,
} EN_DMA_INT_T;

typedef struct
{
    uint32_t src_addr;
    uint32_t dst_addr;
    uint32_t single_pkg_size;
    uint32_t muli_trigger_num;
    bool src_addr_rise;
    bool dst_addr_rise;
} EN_DMA_CFG_T;

#define DMA_INIT_UART_RX_DEFAULT (((DMA_CTRL_CFG_ERR_STOP_EN_MASK) << DMA_CTRL_CFG_ERR_STOP_EN_SHIFT) |                               \
                                  (DMA_CTRL_CFG_FINISH_INT_EN_MASK << DMA_CTRL_CFG_FINISH_INT_EN_SHIFT) |                           \
                                  ((0x0 & DMA_CTRL_CFG_SRC_ADDR_RISE_EN_MASK) << DMA_CTRL_CFG_SRC_ADDR_RISE_EN_SHIFT) |             \
                                  (DMA_CTRL_CFG_DST_ADDR_RISE_EN_MASK << DMA_CTRL_CFG_DST_ADDR_RISE_EN_SHIFT) |                     \
                                  ((EN_DMA_DATA_TYPE_BYTE & DMA_CTRL_CFG_SRC_DATA_TYPE_MASK) << DMA_CTRL_CFG_SRC_DATA_TYPE_SHIFT) | \
                                  ((EN_DMA_DATA_TYPE_BYTE & DMA_CTRL_CFG_DST_DATA_TYPE_MASK) << DMA_CTRL_CFG_DST_DATA_TYPE_SHIFT) | \
                                  (DMA_CTRL_CFG_MUTI_TRIG_EN_MASK << DMA_CTRL_CFG_MUTI_TRIG_EN_SHIFT))

EN_ERR_STA_T rom_hw_dma_transfer_cfg(EN_DMA_CHANNEL_T ch, EN_DMA_CFG_T *cfg);

/**
 * @brief Initialize the DMA channel configuration. note: this function will not enable the channel.
 *
 * @param pstInit @ref stDMAInit_t.
 * @return EN_ERR_STA_T
 */
EN_ERR_STA_T rom_hw_dma_channel_init(stDMAInit_t *pstInit);

/**
 * @brief Enable DMA Channel.
 *
 * @param u8Channel @ref EN_DMA_CHANNEL_T.
 * @param bEnable true: enable. false: disable.
 * @return EN_ERR_STA_T
 */
EN_ERR_STA_T rom_hw_dma_enable_channel(EN_DMA_CHANNEL_T u8Channel, bool bEnable);

/**
 * @brief Gets the number of transfers completed.
 *
 * @param u8Channel @ref EN_DMA_CHANNEL_T.
 * @param ByteNum CHx transmitted data that has been completed. The unit is byte.
 * @return EN_ERR_STA_T
 */
EN_ERR_STA_T rom_hw_dma_channel_get_finish_num(EN_DMA_CHANNEL_T u8Channel, uint16_t *ByteNum);

/**
 * @brief Clear the DMA interrupt flag.
 *
 * @param u32Flag @ref EN_DMA_INT_T.
 * @return EN_ERR_STA_T
 */
EN_ERR_STA_T rom_hw_dma_clear_interrupt_flag(uint32_t u32Flag);

/**
 * @brief Gets the DMA interrupt flag.
 *
 * @param pu32Flag Pointer to Buffer. @ref EN_DMA_INT_T.
 * @return EN_ERR_STA_T
 */
EN_ERR_STA_T rom_hw_dma_get_interrupt_flag(uint32_t *pu32Flag);

#endif
