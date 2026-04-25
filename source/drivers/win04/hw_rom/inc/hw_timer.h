/**
 * ****************************************************************************
 * @file hw_timer.h
 * @author SLC(SC) software team.
 * @brief
 * @version  1.0
 * ****************************************************************************
 *
 * THIS CODE IS LIMITED TO GUIDE APPLICATION DEVELOPMENT AND INCREASE DEVELOPMENT SPEED.
 * AS A RESULT, SILICONDUCTOR IS NOT RESPONSIBLE FOR ANY CLAIMS ARISING DIRECTLY OR INDIRECTLY FROM SUCH FIRMWARE AND/OR CONTENT.
 *
 * Copyright (c) 2025 Siliconductor Semiconductor all rights reserved.
 *
 * ****************************************************************************
 */

#ifndef __HW_TIMER_H__
#define __HW_TIMER_H__

typedef enum
{
    EN_PWM_TIMER_CH0    =  TIM123_SCR_TIM1_EN_MASK << TIM123_SCR_TIM1_EN_SHIFT,
    EN_PWM_TIMER_CH1    =  TIM123_SCR_TIM2_EN_MASK << TIM123_SCR_TIM2_EN_SHIFT,
    EN_PWM_TIMER_CH2    =  TIM123_SCR_TIM3_EN_MASK << TIM123_SCR_TIM3_EN_SHIFT,
}EN_PWM_TIMER_CHANNEL_T;

typedef enum
{
    EN_PWM_POL_HI_MORE_THAN_CCR      = 0,
    EN_PWM_POL_HI_LESS_THAN_CCR      = 1,
}EN_PWM_POL_MODE_T;

typedef enum
{
    EN_PWM_TIMER_INT_CH1    =   TIM123_IER_TIM3_1_INT_EN_MASK << TIM123_IER_TIM3_1_INT_EN_SHIFT,
    EN_PWN_TIMER_INT_CH2    =   TIM123_IER_TIM3_2_INT_EN_MASK << TIM123_IER_TIM3_2_INT_EN_SHIFT,
    EN_PWM_TIMER_INT_CH3    =   TIM123_IER_TIM3_3_INT_EN_MASK << TIM123_IER_TIM3_3_INT_EN_SHIFT,
}EN_PWN_TIMER_INT_T;


/**
 * @brief Start the Timer/lptimer counter.
 *
 * @param pstHandle Must be TIM0/TIM1/TIM2/LPTIM0/LPTIM1.
 * @return EN_ERR_STA_T
 */
EN_ERR_STA_T rom_hw_timer_start_counter(stTimerHandle_t* pstHandle);

/**
 * @brief Stop the Timer/lptimer counter. Pause counting.
 *
 * @param pstHandle Must be TIM0/TIM1/TIM2/LPTIM0/LPTIM1.
 * @return EN_ERR_STA_T
 */
EN_ERR_STA_T rom_hw_timer_stop_counter(stTimerHandle_t* pstHandle);

/**
 * @brief Configure the Counter overload value (set the desired count value), which will automatically be overloaded into the Counter.
 *
 * @param pstHandle Must be TIM0/TIM1/TIM2/LPTIM0/LPTIM1.
 * @param u32Value Reload value, the counting frequency refers to the system clock.
 * @return EN_ERR_STA_T
 */
EN_ERR_STA_T rom_hw_timer_set_conter_reload_value(stTimerHandle_t* pstHandle, uint32_t u32Value);



EN_ERR_STA_T rom_hw_timer_trig_reload_counter(stTimerHandle_t* pstHandle);

/**
 * @brief Gets the current counter value.
 *
 * @param pstHandle Must be TIM0/TIM1/TIM2/LPTIM0/LPTIM1.
 * @param pu32Value A pointer to the stored count.
 * @return EN_ERR_STA_T
 */
EN_ERR_STA_T rom_hw_timer_get_counter(stTimerHandle_t* pstHandle, uint32_t *pu32Value);

/**
 * @brief Enable timer to interrupt.
 *
 * @param pstHandle Must be TIM0/TIM1/TIM2/LPTIM0/LPTIM1.
 * @return EN_ERR_STA_T
 */
EN_ERR_STA_T rom_hw_timer_enable_interrupt(stTimerHandle_t* pstHandle);

/**
 * @brief Disable timer to interrupt.
 *
 * @param pstHandle Must be TIM0/TIM1/TIM2/LPTIM0/LPTIM1.
 * @return EN_ERR_STA_T
 */
EN_ERR_STA_T rom_hw_timer_disable_interrupt(stTimerHandle_t* pstHandle);

/**
 * @brief Enable timer to interrupt mask.
 *
 * @param pstHandle Must be TIM0/TIM1/TIM2/LPTIM0/LPTIM1.
 * @param bEnable true: Enable, false: Disable.
 * @return EN_ERR_STA_T
 */
EN_ERR_STA_T rom_hw_timer_set_interrupt_mask(stTimerHandle_t* pstHandle, bool bEnable);

/**
 * @brief Gets timer to interrupt mask.
 *
 * @param pstHandle Must be TIM0/TIM1/TIM2/LPTIM0/LPTIM1.
 * @param pu8Flag Pointer to store the interrupt flag buffer.
 * @return EN_ERR_STA_T
 */
