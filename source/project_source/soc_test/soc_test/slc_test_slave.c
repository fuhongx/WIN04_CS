#include "slc_test_slave.h"
#include "app_cfg.h"
#include "utility.h"
#include "debug.h"
#include "slc_hal_uart.h"
#include "slc_hal_sysctrl.h"
#include "slc_hal_gpio.h"
#include "slc_hal_delay.h"
#include "slc_hal_iic.h"
#include "slc_hal_spi.h"
#include "slc_hal_intc.h"
#include "slc_hal_lpuart.h"
#include "sw_crc.h"

static int g_spi_slave_id = HAL_SPI0;
static volatile uint8_t g_spi_slave_rx_buf[SLC_SPI_TEST_TRX_LEN];
static volatile uint32_t g_spi_slave_rx_len = 0;

extern void print_reg_4byte_from_to(uint32_t start_addr, uint32_t end_addr);
void slc_test_slave_spi_irq(void)
{
    uint32_t sta = slc_hal_spi_get_irq_sta(g_spi_slave_id);
    uint8_t cmd[4] = {0};
    int ret = 0;

    g_spi_slave_rx_len = 4;
    ret += slc_hal_spi_slave_receive(g_spi_slave_id, (void *)cmd, (uint32_t *)&g_spi_slave_rx_len, 1000);

    if ((cmd[0] == SLC_SPI_TEST_TX_CMD) && (g_spi_slave_rx_len == 4)) {
        g_spi_slave_rx_len = SLC_SPI_TEST_TRX_LEN;
        ret += slc_hal_spi_slave_receive(g_spi_slave_id, (void *)g_spi_slave_rx_buf, (uint32_t *)&g_spi_slave_rx_len, HAL_SPI_TIMEOUT_US);
        dump_u8buf("SPI rx data", (uint8_t *)g_spi_slave_rx_buf, g_spi_slave_rx_len);
    } else if ((cmd[0] == SLC_SPI_TEST_RX_CMD) && (g_spi_slave_rx_len == 4)) {
        ret += slc_hal_spi_slave_transmit(g_spi_slave_id, (void *)g_spi_slave_rx_buf, SLC_SPI_TEST_TRX_LEN, HAL_SPI_TIMEOUT_US);
        dump_u8buf("SPI tx data", (uint8_t *)g_spi_slave_rx_buf, SLC_SPI_TEST_TRX_LEN);
    }

    slc_hal_spi_clear_irq_sta(g_spi_slave_id, sta);
    PRINTF("SPI irq: 0x%08x, len=%u, ret %d cur 0x%x, sta 0x%x\n",
        sta, g_spi_slave_rx_len, ret, slc_hal_spi_get_irq_sta(g_spi_slave_id), slc_hal_spi_get_cur_sta(g_spi_slave_id));
    // if (g_spi_slave_rx_len != 0) {
    //     dump_u8buf("SPI data", (uint8_t *)cmd, 4);
    // }
}

