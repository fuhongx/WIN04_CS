#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "app_cfg.h"
#include "utility.h"
#include "debug.h"
#include "error_def.h"
#include "qmx_hal_intc.h"
#include "qmx_hal_lpuart.h"
#include "qmx_hal_sysctrl.h"
#include "qmx_hal_gpio.h"
#include "qmx_uart_test.h"
#include "qmx_test_master.h"
#include "qmx_hal_delay.h"
#include "qmx_hal_rng.h"
#include "qmx_rf.h"

void qmx_lpuart_test_init(void)
{
    qmx_hal_sysctrl_peripheral_clk_enable(HAL_CLK_RAND, true);
    qmx_hal_sysctrl_peripheral_mod_reset(HAL_CLK_RAND);

    hal_rng_init_t rng_init = {0};
    rng_init.seed = 0x12345678;
    qmx_hal_rng_init(&rng_init);
    qmx_test_common_init();

    // RC32K时，9600概率性误码，切换到XTAL32K后正常
    qmx_rf_enable_xtal32k(true);
    qmx_hal_sysctrl_set_lp_clk(HAL_SYSCLK_LP_XTAL32K);

    qmx_hal_gpio_set_iomux(HAL_GPIO_PIN18, HAL_IOMUX_MODE3);
    qmx_hal_gpio_set_iomux(HAL_GPIO_PIN19, HAL_IOMUX_MODE3);
    qmx_hal_lpio_set_iomux(HAL_GPIO_PIN18, HAL_IOMUX_MODE3);
    qmx_hal_lpio_set_iomux(HAL_GPIO_PIN19, HAL_IOMUX_MODE3);
    qmx_hal_gpio_set_mode(HAL_GPIO_PIN18, HAL_GPIO_PULL_UP);
    qmx_hal_gpio_set_mode(HAL_GPIO_PIN19, HAL_GPIO_PULL_UP);
}

void qmx_lpuart_test_deinit(void)
{
    qmx_hal_rng_deinit();
    qmx_hal_sysctrl_peripheral_clk_enable(HAL_CLK_RAND, false);
    qmx_hal_sysctrl_peripheral_mod_reset(HAL_CLK_RAND);

    qmx_test_common_deinit();

    qmx_hal_sysctrl_set_lp_clk(HAL_SYSCLK_LP_RC32K);
    qmx_rf_enable_xtal32k(false);

    qmx_hal_lpuart_deinit(HAL_LPUART0);
    QMX_HAL_DISABLE_PERIPHERAL_IRQ(LPUART_IRQ);
}

volatile uint8_t g_lpuart_test_rx_buf[QMX_LPUART_TEST_TRX_LEN] = {0};
volatile uint32_t g_lpuart_test_rx_len = 0;
void qmx_lpuart_test_irq(void)
{
    uint32_t status = 0;
    status = qmx_hal_lpuart_get_irq_status(HAL_LPUART0);

    g_lpuart_test_rx_len = QMX_LPUART_TEST_TRX_LEN;
    qmx_hal_lpuart_receive_data(HAL_LPUART0, (uint8_t *)g_lpuart_test_rx_buf, (uint32_t *)&g_lpuart_test_rx_len);

    qmx_hal_lpuart_clear_irq_status(HAL_LPUART0, status);
}

