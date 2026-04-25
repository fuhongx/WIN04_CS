/**
 * ****************************************************************************
 * @file qmx_hal_lpuart.h
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

#ifndef __QMX_HAL_LPUART_H__
#define __QMX_HAL_LPUART_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdint.h>

// LPUART DMA mode suppport, 1: support, 0: not support
#define HAL_LPUART_DMA_SUPPORT      (1)

#define HAL_LPUART_TIMEOUT          (5000U)

/* LPUART interrupt enable mask */
#define HAL_LPUART_INT_EN_TX_DONE       (1U << 0)  /*!< TX done interrupt enable */
#define HAL_LPUART_INT_EN_RX_DONE       (1U << 1)  /*!< RX done interrupt enable */
#define HAL_LPUART_INT_EN_RX_START_ERR  (1U << 2)  /*!< start bit error interrupt enable */
#define HAL_LPUART_INT_EN_RX_PARITY_ERR (1U << 3)  /*!< parity error interrupt enable */
#define HAL_LPUART_INT_EN_RX_STOP_ERR   (1U << 4)  /*!< stop bit error interrupt enable */
#define HAL_LPUART_INT_EN_RX_OVERFLOW   (1U << 5)  /*!< RX overflow interrupt enable */
#define HAL_LPUART_INT_EN_RX_PTHLD      (1U << 6)  /*!< RX FIFO reach threshold interrupt enable */

/* IRQ STATUS, LPUART的中断状态 */
#define HAL_LPUART_ISR_TX_DONE          (1U << 0)  /*!< RX ready */
#define HAL_LPUART_ISR_RX_DONE          (1U << 1)  /*!< RX overflow */
#define HAL_LPUART_ISR_RX_START_ERROR   (1U << 2)  /*!< RX SATRT error */
#define HAL_LPUART_ISR_RX_PARITY_ERROR  (1U << 3)  /*!< RX PARITY ERROR */
#define HAL_LPUART_ISR_RX_STOP_ERROR    (1U << 4)  /*!< RX STOP ERROR */
#define HAL_LPUART_ISR_RX_OVERFLOW      (1U << 5)  /*!< RX overflow error */
#define HAL_LPUART_ISR_RX_PTHLD         (1U << 6)  /*!< RX FIFO reach threshold */

typedef enum
{
    HAL_LPUART0 = 0,

    HAL_LPUART_MAX,
} hal_lpuart_id_e;

typedef enum
{
    HAL_LPUART_PARITY_NONE = 0, // 无奇偶校验
    HAL_LPUART_PARITY_ODD,      // 奇校验
    HAL_LPUART_PARITY_EVEN,     // 偶校验
} hal_lpuart_parity_e;

typedef enum
{
    HAL_LPUART_STOPBIT_1 = 0,
    HAL_LPUART_STOPBIT_2,
    HAL_LPUART_STOPBIT_3,
    HAL_LPUART_STOPBIT_4,
} hal_lpuart_stopbit_e;

typedef enum
{
    HAL_LPUART_DATA_5BIT = 0,   // 5位数据位
    HAL_LPUART_DATA_6BIT,       // 6位数据位
    HAL_LPUART_DATA_7BIT,       // 7位数据位
    HAL_LPUART_DATA_8BIT,       // 8位数据位
} hal_lpuart_datalen_e;

typedef struct
{
    uint32_t baudrate;                      // 波特率, 最大9600
    hal_lpuart_parity_e parity;             // 奇偶校验类型
    hal_lpuart_stopbit_e stopbit;           // 停止位
    hal_lpuart_datalen_e datalen;           // 数据位
    uint8_t rx_fifo_thld;                   // 接收FIFO阈值, 影响DMA请求产生时机, 范围1~4
} hal_lpuart_init_t;

#if HAL_LPUART_DMA_SUPPORT
#include "qmx_hal_dma.h"

typedef struct
{
    hal_dma_ch_e dma_ch;            // DMA通道
    uint32_t dma_irq_enable;        // DMA中断使能，见 HAL_DMA_IRQ_ENABLE_XXX 宏定义
    uint32_t timeout;               // DMA超时时间, 单位: 1个AHB时钟周期
    uint8_t trx_dir;                // 传输方向, 0: 发送, 1: 接收
    bool hign_pri;                  // 优先级, true: 高优先级, false: 低优先级
} hal_lpuart_dma_cfg_t;

#endif // HAL_LPUART_DMA_SUPPORT

/**
 * @brief LPUART初始化
 * 
 * @param lpuart    LPUART模块ID, 参考 hal_lpuart_id_e
 * @param config    LPUART初始化配置参数, 参考 hal_lpuart_init_t
 * 
 * @return int  0: 成功, 其他: 失败
 */
