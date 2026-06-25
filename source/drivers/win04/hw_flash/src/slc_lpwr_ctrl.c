/**
 * ****************************************************************************
 * @file lpwr_ctrl.c
 * @author SLC(SC) software team.
 * @brief
 * @version  1.0
 * ****************************************************************************
 *
 * THIS CODE IS LIMITED TO GUIDE THE APPLICATION AND IMPROVE THE EFFICIENCY OF RESEARCH AND DEVELOPMENT.
 * AS A RESULT, SILICONDUCTOR IS NOT RESPONSIBLE FOR ANY CLAIMS ARISING DIRECTLY OR INDIRECTLY FROM SUCH FIRMWARE AND/OR CONTENT.
 *
 * Copyright (c) 2025 Siliconductor Semiconductor all rights reserved.
 *
 * ****************************************************************************
 */

#include <string.h>

#include "reg_Dev.h"
#include "error_def.h"
#include "utility.h"
#include "compiler.h"
#include "app_cfg.h"

#include "slc_hal_intc.h"
#include "slc_hal_sysctrl.h"
#include "slc_lpwr_ctrl.h"
#include "hw_flash.h"
#include "hw_pmu.h"
#include "hw_sysctrl.h"

#define LPWR_WAKEUP_CB_ADDR     (ADDR_BACKREG_BASE)

#if (defined(__clang__) && defined(__ARMCOMPILER_VERSION)) || defined(__CC_ARM)
extern uint32_t Load$$RAM_APP_CODE$$Base;       // RAM_APP_CODE Load Address (Flash)
extern uint32_t Load$$RAM_APP_DATA$$Base;       // RAM_APP_DATA Load Address (Flash)
extern uint32_t Image$$RAM_APP_CODE$$Base;      // RAM_APP_CODE Exec Address (RAM)
extern uint32_t Image$$RAM_APP_DATA$$Base;      // RAM_APP_DATA Exec Address (RAM)
extern uint32_t Image$$RAM_APP_CODE$$Length;    // RAM_APP_CODE Length
extern uint32_t Image$$RAM_APP_DATA$$Length;    // RAM_APP_DATA Length
#elif defined ( __GNUC__ )
__STATIC_FORCEINLINE void gcc_restore_data(void)
{
    for (__copy_table_t const* pTable = &__copy_table_start__; pTable < &__copy_table_end__; ++pTable) {
        for(uint32_t i=0u; i<pTable->wlen; ++i) {
        pTable->dest[i] = pTable->src[i];
        }
    }
}
#endif

lp_after_cb g_lp_after_cb_func __RETENTION_DATA;
lp_before_cb g_lp_before_cb_func __RETENTION_DATA;

bool g_phy_power_enable __RETENTION_DATA;
bool g_rf_power_enable __RETENTION_DATA;
bool g_flash_force_on __RETENTION_DATA;
uint8_t g_lp_fail_flag __RETENTION_DATA;

void slc_lpwr_ctrl_init(slc_lpwr_ctrl_cfg *lp_cfg)
{
    if (lp_cfg == NULL ) {//|| lp_cfg->lp_wakeup_src_msk == 0
        PRINTF("lp_cfg is NULL or lp_wakeup_src_msk is 0\n");
        return;
    }
    slc_hal_pmu_set_lp_wakeup_source(lp_cfg->lp_wakeup_src_msk);

    g_phy_power_enable = lp_cfg->phy_power_enable;
    g_rf_power_enable = lp_cfg->rf_power_enable;
    g_flash_force_on = lp_cfg->flash_force_on;

    g_lp_before_cb_func = lp_cfg->lp_before_cb_func;
    g_lp_after_cb_func = lp_cfg->lp_after_cb_func;
}

void slc_lpwr_ctrl_sw_deinit(void)
{
    g_lp_after_cb_func = NULL;
    g_lp_before_cb_func = NULL;

    g_phy_power_enable = true;
    g_rf_power_enable = true;
    g_flash_force_on = false;

    slc_hal_pmu_set_boot_flag(0);
}

uint8_t slc_check_boot_status(void)
{
    uint8_t standby_wakeup = 0;

    standby_wakeup = slc_hal_pmu_get_boot_flag();

    if (standby_wakeup != 0) {
        slc_lpwr_ctrl_sleep(HAL_PMU_LP_MODE_STANDBY);
    }

    return standby_wakeup;
}

