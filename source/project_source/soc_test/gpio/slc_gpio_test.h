#ifndef __SLC_GPIO_TEST_H__
#define __SLC_GPIO_TEST_H__

void slc_gpio_test_init(void);
void slc_gpio_test_deinit(void);

int slc_gpio_output_test(void);
int slc_gpio_input_test(void);

int slc_gpio_iomux_test(void);

int slc_gpio_level_interrupt_test(void);
int slc_gpio_rising_interrupt_test(void);
int slc_gpio_falling_interrupt_test(void);

int slc_gpio_od_test(void);

int slc_gpio_filter_test(void);

#endif // __SLC_GPIO_TEST_H__
