/**
 * ****************************************************************************
 * @file hw_timer.c
 * @author SLC(SC) software team.
 * @brief
 * @version  1.0
 * ****************************************************************************
 *
 * THIS CODE IS LIMITED TO GUIDE THE APPLICATION AND IMPROVE THE EFFICIENCY OF RESEARCH AND DEVELOPMENT.
 * AS A RESULT, SILICONDUCTOR IS NOT RESPONSIBLE FOR ANY CLAIMS ARISING DIRECTLY OR INDIRECTLY FROM SUCH FIRMWARE AND/OR CONTENT.
 *
 * Copyright (c) 2025 Siliconductor Semiconductor all rights reserved.
 *
 * ****************************************************************************
 */
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>

#include "reg_Dev.h"
#include "error_def.h"
#include "utility.h"
#include "hw_timer.h"
#include "hw_sysctrl.h"

//------------------------------------------------------------------------------
//NORMAL TIMER & LPTIMER
//------------------------------------------------------------------------------
void rom_hw_timer_lp_delay(stTimerHandle_t* pstHandle)
{
	if((pstHandle == LPTIM0) | (pstHandle == LPTIM1))
	{
		//LP timer need extra delay
		rom_utility_delay_us(LP_RW_DELAY);
	}
}



EN_ERR_STA_T rom_hw_timer_start_counter(stTimerHandle_t* pstHandle)
{
    if(IS_VALID_NORMAL_TIMER(pstHandle))
    {
        pstHandle->TIM_SCR = TIM_SCR_REG_MASK;
		
		rom_hw_timer_lp_delay(pstHandle);
		
        return EN_ERROR_STA_OK;
    }

    return EN_ERROR_STA_INVALID;
}

EN_ERR_STA_T rom_hw_timer_stop_counter(stTimerHandle_t* pstHandle)
{
    if(IS_VALID_NORMAL_TIMER(pstHandle))
    {
        pstHandle->TIM_SCR = (~TIM_CDR_REG_MASK) & TIM_CDR_REG_MASK;
		
		rom_hw_timer_lp_delay(pstHandle);
		
        return EN_ERROR_STA_OK;
    }

    return EN_ERROR_STA_INVALID;
}

EN_ERR_STA_T rom_hw_timer_set_conter_reload_value(stTimerHandle_t* pstHandle, uint32_t u32Value)
{
    if(IS_VALID_NORMAL_TIMER(pstHandle))
    {
        pstHandle->TIM_RDR = u32Value ;
		
		rom_hw_timer_lp_delay(pstHandle);
		
        return EN_ERROR_STA_OK;
    }

    return EN_ERROR_STA_INVALID;
}

EN_ERR_STA_T rom_hw_timer_trig_reload_counter(stTimerHandle_t* pstHandle)
{
	if(IS_VALID_NORMAL_TIMER(pstHandle))
    {
        pstHandle->TIM_CDR = 0;
		
		rom_hw_timer_lp_delay(pstHandle);
		
        return EN_ERROR_STA_OK;
    }
	return EN_ERROR_STA_INVALID;
}

EN_ERR_STA_T rom_hw_timer_get_counter(stTimerHandle_t* pstHandle, uint32_t *pu32Value)
{
    if(IS_VALID_NORMAL_TIMER(pstHandle) & (NULL != pu32Value))
    {
        *pu32Value = pstHandle->TIM_CDR & TIM_CDR_REG_MASK;
		
		rom_hw_timer_lp_delay(pstHandle);
		
        return EN_ERROR_STA_OK;
    }

    return EN_ERROR_STA_INVALID;

}

EN_ERR_STA_T rom_hw_timer_enable_interrupt(stTimerHandle_t* pstHandle)
{
    if(IS_VALID_NORMAL_TIMER(pstHandle) )
    {
        pstHandle->TIM_IER = TIM_IER_REG_MASK;
		
		rom_hw_timer_lp_delay(pstHandle);
		
        return EN_ERROR_STA_OK;
    }

    return EN_ERROR_STA_INVALID;
}

EN_ERR_STA_T rom_hw_timer_disable_interrupt(stTimerHandle_t* pstHandle)
{
    if(IS_VALID_NORMAL_TIMER(pstHandle) )
    {
        pstHandle->TIM_IER = (~TIM_IER_REG_MASK) & TIM_IER_REG_MASK;
		
		rom_hw_timer_lp_delay(pstHandle);
		
        return EN_ERROR_STA_OK;
    }

    return EN_ERROR_STA_INVALID;
}

EN_ERR_STA_T rom_hw_timer_set_interrupt_mask(stTimerHandle_t* pstHandle, bool bEnable)
{
    if(IS_VALID_NORMAL_TIMER(pstHandle) )
    {
        pstHandle->TIM_IMR = (bEnable) & TIM_IMR_REG_MASK;
		
		rom_hw_timer_lp_delay(pstHandle);
		
		
        return EN_ERROR_STA_OK;
    }

    return EN_ERROR_STA_INVALID;
}

