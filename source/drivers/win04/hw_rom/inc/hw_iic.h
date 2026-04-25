/**
 * ****************************************************************************
 * @file hw_iic.h
 * @author SLC(SC) software team.
 * @brief
 * @version  1.0
 * ****************************************************************************
 *
 * Copyright (c) 2025 SLC Semiconductor all rights reserved.
 *
 * ****************************************************************************
 */

#ifndef __HW_IIC_H__
#define __HW_IIC_H__

#define IIC_WAIT_TIMEOUT    (1000*1000)
#define IIC_MAX_FIFO_DEPTH  (8)

typedef enum
{
    EN_IIC_MODE_SLAVE = 0x0,
    EN_IIC_MODE_MASTER = 0x1,
} EN_IIC_MODE_T;

typedef enum
{
    EN_IIC_SPEED_STD_100K = 0x1,
    EN_IIC_SPEED_FAST_400K = 0x2,
    EN_IIC_SPEED_FAST_PLUS_1M = 0x3,
} EN_IIC_SPEED_T;

typedef enum
{
    EN_IIC_ADDR_LEN_7BIT = 0x0,
    EN_IIC_ADDR_LEN_10BIT = 0x1,
} EN_IIC_ADDR_LEN_T;

typedef struct
{
    EN_IIC_MODE_T enMode;
    EN_IIC_SPEED_T enSpeed;
    EN_IIC_ADDR_LEN_T enTargetAddrLen;
    uint16_t u16TargetAddr;
    EN_IIC_ADDR_LEN_T enOwnSlaveAddrLen;
    uint16_t u16OwnSlaveAddr;
    uint8_t u8TxTL;
    uint8_t u8RxTL;
} stIicInt_t;

typedef enum
{
    EN_IIC_INT_RX_UNDER_FLOW = (IIC_INTR_STAT_RX_UNDER_FLOW_INT_MASK << IIC_INTR_STAT_RX_UNDER_FLOW_INT_SHIFT),
    EN_IIC_INT_RX_OVER_FLOW = (IIC_INTR_STAT_RX_OVER_FLOW_INT_MASK << IIC_INTR_STAT_RX_OVER_FLOW_INT_SHIFT),
    EN_IIC_INT_RX_FIFO_FULL = (IIC_INTR_STAT_RX_FIFO_FULL_INT_MASK << IIC_INTR_STAT_RX_FIFO_FULL_INT_SHIFT),
    EN_IIC_INT_TX_OVER_FLOW = (IIC_INTR_STAT_TX_OVER_FLOW_INT_MASK << IIC_INTR_STAT_TX_OVER_FLOW_INT_SHIFT),
    EN_IIC_INT_TX_EMPTY = (IIC_INTR_STAT_TX_EMPTY_INT_MASK << IIC_INTR_STAT_TX_EMPTY_INT_SHIFT),
    EN_IIC_INT_READ_REQ = (IIC_INTR_STAT_READ_REQ_INT_MASK << IIC_INTR_STAT_READ_REQ_INT_SHIFT),
    EN_IIC_INT_TX_ABORT = (IIC_INTR_STAT_TX_ABORT_INT_MASK << IIC_INTR_STAT_TX_ABORT_INT_SHIFT),
    EN_IIC_INT_RX_DONE = (IIC_INTR_STAT_RX_DONE_INT_MASK << IIC_INTR_STAT_RX_DONE_INT_SHIFT),
    EN_IIC_INT_ACTIVITY = (IIC_INTR_STAT_ACTIVITY_INT_MASK << IIC_INTR_STAT_ACTIVITY_INT_SHIFT),
    EN_IIC_INT_STOP_DET = (IIC_INTR_STAT_STOP_DET_INT_MASK << IIC_INTR_STAT_STOP_DET_INT_SHIFT),
    EN_IIC_INT_START_DET = (IIC_INTR_STAT_START_DET_INT_MASK << IIC_INTR_STAT_START_DET_INT_SHIFT),
    EN_IIC_INT_CALL_DET = (IIC_INTR_STAT_CALL_DET_INT_MASK << IIC_INTR_STAT_CALL_DET_INT_SHIFT),
    EN_IIC_INT_RESTART_DET = (IIC_INTR_STAT_RESTART_DET_INT_MASK << IIC_INTR_STAT_RESTART_DET_INT_SHIFT),
} EN_IIC_INT_T;

