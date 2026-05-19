/**
 * ****************************************************************************
 * @file slc_hal_iic.h
 * @author SLC software team.
 * @brief
 * @version  1.0
 * ****************************************************************************
 *
 * THIS CODE IS LIMITED TO GUIDE APPLICATION DEVELOPMENT AND INCREASE DEVELOPMENT SPEED.
 * AS A RESULT, SLC IS NOT RESPONSIBLE FOR ANY CLAIMS ARISING DIRECTLY OR INDIRECTLY FROM SUCH FIRMWARE AND/OR CONTENT.
 *
 * Copyright (c) 2026 SLC Semiconductor all rights reserved.
 *
 * ****************************************************************************
 */

#ifndef __SLC_HAL_IIC_H__
#define __SLC_HAL_IIC_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdint.h>

// IIC DMA mode suppport, 1: support, 0: not support
#define HAL_IIC_DMA_SUPPORT         (1)

// IIC fifo size
#define HAL_IIC_MAX_FIFO_DEPTH      (8)

#define HAL_IIC_TIMEOUT             (1000000)

/* IIC中断使能MASK位 */
#define HAL_IIC_IRQ_NONE                (0)         /*!< 关闭所有中断 */
#define HAL_IIC_IRQ_RX_UNDERFLOW        (1U << 0)   /*!< 接收缓冲区下溢 */
#define HAL_IIC_IRQ_RX_OVERFLOW         (1U << 1)   /*!< 接收缓冲区溢出 */
#define HAL_IIC_IRQ_RX_FIFO_FULL        (1U << 2)   /*!< 接收FIFO满 */
#define HAL_IIC_IRQ_TX_OVERFLOW         (1U << 3)   /*!< 发送缓冲区溢出 */
#define HAL_IIC_IRQ_TX_EMPTY            (1U << 4)   /*!< 发送缓冲区空 */
#define HAL_IIC_IRQ_READ_REQ            (1U << 5)   /*!< 读请求 */
#define HAL_IIC_IRQ_TX_ABORT            (1U << 6)   /*!< 发送中止 */
#define HAL_IIC_IRQ_RX_DONE             (1U << 7)   /*!< 接收完成 */
#define HAL_IIC_IRQ_ACTIVITY            (1U << 8)   /*!< 活动标志 */
#define HAL_IIC_IRQ_STOP_DET            (1U << 9)   /*!< 停止检测 */
#define HAL_IIC_IRQ_START_DET           (1U << 10)  /*!< 开始检测 */
#define HAL_IIC_IRQ_GEN_CALL            (1U << 11)  /*!< 广播响应中断 */
#define HAL_IIC_IRQ_RESTART_DET         (1U << 12)  /*!< 重启检测 */
#define HAL_IIC_IRQ_ALL                 (0x1FFFU)   /*!< 使能所有中断 */

/* IIC中断状态 */
#define HAL_IIC_IRQ_STA_RX_UNDERFLOW    (1U << 0)   /*!< 接收缓冲区下溢 */
#define HAL_IIC_IRQ_STA_RX_OVERFLOW     (1U << 1)   /*!< 接收缓冲区溢出 */
#define HAL_IIC_IRQ_STA_RX_FIFO_FULL    (1U << 2)   /*!< 接收FIFO满 */
#define HAL_IIC_IRQ_STA_TX_OVERFLOW     (1U << 3)   /*!< 发送缓冲区溢出 */
#define HAL_IIC_IRQ_STA_TX_EMPTY        (1U << 4)   /*!< 发送缓冲区空 */
#define HAL_IIC_IRQ_STA_READ_REQ        (1U << 5)   /*!< 读请求 */
#define HAL_IIC_IRQ_STA_TX_ABORT        (1U << 6)   /*!< 发送中止 */
#define HAL_IIC_IRQ_STA_RX_DONE         (1U << 7)   /*!< 接收完成 */
#define HAL_IIC_IRQ_STA_ACTIVITY        (1U << 8)   /*!< 活动标志 */
#define HAL_IIC_IRQ_STA_STOP_DET        (1U << 9)   /*!< 停止检测 */
#define HAL_IIC_IRQ_STA_START_DET       (1U << 10)  /*!< 开始检测 */
#define HAL_IIC_IRQ_STA_GEN_CALL        (1U << 11)  /*!< 广播响应中断 */
#define HAL_IIC_IRQ_STA_RESTART_DET     (1U << 12)  /*!< 重启检测 */

