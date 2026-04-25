#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "app_cfg.h"
#include "utility.h"
#include "debug.h"
#include "error_def.h"
#include "qmx_hal_intc.h"
#include "qmx_hal_sysctrl.h"
#include "qmx_hal_delay.h"
#include "qmx_hal_gpio.h"
#include "qmx_gpio_test.h"
#include "qmx_test_master.h"

void qmx_gpio_test_init(void)
{
    qmx_test_common_init();
}

void qmx_gpio_test_deinit(void)
{
    // 恢复SWD引脚复用,否则无法下载程序
    qmx_hal_gpio_set_iomux(HAL_GPIO_PIN20, HAL_IOMUX_MODE0);
    qmx_hal_gpio_set_iomux(HAL_GPIO_PIN21, HAL_IOMUX_MODE0);

    qmx_test_common_deinit();
}

int qmx_test_check_exclude_gpio(hal_gpio_pin_e pin)
{
    int ret = 0;
    switch (pin)
    {
        case QMX_TEST_UART_TX_PIN:
        case QMX_TEST_UART_RX_PIN:
        case DEBUG_UART_TX_PIN:
        case DEBUG_UART_RX_PIN:
        // boot的strap pin
        case HAL_GPIO_PIN8:
        // vTCXO的供电pin
        case HAL_GPIO_PIN16:
        // lpuart的pin
        case HAL_GPIO_PIN18:
        case HAL_GPIO_PIN19:
        // SWD的pin
        case HAL_GPIO_PIN20:
        case HAL_GPIO_PIN21:
            ret = -1;
            break;

        default:
            ret = 0;
            break;
    }

    return ret;
}

int qmx_gpio_output_test(void)
{
    uint8_t tx_data[QMX_TEST_FRAME_DATA_LEN] = {0};
    uint8_t rx_data[QMX_TEST_FRAME_DATA_LEN] = {0};
    uint8_t rx_len = 0;
    hal_gpio_pin_e pin;
    int ret = 0;

    for (pin = HAL_GPIO_PIN0; pin < HAL_GPIO_PIN_MAX; pin++) {
        if (qmx_test_check_exclude_gpio(pin) != 0) {
            continue;
        }

        qmx_hal_gpio_set_iomux(pin, HAL_IOMUX_GPIO);
        qmx_hal_gpio_set_direction(pin, HAL_GPIO_OUTPUT);
        qmx_hal_gpio_set_mode(pin, HAL_GPIO_PULL_UP);

        tx_data[0] = (uint8_t)pin;

        qmx_hal_gpio_set_value(pin, HAL_GPIO_LEVEL_LOW);
        ret += qmx_test_master_send_cmd(QMX_TEST_CMD_GPIO_GET, tx_data, 1);
        ret += qmx_test_master_get_result(rx_data, &rx_len);
        if ((ret != 0) || (rx_data[0] != 0)) {
            PRINTF("GPIO pin%u get level(low) cmd send failed.\n", pin);
            goto err;
        }
        if (rx_data[1] != HAL_GPIO_LEVEL_LOW) {
            PRINTF("GPIO pin%u set level low failed.\n", pin);
            goto err;
        }

        qmx_hal_gpio_set_value(pin, HAL_GPIO_LEVEL_HIGH);
        ret += qmx_test_master_send_cmd(QMX_TEST_CMD_GPIO_GET, tx_data, 1);
        ret += qmx_test_master_get_result(rx_data, &rx_len);
        if ((ret != 0) || (rx_data[0] != 0)) {
            PRINTF("GPIO pin%u get level(high) cmd send failed.\n", pin);
            goto err;
        }
        if (rx_data[1] != HAL_GPIO_LEVEL_HIGH) {
            PRINTF("GPIO pin%u set level high failed.\n", pin);
            goto err;
        }

        // 切换为input模式，防止短路等情况
        qmx_hal_gpio_set_direction(pin, HAL_GPIO_INPUT);
    }

    return 0;

err:
    qmx_hal_gpio_set_direction(pin, HAL_GPIO_INPUT);
    return -1;
}

