#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "app_cfg.h"
#include "utility.h"
#include "debug.h"
#include "error_def.h"
#include "slc_hal_intc.h"
#include "slc_hal_uart.h"
#include "slc_hal_sysctrl.h"
#include "slc_hal_gpio.h"
#include "slc_uart_test.h"
#include "slc_test_master.h"
#include "slc_hal_delay.h"
#include "slc_hal_rng.h"
#include "hw_uart.h"
#include "reg_uart.h"

#define SLC_UART_TX_FIFO_FILL_TIMEOUT_MS    (200U)

static stUartHandle_t *slc_uart_test_get_handle(void)
{
    return (SLC_TEST_UART_HANDLE == HAL_UART1) ? UART1 : UART0;
}

static uint8_t slc_uart_tx_fifo_cnt_get(stUartHandle_t *handle)
{
    return (uint8_t)(handle->TX_FIFO_CNT & UART_TX_FIFO_CNT_REG_MASK);
}

static int slc_uart_tx_fifo_put(stUartHandle_t *handle, uint8_t byte)
{
    uint16_t sta = 0;

    rom_hw_uart_get_uart_status(handle, &sta);
    if (!(sta & EN_UART_STA_TX_FIFO_NOT_FULL)) {
        return -1;
    }

    handle->DATA_FIFO_AND_CLK_DLV_LO = byte;
    return 0;
}

static int slc_uart_tx_fifo_burst_fill(stUartHandle_t *handle, uint8_t base, uint8_t len, uint8_t *written)
{
    uint16_t sta = 0;
    uint8_t i;

    *written = 0;
    rom_hw_uart_get_uart_status(handle, &sta);
    if (!(sta & EN_UART_STA_TX_FIFO_NOT_FULL)) {
        return -1;
    }

    for (i = 0; i < len; i++) {
        handle->DATA_FIFO_AND_CLK_DLV_LO = (uint8_t)(base + i);
    }

    *written = len;
    return 0;
}

static int slc_uart_tx_fifo_fill(stUartHandle_t *handle, uint8_t base, uint8_t len, uint8_t *written)
{
    uint8_t i;

    for (i = 0; i < len; i++) {
        if (slc_uart_tx_fifo_put(handle, (uint8_t)(base + i)) != 0) {
            *written = i;
            return 0;
        }
    }

    *written = len;
    return 0;
}

static void slc_uart_tx_fifo_clear(stUartHandle_t *handle)
{
    stUartFifoCfg_t fifo = {0};

    fifo.bFifoEn = true;
    fifo.bRxFifoClean = false;
    fifo.bTxFifoClean = true;
    fifo.enDmaMode = EN_UART_DMA_MODE0;
    fifo.enTxFifoThld = EN_UART_TX_FIFO_THLD_EMPTY;
    fifo.enRxFifoThld = EN_UART_RX_FIFO_THLD_1BYTES;
    rom_hw_uart_set_fifo_mode(handle, &fifo);

    fifo.bTxFifoClean = false;
    rom_hw_uart_set_fifo_mode(handle, &fifo);
}

static int slc_uart_tx_fifo_wait_empty(stUartHandle_t *handle, uint32_t timeout_ms)
{
    uint32_t sta = 0;
    uint32_t elapsed = 0;

    while (elapsed < timeout_ms) {
        sta = slc_hal_uart_get_irq_status((handle == UART1) ? HAL_UART1 : HAL_UART0);
        if (sta & HAL_UART_STA_TX_EMPTY) {
            return 0;
        }
        slc_hal_nop_delay_ms(1);
        elapsed++;
    }

    return -1;
}

static int slc_uart_tx_fifo_cfg_both(void)
{
    int ret;
    hal_uart_init_t config = {0};
    uint8_t tx_data[SLC_TEST_FRAME_DATA_LEN] = {0};
    uint8_t rx_data[SLC_TEST_RX_MAX_LEN] = {0};
    uint8_t rx_len = 0;

    config.baudrate = SLC_TEST_UART_BAUDRATE;
    config.parity = HAL_UART_PARITY_NONE;
    config.stopbit = HAL_UART_STOPBIT_1;
    config.databits = HAL_UART_DATA_8BIT;
    config.tx_fifo_thld = HAL_UART_TXFIFO_THLD_EMPTY;
    config.rx_fifo_thld = HAL_UART_RXFIFO_THLD_1BYTE;
    config.fifo_en = true;
    config.flow_ctrl_en = false;

    tx_data[0] = config.baudrate & 0xFF;
    tx_data[1] = (config.baudrate >> 8) & 0xFF;
    tx_data[2] = (config.baudrate >> 16) & 0xFF;
    tx_data[3] = (config.baudrate >> 24) & 0xFF;
    tx_data[4] = config.parity;
    tx_data[5] = config.stopbit;
    tx_data[6] = config.databits;
    tx_data[7] = config.tx_fifo_thld;
    tx_data[8] = config.rx_fifo_thld;
    tx_data[9] = config.fifo_en;
    tx_data[10] = config.flow_ctrl_en;
    tx_data[11] = SLC_TEST_UART_HANDLE;

    slc_test_master_send_cmd(SLC_TEST_CMD_UART_CFG, tx_data, SLC_TEST_FRAME_DATA_LEN);
    ret = slc_test_master_get_result(rx_data, &rx_len);
    if ((ret != 0) || (rx_data[0] != 0)) {
        PRINTF("UART%u tx fifo slave cfg failed.\n", SLC_TEST_UART_HANDLE);
        return -1;
    }

    slc_hal_sysctrl_peripheral_clk_enable(SLC_TEST_UART_CLK_GATE, true);
    slc_hal_sysctrl_peripheral_mod_reset(SLC_TEST_UART_CLK_GATE);
    slc_hal_nop_delay_ms(SLC_TEST_CFG_TIMEOUT_MS);
    slc_hal_uart_init(SLC_TEST_UART_HANDLE, &config);
    slc_hal_uart_enable_irq(SLC_TEST_UART_HANDLE, HAL_UART_INT_EN_RX_NOT_EMPTY);

    return 0;
}

static int slc_uart_tx_fifo_slave_cmd(uint8_t phase, uint8_t pattern, uint8_t *rx_data, uint8_t *rx_len)
{
    uint8_t tx_data[SLC_TEST_FRAME_DATA_LEN] = {0};
    int ret;

    tx_data[0] = phase;
    tx_data[1] = pattern;
    slc_test_master_send_cmd(SLC_TEST_CMD_UART_TX_FIFO, tx_data, SLC_TEST_FRAME_DATA_LEN);
    ret = slc_test_master_get_result(rx_data, rx_len);
    if ((ret != 0) || (rx_data[0] != 0)) {
        PRINTF("UART%u tx fifo slave phase %u failed, ret=%d, status=%u.\n",
               SLC_TEST_UART_HANDLE, phase, ret, rx_data[0]);
        return -1;
    }

    return 0;
}

void slc_uart_test_init(void)
{
    slc_hal_sysctrl_peripheral_clk_enable(HAL_CLK_RAND, true);
    slc_hal_sysctrl_peripheral_mod_reset(HAL_CLK_RAND);

    hal_rng_init_t rng_init = {0};
    rng_init.seed = 0x12345678;
    slc_hal_rng_init(&rng_init);
}

void slc_uart_test_deinit(void)
{
    slc_hal_rng_deinit();
    slc_hal_sysctrl_peripheral_clk_enable(HAL_CLK_RAND, false);
    slc_hal_sysctrl_peripheral_mod_reset(HAL_CLK_RAND);

    slc_test_common_deinit();
}

int slc_uart_test_trx_common(void)
{
    uint8_t tx_data[SLC_TEST_FRAME_DATA_LEN] = {0};
    uint8_t rx_data[SLC_TEST_FRAME_DATA_LEN] = {0};
    uint8_t rx_len = 0;
    int ret = 0;

    ret = slc_hal_get_random_u8(tx_data, SLC_TEST_FRAME_DATA_LEN, 1000);
    if (ret != 0) {
        PRINTF("get random u8 error, ret=0x%X\n", ret);
        return ret;
    }

    slc_test_master_send_cmd(SLC_TEST_CMD_UART_TRX, tx_data, SLC_TEST_FRAME_DATA_LEN);
    slc_test_master_get_result(rx_data, &rx_len);

    if (memcmp(rx_data, tx_data, SLC_TEST_FRAME_DATA_LEN) != 0) {
        PRINTF("UART%u trx data is different.\n", SLC_TEST_UART_HANDLE);
        dump_u8buf("tx", tx_data, SLC_TEST_FRAME_DATA_LEN);
        dump_u8buf("rx", rx_data, SLC_TEST_FRAME_DATA_LEN);
        return -1;
    }

    return 0;
}

