#ifndef __QMX_LPIO_TEST_H__
#define __QMX_LPIO_TEST_H__

void qmx_lpio_test_init(void);
void qmx_lpio_test_deinit(void);

int qmx_lpio_output_test(void);

int qmx_lpio_high_interrupt_test(void);
int qmx_lpio_low_interrupt_test(void);

#endif // __QMX_LPIO_TEST_H__
