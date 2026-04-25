#include <stdint.h>
#include <stdio.h>

#include "app_cfg.h"
#include "utility.h"

#include "qmx_hal_sysctrl.h"
#include "qmx_hal_wdt.h"
#include "qmx_hal_intc.h"
#include "qmx_hal_delay.h"
#include "qmx_wdt_test.h"

uint32_t g_test_wdt_flag = 0;
void WTD_IRQ_Handler(void)
{
    uint32_t sta = 0;

    if (g_test_wdt_flag == 0) {
        sta = qmx_hal_wdt_get_interrupt_status();
        qmx_hal_wdt_clear_interrupt(sta);
        PRINTF("In WTD Irq handler! status = 0x%X, irq_cnt 0x%X\n", sta, g_test_wdt_flag);
        g_test_wdt_flag = 1;
    }
}

int qmx_wdt_int_feed_reset_test(void)
{
    uint32_t timeout = 2;
    QMX_HAL_DISABLE_GLOBAL_IRQ();

    qmx_hal_sysctrl_peripheral_clk_enable(HAL_CLK_WDT, true);

    g_test_wdt_flag = 0;
    hal_wdt_cfg_t cfg;
    cfg.feed_mode = HAL_WDT_FEED_BY_INT;
    cfg.rst_mode = HAL_WDT_MODE_RST;
    cfg.timeout_cnt = (timeout * HAL_SYS_CLK_FREQ_50M);
    qmx_hal_wdt_init(&cfg);

    QMX_HAL_ENABLE_PERIPHERAL_IRQ(WTD_IRQ, 0x3);
    QMX_HAL_ENABLE_GLOBAL_IRQ();

    PRINTF("[WDT] Wait for %ds!\n", timeout);

    qmx_hal_nop_delay_ms(timeout * 1000);

    if (g_test_wdt_flag == 1) {
        PRINTF("[WDT] WDT Interrupt occurred as expected.\n");
    } else {
        PRINTF("[WDT] WDT Interrupt did not occur within the expected time.\n");
        qmx_hal_wdt_deinit();
        return -1;
    }

    PRINTF("[WDT] Wait for reset!\n");
    while (1);
#ifdef __CC_ARM
#pragma diag_suppress 111
#endif
    return 0;
}

int qmx_wdt_feed_reset_test(void)
{
    uint32_t timeout = 5;
    QMX_HAL_DISABLE_GLOBAL_IRQ();

    qmx_hal_sysctrl_peripheral_clk_enable(HAL_CLK_WDT, true);

    hal_wdt_cfg_t cfg;
    cfg.feed_mode = HAL_WDT_FEED_BY_REG;
    cfg.rst_mode = HAL_WDT_MODE_RST;
    cfg.timeout_cnt = (timeout * HAL_SYS_CLK_FREQ_50M);
    qmx_hal_wdt_init(&cfg);

    while (timeout--) {
        qmx_hal_nop_delay_ms(1000);
        qmx_hal_wdt_feed();
        PRINTF("[WDT] WDT Feed %d s\n", timeout);
    }

    PRINTF("[WDT] Wait for reset!\n");
    while (1);
#ifdef __CC_ARM
#pragma diag_suppress 111
#endif
    return 0;
}

int qmx_wdt_int_feed_noreset_test(void)
{
    uint32_t timeout = 2;
    int ret = 0;
    QMX_HAL_DISABLE_GLOBAL_IRQ();

    qmx_hal_sysctrl_peripheral_clk_enable(HAL_CLK_WDT, true);

    g_test_wdt_flag = 0;
    hal_wdt_cfg_t cfg;
    cfg.feed_mode = HAL_WDT_FEED_BY_INT;
    cfg.rst_mode = HAL_WDT_MODE_NO_RST;
    cfg.timeout_cnt = (timeout * HAL_SYS_CLK_FREQ_50M);
    qmx_hal_wdt_init(&cfg);

    QMX_HAL_ENABLE_PERIPHERAL_IRQ(WTD_IRQ, 0x3);
    QMX_HAL_ENABLE_GLOBAL_IRQ();

    qmx_hal_nop_delay_ms(timeout * 1000);

    if (g_test_wdt_flag == 1) {
        PRINTF("[WDT] WDT Interrupt occurred as expected.\n");
    } else {
        PRINTF("[WDT] WDT Interrupt did not occur within the expected time.\n");
        ret = -1;
        goto end;
    }

    QMX_HAL_DISABLE_GLOBAL_IRQ();
    PRINTF("[WDT] after %ds expect not reset!\n", 2*timeout);
    qmx_hal_nop_delay_ms(2 * timeout * 1000);
    PRINTF("[WDT] Test success!\n");

end:
    qmx_hal_wdt_deinit();
    return ret;
}

