/*
 * configure.h
 *
 *  Created on: Jan 9, 2019
 *      Author: PC
 */

#ifndef APPLICATION_INTERFACE_CONFIGURE_H_
#define APPLICATION_INTERFACE_CONFIGURE_H_

#include <time.h>
#include <includes.h>
#include <fsl_debug_console.h>

#define CONFIGURE_SLAVE_ADDRESS    	(0x50)
#define EEPROM_PAGE_SIZE		64
#define EEPROM_PAGE_NUM			1024

int CONF_WriteData(uint16_t addr, const uint8_t *data, int len);
int CONF_ReadData(uint16_t addr, uint8_t *data, int rlen, int *arlen);
int CONF_EraseFlash(void);
int CONF_FillFlash(uint8_t byte);
int CONF_EraseSector(uint16_t start, uint16_t stop);
bool CONF_CheckWritten(const char* key);
int CONF_WriteKey(const char* key);

extern const char* key_msg;

#define CONF_WriteKeyApp() 			CONF_WriteKey(key_msg)
#define CONF_CheckWrittenApp()		CONF_CheckWritten(key_msg)

#endif /* APPLICATION_INTERFACE_CONFIGURE_H_ */
