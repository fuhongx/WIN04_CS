/**
 * ****************************************************************************
 * @file hw_spi.h
 * @author SLC(SC) software team.
 * @brief
 * @version  1.0
 * ****************************************************************************
 *
 * Copyright (c) 2024 SLC Semiconductor all rights reserved.
 *
 * ****************************************************************************
 */

#ifndef __HW_SPI_H__
#define __HW_SPI_H__

typedef enum
{
    EN_SPI_INT_TX_BUFFER_OVERFLOW       = SPI_IMR_INT_TX_BUFFER_OVERFLOW_MASK << SPI_IMR_INT_TX_BUFFER_OVERFLOW_SHIFT,
    EN_SPI_INT_TX_BUFFER_EMPTY          = SPI_IMR_INT_TX_BUFFER_EMPTY_MASK << SPI_IMR_INT_TX_BUFFER_EMPTY_SHIFT,
    EN_SPI_INT_TX_FINISH                = SPI_IMR_INT_TX_FINISH_MASK << SPI_IMR_INT_TX_FINISH_SHIFT,
    EN_SPI_INT_RX_ERROR                 = SPI_IMR_INT_RX_ERROR_MASK << SPI_IMR_INT_RX_ERROR_SHIFT,
    EN_SPI_INT_RX_BUFFER_OVERFLOW       = SPI_IMR_INT_RX_BUFFER_OVERFLOW_MASK << SPI_IMR_INT_RX_BUFFER_OVERFLOW_SHIFT,
    EN_SPI_INT_RX_FINISH                = SPI_IMR_INT_RX_FINISH_MASK << SPI_IMR_INT_RX_FINISH_SHIFT,
    EN_SPI_INT_TRANS_DONE               = SPI_IMR_INT_SPI_TRANS_END_MASK << SPI_IMR_INT_SPI_TRANS_END_SHIFT,
    EN_SPI_INT_TX_EMPTY_ERROR           = SPI_IMR_INT_TX_BUFFER_EMPTY_ERR_MASK << SPI_IMR_INT_TX_BUFFER_EMPTY_SHIFT,
}EN_SPI_INT_T;

typedef enum
{
    EN_SPI_STA_RX_NEW_DATA              = SPI_TSR_RX_NEW_DATA_MASK<< SPI_TSR_RX_NEW_DATA_SHIFT,
    EN_SPI_STA_RX_BUFFER_OVERFLOW       = SPI_TSR_RX_DATA_OVERFLOW_MASK << SPI_TSR_RX_DATA_OVERFLOW_SHIFT,
    EN_SPI_STA_RX_DATA_ERROR            = SPI_TSR_RX_DATA_ERROR_MASK << SPI_TSR_RX_DATA_ERROR_SHIFT,
    EN_SPI_STA_TX_BUFFER_EMPTY          = SPI_TSR_TX_BUFFER_EMPTY_MASK << SPI_TSR_TX_BUFFER_EMPTY_SHIFT,
    EN_SPI_STA_BUSY                     = SPI_TSR_SPI_BUSY_MASK << SPI_TSR_SPI_BUSY_SHIFT,
}EN_SPI_STA_T;

typedef enum
{
    EN_SPI_CS_LEVEL_LO      =   0,
    EN_SPI_CS_LEVEL_HI      =   1,
}EN_SPI_CS_LEVEL_T;

typedef enum
{
    EN_SPI_CLK_DIV4     =   0,
    EN_SPI_CLK_DIV8     =   1,
    EN_SPI_CLK_DIV10    =   2,
    EN_SPI_CLK_DIV16    =   3,
    EN_SPI_CLK_DIV20    =   4,
    EN_SPI_CLK_DIV32    =   5,
    EN_SPI_CLK_DIV64    =   6,
    EN_SPI_CLK_DIV100   =   7,
    EN_SPI_CLK_DIV200   =   8,
    EN_SPI_CLK_DIV400   =   9,
    EN_SPI_CLK_DIV_MAX,
}EN_SPI_CLK_DIV_T;

typedef enum
{
    EN_SPI_CLK_CPOL0_CPHA0      = SPI_CLKMOD_CPOL0_CPHA0,
    EN_SPI_CLK_CPOL1_CPHA0      = SPI_CLKMOD_CPOL1_CPHA0,
    EN_SPI_CLK_CPOL0_CPHA1      = SPI_CLKMOD_CPOL0_CPHA1,
    EN_SPI_CLK_CPOL1_CPHA1      = SPI_CLKMOD_CPOL1_CPHA1,
}EN_SPI_CLK_POLARITY_PHASE_T;


typedef enum
{
    EN_SPI_MODE_SLAVE   = 0,
    EN_SPI_MODE_MASTER  = 1,
}EN_SPI_MODE_T;

