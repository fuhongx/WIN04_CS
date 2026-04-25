/**
 * ****************************************************************************
 * @file qmx_afc.c
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
#include <math.h>

#include "qmx_hal_delay.h"
#include "qmx_cali.h"
#include "debug.h"

qmx_cali_data_t g_qmx_cali_data __attribute__((aligned(4), section(".retention.data"))) = {0};

static const qmx_rccali_t rccalib_table[] = {
    {   0,   0 /* 0b1111111100 */, 0 /* 0b1111111010 */,   2 /* 0b00000010 */,   2 /* 0b00000010 */,  13 /* 0b00001101 */,  14 /* 0b00001110 */},
    { 505,   0 /* 0b1111111100 */, 0 /* 0b1111111011 */,   2 /* 0b00000010 */,   2 /* 0b00000010 */,  13 /* 0b00001101 */,  15 /* 0b00001111 */},
    { 503,   0 /* 0b1111111100 */, 0 /* 0b1111111011 */,   2 /* 0b00000010 */,   2 /* 0b00000010 */,  13 /* 0b00001101 */,  15 /* 0b00001111 */},
    { 501,   0 /* 0b1111111101 */, 0 /* 0b1111111100 */,   2 /* 0b00000010 */,   2 /* 0b00000010 */,  13 /* 0b00001101 */,  16 /* 0b00010000 */},
    { 499,   0 /* 0b1111111101 */, 0 /* 0b1111111100 */,   2 /* 0b00000010 */,   2 /* 0b00000010 */,  13 /* 0b00001101 */,  16 /* 0b00010000 */},
    { 497,   0 /* 0b1111111110 */, 0 /* 0b1111111100 */,   2 /* 0b00000010 */,   2 /* 0b00000010 */,  13 /* 0b00001101 */,  16 /* 0b00010000 */},
    { 494,   0 /* 0b1111111110 */, 0 /* 0b1111111101 */,   2 /* 0b00000010 */,   2 /* 0b00000010 */,  13 /* 0b00001101 */,  17 /* 0b00010001 */},
    { 492,   0 /* 0b1111111111 */, 0 /* 0b1111111101 */,   2 /* 0b00000010 */,   2 /* 0b00000010 */,  14 /* 0b00001110 */,  17 /* 0b00010001 */},
    { 490,   0 /* 0b1111111111 */, 0 /* 0b1111111110 */,   3 /* 0b00000011 */,   2 /* 0b00000010 */,  14 /* 0b00001110 */,  18 /* 0b00010010 */},
    { 488,   0 /* 0b00000000 */,   0 /* 0b1111111110 */,   3 /* 0b00000011 */,   2 /* 0b00000010 */,  14 /* 0b00001110 */,  18 /* 0b00010010 */},
    { 486,   0 /* 0b00000000 */,   0 /* 0b1111111111 */,   3 /* 0b00000011 */,   2 /* 0b00000010 */,  14 /* 0b00001110 */,  19 /* 0b00010011 */},
    { 484,   1 /* 0b00000001 */,   0 /* 0b1111111111 */,   3 /* 0b00000011 */,   2 /* 0b00000010 */,  14 /* 0b00001110 */,  19 /* 0b00010011 */},
    { 481,   1 /* 0b00000001 */,   0 /* 0b00000000 */,   3 /* 0b00000011 */,   2 /* 0b00000010 */,  14 /* 0b00001110 */,  19 /* 0b00010011 */},
    { 479,   1 /* 0b00000001 */,   0 /* 0b00000000 */,   3 /* 0b00000011 */,   2 /* 0b00000010 */,  14 /* 0b00001110 */,  20 /* 0b00010100 */},
    { 477,   2 /* 0b00000010 */,   1 /* 0b00000001 */,   3 /* 0b00000011 */,   2 /* 0b00000010 */,  14 /* 0b00001110 */,  20 /* 0b00010100 */},
    { 475,   2 /* 0b00000010 */,   1 /* 0b00000001 */,   3 /* 0b00000011 */,   3 /* 0b00000011 */,  14 /* 0b00001110 */,  21 /* 0b00010101 */},
    { 473,   3 /* 0b00000011 */,   2 /* 0b00000010 */,   3 /* 0b00000011 */,   3 /* 0b00000011 */,  14 /* 0b00001110 */,  21 /* 0b00010101 */},
    { 470,   3 /* 0b00000011 */,   2 /* 0b00000010 */,   3 /* 0b00000011 */,   3 /* 0b00000011 */,  14 /* 0b00001110 */,  22 /* 0b00010110 */},
    { 468,   4 /* 0b00000100 */,   3 /* 0b00000011 */,   3 /* 0b00000011 */,   3 /* 0b00000011 */,  14 /* 0b00001110 */,  22 /* 0b00010110 */},
    { 466,   4 /* 0b00000100 */,   3 /* 0b00000011 */,   3 /* 0b00000011 */,   3 /* 0b00000011 */,  14 /* 0b00001110 */,  23 /* 0b00010111 */},
    { 464,   5 /* 0b00000101 */,   4 /* 0b00000100 */,   3 /* 0b00000011 */,   3 /* 0b00000011 */,  14 /* 0b00001110 */,  23 /* 0b00010111 */},
    { 462,   5 /* 0b00000101 */,   4 /* 0b00000100 */,   3 /* 0b00000011 */,   3 /* 0b00000011 */,  14 /* 0b00001110 */,  24 /* 0b00011000 */},
    { 459,   6 /* 0b00000110 */,   5 /* 0b00000101 */,   3 /* 0b00000011 */,   3 /* 0b00000011 */,  14 /* 0b00001110 */,  24 /* 0b00011000 */},
    { 457,   7 /* 0b00000111 */,   5 /* 0b00000101 */,   3 /* 0b00000011 */,   3 /* 0b00000011 */,  15 /* 0b00001111 */,  25 /* 0b00011001 */},
    { 455,   7 /* 0b00000111 */,   6 /* 0b00000110 */,   3 /* 0b00000011 */,   3 /* 0b00000011 */,  15 /* 0b00001111 */,  25 /* 0b00011001 */},
    { 453,   8 /* 0b00001000 */,   6 /* 0b00000110 */,   3 /* 0b00000011 */,   3 /* 0b00000011 */,  15 /* 0b00001111 */,  26 /* 0b00011010 */},
    { 451,   8 /* 0b00001000 */,   7 /* 0b00000111 */,   3 /* 0b00000011 */,   3 /* 0b00000011 */,  15 /* 0b00001111 */,  26 /* 0b00011010 */},
    { 449,   9 /* 0b00001001 */,   7 /* 0b00000111 */,   3 /* 0b00000011 */,   3 /* 0b00000011 */,  15 /* 0b00001111 */,  27 /* 0b00011011 */},
    { 446,   9 /* 0b00001001 */,   8 /* 0b00001000 */,   3 /* 0b00000011 */,   3 /* 0b00000011 */,  15 /* 0b00001111 */,  27 /* 0b00011011 */},
    { 444,  10 /* 0b00001010 */,   8 /* 0b00001000 */,   3 /* 0b00000011 */,   3 /* 0b00000011 */,  15 /* 0b00001111 */,  28 /* 0b00011100 */},
    { 442,  10 /* 0b00001010 */,   9 /* 0b00001001 */,   4 /* 0b00000100 */,   3 /* 0b00000011 */,  15 /* 0b00001111 */,  28 /* 0b00011100 */},
    { 440,  11 /* 0b00001011 */,   9 /* 0b00001001 */,   4 /* 0b00000100 */,   3 /* 0b00000011 */,  15 /* 0b00001111 */,  29 /* 0b00011101 */},
    { 438,  12 /* 0b00001100 */,  10 /* 0b00001010 */,   4 /* 0b00000100 */,   3 /* 0b00000011 */,  15 /* 0b00001111 */,  29 /* 0b00011101 */},
    { 435,  12 /* 0b00001100 */,  11 /* 0b00001011 */,   4 /* 0b00000100 */,   3 /* 0b00000011 */,  15 /* 0b00001111 */,  30 /* 0b00011110 */},
    { 433,  13 /* 0b00001101 */,  11 /* 0b00001011 */,   4 /* 0b00000100 */,   4 /* 0b00000100 */,  15 /* 0b00001111 */,  31 /* 0b00011111 */},
    { 431,  13 /* 0b00001101 */,  12 /* 0b00001100 */,   4 /* 0b00000100 */,   4 /* 0b00000100 */,  15 /* 0b00001111 */,  31 /* 0b00011111 */},
    { 429,  14 /* 0b00001110 */,  12 /* 0b00001100 */,   4 /* 0b00000100 */,   4 /* 0b00000100 */,  16 /* 0b00010000 */,  32 /* 0b00100000 */},
    { 427,  14 /* 0b00001110 */,  13 /* 0b00001101 */,   4 /* 0b00000100 */,   4 /* 0b00000100 */,  16 /* 0b00010000 */,  32 /* 0b00100000 */},
    { 425,  15 /* 0b00001111 */,  14 /* 0b00001110 */,   4 /* 0b00000100 */,   4 /* 0b00000100 */,  16 /* 0b00010000 */,  33 /* 0b00100001 */},
    { 422,  16 /* 0b00010000 */,  14 /* 0b00001110 */,   4 /* 0b00000100 */,   4 /* 0b00000100 */,  16 /* 0b00010000 */,  33 /* 0b00100001 */},
    { 420,  16 /* 0b00010000 */,  15 /* 0b00001111 */,   4 /* 0b00000100 */,   4 /* 0b00000100 */,  16 /* 0b00010000 */,  34 /* 0b00100010 */},
    { 418,  17 /* 0b00010001 */,  15 /* 0b00001111 */,   4 /* 0b00000100 */,   4 /* 0b00000100 */,  16 /* 0b00010000 */,  35 /* 0b00100011 */},
    { 416,  18 /* 0b00010010 */,  16 /* 0b00010000 */,   4 /* 0b00000100 */,   4 /* 0b00000100 */,  16 /* 0b00010000 */,  35 /* 0b00100011 */},
    { 414,  18 /* 0b00010010 */,  17 /* 0b00010001 */,   4 /* 0b00000100 */,   4 /* 0b00000100 */,  16 /* 0b00010000 */,  36 /* 0b00100100 */},
    { 411,  19 /* 0b00010011 */,  17 /* 0b00010001 */,   4 /* 0b00000100 */,   4 /* 0b00000100 */,  16 /* 0b00010000 */,  36 /* 0b00100100 */},
    { 409,  20 /* 0b00010100 */,  18 /* 0b00010010 */,   4 /* 0b00000100 */,   4 /* 0b00000100 */,  16 /* 0b00010000 */,  37 /* 0b00100101 */},
    { 407,  20 /* 0b00010100 */,  19 /* 0b00010011 */,   4 /* 0b00000100 */,   4 /* 0b00000100 */,  16 /* 0b00010000 */,  38 /* 0b00100110 */},
    { 405,  21 /* 0b00010101 */,  19 /* 0b00010011 */,   5 /* 0b00000101 */,   4 /* 0b00000100 */,  16 /* 0b00010000 */,  38 /* 0b00100110 */},
    { 403,  22 /* 0b00010110 */,  20 /* 0b00010100 */,   5 /* 0b00000101 */,   4 /* 0b00000100 */,  17 /* 0b00010001 */,  39 /* 0b00100111 */},
    { 401,  22 /* 0b00010110 */,  21 /* 0b00010101 */,   5 /* 0b00000101 */,   4 /* 0b00000100 */,  17 /* 0b00010001 */,  40 /* 0b00101000 */},
    { 398,  23 /* 0b00010111 */,  21 /* 0b00010101 */,   5 /* 0b00000101 */,   5 /* 0b00000101 */,  17 /* 0b00010001 */,  40 /* 0b00101000 */},
    { 396,  24 /* 0b00011000 */,  22 /* 0b00010110 */,   5 /* 0b00000101 */,   5 /* 0b00000101 */,  17 /* 0b00010001 */,  41 /* 0b00101001 */},
    { 394,  24 /* 0b00011000 */,  23 /* 0b00010111 */,   5 /* 0b00000101 */,   5 /* 0b00000101 */,  17 /* 0b00010001 */,  42 /* 0b00101010 */},
    { 392,  25 /* 0b00011001 */,  23 /* 0b00010111 */,   5 /* 0b00000101 */,   5 /* 0b00000101 */,  17 /* 0b00010001 */,  42 /* 0b00101010 */},
    { 390,  26 /* 0b00011010 */,  24 /* 0b00011000 */,   5 /* 0b00000101 */,   5 /* 0b00000101 */,  17 /* 0b00010001 */,  43 /* 0b00101011 */},
    { 387,  27 /* 0b00011011 */,  25 /* 0b00011001 */,   5 /* 0b00000101 */,   5 /* 0b00000101 */,  17 /* 0b00010001 */,  44 /* 0b00101100 */},
    { 385,  27 /* 0b00011011 */,  26 /* 0b00011010 */,   5 /* 0b00000101 */,   5 /* 0b00000101 */,  17 /* 0b00010001 */,  44 /* 0b00101100 */},
    { 383,  28 /* 0b00011100 */,  26 /* 0b00011010 */,   5 /* 0b00000101 */,   5 /* 0b00000101 */,  17 /* 0b00010001 */,  45 /* 0b00101101 */},
    { 381,  29 /* 0b00011101 */,  27 /* 0b00011011 */,   5 /* 0b00000101 */,   5 /* 0b00000101 */,  17 /* 0b00010001 */,  46 /* 0b00101110 */},
    { 379,  30 /* 0b00011110 */,  28 /* 0b00011100 */,   5 /* 0b00000101 */,   5 /* 0b00000101 */,  18 /* 0b00010010 */,  47 /* 0b00101111 */},
    { 377,  30 /* 0b00011110 */,  29 /* 0b00011101 */,   5 /* 0b00000101 */,   5 /* 0b00000101 */,  18 /* 0b00010010 */,  47 /* 0b00101111 */},
    { 374,  31 /* 0b00011111 */,  29 /* 0b00011101 */,   5 /* 0b00000101 */,   5 /* 0b00000101 */,  18 /* 0b00010010 */,  48 /* 0b00110000 */},
    { 372,  32 /* 0b00100000 */,  30 /* 0b00011110 */,   6 /* 0b00000110 */,   5 /* 0b00000101 */,  18 /* 0b00010010 */,  49 /* 0b00110001 */},
    { 370,  33 /* 0b00100001 */,  31 /* 0b00011111 */,   6 /* 0b00000110 */,   5 /* 0b00000101 */,  18 /* 0b00010010 */,  50 /* 0b00110010 */},
    { 368,  34 /* 0b00100010 */,  32 /* 0b00100000 */,   6 /* 0b00000110 */,   6 /* 0b00000110 */,  18 /* 0b00010010 */,  50 /* 0b00110010 */},
    { 366,  34 /* 0b00100010 */,  33 /* 0b00100001 */,   6 /* 0b00000110 */,   6 /* 0b00000110 */,  18 /* 0b00010010 */,  51 /* 0b00110011 */},
    { 363,  35 /* 0b00100011 */,  33 /* 0b00100001 */,   6 /* 0b00000110 */,   6 /* 0b00000110 */,  18 /* 0b00010010 */,  52 /* 0b00110100 */},
    { 361,  36 /* 0b00100100 */,  34 /* 0b00100010 */,   6 /* 0b00000110 */,   6 /* 0b00000110 */,  18 /* 0b00010010 */,  53 /* 0b00110101 */},
    { 359,  37 /* 0b00100101 */,  35 /* 0b00100011 */,   6 /* 0b00000110 */,   6 /* 0b00000110 */,  19 /* 0b00010011 */,  53 /* 0b00110101 */},
    { 357,  38 /* 0b00100110 */,  36 /* 0b00100100 */,   6 /* 0b00000110 */,   6 /* 0b00000110 */,  19 /* 0b00010011 */,  54 /* 0b00110110 */},
    { 355,  39 /* 0b00100111 */,  37 /* 0b00100101 */,   6 /* 0b00000110 */,   6 /* 0b00000110 */,  19 /* 0b00010011 */,  55 /* 0b00110111 */},
    { 353,  40 /* 0b00101000 */,  38 /* 0b00100110 */,   6 /* 0b00000110 */,   6 /* 0b00000110 */,  19 /* 0b00010011 */,  56 /* 0b00111000 */},
    { 350,  40 /* 0b00101000 */,  39 /* 0b00100111 */,   6 /* 0b00000110 */,   6 /* 0b00000110 */,  19 /* 0b00010011 */,  57 /* 0b00111001 */},
    { 348,  41 /* 0b00101001 */,  39 /* 0b00100111 */,   6 /* 0b00000110 */,   6 /* 0b00000110 */,  19 /* 0b00010011 */,  58 /* 0b00111010 */},
    { 346,  42 /* 0b00101010 */,  40 /* 0b00101000 */,   6 /* 0b00000110 */,   6 /* 0b00000110 */,  19 /* 0b00010011 */,  59 /* 0b00111011 */},
    { 344,  43 /* 0b00101011 */,  41 /* 0b00101001 */,   7 /* 0b00000111 */,   7 /* 0b00000111 */,  19 /* 0b00010011 */,  59 /* 0b00111011 */},
    { 342,  44 /* 0b00101100 */,  42 /* 0b00101010 */,   7 /* 0b00000111 */,   7 /* 0b00000111 */,  19 /* 0b00010011 */,  60 /* 0b00111100 */},
    { 339,  45 /* 0b00101101 */,  43 /* 0b00101011 */,   7 /* 0b00000111 */,   7 /* 0b00000111 */,  20 /* 0b00010100 */,  61 /* 0b00111101 */},
    { 337,  46 /* 0b00101110 */,  44 /* 0b00101100 */,   7 /* 0b00000111 */,   7 /* 0b00000111 */,  20 /* 0b00010100 */,  62 /* 0b00111110 */},
    { 335,  47 /* 0b00101111 */,  45 /* 0b00101101 */,   7 /* 0b00000111 */,   7 /* 0b00000111 */,  20 /* 0b00010100 */,  63 /* 0b00111111 */},
    { 333,  48 /* 0b00110000 */,  46 /* 0b00101110 */,   7 /* 0b00000111 */,   7 /* 0b00000111 */,  20 /* 0b00010100 */,  64 /* 0b01000000 */},
    { 331,  49 /* 0b00110001 */,  47 /* 0b00101111 */,   7 /* 0b00000111 */,   7 /* 0b00000111 */,  20 /* 0b00010100 */,  65 /* 0b01000001 */},
    { 329,  50 /* 0b00110010 */,  48 /* 0b00110000 */,   7 /* 0b00000111 */,   7 /* 0b00000111 */,  20 /* 0b00010100 */,  66 /* 0b01000010 */},
    { 326,  51 /* 0b00110011 */,  49 /* 0b00110001 */,   7 /* 0b00000111 */,   7 /* 0b00000111 */,  20 /* 0b00010100 */,  67 /* 0b01000011 */},
    { 324,  52 /* 0b00110100 */,  50 /* 0b00110010 */,   7 /* 0b00000111 */,   7 /* 0b00000111 */,  21 /* 0b00010101 */,  68 /* 0b01000100 */},
    { 322,  53 /* 0b00110101 */,  51 /* 0b00110011 */,   7 /* 0b00000111 */,   8 /* 0b00001000 */,  21 /* 0b00010101 */,  69 /* 0b01000101 */},
    { 320,  54 /* 0b00110110 */,  52 /* 0b00110100 */,   8 /* 0b00001000 */,   8 /* 0b00001000 */,  21 /* 0b00010101 */,  70 /* 0b01000110 */},
    { 318,  55 /* 0b00110111 */,  53 /* 0b00110101 */,   8 /* 0b00001000 */,   8 /* 0b00001000 */,  21 /* 0b00010101 */,  71 /* 0b01000111 */},
    { 315,  56 /* 0b00111000 */,  54 /* 0b00110110 */,   8 /* 0b00001000 */,   8 /* 0b00001000 */,  21 /* 0b00010101 */,  72 /* 0b01001000 */},
    { 313,  58 /* 0b00111010 */,  55 /* 0b00110111 */,   8 /* 0b00001000 */,   8 /* 0b00001000 */,  21 /* 0b00010101 */,  73 /* 0b01001001 */},
    { 311,  59 /* 0b00111011 */,  57 /* 0b00111001 */,   8 /* 0b00001000 */,   8 /* 0b00001000 */,  21 /* 0b00010101 */,  74 /* 0b01001010 */},
    { 309,  60 /* 0b00111100 */,  58 /* 0b00111010 */,   8 /* 0b00001000 */,   8 /* 0b00001000 */,  22 /* 0b00010110 */,  75 /* 0b01001011 */},
    { 307,  61 /* 0b00111101 */,  59 /* 0b00111011 */,   8 /* 0b00001000 */,   8 /* 0b00001000 */,  22 /* 0b00010110 */,  76 /* 0b01001100 */},
    { 305,  62 /* 0b00111110 */,  60 /* 0b00111100 */,   8 /* 0b00001000 */,   8 /* 0b00001000 */,  22 /* 0b00010110 */,  77 /* 0b01001101 */},
    { 302,  63 /* 0b00111111 */,  61 /* 0b00111101 */,   8 /* 0b00001000 */,   9 /* 0b00001001 */,  22 /* 0b00010110 */,  79 /* 0b01001111 */},
    { 300,  65 /* 0b01000001 */,  62 /* 0b00111110 */,   9 /* 0b00001001 */,   9 /* 0b00001001 */,  22 /* 0b00010110 */,  80 /* 0b01010000 */},
    { 298,  66 /* 0b01000010 */,  64 /* 0b01000000 */,   9 /* 0b00001001 */,   9 /* 0b00001001 */,  22 /* 0b00010110 */,  81 /* 0b01010001 */},
    { 296,  67 /* 0b01000011 */,  65 /* 0b01000001 */,   9 /* 0b00001001 */,   9 /* 0b00001001 */,  23 /* 0b00010111 */,  82 /* 0b01010010 */},
    { 294,  68 /* 0b01000100 */,  66 /* 0b01000010 */,   9 /* 0b00001001 */,   9 /* 0b00001001 */,  23 /* 0b00010111 */,  83 /* 0b01010011 */},
    { 291,  70 /* 0b01000110 */,  67 /* 0b01000011 */,   9 /* 0b00001001 */,   9 /* 0b00001001 */,  23 /* 0b00010111 */,  85 /* 0b01010101 */},
    { 289,  71 /* 0b01000111 */,  69 /* 0b01000101 */,   9 /* 0b00001001 */,   9 /* 0b00001001 */,  23 /* 0b00010111 */,  86 /* 0b01010110 */},
    { 287,  72 /* 0b01001000 */,  70 /* 0b01000110 */,   9 /* 0b00001001 */,   9 /* 0b00001001 */,  23 /* 0b00010111 */,  87 /* 0b01010111 */},
    { 285,  74 /* 0b01001010 */,  71 /* 0b01000111 */,   9 /* 0b00001001 */,  10 /* 0b00001010 */,  23 /* 0b00010111 */,  88 /* 0b01011000 */},
    { 283,  75 /* 0b01001011 */,  73 /* 0b01001001 */,   9 /* 0b00001001 */,  10 /* 0b00001010 */,  24 /* 0b00011000 */,  90 /* 0b01011010 */},
    { 281,  77 /* 0b01001101 */,  74 /* 0b01001010 */,  10 /* 0b00001010 */,  10 /* 0b00001010 */,  24 /* 0b00011000 */,  91 /* 0b01011011 */},
    { 278,  78 /* 0b01001110 */,  76 /* 0b01001100 */,  10 /* 0b00001010 */,  10 /* 0b00001010 */,  24 /* 0b00011000 */,  92 /* 0b01011100 */},
    { 276,  79 /* 0b01001111 */,  77 /* 0b01001101 */,  10 /* 0b00001010 */,  10 /* 0b00001010 */,  24 /* 0b00011000 */,  94 /* 0b01011110 */},
    { 274,  81 /* 0b01010001 */,  78 /* 0b01001110 */,  10 /* 0b00001010 */,  10 /* 0b00001010 */,  24 /* 0b00011000 */,  95 /* 0b01011111 */},
    { 272,  82 /* 0b01010010 */,  80 /* 0b01010000 */,  10 /* 0b00001010 */,  10 /* 0b00001010 */,  25 /* 0b00011001 */,  97 /* 0b01100001 */},
    { 270,  84 /* 0b01010100 */,  81 /* 0b01010001 */,  10 /* 0b00001010 */,  11 /* 0b00001011 */,  25 /* 0b00011001 */,  98 /* 0b01100010 */},
    { 267,  85 /* 0b01010101 */,  83 /* 0b01010011 */,  10 /* 0b00001010 */,  11 /* 0b00001011 */,  25 /* 0b00011001 */,  99 /* 0b01100011 */},
    { 265,  87 /* 0b01010111 */,  84 /* 0b01010100 */,  11 /* 0b00001011 */,  11 /* 0b00001011 */,  25 /* 0b00011001 */, 101 /* 0b01100101 */},
    { 263,  89 /* 0b01011001 */,  86 /* 0b01010110 */,  11 /* 0b00001011 */,  11 /* 0b00001011 */,  25 /* 0b00011001 */, 102 /* 0b01100110 */},
    { 261,  90 /* 0b01011010 */,  88 /* 0b01011000 */,  11 /* 0b00001011 */,  11 /* 0b00001011 */,  26 /* 0b00011010 */, 104 /* 0b01101000 */},
    { 259,  92 /* 0b01011100 */,  89 /* 0b01011001 */,  11 /* 0b00001011 */,  11 /* 0b00001011 */,  26 /* 0b00011010 */, 105 /* 0b01101001 */},
    { 256,  93 /* 0b01011101 */,  91 /* 0b01011011 */,  11 /* 0b00001011 */,  11 /* 0b00001011 */,  26 /* 0b00011010 */, 107 /* 0b01101011 */},
    { 254,  95 /* 0b01011111 */,  93 /* 0b01011101 */,  11 /* 0b00001011 */,  12 /* 0b00001100 */,  26 /* 0b00011010 */, 109 /* 0b01101101 */},
    { 252,  97 /* 0b01100001 */,  94 /* 0b01011110 */,  12 /* 0b00001100 */,  12 /* 0b00001100 */,  26 /* 0b00011010 */, 110 /* 0b01101110 */},
    { 250,  99 /* 0b01100011 */,  96 /* 0b01100000 */,  12 /* 0b00001100 */,  12 /* 0b00001100 */,  27 /* 0b00011011 */, 112 /* 0b01110000 */},
    { 248, 100 /* 0b01100100 */,  98 /* 0b01100010 */,  12 /* 0b00001100 */,  12 /* 0b00001100 */,  27 /* 0b00011011 */, 114 /* 0b01110010 */},
    { 246, 102 /* 0b01100110 */, 100 /* 0b01100100 */,  12 /* 0b00001100 */,  12 /* 0b00001100 */,  27 /* 0b00011011 */, 115 /* 0b01110011 */},
    { 243, 104 /* 0b01101000 */, 101 /* 0b01100101 */,  12 /* 0b00001100 */,  13 /* 0b00001101 */,  27 /* 0b00011011 */, 117 /* 0b01110101 */},
    { 241, 106 /* 0b01101010 */, 103 /* 0b01100111 */,  12 /* 0b00001100 */,  13 /* 0b00001101 */,  28 /* 0b00011100 */, 119 /* 0b01110111 */},
    { 239, 108 /* 0b01101100 */, 105 /* 0b01101001 */,  13 /* 0b00001101 */,  13 /* 0b00001101 */,  28 /* 0b00011100 */, 121 /* 0b01111001 */},
    { 237, 110 /* 0b01101110 */, 107 /* 0b01101011 */,  13 /* 0b00001101 */,  13 /* 0b00001101 */,  28 /* 0b00011100 */, 123 /* 0b01111011 */},
    { 235, 112 /* 0b01110000 */, 109 /* 0b01101101 */,  13 /* 0b00001101 */,  13 /* 0b00001101 */,  28 /* 0b00011100 */, 125 /* 0b01111101 */},
    { 232, 114 /* 0b01110010 */, 111 /* 0b01101111 */,  13 /* 0b00001101 */,  14 /* 0b00001110 */,  29 /* 0b00011101 */, 126 /* 0b01111110 */},
    { 230, 116 /* 0b01110100 */, 113 /* 0b01110001 */,  13 /* 0b00001101 */,  14 /* 0b00001110 */,  29 /* 0b00011101 */, 127 /* 0b01111111 */},
    { 228, 118 /* 0b01110110 */, 115 /* 0b01110011 */,  13 /* 0b00001101 */,  14 /* 0b00001110 */,  29 /* 0b00011101 */, 127 /* 0b01111111 */},
    { 226, 120 /* 0b01111000 */, 117 /* 0b01110101 */,  14 /* 0b00001110 */,  14 /* 0b00001110 */,  30 /* 0b00011110 */, 127 /* 0b01111111 */},
    { 224, 123 /* 0b01111011 */, 120 /* 0b01111000 */,  14 /* 0b00001110 */,  14 /* 0b00001110 */,  30 /* 0b00011110 */, 127 /* 0b01111111 */},
    { 222, 125 /* 0b01111101 */, 122 /* 0b01111010 */,  14 /* 0b00001110 */,  15 /* 0b00001111 */,  30 /* 0b00011110 */, 127 /* 0b01111111 */},
    { 219, 127 /* 0b01111111 */, 124 /* 0b01111100 */,  14 /* 0b00001110 */,  15 /* 0b00001111 */,  30 /* 0b00011110 */, 127 /* 0b01111111 */},
    { 217, 127 /* 0b01111111 */, 126 /* 0b01111110 */,  14 /* 0b00001110 */,  15 /* 0b00001111 */,  31 /* 0b00011111 */, 127 /* 0b01111111 */},
    { 215, 127 /* 0b01111111 */, 127 /* 0b01111111 */,  15 /* 0b00001111 */,  15 /* 0b00001111 */,  31 /* 0b00011111 */, 127 /* 0b01111111 */},
    { 213, 127 /* 0b01111111 */, 127 /* 0b01111111 */,  15 /* 0b00001111 */,  15 /* 0b00001111 */,  31 /* 0b00011111 */, 127 /* 0b01111111 */},
    { 211, 127 /* 0b01111111 */, 127 /* 0b01111111 */,  15 /* 0b00001111 */,  15 /* 0b00001111 */,  32 /* 0b00100000 */, 127 /* 0b01111111 */},
    { 208, 127 /* 0b01111111 */, 127 /* 0b01111111 */,  15 /* 0b00001111 */,  15 /* 0b00001111 */,  32 /* 0b00100000 */, 127 /* 0b01111111 */},
    { 206, 127 /* 0b01111111 */, 127 /* 0b01111111 */,  15 /* 0b00001111 */,  15 /* 0b00001111 */,  32 /* 0b00100000 */, 127 /* 0b01111111 */},
    { 204, 127 /* 0b01111111 */, 127 /* 0b01111111 */,  15 /* 0b00001111 */,  15 /* 0b00001111 */,  33 /* 0b00100001 */, 127 /* 0b01111111 */},
    { 202, 127 /* 0b01111111 */, 127 /* 0b01111111 */,  15 /* 0b00001111 */,  15 /* 0b00001111 */,  33 /* 0b00100001 */, 127 /* 0b01111111 */},
    { 200, 127 /* 0b01111111 */, 127 /* 0b01111111 */,  15 /* 0b00001111 */,  15 /* 0b00001111 */,  33 /* 0b00100001 */, 127 /* 0b01111111 */},
    { 198, 127 /* 0b01111111 */, 127 /* 0b01111111 */,  15 /* 0b00001111 */,  15 /* 0b00001111 */,  34 /* 0b00100010 */, 127 /* 0b01111111 */},
    { 195, 127 /* 0b01111111 */, 127 /* 0b01111111 */,  15 /* 0b00001111 */,  15 /* 0b00001111 */,  34 /* 0b00100010 */, 127 /* 0b01111111 */},
    { 193, 127 /* 0b01111111 */, 127 /* 0b01111111 */,  15 /* 0b00001111 */,  15 /* 0b00001111 */,  35 /* 0b00100011 */, 127 /* 0b01111111 */},
    { 191, 127 /* 0b01111111 */, 127 /* 0b01111111 */,  15 /* 0b00001111 */,  15 /* 0b00001111 */,  35 /* 0b00100011 */, 127 /* 0b01111111 */},
    { 189, 127 /* 0b01111111 */, 127 /* 0b01111111 */,  15 /* 0b00001111 */,  15 /* 0b00001111 */,  35 /* 0b00100011 */, 127 /* 0b01111111 */},
    { 187, 127 /* 0b01111111 */, 127 /* 0b01111111 */,  15 /* 0b00001111 */,  15 /* 0b00001111 */,  36 /* 0b00100100 */, 127 /* 0b01111111 */},
    { 184, 127 /* 0b01111111 */, 127 /* 0b01111111 */,  15 /* 0b00001111 */,  15 /* 0b00001111 */,  36 /* 0b00100100 */, 127 /* 0b01111111 */},
    { 182, 127 /* 0b01111111 */, 127 /* 0b01111111 */,  15 /* 0b00001111 */,  15 /* 0b00001111 */,  37 /* 0b00100101 */, 127 /* 0b01111111 */},
    { 180, 127 /* 0b01111111 */, 127 /* 0b01111111 */,  15 /* 0b00001111 */,  15 /* 0b00001111 */,  37 /* 0b00100101 */, 127 /* 0b01111111 */},
    { 178, 127 /* 0b01111111 */, 127 /* 0b01111111 */,  15 /* 0b00001111 */,  15 /* 0b00001111 */,  38 /* 0b00100110 */, 127 /* 0b01111111 */},
    { 176, 127 /* 0b01111111 */, 127 /* 0b01111111 */,  15 /* 0b00001111 */,  15 /* 0b00001111 */,  38 /* 0b00100110 */, 127 /* 0b01111111 */},
    { 174, 127 /* 0b01111111 */, 127 /* 0b01111111 */,  15 /* 0b00001111 */,  15 /* 0b00001111 */,  38 /* 0b00100110 */, 127 /* 0b01111111 */},
    { 171, 127 /* 0b01111111 */, 127 /* 0b01111111 */,  15 /* 0b00001111 */,  15 /* 0b00001111 */,  39 /* 0b00100111 */, 127 /* 0b01111111 */},
    { 169, 127 /* 0b01111111 */, 127 /* 0b01111111 */,  15 /* 0b00001111 */,  15 /* 0b00001111 */,  39 /* 0b00100111 */, 127 /* 0b01111111 */},
    { 167, 127 /* 0b01111111 */, 127 /* 0b01111111 */,  15 /* 0b00001111 */,  15 /* 0b00001111 */,  40 /* 0b00101000 */, 127 /* 0b01111111 */},
    { 165, 127 /* 0b01111111 */, 127 /* 0b01111111 */,  15 /* 0b00001111 */,  15 /* 0b00001111 */,  41 /* 0b00101001 */, 127 /* 0b01111111 */},
    { 163, 127 /* 0b01111111 */, 127 /* 0b01111111 */,  15 /* 0b00001111 */,  15 /* 0b00001111 */,  41 /* 0b00101001 */, 127 /* 0b01111111 */},
    { 160, 127 /* 0b01111111 */, 127 /* 0b01111111 */,  15 /* 0b00001111 */,  15 /* 0b00001111 */,  42 /* 0b00101010 */, 127 /* 0b01111111 */},
    { 158, 127 /* 0b01111111 */, 127 /* 0b01111111 */,  15 /* 0b00001111 */,  15 /* 0b00001111 */,  42 /* 0b00101010 */, 127 /* 0b01111111 */},
};

