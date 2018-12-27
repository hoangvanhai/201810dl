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
#include <time.h>


#define LOGGER_DEV_ID             			0x02
#define LOGGER_PC_ID         				0x01
#define LOGGER_UI_ID         				0x01

// TAG config
#define DIGITAL_INPUT_NUM_CHANNEL			8
#define DIGITAL_OUTPUT_NUM_CHANNEL			6
#define ANALOG_INPUT_NUM_CHANNEL			12
#define SYSTEM_NUM_TAG						ANALOG_INPUT_NUM_CHANNEL

#define TAG_DESC_LENGTH                     15
#define TAG_MEAS_STT_LENGTH                 3
#define TAG_NAME_LENGTH                     15
#define TAG_RAW_UNIT_LENGTH                 15
#define TAG_STD_UNIT_LENGTH                 15
#define SYS_TINH_LENGTH                     6
#define SYS_COSO_LENGTH                     6
#define SYS_TRAM_LENGTH                     10
#define SYS_FTP_PREFIX_LENGTH               30
#define SYS_FTP_USRNAME_LENGTH              15
#define SYS_FTP_PASSWD_LENGTH               15
#define SYS_CTRL_USRNAME_LENGTH             15
#define SYS_CTRL_PASSWD_LENGTH              15
#define SYS_CFG_USER_LENGTH                 15
#define SYS_CFG_PASSWD_LENGTH               15
#define PORT_NAME_LENGTH                    15

#define LOGGER_SUCCESS                      0
#define LOGGER_ERROR                        1


// PC config definition
#define	LOGGER_SET                          0x80
#define LOGGER_GET                          0x00

#define LOGGER_TIME                         0x01
#define LOGGER_COMMON                       0x02
#define LOGGER_TAG                          0x03
#define LOGGER_DI                           0x04
#define LOGGER_DO                           0x05
#define LOGGER_MONITOR_HW                   0x06
#define LOGGER_WRITE_SUCCESS                0x07
#define LOGGER_WRITE_DONE                   0x08
#define LOGGER_SYSTEM_STATUS				0x09

#define LOGGER_LOGIN                        0x10
#define LOGGER_LOGOUT                       0x11
#define LOGGER_CHANGE_PASSWD                0x12


#define LOGGER_STREAM_AI                    0x20
#define LOGGER_STREAM_MB                    0x21
#define LOGGER_CALIB_AI                     0x22
#define LOGGER_CALIB_CURR_PWR               0x23

#define LOGGER_STREAM_HEADER				0x30
#define LOGGER_STREAM_VALUE					0x31
#define LOGGER_STREAM_DI					0x32
#define LOGGER_STREAM_DO					0x33

#define STREAM_AI                           0x01
#define STREAM_MB                           0x02
#define STREAM_VALUE                        0x04

// Control server difinition
#define SER_GET_INPUT_ALL           		0x0001
#define SER_GET_INPUT_GROUP         		0x0002
#define SER_GET_INPUT_CHAN          		0x0003
#define SER_SET_SAMPLE_START        		0x0011
#define LOG_REQ_CALIB_START         		0x0021
#define SER_SET_CALIB_START         		0x0022
#define LOGGER_LOGGING_IN           		0x0100
#define SER_LOGGING_STATUS          		0x0101
#define LOGGER_LOGGING_OUT          		0x0102



#define GET_MSG_TYPE(data)          ((data)[0] << 8 | (data)[1])


#define defaultMAC_ADDR0	0x00
#define defaultMAC_ADDR1	0xCF
#define defaultMAC_ADDR2	0x52
#define defaultMAC_ADDR3	0x35
#define defaultMAC_ADDR4	0x00
#define defaultMAC_ADDR5	0x02



enum ControlMsg {
	Control_Get_Input_All = 0,
	Control_Get_Input_Channel,
	Control_Set_Sample,
};

/* ui definition */



/* modbus definition */
#define MB_LSV_ADDR_IDX                     0
#define MB_FUNC_CODE_IDX                    1
#define MB_DATA_ADDR_IDX                    2
#define MB_NUM_OF_REQ_IDX                   4
#define MB_CRC_MODBUS_IDX                   6

#define MB_DATA_COUNT_IDX                   2
#define MB_DATA_RESP_IDX                    3




enum MbFunctionCode {
    Read_Coils = 0x1,               // single bit access
    Read_Descrete_Inputs = 0x2,     //
    Write_Single_Coil = 0x5,        //
    Write_Multiple_Coils = 0xF,     //
    Read_Holding_Register = 0x3,            // 16 bits access
    Read_Input_Register = 0x4,              //
    Write_Single_Register = 0x6,            //
    Write_Multiple_Registers = 0x10,        //16
    Mask_Write_Register = 0x16,             //22
    Read_Write_Multiple_Registers = 0x17,   //23
    Read_Fifo_Queue = 0x18,                 //24
    Read_File_Record = 0x14,        //20 file recored access
    Write_File_Record = 0x15,       //21
    Diagnostic = 0x8,               // Diagnostic
    Get_Com_Event_Counter = 0xB,    //
    Get_Com_Event_Log = 0xC,        //
    Report_Server_ID = 0x11,        //
};

