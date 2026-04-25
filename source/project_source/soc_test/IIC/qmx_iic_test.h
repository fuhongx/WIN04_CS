#ifndef __QMX_IIC_TEST_H__
#define __QMX_IIC_TEST_H__

void qmx_iic_test_init(void);
void qmx_iic_test_deinit(void);

int qmx_iic_master_speed_test(void);
int qmx_iic_master_addrbits_test(void);

int qmx_iic_slv_addrbits_test(void);

#endif // __QMX_IIC_TEST_H__