#define RCCALIB_TABLE_SIZE (sizeof(rccalib_table) / sizeof(rccalib_table[0]))

int qmx_clk_cali(qmx_cali_clk_e clk)
{
    uint32_t timeout = 0;

    switch (clk)
    {
        case QMX_CALI_RC32K:
            RF_CTRL->RF_CALI_CTRL |= QMX_RFCTRL_CAL_RC32K_START_VAL(1);
            do {
                if ((RF_CTRL->RF_CALI_CTRL & QMX_RFCTRL_CAL_RC32K_START_MASK) == 0)
                    break;

                qmx_hal_nop_delay_us(10);
                timeout += 10;
            } while (timeout < QMX_CALI_CLK_RC32K_TIMEOUT_US);

            if (timeout >= QMX_CALI_CLK_RC32K_TIMEOUT_US)
                return EN_ERROR_STA_TIMEOUT;

            g_qmx_cali_data.rc32k_ctrim = (RF_PMU->RC32K_CTRL & QMX_CMU_OSC32K_FREQ_CTRIM_MASK) >> QMX_CMU_OSC32K_FREQ_CTRIM_SHIFT;
            g_qmx_cali_data.rc32k_rtrim = (RF_PMU->RC32K_CTRL & QMX_CMU_OSC32K_FREQ_RTRIM_MASK) >> QMX_CMU_OSC32K_FREQ_RTRIM_SHIFT;

            break;

        case QMX_CALI_RC50M:
            RF_CTRL->RF_CALI_CTRL |= QMX_RFCTRL_CAL_RC50M_START_VAL(1);
            do {
                if ((RF_CTRL->RF_CALI_CTRL & QMX_RFCTRL_CAL_RC50M_START_MASK) == 0)
                    break;

                qmx_hal_nop_delay_us(10);
                timeout += 10;
            } while (timeout < QMX_CALI_CLK_RC50M_TIMEOUT_US);

            if (timeout >= QMX_CALI_CLK_RC50M_TIMEOUT_US)
                return EN_ERROR_STA_TIMEOUT;

            g_qmx_cali_data.rc50m_ctune = (RF_PMU->RC50M_CTRL & QMX_CMU_OSC50M_FREQ_CTUNE_MASK) >> QMX_CMU_OSC50M_FREQ_CTUNE_SHIFT;
            g_qmx_cali_data.rc50m_rtune = (RF_PMU->RC50M_CTRL & QMX_CMU_OSC50M_FREQ_RTUNE_MASK) >> QMX_CMU_OSC50M_FREQ_RTUNE_SHIFT;

            break;

        case QMX_CALI_DCDC1M:
            RF_CTRL->RF_CALI_CTRL |= QMX_RFCTRL_CAL_DCDC1M_START_VAL(1);
            do {
                if ((RF_CTRL->RF_CALI_CTRL & QMX_RFCTRL_CAL_DCDC1M_START_MASK) == 0)
                    break;

                qmx_hal_nop_delay_us(10);
                timeout += 10;
            } while (timeout < QMX_CALI_CLK_DCDC1M_TIMEOUT_US);

            if (timeout >= QMX_CALI_CLK_DCDC1M_TIMEOUT_US)
                return EN_ERROR_STA_TIMEOUT;

            g_qmx_cali_data.dcdc1m_captrim = (RF_PMU->DCDC_CTRL & QMX_DCDC_OSC_CAPTRIM_MASK) >> QMX_DCDC_OSC_CAPTRIM_SHIFT;

            break;

        default:
            return EN_ERROR_STA_INVALID;
    }

    return EN_ERROR_STA_OK;
}

