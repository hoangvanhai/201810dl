/*
 * rtc_comm.h
 *
 *  Created on: Nov 2, 2018
 *      Author: PC
 */

#ifndef APPLICATION_PERIODIC_DAC_COMM_H_
#define APPLICATION_PERIODIC_DAC_COMM_H_

#include <includes.h>
#include <fsl_debug_console.h>


#define I2C_DAC_SLAVE_ADDRESS    		(0x61)

// Writes data to the DAC
#define MCP4726_CMD_WRITEDACFAST        (0x00)
// Writes data to the DAC
#define MCP4726_CMD_WRITEDAC            (0x40)
// Writes data to the DAC and the EEPROM (persisting the assigned value after reset)
#define MCP4726_CMD_WRITEDACEEPROM      (0x60)

int DAC_InitRefCurr(void);
int DAC_SetRefLevel(uint16_t lev, bool write_eeprom);

#endif /* APPLICATION_PERIODIC_RTC_COMM_H_ */
