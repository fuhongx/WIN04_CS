/**
 * ****************************************************************************
 * @file qmx_hal_dma.h
 * @author QMX software team.
 * @brief
 * @version  1.0
 * ****************************************************************************
 *
 * THIS CODE IS LIMITED TO GUIDE APPLICATION DEVELOPMENT AND INCREASE DEVELOPMENT SPEED.
 * AS A RESULT, QMX IS NOT RESPONSIBLE FOR ANY CLAIMS ARISING DIRECTLY OR INDIRECTLY FROM SUCH FIRMWARE AND/OR CONTENT.
 *
 * Copyright (c) 2026 QMX Semiconductor all rights reserved.
 *
 * ****************************************************************************
 */

#ifndef __QMX_HAL_DMA_H__
#define __QMX_HAL_DMA_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

typedef enum
{
    HAL_DMA_CH0 = 0,
    HAL_DMA_CH1 = 1,
    HAL_DMA_CH2 = 2,
    HAL_DMA_CH3 = 3,
    HAL_DMA_CH4 = 4,
    HAL_DMA_CH5 = 5,
    HAL_DMA_CH6 = 6,
    HAL_DMA_CH7 = 7,

    HAL_DMA_CH_MAX,
} hal_dma_ch_e;

typedef enum
{
    HAL_DMA_WIDTH_BYTE = 0x0,
    HAL_DMA_WIDTH_HALFWORD = 0x1,
    HAL_DMA_WIDTH_WORD = 0x2,
} hal_dma_width_e;

typedef enum
{
    HAL_DMA_REQ_DISABLE   = 0x0,
    HAL_DMA_REQ_UART0_RX  = 0x1,
    HAL_DMA_REQ_UART0_TX  = 0x2,
    HAL_DMA_REQ_UART1_RX  = 0x3,
    HAL_DMA_REQ_UART1_TX  = 0x4,
    HAL_DMA_REQ_SPI0_RX   = 0x5,
    HAL_DMA_REQ_SPI0_TX   = 0x6,
    HAL_DMA_REQ_LPUART_RX = 0x7,
    HAL_DMA_REQ_LPUART_TX = 0x8,
    HAL_DMA_REQ_SPI1_RX   = 0x9,
    HAL_DMA_REQ_SPI1_TX   = 0xA,
    HAL_DMA_REQ_I2C0_RX   = 0xB,
    HAL_DMA_REQ_I2C0_TX   = 0xC,
    HAL_DMA_REQ_I2C1_RX   = 0xD,
    HAL_DMA_REQ_I2C1_TX   = 0xE,
} hal_dma_req_e;

#define HAL_DMA_IRQ_ENABLE_TRANSFER_DONE    (1U << 1)
#define HAL_DMA_IRQ_ENABLE_CFG_ERROR        (1U << 0)
#define HAL_DMA_IRQ_ENABLE_ALL              (HAL_DMA_IRQ_ENABLE_TRANSFER_DONE | HAL_DMA_IRQ_ENABLE_CFG_ERROR)
#define HAL_DMA_IRQ_DISABLE_ALL             (0U)

#define HAL_DMA_IRQ_STA_TRANSFER_DONE_MASK  ((BIT(HAL_DMA_CH_MAX)-1) << 0)
#define HAL_DMA_IRQ_STA_CFG_ERROR_MASK      ((BIT(HAL_DMA_CH_MAX)-1) << HAL_DMA_CH_MAX)

typedef struct
{
    hal_dma_req_e req;
    hal_dma_width_e src_width;
    hal_dma_width_e dst_width;
    uint32_t src_addr;
    uint32_t dst_addr;
    uint32_t single_pkg_size;
    uint32_t muli_trigger_num;
    uint32_t irq_enable;
    uint32_t timeout;
    bool src_addr_rise;
    bool dst_addr_rise;
    bool soft_req;
    bool high_priority;
} hal_dma_init_t;

typedef struct
{
    uint32_t src_addr;
    uint32_t dst_addr;
    uint32_t single_pkg_size;
    uint32_t muli_trigger_num;
    bool src_addr_rise;
    bool dst_addr_rise;
} hal_dma_cfg_t;

/**
 * @brief DMA通道初始化，配置DMA通道参数
 * 
 * @param dma_ch DMA通道号，具体见hal_dma_ch_e
 * @param config DMA通道参数，具体见hal_dma_init_t
 */
void qmx_hal_dma_init(hal_dma_ch_e dma_ch, hal_dma_init_t *config);

/**
 * @brief DMA通道去初始化，关闭DMA通道
 * 
 * @param dma_ch DMA通道号，具体见hal_dma_ch_e
 */
void qmx_hal_dma_deinit(hal_dma_ch_e dma_ch);

/**
 * @brief 配置DMA通道传输参数
 * 
 * @param dma_ch DMA通道号，具体见 hal_dma_ch_e
 * @param cfg DMA通道传输参数，具体见 hal_dma_cfg_t
 */
void qmx_hal_dma_transfer_cfg(hal_dma_ch_e dma_ch, hal_dma_cfg_t *cfg);

/**
 * @brief 启动DMA通道
 * 
 * @param dma_ch DMA通道号，具体见hal_dma_ch_e
 */
void qmx_hal_dma_start(hal_dma_ch_e dma_ch);

/**
 * @brief 停止DMA通道
 * 
 * @param dma_ch DMA通道号，具体见hal_dma_ch_e
 */
void qmx_hal_dma_abort(hal_dma_ch_e dma_ch);

/**
 * @brief 获取DMA通道传输完成字节数
 * 
 * @param dma_ch DMA通道号，具体见hal_dma_ch_e
 * @return uint32_t DMA通道传输完成字节数, 单位byte
 */
uint32_t qmx_hal_dma_get_finish_size(hal_dma_ch_e dma_ch);

/**
 * @brief 获取DMA通道中断状态
 * 
 * @return uint32_t DMA中断状态，1bit对应1通道, 其中[7:0]为传输完成中断状态，[15:8]为配置错误中断状态
 */
uint32_t qmx_hal_dma_get_interrupt_status(void);

/**
 * @brief 清除DMA通道中断状态
 * 
 * @param sta DMA中断状态
 */
void qmx_hal_dma_clear_interrupt_status(uint32_t sta);

#ifdef __cplusplus
}
#endif

#endif  /* __QMX_HAL_DMA_H__ */
