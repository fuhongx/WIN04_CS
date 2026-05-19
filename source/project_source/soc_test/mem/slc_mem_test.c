#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "win04_mem_cfg.h"
#include "slc_mem_test.h"
#include "slc_hal_sysctrl.h"
#include "app_cfg.h"
#include "utility.h"
#include "debug.h"

#define SLC_MEM_TEST_RAM_SIZE    (1 * UNIT_1K)
#define SLC_MEM_TEST_RAM_ADDR    (RAM_ADDR_MAX - SLC_MEM_TEST_RAM_SIZE)

int slc_mem_align_8byte_test(void)
{
    uint64_t *p = (uint64_t *)SLC_MEM_TEST_RAM_ADDR;
    uint32_t i = 0;

    for (i = 0; i < (SLC_MEM_TEST_RAM_SIZE/8); i++) {
        p[i] = i + 0x5AA5AA55FFAA55AA;
    }

    for (i = 0; i < (SLC_MEM_TEST_RAM_SIZE/8); i++) {
        if (p[i] != (i + 0x5AA5AA55FFAA55AA)) {
            PRINTF("SLC mem align 8B test fail, addr 0x%X, expect 0x%llX, actual 0x%llX\n",
                   (uint32_t)&p[i], (uint64_t)(i + 0x5AA5AA55FFAA55AA), p[i]);
            return -1;
        }
    }

    memset(p, 0, SLC_MEM_TEST_RAM_SIZE);

    PRINTF("SLC mem align 8B test pass\n");
    return 0;
}

int slc_mem_align_4byte_test(void)
{
    uint32_t *p = (uint32_t *)SLC_MEM_TEST_RAM_ADDR;
    uint32_t i = 0;

    for (i = 0; i < (SLC_MEM_TEST_RAM_SIZE/4); i++) {
        p[i] = i + 0x5AA5AA55;
    }

    for (i = 0; i < (SLC_MEM_TEST_RAM_SIZE/4); i++) {
        if (p[i] != (i + 0x5AA5AA55)) {
            PRINTF("SLC mem align 4B test fail, addr 0x%X, expect 0x%X, actual 0x%X\n",
                   (uint32_t)&p[i], (uint32_t)(i + 0x5AA5AA55), p[i]);
            return -1;
        }
    }

    memset(p, 0, SLC_MEM_TEST_RAM_SIZE);

    PRINTF("SLC mem align 4B test pass\n");
    return 0;
}

int slc_mem_align_2byte_test(void)
{
    uint16_t *p = (uint16_t *)SLC_MEM_TEST_RAM_ADDR;
    uint32_t i = 0;

    for (i = 0; i < (SLC_MEM_TEST_RAM_SIZE/2); i++) {
        p[i] = i + 0x5AA5;
    }

    for (i = 0; i < (SLC_MEM_TEST_RAM_SIZE/2); i++) {
        if (p[i] != (uint16_t)(i + 0x5AA5)) {
            PRINTF("SLC mem align 2B test fail, addr 0x%X, expect 0x%X, actual 0x%X\n",
                   (uint32_t)&p[i], (uint16_t)(i + 0x5AA5), p[i]);
            return -1;
        }
    }

    memset(p, 0, SLC_MEM_TEST_RAM_SIZE);

    PRINTF("SLC mem align 2B test pass\n");
    return 0;
}

int slc_mem_align_1byte_test(void)
{
    uint8_t *p = (uint8_t *)SLC_MEM_TEST_RAM_ADDR;
    uint32_t i = 0;

    for (i = 0; i < SLC_MEM_TEST_RAM_SIZE; i++) {
        p[i] = i + 0x5A;
    }

    for (i = 0; i < SLC_MEM_TEST_RAM_SIZE; i++) {
        if (p[i] != (uint8_t)(i+0x5A)) {
            PRINTF("SLC mem align 1B test fail, addr 0x%X, expect 0x%X, actual 0x%X\n",
                   (uint32_t)&p[i], (uint8_t)(i+0x5A), p[i]);
            return -1;
        }
    }

    memset(p, 0, SLC_MEM_TEST_RAM_SIZE);

    PRINTF("SLC mem align 1B test pass\n");
    return 0;
}

int slc_mem_unalign_test(void)
{
    uint32_t *p = (uint32_t *)(SLC_MEM_TEST_RAM_ADDR - 1);
    uint32_t i = 0;

    PRINTF("SLC mem unalign test addr: 0x%X, expect occur hardfault\n", (uint32_t)p);

    for (i = 0; i < (SLC_MEM_TEST_RAM_SIZE/4); i++) {
        p[i] = i + 0x5AA5AA55;
    }

    for (i = 0; i < (SLC_MEM_TEST_RAM_SIZE/4); i++) {
        if (p[i] != (i + 0x5AA5AA55)) {
            PRINTF("SLC mem unalign 4B test fail, addr 0x%X, expect 0x%X, actual 0x%X\n",
                   (uint32_t)&p[i], (uint32_t)(i + 0x5AA5AA55), p[i]);
            return -1;
        }
    }

    memset(p, 0, SLC_MEM_TEST_RAM_SIZE);

    PRINTF("SLC mem unalign 4B test pass\n");
    return 0;
}

int slc_mem_reserved_test(void)
{
    uint32_t *p = (uint32_t *)RAM_ADDR_MAX;
    uint32_t i = 0;

    for (i = 0; i < (SLC_MEM_TEST_RAM_SIZE/4); i++) {
        p[i] = i;
    }

    for (i = 0; i < (SLC_MEM_TEST_RAM_SIZE/4); i++) {
        if (p[i] != 0) {
            PRINTF("SLC mem reserved test fail, addr 0x%X, expect 0x%X, actual 0x%X\n",
                   (uint32_t)&p[i], i, p[i]);
            return -1;
        }
    }

    memset(p, 0, SLC_MEM_TEST_RAM_SIZE);

    PRINTF("SLC mem rw reserved test pass\n");
    return 0;
}
