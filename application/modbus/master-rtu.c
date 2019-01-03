/** @FILE NAME:    template.c
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
#include <master-rtu.h>

/************************** Constant Definitions *****************************/

/**************************** Type Definitions *******************************/

/***************** Macros (Inline Functions) Definitions *********************/


/************************** Function Prototypes ******************************/

/************************** Variable Definitions *****************************/

/*****************************************************************************/
/** @brief make modbus rtu frame and send immediately
 *
 *
 *  @param
 *  @return Void.
 *  @note
 */
uint16_t MBMaster_Read(SModbus *pModbus, uint8_t slv_adrr, uint8_t fc, uint16_t data_addr,
						uint16_t num_data, uint8_t *read_frame,
						uint16_t *len) {
	uint8_t retVal;
	uint8_t sendBuff[8];
	uint16_t rlen = 0;
	if(pModbus == NULL)
		return MB_ERR_INVALID_PTR;

	sendBuff[MB_LSV_ADDR_IDX] = slv_adrr;
	sendBuff[MB_FUNC_CODE_IDX] = fc;
	sendBuff[MB_DATA_ADDR_IDX] = (data_addr >> 8) & 0xFF;
	sendBuff[MB_DATA_ADDR_IDX + 1] = data_addr & 0xFF;
	sendBuff[MB_NUM_OF_REQ_IDX] = (num_data >> 8) & 0xFF;
	sendBuff[MB_NUM_OF_REQ_IDX + 1] = num_data & 0xFF;

	uint16_t crc = crc_modbus((const uint8_t*)sendBuff, 6);
	sendBuff[MB_CRC_MODBUS_IDX] = crc & 0xFF;
	sendBuff[MB_CRC_MODBUS_IDX + 1] = (crc >> 8) & 0xFF;

	retVal = Modbus_SendAndRecv(pModbus, sendBuff, 8, read_frame, &rlen, MB_DELAY_MS);
	*len = rlen;
	if(rlen > 0) {
		// check crc here
		crc = crc_modbus(read_frame, rlen -2);
		uint8_t high = (crc >> 8) & 0xFF;
		uint8_t low = crc & 0xFF;

		if(!((read_frame[rlen - 2] == low) && (read_frame[rlen - 1] == high))) {
			retVal = MB_ERR_CRCD;
		}
	}
	return retVal;
}

/*****************************************************************************/
/** @brief
 *
 *
 *  @param
 *  @return Void.
 *  @note
 */
uint16_t MBMaster_Parse(const uint8_t* data, uint8_t data_format, uint8_t data_order, float *rdata) {
	uint8_t dataSize = 1, recvObjNum;
	switch(data_format) {
	case Integer_8bits: dataSize = 1; break;
	case Integer_16bits: dataSize = 2; break;
	case Integer_32bits: dataSize = 4; break;
	case Float_32bits: dataSize = 4; break;
	default: ASSERT(false); break;
	}

	recvObjNum = data[MB_DATA_COUNT_IDX] / dataSize;
	ASSERT_NONVOID(recvObjNum == 1, recvObjNum);

	for(int i = 0, j = 0; i < recvObjNum; i++, j+=dataSize) {
		switch(data_format) {
		case Integer_8bits: {
			*rdata = (float)data[MB_DATA_RESP_IDX + j];
		} break;
		case Integer_16bits: {
			uint16_t value;
			switch(data_order) {
			case HiBFist_HiWFirst:
				value = ((data[MB_DATA_RESP_IDX + j] << 8) & 0xFF00) |
				                    (data[MB_DATA_RESP_IDX + j + 1]);
				break;

			case HiBFist_LoWFirst:
				break;

			case LoBFist_HiWFirst:
				break;

			case LoBFist_LoWFirst:
				value = ((data[MB_DATA_RESP_IDX + j + 1] << 8) & 0xFF00) |
						(data[MB_DATA_RESP_IDX + j ]);
				break;

			default:
				break;
			}

			*rdata = (float)value;

		} break;
		case Integer_32bits: {
			uint32_t value;

			switch(data_order) {
			case HiBFist_HiWFirst:
			value = (data[MB_DATA_RESP_IDX + j]     << 24)  |
							 (data[MB_DATA_RESP_IDX + j + 1] << 16)  |
							 (data[MB_DATA_RESP_IDX + j + 2] << 8)   |
							 (data[MB_DATA_RESP_IDX + j + 3]);
			break;

			case HiBFist_LoWFirst:
				break;

			case LoBFist_HiWFirst:
				break;

			case LoBFist_LoWFirst:
				value = (data[MB_DATA_RESP_IDX + j + 3]     << 24)  |
								 (data[MB_DATA_RESP_IDX + j + 2] << 16)  |
								 (data[MB_DATA_RESP_IDX + j + 1] << 8)   |
								 (data[MB_DATA_RESP_IDX + j]);
				break;

			default:
				break;
			}

			*rdata = (float)value;


		} break;
		case Float_32bits: {
			unionfloat32 value;
			switch(data_order) {
			case HiBFist_HiWFirst:
				value.u8[0] = data[MB_DATA_RESP_IDX + j];
				value.u8[1] = data[MB_DATA_RESP_IDX + j + 1];
				value.u8[2] = data[MB_DATA_RESP_IDX + j + 2];
				value.u8[3] = data[MB_DATA_RESP_IDX + j + 3];
			break;

			case HiBFist_LoWFirst:
				value.u8[2] = data[MB_DATA_RESP_IDX + j];
				value.u8[3] = data[MB_DATA_RESP_IDX + j + 1];
				value.u8[0] = data[MB_DATA_RESP_IDX + j + 2];
				value.u8[1] = data[MB_DATA_RESP_IDX + j + 3];
				break;

			case LoBFist_HiWFirst:
				value.u8[1] = data[MB_DATA_RESP_IDX + j];
				value.u8[0] = data[MB_DATA_RESP_IDX + j + 1];
				value.u8[3] = data[MB_DATA_RESP_IDX + j + 2];
				value.u8[2] = data[MB_DATA_RESP_IDX + j + 3];
				break;

			case LoBFist_LoWFirst:
				value.u8[3] = data[MB_DATA_RESP_IDX + j];
				value.u8[2] = data[MB_DATA_RESP_IDX + j + 1];
				value.u8[1] = data[MB_DATA_RESP_IDX + j + 2];
				value.u8[0] = data[MB_DATA_RESP_IDX + j + 3];
				break;

			default:
				break;
			}

			*rdata = value.f32;


		} break;
		default:
			ASSERT(false);
			break;
		}
	}

	return recvObjNum;
}
/*****************************************************************************/
/** @brief
 *
 *
 *  @param
 *  @return Void.
 *  @note
 */
uint16_t MBMaster_Write(uint8_t id, uint8_t fc, uint16_t addr,
						uint8_t num_reg, uint8_t *data, uint16_t len) {

	return 0;
}











