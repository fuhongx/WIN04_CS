#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "reg_Dev.h"
#include "error_def.h"
#include "debug.h"
#include "utility.h"
#include "app_cfg.h"
#include "slc_iic_test.h"
#include "slc_hal_sysctrl.h"
#include "slc_hal_iic.h"
#include "slc_hal_gpio.h"
#include "slc_hal_intc.h"
#include "slc_hal_delay.h"
#include "slc_hal_rng.h"
#include "slc_test_master.h"

void slc_iic_test_init(void)
{
    slc_hal_sysctrl_peripheral_clk_enable(HAL_CLK_RAND, true);
    slc_hal_sysctrl_peripheral_mod_reset(HAL_CLK_RAND);

    hal_rng_init_t rng_init = {0};
    rng_init.seed = 0x12345678;
    slc_hal_rng_init(&rng_init);

    slc_test_common_init();

    slc_hal_gpio_set_iomux(HAL_GPIO_PIN4, HAL_IOMUX_MODE5); // I2C0 SCL
    slc_hal_gpio_set_iomux(HAL_GPIO_PIN5, HAL_IOMUX_MODE5); // I2C0 SDA
    slc_hal_gpio_set_mode(HAL_GPIO_PIN5, HAL_GPIO_OPEN_DRAIN);
    slc_hal_gpio_set_mode(HAL_GPIO_PIN4, HAL_GPIO_OPEN_DRAIN);

    slc_hal_gpio_set_iomux(HAL_GPIO_PIN6, HAL_IOMUX_MODE5); // I2C1 SCL
    slc_hal_gpio_set_iomux(HAL_GPIO_PIN7, HAL_IOMUX_MODE5); // I2C1 SDA
    slc_hal_gpio_set_mode(HAL_GPIO_PIN6, HAL_GPIO_OPEN_DRAIN);
    slc_hal_gpio_set_mode(HAL_GPIO_PIN7, HAL_GPIO_OPEN_DRAIN);
}

void slc_iic_test_deinit(void)
{
    slc_hal_rng_deinit();
    slc_hal_sysctrl_peripheral_clk_enable(HAL_CLK_RAND, false);
    slc_hal_sysctrl_peripheral_mod_reset(HAL_CLK_RAND);

    slc_test_common_deinit();

    slc_hal_gpio_set_iomux(HAL_GPIO_PIN4, HAL_IOMUX_GPIO);
    slc_hal_gpio_set_iomux(HAL_GPIO_PIN5, HAL_IOMUX_GPIO);
    slc_hal_gpio_set_mode(HAL_GPIO_PIN5, HAL_GPIO_PULL_NONE);
    slc_hal_gpio_set_mode(HAL_GPIO_PIN4, HAL_GPIO_PULL_NONE);

    slc_hal_gpio_set_iomux(HAL_GPIO_PIN6, HAL_IOMUX_GPIO);
    slc_hal_gpio_set_iomux(HAL_GPIO_PIN7, HAL_IOMUX_GPIO);
    slc_hal_gpio_set_mode(HAL_GPIO_PIN6, HAL_GPIO_PULL_NONE);
    slc_hal_gpio_set_mode(HAL_GPIO_PIN7, HAL_GPIO_PULL_NONE);
}

