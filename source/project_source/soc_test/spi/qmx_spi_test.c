#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "reg_Dev.h"
#include "error_def.h"
#include "debug.h"
#include "utility.h"
#include "app_cfg.h"
#include "qmx_spi_test.h"
#include "qmx_hal_sysctrl.h"
#include "qmx_hal_spi.h"
#include "qmx_hal_gpio.h"
#include "qmx_hal_intc.h"
#include "qmx_hal_delay.h"
#include "qmx_hal_rng.h"
#include "qmx_test_master.h"

#define SPI_TEST_START_ID   HAL_SPI1

void qmx_spi_test_init(void)
{
    qmx_hal_sysctrl_peripheral_clk_enable(HAL_CLK_RAND, true);
    qmx_hal_sysctrl_peripheral_mod_reset(HAL_CLK_RAND);

    hal_rng_init_t rng_init = {0};
    rng_init.seed = 0x12345678;
    qmx_hal_rng_init(&rng_init);

    qmx_test_common_init();

    qmx_hal_gpio_set_iomux(HAL_GPIO_PIN4, HAL_IOMUX_MODE2); // SPI1 CS
    qmx_hal_gpio_set_iomux(HAL_GPIO_PIN5, HAL_IOMUX_MODE2); // SPI1 CLK
    qmx_hal_gpio_set_iomux(HAL_GPIO_PIN6, HAL_IOMUX_MODE2); // SPI1 MOSI
    qmx_hal_gpio_set_iomux(HAL_GPIO_PIN7, HAL_IOMUX_MODE2); // SPI1 MISO
    qmx_hal_gpio_set_mode(HAL_GPIO_PIN4, HAL_GPIO_PULL_UP);
    qmx_hal_gpio_set_mode(HAL_GPIO_PIN5, HAL_GPIO_PULL_UP);
    qmx_hal_gpio_set_mode(HAL_GPIO_PIN6, HAL_GPIO_PULL_UP);
    qmx_hal_gpio_set_mode(HAL_GPIO_PIN7, HAL_GPIO_PULL_UP);

    qmx_hal_gpio_set_iomux(HAL_GPIO_PIN8, HAL_IOMUX_MODE2); // SPI0 CS
    qmx_hal_gpio_set_iomux(HAL_GPIO_PIN9, HAL_IOMUX_MODE2); // SPI0 CLK
    qmx_hal_gpio_set_iomux(HAL_GPIO_PIN10, HAL_IOMUX_MODE2); // SPI0 MOSI
    qmx_hal_gpio_set_iomux(HAL_GPIO_PIN11, HAL_IOMUX_MODE2); // SPI0 MISO
    qmx_hal_gpio_set_mode(HAL_GPIO_PIN8, HAL_GPIO_PULL_UP);
    qmx_hal_gpio_set_mode(HAL_GPIO_PIN9, HAL_GPIO_PULL_UP);
    qmx_hal_gpio_set_mode(HAL_GPIO_PIN10, HAL_GPIO_PULL_UP);
    qmx_hal_gpio_set_mode(HAL_GPIO_PIN11, HAL_GPIO_PULL_UP);
}

void qmx_spi_test_deinit(void)
{
    qmx_hal_rng_deinit();
    qmx_hal_sysctrl_peripheral_clk_enable(HAL_CLK_RAND, false);
    qmx_hal_sysctrl_peripheral_mod_reset(HAL_CLK_RAND);

    qmx_test_common_deinit();

    qmx_hal_gpio_set_iomux(HAL_GPIO_PIN4, HAL_IOMUX_GPIO); // SPI1 CS
    qmx_hal_gpio_set_iomux(HAL_GPIO_PIN5, HAL_IOMUX_GPIO); // SPI1 CLK
    qmx_hal_gpio_set_iomux(HAL_GPIO_PIN6, HAL_IOMUX_GPIO); // SPI1 MOSI
    qmx_hal_gpio_set_iomux(HAL_GPIO_PIN7, HAL_IOMUX_GPIO); // SPI1 MISO
    qmx_hal_gpio_set_mode(HAL_GPIO_PIN4, HAL_GPIO_PULL_NONE);
    qmx_hal_gpio_set_mode(HAL_GPIO_PIN5, HAL_GPIO_PULL_NONE);
    qmx_hal_gpio_set_mode(HAL_GPIO_PIN6, HAL_GPIO_PULL_NONE);
    qmx_hal_gpio_set_mode(HAL_GPIO_PIN7, HAL_GPIO_PULL_NONE);

    qmx_hal_gpio_set_iomux(HAL_GPIO_PIN8, HAL_IOMUX_GPIO); // SPI0 CS
    qmx_hal_gpio_set_iomux(HAL_GPIO_PIN9, HAL_IOMUX_GPIO); // SPI0 CLK
    qmx_hal_gpio_set_iomux(HAL_GPIO_PIN10, HAL_IOMUX_GPIO); // SPI0 MOSI
    qmx_hal_gpio_set_iomux(HAL_GPIO_PIN11, HAL_IOMUX_GPIO); // SPI0 MISO
    qmx_hal_gpio_set_mode(HAL_GPIO_PIN8, HAL_GPIO_PULL_NONE);
    qmx_hal_gpio_set_mode(HAL_GPIO_PIN9, HAL_GPIO_PULL_NONE);
    qmx_hal_gpio_set_mode(HAL_GPIO_PIN10, HAL_GPIO_PULL_NONE);
    qmx_hal_gpio_set_mode(HAL_GPIO_PIN11, HAL_GPIO_PULL_NONE);
}