typedef enum
{
    EN_IIC_START_BYTE_CALL = ((0x0 & IIC_TAR_START_BYTE_MODE_MASK) << IIC_TAR_START_BYTE_MODE_SHIFT) | ((0x0 & IIC_TAR_SPECIAL_START_BYTE_MASK) << IIC_TAR_SPECIAL_START_BYTE_SHIFT),
    EN_IIC_START_BYTE_NORMAL = ((0x1 & IIC_TAR_START_BYTE_MODE_MASK) << IIC_TAR_START_BYTE_MODE_SHIFT) | ((0x0 & IIC_TAR_SPECIAL_START_BYTE_MASK) << IIC_TAR_SPECIAL_START_BYTE_SHIFT),
    EN_IIC_START_BYTE_GC_OR_START = ((0x1 & IIC_TAR_START_BYTE_MODE_MASK) << IIC_TAR_START_BYTE_MODE_SHIFT) | ((0x1 & IIC_TAR_SPECIAL_START_BYTE_MASK) << IIC_TAR_SPECIAL_START_BYTE_SHIFT),
} EN_IIC_START_BYTE_T;

typedef enum
{
    EN_IIC_LEVEL_CNT_STAND_SPEED_HI_LEVEL = 0,
    EN_IIC_LEVEL_CNT_STAND_SPEED_LO_LEVEL = 1,
    EN_IIC_LEVEL_CNT_FAST_SPEED_HI_LEVEL = 2,
    EN_IIC_LEVEL_CNT_FAST_SPEED_LO_LEVEL = 3,
} EN_IIC_LEVLE_CNT_T;

typedef enum
{
    EN_IIC_STA_ACTIVITY = IIC_STATUS_ACTIVITY_MASK << IIC_STATUS_ACTIVITY_SHIFT,
    EN_IIC_STA_TX_FIFO_NOT_FULL = IIC_STATUS_TX_FIFO_NOT_FULL_MASK << IIC_STATUS_TX_FIFO_NOT_FULL_SHIFT,
    EN_IIC_STA_TX_FIFO_EMPTY = IIC_STATUS_TX_FIFO_EMPTY_MASK << IIC_STATUS_TX_FIFO_EMPTY_SHIFT,
    EN_IIC_STA_RX_FIFO_NOT_EMPTY = IIC_STATUS_RX_FIFO_NOT_EMPTY_MASK << IIC_STATUS_RX_FIFO_NOT_EMPTY_SHIFT,
    EN_IIC_STA_RX_FIFO_FULL = IIC_STATUS_RX_FIFO_FULL_MASK << IIC_STATUS_RX_FIFO_FULL_SHIFT,
    EN_IIC_STA_MASTER_ACTIVITY = IIC_STATUS_MASTER_BUSY_MASK << IIC_STATUS_MASTER_BUSY_SHIFT,
    EN_IIC_STA_SLAVE_ACTIVITY = IIC_STATUS_SLAVE_BUSY_MASK << IIC_STATUS_SLAVE_BUSY_SHIFT,
} EN_IIC_STA_T;