int slc_iic_test_master_trx_common(hal_iic_id_e iic_id)
{
    uint8_t tx_data[SLC_IIC_TEST_TRX_LEN] = {0};
    uint8_t rx_data[SLC_IIC_TEST_TRX_LEN] = {0};
    uint32_t rx_len = 0;
    int ret = 0;

    ret = slc_hal_get_random_u8(tx_data, SLC_IIC_TEST_TRX_LEN, 1000);
    if (ret != 0) {
        PRINTF("get random u8 error, ret=0x%X\n", ret);
        return ret;
    }

    // 1、master先发送数据给slave，slave端已提前准备好，在中断内接收数据
    slc_hal_iic_master_send(iic_id, tx_data, SLC_IIC_TEST_TRX_LEN, HAL_IIC_TIMEOUT);

    // 2、确保slave端处理完数据
    slc_hal_nop_delay_ms(1000);

    // 3、master再接收数据, slave端在中断内将接收数据原路返回
    rx_len = SLC_IIC_TEST_TRX_LEN;
    ret = slc_hal_iic_master_recv(iic_id, rx_data, &rx_len, HAL_IIC_TIMEOUT);

    // 4、比较接收长度和数据是否一致
    if ((ret != 0) && (rx_len != SLC_IIC_TEST_TRX_LEN)) {
        PRINTF("IIC%d master trx data len: %u != %u\n", iic_id, rx_len, SLC_IIC_TEST_TRX_LEN);
        if (rx_len != 0) {
            dump_u8buf("tx", tx_data, rx_len);
            dump_u8buf("rx", rx_data, rx_len);
        }
        return -1;
    }

    if (memcmp(rx_data, tx_data, SLC_IIC_TEST_TRX_LEN) != 0) {
        PRINTF("IIC%d master trx data is different.\n", iic_id);
        dump_u8buf("tx", tx_data, SLC_IIC_TEST_TRX_LEN);
        dump_u8buf("rx", rx_data, SLC_IIC_TEST_TRX_LEN);
        return -1;
    }

    return 0;
}

int slc_iic_master_speed_test(void)
{
    int i, ret;
    hal_iic_init_t config = {0};
    uint8_t tx_data[SLC_TEST_RX_MAX_LEN] = {0};
    uint8_t rx_data[SLC_TEST_RX_MAX_LEN] = {0};
    uint8_t rx_len = 0;

    config.mode = HAL_IIC_MASTER;
    config.speed = HAL_IIC_SPEED_100K;
    config.addr_bits = HAL_IIC_ADDR_7BIT;
    config.addr = SLC_TEST_I2C_SLAVE_ADDR_7BIT;
    config.tx_thld = HAL_IIC_MAX_FIFO_DEPTH - 1;
    config.rx_thld = 1;

    tx_data[0] = config.mode;
    tx_data[2] = config.addr_bits;
    tx_data[3] = config.addr & 0xFF;
    tx_data[4] = (config.addr >> 8) & 0xFF;
    tx_data[5] = config.tx_thld;
    tx_data[6] = config.rx_thld;
    tx_data[7] = HAL_IIC0;

start:
    slc_test_master_send_cmd(SLC_TEST_CMD_I2C_CFG, tx_data, 8);
    ret = slc_test_master_get_result(rx_data, &rx_len);
    if ((ret != 0) || (rx_data[0] != 0)) {
        PRINTF("IIC%u slave cfg failed.\n", tx_data[7]);
        return -1;
    }

    for (i = 0; i <= HAL_IIC_SPEED_1M; i++) {
        rx_len = 0;
        memset(rx_data, 0, SLC_TEST_RX_MAX_LEN);

        config.speed = (hal_iic_speed_e)i;
        tx_data[1] = config.speed;

        slc_hal_nop_delay_ms(SLC_TEST_CFG_TIMEOUT_MS);

        if (tx_data[7] == HAL_IIC0) {
            slc_hal_sysctrl_peripheral_clk_enable(HAL_CLK_I2C0, true);
            slc_hal_sysctrl_peripheral_mod_reset(HAL_CLK_I2C0);
        } else {
            slc_hal_sysctrl_peripheral_clk_enable(HAL_CLK_I2C1, true);
            slc_hal_sysctrl_peripheral_mod_reset(HAL_CLK_I2C1);
        }

        slc_hal_iic_init((hal_iic_id_e)tx_data[7], &config);

        ret = slc_iic_test_master_trx_common((hal_iic_id_e)tx_data[7]);
        if (ret != 0) {
            PRINTF("IIC%u speed %u test failed.\n", tx_data[7], config.speed);
            return -1;
        }
    }

    slc_hal_nop_delay_ms(SLC_TEST_CFG_TIMEOUT_MS);

    // 遍历IIC0和IIC1
    if (tx_data[7] == HAL_IIC0) {
        tx_data[7] = HAL_IIC1;
        i = 0;
        goto start;
    }

    PRINTF("IIC speed test passed.\n");
    return 0;
}