int qmx_spi_test_master_trx_common(hal_spi_id_e spi_id)
{
    uint8_t tx_data[QMX_SPI_TEST_TRX_LEN] = {0};
    uint8_t rx_data[QMX_SPI_TEST_TRX_LEN] = {0};
    uint8_t cmd[4] = {0};
    uint32_t rx_len = 0;
    int ret = 0;

    ret = qmx_hal_get_random_u8(tx_data, QMX_SPI_TEST_TRX_LEN, 1000);
    if (ret != 0) {
        PRINTF("get random u8 error, ret=0x%X\n", ret);
        return ret;
    }

    cmd[1] = 0;
    cmd[2] = 0;
    cmd[3] = 0;

    // 1、master先发送数据给slave，slave端已提前准备好，在中断内接收数据
    cmd[0] = QMX_SPI_TEST_TX_CMD;
    qmx_hal_spi_master_transmit(spi_id, cmd, 4, HAL_SPI_TIMEOUT_US);
    qmx_hal_nop_delay_ms(5);    // 确保slave端已经准备好
    qmx_hal_spi_master_transmit(spi_id, tx_data, QMX_SPI_TEST_TRX_LEN, HAL_SPI_TIMEOUT_US);
    // 2、确保slave端处理完数据
    qmx_hal_nop_delay_ms(2000);

    // 3、master再接收数据, slave端在中断内将接收数据原路返回
    cmd[0] = QMX_SPI_TEST_RX_CMD;
    qmx_hal_spi_master_transmit(spi_id, cmd, 4, HAL_SPI_TIMEOUT_US);

    qmx_hal_nop_delay_ms(5);    // 确保slave端已经准备好数据
    rx_len = QMX_SPI_TEST_TRX_LEN;
    ret = qmx_hal_spi_master_receive(spi_id, rx_data, rx_len, HAL_SPI_TIMEOUT_US);

    // 4、比较接收长度和数据是否一致
    if (ret != 0) {
        PRINTF("SPI%d master rx data fail: %d\n", spi_id, ret);
        return -1;
    }

    if (memcmp(rx_data, tx_data, QMX_SPI_TEST_TRX_LEN) != 0) {
        PRINTF("SPI%d master trx data is different.\n", spi_id);
        dump_u8buf("tx", tx_data, QMX_SPI_TEST_TRX_LEN);
        dump_u8buf("rx", rx_data, QMX_SPI_TEST_TRX_LEN);
        return -1;
    }

    // 确保slave端处理完数据
    qmx_hal_nop_delay_ms(QMX_TEST_CFG_TIMEOUT_MS);

    return 0;
}