enum MbDataType {
    Integer_8bits = 0,
    Integer_16bits,
    Integer_32bits,
    Float_32bits
};

typedef enum ESysStatus_ {
    SYS_ERR_NONE		= 0 << 0,
    SYS_ERR_RTC			= 1 << 1,
    SYS_ERR_SDCARD_1	= 1 << 2,
    SYS_ERR_SDCARD_2	= 1 << 3,
    SYS_ERR_MODBUS		= 1 << 10,
    SYS_ERR_SPC			= 1 << 11,
    SYS_ERR_NET_PC		= 1 << 11,
    SYS_ERR_NET_FTP 	= 1 << 12,
    SYS_ERR_ETHER_CONN 	= 1 << 13,
    SYS_ERR_3G_CONN 	= 1 << 14,
    SYS_ERR_SUI			= 1 << 15,
}ESysStatus;


typedef enum EControlCode_ {
    CTRL_NONE = 0,
    CTRL_INIT_SDCARD_1 = 	1 << 1,
    CTRL_INIT_SDCARD_2 = 	1 << 2,
    CTRL_INIT_MODBUS = 		1 << 3,
	CTRL_SEND_HEADER = 		1 << 4,
	CTRL_GET_WL_STT = 		1 << 5,
}ECtrlCode;

typedef enum ECfgConnType_ {
    CFG_CONN_SERIAL = 0,
    CFG_CONN_NET
}ECfgConnType;

typedef enum ETagInputType_ {
    TIT_AI = 0,
    TIT_MB,
}ETagInputType;

typedef enum EDataByteOrder_ {
    HiBFist_HiWFirst = 0,    // big endian
    HiBFist_LoWFirst,
    LoBFist_HiWFirst,
    LoBFist_LoWFirst    // little endian
}EDataByteOrder;

typedef enum ECompType_ {
    CT_NONE = 0,
    CT_OXY,
    CT_TEMP_PRESS,
    CT_TEMP_PRESS_OXY,
}ECompType;

typedef enum ETagStatus_ {
    TAG_STT_OK = 0,
    TAG_STT_MB_FAILED,
	TAG_STT_AI_FAILED,
}ETagStatus;

typedef enum EDOActiveType_ {
    ACTIVE_HIGH = 0,
    ACTIVE_LOW
}EDOActiveType;


typedef enum EDOCtrlType_ {
    CTRL_PULSE = 0,
    CTRL_LEVEL
}EDOCtrlType;


typedef struct SSystemStatus_ {
	uint8_t		eth_stat;
	ip_addr_t	ip;
	uint8_t		sim_stat;
	uint8_t		sdcard1_stat;
	uint8_t		sdcard2_stat;
	uint8_t		curr_out;
	uint8_t		simid[22];
	uint8_t		netid[22];
	uint8_t		rssi;
	struct		tm time;
}SSystemStatus;



typedef struct SCtrlMsg_ {
    uint16_t		id;
    uint8_t			*pData;
}SCtrlMsg;


typedef struct SLNode_ {
    uint32_t 		id;
    bool			lev;
}SLogicNode;


typedef struct SVMBNode_ {
	uint8_t			enable;
	uint8_t 		status;
	uint8_t			data_format;
	uint8_t			data_type;
	uint8_t			data_order;
    uint8_t 		address;
    uint16_t		reg_address;
    float 			value;
}SMBValueNode;

typedef struct SVAINode_ {
	uint8_t			enable;
	uint8_t 		status;
    uint8_t 		id;
    float 			value;
}SAiValueNode;

typedef struct SCalibNode_ {
	float			offset;
	float			coefficient;
	float			x1;
	float			x2;
	float			raw;
}SCalibNode;

typedef struct SCtrlPort_ {
    uint8_t			name[PORT_NAME_LENGTH];
    uint32_t 		port;
    uint8_t         activeType;
    uint8_t         ctrlType;
    uint16_t		period;
    uint16_t		duty;	// percen
}SCtrlPort;

typedef struct SInputPort_ {
    uint8_t			name[PORT_NAME_LENGTH];
    uint32_t 		port;
    uint8_t         activeType;
}SInputPort;

typedef struct SDigitalInputLog_ {
    SLogicNode		Node[DIGITAL_INPUT_NUM_CHANNEL];
}SDigitalInputLog;

typedef struct SDigitalOutputLog_ {
    SLogicNode		Node[DIGITAL_OUTPUT_NUM_CHANNEL];
}SDigitalOutputLog;

typedef struct SAnalogInput_ {
	SAiValueNode	Node[ANALOG_INPUT_NUM_CHANNEL];
}SAnalogInput;

