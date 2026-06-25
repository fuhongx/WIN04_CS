#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include "reg_Dev.h"
#include "error_def.h"
#include "utility.h"
#include "compiler.h"
#include "debug.h"
#include "app_cfg.h"
#include "hw_flash.h"
#include "slc_hal_rtc.h"
#include "slc_hal_uart.h"
#include "slc_hal_lpuart.h"
#include "slc_hal_gpio.h"
#include "slc_hal_sysctrl.h"
#include "slc_hal_lptimer.h"
#include "slc_hal_pmu.h"
#include "slc_hal_intc.h"
#include "slc_hal_delay.h"
#include "slc_lpwr_ctrl.h"
#include "slc_low_power_test.h"

#ifdef NT_SHELL
#include "nttop.h"
#endif

// conot use GPIO_PIN20~GPIO_PIN21, these pins are used for SWD
#define TEST_GPIO_PIN       (HAL_GPIO_PIN17)

// 第一个backreg用作存储standby的回调地址，不可用作测试
#define TEST_BACKREG_ADDR   (ADDR_BACKREG_BASE + 0xC)
#define TEST_BACKREG_DATA   (0xABCD1234)

// BCD format, expect subsecond
#define TEST_RTC_YEAR       (0x2025)
#define TEST_RTC_MONTH      (0x10)
#define TEST_RTC_DAY        (0x1)
#define TEST_RTC_WEEK       (0x1)
#define TEST_RTC_HOUR       (0x10)
#define TEST_RTC_MINUTE     (0x0)
#define TEST_RTC_SECOND     (0x0)
#define TEST_RTC_SUBSECOND  (0)

uint8_t test_retention_data[8] __RETENTION_DATA = {0};
uint8_t test_recv_data[16] = {0};
uint32_t test_recv_len = 0;
static const char* week_days[] = {
    "Mon", "Tue", "Wed", "Thu", "Fri", "Sat", "Sun"
};

extern void slc_platform_init(void);

/* IRQ for test */
void LPTIMER0_IRQ_Handler(void)
{
    uint32_t sta = 0;

    sta = slc_hal_lptimer_get_irq_status(HAL_LPTIMER0);
    slc_hal_lptimer_clear_irq(HAL_LPTIMER0, sta);
}

void LPTIMER1_IRQ_Handler(void)
{
    uint32_t sta = 0;

    sta = slc_hal_lptimer_get_irq_status(HAL_LPTIMER1);
    slc_hal_lptimer_clear_irq(HAL_LPTIMER1, sta);
}

void LPUART_IRQ_Handler(void)
{
    uint32_t status = 0;
    status = slc_hal_lpuart_get_irq_status(HAL_LPUART0);
    slc_hal_lpuart_clear_irq_status(HAL_LPUART0, status);

    test_recv_len = 16;
    slc_hal_lpuart_receive_data(HAL_LPUART0, test_recv_data, &test_recv_len);
    dump_u8buf("lpuart recv data", test_recv_data, test_recv_len);
}

void RTC_IRQ_Handler(void)
{
    uint32_t status = 0;
    status = slc_hal_rtc_get_alarm_int_sta();
    slc_hal_rtc_clear_alarm_int_sta(status);
}
#include "slc_hal_pwm.h"
__RETENTION_FUNC bool before_sleep(void)
{
    PRINTF("enter before_sleep\n");

    write32(TEST_BACKREG_ADDR, TEST_BACKREG_DATA);
    for (int i = 0; i < sizeof(test_retention_data); i++)
    {
        test_retention_data[i] = i;
    }

    return true;
}

