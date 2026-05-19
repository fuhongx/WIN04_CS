#ifndef __SLC_WDT_TEST_H__
#define __SLC_WDT_TEST_H__

int slc_wdt_int_feed_reset_test(void);
int slc_wdt_feed_reset_test(void);
int slc_wdt_int_feed_noreset_test(void);
int slc_wdt_feed_noreset_test(void);
int slc_wdt_deinit_test(void);
int slc_wdt_write_protect_test(void);

#endif // __SLC_WDT_TEST_H__