int qmx_gpio_input_test(void)
{
    uint8_t tx_data[QMX_TEST_FRAME_DATA_LEN] = {0};
    uint8_t rx_data[QMX_TEST_FRAME_DATA_LEN] = {0};
    uint8_t rx_len = 0;
    hal_gpio_pin_e pin;
    uint8_t level;
    int ret = 0;

    for (pin = HAL_GPIO_PIN0; pin < HAL_GPIO_PIN_MAX; pin++) {
        if (qmx_test_check_exclude_gpio(pin) != 0) {
            continue;
        }

        qmx_hal_gpio_set_iomux(pin, HAL_IOMUX_GPIO);
        qmx_hal_gpio_set_direction(pin, HAL_GPIO_INPUT);
        qmx_hal_gpio_set_mode(pin, HAL_GPIO_PULL_UP);

        tx_data[0] = (uint8_t)pin;
        tx_data[1] = HAL_GPIO_LEVEL_LOW;
        ret += qmx_test_master_send_cmd(QMX_TEST_CMD_GPIO_SET, tx_data, 2);
        ret += qmx_test_master_get_result(rx_data, &rx_len);
        if ((ret != 0) || (rx_data[0] != 0)) {
            PRINTF("GPIO pin%u get level(low) cmd send failed.\n", pin);
            return -1;
        }
        level = (uint8_t)qmx_hal_gpio_get_value(pin);
        if (level != HAL_GPIO_LEVEL_LOW) {
            PRINTF("GPIO pin%u get level low failed.\n", pin);
            return -1;
        }

        tx_data[1] = HAL_GPIO_LEVEL_HIGH;
        ret += qmx_test_master_send_cmd(QMX_TEST_CMD_GPIO_SET, tx_data, 2);
        ret += qmx_test_master_get_result(rx_data, &rx_len);
        if ((ret != 0) || (rx_data[0] != 0)) {
            PRINTF("GPIO pin%u get level(high) cmd send failed.\n", pin);
            return -1;
        }
        level = (uint8_t)qmx_hal_gpio_get_value(pin);
        if (level != HAL_GPIO_LEVEL_HIGH) {
            PRINTF("GPIO pin%u get level high failed.\n", pin);
            return -1;
        }
    }

    return 0;
}

int qmx_gpio_iomux_test(void)
{
    uint8_t tx_data[QMX_TEST_FRAME_DATA_LEN] = {0};
    uint8_t rx_data[QMX_TEST_FRAME_DATA_LEN] = {0};
    uint8_t rx_len = 0;
    hal_gpio_pin_e pin;
    uint8_t level_old, level_new;
    int ret = 0;

    for (pin = HAL_GPIO_PIN0; pin < HAL_GPIO_PIN_MAX; pin++) {
        if (qmx_test_check_exclude_gpio(pin) != 0) {
            continue;
        }

        qmx_hal_gpio_set_iomux(pin, HAL_IOMUX_MODE7);   // 不能和通信管脚冲突
        qmx_hal_gpio_set_direction(pin, HAL_GPIO_OUTPUT);
        qmx_hal_gpio_set_mode(pin, HAL_GPIO_PULL_UP);   // 设置上拉，避免悬空导致电平不稳定

        tx_data[0] = (uint8_t)pin;
        qmx_hal_gpio_set_value(pin, HAL_GPIO_LEVEL_LOW);
        qmx_hal_nop_delay_ms(20);
        ret += qmx_test_master_send_cmd(QMX_TEST_CMD_GPIO_GET, tx_data, 1);
        ret += qmx_test_master_get_result(rx_data, &rx_len);
        if ((ret != 0) || (rx_data[0] != 0)) {
            PRINTF("GPIO pin%u get level(low) cmd send failed.\n", pin);
            goto err;
        }
        level_old = rx_data[1];
        ret = 0;

        qmx_hal_gpio_set_value(pin, HAL_GPIO_LEVEL_HIGH);
        qmx_hal_nop_delay_ms(20);
        ret += qmx_test_master_send_cmd(QMX_TEST_CMD_GPIO_GET, tx_data, 1);
        ret += qmx_test_master_get_result(rx_data, &rx_len);
        if ((ret != 0) || (rx_data[0] != 0)) {
            PRINTF("GPIO pin%u get level(high) cmd send failed.\n", pin);
            goto err;
        }
        level_new = rx_data[1];
        ret = 0;

        if (level_old != level_new) {
            PRINTF("GPIO pin%u iomux test failed.\n", pin);
            goto err;
        }

        qmx_hal_gpio_set_direction(pin, HAL_GPIO_INPUT);
    }

    return 0;

err:
    qmx_hal_gpio_set_direction(pin, HAL_GPIO_INPUT);
    return -1;
}

