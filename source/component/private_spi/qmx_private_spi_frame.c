/**
 * ****************************************************************************
 * @file qmx_private_spi_frame.c
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
#include "qmx_private_spi_frame.h"
#include "qmx_hal_sysctrl.h"

void qmx_private_spi_init(void)
{
    qmx_hal_sysctrl_peripheral_clk_enable(HAL_CLK_GPIO, true);
    qmx_hal_gpio_set_iomux(QMX_PRI_CS_PIN, QMX_PRI_SPI_IOMUX);
    qmx_hal_gpio_set_iomux(QMX_PRI_CLK_PIN, QMX_PRI_SPI_IOMUX);
    qmx_hal_gpio_set_iomux(QMX_PRI_MOSI_PIN, QMX_PRI_SPI_IOMUX);
    qmx_hal_gpio_set_iomux(QMX_PRI_MISO_PIN, QMX_PRI_SPI_IOMUX);
    qmx_hal_gpio_set_mode(QMX_PRI_CS_PIN, HAL_GPIO_PULL_UP);
    qmx_hal_gpio_set_mode(QMX_PRI_CLK_PIN, HAL_GPIO_PULL_UP);
    qmx_hal_gpio_set_mode(QMX_PRI_MOSI_PIN, HAL_GPIO_PULL_UP);
    qmx_hal_gpio_set_mode(QMX_PRI_MISO_PIN, HAL_GPIO_PULL_UP);

    qmx_hal_sysctrl_peripheral_clk_enable(QMX_PRI_SPI_GATE, true);
    qmx_hal_sysctrl_peripheral_mod_reset(QMX_PRI_SPI_GATE);

    // phy的SPI最大只支持3.125M，RF最大支持25M
    hal_spi_cfg_t config = {0};
    config.mode = HAL_SPI_MASTER;
    config.clk_div = HAL_SPI_DIV_100;
    config.data_mode = HAL_SPI_DATA_MSB;
    config.polarity_phase = HAL_SPI_CPOL0_CPHA0;
    config.data_len = HAL_SPI_DATA_LEN_8BIT;
    config.cs_holding_time = 0x100;
    config.clk_adjust_en = true;
    config.sw_cs_en = true;
    config.anti_noise_level = 0;

    qmx_hal_spi_init(QMX_PRI_SPI, &config);
}

void qmx_rf_spi_write32_cmd(uint16_t addr, uint32_t data)
{
    uint8_t tx_buf[7] = {0};
    uint8_t rx_buf[7] = {0};

    // SUBG default is big endian
    tx_buf[0] = QMX_RF_WRITE;
    tx_buf[1] = (addr >> 8) & 0xFF;
    tx_buf[2] = addr & 0xFF;
    tx_buf[3] = (data >> 24) & 0xFF;
    tx_buf[4] = (data >> 16) & 0xFF;
    tx_buf[5] = (data >> 8) & 0xFF;
    tx_buf[6] = data & 0xFF;

    qmx_hal_spi_set_sw_cs(QMX_PRI_SPI, HAL_SPI_CS_LO);
    qmx_hal_spi_master_trx(QMX_PRI_SPI, tx_buf, rx_buf, 7, HAL_SPI_TIMEOUT_US);
    qmx_hal_spi_set_sw_cs(QMX_PRI_SPI, HAL_SPI_CS_HI);
}

uint32_t qmx_rf_spi_read32_cmd(uint16_t addr)
{
    uint8_t tx_buf[7] = {0};
    uint8_t rx_buf[7] = {0};
    uint32_t rddata = 0;

    // SUBG default is big endian
    tx_buf[0] = QMX_RF_READ;
    tx_buf[1] = (addr >> 8) & 0xFF;
    tx_buf[2]  = addr & 0xFF;

    qmx_hal_spi_set_sw_cs(QMX_PRI_SPI, HAL_SPI_CS_LO);
    qmx_hal_spi_master_trx(QMX_PRI_SPI, tx_buf, rx_buf, 7, HAL_SPI_TIMEOUT_US);
    qmx_hal_spi_set_sw_cs(QMX_PRI_SPI, HAL_SPI_CS_HI);

    rddata = ((uint32_t)rx_buf[3] << 24) |
             ((uint32_t)rx_buf[4] << 16) |
             ((uint32_t)rx_buf[5] << 8)  |
             ((uint32_t)rx_buf[6]);

    return rddata;
}

void qmx_rf_spi_set_bits(uint16_t addr, uint8_t start_bit, uint8_t end_bit, uint32_t val)
{
    uint32_t mask = ((1U << (end_bit - start_bit + 1U)) - 1U) << start_bit;
    uint32_t reg = qmx_rf_spi_read32_cmd(addr);
    reg = (reg & ~mask) | ((val << start_bit) & mask);
    qmx_rf_spi_write32_cmd(addr, reg);
}

uint32_t qmx_rf_spi_get_bits(uint16_t addr, uint8_t start_bit, uint8_t end_bit)
{
    uint32_t mask = (1U << (end_bit - start_bit + 1U)) - 1U;
    uint32_t reg = qmx_rf_spi_read32_cmd(addr);
    return (reg >> start_bit) & mask;
}

void qmx_phy_spi_write32_cmd(uint16_t addr, uint32_t data)
{
    uint8_t tx_buf[8] = {0};
    uint8_t rx_buf[8] = {0};

    // SUBG default is big endian
    tx_buf[0] = QMX_PHY_WRITE;
    tx_buf[1] = (addr >> 8) & 0xFF;
    tx_buf[2] = addr & 0xFF;
    tx_buf[3] = 1; // length, 传输多少个4字节
    tx_buf[4] = (data >> 24) & 0xFF;
    tx_buf[5] = (data >> 16) & 0xFF;
    tx_buf[6] = (data >> 8) & 0xFF;
    tx_buf[7] = data & 0xFF;

    qmx_hal_spi_set_sw_cs(QMX_PRI_SPI, HAL_SPI_CS_LO);
    qmx_hal_spi_master_trx(QMX_PRI_SPI, tx_buf, rx_buf, 8, HAL_SPI_TIMEOUT_US);
    qmx_hal_spi_set_sw_cs(QMX_PRI_SPI, HAL_SPI_CS_HI);
}

uint32_t qmx_phy_spi_read32_cmd(uint16_t addr)
{
    uint8_t tx_buf[8] = {0};
    uint8_t rx_buf[8] = {0};
    uint32_t rddata = 0;

    // SUBG default is big endian
    tx_buf[0] = QMX_PHY_READ;
    tx_buf[1] = (addr >> 8) & 0xFF;
    tx_buf[2]  = addr & 0xFF;
    tx_buf[3] = 1; // length, 传输多少个4字节

    qmx_hal_spi_set_sw_cs(QMX_PRI_SPI, HAL_SPI_CS_LO);
    qmx_hal_spi_master_trx(QMX_PRI_SPI, tx_buf, rx_buf, 8, HAL_SPI_TIMEOUT_US);
    qmx_hal_spi_set_sw_cs(QMX_PRI_SPI, HAL_SPI_CS_HI);

    rddata = ((uint32_t)rx_buf[4] << 24) |
             ((uint32_t)rx_buf[5] << 16) |
             ((uint32_t)rx_buf[6] << 8)  |
             ((uint32_t)rx_buf[7]);

    return rddata;
}

void qmx_phy_spi_set_bits(uint16_t addr, uint8_t start_bit, uint8_t end_bit, uint32_t val)
{
    uint32_t mask = ((1U << (end_bit - start_bit + 1U)) - 1U) << start_bit;
    uint32_t reg = qmx_phy_spi_read32_cmd(addr);
    reg = (reg & ~mask) | ((val << start_bit) & mask);
    qmx_phy_spi_write32_cmd(addr, reg);
}

uint32_t qmx_phy_spi_get_bits(uint16_t addr, uint8_t start_bit, uint8_t end_bit)
{
    uint32_t mask = (1U << (end_bit - start_bit + 1U)) - 1U;
    uint32_t reg = qmx_phy_spi_read32_cmd(addr);
    return (reg >> start_bit) & mask;
}
