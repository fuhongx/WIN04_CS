/**
 * ****************************************************************************
 * @file qmx_hal_uart.h
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

#ifndef __QMX_HAL_UART_H__
#define __QMX_HAL_UART_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdint.h>

// UART RTS/CTS flow control support, 1: support, 0: not support
#define HAL_UART_RTS_CTS_SUPPORT    (1)
// UART DMA mode suppport, 1: support, 0: not support
#define HAL_UART_DMA_SUPPORT        (1)
// UART RS485 suppport, 1: support, 0: not support
#define HAL_UART_RS485_SUPPORT      (1)

#define HAL_UART_TIMEOUT_US         (50000U)

/* UART interrupt enable mask */
#define HAL_UART_INT_EN_RX_NOT_EMPTY    (1 << 0)  /*!< RX not empty interrupt enable */
#define HAL_UART_INT_EN_TX_EMPTY        (1 << 1)  /*!< TX empty interrupt enable */
#define HAL_UART_INT_EN_LSR             (1 << 2)  /*!< LSR status interrupt enable */
#define HAL_UART_INT_EN_CTS             (1 << 3)  /*!< CTS change interrupt enable */
#define HAL_UART_INT_EN_LSR_CLR_MODE    (1 << 4)  /*!< LSR clear mode interrupt enable 
                                                        0: read clear status by RBR/LSR
                                                        1: read clear status only by LSR */
#define HAL_UART_INT_EN_UNDER_THLD      (1 << 7)  /*!< TX FIFO under threshold interrupt enable */

/* IRQ STATUS, UART的中断状态 */
#define HAL_UART_STA_RX_READY           (1 << 0)  /*!< RX ready */
#define HAL_UART_STA_RX_OVERFLOW        (1 << 1)  /*!< RX overflow */
#define HAL_UART_STA_RX_PARITY_ERROR    (1 << 2)  /*!< RX PARITY ERROR */
#define HAL_UART_STA_RX_FRAME_ERROR     (1 << 3)  /*!< RX Frame error */
#define HAL_UART_STA_RX_BREAK_ERROR     (1 << 4)  /*!< RX Break error */
#define HAL_UART_STA_TX_UNDER_THLD      (1 << 5)  /*!< TX fifo under threshold */
#define HAL_UART_STA_TX_FIFO_EMPTY      (1 << 6)  /*!< TX FIFO is empty */
#define HAL_UART_STA_RX_ERROR           (1 << 7)  /*!< RX error: parity/frame/start_bit */
#define HAL_UART_STA_BUSY               (1 << 8)  /*!< Busy */
#define HAL_UART_STA_TX_NOT_FULL        (1 << 9)  /*!< TX fifo not full */
#define HAL_UART_STA_TX_EMPTY           (1 << 10) /*!< TX fifo empty */
#define HAL_UART_STA_RX_NOT_EMPTY       (1 << 11) /*!< RX fifo not empty */
#define HAL_UART_STA_RX_FULL            (1 << 12) /*!< RX fifo full */

typedef enum
{
    HAL_UART0 = 0,
    HAL_UART1,

    HAL_UART_MAX,
} hal_uart_id_e;

typedef enum
{
    HAL_UART_PARITY_NONE = 0,   // 无奇偶校验
    HAL_UART_PARITY_ODD,        // 奇校验
    HAL_UART_PARITY_EVEN,       // 偶校验
} hal_uart_parity_e;

typedef enum
{
    HAL_UART_STOPBIT_1 = 0,     // 1位停止位
    HAL_UART_STOPBIT_2,         // 数据位为5时为1.5个停止位, 否则为2个停止位
} hal_uart_stopbit_e;

typedef enum
{
    HAL_UART_DATA_5BIT = 0,     // 5位数据位
    HAL_UART_DATA_6BIT,         // 6位数据位
    HAL_UART_DATA_7BIT,         // 7位数据位
    HAL_UART_DATA_8BIT,         // 8位数据位
} hal_uart_databits_e;

typedef enum
{
    HAL_UART_DMA_MODE0 = 0,     /* DMA模式0: 对于发送：
                                                当发送FIFO的数量等于或小于发送fifo阈值，模块会发起DMA请求。
                                                当发送FIFO的数量大于发送fifo阈值时，模块会撤销DMA请求。
                                             对于接收：
                                                当接收FIFO或RBR寄存器内有数据时，模块会向DMA发起请求。
                                                当接收FIFO空或RBR寄存器空的时候，模块会撤销DMA请求。*/
    HAL_UART_DMA_MODE1,         /* DMA模式1: 对于发送：
                                                当发送FIFO的数量等于或小于发送fifo阈值，模块会发起DMA请求。
                                                当发送FIFO完全满时，模块会撤销DMA请求。
                                             对于接收：
                                                当接收FIFO等于或大于接收fifo阈值，模块会向DMA发起请求。
                                                当接收FIFO空的时候，模块会撤销DMA请求。*/
} hal_uart_dma_mode_e;

