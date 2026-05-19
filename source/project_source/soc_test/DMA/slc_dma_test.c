#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "slc_dma_test.h"
#include "app_cfg.h"
#include "utility.h"
#include "reg_Dev.h"
#include "error_def.h"
#include "debug.h"
#include "slc_hal_sysctrl.h"
#include "slc_hal_gpio.h"
#include "slc_hal_dma.h"
#include "slc_hal_timer.h"
#include "slc_hal_intc.h"
#include "slc_hal_uart.h"
#include "slc_hal_delay.h"
#include "slc_hal_lpuart.h"
#include "slc_hal_iic.h"
#include "slc_hal_spi.h"
#include "hw_flash.h"

#define DMA_TEST_SIZE   1024

#define DMA_TEST_FLASH_SRC_ADDR (FLASH_USERER_DATA_MAX_ADDR - (DMA_TEST_SIZE * 2))
#define DMA_TEST_FLASH_DST_ADDR (FLASH_USERER_DATA_MAX_ADDR - DMA_TEST_SIZE)

#define DMA_UART_TEST_TX_PIN    (HAL_GPIO_PIN2)
#define DMA_UART_TEST_RX_PIN    (HAL_GPIO_PIN3)
#define DMA_UART_TEST_UART      (HAL_UART1)
#define DMA_UART_TEST_BAUDRATE  (921600)
#define DMA_UART_TEST_SIZE      (32)

#define IIC_INT_DMA_TEST_MASK ((IIC_INTR_STAT_RX_FIFO_FULL_INT_MASK << IIC_INTR_STAT_RX_FIFO_FULL_INT_SHIFT) |   \
                                (IIC_INTR_STAT_READ_REQ_INT_MASK << IIC_INTR_STAT_READ_REQ_INT_SHIFT))

volatile uint32_t g_dma_test_flag = 0;
void DMA_IRQ_Handler(void)
{
    uint32_t flag = 0;
    uint16_t fin_num = 0;

    flag = slc_hal_dma_get_interrupt_status();
    slc_hal_dma_clear_interrupt_status(flag);

    PRINTF("DMA IRQ, flag=0x%X\n", flag);

    for (int i = 0; i < HAL_DMA_CH_MAX; i++) {
        if (flag & (1 << i)) {
            fin_num = slc_hal_dma_get_finish_size((hal_dma_ch_e)i);
            PRINTF("CH%d finish transfer %d byte\n", i, fin_num);
        }
    }

    g_dma_test_flag = 1;
}

int slc_dma_mem_to_mem_common_test(uint32_t src_addr, uint32_t dst_addr)
{
    int ret = 0;
    hal_dma_ch_e chx = HAL_DMA_CH0;
    hal_dma_init_t dma_init;

    // 正常的DMA搬运不需要请求信号
    dma_init.req = HAL_DMA_REQ_DISABLE;
    // 由于flash必须4字节对齐访问，所以此处src和dst地址都必须4字节对齐
    dma_init.src_width = HAL_DMA_WIDTH_WORD;
    dma_init.dst_width = HAL_DMA_WIDTH_WORD;
    dma_init.src_addr = src_addr;
    dma_init.dst_addr = dst_addr;
    dma_init.single_pkg_size = DMA_TEST_SIZE;
    dma_init.muli_trigger_num = 0;
    dma_init.irq_enable = HAL_DMA_IRQ_ENABLE_ALL;
    dma_init.timeout = 0;
    dma_init.src_addr_rise = true;
    dma_init.dst_addr_rise  = true;
    // 正常搬运时需选择软件请求，外设搬运时选择外设请求，也可选择软件请求
    dma_init.soft_req = true;
    dma_init.high_priority = false;

    slc_hal_sysctrl_peripheral_clk_enable(HAL_CLK_DMA, true);
    slc_hal_sysctrl_peripheral_mod_reset(HAL_CLK_DMA);

start:
    // flash需要先擦除才可以写入，否则可能无法写入成功; 最小擦除4K，故直接擦除最后4K用作测试
    if (IS_INVALID_FLASH_ADDR(src_addr) || IS_INVALID_FLASH_ADDR(dst_addr)) {
        rom_hw_flash_erase_by_length(FLASH_USERER_DATA_MAX_ADDR - GT_FLASH_SECTOR_SIZE_IN_BYTE,
                                     GT_FLASH_SECTOR_SIZE_IN_BYTE);
    }

    for (int i = 0; i < DMA_TEST_SIZE; i+=4) {
        *((uint32_t *)(src_addr + i)) = i + 0x5AFFA5FF;
        if (IS_INVALID_FLASH_ADDR(dst_addr + i))
            continue;
        *((uint32_t *)(dst_addr + i)) = 0x0;
    }

    g_dma_test_flag = 0;

    slc_hal_dma_init(chx, &dma_init);
    slc_hal_register_irq_handler(DMA_IRQ, DMA_IRQ_Handler);
    SLC_HAL_ENABLE_PERIPHERAL_IRQ(DMA_IRQ, 0x3);
    slc_hal_dma_start(chx);

    while (g_dma_test_flag == 0);

    for (int i = 0; i < DMA_TEST_SIZE; i+=4) {
        if (*((uint32_t *)(dst_addr + i)) != i + 0x5AFFA5FF) {
            PRINTF("DMA CH%d check fail\n", chx);
            dump_u32buf("src", (uint32_t *)src_addr, DMA_TEST_SIZE);
            dump_u32buf("dst", (uint32_t *)dst_addr, DMA_TEST_SIZE);
            ret = -1;
            goto end;
        }
    }

    if (chx == HAL_DMA_CH7) {
        ret = 0;
        goto end;
    }

    chx++;
    goto start;

end:
    if (IS_INVALID_FLASH_ADDR(src_addr) || IS_INVALID_FLASH_ADDR(dst_addr)) {
        rom_hw_flash_erase_by_length(FLASH_USERER_DATA_MAX_ADDR - GT_FLASH_SECTOR_SIZE_IN_BYTE,
                                     GT_FLASH_SECTOR_SIZE_IN_BYTE);
    }

    slc_hal_dma_deinit(chx);
    return ret;
}