typedef enum
{
    EN_SPI_CLK_ADJUST_DISABLE   = 0,
    EN_SPI_CLK_ADJUST_ENABLE    = 1,
}EN_SPI_CLK_ADJUST_EN_T;

typedef enum
{
    EN_SPI_DATA_MODE_MSB        = 0,
    EN_SPI_DATA_MODE_LSB        = 1,
}EN_SPI_DATA_MODE_T;

typedef enum
{
    EN_SPI_DATA_LEN_8BIT        = 0,
    EN_SPI_DATA_LEN_16BIT       = 1,
    EN_SPI_DATA_LEN_32BIT       = 2,
    EN_SPI_DATA_LEN_MAX,
}EN_SPI_DATA_LEN_T;

typedef enum
{
    EN_SPI_DMA_TX_TIMING_LAST_BIT   = 0,
    EN_SPI_DMA_TX_TIMING_FIRST_BIT  = 1,
}EN_SPI_DMA_TX_TIMING_T;

typedef union
{
    struct
    {
        uint32_t ClkDiv         : 4;  /** BIT 0-3  @ref EN_SPI_CLK_DIV_T*/
        uint32_t MasterMode     : 1;  /** BIT 4  @ref EN_SPI_MODE_T*/
        uint32_t ClkAdjustEn    : 1;  /** BIT 5  @ref EN_SPI_CLK_ADJUST_EN_T*/
        uint32_t DataMode       : 1;  /** BIT 6  @ref EN_SPI_DATA_MODE_T*/
        uint32_t DataLen        : 2;  /** BIT 7-8 @ref EN_SPI_DATA_LEN_T*/
        uint32_t Res            : 1;  /** BIT 9*/
        uint32_t TxDmaEn        : 1;  /** BIT 10*/
        uint32_t RxDmaEn        : 1;  /** BIT 11*/
        uint32_t SwCsEn         : 1;  /** BIT 12*/
        uint32_t AntiNoiseLevel : 1;  /** BIT 13*/
        uint32_t TxEmptyCheck   : 1;  /** BIT 14*/
        uint32_t TxDmaTiming    : 1;  /** BIT 15 @ref EN_SPI_DMA_TX_TIMING_T*/
        uint32_t Res2            		: 16;
    }stSpiInit;
    uint32_t u32Cfg;
}unSpiInit_t;

EN_ERR_STA_T rom_hw_spi_init(stSpiHandle_t *pstHandle, unSpiInit_t unInit);
EN_ERR_STA_T rom_hw_spi_get_status(stSpiHandle_t *pstHandle, uint8_t *pu8Flag);
EN_ERR_STA_T rom_hw_spi_clear_status(stSpiHandle_t *pstHandle, uint8_t u8Flag);
EN_ERR_STA_T rom_hw_spi_set_cs_pin_level(stSpiHandle_t *pstHandle, EN_SPI_CS_LEVEL_T enLevel);
EN_ERR_STA_T rom_hw_spi_master_transmit_receive(stSpiHandle_t *pstHandle, void *tx_buf, void *rx_buf, uint32_t len, uint32_t timeout_us);
EN_ERR_STA_T rom_hw_spi_slave_transmit(stSpiHandle_t *pstHandle, void *tx_buf, uint32_t len, uint32_t timeout_us);
EN_ERR_STA_T rom_hw_spi_slave_receive(stSpiHandle_t *pstHandle, void *rx_buf, uint32_t *len, uint32_t timeout_us);
EN_ERR_STA_T rom_hw_spi_set_interrupt_mask(stSpiHandle_t *pstHandle, uint8_t u8Mask);
EN_ERR_STA_T rom_hw_spi_enable_interrupt(stSpiHandle_t *pstHandle, uint8_t u8Mask, bool bEnable);
EN_ERR_STA_T rom_hw_spi_get_interrupt_flag(stSpiHandle_t *pstHandle, uint8_t *pu8Mask);
EN_ERR_STA_T rom_hw_spi_clear_interrupt_flag(stSpiHandle_t *pstHandle, uint8_t u8Mask);
EN_ERR_STA_T rom_hw_spi_enable_anti_noise(stSpiHandle_t *pstHandle, bool bEnable);
EN_ERR_STA_T rom_hw_spi_set_dma_timeout(stSpiHandle_t *pstHandle, uint32_t u32Cnt);
EN_ERR_STA_T rom_hw_spi_set_cs_min_holding_time(stSpiHandle_t *pstHandle, uint32_t u32Time);
EN_ERR_STA_T rom_hw_spi_set_clock_polarity_phase(stSpiHandle_t *pstHandle, EN_SPI_CLK_POLARITY_PHASE_T enMode);
void rom_hw_spi_dma_mode(stSpiHandle_t *pstHandle, EN_SPI_DMA_TX_TIMING_T mode);

#endif
