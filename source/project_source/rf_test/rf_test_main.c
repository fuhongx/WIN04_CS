

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "string.h"
#include "reg_Dev.h"
#include "error_def.h"
#include "utility.h"
#include "app_cfg.h"
#include "slc_hal_sysctrl.h"
#include "slc_hal_delay.h"
#include "slc_hal_intc.h"
#include "slc_hal_gpio.h"
#include "rf_test_def.h"
#include "slc_cali.h"
#include "slc_phy_trx.h"
#include "slc_private_spi_frame.h"
//------------------------------------------------------------------------------
// Compatible old drivers.
//------------------------------------------------------------------------------
// typedef struct
//{
//    __IO unsigned int PMU_STOP_SET;                 /*!<0x00:*/
//    __IO unsigned int PMU_PHY_PWR;                  /*!<0x04:*/
//    __IO unsigned int PMU_STOP_MSK;                 /*!<0x08:*/
//    __IO unsigned int PMU_WKP_SRC;                  /*!<0x0C:*/
//    __IO unsigned int PMU_WKP_FLG;                  /*!<0x10:*/
//}PMU_TypeDef;

// #define PMU                 ((PMU_TypeDef *)ADDR_PMU_BASE)

//------------------------------------------------------------------------------
// Variable define
//------------------------------------------------------------------------------
uint32_t i;

uint8_t rx_data;

enum
{
    S_HEAD1,
    S_HEAD2,
    S_CMD,
    S_ADDR,
    S_DATA
} state;

union
{
    uint8_t byte[2];
    uint16_t data;
} cmd;

union
{
    uint8_t byte[2];
    uint16_t data;
} addr;

union
{
    uint8_t byte[4];
    uint32_t data;
} payload;

int cmd_cnt, addr_cnt, payload_cnt, payload_num;

uint32_t *op_addr;

// Port define
extern void *pf_transmit;
extern void *pf_get_char;

void rf_test_transmit_bytes(uint8_t *pu8Buffer, uint16_t u16Len)
{
    // wait add
    pf_rf_test_transmit_callback Callback = (pf_rf_test_transmit_callback)pf_transmit;
    if (NULL != pf_transmit)
    {
        Callback(pu8Buffer, u16Len);
    }
}

uint8_t get_char(void)
{
    pf_rf_test_get_char_callback Callback = (pf_rf_test_get_char_callback)pf_get_char;
    if (NULL != pf_get_char)
    {
        uint8_t u8Char = Callback();
        return u8Char;
    }
    return 0;
}

void rf_test_interface_init(void);

uint8_t au8RxBuffer[300];
void rf_test_uart_get_data(uint8_t *pu8Buffer, uint16_t *pu16Len);
void rf_sub_spi_init(void);
void rf_subg_write32_cmd(uint16_t u16Addr, uint8_t *pu8Buffer, uint16_t u16Len);
void rf_subg_read32_cmd(uint16_t u16Addr, uint8_t *pu8Buffer, uint16_t u16Len);
void rf_subg_read32_with_dummy(uint16_t u16Addr, uint8_t *pu8Buffer, uint16_t u16Len);

void spi_cmd_handler(uint8_t *pu8Buffer)
{
    // F9 + Addr LSb + Data
    uint8_t u8Cmd = pu8Buffer[0];
    PRINTF("Cmd = 0x%x\n", u8Cmd);
    uint16_t u16Addr = rom_utility_little_endian_read_16(pu8Buffer, 1);
    uint8_t au8ReadBuffer[10] = {0};
    uint32_t u32Data = 0;

    switch (u8Cmd)
    {
    case 0x2:
    {
        // PRINTF("Subg Write Cmd!\n");
        rf_subg_write32_cmd(u16Addr, &pu8Buffer[3], 4);
        break;
    }

    case 0x3:
    {
        // PRINTF("Subg Read Cmd!\n");
        rf_subg_read32_cmd(u16Addr, au8ReadBuffer, 4);
        break;
    }

    case 0x0B:
    {
        // PRINTF("Subg Read Dummy Cmd!\n");
        rf_subg_read32_with_dummy(u16Addr, au8ReadBuffer, 4);
        rf_test_transmit_bytes(au8ReadBuffer, 4);
        break;
    }

    default:
        break;
    }
}

// void slc_phy0_test_mask_irq(void)
// {
//     uint32_t irq_flag = slc_phy_irq_get_status();

//     // clear irq flag
//     slc_phy_irq_clr_status(irq_flag);