volatile uint8_t g_gpiotest_irq_pin = 0;
volatile uint8_t g_gpiotest_irq_type = 0;
void qmx_test_gpio_irq_handler(void)
{
    uint32_t sta = qmx_hal_gpio_get_all_interrupts();
    g_gpiotest_irq_type = qmx_hal_gpio_get_pin_interrupt_sta((hal_gpio_pin_e)g_gpiotest_irq_pin);
    if (g_gpiotest_irq_type == HAL_GPIO_INTR_STA_LEVEL) {
        qmx_hal_gpio_enable_interrupt((hal_gpio_pin_e)g_gpiotest_irq_pin, false);
    }

    PRINTF("GPIO sta: 0x%X.\n", sta);

    qmx_hal_gpio_clear_all_interrupts();
}

int qmx_gpio_level_interrupt_test(void)
{
    uint8_t tx_data[QMX_TEST_FRAME_DATA_LEN] = {0};
    uint8_t rx_data[QMX_TEST_FRAME_DATA_LEN] = {0};
    uint8_t rx_len = 0;
    hal_gpio_pin_e pin;
    int ret = 0;

    qmx_hal_register_irq_handler(GPIO_IRQ, qmx_test_gpio_irq_handler);

    for (pin = HAL_GPIO_PIN0; pin < HAL_GPIO_PIN_MAX; pin++) {
        if (qmx_test_check_exclude_gpio(pin) != 0) {
            continue;
        }

        qmx_hal_gpio_set_iomux(pin, HAL_IOMUX_GPIO);
        qmx_hal_gpio_set_direction(pin, HAL_GPIO_INPUT);
        qmx_hal_gpio_set_mode(pin, HAL_GPIO_PULL_UP);   // 设置上拉，避免悬空导致电平不稳定

        /* 拉低pin脚 */
        qmx_hal_gpio_enable_interrupt(pin, false);
        g_gpiotest_irq_pin = (uint8_t)pin;
        tx_data[0] = (uint8_t)pin;
        tx_data[1] = HAL_GPIO_LEVEL_LOW;
        ret += qmx_test_master_send_cmd(QMX_TEST_CMD_GPIO_SET, tx_data, 2);
        ret += qmx_test_master_get_result(rx_data, &rx_len);
        if ((ret != 0) || (rx_data[0] != 0)) {
            PRINTF("GPIO pin%u set level(low) cmd send failed.\n", pin);
            ret = -1;
            goto end;
        }

        /* 1. 测试低电平中断触发 */
        g_gpiotest_irq_type = HAL_GPIO_INTR_STA_NONE;
        qmx_hal_gpio_set_interrupt(pin, HAL_GPIO_INTR_LOW_LEVEL);
        qmx_hal_gpio_enable_interrupt(pin, true);
        QMX_HAL_ENABLE_PERIPHERAL_IRQ(GPIO_IRQ, 0x3);
        qmx_hal_nop_delay_ms(10);
        if (g_gpiotest_irq_type != HAL_GPIO_INTR_STA_LEVEL) {
            PRINTF("GPIO pin%u low level interrupt(pull down) failed, type %u.\n", pin, g_gpiotest_irq_type);
            ret = -1;
            goto end;
        }

        /* 2. 测试高电平中断不触发 */
        g_gpiotest_irq_type = HAL_GPIO_INTR_STA_NONE;
        qmx_hal_gpio_set_interrupt(pin, HAL_GPIO_INTR_HIGH_LEVEL);
        qmx_hal_gpio_enable_interrupt(pin, true);
        QMX_HAL_ENABLE_PERIPHERAL_IRQ(GPIO_IRQ, 0x3);
        qmx_hal_nop_delay_ms(10);
        if (g_gpiotest_irq_type != HAL_GPIO_INTR_STA_NONE) {
            PRINTF("GPIO pin%u high level interrupt(pull down) failed, type %u.\n", pin, g_gpiotest_irq_type);
            ret = -1;
            goto end;
        }

        /* 拉高pin脚 */
        qmx_hal_gpio_enable_interrupt(pin, false);
        g_gpiotest_irq_pin = (uint8_t)pin;
        tx_data[0] = (uint8_t)pin;
        tx_data[1] = HAL_GPIO_LEVEL_HIGH;
        ret += qmx_test_master_send_cmd(QMX_TEST_CMD_GPIO_SET, tx_data, 2);
        ret += qmx_test_master_get_result(rx_data, &rx_len);
        if ((ret != 0) || (rx_data[0] != 0)) {
            PRINTF("GPIO pin%u set level(high) cmd send failed.\n", pin);
            ret = -1;
            goto end;
        }

        /* 3. 测试高电平中断触发 */
        g_gpiotest_irq_type = HAL_GPIO_INTR_STA_NONE;
        qmx_hal_gpio_set_interrupt(pin, HAL_GPIO_INTR_HIGH_LEVEL);
        qmx_hal_gpio_enable_interrupt(pin, true);
        QMX_HAL_ENABLE_PERIPHERAL_IRQ(GPIO_IRQ, 0x3);
        qmx_hal_nop_delay_ms(10);
        if (g_gpiotest_irq_type != HAL_GPIO_INTR_STA_LEVEL) {
            PRINTF("GPIO pin%u high level interrupt(pull up) failed, type %u.\n", pin, g_gpiotest_irq_type);
            ret = -1;
            goto end;
        }

        /* 4. 测试低电平中断不触发 */
        g_gpiotest_irq_type = HAL_GPIO_INTR_STA_NONE;
        qmx_hal_gpio_set_interrupt(pin, HAL_GPIO_INTR_LOW_LEVEL);
        qmx_hal_gpio_enable_interrupt(pin, true);
        QMX_HAL_ENABLE_PERIPHERAL_IRQ(GPIO_IRQ, 0x3);
        qmx_hal_nop_delay_ms(10);
        if (g_gpiotest_irq_type != HAL_GPIO_INTR_STA_NONE) {
            PRINTF("GPIO pin%u low level interrupt(pull up) failed, type %u.\n", pin, g_gpiotest_irq_type);
            ret = -1;
            goto end;
        }

        qmx_hal_gpio_enable_interrupt(pin, false);
        qmx_hal_gpio_set_interrupt(pin, HAL_GPIO_INTR_NONE);
    }

end:
    qmx_hal_gpio_set_interrupt(pin, HAL_GPIO_INTR_NONE);
    QMX_HAL_DISABLE_PERIPHERAL_IRQ(GPIO_IRQ);
    qmx_hal_unregister_irq_handler(GPIO_IRQ);

    return ret;
}