int slc_test_slave_spi_cfg(slc_test_common_frame_t *rx_frame)
{
    hal_spi_cfg_t config = {0};

    if (rx_frame->data[0] == HAL_SPI_MASTER) {
        config.mode = HAL_SPI_SLAVE;
    } else {
        config.mode = HAL_SPI_MASTER;
    }

    config.clk_div = (hal_spi_clk_div_e)rx_frame->data[1];
    config.polarity_phase = rx_frame->data[2];
    config.data_mode = rx_frame->data[3];
    config.data_len = rx_frame->data[4];
    config.cs_holding_time = rx_frame->data[5];
    config.cs_gap_time = rx_frame->data[6];
    config.clk_adjust_en = rx_frame->data[7];
    config.sw_cs_en = false;
    config.anti_noise_level = rx_frame->data[9];
    g_spi_slave_id = rx_frame->data[10];
    config.tx_fifo_pfull_th = 12;
    config.tx_fifo_pempty_th = 4;
    config.rx_fifo_pfull_th = 4;
    config.rx_fifo_pempty_th = 2;

    if (g_spi_slave_id == HAL_SPI0) {
        slc_hal_gpio_set_iomux(HAL_GPIO_PIN8, HAL_IOMUX_MODE2); // SPI0 CS
        slc_hal_gpio_set_iomux(HAL_GPIO_PIN9, HAL_IOMUX_MODE2); // SPI0 CLK
        slc_hal_gpio_set_iomux(HAL_GPIO_PIN10, HAL_IOMUX_MODE2); // SPI0 MOSI
        slc_hal_gpio_set_iomux(HAL_GPIO_PIN11, HAL_IOMUX_MODE2); // SPI0 MISO
        slc_hal_gpio_set_mode(HAL_GPIO_PIN8, HAL_GPIO_PULL_UP);
        slc_hal_gpio_set_mode(HAL_GPIO_PIN9, HAL_GPIO_PULL_UP);
        slc_hal_gpio_set_mode(HAL_GPIO_PIN10, HAL_GPIO_PULL_UP);
        slc_hal_gpio_set_mode(HAL_GPIO_PIN11, HAL_GPIO_PULL_UP);

        SLC_HAL_DISABLE_PERIPHERAL_IRQ(SPI0_IRQ);
        slc_hal_sysctrl_peripheral_clk_enable(HAL_CLK_SPI0, true);
        slc_hal_sysctrl_peripheral_mod_reset(HAL_CLK_SPI0);
    } else {
        slc_hal_gpio_set_iomux(HAL_GPIO_PIN4, HAL_IOMUX_MODE2); // SPI1 CS
        slc_hal_gpio_set_iomux(HAL_GPIO_PIN5, HAL_IOMUX_MODE2); // SPI1 CLK
        slc_hal_gpio_set_iomux(HAL_GPIO_PIN6, HAL_IOMUX_MODE2); // SPI1 MOSI
        slc_hal_gpio_set_iomux(HAL_GPIO_PIN7, HAL_IOMUX_MODE2); // SPI1 MISO
        slc_hal_gpio_set_mode(HAL_GPIO_PIN4, HAL_GPIO_PULL_UP);
        slc_hal_gpio_set_mode(HAL_GPIO_PIN5, HAL_GPIO_PULL_UP);
        slc_hal_gpio_set_mode(HAL_GPIO_PIN6, HAL_GPIO_PULL_UP);
        slc_hal_gpio_set_mode(HAL_GPIO_PIN7, HAL_GPIO_PULL_UP);

        SLC_HAL_DISABLE_PERIPHERAL_IRQ(SPI1_IRQ);
        slc_hal_sysctrl_peripheral_clk_enable(HAL_CLK_SPI1, true);
        slc_hal_sysctrl_peripheral_mod_reset(HAL_CLK_SPI1);
    }

    slc_hal_spi_init(g_spi_slave_id, &config);
    // slc_hal_spi_set_irq_mask(g_spi_slave_id, HAL_SPI_IRQ_TX_EMPTY | HAL_SPI_IRQ_TX_PEMPTY);
    slc_hal_spi_clear_irq_sta(g_spi_slave_id, 0x1F);

    if (config.mode == HAL_SPI_SLAVE) {
        if (g_spi_slave_id == HAL_SPI0) {
            slc_hal_register_irq_handler(SPI0_IRQ, slc_test_slave_spi_irq);
            SLC_HAL_ENABLE_PERIPHERAL_IRQ(SPI0_IRQ, 0x3);
        } else {
            slc_hal_register_irq_handler(SPI1_IRQ, slc_test_slave_spi_irq);
            SLC_HAL_ENABLE_PERIPHERAL_IRQ(SPI1_IRQ, 0x3);
        }

        slc_hal_spi_clear_irq_sta(g_spi_slave_id, 0x1F);
        slc_hal_spi_irq_enable(g_spi_slave_id, HAL_SPI_IRQ_RX_PFULL);
    }

    PRINTF("SPI%d slave config done\n", g_spi_slave_id);
    //print_reg_4byte_from_to((uint32_t)&SPI0->SPI_TCR, (uint32_t)&SPI0->SPI_FTR);
    PRINTF("mode=%d, clk_div=%d, polarity_phase=%d, data_mode=%d, data_len=%d, cs_holding_time=%u, clk_adjust_en=%d, sw_cs_en=%d, anti_noise_level=%d\n",
            config.mode, config.clk_div, config.polarity_phase, config.data_mode, config.data_len,
            config.cs_holding_time, config.clk_adjust_en, config.sw_cs_en, config.anti_noise_level);

    return 0;
}