int slc_dma_ram_to_ram_test(void)
{
    uint8_t *src_buf = malloc(DMA_TEST_SIZE);
    uint8_t *dst_buf = malloc(DMA_TEST_SIZE);
    int ret = 0;

    if (src_buf == NULL || dst_buf == NULL) {
        PRINTF("malloc error, src_buf 0x%X, dst_buf 0x%X\n", (uint32_t)src_buf, (uint32_t)dst_buf);
        return -1;
    }

    if (slc_dma_mem_to_mem_common_test((uint32_t)src_buf, (uint32_t)dst_buf) != 0) {
        PRINTF("====>DMA RAM to RAM test fail\n");
        ret = -1;
    } else {
        PRINTF("====>DMA RAM to RAM test success\n");
    }

    free(src_buf);
    free(dst_buf);
    return ret;
}

int slc_dma_flash_to_flash_test(void)
{
    int ret = 0;
    ret = slc_dma_mem_to_mem_common_test(DMA_TEST_FLASH_SRC_ADDR, DMA_TEST_FLASH_DST_ADDR);
    if (ret != 0) {
        PRINTF("====>DMA FLASH to FLASH test fail\n");
    } else {
        PRINTF("====>DMA FLASH to FLASH test success\n");
    }
    slc_hal_sysctrl_cache_mode_set(HAL_CACHE_FLUSH);
    return ret;
}

int slc_dma_ram_to_flash_test(void)
{
    uint8_t *buf = malloc(DMA_TEST_SIZE);
    int ret = 0;

    if (buf == NULL) {
        PRINTF("malloc error, ram buf 0x%X\n", (uint32_t)buf);
        return -1;
    }

    ret = slc_dma_mem_to_mem_common_test((uint32_t)buf, DMA_TEST_FLASH_DST_ADDR);
    if (ret != 0) {
        PRINTF("====>DMA RAM to FLASH test fail\n");
        goto end;
    } else {
        PRINTF("====>DMA RAM to FLASH test success\n");
    }

    ret = slc_dma_mem_to_mem_common_test(DMA_TEST_FLASH_DST_ADDR, (uint32_t)buf);
    if (ret != 0) {
        PRINTF("====>DMA FLASH to RAM test fail\n");
        goto end;
    } else {
        PRINTF("====>DMA FLASH to RAM test success\n");
    }

end:
    slc_hal_sysctrl_cache_mode_set(HAL_CACHE_FLUSH);
    free(buf);
    return ret;
}

int slc_dma_diff_width_common_test(hal_dma_width_e width, bool muti_trig, uint16_t package_size, uint16_t trig_num)
{
    int ret = 0;
    hal_dma_ch_e chx = HAL_DMA_CH0;
    hal_dma_init_t dma_init;
    uint8_t *src_buf = malloc(DMA_TEST_SIZE);
    uint8_t *dst_buf = malloc(DMA_TEST_SIZE);
    uint16_t test_trig_num = 0;

    if (src_buf == NULL || dst_buf == NULL) {
        PRINTF("malloc error, src_buf 0x%X, dst_buf 0x%X\n", (uint32_t)src_buf, (uint32_t)dst_buf);
        return -1;
    }

    // 正常的DMA搬运不需要请求信号
    dma_init.req = HAL_DMA_REQ_DISABLE;
    dma_init.src_width = width;
    dma_init.dst_width = width;
    dma_init.src_addr = (uint32_t)src_buf;
    dma_init.dst_addr = (uint32_t)dst_buf;
    dma_init.single_pkg_size = package_size;
    dma_init.muli_trigger_num = trig_num;
    dma_init.irq_enable = HAL_DMA_IRQ_ENABLE_ALL;
    dma_init.timeout = 0;
    dma_init.src_addr_rise = true;
    dma_init.dst_addr_rise  = true;
    // 正常搬运时需选择软件请求，外设搬运时选择外设请求，也可选择软件请求
    dma_init.soft_req = true;
    dma_init.high_priority = false;

    slc_hal_sysctrl_peripheral_clk_enable(HAL_CLK_DMA, true);
    slc_hal_sysctrl_peripheral_mod_reset(HAL_CLK_DMA);

start:
    test_trig_num = 0;
    for (int i = 0; i < DMA_TEST_SIZE; i+=4) {
        *((uint32_t *)(src_buf + i)) = i + 0x5AFFA5FF;
        *((uint32_t *)(dst_buf + i)) = 0x0;
    }

    g_dma_test_flag = 0;

    slc_hal_dma_init(chx, &dma_init);
    slc_hal_dma_start(chx);

    slc_hal_register_irq_handler(DMA_IRQ, DMA_IRQ_Handler);
    SLC_HAL_ENABLE_PERIPHERAL_IRQ(DMA_IRQ, 0x3);
    test_trig_num++;

    while (g_dma_test_flag == 0) {
        if (muti_trig) {
            slc_hal_dma_start(chx);
            test_trig_num++;
            slc_hal_nop_delay_us(10);   // 确保有足够时间让DMA传输完成
        }
    }

    if (test_trig_num != trig_num) {
        PRINTF("DMA CH%d trig_num check fail, actual 0x%X, expect 0x%X\n", chx, test_trig_num, trig_num);
    }

    if (memcmp((uint8_t *)src_buf, (uint8_t *)dst_buf, DMA_TEST_SIZE) == 0) {
        PRINTF("DMA CH%d transfer success\n", chx);
        if (chx != HAL_DMA_CH7) {
            chx++;
        } else {
            ret = 0;
            goto end;
        }
    } else {
        PRINTF("DMA CH%d transfer fail\n", chx);
        dump_u8buf("src", (uint8_t *)src_buf, DMA_TEST_SIZE);
        dump_u8buf("dst", (uint8_t *)dst_buf, DMA_TEST_SIZE);
        ret = -1;
        goto end;
    }

    goto start;

end:
    free(src_buf);
    free(dst_buf);

    slc_hal_dma_deinit(chx);
    return ret;
}

