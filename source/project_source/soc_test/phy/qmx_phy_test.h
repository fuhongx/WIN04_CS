#ifndef __QMX_PHY_TEST_H__
#define __QMX_PHY_TEST_H__

#include "qmx_phy.h"
#include "qmx_rf.h"
#include "qmx_subg.h"
#include "qmx_phy_trx.h"

void qmx_phy_test(void);

void qmx_phy_test_display_cnt(void);
uint8_t qmx_phy_test_get_flag(void);
void qmx_phy_test_set_flag(uint8_t flag);

#endif // __QMX_PHY_TEST_H__
