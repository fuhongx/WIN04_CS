#ifndef __SLC_PHY_TEST_H__
#define __SLC_PHY_TEST_H__

#include "slc_phy.h"
#include "slc_rf.h"
#include "slc_subg.h"
#include "slc_phy_trx.h"

void slc_phy_test(void);

void slc_phy_test_display_cnt(void);
uint8_t slc_phy_test_get_flag(void);
void slc_phy_test_set_flag(uint8_t flag);

#endif // __SLC_PHY_TEST_H__
