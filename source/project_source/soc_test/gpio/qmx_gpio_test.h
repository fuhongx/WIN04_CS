#ifndef __QMX_GPIO_TEST_H__
#define __QMX_GPIO_TEST_H__

void qmx_gpio_test_init(void);
void qmx_gpio_test_deinit(void);

int qmx_gpio_output_test(void);
int qmx_gpio_input_test(void);

int qmx_gpio_iomux_test(void);

int qmx_gpio_level_interrupt_test(void);
int qmx_gpio_rising_interrupt_test(void);
int qmx_gpio_falling_interrupt_test(void);

int qmx_gpio_od_test(void);

int qmx_gpio_filter_test(void);

#endif // __QMX_GPIO_TEST_H__