int slc_dma_diff_width_test(void)
{
    if (slc_dma_diff_width_common_test(HAL_DMA_WIDTH_BYTE, false, DMA_TEST_SIZE, 1) != 0) {
        PRINTF("====>DMA byte test fail\n");
        return -1;
    } else {
        PRINTF("====>DMA byte test success\n");
    }
    if (slc_dma_diff_width_common_test(HAL_DMA_WIDTH_HALFWORD, false, DMA_TEST_SIZE, 1) != 0) {
        PRINTF("====>DMA half word test fail\n");
        return -1;
    } else {
        PRINTF("====>DMA half word test success\n");
    }
    if (slc_dma_diff_width_common_test(HAL_DMA_WIDTH_WORD, false, DMA_TEST_SIZE, 1) != 0) {
        PRINTF("====>DMA word test fail\n");
        return -1;
    } else {
        PRINTF("====>DMA word test success\n");
    }

    return 0;
}

int slc_dma_muti_trig_test(void)
{
    if (slc_dma_diff_width_common_test(HAL_DMA_WIDTH_BYTE, true, 1, DMA_TEST_SIZE) != 0) {
        PRINTF("====>DMA muti trig byte test fail\n");
        return -1;
    } else {
        PRINTF("====>DMA muti trig byte test success\n");
    }
    if (slc_dma_diff_width_common_test(HAL_DMA_WIDTH_HALFWORD, true, 2, DMA_TEST_SIZE/2) != 0) {
        PRINTF("====>DMA muti trig half word test fail\n");
        return -1;
    } else {
        PRINTF("====>DMA muti trig half word test success\n");
    }
    if (slc_dma_diff_width_common_test(HAL_DMA_WIDTH_WORD, true, 4, DMA_TEST_SIZE/4) != 0) {
        PRINTF("====>DMA muti trig word test fail\n");
        return -1;
    } else {
        PRINTF("====>DMA muti trig word test success\n");
    }

    return 0;
}

int slc_dma_timeout_test(void)
{
    hal_dma_ch_e chx = HAL_DMA_CH0;
    hal_dma_init_t dma_init;
    uint8_t *src_buf = malloc(DMA_TEST_SIZE);
    uint8_t *dst_buf = malloc(DMA_TEST_SIZE);

    if (src_buf == NULL || dst_buf == NULL) {
        PRINTF("malloc error, src_buf 0x%X, dst_buf 0x%X\n", (uint32_t)src_buf, (uint32_t)dst_buf);
        return -1;
    }

    // 正常的DMA搬运不需要请求信号
    dma_init.req = HAL_DMA_REQ_DISABLE;
    dma_init.src_width = HAL_DMA_WIDTH_BYTE;
    dma_init.dst_width = HAL_DMA_WIDTH_BYTE;
    dma_init.src_addr = (uint32_t)src_buf;
    dma_init.dst_addr = (uint32_t)dst_buf;
    dma_init.single_pkg_size = 1;
    dma_init.muli_trigger_num = DMA_TEST_SIZE;
    dma_init.irq_enable = HAL_DMA_IRQ_ENABLE_ALL;
    dma_init.timeout = 0xFFFF;
    dma_init.src_addr_rise = true;
    dma_init.dst_addr_rise  = true;
    // 正常搬运时需选择软件请求，外设搬运时选择外设请求，也可选择软件请求
    dma_init.soft_req = true;
    dma_init.high_priority = false;

    slc_hal_sysctrl_peripheral_clk_enable(HAL_CLK_DMA, true);
    slc_hal_sysctrl_peripheral_mod_reset(HAL_CLK_DMA);

start:
    // flash必须32bit访问，否则可能丢数，经过cache时是会截断低2bit的，DMA和CPU都必须4字节对齐访问
    for (int i = 0; i < DMA_TEST_SIZE; i+=4) {
        *((uint32_t *)(src_buf + i)) = i + 0x5AFFA5FF;
        *((uint32_t *)(dst_buf + i)) = 0x0;
    }

    g_dma_test_flag = 0;

    slc_hal_dma_init(chx, &dma_init);
    slc_hal_register_irq_handler(DMA_IRQ, DMA_IRQ_Handler);

    PRINTF("====>DMA CH%d timeout test start\n", chx);
    slc_hal_dma_start(chx);
    SLC_HAL_ENABLE_PERIPHERAL_IRQ(DMA_IRQ, 0x3);

    while (g_dma_test_flag == 0);

    if (chx == HAL_DMA_CH7) {
        goto end;
    }

    chx++;
    goto start;

end:
    free(src_buf);
    free(dst_buf);
    slc_hal_dma_deinit(chx);
    PRINTF("====>DMA timeout test success\n");
    return 0;
}

uint32_t g_dma_test_start_time = 0;
uint32_t g_dma_test_end_time = 0;

void slc_dma_performance_test_irq(void)
{
    uint32_t flag = 0;

    g_dma_test_end_time = slc_hal_timer_get_count(HAL_TIMER0);

    flag = slc_hal_dma_get_interrupt_status();
    slc_hal_dma_clear_interrupt_status(flag);

    g_dma_test_flag = 1;
}

