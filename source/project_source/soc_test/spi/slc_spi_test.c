#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "reg_Dev.h"
#include "error_def.h"
#include "debug.h"
#include "utility.h"
#include "app_cfg.h"
#include "slc_spi_test.h"
#include "slc_hal_sysctrl.h"
#include "slc_hal_spi.h"
#include "slc_hal_gpio.h"
#include "slc_hal_intc.h"
#include "slc_hal_delay.h"
#include "slc_hal_rng.h"
#include "slc_test_master.h"

#define SPI_TEST_START_ID   HAL_SPI0

void print_reg_4byte_from_to(uint32_t start_addr, uint32_t end_addr)
{
    if (start_addr > end_addr) {
        PRINTF("ADDR ERROR\n");
        return;
    }

    uint32_t total_bytes = end_addr - start_addr + 1;

    uint32_t count = total_bytes / 4;

    uint32_t *addr = (uint32_t *)start_addr;

    for (uint32_t i = 0; i < count; i++) {
        PRINTF("ADDR 0x%08X: 0x%08X\n",
               (uint32_t)&addr[i],
               addr[i]);
    }
}

void slc_spi_test_init(void)
{
    slc_hal_sysctrl_peripheral_clk_enable(HAL_CLK_RAND, true);
    slc_hal_sysctrl_peripheral_mod_reset(HAL_CLK_RAND);

    hal_rng_init_t rng_init = {0};
    rng_init.seed = 0x12345678;
    slc_hal_rng_init(&rng_init);

    slc_test_common_init();

    slc_hal_gpio_set_iomux(HAL_GPIO_PIN4, HAL_IOMUX_MODE2); // SPI1 CS
    slc_hal_gpio_set_iomux(HAL_GPIO_PIN5, HAL_IOMUX_MODE2); // SPI1 CLK
    slc_hal_gpio_set_iomux(HAL_GPIO_PIN6, HAL_IOMUX_MODE2); // SPI1 MOSI
    slc_hal_gpio_set_iomux(HAL_GPIO_PIN7, HAL_IOMUX_MODE2); // SPI1 MISO
    slc_hal_gpio_set_mode(HAL_GPIO_PIN4, HAL_GPIO_PULL_UP);
    slc_hal_gpio_set_mode(HAL_GPIO_PIN5, HAL_GPIO_PULL_UP);
    slc_hal_gpio_set_mode(HAL_GPIO_PIN6, HAL_GPIO_PULL_UP);
    slc_hal_gpio_set_mode(HAL_GPIO_PIN7, HAL_GPIO_PULL_UP);

    slc_hal_gpio_set_iomux(HAL_GPIO_PIN8, HAL_IOMUX_MODE2); // SPI0 CS
    slc_hal_gpio_set_iomux(HAL_GPIO_PIN9, HAL_IOMUX_MODE2); // SPI0 CLK
    slc_hal_gpio_set_iomux(HAL_GPIO_PIN10, HAL_IOMUX_MODE2); // SPI0 MOSI
    slc_hal_gpio_set_iomux(HAL_GPIO_PIN11, HAL_IOMUX_MODE2); // SPI0 MISO
    slc_hal_gpio_set_mode(HAL_GPIO_PIN8, HAL_GPIO_PULL_UP);
    slc_hal_gpio_set_mode(HAL_GPIO_PIN9, HAL_GPIO_PULL_UP);
    slc_hal_gpio_set_mode(HAL_GPIO_PIN10, HAL_GPIO_PULL_UP);
    slc_hal_gpio_set_mode(HAL_GPIO_PIN11, HAL_GPIO_PULL_UP);
}

