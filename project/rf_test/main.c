#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "reg_Dev.h"
#include "error_def.h"
#include "utility.h"
#include "debug.h"
#include "hw_uart.h"
#include "hw_flash.h"
#include "hw_gpio.h"
#include "hw_dma.h"
#include "qmx_hal_sysctrl.h"
#include "qmx_hal_pmu.h"
#include "qmx_cali.h"
#include "app_cfg.h"

extern void rf_test_main(void);

uint8_t au8PrtBuffer[1024 * 6];
uint32_t u32PrtNum = 0;
uint32_t u32PutNum = 0;

void system_init(void)
{
    qmx_hal_sysctrl_cache_mode_set(HAL_CACHE_ENABLE);
    qmx_hal_sysctrl_peripheral_mod_reset(HAL_CLK_GPIO);   // 复位boot内的PIN8管脚配置

    qmx_hal_pmu_phy_power_enable(true);

#ifndef QMX_FPGA
    write32(0x4001403C, 0x637);
    // 校准时切换到TCXO25M，可正常使用外设等，否则时钟波动可能导致打印等异常
    qmx_hal_sysctrl_system_clock_init(HAL_SYSCLK_TCXO25M, HAL_SYSCLK_DIV_NONE);

    // qmx_hal_pmu_phy_power_enable(true);

#if APP_DEBUG_ENABLED
    debug_printf_init();
#endif
    PRINTF("FW start to CALI\n");
    qmx_cali_init(QMX_CALI_DC_IQ);
#endif

    // 校准结束切换回RC50M，需重新进行UART初始化
    qmx_hal_sysctrl_system_clock_init(HAL_SYSCLK_RC50M, HAL_SYSCLK_DIV_NONE);

#if APP_DEBUG_ENABLED
    debug_printf_init();
#endif
}


int main(void)
{
    system_init();
        
    #if (CAD)
//    void cad_test_handler(void);
//        cad_test_handler();
   #endif
//    void subg_read_test(void);
//        subg_read_test();
    rf_test_main();
    
    return 0;
}



    
void NMI_Handler_Proc(void)
{
    
}

