#ifndef __SLC_IIC_TEST_H__
#define __SLC_IIC_TEST_H__

void slc_iic_test_init(void);
void slc_iic_test_deinit(void);

int slc_iic_master_speed_test(void);
int slc_iic_master_addrbits_test(void);

int slc_iic_slv_addrbits_test(void);

#endif // __SLC_IIC_TEST_H__