int qmx_r_cali(void)
{
    uint32_t timeout = 0;

    RF_CTRL->RF_CALI_CTRL |= QMX_RFCTRL_CAL_PMU_START_VAL(1);

    do {
        if ((RF_CTRL->RF_CALI_CTRL & QMX_RFCTRL_CAL_PMU_START_MASK) == 0)
            break;

        qmx_hal_nop_delay_us(10);
        timeout += 10;
    } while (timeout < QMX_CALI_R_TIMEOUT_US);

    if (timeout >= QMX_CALI_R_TIMEOUT_US) {
        PRINTF("R CALI timeout\n");
        return EN_ERROR_STA_TIMEOUT;
    }

    g_qmx_cali_data.pmu_iout_tune_trim = (RF_PMU->PMU_CTRL & QMX_PMU_IOUT_TUNE_TRIM_MASK) >> QMX_PMU_IOUT_TUNE_TRIM_SHIFT;
    g_qmx_cali_data.pmu_iout_ptat_trim = (RF_PMU->PMU_CTRL & QMX_PMU_IOUT_PTAT_TRIM_MASK) >> QMX_PMU_IOUT_PTAT_TRIM_SHIFT;

    return EN_ERROR_STA_OK;
}

void find_rccali_params(uint16_t sum_t1_t2)
{
    uint16_t max_index = 0;
    uint32_t i = 0;
    uint32_t result_idx = 0;

    if (sum_t1_t2 > rccalib_table[1].decision_threshold) {
        PRINTF("[WARN] t1+t2=%d overflow\n", sum_t1_t2);

        result_idx = 0;
        goto find_result;
    }

    if (sum_t1_t2 <= rccalib_table[RCCALIB_TABLE_SIZE - 1].decision_threshold) {
        if (sum_t1_t2 < rccalib_table[RCCALIB_TABLE_SIZE - 1].decision_threshold)
            PRINTF("[WARN] t1+t2=%d underflow\n", sum_t1_t2);

        result_idx = RCCALIB_TABLE_SIZE - 1;
        goto find_result;
    }

    // 根据表可看出decision_threshold大部分都是向下递减2，所以可以简化为除以2求得最大可能的索引
    max_index = (rccalib_table[1].decision_threshold - sum_t1_t2) / 2;
    if (max_index >= RCCALIB_TABLE_SIZE) {
        max_index = RCCALIB_TABLE_SIZE - 2;
    }

    // 从最大索引开始逐次查表, 此方法理论最大查表次数为14次
    for (i = max_index; i > 0; i--) {
        if (sum_t1_t2 <= rccalib_table[i].decision_threshold &&
            sum_t1_t2 > rccalib_table[i+1].decision_threshold) {
            result_idx = i;
            goto find_result;
        }
    }

find_result:
    PRINTF("t1+t2=%d, decision_threshold=%d\n", sum_t1_t2, rccalib_table[result_idx].decision_threshold);

    if (result_idx <= QMX_MAX_RXABB_CBPF_CTRIM_IDX)
        PRINTF("[WARN] RXABB_CBPF_CTRIM overflow, use 0\n");

    if (result_idx <= QMX_MAX_LPF_CTRIM_IDX)
        PRINTF("[WARN] LPF_CTRIM overflow, use 0\n");

    g_qmx_cali_data.rc_cali_idx = result_idx;

    RF_CTRL->RXABB_CBPF_CTRL &= ~RFCTRL_RXABB_CBPF_CTRIM_MASK;
    RF_CTRL->TXABB_LPF_TRIM &= ~QMX_RFCTRL_TXLPF_CTRIM_MASK;
    RF_CTRL->RXRF_MIX_TIA_CTRL &= ~QMX_RFCTRL_RXTIA_CT_CTUNE_MASK;
    RF_PMU->FDB_50M_TUNE &= ~QMX_CMU_FDOUB_CTUNE_MASK;
    RF_PLL->RFPLL_RESERVE &= ~RFPLL_RESERVE_IN_MASK;
    RF_PLL->DAC_CTRL &= ~RFPLL_DAC_TIA_CTRIM_MASK;

    RF_CTRL->RXABB_CBPF_CTRL |= RFCTRL_RXABB_CBPF_CTRIM_VAL(rccalib_table[result_idx].rxabb_cbpf_ctrim);
    RF_CTRL->TXABB_LPF_TRIM |= QMX_RFCTRL_TXLPF_CTRIM_VAL(rccalib_table[result_idx].lpf_ctrim);
    RF_CTRL->RXRF_MIX_TIA_CTRL |= QMX_RFCTRL_RXTIA_CT_CTUNE_VAL(rccalib_table[result_idx].tia);
    RF_PMU->FDB_50M_TUNE |= QMX_CMU_FDOUB_CTUNE_VAL(rccalib_table[result_idx].freq_doubler);
    RF_PLL->RFPLL_RESERVE |= RFPLL_RESERVE_IN_VAL(rccalib_table[result_idx].lo_ppf);
    RF_PLL->DAC_CTRL |= RFPLL_DAC_TIA_CTRIM_VAL(rccalib_table[result_idx].pll_dac_lpf);
}