EN_ERR_STA_T rom_hw_timer_get_interrupt_flag(stTimerHandle_t* pstHandle, uint8_t* pu8Flag)
{
    if(IS_VALID_NORMAL_TIMER(pstHandle) )
    {
        *pu8Flag = pstHandle->TIM_ISR;

        return EN_ERROR_STA_OK;
    }

    return EN_ERROR_STA_INVALID;
}

EN_ERR_STA_T rom_hw_timer_clear_interrupt_flag(stTimerHandle_t* pstHandle)
{
    if(IS_VALID_NORMAL_TIMER(pstHandle) )
    {
        pstHandle->TIM_ISR = TIM_ISR_REG_MASK;
		
		rom_hw_timer_lp_delay(pstHandle);
		
        return EN_ERROR_STA_OK;
    }

    return EN_ERROR_STA_INVALID;
}


//------------------------------------------------------------------------------
//PWM TIMER
//------------------------------------------------------------------------------
EN_ERR_STA_T rom_hw_pwm_timer_start_counter(EN_PWM_TIMER_CHANNEL_T enCh)
{

    TIM3->TIM123_SCR |= (enCh & TIME123_SCR_CHANNLE_EN_MASK);
    return EN_ERROR_STA_OK;

}

EN_ERR_STA_T rom_hw_pwm_timer_stop_counter(EN_PWM_TIMER_CHANNEL_T enCh)
{

    uint32_t u32Cfg = TIM3->TIM123_SCR;
    switch (enCh)
    {
    case EN_PWM_TIMER_CH0:
        {
            u32Cfg &= ~(TIM123_SCR_TIM1_EN_MASK << TIM123_SCR_TIM1_EN_SHIFT);
            break;
        }
    case EN_PWM_TIMER_CH1:
        {
            u32Cfg &= ~(TIM123_SCR_TIM2_EN_MASK << TIM123_SCR_TIM2_EN_SHIFT);
            break;
        }
    case EN_PWM_TIMER_CH2:
        {
            u32Cfg &= ~(TIM123_SCR_TIM3_EN_MASK << TIM123_SCR_TIM3_EN_SHIFT);
            break;
        }
    default:
        return EN_ERROR_STA_ERROR;
        break;
    }

    TIM3->TIM123_SCR = u32Cfg;
    return EN_ERROR_STA_OK;
}

EN_ERR_STA_T rom_hw_pwm_timer_set_pwm_mode(EN_PWM_TIMER_CHANNEL_T enCh, bool bEnable, EN_PWM_POL_MODE_T enMode)
{
    uint32_t u32Cfg = TIM3->TIM123_SCR;
    switch (enCh)
    {
    case EN_PWM_TIMER_CH0:
        {
            u32Cfg &= ~(TIM123_SCR_PWM1_EN_MASK << TIM123_SCR_PWM1_EN_SHIFT);
            u32Cfg &= ~(TIM123_SCR_PWM1_POL_CTRL_MASK << TIM123_SCR_PWM1_POL_CTRL_SHIFT);
            u32Cfg |= (bEnable & TIM123_SCR_PWM1_EN_MASK) << TIM123_SCR_PWM1_EN_SHIFT;
            u32Cfg |= (enMode & TIM123_SCR_PWM1_POL_CTRL_MASK) << TIM123_SCR_PWM1_POL_CTRL_SHIFT;
            break;
        }
    case EN_PWM_TIMER_CH1:
        {
            u32Cfg &= ~(TIM123_SCR_PWM2_EN_MASK << TIM123_SCR_PWM2_EN_SHIFT);
            u32Cfg &= ~(TIM123_SCR_PWM2_POL_CTRL_MASK << TIM123_SCR_PWM2_POL_CTRL_SHIFT);
            u32Cfg |= (bEnable & TIM123_SCR_PWM2_EN_MASK) << TIM123_SCR_PWM2_EN_SHIFT;
            u32Cfg |= (enMode & TIM123_SCR_PWM2_POL_CTRL_MASK) << TIM123_SCR_PWM2_POL_CTRL_SHIFT;
            break;
        }
    case EN_PWM_TIMER_CH2:
        {
            u32Cfg &= ~(TIM123_SCR_PWM3_EN_MASK << TIM123_SCR_PWM3_EN_SHIFT);
            u32Cfg &= ~(TIM123_SCR_PWM3_POL_CTRL_MASK << TIM123_SCR_PWM3_POL_CTRL_SHIFT);
            u32Cfg |= (bEnable & TIM123_SCR_PWM3_EN_MASK) << TIM123_SCR_PWM3_EN_SHIFT;
            u32Cfg |= (enMode & TIM123_SCR_PWM3_POL_CTRL_MASK) << TIM123_SCR_PWM3_POL_CTRL_SHIFT;
            break;
        }
    default:
        return EN_ERROR_STA_ERROR;
        break;
    }

    TIM3->TIM123_SCR = u32Cfg;

    return EN_ERROR_STA_OK;
}