void slc_spi_test_deinit(void)
{
    slc_hal_rng_deinit();
    slc_hal_sysctrl_peripheral_clk_enable(HAL_CLK_RAND, false);
    slc_hal_sysctrl_peripheral_mod_reset(HAL_CLK_RAND);

    slc_test_common_deinit();

    slc_hal_gpio_set_iomux(HAL_GPIO_PIN4, HAL_IOMUX_GPIO); // SPI1 CS
    slc_hal_gpio_set_iomux(HAL_GPIO_PIN5, HAL_IOMUX_GPIO); // SPI1 CLK
    slc_hal_gpio_set_iomux(HAL_GPIO_PIN6, HAL_IOMUX_GPIO); // SPI1 MOSI
    slc_hal_gpio_set_iomux(HAL_GPIO_PIN7, HAL_IOMUX_GPIO); // SPI1 MISO
    slc_hal_gpio_set_mode(HAL_GPIO_PIN4, HAL_GPIO_PULL_NONE);
    slc_hal_gpio_set_mode(HAL_GPIO_PIN5, HAL_GPIO_PULL_NONE);
    slc_hal_gpio_set_mode(HAL_GPIO_PIN6, HAL_GPIO_PULL_NONE);
    slc_hal_gpio_set_mode(HAL_GPIO_PIN7, HAL_GPIO_PULL_NONE);

    slc_hal_gpio_set_iomux(HAL_GPIO_PIN8, HAL_IOMUX_GPIO); // SPI0 CS
    slc_hal_gpio_set_iomux(HAL_GPIO_PIN9, HAL_IOMUX_GPIO); // SPI0 CLK
    slc_hal_gpio_set_iomux(HAL_GPIO_PIN10, HAL_IOMUX_GPIO); // SPI0 MOSI
    slc_hal_gpio_set_iomux(HAL_GPIO_PIN11, HAL_IOMUX_GPIO); // SPI0 MISO
    slc_hal_gpio_set_mode(HAL_GPIO_PIN8, HAL_GPIO_PULL_NONE);
    slc_hal_gpio_set_mode(HAL_GPIO_PIN9, HAL_GPIO_PULL_NONE);
    slc_hal_gpio_set_mode(HAL_GPIO_PIN10, HAL_GPIO_PULL_NONE);
    slc_hal_gpio_set_mode(HAL_GPIO_PIN11, HAL_GPIO_PULL_NONE);
}

int slc_spi_test_master_trx_common(hal_spi_id_e spi_id)
{
    uint8_t tx_data[SLC_SPI_TEST_TRX_LEN] = {0};
    uint8_t tx_data_test[SLC_SPI_TEST_TRX_LEN] = {0x4D,0x0F,0x23,0x1C,0x0C,0x74,0xF4,0x1D,0xB0,0x18,0x87,0xA2,0xD4,0x35,0x9E,0xAC};
    uint8_t rx_data[SLC_SPI_TEST_TRX_LEN] = {0};
    uint8_t cmd[4] = {0};
    uint32_t rx_len = 0;
    int ret = 0;

    ret = slc_hal_get_random_u8(tx_data, SLC_SPI_TEST_TRX_LEN, 1000);
    if (ret != 0) {
        PRINTF("get random u8 error, ret=0x%X\n", ret);
        return ret;
    }

    cmd[1] = 0;
    cmd[2] = 0;
    cmd[3] = 0;
    // 1、master先发送数据给slave，slave端已提前准备好，在中断内接收数据
    cmd[0] = SLC_SPI_TEST_TX_CMD;
    slc_hal_spi_master_transmit(spi_id, cmd, 4, HAL_SPI_TIMEOUT_US);
    slc_hal_nop_delay_ms(5);    // 确保slave端已经准备好
    slc_hal_spi_master_transmit(spi_id, tx_data, SLC_SPI_TEST_TRX_LEN, HAL_SPI_TIMEOUT_US);
    // 2、确保slave端处理完数据
    slc_hal_nop_delay_ms(2000);
    
    // 3、master再接收数据, slave端在中断内将接收数据原路返回
    cmd[0] = SLC_SPI_TEST_RX_CMD;
    slc_hal_spi_master_transmit(spi_id, cmd, 4, HAL_SPI_TIMEOUT_US);

    slc_hal_nop_delay_ms(5);    // 确保slave端已经准备好数据
    rx_len = SLC_SPI_TEST_TRX_LEN;
    ret = slc_hal_spi_master_receive(spi_id, rx_data, rx_len, HAL_SPI_TIMEOUT_US);

    // 4、比较接收长度和数据是否一致
    if (ret != 0) {
        PRINTF("SPI%d master rx data fail: %d\n", spi_id, ret);
        return -1;
    }

    if (memcmp(rx_data, tx_data, SLC_SPI_TEST_TRX_LEN) != 0) {
        PRINTF("SPI%d master trx data is different.\n", spi_id);
        dump_u8buf("tx", tx_data, SLC_SPI_TEST_TRX_LEN);
        dump_u8buf("rx", rx_data, SLC_SPI_TEST_TRX_LEN);
        return -1;
    }

    // 确保slave端处理完数据
    slc_hal_nop_delay_ms(SLC_TEST_CFG_TIMEOUT_MS);

    return 0;
}