static volatile uint8_t g_iic_slave_rx_buf[SLC_IIC_TEST_TRX_LEN];
static volatile uint32_t g_iic_slave_rx_len = 0;
static int g_iic_slave_id = HAL_IIC0;

void slc_test_slave_iic_irq(void)
{
    uint32_t iic_int_flag;
    int ret = 0;

    iic_int_flag = slc_hal_iic_get_irq_status(g_iic_slave_id);
    slc_hal_iic_clear_irq_status(g_iic_slave_id, iic_int_flag);

    if (iic_int_flag & HAL_IIC_IRQ_STA_READ_REQ) {
        ret = slc_hal_iic_slave_send(g_iic_slave_id, (uint8_t *)g_iic_slave_rx_buf, g_iic_slave_rx_len, HAL_IIC_TIMEOUT);
        if (ret != 0) {
            PRINTF("IIC%d slave tx error, ret=0x%X\n", g_iic_slave_id, ret);
        }
    } else if (iic_int_flag & HAL_IIC_IRQ_STA_RX_FIFO_FULL) {
        g_iic_slave_rx_len = SLC_IIC_TEST_TRX_LEN;
        ret = slc_hal_iic_slave_recv(g_iic_slave_id, (uint8_t *)g_iic_slave_rx_buf, (uint32_t *)&g_iic_slave_rx_len, HAL_IIC_TIMEOUT);
        if (ret != 0 && (g_iic_slave_rx_len == 0)) {
            PRINTF("IIC%d slave rx error, ret=0x%X, len=%u\n", g_iic_slave_id, ret, g_iic_slave_rx_len);
        } else {
            dump_u8buf("slave rx buf", (uint8_t *)g_iic_slave_rx_buf, g_iic_slave_rx_len);
        }
    }

    // PRINTF("IIC IRQ Handler, int status 0x%x\n", iic_int_flag);
}

int slc_test_slave_iic_cfg(slc_test_common_frame_t *rx_frame)
{
    hal_iic_init_t config = {0};

    if (rx_frame->data[0] == HAL_IIC_MASTER) {
        config.mode = HAL_IIC_SLAVE;
    } else {
        config.mode = HAL_IIC_MASTER;
    }
    config.speed = (hal_iic_speed_e)rx_frame->data[1];
    config.addr_bits = (hal_iic_addr_bits_e)rx_frame->data[2];
    config.addr = rx_frame->data[3] | (rx_frame->data[4] << 8);
    config.tx_thld = rx_frame->data[5];
    config.rx_thld = rx_frame->data[6];

    if (rx_frame->data[7] == HAL_IIC0) {
        slc_hal_gpio_set_iomux(HAL_GPIO_PIN4, HAL_IOMUX_MODE5); // I2C0 SCL
        slc_hal_gpio_set_iomux(HAL_GPIO_PIN5, HAL_IOMUX_MODE5); // I2C0 SDA

        slc_hal_sysctrl_peripheral_clk_enable(HAL_CLK_I2C0, true);
        slc_hal_sysctrl_peripheral_mod_reset(HAL_CLK_I2C0);
    } else {
        slc_hal_gpio_set_iomux(HAL_GPIO_PIN6, HAL_IOMUX_MODE5); // I2C1 SCL
        slc_hal_gpio_set_iomux(HAL_GPIO_PIN7, HAL_IOMUX_MODE5); // I2C1 SDA

        slc_hal_sysctrl_peripheral_clk_enable(HAL_CLK_I2C1, true);
        slc_hal_sysctrl_peripheral_mod_reset(HAL_CLK_I2C1);
    }

    slc_hal_iic_init((hal_iic_id_e)rx_frame->data[7], &config);

    g_iic_slave_id = rx_frame->data[7];
    if (config.mode == HAL_IIC_SLAVE) {
        if (rx_frame->data[7] == HAL_IIC0) {
            slc_hal_register_irq_handler(I2C0_IRQ, slc_test_slave_iic_irq);
            SLC_HAL_ENABLE_PERIPHERAL_IRQ(I2C0_IRQ, 0x3);
        } else {
            slc_hal_register_irq_handler(I2C1_IRQ, slc_test_slave_iic_irq);
            SLC_HAL_ENABLE_PERIPHERAL_IRQ(I2C1_IRQ, 0x3);
        }

        slc_hal_iic_irq_enable((hal_iic_id_e)rx_frame->data[7], HAL_IIC_IRQ_RX_FIFO_FULL | HAL_IIC_IRQ_READ_REQ);
    }

    PRINTF("IIC%d mode=%d, speed=%d, addr_bits=%d, addr=0x%04X\n", g_iic_slave_id, config.mode,
            config.speed, config.addr_bits, config.addr);
    return 0;
}

