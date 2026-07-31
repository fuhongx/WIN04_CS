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
#include "slc_hal_lpuart.h"
#include "slc_hal_sysctrl.h"
#include "slc_hal_gpio.h"
#include "slc_uart_test.h"
#include "slc_test_master.h"
#include "slc_hal_delay.h"
#include "slc_hal_rng.h"
#include "slc_rf.h"

void slc_lpuart_test_init(void)
{
    slc_hal_sysctrl_peripheral_clk_enable(HAL_CLK_RAND, true);
    slc_hal_sysctrl_peripheral_mod_reset(HAL_CLK_RAND);

    hal_rng_init_t rng_init = {0};
    rng_init.seed = 0x12345678;
    slc_hal_rng_init(&rng_init);
    slc_test_common_init();

    // RC32K时，9600概率性误码，切换到XTAL32K后正常
    slc_rf_enable_xtal32k(true);
    slc_hal_sysctrl_set_phy_pmu_clk(HAL_SYSCLK_LP_XTAL32K);
    slc_hal_sysctrl_set_apb1_clk(HAL_SYSCLK_LP_XTAL32K);

    slc_hal_gpio_set_iomux(HAL_GPIO_PIN18, HAL_IOMUX_MODE3);
    slc_hal_gpio_set_iomux(HAL_GPIO_PIN19, HAL_IOMUX_MODE3);
    slc_hal_lpio_set_iomux(HAL_GPIO_PIN18, HAL_IOMUX_MODE3);
    slc_hal_lpio_set_iomux(HAL_GPIO_PIN19, HAL_IOMUX_MODE3);
    slc_hal_gpio_set_mode(HAL_GPIO_PIN18, HAL_GPIO_PULL_UP);
    slc_hal_gpio_set_mode(HAL_GPIO_PIN19, HAL_GPIO_PULL_UP);
}

void slc_lpuart_test_deinit(void)
{
    slc_hal_rng_deinit();
    slc_hal_sysctrl_peripheral_clk_enable(HAL_CLK_RAND, false);
    slc_hal_sysctrl_peripheral_mod_reset(HAL_CLK_RAND);

    slc_test_common_deinit();

    slc_hal_sysctrl_set_phy_pmu_clk(HAL_SYSCLK_LP_XTAL32K);
    slc_hal_sysctrl_set_apb1_clk(HAL_SYSCLK_LP_RC32K);
    slc_rf_enable_xtal32k(false);

    slc_hal_lpuart_deinit(HAL_LPUART0);
    SLC_HAL_DISABLE_PERIPHERAL_IRQ(LPUART_IRQ);
}

volatile uint8_t g_lpuart_test_rx_buf[SLC_LPUART_TEST_TRX_LEN] = {0};
volatile uint32_t g_lpuart_test_rx_len = 0;
void slc_lpuart_test_irq(void)
{
    uint32_t status = 0;
    status = slc_hal_lpuart_get_irq_status(HAL_LPUART0);

    g_lpuart_test_rx_len = SLC_LPUART_TEST_TRX_LEN;
    slc_hal_lpuart_receive_data(HAL_LPUART0, (uint8_t *)g_lpuart_test_rx_buf, (uint32_t *)&g_lpuart_test_rx_len);

    slc_hal_lpuart_clear_irq_status(HAL_LPUART0, status);
}

