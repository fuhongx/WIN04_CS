#include "qmx_test_master.h"
#include "qmx_hal_uart.h"
#include "app_cfg.h"
#include "utility.h"
#include "debug.h"
#include "qmx_hal_sysctrl.h"
#include "qmx_hal_gpio.h"
#include "qmx_hal_delay.h"
#include "sw_crc.h"

int qmx_test_master_send_cmd(qmx_test_cmd_e cmd, uint8_t *data, uint8_t len)
{
    if (len > QMX_TEST_FRAME_DATA_LEN) {
        PRINTF("cmd data len=%u > %u.\n", len, QMX_TEST_FRAME_DATA_LEN);
        return -1;
    }

    if (cmd >= QMX_TEST_CMD_MAX) {
        PRINTF("cmd=%u invalid.\n", cmd);
        return -1;
    }

    memset((void *)&g_test_common_tx_data, 0, sizeof(qmx_test_common_frame_t));

    g_test_common_tx_data.start = QMX_TEST_FRAME_START;
    g_test_common_tx_data.len = QMX_TEST_FRAME_LEN;
    g_test_common_tx_data.cmd = (uint8_t)cmd;
    if (data != NULL && len > 0) {
        memcpy((void *)g_test_common_tx_data.data, data, len);
    }
    g_test_common_tx_data.end = QMX_TEST_FRAME_END;
    g_test_common_tx_data.crc = crc32((uint8_t *)&g_test_common_tx_data, g_test_common_tx_data.len);

    // dump_u8buf("tx", (uint8_t *)&g_test_common_tx_data, g_test_common_tx_data.len+4);

    qmx_hal_uart_send_data(QMX_TEST_UART_HANDLE, (uint8_t *)&g_test_common_tx_data, (g_test_common_tx_data.len+4), HAL_UART_TIMEOUT_US);

    return 0;
}

int qmx_test_master_get_result(uint8_t *data, uint8_t *len)
{
    qmx_test_common_frame_t *rx_frame = (qmx_test_common_frame_t *)g_test_common_rx_data;
    uint32_t timeout = 0;
    int ret = 0;

    do {
        if (g_test_common_idx != 0)
            break;

        qmx_hal_nop_delay_ms(10);
        timeout += 10;
    } while (timeout < QMX_TEST_TIMEOUT_MS);

    if (timeout >= QMX_TEST_TIMEOUT_MS) {
        PRINTF("master get result timeout.\n");
        return -2;
    }

    if (qmx_test_check_frame() != 0) {
        ret = -1;
        goto end;
    }

    *len = QMX_TEST_FRAME_DATA_LEN;
    memcpy(data, rx_frame->data, QMX_TEST_FRAME_DATA_LEN);

end:
    g_test_common_idx = 0;
    return ret;
}

void qmx_test_master_clear_ack(void)
{
    g_test_common_idx = 0;
    memset((void *)&g_test_common_tx_data, 0, sizeof(qmx_test_common_frame_t));
    memset((void *)&g_test_common_rx_data, 0, QMX_TEST_RX_MAX_LEN);
}