int slc_iic_master_addrbits_test(void)
{
    int i, ret;
    hal_iic_init_t config = {0};
    uint8_t tx_data[SLC_TEST_RX_MAX_LEN] = {0};
    uint8_t rx_data[SLC_TEST_RX_MAX_LEN] = {0};
    uint8_t rx_len = 0;

    config.mode = HAL_IIC_MASTER;
    config.speed = HAL_IIC_SPEED_100K;
    config.addr_bits = HAL_IIC_ADDR_7BIT;
    config.addr = SLC_TEST_I2C_SLAVE_ADDR_7BIT;
    config.tx_thld = HAL_IIC_MAX_FIFO_DEPTH - 1;
    config.rx_thld = 1;

    tx_data[0] = config.mode;
    tx_data[1] = config.speed;
    tx_data[5] = config.tx_thld;
    tx_data[6] = config.rx_thld;
    tx_data[7] = HAL_IIC0;

start:
    for (i = 0; i <= HAL_IIC_ADDR_10BIT; i++) {
        rx_len = 0;
        memset(rx_data, 0, SLC_TEST_RX_MAX_LEN);

        config.addr_bits = (hal_iic_addr_bits_e)i;
        tx_data[2] = config.addr_bits;
        if (config.addr_bits == HAL_IIC_ADDR_10BIT) {
            config.addr = SLC_TEST_I2C_SLAVE_ADDR_10BIT;
        } else {
            config.addr = SLC_TEST_I2C_SLAVE_ADDR_7BIT;
        }
        tx_data[3] = config.addr & 0xFF;
        tx_data[4] = (config.addr >> 8) & 0xFF;

        slc_test_master_send_cmd(SLC_TEST_CMD_I2C_CFG, tx_data, 8);
        ret = slc_test_master_get_result(rx_data, &rx_len);
        if ((ret != 0) || (rx_data[0] != 0)) {
            PRINTF("IIC%u slave addrbits cfg failed.\n", tx_data[7]);
            return -1;
        }

        slc_hal_nop_delay_ms(SLC_TEST_CFG_TIMEOUT_MS);

        if (tx_data[7] == HAL_IIC0) {
            slc_hal_sysctrl_peripheral_clk_enable(HAL_CLK_I2C0, true);
            slc_hal_sysctrl_peripheral_mod_reset(HAL_CLK_I2C0);
        } else {
            slc_hal_sysctrl_peripheral_clk_enable(HAL_CLK_I2C1, true);
            slc_hal_sysctrl_peripheral_mod_reset(HAL_CLK_I2C1);
        }

        slc_hal_iic_init((hal_iic_id_e)tx_data[7], &config);

        ret = slc_iic_test_master_trx_common((hal_iic_id_e)tx_data[7]);
        if (ret != 0) {
            PRINTF("IIC%u addrbits %u test failed.\n", tx_data[7], config.addr_bits);
            return -1;
        }
    }

    slc_hal_nop_delay_ms(SLC_TEST_CFG_TIMEOUT_MS);

    // 遍历IIC0和IIC1
    if (tx_data[7] == HAL_IIC0) {
        tx_data[7] = HAL_IIC1;
        i = 0;
        goto start;
    }

    PRINTF("IIC speed test passed.\n");
    return 0;
}

uint8_t g_iic_test_slv_tx_data[SLC_IIC_TEST_TRX_LEN] = {0};
uint8_t g_iic_test_slv_rx_data[SLC_IIC_TEST_TRX_LEN] = {0};
volatile uint32_t g_iic_test_slv_rx_len = 0;
volatile int g_iic_test_slv_id = HAL_IIC0;

