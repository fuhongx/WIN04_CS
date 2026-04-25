/**
 * ****************************************************************************
 * @file reg_gpio.h
 * @author SLC(SC) software team. 
 * @brief 
 * @version  1.0
 * ****************************************************************************
 * 
 * Copyright (c) 2024 SLC Semiconductor all rights reserved.
 * 
 * ****************************************************************************
 */

#ifndef __REG_GPIO_H__
#define __REG_GPIO_H__

//------------------------------------------------------------------------------
//GPIO Reg list
//------------------------------------------------------------------------------
typedef struct
{
    __IM uint32_t INPUT_LEVEL;              // 0x00
    __IOM uint32_t OUTPUT_LEVEL;            // 0x04
    __IOM uint32_t OUTPUT_EN;               // 0x08
    __IOM uint32_t INPUT_EN;                // 0x0C
    __IOM uint32_t OD_EN;                   // 0x10
    __IOM uint32_t PULL_EN;                 // 0x14
    __IOM uint32_t GPIO_AEN;                // 0x18
    __IOM uint32_t IO_MUX_EN;               // 0x1C
    __IOM uint32_t IO_MUX0;                 // 0x20
    __IOM uint32_t IO_MUX1;                 // 0x24
    __IOM uint32_t IO_MUX2;                 // 0x28
    __IOM uint32_t GPIO_FCR;                // 0x2C
    __IOM uint32_t INT_TYPE;                 // 0x30
    __IOM uint32_t INT_MASK;                // 0x34
    __IOM uint32_t RISE_EDGE_INT_EN;         // 0x38
    __IOM uint32_t FALL_EDGE_INT_EN;         // 0x3C
    __IOM uint32_t LEVEL_INT_SEL;           // 0x40
    __IOM uint32_t INT_FLAG;                // 0x44
    __IOM uint32_t RISE_EDGE_INT_FLAG;       // 0x48
    __IOM uint32_t FALL_EDGE_INT_FLAG;       // 0x4C

}stGPIOHandle_t, *pstGPIOHandle_t;

#define GPIO_MAX_PIN_NUM                    (22)
typedef enum
{
    GPIO_PIN0                               = 0x1 << 0,
    GPIO_PIN1                               = 0x1 << 1,
    GPIO_PIN2                               = 0x1 << 2,
    GPIO_PIN3                               = 0x1 << 3,
    GPIO_PIN4                               = 0x1 << 4,
    GPIO_PIN5                               = 0x1 << 5,
    GPIO_PIN6                               = 0x1 << 6,
    GPIO_PIN7                               = 0x1 << 7,
    GPIO_PIN8                               = 0x1 << 8,
    GPIO_PIN9                               = 0x1 << 9,
    GPIO_PIN10                              = 0x1 << 10,
    GPIO_PIN11                              = 0x1 << 11,
    GPIO_PIN12                              = 0x1 << 12,
    GPIO_PIN13                              = 0x1 << 13,
    GPIO_PIN14                              = 0x1 << 14,
    GPIO_PIN15                              = 0x1 << 15,
    GPIO_PIN16                              = 0x1 << 16,
    GPIO_PIN17                              = 0x1 << 17,
    GPIO_PIN18                              = 0x1 << 18,
    GPIO_PIN19                              = 0x1 << 19,
    GPIO_PIN20                              = 0x1 << 20,
    GPIO_PIN21                              = 0x1 << 21,
}EN_GPIO_PIN_DEF_T;

//------------------------------------------------------------------------------
//0x00 INPUT_LEVEL
//------------------------------------------------------------------------------
#define GPIO_INPUT_LEVEL_REG_MASK               ((1 << GPIO_MAX_PIN_NUM) - 1)

//------------------------------------------------------------------------------
//0x04 OUTPUT_LEVEL
//------------------------------------------------------------------------------
#define GPIO_OUTPUT_LEVEL_REG_MASK              ((1 << GPIO_MAX_PIN_NUM) - 1)

//------------------------------------------------------------------------------
//0x08 OUTPUT_EN
//------------------------------------------------------------------------------
#define GPIO_OUTPUT_EN_REG_MASK                 ((1 << GPIO_MAX_PIN_NUM) - 1)

//------------------------------------------------------------------------------
//0x0C INPUT_EN
//------------------------------------------------------------------------------
#define GPIO_INPUT_EN_REG_MASK                  ((1 << GPIO_MAX_PIN_NUM) - 1)

//------------------------------------------------------------------------------
//0x10 OD_EN
//------------------------------------------------------------------------------
#define GPIO_OD_EN_REG_MASK                     ((1 << GPIO_MAX_PIN_NUM) - 1)

//------------------------------------------------------------------------------
//0x14 PULL EN
//------------------------------------------------------------------------------
#define GPIO_PULL_EN_REG_MASK                   ((1 << GPIO_MAX_PIN_NUM) - 1)

//------------------------------------------------------------------------------
//0x18 PULL MODE
//------------------------------------------------------------------------------
#define GPIO_PULL_MODE_REG_MASK                 ((1 << GPIO_MAX_PIN_NUM) - 1)