int qmx_gpio_rising_interrupt_test(void)
{
    uint8_t tx_data[QMX_TEST_FRAME_DATA_LEN] = {0};
    uint8_t rx_data[QMX_TEST_FRAME_DATA_LEN] = {0};
    uint8_t rx_len = 0;
    hal_gpio_pin_e pin;
    int ret = 0;

    qmx_hal_register_irq_handler(GPIO_IRQ, qmx_test_gpio_irq_handler);

    for (pin = HAL_GPIO_PIN0; pin < HAL_GPIO_PIN_MAX; pin++) {
        if (qmx_test_check_exclude_gpio(pin) != 0) {
            continue;
        }

        /* 初始化GPIO为input */
        qmx_hal_gpio_set_iomux(pin, HAL_IOMUX_GPIO);
        qmx_hal_gpio_set_direction(pin, HAL_GPIO_INPUT);
        qmx_hal_gpio_set_mode(pin, HAL_GPIO_PULL_UP);   // 设置上拉，避免悬空导致电平不稳定

        /* 让对端PIN脚输出低电平，为后续上升沿做准备 */
        qmx_hal_gpio_enable_interrupt(pin, false);
        g_gpiotest_irq_pin = (uint8_t)pin;
        tx_data[0] = (uint8_t)pin;
        tx_data[1] = HAL_GPIO_LEVEL_LOW;
        ret += qmx_test_master_send_cmd(QMX_TEST_CMD_GPIO_SET, tx_data, 2);
        ret += qmx_test_master_get_result(rx_data, &rx_len);
        if ((ret != 0) || (rx_data[0] != 0)) {
            PRINTF("GPIO pin%u set level(low) cmd send failed.\n", pin);
            ret = -1;
            goto end;
        }

        /* 打开测试pin的上升沿中断，预期此时不会有中断发生 */
        g_gpiotest_irq_type = HAL_GPIO_INTR_STA_NONE;
        qmx_hal_gpio_set_interrupt(pin, HAL_GPIO_INTR_RISING_EDGE);
        qmx_hal_gpio_enable_interrupt(pin, true);
        QMX_HAL_ENABLE_PERIPHERAL_IRQ(GPIO_IRQ, 0x3);
        qmx_hal_nop_delay_ms(10);
        if (g_gpiotest_irq_type != HAL_GPIO_INTR_STA_NONE) {
            PRINTF("GPIO pin%u rising interrupt(L) failed, type %u.\n", pin, g_gpiotest_irq_type);
            ret = -1;
            goto end;
        }

        /* 发送cmd让对端输出高电平，构造上升沿 */
        tx_data[1] = HAL_GPIO_LEVEL_HIGH;
        ret += qmx_test_master_send_cmd(QMX_TEST_CMD_GPIO_SET, tx_data, 2);
        ret += qmx_test_master_get_result(rx_data, &rx_len);
        if ((ret != 0) || (rx_data[0] != 0)) {
            PRINTF("GPIO pin%u set level(high) cmd send failed.\n", pin);
            ret = -1;
            goto end;
        }

        /* 此时预期上升沿中断触发 */
        qmx_hal_nop_delay_ms(10);
        if (g_gpiotest_irq_type != HAL_GPIO_INTR_STA_RISING) {
            PRINTF("GPIO pin%u rising interrupt(L->H) failed, type %u.\n", pin, g_gpiotest_irq_type);
            ret = -1;
            goto end;
        }

        /* 发送cmd让对端再次输出低电平，构造下降沿 */
        g_gpiotest_irq_type = HAL_GPIO_INTR_STA_NONE;
        tx_data[1] = HAL_GPIO_LEVEL_LOW;
        ret += qmx_test_master_send_cmd(QMX_TEST_CMD_GPIO_SET, tx_data, 2);
        ret += qmx_test_master_get_result(rx_data, &rx_len);
        if ((ret != 0) || (rx_data[0] != 0)) {
            PRINTF("GPIO pin%u set level(low) cmd send failed.\n", pin);
            ret = -1;
            goto end;
        }

        /* 此时预期上升沿中断不触发 */
        qmx_hal_nop_delay_ms(10);
        if (g_gpiotest_irq_type != HAL_GPIO_INTR_STA_NONE) {
            PRINTF("GPIO pin%u rising interrupt(H->L) failed, type %u.\n", pin, g_gpiotest_irq_type);
            ret = -1;
            goto end;
        }

        qmx_hal_gpio_enable_interrupt(pin, false);
        qmx_hal_gpio_set_interrupt(pin, HAL_GPIO_INTR_NONE);
    }

end:
    qmx_hal_gpio_set_interrupt(pin, HAL_GPIO_INTR_NONE);
    QMX_HAL_DISABLE_PERIPHERAL_IRQ(GPIO_IRQ);
    qmx_hal_unregister_irq_handler(GPIO_IRQ);

    return 0;
}

