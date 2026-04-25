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
#include "qmx_aes_test.h"
#include "qmx_hal_sysctrl.h"
#include "qmx_hal_aes.h"
#include "qmx_hal_rng.h"
#include "qmx_hal_timer.h"
#include "sw_aes.h"

#define AES128_TEST_SIZE        64
#define AES128_PERF_TEST_SIZE   1024

int qmx_aes128_accuracy_test(void)
{
    uint8_t *plain = malloc(AES128_TEST_SIZE);
    uint8_t iv[16] = {0};
    uint8_t key[16] = {0};
    uint8_t *enc_data = malloc(AES128_TEST_SIZE);
    uint8_t *dec_data = malloc(AES128_TEST_SIZE);
    uint8_t *sw_enc = malloc(AES128_TEST_SIZE);
    int ret = 0;

    if (plain == NULL || enc_data == NULL || dec_data == NULL || sw_enc == NULL) {
        PRINTF("malloc error\n");
        return -1;
    }

    qmx_hal_sysctrl_peripheral_clk_enable(HAL_CLK_RAND, true);
    qmx_hal_sysctrl_peripheral_mod_reset(HAL_CLK_RAND);

    hal_rng_init_t rng_init = {0};
    rng_init.seed = 0x12345678;
    qmx_hal_rng_init(&rng_init);

    ret = qmx_hal_get_random_u8(iv, sizeof(iv), 1000);
    if (ret != 0)
        PRINTF("get random IV error, ret=0x%X\n", ret);

    ret = qmx_hal_get_random_u8(key, sizeof(key), 1000);
    if (ret != 0)
        PRINTF("get random KEY error, ret=0x%X\n", ret);

    ret = qmx_hal_get_random_u8(plain, AES128_TEST_SIZE, 1000);
    if (ret != 0)
        PRINTF("get random plain error, ret=0x%X\n", ret);

    qmx_hal_sysctrl_peripheral_clk_enable(HAL_CLK_AES128, true);
    qmx_hal_sysctrl_peripheral_mod_reset(HAL_CLK_AES128);

    qmx_hal_aes128_enc_once(key, iv, plain, AES128_TEST_SIZE, enc_data);
    qmx_hal_aes128_dec_once(key, iv, enc_data, AES128_TEST_SIZE, dec_data);

    AES128_CBC_encrypt_buffer(sw_enc, plain, AES128_TEST_SIZE, key, iv);
    if (memcmp(enc_data, sw_enc, AES128_TEST_SIZE) == 0) {
        PRINTF("AES128 encrypt accuracy test success\n");
        ret += 0;
    } else {
        PRINTF("AES128 encrypt accuracy test fail\n");
        ret += 1;
        goto err;
    }

    if (memcmp(plain, dec_data, AES128_TEST_SIZE) == 0) {
        PRINTF("AES128 decrypt accuracy test success\n");
        ret += 0;
    } else {
        PRINTF("AES128 decrypt accuracy test fail\n");
        ret += 1;
        goto err;
    }

    if (ret == 0)
        goto end;

err:
    dump_u8buf("IV", iv, sizeof(iv));
    dump_u8buf("KEY", key, sizeof(key));
    dump_u8buf("plain", plain, AES128_TEST_SIZE);
    dump_u8buf("enc_data", enc_data, AES128_TEST_SIZE);
    dump_u8buf("sw_enc", sw_enc, AES128_TEST_SIZE);
    dump_u8buf("dec_data", dec_data, AES128_TEST_SIZE);

end:
    qmx_hal_rng_deinit();
    free(plain);
    free(enc_data);
    free(dec_data);
    free(sw_enc);

    return ret;
}