int slc_dma_performance_test(void)
{
    hal_dma_width_e width = HAL_DMA_WIDTH_BYTE;
    hal_dma_ch_e chx = HAL_DMA_CH0;
    hal_dma_init_t dma_init;
    uint8_t *src_buf = malloc(DMA_TEST_SIZE);
    uint8_t *dst_buf = malloc(DMA_TEST_SIZE);

    if (src_buf == NULL || dst_buf == NULL) {
        PRINTF("malloc error, src_buf 0x%X, dst_buf 0x%X\n", (uint32_t)src_buf, (uint32_t)dst_buf);
        return -1;
    }

    slc_hal_sysctrl_peripheral_clk_enable(HAL_CLK_TIM0, true);
    slc_hal_sysctrl_peripheral_mod_reset(HAL_CLK_TIM0);

    slc_hal_timer_init(HAL_TIMER0, 0xFFFFFFFF, false);
    slc_hal_timer_start(HAL_TIMER0);

    // 正常的DMA搬运不需要请求信号
    dma_init.req = HAL_DMA_REQ_DISABLE;
    dma_init.src_width = HAL_DMA_WIDTH_WORD;
    dma_init.dst_width = HAL_DMA_WIDTH_WORD;
    dma_init.src_addr = (uint32_t)src_buf;
    dma_init.dst_addr = (uint32_t)dst_buf;
    dma_init.single_pkg_size = DMA_TEST_SIZE;
    dma_init.muli_trigger_num = 0;
    dma_init.irq_enable = HAL_DMA_IRQ_ENABLE_ALL;
    dma_init.timeout = 0;
    dma_init.src_addr_rise = true;
    dma_init.dst_addr_rise  = true;
    // 正常搬运时需选择软件请求，外设搬运时选择外设请求，也可选择软件请求
    dma_init.soft_req = true;
    dma_init.high_priority = false;

    slc_hal_sysctrl_peripheral_clk_enable(HAL_CLK_DMA, true);
    slc_hal_sysctrl_peripheral_mod_reset(HAL_CLK_DMA);

    for (int i = 0; i < DMA_TEST_SIZE; i+=4) {
        *((uint32_t *)(src_buf + i)) = i + 0x5AFFA5FF;
        *((uint32_t *)(dst_buf + i)) = 0x0;
    }

    g_dma_test_flag = 0;

    slc_hal_register_irq_handler(DMA_IRQ, slc_dma_performance_test_irq);
    SLC_HAL_ENABLE_PERIPHERAL_IRQ(DMA_IRQ, 0x3);

    // slc_hal_sysctrl_cache_mode_set(HAL_CACHE_DISABLE);

start:
    g_dma_test_start_time = 0;
    g_dma_test_end_time = 0;
    g_dma_test_flag = 0;

    dma_init.src_width = width;
    dma_init.dst_width = width;
    slc_hal_dma_init(chx, &dma_init);

    g_dma_test_start_time = slc_hal_timer_get_count(HAL_TIMER0);
    slc_hal_dma_start(chx);

    while (g_dma_test_flag == 0);

    PRINTF("====>DMA performance test(RAM->RAM), package: %u byte, width: %u byte, cost time: %.2f us, perf: %.2f Mbps\n",
            DMA_TEST_SIZE, 1<<width, (g_dma_test_start_time - g_dma_test_end_time) / 50.0f,
            DMA_TEST_SIZE * 8 * 50.0f / (1.024f * 1.024f) / (g_dma_test_start_time - g_dma_test_end_time));


    if (width != HAL_DMA_WIDTH_WORD) {
        width++;
        goto start;
    }

    free(src_buf);
    free(dst_buf);

    slc_hal_timer_stop(HAL_TIMER0);
    slc_hal_dma_deinit(chx);

    return 0;
}

void slc_dma_uart_test_irq(void)
{
    uint32_t flag = 0;
    uint16_t fin_num = 0;

    flag = slc_hal_dma_get_interrupt_status();
    slc_hal_dma_clear_interrupt_status(flag);

    PRINTF("DMA IRQ, flag=0x%X\n", flag);

    for (int i = 0; i < HAL_DMA_CH_MAX; i++) {
        if (flag & (1 << i)) {
            fin_num = slc_hal_dma_get_finish_size((hal_dma_ch_e)i);
            g_dma_test_flag++;
            PRINTF("CH%d finish transfer %d byte\n", i, fin_num);
        }
    }
}

