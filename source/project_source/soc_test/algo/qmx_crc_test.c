#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>

#include "app_cfg.h"
#include "utility.h"
#include "reg_Dev.h"
#include "error_def.h"
#include "debug.h"
#include "qmx_hal_sysctrl.h"
#include "qmx_hal_crc.h"
#include "qmx_hal_rng.h"
#include "qmx_hal_timer.h"
#include "qmx_crc_test.h"
#include "sw_crc.h"
#include "compiler.h"

int qmx_crc16_accuracy_test(void)
{
    uint16_t hw_crc, sw_crc;
    uint8_t crc_data[8] = {0};
    int ret;
    crc_config_t crc_cfg = {
        .poly = 0x1021,
        .init = 0x0000,
        .xorout = 0x0000,
        .refin = true,
        .refout = true,
    };
    hal_rng_init_t rng_init = {0};

    qmx_hal_sysctrl_peripheral_clk_enable(HAL_CLK_RAND, true);
    qmx_hal_sysctrl_peripheral_mod_reset(HAL_CLK_RAND);
    rng_init.seed = 0xA4567891;
    qmx_hal_rng_init(&rng_init);
    ret = qmx_hal_get_random_u8(crc_data, sizeof(crc_data), 1000);
    if (ret != 0) {
        PRINTF("get random u8 error, ret=0x%X\n", ret);
        return ret;
    }

    qmx_hal_sysctrl_peripheral_clk_enable(HAL_CLK_CRC16, true);
    qmx_hal_sysctrl_peripheral_mod_reset(HAL_CLK_CRC16);

    hw_crc = qmx_hal_crc_calc16(crc_data, sizeof(crc_data));
    crc16_init_table(&crc_cfg);
    sw_crc = crc16_compute(crc_data, sizeof(crc_data), &crc_cfg);

    if (hw_crc == sw_crc) {
        PRINTF("CRC16 CCITT accuracy test success! HW CRC: 0x%X, SW CRC: 0x%X\n", hw_crc, sw_crc);
    } else {
        PRINTF("CRC16 CCITT accuracy test failed! HW CRC: 0x%X, SW CRC: 0x%X\n", hw_crc, sw_crc);
        dump_u8buf("Data:", crc_data, sizeof(crc_data));
        return -1;
    }
    return 0;
}

int qmx_crc24_accuracy_test(void)
{
    uint32_t hw_crc, sw_crc;
    uint8_t crc_data[8] = {0};
    int ret;
    crc_config_t crc_cfg = {
        .poly = 0x800063,
        .init = 0xFFFFFF,
        .xorout = 0xFFFFFF,
        .refin = true,
        .refout = true,
    };
    hal_rng_init_t rng_init = {0};

    qmx_hal_sysctrl_peripheral_clk_enable(HAL_CLK_RAND, true);
    qmx_hal_sysctrl_peripheral_mod_reset(HAL_CLK_RAND);
    rng_init.seed = 0xA4567891;
    qmx_hal_rng_init(&rng_init);
    ret = qmx_hal_get_random_u8(crc_data, sizeof(crc_data), 1000);
    if (ret != 0) {
        PRINTF("get random u8 error, ret=0x%X\n", ret);
        return ret;
    }

    qmx_hal_sysctrl_peripheral_clk_enable(HAL_CLK_CRC24, true);
    qmx_hal_sysctrl_peripheral_mod_reset(HAL_CLK_CRC24);

    hw_crc = qmx_hal_crc_calc24(crc_data, sizeof(crc_data));

    crc24_init_table(&crc_cfg);
    sw_crc = crc24_compute(crc_data, sizeof(crc_data), &crc_cfg);

    if (hw_crc == sw_crc) {
        PRINTF("CRC24 accuracy test success! HW CRC: 0x%X, SW CRC: 0x%X\n", hw_crc, sw_crc);
    } else {
        PRINTF("CRC24 accuracy test failed! HW CRC: 0x%X, SW CRC: 0x%X\n", hw_crc, sw_crc);
        dump_u8buf("Data:", crc_data, sizeof(crc_data));
        return -1;
    }
    return 0;
}

