/**
 * ****************************************************************************
 * @file qmx_hal_spi.h
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

#ifndef __QMX_HAL_SPI_H__
#define __QMX_HAL_SPI_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "compiler.h"

// SPI DMA mode suppport, 1: support, 0: not support
#define HAL_SPI_DMA_SUPPORT         (1)

#define HAL_SPI_TIMEOUT_US          (200000)

/* SPI中断使能MASK位 */
#define HAL_SPI_IRQ_NONE                (0)         /*!< 关闭所有中断 */
#define HAL_SPI_IRQ_TX_OVERFLOW         (1U << 0)   /*!< 发送缓冲区覆盖 */
#define HAL_SPI_IRQ_TX_EMPTY            (1U << 1)   /*!< 发送缓冲区空 */
#define HAL_SPI_IRQ_TX_DONE             (1U << 2)   /*!< 发送完成 */
#define HAL_SPI_IRQ_SLV_RX_ERR          (1U << 3)   /*!< 从SPI接收错误 */
#define HAL_SPI_IRQ_RX_OVERFLOW         (1U << 4)   /*!< 接收缓冲区覆盖 */
#define HAL_SPI_IRQ_RX_DONE             (1U << 5)   /*!< 接收完成 */
#define HAL_SPI_IRQ_SLV_FRAME_END       (1U << 6)   /*!< 从SPI帧结束 */
#define HAL_SPI_IRQ_TX_EMPTY_ERR        (1U << 7)   /*!< 发送缓冲区空错误 */

/* SPI中断状态位 */
#define HAL_SPI_IRQ_STA_TX_OVERFLOW     (1U << 0)   /*!< 发送缓冲区覆盖 */
#define HAL_SPI_IRQ_STA_TX_EMPTY        (1U << 1)   /*!< 发送缓冲区空 */
#define HAL_SPI_IRQ_STA_TX_DONE         (1U << 2)   /*!< 发送完成 */
#define HAL_SPI_IRQ_STA_SLV_RX_ERR      (1U << 3)   /*!< 从SPI接收错误 */
#define HAL_SPI_IRQ_STA_RX_OVERFLOW     (1U << 4)   /*!< 接收缓冲区覆盖 */
#define HAL_SPI_IRQ_STA_RX_DONE         (1U << 5)   /*!< 接收完成 */
#define HAL_SPI_IRQ_STA_SLV_FRAME_END   (1U << 6)   /*!< 从SPI帧结束 */
#define HAL_SPI_IRQ_STA_TX_EMPTY_ERR    (1U << 7)   /*!< 发送缓冲区空错误 */

/* SPI当前状态位 */
#define HAL_SPI_STA_RX_VALID            (1U << 0)   /*!< 接收有效 */
#define HAL_SPI_STA_RX_OVERFLOW         (1U << 1)   /*!< 接收数据被覆盖 */
#define HAL_SPI_STA_SLV_RX_ERROR        (1U << 2)   /*!< 从SPI接收错误 */
#define HAL_SPI_STA_TX_EMPTY            (1U << 3)   /*!< 发送缓冲区空 */
#define HAL_SPI_STA_BUSY                (1U << 4)   /*!< SPI忙闲标志 */

typedef enum
{
    HAL_SPI0 = 0,
    HAL_SPI1,

    HAL_SPI_MAX,
} hal_spi_id_e;

typedef enum
{
    HAL_SPI_SLAVE = 0,
    HAL_SPI_MASTER,
} hal_spi_mode_e;

typedef enum
{
    HAL_SPI_DIV_4 = 0,
    HAL_SPI_DIV_8,
    HAL_SPI_DIV_10,
    HAL_SPI_DIV_16,
    HAL_SPI_DIV_20,
    HAL_SPI_DIV_32,
    HAL_SPI_DIV_64,
    HAL_SPI_DIV_100,
    HAL_SPI_DIV_200,
    HAL_SPI_DIV_400,

    HAL_SPI_DIV_MAX,
} hal_spi_clk_div_e;

typedef enum
{
    HAL_SPI_CPOL0_CPHA0 = 0,
    HAL_SPI_CPOL1_CPHA0,
    HAL_SPI_CPOL0_CPHA1,
    HAL_SPI_CPOL1_CPHA1,
} hal_spi_polarity_phase_e;

typedef enum
{
    HAL_SPI_DATA_MSB = 0,
    HAL_SPI_DATA_LSB,
} hal_spi_data_mode_e;