int qmx_lpuart_test_trx_common(hal_lpuart_id_e lpuart)
{
    uint8_t tx_data[QMX_LPUART_TEST_TRX_LEN] = {0};
    uint8_t tx_cmd[QMX_TEST_FRAME_DATA_LEN] = {0};
    uint8_t rx_cmd[QMX_TEST_FRAME_DATA_LEN] = {0};
    uint8_t rx_len = 0;
    int ret = 0;
    uint8_t timeout = 50;

    ret = qmx_hal_get_random_u8(tx_data, QMX_LPUART_TEST_TRX_LEN, 1000);
    if (ret != 0) {
        PRINTF("get random u8 error, ret=0x%X\n", ret);
        return ret;
    }

    qmx_hal_lpuart_send_data(lpuart, tx_data, QMX_LPUART_TEST_TRX_LEN);

    qmx_hal_nop_delay_ms(500);  // wait tx finish

    g_lpuart_test_rx_len = 0;
    tx_cmd[0] = lpuart;
    tx_cmd[1] = 1;  // 0: send, 1: receive
    qmx_test_master_send_cmd(QMX_TEST_CMD_LPUART_TRX, tx_cmd, QMX_TEST_FRAME_DATA_LEN);
    ret = qmx_test_master_get_result(rx_cmd, &rx_len);
    if ((ret != 0) || (rx_cmd[0] != 0)) {
        PRINTF("LPUART%u TRX get data failed.\n", lpuart);
        return -1;
    }

    while (timeout--) {
        if (g_lpuart_test_rx_len != 0)
            break;

        qmx_hal_nop_delay_ms(100);
    }

    if (g_lpuart_test_rx_len != QMX_LPUART_TEST_TRX_LEN) {
        PRINTF("LPUART%u rx len err, %u != %u.\n", lpuart, g_lpuart_test_rx_len, QMX_LPUART_TEST_TRX_LEN);
        dump_u8buf("tx", tx_data, QMX_LPUART_TEST_TRX_LEN);
        if (g_lpuart_test_rx_len != 0)
            dump_u8buf("rx", (uint8_t *)g_lpuart_test_rx_buf, g_lpuart_test_rx_len);
        return -1;
    }

    if (memcmp((void *)g_lpuart_test_rx_buf, tx_data, QMX_LPUART_TEST_TRX_LEN) != 0) {
        PRINTF("LPUART%u trx data is different.\n", lpuart);
        dump_u8buf("tx", tx_data, QMX_LPUART_TEST_TRX_LEN);
        dump_u8buf("rx", (uint8_t *)g_lpuart_test_rx_buf, QMX_LPUART_TEST_TRX_LEN);
        return -1;
    }

    return 0;
}

int qmx_lpuart_baudrate_test(void)
{
    uint32_t baudrate[] = {
        1200, 2400, 3600, 4800, 9600
    };
    int i, ret;
    hal_lpuart_init_t lpuart_init = {0};
    uint8_t tx_data[QMX_TEST_FRAME_DATA_LEN] = {0};
    uint8_t rx_data[QMX_TEST_FRAME_DATA_LEN] = {0};
    uint8_t rx_len = 0;

    qmx_hal_register_irq_handler(LPUART_IRQ, qmx_lpuart_test_irq);
    QMX_HAL_ENABLE_PERIPHERAL_IRQ(LPUART_IRQ, 0x3);

    lpuart_init.parity = HAL_LPUART_PARITY_NONE;
    lpuart_init.stopbit = HAL_LPUART_STOPBIT_1;
    lpuart_init.datalen = HAL_LPUART_DATA_8BIT;
    lpuart_init.rx_fifo_thld = 1;

    tx_data[4] = lpuart_init.parity;
    tx_data[5] = lpuart_init.stopbit;
    tx_data[6] = lpuart_init.datalen;
    tx_data[7] = lpuart_init.rx_fifo_thld;
    tx_data[8] = HAL_LPUART0;

    // 9600波特率时，RC32K概率性错包
    for (i = 0; i < (sizeof(baudrate) / sizeof(baudrate[0])); i++) {
        rx_len = 0;
        memset(rx_data, 0, QMX_TEST_FRAME_DATA_LEN);

        lpuart_init.baudrate = baudrate[i];
        tx_data[0] = lpuart_init.baudrate & 0xFF;
        tx_data[1] = (lpuart_init.baudrate >> 8) & 0xFF;
        tx_data[2] = (lpuart_init.baudrate >> 16) & 0xFF;
        tx_data[3] = (lpuart_init.baudrate >> 24) & 0xFF;

        qmx_test_master_send_cmd(QMX_TEST_CMD_LPUART_CFG, tx_data, QMX_TEST_FRAME_DATA_LEN);
        ret = qmx_test_master_get_result(rx_data, &rx_len);
        if ((ret != 0) || (rx_data[0] != 0)) {
            PRINTF("LPUART%u baudrate cfg cmd failed.\n", HAL_LPUART0);
            return -1;
        }

        qmx_hal_nop_delay_ms(QMX_TEST_CFG_TIMEOUT_MS);

        qmx_hal_sysctrl_peripheral_clk_enable(HAL_CLK_LPUART, true);
        qmx_hal_sysctrl_peripheral_mod_reset(HAL_CLK_LPUART);
        qmx_hal_lpuart_init(HAL_LPUART0, &lpuart_init);
        qmx_hal_lpuart_enable_irq(HAL_LPUART0, HAL_LPUART_INT_EN_RX_DONE);

        ret = qmx_lpuart_test_trx_common(HAL_LPUART0);
        if (ret != 0) {
            PRINTF("LPUART%u baudrate %u test failed.\n", HAL_LPUART0, baudrate[i]);
            return -1;
        }
    }

    PRINTF("LPUART%u baudrate test passed.\n", HAL_LPUART0);
    return 0;
}

