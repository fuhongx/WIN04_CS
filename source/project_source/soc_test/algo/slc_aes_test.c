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
#include "slc_aes_test.h"
#include "slc_hal_sysctrl.h"
#include "slc_hal_aes.h"
#include "slc_hal_rng.h"
#include "slc_hal_timer.h"
#include "slc_hal_intc.h"
#include "hw_aes.h"
#include "sw_aes.h"

#define AES128_TEST_SIZE        64
#define AES128_PERF_TEST_SIZE   1024

static volatile bool g_aes_irq_block_done;
static volatile bool g_aes_irq_bytes_done;
static volatile uint32_t g_aes_irq_block_cnt;
static volatile uint32_t g_aes_irq_bytes_cnt;
static volatile uint32_t g_aes_irq_round_cnt;
static volatile uint8_t g_aes_irq_clr_fail;

static uint32_t aes_test_data_to_be32(const uint8_t *data)
{
    return ((uint32_t)data[0] << 24) |
           ((uint32_t)data[1] << 16) |
           ((uint32_t)data[2] << 8)  |
           ((uint32_t)data[3]);
}

static void aes_test_data_to_le(uint32_t data, uint8_t *out)
{
    out[0] = (uint8_t)(data >> 24);
    out[1] = (uint8_t)(data >> 16);
    out[2] = (uint8_t)(data >> 8);
    out[3] = (uint8_t)(data);
}

static bool aes_test_irq_wait_flag(volatile bool *flag, uint32_t timeout)
{
    while (timeout--) {
        if (*flag) {
            return true;
        }
    }

    return false;
}

static void aes_test_irq_check_clear(uint8_t int_bit)
{
    uint8_t after_clr = 0;

    rom_hw_aes128_get_interrupt_flag(&after_clr);
    if (after_clr & int_bit) {
        g_aes_irq_clr_fail |= int_bit;
    }
}

void slc_aes128_irq_handler(void)
{
    uint8_t sta = 0;

    rom_hw_aes128_get_interrupt_flag(&sta);

    if (sta & EN_AES128_INT_BLOCK_DONE) {
        g_aes_irq_block_cnt++;
        rom_hw_aes128_clear_interrupt_flag(EN_AES128_INT_BLOCK_DONE);
        aes_test_irq_check_clear(EN_AES128_INT_BLOCK_DONE);
        g_aes_irq_block_done = true;
    }

    if (sta & EN_AES128_INT_BYTES_DONE) {
        g_aes_irq_bytes_cnt++;
        rom_hw_aes128_clear_interrupt_flag(EN_AES128_INT_BYTES_DONE);
        aes_test_irq_check_clear(EN_AES128_INT_BYTES_DONE);
        g_aes_irq_bytes_done = true;
    }

    if (sta & EN_AES128_INT_ROUND_DONE) {
        g_aes_irq_round_cnt++;
        rom_hw_aes128_clear_interrupt_flag(EN_AES128_INT_ROUND_DONE);
        aes_test_irq_check_clear(EN_AES128_INT_ROUND_DONE);
    }
}

static void aes128_irq_prepare(uint8_t int_mask)
{
    rom_hw_aes128_clear_interrupt_flag(EN_AES128_INT_BLOCK_DONE |
                                       EN_AES128_INT_BYTES_DONE |
                                       EN_AES128_INT_ROUND_DONE);
    rom_hw_aes128_set_interrupt_mask(int_mask, false);
    AES128->AES_INT_IER = int_mask & AES_INT_IER_REG_MASK;
}

static int aes128_irq_run_block(const uint8_t *plain_block, uint8_t *enc_block)
{
    uint8_t i = 0;
    uint32_t temp[AES128_BLOCK_SIZE / 4] = {0};
    uint32_t timeout = 0;

    for (i = 0; i < AES128_BLOCK_SIZE; i += 4) {
        temp[AES128_BLOCK_SIZE / 4 - 1 - i / 4] = aes_test_data_to_be32(plain_block + i);
    }

    g_aes_irq_block_done = false;
    rom_hw_aes128_clear_interrupt_flag(EN_AES128_INT_BLOCK_DONE);
    rom_hw_aes128_set_data(temp);
    AES128->AES_START = 1;

    timeout = AES128_WAIT_TIMEOUT;
    if (!aes_test_irq_wait_flag(&g_aes_irq_block_done, timeout)) {
        PRINTF("AES128 block irq timeout\n");
        return -1;
    }

    rom_hw_aes128_get_data(temp);
    for (i = 0; i < AES128_BLOCK_SIZE; i += 4) {
        aes_test_data_to_le(temp[AES128_BLOCK_SIZE / 4 - 1 - i / 4], enc_block + i);
    }

    return 0;
}