int slc_dma_uart_trx_test(void)
{
    hal_uart_init_t config = {0};
    hal_dma_ch_e rx_chx = HAL_DMA_CH0;
    hal_dma_ch_e tx_chx = HAL_DMA_CH3;
    hal_uart_dma_cfg_t dma_cfg;
    uint8_t rx_buf[DMA_UART_TEST_SIZE] = {0};
    uint8_t tx_buf[DMA_UART_TEST_SIZE] = {0};

    g_dma_test_flag = 0;
    for (int i = 0; i < DMA_UART_TEST_SIZE; i++) {
        tx_buf[i] = i;
    }

    slc_hal_sysctrl_peripheral_clk_enable(HAL_CLK_UART1, true);
    slc_hal_sysctrl_peripheral_mod_reset(HAL_CLK_UART1);
    SLC_HAL_DISABLE_PERIPHERAL_IRQ(UART1_IRQ);
    slc_hal_unregister_irq_handler(UART1_IRQ);

    slc_hal_gpio_set_iomux(DMA_UART_TEST_TX_PIN, HAL_IOMUX_MODE0);
    slc_hal_gpio_set_iomux(DMA_UART_TEST_RX_PIN, HAL_IOMUX_MODE0);

    config.baudrate = DMA_UART_TEST_BAUDRATE;
    config.parity = HAL_UART_PARITY_NONE;
    config.stopbit = HAL_UART_STOPBIT_1;
    config.databits = HAL_UART_DATA_8BIT;
    config.tx_fifo_thld = HAL_UART_TXFIFO_THLD_EMPTY;
    config.rx_fifo_thld = HAL_UART_RXFIFO_THLD_1BYTE;
    config.fifo_en = true;
    config.flow_ctrl_en = false;

    slc_hal_uart_init(DMA_UART_TEST_UART, &config);

    slc_hal_sysctrl_peripheral_clk_enable(HAL_CLK_DMA, true);
    slc_hal_sysctrl_peripheral_mod_reset(HAL_CLK_DMA);

    // 配置UART RX DMA通道
    dma_cfg.dma_ch = rx_chx;            // DMA通道
    dma_cfg.mode = HAL_UART_DMA_MODE0;  // DMA模式
    dma_cfg.dma_irq_enable = HAL_DMA_IRQ_ENABLE_ALL;
    dma_cfg.trx_dir = 1;                // 传输方向, 0: 发送, 1: 接收
    dma_cfg.hign_pri = false;           // 优先级, true: 高优先级, false: 低优先级
    dma_cfg.timeout = 0;                // 超时时间, 单位: 1个AHB时钟周期
    slc_hal_uart_dma_config(DMA_UART_TEST_UART, &dma_cfg);

    // 配置UART TX DMA通道
    dma_cfg.dma_ch = tx_chx;            // DMA通道
    dma_cfg.mode = HAL_UART_DMA_MODE0;  // DMA模式
    dma_cfg.dma_irq_enable = HAL_DMA_IRQ_ENABLE_ALL;
    dma_cfg.trx_dir = 0;                // 传输方向, 0: 发送, 1: 接收
    dma_cfg.hign_pri = false;           // 优先级, true: 高优先级, false: 低优先级
    dma_cfg.timeout = 0;                // 超时时间, 单位: 1个AHB时钟周期
    slc_hal_uart_dma_config(DMA_UART_TEST_UART, &dma_cfg);

    slc_hal_register_irq_handler(DMA_IRQ, slc_dma_uart_test_irq);
    SLC_HAL_ENABLE_PERIPHERAL_IRQ(DMA_IRQ, 0x3);

    slc_hal_uart_dma_receive(DMA_UART_TEST_UART, rx_buf, DMA_UART_TEST_SIZE);
    slc_hal_uart_dma_send(DMA_UART_TEST_UART, tx_buf, DMA_UART_TEST_SIZE);

    PRINTF("====>DMA UART test start, please input %d bytes data\n", DMA_UART_TEST_SIZE);
    while (g_dma_test_flag < 2);

    dump_u8buf("uart rx", rx_buf, DMA_UART_TEST_SIZE);

    PRINTF("====>DMA UART test end\n");
    slc_hal_register_irq_handler(DMA_IRQ, DMA_IRQ_Handler);

    NVIC_DisableIRQ(DMA_IRQ);

    return 0;
}

int slc_dma_lpuart_trx_test(void)
{
    hal_dma_ch_e rx_chx = HAL_DMA_CH1;
    hal_dma_ch_e tx_chx = HAL_DMA_CH2;
    hal_lpuart_dma_cfg_t dma_cfg;
    hal_lpuart_init_t lpuart_init;
    uint8_t rx_buf[DMA_UART_TEST_SIZE] = {0};
    uint8_t tx_buf[DMA_UART_TEST_SIZE] = {0};

    g_dma_test_flag = 0;
    for (int i = 0; i < DMA_UART_TEST_SIZE; i++) {
        tx_buf[i] = i;
    }

    slc_hal_sysctrl_peripheral_clk_enable(HAL_CLK_LPUART, true);
    slc_hal_sysctrl_peripheral_mod_reset(HAL_CLK_LPUART);
    SLC_HAL_DISABLE_PERIPHERAL_IRQ(LPUART_IRQ);
    slc_hal_unregister_irq_handler(LPUART_IRQ);

    slc_hal_gpio_set_iomux(HAL_GPIO_PIN18, HAL_IOMUX_MODE3);
    slc_hal_gpio_set_iomux(HAL_GPIO_PIN19, HAL_IOMUX_MODE3);
    slc_hal_lpio_set_iomux(HAL_GPIO_PIN18, HAL_IOMUX_MODE3);
    slc_hal_lpio_set_iomux(HAL_GPIO_PIN19, HAL_IOMUX_MODE3);
    slc_hal_gpio_set_mode(HAL_GPIO_PIN18, HAL_GPIO_PULL_UP);
    slc_hal_gpio_set_mode(HAL_GPIO_PIN19, HAL_GPIO_PULL_UP);

    lpuart_init.baudrate = 9600;
    lpuart_init.parity = HAL_LPUART_PARITY_NONE;
    lpuart_init.stopbit = HAL_LPUART_STOPBIT_1;
    lpuart_init.datalen = HAL_LPUART_DATA_8BIT;
    lpuart_init.rx_fifo_thld = 1;

    slc_hal_lpuart_init(HAL_LPUART0, &lpuart_init);

    slc_hal_sysctrl_peripheral_clk_enable(HAL_CLK_DMA, true);
    slc_hal_sysctrl_peripheral_mod_reset(HAL_CLK_DMA);

    dma_cfg.dma_ch = rx_chx;
    dma_cfg.dma_irq_enable = HAL_DMA_IRQ_ENABLE_ALL;
    dma_cfg.timeout = 0;
    dma_cfg.trx_dir = 1;    // 传输方向, 0: 发送, 1: 接收
    dma_cfg.hign_pri = false;
    slc_hal_lpuart_dma_config(HAL_LPUART0, &dma_cfg);

    // 配置LPUART TX DMA通道
    dma_cfg.dma_ch = tx_chx;
    dma_cfg.dma_irq_enable = HAL_DMA_IRQ_ENABLE_ALL;
    dma_cfg.timeout = 0;
    dma_cfg.trx_dir = 0;    // 传输方向, 0: 发送, 1: 接收
    dma_cfg.hign_pri = false;
    slc_hal_lpuart_dma_config(HAL_LPUART0, &dma_cfg);

    slc_hal_register_irq_handler(DMA_IRQ, slc_dma_uart_test_irq);
    SLC_HAL_ENABLE_PERIPHERAL_IRQ(DMA_IRQ, 0x3);

    slc_hal_lpuart_dma_receive(HAL_LPUART0, rx_buf, DMA_UART_TEST_SIZE);
    slc_hal_lpuart_dma_send(HAL_LPUART0, tx_buf, DMA_UART_TEST_SIZE);

    PRINTF("====>DMA LPUART test start, please input %d bytes data\n", DMA_UART_TEST_SIZE);
    while (g_dma_test_flag < 2);

    dump_u8buf("lpuart rx", rx_buf, DMA_UART_TEST_SIZE);

    PRINTF("====>DMA LPUART test end\n");
    slc_hal_register_irq_handler(DMA_IRQ, DMA_IRQ_Handler);
    NVIC_DisableIRQ(DMA_IRQ);

    return 0;
}

