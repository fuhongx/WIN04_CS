#include "slc_test_master.h"
#include "slc_hal_uart.h"
#include "app_cfg.h"
#include "utility.h"
#include "debug.h"
#include "slc_hal_sysctrl.h"
#include "slc_hal_gpio.h"
#include "slc_hal_delay.h"
#include "sw_crc.h"

int slc_test_master_send_cmd(slc_test_cmd_e cmd, uint8_t *data, uint8_t len)
{
    if (len > SLC_TEST_FRAME_DATA_LEN) {
        PRINTF("cmd data len=%u > %u.\n", len, SLC_TEST_FRAME_DATA_LEN);
        return -1;
    }

    if (cmd >= SLC_TEST_CMD_MAX) {
        PRINTF("cmd=%u invalid.\n", cmd);
        return -1;
    }

    memset((void *)&g_test_common_tx_data, 0, sizeof(slc_test_common_frame_t));

    g_test_common_tx_data.start = SLC_TEST_FRAME_START;
    g_test_common_tx_data.len = SLC_TEST_FRAME_LEN;
    g_test_common_tx_data.cmd = (uint8_t)cmd;
    if (data != NULL && len > 0) {
        memcpy((void *)g_test_common_tx_data.data, data, len);
    }
    g_test_common_tx_data.end = SLC_TEST_FRAME_END;
    g_test_common_tx_data.crc = crc32((uint8_t *)&g_test_common_tx_data, g_test_common_tx_data.len);

    // dump_u8buf("tx", (uint8_t *)&g_test_common_tx_data, g_test_common_tx_data.len+4);

    slc_hal_uart_send_data(SLC_TEST_UART_HANDLE, (uint8_t *)&g_test_common_tx_data, (g_test_common_tx_data.len+4), HAL_UART_TIMEOUT_US);

    return 0;
}

int slc_test_master_get_result(uint8_t *data, uint8_t *len)
{
    slc_test_common_frame_t *rx_frame = (slc_test_common_frame_t *)g_test_common_rx_data;
    uint32_t timeout = 0;
    int ret = 0;

    do {
        if (g_test_common_idx != 0)
            break;

        slc_hal_nop_delay_ms(10);
        timeout += 10;
    } while (timeout < SLC_TEST_TIMEOUT_MS);

    if (timeout >= SLC_TEST_TIMEOUT_MS) {
        PRINTF("master get result timeout.\n");
        return -2;
    }

    if (slc_test_check_frame() != 0) {
        ret = -1;
        goto end;
    }

    *len = SLC_TEST_FRAME_DATA_LEN;
    memcpy(data, rx_frame->data, SLC_TEST_FRAME_DATA_LEN);

end:
    g_test_common_idx = 0;
    return ret;
}

void slc_test_master_clear_ack(void)
{
    g_test_common_idx = 0;
    memset((void *)&g_test_common_tx_data, 0, sizeof(slc_test_common_frame_t));
    memset((void *)&g_test_common_rx_data, 0, SLC_TEST_RX_MAX_LEN);
}
