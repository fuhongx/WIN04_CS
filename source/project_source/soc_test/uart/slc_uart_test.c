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
