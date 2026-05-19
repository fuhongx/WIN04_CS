#ifndef __SLC_TEST_MASTER_H__
#define __SLC_TEST_MASTER_H__

#include "slc_test_master_slave_common.h"

int slc_test_master_send_cmd(slc_test_cmd_e cmd, uint8_t *data, uint8_t len);
int slc_test_master_get_result(uint8_t *data, uint8_t *len);
void slc_test_master_clear_ack(void);

#endif  // __SLC_TEST_MASTER_H__