int qmx_rc_cali(void)
{
    uint32_t timeout = 0;
    uint16_t rc_calib_cnt1 = 0;
    uint16_t rc_calib_cnt2 = 0;
    uint32_t sum_t1_t2 = 0;

    RF_CTRL->RF_CALI_CTRL |= QMX_RFCTRL_CAL_RXABB_START_VAL(1);

    do {
        if ((RF_CTRL->RF_CALI_CTRL & QMX_RFCTRL_CAL_RXABB_START_MASK) == 0)
            break;

        qmx_hal_nop_delay_us(10);
        timeout += 10;
    } while (timeout < QMX_CALI_RC_TIMEOUT_US);

    if (timeout >= QMX_CALI_RC_TIMEOUT_US) {
        PRINTF("RC CALI timeout\n");
        return EN_ERROR_STA_TIMEOUT;
    }

    rc_calib_cnt1 = (RF_CTRL->CAL_RC_RPT & QMX_RC_CALIB_CNT1_MASK) >> QMX_RC_CALIB_CNT1_SHIFT;
    rc_calib_cnt2 = (RF_CTRL->CAL_RC_RPT & QMX_RC_CALIB_CNT2_MASK) >> QMX_RC_CALIB_CNT2_SHIFT;
    sum_t1_t2 = rc_calib_cnt1 + rc_calib_cnt2;

    find_rccali_params(sum_t1_t2);

    return EN_ERROR_STA_OK;
}

int qmx_afc_cali(uint32_t freq_hz, int32_t inter_freq_hz, bool tx, bool polar)
{
    uint32_t timeout = 0;
    uint32_t remainder_hz = freq_hz % 1000000;
    uint64_t temp_frac = (uint64_t)remainder_hz * (1 << 24);
    uint32_t frac_24bit = (uint32_t)(temp_frac / 1000000);
    uint32_t int_11bit = freq_hz / 1000000;
    float inter_freq_mhz = (float)inter_freq_hz / 1000000.0;
    int32_t inter_freq = (int32_t)round(inter_freq_mhz * (1 << 24));

    RF_PLL->TX_FREQ_INT = PLL_TX_FREQ_INT_VAL(int_11bit);
    RF_PLL->TX_FREQ_FRAC = PLL_TX_FREQ_FRAC_VAL(frac_24bit);
    RF_PLL->INTER_FREQ = PLL_INTER_FREQ_VAL(inter_freq);

    // TODO: AFC校准试ramping cycle若不设置小些，校准切换时间会达到毫秒级别，此处临时
    RF_CTRL->TX_RAMPING_CTRL &= ~QMX_RFCTRL_TX_RAMP_CYCLE_MASK;

    RF_CTRL->RF_MODE_CTRL &= ~(QMX_RFCTRL_TX_EN_MO_MASK | QMX_RFCTRL_RX_EN_MO_MASK);
    RF_CTRL->RF_MODE_CTRL |= (QMX_RFCTRL_TX_EN_ME_VAL(1) | QMX_RFCTRL_RX_EN_ME_VAL(1));

    do {
        if ((RF_PLL->PLLDIG_CTRL & PLL_RPT_AFC_DONE_MASK) == 0)
            break;

        qmx_hal_nop_delay_us(1);
        timeout += 1;
    } while (timeout < 10);

    if (timeout >= 10) {
        PRINTF("wait AFC CALI start timeout\n");
        return EN_ERROR_STA_TIMEOUT;
    }
    timeout = 0;

    RF_PLL->DTEMP_CTRL2 |= 0x300;
    RF_PLL->DTEMP_CTRL0 |= 0x300;
    RF_PLL->DTEMP_CTRL5 |= 0x100;
    RF_PLL->DTEMP_CTRL5 &= ~0x1FF;
    RF_PLL->DTEMP_CTRL5 |= 0x180;

    if (tx) {
        if (polar)
            RF_PLL->PD_DAC_CTRL |= 0x200;
        else
            RF_PLL->PD_DAC_CTRL &= ~0x200;

        RF_PLL->PK_CTRL &= ~RFPLL_PEAKDET_CON_VREF_1P1_MASK;
        RF_PLL->PK_CTRL |= RFPLL_PEAKDET_CON_VREF_1P1_VAL(QMX_PEAKDET_VREF_1P1_TX_CODE);

        RF_PLL->FSM_CTRL &= ~RFPLL_FSM_SKIPPER_MASK;
        RF_PLL->FSM_CTRL |= RFPLL_FSM_SKIPPER_VAL(8);   // skip KVCO2
        RF_PLL->PLLDIG_CTRL |= PLL_SW_PLL_EN_VAL(1);
        RF_CTRL->RF_MODE_CTRL |= QMX_RFCTRL_TX_EN_MO_VAL(1);
    } else {
        RF_PLL->PK_CTRL &= ~RFPLL_PEAKDET_CON_VREF_1P1_MASK;
        RF_PLL->PK_CTRL |= RFPLL_PEAKDET_CON_VREF_1P1_VAL(QMX_PEAKDET_VREF_1P1_RX_CODE);
        RF_PLL->FSM_CTRL &= ~RFPLL_FSM_SKIPPER_MASK;
        RF_PLL->FSM_CTRL |= RFPLL_FSM_SKIPPER_VAL(8);   // skip KVCO2

        RF_PLL->PLLDIG_CTRL |= PLL_SW_PLL_EN_VAL(1);
        RF_CTRL->RF_MODE_CTRL |= QMX_RFCTRL_RX_EN_MO_VAL(1);
    }

    do {
        if ((RF_PLL->PLLDIG_CTRL & PLL_RPT_AFC_DONE_MASK) != 0)
            break;

        qmx_hal_nop_delay_us(10);
        timeout += 10;
    } while (timeout < QMX_CALI_AFC_TIMEOUT_US);

    if (timeout >= QMX_CALI_AFC_TIMEOUT_US) {
        PRINTF("AFC CALI done timeout\n");
        return EN_ERROR_STA_TIMEOUT;
    }

    qmx_hal_nop_delay_us(10); // 等待一段时间，让PLL稳定下来

    if (tx) {
        RF_CTRL->RF_MODE_CTRL &= ~QMX_RFCTRL_TX_EN_MO_MASK;
        RF_CTRL->RF_MODE_CTRL &= ~(QMX_RFCTRL_TX_EN_ME_MASK | QMX_RFCTRL_RX_EN_ME_MASK);
    } else {
        RF_CTRL->RF_MODE_CTRL &= ~QMX_RFCTRL_RX_EN_MO_MASK;
        RF_CTRL->RF_MODE_CTRL &= ~(QMX_RFCTRL_TX_EN_ME_MASK | QMX_RFCTRL_RX_EN_ME_MASK);
    }

    RF_PLL->PLLDIG_CTRL &= ~PLL_SW_PLL_EN_MASK;
    RF_PLL->FSM_CTRL &= ~RFPLL_FSM_SKIPPER_MASK;

    RF_PLL->DTEMP_CTRL2 &= ~0x300;
    RF_PLL->DTEMP_CTRL0 &= ~0x300;
    RF_PLL->DTEMP_CTRL5 &= ~0x100;

    RF_PLL->PD_DAC_CTRL &= ~0x200;

    timeout = 0;
    do {
        if ((RF_PLL->PLLDIG_CTRL & PLL_RPT_AFC_DONE_MASK) == 0)
            break;

        qmx_hal_nop_delay_us(1);
        timeout += 1;
    } while (timeout < 50);

    if (timeout >= 50) {
        PRINTF("wait AFC CALI end timeout\n");
        return EN_ERROR_STA_TIMEOUT;
    }

    RF_CTRL->TX_RAMPING_CTRL |= QMX_RFCTRL_TX_RAMP_CYCLE_VAL(0x1);
    // PRINTF("AFC %s CALI succ, FREQ_INT 0x%X, FREQ_FRAC 0x%X, INTER_FREQ 0x%X\n", (tx ? "TX" : "RX"),
    //         RF_PLL->TX_FREQ_INT, RF_PLL->TX_FREQ_FRAC, RF_PLL->INTER_FREQ);

    return EN_ERROR_STA_OK;
}

