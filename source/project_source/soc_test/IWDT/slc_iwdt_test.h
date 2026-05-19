#ifndef __SLC_IWDT_TEST_H__
#define __SLC_IWDT_TEST_H__

int slc_iwdt_int_feed_reset_test(void);
int slc_iwdt_feed_reset_test(void);
int slc_iwdt_int_feed_noreset_test(void);
int slc_iwdt_feed_noreset_test(void);
int slc_iwdt_deinit_test(void);
int slc_iwdt_write_protect_test(void);

#endif // __SLC_WDT_TEST_H__
