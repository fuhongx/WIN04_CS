#ifndef __SLC_TEST_MASTER_SLAVE_COMMON_H__
#define __SLC_TEST_MASTER_SLAVE_COMMON_H__

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>

#include "reg_Dev.h"
#include "error_def.h"
#include "utility.h"
#include "debug.h"
#include "slc_hal_gpio.h"

#define SLC_TEST_FRAME_START        0xA5
#define SLC_TEST_FRAME_END          0x5A
#define SLC_TEST_FRAME_DATA_LEN     12
#define SLC_TEST_FRAME_LEN          (4 + SLC_TEST_FRAME_DATA_LEN)

#define SLC_TEST_UART_TX_PIN        (HAL_GPIO_PIN2)
#define SLC_TEST_UART_RX_PIN        (HAL_GPIO_PIN3)
#define SLC_TEST_UART_IOMUX         (HAL_IOMUX_MODE0)

#define SLC_TEST_UART_HANDLE        (HAL_UART1)
#define SLC_TEST_UART_IRQ           (UART1_IRQ)
#define SLC_TEST_UART_CLK_GATE      (HAL_CLK_UART1)
#define SLC_TEST_UART_BAUDRATE      (921600)

#define SLC_TEST_RX_MAX_LEN         (32U)

#define SLC_TEST_UART_RX_FIFO_THLD  (16U)

#define SLC_TEST_TIMEOUT_MS         (5000U)

#define SLC_TEST_CFG_TIMEOUT_MS     (80)

// LPUART测试长度
#define SLC_LPUART_TEST_TRX_LEN         (64)

// IIC测试的slave地址
#define SLC_IIC_TEST_TRX_LEN            (256)
#define SLC_TEST_I2C_SLAVE_ADDR_7BIT    (0x51)
#define SLC_TEST_I2C_SLAVE_ADDR_10BIT   (0x364)

// SPI测试
#define SLC_SPI_TEST_TRX_LEN            (16)
#define SLC_SPI_TEST_TX_CMD             (0x02)
#define SLC_SPI_TEST_RX_CMD             (0x03)

typedef enum
{
    // UART相关测试命令
    SLC_TEST_CMD_UART_CFG = 0,  // UART配置
    SLC_TEST_CMD_UART_TRX,      // UART收发测试
    SLC_TEST_CMD_UART_RX_FIFO,  // UART RX FIFO阈值测试

    // LPUART相关测试命令
    SLC_TEST_CMD_LPUART_CFG,    // LPUART配置
    SLC_TEST_CMD_LPUART_TRX,    // LPUART收发测试

    // GPIO相关测试命令
    SLC_TEST_CMD_GPIO_SET,      // 设置slave端GPIO输出电平
    SLC_TEST_CMD_GPIO_GET,      // 获取slave端GPIO输入电平

    // I2C相关测试命令
    SLC_TEST_CMD_I2C_CFG,       // I2C配置
    SLC_TEST_CMD_I2C_S_TRX,     // I2C SLAVE收发测试

    // SPI相关测试命令
    SLC_TEST_CMD_SPI_CFG,       // SPI配置
    SLC_TEST_CMD_SPI_S_TRX,     // SPI SLAVE收发测试

    SLC_TEST_CMD_MAX,
} slc_test_cmd_e;

typedef struct __attribute__((packed))
{
    uint8_t start;      // 帧头
    uint8_t len;        // 帧长度
    uint8_t cmd;        // 命令字, 见 slc_test_cmd_e
    uint8_t data[12];   // master时data为参数，slave时data为结果
    uint8_t end;        // 帧尾
    uint32_t crc;       // CRC32, 小端存储
} slc_test_common_frame_t;

extern volatile slc_test_common_frame_t g_test_common_tx_data;
extern volatile uint8_t g_test_common_rx_data[SLC_TEST_RX_MAX_LEN];
extern volatile uint8_t g_test_common_idx;

int slc_test_check_frame(void);
void slc_test_common_init(void);
void slc_test_common_deinit(void);

#endif  // __SLC_TEST_MASTER_SLAVE_COMMON_H__