int qmx_crc32_accuracy_test(void)
{
    uint32_t hw_crc, sw_crc;
    uint8_t crc_data[8] = {0};
    int ret;
    crc_config_t crc_cfg = {
        .poly = 0x04C11DB7,
        .init = 0xFFFFFFFF,
        .xorout = 0xFFFFFFFF,
        .refin = true,
        .refout = true,
    };
    hal_rng_init_t rng_init = {0};

    qmx_hal_sysctrl_peripheral_clk_enable(HAL_CLK_RAND, true);
    qmx_hal_sysctrl_peripheral_mod_reset(HAL_CLK_RAND);
    rng_init.seed = 0xA4567891;
    qmx_hal_rng_init(&rng_init);
    ret = qmx_hal_get_random_u8(crc_data, sizeof(crc_data), 1000);
    if (ret != 0) {
        PRINTF("get random u8 error, ret=0x%X\n", ret);
        return ret;
    }

    qmx_hal_sysctrl_peripheral_clk_enable(HAL_CLK_CRC32, true);
    qmx_hal_sysctrl_peripheral_mod_reset(HAL_CLK_CRC32);

    hw_crc = qmx_hal_crc_calc32(crc_data, sizeof(crc_data));

    crc32_init_table(&crc_cfg);
    sw_crc = crc32_compute(crc_data, sizeof(crc_data), &crc_cfg);

    if (hw_crc == sw_crc) {
        PRINTF("CRC32 accuracy test success! HW CRC: 0x%X, SW CRC: 0x%X\n", hw_crc, sw_crc);
    } else {
        PRINTF("CRC32 accuracy test failed! HW CRC: 0x%X, SW CRC: 0x%X\n", hw_crc, sw_crc);
        dump_u8buf("Data:", crc_data, sizeof(crc_data));
        return -1;
    }
    return 0;
}

