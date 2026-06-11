/**
 * ****************************************************************************
 * @file slc_private_spi_frame.h
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

#ifndef __SLC_PRIVATE_SPI_FRAME_H__
#define __SLC_PRIVATE_SPI_FRAME_H__

#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#include "reg_Dev.h"
#include "error_def.h"
#include "utility.h"

#include "slc_hal_gpio.h"
#include "slc_hal_spi.h"

#define SLC_PRI_CS_PIN      (HAL_GPIO_PIN4)
#define SLC_PRI_CLK_PIN     (HAL_GPIO_PIN5)
#define SLC_PRI_MOSI_PIN    (HAL_GPIO_PIN6)
#define SLC_PRI_MISO_PIN    (HAL_GPIO_PIN7)
#define SLC_PRI_SPI_IOMUX   (HAL_IOMUX_MODE2)
#define SLC_PRI_SPI_GATE    (HAL_CLK_SPI1)

#define SLC_PRI_SPI         (HAL_SPI1)

enum slc_phy_spi_cmd_type {
    SLC_PHY_WRITE = 0x02,
    SLC_PHY_READ  = 0x03,
};

enum slc_rf_spi_cmd_type {
    SLC_RF_WRITE = 0x02,
    SLC_RF_READ  = 0x03,
    SLC_RF_DUMMY = 0x0B,
};

void slc_private_spi_init(void);

void slc_rf_spi_write32_cmd(uint16_t addr, uint32_t data);
uint32_t slc_rf_spi_read32_cmd(uint16_t addr);
uint32_t slc_rf_spi_read32_cmd_with_dummy(uint16_t addr); // for read 32bit data with dummy
void slc_rf_spi_set_bits(uint16_t addr, uint8_t start_bit, uint8_t end_bit, uint32_t val);
uint32_t slc_rf_spi_get_bits(uint16_t addr, uint8_t start_bit, uint8_t end_bit);

/* SPI 地址 = 相对 ADDR_PHY_RFE_BASE(0x40010000) 的 16bit 偏移，与 AHB 地址低 16 位一致 */
#define SLC_RF_SPI_AHB_ADDR(ahb_addr)       ((uint16_t)((ahb_addr) & 0xFFFFU))
#define SLC_RF_SPI_ADDR_PMU(reg_off)        SLC_RF_SPI_AHB_ADDR(ADDR_RF_PMU_BASE + (reg_off))
#define SLC_RF_SPI_ADDR_CTRL(reg_off)       SLC_RF_SPI_AHB_ADDR(ADDR_RF_CTRL_BASE + (reg_off))
#define SLC_RF_SPI_ADDR_AGC(reg_off)        SLC_RF_SPI_AHB_ADDR(ADDR_RF_AGC_BASE + (reg_off))
#define SLC_RF_SPI_ADDR_PLL(reg_off)        SLC_RF_SPI_AHB_ADDR(ADDR_RF_PLL_BASE + (reg_off))
#define SLC_RF_SPI_ADDR_DFE(reg_off)        SLC_RF_SPI_AHB_ADDR(ADDR_RF_DFE_BASE + (reg_off))

/* 等价于总线: reg &= ~mask; reg |= set_bits; */
void slc_rf_spi_reg_update(uint16_t addr, uint32_t clr_mask, uint32_t set_bits);
/* 等价于总线: reg &= ~mask; */
void slc_rf_spi_reg_clr_mask(uint16_t addr, uint32_t mask);
/* 等价于总线: reg |= mask; */
void slc_rf_spi_reg_or_mask(uint16_t addr, uint32_t mask);

void slc_phy_spi_write32_cmd(uint16_t addr, uint32_t data);
uint32_t slc_phy_spi_read32_cmd(uint16_t addr);
void slc_phy_spi_set_bits(uint16_t addr, uint8_t start_bit, uint8_t end_bit, uint32_t val);
uint32_t slc_phy_spi_get_bits(uint16_t addr, uint8_t start_bit, uint8_t end_bit);

#endif