typedef enum
{
    HAL_UART_RXFIFO_THLD_1BYTE = 0,
    HAL_UART_RXFIFO_THLD_4BYTES,
    HAL_UART_RXFIFO_THLD_8BYTES,
    HAL_UART_RXFIFO_THLD_14BYTES,
} hal_uart_rxfifo_thld_e;

typedef enum
{
    HAL_UART_TXFIFO_THLD_EMPTY = 0,
    HAL_UART_TXFIFO_THLD_2BYTES,
    HAL_UART_TXFIFO_THLD_4BYTES,
    HAL_UART_TXFIFO_THLD_8BYTES,
} hal_uart_txfifo_thld_e;

typedef struct
{
    uint32_t baudrate;                      // 波特率, 最大3.125Mbps(RC50M)
    hal_uart_parity_e parity;               // 奇偶校验类型
    hal_uart_stopbit_e stopbit;             // 停止位
    hal_uart_databits_e databits;           // 数据位
    hal_uart_txfifo_thld_e tx_fifo_thld;    // 发送FIFO阈值, 会影响DMA发送请求的时机
    hal_uart_rxfifo_thld_e rx_fifo_thld;    // 接收FIFO阈值, 会影响RTS的拉起时机
    bool fifo_en;                           // FIFO使能, true: 使能, false: 禁用
    bool flow_ctrl_en;                      // 流控使能, true: 使能, false: 禁用
} hal_uart_init_t;

#if HAL_UART_RS485_SUPPORT
typedef enum
{
    HAL_RS485_SW_DE = 0,        // 软件DE控制
    HAL_RS485_HW_DE,            // 硬件DE控制
} hal_rs485_mode_e;

typedef enum
{
    HAL_RS485_DE_LOW = 0,       // DE低电平有效
    HAL_RS485_DE_HIGH,          // DE高电平有效
} hal_rs485_de_polarity_e;

typedef struct
{
    hal_rs485_mode_e mode;          // RS485 DE控制模式
    hal_rs485_de_polarity_e de_polarity;  // DE极性
    uint32_t de_assertion_time;     // DE保持时间，DE使能后开始TX输出的时间
    uint32_t de_deassertion_time;   // DE建立时间，TX完成后关闭DE的时间
} hal_rs485_cfg_t;
#endif  // HAL_UART_RS485_SUPPORT

#if HAL_UART_DMA_SUPPORT
#include "qmx_hal_dma.h"

typedef struct
{
    hal_dma_ch_e dma_ch;            // DMA通道
    hal_uart_dma_mode_e mode;       // DMA模式
    uint32_t dma_irq_enable;        // DMA中断使能，见 HAL_DMA_IRQ_ENABLE_XXX 宏定义
    uint32_t timeout;               // DMA超时时间, 单位: 1个AHB时钟周期
    uint8_t trx_dir;                // 传输方向, 0: 发送, 1: 接收
    bool hign_pri;                  // 优先级, true: 高优先级, false: 低优先级
} hal_uart_dma_cfg_t;

#endif // HAL_UART_DMA_SUPPORT

/**
 * @brief UART初始化
 * 
 * @param uart   UART模块ID, 参考 hal_uart_id_e
 * @param config UART初始化配置参数, 参考 hal_uart_init_t
 * 
 * @return int  0: 成功, 其他: 失败
 */
int qmx_hal_uart_init(hal_uart_id_e uart, hal_uart_init_t *config);

/**
 * @brief UART反初始化
 * 
 * @param uart UART模块ID, 参考 hal_uart_id_e
 */
void qmx_hal_uart_deinit(hal_uart_id_e uart);

/**
 * @brief 通过UART发送数据
 * 
 * @param uart UART模块ID, 参考 hal_uart_id_e
 * @param data 待发送数据指针
 * @param len  待发送数据长度, 单位: 字节byte
 * @param timeout 超时时间, 单位: us
 * 
 * @return int  0: 成功, 其他: 失败
 */
int qmx_hal_uart_send_data(hal_uart_id_e uart, uint8_t *data, uint32_t len, uint32_t timeout);

/**
 * @brief 通过UART接收数据
 * 
 * @param uart UART模块ID, 参考 hal_uart_id_e
 * @param data 接收数据存放指针
 * @param len  输入时为期望接收的最大数据长度, 输出时为实际接收的数据长度（小于输入）, 单位: 字节byte
 * @param timeout 超时时间, 单位: us
 * 
 * @return int  0: 成功, 其他: 失败
 */
int qmx_hal_uart_receive_data(hal_uart_id_e uart, uint8_t *data, uint32_t *len, uint32_t timeout);

/**
 * @brief 使能UART中断
 * 
 * @param uart     UART模块ID, 参考 hal_uart_id_e
 * @param irq_mask 中断使能掩码, 参考 HAL_UART_INT_EN_XXX 宏定义
 */