int slc_uart_baudrate_test(void)
{
    uint32_t baudrate[] = {
        1200, 2400, 3600, 4800, 9600, 19200, 38400, 57600,
        115200, 128000, 153600, 230400, 460800, 921600, 1000000, 2000000,
        SLC_TEST_UART_BAUDRATE
    };
    int i, ret;
    hal_uart_init_t config = {0};
    uint8_t tx_data[SLC_TEST_RX_MAX_LEN] = {0};
    uint8_t rx_data[SLC_TEST_RX_MAX_LEN] = {0};
    uint8_t rx_len = 0;

    slc_test_common_init();

    config.parity = HAL_UART_PARITY_NONE;
    config.stopbit = HAL_UART_STOPBIT_1;
    config.databits = HAL_UART_DATA_8BIT;
    config.tx_fifo_thld = HAL_UART_TXFIFO_THLD_EMPTY;
    config.rx_fifo_thld = HAL_UART_RXFIFO_THLD_1BYTE;
    config.fifo_en = true;
    config.flow_ctrl_en = false;

    tx_data[4] = config.parity;
    tx_data[5] = config.stopbit;
    tx_data[6] = config.databits;
    tx_data[7] = config.tx_fifo_thld;
    tx_data[8] = config.rx_fifo_thld;
    tx_data[9] = config.fifo_en;
    tx_data[10] = config.flow_ctrl_en;
    tx_data[11] = SLC_TEST_UART_HANDLE;

    for (i = 0; i < sizeof(baudrate) / sizeof(baudrate[0]); i++) {
        rx_len = 0;
        memset(rx_data, 0, SLC_TEST_RX_MAX_LEN);

        config.baudrate = baudrate[i];
        tx_data[0] = config.baudrate & 0xFF;
        tx_data[1] = (config.baudrate >> 8) & 0xFF;
        tx_data[2] = (config.baudrate >> 16) & 0xFF;
        tx_data[3] = (config.baudrate >> 24) & 0xFF;

        slc_test_master_send_cmd(SLC_TEST_CMD_UART_CFG, tx_data, SLC_TEST_FRAME_DATA_LEN);
        ret = slc_test_master_get_result(rx_data, &rx_len);
        if ((ret != 0) || (rx_data[0] != 0)) {
            PRINTF("UART%u baudrate %u cfg failed.\n", tx_data[11], config.baudrate);
            return -1;
        }

        slc_hal_sysctrl_peripheral_clk_enable(SLC_TEST_UART_CLK_GATE, true);
        slc_hal_sysctrl_peripheral_mod_reset(SLC_TEST_UART_CLK_GATE);
        slc_hal_nop_delay_ms(80);

        slc_hal_uart_init(SLC_TEST_UART_HANDLE, &config);
        slc_hal_uart_enable_irq(SLC_TEST_UART_HANDLE, HAL_UART_INT_EN_RX_NOT_EMPTY);

        ret = slc_uart_test_trx_common();
        if (ret != 0) {
            PRINTF("UART%u baudrate %u test failed.\n", SLC_TEST_UART_HANDLE, config.baudrate);
            return -1;
        }
    }

    PRINTF("UART%u baudrate test passed.\n", SLC_TEST_UART_HANDLE);
    return 0;
}

int slc_uart_stopbit_test(void)
{
    int i, ret;
    hal_uart_init_t config = {0};
    uint8_t tx_data[SLC_TEST_RX_MAX_LEN] = {0};
    uint8_t rx_data[SLC_TEST_RX_MAX_LEN] = {0};
    uint8_t rx_len = 0;

    slc_test_common_init();

    config.parity = HAL_UART_PARITY_NONE;
    config.databits = HAL_UART_DATA_8BIT;
    config.tx_fifo_thld = HAL_UART_TXFIFO_THLD_EMPTY;
    config.rx_fifo_thld = HAL_UART_RXFIFO_THLD_1BYTE;
    config.fifo_en = true;
    config.flow_ctrl_en = false;
    config.baudrate = SLC_TEST_UART_BAUDRATE;

    tx_data[0] = config.baudrate & 0xFF;
    tx_data[1] = (config.baudrate >> 8) & 0xFF;
    tx_data[2] = (config.baudrate >> 16) & 0xFF;
    tx_data[3] = (config.baudrate >> 24) & 0xFF;
    tx_data[4] = config.parity;
    tx_data[6] = config.databits;
    tx_data[7] = config.tx_fifo_thld;
    tx_data[8] = config.rx_fifo_thld;
    tx_data[9] = config.fifo_en;
    tx_data[10] = config.flow_ctrl_en;
    tx_data[11] = SLC_TEST_UART_HANDLE;

    for (i = 0; i <= HAL_UART_STOPBIT_2; i++) {
        rx_len = 0;
        memset(rx_data, 0, SLC_TEST_RX_MAX_LEN);

        config.stopbit = (hal_uart_stopbit_e)i;
        tx_data[5] = config.stopbit;

        slc_test_master_send_cmd(SLC_TEST_CMD_UART_CFG, tx_data, SLC_TEST_FRAME_DATA_LEN);
        ret = slc_test_master_get_result(rx_data, &rx_len);
        if ((ret != 0) || (rx_data[0] != 0)) {
            PRINTF("UART%u stopbit %u cfg failed.\n", tx_data[11], config.stopbit);
            return -1;
        }

        slc_hal_sysctrl_peripheral_clk_enable(SLC_TEST_UART_CLK_GATE, true);
        slc_hal_sysctrl_peripheral_mod_reset(SLC_TEST_UART_CLK_GATE);
        slc_hal_nop_delay_ms(SLC_TEST_CFG_TIMEOUT_MS);
        slc_hal_uart_init(SLC_TEST_UART_HANDLE, &config);
        slc_hal_uart_enable_irq(SLC_TEST_UART_HANDLE, HAL_UART_INT_EN_RX_NOT_EMPTY);

        ret = slc_uart_test_trx_common();
        if (ret != 0) {
            PRINTF("UART%u stopbit %u test failed.\n", SLC_TEST_UART_HANDLE, config.stopbit);
            return -1;
        }
    }

    // 恢复slave端uart配置
    rx_len = 0;
    memset(rx_data, 0, SLC_TEST_RX_MAX_LEN);
    tx_data[5] = HAL_UART_STOPBIT_1;

    slc_test_master_send_cmd(SLC_TEST_CMD_UART_CFG, tx_data, SLC_TEST_FRAME_DATA_LEN);
    ret = slc_test_master_get_result(rx_data, &rx_len);
    if ((ret != 0) || (rx_data[0] != 0)) {
        PRINTF("restore slave UART%u stopbit %u cfg failed.\n", tx_data[11], HAL_UART_STOPBIT_1);
        return -1;
    }
    slc_hal_nop_delay_ms(SLC_TEST_CFG_TIMEOUT_MS);

    PRINTF("UART%u stopbit test passed.\n", SLC_TEST_UART_HANDLE);
    return 0;
}

int slc_uart_parity_test(void)
{
    int i, ret;
    hal_uart_init_t config = {0};
    uint8_t tx_data[SLC_TEST_RX_MAX_LEN] = {0};
    uint8_t rx_data[SLC_TEST_RX_MAX_LEN] = {0};
    uint8_t rx_len = 0;

    slc_test_common_init();

    config.stopbit = HAL_UART_STOPBIT_1;
    config.databits = HAL_UART_DATA_8BIT;
    config.tx_fifo_thld = HAL_UART_TXFIFO_THLD_EMPTY;
    config.rx_fifo_thld = HAL_UART_RXFIFO_THLD_1BYTE;
    config.fifo_en = true;
    config.flow_ctrl_en = false;
    config.baudrate = SLC_TEST_UART_BAUDRATE;

    tx_data[0] = config.baudrate & 0xFF;
    tx_data[1] = (config.baudrate >> 8) & 0xFF;
    tx_data[2] = (config.baudrate >> 16) & 0xFF;
    tx_data[3] = (config.baudrate >> 24) & 0xFF;
    tx_data[5] = config.stopbit;
    tx_data[6] = config.databits;
    tx_data[7] = config.tx_fifo_thld;
    tx_data[8] = config.rx_fifo_thld;
    tx_data[9] = config.fifo_en;
    tx_data[10] = config.flow_ctrl_en;
    tx_data[11] = SLC_TEST_UART_HANDLE;

    for (i = 0; i <= HAL_UART_PARITY_EVEN; i++) {
        rx_len = 0;
        memset(rx_data, 0, SLC_TEST_RX_MAX_LEN);

        config.parity = (hal_uart_parity_e)i;
        tx_data[4] = config.parity;

        slc_test_master_send_cmd(SLC_TEST_CMD_UART_CFG, tx_data, SLC_TEST_FRAME_DATA_LEN);
        ret = slc_test_master_get_result(rx_data, &rx_len);
        if ((ret != 0) || (rx_data[0] != 0)) {
            PRINTF("UART%u parity %u cfg failed.\n", tx_data[11], config.parity);
            return -1;
        }

        slc_hal_sysctrl_peripheral_clk_enable(SLC_TEST_UART_CLK_GATE, true);
        slc_hal_sysctrl_peripheral_mod_reset(SLC_TEST_UART_CLK_GATE);
        slc_hal_nop_delay_ms(SLC_TEST_CFG_TIMEOUT_MS);
        slc_hal_uart_init(SLC_TEST_UART_HANDLE, &config);
        slc_hal_uart_enable_irq(SLC_TEST_UART_HANDLE, HAL_UART_INT_EN_RX_NOT_EMPTY);

        ret = slc_uart_test_trx_common();
        if (ret != 0) {
            PRINTF("UART%u parity %u test failed.\n", SLC_TEST_UART_HANDLE, config.parity);
            return -1;
        }
    }

    rx_len = 0;
    memset(rx_data, 0, SLC_TEST_RX_MAX_LEN);
    tx_data[4] = HAL_UART_PARITY_NONE;

    slc_test_master_send_cmd(SLC_TEST_CMD_UART_CFG, tx_data, SLC_TEST_FRAME_DATA_LEN);
    ret = slc_test_master_get_result(rx_data, &rx_len);
    if ((ret != 0) || (rx_data[0] != 0)) {
        PRINTF("restore slave UART%u parity %u cfg failed.\n", tx_data[11], HAL_UART_PARITY_NONE);
        return -1;
    }
    slc_hal_nop_delay_ms(SLC_TEST_CFG_TIMEOUT_MS);

    PRINTF("UART%u parity test passed.\n", SLC_TEST_UART_HANDLE);
    return 0;
}

