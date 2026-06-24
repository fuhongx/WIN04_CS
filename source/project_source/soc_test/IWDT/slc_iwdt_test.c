#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>

#include "app_cfg.h"
#include "utility.h"
#include "reg_Dev.h"
#include "error_def.h"
#include "slc_iwdt_test.h"
#include "slc_hal_sysctrl.h"
#include "slc_hal_iwdt.h"
#include "slc_hal_intc.h"
#include "slc_hal_delay.h"

uint32_t g_test_iwdt_flag = 0;
static bool g_test_iwdt_first_feed_only = false;

void IWTD_IRQ_Handler(void)
{
    uint32_t sta = slc_hal_iwdt_get_interrupt_status();

    if (!g_test_iwdt_first_feed_only || (g_test_iwdt_flag == 0)) {
        slc_hal_iwdt_clear_interrupt(sta);
    }

    if (g_test_iwdt_flag == 0) {
        PRINTF("In IWTD Irq handler! status = 0x%X\n", sta);
        g_test_iwdt_flag = 1;
    }
}

int slc_iwdt_int_feed_reset_test(void)
{
    uint32_t timeout_s = 2;
    SLC_HAL_DISABLE_GLOBAL_IRQ();

    g_test_iwdt_flag = 0;
    g_test_iwdt_first_feed_only = true;

    hal_iwdt_cfg_t cfg;
    cfg.feed_mode = HAL_IWDT_FEED_BY_INT;
    cfg.rst_mode = HAL_IWDT_MODE_RST;
    cfg.timeout_cnt = (timeout_s * HAL_SYS_CLK_FREQ_32K);
    cfg.clk_div = 0;
    slc_hal_iwdt_init(&cfg);

    SLC_HAL_ENABLE_PERIPHERAL_IRQ(IWTD_IRQ, 0x3);
    SLC_HAL_ENABLE_GLOBAL_IRQ();

    /* 4、延时 2s，等待第 1 次超时 */
    slc_hal_nop_delay_ms(timeout_s * 1000);

    /* check 1：flag 置 1 且未复位 */
    if (g_test_iwdt_flag != 1) {
        PRINTF("[IWDT] int feed reset test fail! flag=%u, expect 1 after 1st timeout\n",
               g_test_iwdt_flag);
        slc_hal_iwdt_deinit();
        return -1;
    }
    PRINTF("[IWDT] 1st timeout: flag=1, no reset, check pass\n");

    /* 关闭 IRQ，避免第 2 次超时进 ISR 不清中断导致卡死，仅观察是否复位 */
    SLC_HAL_DISABLE_PERIPHERAL_IRQ(IWTD_IRQ);

    /* 5、等待第 2 次超时（约 4s）：不应复位 */
    slc_hal_nop_delay_ms(timeout_s * 1000);
    PRINTF("[IWDT] 2nd timeout: still alive, no reset, check pass\n");

    /* 等待第 3 次超时（约 6s）：应复位 */
    PRINTF("[IWDT] wait 3rd timeout (~%us), expect reset\n", 3 * timeout_s);
    slc_hal_nop_delay_ms(timeout_s * 1000);
    PRINTF("[IWDT] int feed reset test fail! still alive after 3rd timeout\n");
    slc_hal_iwdt_deinit();
    return -1;
}

int slc_iwdt_feed_reset_test(void)
{
    uint32_t timeout = 5;
    SLC_HAL_DISABLE_GLOBAL_IRQ();

    hal_iwdt_cfg_t cfg;
    cfg.feed_mode = HAL_IWDT_FEED_BY_REG;
    cfg.rst_mode = HAL_IWDT_MODE_RST;
    cfg.timeout_cnt = (timeout * HAL_SYS_CLK_FREQ_32K);
    cfg.clk_div = 0;
    slc_hal_iwdt_init(&cfg);

    while (timeout--) {
        slc_hal_nop_delay_ms(1000);
        slc_hal_iwdt_feed();
        PRINTF("[IWDT] Feed dog, cnt %d\n", timeout);
    }

    PRINTF("[IWDT] Wait for reset!\n");
    __WFI();
    return 0;
}

int slc_iwdt_int_feed_noreset_test(void)
{
    uint32_t timeout = 2;
    SLC_HAL_DISABLE_GLOBAL_IRQ();

    g_test_iwdt_flag = 0;
    g_test_iwdt_first_feed_only = false;

    hal_iwdt_cfg_t cfg;
    cfg.feed_mode = HAL_IWDT_FEED_BY_INT;
    cfg.rst_mode = HAL_IWDT_MODE_NO_RST;
    cfg.timeout_cnt = (timeout * HAL_SYS_CLK_FREQ_32K);
    cfg.clk_div = 0;
    slc_hal_iwdt_init(&cfg);

    SLC_HAL_ENABLE_PERIPHERAL_IRQ(IWTD_IRQ, 0x3);
    SLC_HAL_ENABLE_GLOBAL_IRQ();

    slc_hal_nop_delay_ms(timeout * 1000);

    if (g_test_iwdt_flag == 1) {
        PRINTF("[IWDT] Interrupt occurred as expected.\n");
    } else {
        PRINTF("[IWDT] Interrupt did not occur within the expected time.\n");
        slc_hal_iwdt_deinit();
        return -1;
    }

    PRINTF("[IWDT] after %ds expect not reset!\n", 2*timeout);

    slc_hal_nop_delay_ms(2 * timeout * 1000);
    PRINTF("[IWDT] Test success!\n");

    slc_hal_iwdt_deinit();
    return 0;
}

