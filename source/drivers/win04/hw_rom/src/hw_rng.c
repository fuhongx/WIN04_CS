/**
 * ****************************************************************************
 * @file hw_rng.c
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
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>

#include "reg_Dev.h"
#include "error_def.h"
#include "utility.h"
#include "hw_sysctrl.h"
#include "hw_rng.h"

void rom_hw_rng_init(uint8_t freq_div, bool enable)
{
    RAND->RAND_CTRL = ((freq_div << RNG_FLUSH_FREQ_SHIFT) & RNG_FLUSH_FREQ_MASK) |
                        ((enable << RNG_DATA_STATUS_SHIFT) & RNG_DATA_STATUS_MASK);
}

void rom_hw_update_rng_seed(uint32_t seed)
{
    RAND->REED_DR = seed;
}

EN_ERR_STA_T rom_hw_get_random_u32(uint32_t *data, uint32_t len, uint32_t timeout)
{
    if ((data == NULL) || (len % 4) != 0)
        return EN_ERROR_STA_INVALID;

    uint32_t i = 0;
    uint32_t time = 0;

    for (i = 0; i < (len / 4); i++) {
        time = 0;
        while ((RAND->RAND_SR & RNG_DATA_STATUS_MASK) == 0) {
            time++;
            rom_utility_delay_us(1);
            if (time > timeout)
                return EN_ERROR_STA_TIMEOUT;
        }

        data[i] = RAND->RAND_DR;
    }

    return EN_ERROR_STA_OK;
}

EN_ERR_STA_T rom_hw_get_random_u8(uint8_t *data, uint32_t len, uint32_t timeout)
{
    if (data == NULL)
        return EN_ERROR_STA_INVALID;

    uint32_t i = 0;
    uint32_t time = 0;
    uint32_t temp = 0;

    for (i = 0; i < len; i += 4) {
        time = 0;
        while ((RAND->RAND_SR & RNG_DATA_STATUS_MASK) == 0) {
            time++;
            rom_utility_delay_us(1);
            if (time > timeout)
                return EN_ERROR_STA_TIMEOUT;
        }

        temp = RAND->RAND_DR;

        data[i] = (uint8_t)(temp & 0xFF);
        if (i + 1 >= len) break;
        data[i + 1] = (uint8_t)((temp >> 8) & 0xFF);
        if (i + 2 >= len) break;
        data[i + 2] = (uint8_t)((temp >> 16) & 0xFF);
        if (i + 3 >= len) break;
        data[i + 3] = (uint8_t)((temp >> 24) & 0xFF);
    }

    return EN_ERROR_STA_OK;
}