int slc_lpuart_test_trx_common(hal_lpuart_id_e lpuart)
{
    uint8_t tx_data[SLC_LPUART_TEST_TRX_LEN] = {0};
    uint8_t tx_cmd[SLC_TEST_FRAME_DATA_LEN] = {0};
    uint8_t rx_cmd[SLC_TEST_FRAME_DATA_LEN] = {0};
    uint8_t rx_len = 0;
    int ret = 0;
    uint8_t timeout = 50;

    ret = slc_hal_get_random_u8(tx_data, SLC_LPUART_TEST_TRX_LEN, 1000);
    if (ret != 0) {
        PRINTF("get random u8 error, ret=0x%X\n", ret);
        return ret;
    }

    slc_hal_lpuart_send_data(lpuart, tx_data, SLC_LPUART_TEST_TRX_LEN);

    /* 1200bps 下 64 字节约 533ms，留足对端 RX 完成时间 */
    slc_hal_nop_delay_ms(700);

    g_lpuart_test_rx_len = 0;
    tx_cmd[0] = lpuart;
    tx_cmd[1] = 1;  // 0: send, 1: receive
    slc_test_master_send_cmd(SLC_TEST_CMD_LPUART_TRX, tx_cmd, SLC_TEST_FRAME_DATA_LEN);
    ret = slc_test_master_get_result(rx_cmd, &rx_len);
    if ((ret != 0) || (rx_cmd[0] != 0)) {
        PRINTF("LPUART%u TRX get data failed.\n", lpuart);
        return -1;
    }

    while (timeout--) {
        if (g_lpuart_test_rx_len != 0)
            break;

        slc_hal_nop_delay_ms(100);
    }

    if (g_lpuart_test_rx_len != SLC_LPUART_TEST_TRX_LEN) {
        PRINTF("LPUART%u rx len err, %u != %u.\n", lpuart, g_lpuart_test_rx_len, SLC_LPUART_TEST_TRX_LEN);
        dump_u8buf("tx", tx_data, SLC_LPUART_TEST_TRX_LEN);
        if (g_lpuart_test_rx_len != 0)
            dump_u8buf("rx", (uint8_t *)g_lpuart_test_rx_buf, g_lpuart_test_rx_len);
        return -1;
    }

    if (memcmp((void *)g_lpuart_test_rx_buf, tx_data, SLC_LPUART_TEST_TRX_LEN) != 0) {
        PRINTF("LPUART%u trx data is different.\n", lpuart);
        dump_u8buf("tx", tx_data, SLC_LPUART_TEST_TRX_LEN);
        dump_u8buf("rx", (uint8_t *)g_lpuart_test_rx_buf, SLC_LPUART_TEST_TRX_LEN);
        return -1;
    }

    return 0;
}

int slc_lpuart_baudrate_test(void)
{
    uint32_t baudrate[] = {
        1200, 2400, 3600, 4800, 9600
    };
    int i, ret;
    hal_lpuart_init_t lpuart_init = {0};
    uint8_t tx_data[SLC_TEST_FRAME_DATA_LEN] = {0};
    uint8_t rx_data[SLC_TEST_FRAME_DATA_LEN] = {0};
    uint8_t rx_len = 0;

    slc_hal_register_irq_handler(LPUART_IRQ, slc_lpuart_test_irq);
    SLC_HAL_ENABLE_PERIPHERAL_IRQ(LPUART_IRQ, 0x3);

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
        memset(rx_data, 0, SLC_TEST_FRAME_DATA_LEN);

        lpuart_init.baudrate = baudrate[i];
        tx_data[0] = lpuart_init.baudrate & 0xFF;
        tx_data[1] = (lpuart_init.baudrate >> 8) & 0xFF;
        tx_data[2] = (lpuart_init.baudrate >> 16) & 0xFF;
        tx_data[3] = (lpuart_init.baudrate >> 24) & 0xFF;

        slc_test_master_send_cmd(SLC_TEST_CMD_LPUART_CFG, tx_data, SLC_TEST_FRAME_DATA_LEN);
        ret = slc_test_master_get_result(rx_data, &rx_len);
        if ((ret != 0) || (rx_data[0] != 0)) {
            PRINTF("LPUART%u baudrate cfg cmd failed.\n", HAL_LPUART0);
            return -1;
        }

        slc_hal_nop_delay_ms(SLC_TEST_CFG_TIMEOUT_MS);

        slc_hal_sysctrl_peripheral_clk_enable(HAL_CLK_LPUART, true);
        slc_hal_sysctrl_peripheral_mod_reset(HAL_CLK_LPUART);
        slc_hal_lpuart_init(HAL_LPUART0, &lpuart_init);
        slc_hal_lpuart_enable_irq(HAL_LPUART0, HAL_LPUART_INT_EN_RX_DONE);

        ret = slc_lpuart_test_trx_common(HAL_LPUART0);
        if (ret != 0) {
            PRINTF("LPUART%u baudrate %u test failed.\n", HAL_LPUART0, baudrate[i]);
            return -1;
        }
    }

    PRINTF("LPUART%u baudrate test passed.\n", HAL_LPUART0);
    return 0;
}

