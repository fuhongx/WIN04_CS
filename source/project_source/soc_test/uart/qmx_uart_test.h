#ifndef __QMX_UART_TEST_H__
#define __QMX_UART_TEST_H__

void qmx_uart_test_init(void);
void qmx_uart_test_deinit(void);

int qmx_uart_baudrate_test(void);
int qmx_uart_stopbit_test(void);
int qmx_uart_parity_test(void);
int qmx_uart_databits_test(void);
int qmx_uart_complex_test(void);

int qmx_uart_rx_fifo_test(void);

int qmx_uart_rts_cts_test(void);

int qmx_uart_rs485_test(void);

#endif  // __QMX_UART_TEST_H__