int qmx_spi_master_div_test(void)
{
    int i, ret;
    hal_spi_cfg_t config = {0};
    uint8_t tx_data[QMX_TEST_RX_MAX_LEN] = {0};
    uint8_t rx_data[QMX_TEST_RX_MAX_LEN] = {0};
    uint8_t rx_len = 0;
    hal_spi_id_e test_id = SPI_TEST_START_ID;

    config.mode = HAL_SPI_MASTER;
    config.polarity_phase = HAL_SPI_CPOL0_CPHA0;
    config.data_mode = HAL_SPI_DATA_MSB;
    config.data_len = HAL_SPI_DATA_LEN_8BIT;
    config.cs_holding_time = 0x8;
    config.clk_adjust_en = true;
    config.sw_cs_en = false;
    config.anti_noise_level = 1;
    config.cs_gap_time = 0x8;
    config.tx_fifo_pfull_th = 12;
    config.rx_fifo_pfull_th = 12;
    config.rx_fifo_pempty_th = 4;
    config.tx_fifo_pempty_th = 4;

    tx_data[0] = config.mode;
    tx_data[2] = config.polarity_phase;
    tx_data[3] = config.data_mode;
    tx_data[4] = config.data_len;
    tx_data[5] = config.cs_holding_time;
    tx_data[6] = config.cs_gap_time;
    tx_data[7] = config.clk_adjust_en;
    tx_data[8] = config.sw_cs_en;
    tx_data[9] = config.anti_noise_level;

start:
    tx_data[10] = test_id;
    qmx_test_master_send_cmd(QMX_TEST_CMD_SPI_CFG, tx_data, 11);
    ret = qmx_test_master_get_result(rx_data, &rx_len);
    if ((ret != 0) || (rx_data[0] != 0)) {
        PRINTF("SPI%u slave cfg failed.\n", test_id);
        return -1;
    } else {
        PRINTF("SPI%u slave cfg success.\n", test_id);
    }

    // slave端最大输入时钟只有6.25MHz
    for (i = HAL_SPI_DIV_8; i < HAL_SPI_DIV_MAX; i++) {
        rx_len = 0;
        memset(rx_data, 0, QMX_TEST_RX_MAX_LEN);

        config.clk_div = (hal_spi_clk_div_e)i;
        tx_data[1] = config.clk_div;

        qmx_hal_nop_delay_ms(QMX_TEST_CFG_TIMEOUT_MS);

        if (test_id == HAL_SPI0) {
            qmx_hal_sysctrl_peripheral_clk_enable(HAL_CLK_SPI0, true);
            qmx_hal_sysctrl_peripheral_mod_reset(HAL_CLK_SPI0);
        } else {
            qmx_hal_sysctrl_peripheral_clk_enable(HAL_CLK_SPI1, true);
            qmx_hal_sysctrl_peripheral_mod_reset(HAL_CLK_SPI1);
        }

        qmx_hal_spi_init(test_id, &config);

        ret = qmx_spi_test_master_trx_common(test_id);
        if (ret != 0) {
            PRINTF("SPI%u clk_div %u test failed.\n", test_id, config.clk_div);
            return -1;
        }
    }

    qmx_hal_nop_delay_ms(QMX_TEST_CFG_TIMEOUT_MS);

    // 遍历SPI0和SPI1
    if (test_id == HAL_SPI0) {
        test_id = HAL_SPI1;
        i = HAL_SPI_DIV_20;
        goto start;
    }

    PRINTF("SPI master clk div test passed.\n");
    return 0;
}

