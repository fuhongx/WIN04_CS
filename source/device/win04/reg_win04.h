/**
 * ****************************************************************************
 * @file reg_win04.h
 * @author SLC(SC) software team.
 * @brief
 * @version  1.0
 * ****************************************************************************
 *
 * Copyright (c) 2024 SLC Semiconductor all rights reserved.
 *
 * ****************************************************************************
 */

#ifndef __REG_WIN04_H__
#define __REG_WIN04_H__

#ifdef __cplusplus
extern "C"
{
#endif

// Cortex-M Define
#include "ARMCM0plus.h"

// Peripheral Register Define
#include "win04_mem_cfg.h"

#include "reg_phy.h"
#include "reg_rf.h"

#include "reg_sys_ctrl.h" //AHB Base

#include "reg_crc.h"
#include "reg_rand.h"
#include "reg_aes.h" // AHB

#include "reg_uart.h"       //APB0 + 0x0
#include "reg_lpuart.h"     //APB1 + 0x1000
#include "reg_flash_ctrl.h" //APB0 + 0x2000
#include "reg_timer.h"      //APB0 + 0x3000
#include "reg_gpio.h"       //APB0 + 0x7000
#include "reg_spi.h"        //APB0 + 0x8000
#include "reg_wdt.h"        //APB0 + 0xA000

#include "reg_dma.h" //APB0 + 0xB000
#include "reg_iic.h" //APB0 + 0xC000
#include "reg_ila.h" //APB0 + 0xE000

#include "reg_rtc.h" //APB2 + 0x0

#include "reg_pmu.h"
#include "reg_lpio.h"

#define UNIT_KB             (1024)
#define UNIT_WORD_WIDE      (0xFFFFFFFF)
#define UNIT_HALF_WORD_WIDE (0xFFFF)
#define UNIT_BYTE_WIDE      (0xFF)

// Addr base in mcu
#define ADDR_ROM_BASE       (0x0)
#define ADDR_FLASH_BASE     (0x08000000)
#define ADDR_SRAM_BASE      (0x20000000)

// AHB MUX
#define ADDR_SYS_CTRL_BASE      (0x40000000)
#define ADDR_PHY_RFE_BASE       (0x40010000)
#define ADDR_PHY_CTRL_BASE      (ADDR_PHY_RFE_BASE)
#define ADDR_PHY_MONITOR_BASE   (ADDR_PHY_RFE_BASE + 0x100)
#define ADDR_PHY_INT_BASE       (ADDR_PHY_RFE_BASE + 0x200)
#define ADDR_PHY_DEBUG_BASE     (ADDR_PHY_RFE_BASE + 0x400)
#define ADDR_RF_BASE            (ADDR_PHY_RFE_BASE + 0x4000)
#define ADDR_RF_PMU_BASE        (ADDR_RF_BASE)
#define ADDR_RF_CTRL_BASE       (ADDR_RF_BASE + 0x100)
#define ADDR_RF_AGC_BASE        (ADDR_RF_BASE + 0x200)
#define ADDR_RF_PLL_BASE        (ADDR_RF_BASE + 0x300)
#define ADDR_RF_DFE_BASE        (ADDR_RF_BASE + 0x400)
#define ADDR_RF_DCOCI_BASE      (ADDR_RF_BASE + 0x800)
#define ADDR_RF_DCOCQ_BASE      (ADDR_RF_BASE + 0xC00)
#define ADDR_PHY_TX_BUFFER_BASE (ADDR_PHY_RFE_BASE + 0x8000)
#define ADDR_PHY_RX_BUFFER_BASE (ADDR_PHY_RFE_BASE + 0xC000)
#define ADDR_DFE_BUFFER_BASE    (ADDR_PHY_RFE_BASE + 0xD000)

#define ADDR_APB0_BASE      (0x40050000)
#define ADDR_APB1_BASE      (0x40070000)
#define ADDR_APB2_BASE      (0x40090000)
#define ADDR_CRC16_BASE     (0x400B0000)
#define ADDR_CRC24_BASE     (0x400B1000)
#define ADDR_CRC32_BASE     (0x400B2000)
#define ADDR_RAND_BASE      (0x400B3000)
#define ADDR_AES128_BASE    (0x400B4000)

// APB0
#define ADDR_UART0_BASE     (ADDR_APB0_BASE + 0x0)
#define ADDR_UART1_BASE     (ADDR_APB0_BASE + 0x1000)
#define ADDR_FLASH_SPI_BASE (ADDR_APB0_BASE + 0x2000)
#define ADDR_TIMER0_BASE    (ADDR_APB0_BASE + 0x3000)
#define ADDR_TIMER1_BASE    (ADDR_APB0_BASE + 0x4000)
#define ADDR_TIMER2_BASE    (ADDR_APB0_BASE + 0x5000)
#define ADDR_TIMER3_BASE    (ADDR_APB0_BASE + 0x6000)
#define ADDR_GPIO_BASE      (ADDR_APB0_BASE + 0x7000)
#define ADDR_SPI0_BASE      (ADDR_APB0_BASE + 0x8000)
#define ADDR_SPI1_BASE      (ADDR_APB0_BASE + 0x9000)
#define ADDR_WDT_BASE       (ADDR_APB0_BASE + 0xA000)
#define ADDR_DMA_BASE       (ADDR_APB0_BASE + 0xB000) // 0x4005B000
#define ADDR_I2C0_BASE      (ADDR_APB0_BASE + 0xC000)
#define ADDR_I2C1_BASE      (ADDR_APB0_BASE + 0xD000)
#define ADDR_ILA_BASE       (ADDR_APB0_BASE + 0xE000)

// APB1
#define ADDR_LPTIMER0_BASE  (ADDR_APB1_BASE + 0x0)
#define ADDR_LPTIMER1_BASE  (ADDR_APB1_BASE + 0x1000)
#define ADDR_LPUART_BASE    (ADDR_APB1_BASE + 0x2000)

// APB2
#define ADDR_RTC_BASE       (ADDR_APB1_BASE + 0x3000)
#define ADDR_IWDT_BASE      (ADDR_APB1_BASE + 0x4000)
#define ADDR_PMU_BASE       (ADDR_APB1_BASE + 0x5000)
#define ADDR_BACKREG_BASE   (ADDR_APB1_BASE + 0x6000)
#define ADDR_LPIO_BASE      (ADDR_APB1_BASE + 0x7000)

// Phy Handle
#define PHY_CTRL        ((stPhyCtrlHandle_t *)ADDR_PHY_CTRL_BASE)
#define PHY_INT         ((stPhyIntHandle_t *)ADDR_PHY_INT_BASE)
#define PHY_MONITOR     ((stPhyMoniHandle_t *)ADDR_PHY_MONITOR_BASE)
#define PHY_DEBUG       ((stPhyDebugHandle_t *)ADDR_PHY_DEBUG_BASE)
#define PHY_TX_BUFFER   ((volatile uint8_t *)ADDR_PHY_TX_BUFFER_BASE)
#define PHY_RX_BUFFER   ((volatile uint8_t *)ADDR_PHY_RX_BUFFER_BASE)

// RF Handles
#define RF_PMU          ((rf_pmu_handle_t *)ADDR_RF_PMU_BASE)
#define RF_CTRL         ((rf_ctrl_handle_t *)ADDR_RF_CTRL_BASE)
#define RF_AGC          ((rf_agc_handle_t *)ADDR_RF_AGC_BASE)
#define RF_PLL          ((rf_pll_handle_t *)ADDR_RF_PLL_BASE)
#define RF_DFE          ((rf_dfe_handle_t *)ADDR_RF_DFE_BASE)
#define RF_DCOCI        ((rf_dcoc_i_handle_t *)ADDR_RF_DCOCI_BASE)
#define RF_DCOCQ        ((rf_dcoc_q_handle_t *)ADDR_RF_DCOCQ_BASE)

// Handles
#define SYS_CTRL    ((stSysCtrlHandle_t *)ADDR_SYS_CTRL_BASE)

#define CRC16       ((stCrcHandle_t *)ADDR_CRC16_BASE)
#define CRC24       ((stCrcHandle_t *)ADDR_CRC24_BASE)
#define CRC32       ((stCrcHandle_t *)ADDR_CRC32_BASE)
#define RAND        ((stRandHandle_t *)ADDR_RAND_BASE)
#define AES128      ((stAesHandle_t *)ADDR_AES128_BASE)

#define UART0       ((stUartHandle_t *)ADDR_UART0_BASE)
#define UART1       ((stUartHandle_t *)ADDR_UART1_BASE)
#define FLASH_CTRL  ((stFlashCtrlHandle_t *)ADDR_FLASH_SPI_BASE)
#define TIM0        ((stTimerHandle_t *)ADDR_TIMER0_BASE)
#define TIM1        ((stTimerHandle_t *)ADDR_TIMER1_BASE)
#define TIM2        ((stTimerHandle_t *)ADDR_TIMER2_BASE)
#define TIM3        ((stPwmTimerHandle_t *)ADDR_TIMER3_BASE)

#define GPIO        ((stGPIOHandle_t *)ADDR_GPIO_BASE)
#define SPI0        ((stSpiHandle_t *)ADDR_SPI0_BASE)
#define SPI1        ((stSpiHandle_t *)ADDR_SPI1_BASE)
#define WDT         ((stWdtHandle_t *)ADDR_WDT_BASE)

#define IIC0        ((stIicHandle_t *)ADDR_I2C0_BASE)
#define IIC1        ((stIicHandle_t *)ADDR_I2C1_BASE)

#define ILA         ((qmx_ila_reg_t *)ADDR_ILA_BASE)

#define DMA         ((stDMAHandle_t *)ADDR_DMA_BASE)
#define DMA_CH0     ((stDMAChannel_t *)(ADDR_DMA_BASE + 0x00)) // APB0 + D00
#define DMA_CH1     ((stDMAChannel_t *)(ADDR_DMA_BASE + 0x14)) // APB0 + D14
#define DMA_CH2     ((stDMAChannel_t *)(ADDR_DMA_BASE + 0x28)) // APB0 + D28
#define DMA_CH3     ((stDMAChannel_t *)(ADDR_DMA_BASE + 0x3C)) // APB0 + D3C
#define DMA_CH4     ((stDMAChannel_t *)(ADDR_DMA_BASE + 0x50)) // APB0 + D50
#define DMA_CH5     ((stDMAChannel_t *)(ADDR_DMA_BASE + 0x64)) // APB0 + D64
#define DMA_CH6     ((stDMAChannel_t *)(ADDR_DMA_BASE + 0x78)) // APB0 + D78
#define DMA_CH7     ((stDMAChannel_t *)(ADDR_DMA_BASE + 0x8C)) // APB0 + D8C

#define LPTIM0      ((stTimerHandle_t *)ADDR_LPTIMER0_BASE)
#define LPTIM1      ((stTimerHandle_t *)ADDR_LPTIMER1_BASE)
#define LPUART      ((stLpUartHandle_t *)ADDR_LPUART_BASE)
#define RTC         ((stRtcHandle_t *)ADDR_RTC_BASE) // APB2 + 0x0
#define IWDT        ((stWdtHandle_t *)ADDR_IWDT_BASE)
#define PMU         ((stPmuHandle_t *)ADDR_PMU_BASE)
#define LPIO        ((stLpioHandle_t *)ADDR_LPIO_BASE)

#ifdef __cplusplus
}
#endif

#endif
