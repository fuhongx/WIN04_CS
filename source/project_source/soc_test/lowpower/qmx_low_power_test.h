#ifndef __QMX_LOW_POWER_TEST_H__
#define __QMX_LOW_POWER_TEST_H__

// Basic Function Test
int low_power_sleep_phy_enable_test(void);
int low_power_sleep_phy_disable_test(void);

int low_power_stop_phy_disable_test(void);
int low_power_stop_phy_enable_test(void);

int low_power_shutdown_test(void);

int low_power_standby_test(void);
int low_power_standby_flash_force_on_test(void);
int low_power_faston_test(void);
int low_power_fourceon_faston_test(void);

// multiple wakeup source test
int low_power_sleep_multiple_wakeup_test(void);
int low_power_stop_multiple_wakeup_test(void);
int low_power_standby_multiple_wakeup_test(void);

int low_power_stop_not_wakeup_test(void);
int low_power_standby_not_wakeup_test(void);

int low_power_stop_wakeup_by_lpio_test(void);
int low_power_standby_wakeup_by_lpio_test(void);

#endif // __QMX_LOW_POWER_TEST_H__
