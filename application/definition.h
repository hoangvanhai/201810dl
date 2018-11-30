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



#define LOGGER_LOGIN                        0x10
#define LOGGER_LOGOUT                       0x11
#define LOGGER_CHANGE_PASSWD                0x12

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
    SYS_ERR_NONE		= 1 << 0,
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
    float 		value;
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
}ETagStatus;



typedef struct SDigitalInputLog_ {
    SLogicNode		Node[DIGITAL_INPUT_NUM_CHANNEL];
}SDigitalInputLog;

typedef struct SAnalogInput_ {
    SValueNode		Node[ANALOG_INPUT_NUM_CHANNEL];
}SAnalogInput;

typedef struct SModbusValue {
    SValueNode		Node[SYSTEM_NUM_TAG];
}SModbusValue;


typedef struct STag_ {
    uint16_t		id;
    uint8_t         status;
    uint8_t			desc[TAG_DESC_LENGTH];
    uint8_t			meas_stt[TAG_MEAS_STT_LENGTH];
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
    float    		scratch_value; 	//
    float    		raw_value; 		//
    float    		std_value; 		//
}STag;


typedef struct SCommon_ {
    ip_addr_t		dev_ip;
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


typedef struct SSysCfg_ {
    STag				sTag[SYSTEM_NUM_TAG];
    SCommon				sCom;
    SCtrlPort           sDO[DIGITAL_OUTPUT_NUM_CHANNEL];
    SInputPort          sDI[DIGITAL_INPUT_NUM_CHANNEL];
}SSysCfg;



#endif /* APPLICATION_DEFINITION_H_ */