typedef enum
{
    EN_IIC_TX_ABORT_7BIT_ADDR_NO_ACK = IIC_TX_ABRT_SOURCE_7BIT_ADDR_NO_ACK_MASK << IIC_TX_ABRT_SOURCE_7BIT_ADDR_NO_ACK_SHIFT,
    EN_IIC_TX_ABORT_10BIT_ADDR1_NO_ACK = IIC_TX_ABRT_SOURCE_10BIT_ADDR1_NO_ACK_MASK << IIC_TX_ABRT_SOURCE_10BIT_ADDR1_NO_ACK_SHIFT,
    EN_IIC_TX_ABORT_10BIT_ADDR2_NO_ACK = IIC_TX_ABRT_SOURCE_10BIT_ADDR2_NO_ACK_MASK << IIC_TX_ABRT_SOURCE_10BIT_ADDR2_NO_ACK_SHIFT,
    EN_IIC_TX_ABORT_TX_DATA_NO_ACK = IIC_TX_ABRT_SOURCE_TX_DATA_NO_ACK_MASK << IIC_TX_ABRT_SOURCE_TX_DATA_NO_ACK_SHIFT,
    EN_IIC_TX_ABORT_CALL_NO_ACK = IIC_TX_ABRT_SOURCE_CALL_NO_ACK_MASK << IIC_TX_ABRT_SOURCE_CALL_NO_ACK_SHIFT,
    EN_IIC_TX_ABORT_CALL_READ = IIC_TX_ABRT_SOURCE_CALL_READ_MASK << IIC_TX_ABRT_SOURCE_CALL_READ_SHIFT,
    EN_IIC_TX_ABORT_HI_SPEED_ACK_DET = IIC_TX_ABRT_SOURCE_HISPEED_ACK_DET_MASK << IIC_TX_ABRT_SOURCE_HISPEED_ACK_DET_SHIFT,
    EN_IIC_TX_ABORT_START_ACK = IIC_TX_ABRT_SOURCE_START_ACK_MASK << IIC_TX_ABRT_SOURCE_START_ACK_SHIFT,
    EN_IIC_TX_ABORT_HI_SPEED_NO_RESTART = IIC_TX_ABRT_SOURCE_HISPEED_RESTART_ERROR_MASK << IIC_TX_ABRT_SOURCE_HISPEED_RESTART_ERROR_SHIFT,
    EN_IIC_TX_ABORT_10BIT_READ_NO_RESTART = IIC_TX_ABRT_SOURCE_10BIT_READ_RESTART_ERROR_MASK << IIC_TX_ABRT_SOURCE_10BIT_READ_RESTART_ERROR_SHIFT,
    EN_IIC_TX_ABORT_MASTER_DISABLE = IIC_TX_ABRT_SOURCE_MASTER_DISABLE_MASK << IIC_TX_ABRT_SOURCE_MASTER_DISABLE_SHIFT,
    EN_IIC_TX_ABORT_ARB_LOST = IIC_TX_ABRT_SOURCE_ARB_LOST_MASK << IIC_TX_ABRT_SOURCE_ARB_LOST_SHIFT,
    EN_IIC_TX_ABORT_SLAVE_TX_FIFO_EMPTY = IIC_TX_ABRT_SOURCE_SLAVE_TX_FIFO_EMPTY_MASK << IIC_TX_ABRT_SOURCE_SLAVE_TX_FIFO_EMPTY_SHIFT,
    EN_IIC_TX_ABORT_SLAVE_BUS_LOST = IIC_TX_ABRT_SOURCE_SLAVE_BUS_ERROR_MASK << IIC_TX_ABRT_SOURCE_SLAVE_BUS_ERROR_SHIFT,
    EN_IIC_TX_ABORT_SLAVE_READ_CMD_ERROR = IIC_TX_ABRT_SOURCE_SLAVE_READ_CMD_ERROR_MASK << IIC_TX_ABRT_SOURCE_SLAVE_READ_CMD_ERROR_SHIFT,
    EN_IIC_TX_ABORT_USER_ABORT = IIC_TX_ABRT_SOURCE_USER_CLOSE_MASK << IIC_TX_ABRT_SOURCE_USER_CLOSE_SHIFT,
} EN_IIC_TX_ABORT_T;