typedef struct SAnalogCalib_ {
	SCalibNode		calib[ANALOG_INPUT_NUM_CHANNEL];
}SAnalogCalib;

typedef struct SModbusValue {
	SMBValueNode	Node[SYSTEM_NUM_TAG];
}SModbusValue;


typedef struct STag_ {
    uint16_t		id;
    uint8_t			desc[TAG_DESC_LENGTH];
    uint8_t			enable;
    uint8_t			report;
    uint8_t    		has_calib; 		//
    uint8_t    		has_error; 		//
    uint8_t    		alarm_enable; 	//
    uint8_t         input_type;		// ai or modbus
    uint16_t		input_id; 		// slave address or ai index
    uint16_t		slave_reg_addr; // register addr to read value
    uint8_t         data_type;
    uint8_t         data_format;
    uint8_t         data_order;
    uint8_t         comp_type; 		// compose none/o2/temp-press
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

}STag;


typedef struct STagHeader_ {
	uint16_t 		id;
	uint8_t			enable;
	uint8_t    		name[TAG_NAME_LENGTH];
	uint8_t    		raw_unit[TAG_RAW_UNIT_LENGTH];
	uint8_t    		std_unit[TAG_STD_UNIT_LENGTH];
	float			min;
	float			max;
	float    		alarm_value;
	uint8_t    		alarm_enable;
}STagHeader;

typedef struct STagRV_ {
	float			raw_value;
	float			std_value;
	uint8_t			meas_stt[TAG_MEAS_STT_LENGTH];
}STagRV;

typedef struct STagNode_ {
    uint16_t 		id;
    uint8_t         status;
    float    		scratch_value;
    float    		raw_value;
    float    		std_value;
    uint8_t    		name[TAG_NAME_LENGTH];
    uint8_t    		raw_unit[TAG_RAW_UNIT_LENGTH];
    uint8_t    		std_unit[TAG_STD_UNIT_LENGTH];
    uint8_t			meas_stt[TAG_MEAS_STT_LENGTH];
    float    		alarm_value;
    uint8_t    		alarm_enable;
}STagNode;

typedef struct STagValue_ {
    STagNode 		Node[SYSTEM_NUM_TAG];
}STagValue;

typedef struct STagHArray_ {
    STagHeader      Node[SYSTEM_NUM_TAG];
}STagHArray;


typedef struct STagVArray_ {
    STagRV          Node[SYSTEM_NUM_TAG];
}STagVArray;


typedef struct SCommon_ {
    ip_addr_t		dev_ip;
    ip_addr_t       dev_netmask;
    ip_addr_t       dev_gw;
    uint8_t			dev_hwaddr[6];
    uint8_t         dev_dhcp;
    uint8_t         ftp_enable1;
    uint8_t         ftp_enable2;
    ip_addr_t		server_ftp_ip1;
    uint16_t		server_ftp_port1;
    ip_addr_t		server_ftp_ip2;
    uint16_t		server_ftp_port2;
    ip_addr_t		server_ctrl_ip;
    uint16_t		server_ctrl_port;
    uint8_t			tinh[SYS_TINH_LENGTH];
    uint8_t			coso[SYS_COSO_LENGTH];
    uint8_t			tram[SYS_TRAM_LENGTH];
    uint8_t			ftp_prefix1[SYS_FTP_PREFIX_LENGTH];
    uint8_t			ftp_usrname1[SYS_FTP_USRNAME_LENGTH];
    uint8_t			ftp_passwd1[SYS_FTP_PASSWD_LENGTH];
    uint8_t			ftp_prefix2[SYS_FTP_PREFIX_LENGTH];
    uint8_t			ftp_usrname2[SYS_FTP_USRNAME_LENGTH];
    uint8_t			ftp_passwd2[SYS_FTP_PASSWD_LENGTH];
    uint8_t			ctrl_usrname[SYS_CTRL_USRNAME_LENGTH];
    uint8_t			ctrl_passwd[SYS_CTRL_PASSWD_LENGTH];
    uint8_t			scan_dur;	// second
    uint8_t			log_dur;	// minute
    uint32_t		modbus_brate;
}SCommon;


typedef struct SAccount_ {
	uint8_t 		username[SYS_CFG_USER_LENGTH];
	uint8_t			password[SYS_CFG_PASSWD_LENGTH];
	uint8_t			rootname[SYS_CFG_USER_LENGTH];
	uint8_t			rootpass[SYS_CFG_PASSWD_LENGTH];
}SAccount;

typedef struct SSysCfg_ {
    SCommon				sCom;
    STag				sTag[SYSTEM_NUM_TAG];
    SCtrlPort           sDO[DIGITAL_OUTPUT_NUM_CHANNEL];
    SInputPort          sDI[DIGITAL_INPUT_NUM_CHANNEL];
    SAccount			sAccount;
    SAnalogCalib		sAiCalib;
}SSysCfg;



#endif /* APPLICATION_DEFINITION_H_ */
