#ifndef __SLC_UART_TEST_H__
#define __SLC_UART_TEST_H__

void slc_uart_test_init(void);
void slc_uart_test_deinit(void);

int slc_uart_baudrate_test(void);
int slc_uart_stopbit_test(void);
int slc_uart_parity_test(void);
int slc_uart_databits_test(void);
int slc_uart_complex_test(void);

int slc_uart_rx_fifo_test(void);

int slc_uart_rts_cts_test(void);

int slc_uart_rs485_test(void);

#endif  // __SLC_UART_TEST_H__
