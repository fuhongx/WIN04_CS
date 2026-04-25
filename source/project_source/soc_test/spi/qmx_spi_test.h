#ifndef __QMX_SPI_TEST_H__
#define __QMX_SPI_TEST_H__

void qmx_spi_test_init(void);
void qmx_spi_test_deinit(void);

int qmx_spi_master_div_test(void);
int qmx_spi_master_polarity_phase_test(void);
int qmx_spi_master_data_mode_test(void);
int qmx_spi_master_data_len_test(void);

int qmx_spi_salve_polarity_phase_test(void);
int qmx_spi_slave_data_mode_test(void);
int qmx_spi_slave_data_len_test(void);

#endif // __QMX_SPI_TEST_H__