__RETENTION_FUNC void after_wakeup_stop(void)
{
    // 睡前拉低，醒后拉高，统计唤醒时间
    slc_hal_sysctrl_peripheral_clk_enable(HAL_CLK_GPIO, true);

#if APP_DEBUG_ENABLED
    debug_printf_init();
#endif

#ifdef NT_SHELL
    extern void slc_debug_uart_irq_handler(void);
    slc_hal_register_irq_handler(DEBUG_UART_IRQ, slc_debug_uart_irq_handler);
    slc_hal_uart_enable_irq(DEBUG_UART_HANDLE, HAL_UART_INT_EN_RX_NOT_EMPTY);
    SLC_HAL_ENABLE_PERIPHERAL_IRQ(DEBUG_UART_IRQ, 0x3);

    ntshell_top_init();
#endif

    if (read32(TEST_BACKREG_ADDR) != TEST_BACKREG_DATA)
        PRINTF("check backreg data error: 0x%X, expect 0x%X\n", read32(TEST_BACKREG_ADDR), TEST_BACKREG_DATA);
    else
        PRINTF("check backreg data ok: 0x%x\n", read32(TEST_BACKREG_ADDR));

    for (int i = 0; i < sizeof(test_retention_data); i++)
    {
        if (test_retention_data[i] != i)
            PRINTF("check retention data error: test_retention_data[%d] = 0x%X, expect 0x%X\n", i, test_retention_data[i], i);
        else
            PRINTF("check retention data ok: test_retention_data[%d] = 0x%X\n", i, test_retention_data[i]);
    }

    if (test_recv_len != 0)
        dump_u8buf("lpuart recv data", test_recv_data, test_recv_len);
    else
        PRINTF("no lpuart data received\n");
}

__RETENTION_FUNC void after_wakeup(void)
{
    // 睡前拉低，醒后拉高，统计唤醒时间
    slc_hal_sysctrl_peripheral_clk_enable(HAL_CLK_GPIO, true);

    if (read32(TEST_BACKREG_ADDR) != TEST_BACKREG_DATA)
        PRINTF("check backreg data error: 0x%X, expect 0x%X\n", read32(TEST_BACKREG_ADDR), TEST_BACKREG_DATA);
    else
        PRINTF("check backreg data ok: 0x%x\n", read32(TEST_BACKREG_ADDR));

    for (int i = 0; i < sizeof(test_retention_data); i++)
    {
        if (test_retention_data[i] != i)
            PRINTF("check retention data error: test_retention_data[%d] = 0x%X, expect 0x%X\n", i, test_retention_data[i], i);
        else
            PRINTF("check retention data ok: test_retention_data[%d] = 0x%X\n", i, test_retention_data[i]);
    }
}

__RETENTION_FUNC void after_wakeup_standby(void)
{
    // 睡前拉低，醒后拉高，统计唤醒时间
    slc_hal_sysctrl_peripheral_clk_enable(HAL_CLK_GPIO, true);
    slc_hal_gpio_set_iomux(TEST_GPIO_PIN, HAL_IOMUX_GPIO);
    slc_hal_gpio_set_direction(TEST_GPIO_PIN, HAL_GPIO_OUTPUT);
    slc_hal_gpio_set_value(TEST_GPIO_PIN, HAL_GPIO_LEVEL_LOW);

#if APP_DEBUG_ENABLED
    debug_printf_init();
#endif

    if (read32(TEST_BACKREG_ADDR) != TEST_BACKREG_DATA)
        PRINTF("check backreg data error: 0x%X, expect 0x%X\n", read32(TEST_BACKREG_ADDR), TEST_BACKREG_DATA);
    else
        PRINTF("check backreg data ok: 0x%x\n", read32(TEST_BACKREG_ADDR));

    if (test_recv_len != 0)
        dump_u8buf("lpuart recv data", test_recv_data, test_recv_len);
    else
        PRINTF("no lpuart data received\n");
}

void wakeup_by_lptimer(hal_lptimer_id_e lptimer_id)
{
    hal_peripheral_clk_e lptimer_gate = (lptimer_id == HAL_LPTIMER0) ? HAL_CLK_LPTIM0 : HAL_CLK_LPTIM1;
    uint32_t lptimer_irq = (lptimer_id == HAL_LPTIMER0) ? LPTIMER0_IRQ : LPTIMER1_IRQ;

    slc_hal_sysctrl_peripheral_clk_enable(lptimer_gate, true);
    slc_hal_sysctrl_peripheral_mod_reset(lptimer_gate);

    slc_hal_lptimer_init(lptimer_id, HAL_SYS_CLK_FREQ_32K * 2, true);

    if (lptimer_id == HAL_LPTIMER0)
        slc_hal_register_irq_handler(LPTIMER0_IRQ, LPTIMER0_IRQ_Handler);
    else
        slc_hal_register_irq_handler(LPTIMER1_IRQ, LPTIMER1_IRQ_Handler);

    SLC_HAL_ENABLE_PERIPHERAL_IRQ((IRQn_Type)lptimer_irq, 0x3);

    slc_hal_lptimer_start(lptimer_id);

    PRINTF("--Lptimer%d init done!--\n", lptimer_id);
}

