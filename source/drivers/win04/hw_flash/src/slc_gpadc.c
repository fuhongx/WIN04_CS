/**
 * ****************************************************************************
 * @file qmx_hal_gpadc.c
 * @author QMX software team.
 * @brief
 * @version  1.0
 * ****************************************************************************
 *
 * THIS CODE IS LIMITED TO GUIDE THE APPLICATION AND IMPROVE THE EFFICIENCY OF RESEARCH AND DEVELOPMENT.
 * AS A RESULT, QMX IS NOT RESPONSIBLE FOR ANY CLAIMS ARISING DIRECTLY OR INDIRECTLY FROM SUCH FIRMWARE AND/OR CONTENT.
 *
 * Copyright (c) 2026 QMX Semiconductor all rights reserved.
 *
 * ****************************************************************************
 */
 
#include "slc_gpadc.h"
 
#include "slc_hal_delay.h"
//#include "qmx_phy.h"
#include "slc_rf.h"
#include "app_cfg.h"
#include "utility.h"

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


void adc_gpio_init(hal_gpio_pin_e pin)
{
    GPIO->INPUT_EN  &= ~1<<pin;
    GPIO->OUTPUT_EN &= ~1<<pin;
    GPIO->PULL_EN   &= ~1<<pin;
    GPIO->GPIO_AEN  |=  1<<pin;
    
    //GPIO->INPUT_EN  &= ~0x1FF;
    //GPIO->OUTPUT_EN &= ~0x1FF;
    //GPIO->PULL_EN   &= ~0x1FF;
    //GPIO->GPIO_AEN  |=  0x1FF;
    slc_rf_spi_reg_update
}

void adc_channel_config(ADC_ChannelTypeDef ch)
{
    // 清除通道配置位
    //RF_CTRL->GPADC_CTRL &= ~(GPADC_BUFFER_SEL_Msk | GPADC_VIP_SEL2_Msk | GPADC_VIP_SEL_Msk);
    slc_rf_spi_reg_clr_mask(SLC_RF_SPI_ADDR_CTRL(0xC0), (GPADC_BUFFER_SEL_Msk | GPADC_VIP_SEL2_Msk | GPADC_VIP_SEL_Msk));

    switch(ch)
    {
        // ==================== 固定内置通道 ====================
        case ADC_CH_VSENSOR:
            // BUFFER_SEL=00，无需配置VIP位
            //RF_CTRL->GPADC_CTRL |= (0x00 << GPADC_BUFFER_SEL_Pos);
            slc_rf_spi_reg_or_mask(SLC_RF_SPI_ADDR_CTRL(0xC0), (0x00 << GPADC_BUFFER_SEL_Pos));
            break;

        case ADC_CH_TSENSOR:
            // BUFFER_SEL=01
            //RF_CTRL->GPADC_CTRL |= (0x01 << GPADC_BUFFER_SEL_Pos);
            slc_rf_spi_reg_or_mask(SLC_RF_SPI_ADDR_CTRL(0xC0), (0x01 << GPADC_BUFFER_SEL_Pos));
            break;

        case ADC_CH_TXPDT:
            // BUFFER_SEL=10
            ///RF_CTRL->GPADC_CTRL |= (0x02 << GPADC_BUFFER_SEL_Pos);
            slc_rf_spi_reg_or_mask(SLC_RF_SPI_ADDR_CTRL(0xC0), (0x02 << GPADC_BUFFER_SEL_Pos));
            break;

        // ==================== GPIO单端通道 ====================
        case ADC_CH_GPIO5:
            // BUFFER_SEL=11 + VIP_SEL2=0 + VIP_SEL=00
            adc_gpio_init(HAL_GPIO_PIN5);
            
            PRINTF("ADDR:%08x RF_CTRL->ATB_TRXABB_CTRL=%08x\n",&RF_CTRL->ATB_TRXABB_CTRL,RF_CTRL->ATB_TRXABB_CTRL);
            slc_rf_spi_reg_or_mask(SLC_RF_SPI_ADDR_CTRL(0xC0), ((0x03 << GPADC_BUFFER_SEL_Pos) |
                                                                    (0x00 << GPADC_VIP_SEL2_Pos) |
                                                                    (0x00 << GPADC_VIP_SEL_Pos)));
            break;

        case ADC_CH_GPIO6:
            // BUFFER_SEL=11 + VIP_SEL2=0 + VIP_SEL=01
            adc_gpio_init(HAL_GPIO_PIN6);
            slc_rf_spi_reg_or_mask(SLC_RF_SPI_ADDR_CTRL(0xC0), ((0x03 << GPADC_BUFFER_SEL_Pos) |
                                                                    (0x00 << GPADC_VIP_SEL2_Pos) |
                                                                    (0x01 << GPADC_VIP_SEL_Pos)));
            break;

        case ADC_CH_GPIO7:
            // BUFFER_SEL=11 + VIP_SEL2=0 + VIP_SEL=10
            adc_gpio_init(HAL_GPIO_PIN7);
            slc_rf_spi_reg_or_mask(SLC_RF_SPI_ADDR_CTRL(0xC0), ((0x03 << GPADC_BUFFER_SEL_Pos) |
                                                                    (0x00 << GPADC_VIP_SEL2_Pos) |
                                                                    (0x02 << GPADC_VIP_SEL_Pos)));
            break;

        case ADC_CH_GPIO8:
            // BUFFER_SEL=11 + VIP_SEL2=0 + VIP_SEL=11
            adc_gpio_init(HAL_GPIO_PIN8);
            PRINTF("ADDR:%08x RF_CTRL->ATB_TRXABB_CTRL=%08x\n",&RF_CTRL->ATB_TRXABB_CTRL,RF_CTRL->ATB_TRXABB_CTRL);
            slc_rf_spi_reg_or_mask(SLC_RF_SPI_ADDR_CTRL(0xC0), ((0x03 << GPADC_BUFFER_SEL_Pos) |
                                                                    (0x00 << GPADC_VIP_SEL2_Pos) |
                                                                    (0x03 << GPADC_VIP_SEL_Pos)));
            break;

        // ==================== GPIO差分通道 ====================
        case ADC_CH_GPIO6_GPIO5:
            // BUFFER_SEL=11 + VIP_SEL2=1 + VIP_SEL=00/01 
            slc_rf_spi_reg_or_mask(SLC_RF_SPI_ADDR_CTRL(0xC0), ((0x03 << GPADC_BUFFER_SEL_Pos) |
                                                                    (0x01 << GPADC_VIP_SEL2_Pos) |
                                                                    (0x00 << GPADC_VIP_SEL_Pos)));
            break;

        case ADC_CH_GPIO8_GPIO7:
            // BUFFER_SEL=11 + VIP_SEL2=1 + VIP_SEL=10/11 
            slc_rf_spi_reg_or_mask(SLC_RF_SPI_ADDR_CTRL(0xC0), ((0x03 << GPADC_BUFFER_SEL_Pos) |
                                                                    (0x01 << GPADC_VIP_SEL2_Pos) |
                                                                    (0x02 << GPADC_VIP_SEL_Pos)));
            break;

        default:
            // 默认配置为Vsensor
            //RF_CTRL->GPADC_CTRL |= (0x00 << GPADC_BUFFER_SEL_Pos);
            slc_rf_spi_reg_or_mask(SLC_RF_SPI_ADDR_CTRL(0xC0), (0x00 << GPADC_BUFFER_SEL_Pos));
            break;
    }
}

