/*
 * rtc_comm.c
 *
 *  Created on: Nov 2, 2018
 *      Author: PC
 */


#include <rtc_comm.h>


static int cmd_read_date_time(uint8_t *recvData);

SDateTime g_DateTime;
static i2c_master_state_t master;
// i2c slave info
static i2c_device_t slave =
{
    .address = I2C_RTOS_SLAVE_ADDRESS,
    .baudRate_kbps = 100
};


uint32_t RTC_InitI2C(uint32_t baudrate) {
	I2C_DRV_MasterInit(I2C_RTOS_MASTER_INSTANCE, &master);
	return 0;
}


uint32_t RTC_InitRTCIC(uint32_t address) {
	return 0;
}


uint32_t RTC_GetTimeDate(SDateTime *time) {
	uint8_t recvData[7];
	memset(recvData, 0, 7);
	if(cmd_read_date_time(recvData) == kStatus_I2C_Success) {
		for(int i = 0; i < 7; i++) {
			LREP("%x-", recvData[i]);
		}
	}

	return 0;
}


uint32_t RTC_SetTimeDate(SDateTime *time) {

//	unsigned char I2CData[] = {0x00, 0x00, 0x00, 0x01, 0x01, 0x01, 0x09, 0x00};
//	i2c_status_t i2c_ret;
//	i2c_ret = I2C_DRV_MasterSendDataBlocking(I2C_RTOS_MASTER_INSTANCE, &slave, NULL, 0, I2CData, 8, 20);
//	if(i2c_ret != kStatus_I2C_Success) {
//		LREP("I2C write return error: %d\r\n", i2c_ret);
//	}
//	return i2c_ret;

}


static int cmd_read_date_time(uint8_t *recvData)
{
    i2c_status_t i2c_ret;
    uint8_t cmd[1];
    cmd[0] = 0xD1;
    // read from slave
    i2c_ret = I2C_DRV_MasterReceiveDataBlocking(I2C_RTOS_MASTER_INSTANCE,
                                      &slave, cmd, 1, (uint8_t*)recvData, 7, 20);
    if(i2c_ret != kStatus_I2C_Success) {
    	LREP("I2C return error: %d\r\n", i2c_ret);
    }


    return i2c_ret;
}
