#include <stdint.h>
#include <stdio.h>

#include "app_cfg.h"
#include "utility.h"
#include "reg_Dev.h"
#include "error_def.h"
#include "qmx_iwdt_test.h"
#include "qmx_hal_sysctrl.h"
#include "qmx_hal_iwdt.h"
#include "qmx_hal_intc.h"
#include "qmx_hal_delay.h"

uint32_t g_test_iwdt_flag = 0;
void IWTD_IRQ_Handler(void)
{
    uint32_t sta = 0;

    if (g_test_iwdt_flag == 0) {
        sta = qmx_hal_iwdt_get_interrupt_status();
        qmx_hal_iwdt_clear_interrupt(sta);
        PRINTF("In IWTD Irq handler! status = 0x%X, irq_cnt 0x%X\n", sta, g_test_iwdt_flag);
        g_test_iwdt_flag = 1;
    }
}

int qmx_iwdt_int_feed_reset_test(void)
{
    uint32_t timeout = 5;
    QMX_HAL_DISABLE_GLOBAL_IRQ();

    g_test_iwdt_flag = 0;
    hal_iwdt_cfg_t cfg;
    cfg.feed_mode = HAL_IWDT_FEED_BY_INT;
    cfg.rst_mode = HAL_IWDT_MODE_RST;
    cfg.timeout_cnt = (timeout * HAL_SYS_CLK_FREQ_32K);
    cfg.clk_div = 0;
    qmx_hal_iwdt_init(&cfg);

    QMX_HAL_ENABLE_PERIPHERAL_IRQ(IWTD_IRQ, 0x3);
    QMX_HAL_ENABLE_GLOBAL_IRQ();

    qmx_hal_nop_delay_ms(timeout * 1000);

    if (g_test_iwdt_flag == 1) {
        PRINTF("[IWDT] Interrupt occurred as expected.\n");
    } else {
        PRINTF("[IWDT] Interrupt did not occur within the expected time.\n");
        qmx_hal_iwdt_deinit();
        return -1;
    }

    PRINTF("[IWDT] Wait for reset!\n");
    __WFI();
    return 0;
}

int qmx_iwdt_feed_reset_test(void)
{
    uint32_t timeout = 5;
    QMX_HAL_DISABLE_GLOBAL_IRQ();

    hal_iwdt_cfg_t cfg;
    cfg.feed_mode = HAL_IWDT_FEED_BY_REG;
    cfg.rst_mode = HAL_IWDT_MODE_RST;
    cfg.timeout_cnt = (timeout * HAL_SYS_CLK_FREQ_32K);
    cfg.clk_div = 0;
    qmx_hal_iwdt_init(&cfg);

    while (timeout--) {
        qmx_hal_nop_delay_ms(1000);
        qmx_hal_iwdt_feed();
        PRINTF("[IWDT] Feed dog, cnt %d\n", timeout);
    }

    PRINTF("[IWDT] Wait for reset!\n");
    __WFI();
    return 0;
}

int qmx_iwdt_int_feed_noreset_test(void)
{
    uint32_t timeout = 2;
    QMX_HAL_DISABLE_GLOBAL_IRQ();

    g_test_iwdt_flag = 0;
    hal_iwdt_cfg_t cfg;
    cfg.feed_mode = HAL_IWDT_FEED_BY_INT;
    cfg.rst_mode = HAL_IWDT_MODE_NO_RST;
    cfg.timeout_cnt = (timeout * HAL_SYS_CLK_FREQ_32K);
    cfg.clk_div = 0;
    qmx_hal_iwdt_init(&cfg);

    QMX_HAL_ENABLE_PERIPHERAL_IRQ(IWTD_IRQ, 0x3);
    QMX_HAL_ENABLE_GLOBAL_IRQ();

    qmx_hal_nop_delay_ms(timeout * 1000);

    if (g_test_iwdt_flag == 1) {
        PRINTF("[IWDT] Interrupt occurred as expected.\n");
    } else {
        PRINTF("[IWDT] Interrupt did not occur within the expected time.\n");
        qmx_hal_iwdt_deinit();
        return -1;
    }

    PRINTF("[IWDT] after %ds expect not reset!\n", 2*timeout);

    qmx_hal_nop_delay_ms(2 * timeout * 1000);
    PRINTF("[IWDT] Test success!\n");

    qmx_hal_iwdt_deinit();
    return 0;
}

