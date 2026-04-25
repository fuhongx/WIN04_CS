#ifndef __QMX_DMA_TEST_H__
#define __QMX_DMA_TEST_H__

int qmx_dma_ram_to_ram_test(void);
int qmx_dma_ram_to_flash_test(void);
int qmx_dma_flash_to_flash_test(void);

int qmx_dma_muti_trig_test(void);
int qmx_dma_diff_width_test(void);

int qmx_dma_timeout_test(void);

int qmx_dma_performance_test(void);

int qmx_dma_uart_trx_test(void);
int qmx_dma_lpuart_trx_test(void);
int qmx_dma_spi_slave_test(void);
int qmx_dma_spi_master_test(void);
int qmx_dma_iic_master_test(void);
int qmx_dma_iic_slave_test(void);

#endif // __QMX_DMA_TEST_H__