void slc_iic_test_slv_irq(void)
{
    uint32_t iic_int_flag;
    int ret = 0;

    iic_int_flag = slc_hal_iic_get_irq_status(g_iic_test_slv_id);
    slc_hal_iic_clear_irq_status(g_iic_test_slv_id, iic_int_flag);

    if (iic_int_flag & HAL_IIC_IRQ_STA_READ_REQ) {
        ret = slc_hal_iic_slave_send(g_iic_test_slv_id, g_iic_test_slv_tx_data, SLC_IIC_TEST_TRX_LEN, HAL_IIC_TIMEOUT);
        if (ret != 0) {
            PRINTF("IIC%d slave tx error, ret=0x%X\n", g_iic_test_slv_id, ret);
        }
    } else if (iic_int_flag & HAL_IIC_IRQ_STA_RX_FIFO_FULL) {
        g_iic_test_slv_rx_len = SLC_IIC_TEST_TRX_LEN;
        ret = slc_hal_iic_slave_recv(g_iic_test_slv_id, g_iic_test_slv_rx_data, (uint32_t *)&g_iic_test_slv_rx_len, HAL_IIC_TIMEOUT);
        if (ret != 0 && (g_iic_test_slv_rx_len == 0)) {
            PRINTF("IIC%d slave rx error, ret=0x%X, len=%u\n", g_iic_test_slv_id, ret, g_iic_test_slv_rx_len);
        } else {
            dump_u8buf("slave rx buf", g_iic_test_slv_rx_data, g_iic_test_slv_rx_len);
        }
    }

    // PRINTF("IIC IRQ Handler, int status 0x%x\n", iic_int_flag);
}

int slc_iic_test_slv_trx_common(hal_iic_id_e iic_id)
{
    uint8_t tx_data[SLC_TEST_RX_MAX_LEN] = {0};
    uint8_t rx_data[SLC_TEST_RX_MAX_LEN] = {0};
    uint8_t rx_len = 0;
    int ret = 0;

    memset(g_iic_test_slv_tx_data, 0, SLC_IIC_TEST_TRX_LEN);
    memset(g_iic_test_slv_rx_data, 0, SLC_IIC_TEST_TRX_LEN);
    g_iic_test_slv_rx_len = 0;
    ret = slc_hal_get_random_u8(g_iic_test_slv_tx_data, SLC_IIC_TEST_TRX_LEN, 1000);
    if (ret != 0) {
        PRINTF("get random u8 error, ret=0x%X\n", ret);
        return ret;
    }

    tx_data[0] = iic_id;
    tx_data[1] = 0; // 对端是master，读数据
    // 1、master通知slave读取准备好的数据，然后slave将数据返回
    slc_test_master_send_cmd(SLC_TEST_CMD_I2C_S_TRX, tx_data, 2);
    ret = slc_test_master_get_result(rx_data, &rx_len);
    if ((ret != 0) || (rx_data[0] != 0)) {
        PRINTF("IIC%d slave tx failed.\n", iic_id);
        return -1;
    }
    memset(rx_data, 0, SLC_TEST_RX_MAX_LEN);

    // 2、确保slave端接收完数据
    slc_hal_nop_delay_s(2);

    // 3、master再接收数据, slave端将接收数据原路返回
    tx_data[1] = 1; // 对端是master，发数据
    slc_test_master_send_cmd(SLC_TEST_CMD_I2C_S_TRX, tx_data, 2);
    ret = slc_test_master_get_result(rx_data, &rx_len);
    if ((ret != 0) || (rx_data[0] != 0)) {
        PRINTF("IIC%d slave rx failed.\n", iic_id);
        return -1;
    }

    // 2、确保slave端发完数据
    slc_hal_nop_delay_s(2);

    // 4、比较接收长度和数据是否一致
    if ((g_iic_test_slv_rx_len != SLC_IIC_TEST_TRX_LEN)) {
        PRINTF("IIC%d slv trx data len: %u != %u\n", iic_id, g_iic_test_slv_rx_len, SLC_IIC_TEST_TRX_LEN);
        if (g_iic_test_slv_rx_len != 0) {
            dump_u8buf("tx", g_iic_test_slv_tx_data, SLC_IIC_TEST_TRX_LEN);
            dump_u8buf("rx", g_iic_test_slv_rx_data, SLC_IIC_TEST_TRX_LEN);
        }
        return -1;
    }

    if (memcmp(g_iic_test_slv_tx_data, g_iic_test_slv_rx_data, SLC_IIC_TEST_TRX_LEN) != 0) {
        PRINTF("IIC%d slv trx data is different.\n", iic_id);
        dump_u8buf("tx", g_iic_test_slv_tx_data, SLC_IIC_TEST_TRX_LEN);
        dump_u8buf("rx", g_iic_test_slv_rx_data, SLC_IIC_TEST_TRX_LEN);
        return -1;
    }

    return 0;
}

