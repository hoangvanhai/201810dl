/*
 * rtc_comm.h
 *
 *  Created on: Nov 2, 2018
 *      Author: PC
 */

#ifndef APPLICATION_PERIODIC_RTC_COMM_H_
#define APPLICATION_PERIODIC_RTC_COMM_H_

#include <time.h>
#include <includes.h>
#include <fsl_debug_console.h>


#define I2C_RTOS_SLAVE_ADDRESS    	(0x68)
#define I2C_RTOS_MASTER_INSTANCE	1
#define RTC_START_REG				0
#define RTC_NUM_REG					7


typedef struct	tm		SDateTime;

int RTC_InitDateTime(SDateTime *time);
int RTC_GetTimeDate(SDateTime *time);
int RTC_SetTimeDate(SDateTime *time);
int RTC_SetDateTime(uint8_t min, uint8_t hour,
					uint8_t date, uint8_t month,
					uint32_t year);

#endif /* APPLICATION_PERIODIC_RTC_COMM_H_ */