int qmx_lpuart_stopbit_test(void)
{
    int i, ret;
    hal_lpuart_init_t lpuart_init = {0};
    uint8_t tx_data[QMX_TEST_FRAME_DATA_LEN] = {0};
    uint8_t rx_data[QMX_TEST_FRAME_DATA_LEN] = {0};
    uint8_t rx_len = 0;

    qmx_hal_register_irq_handler(LPUART_IRQ, qmx_lpuart_test_irq);
    QMX_HAL_ENABLE_PERIPHERAL_IRQ(LPUART_IRQ, 0x3);

    lpuart_init.baudrate = 4800;
    lpuart_init.parity = HAL_LPUART_PARITY_NONE;
    lpuart_init.datalen = HAL_LPUART_DATA_8BIT;
    lpuart_init.rx_fifo_thld = 1;

    tx_data[0] = lpuart_init.baudrate & 0xFF;
    tx_data[1] = (lpuart_init.baudrate >> 8) & 0xFF;
    tx_data[2] = (lpuart_init.baudrate >> 16) & 0xFF;
    tx_data[3] = (lpuart_init.baudrate >> 24) & 0xFF;
    tx_data[4] = lpuart_init.parity;
    tx_data[6] = lpuart_init.datalen;
    tx_data[7] = lpuart_init.rx_fifo_thld;
    tx_data[8] = HAL_LPUART0;

    for (i = HAL_LPUART_STOPBIT_1; i <= HAL_LPUART_STOPBIT_4; i++) {
        rx_len = 0;
        memset(rx_data, 0, QMX_TEST_FRAME_DATA_LEN);

        lpuart_init.stopbit = (hal_lpuart_stopbit_e)i;
        tx_data[5] = lpuart_init.stopbit;

        qmx_test_master_send_cmd(QMX_TEST_CMD_LPUART_CFG, tx_data, QMX_TEST_FRAME_DATA_LEN);
        ret = qmx_test_master_get_result(rx_data, &rx_len);
        if ((ret != 0) || (rx_data[0] != 0)) {
            PRINTF("LPUART%u stopbit cfg cmd failed.\n", HAL_LPUART0);
            return -1;
        }

        qmx_hal_nop_delay_ms(QMX_TEST_CFG_TIMEOUT_MS);

        qmx_hal_sysctrl_peripheral_clk_enable(HAL_CLK_LPUART, true);
        qmx_hal_sysctrl_peripheral_mod_reset(HAL_CLK_LPUART);
        qmx_hal_lpuart_init(HAL_LPUART0, &lpuart_init);
        qmx_hal_lpuart_enable_irq(HAL_LPUART0, HAL_LPUART_INT_EN_RX_DONE);

        ret = qmx_lpuart_test_trx_common(HAL_LPUART0);
        if (ret != 0) {
            PRINTF("LPUART%u stopbit %u test failed.\n", HAL_LPUART0, i);
            return -1;
        }
    }

    PRINTF("LPUART%u stopbit test passed.\n", HAL_LPUART0);
    return 0;
}

int qmx_lpuart_parity_test(void)
{
    int i, ret;
    hal_lpuart_init_t lpuart_init = {0};
    uint8_t tx_data[QMX_TEST_FRAME_DATA_LEN] = {0};
    uint8_t rx_data[QMX_TEST_FRAME_DATA_LEN] = {0};
    uint8_t rx_len = 0;

    qmx_hal_register_irq_handler(LPUART_IRQ, qmx_lpuart_test_irq);
    QMX_HAL_ENABLE_PERIPHERAL_IRQ(LPUART_IRQ, 0x3);

    lpuart_init.baudrate = 4800;
    lpuart_init.stopbit = HAL_LPUART_STOPBIT_1;
    lpuart_init.datalen = HAL_LPUART_DATA_8BIT;
    lpuart_init.rx_fifo_thld = 1;

    tx_data[0] = lpuart_init.baudrate & 0xFF;
    tx_data[1] = (lpuart_init.baudrate >> 8) & 0xFF;
    tx_data[2] = (lpuart_init.baudrate >> 16) & 0xFF;
    tx_data[3] = (lpuart_init.baudrate >> 24) & 0xFF;
    tx_data[5] = lpuart_init.stopbit;
    tx_data[6] = lpuart_init.datalen;
    tx_data[7] = lpuart_init.rx_fifo_thld;
    tx_data[8] = HAL_LPUART0;

    for (i = HAL_LPUART_PARITY_NONE; i <= HAL_LPUART_PARITY_EVEN; i++) {
        rx_len = 0;
        memset(rx_data, 0, QMX_TEST_FRAME_DATA_LEN);

        lpuart_init.parity = (hal_lpuart_parity_e)i;
        tx_data[4] = lpuart_init.parity;

        qmx_test_master_send_cmd(QMX_TEST_CMD_LPUART_CFG, tx_data, QMX_TEST_FRAME_DATA_LEN);
        ret = qmx_test_master_get_result(rx_data, &rx_len);
        if ((ret != 0) || (rx_data[0] != 0)) {
            PRINTF("LPUART%u parity cfg cmd failed.\n", HAL_LPUART0);
            return -1;
        }

        qmx_hal_nop_delay_ms(QMX_TEST_CFG_TIMEOUT_MS);

        qmx_hal_sysctrl_peripheral_clk_enable(HAL_CLK_LPUART, true);
        qmx_hal_sysctrl_peripheral_mod_reset(HAL_CLK_LPUART);
        qmx_hal_lpuart_init(HAL_LPUART0, &lpuart_init);
        qmx_hal_lpuart_enable_irq(HAL_LPUART0, HAL_LPUART_INT_EN_RX_DONE);

        ret = qmx_lpuart_test_trx_common(HAL_LPUART0);
        if (ret != 0) {
            PRINTF("LPUART%u parity %u test failed.\n", HAL_LPUART0, i);
            return -1;
        }
    }

    PRINTF("LPUART%u parity test passed.\n", HAL_LPUART0);
    return 0;
}