void qmx_adc_init(ADC_ChannelTypeDef ch)
{
    uint32_t adc_5BW;
    uint32_t adc_5M;
    uint32_t adc_25M;
    uint32_t adc_i;
    uint32_t adc_q;
    double tseensor;

    //RF_PMU->CMU_BUF_CTRL |= 1<<21;
    //RF_PMU->CMU_BUF_CTRL |= 1<<5;
    slc_rf_spi_reg_or_mask(SLC_RF_SPI_ADDR_PMU(0x44), ((1<<21) | (1<<5)));
    if(ch == ADC_CH_TSENSOR){
        //RF_CTRL->TXRF_PD_CTRL |= 1<<30;
        //RF_CTRL->TXRF_PD_CTRL &= ~1<<14;
        slc_rf_spi_reg_or_mask(SLC_RF_SPI_ADDR_CTRL(0x50), (1<<30));
        slc_rf_spi_reg_clr_mask(SLC_RF_SPI_ADDR_CTRL(0x50), (1<<14));
    }
    else if(ch >= ADC_CH_GPIO5){
        ///RF_CTRL->ATB_TRXABB_CTRL &= ~0xFF;
        //RF_CTRL->ATB_TRXABB_CTRL |= 0x10;
        //RF_CTRL->ATB_TRXABB_CTRL |= 1<<16;
        slc_rf_spi_reg_clr_mask(SLC_RF_SPI_ADDR_CTRL(0xD0), (0xFF));
        slc_rf_spi_reg_or_mask(SLC_RF_SPI_ADDR_CTRL(0xD0), (0x10 | (1<<16)));
    }
    
    adc_channel_config(ch);
    //RF_CTRL->RF_MODE_CTRL &= ~GPADC_EN_Msk;
    //RF_CTRL->RF_MODE_CTRL |= 0x01<<GPADC_BUFFER_SEL_Pos;
    slc_rf_spi_reg_clr_mask(SLC_RF_SPI_ADDR_CTRL(0x00), (GPADC_EN_Msk));
    slc_rf_spi_reg_or_mask(SLC_RF_SPI_ADDR_CTRL(0x00), (0x01<<GPADC_BUFFER_SEL_Pos));
    slc_hal_nop_delay_us(35);

    RF_AGC->AGC_BHF_CTRL |= 1<<17;
    RF_AGC->AGC_BHF_CTRL &= ~0xF<<4;
    RF_AGC->AGC_BHF_CTRL |= 2<<4;
    slc_rf_spi_reg_clr_mask(SLC_RF_SPI_ADDR_AGC(0xB8), (GPADC_EN_Msk));
    slc_rf_spi_reg_or_mask(SLC_RF_SPI_ADDR_AGC(0xB8), (0x01<<GPADC_BUFFER_SEL_Pos));
    
    
    RF_DFE->DFE_WORK_MODE = 0x03;
    RF_DFE->DFE_CFG_GPADC_START = 0x01;
    slc_hal_nop_delay_ms(2);

    RF_CTRL->GPADC_CTRL &= ~0x01;

    adc_5BW = RF_DFE->DFE_MNT_GPADC_DATA0 & 0x3FFF;
    adc_5M  = (RF_DFE->DFE_MNT_GPADC_DATA0 & 0x3FFFFFFF)>>16;
    adc_25M = RF_DFE->DFE_MNT_GPADC_DATA1 & 0x1FF;
    PRINTF("adc_5BW=%d  adc_5M=%d  adc_25M=%d\n",adc_5BW,adc_5M,adc_25M);
    //PRINTF("DFE_MNT_GPADC_DATA0=%08x\n",RF_DFE->DFE_MNT_GPADC_DATA0);
    //PRINTF("DFE_MNT_GPADC_DATA1=%08x\n",RF_DFE->DFE_MNT_GPADC_DATA1);
    PRINTF("ADDR:%08x RF_CTRL->RXABB_OUT=%08x\n",&RF_CTRL->RXABB_OUT,RF_CTRL->RXABB_OUT);
    adc_i = RF_CTRL->RXABB_OUT & 0x1FF;
    adc_q = (RF_CTRL->RXABB_OUT & 0x1FFFFF)>>12;
    PRINTF("adc_i=%d  adc_q=%d \n",adc_i,adc_q);

    if(ch == ADC_CH_TSENSOR){
        tseensor = 33+(((double)adc_i - 262.8) / 0.423);
        PRINTF("tseensor=%f\n",tseensor);
    }
    
    double code = (double)adc_i;
    double vin = 4 * (0.5 + ((code-256) * (0.92/512)) ) ;
    PRINTF("V=%f\n",vin);
    
}