int qmx_spi_master_polarity_phase_test(void)
{
    int i, ret;
    hal_spi_cfg_t config = {0};
    uint8_t tx_data[QMX_TEST_RX_MAX_LEN] = {0};
    uint8_t rx_data[QMX_TEST_RX_MAX_LEN] = {0};
    uint8_t rx_len = 0;
    hal_spi_id_e test_id = SPI_TEST_START_ID;

    config.mode = HAL_SPI_MASTER;
    config.clk_div = HAL_SPI_DIV_20;
    config.data_mode = HAL_SPI_DATA_MSB;
    config.data_len = HAL_SPI_DATA_LEN_8BIT;
    config.cs_holding_time = 0x8;
    config.clk_adjust_en = true;
    config.sw_cs_en = false;
    config.anti_noise_level = 1;
    config.cs_gap_time = 0x8;
    config.tx_fifo_pfull_th = 12;
    config.rx_fifo_pfull_th = 12;
    config.rx_fifo_pempty_th = 4;
    config.tx_fifo_pempty_th = 4;

    tx_data[0] = config.mode;
    tx_data[1] = config.clk_div;
    tx_data[3] = config.data_mode;
    tx_data[4] = config.data_len;
    tx_data[5] = config.cs_holding_time;
    tx_data[6] = config.cs_gap_time;
    tx_data[7] = config.clk_adjust_en;
    tx_data[8] = config.sw_cs_en;
    tx_data[9] = config.anti_noise_level;

start:
    tx_data[10] = test_id;

    for (i = 0; i <= HAL_SPI_CPOL1_CPHA1; i++) {
        rx_len = 0;
        memset(rx_data, 0, QMX_TEST_RX_MAX_LEN);

        config.polarity_phase = (hal_spi_polarity_phase_e)i;
        tx_data[2] = config.polarity_phase;

        qmx_test_master_send_cmd(QMX_TEST_CMD_SPI_CFG, tx_data, 11);
        ret = qmx_test_master_get_result(rx_data, &rx_len);
        if ((ret != 0) || (rx_data[0] != 0)) {
            PRINTF("SPI%u slave cfg %d failed.\n", test_id, i);
            return -1;
        }

        qmx_hal_nop_delay_ms(QMX_TEST_CFG_TIMEOUT_MS);

        if (test_id == HAL_SPI0) {
            qmx_hal_sysctrl_peripheral_clk_enable(HAL_CLK_SPI0, true);
            qmx_hal_sysctrl_peripheral_mod_reset(HAL_CLK_SPI0);
        } else {
            qmx_hal_sysctrl_peripheral_clk_enable(HAL_CLK_SPI1, true);
            qmx_hal_sysctrl_peripheral_mod_reset(HAL_CLK_SPI1);
        }

        qmx_hal_spi_init(test_id, &config);

        ret = qmx_spi_test_master_trx_common(test_id);
        if (ret != 0) {
            PRINTF("SPI%u polarity_phase %u test failed.\n", test_id, config.polarity_phase);
            return -1;
        }
    }

    qmx_hal_nop_delay_ms(QMX_TEST_CFG_TIMEOUT_MS);

    // 遍历SPI0和SPI1
    if (test_id == HAL_SPI0) {
        test_id = HAL_SPI1;
        i = 0;
        goto start;
    }

    PRINTF("SPI master polarity_phase test passed.\n");
    return 0;
}

int qmx_spi_master_data_mode_test(void)
{
    int i, ret;
    hal_spi_cfg_t config = {0};
    uint8_t tx_data[QMX_TEST_RX_MAX_LEN] = {0};
    uint8_t rx_data[QMX_TEST_RX_MAX_LEN] = {0};
    uint8_t rx_len = 0;
    hal_spi_id_e test_id = SPI_TEST_START_ID;

    config.mode = HAL_SPI_MASTER;
    config.clk_div = HAL_SPI_DIV_20;
    config.polarity_phase = HAL_SPI_CPOL0_CPHA0;
    config.data_len = HAL_SPI_DATA_LEN_8BIT;
    config.cs_holding_time = 0x8;
    config.clk_adjust_en = true;
    config.sw_cs_en = false;
    config.anti_noise_level = 1;
    config.cs_gap_time = 0x8;
    config.tx_fifo_pfull_th = 12;
    config.rx_fifo_pfull_th = 12;
    config.rx_fifo_pempty_th = 4;
    config.tx_fifo_pempty_th = 4;

    tx_data[0] = config.mode;
    tx_data[1] = config.clk_div;
    tx_data[2] = config.polarity_phase;
    tx_data[4] = config.data_len;
    tx_data[5] = config.cs_holding_time;
    tx_data[6] = config.cs_gap_time;
    tx_data[7] = config.clk_adjust_en;
    tx_data[8] = config.sw_cs_en;
    tx_data[9] = config.anti_noise_level;

start:
    tx_data[10] = test_id;

    for (i = 0; i <= HAL_SPI_DATA_LSB; i++) {
        rx_len = 0;
        memset(rx_data, 0, QMX_TEST_RX_MAX_LEN);

        config.data_mode = (hal_spi_data_mode_e)i;
        tx_data[3] = config.data_mode;

        qmx_test_master_send_cmd(QMX_TEST_CMD_SPI_CFG, tx_data, 11);
        ret = qmx_test_master_get_result(rx_data, &rx_len);
        if ((ret != 0) || (rx_data[0] != 0)) {
            PRINTF("SPI%u slave cfg failed.\n", test_id);
            return -1;
        }

        qmx_hal_nop_delay_ms(QMX_TEST_CFG_TIMEOUT_MS);

        if (test_id == HAL_SPI0) {
            qmx_hal_sysctrl_peripheral_clk_enable(HAL_CLK_SPI0, true);
            qmx_hal_sysctrl_peripheral_mod_reset(HAL_CLK_SPI0);
        } else {
            qmx_hal_sysctrl_peripheral_clk_enable(HAL_CLK_SPI1, true);
            qmx_hal_sysctrl_peripheral_mod_reset(HAL_CLK_SPI1);
        }

        qmx_hal_spi_init(test_id, &config);

        ret = qmx_spi_test_master_trx_common(test_id);
        if (ret != 0) {
            PRINTF("SPI%u data_mode %u test failed.\n", test_id, config.data_mode);
            return -1;
        }
    }

    qmx_hal_nop_delay_ms(QMX_TEST_CFG_TIMEOUT_MS);

    // 遍历SPI0和SPI1
    if (test_id == HAL_SPI0) {
        test_id = HAL_SPI1;
        i = 0;
        goto start;
    }

    PRINTF("SPI master data_mode test passed.\n");
    return 0;
}