int slc_iwdt_feed_noreset_test(void)
{
    uint32_t timeout = 5;
    SLC_HAL_DISABLE_GLOBAL_IRQ();

    hal_iwdt_cfg_t cfg;
    cfg.feed_mode = HAL_IWDT_FEED_BY_REG;
    cfg.rst_mode = HAL_IWDT_MODE_NO_RST;
    cfg.timeout_cnt = (timeout * HAL_SYS_CLK_FREQ_32K);
    cfg.clk_div = 0;
    slc_hal_iwdt_init(&cfg);

    while (timeout--) {
        slc_hal_nop_delay_ms(1000);
        slc_hal_iwdt_feed();
        PRINTF("[IWDT] Feed %ds\n", timeout);
    }

    timeout = 5;
    slc_hal_iwdt_clear_interrupt(HAL_IWDT_INT_TIMEOUT_MASK | HAL_IWDT_INT_WRITE_PROTECT_MASK);
    PRINTF("[IWDT] after %ds expect not reset!\n", timeout);
    slc_hal_nop_delay_ms(timeout * 1000);
    PRINTF("[IWDT] Test success!\n");
    slc_hal_iwdt_deinit();
    return 0;
}

int slc_iwdt_deinit_test(void)
{
    uint32_t timeout_s = 2;
    SLC_HAL_DISABLE_GLOBAL_IRQ();

    g_test_iwdt_flag = 0;
    g_test_iwdt_first_feed_only = true;

    /* 1、中断喂狗 + 复位模式；2、超时 2s，使能中断 */
    hal_iwdt_cfg_t cfg;
    cfg.feed_mode = HAL_IWDT_FEED_BY_INT;
    cfg.rst_mode = HAL_IWDT_MODE_RST;
    cfg.timeout_cnt = (timeout_s * HAL_SYS_CLK_FREQ_32K);
    cfg.clk_div = 0;
    slc_hal_iwdt_init(&cfg);

    SLC_HAL_ENABLE_PERIPHERAL_IRQ(IWTD_IRQ, 0x3);
    SLC_HAL_ENABLE_GLOBAL_IRQ();

    /* 4、延时 2s，等待第 1 次超时（3、ISR 仅第 1 次清中断喂狗并置 flag） */
    slc_hal_nop_delay_ms(timeout_s * 1000);

    /* check 1：flag 置 1 且未复位 */
    if (g_test_iwdt_flag != 1) {
        PRINTF("[IWDT] deinit test fail! flag=%u, expect 1 after 1st timeout\n",
               g_test_iwdt_flag);
        slc_hal_iwdt_deinit();
        return -1;
    }
    PRINTF("[IWDT] check1 pass: flag=1, no reset\n");

    /* 5、去初始化 IWDT，等待 2 个超时（4s） */
    SLC_HAL_DISABLE_PERIPHERAL_IRQ(IWTD_IRQ);
    slc_hal_iwdt_deinit();
    slc_hal_nop_delay_ms(2 * timeout_s * 1000);

    /* check 2：deinit 后 2 个超时时间内不复位 */
    PRINTF("[IWDT] check2 pass: no reset within %us after deinit\n", 2 * timeout_s);
    PRINTF("[IWDT] deinit test success!\n");
    return 0;
}


/* 下面的用例直接调用底层驱动接口 */
#include "hw_wdt.h"
#define SLC_WDT_TEST_CNT        0x123456
#define SLC_WDT_TEST_CNT_BASE   0x1000000U
#define SLC_WDT_CNT_DRIFT_MAX   0x1000U

int slc_iwdt_write_protect_test(void)
{
    uint32_t before_cnt = 0;
    uint32_t after_cnt = 0;

    g_test_iwdt_flag = 0;
    PRINTF("[IWDT] write protect test start!\n");
    rom_hw_iwdt_timer_enable(false);

    /* 关闭写保护，写入应生效 */
    rom_hw_iwdt_enable_write(false);
    rom_hw_iwdt_set_counter(SLC_WDT_TEST_CNT_BASE);
    rom_hw_iwdt_get_counter(&before_cnt);
    rom_hw_iwdt_set_counter(before_cnt + SLC_WDT_TEST_CNT);
    rom_hw_iwdt_get_counter(&after_cnt);
    if ((after_cnt <= before_cnt) ||
        ((after_cnt - before_cnt) > SLC_WDT_TEST_CNT) ||
        ((SLC_WDT_TEST_CNT - (after_cnt - before_cnt)) > SLC_WDT_CNT_DRIFT_MAX)) {
        PRINTF("[IWDT] write protect disable test failed! before_cnt 0x%X, after_cnt 0x%X\n",
               before_cnt, after_cnt);
        return -1;
    }
    PRINTF("[IWDT] write protect disable test pass! before_cnt 0x%X, after_cnt 0x%X\n",
           before_cnt, after_cnt);

    /* 开启写保护，写入应被忽略 */
    rom_hw_iwdt_enable_write(true);
    rom_hw_iwdt_get_counter(&before_cnt);
    rom_hw_iwdt_set_counter(before_cnt + SLC_WDT_TEST_CNT);
    rom_hw_iwdt_get_counter(&after_cnt);
    slc_hal_nop_delay_us(100);
    if ((after_cnt - before_cnt) == SLC_WDT_TEST_CNT) {
        PRINTF("[IWDT] write protect enable test failed! before_cnt 0x%X, after_cnt 0x%X\n",
               before_cnt, after_cnt);
        return -1;
    }
    PRINTF("[IWDT] write protect enable test pass! before_cnt 0x%X, after_cnt 0x%X\n",
           before_cnt, after_cnt);
    PRINTF("[IWDT] write protect test success!\n");

    return 0;
}