int qmx_aes128_polling_test(void)
{
    uint8_t *plain = malloc(AES128_TEST_SIZE);
    uint8_t iv[16] = {0};
    uint8_t key[16] = {0};
    uint8_t *enc_data = malloc(AES128_TEST_SIZE);
    uint8_t *sw_enc = malloc(AES128_TEST_SIZE);
    int ret = 0;
    int i = 0;

    if (plain == NULL || enc_data == NULL || sw_enc == NULL) {
        PRINTF("malloc error\n");
        return -1;
    }

    qmx_hal_sysctrl_peripheral_clk_enable(HAL_CLK_RAND, true);
    qmx_hal_sysctrl_peripheral_mod_reset(HAL_CLK_RAND);

    hal_rng_init_t rng_init = {0};
    rng_init.seed = 0x12345678;
    qmx_hal_rng_init(&rng_init);

    ret = qmx_hal_get_random_u8(iv, sizeof(iv), 1000);
    if (ret != 0)
        PRINTF("get random IV error, ret=0x%X\n", ret);

    ret = qmx_hal_get_random_u8(key, sizeof(key), 1000);
    if (ret != 0)
        PRINTF("get random KEY error, ret=0x%X\n", ret);

    ret = qmx_hal_get_random_u8(plain, AES128_TEST_SIZE, 1000);
    if (ret != 0)
        PRINTF("get random plain error, ret=0x%X\n", ret);

    qmx_hal_sysctrl_peripheral_clk_enable(HAL_CLK_AES128, true);
    qmx_hal_sysctrl_peripheral_mod_reset(HAL_CLK_AES128);

    qmx_hal_aes_init(HAL_AES128_CBC, HAL_AES_ENC, key, iv);
    for (i = 0; i < AES128_TEST_SIZE; i+=HAL_AES128_BLOCK_SIZE) {
        qmx_hal_aes_update(plain+i, HAL_AES128_BLOCK_SIZE, enc_data+i);
    }
    qmx_hal_aes_final(HAL_AES128_CBC);

    AES128_CBC_encrypt_buffer(sw_enc, plain, AES128_TEST_SIZE, key, iv);
    if (memcmp(enc_data, sw_enc, AES128_TEST_SIZE) == 0) {
        PRINTF("AES128 encrypt accuracy test success\n");
        ret += 0;
    } else {
        PRINTF("AES128 encrypt accuracy test fail\n");
        ret += 1;
        goto err;
    }

    if (ret == 0)
        goto end;

err:
    dump_u8buf("IV", iv, sizeof(iv));
    dump_u8buf("KEY", key, sizeof(key));
    dump_u8buf("plain", plain, AES128_TEST_SIZE);
    dump_u8buf("enc_data", enc_data, AES128_TEST_SIZE);
    dump_u8buf("sw_enc", sw_enc, AES128_TEST_SIZE);

end:
    qmx_hal_rng_deinit();
    free(plain);
    free(enc_data);
    free(sw_enc);

    return ret;
}

int qmx_aes128_performance_test(void)
{
    uint8_t *plain = malloc(AES128_PERF_TEST_SIZE);
    uint8_t iv[16] = {0};
    uint8_t key[16] = {0};
    uint8_t *enc_data = malloc(AES128_PERF_TEST_SIZE);
    uint8_t *dec_data = malloc(AES128_PERF_TEST_SIZE);
    int ret = 0;
    uint32_t start_time = 0;
    uint32_t end_time = 0;
    uint16_t pakage_len[4] = {AES128_PERF_TEST_SIZE, 512, 256, 128};
    uint8_t idx = 0;

    if (plain == NULL || enc_data == NULL || dec_data == NULL) {
        PRINTF("malloc error\n");
        return -1;
    }

    qmx_hal_sysctrl_peripheral_clk_enable(HAL_CLK_RAND, true);
    qmx_hal_sysctrl_peripheral_mod_reset(HAL_CLK_RAND);

    hal_rng_init_t rng_init = {0};
    rng_init.seed = 0x12345678;
    qmx_hal_rng_init(&rng_init);

    ret = qmx_hal_get_random_u8(iv, sizeof(iv), 1000);
    if (ret != 0) {
        PRINTF("get random IV error, ret=0x%X\n", ret);
        goto end;
    }

    ret = qmx_hal_get_random_u8(key, sizeof(key), 1000);
    if (ret != 0) {
        PRINTF("get random KEY error, ret=0x%X\n", ret);
        goto end;
    }

    ret = qmx_hal_get_random_u8(plain, AES128_PERF_TEST_SIZE, 1000);
    if (ret != 0) {
        PRINTF("get random plain error, ret=0x%X\n", ret);
        goto end;
    }

    qmx_hal_sysctrl_peripheral_clk_enable(HAL_CLK_TIM0, true);
    qmx_hal_sysctrl_peripheral_mod_reset(HAL_CLK_TIM0);

    qmx_hal_timer_init(HAL_TIMER0, 0xFFFFFFFF, false);
    qmx_hal_timer_start(HAL_TIMER0);

    qmx_hal_sysctrl_peripheral_clk_enable(HAL_CLK_AES128, true);
    qmx_hal_sysctrl_peripheral_mod_reset(HAL_CLK_AES128);

start:
    start_time = qmx_hal_timer_get_count(HAL_TIMER0);
    qmx_hal_aes128_enc_once(key, iv, plain, pakage_len[idx], enc_data);
    end_time = qmx_hal_timer_get_count(HAL_TIMER0);
    PRINTF("AES128-CBC encrypt performance test, size: %u byte, cost time: %.2f us, perf: %.2f Mbps\n",
           pakage_len[idx], (float)(start_time - end_time) / 50.0f,
           pakage_len[idx] * 8 * 50.0f / (1.024f * 1.024f) / (start_time - end_time));

    start_time = qmx_hal_timer_get_count(HAL_TIMER0);
    qmx_hal_aes128_dec_once(key, iv, enc_data, pakage_len[idx], dec_data);
    end_time = qmx_hal_timer_get_count(HAL_TIMER0);
    PRINTF("AES128-CBC decrypt performance test, size: %u byte, cost time: %.2f us, perf: %.2f Mbps\n",
           pakage_len[idx], (float)(start_time - end_time) / 50.0f,
           pakage_len[idx] * 8 * 50.0f / (1.024f * 1.024f) / (start_time - end_time));

    if (memcmp(plain, dec_data, pakage_len[idx]) == 0) {
        PRINTF("AES128 encrypt/decrypt performance test success\n");
        ret = 0;
    } else {
        PRINTF("AES128 encrypt/decrypt performance test fail\n");
        ret = -1;
    }

    idx++;
    if (ret == 0 && idx < (sizeof(pakage_len)/sizeof(pakage_len[0])))
        goto start;



end:
    qmx_hal_timer_stop(HAL_TIMER0);
    qmx_hal_rng_deinit();
    free(plain);
    free(enc_data);
    free(dec_data);

    return ret;
}