void wakeup_by_lpuart(void)
{
    slc_hal_sysctrl_peripheral_clk_enable(HAL_CLK_LPUART, true);

    slc_hal_gpio_set_iomux(HAL_GPIO_PIN18, HAL_IOMUX_MODE3);
    slc_hal_gpio_set_iomux(HAL_GPIO_PIN19, HAL_IOMUX_MODE3);
    slc_hal_lpio_set_iomux(HAL_GPIO_PIN18, HAL_IOMUX_MODE3);
    slc_hal_lpio_set_iomux(HAL_GPIO_PIN19, HAL_IOMUX_MODE3);
    slc_hal_gpio_set_mode(HAL_GPIO_PIN18, HAL_GPIO_PULL_UP);
    slc_hal_gpio_set_mode(HAL_GPIO_PIN19, HAL_GPIO_PULL_UP);

    hal_lpuart_init_t lpuart_init;
    lpuart_init.baudrate = 9600;
    lpuart_init.parity = HAL_LPUART_PARITY_NONE;
    lpuart_init.stopbit = HAL_LPUART_STOPBIT_1;
    lpuart_init.datalen = HAL_LPUART_DATA_8BIT;
    lpuart_init.rx_fifo_thld = 1;

    slc_hal_lpuart_init(HAL_LPUART0, &lpuart_init);
    slc_hal_lpuart_enable_irq(HAL_LPUART0, HAL_LPUART_INT_EN_RX_DONE);
    slc_hal_register_irq_handler(LPUART_IRQ, LPUART_IRQ_Handler);
    SLC_HAL_ENABLE_PERIPHERAL_IRQ(LPUART_IRQ, 0x3);

    PRINTF("--Lpuart init done, baud %d--\n", 9600);
}

void wakeup_by_rtc(void)
{
    hal_rtc_calendar_t rtc_time;
    hal_rtc_alarm_msk_t alarm_mask = {0};

    // BCD format: year/month/day(week) hour:minute:second.subsecond
    rtc_time.year = TEST_RTC_YEAR;
    rtc_time.month = TEST_RTC_MONTH;
    rtc_time.day = TEST_RTC_DAY;
    rtc_time.week = TEST_RTC_WEEK;
    rtc_time.hour = TEST_RTC_HOUR;
    rtc_time.minute = TEST_RTC_MINUTE;
    rtc_time.second = TEST_RTC_SECOND;
    rtc_time.subsecond = TEST_RTC_SUBSECOND;

    slc_hal_rtc_stop();
    slc_hal_rtc_set_calendar(&rtc_time);

    PRINTF("--RTC init done, start at %04X/%02X/%02X(%s) %02X:%02X:%02X.%d--\n",
           rtc_time.year, rtc_time.month, rtc_time.day, week_days[rtc_time.week],
           rtc_time.hour, rtc_time.minute, rtc_time.second, rtc_time.subsecond);

    alarm_mask.year_mask = 0;
    alarm_mask.month_mask = 0;
    alarm_mask.day_mask = 0;
    alarm_mask.week_mask = 0;
    alarm_mask.hour_mask = 0;
    alarm_mask.minute_mask = 0;
    alarm_mask.second_mask = 1;
    alarm_mask.subsecond_mask = 1;

    rtc_time.second += 0x2;

    slc_hal_rtc_set_alarm(HAL_RTC_ALARM0, &alarm_mask, &rtc_time);
    slc_hal_rtc_enable_alarm(HAL_RTC_ALARM0);
    slc_hal_register_irq_handler(RTC_IRQ, RTC_IRQ_Handler);
    SLC_HAL_ENABLE_PERIPHERAL_IRQ(RTC_IRQ, 0x3);

    slc_hal_rtc_start();
}