EN_ERR_STA_T rom_hw_pwm_timer_set_conter_reload_value(EN_PWM_TIMER_CHANNEL_T enCh, uint32_t u32Value)
{
    switch (enCh)
    {
    case EN_PWM_TIMER_CH0:
        {
            TIM3->TIM1_RDR = u32Value;
            break;
        }
    case EN_PWM_TIMER_CH1:
        {
            TIM3->TIM2_RDR = u32Value;
            break;
        }
    case EN_PWM_TIMER_CH2:
        {
            TIM3->TIM3_RDR = u32Value;
            break;
        }
    default:
        return EN_ERROR_STA_ERROR;
        break;
    }

    return EN_ERROR_STA_OK;
}

EN_ERR_STA_T rom_hw_pwm_timer_get_conter(EN_PWM_TIMER_CHANNEL_T enCh, uint32_t* pu32Value)
{
    switch (enCh)
    {
    case EN_PWM_TIMER_CH0:
        {
            *pu32Value = TIM3->TIM1_CDR;
            break;
        }
    case EN_PWM_TIMER_CH1:
        {
            *pu32Value = TIM3->TIM2_CDR;
            break;
        }
    case EN_PWM_TIMER_CH2:
        {
            *pu32Value = TIM3->TIM3_CDR;
            break;
        }
    default:
        return EN_ERROR_STA_ERROR;
        break;
    }

    return EN_ERROR_STA_OK;
}

EN_ERR_STA_T rom_hw_pwm_timer_set_pwm_duty_cycle(EN_PWM_TIMER_CHANNEL_T enCh, uint32_t u32Duty)
{
    switch (enCh)
    {
    case EN_PWM_TIMER_CH0:
        {
            TIM3->PWM1_CCR = u32Duty;
            break;
        }
    case EN_PWM_TIMER_CH1:
        {
            TIM3->PWM2_CCR = u32Duty;
            break;
        }
    case EN_PWM_TIMER_CH2:
        {
            TIM3->PWM3_CCR = u32Duty;
            break;
        }
    default:
        return EN_ERROR_STA_ERROR;
        break;
    }

    return EN_ERROR_STA_OK;
}

EN_ERR_STA_T rom_hw_pwm_timer_get_pwm_duty_cycle(EN_PWM_TIMER_CHANNEL_T enCh, uint32_t* pu32Duty)
{
    switch (enCh)
    {
    case EN_PWM_TIMER_CH0:
        {
            *pu32Duty = TIM3->PWM1_CCR;
            break;
        }
    case EN_PWM_TIMER_CH1:
        {
            *pu32Duty = TIM3->PWM2_CCR;
            break;
        }
    case EN_PWM_TIMER_CH2:
        {
            *pu32Duty = TIM3->PWM3_CCR;
            break;
        }
    default:
        return EN_ERROR_STA_ERROR;
        break;
    }

    return EN_ERROR_STA_OK;
}

EN_ERR_STA_T rom_hw_pwm_timer_enable_interrupt(EN_PWN_TIMER_INT_T enInt)
{
    TIM3->TIM123_IER |= enInt & TIM123_IER_REG_MASK;
    return EN_ERROR_STA_OK;
}

EN_ERR_STA_T rom_hw_pwm_timer_disable_interrupt(EN_PWN_TIMER_INT_T enInt)
{
    TIM3->TIM123_IER &= (~enInt) & TIM123_IER_REG_MASK;
    return EN_ERROR_STA_OK;
}

EN_ERR_STA_T rom_hw_pwm_timer_set_interrupt_mask(uint8_t u8Mask, bool bEnable)
{
    if(bEnable)
    {
        TIM3->TIM123_IMR |= u8Mask & TIM123_IMR_REG_MASK;
    }
    else
    {
        TIM3->TIM123_IMR = (~u8Mask) & TIM123_IMR_REG_MASK;
    }

    return EN_ERROR_STA_OK;
}

EN_ERR_STA_T rom_hw_pwm_timer_get_interrupt_mask(uint8_t* pu8Mask)
{
    *pu8Mask = TIM3->TIM123_IMR & TIM123_IMR_REG_MASK;
    return EN_ERROR_STA_OK;
}

EN_ERR_STA_T rom_hw_pwm_timer_get_interrupt_flag(uint8_t* pu8Flag)
{
    *pu8Flag = TIM3->TIM123_ISR;
    return EN_ERROR_STA_OK;
}

EN_ERR_STA_T rom_hw_pwm_timer_clear_interrupt_flag(uint8_t u8Flag)
{
    TIM3->TIM123_ISR &= u8Flag;
    return EN_ERROR_STA_OK;
}