void qmx_hal_uart_enable_irq(hal_uart_id_e uart, uint32_t irq_mask);

/**
 * @brief 禁用UART中断
 * 
 * @param uart     UART模块ID, 参考 hal_uart_id_e
 * @param irq_mask 中断使能掩码, 参考 HAL_UART_INT_EN_XXX 宏定义
 */
void qmx_hal_uart_disable_irq(hal_uart_id_e uart, uint32_t irq_mask);

/**
 * @brief 获取UART中断状态
 * 
 * @param uart      UART模块ID, 参考 hal_uart_id_e
 * @return uint32_t 中断状态掩码, 参考 HAL_UART_STA_XXX 宏定义
 */
uint32_t qmx_hal_uart_get_irq_status(hal_uart_id_e uart);

/**
 * @brief 清除UART中断状态（获取过UART中断状态后无需再次清除，会自动清除）
 * 
 * @param uart     UART模块ID, 参考 hal_uart_id_e
 * @param irq_mask 中断状态掩码, 参考 HAL_UART_STA_XXX 宏定义
 */
void qmx_hal_uart_clear_irq_status(hal_uart_id_e uart, uint32_t irq_mask);

#if HAL_UART_RTS_CTS_SUPPORT

/**
 * @brief 使能或禁用UART的RTS/CTS硬件流控功能
 * 
 * @param uart   UART模块ID, 参考 hal_uart_id_e
 * @param enable true: 使能RTS/CTS硬件流控, false: 禁用RTS/CTS硬件流控
 */
void qmx_hal_uart_rts_cts_enable(hal_uart_id_e uart, bool enable);

/**
 * @brief 获取UART的CTS引脚状态
 * 
 * @param uart UART模块ID, 参考 hal_uart_id_e
 * @return uint32_t 0: CTS未激活，CTS为高电平, 1: CTS激活，CTS为低电平
 */
uint32_t qmx_hal_uart_get_cts_status(hal_uart_id_e uart);

#endif // HAL_UART_RTS_CTS_SUPPORT

#if HAL_UART_RS485_SUPPORT

/**
 * @brief 使能或禁用UART的RS485功能
 * 
 * @param uart   UART模块ID, 参考 hal_uart_id_e
 * @param enable true: 使能RS485, false: 禁用RS485
 */
void qmx_hal_rs485_enable(hal_uart_id_e uart, bool enable);

/**
 * @brief 配置UART的RS485参数
 * 
 * @param uart   UART模块ID, 参考 hal_uart_id_e
 * @param config RS485配置参数, 参考 hal_rs485_cfg_t
 */
void qmx_hal_rs485_config(hal_uart_id_e uart, hal_rs485_cfg_t *config);

/**
 * @brief 手动设置RS485的DE引脚状态，仅当RS485工作在软件DE控制模式下有效
 * 
 * @param uart   UART模块ID, 参考 hal_uart_id_e
 * @param enable true: 使能DE（驱动总线为发送状态）, false: 禁用DE（驱动总线为接收状态）
 */
void qmx_hal_rs485_manual_set_de(hal_uart_id_e uart, bool enable);

#endif  // HAL_UART_RS485_SUPPORT

#if HAL_UART_DMA_SUPPORT

/**
 * @brief 配置UART的DMA参数
 * @note UART DMA的收发接口默认TRX fifo阈值都为1字节，需用户在qmx_hal_uart_init时确保配置一致
 * 
 * @param uart   UART模块ID, 参考 hal_uart_id_e
 * @param config UART DMA配置参数, 参考 hal_uart_dma_cfg_t
 */
void qmx_hal_uart_dma_config(hal_uart_id_e uart, hal_uart_dma_cfg_t *config);

/**
 * @brief 通过UART DMA发送数据
 * @note UART DMA的收发接口默认TRX fifo阈值都为1字节，需用户在qmx_hal_uart_init时确保配置一致
 * 
 * @param uart  UART模块ID, 参考 hal_uart_id_e
 * @param data  待发送数据指针
 * @param len   待发送数据长度, 单位: 字节byte
 * @return int  0: 成功, 其他: 失败
 */
int qmx_hal_uart_dma_send(hal_uart_id_e uart, uint8_t *data, uint32_t len);

/**
 * @brief 通过UART DMA接收数据
 * @note UART DMA的收发接口默认TRX fifo阈值都为1字节，需用户在qmx_hal_uart_init时确保配置一致
 * 
 * @param uart  UART模块ID, 参考 hal_uart_id_e
 * @param data  接收数据存放指针
 * @param len   待接收数据长度, 单位: 字节byte; 实际接收数据长度需通过DMA通道的已传输数据长度获取
 * @return int  0: 成功, 其他: 失败
 */
int qmx_hal_uart_dma_receive(hal_uart_id_e uart, uint8_t *data, uint32_t len);

#endif // HAL_UART_DMA_SUPPORT

#ifdef __cplusplus
}
#endif

#endif  /* __QMX_HAL_UART_H__ */
