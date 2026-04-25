#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <stdbool.h>

#include "app_cfg.h"
#include "utility.h"
#include "debug.h"
#include "error_def.h"
#include "qmx_hal_intc.h"
#include "qmx_hal_sysctrl.h"
#include "qmx_hal_delay.h"
#include "qmx_hal_pmu.h"
#include "qmx_hal_lpuart.h"
#include "qmx_hal_gpio.h"
#include "qmx_lpio_test.h"
#include "qmx_test_master.h"

// 不可打印浮点数
void lpuart_test_print(const char *fmt, ...)
{
    char buf[256];
    int size;

    va_list args;

    va_start(args, fmt);
    size = vsnprintf((char *)buf, sizeof(buf), fmt, args);
    va_end(args);

    qmx_hal_lpuart_send_data(HAL_LPUART0, (uint8_t *)buf, size);
}

void qmx_lpio_test_init(void)
{
    hal_lpuart_init_t lpuart_init;

    qmx_hal_sysctrl_peripheral_clk_enable(HAL_CLK_LPUART, true);
    qmx_hal_sysctrl_peripheral_mod_reset(HAL_CLK_LPUART);
    QMX_HAL_DISABLE_PERIPHERAL_IRQ(LPUART_IRQ);
    qmx_hal_unregister_irq_handler(LPUART_IRQ);

    qmx_hal_gpio_set_iomux(HAL_GPIO_PIN18, HAL_IOMUX_MODE3);
    qmx_hal_gpio_set_iomux(HAL_GPIO_PIN19, HAL_IOMUX_MODE3);
    qmx_hal_lpio_set_iomux(HAL_GPIO_PIN18, HAL_IOMUX_MODE3);
    qmx_hal_lpio_set_iomux(HAL_GPIO_PIN19, HAL_IOMUX_MODE3);
    qmx_hal_gpio_set_mode(HAL_GPIO_PIN18, HAL_GPIO_PULL_UP);
    qmx_hal_gpio_set_mode(HAL_GPIO_PIN19, HAL_GPIO_PULL_UP);

    lpuart_init.baudrate = 9600;
    lpuart_init.parity = HAL_LPUART_PARITY_NONE;
    lpuart_init.stopbit = HAL_LPUART_STOPBIT_1;
    lpuart_init.datalen = HAL_LPUART_DATA_8BIT;
    lpuart_init.rx_fifo_thld = 1;

    qmx_hal_lpuart_init(HAL_LPUART0, &lpuart_init);

    qmx_hal_pmu_lpio_enable(true);
}

void qmx_lpio_test_deinit(void)
{
    qmx_hal_lpuart_deinit(HAL_LPUART0);
    qmx_hal_pmu_lpio_enable(false); // 关闭lpio
}

// 逻辑分析仪抓取波形看是否有输出
int qmx_lpio_output_test(void)
{
    hal_gpio_pin_e pin = HAL_GPIO_PIN0;

    qmx_hal_lpio_set_iomux(pin, HAL_IOMUX_GPIO);
    qmx_hal_lpio_set_mode(pin, HAL_GPIO_PULL_UP);
    qmx_hal_lpio_set_direction(pin, HAL_GPIO_OUTPUT);

    qmx_hal_lpio_set_value(pin, HAL_GPIO_LEVEL_HIGH);
    qmx_hal_nop_delay_ms(100);
    qmx_hal_lpio_set_value(pin, HAL_GPIO_LEVEL_LOW);
    qmx_hal_nop_delay_ms(100);
    qmx_hal_lpio_set_value(pin, HAL_GPIO_LEVEL_HIGH);
    qmx_hal_nop_delay_ms(100);
    qmx_hal_lpio_set_value(pin, HAL_GPIO_LEVEL_LOW);

    return 0;
}

