#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>

#include "app_cfg.h"
#include "utility.h"
#include "debug.h"
#include "reg_Dev.h"
#include "error_def.h"
#include "qmx_hal_sysctrl.h"
#include "qmx_hal_rng.h"
#include "qmx_hal_timer.h"
#include "qmx_rng_test.h"

int qmx_rng_generate_test(void)
{
    qmx_hal_sysctrl_peripheral_clk_enable(HAL_CLK_RAND, true);
    qmx_hal_sysctrl_peripheral_mod_reset(HAL_CLK_RAND);

    hal_rng_init_t rng_init = {0};
    rng_init.seed = 0x12345678;
    qmx_hal_rng_init(&rng_init);
    uint32_t buf[4] = {0};
    int ret = 0;
    ret = qmx_hal_get_random_u32(buf, sizeof(buf), 1000);
    if (ret != 0)
        PRINTF("get random u32 error, ret=0x%X\n", ret);
    else
        dump_u32buf("rng", buf, sizeof(buf));

    qmx_hal_rng_deinit();

    if (buf[0] != 0 && buf[1] != 0 && buf[2] != 0 && buf[3] != 0)
        return 0;
    else
        return -1;
}

#define QMX_RNG_PERF_TEST_SIZE  1024
int qmx_rng_perf_test(void)
{
    uint32_t rng_test_start_time = 0;
    uint32_t rng_test_end_time = 0;
    uint8_t div = 0;
    hal_rng_init_t rng_init = {0};
    uint32_t *data = malloc(QMX_RNG_PERF_TEST_SIZE);
    int ret = 0;

    if (data == NULL) {
        PRINTF("malloc error\n");
        return -1;
    }

    rng_init.seed = 0xACBD6543;

    qmx_hal_sysctrl_peripheral_clk_enable(HAL_CLK_TIM0, true);
    qmx_hal_sysctrl_peripheral_mod_reset(HAL_CLK_TIM0);

    qmx_hal_timer_init(HAL_TIMER0, 0xFFFFFFFF, false);
    qmx_hal_timer_start(HAL_TIMER0);

    qmx_hal_sysctrl_peripheral_clk_enable(HAL_CLK_RAND, true);
    qmx_hal_sysctrl_peripheral_mod_reset(HAL_CLK_RAND);

    qmx_hal_rng_init(&rng_init);

    rng_test_start_time = 0;
    rng_test_end_time = 0;

    rng_test_start_time = qmx_hal_timer_get_count(HAL_TIMER0);
    ret = qmx_hal_get_random_u32(data, QMX_RNG_PERF_TEST_SIZE, 1000);
    rng_test_end_time = qmx_hal_timer_get_count(HAL_TIMER0);
    if (ret != 0) {
        PRINTF("get random u32 error, ret=0x%X\n", ret);
        goto end;
    }

    PRINTF("====>rng performance test, generate data: %u byte, cost time: %.2f us, perf: %.2f Mbps\n",
            QMX_RNG_PERF_TEST_SIZE, (rng_test_start_time - rng_test_end_time) / 50.0f,
            QMX_RNG_PERF_TEST_SIZE * 8 * 50.0f / (1.024f * 1.024f) / (rng_test_start_time - rng_test_end_time));

end:
    qmx_hal_rng_deinit();
    qmx_hal_timer_stop(HAL_TIMER0);
    free(data);
    return ret;
}


#define RNG_TEST_DATA_SIZE  5000    // 最小5000byte，测试结果更可靠
#define GROUP_SIZE          5

// 卡方分布临界值表（自由度为6，显著性水平0.05）
#define CHI2_CRITICAL       12.59