int qmx_spi_master_data_len_test(void)
{
    int i, ret;
    hal_spi_cfg_t config = {0};
    uint8_t tx_data[QMX_TEST_RX_MAX_LEN] = {0};
    uint8_t rx_data[QMX_TEST_RX_MAX_LEN] = {0};
    uint8_t rx_len = 0;
    hal_spi_id_e test_id = SPI_TEST_START_ID;

    config.mode = HAL_SPI_MASTER;
    config.clk_div = HAL_SPI_DIV_20;
    config.polarity_phase = HAL_SPI_CPOL0_CPHA0;
    config.data_mode = HAL_SPI_DATA_MSB;
    config.cs_holding_time = 0x8;
    config.clk_adjust_en = true;
    config.sw_cs_en = false;
    config.anti_noise_level = 1;
    config.cs_gap_time = 0x8;
    config.tx_fifo_pfull_th = 12;
    config.rx_fifo_pfull_th = 12;
    config.rx_fifo_pempty_th = 4;
    config.tx_fifo_pempty_th = 4;

    tx_data[0] = config.mode;
    tx_data[1] = config.clk_div;
    tx_data[2] = config.polarity_phase;
    tx_data[3] = config.data_mode;
    tx_data[5] = config.cs_holding_time;
    tx_data[6] = config.cs_gap_time;
    tx_data[7] = config.clk_adjust_en;
    tx_data[8] = config.sw_cs_en;
    tx_data[9] = config.anti_noise_level;

start:
    tx_data[10] = test_id;

    for (i = 0; i < HAL_SPI_DATA_LEN_MAX; i++) {
        rx_len = 0;
        memset(rx_data, 0, QMX_TEST_RX_MAX_LEN);

        config.data_len = (hal_spi_data_len_e)i;
        tx_data[4] = config.data_len;

        qmx_test_master_send_cmd(QMX_TEST_CMD_SPI_CFG, tx_data, 11);
        ret = qmx_test_master_get_result(rx_data, &rx_len);
        if ((ret != 0) || (rx_data[0] != 0)) {
            PRINTF("SPI%u slave cfg failed.\n", test_id);
            return -1;
        }

        qmx_hal_nop_delay_ms(QMX_TEST_CFG_TIMEOUT_MS);

        if (test_id == HAL_SPI0) {
            qmx_hal_sysctrl_peripheral_clk_enable(HAL_CLK_SPI0, true);
            qmx_hal_sysctrl_peripheral_mod_reset(HAL_CLK_SPI0);
        } else {
            qmx_hal_sysctrl_peripheral_clk_enable(HAL_CLK_SPI1, true);
            qmx_hal_sysctrl_peripheral_mod_reset(HAL_CLK_SPI1);
        }

        qmx_hal_spi_init(test_id, &config);

        ret = qmx_spi_test_master_trx_common(test_id);
        if (ret != 0) {
            PRINTF("SPI%u data_len %u test failed.\n", test_id, config.data_len);
            return -1;
        }
    }

    qmx_hal_nop_delay_ms(QMX_TEST_CFG_TIMEOUT_MS);

    // 遍历SPI0和SPI1
    if (test_id == HAL_SPI0) {
        test_id = HAL_SPI1;
        i = 0;
        goto start;
    }

    PRINTF("SPI master data_len test passed.\n");
    return 0;
}

