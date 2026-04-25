#ifndef __QMX_FLASH_TEST_H__
#define __QMX_FLASH_TEST_H__

#include "compiler.h"

int qmx_flash_read_id_test(void);
__RAM_FUNC int qmx_flash_erase_write_read_test(void);
int qmx_flash_qspi_div_test(void);
int qmx_flash_read_write_mode_test(void);
int qmx_flash_qspi_mode_test(void);
int qmx_flash_read_data_capture_test(void);
int qmx_flash_qspi_delay_test(void);
__RAM_FUNC int qmx_flash_security_register_test(void);
__RAM_FUNC int qmx_flash_protect_test(void);

#endif // __QMX_FLASH_TEST_H__