#define QMX_CRC_PERF_TEST_SIZE  1024
int qmx_crc_performance_test(void)
{
    uint32_t start_time = 0;
    uint32_t end_time = 0;
    uint32_t crc_value __MAYBE_UNUSED, ret;
    uint8_t *data = malloc(QMX_CRC_PERF_TEST_SIZE);
    hal_rng_init_t rng_init = {0};

    if (data == NULL) {
        PRINTF("malloc error\n");
        return -1;
    }

    qmx_hal_sysctrl_peripheral_clk_enable(HAL_CLK_TIM0, true);
    qmx_hal_sysctrl_peripheral_mod_reset(HAL_CLK_TIM0);

    qmx_hal_timer_init(HAL_TIMER0, 0xFFFFFFFF, false);
    qmx_hal_timer_start(HAL_TIMER0);

    qmx_hal_sysctrl_peripheral_clk_enable(HAL_CLK_RAND, true);
    qmx_hal_sysctrl_peripheral_mod_reset(HAL_CLK_RAND);
    rng_init.seed = 0xA4567891;
    qmx_hal_rng_init(&rng_init);
    ret = qmx_hal_get_random_u8(data, QMX_CRC_PERF_TEST_SIZE, 1000);
    if (ret != 0) {
        PRINTF("get random u8 error, ret=0x%X\n", ret);
        return ret;
    }

    qmx_hal_sysctrl_peripheral_clk_enable(HAL_CLK_CRC16, true);
    qmx_hal_sysctrl_peripheral_mod_reset(HAL_CLK_CRC16);
    qmx_hal_sysctrl_peripheral_clk_enable(HAL_CLK_CRC24, true);
    qmx_hal_sysctrl_peripheral_mod_reset(HAL_CLK_CRC24);
    qmx_hal_sysctrl_peripheral_clk_enable(HAL_CLK_CRC32, true);
    qmx_hal_sysctrl_peripheral_mod_reset(HAL_CLK_CRC32);

    start_time = qmx_hal_timer_get_count(HAL_TIMER0);
    crc_value = qmx_hal_crc_calc16(data, QMX_CRC_PERF_TEST_SIZE);
    end_time = qmx_hal_timer_get_count(HAL_TIMER0);
    PRINTF("CRC16 CCITT performance test, size: %u byte, cost time: %.2f us, perf: %.2f Mbps\n",
           QMX_CRC_PERF_TEST_SIZE, (float)(start_time - end_time) / 50.0f,
           QMX_CRC_PERF_TEST_SIZE * 8 * 50.0f / (1.024f * 1.024f) / (start_time - end_time));

    start_time = qmx_hal_timer_get_count(HAL_TIMER0);
    crc_value = qmx_hal_crc_calc24(data, QMX_CRC_PERF_TEST_SIZE);
    end_time = qmx_hal_timer_get_count(HAL_TIMER0);
    PRINTF("CRC24 performance test, size: %u byte, cost time: %.2f us, perf: %.2f Mbps\n",
           QMX_CRC_PERF_TEST_SIZE, (float)(start_time - end_time) / 50.0f,
           QMX_CRC_PERF_TEST_SIZE * 8 * 50.0f / (1.024f * 1.024f) / (start_time - end_time));

    start_time = qmx_hal_timer_get_count(HAL_TIMER0);
    crc_value = qmx_hal_crc_calc32(data, QMX_CRC_PERF_TEST_SIZE);
    end_time = qmx_hal_timer_get_count(HAL_TIMER0);
    PRINTF("CRC32 performance test, size: %u byte, cost time: %.2f us, perf: %.2f Mbps\n",
           QMX_CRC_PERF_TEST_SIZE, (float)(start_time - end_time) / 50.0f,
           QMX_CRC_PERF_TEST_SIZE * 8 * 50.0f / (1.024f * 1.024f) / (start_time - end_time));

    qmx_hal_timer_stop(HAL_TIMER0);
    free(data);
    return 0;
}

#define CRC16_POLY          0x1021
#define CRC24_POLY          0x800063
#define CRC32_POLY          0x04C11DB7

