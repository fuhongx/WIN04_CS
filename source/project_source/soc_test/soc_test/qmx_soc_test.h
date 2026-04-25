#ifndef __QMX_SOC_TEST_H__
#define __QMX_SOC_TEST_H__

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "reg_Dev.h"
#include "error_def.h"
#include "utility.h"
#include "debug.h"

#define MAX_SOCTEST_CASE_NAME_LEN   (48)
#define MAX_SOCTEST_MOD_NAME_LEN    (16)

typedef int (*soctest_case)(void);

typedef void (*soctest_mod_init)(void);
typedef void (*soctest_mod_deinit)(void);

enum case_type_id {
    SOCTEST_SMOKE,
    SOCTEST_AUTO,
    SOCTEST_MANUAL,
    SOCTEST_PRESSURE,
    SOCTEST_EXCEPTION,

    SOCTEST_CASE_TYPE_ID_MAX,
};

typedef enum {
    SMOKE_TEST      = BIT(SOCTEST_SMOKE),       // 冒烟测试，验证功能是否正常，可自动也可手动
    AUTO_TEST       = BIT(SOCTEST_AUTO),        // 自动测试，需支持测试结果自动判定
    MANUAL_TEST     = BIT(SOCTEST_MANUAL),      // 手动测试，需人工判定测试结果或人工操作测试
    PRESSURE_TEST   = BIT(SOCTEST_PRESSURE),    // 压力测试，需支持测试结果自动判定及可重复测试
    EXCEPTION_TEST  = BIT(SOCTEST_EXCEPTION),   // 异常测试，会产生异常中断或复位等情况的测试
    MAX_MODE_TEST    = BIT(SOCTEST_CASE_TYPE_ID_MAX),
} soctest_case_type_e;

typedef enum {
    // CPU
    SOCTEST_MOD_NVIC,
    SOCTEST_MOD_CACHE,
    SOCTEST_MOD_SYSTICK,
    SOCTEST_MOD_EXCPTION,

    // MEM
    SOCTEST_MOD_FLASH,
    SOCTEST_MOD_MEM,

    // PERI
    SOCTEST_MOD_UART,
    SOCTEST_MOD_IIC,
    SOCTEST_MOD_SPI,
    SOCTEST_MOD_GPIO,
    SOCTEST_MOD_WDT,
    SOCTEST_MOD_DMA,
    SOCTEST_MOD_RTC,
    SOCTEST_MOD_TIMER,
    SOCTEST_MOD_PWM,

    // algo
    SOCTEST_MOD_RNG,
    SOCTEST_MOD_CRC,
    SOCTEST_MOD_AES,

    SOCTEST_MOD_IWDT,
    SOCTEST_MOD_LPUART,
    SOCTEST_MOD_LPTIMER,
    SOCTEST_MOD_LPIO,

    // low power
    SOCTEST_MOD_LPWR,

    // reset
    SOCTEST_MOD_RESET,

    SOCTEST_MOD_TYPE_MAX,
} soctest_mod_type_e;

typedef struct {
    char case_name[MAX_SOCTEST_CASE_NAME_LEN];  // 测试用例名称
    soctest_case        case_func;              // 测试函数入口
    uint32_t            case_type;              // 测试类型，参考 soctest_case_type_e 定义
} soctest_case_t;

typedef struct {
    char mod_name[MAX_SOCTEST_MOD_NAME_LEN];    // 模块名称
    soctest_mod_init   mod_init;                // 模块初始化函数
    soctest_mod_deinit mod_deinit;              // 模块去初始化函数
    const soctest_case_t    *case_list;         // 测试用例列表
    uint32_t           case_num;                // 测试用例数量
} soctest_mod_t;

typedef struct {
    volatile uint32_t test_num;
    volatile uint32_t test_mode;
    volatile uint32_t test_module;
    volatile uint32_t test_case;
    volatile uint32_t test_result;
} soctest_record_t;

void soctest_main(void);
void soctest_run_case(void);

void soctest_set_test_num(uint32_t test_num);
void soctest_set_test_mode(uint32_t test_mode);
void soctest_set_test_case(uint32_t case_mask);
void soctest_set_test_module(uint32_t test_module);
uint32_t soctest_get_test_result(void);
void soctest_clear_test_result(void);

#endif // __QMX_SOC_TEST_H__
