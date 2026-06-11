#include <stdint.h>
#include <stdio.h>

#include "reg_Dev.h"
#include "debug.h"
#include "slc_hal_delay.h"
#include "slc_reset_test.h"

int slc_reset_soft_test(void)
{
    PRINTF("\n========================================\n");
    PRINTF("RESET Test: calling __NVIC_SystemReset()...\n");
    PRINTF("Observe the serial output or chip behaviour:\n");
    PRINTF("  1) If the chip reboots -> TEST PASS\n");
    PRINTF("  2) If the chip hangs (no reboot) -> TEST FAIL\n");
    PRINTF("  (wait 3 seconds, if no reboot, reset manually)\n");
    PRINTF("========================================\n\n");

    /* 等待 printf 缓冲区通过 UART 全部发出 */
    slc_hal_nop_delay_ms(3000);

    __NVIC_SystemReset();

    /* 如果芯片没有复位，永远到不了这里 */
    return 0;
}