void gpadc_test(void)
{
    GPIO->INPUT_EN  &= ~0x1FF;
    GPIO->OUTPUT_EN &= ~0x1FF;
    GPIO->PULL_EN   &= ~0x1FF;
    GPIO->GPIO_AEN  |=  0x1FF;
    PRINTF("GPIO->INPUT_EN=%08x\n",GPIO->INPUT_EN);
    PRINTF("GPIO->OUTPUT_EN=%08x\n",GPIO->OUTPUT_EN);
    PRINTF("GPIO->PULL_EN=%08x\n",GPIO->PULL_EN);
    PRINTF("GPIO->GPIO_AEN=%08x\n",GPIO->GPIO_AEN);
    
    RF_CTRL->ATB_TRXABB_CTRL |= 1<<16;
        
    RF_CTRL->ATB_TRXABB_CTRL &= ~0xff;
    RF_CTRL->ATB_TRXABB_CTRL |= 0xE;
    PRINTF("ADDR:%08x RF_CTRL->ATB_TRXABB_CTRL=%08x\n",&RF_CTRL->ATB_TRXABB_CTRL,RF_CTRL->ATB_TRXABB_CTRL);

    RF_CTRL->ATB_TRXRF_CTRL &= ~7<<8;
    RF_CTRL->ATB_TRXRF_CTRL &= ~7<<16;
    RF_CTRL->ATB_TRXRF_CTRL |= 3<<8;
    RF_CTRL->GPADC_PD_CTRL = 0x70000;
    RF_CTRL->RXABB_PD_CTRL = 0x7FF70AFE;
    RF_CTRL->RXABB_ATB_CTRL = 0x08;
    
    //RF_CTRL->ATB_TRXRF_CTRL = 0x300;
    PRINTF("ADDR:%08x RF_CTRL->ATB_TRXABB_CTRL=%08x\n",&RF_CTRL->ATB_TRXABB_CTRL,RF_CTRL->ATB_TRXABB_CTRL);
    PRINTF("ADDR:%08x RF_CTRL->ATB_TRXRF_CTRL=%08x\n",&RF_CTRL->ATB_TRXRF_CTRL,RF_CTRL->ATB_TRXRF_CTRL);
    PRINTF("ADDR:%08x GPADC_CTRL=%08x\n",&RF_CTRL->GPADC_CTRL,RF_CTRL->GPADC_CTRL);
    PRINTF("ADDR:%08x GPADC_PD_CTRL=%08x\n",&RF_CTRL->GPADC_PD_CTRL,RF_CTRL->GPADC_PD_CTRL);
    PRINTF("ADDR:%08x RXABB_PD_CTRL=%08x\n",&RF_CTRL->RXABB_PD_CTRL,RF_CTRL->RXABB_PD_CTRL);
    PRINTF("ADDR:%08x RXABB_ATB_CTRL=%08x\n",&RF_CTRL->RXABB_ATB_CTRL,RF_CTRL->RXABB_ATB_CTRL);

}