int qmx_kdac_cali(void)
{
    uint32_t timeout = 0;

    RF_PLL->PLLDIG_CTRL |= PLL_CAL_DAC_START_VAL(1);

    do {
        if ((RF_PLL->PLLDIG_CTRL & PLL_CAL_DAC_START_MASK) == 0)
            break;

        qmx_hal_nop_delay_us(10);
        timeout += 10;
    } while (timeout < QMX_CALI_KADC_TIMEOUT_US);

    if (timeout >= QMX_CALI_KADC_TIMEOUT_US) {
        PRINTF("KDAC CALI timeout\n");
        return EN_ERROR_STA_TIMEOUT;
    }

    g_qmx_cali_data.kdac_gain = (RF_PLL->POLAR_CTRL & PLL_KDAC_GAIN_MASK) >> PLL_KDAC_GAIN_SHIFT;
    PRINTF("KVCO2 CALI succ, gain=%d\n", g_qmx_cali_data.kdac_gain);

    return EN_ERROR_STA_OK;
}

#define QMX_CALI_TXIQ_STEP      (1)
#define QMX_CALI_TXIQ_BUF_LEN   (256/QMX_CALI_TXIQ_STEP)
#define QMX_CALI_TXIQ_MIN_IDX   (-128)

EN_ERR_STA_T qmx_cali_i(int32_t vt)
{
    uint16_t buf_vt0[QMX_CALI_TXIQ_BUF_LEN] = {0};
    uint16_t buf_vt1[QMX_CALI_TXIQ_BUF_LEN] = {0};
    uint16_t i = 0;
    uint16_t diff, min_diff;
    int16_t dc_compi = 0;
    int16_t iopt = 0;
    uint32_t timeout = 0;

    RF_DFE->DFE_CALIB_DC_VT = DFE_CAL_SEND_DC_VTI_VAL(vt);
    i = 0;
    for (dc_compi = -128; dc_compi < 128; dc_compi += QMX_CALI_TXIQ_STEP) {
        RF_DFE->DFE_CFG_DC_COMP &= ~DFE_CALIB_DC_COMPI_MASK;
        RF_DFE->DFE_CFG_DC_COMP |= DFE_CALIB_DC_COMPI_VAL(dc_compi);
        RF_DFE->DFE_CALIB_START |= DFE_CAL_SEND_START_VAL(1);
        timeout = 0;

        do {
            if ((RF_DFE->DFE_CALIB_FINISH & DFE_CAL_EST_DC_FINISH_MASK) != 0)
                break;

            qmx_hal_nop_delay_us(10);
            timeout += 10;
        } while (timeout < QMX_CALI_DCIQ_TIMEOUT_US);

        if (timeout >= QMX_CALI_DCIQ_TIMEOUT_US) {
            PRINTF("DCIQ CALI I(Vt) timeout, dc_compi=%d\n", dc_compi);
            return EN_ERROR_STA_TIMEOUT;
        }

        buf_vt0[i] = (RF_DFE->DFE_CALIB_DC_RSLT & DFE_CAL_EST_DC_RSLT_MASK) >> DFE_CAL_EST_DC_RSLT_SHIFT;
        i++;

        qmx_hal_nop_delay_us(QMX_CALI_TXIQ_TIMEOUT_US);
    }

    RF_DFE->DFE_CALIB_DC_VT = DFE_CAL_SEND_DC_VTI_VAL(-vt);
    i = 0;
    for (dc_compi = -128; dc_compi < 128; dc_compi += QMX_CALI_TXIQ_STEP) {
        RF_DFE->DFE_CFG_DC_COMP &= ~DFE_CALIB_DC_COMPI_MASK;
        RF_DFE->DFE_CFG_DC_COMP |= DFE_CALIB_DC_COMPI_VAL(dc_compi);
        RF_DFE->DFE_CALIB_START |= DFE_CAL_SEND_START_VAL(1);
        timeout = 0;

        do {
            if ((RF_DFE->DFE_CALIB_FINISH & DFE_CAL_EST_DC_FINISH_MASK) != 0)
                break;

            qmx_hal_nop_delay_us(10);
            timeout += 10;
        } while (timeout < QMX_CALI_DCIQ_TIMEOUT_US);

        if (timeout >= QMX_CALI_DCIQ_TIMEOUT_US) {
            PRINTF("DCIQ CALI I(-Vt) timeout, dc_compi=%d\n", dc_compi);
            return EN_ERROR_STA_TIMEOUT;
        }

        buf_vt1[i] = (RF_DFE->DFE_CALIB_DC_RSLT & DFE_CAL_EST_DC_RSLT_MASK) >> DFE_CAL_EST_DC_RSLT_SHIFT;
        i++;
        qmx_hal_nop_delay_us(QMX_CALI_TXIQ_TIMEOUT_US);
    }

    iopt = 0;
    min_diff = 0xFFFF;
    for (i = 0; i < QMX_CALI_TXIQ_BUF_LEN; i++) {
        diff = abs(buf_vt0[i] - buf_vt1[i]);
        if (diff >= min_diff)
            continue;

        min_diff = diff;
        iopt = QMX_CALI_TXIQ_MIN_IDX + i*QMX_CALI_TXIQ_STEP;
    }

    RF_DFE->DFE_CFG_DC_COMP &= ~DFE_CALIB_DC_COMPI_MASK;
    RF_DFE->DFE_CFG_DC_COMP |= DFE_CALIB_DC_COMPI_VAL(iopt);
    g_qmx_cali_data.dciq_iopt = iopt;
    PRINTF("DCIQ CALI I succ, Vt=%d, min_diff=%u, iopt=%d\n", vt, min_diff, iopt);

    return EN_ERROR_STA_OK;
}

EN_ERR_STA_T qmx_cali_q(int32_t vt)
{
    uint16_t buf_vt0[QMX_CALI_TXIQ_BUF_LEN] = {0};
    uint16_t buf_vt1[QMX_CALI_TXIQ_BUF_LEN] = {0};
    uint16_t i = 0;
    uint16_t diff, min_diff;
    int16_t dc_compq = 0;
    int16_t qopt = 0;
    uint32_t timeout = 0;

    RF_DFE->DFE_CALIB_DC_VT = DFE_CAL_SEND_DC_VTQ_VAL(vt);
    i = 0;
    for (dc_compq = -128; dc_compq < 128; dc_compq += QMX_CALI_TXIQ_STEP) {
        RF_DFE->DFE_CFG_DC_COMP &= ~DFE_CALIB_DC_COMPQ_MASK;
        RF_DFE->DFE_CFG_DC_COMP |= DFE_CALIB_DC_COMPQ_VAL(dc_compq);
        RF_DFE->DFE_CALIB_START |= DFE_CAL_SEND_START_VAL(1);
        timeout = 0;

        do {
            if ((RF_DFE->DFE_CALIB_FINISH & DFE_CAL_EST_DC_FINISH_MASK) != 0)
                break;

            qmx_hal_nop_delay_us(10);
            timeout += 10;
        } while (timeout < QMX_CALI_DCIQ_TIMEOUT_US);

        if (timeout >= QMX_CALI_DCIQ_TIMEOUT_US) {
            PRINTF("DCIQ CALI Q(Vt) timeout, dc_compi=%d\n", dc_compq);
            return EN_ERROR_STA_TIMEOUT;
        }

        buf_vt0[i] = (RF_DFE->DFE_CALIB_DC_RSLT & DFE_CAL_EST_DC_RSLT_MASK) >> DFE_CAL_EST_DC_RSLT_SHIFT;
        i++;
        qmx_hal_nop_delay_us(QMX_CALI_TXIQ_TIMEOUT_US);
    }

    RF_DFE->DFE_CALIB_DC_VT = DFE_CAL_SEND_DC_VTQ_VAL(-vt);
    i = 0;
    for (dc_compq = -128; dc_compq < 128; dc_compq += QMX_CALI_TXIQ_STEP) {
        RF_DFE->DFE_CFG_DC_COMP &= ~DFE_CALIB_DC_COMPQ_MASK;
        RF_DFE->DFE_CFG_DC_COMP |= DFE_CALIB_DC_COMPQ_VAL(dc_compq);
        RF_DFE->DFE_CALIB_START |= DFE_CAL_SEND_START_VAL(1);
        timeout = 0;

        do {
            if ((RF_DFE->DFE_CALIB_FINISH & DFE_CAL_EST_DC_FINISH_MASK) != 0)
                break;

            qmx_hal_nop_delay_us(10);
            timeout += 10;
        } while (timeout < QMX_CALI_DCIQ_TIMEOUT_US);

        if (timeout >= QMX_CALI_DCIQ_TIMEOUT_US) {
            PRINTF("DCIQ CALI Q(-Vt) timeout, dc_compi=%d\n", dc_compq);
            return EN_ERROR_STA_TIMEOUT;
        }

        buf_vt1[i] = (RF_DFE->DFE_CALIB_DC_RSLT & DFE_CAL_EST_DC_RSLT_MASK) >> DFE_CAL_EST_DC_RSLT_SHIFT;
        i++;
        qmx_hal_nop_delay_us(QMX_CALI_TXIQ_TIMEOUT_US);
    }

    qopt = 0;
    min_diff = 0xFFFF;
    for (i = 0; i < QMX_CALI_TXIQ_BUF_LEN; i++) {
        diff = abs(buf_vt0[i] - buf_vt1[i]);
        if (diff >= min_diff)
            continue;

        min_diff = diff;
        qopt = QMX_CALI_TXIQ_MIN_IDX + i*QMX_CALI_TXIQ_STEP;
    }

    RF_DFE->DFE_CFG_DC_COMP &= ~DFE_CALIB_DC_COMPQ_MASK;
    RF_DFE->DFE_CFG_DC_COMP |= DFE_CALIB_DC_COMPQ_VAL(qopt);
    g_qmx_cali_data.dciq_qopt = qopt;
    PRINTF("DCIQ CALI Q succ, Vt=%d, min_diff=%u, qopt=%d\n", vt, min_diff, qopt);

    return EN_ERROR_STA_OK;
}