int qmx_crc_random_test(void)
{
    uint32_t hw_crc, sw_crc, ret;
    uint8_t *data = malloc(QMX_CRC_PERF_TEST_SIZE);
    uint32_t cal_len = 0;
    uint32_t crc_poly[HAL_CRC_TYPE_MAX] = {CRC16_POLY, CRC24_POLY, CRC32_POLY};
    hal_crc_type_e crc_type = HAL_CRC32;
    uint8_t refin = 1;
    uint8_t refout = 1;
    uint32_t xorout = 0x0;
    uint32_t init = 0x0;
    hal_crc_init_t crc_init;
    crc_config_t crc_cfg;
    hal_rng_init_t rng_init = {0};

    if (data == NULL) {
        PRINTF("malloc error\n");
        return -1;
    }

    crc_init.output_xor = true;

    qmx_hal_sysctrl_peripheral_clk_enable(HAL_CLK_RAND, true);
    qmx_hal_sysctrl_peripheral_mod_reset(HAL_CLK_RAND);
    rng_init.seed = 0xA4567891;
    qmx_hal_rng_init(&rng_init);

    qmx_hal_sysctrl_peripheral_clk_enable(HAL_CLK_CRC16, true);
    qmx_hal_sysctrl_peripheral_mod_reset(HAL_CLK_CRC16);
    qmx_hal_sysctrl_peripheral_clk_enable(HAL_CLK_CRC24, true);
    qmx_hal_sysctrl_peripheral_mod_reset(HAL_CLK_CRC24);
    qmx_hal_sysctrl_peripheral_clk_enable(HAL_CLK_CRC32, true);
    qmx_hal_sysctrl_peripheral_mod_reset(HAL_CLK_CRC32);

once:
    ret = qmx_hal_get_random_u8(data, QMX_CRC_PERF_TEST_SIZE, 1000);
    if (ret != 0) {
        PRINTF("get random u8 error, ret=0x%X\n", ret);
        goto err;
    }

    // 获取随机长度、CRC类型、初始值、异或值、输入输出反转标志
    cal_len = ((uint32_t *)data)[0];
    cal_len = (cal_len % QMX_CRC_PERF_TEST_SIZE == 0) ? QMX_CRC_PERF_TEST_SIZE : cal_len % QMX_CRC_PERF_TEST_SIZE;
    crc_type = (hal_crc_type_e)(((uint32_t *)data)[4] % HAL_CRC_TYPE_MAX);
    init = ((uint32_t *)data)[8];
    xorout = ((uint32_t *)data)[12];
    refin = data[16] & 0x1;
    refout = data[17] & 0x1;

    crc_init.input_reverse = refin ? true : false;
    crc_init.output_mode = refout ? HAL_CRC_OUTPUT_REVERSE : HAL_CRC_OUTPUT_NORMAL;

    crc_cfg.poly = crc_poly[crc_type];
    crc_cfg.init = init;
    crc_cfg.xorout = xorout;
    crc_cfg.refin = refin;
    crc_cfg.refout = refout;

    switch (crc_type) {
        case HAL_CRC16:
            crc_init.crc_type = HAL_CRC16;
            crc_init.init_val = (init & 0xFFFF);
            crc_init.xor_val = (xorout & 0xFFFF);
            qmx_hal_crc_init(&crc_init);
            hw_crc = qmx_hal_crc_calculate(HAL_CRC16, data, cal_len) & 0xFFFF;

            crc_cfg.init = init & 0xFFFF;
            crc_cfg.xorout = xorout & 0xFFFF;

            crc16_init_table(&crc_cfg);
            sw_crc = crc16_compute(data, cal_len, &crc_cfg);
            break;

        case HAL_CRC24:
            crc_init.crc_type = HAL_CRC24;
            crc_init.init_val = (init & 0xFFFFFF);
            crc_init.xor_val = (xorout & 0xFFFFFF);
            qmx_hal_crc_init(&crc_init);
            hw_crc = qmx_hal_crc_calculate(HAL_CRC24, data, cal_len) & 0xFFFFFF;

            crc_cfg.init = init & 0xFFFFFF;
            crc_cfg.xorout = xorout & 0xFFFFFF;

            crc24_init_table(&crc_cfg);
            sw_crc = crc24_compute(data, cal_len, &crc_cfg);
            break;

        case HAL_CRC32:
            crc_init.crc_type = HAL_CRC32;
            crc_init.init_val = init;
            crc_init.xor_val = xorout;
            qmx_hal_crc_init(&crc_init);
            hw_crc = qmx_hal_crc_calculate(HAL_CRC32, data, cal_len);

            crc32_init_table(&crc_cfg);
            sw_crc = crc32_compute(data, cal_len, &crc_cfg);
            break;

        default:
            PRINTF("Unknown CRC type %d\n", crc_type);
            ret = 1;
            goto err;
    }

    if (hw_crc != sw_crc) {
        PRINTF("CRC random err, type: %d, hw=0x%X, sw: 0x%X, init: 0x%X, xorout: 0x%X, refin: %d, refout: %d\n",
                crc_type, hw_crc, sw_crc, crc_cfg.init, crc_cfg.xorout, refin, refout);
        ret = 1;
        goto err;
    }
    PRINTF("CRC random pass, type: %d, len=%d, crc: 0x%X, init: 0x%X, xorout: 0x%X, refin: %d, refout: %d\n",
           crc_type, cal_len, sw_crc, crc_cfg.init, crc_cfg.xorout, refin, refout);

    goto once;

err:
    dump_u8buf("CRC test data", data, cal_len);

    free(data);
    return ret;
}
