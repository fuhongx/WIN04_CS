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

#define TX_FIFO_DEPTH   16
#define RX_FIFO_DEPTH   16

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
        uint32_t Res0           : 3;  /** BIT 7-9*/
        uint32_t TxDmaEn        : 1;  /** BIT 10*/
        uint32_t RxDmaEn        : 1;  /** BIT 11*/
        uint32_t SwCsEn         : 1;  /** BIT 12*/
        uint32_t snoise_mode    : 1;  /** BIT 13*/
        uint32_t Res1           : 2;  /** BIT 14-15*/
        uint32_t cpol           : 1;  /** BIT 16*/
        uint32_t cpha           : 1;  /** BIT 17*/
        uint32_t snoise_en      : 1;  /** BIT 18*/
        uint32_t Res2           : 13;
    } stSpiInit;
    uint32_t u32Cfg;
}unSpiInit_t;

EN_ERR_STA_T rom_hw_spi_init(stSpiHandle_t *pstHandle, unSpiInit_t unInit);
EN_ERR_STA_T rom_hw_spi_get_status(stSpiHandle_t *pstHandle, uint32_t *pu32Flag);
EN_ERR_STA_T rom_hw_spi_set_cs_pin_level(stSpiHandle_t *pstHandle, EN_SPI_CS_LEVEL_T enLevel);
EN_ERR_STA_T rom_hw_spi_master_transmit_receive(stSpiHandle_t *pstHandle, void *tx_buf, void *rx_buf, uint32_t len, uint32_t timeout_us);
EN_ERR_STA_T rom_hw_spi_slave_transmit(stSpiHandle_t *pstHandle, void *tx_buf, uint32_t len, uint32_t timeout_us);
EN_ERR_STA_T rom_hw_spi_slave_receive(stSpiHandle_t *pstHandle, void *rx_buf, uint32_t *len, uint32_t timeout_us);
EN_ERR_STA_T rom_hw_spi_set_interrupt_mask(stSpiHandle_t *pstHandle, uint32_t u32Mask);
EN_ERR_STA_T rom_hw_spi_enable_interrupt(stSpiHandle_t *pstHandle, uint32_t u32Mask, bool bEnable);
EN_ERR_STA_T rom_hw_spi_get_interrupt_flag(stSpiHandle_t *pstHandle, uint32_t *pu32Mask);
EN_ERR_STA_T rom_hw_spi_clear_interrupt_flag(stSpiHandle_t *pstHandle, uint32_t u32Mask);
EN_ERR_STA_T rom_hw_spi_set_hold_gap_time(stSpiHandle_t *pstHandle, uint32_t hold_cycles, uint32_t gap_cycles);
EN_ERR_STA_T rom_hw_spi_set_clock_polarity_phase(stSpiHandle_t *pstHandle, bool bCPOL, bool bCPHA);
void rom_hw_spi_set_fifo_threshold(stSpiHandle_t *pstHandle,
                                    uint32_t tx_empty_th, uint32_t rx_empty_th,
                                    uint32_t tx_th, uint32_t rx_th);

#endif