static int aes128_irq_encrypt(uint8_t *key, uint8_t *iv, uint8_t *plain, uint32_t len, uint8_t *enc_data)
{
    uint8_t *plain_ptr = plain;
    uint32_t round = 0;

    if ((key == NULL) || (iv == NULL) || (plain == NULL) || (enc_data == NULL) ||
        (len == 0) || ((len % AES128_BLOCK_SIZE) != 0)) {
        return -1;
    }

    g_aes_irq_block_cnt = 0;
    g_aes_irq_bytes_cnt = 0;
    g_aes_irq_round_cnt = 0;
    g_aes_irq_clr_fail = 0;

    rom_hw_aes128_init(EN_AES128_MODE_ENCRYPT, key, iv);
    aes128_irq_prepare(EN_AES128_INT_BLOCK_DONE);

    g_aes_total_len += len;
    round = len / AES128_BLOCK_SIZE;
    AES128->AES_BYTE_NUM = g_aes_total_len;
    AES128->AES_ROUND_NUM = 1;

    while (round--) {
        if (aes128_irq_run_block(plain_ptr, enc_data) != 0) {
            return -1;
        }
        plain_ptr += AES128_BLOCK_SIZE;
        enc_data += AES128_BLOCK_SIZE;
    }

    rom_hw_aes128_clear_interrupt_flag(EN_AES128_INT_BLOCK_DONE |
                                       EN_AES128_INT_BYTES_DONE |
                                       EN_AES128_INT_ROUND_DONE);
    AES128->AES_START = 0;

    return 0;
}

static void aes128_irq_disable(void)
{
    AES128->AES_INT_IER = 0;
    rom_hw_aes128_set_interrupt_mask(EN_AES128_INT_BLOCK_DONE |
                                     EN_AES128_INT_BYTES_DONE |
                                     EN_AES128_INT_ROUND_DONE, true);
    rom_hw_aes128_clear_interrupt_flag(EN_AES128_INT_BLOCK_DONE |
                                       EN_AES128_INT_BYTES_DONE |
                                       EN_AES128_INT_ROUND_DONE);
    AES128->AES_START = 0;
}

static int aes128_irq_bytes_round_test(uint8_t *key, uint8_t *iv, uint8_t *plain_block, uint8_t *enc_block)
{
    uint8_t final_sta = 0;

    g_aes_irq_block_cnt = 0;
    g_aes_irq_bytes_cnt = 0;
    g_aes_irq_round_cnt = 0;
    g_aes_irq_clr_fail = 0;
    g_aes_irq_bytes_done = false;

    rom_hw_aes128_init(EN_AES128_MODE_ENCRYPT, key, iv);
    aes128_irq_prepare(EN_AES128_INT_BLOCK_DONE |
                       EN_AES128_INT_BYTES_DONE |
                       EN_AES128_INT_ROUND_DONE);

    g_aes_total_len = AES128_BLOCK_SIZE;
    AES128->AES_BYTE_NUM = AES128_BLOCK_SIZE;
    AES128->AES_ROUND_NUM = 1;

    if (aes128_irq_run_block(plain_block, enc_block) != 0) {
        return -1;
    }

    if (!aes_test_irq_wait_flag(&g_aes_irq_bytes_done, AES128_WAIT_TIMEOUT)) {
        PRINTF("AES128 bytes done irq timeout\n");
        return -1;
    }

    if (g_aes_irq_bytes_cnt == 0) {
        PRINTF("AES128 bytes done irq not triggered\n");
        return -1;
    }

    if (g_aes_irq_round_cnt == 0) {
        PRINTF("AES128 round done irq not triggered\n");
        return -1;
    }

    if (g_aes_irq_clr_fail != 0) {
        PRINTF("AES128 bytes/round irq clear failed, fail=0x%02X\n", g_aes_irq_clr_fail);
        return -1;
    }

    rom_hw_aes128_get_interrupt_flag(&final_sta);
    if (final_sta != 0) {
        PRINTF("AES128 bytes/round irq flag not cleared, sta=0x%02X\n", final_sta);
        return -1;
    }

    rom_hw_aes128_clear_interrupt_flag(EN_AES128_INT_BLOCK_DONE |
                                       EN_AES128_INT_BYTES_DONE |
                                       EN_AES128_INT_ROUND_DONE);
    AES128->AES_START = 0;

    return 0;
}