/* salve test */
int qmx_spi_test_slv_trx_common(hal_spi_id_e spi_id)
{
    uint8_t tx_data[QMX_TEST_RX_MAX_LEN] = {0};
    uint8_t rx_data[QMX_TEST_RX_MAX_LEN] = {0};
    uint8_t tx_buf[QMX_SPI_TEST_TRX_LEN] = {0};
    uint8_t rx_buf[QMX_SPI_TEST_TRX_LEN] = {0};
    uint32_t rx_len = 0;
    int ret = 0;

    ret = qmx_hal_get_random_u8(tx_buf, QMX_SPI_TEST_TRX_LEN, 1000);
    if (ret != 0) {
        PRINTF("get random u8 error, ret=0x%X\n", ret);
        return ret;
    }

    tx_data[0] = spi_id;
    tx_data[1] = 0; // 对端是master，读数据
    // 1、master通知slave读取准备好的数据，然后slave将数据返回
    qmx_test_master_send_cmd(QMX_TEST_CMD_SPI_S_TRX, tx_data, 2);
    ret = qmx_test_master_get_result(rx_data, (uint8_t *)&rx_len);
    if ((ret != 0) || (rx_data[0] != 0)) {
        PRINTF("SPI%d slave tx failed.\n", spi_id);
        return -1;
    }
    memset(rx_data, 0, QMX_TEST_RX_MAX_LEN);

    // 2、确保slave端接收完数据
    qmx_hal_spi_slave_transmit(spi_id, tx_buf, QMX_SPI_TEST_TRX_LEN, HAL_SPI_TIMEOUT_US);

    // 3、master再接收数据, slave端将接收数据原路返回
    tx_data[1] = 1; // 对端是master，发数据
    qmx_test_master_send_cmd(QMX_TEST_CMD_SPI_S_TRX, tx_data, 2);
    ret = qmx_test_master_get_result(rx_data, (uint8_t *)&rx_len);
    if ((ret != 0) || (rx_data[0] != 0)) {
        PRINTF("SPI%d slave rx failed.\n", spi_id);
        return -1;
    }

    // 4、确保slave端发完数据
    rx_len = QMX_SPI_TEST_TRX_LEN;
    qmx_hal_spi_slave_receive(spi_id, rx_buf, &rx_len, HAL_SPI_TIMEOUT_US);

    // 5、比较接收长度和数据是否一致
    if ((rx_len != QMX_SPI_TEST_TRX_LEN)) {
        PRINTF("SPI%d slv trx data len: %u != %u\n", spi_id, rx_len, QMX_SPI_TEST_TRX_LEN);
        if (rx_len != 0) {
            dump_u8buf("tx", tx_buf, QMX_SPI_TEST_TRX_LEN);
            dump_u8buf("rx", rx_buf, QMX_SPI_TEST_TRX_LEN);
        }
        return -1;
    }

    if (memcmp(tx_buf, rx_buf, QMX_SPI_TEST_TRX_LEN) != 0) {
        PRINTF("SPI%d slv trx data is different.\n", spi_id);
        dump_u8buf("tx", tx_buf, QMX_SPI_TEST_TRX_LEN);
        dump_u8buf("rx", rx_buf, QMX_SPI_TEST_TRX_LEN);
        return -1;
    }

    return 0;
}

