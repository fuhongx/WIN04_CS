#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <math.h>

#include "reg_Dev.h"
#include "error_def.h"
#include "utility.h"
#include "compiler.h"
#include "debug.h"
#include "app_cfg.h"
#include "qmx_hal_uart.h"
#include "qmx_hal_sysctrl.h"
#include "qmx_hal_intc.h"
#include "qmx_cali.h"
#include "qmx_lpwr_ctrl.h"
#include "qmx_pmu.h"

#ifdef QMX_PHYTEST
#include "qmx_phy_test.h"
#endif

#ifdef QMX_SOCTEST
#include "qmx_soc_test.h"
#endif

#ifdef QMX_AUTOTEST
#include "qmx_uart_cmd_process.h"
#endif

#ifdef NT_SHELL
#include "nttop.h"

void qmx_debug_uart_irq_handler(void)
{
    uint32_t sta __MAYBE_UNUSED = 0;
    sta = qmx_hal_uart_get_irq_status(DEBUG_UART_HANDLE);

    ntshell_top_run_once();
}
#endif

void qmx_platform_init(void)
{
    qmx_hal_sysctrl_cache_mode_set(HAL_CACHE_ENABLE);
    qmx_hal_sysctrl_peripheral_mod_reset(HAL_CLK_GPIO);   // 复位boot内的PIN8管脚配置

#ifndef QMX_FPGA
    // 校准时切换到TCXO25M，可正常使用外设等，否则时钟波动可能导致打印等异常
    qmx_hal_sysctrl_system_clock_init(HAL_SYSCLK_TCXO25M, HAL_SYSCLK_DIV_NONE);

    qmx_hal_pmu_phy_power_enable(true);

#if APP_DEBUG_ENABLED
    debug_printf_init();
#endif
    qmx_cali_init(QMX_CALI_FFT_IQ);
#endif

    // 校准结束切换回RC50M，需重新进行UART初始化
    qmx_hal_sysctrl_system_clock_init(HAL_SYSCLK_RC50M, HAL_SYSCLK_DIV_NONE);
    // while(1);

#if APP_DEBUG_ENABLED
    debug_printf_init();

#ifdef NT_SHELL
    qmx_hal_register_irq_handler(DEBUG_UART_IRQ, qmx_debug_uart_irq_handler);
    qmx_hal_uart_enable_irq(DEBUG_UART_HANDLE, HAL_UART_INT_EN_RX_NOT_EMPTY);
    QMX_HAL_ENABLE_PERIPHERAL_IRQ(DEBUG_UART_IRQ, 0x3);

    ntshell_top_init();
#endif
#endif
}

int main(void)
{
    qmx_check_boot_status();

#ifdef QMX_AUTOTEST
    qmx_uart_cmd_process_main();
#endif // QMX_AUTOTEST

    qmx_platform_init();

    PRINTF("FW start to work.Build Time:[%s T %s].\n", __DATE__, __TIME__);

#ifdef QMX_PHYTEST
    qmx_phy_test();
#endif // QMX_PHYTEST

#ifdef QMX_SOCTEST
#ifndef QMX_TEST_SLAVE
    soctest_main();
#else
    qmx_test_slave_main();
#endif // QMX_TEST_SLAVE
#endif // QMX_SOCTEST

    while (1);
#ifdef __CC_ARM
#pragma diag_suppress 111
#endif
    return 0;
}

void NMI_Handler_Proc(void)
{
    while (1);
}