void slc_lpwr_test_lpio_irq(void)
{
    SLC_HAL_DISABLE_PERIPHERAL_IRQ(LPIO_IRQ);
    slc_hal_lpio_clear_interrupt();
}

void wakeup_by_lpio(hal_gpio_pin_e pin)
{
    slc_hal_register_irq_handler(LPIO_IRQ, slc_lpwr_test_lpio_irq);

    slc_hal_gpio_set_iomux(pin, HAL_IOMUX_GPIO);
    slc_hal_gpio_set_mode(pin, HAL_GPIO_PULL_UP);   // 设置上拉，避免悬空导致电平不稳定
    slc_hal_gpio_set_direction(pin, HAL_GPIO_INPUT);

    slc_hal_pmu_lpio_enable(true);
    slc_hal_lpio_set_iomux(pin, HAL_IOMUX_GPIO);
    slc_hal_lpio_set_mode(pin, HAL_GPIO_PULL_UP);
    slc_hal_lpio_set_direction(pin, HAL_GPIO_INPUT);

    slc_hal_lpio_set_interrupt(pin, HAL_GPIO_INTR_LOW_LEVEL);
    slc_hal_lpio_enable_interrupt(pin, true);
    slc_hal_lpio_clear_interrupt();
    SLC_HAL_ENABLE_PERIPHERAL_IRQ(LPIO_IRQ, 0x3);

    PRINTF("--wakeup by Lpio%d--\n", pin);
}

void wakeup_src_select(uint32_t lp_wakeup_src_msk)
{
    // 清除残留值，确保下一次测试不会被影响
    write32(TEST_BACKREG_ADDR, 0);
    for (int i = 0; i < sizeof(test_retention_data); i++)
    {
        test_retention_data[i] = 0;
    }

    if (lp_wakeup_src_msk & HAL_PMU_LP_WAKEUP_SRC_MSK_PHY0) {
        // TODO: phy int0
    }
    if (lp_wakeup_src_msk & HAL_PMU_LP_WAKEUP_SRC_MSK_LPTIMER1) {
        wakeup_by_lptimer(HAL_LPTIMER1);
    }
    if (lp_wakeup_src_msk & HAL_PMU_LP_WAKEUP_SRC_MSK_RTC) {
        wakeup_by_rtc();
    }
    if (lp_wakeup_src_msk & HAL_PMU_LP_WAKEUP_SRC_MSK_LPTIMER0) {
        wakeup_by_lptimer(HAL_LPTIMER0);
    }
    if (lp_wakeup_src_msk & HAL_PMU_LP_WAKEUP_SRC_MSK_LPUART) {
        wakeup_by_lpuart();
    }
    if (lp_wakeup_src_msk & HAL_PMU_LP_WAKEUP_SRC_MSK_RF) {
        // TODO: rf
    }
    if (lp_wakeup_src_msk & HAL_PMU_LP_WAKEUP_SRC_MSK_LPIO) {
        wakeup_by_lpio(HAL_GPIO_PIN14);
    }

    slc_hal_pmu_set_lp_wakeup_source(lp_wakeup_src_msk);
}

int low_power_sleep_phy_disable_test(void)
{
    wakeup_src_select(HAL_PMU_LP_WAKEUP_SRC_MSK_RTC);

    slc_lpwr_ctrl_cfg cfg;
    cfg.lp_wakeup_src_msk = HAL_PMU_LP_WAKEUP_SRC_MSK_RTC;
    cfg.phy_power_enable = false;
    cfg.rf_power_enable = false;
    cfg.flash_force_on = false;
    cfg.lp_before_cb_func = before_sleep;
    cfg.lp_after_cb_func = after_wakeup;
    slc_lpwr_ctrl_init(&cfg);

    SLC_HAL_ENABLE_GLOBAL_IRQ();

    slc_lpwr_ctrl_sleep(HAL_PMU_LP_MODE_SLEEP);

    return 0;
}