void slc_uart_test_databit_irq(void)
{
    uint32_t sta = 0;
    uint32_t rx_len = SLC_TEST_RX_MAX_LEN;
    uint8_t rx_data[SLC_TEST_RX_MAX_LEN] = {0};
    sta = slc_hal_uart_get_irq_status(SLC_TEST_UART_HANDLE);

    slc_hal_uart_receive_data(SLC_TEST_UART_HANDLE, rx_data, &rx_len, HAL_UART_TIMEOUT_US);
    if (rx_len > 0)
        dump_u8buf("rx data", rx_data, rx_len);
    else
        PRINTF("rx data len = 0, sta=0x%08X\n", sta);
}

int slc_uart_databits_test(void)
{
    int i, ret;
    hal_uart_init_t config = {0};
    uint8_t tx_data[SLC_TEST_RX_MAX_LEN] = {0};
    uint8_t rx_len = 0;

    for (i = 0; i < SLC_TEST_RX_MAX_LEN; i++)
    {
        tx_data[i] = (i & 0x1F);
    }
    dump_u8buf("tx_data", tx_data, SLC_TEST_FRAME_DATA_LEN);

    config.parity = HAL_UART_PARITY_NONE;
    config.stopbit = HAL_UART_STOPBIT_1;
    config.tx_fifo_thld = HAL_UART_TXFIFO_THLD_EMPTY;
    config.rx_fifo_thld = HAL_UART_RXFIFO_THLD_1BYTE;
    config.fifo_en = true;
    config.flow_ctrl_en = false;
    config.baudrate = SLC_TEST_UART_BAUDRATE;

    i = 0;
    for (i = 0; i <= HAL_UART_DATA_8BIT; i++) {
        config.databits = (hal_uart_databits_e)i;
        PRINTF("UART%u databits %u test start.\n", SLC_TEST_UART_HANDLE, config.databits+5);
        slc_hal_nop_delay_s(5);

        slc_hal_gpio_set_iomux(SLC_TEST_UART_TX_PIN, SLC_TEST_UART_IOMUX);
        slc_hal_gpio_set_iomux(SLC_TEST_UART_RX_PIN, SLC_TEST_UART_IOMUX);
        slc_hal_sysctrl_peripheral_clk_enable(SLC_TEST_UART_CLK_GATE, true);
        slc_hal_sysctrl_peripheral_mod_reset(SLC_TEST_UART_CLK_GATE);
        slc_hal_uart_init(SLC_TEST_UART_HANDLE, &config);
        slc_hal_register_irq_handler(SLC_TEST_UART_IRQ, slc_uart_test_databit_irq);
        SLC_HAL_ENABLE_PERIPHERAL_IRQ(SLC_TEST_UART_IRQ, 0x3);
        slc_hal_uart_enable_irq(SLC_TEST_UART_HANDLE, HAL_UART_INT_EN_RX_NOT_EMPTY);

        slc_hal_uart_send_data(SLC_TEST_UART_HANDLE, tx_data, SLC_TEST_FRAME_DATA_LEN, HAL_UART_TIMEOUT_US);

        PRINTF("UART%u databits %u test wait recv data.\n", SLC_TEST_UART_HANDLE, config.databits+5);
        slc_hal_nop_delay_s(5);
    }

    PRINTF("UART%u databits test passed.\n", SLC_TEST_UART_HANDLE);
    return 0;
}

int slc_uart_complex_test(void)
{
    int i, ret;
    hal_uart_init_t config = {0};
    uint8_t tx_data[SLC_TEST_RX_MAX_LEN] = {0};
    uint8_t rx_data[SLC_TEST_RX_MAX_LEN] = {0};
    uint8_t rx_len = 0;

    slc_test_common_init();

    config.baudrate = SLC_TEST_UART_BAUDRATE;
    config.tx_fifo_thld = HAL_UART_TXFIFO_THLD_EMPTY;
    config.rx_fifo_thld = HAL_UART_RXFIFO_THLD_1BYTE;
    config.fifo_en = true;
    config.flow_ctrl_en = false;

    config.stopbit = HAL_UART_STOPBIT_1;
    config.databits = HAL_UART_DATA_8BIT;

    tx_data[0] = config.baudrate & 0xFF;
    tx_data[1] = (config.baudrate >> 8) & 0xFF;
    tx_data[2] = (config.baudrate >> 16) & 0xFF;
    tx_data[3] = (config.baudrate >> 24) & 0xFF;
    tx_data[6] = config.databits;
    tx_data[7] = config.tx_fifo_thld;
    tx_data[8] = config.rx_fifo_thld;
    tx_data[9] = config.fifo_en;
    tx_data[10] = config.flow_ctrl_en;
    tx_data[11] = SLC_TEST_UART_HANDLE;

repeat_complex_test:
    for (i = 0; i <= HAL_UART_PARITY_EVEN; i++) {
        rx_len = 0;
        memset(rx_data, 0, SLC_TEST_RX_MAX_LEN);

        config.parity = (hal_uart_parity_e)i;
        tx_data[4] = config.parity;
        tx_data[5] = config.stopbit;

        slc_test_master_send_cmd(SLC_TEST_CMD_UART_CFG, tx_data, SLC_TEST_FRAME_DATA_LEN);
        ret = slc_test_master_get_result(rx_data, &rx_len);
        if ((ret != 0) || (rx_data[0] != 0)) {
            PRINTF("UART%u complex cfg failed. parity:%u, stopbit:%u, databits:%u\n", tx_data[11],
                    config.parity, config.stopbit, config.databits);
            return -1;
        }

        slc_hal_sysctrl_peripheral_clk_enable(SLC_TEST_UART_CLK_GATE, true);
        slc_hal_sysctrl_peripheral_mod_reset(SLC_TEST_UART_CLK_GATE);
        slc_hal_nop_delay_ms(SLC_TEST_CFG_TIMEOUT_MS);
        slc_hal_uart_init(SLC_TEST_UART_HANDLE, &config);
        slc_hal_uart_enable_irq(SLC_TEST_UART_HANDLE, HAL_UART_INT_EN_RX_NOT_EMPTY);

        // PRINTF("UART%u complex test start. parity:%u, stopbit:%u, databits:%u\n", SLC_TEST_UART_HANDLE,
        //         config.parity, config.stopbit, config.databits);

        ret = slc_uart_test_trx_common();
        if (ret != 0) {
            PRINTF("UART%u complex test failed. parity:%u, stopbit:%u, databits:%u\n", SLC_TEST_UART_HANDLE,
                    config.parity, config.stopbit, config.databits);
            return -1;
        }
    }

    if ((config.stopbit != HAL_UART_STOPBIT_2)) {
        config.stopbit = HAL_UART_STOPBIT_2;
        i = 0;
        goto  repeat_complex_test;
    }

    rx_len = 0;
    memset(rx_data, 0, SLC_TEST_RX_MAX_LEN);
    tx_data[4] = HAL_UART_PARITY_NONE;
    tx_data[5] = HAL_UART_STOPBIT_1;

    slc_test_master_send_cmd(SLC_TEST_CMD_UART_CFG, tx_data, SLC_TEST_FRAME_DATA_LEN);
    ret = slc_test_master_get_result(rx_data, &rx_len);
    if ((ret != 0) || (rx_data[0] != 0)) {
        PRINTF("restore slave UART%u complex cfg failed.\n", tx_data[11]);
        return -1;
    }
    slc_hal_nop_delay_ms(SLC_TEST_CFG_TIMEOUT_MS);

    PRINTF("UART%u complex test passed.\n", SLC_TEST_UART_HANDLE);
    return 0;
}

