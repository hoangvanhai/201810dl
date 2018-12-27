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


#define I2C_DAC_SLAVE_ADDRESS    	(0x60)
#define DAC_START_REG				0
#define DAC_NUM_REG					7

//#define USE_VREF_REF_INPUT
#define USE_VCC_REF_INPUT

#if defined(USE_VREF_REF_INPUT)
	#define MCP4726_CMD_WRITEDAC	(0x58)	//Write data to the DAC and uses Vref input
#elif defined(USE_VCC_REF_INPUT)
	#define MCP4726_CMD_WRITEDAC    (0x40)  // Writes data to the DAC with Vcc as Vref
#endif // #if defined


int DAC_InitRefCurr(void);
int DAC_SetRefLevel(uint16_t lev);

#endif /* APPLICATION_PERIODIC_RTC_COMM_H_ */