int slc_dma_spi_master_test(void)
{
    hal_dma_ch_e rx_chx = HAL_DMA_CH5;
    hal_dma_ch_e tx_chx = HAL_DMA_CH6;
    hal_dma_init_t dma_init;
    hal_spi_dma_cfg_t dma_cfg;
    uint8_t rx_buf[DMA_UART_TEST_SIZE] = {0};
    uint8_t tx_buf[DMA_UART_TEST_SIZE] = {0};

    for (int i = 0; i < DMA_UART_TEST_SIZE; i++) {
        tx_buf[i] = i + 0xA;
    }

    g_dma_test_flag = 0;

    slc_hal_gpio_set_iomux(HAL_GPIO_PIN4, HAL_IOMUX_MODE2);
    slc_hal_gpio_set_iomux(HAL_GPIO_PIN5, HAL_IOMUX_MODE2);
    slc_hal_gpio_set_iomux(HAL_GPIO_PIN6, HAL_IOMUX_MODE2);
    slc_hal_gpio_set_iomux(HAL_GPIO_PIN7, HAL_IOMUX_MODE2);
    slc_hal_gpio_set_mode(HAL_GPIO_PIN4, HAL_GPIO_PULL_UP);
    slc_hal_gpio_set_mode(HAL_GPIO_PIN5, HAL_GPIO_PULL_UP);
    slc_hal_gpio_set_mode(HAL_GPIO_PIN6, HAL_GPIO_PULL_UP);
    slc_hal_gpio_set_mode(HAL_GPIO_PIN7, HAL_GPIO_PULL_UP);

    hal_spi_cfg_t config = {0};
    config.mode = HAL_SPI_MASTER;
    config.clk_div = HAL_SPI_DIV_20;
    config.polarity_phase = HAL_SPI_CPOL0_CPHA0;
    config.data_mode = HAL_SPI_DATA_LSB;
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

    slc_hal_sysctrl_peripheral_clk_enable(HAL_CLK_SPI1, true);
    slc_hal_sysctrl_peripheral_mod_reset(HAL_CLK_SPI1);
    slc_hal_spi_init(HAL_SPI1, &config);

    dma_cfg.dma_ch = rx_chx;
    dma_cfg.dma_irq_enable = HAL_DMA_IRQ_ENABLE_ALL;
    dma_cfg.dma_timeout = 0;
    dma_cfg.trx_dir = 1;
    dma_cfg.hign_pri = false;

    slc_hal_sysctrl_peripheral_clk_enable(HAL_CLK_DMA, true);
    slc_hal_sysctrl_peripheral_mod_reset(HAL_CLK_DMA);
    slc_hal_spi_dma_config(HAL_SPI1, &dma_cfg);

    dma_cfg.dma_ch = tx_chx;
    dma_cfg.trx_dir = 0;
    slc_hal_spi_dma_config(HAL_SPI1, &dma_cfg);

    slc_hal_register_irq_handler(DMA_IRQ, slc_dma_uart_test_irq);
    SLC_HAL_ENABLE_PERIPHERAL_IRQ(DMA_IRQ, 0x3);

    slc_hal_spi_dma_recv(HAL_SPI1, rx_buf, DMA_UART_TEST_SIZE);
    slc_hal_spi_dma_send(HAL_SPI1, tx_buf, DMA_UART_TEST_SIZE);

    while (g_dma_test_flag < 2);
    dump_u8buf("spi rx", rx_buf, DMA_UART_TEST_SIZE);

    PRINTF("====>DMA SPI test end\n");
    slc_hal_register_irq_handler(DMA_IRQ, DMA_IRQ_Handler);

    slc_hal_dma_deinit(rx_chx);
    slc_hal_dma_deinit(tx_chx);
    NVIC_DisableIRQ(DMA_IRQ);

    return 0;
}

