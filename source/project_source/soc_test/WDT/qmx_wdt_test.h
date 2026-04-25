#ifndef __QMX_WDT_TEST_H__
#define __QMX_WDT_TEST_H__

int qmx_wdt_int_feed_reset_test(void);
int qmx_wdt_feed_reset_test(void);
int qmx_wdt_int_feed_noreset_test(void);
int qmx_wdt_feed_noreset_test(void);
int qmx_wdt_deinit_test(void);
int qmx_wdt_write_protect_test(void);

#endif // __QMX_WDT_TEST_H__