int low_power_sleep_phy_enable_test(void)
{
    wakeup_src_select(HAL_PMU_LP_WAKEUP_SRC_MSK_LPTIMER0);

    slc_lpwr_ctrl_cfg cfg;
    cfg.lp_wakeup_src_msk = HAL_PMU_LP_WAKEUP_SRC_MSK_LPTIMER0;
    cfg.phy_power_enable = true;
    cfg.rf_power_enable = false;
    cfg.flash_force_on = false;
    cfg.lp_before_cb_func = before_sleep;
    cfg.lp_after_cb_func = after_wakeup;
    slc_lpwr_ctrl_init(&cfg);

    SLC_HAL_ENABLE_GLOBAL_IRQ();
    slc_lpwr_ctrl_sleep(HAL_PMU_LP_MODE_STOP);

    return 0;
}

int low_power_stop_phy_disable_test(void)
{
    wakeup_src_select(HAL_PMU_LP_WAKEUP_SRC_MSK_LPTIMER1);

    slc_lpwr_ctrl_cfg cfg;
    cfg.lp_wakeup_src_msk = HAL_PMU_LP_WAKEUP_SRC_MSK_LPTIMER1;
    cfg.phy_power_enable = false;
    cfg.rf_power_enable = false;
    cfg.flash_force_on = true;
    cfg.lp_before_cb_func = before_sleep;
    cfg.lp_after_cb_func = after_wakeup_stop;
    slc_lpwr_ctrl_init(&cfg);

    SLC_HAL_ENABLE_GLOBAL_IRQ();

    slc_lpwr_ctrl_sleep(HAL_PMU_LP_MODE_STOP);

    return 0;
}

int low_power_stop_phy_enable_test(void)
{
    wakeup_src_select(HAL_PMU_LP_WAKEUP_SRC_MSK_LPTIMER0);

    slc_lpwr_ctrl_cfg cfg;
    cfg.lp_wakeup_src_msk = HAL_PMU_LP_WAKEUP_SRC_MSK_LPTIMER0;
    cfg.phy_power_enable = true;
    cfg.rf_power_enable = false;
    cfg.flash_force_on = true;
    cfg.lp_before_cb_func = before_sleep;
    cfg.lp_after_cb_func = after_wakeup_stop;
    slc_lpwr_ctrl_init(&cfg);

    SLC_HAL_ENABLE_GLOBAL_IRQ();

    slc_lpwr_ctrl_sleep(HAL_PMU_LP_MODE_STOP);

    return 0;
}

int low_power_stop_wakeup_by_lpio_test(void)
{
    wakeup_src_select(HAL_PMU_LP_WAKEUP_SRC_MSK_LPIO);

    slc_lpwr_ctrl_cfg cfg;
    cfg.lp_wakeup_src_msk = HAL_PMU_LP_WAKEUP_SRC_MSK_LPIO;
    cfg.phy_power_enable = false;
    cfg.rf_power_enable = false;
    cfg.flash_force_on = true;
    cfg.lp_before_cb_func = before_sleep;
    cfg.lp_after_cb_func = after_wakeup_stop;
    slc_lpwr_ctrl_init(&cfg);

    SLC_HAL_ENABLE_GLOBAL_IRQ();

    slc_lpwr_ctrl_sleep(HAL_PMU_LP_MODE_STOP);

    return 0;
}

int low_power_standby_wakeup_by_lpio_test(void)
{
    wakeup_src_select(HAL_PMU_LP_WAKEUP_SRC_MSK_LPIO);

    slc_lpwr_ctrl_cfg cfg;
    cfg.lp_wakeup_src_msk = HAL_PMU_LP_WAKEUP_SRC_MSK_LPIO;
    cfg.phy_power_enable = false;
    cfg.rf_power_enable = false;
    cfg.flash_force_on = true;
    cfg.lp_before_cb_func = before_sleep;
    cfg.lp_after_cb_func = after_wakeup_standby;
    slc_lpwr_ctrl_init(&cfg);

    SLC_HAL_ENABLE_GLOBAL_IRQ();

    slc_lpwr_ctrl_sleep(HAL_PMU_LP_MODE_STANDBY);

    return 0;
}

