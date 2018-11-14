/*
 * definition.h
 *
 *  Created on: Nov 1, 2018
 *      Author: PC
 */

#ifndef APPLICATION_DEFINITION_H_
#define APPLICATION_DEFINITION_H_

#include <stdint.h>
#include <stdbool.h>
#include <app_cfg.h>
#include <lwip/netif.h>


typedef enum ESysStatus_ {
	SYS_ERR_NONE		= 1 < 0,
	SYS_ERR_RTC			= 1 < 1,
	SYS_ERR_SDCARD_1	= 1 < 2,
	SYS_ERR_SDCARD_2	= 1 < 3,
	SYS_ERR_MODBUS		= 1 < 10,
	SYS_ERR_SPC			= 1 < 11,
	SYS_ERR_NET_PC		= 1 < 11,
	SYS_ERR_NET_FTP 	= 1 < 12,
	SYS_ERR_ETHER_CONN 	= 1 < 13,
	SYS_ERR_3G_CONN 	= 1 < 14,
	SYS_ERR_SUI			= 1 < 15,
}ESysStatus;

typedef enum ECfgConnType_ {
	CFG_CONN_SERIAL = 0,
	CFG_CONN_NET
}ECfgConnType;

typedef struct SMsg_ {
	uint16_t	id;
	uint8_t		*pData;
}SMsg;


typedef struct SLNode_ {
	uint32_t 	id;
	bool		lev;
}SLogicNode;


typedef struct SVNode_ {
	uint32_t 	id;
	double 		value;
}SValueNode;

typedef struct SCtrlPort_ {
	uint8_t	name[15];
	uint32_t port;
}SCtrlPort;


typedef struct SDigitalInput_ {
	SLogicNode		Node[DIGITAL_INPUT_NUM_CHANNEL];
}SDigitalInput;

typedef struct SDigitalOutput_ {
	SCtrlPort 		Node[DIGITAL_OUTPUT_NUM_CHANNEL];
}SDigitalOutput;

typedef struct SAnalogInput_ {
	SValueNode		Node[ANALOG_INPUT_NUM_CHANNEL];
}SAnalogInput;

typedef struct SModbusValue {
	SValueNode		Node[SYSTEM_NUM_TAG];
}SModbusValue;



typedef enum ETagInputType_ {
	TIT_AI = 0,
	TIT_MB,
}ETagInputType;


typedef enum ECompType_ {
	CT_NONE = 0,
	CT_OXY,
	CT_TERMP_PRESS,
	CT_TEMP_PRESS_OXY,
}ECompType;

typedef enum ETagStatus_ {
	TAG_STT_OK = 0,
	TAG_STT_MB_FAILED,
}ETagStatus;



typedef struct STag_ {
	uint16_t		id;
	ETagStatus		status;
	uint8_t			desc[15];
	uint8_t			meas_stt[3];
	bool			enable;
	bool			report;
	bool      		has_calib; 		//
	bool      		has_error; 		//
	bool      		alarm_enable; 	//
	ETagInputType	input_type;		// ai or modbus
	uint16_t		input_id; 		// slave address or ai index
	uint16_t		slave_reg_addr; // register addr to read value
	ECompType		comp_type; 		// compose none/o2/temp-press
	uint8_t    		name[15]; 		//
	uint8_t    		raw_unit[15]; //
	uint8_t    		std_unit[15]; //
	uint16_t       	pin_calib;   //	index on DI
	uint16_t       	pin_error;   //	index on DI
	uint16_t       	input_o2;    //	index on AI / MB
	uint16_t       	input_temp;  //	index on AI / MB
	uint16_t       	input_press; // index on AI / MB
	float    		o2_comp; 	 //
	float    		temp_comp;   //
	float    		press_comp;  //
	float    		raw_min;     //
	float    		raw_max;     //
	float    		scratch_min; //
	float    		scratch_max; //
	float    		coef_a;  	 //
	float    		coef_b;  	 //
	float    		alarm_value; 	//
	float    		scratch_value; 	//
	float    		raw_value; 		//
	float    		std_value; 		//
}STag;



typedef struct SCommon_ {
	ip_addr_t		dev_ip;
	ip_addr_t		server_ftp_ip;
	uint16_t		server_ftp_port;
	ip_addr_t		server_ctrl_ip;
	uint16_t		server_ctrl_port;
	uint8_t			tinh[6];
	uint8_t			coso[6];
	uint8_t			tram[10];
	uint8_t			scan_dur;	// second
	uint8_t			log_dur;	// minute
	uint32_t		modbus_brate;
}SCommon;


typedef struct SSysCfg_ {
	STag				sTag[SYSTEM_NUM_TAG];
	SCommon				sCom;
	SDigitalOutput		sDO;
}SSysCfg;

#endif /* APPLICATION_DEFINITION_H_ */
