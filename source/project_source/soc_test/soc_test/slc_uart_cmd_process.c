#include "slc_uart_cmd_process.h"
#include "slc_hal_sysctrl.h"
#include "slc_hal_uart.h"
#include "slc_hal_intc.h"
#include "slc_hal_gpio.h"
#include "slc_subg.h"
#include "sw_crc.h"
#include "app_cfg.h"
#include "slc_soc_test.h"
#include "slc_private_spi_frame.h"
#include "compiler.h"

#define SLC_AUTOTEST_UART_TX_PIN        (HAL_GPIO_PIN0)
#define SLC_AUTOTEST_UART_RX_PIN        (HAL_GPIO_PIN1)
#define SLC_AUTOTEST_UART_IOMUX         (HAL_IOMUX_MODE0)

#define SLC_AUTOTEST_UART_HANDLE        (HAL_UART0)
#define SLC_AUTOTEST_UART_IRQ           (UART0_IRQ)
#define SLC_AUTOTEST_UART_CLK_GATE      (HAL_CLK_UART0)
#define SLC_AUTOTEST_UART_BAUDRATE      (921600)

#define SLC_AUTOTEST_RX_MAX_LEN         (128U)
#define SLC_AUTOTEST_TX_MAX_LEN         (8 + 2 + 8192)

volatile uint8_t g_autotest_tx_data[SLC_AUTOTEST_TX_MAX_LEN] = {0};
volatile uint8_t g_autotest_rx_data[SLC_AUTOTEST_RX_MAX_LEN] = {0};
volatile uint8_t g_autotest_rx_idx = 0;
volatile uint32_t g_autotest_baudrate = SLC_AUTOTEST_UART_BAUDRATE;

int slc_check_frame(void)
{
    uint16_t crc_expect = 0;
    uint16_t crc_real = 0;
    uint16_t rx_len = g_autotest_rx_data[RX_FRAME_LEN_IDX];

    if ((g_autotest_rx_data[0] != SLC_UART_FRAME_START) || (g_autotest_rx_data[rx_len-1] != SLC_UART_FRAME_END))
        return -1;

    crc_real = crc16_modbus((uint8_t *)g_autotest_rx_data, rx_len);

    crc_expect = (((uint16_t)g_autotest_rx_data[rx_len] << 8) | (uint16_t)g_autotest_rx_data[rx_len + 1]);

    if (crc_real != crc_expect)
        return -1;

    return 0;
}

void slc_autotest_uart_irq_handler(void)
{
    uint32_t sta __MAYBE_UNUSED = 0;
    uint16_t rx_len = SLC_AUTOTEST_RX_MAX_LEN;
    sta = slc_hal_uart_get_irq_status(SLC_AUTOTEST_UART_HANDLE);

    slc_hal_uart_receive_data(SLC_AUTOTEST_UART_HANDLE, (uint8_t *)g_autotest_rx_data, (uint32_t *)&rx_len, HAL_UART_TIMEOUT_US);
    g_autotest_rx_idx += rx_len;
}

void slc_auto_test_init(hal_sysctrl_src_e clk_src, hal_sysctrl_div_e div)
{
    slc_hal_sysctrl_cache_mode_set(HAL_CACHE_ENABLE);
    slc_hal_sysctrl_peripheral_mod_reset(HAL_CLK_GPIO);
    slc_hal_sysctrl_system_clock_init(clk_src, div);

    slc_hal_gpio_set_iomux(SLC_AUTOTEST_UART_TX_PIN, SLC_AUTOTEST_UART_IOMUX);
    slc_hal_gpio_set_iomux(SLC_AUTOTEST_UART_RX_PIN, SLC_AUTOTEST_UART_IOMUX);

    slc_hal_sysctrl_peripheral_clk_enable(SLC_AUTOTEST_UART_CLK_GATE, true);
    slc_hal_sysctrl_peripheral_mod_reset(SLC_AUTOTEST_UART_CLK_GATE);
    hal_uart_init_t config = {0};
    config.baudrate = g_autotest_baudrate;
    config.parity = HAL_UART_PARITY_EVEN;
    config.stopbit = HAL_UART_STOPBIT_1;
    config.databits = HAL_UART_DATA_8BIT;
    config.tx_fifo_thld = HAL_UART_TXFIFO_THLD_EMPTY;
    config.rx_fifo_thld = HAL_UART_RXFIFO_THLD_1BYTE;
    config.fifo_en = true;
    config.flow_ctrl_en = false;
    slc_hal_uart_init(SLC_AUTOTEST_UART_HANDLE, &config);

    slc_hal_register_irq_handler(SLC_AUTOTEST_UART_IRQ, slc_autotest_uart_irq_handler);
    slc_hal_uart_enable_irq(SLC_AUTOTEST_UART_HANDLE, HAL_UART_INT_EN_RX_NOT_EMPTY);
    SLC_HAL_ENABLE_PERIPHERAL_IRQ(SLC_AUTOTEST_UART_IRQ, 0x3);

    slc_private_spi_init();
}