EN_ERR_STA_T qmx_cali_gain(int32_t vt)
{
    uint16_t buf_i_vt0[QMX_CALI_TXIQ_BUF_LEN] = {0};
    uint16_t buf_i_vt1[QMX_CALI_TXIQ_BUF_LEN] = {0};
    uint16_t buf_q_vt2[QMX_CALI_TXIQ_BUF_LEN] = {0};
    uint16_t buf_q_vt3[QMX_CALI_TXIQ_BUF_LEN] = {0};
    uint16_t i = 0;
    uint16_t diff, min_diff;
    int16_t idx = 0;
    uint32_t timeout = 0;
    int amp_compi, amp_compq;

    /* Gain I cali */
    RF_DFE->DFE_CALIB_DC_VT = DFE_CAL_SEND_DC_VTI_VAL(vt);
    i = 0;
    for (idx = -128; idx < 128; idx += QMX_CALI_TXIQ_STEP) {
        amp_compi = (int)((pow(10,-5.0/20)*7.0/8.0*(1.0+idx*1.0/2048.0)/1.0625)*4096);
        amp_compq = (int)((pow(10,-5.0/20)*7.0/8.0*(1.0-idx*1.0/2048.0)/1.0625)*4096);
        RF_DFE->DFE_CFG_AMP_COMP = (DFE_CALIB_AMP_COMPQ_VAL(amp_compq) | DFE_CALIB_AMP_COMPI_VAL(amp_compi));
        timeout = 0;

        RF_DFE->DFE_CALIB_START |= DFE_CAL_SEND_START_VAL(1);
        do {
            if ((RF_DFE->DFE_CALIB_FINISH & DFE_CAL_EST_DC_FINISH_MASK) != 0)
                break;

            qmx_hal_nop_delay_us(10);
            timeout += 10;
        } while (timeout < QMX_CALI_DCIQ_TIMEOUT_US);

        if (timeout >= QMX_CALI_DCIQ_TIMEOUT_US) {
            PRINTF("DCIQ CALI gain_i(Vt) timeout, idx=%d, amp_compi=0x%X, amp_compq=0x%X\n", idx, amp_compi, amp_compq);
            return EN_ERROR_STA_TIMEOUT;
        }

        buf_i_vt0[i] = (RF_DFE->DFE_CALIB_DC_RSLT & DFE_CAL_EST_DC_RSLT_MASK) >> DFE_CAL_EST_DC_RSLT_SHIFT;
        i++;
        qmx_hal_nop_delay_us(QMX_CALI_TXIQ_TIMEOUT_US);
    }

    RF_DFE->DFE_CALIB_DC_VT = DFE_CAL_SEND_DC_VTI_VAL(-vt);
    i = 0;
    for (idx = -128; idx < 128; idx += QMX_CALI_TXIQ_STEP) {
        amp_compi = (int)((pow(10,-5.0/20)*7.0/8.0*(1.0+idx*1.0/2048.0)/1.0625)*4096);
        amp_compq = (int)((pow(10,-5.0/20)*7.0/8.0*(1.0-idx*1.0/2048.0)/1.0625)*4096);
        RF_DFE->DFE_CFG_AMP_COMP = (DFE_CALIB_AMP_COMPQ_VAL(amp_compq) | DFE_CALIB_AMP_COMPI_VAL(amp_compi));
        timeout = 0;

        RF_DFE->DFE_CALIB_START |= DFE_CAL_SEND_START_VAL(1);
        do {
            if ((RF_DFE->DFE_CALIB_FINISH & DFE_CAL_EST_DC_FINISH_MASK) != 0)
                break;

            qmx_hal_nop_delay_us(10);
            timeout += 10;
        } while (timeout < QMX_CALI_DCIQ_TIMEOUT_US);

        if (timeout >= QMX_CALI_DCIQ_TIMEOUT_US) {
            PRINTF("DCIQ CALI gain_i(-Vt) timeout, idx=%d, amp_compi=0x%X, amp_compq=0x%X\n", idx, amp_compi, amp_compq);
            return EN_ERROR_STA_TIMEOUT;
        }

        buf_i_vt1[i] = (RF_DFE->DFE_CALIB_DC_RSLT & DFE_CAL_EST_DC_RSLT_MASK) >> DFE_CAL_EST_DC_RSLT_SHIFT;
        i++;
        qmx_hal_nop_delay_us(QMX_CALI_TXIQ_TIMEOUT_US);
    }

    /* Gain Q cali */
    RF_DFE->DFE_CALIB_DC_VT = DFE_CAL_SEND_DC_VTQ_VAL(vt);
    i = 0;
    for (idx = -128; idx < 128; idx += QMX_CALI_TXIQ_STEP) {
        amp_compi = (int)((pow(10,-5.0/20)*7.0/8.0*(1.0+idx*1.0/2048.0)/1.0625)*4096);
        amp_compq = (int)((pow(10,-5.0/20)*7.0/8.0*(1.0-idx*1.0/2048.0)/1.0625)*4096);
        RF_DFE->DFE_CFG_AMP_COMP = (DFE_CALIB_AMP_COMPQ_VAL(amp_compq) | DFE_CALIB_AMP_COMPI_VAL(amp_compi));
        timeout = 0;

        RF_DFE->DFE_CALIB_START |= DFE_CAL_SEND_START_VAL(1);
        do {
            if ((RF_DFE->DFE_CALIB_FINISH & DFE_CAL_EST_DC_FINISH_MASK) != 0)
                break;

            qmx_hal_nop_delay_us(10);
            timeout += 10;
        } while (timeout < QMX_CALI_DCIQ_TIMEOUT_US);

        if (timeout >= QMX_CALI_DCIQ_TIMEOUT_US) {
            PRINTF("DCIQ CALI gain_q(Vt) timeout, idx=%d, amp_compi=0x%X, amp_compq=0x%X\n", idx, amp_compi, amp_compq);
            return EN_ERROR_STA_TIMEOUT;
        }

        buf_q_vt2[i] = (RF_DFE->DFE_CALIB_DC_RSLT & DFE_CAL_EST_DC_RSLT_MASK) >> DFE_CAL_EST_DC_RSLT_SHIFT;
        i++;
        qmx_hal_nop_delay_us(QMX_CALI_TXIQ_TIMEOUT_US);
    }

    RF_DFE->DFE_CALIB_DC_VT = DFE_CAL_SEND_DC_VTQ_VAL(-vt);
    i = 0;
    for (idx = -128; idx < 128; idx += QMX_CALI_TXIQ_STEP) {
        amp_compi = (int)((pow(10,-5.0/20)*7.0/8.0*(1.0+idx*1.0/2048.0)/1.0625)*4096);
        amp_compq = (int)((pow(10,-5.0/20)*7.0/8.0*(1.0-idx*1.0/2048.0)/1.0625)*4096);
        RF_DFE->DFE_CFG_AMP_COMP = (DFE_CALIB_AMP_COMPQ_VAL(amp_compq) | DFE_CALIB_AMP_COMPI_VAL(amp_compi));
        timeout = 0;

        RF_DFE->DFE_CALIB_START |= DFE_CAL_SEND_START_VAL(1);
        do {
            if ((RF_DFE->DFE_CALIB_FINISH & DFE_CAL_EST_DC_FINISH_MASK) != 0)
                break;

            qmx_hal_nop_delay_us(10);
            timeout += 10;
        } while (timeout < QMX_CALI_DCIQ_TIMEOUT_US);

        if (timeout >= QMX_CALI_DCIQ_TIMEOUT_US) {
            PRINTF("DCIQ CALI gain_q(-Vt) timeout, idx=%d, amp_compi=0x%X, amp_compq=0x%X\n", idx, amp_compi, amp_compq);
            return EN_ERROR_STA_TIMEOUT;
        }

        buf_q_vt3[i] = (RF_DFE->DFE_CALIB_DC_RSLT & DFE_CAL_EST_DC_RSLT_MASK) >> DFE_CAL_EST_DC_RSLT_SHIFT;
        i++;
        qmx_hal_nop_delay_us(QMX_CALI_TXIQ_TIMEOUT_US);
    }

    idx = 0;
    min_diff = 0xFFFF;
    for (i = 0; i < QMX_CALI_TXIQ_BUF_LEN; i++) {
        diff = abs((buf_i_vt0[i] + buf_i_vt1[i]) - (buf_q_vt2[i] + buf_q_vt3[i]));
        if (diff >= min_diff)
            continue;

        min_diff = diff;
        idx = QMX_CALI_TXIQ_MIN_IDX + i*QMX_CALI_TXIQ_STEP;
    }

    amp_compi = (int)((pow(10,-5.0/20)*7.0/8.0*(1.0+idx*1.0/2048.0)/1.0625)*4096);
    amp_compq = (int)((pow(10,-5.0/20)*7.0/8.0*(1.0-idx*1.0/2048.0)/1.0625)*4096);
    RF_DFE->DFE_CFG_AMP_COMP = (DFE_CALIB_AMP_COMPQ_VAL(amp_compq) | DFE_CALIB_AMP_COMPI_VAL(amp_compi));
    g_qmx_cali_data.dciq_amp_compi = amp_compi;
    g_qmx_cali_data.dciq_amp_compq = amp_compq;
    PRINTF("DCIQ CALI GAIN succ, Vt=%d, min_diff=%u, idx=%d, amp_compi=0x%X, amp_compq=0x%X\n",
            vt, min_diff, idx, amp_compi, amp_compq);

    return EN_ERROR_STA_OK;
}

EN_ERR_STA_T qmx_cali_phase(int32_t vt)
{
    uint16_t buf_vt0[QMX_CALI_TXIQ_BUF_LEN] = {0};
    uint16_t buf_vt1[QMX_CALI_TXIQ_BUF_LEN] = {0};
    uint16_t i = 0;
    uint16_t diff, min_diff;
    int16_t idx = 0;
    uint32_t timeout = 0;

    RF_DFE->DFE_CALIB_DC_VT = DFE_CAL_SEND_DC_VTQ_VAL(vt) | DFE_CAL_SEND_DC_VTI_VAL(vt);
    i = 0;
    for (idx = -128; idx < 128; idx += QMX_CALI_TXIQ_STEP) {
        RF_DFE->DFE_CFG_PHASE_COMP = DFE_CALIB_PHASE_COMP_VAL(idx);
        RF_DFE->DFE_CALIB_START |= DFE_CAL_SEND_START_VAL(1);
        timeout = 0;

        do {
            if ((RF_DFE->DFE_CALIB_FINISH & DFE_CAL_EST_DC_FINISH_MASK) != 0)
                break;

            qmx_hal_nop_delay_us(10);
            timeout += 10;
        } while (timeout < QMX_CALI_DCIQ_TIMEOUT_US);

        if (timeout >= QMX_CALI_DCIQ_TIMEOUT_US) {
            PRINTF("DCIQ CALI phase(Vt) timeout, idx=%d\n", idx);
            return EN_ERROR_STA_TIMEOUT;
        }

        buf_vt0[i] = (RF_DFE->DFE_CALIB_DC_RSLT & DFE_CAL_EST_DC_RSLT_MASK) >> DFE_CAL_EST_DC_RSLT_SHIFT;
        i++;
        qmx_hal_nop_delay_us(QMX_CALI_TXIQ_TIMEOUT_US);
    }

    RF_DFE->DFE_CALIB_DC_VT = DFE_CAL_SEND_DC_VTQ_VAL(-vt) | DFE_CAL_SEND_DC_VTI_VAL(vt);
    i = 0;
    for (idx = -128; idx < 128; idx += QMX_CALI_TXIQ_STEP) {
        RF_DFE->DFE_CFG_PHASE_COMP = DFE_CALIB_PHASE_COMP_VAL(idx);
        RF_DFE->DFE_CALIB_START |= DFE_CAL_SEND_START_VAL(1);
        timeout = 0;

        do {
            if ((RF_DFE->DFE_CALIB_FINISH & DFE_CAL_EST_DC_FINISH_MASK) != 0)
                break;

            qmx_hal_nop_delay_us(10);
            timeout += 10;
        } while (timeout < QMX_CALI_DCIQ_TIMEOUT_US);

        if (timeout >= QMX_CALI_DCIQ_TIMEOUT_US) {
            PRINTF("DCIQ CALI phase(-Vt) timeout, idx=%d\n", idx);
            return EN_ERROR_STA_TIMEOUT;
        }

        buf_vt1[i] = (RF_DFE->DFE_CALIB_DC_RSLT & DFE_CAL_EST_DC_RSLT_MASK) >> DFE_CAL_EST_DC_RSLT_SHIFT;
        i++;
        qmx_hal_nop_delay_us(QMX_CALI_TXIQ_TIMEOUT_US);
    }

    idx = 0;
    min_diff = 0xFFFF;
    for (i = 0; i < QMX_CALI_TXIQ_BUF_LEN; i++) {
        diff = abs(buf_vt0[i] - buf_vt1[i]);
        if (diff >= min_diff)
            continue;

        min_diff = diff;
        idx = QMX_CALI_TXIQ_MIN_IDX + i*QMX_CALI_TXIQ_STEP;
    }

    RF_DFE->DFE_CFG_PHASE_COMP = DFE_CALIB_PHASE_COMP_VAL(idx);
    g_qmx_cali_data.dciq_phase_idx = idx;
    PRINTF("DCIQ CALI phase succ, Vt=%d, min_diff=%u, idx=%d\n", vt, min_diff, idx);

    return EN_ERROR_STA_OK;
}