int slc_spi_master_div_test(void)
{
    int i, ret;
    hal_spi_cfg_t config = {0};
    uint8_t tx_data[SLC_TEST_RX_MAX_LEN] = {0};
    uint8_t rx_data[SLC_TEST_RX_MAX_LEN] = {0};
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
    config.cs_gap_time = 0x30;
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
    slc_test_master_send_cmd(SLC_TEST_CMD_SPI_CFG, tx_data, 11);
    ret = slc_test_master_get_result(rx_data, &rx_len);
    if ((ret != 0) || (rx_data[0] != 0)) {
        PRINTF("SPI%u slave cfg failed.\n", test_id);
        return -1;
    } else {
        PRINTF("SPI%u slave cfg success.\n", test_id);
    }
    slc_hal_nop_delay_ms(SLC_TEST_CFG_TIMEOUT_MS);

    // slave端最大输入时钟只有6.25MHz
    for (i = HAL_SPI_DIV_8; i < HAL_SPI_DIV_MAX; i++) {
        
        rx_len = 0;
        memset(rx_data, 0, SLC_TEST_RX_MAX_LEN);

        config.clk_div = (hal_spi_clk_div_e)i;
        tx_data[1] = config.clk_div;

        slc_hal_nop_delay_ms(SLC_TEST_CFG_TIMEOUT_MS);

        if (test_id == HAL_SPI0) {
            slc_hal_sysctrl_peripheral_clk_enable(HAL_CLK_SPI0, true);
            slc_hal_sysctrl_peripheral_mod_reset(HAL_CLK_SPI0);
        } else {
            slc_hal_sysctrl_peripheral_clk_enable(HAL_CLK_SPI1, true);
            slc_hal_sysctrl_peripheral_mod_reset(HAL_CLK_SPI1);
        }

        slc_hal_spi_init(test_id, &config);
        //print_reg_4byte_from_to((uint32_t)&SPI0->SPI_TCR, (uint32_t)&SPI0->SPI_FTR);

        ret = slc_spi_test_master_trx_common(test_id);
        if (ret != 0) {
            PRINTF("SPI%u clk_div %u test failed.\n", test_id, config.clk_div);
            return -1;
        }
    }

    slc_hal_nop_delay_ms(SLC_TEST_CFG_TIMEOUT_MS);

    // 遍历SPI0和SPI1
    if (test_id == HAL_SPI0) {
        test_id = HAL_SPI1;
        i = HAL_SPI_DIV_20;
        goto start;
    }

    PRINTF("SPI master clk div test passed.\n");
    return 0;
}

int slc_spi_master_polarity_phase_test(void)
{
    int i, ret;
    hal_spi_cfg_t config = {0};
    uint8_t tx_data[SLC_TEST_RX_MAX_LEN] = {0};
    uint8_t rx_data[SLC_TEST_RX_MAX_LEN] = {0};
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
        memset(rx_data, 0, SLC_TEST_RX_MAX_LEN);

        config.polarity_phase = (hal_spi_polarity_phase_e)i;
        tx_data[2] = config.polarity_phase;

        slc_test_master_send_cmd(SLC_TEST_CMD_SPI_CFG, tx_data, 11);
        ret = slc_test_master_get_result(rx_data, &rx_len);
        if ((ret != 0) || (rx_data[0] != 0)) {
            PRINTF("SPI%u slave cfg %d failed.\n", test_id, i);
            return -1;
        }
        slc_hal_nop_delay_ms(SLC_TEST_CFG_TIMEOUT_MS);

        slc_hal_nop_delay_ms(SLC_TEST_CFG_TIMEOUT_MS);

        if (test_id == HAL_SPI0) {
            slc_hal_sysctrl_peripheral_clk_enable(HAL_CLK_SPI0, true);
            slc_hal_sysctrl_peripheral_mod_reset(HAL_CLK_SPI0);
        } else {
            slc_hal_sysctrl_peripheral_clk_enable(HAL_CLK_SPI1, true);
            slc_hal_sysctrl_peripheral_mod_reset(HAL_CLK_SPI1);
        }

        slc_hal_spi_init(test_id, &config);

        ret = slc_spi_test_master_trx_common(test_id);
        if (ret != 0) {
            PRINTF("SPI%u polarity_phase %u test failed.\n", test_id, config.polarity_phase);
            return -1;
        }
    }

    slc_hal_nop_delay_ms(SLC_TEST_CFG_TIMEOUT_MS);

    // 遍历SPI0和SPI1
    if (test_id == HAL_SPI0) {
        test_id = HAL_SPI1;
        i = 0;
        goto start;
    }

    PRINTF("SPI master polarity_phase test passed.\n");
    return 0;
}

