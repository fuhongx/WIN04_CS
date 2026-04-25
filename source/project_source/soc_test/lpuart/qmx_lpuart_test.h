#ifndef __QMX_LPUART_TEST_H__
#define __QMX_LPUART_TEST_H__

void qmx_lpuart_test_init(void);
void qmx_lpuart_test_deinit(void);

int qmx_lpuart_baudrate_test(void);
int qmx_lpuart_stopbit_test(void);
int qmx_lpuart_parity_test(void);
int qmx_lpuart_complex_test(void);

#endif  // __QMX_LPUART_TEST_H__
