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
#include "slc_hal_sysctrl.h"
#include "slc_hal_rng.h"
#include "slc_hal_timer.h"
#include "slc_rng_test.h"
#include "hw_rng.h"

/* RNG 性能测试：单次搬运数据量 1KB */
#define SLC_RNG_PERF_TEST_SIZE      1024
/* 随机数 word 个数（4 字节对齐） */
#define SLC_RNG_PERF_WORD_CNT       (SLC_RNG_PERF_TEST_SIZE / 4U)
/* 刷新分频 div=0，实际为 clk/(0+1)，即分频 1 */
#define SLC_RNG_PERF_DIV_MIN        0U
/* 刷新分频 div=1，实际为 clk/(1+1)，即分频 2 */
#define SLC_RNG_PERF_DIV_MAX        1U
/* 唯一值占比下限 75%，低于则认为有明显重复 */
#define SLC_RNG_UNIQUE_MIN_RATIO    75U

/**
 * @brief 检查随机数是否无明显重复
 * @param data     随机数缓冲区（u32）
 * @param word_cnt word 个数
 * @return 0=通过，-1=全 0 或重复过多
 */
static int slc_rng_check_no_obvious_repeat(const uint32_t *data, uint32_t word_cnt)
{
    uint32_t unique = 0;
    uint32_t min_unique = (word_cnt * SLC_RNG_UNIQUE_MIN_RATIO) / 100U;

    for (uint32_t i = 0; i < word_cnt; i++) {
        if (data[i] == 0) {
            return -1;
        }

        for (uint32_t j = 0; j < i; j++) {
            if (data[j] == data[i]) {
                goto next_word;
            }
        }
        unique++;

next_word:
        continue;
    }

    if (unique < min_unique) {
        PRINTF("[RNG] obvious repeat, unique=%u, min=%u\n", unique, min_unique);
        return -1;
    }

    return 0;
}

/**
 * @brief 单次 RNG 性能测试
 * @param freq_div   刷新率分频（寄存器值，实际分频 = freq_div + 1）
 * @param seed       随机数种子
 * @param data       输出缓冲区
 * @param cost_ticks 输出：Timer0 计数值差（start - end）
 * @return 0=成功，-1=失败
 */
static int slc_rng_perf_run_once(uint8_t freq_div, uint32_t seed, uint32_t *data,
                                 uint32_t *cost_ticks)
{
    uint32_t start_time;
    uint32_t end_time;
    int ret;

    /* 使能 RNG，配置刷新分频并初始化种子 */
    rom_hw_rng_init(freq_div, true);
    rom_hw_update_rng_seed(seed);

    /* 生成 1K 随机数并统计耗时 */
    start_time = slc_hal_timer_get_count(HAL_TIMER0);
    ret = slc_hal_get_random_u32(data, SLC_RNG_PERF_TEST_SIZE, 50000);
    end_time = slc_hal_timer_get_count(HAL_TIMER0);

    if (ret != 0) {
        PRINTF("[RNG] get random fail, freq_div=%u, ret=%d\n", freq_div, ret);
        return ret;
    }

    /* check：数据无明显重复 */
    if (slc_rng_check_no_obvious_repeat(data, SLC_RNG_PERF_WORD_CNT) != 0) {
        return -1;
    }

    *cost_ticks = start_time - end_time;
    return 0;
}

int slc_rng_generate_test(void)
{
    slc_hal_sysctrl_peripheral_clk_enable(HAL_CLK_RAND, true);
    slc_hal_sysctrl_peripheral_mod_reset(HAL_CLK_RAND);

    hal_rng_init_t rng_init = {0};
    rng_init.seed = 0x12345678;
    slc_hal_rng_init(&rng_init);
    uint32_t buf[4] = {0};
    int ret = 0;
    ret = slc_hal_get_random_u32(buf, sizeof(buf), 1000);
    if (ret != 0)
        PRINTF("get random u32 error, ret=0x%X\n", ret);
    else
        dump_u32buf("rng", buf, sizeof(buf));

    slc_hal_rng_deinit();

    if (buf[0] != 0 && buf[1] != 0 && buf[2] != 0 && buf[3] != 0)
        return 0;
    else
        return -1;
}

