/*
 * rtc_comm.c
 *
 *  Created on: Nov 2, 2018
 *      Author: PC
 */


#include <rtc_comm.h>
#include <pin_mux.h>


static int cmd_read_date_time(uint8_t *recvData);

SDateTime *pDateTime;
static i2c_master_state_t master;
// i2c slave info
static i2c_device_t slave =
{
    .address = I2C_RTOS_SLAVE_ADDRESS,
    .baudRate_kbps = 100
};



/*****************************************************************************/
/** @brief
 *
 *
 *  @param
 *  @return Void.
 *  @note
 */
int RTC_InitDateTime(SDateTime *time) {

	configure_i2c_pins(BOARD_I2C_RTC_INSTANCE);

	i2c_status_t retVal =  I2C_DRV_MasterInit(BOARD_I2C_RTC_INSTANCE, &master);
	if(retVal != kStatus_I2C_Success)
		return retVal;

	if(time != NULL) {
		pDateTime = time;
		pDateTime->tm_min = 0;
		pDateTime->tm_hour = 0;
		pDateTime->tm_wday = 1;
		pDateTime->tm_mday = 1;
		pDateTime->tm_mon = 1;
		pDateTime->tm_year = 2018;
	} else {
		ASSERT(FALSE);
		return -1;
	}

	return retVal;
}

/*****************************************************************************/
/** @brief
 *
 *
 *  @param
 *  @return Void.
 *  @note
 */
int RTC_GetTimeDate(SDateTime *time) {
	//return 0;
	if(time) {
		uint8_t recvData[7];
		memset(recvData, 0, 7);
		int retVal = cmd_read_date_time(recvData);
		if(retVal == kStatus_I2C_Success) {

			/*LREP(" data: ");
			for(int i = 0; i < 7; i++) {
				LREP("%x-", recvData[i]);
			}
			LREP("\r\n");*/

			time->tm_sec = (((recvData[0] >> 4) & 0x07 ) * 10) + (recvData[0] & 0x0F);
			time->tm_min = (((recvData[1] >> 4) & 0x07 ) * 10) + (recvData[1] & 0x0F);
			time->tm_hour = (((recvData[2] >> 4) & 0x03 ) * 10) + (recvData[2] & 0x0F);
			time->tm_wday = (recvData[3] & 0x07);
			time->tm_mday = (((recvData[4] >> 4) & 0x03 ) * 10) + (recvData[4] & 0x0F);
			time->tm_mon = (((recvData[5] >> 4) & 0x01 ) * 10) + (recvData[5] & 0x0F);
			time->tm_year = (((recvData[6] >> 4) & 0x0F ) * 10) + (recvData[6] & 0x0F) + 1990;
		}
		return retVal;
	}
	return -1;
}

/*****************************************************************************/
/** @brief
 *
 *
 *  @param
 *  @return Void.
 *  @note
 */
int RTC_SetTimeDate(SDateTime *time) {

	uint8_t DateTime[7];
	uint8_t cmd[1];
	cmd[0] = 0x00;
	i2c_status_t i2c_ret;

	if(time->tm_year > 1990)
		time->tm_year -= 1990;

	ASSERT_NONVOID(time->tm_min >= 0 && time->tm_min <= 59, -1);
	ASSERT_NONVOID(time->tm_hour >= 0 && time->tm_hour <= 23, -1);
	//ASSERT_NONVOID(time->tm_wday >= 1 && time->tm_wday <= 7, -1);
	ASSERT_NONVOID(time->tm_mday >= 1 && time->tm_mon <= 31, -1);
	ASSERT_NONVOID(time->tm_mon >= 1 && time->tm_mon <= 12, -1);
	ASSERT_NONVOID(time->tm_year >= 0 && time->tm_year <= 99, -1);

	DateTime[0] = 0;
	DateTime[1] = DEC_TO_HEX_1BYTE(time->tm_min);
	DateTime[2] = DEC_TO_HEX_1BYTE(time->tm_hour);
	DateTime[3] = DEC_TO_HEX_1BYTE(time->tm_wday);
	DateTime[4] = DEC_TO_HEX_1BYTE(time->tm_mday);
	DateTime[5] = DEC_TO_HEX_1BYTE(time->tm_mon);
	DateTime[6] = DEC_TO_HEX_1BYTE(time->tm_year);

	i2c_ret = I2C_DRV_MasterSendDataBlocking(BOARD_I2C_RTC_INSTANCE,
			&slave, cmd, 1, DateTime, 7, 20);

	if(i2c_ret != kStatus_I2C_Success) {
		LREP("ret = %d\r\n", i2c_ret);
	}
	return i2c_ret;
}

/*****************************************************************************/
/** @brief
 *
 *
 *  @param
 *  @return Void.
 *  @note
 */
int  RTC_SetDateTime(uint8_t min, uint8_t hour, uint8_t date,
						uint8_t month, uint32_t year) {

	int retVal = RTC_GetTimeDate(pDateTime);
	if(retVal == kStatus_I2C_Success) {
		pDateTime->tm_min = min;
		pDateTime->tm_hour = hour;
		pDateTime->tm_mday = date;
		pDateTime->tm_mon = month;
		pDateTime->tm_year = year - 1990;
		return RTC_SetTimeDate(pDateTime);
	}
	return retVal;
}

/*****************************************************************************/
/** @brief
 *
 *
 *  @param
 *  @return Void.
 *  @note
 */
static int cmd_read_date_time(uint8_t *recvData)
{
    i2c_status_t i2c_ret;
    uint8_t cmd[1];
    cmd[0] = 0x00;
    // read from slave
    i2c_ret = I2C_DRV_MasterReceiveDataBlocking(BOARD_I2C_RTC_INSTANCE,
                                      &slave, cmd, 1, recvData, 7, 20);
    if(i2c_ret != kStatus_I2C_Success) {
    	ASSERT(FALSE);
    }

    return i2c_ret;
}