int qmx_spi_salve_polarity_phase_test(void)
{
    int i, ret;
    hal_spi_cfg_t config = {0};
    uint8_t tx_data[QMX_TEST_RX_MAX_LEN] = {0};
    uint8_t rx_data[QMX_TEST_RX_MAX_LEN] = {0};
    uint8_t rx_len = 0;
    hal_spi_id_e test_id = SPI_TEST_START_ID;

    config.mode = HAL_SPI_SLAVE;
    config.clk_div = HAL_SPI_DIV_20;
    config.data_mode = HAL_SPI_DATA_MSB;
    config.data_len = HAL_SPI_DATA_LEN_8BIT;
    config.cs_holding_time = 0x8;
    config.clk_adjust_en = true;
    config.sw_cs_en = false;
    config.anti_noise_level = 1;
    config.cs_gap_time = 0x8;
    config.tx_fifo_pfull_th = 12;
    config.rx_fifo_pfull_th = 12;
    config.rx_fifo_pempty_th = 4;
    config.tx_fifo_pempty_th = 4;

    tx_data[0] = config.mode;
    tx_data[1] = config.clk_div;
    tx_data[3] = config.data_mode;
    tx_data[4] = config.data_len;
    tx_data[5] = config.cs_holding_time;
    tx_data[6] = config.cs_gap_time;
    tx_data[7] = config.clk_adjust_en;
    tx_data[8] = config.sw_cs_en;
    tx_data[9] = config.anti_noise_level;

start:
    tx_data[10] = test_id;

    for (i = 0; i <= HAL_SPI_CPOL1_CPHA1; i++) {
        rx_len = 0;
        memset(rx_data, 0, QMX_TEST_RX_MAX_LEN);

        config.polarity_phase = (hal_spi_polarity_phase_e)i;
        tx_data[2] = config.polarity_phase;

        qmx_test_master_send_cmd(QMX_TEST_CMD_SPI_CFG, tx_data, 11);
        ret = qmx_test_master_get_result(rx_data, &rx_len);
        if ((ret != 0) || (rx_data[0] != 0)) {
            PRINTF("SPI%u master cfg failed.\n", test_id);
            return -1;
        }

        qmx_hal_nop_delay_ms(QMX_TEST_CFG_TIMEOUT_MS);

        if (test_id == HAL_SPI0) {
            qmx_hal_sysctrl_peripheral_clk_enable(HAL_CLK_SPI0, true);
            qmx_hal_sysctrl_peripheral_mod_reset(HAL_CLK_SPI0);
        } else {
            qmx_hal_sysctrl_peripheral_clk_enable(HAL_CLK_SPI1, true);
            qmx_hal_sysctrl_peripheral_mod_reset(HAL_CLK_SPI1);
        }

        qmx_hal_spi_init(test_id, &config);

        ret = qmx_spi_test_slv_trx_common(test_id);
        if (ret != 0) {
            PRINTF("SPI%u polarity_phase %u test failed.\n", test_id, config.polarity_phase);
            return -1;
        }
    }

    qmx_hal_nop_delay_ms(QMX_TEST_CFG_TIMEOUT_MS);

    // 遍历SPI0和SPI1
    if (test_id == HAL_SPI0) {
        test_id = HAL_SPI1;
        i = 0;
        goto start;
    }

    PRINTF("SPI master polarity_phase test passed.\n");
    return 0;
}

int qmx_spi_slave_data_mode_test(void)
{
    int i, ret;
    hal_spi_cfg_t config = {0};
    uint8_t tx_data[QMX_TEST_RX_MAX_LEN] = {0};
    uint8_t rx_data[QMX_TEST_RX_MAX_LEN] = {0};
    uint8_t rx_len = 0;
    hal_spi_id_e test_id = SPI_TEST_START_ID;

    config.mode = HAL_SPI_SLAVE;
    config.clk_div = HAL_SPI_DIV_20;
    config.polarity_phase = HAL_SPI_CPOL0_CPHA0;
    config.data_len = HAL_SPI_DATA_LEN_8BIT;
    config.cs_holding_time = 0x8;
    config.clk_adjust_en = true;
    config.sw_cs_en = false;
    config.anti_noise_level = 1;
    config.cs_gap_time = 0x8;
    config.tx_fifo_pfull_th = 12;
    config.rx_fifo_pfull_th = 12;
    config.rx_fifo_pempty_th = 4;
    config.tx_fifo_pempty_th = 4;

    tx_data[0] = config.mode;
    tx_data[1] = config.clk_div;
    tx_data[2] = config.polarity_phase;
    tx_data[4] = config.data_len;
    tx_data[5] = config.cs_holding_time;
    tx_data[6] = config.cs_gap_time;
    tx_data[7] = config.clk_adjust_en;
    tx_data[8] = config.sw_cs_en;
    tx_data[9] = config.anti_noise_level;

start:
    tx_data[10] = test_id;

    for (i = 0; i <= HAL_SPI_DATA_LSB; i++) {
        rx_len = 0;
        memset(rx_data, 0, QMX_TEST_RX_MAX_LEN);

        config.data_mode = (hal_spi_data_mode_e)i;
        tx_data[3] = config.data_mode;

        qmx_test_master_send_cmd(QMX_TEST_CMD_SPI_CFG, tx_data, 11);
        ret = qmx_test_master_get_result(rx_data, &rx_len);
        if ((ret != 0) || (rx_data[0] != 0)) {
            PRINTF("SPI%u master cfg failed.\n", test_id);
            return -1;
        }

        qmx_hal_nop_delay_ms(QMX_TEST_CFG_TIMEOUT_MS);

        if (test_id == HAL_SPI0) {
            qmx_hal_sysctrl_peripheral_clk_enable(HAL_CLK_SPI0, true);
            qmx_hal_sysctrl_peripheral_mod_reset(HAL_CLK_SPI0);
        } else {
            qmx_hal_sysctrl_peripheral_clk_enable(HAL_CLK_SPI1, true);
            qmx_hal_sysctrl_peripheral_mod_reset(HAL_CLK_SPI1);
        }

        qmx_hal_spi_init(test_id, &config);

        ret = qmx_spi_test_slv_trx_common(test_id);
        if (ret != 0) {
            PRINTF("SPI%u data_mode %u test failed.\n", test_id, config.data_mode);
            return -1;
        }
    }

    qmx_hal_nop_delay_ms(QMX_TEST_CFG_TIMEOUT_MS);

    // 遍历SPI0和SPI1
    if (test_id == HAL_SPI0) {
        test_id = HAL_SPI1;
        i = 0;
        goto start;
    }

    PRINTF("SPI slave data_mode test passed.\n");
    return 0;
}