EN_ERR_STA_T qmx_txiq_cali_common(uint8_t type, uint8_t rxabb_cbpf_gain2_lut0, uint16_t cali_send_duration,
                                  uint16_t cali_est_wait_time, uint32_t sine_freq, uint32_t f_cail)
{
    uint32_t sine_fcw = 0;
    int32_t fft_fcw = 0;

    if (rxabb_cbpf_gain2_lut0 > 4) {
        PRINTF("rxabb_cbpf_gain2_lut0=%d error, need < 4\n", rxabb_cbpf_gain2_lut0);
        return EN_ERROR_STA_INVALID;
    }

    RF_CTRL->RF_MODE_CTRL |= QMX_RFCTRL_TX_CAL_EN_VAL(1);

    // 仿真发现至少需要65us, 否则会失败, 建议200us和仿真保持一致
    qmx_hal_nop_delay_us(200);

    RF_AGC->AGC_BHF_LUT0 &= ~AGC_RXABB_CBPF_GAIN2_LUT0_MASK;
    RF_AGC->AGC_BHF_LUT0 |= AGC_RXABB_CBPF_GAIN2_LUT0_VAL(rxabb_cbpf_gain2_lut0);

    RF_CTRL->GPADC_CTRL &= ~QMX_RFCTRL_GPADC_BUF_SEL_MASK;
    RF_CTRL->GPADC_CTRL |= QMX_RFCTRL_GPADC_BUF_SEL_VAL(QMX_GPADC_TXPDT);

    RF_DFE->DFE_DIG_DOWN_CONV |= DFE_DDC_BYPASS_VAL(1);
    RF_DFE->DFE_DC0_PARA_CFG |= DFE_RMDC0_BYPASS_VAL(1);
    RF_DFE->DFE_CALIB_PARAMETER &= ~DFE_SINE_DC_SEL_MASK;
    RF_DFE->DFE_CALIB_PARAMETER |= DFE_SINE_DC_SEL_VAL(type);  // 0: sine, 1: DC

    RF_DFE->DFE_CALIB_WAIT_TIME &= ~DFE_CAL_SEND_DURATION_MASK;
    RF_DFE->DFE_CALIB_WAIT_TIME |= DFE_CAL_SEND_DURATION_VAL(cali_send_duration);
    RF_DFE->DFE_CALIB_WAIT_TIME &= ~DFE_CAL_EST_WAIT_MASK;
    RF_DFE->DFE_CALIB_WAIT_TIME |= DFE_CAL_EST_WAIT_VAL(cali_est_wait_time);

    if (type == QMX_CALI_SINE) {
        sine_fcw = (uint32_t)round(2.0 * PI * sine_freq * 16777216.0 / 25000000.0);
        fft_fcw = (int32_t)round(-2.0 * PI * f_cail * 16777216.0 / 5000000.0);
        RF_DFE->DFE_CALIB_SIN_FCW = DFE_CAL_SEND_SINE_FCW_VAL(sine_fcw);
        RF_DFE->DFE_CALIB_FFT_FCW = DFE_CAL_EST_FFT_FCW_VAL(fft_fcw);

        PRINTF("TXIQ CALI SINE mode, SINE_FCW=0x%X, FFT_FCW=0x%X\n", sine_fcw, fft_fcw);
    }

    RF_CTRL->RXABB_ADC_CTRL &= ~(QMX_RFCTRL_RXADC_RST_ME_MASK | QMX_RFCTRL_RXADC_RST_MO_MASK);
    RF_CTRL->RXABB_ADC_CTRL |= QMX_RFCTRL_RXADC_RST_ME_VAL(1) | QMX_RFCTRL_RXADC_RST_MO_VAL(0);

    return EN_ERROR_STA_OK;
}

EN_ERR_STA_T qmx_dciq_cali(uint8_t rxabb_cbpf_gain2_lut0, uint16_t cali_send_duration, uint16_t cali_est_wait_time,
                           int32_t vt)
{
    EN_ERR_STA_T ret = EN_ERROR_STA_OK;

    ret = qmx_txiq_cali_common(QMX_CALI_DC, rxabb_cbpf_gain2_lut0, cali_send_duration, cali_est_wait_time, 0, 0);
    if (ret != EN_ERROR_STA_OK)
        return ret;

    RF_DFE->DFE_CFG_DC_COMP &= ~(DFE_CALIB_DC_COMPI_MASK | DFE_CALIB_DC_COMPQ_MASK);
    if (qmx_cali_i(vt) != EN_ERROR_STA_OK)
        return EN_ERROR_STA_ERROR;

    if (qmx_cali_q(vt) != EN_ERROR_STA_OK)
        return EN_ERROR_STA_ERROR;

    if (qmx_cali_i(vt) != EN_ERROR_STA_OK)
        return EN_ERROR_STA_ERROR;

    if (qmx_cali_gain(vt) != EN_ERROR_STA_OK)
        return EN_ERROR_STA_ERROR;

    if (qmx_cali_phase(vt) != EN_ERROR_STA_OK)
        return EN_ERROR_STA_ERROR;

    RF_DFE->DFE_DIG_DOWN_CONV &= ~DFE_DDC_BYPASS_MASK;
    RF_CTRL->RF_MODE_CTRL &= ~QMX_RFCTRL_TX_CAL_EN_MASK;
    PRINTF("DCIQ CALI succ\n");
    return EN_ERROR_STA_OK;
}

EN_ERR_STA_T qmx_cali_fft_dc(void)
{
    uint32_t buf_dc[QMX_CALI_TXIQ_BUF_LEN] = {0};
    uint16_t i = 0;
    uint32_t diff, min_diff;
    int16_t dc_compi, dc_compq;
    int16_t iopt, qopt;
    uint32_t timeout = 0;
    uint32_t fft_rslti, fft_rsltq, fft_rslti_temp, fft_rsltq_temp;

    RF_DFE->DFE_CFG_AMP_COMP = (DFE_CALIB_AMP_COMPQ_VAL(1024) | DFE_CALIB_AMP_COMPI_VAL(1024));
    i = 0;
    for (dc_compi = -128; dc_compi < 128; dc_compi += QMX_CALI_TXIQ_STEP) {
        RF_DFE->DFE_CFG_DC_COMP = DFE_CALIB_DC_COMPI_VAL(dc_compi);
        RF_DFE->DFE_CALIB_START |= DFE_CAL_SEND_START_VAL(1);
        timeout = 0;

        do {
            if ((RF_DFE->DFE_CALIB_FINISH & DFE_CAL_EST_FFT_FINISH_MASK) != 0)
                break;

            qmx_hal_nop_delay_us(10);
            timeout += 10;
        } while (timeout < QMX_CALI_FFTIQ_TIMEOUT_US);

        if (timeout >= QMX_CALI_FFTIQ_TIMEOUT_US) {
            PRINTF("FFT IQ CALI I timeout, dc_compi=%d\n", dc_compi);
            return EN_ERROR_STA_TIMEOUT;
        }

        fft_rslti = RF_DFE->DFE_CALIB_FFT_RSLTI;
        fft_rsltq = RF_DFE->DFE_CALIB_FFT_RSLTQ;
        fft_rslti_temp = (fft_rslti & 0x200000) ? (0x400000 - fft_rslti) : fft_rslti;
        fft_rsltq_temp = (fft_rsltq & 0x200000) ? (0x400000 - fft_rsltq) : fft_rsltq;

        buf_dc[i] = fft_rslti_temp * fft_rslti_temp + fft_rsltq_temp * fft_rsltq_temp ;
        i++;

        qmx_hal_nop_delay_us(QMX_CALI_TXIQ_TIMEOUT_US);
    }

    iopt = 0;
    min_diff = buf_dc[0];
    for (i = 0; i < QMX_CALI_TXIQ_BUF_LEN; i++) {
        diff = buf_dc[i];
        if (diff >= min_diff)
            continue;

        min_diff = diff;
        iopt = i;
    }
    iopt = QMX_CALI_TXIQ_MIN_IDX + iopt;

    i = 0;
    for (dc_compq = -128; dc_compq < 128; dc_compq += QMX_CALI_TXIQ_STEP) {
        RF_DFE->DFE_CFG_DC_COMP = (DFE_CALIB_DC_COMPI_VAL(iopt) | DFE_CALIB_DC_COMPQ_VAL(dc_compq));
        RF_DFE->DFE_CALIB_START |= DFE_CAL_SEND_START_VAL(1);
        timeout = 0;

        do {
            if ((RF_DFE->DFE_CALIB_FINISH & DFE_CAL_EST_FFT_FINISH_MASK) != 0)
                break;

            qmx_hal_nop_delay_us(10);
            timeout += 10;
        } while (timeout < QMX_CALI_FFTIQ_TIMEOUT_US);

        if (timeout >= QMX_CALI_FFTIQ_TIMEOUT_US) {
            PRINTF("FFT IQ CALI Q timeout, dc_compq=%d\n", dc_compq);
            return EN_ERROR_STA_TIMEOUT;
        }

        fft_rslti = RF_DFE->DFE_CALIB_FFT_RSLTI;
        fft_rsltq = RF_DFE->DFE_CALIB_FFT_RSLTQ;
        fft_rslti_temp = (fft_rslti & 0x200000) ? (0x400000 - fft_rslti) : fft_rslti;
        fft_rsltq_temp = (fft_rsltq & 0x200000) ? (0x400000 - fft_rsltq) : fft_rsltq;

        buf_dc[i] = fft_rslti_temp * fft_rslti_temp + fft_rsltq_temp * fft_rsltq_temp ;
        i++;

        qmx_hal_nop_delay_us(QMX_CALI_TXIQ_TIMEOUT_US);
    }

    qopt = 0;
    min_diff = buf_dc[0];
    for (i = 0; i < QMX_CALI_TXIQ_BUF_LEN; i++) {
        diff = buf_dc[i];
        if (diff >= min_diff)
            continue;

        min_diff = diff;
        qopt = i;
    }
    qopt = QMX_CALI_TXIQ_MIN_IDX + qopt;

    RF_DFE->DFE_CFG_DC_COMP = (DFE_CALIB_DC_COMPI_VAL(iopt) | DFE_CALIB_DC_COMPQ_VAL(qopt));
    g_qmx_cali_data.fftiq_dc_i = iopt;
    g_qmx_cali_data.fftiq_dc_q  = qopt;
    PRINTF("FFT IQ CALI DC succ, i=%d, q=%d\n", iopt, qopt);

    return EN_ERROR_STA_OK;
}

#define QMX_CALI_FFTIQ_AMP_STEP     (2)
EN_ERR_STA_T qmx_cali_fft_amp(void)
{
    uint32_t buf_amp[QMX_CALI_TXIQ_BUF_LEN] = {0};
    uint16_t i = 0;
    int16_t idx, amp_result;
    uint32_t diff, min_diff;
    int amp_compi, amp_compq;
    uint32_t timeout = 0;
    uint32_t fft_rslti, fft_rsltq, fft_rslti_temp, fft_rsltq_temp;

    i = 0;
    for (idx = -128; idx < 128; idx += QMX_CALI_FFTIQ_AMP_STEP) {
        amp_compi = (int)(1024 + idx/2);
        amp_compq = (int)(1024 - idx/2);
        RF_DFE->DFE_CFG_AMP_COMP = (DFE_CALIB_AMP_COMPQ_VAL(amp_compq) | DFE_CALIB_AMP_COMPI_VAL(amp_compi));
        RF_DFE->DFE_CALIB_START |= DFE_CAL_SEND_START_VAL(1);
        timeout = 0;

        do {
            if ((RF_DFE->DFE_CALIB_FINISH & DFE_CAL_EST_FFT_FINISH_MASK) != 0)
                break;

            qmx_hal_nop_delay_us(10);
            timeout += 10;
        } while (timeout < QMX_CALI_FFTIQ_TIMEOUT_US);

        if (timeout >= QMX_CALI_FFTIQ_TIMEOUT_US) {
            PRINTF("FFT IQ CALI AMP timeout, idx=%d\n", idx);
            return EN_ERROR_STA_TIMEOUT;
        }

        fft_rslti = RF_DFE->DFE_CALIB_FFT_RSLTI;
        fft_rsltq = RF_DFE->DFE_CALIB_FFT_RSLTQ;
        fft_rslti_temp = (fft_rslti & 0x200000) ? (0x400000 - fft_rslti) : fft_rslti;
        fft_rsltq_temp = (fft_rsltq & 0x200000) ? (0x400000 - fft_rsltq) : fft_rsltq;

        buf_amp[i] = fft_rslti_temp * fft_rslti_temp + fft_rsltq_temp * fft_rsltq_temp ;
        i+=QMX_CALI_FFTIQ_AMP_STEP;

        qmx_hal_nop_delay_us(QMX_CALI_TXIQ_TIMEOUT_US);
    }

    amp_result = 0;
    min_diff = buf_amp[0];
    for (i = 0; i < 256; i += QMX_CALI_FFTIQ_AMP_STEP) {
        diff = buf_amp[i];
        if (diff >= min_diff)
            continue;

        min_diff = diff;
        amp_result = i;
    }
    amp_result = QMX_CALI_TXIQ_MIN_IDX + amp_result;
    amp_compi = (int)(1024 + amp_result/2);
    amp_compq = (int)(1024 - amp_result/2);
    RF_DFE->DFE_CFG_AMP_COMP = (DFE_CALIB_AMP_COMPQ_VAL(amp_compq) | DFE_CALIB_AMP_COMPI_VAL(amp_compi));
    g_qmx_cali_data.fftiq_amp_compi = amp_compi;
    g_qmx_cali_data.fftiq_amp_compq = amp_compq;
    PRINTF("FFT IQ CALI AMP succ, i=%d, q=%d\n", amp_compi, amp_compq);

    return EN_ERROR_STA_OK;
}

