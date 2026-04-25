/**
 * ****************************************************************************
 * @file qmx_hal_rng.c
 * @author QMX software team.
 * @brief
 * @version  1.0
 * ****************************************************************************
 *
 * THIS CODE IS LIMITED TO GUIDE THE APPLICATION AND IMPROVE THE EFFICIENCY OF RESEARCH AND DEVELOPMENT.
 * AS A RESULT, QMX IS NOT RESPONSIBLE FOR ANY CLAIMS ARISING DIRECTLY OR INDIRECTLY FROM SUCH FIRMWARE AND/OR CONTENT.
 *
 * Copyright (c) 2026 QMX Semiconductor all rights reserved.
 *
 * ****************************************************************************
 */
#include "qmx_hal_rng.h"
#include "reg_Dev.h"
#include "utility.h"
#include "error_def.h"
#include "hw_rng.h"

void qmx_hal_rng_init(hal_rng_init_t *init)
{
    rom_hw_rng_init(0, true);
    if (init != NULL) {
        rom_hw_update_rng_seed(init->seed);
    }
}

void qmx_hal_rng_deinit(void)
{
    rom_hw_rng_init(0, false);
}

int qmx_hal_get_random_u32(uint32_t *data, uint32_t len, uint32_t timeout)
{
    EN_ERR_STA_T status = rom_hw_get_random_u32(data, len, timeout);
    return (status == EN_ERROR_STA_OK) ? 0 : -1;
}

int qmx_hal_get_random_u8(uint8_t *data, uint32_t len, uint32_t timeout)
{
    EN_ERR_STA_T status = rom_hw_get_random_u8(data, len, timeout);
    return (status == EN_ERROR_STA_OK) ? 0 : -1;
}

void qmx_hal_update_rng_seed(uint32_t seed)
{
    rom_hw_update_rng_seed(seed);
}