int qmx_spi_slave_data_len_test(void)
{
    int i, ret;
    hal_spi_cfg_t config = {0};
    uint8_t tx_data[QMX_TEST_RX_MAX_LEN] = {0};
    uint8_t rx_data[QMX_TEST_RX_MAX_LEN] = {0};
    uint8_t rx_len = 0;
    hal_spi_id_e test_id = SPI_TEST_START_ID;

    config.mode = HAL_SPI_SLAVE;
    config.clk_div = HAL_SPI_DIV_20;
    config.polarity_phase = HAL_SPI_CPOL0_CPHA0;
    config.data_mode = HAL_SPI_DATA_MSB;
    config.cs_holding_time = 0x8;
    config.clk_adjust_en = true;
    config.sw_cs_en = false;
    config.anti_noise_level = 1;
    config.cs_gap_time = 8;
    config.tx_fifo_pfull_th = 12;
    config.rx_fifo_pfull_th = 12;
    config.rx_fifo_pempty_th = 4;
    config.tx_fifo_pempty_th = 4;

    tx_data[0] = config.mode;
    tx_data[1] = config.clk_div;
    tx_data[2] = config.polarity_phase;
    tx_data[3] = config.data_mode;
    tx_data[5] = config.cs_holding_time;
    tx_data[6] = config.cs_gap_time;
    tx_data[7] = config.clk_adjust_en;
    tx_data[8] = config.sw_cs_en;
    tx_data[9] = config.anti_noise_level;

start:
    tx_data[10] = test_id;

    for (i = 0; i < HAL_SPI_DATA_LEN_MAX; i++) {
        rx_len = 0;
        memset(rx_data, 0, QMX_TEST_RX_MAX_LEN);

        config.data_len = (hal_spi_data_len_e)i;
        tx_data[4] = config.data_len;

        qmx_test_master_send_cmd(QMX_TEST_CMD_SPI_CFG, tx_data, 11);
        ret = qmx_test_master_get_result(rx_data, &rx_len);
        if ((ret != 0) || (rx_data[0] != 0)) {
            PRINTF("SPI%u master cfg failed.\n", test_id);
            return -1;
        }

        qmx_hal_nop_delay_ms(QMX_TEST_CFG_TIMEOUT_MS);

        if (test_id == HAL_SPI0) {
            qmx_hal_sysctrl_peripheral_clk_enable(HAL_CLK_SPI0, true);
            qmx_hal_sysctrl_peripheral_mod_reset(HAL_CLK_SPI0);
        } else {
            qmx_hal_sysctrl_peripheral_clk_enable(HAL_CLK_SPI1, true);
            qmx_hal_sysctrl_peripheral_mod_reset(HAL_CLK_SPI1);
        }

        qmx_hal_spi_init(test_id, &config);

        ret = qmx_spi_test_slv_trx_common(test_id);
        if (ret != 0) {
            PRINTF("SPI%u data_len %u test failed.\n", test_id, config.data_len);
            return -1;
        }
    }

    qmx_hal_nop_delay_ms(QMX_TEST_CFG_TIMEOUT_MS);

    // 遍历SPI0和SPI1
    if (test_id == HAL_SPI0) {
        test_id = HAL_SPI1;
        i = 0;
        goto start;
    }

    PRINTF("SPI slave data_len test passed.\n");
    return 0;
}