int low_power_standby_test(void)
{
    wakeup_src_select(HAL_PMU_LP_WAKEUP_SRC_MSK_LPTIMER0);

    slc_lpwr_ctrl_cfg cfg;
    cfg.lp_wakeup_src_msk = HAL_PMU_LP_WAKEUP_SRC_MSK_LPTIMER0;
    cfg.phy_power_enable = false;
    cfg.rf_power_enable = false;
    cfg.flash_force_on = false;
    cfg.lp_before_cb_func = before_sleep;
    cfg.lp_after_cb_func = after_wakeup_standby;
    slc_lpwr_ctrl_init(&cfg);

    SLC_HAL_ENABLE_GLOBAL_IRQ();

    slc_lpwr_ctrl_sleep(HAL_PMU_LP_MODE_STANDBY);

    return 0;
}

int low_power_shutdown_test(void)
{
    wakeup_src_select(HAL_PMU_LP_WAKEUP_SRC_MSK_LPTIMER1);

    slc_lpwr_ctrl_cfg cfg;
    cfg.lp_wakeup_src_msk = HAL_PMU_LP_WAKEUP_SRC_MSK_LPTIMER1;
    cfg.phy_power_enable = false;
    cfg.rf_power_enable = false;
    cfg.flash_force_on = false;
    cfg.lp_before_cb_func = before_sleep;
    cfg.lp_after_cb_func = after_wakeup_standby;
    slc_lpwr_ctrl_init(&cfg);

    SLC_HAL_ENABLE_GLOBAL_IRQ();

    slc_lpwr_ctrl_sleep(HAL_PMU_LP_MODE_SHUTDOWN);

    return 0;
}

int low_power_standby_flash_force_on_test(void)
{
    wakeup_src_select(HAL_PMU_LP_WAKEUP_SRC_MSK_LPTIMER1);

    slc_lpwr_ctrl_cfg cfg;
    cfg.lp_wakeup_src_msk = HAL_PMU_LP_WAKEUP_SRC_MSK_LPTIMER1;
    cfg.phy_power_enable = false;
    cfg.rf_power_enable = false;
    cfg.flash_force_on = true;
    cfg.lp_before_cb_func = before_sleep;
    cfg.lp_after_cb_func = after_wakeup_standby;
    slc_lpwr_ctrl_init(&cfg);

    SLC_HAL_ENABLE_GLOBAL_IRQ();

    slc_lpwr_ctrl_sleep(HAL_PMU_LP_MODE_STANDBY);

    return 0;
}

int low_power_faston_test(void)
{
    wakeup_src_select(HAL_PMU_LP_WAKEUP_SRC_MSK_LPTIMER1);

    slc_lpwr_ctrl_cfg cfg;
    cfg.lp_wakeup_src_msk = HAL_PMU_LP_WAKEUP_SRC_MSK_LPTIMER1;
    cfg.phy_power_enable = false;
    cfg.rf_power_enable = true;
    cfg.flash_force_on = false;
    cfg.lp_before_cb_func = before_sleep;
    cfg.lp_after_cb_func = after_wakeup_standby;
    slc_lpwr_ctrl_init(&cfg);

    SLC_HAL_ENABLE_GLOBAL_IRQ();

    slc_lpwr_ctrl_sleep(HAL_PMU_LP_MODE_STANDBY);

    return 0;
}

int low_power_fourceon_faston_test(void)
{
    wakeup_src_select(HAL_PMU_LP_WAKEUP_SRC_MSK_LPTIMER0);

    slc_lpwr_ctrl_cfg cfg;
    cfg.lp_wakeup_src_msk = HAL_PMU_LP_WAKEUP_SRC_MSK_LPTIMER0;
    cfg.phy_power_enable = false;
    cfg.rf_power_enable = true;
    cfg.flash_force_on = true;
    cfg.lp_before_cb_func = before_sleep;
    cfg.lp_after_cb_func = after_wakeup_standby;
    slc_lpwr_ctrl_init(&cfg);

    SLC_HAL_ENABLE_GLOBAL_IRQ();

    slc_lpwr_ctrl_sleep(HAL_PMU_LP_MODE_STANDBY);

    return 0;
}