int slc_uart_rx_fifo_test(void)
{
    int i;
    uint8_t tx_data[SLC_TEST_FRAME_DATA_LEN] = {0};
    uint8_t rx_data[SLC_TEST_RX_MAX_LEN] = {0};
    uint32_t rx_len = 0;
    uint8_t rx_fifo_detect = 0;

    slc_test_common_init();

fifo_detect:
    rx_fifo_detect++;
    tx_data[0] = rx_fifo_detect;

    slc_hal_uart_enable_irq(SLC_TEST_UART_HANDLE, HAL_UART_INT_EN_RX_NOT_EMPTY);
    SLC_HAL_ENABLE_PERIPHERAL_IRQ(SLC_TEST_UART_IRQ, 0x3);

    slc_test_master_send_cmd(SLC_TEST_CMD_UART_RX_FIFO, tx_data, 1);
    slc_test_master_get_result(rx_data, (uint8_t *)&rx_len);
    if (rx_data[0] != 0) {
        PRINTF("UART%u send rx_fifo=%u cfg test failed.\n", SLC_TEST_UART_HANDLE, rx_fifo_detect);
        return -1;
    }

    slc_hal_uart_disable_irq(SLC_TEST_UART_HANDLE, HAL_UART_INT_EN_RX_NOT_EMPTY);
    SLC_HAL_DISABLE_PERIPHERAL_IRQ(SLC_TEST_UART_IRQ);
    slc_hal_nop_delay_ms(SLC_TEST_CFG_TIMEOUT_MS);

    memset(rx_data, 0, SLC_TEST_RX_MAX_LEN);
    rx_len = SLC_TEST_RX_MAX_LEN;
    slc_hal_uart_receive_data(SLC_TEST_UART_HANDLE, rx_data, &rx_len, HAL_UART_TIMEOUT_US);

    for (i = 0; i < rx_len; i++) {
        if (rx_data[i] != (uint8_t)(i + 1))
            goto end;
    }

    if (rx_len != rx_fifo_detect)
        goto end;

    goto fifo_detect;

end:
    if ((rx_fifo_detect-1) != SLC_TEST_UART_RX_FIFO_THLD) {
        PRINTF("UART%u rx fifo test fail. rx fifo detect[%u] != expect[%u]\n",
                SLC_TEST_UART_HANDLE, rx_fifo_detect-1, SLC_TEST_UART_RX_FIFO_THLD);
        return -1;
    }

    PRINTF("UART%u rx fifo test passed, fifo depth = %u.\n", SLC_TEST_UART_HANDLE, rx_fifo_detect-1);
    return 0;
}

int slc_uart_tx_fifo_test(void)
{
    stUartHandle_t *uart = slc_uart_test_get_handle();
    uint8_t tx_data[SLC_TEST_FRAME_DATA_LEN] = {0};
    uint8_t rx_data[SLC_TEST_RX_MAX_LEN] = {0};
    uint8_t rx_len = 0;
    uint8_t written = 0;
    uint32_t sta = 0;
    int ret;

    slc_test_common_init();

    if (slc_uart_tx_fifo_cfg_both() != 0) {
        return -1;
    }

    /* Step 2: Slave 先进入原始接收模式，再突发写满 TX FIFO 验证深度 */
    if (slc_uart_tx_fifo_slave_cmd(SLC_UART_TX_FIFO_SLAVE_RX_PREP, 0xA1, rx_data, &rx_len) != 0) {
        return -1;
    }

    slc_hal_nop_delay_ms(SLC_TEST_CFG_TIMEOUT_MS);

    if (slc_uart_tx_fifo_wait_empty(uart, SLC_UART_TX_FIFO_FILL_TIMEOUT_MS) != 0) {
        PRINTF("UART%u tx fifo not empty before step2 fill.\n", SLC_TEST_UART_HANDLE);
        return -1;
    }

    if (slc_uart_tx_fifo_burst_fill(uart, 0xA1, SLC_TEST_UART_TX_FIFO_DEPTH, &written) != 0) {
        PRINTF("UART%u tx fifo burst fill failed.\n", SLC_TEST_UART_HANDLE);
        return -1;
    }

    if (written != SLC_TEST_UART_TX_FIFO_DEPTH) {
        PRINTF("UART%u tx fifo write fail, written=%u, expect=%u.\n",
               SLC_TEST_UART_HANDLE, written, SLC_TEST_UART_TX_FIFO_DEPTH);
        return -1;
    }

    PRINTF("UART%u tx fifo step2 passed, fifo depth = %u.\n",
           SLC_TEST_UART_HANDLE, SLC_TEST_UART_TX_FIFO_DEPTH);

    /* Step 3: 停止写入，检查 FIFO 数据被正常发送（Slave 侧接收校验） */
    if (slc_uart_tx_fifo_wait_empty(uart, SLC_UART_TX_FIFO_FILL_TIMEOUT_MS) != 0) {
        PRINTF("UART%u tx fifo drain timeout.\n", SLC_TEST_UART_HANDLE);
        return -1;
    }

    slc_hal_nop_delay_ms(SLC_TEST_CFG_TIMEOUT_MS);

    if (slc_uart_tx_fifo_slave_cmd(SLC_UART_TX_FIFO_SLAVE_RX_GET, 0xA1, rx_data, &rx_len) != 0) {
        return -1;
    }

    if (rx_data[1] != SLC_TEST_UART_TX_FIFO_DEPTH) {
        PRINTF("UART%u tx fifo step3 fail, slave rx len=%u, expect=%u.\n",
               SLC_TEST_UART_HANDLE, rx_data[1], SLC_TEST_UART_TX_FIFO_DEPTH);
        return -1;
    }

    PRINTF("UART%u tx fifo step3 passed, slave received %u bytes.\n",
           SLC_TEST_UART_HANDLE, rx_data[1]);

    /* Step 4/5: 再次写满 16 字节后清理 TX FIFO，并确认清理成功 */
    if (slc_uart_tx_fifo_slave_cmd(SLC_UART_TX_FIFO_SLAVE_RX_PREP_POLL, 0xC1, rx_data, &rx_len) != 0) {
        return -1;
    }

    slc_hal_nop_delay_ms(SLC_TEST_CFG_TIMEOUT_MS);

    if (slc_uart_tx_fifo_wait_empty(uart, SLC_UART_TX_FIFO_FILL_TIMEOUT_MS) != 0) {
        PRINTF("UART%u tx fifo not empty before step4 fill.\n", SLC_TEST_UART_HANDLE);
        return -1;
    }

    if (slc_uart_tx_fifo_burst_fill(uart, 0xC1, SLC_TEST_UART_TX_FIFO_DEPTH, &written) != 0) {
        PRINTF("UART%u tx fifo step4 burst fill failed.\n", SLC_TEST_UART_HANDLE);
        return -1;
    }

    if (written != SLC_TEST_UART_TX_FIFO_DEPTH) {
        PRINTF("UART%u tx fifo refill fail, written=%u.\n", SLC_TEST_UART_HANDLE, written);
        return -1;
    }

    slc_uart_tx_fifo_clear(uart);
    sta = slc_hal_uart_get_irq_status(SLC_TEST_UART_HANDLE);
    if ((slc_uart_tx_fifo_cnt_get(uart) != 0) || !(sta & HAL_UART_STA_TX_EMPTY)) {
        PRINTF("UART%u tx fifo clear fail, cnt=%u, sta=0x%08X.\n",
               SLC_TEST_UART_HANDLE, slc_uart_tx_fifo_cnt_get(uart), sta);
        return -1;
    }

    slc_hal_nop_delay_ms(SLC_TEST_CFG_TIMEOUT_MS);

    if (slc_uart_tx_fifo_slave_cmd(SLC_UART_TX_FIFO_SLAVE_RX_GET, 0xC1, rx_data, &rx_len) != 0) {
        return -1;
    }

    if (rx_data[1] != 0) {
        PRINTF("UART%u tx fifo step5 fail, slave rx len=%u after clear.\n",
               SLC_TEST_UART_HANDLE, rx_data[1]);
        return -1;
    }

    PRINTF("UART%u tx fifo step4/5 passed, fifo cleared and no spurious tx.\n", SLC_TEST_UART_HANDLE);

    /* Slave 侧对称测试：Slave 写满 TX FIFO 并发送，Master 接收校验 */
    if (slc_uart_tx_fifo_slave_cmd(SLC_UART_TX_FIFO_SLAVE_TX_SEND, 0xD1, rx_data, &rx_len) != 0) {
        return -1;
    }

    if (rx_data[1] != SLC_TEST_UART_TX_FIFO_DEPTH) {
        PRINTF("UART%u slave tx fifo fill fail, written=%u.\n", SLC_TEST_UART_HANDLE, rx_data[1]);
        return -1;
    }

    slc_hal_uart_disable_irq(SLC_TEST_UART_HANDLE, HAL_UART_INT_EN_RX_NOT_EMPTY);
    SLC_HAL_DISABLE_PERIPHERAL_IRQ(SLC_TEST_UART_IRQ);
    slc_hal_nop_delay_ms(SLC_TEST_CFG_TIMEOUT_MS);

    memset(rx_data, 0, sizeof(rx_data));
    rx_len = SLC_TEST_UART_TX_FIFO_DEPTH;
    slc_hal_uart_receive_data(SLC_TEST_UART_HANDLE, rx_data, (uint32_t *)&rx_len, HAL_UART_TIMEOUT_US);

    slc_hal_uart_enable_irq(SLC_TEST_UART_HANDLE, HAL_UART_INT_EN_RX_NOT_EMPTY);
    SLC_HAL_ENABLE_PERIPHERAL_IRQ(SLC_TEST_UART_IRQ, 0x3);

    uint8_t i;

    if (rx_len != SLC_TEST_UART_TX_FIFO_DEPTH) {
        PRINTF("UART%u slave tx fifo send fail, master rx len=%u.\n", SLC_TEST_UART_HANDLE, rx_len);
        return -1;
    }

    for (i = 0; i < SLC_TEST_UART_TX_FIFO_DEPTH; i++) {
        if (rx_data[i] != (uint8_t)(0xD1 + i)) {
            PRINTF("UART%u slave tx fifo data mismatch at %u, rx=0x%02X.\n",
                   SLC_TEST_UART_HANDLE, i, rx_data[i]);
            return -1;
        }
    }

    slc_test_master_clear_ack();
    if (slc_uart_tx_fifo_slave_cmd(SLC_UART_TX_FIFO_SLAVE_TX_FILL_CLEAR, 0xE1, rx_data, &rx_len) != 0) {
        return -1;
    }

    slc_test_master_clear_ack();

    PRINTF("UART%u tx fifo test passed.\n", SLC_TEST_UART_HANDLE);
    return 0;
}