int slc_lpuart_stopbit_test(void)
{
    int i, ret;
    hal_lpuart_init_t lpuart_init = {0};
    uint8_t tx_data[SLC_TEST_FRAME_DATA_LEN] = {0};
    uint8_t rx_data[SLC_TEST_FRAME_DATA_LEN] = {0};
    uint8_t rx_len = 0;

    slc_hal_register_irq_handler(LPUART_IRQ, slc_lpuart_test_irq);
    SLC_HAL_ENABLE_PERIPHERAL_IRQ(LPUART_IRQ, 0x3);

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
        memset(rx_data, 0, SLC_TEST_FRAME_DATA_LEN);

        lpuart_init.stopbit = (hal_lpuart_stopbit_e)i;
        tx_data[5] = lpuart_init.stopbit;

        slc_test_master_send_cmd(SLC_TEST_CMD_LPUART_CFG, tx_data, SLC_TEST_FRAME_DATA_LEN);
        ret = slc_test_master_get_result(rx_data, &rx_len);
        if ((ret != 0) || (rx_data[0] != 0)) {
            PRINTF("LPUART%u stopbit cfg cmd failed.\n", HAL_LPUART0);
            return -1;
        }

        slc_hal_nop_delay_ms(SLC_TEST_CFG_TIMEOUT_MS);

        slc_hal_sysctrl_peripheral_clk_enable(HAL_CLK_LPUART, true);
        slc_hal_sysctrl_peripheral_mod_reset(HAL_CLK_LPUART);
        slc_hal_lpuart_init(HAL_LPUART0, &lpuart_init);
        slc_hal_lpuart_enable_irq(HAL_LPUART0, HAL_LPUART_INT_EN_RX_DONE);

        ret = slc_lpuart_test_trx_common(HAL_LPUART0);
        if (ret != 0) {
            PRINTF("LPUART%u stopbit %u test failed.\n", HAL_LPUART0, i);
            return -1;
        }
    }

    PRINTF("LPUART%u stopbit test passed.\n", HAL_LPUART0);
    return 0;
}

int slc_lpuart_parity_test(void)
{
    int i, ret;
    hal_lpuart_init_t lpuart_init = {0};
    uint8_t tx_data[SLC_TEST_FRAME_DATA_LEN] = {0};
    uint8_t rx_data[SLC_TEST_FRAME_DATA_LEN] = {0};
    uint8_t rx_len = 0;

    slc_hal_register_irq_handler(LPUART_IRQ, slc_lpuart_test_irq);
    SLC_HAL_ENABLE_PERIPHERAL_IRQ(LPUART_IRQ, 0x3);

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
        memset(rx_data, 0, SLC_TEST_FRAME_DATA_LEN);

        lpuart_init.parity = (hal_lpuart_parity_e)i;
        tx_data[4] = lpuart_init.parity;

        slc_test_master_send_cmd(SLC_TEST_CMD_LPUART_CFG, tx_data, SLC_TEST_FRAME_DATA_LEN);
        ret = slc_test_master_get_result(rx_data, &rx_len);
        if ((ret != 0) || (rx_data[0] != 0)) {
            PRINTF("LPUART%u parity cfg cmd failed.\n", HAL_LPUART0);
            return -1;
        }

        slc_hal_nop_delay_ms(SLC_TEST_CFG_TIMEOUT_MS);

        slc_hal_sysctrl_peripheral_clk_enable(HAL_CLK_LPUART, true);
        slc_hal_sysctrl_peripheral_mod_reset(HAL_CLK_LPUART);
        slc_hal_lpuart_init(HAL_LPUART0, &lpuart_init);
        slc_hal_lpuart_enable_irq(HAL_LPUART0, HAL_LPUART_INT_EN_RX_DONE);

        ret = slc_lpuart_test_trx_common(HAL_LPUART0);
        if (ret != 0) {
            PRINTF("LPUART%u parity %u test failed.\n", HAL_LPUART0, i);
            return -1;
        }
    }

    PRINTF("LPUART%u parity test passed.\n", HAL_LPUART0);
    return 0;
}