uint32_t slc_autotest_cmd_process(void)
{
    uint8_t cmd = 0;
    uint8_t val0 = 0;
    uint8_t val1 = 0;
    uint32_t result = 0;
    uint32_t send_length = 0;

    cmd = g_autotest_rx_data[AUTOTEST_CMD_IDX];
    val0 = g_autotest_rx_data[AUTOTEST_VAL0_IDX];
    val1 = g_autotest_rx_data[AUTOTEST_VAL1_IDX];

    switch (cmd) {
        case AUTOTEST_SET_CLK_CMD:
            slc_auto_test_init((hal_sysctrl_src_e)val0, (hal_sysctrl_div_e)val1);
            g_autotest_tx_data[AUTOTEST_VAL0_IDX] = 0;
            break;

        case AUTOTEST_SEL_CASE_CMD:
            if (val0 >= SOCTEST_MOD_TYPE_MAX) {
                g_autotest_tx_data[AUTOTEST_VAL0_IDX] = 0x1;
                break;
            }

            soctest_set_test_mode(SOCTEST_MANUAL);
            soctest_set_test_num(1);
            soctest_set_test_module(val0);
            soctest_set_test_case(BIT(val1));

            soctest_run_case();

            result = soctest_get_test_result();
            if (result & BIT(val1)) {
                g_autotest_tx_data[AUTOTEST_VAL0_IDX] = 1;
            } else {
                g_autotest_tx_data[AUTOTEST_VAL0_IDX] = 0;
            }
    }

    g_autotest_tx_data[AUTOTEST_START_IDX] = SLC_UART_FRAME_START;
    g_autotest_tx_data[AUTOTEST_LEN_IDX] = 0x7;
    g_autotest_tx_data[AUTOTEST_RESV_IDX] = 0xFF;
    g_autotest_tx_data[AUTOTEST_CMD_IDX] = cmd;
    g_autotest_tx_data[AUTOTEST_VAL1_IDX] = 0xFF;
    g_autotest_tx_data[AUTOTEST_END_IDX] = SLC_UART_FRAME_END;
    send_length = 0x7;

    return send_length;
}