typedef enum
{
    HAL_SPI_DATA_LEN_8BIT = 0,
    HAL_SPI_DATA_LEN_16BIT,
    HAL_SPI_DATA_LEN_32BIT,

    HAL_SPI_DATA_LEN_MAX,
} hal_spi_data_len_e;

typedef enum
{
    HAL_SPI_CS_LO = 0,
    HAL_SPI_CS_HI,
} hal_spi_cs_level_e;

typedef struct
{
    hal_spi_mode_e mode;
    hal_spi_clk_div_e clk_div;
    hal_spi_polarity_phase_e polarity_phase;
    hal_spi_data_mode_e data_mode;
    hal_spi_data_len_e data_len;
    uint32_t cs_holding_time;   // unit: 20.8ns, master时SPI CS持续时间
    bool clk_adjust_en;
    bool sw_cs_en;
    uint8_t anti_noise_level; // 0: 可以消除小于20ns的噪声; 1: 可以消除小于30ns的噪声
} hal_spi_cfg_t;

#if HAL_SPI_DMA_SUPPORT
#include "qmx_hal_dma.h"

typedef enum
{
    HAL_SPI_DMA_TX_REQ_BY_LAST_BIT = 0,
    HAL_SPI_DMA_TX_REQ_BY_FIRST_BIT,
} hal_spi_dma_tx_req_e;

typedef struct
{
    hal_spi_dma_tx_req_e tx_req_mode;
    uint32_t spi_timeout;       // SPI传输超时时间，unit: SPI工作周期
    hal_dma_ch_e dma_ch;        // DMA通道
    uint32_t dma_irq_enable;    // DMA中断使能，见 HAL_DMA_IRQ_ENABLE_XXX 宏定义
    uint32_t dma_timeout;       // DMA超时时间, 单位: 1个AHB时钟周期
    uint8_t trx_dir;            // 传输方向, 0: 发送, 1: 接收
    bool hign_pri;              // 优先级, true: 高优先级, false: 低优先级
} hal_spi_dma_cfg_t;

#endif /* HAL_SPI_DMA_SUPPORT */

/**
 * @brief SPI初始化
 * 
 * @param id        SPI设备ID，具体见hal_spi_id_e
 * @param spi_cfg   SPI配置参数，具体见hal_spi_cfg_t
 */
void qmx_hal_spi_init(hal_spi_id_e id, hal_spi_cfg_t *spi_cfg);

/**
 * @brief SPI去初始化
 * 
 * @param id    SPI设备ID，具体见hal_spi_id_e
 */
void qmx_hal_spi_deinit(hal_spi_id_e id);

/**
 * @brief 设置SPI软件CS电平（需将对应GPIO的IOMUX设置为CS才可控制）
 * 
 * @param id        SPI设备ID，具体见hal_spi_id_e
 * @param cs_level  CS电平，具体见hal_spi_cs_level_e
 */
void qmx_hal_spi_set_sw_cs(hal_spi_id_e id, hal_spi_cs_level_e cs_level);

/**
 * @brief SPI主模式发送接收数据
 * 
 * @param id        SPI设备ID，具体见hal_spi_id_e
 * @param tx_buf    发送数据buffer指针，数据位宽需与hal_spi_cfg_t中data_len配置一致
 * @param rx_buf    接收数据buffer指针，数据位宽需与hal_spi_cfg_t中data_len配置一致
 * @param len       发送接收数据长度，单位：字节, 需要根据hal_spi_cfg_t中data_len配置转换为实际数据个数
 * @param timeout   超时时间，单位：us
 * @return int 
 */
int qmx_hal_spi_master_trx(hal_spi_id_e id, void *tx_buf, void *rx_buf, uint32_t len, uint32_t timeout);

/**
 * @brief SPI主模式发送数据
 * 
 * @param id        SPI设备ID，具体见hal_spi_id_e
 * @param tx_buf    发送数据buffer指针，数据位宽需与hal_spi_cfg_t中data_len配置一致
 * @param len       发送数据长度，单位：字节, 需要根据hal_spi_cfg_t中data_len配置转换为实际数据个数
 * @param timeout   超时时间，单位：us
 * @return int 
 */
int qmx_hal_spi_master_transmit(hal_spi_id_e id, void *tx_buf, uint32_t len, uint32_t timeout);

/**
 * @brief SPI主模式接收数据
 * 
 * @param id        SPI设备ID，具体见hal_spi_id_e
 * @param rx_buf    接收数据buffer指针，数据位宽需与hal_spi_cfg_t中data_len配置一致
 * @param len       接收数据长度，单位：字节; 传参时为最大接收长度，接收完成后为实际接收长度
 * @param timeout   超时时间，单位：us
 * @return int 
 */