#define LPIO_TEST_IRQ_PIN   HAL_GPIO_PIN0
void qmx_lpio_test_irq(void)
{
    uint32_t sta = qmx_hal_lpio_get_interrupt_sta();
    hal_gpio_level_e level = qmx_hal_lpio_get_value(LPIO_TEST_IRQ_PIN);

    QMX_HAL_DISABLE_PERIPHERAL_IRQ(LPIO_IRQ);
    qmx_hal_lpio_enable_interrupt(LPIO_TEST_IRQ_PIN, false);
    lpuart_test_print("LPIO IRQ, sta: 0x%X, level %d.\n", sta, level);

    qmx_hal_lpio_clear_interrupt();
}

int qmx_lpio_high_interrupt_test(void)
{
    hal_gpio_pin_e in_pin = LPIO_TEST_IRQ_PIN;
    hal_gpio_pin_e out_pin = HAL_GPIO_PIN1;

    qmx_hal_register_irq_handler(LPIO_IRQ, qmx_lpio_test_irq);

    qmx_hal_lpio_set_iomux(out_pin, HAL_IOMUX_GPIO);
    qmx_hal_lpio_set_mode(out_pin, HAL_GPIO_PULL_UP);
    qmx_hal_lpio_set_direction(out_pin, HAL_GPIO_OUTPUT);
    qmx_hal_lpio_set_value(out_pin, HAL_GPIO_LEVEL_LOW);

    lpuart_test_print("LPIO start high level interrupt test.\n");

    qmx_hal_lpio_set_iomux(in_pin, HAL_IOMUX_GPIO);
    qmx_hal_lpio_set_mode(in_pin, HAL_GPIO_PULL_UP);
    qmx_hal_lpio_set_direction(in_pin, HAL_GPIO_INPUT);

    qmx_hal_lpio_set_interrupt(in_pin, HAL_GPIO_INTR_HIGH_LEVEL);
    qmx_hal_lpio_enable_interrupt(in_pin, true);
    qmx_hal_lpio_clear_interrupt();
    QMX_HAL_ENABLE_PERIPHERAL_IRQ(LPIO_IRQ, 0x3);

    qmx_hal_lpio_set_value(out_pin, HAL_GPIO_LEVEL_HIGH);

    qmx_hal_nop_delay_ms(1000); // 等待中断触发
    lpuart_test_print("LPIO high interrupt test end.\n");

    return 0;
}

int qmx_lpio_low_interrupt_test(void)
{
    hal_gpio_pin_e in_pin = LPIO_TEST_IRQ_PIN;
    hal_gpio_pin_e out_pin = HAL_GPIO_PIN1;

    qmx_hal_register_irq_handler(LPIO_IRQ, qmx_lpio_test_irq);

    qmx_hal_lpio_set_iomux(out_pin, HAL_IOMUX_GPIO);
    qmx_hal_lpio_set_mode(out_pin, HAL_GPIO_PULL_UP);
    qmx_hal_lpio_set_direction(out_pin, HAL_GPIO_OUTPUT);
    qmx_hal_lpio_set_value(out_pin, HAL_GPIO_LEVEL_HIGH);

    lpuart_test_print("LPIO start low level interrupt test.\n");

    qmx_hal_lpio_set_iomux(in_pin, HAL_IOMUX_GPIO);
    qmx_hal_lpio_set_mode(in_pin, HAL_GPIO_PULL_UP);
    qmx_hal_lpio_set_direction(in_pin, HAL_GPIO_INPUT);

    qmx_hal_lpio_set_interrupt(in_pin, HAL_GPIO_INTR_LOW_LEVEL);
    qmx_hal_lpio_enable_interrupt(in_pin, true);
    qmx_hal_lpio_clear_interrupt();
    QMX_HAL_ENABLE_PERIPHERAL_IRQ(LPIO_IRQ, 0x3);

    qmx_hal_nop_delay_ms(10);
    qmx_hal_lpio_set_value(out_pin, HAL_GPIO_LEVEL_LOW);

    qmx_hal_nop_delay_ms(1000); // 等待中断触发
    lpuart_test_print("LPIO low interrupt test end.\n");

    return 0;
}
