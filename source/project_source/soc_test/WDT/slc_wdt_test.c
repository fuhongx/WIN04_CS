#include <stdint.h>
#include <stdio.h>

#include "app_cfg.h"
#include "utility.h"

#include "slc_hal_sysctrl.h"
#include "slc_hal_wdt.h"
#include "slc_hal_intc.h"
#include "slc_hal_delay.h"
#include "slc_wdt_test.h"

uint32_t g_test_wdt_flag = 0;
void WTD_IRQ_Handler(void)
{
    uint32_t sta = 0;

    if (g_test_wdt_flag == 0) {
        sta = slc_hal_wdt_get_interrupt_status();
        slc_hal_wdt_clear_interrupt(sta);
        PRINTF("In WTD Irq handler! status = 0x%X, irq_cnt 0x%X\n", sta, g_test_wdt_flag);
        g_test_wdt_flag = 1;
    }
}

int slc_wdt_int_feed_reset_test(void)
{
    uint32_t timeout = 2;
    SLC_HAL_DISABLE_GLOBAL_IRQ();

    slc_hal_sysctrl_peripheral_clk_enable(HAL_CLK_WDT, true);

    g_test_wdt_flag = 0;
    hal_wdt_cfg_t cfg;
    cfg.feed_mode = HAL_WDT_FEED_BY_INT;
    cfg.rst_mode = HAL_WDT_MODE_RST;
    cfg.timeout_cnt = (timeout * HAL_SYS_CLK_FREQ_50M);
    slc_hal_wdt_init(&cfg);

    SLC_HAL_ENABLE_PERIPHERAL_IRQ(WTD_IRQ, 0x3);
    SLC_HAL_ENABLE_GLOBAL_IRQ();

    PRINTF("[WDT] Wait for %ds!\n", timeout);

    slc_hal_nop_delay_ms(timeout * 1000);

    if (g_test_wdt_flag == 1) {
        PRINTF("[WDT] WDT Interrupt occurred as expected.\n");
    } else {
        PRINTF("[WDT] WDT Interrupt did not occur within the expected time.\n");
        slc_hal_wdt_deinit();
        return -1;
    }

    PRINTF("[WDT] Wait for reset!\n");
    while (1);
#ifdef __CC_ARM
#pragma diag_suppress 111
#endif
    return 0;
}

int slc_wdt_feed_reset_test(void)
{
    uint32_t timeout = 5;
    SLC_HAL_DISABLE_GLOBAL_IRQ();

    slc_hal_sysctrl_peripheral_clk_enable(HAL_CLK_WDT, true);

    hal_wdt_cfg_t cfg;
    cfg.feed_mode = HAL_WDT_FEED_BY_REG;
    cfg.rst_mode = HAL_WDT_MODE_RST;
    cfg.timeout_cnt = (timeout * HAL_SYS_CLK_FREQ_50M);
    slc_hal_wdt_init(&cfg);

    while (timeout--) {
        slc_hal_nop_delay_ms(1000);
        slc_hal_wdt_feed();
        PRINTF("[WDT] WDT Feed %d s\n", timeout);
    }

    PRINTF("[WDT] Wait for reset!\n");
    while (1);
#ifdef __CC_ARM
#pragma diag_suppress 111
#endif
    return 0;
}

int slc_wdt_int_feed_noreset_test(void)
{
    uint32_t timeout = 2;
    int ret = 0;
    SLC_HAL_DISABLE_GLOBAL_IRQ();

    slc_hal_sysctrl_peripheral_clk_enable(HAL_CLK_WDT, true);

    g_test_wdt_flag = 0;
    hal_wdt_cfg_t cfg;
    cfg.feed_mode = HAL_WDT_FEED_BY_INT;
    cfg.rst_mode = HAL_WDT_MODE_NO_RST;
    cfg.timeout_cnt = (timeout * HAL_SYS_CLK_FREQ_50M);
    slc_hal_wdt_init(&cfg);

    SLC_HAL_ENABLE_PERIPHERAL_IRQ(WTD_IRQ, 0x3);
    SLC_HAL_ENABLE_GLOBAL_IRQ();

    slc_hal_nop_delay_ms(timeout * 1000);

    if (g_test_wdt_flag == 1) {
        PRINTF("[WDT] WDT Interrupt occurred as expected.\n");
    } else {
        PRINTF("[WDT] WDT Interrupt did not occur within the expected time.\n");
        ret = -1;
        goto end;
    }

    SLC_HAL_DISABLE_GLOBAL_IRQ();
    PRINTF("[WDT] after %ds expect not reset!\n", 2*timeout);
    slc_hal_nop_delay_ms(2 * timeout * 1000);
    PRINTF("[WDT] Test success!\n");

end:
    slc_hal_wdt_deinit();
    return ret;
}

