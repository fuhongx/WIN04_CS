#ifndef __SLC_LPIO_TEST_H__
#define __SLC_LPIO_TEST_H__

void slc_lpio_test_init(void);
void slc_lpio_test_deinit(void);

int slc_lpio_output_test(void);

int slc_lpio_high_interrupt_test(void);
int slc_lpio_low_interrupt_test(void);

#endif // __SLC_LPIO_TEST_H__