#define SLC_UART_SB_LOOPBACK_BAUD       (115200U)
#define SLC_UART_SB_WAIT_MS             (100U)

#ifdef SLC_FPGA
/*
 * FPGA: DEBUG UART0@PIN0/1 不可占用。
 * PIN12/13 MODE0 也是 UART0，会冲掉 DEBUG。
 * UART1 TX/RX 可用 PIN14/PIN15 (MODE0)。
 */
#define SLC_UART_SB_TX_PIN              (HAL_GPIO_PIN14)
#define SLC_UART_SB_RX_PIN              (HAL_GPIO_PIN15)
#else
/* ASIC: DEBUG UART0@PIN12/13; sb test uses PIN14/15 + UART1 */
#define SLC_UART_SB_TX_PIN              (HAL_GPIO_PIN14)
#define SLC_UART_SB_RX_PIN              (HAL_GPIO_PIN15)
#endif
#define SLC_UART_SB_IOMUX               (HAL_IOMUX_MODE0)
#define SLC_UART_SB_UART_HANDLE         (HAL_UART1)
#define SLC_UART_SB_UART_IRQ            (UART1_IRQ)
#define SLC_UART_SB_UART_CLK_GATE       (HAL_CLK_UART1)

static uint8_t slc_uart_rx_fifo_cnt_get(stUartHandle_t *handle)
{
    return (uint8_t)(handle->RX_FIFO_CNT & UART_RX_FIFO_CNT_REG_MASK);
}

static stUartHandle_t *slc_uart_sb_get_handle(void)
{
    return (SLC_UART_SB_UART_HANDLE == HAL_UART1) ? UART1 : UART0;
}

static void slc_uart_sb_fifo_clear(stUartHandle_t *handle, bool clear_tx, bool clear_rx)
{
    stUartFifoCfg_t fifo = {0};

    fifo.bFifoEn = true;
    fifo.bRxFifoClean = clear_rx;
    fifo.bTxFifoClean = clear_tx;
    fifo.enDmaMode = EN_UART_DMA_MODE0;
    fifo.enTxFifoThld = EN_UART_TX_FIFO_THLD_EMPTY;
    fifo.enRxFifoThld = EN_UART_RX_FIFO_THLD_1BYTES;
    rom_hw_uart_set_fifo_mode(handle, &fifo);

    fifo.bRxFifoClean = false;
    fifo.bTxFifoClean = false;
    rom_hw_uart_set_fifo_mode(handle, &fifo);
}

static int slc_uart_sb_loopback_init(void)
{
    hal_uart_init_t config = {0};

    slc_hal_uart_disable_irq(SLC_UART_SB_UART_HANDLE, HAL_UART_INT_EN_RX_NOT_EMPTY);
    SLC_HAL_DISABLE_PERIPHERAL_IRQ(SLC_UART_SB_UART_IRQ);
    slc_hal_unregister_irq_handler(SLC_UART_SB_UART_IRQ);

    slc_hal_gpio_set_iomux(SLC_UART_SB_TX_PIN, SLC_UART_SB_IOMUX);
    slc_hal_gpio_set_iomux(SLC_UART_SB_RX_PIN, SLC_UART_SB_IOMUX);
    slc_hal_gpio_set_mode(SLC_UART_SB_TX_PIN, HAL_GPIO_PULL_UP);
    slc_hal_gpio_set_mode(SLC_UART_SB_RX_PIN, HAL_GPIO_PULL_UP);

    slc_hal_sysctrl_peripheral_clk_enable(SLC_UART_SB_UART_CLK_GATE, true);
    slc_hal_sysctrl_peripheral_mod_reset(SLC_UART_SB_UART_CLK_GATE);

    config.baudrate = SLC_UART_SB_LOOPBACK_BAUD;
    config.parity = HAL_UART_PARITY_NONE;
    config.stopbit = HAL_UART_STOPBIT_1;
    config.databits = HAL_UART_DATA_8BIT;
    config.tx_fifo_thld = HAL_UART_TXFIFO_THLD_EMPTY;
    config.rx_fifo_thld = HAL_UART_RXFIFO_THLD_1BYTE;
    config.fifo_en = true;
    config.flow_ctrl_en = false;

    return slc_hal_uart_init(SLC_UART_SB_UART_HANDLE, &config);
}

static void slc_uart_sb_loopback_deinit(void)
{
    slc_hal_uart_disable_irq(SLC_UART_SB_UART_HANDLE,
                              HAL_UART_INT_EN_RX_NOT_EMPTY |
                              HAL_UART_INT_EN_TX_EMPTY |
                              HAL_UART_INT_EN_UNDER_THLD);
    SLC_HAL_DISABLE_PERIPHERAL_IRQ(SLC_UART_SB_UART_IRQ);
    slc_hal_unregister_irq_handler(SLC_UART_SB_UART_IRQ);
    slc_hal_uart_deinit(SLC_UART_SB_UART_HANDLE);
}

static int slc_uart_sb_drain_rx(stUartHandle_t *uart, uint8_t *buf, uint32_t expect_len)
{
    uint32_t rx_len = expect_len;
    int i;

    if (slc_hal_uart_receive_data(SLC_UART_SB_UART_HANDLE, buf, &rx_len, HAL_UART_TIMEOUT_US) != 0) {
        PRINTF("UART%u drain rx failed\n", SLC_UART_SB_UART_HANDLE);
        return -1;
    }

    if (rx_len != expect_len) {
        PRINTF("UART%u drain rx len %u != %u\n", SLC_UART_SB_UART_HANDLE, rx_len, expect_len);
        return -1;
    }

    for (i = 0; i < (int)expect_len; i++) {
        if (buf[i] != (uint8_t)(0xA0 + i)) {
            PRINTF("UART%u drain rx data mismatch at %d, 0x%02X\n",
                   SLC_UART_SB_UART_HANDLE, i, buf[i]);
            return -1;
        }
    }

    return 0;
}

