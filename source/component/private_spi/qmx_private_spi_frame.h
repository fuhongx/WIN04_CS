/**
 * ****************************************************************************
 * @file qmx_private_spi_frame.h
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

#ifndef __QMX_PRIVATE_SPI_FRAME_H__
#define __QMX_PRIVATE_SPI_FRAME_H__

#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#include "reg_Dev.h"
#include "error_def.h"
#include "utility.h"

#include "qmx_hal_gpio.h"
#include "qmx_hal_spi.h"

#define QMX_PRI_CS_PIN      (HAL_GPIO_PIN4)
#define QMX_PRI_CLK_PIN     (HAL_GPIO_PIN5)
#define QMX_PRI_MOSI_PIN    (HAL_GPIO_PIN6)
#define QMX_PRI_MISO_PIN    (HAL_GPIO_PIN7)
#define QMX_PRI_SPI_IOMUX   (HAL_IOMUX_MODE2)
#define QMX_PRI_SPI_GATE    (HAL_CLK_SPI1)

#define QMX_PRI_SPI         (HAL_SPI1)

enum qmx_phy_spi_cmd_type {
    QMX_PHY_WRITE = 0x02,
    QMX_PHY_READ  = 0x03,
};

enum qmx_rf_spi_cmd_type {
    QMX_RF_WRITE = 0x02,
    QMX_RF_READ  = 0x03,
    QMX_RF_DUMMY = 0x0B,
};

void qmx_private_spi_init(void);

void qmx_rf_spi_write32_cmd(uint16_t addr, uint32_t data);
uint32_t qmx_rf_spi_read32_cmd(uint16_t addr);
void qmx_rf_spi_set_bits(uint16_t addr, uint8_t start_bit, uint8_t end_bit, uint32_t val);
uint32_t qmx_rf_spi_get_bits(uint16_t addr, uint8_t start_bit, uint8_t end_bit);

void qmx_phy_spi_write32_cmd(uint16_t addr, uint32_t data);
uint32_t qmx_phy_spi_read32_cmd(uint16_t addr);
void qmx_phy_spi_set_bits(uint16_t addr, uint8_t start_bit, uint8_t end_bit, uint32_t val);
uint32_t qmx_phy_spi_get_bits(uint16_t addr, uint8_t start_bit, uint8_t end_bit);

#endif