int qmx_hal_lpuart_init(hal_lpuart_id_e lpuart, hal_lpuart_init_t *config);

/**
 * @brief LPUART反初始化
 * 
 * @param lpuart LPUART模块ID, 参考 hal_lpuart_id_e
 */
void qmx_hal_lpuart_deinit(hal_lpuart_id_e lpuart);

/**
 * @brief 通过LPUART发送数据
 * 
 * @param lpuart LPUART模块ID, 参考 hal_lpuart_id_e
 * @param data 待发送数据指针
 * @param len  待发送数据长度, 单位: 字节byte
 * 
 * @return int  0: 成功, 其他: 失败
 */
int qmx_hal_lpuart_send_data(hal_lpuart_id_e lpuart, uint8_t *data, uint32_t len);

/**
 * @brief 通过LPUART接收数据
 * 
 * @param lpuart LPUART模块ID, 参考 hal_lpuart_id_e
 * @param data 接收数据存放指针
 * @param len  输入时为期望接收的最大数据长度, 输出时为实际接收的数据长度（小于输入）, 单位: 字节byte
 * 
 * @return int  0: 成功, 其他: 失败
 */
int qmx_hal_lpuart_receive_data(hal_lpuart_id_e lpuart, uint8_t *data, uint32_t *len);

/**
 * @brief 使能LPUART中断
 * 
 * @param lpuart    UART模块ID, 参考 hal_lpuart_id_e
 * @param irq_mask  中断使能掩码, 参考 HAL_LPUART_INT_EN_XXX 宏定义
 */
void qmx_hal_lpuart_enable_irq(hal_lpuart_id_e lpuart, uint32_t irq_mask);

/**
 * @brief 禁用LPUART中断
 * 
 * @param lpuart    LPUART模块ID, 参考 hal_lpuart_id_e
 * @param irq_mask  中断使能掩码, 参考 HAL_LPUART_INT_EN_XXX 宏定义
 */
void qmx_hal_lpuart_disable_irq(hal_lpuart_id_e lpuart, uint32_t irq_mask);

/**
 * @brief 获取LPUART中断状态
 * 
 * @param lpuart      LPUART模块ID, 参考 hal_lpuart_id_e
 * @return uint32_t 中断状态掩码, 参考 HAL_LPUART_ISR_XXX 宏定义
 */
uint32_t qmx_hal_lpuart_get_irq_status(hal_lpuart_id_e lpuart);

/**
 * @brief 清除LPUART中断状态
 * 
 * @param lpuart    LPUART模块ID, 参考 hal_lpuart_id_e
 * @param irq_sta   中断状态
 */
void qmx_hal_lpuart_clear_irq_status(hal_lpuart_id_e lpuart, uint32_t irq_sta);

#if HAL_LPUART_DMA_SUPPORT

/**
 * @brief 配置LPUART的DMA参数
 * @note  LPUART DMA的收发接口默认TRX fifo阈值都为1字节，需用户在qmx_hal_lpuart_init时确保配置一致
 * 
 * @param lpuart    LPUART模块ID, 参考 hal_lpuart_id_e
 * @param config    LPUART DMA配置参数, 参考 hal_lpuart_dma_cfg_t
 */
void qmx_hal_lpuart_dma_config(hal_lpuart_id_e lpuart, hal_lpuart_dma_cfg_t *config);

/**
 * @brief 通过LPUART DMA发送数据
 * @note  LPUART DMA的收发接口默认TRX fifo阈值都为1字节，需用户在qmx_hal_lpuart_init时确保配置一致
 * 
 * @param lpuart  LPUART模块ID, 参考 hal_lpuart_id_e
 * @param data  待发送数据指针
 * @param len   待发送数据长度, 单位: 字节byte
 * @return int  0: 成功, 其他: 失败
 */
int qmx_hal_lpuart_dma_send(hal_lpuart_id_e lpuart, uint8_t *data, uint32_t len);

/**
 * @brief 通过LPUART DMA接收数据
 * @note  LPUART DMA的收发接口默认TRX fifo阈值都为1字节，需用户在qmx_hal_lpuart_init时确保配置一致
 * 
 * @param lpuart  LPUART模块ID, 参考 hal_lpuart_id_e
 * @param data  接收数据存放指针
 * @param len   待接收数据长度, 单位: 字节byte; 实际接收数据长度需通过DMA通道的已传输数据长度获取
 * @return int  0: 成功, 其他: 失败
 */
int qmx_hal_lpuart_dma_receive(hal_lpuart_id_e lpuart, uint8_t *data, uint32_t len);

#endif // HAL_LPUART_DMA_SUPPORT

#ifdef __cplusplus
}
#endif

#endif  /* __QMX_HAL_LPUART_H__ */