/* 1) RX FIFO 深度：环回灌满 RX FIFO，验证深度与 RX_FULL */
static int slc_uart_sb_rx_fifo_depth_test(stUartHandle_t *uart)
{
    uint8_t written = 0;
    uint8_t rx_buf[SLC_TEST_UART_RX_FIFO_THLD + 1U];
    uint32_t sta = 0;
    uint8_t rx_cnt = 0;

    PRINTF("UART sb test 1: RX FIFO depth\n");

    slc_uart_sb_fifo_clear(uart, true, true);
    slc_hal_uart_disable_irq(SLC_UART_SB_UART_HANDLE, HAL_UART_INT_EN_RX_NOT_EMPTY);

    if (slc_uart_tx_fifo_burst_fill(uart, 0xA0, SLC_TEST_UART_RX_FIFO_THLD, &written) != 0) {
        PRINTF("UART%u sb rx depth: tx burst fill failed\n", SLC_UART_SB_UART_HANDLE);
        return -1;
    }

    if (written != SLC_TEST_UART_RX_FIFO_THLD) {
        PRINTF("UART%u sb rx depth: written=%u expect=%u\n",
               SLC_UART_SB_UART_HANDLE, written, SLC_TEST_UART_RX_FIFO_THLD);
        return -1;
    }

    if (slc_uart_tx_fifo_wait_empty(uart, SLC_UART_SB_WAIT_MS) != 0) {
        PRINTF("UART%u sb rx depth: tx drain timeout\n", SLC_UART_SB_UART_HANDLE);
        return -1;
    }

    slc_hal_nop_delay_ms(5);

    rx_cnt = slc_uart_rx_fifo_cnt_get(uart);
    sta = slc_hal_uart_get_irq_status(SLC_UART_SB_UART_HANDLE);

    if (rx_cnt != SLC_TEST_UART_RX_FIFO_THLD) {
        PRINTF("UART%u sb rx depth fail: cnt=%u expect=%u, sta=0x%X\n",
               SLC_UART_SB_UART_HANDLE, rx_cnt, SLC_TEST_UART_RX_FIFO_THLD, sta);
        return -1;
    }

    if (!(sta & HAL_UART_STA_RX_FULL)) {
        PRINTF("UART%u sb rx depth fail: RX_FULL not set, sta=0x%X\n",
               SLC_UART_SB_UART_HANDLE, sta);
        return -1;
    }

    if (slc_uart_tx_fifo_put(uart, 0xFF) != 0) {
        PRINTF("UART%u sb rx depth: extra tx write rejected\n", SLC_UART_SB_UART_HANDLE);
        return -1;
    }

    if (slc_uart_tx_fifo_wait_empty(uart, SLC_UART_SB_WAIT_MS) != 0) {
        PRINTF("UART%u sb rx depth: extra tx drain timeout\n", SLC_UART_SB_UART_HANDLE);
        return -1;
    }

    slc_hal_nop_delay_ms(5);

    sta = slc_hal_uart_get_irq_status(SLC_UART_SB_UART_HANDLE);
    rx_cnt = slc_uart_rx_fifo_cnt_get(uart);
    if ((rx_cnt != SLC_TEST_UART_RX_FIFO_THLD) && !(sta & HAL_UART_STA_RX_OVERFLOW)) {
        PRINTF("UART%u sb rx depth fail: no overflow guard, cnt=%u sta=0x%X\n",
               SLC_UART_SB_UART_HANDLE, rx_cnt, sta);
        return -1;
    }

    if (slc_uart_sb_drain_rx(uart, rx_buf, SLC_TEST_UART_RX_FIFO_THLD) != 0) {
        return -1;
    }

    slc_uart_sb_fifo_clear(uart, true, true);

    PRINTF("UART sb test 1 pass: RX FIFO depth = %u\n", SLC_TEST_UART_RX_FIFO_THLD);
    return 0;
}

/* 2) FIFO 状态：验证空/非空/满及 TX 满等状态位 */
static int slc_uart_sb_fifo_status_test(stUartHandle_t *uart)
{
    uint8_t written = 0;
    uint32_t sta = 0;

    PRINTF("UART sb test 2: FIFO status\n");

    slc_uart_sb_fifo_clear(uart, true, true);

    sta = slc_hal_uart_get_irq_status(SLC_UART_SB_UART_HANDLE);
    if (!(sta & HAL_UART_STA_TX_EMPTY) || !(sta & HAL_UART_STA_TX_FIFO_EMPTY)) {
        PRINTF("UART%u sb fifo status fail: idle TX not empty, sta=0x%X\n",
               SLC_UART_SB_UART_HANDLE, sta);
        return -1;
    }

    if ((sta & HAL_UART_STA_RX_NOT_EMPTY) || (slc_uart_rx_fifo_cnt_get(uart) != 0U)) {
        PRINTF("UART%u sb fifo status fail: idle RX not empty, sta=0x%X\n",
               SLC_UART_SB_UART_HANDLE, sta);
        return -1;
    }

    if (slc_uart_tx_fifo_put(uart, 0x55) != 0) {
        PRINTF("UART%u sb fifo status: tx write failed\n", SLC_UART_SB_UART_HANDLE);
        return -1;
    }

    if (slc_uart_tx_fifo_wait_empty(uart, SLC_UART_SB_WAIT_MS) != 0) {
        PRINTF("UART%u sb fifo status: single tx timeout\n", SLC_UART_SB_UART_HANDLE);
        return -1;
    }

    slc_hal_nop_delay_ms(2);

    sta = slc_hal_uart_get_irq_status(SLC_UART_SB_UART_HANDLE);
    if (!(sta & HAL_UART_STA_RX_NOT_EMPTY) || (slc_uart_rx_fifo_cnt_get(uart) != 1U)) {
        PRINTF("UART%u sb fifo status fail: RX not ready after 1 byte, sta=0x%X\n",
               SLC_UART_SB_UART_HANDLE, sta);
        return -1;
    }

    if (sta & HAL_UART_STA_RX_FULL) {
        PRINTF("UART%u sb fifo status fail: RX_FULL set too early, sta=0x%X\n",
               SLC_UART_SB_UART_HANDLE, sta);
        return -1;
    }

    slc_uart_sb_fifo_clear(uart, true, true);

    if (slc_uart_tx_fifo_burst_fill(uart, 0xB0, SLC_TEST_UART_TX_FIFO_DEPTH, &written) != 0) {
        PRINTF("UART%u sb fifo status: tx burst fill failed\n", SLC_UART_SB_UART_HANDLE);
        return -1;
    }

    if (written != SLC_TEST_UART_TX_FIFO_DEPTH) {
        PRINTF("UART%u sb fifo status: tx written=%u expect=%u\n",
               SLC_UART_SB_UART_HANDLE, written, SLC_TEST_UART_TX_FIFO_DEPTH);
        return -1;
    }

    /*
     * 单板环回下边发边收，TX FIFO 会快速移出，不能再要求“第17字节写不进去”
     * 或 NOT_FULL 保持为 0；burst 一次写入 16 字节即验证 TX FIFO 深度。
     */
    sta = slc_hal_uart_get_irq_status(SLC_UART_SB_UART_HANDLE);
    if ((sta & HAL_UART_STA_TX_EMPTY) && (slc_uart_tx_fifo_cnt_get(uart) == 0U)) {
        PRINTF("UART%u sb fifo status fail: TX empty right after burst fill, sta=0x%X\n",
               SLC_UART_SB_UART_HANDLE, sta);
        return -1;
    }

    if (slc_uart_tx_fifo_wait_empty(uart, SLC_UART_SB_WAIT_MS) != 0) {
        PRINTF("UART%u sb fifo status: tx drain timeout\n", SLC_UART_SB_UART_HANDLE);
        return -1;
    }

    sta = slc_hal_uart_get_irq_status(SLC_UART_SB_UART_HANDLE);
    if (!(sta & HAL_UART_STA_TX_EMPTY) || !(sta & HAL_UART_STA_TX_FIFO_EMPTY)) {
        PRINTF("UART%u sb fifo status fail: TX not empty after drain, sta=0x%X\n",
               SLC_UART_SB_UART_HANDLE, sta);
        return -1;
    }

    slc_uart_sb_fifo_clear(uart, true, true);

    PRINTF("UART sb test 2 pass: FIFO status bits OK\n");
    return 0;
}

/**
 * @brief UART 单板 FIFO 测试：UART1@PIN14(TX)/PIN15(RX) 短接环回。
 *
 * 设计逻辑
 * 1、init UART1 环回，Test1 验 RX FIFO 深度，Test2 验 FIFO 状态位
 * 2、中断/NVIC 不在本用例覆盖，由主从 UART / databits 等用例验证
 *
 * check逻辑
 * 1、Test1/Test2 均 pass 则用例 PASS
 */
int slc_uart_single_board_fifo_test(void)
{
    stUartHandle_t *uart = slc_uart_sb_get_handle();
    int ret;

    PRINTF("UART single board fifo test: short PIN14(TX) and PIN15(RX) for loopback\n");

    if (slc_uart_sb_loopback_init() != 0) {
        PRINTF("UART%u sb loopback init failed\n", SLC_UART_SB_UART_HANDLE);
        return -1;
    }

    ret = slc_uart_sb_rx_fifo_depth_test(uart);
    if (ret != 0) {
        slc_uart_sb_loopback_deinit();
        return -1;
    }

    ret = slc_uart_sb_fifo_status_test(uart);
    slc_uart_sb_loopback_deinit();

    if (ret != 0) {
        return -1;
    }

    PRINTF("UART%u single board fifo test passed\n", SLC_UART_SB_UART_HANDLE);
    return 0;
}

#define SLC_TEST_UART_RTS_DE_PIN        (HAL_GPIO_PIN11)
#define SLC_TEST_UART_RTS_IOMUX         (HAL_IOMUX_MODE1)
#define SLC_TEST_UART_CTS_PIN           (HAL_GPIO_PIN10)
#define SLC_TEST_UART_CTS_IOMUX         (HAL_IOMUX_MODE1)