int slc_spi_master_data_mode_test(void)
{
    int i, ret;
    hal_spi_cfg_t config = {0};
    uint8_t tx_data[SLC_TEST_RX_MAX_LEN] = {0};
    uint8_t rx_data[SLC_TEST_RX_MAX_LEN] = {0};
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
    config.cs_gap_time = 128;//0x8;
    config.tx_fifo_pfull_th = 12;
    config.rx_fifo_pfull_th = 12;
    config.rx_fifo_pempty_th = 4;
    config.tx_fifo_pempty_th = 4;

    tx_data[0] = config.mode;
    tx_data[1] = HAL_SPI_DIV_4;//config.clk_div;
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
        memset(rx_data, 0, SLC_TEST_RX_MAX_LEN);

        config.data_mode = (hal_spi_data_mode_e)i;
        tx_data[3] = config.data_mode;

        slc_test_master_send_cmd(SLC_TEST_CMD_SPI_CFG, tx_data, 11);
        ret = slc_test_master_get_result(rx_data, &rx_len);
        if ((ret != 0) || (rx_data[0] != 0)) {
            PRINTF("SPI%u slave cfg failed.\n", test_id);
            return -1;
        }

        slc_hal_nop_delay_ms(SLC_TEST_CFG_TIMEOUT_MS);

        if (test_id == HAL_SPI0) {
            slc_hal_sysctrl_peripheral_clk_enable(HAL_CLK_SPI0, true);
            slc_hal_sysctrl_peripheral_mod_reset(HAL_CLK_SPI0);
        } else {
            slc_hal_sysctrl_peripheral_clk_enable(HAL_CLK_SPI1, true);
            slc_hal_sysctrl_peripheral_mod_reset(HAL_CLK_SPI1);
        }

        slc_hal_spi_init(test_id, &config);

        ret = slc_spi_test_master_trx_common(test_id);
        if (ret != 0) {
            PRINTF("SPI%u data_mode %u test failed.\n", test_id, config.data_mode);
            return -1;
        }
    }

    slc_hal_nop_delay_ms(SLC_TEST_CFG_TIMEOUT_MS);

    // 遍历SPI0和SPI1
    if (test_id == HAL_SPI0) {
        test_id = HAL_SPI1;
        i = 0;
        goto start;
    }

    PRINTF("SPI master data_mode test passed.\n");
    return 0;
}