/**
 * @brief RNG 性能测试（两种分频各测 1K，校验重复性与耗时比例）
 * @return 0=PASS，-1=FAIL
 */
int slc_rng_perf_test(void)
{
    const uint8_t div_list[] = {SLC_RNG_PERF_DIV_MIN, SLC_RNG_PERF_DIV_MAX};
    const uint32_t seed = 0xACBD6543;
    uint32_t cost_ticks[2] = {0};
    uint32_t *data = malloc(SLC_RNG_PERF_TEST_SIZE);
    int ret = 0;
    float time_us[2];
    float perf_mbps[2];

    if (data == NULL) {
        PRINTF("malloc error\n");
        return -1;
    }

    /* 初始化 Timer0 用于测时 */
    slc_hal_sysctrl_peripheral_clk_enable(HAL_CLK_TIM0, true);
    slc_hal_sysctrl_peripheral_mod_reset(HAL_CLK_TIM0);
    slc_hal_timer_init(HAL_TIMER0, 0xFFFFFFFF, false);
    slc_hal_timer_start(HAL_TIMER0);

    slc_hal_sysctrl_peripheral_clk_enable(HAL_CLK_RAND, true);
    slc_hal_sysctrl_peripheral_mod_reset(HAL_CLK_RAND);

    /* 分频 1 和分频 2 各测一次 */
    for (int i = 0; i < 2; i++) {
        uint8_t freq_div = div_list[i];

        ret = slc_rng_perf_run_once(freq_div, seed, data, &cost_ticks[i]);
        if (ret != 0) {
            goto end;
        }

        time_us[i] = cost_ticks[i] / 50.0f;
        perf_mbps[i] = SLC_RNG_PERF_TEST_SIZE * 8.0f * 50.0f /
                       (1.024f * 1.024f * (float)cost_ticks[i]);

        PRINTF("[RNG] perf div=%u (clk/%u), size=%u byte, time=%.2f us, perf=%.2f Mbps\n",
               freq_div, freq_div + 1U, SLC_RNG_PERF_TEST_SIZE, time_us[i], perf_mbps[i]);
    }

    /* check：两次耗时比例应与分频倍数一致（±30% 容差） */
    {
        uint32_t expect_ratio_x10 = ((div_list[1] + 1U) * 10U) / (div_list[0] + 1U);
        uint32_t actual_ratio_x10 = (cost_ticks[1] * 10U) / cost_ticks[0];

        PRINTF("[RNG] perf ratio: actual=%u.%u, expect=%u.%u (time div%d/time div%d)\n",
               actual_ratio_x10 / 10U, actual_ratio_x10 % 10U,
               expect_ratio_x10 / 10U, expect_ratio_x10 % 10U,
               div_list[1] + 1U, div_list[0] + 1U);

        if ((actual_ratio_x10 < (expect_ratio_x10 * 7U / 10U)) ||
            (actual_ratio_x10 > (expect_ratio_x10 * 13U / 10U))) {
            PRINTF("[RNG] perf ratio check fail\n");
            ret = -1;
            goto end;
        }
    }

    PRINTF("====>RNG performance test success\n");

end:
    rom_hw_rng_init(0, false);
    slc_hal_timer_stop(HAL_TIMER0);
    free(data);
    return ret;
}


#define RNG_TEST_DATA_SIZE  5000    // 最小5000byte，测试结果更可靠
#define GROUP_SIZE          5

// 卡方分布临界值表（自由度为6，显著性水平0.05）
#define CHI2_CRITICAL       12.59

// 扑克测试：检验随机数序列的随机性
int slc_rng_poker_test(void)
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

    slc_hal_sysctrl_peripheral_clk_enable(HAL_CLK_RAND, true);
    slc_hal_sysctrl_peripheral_mod_reset(HAL_CLK_RAND);

    slc_hal_rng_init(&rng_init);
    int ret = slc_hal_get_random_u8(data, RNG_TEST_DATA_SIZE, 1000);
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

    slc_hal_rng_deinit();
    free(data);
    return ret;
}