int slc_dma_spi_slave_test(void)
{
    hal_dma_ch_e rx_chx = HAL_DMA_CH5;
    hal_dma_ch_e tx_chx = HAL_DMA_CH6;
    hal_dma_init_t dma_init;
    hal_spi_dma_cfg_t dma_cfg;
    uint8_t rx_buf[DMA_UART_TEST_SIZE] = {0};
    uint8_t tx_buf[DMA_UART_TEST_SIZE] = {0};

    for (int i = 0; i < DMA_UART_TEST_SIZE; i++) {
        tx_buf[i] = i + 0xA;
    }

    g_dma_test_flag = 0;

    slc_hal_gpio_set_iomux(HAL_GPIO_PIN4, HAL_IOMUX_MODE2);
    slc_hal_gpio_set_iomux(HAL_GPIO_PIN5, HAL_IOMUX_MODE2);
    slc_hal_gpio_set_iomux(HAL_GPIO_PIN6, HAL_IOMUX_MODE2);
    slc_hal_gpio_set_iomux(HAL_GPIO_PIN7, HAL_IOMUX_MODE2);
    slc_hal_gpio_set_mode(HAL_GPIO_PIN4, HAL_GPIO_PULL_UP);
    slc_hal_gpio_set_mode(HAL_GPIO_PIN5, HAL_GPIO_PULL_UP);
    slc_hal_gpio_set_mode(HAL_GPIO_PIN6, HAL_GPIO_PULL_UP);
    slc_hal_gpio_set_mode(HAL_GPIO_PIN7, HAL_GPIO_PULL_UP);

    hal_spi_cfg_t config = {0};
    config.mode = HAL_SPI_SLAVE;
    config.clk_div = HAL_SPI_DIV_20;
    config.polarity_phase = HAL_SPI_CPOL0_CPHA0;
    config.data_mode = HAL_SPI_DATA_LSB;
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

    slc_hal_sysctrl_peripheral_clk_enable(HAL_CLK_SPI1, true);
    slc_hal_sysctrl_peripheral_mod_reset(HAL_CLK_SPI1);
    slc_hal_spi_init(HAL_SPI1, &config);

    dma_cfg.dma_ch = rx_chx;
    dma_cfg.dma_irq_enable = HAL_DMA_IRQ_ENABLE_ALL;
    dma_cfg.dma_timeout = 0;
    dma_cfg.trx_dir = 1;
    dma_cfg.hign_pri = false;

    slc_hal_sysctrl_peripheral_clk_enable(HAL_CLK_DMA, true);
    slc_hal_sysctrl_peripheral_mod_reset(HAL_CLK_DMA);
    slc_hal_spi_dma_config(HAL_SPI1, &dma_cfg);

    dma_cfg.dma_ch = tx_chx;
    dma_cfg.trx_dir = 0;
    slc_hal_spi_dma_config(HAL_SPI1, &dma_cfg);

    slc_hal_register_irq_handler(DMA_IRQ, slc_dma_uart_test_irq);
    SLC_HAL_ENABLE_PERIPHERAL_IRQ(DMA_IRQ, 0x3);

    slc_hal_spi_dma_send(HAL_SPI1, tx_buf, DMA_UART_TEST_SIZE);
    slc_hal_spi_dma_recv(HAL_SPI1, rx_buf, DMA_UART_TEST_SIZE);

    PRINTF("====>DMA SPI slave test start, please input %d bytes data\n", DMA_UART_TEST_SIZE);

    while (g_dma_test_flag < 2);
    dump_u8buf("spi rx", rx_buf, DMA_UART_TEST_SIZE);

    PRINTF("====>DMA SPI test end\n");
    slc_hal_register_irq_handler(DMA_IRQ, DMA_IRQ_Handler);
    NVIC_DisableIRQ(DMA_IRQ);

    return 0;
}

uint8_t g_dma_test_iic_buf[DMA_UART_TEST_SIZE] = {0};
void slc_iic_slave_dma_test_irq(void)
{
    uint32_t iic_int_flag;
    uint8_t test_buf[DMA_UART_TEST_SIZE] = {0};

    iic_int_flag = slc_hal_iic_get_irq_status(HAL_IIC0);
    slc_hal_iic_clear_irq_status(HAL_IIC0, iic_int_flag);

    if (iic_int_flag & HAL_IIC_IRQ_STA_READ_REQ) {
        slc_hal_iic_slave_dma_send(HAL_IIC0, g_dma_test_iic_buf, DMA_UART_TEST_SIZE);
    } else if (iic_int_flag & HAL_IIC_IRQ_STA_RX_FIFO_FULL) {
        slc_hal_iic_slave_dma_recv(HAL_IIC0, test_buf, DMA_UART_TEST_SIZE);
        dump_u8buf("iic rx", test_buf, DMA_UART_TEST_SIZE);
    }
}