int slc_spi_master_data_len_test(void)
{
    int i, ret;
    hal_spi_cfg_t config = {0};
    uint8_t tx_data[SLC_TEST_RX_MAX_LEN] = {0};
    uint8_t rx_data[SLC_TEST_RX_MAX_LEN] = {0};
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
    tx_data[1] = HAL_SPI_DIV_4;//config.clk_div;
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
        memset(rx_data, 0, SLC_TEST_RX_MAX_LEN);

        config.data_len = (hal_spi_data_len_e)i;
        tx_data[4] = config.data_len;

        slc_test_master_send_cmd(SLC_TEST_CMD_SPI_CFG, tx_data, 11);
        ret = slc_test_master_get_result(rx_data, &rx_len);
        if ((ret != 0) || (rx_data[0] != 0)) {
            PRINTF("SPI%u slave cfg failed.\n", test_id);
            return -1;
        }

        slc_hal_nop_delay_ms(SLC_TEST_CFG_TIMEOUT_MS);

        if (test_id == HAL_SPI0) {
            slc_hal_sysctrl_peripheral_clk_enable(HAL_CLK_SPI0, true);
            slc_hal_sysctrl_peripheral_mod_reset(HAL_CLK_SPI0);
        } else {
            slc_hal_sysctrl_peripheral_clk_enable(HAL_CLK_SPI1, true);
            slc_hal_sysctrl_peripheral_mod_reset(HAL_CLK_SPI1);
        }

        slc_hal_spi_init(test_id, &config);

        ret = slc_spi_test_master_trx_common(test_id);
        if (ret != 0) {
            PRINTF("SPI%u data_len %u test failed.\n", test_id, config.data_len);
            return -1;
        }
    }

    slc_hal_nop_delay_ms(SLC_TEST_CFG_TIMEOUT_MS);

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
int slc_spi_test_slv_trx_common(hal_spi_id_e spi_id)
{
    uint8_t tx_data[SLC_TEST_RX_MAX_LEN] = {0};
    uint8_t rx_data[SLC_TEST_RX_MAX_LEN] = {0};
    uint8_t tx_buf[SLC_SPI_TEST_TRX_LEN] = {0};
    uint8_t rx_buf[SLC_SPI_TEST_TRX_LEN] = {0};
    uint32_t rx_len = 0;
    int ret = 0;

    ret = slc_hal_get_random_u8(tx_buf, SLC_SPI_TEST_TRX_LEN, 1000);
    if (ret != 0) {
        PRINTF("get random u8 error, ret=0x%X\n", ret);
        return ret;
    }

    tx_data[0] = spi_id;
    tx_data[1] = 0; // 对端是master，读数据
    // 1、master通知slave读取准备好的数据，然后slave将数据返回
    slc_test_master_send_cmd(SLC_TEST_CMD_SPI_S_TRX, tx_data, 2);
    ret = slc_test_master_get_result(rx_data, (uint8_t *)&rx_len);
    if ((ret != 0) || (rx_data[0] != 0)) {
        PRINTF("SPI%d slave tx failed.\n", spi_id);
        return -1;
    }
    memset(rx_data, 0, SLC_TEST_RX_MAX_LEN);
    // 2、确保slave端接收完数据
    slc_hal_spi_slave_transmit(spi_id, tx_buf, SLC_SPI_TEST_TRX_LEN, HAL_SPI_TIMEOUT_US);

    // 3、master再接收数据, slave端将接收数据原路返回
    tx_data[1] = 1; // 对端是master，发数据
    slc_test_master_send_cmd(SLC_TEST_CMD_SPI_S_TRX, tx_data, 2);
    ret = slc_test_master_get_result(rx_data, (uint8_t *)&rx_len);
    if ((ret != 0) || (rx_data[0] != 0)) {
        PRINTF("SPI%d slave rx failed.\n", spi_id);
        return -1;
    }

    // 4、确保slave端发完数据
    rx_len = SLC_SPI_TEST_TRX_LEN;
    slc_hal_spi_slave_receive(spi_id, rx_buf, &rx_len, HAL_SPI_TIMEOUT_US);

    // 5、比较接收长度和数据是否一致
    if ((rx_len != SLC_SPI_TEST_TRX_LEN)) {
        PRINTF("SPI%d slv trx data len: %u != %u\n", spi_id, rx_len, SLC_SPI_TEST_TRX_LEN);
        if (rx_len != 0) {
            dump_u8buf("tx", tx_buf, SLC_SPI_TEST_TRX_LEN);
            dump_u8buf("rx", rx_buf, SLC_SPI_TEST_TRX_LEN);
        }
        return -1;
    }

    if (memcmp(tx_buf, rx_buf, SLC_SPI_TEST_TRX_LEN) != 0) {
        PRINTF("SPI%d slv trx data is different.\n", spi_id);
        dump_u8buf("tx", tx_buf, SLC_SPI_TEST_TRX_LEN);
        dump_u8buf("rx", rx_buf, SLC_SPI_TEST_TRX_LEN);
        return -1;
    }

    return 0;
}

int slc_spi_salve_polarity_phase_test(void)
{
    int i, ret;
    hal_spi_cfg_t config = {0};
    uint8_t tx_data[SLC_TEST_RX_MAX_LEN] = {0};
    uint8_t rx_data[SLC_TEST_RX_MAX_LEN] = {0};
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
        memset(rx_data, 0, SLC_TEST_RX_MAX_LEN);

        config.polarity_phase = (hal_spi_polarity_phase_e)i;
        tx_data[2] = config.polarity_phase;

        slc_test_master_send_cmd(SLC_TEST_CMD_SPI_CFG, tx_data, 11);
        ret = slc_test_master_get_result(rx_data, &rx_len);
        if ((ret != 0) || (rx_data[0] != 0)) {
            PRINTF("SPI%u master cfg failed.\n", test_id);
            return -1;
        }

        slc_hal_nop_delay_ms(SLC_TEST_CFG_TIMEOUT_MS);

        if (test_id == HAL_SPI0) {
            slc_hal_sysctrl_peripheral_clk_enable(HAL_CLK_SPI0, true);
            slc_hal_sysctrl_peripheral_mod_reset(HAL_CLK_SPI0);
        } else {
            slc_hal_sysctrl_peripheral_clk_enable(HAL_CLK_SPI1, true);
            slc_hal_sysctrl_peripheral_mod_reset(HAL_CLK_SPI1);
        }
        slc_hal_spi_init(test_id, &config);
        
        ret = slc_spi_test_slv_trx_common(test_id);
        if (ret != 0) {
            PRINTF("SPI%u polarity_phase %u test failed.\n", test_id, config.polarity_phase);
            return -1;
        }
    }

    slc_hal_nop_delay_ms(SLC_TEST_CFG_TIMEOUT_MS);

    // 遍历SPI0和SPI1
    if (test_id == HAL_SPI0) {
        test_id = HAL_SPI1;
        i = 0;
        goto start;
    }

    PRINTF("SPI master polarity_phase test passed.\n");
    return 0;
}