int qmx_aes128_random_test(void)
{
    uint8_t *plain = malloc(AES128_PERF_TEST_SIZE);
    uint8_t iv[16] = {0};
    uint8_t key[16] = {0};
    uint8_t *enc_data = malloc(AES128_PERF_TEST_SIZE);
    uint8_t *dec_data = malloc(AES128_PERF_TEST_SIZE);
    uint8_t *sw_enc = malloc(AES128_PERF_TEST_SIZE);
    uint32_t cal_len = 0;
    int ret = 0;

    if (plain == NULL || enc_data == NULL || dec_data == NULL || sw_enc == NULL) {
        PRINTF("malloc error\n");
        return -1;
    }

    qmx_hal_sysctrl_peripheral_clk_enable(HAL_CLK_RAND, true);
    qmx_hal_sysctrl_peripheral_mod_reset(HAL_CLK_RAND);
    qmx_hal_sysctrl_peripheral_clk_enable(HAL_CLK_AES128, true);

    hal_rng_init_t rng_init = {0};
    rng_init.seed = 0x12345678;
    qmx_hal_rng_init(&rng_init);

once:
    memset(sw_enc, 0, AES128_PERF_TEST_SIZE);

    ret = qmx_hal_get_random_u8(iv, sizeof(iv), 1000);
    if (ret != 0) {
        PRINTF("get random IV error, ret=0x%X\n", ret);
        goto err;
    }

    ret = qmx_hal_get_random_u8(key, sizeof(key), 1000);
    if (ret != 0) {
        PRINTF("get random KEY error, ret=0x%X\n", ret);
        goto err;
    }

    cal_len = ((uint32_t *)key)[0];
    qmx_hal_update_rng_seed(cal_len);
    cal_len = (cal_len + 15) / 16 * 16; // align to 16 bytes
    cal_len = (cal_len % AES128_PERF_TEST_SIZE == 0) ? AES128_PERF_TEST_SIZE : cal_len % AES128_PERF_TEST_SIZE;

    ret = qmx_hal_get_random_u8(plain, cal_len, 1000);
    if (ret != 0) {
        PRINTF("get random plain error, ret=0x%X\n", ret);
        goto err;
    }

    qmx_hal_aes128_enc_once(key, iv, plain, cal_len, enc_data);
    qmx_hal_aes128_dec_once(key, iv, enc_data, cal_len, dec_data);

    AES128_CBC_encrypt_buffer(sw_enc, plain, cal_len, key, iv);
    if (memcmp(sw_enc, enc_data, cal_len) != 0) {
        PRINTF("AES128 random encrypt test fail\n");
        ret = -1;
        goto err;
    }

    if (memcmp(plain, dec_data, cal_len) != 0) {
        PRINTF("AES128 decrypt test fail\n");
        ret = -1;
        goto err;
    }

    PRINTF("AES128 random test success, cal_len=%u\n", cal_len);
    goto once;

err:
    dump_u8buf("plain", plain, cal_len);
    dump_u8buf("iv", iv, sizeof(iv));
    dump_u8buf("key", key, sizeof(key));
    dump_u8buf("enc_data", enc_data, cal_len);
    dump_u8buf("sw_enc", sw_enc, cal_len);
    dump_u8buf("dec_data", dec_data, cal_len);

    qmx_hal_rng_deinit();
    free(plain);
    free(enc_data);
    free(dec_data);
    free(sw_enc);

    return ret;
}