/* IIC当前状态 */
#define HAL_IIC_STA_ACTIVITY            (1U << 0)   /*!< IIC总线状态, 0: 空闲; 1: 忙 */
#define HAL_IIC_STA_TX_NOT_FULL         (1U << 1)   /*!< 发送缓冲区非空信号 */
#define HAL_IIC_STA_TX_EMPTY            (1U << 2)   /*!< 发送缓冲区空信号 */
#define HAL_IIC_STA_RX_NOT_EMPTY        (1U << 3)   /*!< 接收缓冲区非空信号 */
#define HAL_IIC_STA_RX_FULL             (1U << 4)   /*!< 接收缓冲区满信号 */
#define HAL_IIC_STA_MST_ACTIVITY        (1U << 5)   /*!< 主设备状态机忙信号, 0: 空闲; 1: 忙 */
#define HAL_IIC_STA_SLV_ACTIVITY        (1U << 6)   /*!< 从设备状态机忙信号, 0: 空闲; 1: 忙 */

typedef enum
{
    HAL_IIC0 = 0,
    HAL_IIC1,

    HAL_IIC_MAX,
} hal_iic_id_e;

typedef enum
{
    HAL_IIC_SLAVE = 0,
    HAL_IIC_MASTER,
} hal_iic_mode_e;

typedef enum
{
    HAL_IIC_SPEED_100K = 0,
    HAL_IIC_SPEED_400K,
    HAL_IIC_SPEED_1M,
} hal_iic_speed_e;

typedef enum
{
    HAL_IIC_ADDR_7BIT = 0,
    HAL_IIC_ADDR_10BIT,
} hal_iic_addr_bits_e;

typedef struct
{
    hal_iic_mode_e mode;
    hal_iic_speed_e speed;
    hal_iic_addr_bits_e addr_bits;
    uint16_t addr;
    uint8_t tx_thld;
    uint8_t rx_thld;
} hal_iic_init_t;

#if HAL_IIC_DMA_SUPPORT
#include "slc_hal_dma.h"

typedef struct
{
    hal_iic_mode_e mode;            // IIC模式，主设备或从设备
    hal_dma_ch_e dma_ch;            // DMA通道
    uint32_t dma_irq_enable;        // DMA中断使能，见 HAL_DMA_IRQ_ENABLE_XXX 宏定义
    uint32_t timeout;               // DMA超时时间, 单位: 1个AHB时钟周期
    uint8_t trx_dir;                // 传输方向, 0: 发送, 1: 接收
    bool hign_pri;                  // 优先级, true: 高优先级, false: 低优先级
} hal_iic_dma_cfg_t;

#endif // HAL_IIC_DMA_SUPPORT

/**
 * @brief IIC初始化
 * 
 * @param iic_id    iic设备ID，具体见hal_iic_id_e
 * @param iic_init  iic初始化参数，具体见hal_iic_init_t
 */
void slc_hal_iic_init(hal_iic_id_e iic_id, hal_iic_init_t *iic_init);

/**
 * @brief IIC去初始化
 * 
 * @param iic_id iic设备ID，具体见hal_iic_id_e
 */
void slc_hal_iic_deinit(hal_iic_id_e iic_id);

/**
 * @brief IIC主设备发送数据
 * 
 * @param iic_id iic设备ID，具体见hal_iic_id_e
 * @param data   数据指针
 * @param len    数据长度
 * @param timeout 发送超时时间，单位us
 * @return int 0: 成功，其他: 失败
 */
int slc_hal_iic_master_send(hal_iic_id_e iic_id, uint8_t *data, uint32_t len, uint32_t timeout);

/**
 * @brief IIC主设备接收数据
 * 
 * @param iic_id iic设备ID，具体见hal_iic_id_e
 * @param data   数据指针
 * @param len    初始为最大接收长度，初始值不可为0；接收完成为实际接收长度
 * @param timeout 接收超时时间，单位us
 * @return int 0: 成功，其他: 失败
 */
int slc_hal_iic_master_recv(hal_iic_id_e iic_id, uint8_t *data, uint32_t *len, uint32_t timeout);

/**
 * @brief IIC从设备发送数据
 * 
 * @param iic_id iic设备ID，具体见hal_iic_id_e
 * @param data   数据指针
 * @param len    数据长度
 * @param timeout 发送超时时间，单位us
 * @return int 0: 成功，其他: 失败
 */
int slc_hal_iic_slave_send(hal_iic_id_e iic_id, uint8_t *data, uint32_t len, uint32_t timeout);