int low_power_standby_multiple_wakeup_test(void)
{
    wakeup_src_select(HAL_PMU_LP_WAKEUP_SRC_MSK_LPTIMER1 | HAL_PMU_LP_WAKEUP_SRC_MSK_LPUART);
    test_recv_len = 0;

    slc_lpwr_ctrl_cfg cfg;
    cfg.lp_wakeup_src_msk = HAL_PMU_LP_WAKEUP_SRC_MSK_LPTIMER1 | HAL_PMU_LP_WAKEUP_SRC_MSK_LPUART;
    cfg.phy_power_enable = false;
    cfg.rf_power_enable = false;
    cfg.flash_force_on = false;
    cfg.lp_before_cb_func = before_sleep;
    cfg.lp_after_cb_func = after_wakeup_standby;
    slc_lpwr_ctrl_init(&cfg);

    SLC_HAL_ENABLE_GLOBAL_IRQ();

    slc_lpwr_ctrl_sleep(HAL_PMU_LP_MODE_STANDBY);

    return 0;
}

int low_power_stop_multiple_wakeup_test(void)
{
    wakeup_src_select(HAL_PMU_LP_WAKEUP_SRC_MSK_LPTIMER1 | HAL_PMU_LP_WAKEUP_SRC_MSK_LPUART);

    test_recv_len = 0;

    slc_lpwr_ctrl_cfg cfg;
    cfg.lp_wakeup_src_msk = HAL_PMU_LP_WAKEUP_SRC_MSK_LPTIMER1 | HAL_PMU_LP_WAKEUP_SRC_MSK_LPUART;
    cfg.phy_power_enable = false;
    cfg.rf_power_enable = false;
    cfg.flash_force_on = false;
    cfg.lp_before_cb_func = before_sleep;
    cfg.lp_after_cb_func = after_wakeup_stop;
    slc_lpwr_ctrl_init(&cfg);

    SLC_HAL_ENABLE_GLOBAL_IRQ();

    slc_lpwr_ctrl_sleep(HAL_PMU_LP_MODE_STOP);

    return 0;
}

int low_power_sleep_multiple_wakeup_test(void)
{
    wakeup_src_select(HAL_PMU_LP_WAKEUP_SRC_MSK_LPTIMER1 | HAL_PMU_LP_WAKEUP_SRC_MSK_LPUART);

    test_recv_len = 0;

    slc_lpwr_ctrl_cfg cfg;
    cfg.lp_wakeup_src_msk = HAL_PMU_LP_WAKEUP_SRC_MSK_LPTIMER1 | HAL_PMU_LP_WAKEUP_SRC_MSK_LPUART;
    cfg.phy_power_enable = false;
    cfg.rf_power_enable = false;
    cfg.flash_force_on = false;
    cfg.lp_before_cb_func = before_sleep;
    cfg.lp_after_cb_func = after_wakeup;
    slc_lpwr_ctrl_init(&cfg);

    SLC_HAL_ENABLE_GLOBAL_IRQ();

    slc_lpwr_ctrl_sleep(HAL_PMU_LP_MODE_SLEEP);

    return 0;
}

int low_power_stop_not_wakeup_test(void)
{
    wakeup_src_select(HAL_PMU_LP_WAKEUP_SRC_MSK_LPTIMER1);

    slc_lpwr_ctrl_cfg cfg;
    cfg.lp_wakeup_src_msk = HAL_PMU_LP_WAKEUP_SRC_MSK_RTC;
    cfg.phy_power_enable = false;
    cfg.rf_power_enable = false;
    cfg.flash_force_on = true;
    cfg.lp_before_cb_func = before_sleep;
    cfg.lp_after_cb_func = after_wakeup_stop;
    slc_lpwr_ctrl_init(&cfg);

    SLC_HAL_ENABLE_GLOBAL_IRQ();

    slc_lpwr_ctrl_sleep(HAL_PMU_LP_MODE_STOP);

    return 0;
}

int low_power_standby_not_wakeup_test(void)
{
    wakeup_src_select(HAL_PMU_LP_WAKEUP_SRC_MSK_LPTIMER1);

    slc_lpwr_ctrl_cfg cfg;
    cfg.lp_wakeup_src_msk = HAL_PMU_LP_WAKEUP_SRC_MSK_RTC;
    cfg.phy_power_enable = false;
    cfg.rf_power_enable = false;
    cfg.flash_force_on = true;
    cfg.lp_before_cb_func = before_sleep;
    cfg.lp_after_cb_func = after_wakeup_standby;
    slc_lpwr_ctrl_init(&cfg);

    SLC_HAL_ENABLE_GLOBAL_IRQ();

    slc_lpwr_ctrl_sleep(HAL_PMU_LP_MODE_STANDBY);

    return 0;
}

