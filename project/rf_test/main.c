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
#include "slc_hal_sysctrl.h"
#include "slc_hal_pmu.h"
#include "slc_cali.h"
#include "app_cfg.h"

extern void rf_test_main(void);

uint8_t au8PrtBuffer[1024 * 6];
uint32_t u32PrtNum = 0;
uint32_t u32PutNum = 0;

void system_init(void)
{
    slc_hal_sysctrl_cache_mode_set(HAL_CACHE_ENABLE);

    slc_hal_pmu_phy_power_enable(true);

#ifndef SLC_FPGA
    write32(0x4001403C, 0x637);
    // 校准时切换到TCXO25M，可正常使用外设等，否则时钟波动可能导致打印等异常
    slc_hal_sysctrl_system_clock_init(HAL_SYSCLK_TCXO25M, HAL_SYSCLK_DIV_NONE);

    // slc_hal_pmu_phy_power_enable(true);

#if APP_DEBUG_ENABLED
    debug_printf_init();
#endif
    PRINTF("FW start to CALI\n");
    slc_cali_init(SLC_CALI_DC_IQ);
#endif

    // 校准结束切换回RC50M，需重新进行UART初始化
    slc_hal_sysctrl_system_clock_init(HAL_SYSCLK_FDB50M, HAL_SYSCLK_DIV_NONE);

#if APP_DEBUG_ENABLED
    debug_printf_init();
#endif
    PRINTF("FW start to work(freq: %dMHz). Build Time:[%s T %s].\n",
            slc_hal_sysctrl_get_system_clock() / 1000000, __DATE__, __TIME__);
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