int slc_spi_slave_data_mode_test(void)
{
    int i, ret;
    hal_spi_cfg_t config = {0};
    uint8_t tx_data[SLC_TEST_RX_MAX_LEN] = {0};
    uint8_t rx_data[SLC_TEST_RX_MAX_LEN] = {0};
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
        memset(rx_data, 0, SLC_TEST_RX_MAX_LEN);

        config.data_mode = (hal_spi_data_mode_e)i;
        tx_data[3] = config.data_mode;

        slc_test_master_send_cmd(SLC_TEST_CMD_SPI_CFG, tx_data, 11);
        ret = slc_test_master_get_result(rx_data, &rx_len);
        if ((ret != 0) || (rx_data[0] != 0)) {
            PRINTF("SPI%u master cfg failed.\n", test_id);
            return -1;
        }

        slc_hal_nop_delay_ms(SLC_TEST_CFG_TIMEOUT_MS);

        if (test_id == HAL_SPI0) {
            slc_hal_sysctrl_peripheral_clk_enable(HAL_CLK_SPI0, true);
            slc_hal_sysctrl_peripheral_mod_reset(HAL_CLK_SPI0);
        } else {
            slc_hal_sysctrl_peripheral_clk_enable(HAL_CLK_SPI1, true);
            slc_hal_sysctrl_peripheral_mod_reset(HAL_CLK_SPI1);
        }

        slc_hal_spi_init(test_id, &config);

        ret = slc_spi_test_slv_trx_common(test_id);
        if (ret != 0) {
            PRINTF("SPI%u data_mode %u test failed.\n", test_id, config.data_mode);
            return -1;
        }
    }

    slc_hal_nop_delay_ms(SLC_TEST_CFG_TIMEOUT_MS);

    // 遍历SPI0和SPI1
    if (test_id == HAL_SPI0) {
        test_id = HAL_SPI1;
        i = 0;
        goto start;
    }

    PRINTF("SPI slave data_mode test passed.\n");
    return 0;
}

int slc_spi_slave_data_len_test(void)
{
    int i, ret;
    hal_spi_cfg_t config = {0};
    uint8_t tx_data[SLC_TEST_RX_MAX_LEN] = {0};
    uint8_t rx_data[SLC_TEST_RX_MAX_LEN] = {0};
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
        memset(rx_data, 0, SLC_TEST_RX_MAX_LEN);

        config.data_len = (hal_spi_data_len_e)i;
        tx_data[4] = config.data_len;

        slc_test_master_send_cmd(SLC_TEST_CMD_SPI_CFG, tx_data, 11);
        ret = slc_test_master_get_result(rx_data, &rx_len);
        if ((ret != 0) || (rx_data[0] != 0)) {
            PRINTF("SPI%u master cfg failed.\n", test_id);
            return -1;
        }

        slc_hal_nop_delay_ms(SLC_TEST_CFG_TIMEOUT_MS);

        if (test_id == HAL_SPI0) {
            slc_hal_sysctrl_peripheral_clk_enable(HAL_CLK_SPI0, true);
            slc_hal_sysctrl_peripheral_mod_reset(HAL_CLK_SPI0);
        } else {
            slc_hal_sysctrl_peripheral_clk_enable(HAL_CLK_SPI1, true);
            slc_hal_sysctrl_peripheral_mod_reset(HAL_CLK_SPI1);
        }

        slc_hal_spi_init(test_id, &config);

        ret = slc_spi_test_slv_trx_common(test_id);
        if (ret != 0) {
            PRINTF("SPI%u data_len %u test failed.\n", test_id, config.data_len);
            return -1;
        }
    }

    slc_hal_nop_delay_ms(SLC_TEST_CFG_TIMEOUT_MS);

    // 遍历SPI0和SPI1
    if (test_id == HAL_SPI0) {
        test_id = HAL_SPI1;
        i = 0;
        goto start;
    }

    PRINTF("SPI slave data_len test passed.\n");
    return 0;
}