extern uint32_t g_test_nmi_continue_flag;
extern uint8_t g_lp_fail_flag;
extern void NMI_Handler_in_RAM(void);

int low_power_enter_failed_test(void)
{
    slc_register_nmi_handler(NMI_Handler_in_RAM);  // must use nmi handler in RAM
    g_test_nmi_continue_flag = 1;
    PRINTF("g_lp_fail_flag = %0x\n",g_lp_fail_flag);
    
    slc_lpwr_ctrl_cfg cfg;
    for(int i=0; i<4; i++) {
        // stop
        cfg.lp_wakeup_src_msk = HAL_PMU_LP_WAKEUP_SRC_MSK_LPTIMER1;
        cfg.phy_power_enable = false;
        cfg.rf_power_enable = false;
        cfg.flash_force_on = true;
        cfg.lp_before_cb_func = before_sleep;
        cfg.lp_after_cb_func = after_wakeup_stop;
        slc_lpwr_ctrl_init(&cfg);
        
        // prepare negative conditions to stop going into lp mode
        if(i == 0) {
            PRINTF("round 1: go to stop but wakeup mask all ZERO\n");
            slc_hal_pmu_set_lp_wakeup_source(0);    // wakeup mask all ZERO
            slc_lpwr_ctrl_sleep(HAL_PMU_LP_MODE_STOP);
        } else if(i == 1) {
            PRINTF("round 2: go to stop but lp wakeup interrupt ACTIVE\n");
            wakeup_src_select(HAL_PMU_LP_WAKEUP_SRC_MSK_LPTIMER1); // setup wakeup interrupt trigger
            // disable irq
            SLC_HAL_DISABLE_PERIPHERAL_IRQ(LPTIMER1_IRQ);
            SLC_HAL_DISABLE_GLOBAL_IRQ();
            // wait wakeup interrupt active: rc32_clk * 64K
            slc_hal_nop_delay_ms(2000); 
            slc_lpwr_ctrl_sleep(HAL_PMU_LP_MODE_STOP);
        } else if(i == 2) {
            PRINTF("round 3: go to stop but lpio_en DISABLED\n");
            slc_lpwr_ctrl_sleep_fake(HAL_PMU_LP_MODE_STOP);
        } else {
            uint32_t clk_cfg = SYS_CTRL->AHB_CLK_CFG & SYSCTRL_AHB_CLK_EN_REG_MASK;
            clk_cfg |= (SYS_CTRL->APB0_CLK_CFG & SYSCTRL_APB0_CLK_CFG_REG_MASK) << SYSCTRL_AHB_CLK_EN_REG_LEN;
            clk_cfg |= (SYS_CTRL->LP_CLK_CFG & SYSCTRL_LP_CLK_CFG_REG_MASK) << (SYSCTRL_AHB_CLK_EN_REG_LEN + SYSCTRL_APB0_CLK_CFG_REG_LEN);
            cfg.lp_after_cb_func = NULL;
            cfg.phy_power_enable = true;
            cfg.rf_power_enable = true;
            slc_lpwr_ctrl_init(&cfg);
            PRINTF("round 4: go to standby but phy POWER ON\n");
            slc_lpwr_ctrl_sleep(HAL_PMU_LP_MODE_STANDBY);
            
            rom_hw_flash_release_deep_power_down();
            slc_hal_pmu_lpio_enable(false);
            rom_hw_sysctrl_enable_clock_gate(clk_cfg, true);
        }
        // Here NMI should be called, lp_fail_flag has been updated.
        
        // restore module status IF NEEDED
        PRINTF("Enter lp mode failed with: 0x%02X\n", g_lp_fail_flag);
        if (g_lp_fail_flag & (0x1 << i)) {
            PRINTF("round %d: lp mode negative test PASS.\n", i+1);
        } else {
            PRINTF("round %d: lp mode negative test FAILED.\n", i+1);
        }
    }
    
    PRINTF("finish lp mode negative test.\n");
    g_test_nmi_continue_flag = 0;
    return 0;
}


