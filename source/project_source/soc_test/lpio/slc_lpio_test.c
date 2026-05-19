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
#include "slc_hal_intc.h"
#include "slc_hal_sysctrl.h"
#include "slc_hal_delay.h"
#include "slc_hal_pmu.h"
#include "slc_hal_lpuart.h"
#include "slc_hal_gpio.h"
#include "slc_lpio_test.h"
#include "slc_test_master.h"

// 不可打印浮点数
void lpuart_test_print(const char *fmt, ...)
{
    char buf[256];
    int size;

    va_list args;

    va_start(args, fmt);
    size = vsnprintf((char *)buf, sizeof(buf), fmt, args);
    va_end(args);

    slc_hal_lpuart_send_data(HAL_LPUART0, (uint8_t *)buf, size);
}

void slc_lpio_test_init(void)
{
    hal_lpuart_init_t lpuart_init;

    slc_hal_sysctrl_peripheral_clk_enable(HAL_CLK_LPUART, true);
    slc_hal_sysctrl_peripheral_mod_reset(HAL_CLK_LPUART);
    SLC_HAL_DISABLE_PERIPHERAL_IRQ(LPUART_IRQ);
    slc_hal_unregister_irq_handler(LPUART_IRQ);

    slc_hal_gpio_set_iomux(HAL_GPIO_PIN18, HAL_IOMUX_MODE3);
    slc_hal_gpio_set_iomux(HAL_GPIO_PIN19, HAL_IOMUX_MODE3);
    slc_hal_lpio_set_iomux(HAL_GPIO_PIN18, HAL_IOMUX_MODE3);
    slc_hal_lpio_set_iomux(HAL_GPIO_PIN19, HAL_IOMUX_MODE3);
    slc_hal_gpio_set_mode(HAL_GPIO_PIN18, HAL_GPIO_PULL_UP);
    slc_hal_gpio_set_mode(HAL_GPIO_PIN19, HAL_GPIO_PULL_UP);

    lpuart_init.baudrate = 9600;
    lpuart_init.parity = HAL_LPUART_PARITY_NONE;
    lpuart_init.stopbit = HAL_LPUART_STOPBIT_1;
    lpuart_init.datalen = HAL_LPUART_DATA_8BIT;
    lpuart_init.rx_fifo_thld = 1;

    slc_hal_lpuart_init(HAL_LPUART0, &lpuart_init);

    slc_hal_pmu_lpio_enable(true);
}

void slc_lpio_test_deinit(void)
{
    slc_hal_lpuart_deinit(HAL_LPUART0);
    slc_hal_pmu_lpio_enable(false); // 关闭lpio
}

// 逻辑分析仪抓取波形看是否有输出
int slc_lpio_output_test(void)
{
    hal_gpio_pin_e pin = HAL_GPIO_PIN0;

    slc_hal_lpio_set_iomux(pin, HAL_IOMUX_GPIO);
    slc_hal_lpio_set_mode(pin, HAL_GPIO_PULL_UP);
    slc_hal_lpio_set_direction(pin, HAL_GPIO_OUTPUT);

    slc_hal_lpio_set_value(pin, HAL_GPIO_LEVEL_HIGH);
    slc_hal_nop_delay_ms(100);
    slc_hal_lpio_set_value(pin, HAL_GPIO_LEVEL_LOW);
    slc_hal_nop_delay_ms(100);
    slc_hal_lpio_set_value(pin, HAL_GPIO_LEVEL_HIGH);
    slc_hal_nop_delay_ms(100);
    slc_hal_lpio_set_value(pin, HAL_GPIO_LEVEL_LOW);

    return 0;
}

#define LPIO_TEST_IRQ_PIN   HAL_GPIO_PIN0
void slc_lpio_test_irq(void)
{
    uint32_t sta = slc_hal_lpio_get_interrupt_sta();
    hal_gpio_level_e level = slc_hal_lpio_get_value(LPIO_TEST_IRQ_PIN);

    SLC_HAL_DISABLE_PERIPHERAL_IRQ(LPIO_IRQ);
    slc_hal_lpio_enable_interrupt(LPIO_TEST_IRQ_PIN, false);
    lpuart_test_print("LPIO IRQ, sta: 0x%X, level %d.\n", sta, level);

    slc_hal_lpio_clear_interrupt();
}

int slc_lpio_high_interrupt_test(void)
{
    hal_gpio_pin_e in_pin = LPIO_TEST_IRQ_PIN;
    hal_gpio_pin_e out_pin = HAL_GPIO_PIN1;

    slc_hal_register_irq_handler(LPIO_IRQ, slc_lpio_test_irq);

    slc_hal_lpio_set_iomux(out_pin, HAL_IOMUX_GPIO);
    slc_hal_lpio_set_mode(out_pin, HAL_GPIO_PULL_UP);
    slc_hal_lpio_set_direction(out_pin, HAL_GPIO_OUTPUT);
    slc_hal_lpio_set_value(out_pin, HAL_GPIO_LEVEL_LOW);

    lpuart_test_print("LPIO start high level interrupt test.\n");

    slc_hal_lpio_set_iomux(in_pin, HAL_IOMUX_GPIO);
    slc_hal_lpio_set_mode(in_pin, HAL_GPIO_PULL_UP);
    slc_hal_lpio_set_direction(in_pin, HAL_GPIO_INPUT);

    slc_hal_lpio_set_interrupt(in_pin, HAL_GPIO_INTR_HIGH_LEVEL);
    slc_hal_lpio_enable_interrupt(in_pin, true);
    slc_hal_lpio_clear_interrupt();
    SLC_HAL_ENABLE_PERIPHERAL_IRQ(LPIO_IRQ, 0x3);

    slc_hal_lpio_set_value(out_pin, HAL_GPIO_LEVEL_HIGH);

    slc_hal_nop_delay_ms(1000); // 等待中断触发
    lpuart_test_print("LPIO high interrupt test end.\n");

    return 0;
}

int slc_lpio_low_interrupt_test(void)
{
    hal_gpio_pin_e in_pin = LPIO_TEST_IRQ_PIN;
    hal_gpio_pin_e out_pin = HAL_GPIO_PIN1;

    slc_hal_register_irq_handler(LPIO_IRQ, slc_lpio_test_irq);

    slc_hal_lpio_set_iomux(out_pin, HAL_IOMUX_GPIO);
    slc_hal_lpio_set_mode(out_pin, HAL_GPIO_PULL_UP);
    slc_hal_lpio_set_direction(out_pin, HAL_GPIO_OUTPUT);
    slc_hal_lpio_set_value(out_pin, HAL_GPIO_LEVEL_HIGH);

    lpuart_test_print("LPIO start low level interrupt test.\n");

    slc_hal_lpio_set_iomux(in_pin, HAL_IOMUX_GPIO);
    slc_hal_lpio_set_mode(in_pin, HAL_GPIO_PULL_UP);
    slc_hal_lpio_set_direction(in_pin, HAL_GPIO_INPUT);

    slc_hal_lpio_set_interrupt(in_pin, HAL_GPIO_INTR_LOW_LEVEL);
    slc_hal_lpio_enable_interrupt(in_pin, true);
    slc_hal_lpio_clear_interrupt();
    SLC_HAL_ENABLE_PERIPHERAL_IRQ(LPIO_IRQ, 0x3);

    slc_hal_nop_delay_ms(10);
    slc_hal_lpio_set_value(out_pin, HAL_GPIO_LEVEL_LOW);

    slc_hal_nop_delay_ms(1000); // 等待中断触发
    lpuart_test_print("LPIO low interrupt test end.\n");

    return 0;
}