typedef enum
{
    EN_IIC_DMA_TRX_DISABLE = 0,
    EN_IIC_DMA_RX_ENABLE = 1,
    EN_IIC_DMA_TX_ENABLE = 2,
    EN_IIC_DMA_TRX_ENABLE = 3,
} EN_IIC_DMA_TRX_T;

/**
 * @brief Config iic mode and speed.
 *
 * @param pstHandle Must be IIC0/IIC1.
 * @param enMode @ref EN_IIC_MODE_T.
 * @param enSpeed @ref EN_IIC_SPEED_T.
 * @return EN_ERR_STA_T
 */
EN_ERR_STA_T rom_hw_iic_mode_config(stIicHandle_t *pstHandle, EN_IIC_MODE_T enMode, EN_IIC_SPEED_T enSpeed);

/**
 * @brief Configure the address of the slave, which can also be configured in host mode.
 *
 * @param pstHandle Must be IIC0/IIC1.
 * @param u16Addr The configuration value is either 7bit or 10bit.
 * @param enAddrLen @ref EN_IIC_ADDR_LEN_T.
 * @return EN_ERR_STA_T
 */
EN_ERR_STA_T rom_hw_iic_set_slave_addr(stIicHandle_t *pstHandle, uint16_t u16Addr, EN_IIC_ADDR_LEN_T enAddrLen);

/**
 * @brief Configure the destination address.
 *
 * @param pstHandle Must be IIC0/IIC1.
 * @param u16Addr The configuration value is either 7bit or 10bit.
 * @param enAddrLen @ref EN_IIC_ADDR_LEN_T.
 * @return EN_ERR_STA_T
 */
EN_ERR_STA_T rom_hw_iic_set_target_addr(stIicHandle_t *pstHandle, uint16_t u16Addr, EN_IIC_ADDR_LEN_T enAddrLen);

/**
 * @brief Enable IIC interrupts.
 *
 * @param pstHandle Must be IIC0/IIC1.
 * @param u16Mask IIC interrupts that need to be enabled, @ref EN_IIC_INT_T.
 * @return EN_ERR_STA_T
 */
EN_ERR_STA_T rom_hw_iic_enable_interrupt(stIicHandle_t *pstHandle, uint16_t u16Mask);

/**
 * @brief Disabled IIC interrupt.
 *
 * @param pstHandle Must be IIC0/IIC1.
 * @param u16Mask IIC interrupts that need to be disabled, @ref EN_IIC_INT_T.
 * @return EN_ERR_STA_T
 */
EN_ERR_STA_T rom_hw_iic_disable_interrupt(stIicHandle_t *pstHandle, uint16_t u16Mask);

/**
 * @brief Gets the IIC interrupt flag.
 *
 * @param pstHandle Must be IIC0/IIC1.
 * @param pu16Flag Pointer to acquire the IIC interrupt flag Buffer. @ref EN_IIC_INT_T.
 * @return EN_ERR_STA_T
 */
EN_ERR_STA_T rom_hw_iic_get_interrupt_flag(stIicHandle_t *pstHandle, uint16_t *pu16Flag);

/**
 * @brief Clear interrupt flags.
 *
 * @param pstHandle Must be IIC0/IIC1.
 * @param u16Flag The interrupt flags needs to be cleared. @ref EN_IIC_INT_T.
 * @return EN_ERR_STA_T
 */
EN_ERR_STA_T rom_hw_iic_clear_interrupt_flag(stIicHandle_t *pstHandle, uint16_t u16Flag);

/**
 * @brief Configure the IIC TRX FIFO threshold.
 *
 * @param pstHandle Must be IIC0/IIC1.
 * @param u8TxThld TX FIFO threshold.
 * @param u8RxThld RX FIFO threshold
 * @return EN_ERR_STA_T
 */
EN_ERR_STA_T rom_hw_iic_set_fifo_threshold(stIicHandle_t *pstHandle, uint8_t u8TxThld, uint8_t u8RxThld);