EN_ERR_STA_T rom_hw_timer_get_interrupt_flag(stTimerHandle_t* pstHandle, uint8_t* pu8Flag);

/**
 * @brief Clear the timer interrupt.
 *
 * @param pstHandle Must be TIM0/TIM1/TIM2/LPTIM0/LPTIM1.
 * @return EN_ERR_STA_T
 */
EN_ERR_STA_T rom_hw_timer_clear_interrupt_flag(stTimerHandle_t* pstHandle);

/**
 * @brief Start the PWM Timer(TIM3) counter.
 *
 * @param enCh @ref EN_PWM_TIMER_CHANNEL_T.
 * @return EN_ERR_STA_T
 */
EN_ERR_STA_T rom_hw_pwm_timer_start_counter(EN_PWM_TIMER_CHANNEL_T enCh);

/**
 * @brief Stop the PWM Timer(TIM3) counter. Pause counting.
 *
 * @param enCh @ref EN_PWM_TIMER_CHANNEL_T.
 * @return EN_ERR_STA_T
 */
EN_ERR_STA_T rom_hw_pwm_timer_stop_counter(EN_PWM_TIMER_CHANNEL_T enCh);

/**
 * @brief Enable PWM output and configure PWM mode.
 *
 * @param enCh @ref EN_PWM_TIMER_CHANNEL_T.
 * @param bEnable true: enable PWM mode, false: disable PWM mode.
 * @param enMode @ref EN_PWM_POL_MODE_T.
 * @return EN_ERR_STA_T
 */
EN_ERR_STA_T rom_hw_pwm_timer_set_pwm_mode(EN_PWM_TIMER_CHANNEL_T enCh, bool bEnable, EN_PWM_POL_MODE_T enMode);

/**
 * @brief Configure the Counter overload value (set the desired count value), which will automatically be overloaded into the Counter.
 *
 * @param enCh @ref EN_PWM_TIMER_CHANNEL_T.
 * @param u32Value Reload Value, the counting frequency refers to the system clock..
 * @return EN_ERR_STA_T
 */
EN_ERR_STA_T rom_hw_pwm_timer_set_conter_reload_value(EN_PWM_TIMER_CHANNEL_T enCh, uint32_t u32Value);

/**
 * @brief Gets the current counter value.
 *
 * @param enCh @ref EN_PWM_TIMER_CHANNEL_T.
 * @param pu32Value A pointer to the stored count.
 * @return EN_ERR_STA_T
 */
EN_ERR_STA_T rom_hw_pwm_timer_get_conter(EN_PWM_TIMER_CHANNEL_T enCh, uint32_t* pu32Value);

/**
 * @brief Configure PWM duty cycle.
 *
 * @param enCh @ref EN_PWM_TIMER_CHANNEL_T.
 * @param u32Duty
 * @return EN_ERR_STA_T
 */
EN_ERR_STA_T rom_hw_pwm_timer_set_pwm_duty_cycle(EN_PWM_TIMER_CHANNEL_T enCh, uint32_t u32Duty);

/**
 * @brief Read the duty ratio of PWM Channel.
 *
 * @param enCh @ref EN_PWM_TIMER_CHANNEL_T.
 * @param pu32Duty A pointer to the stored value.
 * @return EN_ERR_STA_T
 */
EN_ERR_STA_T rom_hw_pwm_timer_get_pwm_duty_cycle(EN_PWM_TIMER_CHANNEL_T enCh, uint32_t* pu32Duty);

/**
 * @brief Enable PWM timer(TIM3) interrupt.
 *
 * @param enInt @ref EN_PWN_TIMER_INT_T.
 * @return EN_ERR_STA_T
 */
EN_ERR_STA_T rom_hw_pwm_timer_enable_interrupt(EN_PWN_TIMER_INT_T enInt);

/**
 * @brief Disable PWM timer(TIM3) interrupt.
 *
 * @param enInt @ref EN_PWN_TIMER_INT_T.
 * @return EN_ERR_STA_T
 */
EN_ERR_STA_T rom_hw_pwm_timer_disable_interrupt(EN_PWN_TIMER_INT_T enInt);

/**
 * @brief Configure the PWM timer(TIM3) interrupt mask.
 *
 * @param u8Mask
 * @return EN_ERR_STA_T
 */
EN_ERR_STA_T rom_hw_pwm_timer_set_interrupt_mask(uint8_t u8Mask, bool bEnable);

/**
 * @brief Get the PWM timer(TIM3) interrupt mask.
 *
 * @param pu8Mask A pointer to the store interrupt mask.
 * @return EN_ERR_STA_T
 */
EN_ERR_STA_T rom_hw_pwm_timer_get_interrupt_mask(uint8_t* pu8Mask);

/**
 * @brief Gets the interrupt flag of the PWM timer(TIM3).
 *
 * @param pu8Flag Pointer to store interrupt flag.
 * @return EN_ERR_STA_T
 */
EN_ERR_STA_T rom_hw_pwm_timer_get_interrupt_flag(uint8_t* pu8Flag);

/**
 * @brief Clear the interrupt flag of the PWM timer(TIM3).
 *
 * @param u8Flag @ref EN_PWN_TIMER_INT_T.
 * @return EN_ERR_STA_T
 */
EN_ERR_STA_T rom_hw_pwm_timer_clear_interrupt_flag(uint8_t u8Flag);

#endif