int slc_aes128_interrupt_test(void)
{
    uint8_t key[16] = {0};
    uint8_t iv[16] = {0};
    uint8_t block_plain[AES128_BLOCK_SIZE] = {0};
    uint8_t block_enc[AES128_BLOCK_SIZE] = {0};
    uint8_t block_sw_enc[AES128_BLOCK_SIZE] = {0};
    uint8_t *plain = malloc(AES128_TEST_SIZE);
    uint8_t *enc_data = malloc(AES128_TEST_SIZE);
    uint8_t *ref_enc = malloc(AES128_TEST_SIZE);
    uint8_t *sw_enc = malloc(AES128_TEST_SIZE);
    uint8_t final_sta = 0;
    uint32_t block_num = AES128_TEST_SIZE / AES128_BLOCK_SIZE;
    int ret = 0;

    if (plain == NULL || enc_data == NULL || ref_enc == NULL || sw_enc == NULL) {
        PRINTF("malloc error\n");
        return -1;
    }

    slc_hal_sysctrl_peripheral_clk_enable(HAL_CLK_RAND, true);
    slc_hal_sysctrl_peripheral_mod_reset(HAL_CLK_RAND);
    slc_hal_sysctrl_peripheral_clk_enable(HAL_CLK_AES128, true);
    slc_hal_sysctrl_peripheral_mod_reset(HAL_CLK_AES128);

    hal_rng_init_t rng_init = {0};
    rng_init.seed = 0x12345678;
    slc_hal_rng_init(&rng_init);

    ret = slc_hal_get_random_u8(iv, sizeof(iv), 1000);
    if (ret != 0) {
        PRINTF("get random IV error, ret=0x%X\n", ret);
        goto end;
    }

    ret = slc_hal_get_random_u8(key, sizeof(key), 1000);
    if (ret != 0) {
        PRINTF("get random KEY error, ret=0x%X\n", ret);
        goto end;
    }

    ret = slc_hal_get_random_u8(plain, AES128_TEST_SIZE, 1000);
    if (ret != 0) {
        PRINTF("get random plain error, ret=0x%X\n", ret);
        goto end;
    }

    memcpy(block_plain, plain, AES128_BLOCK_SIZE);

    slc_hal_aes128_enc_once(key, iv, plain, AES128_TEST_SIZE, ref_enc);
    AES128_CBC_encrypt_buffer(sw_enc, plain, AES128_TEST_SIZE, key, iv);
    AES128_CBC_encrypt_buffer(block_sw_enc, block_plain, AES128_BLOCK_SIZE, key, iv);

    slc_hal_register_irq_handler(AES128_IRQ, slc_aes128_irq_handler);
    SLC_HAL_ENABLE_PERIPHERAL_IRQ(AES128_IRQ, 0x3);

    ret = aes128_irq_encrypt(key, iv, plain, AES128_TEST_SIZE, enc_data);
    if (ret != 0) {
        PRINTF("AES128 block interrupt encrypt failed\n");
        goto cleanup;
    }

    rom_hw_aes128_get_interrupt_flag(&final_sta);
    if (final_sta != 0) {
        PRINTF("AES128 block irq flag not cleared, sta=0x%02X\n", final_sta);
        ret = -1;
        goto err;
    }

    if (g_aes_irq_block_cnt != block_num) {
        PRINTF("AES128 block irq cnt err, %u != %u\n", g_aes_irq_block_cnt, block_num);
        ret = -1;
        goto err;
    }

    if (g_aes_irq_clr_fail != 0) {
        PRINTF("AES128 block irq clear failed, fail=0x%02X\n", g_aes_irq_clr_fail);
        ret = -1;
        goto err;
    }

    {
        uint32_t phase1_block_cnt = g_aes_irq_block_cnt;

        ret = aes128_irq_bytes_round_test(key, iv, block_plain, block_enc);
        if (ret != 0) {
            PRINTF("AES128 bytes/round interrupt test failed\n");
            goto err;
        }

        aes128_irq_disable();

        if (memcmp(enc_data, ref_enc, AES128_TEST_SIZE) != 0) {
            PRINTF("AES128 interrupt encrypt mismatch with enc_once\n");
            ret = -1;
            goto err;
        }

        if (memcmp(enc_data, sw_enc, AES128_TEST_SIZE) != 0) {
            PRINTF("AES128 interrupt encrypt mismatch with sw lib\n");
            ret = -1;
            goto err;
        }

        if (memcmp(block_enc, block_sw_enc, AES128_BLOCK_SIZE) != 0) {
            PRINTF("AES128 single block interrupt encrypt mismatch\n");
            ret = -1;
            goto err;
        }

        PRINTF("AES128 interrupt test success, block_irq=%u bytes_irq=%u round_irq=%u\n",
               phase1_block_cnt, g_aes_irq_bytes_cnt, g_aes_irq_round_cnt);
    }
    goto cleanup;

err:
    aes128_irq_disable();
    dump_u8buf("IV", iv, sizeof(iv));
    dump_u8buf("KEY", key, sizeof(key));
    dump_u8buf("plain", plain, AES128_TEST_SIZE);
    dump_u8buf("enc_data", enc_data, AES128_TEST_SIZE);
    dump_u8buf("ref_enc", ref_enc, AES128_TEST_SIZE);
    dump_u8buf("sw_enc", sw_enc, AES128_TEST_SIZE);
    dump_u8buf("block_enc", block_enc, AES128_BLOCK_SIZE);
    dump_u8buf("block_sw_enc", block_sw_enc, AES128_BLOCK_SIZE);

cleanup:
    aes128_irq_disable();
    SLC_HAL_DISABLE_PERIPHERAL_IRQ(AES128_IRQ);
    slc_hal_unregister_irq_handler(AES128_IRQ);

end:
    slc_hal_rng_deinit();
    free(plain);
    free(enc_data);
    free(ref_enc);
    free(sw_enc);

    return ret;
}