// 扑克测试：检验随机数序列的随机性
int qmx_rng_poker_test(void)
{
    uint8_t *data = malloc(RNG_TEST_DATA_SIZE);
    int n = RNG_TEST_DATA_SIZE;
    int r = 16; // 基数，表示每位数字的范围是 0 到 r-1
    hal_rng_init_t rng_init = {0};

    rng_init.seed = 0x12345678;

    if (data == NULL) {
        PRINTF("malloc error\n");
        return -1;
    }

    if (n % GROUP_SIZE != 0) {
        PRINTF("data len must align 5\n");
        free(data);
        return -1;
    }

    qmx_hal_sysctrl_peripheral_clk_enable(HAL_CLK_RAND, true);
    qmx_hal_sysctrl_peripheral_mod_reset(HAL_CLK_RAND);

    qmx_hal_rng_init(&rng_init);
    int ret = qmx_hal_get_random_u8(data, RNG_TEST_DATA_SIZE, 1000);
    if (ret != EN_ERROR_STA_OK) {
        PRINTF("get random u8 error, ret=0x%X\n", ret);
        free(data);
        return ret;
    }

    int groups = n / GROUP_SIZE;

    // 分类计数：0=全部不同, 1=一对, 2=两对, 3=三条, 4=葫芦, 5=四条, 6=五条
    int freq[7] = {0};

    for (int i = 0; i < n; i += 5) {
        int count[256] = {0};   // 支持 0~255
        for (int j = 0; j < 5; j++) {
            int digit = data[i + j];

            digit %= r;
            count[digit]++;
        }

        // 统计频次的频次（例如：有几个数字出现2次）
        int freq_of_count[6] = {0};  // freq_of_count[k] = 有多少个数字出现了 k 次
        for (int j = 0; j < r; j++) {
            if (count[j] > 0) {
                if (count[j] > 0 && count[j] < 6) freq_of_count[count[j]]++;
            }
        }

        // 判断模式
        if (freq_of_count[5] == 1) {
            freq[6]++; // 五条 AAAAA
        } else if (freq_of_count[4] == 1) {
            freq[5]++; // 四条 AAAAB
        } else if (freq_of_count[3] == 1 && freq_of_count[2] == 1) {
            freq[4]++; // 葫芦 AAABB
        } else if (freq_of_count[3] == 1) {
            freq[3]++; // 三条 AAABC
        } else if (freq_of_count[2] == 2) {
            freq[2]++; // 两对 AABBC
        } else if (freq_of_count[2] == 1) {
            freq[1]++; // 一对 AABCD
        } else {
            freq[0]++; // 全不同 ABCDE
        }
    }

    // 理论概率（基于组合数学）
    double expected[7];
    double total = groups;

    expected[0] = total * (r * (r-1) * (r-2) * (r-3) * (r-4)) / pow(r, 5);  // 全不同
    expected[1] = total * (10 * r * (r-1) * (r-2) * (r-3)) / pow(r, 5);     // 一对
    expected[2] = total * (15 * r * (r-1) * (r-2)) / pow(r, 5);             // 两对
    expected[3] = total * (10 * r * (r-1) * (r-2)) / pow(r, 5);             // 三条
    expected[4] = total * (10 * r * (r-1)) / pow(r, 5);                     // 葫芦
    expected[5] = total * (5 * r * (r-1)) / pow(r, 5);                      // 四条
    expected[6] = total * 1 / pow(r, 4);                                    // 五条

    // 计算卡方统计量
    double chi_square = 0.0;
    for (int i = 0; i < 7; i++) {
        if (expected[i] > 0.5) {
            chi_square += pow(freq[i] - expected[i], 2) / expected[i];
        }
    }

    // 输出结果
    PRINTF("Poker Test Results (Groups: %d, Base r=%d):\n", groups, r);
    PRINTF("Pattern\t\tObserved\tExpected\tDeviation\n");
    const char *labels[] = {"All different", "One pair", "Two pairs", "Three of a kind", "Full house", "Four of a kind", "Five of a kind"};
    for (int i = 0; i < 7; i++) {
        PRINTF("%s\t%d\t%.2f\t%.2f\n", labels[i], freq[i], expected[i], freq[i] - expected[i]);
    }

    // 自由度 = 6，显著性水平 α=0.05 时，临界值约为 12.59
    if (chi_square < CHI2_CRITICAL) {
        PRINTF("test success, chi_square %.4f\n", chi_square);
    } else {
        PRINTF("test fail, chi_square %.4f\n", chi_square);
        ret = -1;
        dump_u8buf("rng", data, 1024);
    }

    qmx_hal_rng_deinit();
    free(data);
    return ret;
}
