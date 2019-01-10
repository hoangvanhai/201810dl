/*
 * configure.c
 *
 *  Created on: Jan 9, 2019
 *      Author: PC
 */


#include <configure.h>
#include <board.h>
#include <lib_str.h>

// i2c slave info
static i2c_device_t slave_eeprom =
{
    .address = CONFIGURE_SLAVE_ADDRESS,
    .baudRate_kbps = 400
};


const char* key_msg = "*configuration*";

/*****************************************************************************/
/** @brief
 *
 *
 *  @param
 *  @return Void.
 *  @note
 */
int CONF_WriteData(uint16_t addr, const uint8_t *data, int len) {

	i2c_status_t i2c_ret;
	uint8_t cmd[2];
	uint16_t 	page_num;
	uint16_t	written_len = 0, len_to_write;
	uint16_t 	write_addr = addr;

	if(len % EEPROM_PAGE_SIZE != 0) {
		page_num = (len / EEPROM_PAGE_SIZE) + 1;
	} else {
		page_num = len / EEPROM_PAGE_SIZE;
	}

	GPIO_DRV_ClearPinOutput(EpromWp);

	for(int i = 0; i < page_num; i++) {
		cmd[0] = (write_addr >> 8) & 0xFF;
		cmd[1] = write_addr & 0xFF;

		if(len - written_len > EEPROM_PAGE_SIZE) {
			len_to_write = EEPROM_PAGE_SIZE;
		} else {
			len_to_write = len - written_len;
		}

		LREP("write to addr %x len = %d\r\n", write_addr, len_to_write);

		i2c_ret = I2C_DRV_MasterSendDataBlocking(BOARD_I2C_RTC_EEPROM_INSTANCE,
										&slave_eeprom, cmd, 2,
										&data[i * EEPROM_PAGE_SIZE],
										len_to_write, 100);

		if(i2c_ret != kStatus_I2C_Success) {
			LREP("i2c eeprom write err = %d\r\n", i2c_ret);
			break;
		}
		write_addr += EEPROM_PAGE_SIZE;
		written_len += len_to_write;

		if(written_len >= len) break;
		OSA_SleepMs(100);
	}

	GPIO_DRV_SetPinOutput(EpromWp);

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
int CONF_ReadData(uint16_t addr, uint8_t *data, int rlen, int *arlen) {
    i2c_status_t i2c_ret;
    uint8_t cmd[2];
	cmd[0] = (addr >> 8) & 0xFF;
	cmd[1] = addr & 0xFF;

	i2c_ret = I2C_DRV_MasterReceiveDataBlocking(BOARD_I2C_RTC_EEPROM_INSTANCE,
                                      &slave_eeprom, cmd, 2, data, rlen, 100);

    if(i2c_ret == kStatus_I2C_Success) {
    	*arlen = rlen;
    } else {
    	LREP("i2c eeprom read err = %d\r\n", i2c_ret);
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
int CONF_FillFlash(uint8_t byte) {
	int retVal;
	uint8_t data[EEPROM_PAGE_SIZE];
	memset(data, 0xFF, EEPROM_PAGE_SIZE);
	GPIO_DRV_ClearPinOutput(EpromWp);
	for(int i = 0; i < EEPROM_PAGE_NUM; i++) {
		retVal = CONF_WriteData(i * EEPROM_PAGE_SIZE, data, EEPROM_PAGE_SIZE);
		if(retVal != kStatus_I2C_Success) {
			break;
		}
	}
	GPIO_DRV_SetPinOutput(EpromWp);
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
int CONF_EraseSector(uint16_t start, uint16_t stop) {
	int retVal;
	uint8_t data[EEPROM_PAGE_SIZE];
	memset(data, 0xFF, EEPROM_PAGE_SIZE);
	GPIO_DRV_ClearPinOutput(EpromWp);
	for(int i = start; i <= stop; i++) {
		retVal = CONF_WriteData(i * EEPROM_PAGE_SIZE, data, EEPROM_PAGE_SIZE);
		if(retVal != kStatus_I2C_Success) {
			break;
		}
	}
	GPIO_DRV_SetPinOutput(EpromWp);
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
int CONF_EraseFlash(void) {
	return CONF_FillFlash(0xFF);
}


/*****************************************************************************/
/** @brief
 *
 *
 *  @param
 *  @return Void.
 *  @note
 */

bool CONF_CheckWritten(const char* key) {
	int retVal;
	bool ret = false;
	int rlen = 0;
	int size = Str_Len((CPU_CHAR*)key);

	uint8_t data[size];
	retVal = CONF_ReadData(0, data, size + 1, &rlen);
	if(retVal != kStatus_I2C_Success) {
		return ret;
	}

	data[size] = 0;
	LREP("read key = %s\r\n", data);

	if(Str_Cmp_N((CPU_CHAR*)key, (CPU_CHAR*)data, size + 1) == 0) {
		ret = true;
	}

	return ret;
}

/*****************************************************************************/
/** @brief
 *
 *
 *  @param
 *  @return Void.
 *  @note
 */
int CONF_WriteKey(const char* key) {
	return  CONF_WriteData(0, (const uint8_t*)key, Str_Len((CPU_CHAR*)key));
}

