static volatile uint8_t g_test_lpuart_rx_buf[SLC_LPUART_TEST_TRX_LEN] = {0};
static volatile uint32_t g_test_lpuart_rx_len = 0;
void slc_test_slv_lpuart_irq(void)
{
    uint32_t status = 0;
    status = slc_hal_lpuart_get_irq_status(HAL_LPUART0);

    g_test_lpuart_rx_len = SLC_LPUART_TEST_TRX_LEN;
    slc_hal_lpuart_receive_data(HAL_LPUART0, (uint8_t *)g_test_lpuart_rx_buf, (uint32_t *)&g_test_lpuart_rx_len);
    if (g_test_lpuart_rx_len != 0) {
        dump_u8buf("lpuart rx buf", (uint8_t *)g_test_lpuart_rx_buf, g_test_lpuart_rx_len);
    }

    slc_hal_lpuart_clear_irq_status(HAL_LPUART0, status);
}

int slc_test_slave_lpuart_cfg(slc_test_common_frame_t *rx_frame)
{
    hal_lpuart_init_t lpuart_init = {0};

    lpuart_init.baudrate = rx_frame->data[0] | (rx_frame->data[1] << 8) | (rx_frame->data[2] << 16) | (rx_frame->data[3] << 24);
    lpuart_init.parity = (hal_lpuart_parity_e)rx_frame->data[4];
    lpuart_init.stopbit = (hal_lpuart_stopbit_e)rx_frame->data[5];
    lpuart_init.datalen = (hal_lpuart_datalen_e)rx_frame->data[6];
    lpuart_init.rx_fifo_thld = rx_frame->data[7];

    slc_hal_gpio_set_iomux(HAL_GPIO_PIN18, HAL_IOMUX_MODE3);
    slc_hal_gpio_set_iomux(HAL_GPIO_PIN19, HAL_IOMUX_MODE3);
    slc_hal_lpio_set_iomux(HAL_GPIO_PIN18, HAL_IOMUX_MODE3);
    slc_hal_lpio_set_iomux(HAL_GPIO_PIN19, HAL_IOMUX_MODE3);
    slc_hal_gpio_set_mode(HAL_GPIO_PIN18, HAL_GPIO_PULL_UP);
    slc_hal_gpio_set_mode(HAL_GPIO_PIN19, HAL_GPIO_PULL_UP);

    slc_hal_sysctrl_peripheral_clk_enable(HAL_CLK_LPUART, true);
    slc_hal_sysctrl_peripheral_mod_reset(HAL_CLK_LPUART);

    slc_hal_lpuart_init((hal_lpuart_id_e)rx_frame->data[8], &lpuart_init);
    slc_hal_lpuart_enable_irq(HAL_LPUART0, HAL_LPUART_INT_EN_RX_DONE);
    slc_hal_register_irq_handler(LPUART_IRQ, slc_test_slv_lpuart_irq);
    SLC_HAL_ENABLE_PERIPHERAL_IRQ(LPUART_IRQ, 0x3);

    return 0;
}

int slc_test_slave_uart_cfg(slc_test_common_frame_t *rx_frame)
{
    hal_uart_init_t config = {0};
    int ret = 0;

    config.baudrate = rx_frame->data[0] | (rx_frame->data[1] << 8) | (rx_frame->data[2] << 16) | (rx_frame->data[3] << 24);
    config.parity = (hal_uart_parity_e)rx_frame->data[4];
    config.stopbit = (hal_uart_stopbit_e)rx_frame->data[5];
    config.databits = (hal_uart_databits_e)rx_frame->data[6];
    config.tx_fifo_thld = (hal_uart_txfifo_thld_e)rx_frame->data[7];
    config.rx_fifo_thld = (hal_uart_rxfifo_thld_e)rx_frame->data[8];
    config.fifo_en = rx_frame->data[9];
    config.flow_ctrl_en = rx_frame->data[10];

    slc_hal_sysctrl_peripheral_clk_enable(SLC_TEST_UART_CLK_GATE, true);
    slc_hal_sysctrl_peripheral_mod_reset(SLC_TEST_UART_CLK_GATE);

    ret = slc_hal_uart_init((hal_uart_id_e)rx_frame->data[11], &config);
    slc_hal_uart_enable_irq(SLC_TEST_UART_HANDLE, HAL_UART_INT_EN_RX_NOT_EMPTY);

    return ret;
}