int qmx_gpio_falling_interrupt_test(void)
{
    uint8_t tx_data[QMX_TEST_FRAME_DATA_LEN] = {0};
    uint8_t rx_data[QMX_TEST_FRAME_DATA_LEN] = {0};
    uint8_t rx_len = 0;
    hal_gpio_pin_e pin;
    int ret = 0;

    qmx_hal_register_irq_handler(GPIO_IRQ, qmx_test_gpio_irq_handler);

    for (pin = HAL_GPIO_PIN0; pin < HAL_GPIO_PIN_MAX; pin++) {
        if (qmx_test_check_exclude_gpio(pin) != 0) {
            continue;
        }

        /* 初始化GPIO为input */
        qmx_hal_gpio_set_iomux(pin, HAL_IOMUX_GPIO);
        qmx_hal_gpio_set_direction(pin, HAL_GPIO_INPUT);
        qmx_hal_gpio_set_mode(pin, HAL_GPIO_PULL_UP);   // 设置上拉，避免悬空导致电平不稳定

        /* 让对端PIN脚输出高电平，为后续下降沿做准备 */
        qmx_hal_gpio_enable_interrupt(pin, false);
        g_gpiotest_irq_pin = (uint8_t)pin;
        tx_data[0] = (uint8_t)pin;
        tx_data[1] = HAL_GPIO_LEVEL_HIGH;
        ret += qmx_test_master_send_cmd(QMX_TEST_CMD_GPIO_SET, tx_data, 2);
        ret += qmx_test_master_get_result(rx_data, &rx_len);
        if ((ret != 0) || (rx_data[0] != 0)) {
            PRINTF("GPIO pin%u set level(high) cmd send failed.\n", pin);
            ret = -1;
            goto end;
        }

        /* 打开测试pin的下降沿中断，预期此时不会有中断发生 */
        g_gpiotest_irq_type = HAL_GPIO_INTR_STA_NONE;
        qmx_hal_gpio_set_interrupt(pin, HAL_GPIO_INTR_FALLING_EDGE);
        qmx_hal_gpio_enable_interrupt(pin, true);
        QMX_HAL_ENABLE_PERIPHERAL_IRQ(GPIO_IRQ, 0x3);
        qmx_hal_nop_delay_ms(10);
        if (g_gpiotest_irq_type != HAL_GPIO_INTR_STA_NONE) {
            PRINTF("GPIO pin%u falling interrupt(H) failed, type %u.\n", pin, g_gpiotest_irq_type);
            ret = -1;
            goto end;
        }

        /* 发送cmd让对端输出低电平，构造下降沿 */
        tx_data[1] = HAL_GPIO_LEVEL_LOW;
        ret += qmx_test_master_send_cmd(QMX_TEST_CMD_GPIO_SET, tx_data, 2);
        ret += qmx_test_master_get_result(rx_data, &rx_len);
        if ((ret != 0) || (rx_data[0] != 0)) {
            PRINTF("GPIO pin%u set level(low) cmd send failed.\n", pin);
            ret = -1;
            goto end;
        }

        /* 此时预期下降沿中断触发 */
        qmx_hal_nop_delay_ms(10);
        if (g_gpiotest_irq_type != HAL_GPIO_INTR_STA_FALLING) {
            PRINTF("GPIO pin%u falling interrupt(H->L) failed, type %u.\n", pin, g_gpiotest_irq_type);
            ret = -1;
            goto end;
        }

        /* 发送cmd让对端再次输出高电平，构造上升沿 */
        g_gpiotest_irq_type = HAL_GPIO_INTR_STA_NONE;
        tx_data[1] = HAL_GPIO_LEVEL_HIGH;
        ret += qmx_test_master_send_cmd(QMX_TEST_CMD_GPIO_SET, tx_data, 2);
        ret += qmx_test_master_get_result(rx_data, &rx_len);
        if ((ret != 0) || (rx_data[0] != 0)) {
            PRINTF("GPIO pin%u set level(high) cmd send failed.\n", pin);
            ret = -1;
            goto end;
        }

        /* 此时预期下降沿中断不触发 */
        qmx_hal_nop_delay_ms(10);
        if (g_gpiotest_irq_type != HAL_GPIO_INTR_STA_NONE) {
            PRINTF("GPIO pin%u falling interrupt(L->H) failed, type %u.\n", pin, g_gpiotest_irq_type);
            ret = -1;
            goto end;
        }

        qmx_hal_gpio_enable_interrupt(pin, false);
        qmx_hal_gpio_set_interrupt(pin, HAL_GPIO_INTR_NONE);
    }

end:
    qmx_hal_gpio_set_interrupt(pin, HAL_GPIO_INTR_NONE);
    QMX_HAL_DISABLE_PERIPHERAL_IRQ(GPIO_IRQ);
    qmx_hal_unregister_irq_handler(GPIO_IRQ);

    return 0;
}