int slc_dma_iic_slave_test(void)
{
    hal_dma_ch_e rx_chx = HAL_DMA_CH6;
    hal_dma_ch_e tx_chx = HAL_DMA_CH7;
    hal_iic_dma_cfg_t dma_init = {0};
    hal_iic_init_t config = {0};

    g_dma_test_flag = 0;

    for (int i = 0; i < DMA_UART_TEST_SIZE; i++) {
        g_dma_test_iic_buf[i] = i + 2;
    }

    slc_hal_sysctrl_peripheral_clk_enable(HAL_CLK_DMA, true);
    slc_hal_sysctrl_peripheral_mod_reset(HAL_CLK_DMA);
    slc_hal_sysctrl_peripheral_clk_enable(HAL_CLK_I2C0, true);
    slc_hal_sysctrl_peripheral_mod_reset(HAL_CLK_I2C0);

    slc_hal_gpio_set_iomux(HAL_GPIO_PIN4, HAL_IOMUX_MODE5);
    slc_hal_gpio_set_iomux(HAL_GPIO_PIN5, HAL_IOMUX_MODE5);
    slc_hal_gpio_set_mode(HAL_GPIO_PIN4, HAL_GPIO_OPEN_DRAIN);
    slc_hal_gpio_set_mode(HAL_GPIO_PIN5, HAL_GPIO_OPEN_DRAIN);

    config.mode = HAL_IIC_SLAVE;
    config.speed = HAL_IIC_SPEED_100K;  // slave时速度无效，根据master变化
    config.addr_bits = HAL_IIC_ADDR_7BIT;
    config.addr = 0x51;
    config.tx_thld = HAL_IIC_MAX_FIFO_DEPTH - 1;
    config.rx_thld = 1;

    slc_hal_iic_init(HAL_IIC0, &config);
    slc_hal_iic_irq_enable(HAL_IIC0, HAL_IIC_IRQ_RX_FIFO_FULL | HAL_IIC_IRQ_READ_REQ);
    slc_hal_register_irq_handler(I2C0_IRQ, slc_iic_slave_dma_test_irq);
    SLC_HAL_ENABLE_PERIPHERAL_IRQ(I2C0_IRQ, 0x3);

    dma_init.dma_ch = rx_chx;
    dma_init.mode = HAL_IIC_SLAVE;
    dma_init.dma_irq_enable = HAL_DMA_IRQ_ENABLE_ALL;
    dma_init.timeout = 0;
    dma_init.trx_dir = 1;
    dma_init.hign_pri = false;
    slc_hal_iic_dma_config(HAL_IIC0, &dma_init);

    dma_init.dma_ch = tx_chx;
    dma_init.trx_dir = 0;
    slc_hal_iic_dma_config(HAL_IIC0, &dma_init);

    slc_hal_register_irq_handler(DMA_IRQ, slc_dma_uart_test_irq);
    SLC_HAL_ENABLE_PERIPHERAL_IRQ(DMA_IRQ, 0x3);

    PRINTF("====>DMA IIC slave test start, please input %d bytes data\n", DMA_UART_TEST_SIZE);
    while (g_dma_test_flag < 2);

    slc_hal_nop_delay_s(5);

    PRINTF("====>DMA IIC test end\n");
    slc_hal_register_irq_handler(DMA_IRQ, DMA_IRQ_Handler);

    slc_hal_dma_deinit(rx_chx);
    slc_hal_dma_deinit(tx_chx);
    NVIC_DisableIRQ(DMA_IRQ);

    return 0;
}

int slc_dma_iic_master_test(void)
{
    hal_dma_ch_e rx_chx = HAL_DMA_CH4;
    hal_dma_ch_e tx_chx = HAL_DMA_CH5;
    hal_iic_dma_cfg_t dma_init = {0};
    hal_iic_init_t config = {0};
    uint8_t test_rx_buf[DMA_UART_TEST_SIZE] = {0};
    // master DMA发送时，数据位宽必须是2字节，但是高字节为0x00，低字节为实际传输数据，最后1个数据的自动最高位设置stop bit
    uint16_t test_tx_buf[DMA_UART_TEST_SIZE] = {0};

    g_dma_test_flag = 0;

    for (int i = 0; i < DMA_UART_TEST_SIZE; i++) {
        test_tx_buf[i] = (i + 2) & 0xFF;
    }
    test_tx_buf[DMA_UART_TEST_SIZE - 1] |= 0x200; // last byte set stop bit

    slc_hal_sysctrl_peripheral_clk_enable(HAL_CLK_DMA, true);
    slc_hal_sysctrl_peripheral_mod_reset(HAL_CLK_DMA);
    slc_hal_sysctrl_peripheral_clk_enable(HAL_CLK_I2C0, true);
    slc_hal_sysctrl_peripheral_mod_reset(HAL_CLK_I2C0);

    slc_hal_gpio_set_iomux(HAL_GPIO_PIN4, HAL_IOMUX_MODE5);
    slc_hal_gpio_set_iomux(HAL_GPIO_PIN5, HAL_IOMUX_MODE5);
    slc_hal_gpio_set_mode(HAL_GPIO_PIN4, HAL_GPIO_OPEN_DRAIN);
    slc_hal_gpio_set_mode(HAL_GPIO_PIN5, HAL_GPIO_OPEN_DRAIN);

    config.mode = HAL_IIC_MASTER;
    config.speed = HAL_IIC_SPEED_100K;
    config.addr_bits = HAL_IIC_ADDR_7BIT;
    config.addr = 0x51;
    config.tx_thld = HAL_IIC_MAX_FIFO_DEPTH - 1;
    config.rx_thld = 1;

    slc_hal_iic_init(HAL_IIC0, &config);

    dma_init.dma_ch = rx_chx;
    dma_init.mode = HAL_IIC_MASTER;
    dma_init.dma_irq_enable = HAL_DMA_IRQ_ENABLE_ALL;
    dma_init.timeout = 0;
    dma_init.trx_dir = 1;
    dma_init.hign_pri = false;
    slc_hal_iic_dma_config(HAL_IIC0, &dma_init);

    dma_init.dma_ch = tx_chx;
    dma_init.trx_dir = 0;
    slc_hal_iic_dma_config(HAL_IIC0, &dma_init);

    slc_hal_register_irq_handler(DMA_IRQ, slc_dma_uart_test_irq);
    SLC_HAL_ENABLE_PERIPHERAL_IRQ(DMA_IRQ, 0x3);

    slc_hal_iic_master_dma_send(HAL_IIC0, test_tx_buf, DMA_UART_TEST_SIZE);

    while (g_dma_test_flag == 0);

    slc_hal_iic_master_dma_recv(HAL_IIC0, test_tx_buf, test_rx_buf, DMA_UART_TEST_SIZE);

    while (g_dma_test_flag < 3);
    dump_u8buf("IIC master rx", test_rx_buf, DMA_UART_TEST_SIZE);

    PRINTF("====>DMA IIC test end\n");
    slc_hal_register_irq_handler(DMA_IRQ, DMA_IRQ_Handler);

    slc_hal_dma_deinit(rx_chx);
    slc_hal_dma_deinit(tx_chx);
    NVIC_DisableIRQ(DMA_IRQ);

    return 0;
}
