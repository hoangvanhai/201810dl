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

// TAG config
#define DIGITAL_INPUT_NUM_CHANNEL			8
#define DIGITAL_OUTPUT_NUM_CHANNEL			6
#define ANALOG_INPUT_NUM_CHANNEL			12
#define SYSTEM_NUM_TAG						ANALOG_INPUT_NUM_CHANNEL

#define TAG_DESC_LENGTH			15
#define TAG_MEAS_STT_LENGTH		3
#define TAG_NAME_LENGTH			15
#define TAG_RAW_UNIT_LENGTH		15
#define TAG_STD_UNIT_LENGTH		15
#define SYS_TINH_LENGTH			6
#define SYS_COSO_LENGTH			6
#define SYS_TRAM_LENGTH			10
#define SYS_FTP_USRNAME_LENGTH	10
#define SYS_FTP_PASSWD_LENGTH	10
#define SYS_CTRL_USRNAME_LENGTH	10
#define SYS_CTRL_PASSWD_LENGTH	10

#define DO_CTRL_PORT_LENGTH		15


typedef enum ESysStatus_ {
	SYS_ERR_NONE		= 1 << 0,
	SYS_ERR_RTC			= 1 << 1,
	SYS_ERR_SDCARD_1	= 1 << 2,
	SYS_ERR_SDCARD_2	= 1 << 3,
	SYS_ERR_MODBUS		= 1 << 10,
	SYS_ERR_SPC			= 1 << 11,
	SYS_ERR_NET_PC		= 1 << 12,
	SYS_ERR_NET_FTP 	= 1 << 13,
	SYS_ERR_ETHER_CONN 	= 1 << 14,
	SYS_ERR_3G_CONN 	= 1 << 15,
	SYS_ERR_SUI			= 1 << 16,
}ESysStatus;


typedef enum EControlCode_ {
	CTRL_NONE = 0,
	CTRL_INIT_SDCARD_1 = 	1 << 1,
	CTRL_INIT_SDCARD_2 = 	1 << 2,
	CTRL_INIT_MODBUS = 		1 << 3,
}ECtrlCode;

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

typedef enum EDOActiveType_ {
	ACTIVE_HIGH = 0,
	ACTIVE_LOW
}EDOActiveType;


typedef enum EDOCtrlType_ {
	CTRL_PULSE = 0,
	CTRL_LEVEL
}EDOCtrlType;

typedef struct SCtrlPort_ {
	uint8_t			name[DO_CTRL_PORT_LENGTH];
	uint32_t 		port;
	EDOActiveType 	activeType;
	EDOCtrlType 	ctrlType;
	float			period;
	float			duty;	// percen
}SCtrlPort;


typedef struct SInputPort_ {
	uint8_t			name[DO_CTRL_PORT_LENGTH];
	uint32_t 		port;
	EDOActiveType 	activeType;
}SInputPort;

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



typedef struct SDigitalInputLog_ {
	SLogicNode		Node[DIGITAL_INPUT_NUM_CHANNEL];
}SDigitalInputLog;

typedef struct SDigitalOutput_ {
	SCtrlPort 		Node[DIGITAL_OUTPUT_NUM_CHANNEL];
}SDigitalOutput;

typedef struct SDigitalInput {
	SInputPort		Node[DIGITAL_INPUT_NUM_CHANNEL];
}SDigitalInput;

typedef struct SAnalogInput_ {
	SValueNode		Node[ANALOG_INPUT_NUM_CHANNEL];
}SAnalogInput;

typedef struct SModbusValue {
	SValueNode		Node[SYSTEM_NUM_TAG];
}SModbusValue;


typedef struct STag_ {
	uint16_t		id;
	ETagStatus		status;
	uint8_t			desc[TAG_DESC_LENGTH];
	uint8_t			meas_stt[TAG_MEAS_STT_LENGTH];
	bool			enable;
	bool			report;
	bool      		has_calib; 		//
	bool      		has_error; 		//
	bool      		alarm_enable; 	//
	ETagInputType	input_type;		// ai or modbus
	uint16_t		input_id; 		// slave address or ai index
	uint16_t		slave_reg_addr; // register addr to read value
	ECompType		comp_type; 		// compose none/o2/temp-press
	uint8_t    		name[TAG_NAME_LENGTH]; 		//
	uint8_t    		raw_unit[TAG_RAW_UNIT_LENGTH]; //
	uint8_t    		std_unit[TAG_STD_UNIT_LENGTH]; //
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
	uint8_t			tinh[SYS_TINH_LENGTH];
	uint8_t			coso[SYS_COSO_LENGTH];
	uint8_t			tram[SYS_TRAM_LENGTH];
	uint8_t			ftp_usrname[SYS_FTP_USRNAME_LENGTH];
	uint8_t			ftp_passwd[SYS_FTP_PASSWD_LENGTH];
	uint8_t			ctrl_usrname[SYS_CTRL_USRNAME_LENGTH];
	uint8_t			ctrl_passwd[SYS_CTRL_PASSWD_LENGTH];
	uint8_t			scan_dur;	// second
	uint8_t			log_dur;	// minute
	uint32_t		modbus_brate;
}SCommon;


typedef struct SSysCfg_ {
	STag				sTag[SYSTEM_NUM_TAG];
	SCommon				sCom;
	SDigitalOutput		sDO;
	SDigitalInput		sDI;
}SSysCfg;

#endif /* APPLICATION_DEFINITION_H_ */