int slc_lpuart_complex_test(void)
{
    int i, ret, stopbit;
    hal_lpuart_init_t lpuart_init = {0};
    uint8_t tx_data[SLC_TEST_FRAME_DATA_LEN] = {0};
    uint8_t rx_data[SLC_TEST_FRAME_DATA_LEN] = {0};
    uint8_t rx_len = 0;

    slc_hal_register_irq_handler(LPUART_IRQ, slc_lpuart_test_irq);
    SLC_HAL_ENABLE_PERIPHERAL_IRQ(LPUART_IRQ, 0x3);

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
        memset(rx_data, 0, SLC_TEST_FRAME_DATA_LEN);

        lpuart_init.parity = (hal_lpuart_parity_e)i;
        tx_data[4] = lpuart_init.parity;

        slc_test_master_send_cmd(SLC_TEST_CMD_LPUART_CFG, tx_data, SLC_TEST_FRAME_DATA_LEN);
        ret = slc_test_master_get_result(rx_data, &rx_len);
        if ((ret != 0) || (rx_data[0] != 0)) {
            PRINTF("LPUART%u complex cfg cmd failed. parity %d, stopbit %d\n", HAL_LPUART0, i, stopbit);
            return -1;
        }

        slc_hal_nop_delay_ms(SLC_TEST_CFG_TIMEOUT_MS);

        slc_hal_sysctrl_peripheral_clk_enable(HAL_CLK_LPUART, true);
        slc_hal_sysctrl_peripheral_mod_reset(HAL_CLK_LPUART);
        slc_hal_lpuart_init(HAL_LPUART0, &lpuart_init);
        slc_hal_lpuart_enable_irq(HAL_LPUART0, HAL_LPUART_INT_EN_RX_DONE);

        ret = slc_lpuart_test_trx_common(HAL_LPUART0);
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

static uint32_t slc_lpuart_data_mask(hal_lpuart_datalen_e datalen)
{
    return (1U << ((uint32_t)datalen + 5U)) - 1U;
}

static int slc_lpuart_loopback_trx_with_cfg(hal_lpuart_id_e lpuart, hal_lpuart_init_t *cfg)
{
    uint8_t tx_data[SLC_LPUART_TEST_TRX_LEN] = {0};
    uint8_t rx_data[SLC_LPUART_TEST_TRX_LEN] = {0};
    uint32_t trx_len = SLC_LPUART_TEST_TRX_LEN;
    uint32_t mask = slc_lpuart_data_mask(cfg->datalen);
    uint32_t rx_one = 1U;
    uint32_t i;
    int ret;

    ret = slc_hal_get_random_u8(tx_data, trx_len, 1000);
    if (ret != 0) {
        PRINTF("get random u8 error, ret=0x%X\n", ret);
        return ret;
    }

    for (i = 0; i < trx_len; i++) {
        tx_data[i] &= (uint8_t)mask;
    }

    /*
     * 单板环回：逐字节发/收，避免 RX FIFO 浅导致溢出；
     * 不用 RX 中断（RX_DONE 在首字节就会触发，原等待逻辑会误判）。
     */
    for (i = 0; i < trx_len; i++) {
        ret = slc_hal_lpuart_send_data(lpuart, &tx_data[i], 1U);
        if (ret != 0) {
            PRINTF("LPUART%u loopback tx fail at %u, ret=%d\n", lpuart, i, ret);
            return -1;
        }

        rx_one = 1U;
        ret = slc_hal_lpuart_receive_data(lpuart, &rx_data[i], &rx_one);
        if (ret != 0 || rx_one != 1U) {
            PRINTF("LPUART%u loopback rx fail at %u, ret=%d, len=%u\n", lpuart, i, ret, rx_one);
            dump_u8buf("tx", tx_data, i + 1U);
            dump_u8buf("rx", rx_data, i);
            return -1;
        }

        if ((rx_data[i] & (uint8_t)mask) != tx_data[i]) {
            PRINTF("LPUART%u loopback data mismatch at %u\n", lpuart, i);
            dump_u8buf("tx", tx_data, trx_len);
            dump_u8buf("rx", rx_data, trx_len);
            return -1;
        }
    }

    return 0;
}

/**
 * @brief LPUART 单板环回测试：PIN18 与 PIN19 短接，遍历数据位宽(5~8)与停止位(1~4)。
 * @return 0: 通过; -1: 失败
 */
int slc_lpuart_single_board_frame_test(void)
{
    hal_lpuart_init_t lpuart_init = {0};
    int datalen;
    int stopbit;
    int ret;

    PRINTF("LPUART single board test: short PIN18 and PIN19 for TX/RX loopback\n");

    SLC_HAL_DISABLE_PERIPHERAL_IRQ(LPUART_IRQ);

    lpuart_init.baudrate = 4800;
    lpuart_init.parity = HAL_LPUART_PARITY_NONE;
    lpuart_init.rx_fifo_thld = 1;

    for (datalen = HAL_LPUART_DATA_5BIT; datalen <= HAL_LPUART_DATA_8BIT; datalen++) {
        for (stopbit = HAL_LPUART_STOPBIT_1; stopbit <= HAL_LPUART_STOPBIT_4; stopbit++) {
            lpuart_init.datalen = (hal_lpuart_datalen_e)datalen;
            lpuart_init.stopbit = (hal_lpuart_stopbit_e)stopbit;

            PRINTF("LPUART loopback: datalen=%u stopbit=%u\n",
                   (uint32_t)datalen + 5U, (uint32_t)stopbit + 1U);

            slc_hal_sysctrl_peripheral_clk_enable(HAL_CLK_LPUART, true);
            slc_hal_sysctrl_peripheral_mod_reset(HAL_CLK_LPUART);
            slc_hal_lpuart_init(HAL_LPUART0, &lpuart_init);

            ret = slc_lpuart_loopback_trx_with_cfg(HAL_LPUART0, &lpuart_init);
            if (ret != 0) {
                PRINTF("LPUART loopback failed, datalen=%u stopbit=%u\n",
                       (uint32_t)datalen + 5U, (uint32_t)stopbit + 1U);
                return -1;
            }
        }
    }

    PRINTF("LPUART single board frame test passed.\n");
    return 0;
}

#define SLC_LPUART_RX_FIFO_THLD_MAX     (4U)
#define SLC_LPUART_RX_FIFO_BAUDRATE     (9600U)
#define SLC_LPUART_RX_FIFO_IRQ_TIMEOUT  (50U)

static volatile uint32_t g_lpuart_fifo_irq_sta = 0;
static volatile uint32_t g_lpuart_fifo_irq_cnt = 0;
static volatile uint8_t g_lpuart_fifo_rx_buf[SLC_LPUART_RX_FIFO_THLD_MAX] = {0};
static volatile uint32_t g_lpuart_fifo_rx_len = 0;

static void slc_lpuart_rx_fifo_thld_irq(void)
{
    uint32_t status = slc_hal_lpuart_get_irq_status(HAL_LPUART0);

    g_lpuart_fifo_irq_sta |= status;
    g_lpuart_fifo_irq_cnt++;

    if (status & HAL_LPUART_ISR_RX_PTHLD) {
        g_lpuart_fifo_rx_len = SLC_LPUART_RX_FIFO_THLD_MAX;
        slc_hal_lpuart_receive_data(HAL_LPUART0, (uint8_t *)g_lpuart_fifo_rx_buf,
                                    (uint32_t *)&g_lpuart_fifo_rx_len);
    }

    slc_hal_lpuart_clear_irq_status(HAL_LPUART0, status);
}

static void slc_lpuart_rx_fifo_clear_irq_ctx(void)
{
    g_lpuart_fifo_irq_sta = 0;
    g_lpuart_fifo_irq_cnt = 0;
    g_lpuart_fifo_rx_len = 0;
    memset((void *)g_lpuart_fifo_rx_buf, 0, sizeof(g_lpuart_fifo_rx_buf));
}

static int slc_lpuart_rx_fifo_cfg_both(uint8_t rx_thld)
{
    uint8_t tx_data[SLC_TEST_FRAME_DATA_LEN] = {0};
    uint8_t rx_data[SLC_TEST_RX_MAX_LEN] = {0};
    uint8_t rx_len = 0;
    int ret;
    hal_lpuart_init_t lpuart_init = {0};

    lpuart_init.baudrate = SLC_LPUART_RX_FIFO_BAUDRATE;
    lpuart_init.parity = HAL_LPUART_PARITY_NONE;
    lpuart_init.stopbit = HAL_LPUART_STOPBIT_1;
    lpuart_init.datalen = HAL_LPUART_DATA_8BIT;
    lpuart_init.rx_fifo_thld = rx_thld;

    tx_data[0] = (uint8_t)(lpuart_init.baudrate & 0xFF);
    tx_data[1] = (uint8_t)((lpuart_init.baudrate >> 8) & 0xFF);
    tx_data[2] = (uint8_t)((lpuart_init.baudrate >> 16) & 0xFF);
    tx_data[3] = (uint8_t)((lpuart_init.baudrate >> 24) & 0xFF);
    tx_data[4] = lpuart_init.parity;
    tx_data[5] = lpuart_init.stopbit;
    tx_data[6] = lpuart_init.datalen;
    tx_data[7] = lpuart_init.rx_fifo_thld;
    tx_data[8] = HAL_LPUART0;

    slc_test_master_send_cmd(SLC_TEST_CMD_LPUART_CFG, tx_data, SLC_TEST_FRAME_DATA_LEN);
    ret = slc_test_master_get_result(rx_data, &rx_len);
    if ((ret != 0) || (rx_data[0] != 0)) {
        PRINTF("LPUART RX FIFO cfg slave failed, thld=%u\n", rx_thld);
        return -1;
    }
    slc_hal_nop_delay_ms(SLC_TEST_CFG_TIMEOUT_MS);

    slc_hal_sysctrl_peripheral_clk_enable(HAL_CLK_LPUART, true);
    slc_hal_sysctrl_peripheral_mod_reset(HAL_CLK_LPUART);
    slc_hal_lpuart_init(HAL_LPUART0, &lpuart_init);

    slc_hal_lpuart_disable_irq(HAL_LPUART0, 0xFF);
    slc_hal_lpuart_clear_irq_status(HAL_LPUART0, slc_hal_lpuart_get_irq_status(HAL_LPUART0));
    slc_hal_register_irq_handler(LPUART_IRQ, slc_lpuart_rx_fifo_thld_irq);
    slc_hal_lpuart_enable_irq(HAL_LPUART0, HAL_LPUART_INT_EN_RX_PTHLD);
    SLC_HAL_ENABLE_PERIPHERAL_IRQ(LPUART_IRQ, 0x3);

    return 0;
}

static int slc_lpuart_rx_fifo_slave_send(uint8_t count, uint8_t pattern_start)
{
    uint8_t tx_data[SLC_TEST_FRAME_DATA_LEN] = {0};
    uint8_t rx_data[SLC_TEST_RX_MAX_LEN] = {0};
    uint8_t rx_len = 0;
    int ret;

    tx_data[0] = count;
    tx_data[1] = pattern_start;

    slc_test_master_send_cmd(SLC_TEST_CMD_LPUART_RX_FIFO, tx_data, SLC_TEST_FRAME_DATA_LEN);
    ret = slc_test_master_get_result(rx_data, &rx_len);
    if ((ret != 0) || (rx_data[0] != 0)) {
        PRINTF("LPUART RX FIFO slave send cmd failed, cnt=%u\n", count);
        return -1;
    }

    return 0;
}

static int slc_lpuart_rx_fifo_wait_pthld(uint8_t expect_len, uint8_t pattern_start)
{
    uint8_t timeout = SLC_LPUART_RX_FIFO_IRQ_TIMEOUT;
    uint32_t i;

    while (timeout--) {
        if ((g_lpuart_fifo_irq_cnt != 0) && (g_lpuart_fifo_irq_sta & HAL_LPUART_ISR_RX_PTHLD)) {
            break;
        }
        slc_hal_nop_delay_ms(10);
    }

    if (!(g_lpuart_fifo_irq_sta & HAL_LPUART_ISR_RX_PTHLD)) {
        PRINTF("LPUART RX FIFO thld irq not triggered, sta=0x%X cnt=%u expect_len=%u\n",
               g_lpuart_fifo_irq_sta, g_lpuart_fifo_irq_cnt, expect_len);
        return -1;
    }

    if (g_lpuart_fifo_rx_len != expect_len) {
        PRINTF("LPUART RX FIFO len err, %u != %u\n", g_lpuart_fifo_rx_len, expect_len);
        if (g_lpuart_fifo_rx_len != 0) {
            dump_u8buf("rx", (uint8_t *)g_lpuart_fifo_rx_buf, g_lpuart_fifo_rx_len);
        }
        return -1;
    }

    for (i = 0; i < expect_len; i++) {
        if (g_lpuart_fifo_rx_buf[i] != (uint8_t)(pattern_start + i)) {
            PRINTF("LPUART RX FIFO data mismatch at %u, 0x%02X != 0x%02X\n",
                   i, g_lpuart_fifo_rx_buf[i], (uint8_t)(pattern_start + i));
            dump_u8buf("rx", (uint8_t *)g_lpuart_fifo_rx_buf, expect_len);
            return -1;
        }
    }

    return 0;
}

/**
 * @brief LPUART RX FIFO 水线双板测试
 *
 * 设计逻辑
 * 1、9600/8N1，遍历 RX 阈值 1~4，仅使能 RX_PTHLD 中断
 * 2、Slave 通过 LPUART 发送对应字节数（替代上位机）
 * 3、阈值>1 时先发 thld-1 确认未触发，再补 1 字节触发水线
 *
 * check逻辑
 * 1、水线中断是否按阈值触发
 * 2、接收长度与数据是否正确
 *
 * 接线：双板 PIN18/PIN19 交叉（LPUART TX/RX），协议口仍用测试 UART
 */
int slc_lpuart_rx_fifo_thld_test(void)
{
    uint8_t thld;
    uint8_t pattern = 0xA0;

    PRINTF("LPUART RX FIFO thld test: dual-board, cross PIN18/PIN19\n");

    for (thld = 1; thld <= SLC_LPUART_RX_FIFO_THLD_MAX; thld++) {
        PRINTF("LPUART RX FIFO thld=%u start\n", thld);

        if (slc_lpuart_rx_fifo_cfg_both(thld) != 0) {
            return -1;
        }
        slc_lpuart_rx_fifo_clear_irq_ctx();

        if (thld > 1) {
            if (slc_lpuart_rx_fifo_slave_send((uint8_t)(thld - 1U), pattern) != 0) {
                return -1;
            }
            /* 9600bps 下数字节传输很快，等待后确认未达水线 */
            slc_hal_nop_delay_ms(50);
            if ((g_lpuart_fifo_irq_cnt != 0) || (g_lpuart_fifo_irq_sta & HAL_LPUART_ISR_RX_PTHLD)) {
                PRINTF("LPUART RX FIFO false trigger before thld, thld=%u sta=0x%X cnt=%u\n",
                       thld, g_lpuart_fifo_irq_sta, g_lpuart_fifo_irq_cnt);
                return -1;
            }

            if (slc_lpuart_rx_fifo_slave_send(1, (uint8_t)(pattern + thld - 1U)) != 0) {
                return -1;
            }
        } else {
            if (slc_lpuart_rx_fifo_slave_send(thld, pattern) != 0) {
                return -1;
            }
        }

        if (slc_lpuart_rx_fifo_wait_pthld(thld, pattern) != 0) {
            PRINTF("LPUART RX FIFO thld=%u fail\n", thld);
            return -1;
        }

        PRINTF("LPUART RX FIFO thld=%u pass, irq_cnt=%u sta=0x%X\n",
               thld, g_lpuart_fifo_irq_cnt, g_lpuart_fifo_irq_sta);
        pattern = (uint8_t)(pattern + 0x10);
    }

    PRINTF("LPUART RX FIFO thld test passed.\n");
    return 0;
}
