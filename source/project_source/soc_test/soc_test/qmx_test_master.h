#ifndef __QMX_TEST_MASTER_H__
#define __QMX_TEST_MASTER_H__

#include "qmx_test_master_slave_common.h"

int qmx_test_master_send_cmd(qmx_test_cmd_e cmd, uint8_t *data, uint8_t len);
int qmx_test_master_get_result(uint8_t *data, uint8_t *len);
void qmx_test_master_clear_ack(void);

#endif  // __QMX_TEST_MASTER_H__