int qmx_hal_spi_master_receive(hal_spi_id_e id, void *rx_buf, uint32_t len, uint32_t timeout);

/**
 * @brief SPI从模式发送数据
 * 
 * @param id        SPI设备ID，具体见hal_spi_id_e
 * @param tx_buf    发送数据buffer指针，数据位宽需与hal_spi_cfg_t中data_len配置一致
 * @param len       发送数据长度，单位：字节, 需要根据hal_spi_cfg_t中data_len配置转换为实际数据个数
 * @param timeout   超时时间，单位：us
 * @return int 
 */
int qmx_hal_spi_slave_transmit(hal_spi_id_e id, void *tx_buf, uint32_t len, uint32_t timeout);

/**
 * @brief SPI从模式接收数据
 * 
 * @param id        SPI设备ID，具体见hal_spi_id_e
 * @param rx_buf    接收数据buffer指针，数据位宽需与hal_spi_cfg_t中data_len配置一致
 * @param len       接收数据长度，单位：字节; 传参时为最大接收长度，接收完成后为实际接收长度
 * @param timeout   超时时间，单位：us
 * @return int 
 */
int qmx_hal_spi_slave_receive(hal_spi_id_e id, void *rx_buf, uint32_t *len, uint32_t timeout);

/**
 * @brief 获取SPI当前状态
 * 
 * @param id    SPI设备ID，具体见hal_spi_id_e
 * @return uint32_t SPI状态，具体见HAL_SPI_STA_XXX宏定义
 */
uint32_t qmx_hal_spi_get_cur_sta(hal_spi_id_e id);

/**
 * @brief 使能SPI中断
 * 
 * @param id        SPI设备ID，具体见hal_spi_id_e
 * @param irq_mask  中断使能位，具体见HAL_SPI_IRQ_XXX宏定义
 */
void qmx_hal_spi_irq_enable(hal_spi_id_e id, uint32_t irq_mask);

/**
 * @brief 禁用SPI中断
 * 
 * @param id        SPI设备ID，具体见hal_spi_id_e
 * @param irq_mask  中断使能位，具体见HAL_SPI_IRQ_XXX宏定义
 */
void qmx_hal_spi_irq_disable(hal_spi_id_e id, uint32_t irq_mask);

/**
 * @brief 获取SPI中断状态
 * 
 * @param id        SPI设备ID，具体见hal_spi_id_e
 * @return uint32_t 中断状态，具体见HAL_SPI_IRQ_XXX宏定义
 */
uint32_t qmx_hal_spi_get_irq_sta(hal_spi_id_e id);

/**
 * @brief 清除SPI中断状态
 * 
 * @param id        SPI设备ID，具体见hal_spi_id_e
 * @param irq_sta   中断状态，具体见HAL_SPI_IRQ_XXX宏定义
 */
void qmx_hal_spi_clear_irq_sta(hal_spi_id_e id, uint32_t irq_sta);

#if HAL_SPI_DMA_SUPPORT

/**
 * @brief 配置SPI DMA
 * 
 * @param id            SPI设备ID，具体见hal_spi_id_e
 * @param spi_dma_init  SPI DMA配置参数，具体见hal_spi_dma_cfg_t
 */
void qmx_hal_spi_dma_config(hal_spi_id_e id, hal_spi_dma_cfg_t *spi_dma_init);

/**
 * @brief SPI DMA发送数据
 * 
 * @param id    SPI设备ID，具体见hal_spi_id_e
 * @param data  发送数据buffer指针，数据位宽需与hal_spi_cfg_t中data_len配置一致, 且地址需对齐
 * @param len   发送数据长度，单位：字节，需是hal_spi_cfg_t中data_len配置的数据位宽的整数倍
 * @return int 
 */
int qmx_hal_spi_dma_send(hal_spi_id_e id, void *data, uint32_t len);

/**
 * @brief SPI DMA接收数据
 * 
 * @param id    SPI设备ID，具体见hal_spi_id_e
 * @param data  接收数据buffer指针，数据位宽需与hal_spi_cfg_t中data_len配置一致, 且地址需对齐
 * @param len   接收数据长度，单位：字节，需是hal_spi_cfg_t中data_len配置的数据位宽的整数倍
 * @return int 
 */
int qmx_hal_spi_dma_recv(hal_spi_id_e id, void *data, uint32_t len);

#endif /* HAL_SPI_DMA_SUPPORT */

#ifdef __cplusplus
}
#endif

#endif  /* __QMX_HAL_SPI_H__ */