int slc_uart_rts_cts_test(void)
{
    char tx_data[] = "test rts cts\n";
    uint8_t rx_data[SLC_TEST_RX_MAX_LEN] = {0};
    uint32_t rx_len = SLC_TEST_RX_MAX_LEN;
    uint32_t timeout = 20;

    slc_hal_gpio_set_iomux(SLC_TEST_UART_RTS_DE_PIN, SLC_TEST_UART_RTS_IOMUX);
    slc_hal_gpio_set_iomux(SLC_TEST_UART_CTS_PIN, SLC_TEST_UART_CTS_IOMUX);
    slc_hal_uart_disable_irq(DEBUG_UART_HANDLE, HAL_UART_INT_EN_RX_NOT_EMPTY);

    SLC_HAL_DISABLE_PERIPHERAL_IRQ(DEBUG_UART_IRQ);
    slc_hal_uart_rts_cts_enable(DEBUG_UART_HANDLE, true);

    PRINTF("UART%u rts test start.\n", DEBUG_UART_HANDLE);
    while (timeout--)
    {
        PRINTF("pull down cts pin, slave will receive data\n");
        PRINTF("pull up cts pin, slave will not receive data.\n");
        slc_hal_uart_send_data(DEBUG_UART_HANDLE, (uint8_t *)tx_data, sizeof(tx_data), HAL_UART_TIMEOUT_US);
        slc_hal_nop_delay_s(1);
    }

    while (1) {
        PRINTF("RTS default voltage is low(0V)\n");
        PRINTF("slave send data(%u bytes) to master, RTS will pull up, after 5s will pull down.\n", SLC_TEST_UART_RX_FIFO_THLD);
        slc_hal_nop_delay_s(5);
        rx_len = SLC_TEST_RX_MAX_LEN;
        slc_hal_uart_receive_data(DEBUG_UART_HANDLE, rx_data, &rx_len, HAL_UART_TIMEOUT_US);
    }

    PRINTF("UART%u rts test end.\n", DEBUG_UART_HANDLE);

    return 0;
}

void slc_uart_rs485_irq_handler(void)
{
    uint32_t sta = 0;
    uint32_t rx_len = SLC_TEST_RX_MAX_LEN;
    uint8_t rx_data[SLC_TEST_RX_MAX_LEN] = {0};
    sta = slc_hal_uart_get_irq_status(SLC_TEST_UART_HANDLE);

    slc_hal_uart_receive_data(SLC_TEST_UART_HANDLE, rx_data, &rx_len, HAL_UART_TIMEOUT_US);
    if (rx_len > 0)
        dump_u8buf("rs485 rx data", rx_data, rx_len);
}

int slc_uart_rs485_test(void)
{
    hal_uart_init_t config = {0};
    hal_rs485_cfg_t rs485_cfg;
    uint8_t tx_data[SLC_TEST_RX_MAX_LEN] = {0};
    uint32_t rx_len = 0;
    uint8_t rx_data[SLC_TEST_RX_MAX_LEN] = {0};

    memset((void *)tx_data, 0x5a, sizeof(tx_data));

    config.baudrate = 921600;
    config.parity = HAL_UART_PARITY_NONE;
    config.stopbit = HAL_UART_STOPBIT_1;
    config.databits = HAL_UART_DATA_8BIT;
    config.tx_fifo_thld = HAL_UART_TXFIFO_THLD_EMPTY;
    config.rx_fifo_thld = HAL_UART_RXFIFO_THLD_1BYTE;
    config.fifo_en = true;
    config.flow_ctrl_en = false;

    rs485_cfg.mode = HAL_RS485_HW_DE;
    rs485_cfg.de_polarity = HAL_RS485_DE_HIGH;
    rs485_cfg.de_assertion_time = 0x10;
    rs485_cfg.de_deassertion_time = 0x10;

    slc_hal_gpio_set_iomux(SLC_TEST_UART_TX_PIN, SLC_TEST_UART_IOMUX);
    slc_hal_gpio_set_iomux(SLC_TEST_UART_RX_PIN, SLC_TEST_UART_IOMUX);
    slc_hal_gpio_set_iomux(HAL_GPIO_PIN5, SLC_TEST_UART_RTS_IOMUX);

    slc_hal_sysctrl_peripheral_clk_enable(SLC_TEST_UART_CLK_GATE, true);
    slc_hal_sysctrl_peripheral_mod_reset(SLC_TEST_UART_CLK_GATE);
    slc_hal_uart_init(SLC_TEST_UART_HANDLE, &config);
    slc_hal_rs485_config(SLC_TEST_UART_HANDLE, &rs485_cfg);
    slc_hal_rs485_enable(SLC_TEST_UART_HANDLE, true);
    slc_hal_uart_enable_irq(SLC_TEST_UART_HANDLE, HAL_UART_INT_EN_RX_NOT_EMPTY);
    slc_hal_register_irq_handler(SLC_TEST_UART_IRQ, slc_uart_rs485_irq_handler);
    SLC_HAL_ENABLE_PERIPHERAL_IRQ(SLC_TEST_UART_IRQ, 0x3);

    PRINTF("UART%u RS485 test start.\n", SLC_TEST_UART_HANDLE);

    while (1) {
        slc_hal_uart_send_data(SLC_TEST_UART_HANDLE, tx_data, sizeof(tx_data), HAL_UART_TIMEOUT_US);
        slc_hal_nop_delay_s(5);
    }

    return 0;
}

/*
 * UART 中断状态例程（主从两端 8N1）
 * 1、两端配置 UART 8bit / 无校验 / 1 stopbit
 * 2、构造多场景触发不同 STA 置位
 * 3、打印 sta，由上位机判断是否符合预期
 *
 * 打印格式: irq_case N: name, sta=0xXXXX, expect=0xYYYY
 * expect 为关注位掩码，上位机可用 (sta & expect) == expect 判定
 */
static int slc_uart_irq_cfg_both_8n1(void)
{
    hal_uart_init_t config = {0};
    uint8_t tx_data[SLC_TEST_RX_MAX_LEN] = {0};
    uint8_t rx_data[SLC_TEST_RX_MAX_LEN] = {0};
    uint8_t rx_len = 0;
    int ret;

    config.baudrate = SLC_TEST_UART_BAUDRATE;
    config.parity = HAL_UART_PARITY_NONE;
    config.stopbit = HAL_UART_STOPBIT_1;
    config.databits = HAL_UART_DATA_8BIT;
    config.tx_fifo_thld = HAL_UART_TXFIFO_THLD_EMPTY;
    config.rx_fifo_thld = HAL_UART_RXFIFO_THLD_1BYTE;
    config.fifo_en = true;
    config.flow_ctrl_en = false;

    tx_data[0] = config.baudrate & 0xFF;
    tx_data[1] = (config.baudrate >> 8) & 0xFF;
    tx_data[2] = (config.baudrate >> 16) & 0xFF;
    tx_data[3] = (config.baudrate >> 24) & 0xFF;
    tx_data[4] = config.parity;
    tx_data[5] = config.stopbit;
    tx_data[6] = config.databits;
    tx_data[7] = config.tx_fifo_thld;
    tx_data[8] = config.rx_fifo_thld;
    tx_data[9] = config.fifo_en;
    tx_data[10] = config.flow_ctrl_en;
    tx_data[11] = SLC_TEST_UART_HANDLE;

    slc_test_master_send_cmd(SLC_TEST_CMD_UART_CFG, tx_data, SLC_TEST_FRAME_DATA_LEN);
    ret = slc_test_master_get_result(rx_data, &rx_len);
    if ((ret != 0) || (rx_data[0] != 0)) {
        PRINTF("UART%u irq status slave cfg failed.\n", SLC_TEST_UART_HANDLE);
        return -1;
    }

    slc_hal_sysctrl_peripheral_clk_enable(SLC_TEST_UART_CLK_GATE, true);
    slc_hal_sysctrl_peripheral_mod_reset(SLC_TEST_UART_CLK_GATE);
    slc_hal_nop_delay_ms(SLC_TEST_CFG_TIMEOUT_MS);
    slc_hal_uart_init(SLC_TEST_UART_HANDLE, &config);

    return 0;
}

static void slc_uart_irq_print_sta(uint8_t case_id, const char *name, uint32_t sta, uint32_t expect)
{
    PRINTF("irq_case %u: %s, sta=0x%04X, expect=0x%04X\n", case_id, name, sta, expect);
}

static void slc_uart_irq_rx_drain(void)
{
    uint8_t buf[SLC_TEST_RX_MAX_LEN];
    uint32_t len = SLC_TEST_RX_MAX_LEN;

    slc_hal_uart_receive_data(SLC_TEST_UART_HANDLE, buf, &len, HAL_UART_TIMEOUT_US);
}

static int slc_uart_irq_slave_send_bytes(uint8_t len)
{
    uint8_t tx_data[SLC_TEST_FRAME_DATA_LEN] = {0};
    uint8_t rx_data[SLC_TEST_RX_MAX_LEN] = {0};
    uint8_t rx_len = 0;

    tx_data[0] = len;

    slc_hal_uart_enable_irq(SLC_TEST_UART_HANDLE, HAL_UART_INT_EN_RX_NOT_EMPTY);
    SLC_HAL_ENABLE_PERIPHERAL_IRQ(SLC_TEST_UART_IRQ, 0x3);

    slc_test_master_send_cmd(SLC_TEST_CMD_UART_RX_FIFO, tx_data, 1);
    slc_test_master_get_result(rx_data, &rx_len);
    if (rx_data[0] != 0) {
        PRINTF("UART%u irq status slave send %u failed.\n", SLC_TEST_UART_HANDLE, len);
        return -1;
    }

    /* 关掉共享协议中断，避免 payload 被 common IRQ 收走，便于读 STA */
    slc_hal_uart_disable_irq(SLC_TEST_UART_HANDLE, HAL_UART_INT_EN_RX_NOT_EMPTY);
    SLC_HAL_DISABLE_PERIPHERAL_IRQ(SLC_TEST_UART_IRQ);
    slc_hal_nop_delay_ms(SLC_TEST_CFG_TIMEOUT_MS);

    return 0;
}