//------------------------------------------------------------------------------
//0x1C TURN_RATE
//------------------------------------------------------------------------------
#define GPIO_TURN_RATE_REG_MASK                 ((1 << GPIO_MAX_PIN_NUM) - 1)

//------------------------------------------------------------------------------
//0x20 DRIVEN_STRENGTH0
//------------------------------------------------------------------------------
#define GPIO_DRIVEN_STRENGTH_BIT_WIDE           (4)
#define GPIO_DRIVEN_STRENGTH_BIT_MASK           ((1 << GPIO_DRIVEN_STRENGTH_BIT_WIDE) - 1)


#define GPIO_DRIVEN_STRENGTH0_REG_WIDE          (32)
#define GPIO_DRIVEN_STRENGTH0_REG_MASK          ((0x1 << GPIO_DRIVEN_STRENGTH0_REG_WIDE) - 1)
#define GPIO_DRIVEN_STRENGTH0_MAX_PIN           (7)

//------------------------------------------------------------------------------
//0x24 DRIVEN_STRENGTH1
//------------------------------------------------------------------------------
#define GPIO_DRIVEN_STRENGTH1_REG_WIDE          (32)
#define GPIO_DRIVEN_STRENGTH1_REG_MASK          ((0x1 << GPIO_DRIVEN_STRENGTH1_REG_MASK) - 1)
#define GPIO_DRIVEN_STRENGTH1_MAX_PIN           (15)

//------------------------------------------------------------------------------
//0x28 DRIVEN_STRENGTH2
//------------------------------------------------------------------------------
#define GPIO_DRIVEN_STRENGTH2_REG_WIDE          (32)
#define GPIO_DRIVEN_STRENGTH2_REG_MASK          ((0x1 << GPIO_DRIVEN_STRENGTH2_REG_MASK) - 1)
#define GPIO_DRIVEN_STRENGTH2_MAX_PIN           (23)

//------------------------------------------------------------------------------
//0x30 IO_MUX_EN
//------------------------------------------------------------------------------
#define GPIO_IO_MUX_EN_REG_MASK                 ((1 << GPIO_MAX_PIN_NUM) - 1)

//------------------------------------------------------------------------------
//0x34 IO_MUX0
//------------------------------------------------------------------------------
#define GPIO_IO_MUX_BIT_WIDE                    (4)
#define GPIO_IO_MUX_BIT_MASK                    ((1 << GPIO_IO_MUX_BIT_WIDE) - 1)

#define GPIO_IO_MUX0_REG_MASK                   ((1 << GPIO_MAX_PIN_NUM) - 1)
#define GPIO_IO_MUX0_MAX_PIN                    (7)

//------------------------------------------------------------------------------
//0x38 IO_MUX1
//------------------------------------------------------------------------------
#define GPIO_IO_MUX1_REG_MASK                   ((1 << GPIO_MAX_PIN_NUM) - 1)
#define GPIO_IO_MUX1_MAX_PIN                    (15)

//------------------------------------------------------------------------------
//0x3C IO_MUX2
//------------------------------------------------------------------------------
#define GPIO_IO_MUX2_REG_MASK                   ((1 << GPIO_MAX_PIN_NUM) - 1)
#define GPIO_IO_MUX2_MAX_PIN                    (23)


//------------------------------------------------------------------------------
//SAMPLE_DIV
//------------------------------------------------------------------------------
#define GPIO_SAMPLE_DIV_REG_MASK                (0xFF)

//------------------------------------------------------------------------------
//INT_TYPE
//------------------------------------------------------------------------------
#define GPIO_INT_TYPE_REG_MASK                  ((1 << GPIO_MAX_PIN_NUM) - 1)

//------------------------------------------------------------------------------
//INT_MASK
//------------------------------------------------------------------------------
#define GPIO_INT_MASK_REG_MASK                 ((1 << GPIO_MAX_PIN_NUM) - 1)

//------------------------------------------------------------------------------
//RISE_EDGE_INT_EN
//------------------------------------------------------------------------------
#define GPIO_RISE_EDGE_INT_EN_REG_MASK         ((1 << GPIO_MAX_PIN_NUM) - 1)

//------------------------------------------------------------------------------
//FALL_EDGE_INT_EN
//------------------------------------------------------------------------------
#define GPIO_FALL_EDGE_INT_EN_REG_MASK         ((1 << GPIO_MAX_PIN_NUM) - 1)

//------------------------------------------------------------------------------
//LEVEL_INT_SEL
//------------------------------------------------------------------------------
#define GPIO_LEVEL_INT_SEL_REG_MASK         ((1 << GPIO_MAX_PIN_NUM) - 1)

//------------------------------------------------------------------------------
//INT_FLAG
//------------------------------------------------------------------------------
#define GPIO_INT_FLAG_REG_MASK                 ((1 << GPIO_MAX_PIN_NUM) - 1)

#endif