uint32_t slc_rw_cmd_process(void)
{
    uint32_t addr = 0;
    uint32_t value = 0;
    uint32_t send_length = 0;
    uint32_t block_length_bytes = 0;
    uint32_t i = 0;
    uint8_t rw_option = 0;
    uint8_t space = 0;

    // get addr from uart_rx_buf
    addr = ((uint32_t)g_autotest_rx_data[RX_FRAME_ADDR_IDX] << 24) | ((uint32_t)g_autotest_rx_data[RX_FRAME_ADDR_IDX+1] << 16) |
            ((uint32_t)g_autotest_rx_data[RX_FRAME_ADDR_IDX+2] << 8) | ((uint32_t)g_autotest_rx_data[RX_FRAME_ADDR_IDX+3]);
    // get the value to be written
    value = ((uint32_t)g_autotest_rx_data[RX_FRAME_DATA_IDX] << 24) | ((uint32_t)g_autotest_rx_data[RX_FRAME_DATA_IDX+1] << 16) |
            ((uint32_t)g_autotest_rx_data[RX_FRAME_DATA_IDX+2] << 8) | ((uint32_t)g_autotest_rx_data[RX_FRAME_DATA_IDX+3]);
    block_length_bytes = value; // if block read, block_length_bytes = value

    // get rw_flag from uart_rx_buf, [7:4]
    rw_option = g_autotest_rx_data[RX_FRAME_RW_FLAG_IDX] >> 4;
    // get space from uart_rx_buf, [3:0]
    space = g_autotest_rx_data[RX_FRAME_RW_FLAG_IDX] & 0xF;

    switch (rw_option) {
        // read & block_read similarly
        case SLC_UART_READ:
            block_length_bytes = 1;
        case SLC_UART_BLOCK_READ:
            for (i = 0; i < block_length_bytes * 4 ; i += 4) {
                if (space == RF_SPI_OPTION)
                    value = slc_rf_spi_read32_cmd((uint16_t)addr);
                else if (space == PHY_SPI_OPTION)
                    value = slc_phy_spi_read32_cmd((uint16_t)addr);
                else if (space == CPU_OPTION)
                    value = read32(addr);
                else
                    value = 0xDEADB5B5;

                g_autotest_tx_data[RX_FRAME_DATA_IDX + 0 + i] = (value >> 24) & 0xFF;
                g_autotest_tx_data[RX_FRAME_DATA_IDX + 1 + i] = (value >> 16) & 0xFF;
                g_autotest_tx_data[RX_FRAME_DATA_IDX + 2 + i] = (value >> 8) & 0xFF;
                g_autotest_tx_data[RX_FRAME_DATA_IDX + 3 + i] = value & 0xFF;
                addr += 4;
            }

            // build g_autotest_tx_data package
            g_autotest_tx_data[RX_FRAME_START_IDX] = SLC_UART_FRAME_START;
            g_autotest_tx_data[RX_FRAME_LEN_IDX] = SLC_UART_RX_FRAME_LEN(block_length_bytes);
            g_autotest_tx_data[RX_FRAME_RW_FLAG_IDX] = g_autotest_rx_data[RX_FRAME_RW_FLAG_IDX];
            g_autotest_tx_data[RX_FRAME_ADDR_IDX]   = g_autotest_rx_data[RX_FRAME_ADDR_IDX];
            g_autotest_tx_data[RX_FRAME_ADDR_IDX+1] = g_autotest_rx_data[RX_FRAME_ADDR_IDX+1];
            g_autotest_tx_data[RX_FRAME_ADDR_IDX+2] = g_autotest_rx_data[RX_FRAME_ADDR_IDX+2];
            g_autotest_tx_data[RX_FRAME_ADDR_IDX+3] = g_autotest_rx_data[RX_FRAME_ADDR_IDX+3];
            g_autotest_tx_data[RX_FRAME_DATA_IDX + block_length_bytes * 4] = SLC_UART_FRAME_END;
            send_length = SLC_UART_RX_FRAME_LEN(block_length_bytes);
            break;

        case SLC_UART_WRITE:
            // judgment spi_flag and execute the corresponding spi operation
            if (space == RF_SPI_OPTION)
                slc_rf_spi_write32_cmd((uint16_t)addr, value);
            else if (space == PHY_SPI_OPTION)
                slc_phy_spi_write32_cmd((uint16_t)addr, value);
            else if (space == CPU_OPTION)
                write32(addr, value);
            else
                break;

            // build g_autotest_tx_data package
            g_autotest_tx_data[TX_FRAME_START_IDX] = SLC_UART_FRAME_START;
            g_autotest_tx_data[TX_FRAME_LEN_IDX] = SLC_UART_TX_FRAME_LEN;
            g_autotest_tx_data[TX_FRAME_RW_FLAG_IDX] = g_autotest_rx_data[RX_FRAME_RW_FLAG_IDX];
            g_autotest_tx_data[TX_FRAME_ADDR_IDX] = g_autotest_rx_data[RX_FRAME_ADDR_IDX];
            g_autotest_tx_data[TX_FRAME_ADDR_IDX+1] = g_autotest_rx_data[RX_FRAME_ADDR_IDX+1];
            g_autotest_tx_data[TX_FRAME_ADDR_IDX+2] = g_autotest_rx_data[RX_FRAME_ADDR_IDX+2];
            g_autotest_tx_data[TX_FRAME_ADDR_IDX+3] = g_autotest_rx_data[RX_FRAME_ADDR_IDX+3];
            g_autotest_tx_data[TX_FRAME_STATUS_IDX] = 0x01;
            g_autotest_tx_data[TX_FRAME_END_IDX] = SLC_UART_FRAME_END;
            send_length = SLC_UART_TX_FRAME_LEN;
            break;

        default:
            break;
    }

    return send_length;
}

void slc_uart_cmd_process_main(void)
{
    uint32_t send_length = 0;
    uint8_t option = 0;
    uint16_t tx_crc = 0;

    slc_auto_test_init(HAL_SYSCLK_RC50M, HAL_SYSCLK_DIV_NONE);

    memset((void *)g_autotest_tx_data, 0, sizeof(g_autotest_tx_data));

    while (1) {
        if (g_autotest_rx_idx == 0)
            continue;

        if (slc_check_frame() != 0) {
            g_autotest_rx_idx = 0;
            continue;
        }

        option = g_autotest_rx_data[RX_FRAME_RW_FLAG_IDX];
        if (option != 0xFF)
            send_length = slc_rw_cmd_process();
        else
            send_length = slc_autotest_cmd_process();

        if (send_length == 0)
            continue;

        // after each processing of uart data, clear uart_rx_buf and g_autotest_rx_idx
        g_autotest_rx_idx = 0;
        memset((void *)g_autotest_rx_data, 0, sizeof(g_autotest_rx_data));

        tx_crc = crc16_modbus((uint8_t *)g_autotest_tx_data, send_length);
        g_autotest_tx_data[send_length] = (tx_crc >> 8);
        g_autotest_tx_data[send_length+1] = (tx_crc & 0xFF);
        send_length += 2;

        // Send read and write results to the host computer
        slc_hal_uart_send_data(SLC_AUTOTEST_UART_HANDLE, (uint8_t *)g_autotest_tx_data, send_length, HAL_UART_TIMEOUT_US);

        // 在发送完数据（空闲）后再去清空已使用的buffer
        memset((void *)g_autotest_tx_data, 0, send_length);
    }
}
