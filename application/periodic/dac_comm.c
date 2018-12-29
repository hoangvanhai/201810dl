/*
 * rtc_comm.c
 *
 *  Created on: Nov 2, 2018
 *      Author: PC
 */


#include <dac_comm.h>
#include <pin_mux.h>



static i2c_master_state_t master_dac;
// i2c slave info
static i2c_device_t slave_dac =
{
    .address = I2C_DAC_SLAVE_ADDRESS,
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
int DAC_InitRefCurr(void) {

	configure_i2c_pins(BOARD_I2C_DAC_INSTANCE);
	i2c_status_t retVal =  I2C_DRV_MasterInit(BOARD_I2C_DAC_INSTANCE, &master_dac);
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
int DAC_SetRefLevel(uint16_t lev, bool write_eeprom) {

	i2c_status_t i2c_ret;
	uint8_t cmd[1];
	uint8_t level[3];

	cmd[0] = 0;

	if(write_eeprom) {
		level[0] = MCP4726_CMD_WRITEDACEEPROM;
	} else {
		level[0] = MCP4726_CMD_WRITEDAC;
	}

	level[0] = MCP4726_CMD_WRITEDAC;

	level[1] =  (lev >> 8) & 0x0F;
	level[2] =  lev & 0xFF;

	i2c_ret = I2C_DRV_MasterSendDataBlocking(
			BOARD_I2C_DAC_INSTANCE, &slave_dac,
			cmd, 1, level, 3, 100);

	if(i2c_ret != kStatus_I2C_Success) {
		ASSERT(FALSE);
	}

	return i2c_ret;
}

