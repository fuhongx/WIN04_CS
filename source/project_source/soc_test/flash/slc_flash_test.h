#ifndef __SLC_FLASH_TEST_H__
#define __SLC_FLASH_TEST_H__

#include "compiler.h"

int slc_flash_read_id_test(void);
__RAM_FUNC int slc_flash_erase_write_read_test(void);
int slc_flash_qspi_div_test(void);
int slc_flash_read_write_mode_test(void);
int slc_flash_qspi_mode_test(void);
int slc_flash_read_data_capture_test(void);
int slc_flash_qspi_delay_test(void);
__RAM_FUNC int slc_flash_security_register_test(void);
__RAM_FUNC int slc_flash_protect_test(void);
__RAM_FUNC int slc_flash_endurance_test(void);
__RAM_FUNC int slc_flash_encrypt_test(void);
__RAM_FUNC int slc_flash_encrypt_without_cache_test(void);

#endif // __SLC_FLASH_TEST_H__