__RETENTION_FUNC void slc_lpwr_ctrl_sleep_fake(hal_pmu_lp_mode lp_mode)
{
    uint8_t boot_lpwr_mode = 0;
    uint32_t system_old_clk = 0;
    uint8_t flash_type_old = rom_hw_flash_get_type();
    uint32_t i;
#if (defined(__clang__) && defined(__ARMCOMPILER_VERSION)) || defined(__CC_ARM)
    uint32_t *ram_code_flash_addr = (uint32_t *)&Load$$RAM_APP_CODE$$Base;
    uint32_t *ram_code_ram_addr = (uint32_t *)&Image$$RAM_APP_CODE$$Base;
    uint32_t *ram_data_flash_addr = (uint32_t *)&Load$$RAM_APP_DATA$$Base;
    uint32_t *ram_data_ram_addr = (uint32_t *)&Image$$RAM_APP_DATA$$Base;
    uint32_t ram_code_len = (uint32_t)&Image$$RAM_APP_CODE$$Length;
    uint32_t ram_data_len = (uint32_t)&Image$$RAM_APP_DATA$$Length;
#endif
    if (g_lp_before_cb_func != NULL) {
        bool bRet = g_lp_before_cb_func();
        if (false == bRet)
            return;
    }
    SLC_HAL_DISABLE_GLOBAL_IRQ();

    switch (lp_mode)
    {
        case HAL_PMU_LP_MODE_STOP:
        {
            system_old_clk = SystemCoreClock;
            slc_hal_pmu_phy_power_enable(g_phy_power_enable);
            slc_hal_pmu_rf_lp_bypass(g_rf_power_enable);

            slc_hal_pmu_set_lp_mode(HAL_PMU_LP_MODE_STOP);
            slc_hal_sysctrl_disable_ahb_apb();
            slc_hal_pmu_lpio_enable(false); // Different with slc_lpwr_ctrl_sleep
            rom_hw_flash_deep_power_down();

            __WFI();
            
            for (i = 0; i < (RAM_SIZE_MAX - RAM_RETENTION_SIZE); i+=4)
            {
                write32((RAM_RETENTION_MAX_ADDR + i), 0);
            }

#if (defined(__clang__) && defined(__ARMCOMPILER_VERSION)) || defined(__CC_ARM)
            for (i = 0; i < ram_code_len; i+=4)
                ram_code_ram_addr[i] = ram_code_flash_addr[i];

            for (i = 0; i < ram_data_len; i+=4)
                ram_data_ram_addr[i] = ram_data_flash_addr[i];
#elif defined ( __GNUC__ )
            gcc_restore_data();
#endif

            rom_hw_flash_set_type(flash_type_old);
            SystemCoreClock = system_old_clk;

            rom_hw_flash_release_deep_power_down();
            SLC_HAL_ENABLE_GLOBAL_IRQ();
            break;
        }
        default:
            break;
    }

    if (NULL != g_lp_after_cb_func) {
        g_lp_after_cb_func();
    }
    slc_lpwr_ctrl_sw_deinit();
}