/**
 * @brief Enable the IIC module.
 *
 * @param pstHandle Must be IIC0/IIC1.
 * @param bEnable true: enable, false : disable.
 * @return EN_ERR_STA_T
 */
EN_ERR_STA_T rom_hw_iic_enable(stIicHandle_t *pstHandle, bool bEnable);

/**
 * @brief Get the IIC status.
 *
 * @param pstHandle Must be IIC0/IIC1.
 * @param pu8Sta A pointer to the state of the stored IIC. @ref EN_IIC_STA_T.
 * @return EN_ERR_STA_T
 */
EN_ERR_STA_T rom_hw_iic_get_status(stIicHandle_t *pstHandle, uint8_t *pu8Sta);

/**
 * @brief Enable IIC TRX DMA.
 *
 * @param pstHandle Must be IIC0/IIC1.
 * @param enMode @ref EN_IIC_DMA_TRX_T.
 * @return EN_ERR_STA_T
 */
EN_ERR_STA_T rom_hw_iic_enable_dma_transmit(stIicHandle_t *pstHandle, EN_IIC_DMA_TRX_T enMode);

/**
 * @brief Configure the IIC TRX DMA threshold.
 *
 * @param pstHandle Must be IIC0/IIC1.
 * @param u8TxThld TX DMA threshold.
 * @param u8RxThld RX DMA threshold.
 * @return EN_ERR_STA_T
 */
EN_ERR_STA_T rom_hw_iic_set_dma_trx_threshold(stIicHandle_t *pstHandle, uint8_t u8TxThld, uint8_t u8RxThld);

/**
 * @brief IIC master send.
 * 
 * @param pstHandle Must be IIC0/IIC1.
 * @param u8Buf Data buffer.
 * @param u16Len Data length.
 * @param u32Timeout Timeout value.
 * @return EN_ERR_STA_T
 */
EN_ERR_STA_T rom_hw_iic_master_tx(stIicHandle_t *pstHandle, uint8_t *u8Buf, uint16_t u16Len, uint32_t u32Timeout);

/**
 * @brief IIC master receive.
 * 
 * @param pstHandle Must be IIC0/IIC1.
 * @param u8Buf Data buffer.
 * @param u16Len Data length pointer. input: max length, output: real length.
 * @param u32Timeout Timeout value.
 * @return EN_ERR_STA_T
 */
EN_ERR_STA_T rom_hw_iic_master_rx(stIicHandle_t *pstHandle, uint8_t *u8Buf, uint16_t *u16Len, uint32_t u32Timeout);

/**
 * @brief IIC slave send.
 * 
 * @param pstHandle Must be IIC0/IIC1.
 * @param u8Buf Data buffer.
 * @param u16Len Data length.
 * @param u32Timeout Timeout value.
 * @return EN_ERR_STA_T
 */
EN_ERR_STA_T rom_hw_iic_slave_tx(stIicHandle_t *pstHandle, uint8_t *u8Buf, uint16_t u16Len, uint32_t u32Timeout);

/**
 * @brief IIC slave receive.
 * 
 * @param pstHandle Must be IIC0/IIC1.
 * @param u8Buf Data buffer.
 * @param u16Len Data length pointer. input: max length, output: real length.
 * @param u32Timeout Timeout value.
 * @return EN_ERR_STA_T
 */
EN_ERR_STA_T rom_hw_iic_slave_rx(stIicHandle_t *pstHandle, uint8_t *u8Buf, uint16_t *u16Len, uint32_t u32Timeout);

/**
 * @brief Init iic module.
 *
 * @param pstHandle Must be IIC0/IIC1.
 * @param pstInit @ref stIicInt_t.
 * @return EN_ERR_STA_T
 */
EN_ERR_STA_T rom_hw_iic_init(stIicHandle_t *pstHandle, stIicInt_t *pstInit);

#endif