int qmx_gpio_od_test(void)
{
    uint8_t tx_data[QMX_TEST_FRAME_DATA_LEN] = {0};
    uint8_t rx_data[QMX_TEST_FRAME_DATA_LEN] = {0};
    uint8_t rx_len = 0;
    hal_gpio_pin_e pin = HAL_GPIO_PIN15;
    int ret = 0;

    // 该测试需确保测试的pin脚外部无上拉电阻，这样预期就无法输出高电平
    qmx_hal_gpio_set_iomux(pin, HAL_IOMUX_GPIO);
    qmx_hal_gpio_set_mode(pin, HAL_GPIO_OPEN_DRAIN);
    qmx_hal_gpio_set_direction(pin, HAL_GPIO_OUTPUT);

    // 开漏模式下，高电平需要依赖外部上拉电阻才能拉高，否则依然是低电平
    qmx_hal_gpio_set_value(pin, HAL_GPIO_LEVEL_HIGH);

    tx_data[0] = (uint8_t)pin;
    ret += qmx_test_master_send_cmd(QMX_TEST_CMD_GPIO_GET, tx_data, 1);
    ret += qmx_test_master_get_result(rx_data, &rx_len);
    if ((ret != 0) || (rx_data[0] != 0)) {
        PRINTF("GPIO pin%u get level cmd send failed.\n", pin);
        goto err;
    }
    if (rx_data[1] != HAL_GPIO_LEVEL_LOW) {
        PRINTF("GPIO pin%u OD test failed.\n", pin);
        goto err;
    }

    qmx_hal_gpio_set_direction(pin, HAL_GPIO_INPUT);
    return 0;

err:
    qmx_hal_gpio_set_direction(pin, HAL_GPIO_INPUT);
    return -1;
}