int qmx_wdt_feed_noreset_test(void)
{
    uint32_t timeout = 5;
    QMX_HAL_DISABLE_GLOBAL_IRQ();

    qmx_hal_sysctrl_peripheral_clk_enable(HAL_CLK_WDT, true);

    hal_wdt_cfg_t cfg;
    cfg.feed_mode = HAL_WDT_FEED_BY_REG;
    cfg.rst_mode = HAL_WDT_MODE_NO_RST;
    cfg.timeout_cnt = (timeout * HAL_SYS_CLK_FREQ_50M);
    qmx_hal_wdt_init(&cfg);

    while (timeout--) {
        qmx_hal_nop_delay_ms(1000);
        qmx_hal_wdt_feed();
        PRINTF("[WDT] Feed dog, cnt %d!\n", timeout);
    }

    timeout = 5;
    PRINTF("[WDT] after %ds expect not reset!\n", timeout);
    qmx_hal_nop_delay_ms(timeout * 1000);
    qmx_hal_wdt_deinit();
    PRINTF("[WDT] Test success!\n");
    return 0;
}

int qmx_wdt_deinit_test(void)
{
    uint32_t timeout = 2;
    int ret = 0;
    QMX_HAL_DISABLE_GLOBAL_IRQ();

    qmx_hal_sysctrl_peripheral_clk_enable(HAL_CLK_WDT, true);

    g_test_wdt_flag = 0;
    hal_wdt_cfg_t cfg;
    cfg.feed_mode = HAL_WDT_FEED_BY_INT;
    cfg.rst_mode = HAL_WDT_MODE_RST;
    cfg.timeout_cnt = (timeout * HAL_SYS_CLK_FREQ_50M);
    qmx_hal_wdt_init(&cfg);

    QMX_HAL_ENABLE_PERIPHERAL_IRQ(WTD_IRQ, 0x3);
    QMX_HAL_ENABLE_GLOBAL_IRQ();

    qmx_hal_nop_delay_ms(timeout * 1000);

    if (g_test_wdt_flag == 1) {
        PRINTF("[WDT] WDT Interrupt occurred as expected.\n");
    } else {
        PRINTF("[WDT] WDT Interrupt did not occur within the expected time.\n");
        ret = -1;
    }

    qmx_hal_wdt_deinit();
    qmx_hal_nop_delay_ms(2 * timeout * 1000);
    PRINTF("[WDT] deinit test success!\n");
    return ret;
}

/* 下面的用例直接调用底层驱动接口 */
#include "hw_wdt.h"
#define QMX_WDT_TEST_CNT    0x123456
int qmx_wdt_write_protect_test(void)
{
    uint32_t before_cnt = 0;
    uint32_t after_cnt = 0;

    g_test_wdt_flag = 0;
    qmx_hal_sysctrl_peripheral_clk_enable(HAL_CLK_WDT, true);
    qmx_hal_sysctrl_peripheral_mod_reset(HAL_CLK_WDT);

    PRINTF("[WDT] write protect test start!\n");
    rom_hw_wdt_enable_write(true);
    rom_hw_wdt_get_counter(&before_cnt);
    rom_hw_wdt_set_counter(before_cnt + QMX_WDT_TEST_CNT);
    rom_hw_wdt_get_counter(&after_cnt);
    if ((after_cnt - before_cnt) == QMX_WDT_TEST_CNT) {
        PRINTF("[WDT] write protect enable test failed! before_cnt 0x%X, after_cnt 0x%X\n",
                before_cnt, after_cnt);
        return -1;
    }

    PRINTF("[WDT] write protect test success! before_cnt 0x%X, after_cnt 0x%X\n",
           before_cnt, after_cnt);
    return 0;
}