void slc_test_slave_main(void)
{
    slc_test_common_frame_t *rx_frame = NULL;
    uint8_t i, iic_dir, lpuart_dir, test_len = 0;
    uint8_t buf[SLC_TEST_UART_RX_FIFO_THLD*2] = {0};
    hal_gpio_pin_e pin;
    uint32_t crc = 0;

    slc_test_common_init();

    while (1) {
        if (g_test_common_idx == 0)
            continue;

        if (slc_test_check_frame() != 0) {
            g_test_common_idx = 0;
            continue;
        }
        dump_u8buf("rx_frame", (uint8_t *)g_test_common_rx_data, g_test_common_idx);

        rx_frame = (slc_test_common_frame_t *)g_test_common_rx_data;
        memset((void *)&g_test_common_tx_data, 0, sizeof(g_test_common_tx_data));
        g_test_common_idx = 0;
        test_len = 0;
        memset((void *)buf, 0, sizeof(buf));

        g_test_common_tx_data.start = SLC_TEST_FRAME_START;
        g_test_common_tx_data.len = SLC_TEST_FRAME_LEN;
        g_test_common_tx_data.cmd = rx_frame->cmd;
        g_test_common_tx_data.end = SLC_TEST_FRAME_END;

        switch (rx_frame->cmd)
        {
            case SLC_TEST_CMD_UART_CFG:
                g_test_common_tx_data.data[0] = 0;

                break;

            case SLC_TEST_CMD_UART_TRX:
                memcpy((void *)g_test_common_tx_data.data, (void *)rx_frame->data, SLC_TEST_FRAME_DATA_LEN);
                break;

            case SLC_TEST_CMD_UART_RX_FIFO:
                test_len = rx_frame->data[0];
                g_test_common_tx_data.data[0] = (test_len != 0) ? 0 : 1;

                break;

            case SLC_TEST_CMD_LPUART_CFG:
                g_test_common_tx_data.data[0] = 0;
                break;

            case SLC_TEST_CMD_LPUART_TRX:
                g_test_common_tx_data.data[0] = 0;
                lpuart_dir = rx_frame->data[1];
                break;

            case SLC_TEST_CMD_GPIO_SET:
                pin = (hal_gpio_pin_e)rx_frame->data[0];
                slc_hal_gpio_set_iomux(pin, HAL_IOMUX_GPIO);
                slc_hal_gpio_set_direction(pin, HAL_GPIO_OUTPUT);
                slc_hal_gpio_set_mode(pin, HAL_GPIO_PULL_NONE);
                slc_hal_gpio_set_value(pin, (hal_gpio_level_e)rx_frame->data[1]);

                g_test_common_tx_data.data[0] = 0;

                slc_hal_nop_delay_ms(20);
                break;

            case SLC_TEST_CMD_GPIO_GET:
                pin = (hal_gpio_pin_e)rx_frame->data[0];
                slc_hal_gpio_set_iomux(pin, HAL_IOMUX_GPIO);
                slc_hal_gpio_set_direction(pin, HAL_GPIO_INPUT);
                slc_hal_gpio_set_mode(pin, HAL_GPIO_PULL_NONE);
                g_test_common_tx_data.data[1] = (uint8_t)slc_hal_gpio_get_value(pin);

                g_test_common_tx_data.data[0] = 0;

                slc_hal_nop_delay_ms(20);
                break;

            case SLC_TEST_CMD_I2C_CFG:
                g_test_common_tx_data.data[0] = 0;
                break;

            case SLC_TEST_CMD_I2C_S_TRX:
                g_test_common_tx_data.data[0] = 0;
                iic_dir = rx_frame->data[1];
                break;

            case SLC_TEST_CMD_SPI_CFG:
                g_test_common_tx_data.data[0] = 0;
                break;

            case SLC_TEST_CMD_SPI_S_TRX:
                g_test_common_tx_data.data[0] = 0;
                iic_dir = rx_frame->data[1];
                break;

            default:
                PRINTF("slave unknown cmd: %u\r\n", rx_frame->cmd);
                break;
        }

        crc = crc32((uint8_t *)&g_test_common_tx_data, g_test_common_tx_data.len);
        g_test_common_tx_data.crc = crc;

        slc_hal_uart_send_data(SLC_TEST_UART_HANDLE, (uint8_t *)&g_test_common_tx_data, (g_test_common_tx_data.len+4), HAL_UART_TIMEOUT_US);

        if (rx_frame->cmd == SLC_TEST_CMD_UART_CFG) {
            slc_hal_nop_delay_ms(SLC_TEST_CFG_TIMEOUT_MS);    // 确保fifo数据都发完
            slc_test_slave_uart_cfg(rx_frame);
        }

        if ((test_len != 0) && (rx_frame->cmd == SLC_TEST_CMD_UART_RX_FIFO)) {
            slc_hal_nop_delay_ms(100);

            for (i = 0; i < test_len; i++) {
                buf[i] = (uint8_t)(i + 1);
            }

            slc_hal_uart_send_data(SLC_TEST_UART_HANDLE, buf, test_len, HAL_UART_TIMEOUT_US);
        }

        if (rx_frame->cmd == SLC_TEST_CMD_LPUART_CFG) {
            slc_hal_nop_delay_ms(SLC_TEST_CFG_TIMEOUT_MS);    // 确保fifo数据都发完
            slc_test_slave_lpuart_cfg(rx_frame);
        }

        if (rx_frame->cmd == SLC_TEST_CMD_LPUART_TRX) {
            slc_hal_nop_delay_ms(SLC_TEST_CFG_TIMEOUT_MS);    // 确保fifo数据都发完
            if (lpuart_dir == 1) {
                slc_hal_lpuart_send_data((hal_lpuart_id_e)rx_frame->data[0], (uint8_t *)g_test_lpuart_rx_buf, g_test_lpuart_rx_len);
            }
        }

        if (rx_frame->cmd == SLC_TEST_CMD_I2C_CFG) {
            slc_test_slave_iic_cfg(rx_frame);
        }

        if (rx_frame->cmd == SLC_TEST_CMD_I2C_S_TRX) {
            slc_hal_nop_delay_ms(SLC_TEST_CFG_TIMEOUT_MS);    // 确保fifo数据都发完
            if (iic_dir == 0) {
                g_iic_slave_rx_len = SLC_IIC_TEST_TRX_LEN;
                slc_hal_iic_master_recv(g_iic_slave_id, (uint8_t *)g_iic_slave_rx_buf, (uint32_t *)&g_iic_slave_rx_len, HAL_IIC_TIMEOUT);
            } else {
                slc_hal_iic_master_send(g_iic_slave_id, (uint8_t *)g_iic_slave_rx_buf, g_iic_slave_rx_len, HAL_IIC_TIMEOUT);
            }
            PRINTF("iic master %s len: %u\r\n", (iic_dir == 0) ? "rx" : "tx", g_iic_slave_rx_len);
        }

        if (rx_frame->cmd == SLC_TEST_CMD_SPI_CFG) {
            slc_test_slave_spi_cfg(rx_frame);
        }

        if (rx_frame->cmd == SLC_TEST_CMD_SPI_S_TRX) {
            slc_hal_nop_delay_ms(SLC_TEST_CFG_TIMEOUT_MS);    // 确保fifo数据都发完
            if (iic_dir == 0) {
                g_spi_slave_rx_len = SLC_SPI_TEST_TRX_LEN;
                slc_hal_spi_master_receive(g_spi_slave_id, (uint8_t *)g_spi_slave_rx_buf, g_spi_slave_rx_len, HAL_SPI_TIMEOUT_US);
            } else {
                slc_hal_spi_master_transmit(g_spi_slave_id, (void *)g_spi_slave_rx_buf, g_spi_slave_rx_len, HAL_SPI_TIMEOUT_US);
            }
            PRINTF("spi%d master %s len: %u\r\n", g_spi_slave_id, (iic_dir == 0) ? "rx" : "tx", g_spi_slave_rx_len);
            
        }
    }
}