/**
 * @brief IIC从设备接收数据
 * 
 * @param iic_id iic设备ID，具体见hal_iic_id_e
 * @param data   数据指针
 * @param len    初始为最大接收长度，初始值不可为0；接收完成为实际接收长度
 * @param timeout 接收超时时间，单位us
 * @return int 0: 成功，其他: 失败
 */
int slc_hal_iic_slave_recv(hal_iic_id_e iic_id, uint8_t *data, uint32_t *len, uint32_t timeout);

/**
 * @brief IIC中断使能
 * 
 * @param iic_id    iic设备ID，具体见hal_iic_id_e
 * @param irq_mask  中断使能，具体见HAL_IIC_IRQ_XXX宏定义
 */
void slc_hal_iic_irq_enable(hal_iic_id_e iic_id, uint32_t irq_mask);

/**
 * @brief IIC中断去使能
 * 
 * @param iic_id    iic设备ID，具体见hal_iic_id_e
 * @param irq_mask  中断使能，具体见HAL_IIC_IRQ_XXX宏定义
 */
void slc_hal_iic_irq_disable(hal_iic_id_e iic_id, uint32_t irq_mask);

/**
 * @brief 获取IIC中断状态
 * 
 * @param iic_id    iic设备ID，具体见hal_iic_id_e
 * @return uint32_t 中断状态，具体见HAL_IIC_IRQ_STA_XXX宏定义
 */
uint32_t slc_hal_iic_get_irq_status(hal_iic_id_e iic_id);

/**
 * @brief 清除IIC中断状态
 * 
 * @param iic_id    iic设备ID，具体见hal_iic_id_e
 * @param irq_sta   中断状态，具体见HAL_IIC_IRQ_STA_XXX宏定义
 */
void slc_hal_iic_clear_irq_status(hal_iic_id_e iic_id, uint32_t irq_sta);

/**
 * @brief 获取IIC当前状态
 * 
 * @param iic_id    iic设备ID，具体见hal_iic_id_e
 * @return uint32_t IIC状态，具体见HAL_IIC_STA_XXX宏定义
 */
uint32_t slc_hal_iic_get_status(hal_iic_id_e iic_id);

#if HAL_IIC_DMA_SUPPORT

/**
 * @brief IIC DMA配置
 * 
 * @param iic_id iic设备ID，具体见hal_iic_id_e
 * @param iic_dma_init iic DMA配置参数，具体见hal_iic_dma_cfg_t
 */
void slc_hal_iic_dma_config(hal_iic_id_e iic_id, hal_iic_dma_cfg_t *iic_dma_init);

/**
 * @brief IIC主设备DMA发送数据
 * 
 * @param iic_id iic设备ID，具体见hal_iic_id_e
 * @param data   数据指针, 数据位宽必须是2字节，但是高字节为0x00，低字节为实际传输数据
 * @param len    数据个数，单位为uint16_t，而非字节数
 * @return int 0: 成功，其他: 失败
 */
int slc_hal_iic_master_dma_send(hal_iic_id_e iic_id, uint16_t *data, uint32_t len);

/**
 * @brief IIC主设备DMA接收数据
 * 
 * @param iic_id    iic设备ID，具体见hal_iic_id_e
 * @param cmd_buf   接收控制字buffer，用于发送接收请求，数据位宽必须是2字节，个数为len，总长度为len*2字节
 * @param data      数据指针
 * @param len       数据长度，单位为uint8_t
 * @return int 0: 成功，其他: 失败
 */
int slc_hal_iic_master_dma_recv(hal_iic_id_e iic_id, uint16_t *cmd_buf, uint8_t *data, uint32_t len);

/**
 * @brief IIC从设备DMA发送数据
 * 
 * @param iic_id iic设备ID，具体见hal_iic_id_e
 * @param data   数据指针
 * @param len    数据长度
 * @return int 0: 成功，其他: 失败
 */
int slc_hal_iic_slave_dma_send(hal_iic_id_e iic_id, uint8_t *data, uint32_t len);

/**
 * @brief IIC从设备DMA接收数据
 * 
 * @param iic_id iic设备ID，具体见hal_iic_id_e
 * @param data   数据指针
 * @param len    数据长度
 * @return int 0: 成功，其他: 失败
 */
int slc_hal_iic_slave_dma_recv(hal_iic_id_e iic_id, uint8_t *data, uint32_t len);

#endif // HAL_IIC_DMA_SUPPORT

#ifdef __cplusplus
}
#endif

#endif  /* __SLC_HAL_IIC_H__ */