//     PRINTF("PHY0 IRQ: 0x%08X\r\n", irq_flag);
// }

void rf_test_main(void)
{
    // WIN 04 No need open phy clock - 2024/12/25
    //    rom_hw_sysctrl_reset_phy();

    rf_test_interface_init();
#ifdef SLC_FPGA
    rf_sub_spi_init();
    // slc_private_spi_init();
#endif
    slc_hal_gpio_set_iomux(HAL_GPIO_PIN17, HAL_IOMUX_MODE7);
    // SLC_HAL_ENABLE_PERIPHERAL_IRQ(PHY0_IRQ, 0x3);
    // slc_hal_register_irq_handler(PHY0_IRQ, slc_phy0_test_mask_irq);
    // slc_phy0_irq_enable(PHY_IRQ_ALL_MASK);

    slc_hal_nop_delay_us(800);

    uint32_t u32Addr = 0x40010000;

    for (int i = 0; i < 10; i++)
    {
        // PRINTF("Addr 0x%x = 0x%x\n", u32Addr, *(volatile uint32_t *)u32Addr);
        u32Addr += 4;
    }

    u32Addr = 0x40000000;

    for (int i = 0; i < 10; i++)
    {
        // PRINTF("Addr 0x%x = 0x%x\n", u32Addr, *(volatile uint32_t *)u32Addr);
        u32Addr += 4;
    }

    //    while( PMU->PHY_PWR==0 );

    //     extern void cad_test_rf_int_init(void);
    //    cad_test_rf_int_init();

    //    uint8_t u8Data[10] = {0};
    //    uint8_t u8Idx = 0;
    //    while(1)
    //    {
    //        rx_data = get_char();
    //        u8Data[u8Idx] = rx_data;
    //        u8Idx ++;
    //        if(u8Idx == 10)
    //        {
    //           for(int i = 0; i < 10; i++)
    //            {
    //                PRINTF("0x%x\t", u8Data[i]);
    //            }
    //            u8Idx = 0;
    //        }
    //
    //    }

    state = S_HEAD1;

#define ADDR_BASE_FOR_TEST (0x40000000) // 0x40010000

    while (1)

    {

        rx_data = get_char();
        // PRINTF("Rx Data = 0x%x\n", rx_data);
        // GPIO1->GPIO_DAT = state;

        // GPIO2->GPIO_DAT = rx_data;

        switch (state)

        {

        case S_HEAD1:

            cmd_cnt = 0;

            addr_cnt = 0;

            payload_cnt = 0;

            if (rx_data == 0x9F)

            {

                state = S_HEAD2;
            }

            else

            {
                if (rx_data == 0xF9)
                {
                    uint16_t u16RxLen = 16;
                    rf_test_uart_get_data(au8RxBuffer, &u16RxLen);
                    for (int i = 0; i < u16RxLen; i++)
                    {
                        // PRINTF("R %d = 0x%x\n", i, au8RxBuffer[i]);
                    }
                    spi_cmd_handler(&au8RxBuffer[0]);
                    memset(au8RxBuffer, 0, sizeof(au8RxBuffer));
                }
                state = S_HEAD1;
            }

            break;

        case S_HEAD2:

            if (rx_data == 0x8F)

            {

                state = S_CMD;
            }

            else

            {

                state = S_HEAD1;
            }

            break;

        case S_CMD:

            cmd.byte[cmd_cnt] = rx_data;

            cmd_cnt = cmd_cnt + 1;

            if (cmd_cnt == 2)

            {

                state = S_ADDR;
            }

            else

            {

                state = S_CMD;
            }

            break;

        case S_ADDR:

            addr.byte[addr_cnt] = rx_data;

            addr_cnt = addr_cnt + 1;

            if (addr_cnt == 2)

            {

                state = S_DATA;

                if (cmd.data == 0x0002)

                    payload_num = 2;

                else if (cmd.data == 0x0001)

                    payload_num = 2;

                else if (cmd.data == 0x0004)

                    payload_num = 4;

                else if (cmd.data == 0x0005)

                    payload_num = 4;
                // soc_ctrl wrtie
                else if (cmd.data == 0x0010)

                    payload_num = 4;
                // soc_ctrl read
                else if (cmd.data == 0x0011)

                    payload_num = 4;
                else if (cmd.data == 0x001E)
                    payload_num = 4;
                else if (cmd.data == 0x001F)
                    payload_num = 4;
                else if (cmd.data == 0x0020)
                    payload_num = 4;
                else if (cmd.data == 0x0021)
                    payload_num = 4;
                else

                {

                    payload_num = 0;

                    state = S_HEAD1;
                }
            }

            else

            {

                state = S_ADDR;
            }

            break;

        case S_DATA:

            payload.byte[payload_cnt] = rx_data;

            payload_cnt = payload_cnt + 1;

            if (payload_cnt == payload_num)

            {
                // op_addr = ((uint32_t *)(0x40010000+addr.data));

                switch (cmd.data)

                {

                case (0x0002):
                    op_addr = ((uint32_t *)(0x40010000 + addr.data));

                    payload.byte[2] = 0;

                    payload.byte[3] = 0;

                    *op_addr = payload.data;
                    // PRINTF("WRITE_RF_REG(0x%x, 0x%x);\n", op_addr, payload.data);
                    // slc_phy_spi_write32_cmd(addr.data, payload.data);

                    break;

                case (0x0001):
                    op_addr = ((uint32_t *)(0x40010000 + addr.data));

                    payload.data = *op_addr;
                    // payload.data = slc_phy_spi_read32_cmd(addr.data);

                    // put_char(payload.byte[0]);
                    // put_char(payload.byte[1]);

                    rf_test_transmit_bytes(&payload.byte[0], 2);

                    break;

                case (0x0005):
                    op_addr = ((uint32_t *)(0x40010000 + addr.data));

                    *op_addr = payload.data;

                    // slc_phy_spi_write32_cmd(addr.data, payload.data);
                    // PRINTF("WRITE_RF_REG(0x%x, 0x%x);\n", op_addr, payload.data);

                    break;

                case (0x0004):
                    op_addr = ((uint32_t *)(0x40010000 + addr.data));

                    // PRINTF("READ 004\n");
                    payload.data = *op_addr;

                    // put_char(payload.byte[0]);
                    // put_char(payload.byte[1]);
                    // put_char(payload.byte[2]);
                    // put_char(payload.byte[3]);

                    // payload.data = slc_phy_spi_read32_cmd(addr.data);

                    rf_test_transmit_bytes(&payload.byte[0], 4);

                    break;

                    // soc ctrl write
                case (0x0010):
                    op_addr = ((uint32_t *)(0x40000000 + addr.data));

                    *op_addr = payload.data;
                    // PRINTF("WRITE_REG(0x%x, 0x%x);\n", op_addr, payload.data);

                    break;

                    // soc ctrl read
                case (0x0011):
                    op_addr = ((uint32_t *)(0x40000000 + addr.data));

                    payload.data = *op_addr;

                    // put_char(payload.byte[0]);
                    // put_char(payload.byte[1]);
                    // put_char(payload.byte[2]);
                    // put_char(payload.byte[3]);

                    rf_test_transmit_bytes(&payload.byte[0], 4);

                    break;

                case (0x001E):
                    op_addr = ((uint32_t *)(0x40057000 + addr.data));

                    *op_addr = payload.data;
                    // PRINTF("WRITE_REG(0x%x, 0x%x);\n", op_addr, payload.data);

                    break;

                case (0x001F):
                    op_addr = ((uint32_t *)(0x40057000 + addr.data));

                    payload.data = *op_addr;

                    // put_char(payload.byte[0]);
                    // put_char(payload.byte[1]);
                    // put_char(payload.byte[2]);
                    // put_char(payload.byte[3]);

                    rf_test_transmit_bytes(&payload.byte[0], 4);

                case (0x0020):
                    op_addr = ((uint32_t *)(0x40075000 + addr.data));

                    *op_addr = payload.data;
                    // PRINTF("WRITE_REG(0x%x, 0x%x);\n", op_addr, payload.data);

                    if ((addr.data == 0x0004) && (payload.data == 0x1)) {
                        slc_clk_cali(SLC_CALI_DCDC1M);
                        slc_clk_cali(SLC_CALI_RC32K);
                        slc_clk_cali(SLC_CALI_RC50M);
                    }
                    break;

                case (0x0021):
                    op_addr = ((uint32_t *)(0x40075000 + addr.data));

                    payload.data = *op_addr;

                    // put_char(payload.byte[0]);
                    // put_char(payload.byte[1]);
                    // put_char(payload.byte[2]);
                    // put_char(payload.byte[3]);

                    rf_test_transmit_bytes(&payload.byte[0], 4);

                    break;

                default:
                    break;
                }

                state = S_HEAD1;
            }

            else

            {

                state = S_DATA;
            }

            break;

        default:
            break;
        }
    }
}
