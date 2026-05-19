#ifndef __SLC_SPI_TEST_H__
#define __SLC_SPI_TEST_H__

void slc_spi_test_init(void);
void slc_spi_test_deinit(void);

int slc_spi_master_div_test(void);
int slc_spi_master_polarity_phase_test(void);
int slc_spi_master_data_mode_test(void);
int slc_spi_master_data_len_test(void);

int slc_spi_salve_polarity_phase_test(void);
int slc_spi_slave_data_mode_test(void);
int slc_spi_slave_data_len_test(void);

#endif // __SLC_SPI_TEST_H__