int slc_iic_slv_addrbits_test(void)
{
    int i, ret;
    hal_iic_init_t config = {0};
    uint8_t tx_data[SLC_TEST_RX_MAX_LEN] = {0};
    uint8_t rx_data[SLC_TEST_RX_MAX_LEN] = {0};
    uint8_t rx_len = 0;

    config.mode = HAL_IIC_SLAVE;
    config.speed = HAL_IIC_SPEED_100K;  // slave时速度无效，根据master变化
    config.addr_bits = HAL_IIC_ADDR_7BIT;
    config.addr = SLC_TEST_I2C_SLAVE_ADDR_7BIT;
    config.tx_thld = HAL_IIC_MAX_FIFO_DEPTH - 1;
    config.rx_thld = 1;

    tx_data[0] = config.mode;
    tx_data[1] = config.speed;
    tx_data[5] = config.tx_thld;
    tx_data[6] = config.rx_thld;
    tx_data[7] = HAL_IIC0;

start:
    g_iic_test_slv_id = tx_data[7];
    for (i = 0; i <= HAL_IIC_ADDR_10BIT; i++) {
        rx_len = 0;
        memset(rx_data, 0, SLC_TEST_RX_MAX_LEN);

        config.addr_bits = (hal_iic_addr_bits_e)i;
        tx_data[2] = config.addr_bits;
        if (config.addr_bits == HAL_IIC_ADDR_10BIT) {
            config.addr = SLC_TEST_I2C_SLAVE_ADDR_10BIT;
        } else {
            config.addr = SLC_TEST_I2C_SLAVE_ADDR_7BIT;
        }
        tx_data[3] = config.addr & 0xFF;
        tx_data[4] = (config.addr >> 8) & 0xFF;

        slc_test_master_send_cmd(SLC_TEST_CMD_I2C_CFG, tx_data, 8);
        ret = slc_test_master_get_result(rx_data, &rx_len);
        if ((ret != 0) || (rx_data[0] != 0)) {
            PRINTF("IIC%d slave addrbits cfg failed.\n", g_iic_test_slv_id);
            return -1;
        }

        slc_hal_nop_delay_ms(SLC_TEST_CFG_TIMEOUT_MS);

        if (g_iic_test_slv_id == HAL_IIC0) {
            slc_hal_sysctrl_peripheral_clk_enable(HAL_CLK_I2C0, true);
            slc_hal_sysctrl_peripheral_mod_reset(HAL_CLK_I2C0);

            slc_hal_register_irq_handler(I2C0_IRQ, slc_iic_test_slv_irq);
            SLC_HAL_ENABLE_PERIPHERAL_IRQ(I2C0_IRQ, 0x3);
        } else {
            slc_hal_sysctrl_peripheral_clk_enable(HAL_CLK_I2C1, true);
            slc_hal_sysctrl_peripheral_mod_reset(HAL_CLK_I2C1);

            slc_hal_register_irq_handler(I2C1_IRQ, slc_iic_test_slv_irq);
            SLC_HAL_ENABLE_PERIPHERAL_IRQ(I2C1_IRQ, 0x3);
        }

        slc_hal_iic_init((hal_iic_id_e)g_iic_test_slv_id, &config);
        slc_hal_iic_irq_enable((hal_iic_id_e)g_iic_test_slv_id, HAL_IIC_IRQ_RX_FIFO_FULL | HAL_IIC_IRQ_READ_REQ);

        ret = slc_iic_test_slv_trx_common((hal_iic_id_e)g_iic_test_slv_id);
        if (ret != 0) {
            PRINTF("IIC%d addrbits %u test failed.\n", g_iic_test_slv_id, config.addr_bits);
            return -1;
        }

        PRINTF("IIC%d addrbits %u test success.\n", g_iic_test_slv_id, config.addr_bits);
    }

    slc_hal_nop_delay_ms(SLC_TEST_CFG_TIMEOUT_MS);

    // 遍历IIC0和IIC1
    if (g_iic_test_slv_id == HAL_IIC0) {
        tx_data[7] = HAL_IIC1;
        i = 0;
        goto start;
    }

    PRINTF("IIC speed test passed.\n");
    return 0;
}