int qmx_iwdt_feed_noreset_test(void)
{
    uint32_t timeout = 5;
    QMX_HAL_DISABLE_GLOBAL_IRQ();

    hal_iwdt_cfg_t cfg;
    cfg.feed_mode = HAL_IWDT_FEED_BY_REG;
    cfg.rst_mode = HAL_IWDT_MODE_NO_RST;
    cfg.timeout_cnt = (timeout * HAL_SYS_CLK_FREQ_32K);
    cfg.clk_div = 0;
    qmx_hal_iwdt_init(&cfg);

    while (timeout--) {
        qmx_hal_nop_delay_ms(1000);
        qmx_hal_iwdt_feed();
        PRINTF("[IWDT] Feed %ds\n", timeout);
    }

    timeout = 5;
    qmx_hal_iwdt_clear_interrupt(HAL_IWDT_INT_TIMEOUT_MASK | HAL_IWDT_INT_WRITE_PROTECT_MASK);
    PRINTF("[IWDT] after %ds expect not reset!\n", timeout);
    qmx_hal_nop_delay_ms(timeout * 1000);
    PRINTF("[IWDT] Test success!\n");
    qmx_hal_iwdt_deinit();
    return 0;
}

int qmx_iwdt_deinit_test(void)
{
    uint32_t timeout = 2;
    QMX_HAL_DISABLE_GLOBAL_IRQ();

    g_test_iwdt_flag = 0;
    hal_iwdt_cfg_t cfg;
    cfg.feed_mode = HAL_IWDT_FEED_BY_INT;
    cfg.rst_mode = HAL_IWDT_MODE_RST;
    cfg.timeout_cnt = (timeout * HAL_SYS_CLK_FREQ_32K);
    cfg.clk_div = 0;
    qmx_hal_iwdt_init(&cfg);

    QMX_HAL_ENABLE_PERIPHERAL_IRQ(IWTD_IRQ, 0x3);
    QMX_HAL_ENABLE_GLOBAL_IRQ();

    qmx_hal_nop_delay_ms(timeout * 1000);

    if (g_test_iwdt_flag == 1) {
        PRINTF("[IWDT] Interrupt occurred as expected.\n");
    } else {
        PRINTF("[IWDT] Interrupt did not occur within the expected time.\n");
        qmx_hal_iwdt_deinit();
        return -1;
    }

    qmx_hal_iwdt_deinit();
    qmx_hal_nop_delay_ms(2 * timeout * 1000);
    PRINTF("[IWDT] deinit test success!\n");
    return 0;
}


/* 下面的用例直接调用底层驱动接口 */
#include "hw_wdt.h"
#define QMX_WDT_TEST_CNT    0x123456
int qmx_iwdt_write_protect_test(void)
{
    uint32_t before_cnt = 0;
    uint32_t after_cnt = 0;

    g_test_iwdt_flag = 0;
    PRINTF("[IWDT] write protect test start!\n");
    rom_hw_iwdt_enable_write(true);
    rom_hw_iwdt_timer_enable(false);

    rom_hw_iwdt_get_counter(&before_cnt);
    rom_hw_iwdt_set_counter(before_cnt + QMX_WDT_TEST_CNT);
    rom_hw_iwdt_get_counter(&after_cnt);
    if ((after_cnt - before_cnt) == QMX_WDT_TEST_CNT) {
        PRINTF("[IWDT] write protect enable test failed! before_cnt 0x%X, after_cnt 0x%X\n",
               before_cnt, after_cnt);
        return -1;
    }

    PRINTF("[IWDT] write protect test success! before_cnt 0x%X, after_cnt 0x%X\n",
           before_cnt, after_cnt);

    return 0;
}
