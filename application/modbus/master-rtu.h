
#ifndef MODBUS_MASTER_H_
#define MODBUS_MASTER_H_

/** @FILE NAME:    template.h
 *  @DESCRIPTION:  This file for ...
 *
 *  Copyright (c) 2018 EES Ltd.
 *  All Rights Reserved This program is the confidential and proprietary
 *  product of EES Ltd. Any Unauthorized use, reproduction or transfer
 *  of this program is strictly prohibited.
 *
 *  @Author: HaiHoang
 *  @NOTE:   No Note at the moment
 *  @BUG:    No known bugs.
 *
 *<pre>
 *  MODIFICATION HISTORY:
 *
 *  Ver   Who       Date                Changes
 *  ----- --------- ------------------  ----------------------------------------
 *  1.00  HaiHoang  August 1, 2018      First release
 *
 *
 *</pre>
 ******************************************************************************/

/***************************** Include Files *********************************/
#include <stdint.h>
#include <layer1.h>
#include <checksum.h>
/************************** Constant Definitions *****************************/


#define MB_DELAY_MS				100

#define MB_LSV_ADDR_IDX         0
#define MB_FUNC_CODE_IDX        1
#define MB_DATA_ADDR_IDX        2
#define MB_NUM_OF_REQ_IDX       4
#define MB_CRC_MODBUS_IDX       6

#define MB_DATA_COUNT_IDX       2
#define MB_DATA_RESP_IDX        3


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



/**************************** Type Definitions *******************************/

/***************** Macros (Inline Functions) Definitions *********************/

/************************** Function Prototypes ******************************/
uint16_t MBMaster_Read(SModbus *pModbus, uint8_t slv_adrr, uint8_t fc,
						uint16_t data_addr, uint16_t num_data,
						uint8_t *read_frame, uint16_t *len);

uint16_t MBMaster_Parse(const uint8_t* data, uint8_t data_format, void *rdata);


uint16_t MBMaster_Write(uint8_t id, uint8_t fc, uint16_t addr, uint8_t num_reg, uint8_t *data, uint16_t len);
/************************** Variable Definitions *****************************/

/*****************************************************************************/



#endif /* MODBUS_MASTER_H_ */