#define GPIO_FILTER_TEST_PIN    HAL_GPIO_PIN19
void qmx_test_gpio_filter_irq(void)
{
    static int pin_level = HAL_GPIO_LEVEL_LOW;
    pin_level = (pin_level != HAL_GPIO_LEVEL_LOW) ? HAL_GPIO_LEVEL_LOW : HAL_GPIO_LEVEL_HIGH;

    qmx_hal_gpio_clear_all_interrupts();
    qmx_hal_gpio_set_value(GPIO_FILTER_TEST_PIN, pin_level);
}

int qmx_gpio_filter_test(void)
{
    hal_gpio_pin_e input_pin = HAL_GPIO_PIN18;
    hal_gpio_pin_e output_pin = GPIO_FILTER_TEST_PIN;

    qmx_hal_gpio_set_iomux(output_pin, HAL_IOMUX_GPIO);
    qmx_hal_gpio_set_mode(output_pin, HAL_GPIO_PULL_UP);
    qmx_hal_gpio_set_direction(output_pin, HAL_GPIO_OUTPUT);
    qmx_hal_gpio_set_value(output_pin, HAL_GPIO_LEVEL_LOW);

    qmx_hal_gpio_set_iomux(HAL_GPIO_PIN17, HAL_IOMUX_GPIO);
    qmx_hal_gpio_set_mode(HAL_GPIO_PIN17, HAL_GPIO_PULL_UP);
    qmx_hal_gpio_set_direction(HAL_GPIO_PIN17, HAL_GPIO_OUTPUT);
    qmx_hal_gpio_set_value(HAL_GPIO_PIN17, HAL_GPIO_LEVEL_LOW);

    qmx_hal_gpio_set_iomux(input_pin, HAL_IOMUX_GPIO);
    qmx_hal_gpio_set_direction(input_pin, HAL_GPIO_INPUT);
    qmx_hal_gpio_set_mode(input_pin, HAL_GPIO_PULL_UP);   // 设置上拉，避免悬空导致电平不稳定
    qmx_hal_gpio_set_interrupt(input_pin, HAL_GPIO_INTR_BOTH_EDGE);
    qmx_hal_gpio_set_input_filter(200);    // 设置滤波周期为100个APB周期, 高于250K的应该会被滤波掉
    qmx_hal_register_irq_handler(GPIO_IRQ, qmx_test_gpio_filter_irq);
    qmx_hal_gpio_enable_interrupt(input_pin, true);
    QMX_HAL_ENABLE_PERIPHERAL_IRQ(GPIO_IRQ, 0x3);

    // 观察输入的PWM信号是否被滤波掉了
    while (1) {
        // 下方这几句预期会被过滤掉，不触发中断
        qmx_hal_gpio_set_value(HAL_GPIO_PIN17, HAL_GPIO_LEVEL_LOW);
        qmx_hal_gpio_set_value(HAL_GPIO_PIN17, HAL_GPIO_LEVEL_HIGH);
        qmx_hal_gpio_set_value(HAL_GPIO_PIN17, HAL_GPIO_LEVEL_LOW);
        qmx_hal_gpio_set_value(HAL_GPIO_PIN17, HAL_GPIO_LEVEL_HIGH);

        // 下方这几句预期会触发中断
        qmx_hal_gpio_set_value(HAL_GPIO_PIN17, HAL_GPIO_LEVEL_LOW);
        qmx_hal_nop_delay_ms(5);
        qmx_hal_gpio_set_value(HAL_GPIO_PIN17, HAL_GPIO_LEVEL_HIGH);
        qmx_hal_nop_delay_ms(5);
    }

    return 0;
}
