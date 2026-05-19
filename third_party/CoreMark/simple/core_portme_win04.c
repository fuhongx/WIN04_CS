/*
Copyright 2018 Embedded Microprocessor Benchmark Consortium (EEMBC)

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.

Original Author: Shay Gal-on
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "reg_Dev.h"
#include "error_def.h"
#include "slc_hal_sysctrl.h"
#include "slc_hal_intc.h"
#include "slc_hal_pmu.h"
#include "slc_rf.h"
#include "slc_cali.h"

#include "app_cfg.h"
#include "utility.h"
#include "debug.h"

#include "hw_flash.h"

#include "coremark.h"

#if VALIDATION_RUN
volatile ee_s32 seed1_volatile = 0x3415;
volatile ee_s32 seed2_volatile = 0x3415;
volatile ee_s32 seed3_volatile = 0x66;
#endif
#if PERFORMANCE_RUN
volatile ee_s32 seed1_volatile = 0x0;
volatile ee_s32 seed2_volatile = 0x0;
volatile ee_s32 seed3_volatile = 0x66;
#endif
#if PROFILE_RUN
volatile ee_s32 seed1_volatile = 0x8;
volatile ee_s32 seed2_volatile = 0x8;
volatile ee_s32 seed3_volatile = 0x8;
#endif
volatile ee_s32 seed4_volatile = ITERATIONS;
volatile ee_s32 seed5_volatile = 0;
/* Porting : Timing functions
        How to capture time and convert to seconds must be ported to whatever is
   supported by the platform. e.g. Read value from on board RTC, read value from
   cpu clock cycles performance counter etc. Sample implementation for standard
   time.h and windows.h definitions included.
*/
/* Define : TIMER_RES_DIVIDER
        Divider to trade off timer resolution and total time that can be
   measured.

        Use lower values to increase resolution, but make sure that overflow
   does not occur. If there are issues with the return value overflowing,
   increase this value.
        */
// #define NSECS_PER_SEC              1000//CLOCKS_PER_SEC
// #define CORETIMETYPE               clock_t
// #define GETMYTIME(_t)              (*_t = clock())
// #define MYTIMEDIFF(fin, ini)       ((fin) - (ini))
// #define TIMER_RES_DIVIDER          1000
// #define SAMPLE_TIME_IMPLEMENTATION 1
//#define EE_TICKS_PER_SEC           (NSECS_PER_SEC / TIMER_RES_DIVIDER)

#define SysTick_Counter_Disable ((ee_u32)0xFFFFFFFE)
#define SysTick_Counter_Enable ((ee_u32)0x00000001)
#define SysTick_Counter_Clear ((ee_u32)0x00000000)
static volatile ee_u32 systick_count = 0;

#if 0

__RAM_FUNC void SysTick_Handler(void);
__RAM_FUNC void start_time(void);
__RAM_FUNC void stop_time(void);
__RAM_FUNC CORE_TICKS get_time(void);
__RAM_FUNC secs_ret time_in_secs(CORE_TICKS ticks);

#endif


/** Define Host specific (POSIX), or target specific global time variables. */
// static CORETIMETYPE start_time_val, stop_time_val;

/* Function : start_time
        This function will be called right before starting the timed portion of
   the benchmark.

        Implementation may be capturing a system timer (as implemented in the
   example code) or zeroing some system parameters - e.g. setting the cpu clocks
   cycles to 0.
*/
__attribute__((used))
void SysTick_Handler(void)
{
    systick_count++;
}

void start_time(void)
{
    systick_count = 0;
    SysTick_Config(slc_hal_sysctrl_get_system_clock()/10);
}
/* Function : stop_time
        This function will be called right after ending the timed portion of the
   benchmark.

        Implementation may be capturing a system timer (as implemented in the
   example code) or other system parameters - e.g. reading the current value of
   cpu cycles counter.
*/
void stop_time(void)
{
    SysTick->CTRL &= SysTick_Counter_Disable;
    SysTick->VAL = SysTick_Counter_Clear;
}
/* Function : get_time
        Return an abstract "ticks" number that signifies time on the system.

        Actual value returned may be cpu cycles, milliseconds or any other
   value, as long as it can be converted to seconds by <time_in_secs>. This
   methodology is taken to accommodate any hardware or simulated platform. The
   sample implementation returns millisecs by default, and the resolution is
   controlled by <TIMER_RES_DIVIDER>
*/
CORE_TICKS get_time(void)
{
   return (CORE_TICKS)systick_count;
}
/* Function : time_in_secs
        Convert the value returned by get_time to seconds.

        The <secs_ret> type is used to accommodate systems with no support for
   floating point. Default implementation implemented by the EE_TICKS_PER_SEC
   macro above.
*/
secs_ret time_in_secs(CORE_TICKS ticks)
{
   secs_ret retval = ((secs_ret)ticks) / (secs_ret)10;
   return retval;
}

ee_u32 default_num_contexts = 1;

/* Function : portable_init
        Target specific initialization code
        Test for some common mistakes.
*/
void
portable_init(core_portable *p, int *argc, char *argv[])
{
    slc_hal_intc_init();

#ifndef SLC_FPGA
    slc_hal_pmu_phy_power_enable(true);
    // vtcxo bypass，使用vBAT供电，切25M之前配置，防止TCXO未工作，切换后CPU卡死
    slc_rf_tcxo_bypass(true);

    // 校准时切换到TCXO25M，可正常使用外设等，否则时钟波动可能导致打印等异常
    slc_hal_sysctrl_system_clock_init(HAL_SYSCLK_TCXO25M, HAL_SYSCLK_DIV_NONE);

    debug_printf_init();

    PRINTF("FW start to CALI\n");
    if (slc_cali_init(SLC_CALI_DC_IQ) != 0) {
        PRINTF("FW CALI fail\n");
    }
#endif

    slc_hal_sysctrl_system_clock_init(HAL_SYSCLK_RC50M, HAL_SYSCLK_DIV_NONE);
    debug_printf_init();

    // rom_hw_flash_config_read_write_mode(EN_FLASH_RW_MODE_SPI);
    // rom_hw_flash_config_read_write_mode(EN_FLASH_RW_MODE_DPI);
    // rom_hw_flash_config_read_write_mode(EN_FLASH_RW_MODE_QSPI);
    rom_hw_flash_config_read_write_mode(EN_FLASH_RW_MODE_FQSPI);

    slc_hal_sysctrl_cache_mode_set(HAL_CACHE_ENABLE);
    // slc_hal_sysctrl_cache_mode_set(HAL_CACHE_DISABLE);

    PRINTF("CoreMark Init...\n");
    if (sizeof(ee_ptr_int) != sizeof(ee_u8 *))
    {
        ee_printf(
            "ERROR! Please define ee_ptr_int to a type that holds a "
            "pointer!\n");
    }
    if (sizeof(ee_u32) != 4)
    {
        ee_printf("ERROR! Please define ee_u32 to a 32b unsigned type!\n");
    }
    p->portable_id = 1;
}
/* Function : portable_fini
        Target specific final code
*/
void
portable_fini(core_portable *p)
{
    p->portable_id = 0;
}