int slc_wdt_feed_noreset_test(void)
{
    uint32_t timeout = 5;
    SLC_HAL_DISABLE_GLOBAL_IRQ();

    slc_hal_sysctrl_peripheral_clk_enable(HAL_CLK_WDT, true);

    hal_wdt_cfg_t cfg;
    cfg.feed_mode = HAL_WDT_FEED_BY_REG;
    cfg.rst_mode = HAL_WDT_MODE_NO_RST;
    cfg.timeout_cnt = (timeout * HAL_SYS_CLK_FREQ_50M);
    slc_hal_wdt_init(&cfg);

    while (timeout--) {
        slc_hal_nop_delay_ms(1000);
        slc_hal_wdt_feed();
        PRINTF("[WDT] Feed dog, cnt %d!\n", timeout);
    }

    timeout = 5;
    PRINTF("[WDT] after %ds expect not reset!\n", timeout);
    slc_hal_nop_delay_ms(timeout * 1000);
    slc_hal_wdt_deinit();
    PRINTF("[WDT] Test success!\n");
    return 0;
}

int slc_wdt_deinit_test(void)
{
    uint32_t timeout = 2;
    int ret = 0;
    SLC_HAL_DISABLE_GLOBAL_IRQ();

    slc_hal_sysctrl_peripheral_clk_enable(HAL_CLK_WDT, true);

    g_test_wdt_flag = 0;
    hal_wdt_cfg_t cfg;
    cfg.feed_mode = HAL_WDT_FEED_BY_INT;
    cfg.rst_mode = HAL_WDT_MODE_RST;
    cfg.timeout_cnt = (timeout * HAL_SYS_CLK_FREQ_50M);
    slc_hal_wdt_init(&cfg);

    SLC_HAL_ENABLE_PERIPHERAL_IRQ(WTD_IRQ, 0x3);
    SLC_HAL_ENABLE_GLOBAL_IRQ();

    slc_hal_nop_delay_ms(timeout * 1000);

    if (g_test_wdt_flag == 1) {
        PRINTF("[WDT] WDT Interrupt occurred as expected.\n");
    } else {
        PRINTF("[WDT] WDT Interrupt did not occur within the expected time.\n");
        ret = -1;
    }

    slc_hal_wdt_deinit();
    slc_hal_nop_delay_ms(2 * timeout * 1000);
    PRINTF("[WDT] deinit test success!\n");
    return ret;
}

/* 下面的用例直接调用底层驱动接口 */
#include "hw_wdt.h"
#define SLC_WDT_TEST_CNT    0x123456
int slc_wdt_write_protect_test(void)
{
    uint32_t before_cnt = 0;
    uint32_t after_cnt = 0;

    g_test_wdt_flag = 0;
    slc_hal_sysctrl_peripheral_clk_enable(HAL_CLK_WDT, true);
    slc_hal_sysctrl_peripheral_mod_reset(HAL_CLK_WDT);

    PRINTF("[WDT] write protect test start!\n");
    rom_hw_wdt_enable_write(true);
    rom_hw_wdt_get_counter(&before_cnt);
    rom_hw_wdt_set_counter(before_cnt + SLC_WDT_TEST_CNT);
    rom_hw_wdt_get_counter(&after_cnt);
    if ((after_cnt - before_cnt) == SLC_WDT_TEST_CNT) {
        PRINTF("[WDT] write protect enable test failed! before_cnt 0x%X, after_cnt 0x%X\n",
                before_cnt, after_cnt);
        return -1;
    }

    PRINTF("[WDT] write protect test success! before_cnt 0x%X, after_cnt 0x%X\n",
           before_cnt, after_cnt);
    return 0;
}