__RETENTION_FUNC void slc_lpwr_ctrl_sleep(hal_pmu_lp_mode lp_mode)
{
    uint8_t boot_lpwr_mode = 0;
    uint32_t system_old_clk = 0;
    uint8_t flash_type_old = rom_hw_flash_get_type();
    uint32_t i;
    // TODO: 暂未适配GCC工程
#if (defined(__clang__) && defined(__ARMCOMPILER_VERSION)) || defined(__CC_ARM)
    uint32_t *ram_code_flash_addr = (uint32_t *)&Load$$RAM_APP_CODE$$Base;
    uint32_t *ram_code_ram_addr = (uint32_t *)&Image$$RAM_APP_CODE$$Base;
    uint32_t *ram_data_flash_addr = (uint32_t *)&Load$$RAM_APP_DATA$$Base;
    uint32_t *ram_data_ram_addr = (uint32_t *)&Image$$RAM_APP_DATA$$Base;
    uint32_t ram_code_len = (uint32_t)&Image$$RAM_APP_CODE$$Length;
    uint32_t ram_data_len = (uint32_t)&Image$$RAM_APP_DATA$$Length;
#endif

    if (lp_mode == HAL_PMU_LP_MODE_NORMAL)
        return;

    // manage standby wakeup
    boot_lpwr_mode = slc_hal_pmu_get_boot_flag();
    if (boot_lpwr_mode) {
        slc_hal_pmu_set_boot_flag(0);
        goto standby_wakeup;
    }

    // check whether data and config need to be saved before sleep
    if (g_lp_before_cb_func != NULL) {
        bool bRet = g_lp_before_cb_func();
        if (false == bRet)
            return;
    }

    // disable irq after callback, avoid irq enable in callback
    SLC_HAL_DISABLE_GLOBAL_IRQ();

    switch (lp_mode)
    {
        case HAL_PMU_LP_MODE_SLEEP:
        {
            __WFI();
            SLC_HAL_ENABLE_GLOBAL_IRQ();

            break;
        }

        case HAL_PMU_LP_MODE_STOP:
        {
            // save system clock because SystemCoreClock not retention
            system_old_clk = SystemCoreClock;
            slc_hal_pmu_phy_power_enable(g_phy_power_enable);
            slc_hal_pmu_rf_lp_bypass(g_rf_power_enable);

            // TODO: save cali data before stop mode
            slc_hal_pmu_set_lp_mode(HAL_PMU_LP_MODE_STOP);
            slc_hal_sysctrl_disable_ahb_apb();
            slc_hal_pmu_lpio_enable(true);
            // TODO: flash深睡后存在异常，待debug后打开
            rom_hw_flash_deep_power_down();

            // TODO: MPW上进入stop的80us内发生唤醒会导致CPU挂死，待ES修复
            __WFI();

            /*
             * 1、唤醒后非retention的48K RAM不可直接访问，必须先初始化，否则会产生X态导致异常
             * 2、memset在flash内，所以必须在flash deep power down唤醒后，否则会导致异常
             */
            for (i = 0; i < (RAM_SIZE_MAX - RAM_RETENTION_SIZE); i+=4)
            {
                write32((RAM_RETENTION_MAX_ADDR + i), 0);
            }

#if (defined(__clang__) && defined(__ARMCOMPILER_VERSION)) || defined(__CC_ARM)
            // 此处仅能恢复.data段的上电初始值，恢复stop前的数据需要用户在callback自行处理
            for (i = 0; i < ram_code_len; i+=4)
                ram_code_ram_addr[i] = ram_code_flash_addr[i];

            for (i = 0; i < ram_data_len; i+=4)
                ram_data_ram_addr[i] = ram_data_flash_addr[i];
#elif defined ( __GNUC__ )
            gcc_restore_data();
#endif

            // restore flash type
            rom_hw_flash_set_type(flash_type_old);
            // restore system clock, for delay function
            SystemCoreClock = system_old_clk;

            // need first release flash deep power down
            rom_hw_flash_release_deep_power_down();

            SLC_HAL_ENABLE_GLOBAL_IRQ();
            slc_hal_pmu_lpio_enable(false);

            // TODO: recover cali data after stop mode

            break;
        }

        case HAL_PMU_LP_MODE_STANDBY:
        {
            // save standby callback address, when wakeup from standby, will jump to this address
            if (g_lp_after_cb_func != NULL) {
                write32(LPWR_WAKEUP_CB_ADDR, (uint32_t)g_lp_after_cb_func);
            }

            slc_hal_pmu_phy_power_enable(false);
            slc_hal_pmu_rf_lp_bypass(g_rf_power_enable);

            slc_hal_pmu_set_lp_mode(HAL_PMU_LP_MODE_STANDBY);
            slc_hal_sysctrl_disable_ahb_apb();
            slc_hal_pmu_lpio_enable(true);

            if (g_flash_force_on == true) {
                slc_hal_pmu_force_flash_on(true);
                rom_hw_flash_deep_power_down();
            } else {
                slc_hal_pmu_force_flash_on(false);
                rom_hw_flash_deep_power_down();
            }

            __WFI();

standby_wakeup:
            SLC_HAL_ENABLE_GLOBAL_IRQ();

            uint32_t addr = read32(LPWR_WAKEUP_CB_ADDR);
            if ((addr != 0) && (addr != UNIT_WORD_WIDE)) {
                g_lp_after_cb_func = (lp_after_cb)addr;
            }
            write32(LPWR_WAKEUP_CB_ADDR, 0);

            break;
        }

        case HAL_PMU_LP_MODE_SHUTDOWN:
        {
            slc_hal_pmu_phy_power_enable(false);
            slc_hal_pmu_rf_lp_bypass(false);
            slc_hal_pmu_force_flash_on(false);
            slc_hal_pmu_set_lp_mode(HAL_PMU_LP_MODE_SHUTDOWN);
            slc_hal_pmu_powerdown();
            rom_hw_flash_deep_power_down();

            __WFI();
        }

        default:
            break;
    }

    // check whether data and config need to be recovered after sleep
    if (NULL != g_lp_after_cb_func)
    {
        g_lp_after_cb_func();
    }

    slc_lpwr_ctrl_sw_deinit();
}