int slc_aes128_accuracy_test(void)
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

    slc_hal_sysctrl_peripheral_clk_enable(HAL_CLK_RAND, true);
    slc_hal_sysctrl_peripheral_mod_reset(HAL_CLK_RAND);

    hal_rng_init_t rng_init = {0};
    rng_init.seed = 0x12345678;
    slc_hal_rng_init(&rng_init);

    ret = slc_hal_get_random_u8(iv, sizeof(iv), 1000);
    if (ret != 0)
        PRINTF("get random IV error, ret=0x%X\n", ret);

    ret = slc_hal_get_random_u8(key, sizeof(key), 1000);
    if (ret != 0)
        PRINTF("get random KEY error, ret=0x%X\n", ret);

    ret = slc_hal_get_random_u8(plain, AES128_TEST_SIZE, 1000);
    if (ret != 0)
        PRINTF("get random plain error, ret=0x%X\n", ret);

    slc_hal_sysctrl_peripheral_clk_enable(HAL_CLK_AES128, true);
    slc_hal_sysctrl_peripheral_mod_reset(HAL_CLK_AES128);

    slc_hal_aes128_enc_once(key, iv, plain, AES128_TEST_SIZE, enc_data);
    slc_hal_aes128_dec_once(key, iv, enc_data, AES128_TEST_SIZE, dec_data);

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
    slc_hal_rng_deinit();
    free(plain);
    free(enc_data);
    free(dec_data);
    free(sw_enc);

    return ret;
}

int slc_aes128_polling_test(void)
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

    slc_hal_sysctrl_peripheral_clk_enable(HAL_CLK_RAND, true);
    slc_hal_sysctrl_peripheral_mod_reset(HAL_CLK_RAND);

    hal_rng_init_t rng_init = {0};
    rng_init.seed = 0x12345678;
    slc_hal_rng_init(&rng_init);

    ret = slc_hal_get_random_u8(iv, sizeof(iv), 1000);
    if (ret != 0)
        PRINTF("get random IV error, ret=0x%X\n", ret);

    ret = slc_hal_get_random_u8(key, sizeof(key), 1000);
    if (ret != 0)
        PRINTF("get random KEY error, ret=0x%X\n", ret);

    ret = slc_hal_get_random_u8(plain, AES128_TEST_SIZE, 1000);
    if (ret != 0)
        PRINTF("get random plain error, ret=0x%X\n", ret);

    slc_hal_sysctrl_peripheral_clk_enable(HAL_CLK_AES128, true);
    slc_hal_sysctrl_peripheral_mod_reset(HAL_CLK_AES128);

    slc_hal_aes_init(HAL_AES128_CBC, HAL_AES_ENC, key, iv);
    for (i = 0; i < AES128_TEST_SIZE; i+=HAL_AES128_BLOCK_SIZE) {
        slc_hal_aes_update(plain+i, HAL_AES128_BLOCK_SIZE, enc_data+i);
    }
    slc_hal_aes_final(HAL_AES128_CBC);

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
    slc_hal_rng_deinit();
    free(plain);
    free(enc_data);
    free(sw_enc);

    return ret;
}