int qmx_lpuart_complex_test(void)
{
    int i, ret, stopbit;
    hal_lpuart_init_t lpuart_init = {0};
    uint8_t tx_data[QMX_TEST_FRAME_DATA_LEN] = {0};
    uint8_t rx_data[QMX_TEST_FRAME_DATA_LEN] = {0};
    uint8_t rx_len = 0;

    qmx_hal_register_irq_handler(LPUART_IRQ, qmx_lpuart_test_irq);
    QMX_HAL_ENABLE_PERIPHERAL_IRQ(LPUART_IRQ, 0x3);

    lpuart_init.baudrate = 4800;
    lpuart_init.datalen = HAL_LPUART_DATA_8BIT;
    lpuart_init.rx_fifo_thld = 1;

    tx_data[0] = lpuart_init.baudrate & 0xFF;
    tx_data[1] = (lpuart_init.baudrate >> 8) & 0xFF;
    tx_data[2] = (lpuart_init.baudrate >> 16) & 0xFF;
    tx_data[3] = (lpuart_init.baudrate >> 24) & 0xFF;
    tx_data[6] = lpuart_init.datalen;
    tx_data[7] = lpuart_init.rx_fifo_thld;
    tx_data[8] = HAL_LPUART0;

    stopbit = HAL_LPUART_STOPBIT_1;
start:
    lpuart_init.stopbit = stopbit;
    tx_data[5] = lpuart_init.stopbit;
    for (i = HAL_LPUART_PARITY_NONE; i <= HAL_LPUART_PARITY_EVEN; i++) {
        rx_len = 0;
        memset(rx_data, 0, QMX_TEST_FRAME_DATA_LEN);

        lpuart_init.parity = (hal_lpuart_parity_e)i;
        tx_data[4] = lpuart_init.parity;

        qmx_test_master_send_cmd(QMX_TEST_CMD_LPUART_CFG, tx_data, QMX_TEST_FRAME_DATA_LEN);
        ret = qmx_test_master_get_result(rx_data, &rx_len);
        if ((ret != 0) || (rx_data[0] != 0)) {
            PRINTF("LPUART%u complex cfg cmd failed. parity %d, stopbit %d\n", HAL_LPUART0, i, stopbit);
            return -1;
        }

        qmx_hal_nop_delay_ms(QMX_TEST_CFG_TIMEOUT_MS);

        qmx_hal_sysctrl_peripheral_clk_enable(HAL_CLK_LPUART, true);
        qmx_hal_sysctrl_peripheral_mod_reset(HAL_CLK_LPUART);
        qmx_hal_lpuart_init(HAL_LPUART0, &lpuart_init);
        qmx_hal_lpuart_enable_irq(HAL_LPUART0, HAL_LPUART_INT_EN_RX_DONE);

        ret = qmx_lpuart_test_trx_common(HAL_LPUART0);
        if (ret != 0) {
            PRINTF("LPUART%u complex test failed. parity %d, stopbit %d\n", HAL_LPUART0, i, stopbit);
            return -1;
        }
    }

    if (stopbit < HAL_LPUART_STOPBIT_4) {
        stopbit++;
        goto start;
    }

    PRINTF("LPUART%u complex test passed.\n", HAL_LPUART0);
    return 0;
}
