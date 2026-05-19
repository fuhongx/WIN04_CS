#ifndef __SLC_LPUART_TEST_H__
#define __SLC_LPUART_TEST_H__

void slc_lpuart_test_init(void);
void slc_lpuart_test_deinit(void);

int slc_lpuart_baudrate_test(void);
int slc_lpuart_stopbit_test(void);
int slc_lpuart_parity_test(void);
int slc_lpuart_complex_test(void);

#endif  // __SLC_LPUART_TEST_H__