int slc_aes128_performance_test(void)
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

    slc_hal_sysctrl_peripheral_clk_enable(HAL_CLK_RAND, true);
    slc_hal_sysctrl_peripheral_mod_reset(HAL_CLK_RAND);

    hal_rng_init_t rng_init = {0};
    rng_init.seed = 0x12345678;
    slc_hal_rng_init(&rng_init);

    ret = slc_hal_get_random_u8(iv, sizeof(iv), 1000);
    if (ret != 0) {
        PRINTF("get random IV error, ret=0x%X\n", ret);
        goto end;
    }

    ret = slc_hal_get_random_u8(key, sizeof(key), 1000);
    if (ret != 0) {
        PRINTF("get random KEY error, ret=0x%X\n", ret);
        goto end;
    }

    ret = slc_hal_get_random_u8(plain, AES128_PERF_TEST_SIZE, 1000);
    if (ret != 0) {
        PRINTF("get random plain error, ret=0x%X\n", ret);
        goto end;
    }

    slc_hal_sysctrl_peripheral_clk_enable(HAL_CLK_TIM0, true);
    slc_hal_sysctrl_peripheral_mod_reset(HAL_CLK_TIM0);

    slc_hal_timer_init(HAL_TIMER0, 0xFFFFFFFF, false);
    slc_hal_timer_start(HAL_TIMER0);

    slc_hal_sysctrl_peripheral_clk_enable(HAL_CLK_AES128, true);
    slc_hal_sysctrl_peripheral_mod_reset(HAL_CLK_AES128);

start:
    start_time = slc_hal_timer_get_count(HAL_TIMER0);
    slc_hal_aes128_enc_once(key, iv, plain, pakage_len[idx], enc_data);
    end_time = slc_hal_timer_get_count(HAL_TIMER0);
    PRINTF("AES128-CBC encrypt performance test, size: %u byte, cost time: %.2f us, perf: %.2f Mbps\n",
           pakage_len[idx], (float)(start_time - end_time) / 50.0f,
           pakage_len[idx] * 8 * 50.0f / (1.024f * 1.024f) / (start_time - end_time));

    start_time = slc_hal_timer_get_count(HAL_TIMER0);
    slc_hal_aes128_dec_once(key, iv, enc_data, pakage_len[idx], dec_data);
    end_time = slc_hal_timer_get_count(HAL_TIMER0);
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
    slc_hal_timer_stop(HAL_TIMER0);
    slc_hal_rng_deinit();
    free(plain);
    free(enc_data);
    free(dec_data);

    return ret;
}

int slc_aes128_random_test(void)
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

    slc_hal_sysctrl_peripheral_clk_enable(HAL_CLK_RAND, true);
    slc_hal_sysctrl_peripheral_mod_reset(HAL_CLK_RAND);
    slc_hal_sysctrl_peripheral_clk_enable(HAL_CLK_AES128, true);

    hal_rng_init_t rng_init = {0};
    rng_init.seed = 0x12345678;
    slc_hal_rng_init(&rng_init);

once:
    memset(sw_enc, 0, AES128_PERF_TEST_SIZE);

    ret = slc_hal_get_random_u8(iv, sizeof(iv), 1000);
    if (ret != 0) {
        PRINTF("get random IV error, ret=0x%X\n", ret);
        goto err;
    }

    ret = slc_hal_get_random_u8(key, sizeof(key), 1000);
    if (ret != 0) {
        PRINTF("get random KEY error, ret=0x%X\n", ret);
        goto err;
    }

    cal_len = ((uint32_t *)key)[0];
    slc_hal_update_rng_seed(cal_len);
    cal_len = (cal_len + 15) / 16 * 16; // align to 16 bytes
    cal_len = (cal_len % AES128_PERF_TEST_SIZE == 0) ? AES128_PERF_TEST_SIZE : cal_len % AES128_PERF_TEST_SIZE;

    ret = slc_hal_get_random_u8(plain, cal_len, 1000);
    if (ret != 0) {
        PRINTF("get random plain error, ret=0x%X\n", ret);
        goto err;
    }

    slc_hal_aes128_enc_once(key, iv, plain, cal_len, enc_data);
    slc_hal_aes128_dec_once(key, iv, enc_data, cal_len, dec_data);

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

    slc_hal_rng_deinit();
    free(plain);
    free(enc_data);
    free(dec_data);
    free(sw_enc);

    return ret;
}
