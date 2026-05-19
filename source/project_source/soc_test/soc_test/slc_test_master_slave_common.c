#include "slc_test_master_slave_common.h"
#include "slc_hal_uart.h"
#include "app_cfg.h"
#include "utility.h"
#include "debug.h"
#include "slc_hal_sysctrl.h"
#include "slc_hal_intc.h"
#include "sw_crc.h"

volatile slc_test_common_frame_t g_test_common_tx_data = {0};
volatile uint8_t g_test_common_rx_data[SLC_TEST_RX_MAX_LEN] = {0};
volatile uint8_t g_test_common_idx = 0;

int slc_test_check_frame(void)
{
    uint32_t crc_real = 0;
    slc_test_common_frame_t *rx_frame = (slc_test_common_frame_t *)g_test_common_rx_data;

    if ((rx_frame->start != SLC_TEST_FRAME_START) || (rx_frame->end != SLC_TEST_FRAME_END) ||
        (rx_frame->len != SLC_TEST_FRAME_LEN) || (rx_frame->cmd >= SLC_TEST_CMD_MAX)) {
        PRINTF("frame error, start: 0x%02X, end: 0x%02X, len: %u, cmd: %u\r\n",
               rx_frame->start, rx_frame->end, rx_frame->len, rx_frame->cmd);
        if (g_test_common_idx > 0)
            dump_u8buf("frame data", (uint8_t *)g_test_common_rx_data, rx_frame->len+4);
        return -1;
    }

    crc_real = crc32((uint8_t *)g_test_common_rx_data, rx_frame->len);

    if (crc_real != rx_frame->crc) {
        PRINTF("crc error, real: 0x%08X, expect: 0x%08X\r\n", crc_real, rx_frame->crc);
        if (g_test_common_idx > 0)
            dump_u8buf("frame data", (uint8_t *)g_test_common_rx_data, rx_frame->len+4);
        return -1;
    }

    return 0;
}

void slc_test_uart_irq_handler(void)
{
    uint32_t sta = 0;
    uint16_t rx_len = SLC_TEST_RX_MAX_LEN;
    sta = slc_hal_uart_get_irq_status(SLC_TEST_UART_HANDLE);

    if (sta & (HAL_UART_STA_RX_READY | HAL_UART_STA_RX_NOT_EMPTY)) {
        slc_hal_uart_receive_data(SLC_TEST_UART_HANDLE, (uint8_t *)g_test_common_rx_data, (uint32_t *)&rx_len, HAL_UART_TIMEOUT_US);
        g_test_common_idx += rx_len;
    }

    if (sta & (HAL_UART_STA_TX_FIFO_EMPTY | HAL_UART_STA_TX_EMPTY)) {
        slc_hal_uart_disable_irq(SLC_TEST_UART_HANDLE, HAL_UART_INT_EN_TX_EMPTY);
    }
    if (sta & HAL_UART_STA_TX_UNDER_THLD) {
        slc_hal_uart_disable_irq(SLC_TEST_UART_HANDLE, HAL_UART_INT_EN_UNDER_THLD);
    }

    if (sta & (HAL_UART_STA_RX_OVERFLOW | HAL_UART_STA_RX_PARITY_ERROR | HAL_UART_STA_RX_FRAME_ERROR | HAL_UART_STA_RX_ERROR)) {
        PRINTF("uart RX error, status: 0x%08X\r\n", sta);
    }

    // clear irq status
    slc_hal_uart_clear_irq_status(SLC_TEST_UART_HANDLE, sta);
    // PRINTF("uart irq, status: 0x%08X\r\n", sta);
}

void slc_test_common_init(void)
{
    memset((void *)&g_test_common_tx_data, 0, sizeof(slc_test_common_frame_t));
    g_test_common_idx = 0;
    memset((void *)g_test_common_rx_data, 0, sizeof(g_test_common_rx_data));

    slc_hal_gpio_set_iomux(SLC_TEST_UART_TX_PIN, SLC_TEST_UART_IOMUX);
    slc_hal_gpio_set_iomux(SLC_TEST_UART_RX_PIN, SLC_TEST_UART_IOMUX);
    slc_hal_gpio_set_mode(SLC_TEST_UART_TX_PIN, HAL_GPIO_PULL_UP);
    slc_hal_gpio_set_mode(SLC_TEST_UART_RX_PIN, HAL_GPIO_PULL_UP);

    slc_hal_sysctrl_peripheral_clk_enable(SLC_TEST_UART_CLK_GATE, true);
    slc_hal_sysctrl_peripheral_mod_reset(SLC_TEST_UART_CLK_GATE);
    hal_uart_init_t config = {0};
    config.baudrate = SLC_TEST_UART_BAUDRATE;
    config.parity = HAL_UART_PARITY_NONE;
    config.stopbit = HAL_UART_STOPBIT_1;
    config.databits = HAL_UART_DATA_8BIT;
    config.tx_fifo_thld = HAL_UART_TXFIFO_THLD_EMPTY;
    config.rx_fifo_thld = HAL_UART_RXFIFO_THLD_1BYTE;
    config.fifo_en = true;
    config.flow_ctrl_en = false;
    slc_hal_uart_init(SLC_TEST_UART_HANDLE, &config);

    slc_hal_register_irq_handler(SLC_TEST_UART_IRQ, slc_test_uart_irq_handler);
    slc_hal_uart_enable_irq(SLC_TEST_UART_HANDLE, HAL_UART_INT_EN_RX_NOT_EMPTY);
    SLC_HAL_ENABLE_PERIPHERAL_IRQ(SLC_TEST_UART_IRQ, 0x3);
}

void slc_test_common_deinit(void)
{
    slc_hal_uart_disable_irq(SLC_TEST_UART_HANDLE, HAL_UART_INT_EN_RX_NOT_EMPTY);
    SLC_HAL_DISABLE_PERIPHERAL_IRQ(SLC_TEST_UART_IRQ);
    slc_hal_unregister_irq_handler(SLC_TEST_UART_IRQ);

    slc_hal_uart_deinit(SLC_TEST_UART_HANDLE);
}
