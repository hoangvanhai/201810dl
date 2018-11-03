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


#define I2C_RTOS_SLAVE_ADDRESS    	(0xD0)
#define I2C_RTOS_MASTER_INSTANCE	1
#define RTC_START_REG				0
#define RTC_NUM_REG					7


typedef struct	tm		SDateTime;

uint32_t RTC_InitI2C(uint32_t baudrate);
uint32_t RTC_InitRTCIC(uint32_t address);
uint32_t RTC_GetTimeDate(SDateTime *time);
uint32_t RTC_SetTimeDate(SDateTime *time);



extern SDateTime g_DateTime;


#endif /* APPLICATION_PERIODIC_RTC_COMM_H_ */