/**
 * @brief UART 中断状态例程（主从两端 8N1），sta 打印给上位机判定。
 *
 */
int slc_uart_irq_status_test(void)
{
    stUartHandle_t *uart = slc_uart_test_get_handle();
    uint8_t written = 0;
    uint32_t sta = 0;

    PRINTF("UART irq status test: both ends 8N1, report sta for host\n");

    slc_test_common_init();

    if (slc_uart_irq_cfg_both_8n1() != 0) {
        return -1;
    }

    /* case1: 空闲，期望 TX empty / not full */
    slc_hal_uart_disable_irq(SLC_TEST_UART_HANDLE,
                             HAL_UART_INT_EN_RX_NOT_EMPTY |
                             HAL_UART_INT_EN_TX_EMPTY |
                             HAL_UART_INT_EN_UNDER_THLD |
                             HAL_UART_INT_EN_LSR);
    SLC_HAL_DISABLE_PERIPHERAL_IRQ(SLC_TEST_UART_IRQ);
    slc_uart_sb_fifo_clear(uart, true, true);
    slc_hal_nop_delay_ms(10);
    sta = slc_hal_uart_get_irq_status(SLC_TEST_UART_HANDLE);
    slc_uart_irq_print_sta(1, "idle_tx_empty", sta,
                           HAL_UART_STA_TX_EMPTY | HAL_UART_STA_TX_FIFO_EMPTY | HAL_UART_STA_TX_NOT_FULL);

    /* case2: slave 发 1 字节，期望 RX_NOT_EMPTY */
    if (slc_uart_irq_slave_send_bytes(1) != 0) {
        return -1;
    }
    sta = slc_hal_uart_get_irq_status(SLC_TEST_UART_HANDLE);
    slc_uart_irq_print_sta(2, "rx_not_empty", sta, HAL_UART_STA_RX_NOT_EMPTY);
    slc_uart_irq_rx_drain();
    slc_uart_sb_fifo_clear(uart, false, true);

    /* case3: slave 发满 RX FIFO，期望 RX_FULL */
    if (slc_uart_irq_slave_send_bytes(SLC_TEST_UART_RX_FIFO_THLD) != 0) {
        return -1;
    }
    sta = slc_hal_uart_get_irq_status(SLC_TEST_UART_HANDLE);
    slc_uart_irq_print_sta(3, "rx_full", sta,
                           HAL_UART_STA_RX_NOT_EMPTY | HAL_UART_STA_RX_FULL);
    slc_uart_irq_rx_drain();
    slc_uart_sb_fifo_clear(uart, false, true);

    /* case4: 再溢出 1 字节，期望 RX_OVERFLOW（或仍满） */
    if (slc_uart_irq_slave_send_bytes((uint8_t)(SLC_TEST_UART_RX_FIFO_THLD + 1U)) != 0) {
        return -1;
    }
    sta = slc_hal_uart_get_irq_status(SLC_TEST_UART_HANDLE);
    slc_uart_irq_print_sta(4, "rx_overflow", sta,
                           HAL_UART_STA_RX_FULL | HAL_UART_STA_RX_OVERFLOW);
    slc_uart_irq_rx_drain();
    slc_uart_sb_fifo_clear(uart, true, true);

    /* case5: 突发填满 TX FIFO；先 PREP slave 再关协议中断读 STA */
    slc_hal_uart_enable_irq(SLC_TEST_UART_HANDLE, HAL_UART_INT_EN_RX_NOT_EMPTY);
    SLC_HAL_ENABLE_PERIPHERAL_IRQ(SLC_TEST_UART_IRQ, 0x3);
    {
        uint8_t rx_data[SLC_TEST_RX_MAX_LEN] = {0};
        uint8_t rx_len = 0;

        if (slc_uart_tx_fifo_slave_cmd(SLC_UART_TX_FIFO_SLAVE_RX_PREP, 0x51, rx_data, &rx_len) != 0) {
            return -1;
        }
    }
    slc_hal_nop_delay_ms(SLC_TEST_CFG_TIMEOUT_MS);
    slc_hal_uart_disable_irq(SLC_TEST_UART_HANDLE, HAL_UART_INT_EN_RX_NOT_EMPTY);
    SLC_HAL_DISABLE_PERIPHERAL_IRQ(SLC_TEST_UART_IRQ);
    if (slc_uart_tx_fifo_wait_empty(uart, SLC_UART_TX_FIFO_FILL_TIMEOUT_MS) != 0) {
        PRINTF("UART%u irq status: tx not empty before fill\n", SLC_TEST_UART_HANDLE);
        return -1;
    }
    if (slc_uart_tx_fifo_burst_fill(uart, 0x51, SLC_TEST_UART_TX_FIFO_DEPTH, &written) != 0) {
        PRINTF("UART%u irq status: tx burst fill failed\n", SLC_TEST_UART_HANDLE);
        return -1;
    }
    sta = slc_hal_uart_get_irq_status(SLC_TEST_UART_HANDLE);
    slc_uart_irq_print_sta(5, "tx_busy_after_fill", sta, 0);
    if (slc_uart_tx_fifo_wait_empty(uart, SLC_UART_TX_FIFO_FILL_TIMEOUT_MS) != 0) {
        PRINTF("UART%u irq status: tx drain timeout\n", SLC_TEST_UART_HANDLE);
        return -1;
    }
    sta = slc_hal_uart_get_irq_status(SLC_TEST_UART_HANDLE);
    slc_uart_irq_print_sta(6, "tx_empty_after_drain", sta,
                           HAL_UART_STA_TX_EMPTY | HAL_UART_STA_TX_FIFO_EMPTY);
    slc_hal_uart_enable_irq(SLC_TEST_UART_HANDLE, HAL_UART_INT_EN_RX_NOT_EMPTY);
    SLC_HAL_ENABLE_PERIPHERAL_IRQ(SLC_TEST_UART_IRQ, 0x3);
    {
        uint8_t rx_data[SLC_TEST_RX_MAX_LEN] = {0};
        uint8_t rx_len = 0;

        (void)slc_uart_tx_fifo_slave_cmd(SLC_UART_TX_FIFO_SLAVE_RX_GET, 0x51, rx_data, &rx_len);
    }

    /* case7: TX_UNDER_THLD */
    {
        uint8_t rx_data[SLC_TEST_RX_MAX_LEN] = {0};
        uint8_t rx_len = 0;

        if (slc_uart_tx_fifo_slave_cmd(SLC_UART_TX_FIFO_SLAVE_RX_PREP, 0x61, rx_data, &rx_len) != 0) {
            return -1;
        }
    }
    slc_hal_nop_delay_ms(SLC_TEST_CFG_TIMEOUT_MS);
    slc_hal_uart_disable_irq(SLC_TEST_UART_HANDLE, HAL_UART_INT_EN_RX_NOT_EMPTY);
    SLC_HAL_DISABLE_PERIPHERAL_IRQ(SLC_TEST_UART_IRQ);
    slc_hal_uart_enable_irq(SLC_TEST_UART_HANDLE, HAL_UART_INT_EN_UNDER_THLD);
    slc_uart_sb_fifo_clear(uart, true, true);
    if (slc_uart_tx_fifo_burst_fill(uart, 0x61, 1, &written) != 0) {
        PRINTF("UART%u irq status: tx under-thld fill failed\n", SLC_TEST_UART_HANDLE);
        return -1;
    }
    sta = slc_hal_uart_get_irq_status(SLC_TEST_UART_HANDLE);
    slc_uart_irq_print_sta(7, "tx_under_thld", sta, HAL_UART_STA_TX_UNDER_THLD);
    (void)slc_uart_tx_fifo_wait_empty(uart, SLC_UART_TX_FIFO_FILL_TIMEOUT_MS);
    slc_hal_uart_disable_irq(SLC_TEST_UART_HANDLE, HAL_UART_INT_EN_UNDER_THLD);
    slc_hal_uart_enable_irq(SLC_TEST_UART_HANDLE, HAL_UART_INT_EN_RX_NOT_EMPTY);
    SLC_HAL_ENABLE_PERIPHERAL_IRQ(SLC_TEST_UART_IRQ, 0x3);
    {
        uint8_t rx_data[SLC_TEST_RX_MAX_LEN] = {0};
        uint8_t rx_len = 0;

        (void)slc_uart_tx_fifo_slave_cmd(SLC_UART_TX_FIFO_SLAVE_RX_GET, 0x61, rx_data, &rx_len);
    }

    PRINTF("UART%u irq status test done (host judge by irq_case lines)\n", SLC_TEST_UART_HANDLE);
    return 0;
}