EN_ERR_STA_T qmx_cali_fft_phase(void)
{
    uint32_t buf_pha[QMX_CALI_TXIQ_BUF_LEN] = {0};
    uint16_t i = 0;
    int16_t idx, pha_result;
    uint32_t diff, min_diff;
    uint32_t timeout = 0;
    uint32_t fft_rslti, fft_rsltq, fft_rslti_temp, fft_rsltq_temp;

    i = 0;
    for (idx = -128; idx < 128; idx += QMX_CALI_TXIQ_STEP) {
        RF_DFE->DFE_CFG_PHASE_COMP = DFE_CALIB_PHASE_COMP_VAL(idx);
        RF_DFE->DFE_CALIB_START |= DFE_CAL_SEND_START_VAL(1);
        timeout = 0;

        do {
            if ((RF_DFE->DFE_CALIB_FINISH & DFE_CAL_EST_FFT_FINISH_MASK) != 0)
                break;

            qmx_hal_nop_delay_us(10);
            timeout += 10;
        } while (timeout < QMX_CALI_FFTIQ_TIMEOUT_US);

        if (timeout >= QMX_CALI_FFTIQ_TIMEOUT_US) {
            PRINTF("FFT IQ CALI AMP timeout, idx=%d\n", idx);
            return EN_ERROR_STA_TIMEOUT;
        }

        fft_rslti = RF_DFE->DFE_CALIB_FFT_RSLTI;
        fft_rsltq = RF_DFE->DFE_CALIB_FFT_RSLTQ;
        fft_rslti_temp = (fft_rslti & 0x200000) ? (0x400000 - fft_rslti) : fft_rslti;
        fft_rsltq_temp = (fft_rsltq & 0x200000) ? (0x400000 - fft_rsltq) : fft_rsltq;

        buf_pha[i] = fft_rslti_temp * fft_rslti_temp + fft_rsltq_temp * fft_rsltq_temp ;
        i++;

        qmx_hal_nop_delay_us(QMX_CALI_TXIQ_TIMEOUT_US);
    }

    pha_result = 0;
    min_diff = buf_pha[0];
    for (i = 0; i < QMX_CALI_TXIQ_BUF_LEN; i++) {
        diff = buf_pha[i];
        if (diff >= min_diff)
            continue;

        min_diff = diff;
        pha_result = i;
    }
    pha_result = QMX_CALI_TXIQ_MIN_IDX + pha_result;

    RF_DFE->DFE_CFG_PHASE_COMP = DFE_CALIB_PHASE_COMP_VAL(pha_result);
    g_qmx_cali_data.fftiq_phase_idx = pha_result;
    PRINTF("FFT IQ CALI PHASE succ, idx=%d\n", pha_result);

    return EN_ERROR_STA_OK;
}

EN_ERR_STA_T qmx_fftiq_cali(uint8_t rxabb_cbpf_gain2_lut0, uint16_t cali_send_duration, uint16_t cali_est_wait_time,
                            int32_t vt)
{
    uint32_t timeout = 0;
    EN_ERR_STA_T ret = EN_ERROR_STA_OK;

    ret = qmx_txiq_cali_common(QMX_CALI_SINE, rxabb_cbpf_gain2_lut0, cali_send_duration, cali_est_wait_time,
                               100000, 20000);
    if (ret != EN_ERROR_STA_OK)
        return ret;

    ret = qmx_cali_fft_dc();
    if (ret != EN_ERROR_STA_OK)
        return ret;

    ret = qmx_cali_fft_amp();
    if (ret != EN_ERROR_STA_OK)
        return ret;

    ret = qmx_cali_fft_phase();
    if (ret != EN_ERROR_STA_OK)
        return ret;

    RF_DFE->DFE_CALIB_START |= DFE_CAL_SEND_START_VAL(1);
    do {
        if ((RF_DFE->DFE_CALIB_FINISH & DFE_CAL_EST_FFT_FINISH_MASK) != 0)
            break;

        qmx_hal_nop_delay_us(10);
        timeout += 10;
    } while (timeout < QMX_CALI_FFTIQ_TIMEOUT_US);

    if (timeout >= QMX_CALI_FFTIQ_TIMEOUT_US) {
        PRINTF("FFT IQ CALI timeout\n");
        return EN_ERROR_STA_TIMEOUT;
    }

    RF_DFE->DFE_DIG_DOWN_CONV &= ~DFE_DDC_BYPASS_MASK;
    RF_CTRL->RF_MODE_CTRL &= ~QMX_RFCTRL_TX_CAL_EN_MASK;
    return EN_ERROR_STA_OK;
}

qmx_cali_data_t *qmx_get_cali_data_ptr(void)
{
    return &g_qmx_cali_data;
}

uint32_t qmx_get_cali_data_size(void)
{
    return sizeof(g_qmx_cali_data);
}

/* 按照上电时序逐步恢复校准数据 */
void qmx_restore_cali_data(qmx_cali_iq_e iq_type)
{
    uint8_t result_idx = g_qmx_cali_data.rc_cali_idx;

    // 恢复时钟校准数据
    RF_PMU->DCDC_CTRL &= ~QMX_DCDC_OSC_CAPTRIM_MASK;
    RF_PMU->RC32K_CTRL &= ~(QMX_CMU_OSC32K_FREQ_CTRIM_MASK | QMX_CMU_OSC32K_FREQ_RTRIM_MASK);
    RF_PMU->RC50M_CTRL &= ~(QMX_CMU_OSC50M_FREQ_CTUNE_MASK | QMX_CMU_OSC50M_FREQ_RTUNE_MASK);
    RF_PMU->DCDC_CTRL |= QMX_DCDC_OSC_CAPTRIM_VAL(g_qmx_cali_data.dcdc1m_captrim);
    RF_PMU->RC32K_CTRL |= (QMX_CMU_OSC32K_FREQ_CTRIM_VAL(g_qmx_cali_data.rc32k_ctrim) |
                           QMX_CMU_OSC32K_FREQ_RTRIM_VAL(g_qmx_cali_data.rc32k_rtrim));
    RF_PMU->RC50M_CTRL |= (QMX_CMU_OSC50M_FREQ_CTUNE_VAL(g_qmx_cali_data.rc50m_ctune) |
                           QMX_CMU_OSC50M_FREQ_RTUNE_VAL(g_qmx_cali_data.rc50m_rtune));

    // 恢复RC校准数据
    RF_CTRL->RXABB_CBPF_CTRL |= RFCTRL_RXABB_CBPF_CTRIM_VAL(rccalib_table[result_idx].rxabb_cbpf_ctrim);
    RF_CTRL->TXABB_LPF_TRIM |= QMX_RFCTRL_TXLPF_CTRIM_VAL(rccalib_table[result_idx].lpf_ctrim);
    RF_CTRL->RXRF_MIX_TIA_CTRL |= QMX_RFCTRL_RXTIA_CT_CTUNE_VAL(rccalib_table[result_idx].tia);
    RF_PMU->FDB_50M_TUNE |= QMX_CMU_FDOUB_CTUNE_VAL(rccalib_table[result_idx].freq_doubler);
    RF_PLL->RFPLL_RESERVE |= RFPLL_RESERVE_IN_VAL(rccalib_table[result_idx].lo_ppf);
    RF_PLL->DAC_CTRL |= RFPLL_DAC_TIA_CTRIM_VAL(rccalib_table[result_idx].pll_dac_lpf);

    // 恢复R校准数据
    RF_PMU->PMU_CTRL &= ~QMX_PMU_IOUT_TUNE_TRIM_MASK;
    RF_PMU->PMU_CTRL &= ~QMX_PMU_IOUT_PTAT_TRIM_MASK;
    RF_PMU->PMU_CTRL |= (QMX_PMU_IOUT_TUNE_TRIM_VAL(g_qmx_cali_data.pmu_iout_tune_trim) |
                         QMX_PMU_IOUT_PTAT_TRIM_VAL(g_qmx_cali_data.pmu_iout_ptat_trim));

    // 恢复KDAC校准数据
    RF_PLL->POLAR_CTRL &= ~PLL_KDAC_GAIN_MASK;
    RF_PLL->POLAR_CTRL |= PLL_KDAC_GAIN_VAL(g_qmx_cali_data.kdac_gain);

    // standby后重新做AFC校准，无需存储AFC校准数据

    if (iq_type == QMX_CALI_DC_IQ) {
        // 恢复DC IQ校准数据
        RF_DFE->DFE_CFG_DC_COMP = (DFE_CALIB_DC_COMPI_VAL(g_qmx_cali_data.dciq_iopt) |
                                    DFE_CALIB_DC_COMPQ_VAL(g_qmx_cali_data.dciq_qopt));
        RF_DFE->DFE_CFG_AMP_COMP = (DFE_CALIB_AMP_COMPQ_VAL(g_qmx_cali_data.dciq_amp_compq) |
                                    DFE_CALIB_AMP_COMPI_VAL(g_qmx_cali_data.dciq_amp_compi));
        RF_DFE->DFE_CFG_PHASE_COMP = DFE_CALIB_PHASE_COMP_VAL(g_qmx_cali_data.dciq_phase_idx);
    } else {
        // 恢复FFT IQ校准数据
        RF_DFE->DFE_CFG_DC_COMP = (DFE_CALIB_DC_COMPI_VAL(g_qmx_cali_data.fftiq_dc_i) |
                                    DFE_CALIB_DC_COMPQ_VAL(g_qmx_cali_data.fftiq_dc_q));
        RF_DFE->DFE_CFG_AMP_COMP = (DFE_CALIB_AMP_COMPQ_VAL(g_qmx_cali_data.fftiq_amp_compq) |
                                    DFE_CALIB_AMP_COMPI_VAL(g_qmx_cali_data.fftiq_amp_compi));
        RF_DFE->DFE_CFG_PHASE_COMP = DFE_CALIB_PHASE_COMP_VAL(g_qmx_cali_data.fftiq_phase_idx);
    }
}

int qmx_cali_init(qmx_cali_iq_e iq_type)
{
    int ret = EN_ERROR_STA_OK;

    // 以下校准顺序不可变，需按照规定顺序进行校准
    ret += qmx_clk_cali(QMX_CALI_DCDC1M);
    ret += qmx_clk_cali(QMX_CALI_RC32K);
    ret += qmx_clk_cali(QMX_CALI_RC50M);
    if (ret != EN_ERROR_STA_OK) {
        ret = EN_ERROR_STA_ERROR;
        goto end;
    }

    ret = qmx_rc_cali();
    if (ret != EN_ERROR_STA_OK)
        goto end;

    // ret = qmx_r_cali();
    // if (ret != EN_ERROR_STA_OK)
    //     goto end;

    // 该校准只在TX，且仅BW=0/1/2时才需要做，若BW无改变无需做
    // ret = qmx_kdac_cali();
    // if (ret != EN_ERROR_STA_OK)
    //     goto end;

    // 本振490MHz，中频-0.25MHz，本振修改后都需要重新校准
    // ret = qmx_afc_cali(490000000, -250000, true, false);
    // if (ret != EN_ERROR_STA_OK)
    //     goto end;

    // DC IQ和FFT IQ校准二选一即可
    // if (iq_type == QMX_CALI_DC_IQ) {
    //     ret = qmx_dciq_cali(1, 0xC8, 0x1E, 3276);
    // } else {
    //     ret = qmx_fftiq_cali(1, 0xC8, 0x1E, 3276);
    // }

end:
    qmx_hal_nop_delay_ms(5);
    return ret;
}
