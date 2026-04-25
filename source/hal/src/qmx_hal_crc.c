/**
 * ****************************************************************************
 * @file qmx_hal_crc.c
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
#include "qmx_hal_crc.h"
#include "qmx_hal_sysctrl.h"
#include "reg_Dev.h"
#include "utility.h"
#include "error_def.h"
#include "hw_crc.h"

stCrcHandle_t *qmx_hal_crc_get_handle(hal_crc_type_e crc)
{
    switch (crc) {
        case HAL_CRC16:
            return CRC16;
        case HAL_CRC24:
            return CRC24;
        case HAL_CRC32:
            return CRC32;
        default:
            return NULL;
    }
}

void qmx_hal_crc_init(hal_crc_init_t *init)
{
    stCrcInit_t stInit;
    stCrcHandle_t *crc = qmx_hal_crc_get_handle(init->crc_type);

    if (crc == NULL) {
        return;
    }

    stInit.bDmaEn = false;
    stInit.bInputReverse = init->input_reverse;
    stInit.bOutputXor = init->output_xor;
    stInit.enOutputMode = (EN_CRC_OUTPUT_T)init->output_mode;

    rom_hw_crc_init(crc, &stInit);

    rom_hw_crc_set_init_value(crc, init->init_val);
    rom_hw_crc_set_xor_bytes(crc, init->xor_val);
}

void qmx_hal_crc_deinit(hal_crc_type_e crc_type)
{
    hal_peripheral_clk_e crc_clk = HAL_CLK_CRC16;
    if (crc_type == HAL_CRC16) {
        crc_clk = HAL_CLK_CRC16;
    } else if (crc_type == HAL_CRC24) {
        crc_clk = HAL_CLK_CRC24;
    } else if (crc_type == HAL_CRC32) {
        crc_clk = HAL_CLK_CRC32;
    } else {
        return;
    }

    qmx_hal_sysctrl_peripheral_mod_reset(crc_clk);
    qmx_hal_sysctrl_peripheral_clk_enable(crc_clk, false);
}

uint32_t qmx_hal_crc_calculate(hal_crc_type_e crc_type, uint8_t *data, uint32_t len)
{
    stCrcHandle_t *crc = qmx_hal_crc_get_handle(crc_type);

    if (crc == NULL) {
        return 0;
    }

    return rom_hw_crc_calculate_data(crc, data, (uint16_t)len);
}

uint16_t qmx_hal_crc_calc16(uint8_t *data, uint32_t len)
{
    return rom_hw_crc_get_crc16_value(data, (uint16_t)len);
}

uint32_t qmx_hal_crc_calc24(uint8_t *data, uint32_t len)
{
    return rom_hw_crc_get_crc24_value(data, (uint16_t)len);
}

uint32_t qmx_hal_crc_calc32(uint8_t *data, uint32_t len)
{
    return rom_hw_crc_get_crc32_value(data, (uint16_t)len);
}
