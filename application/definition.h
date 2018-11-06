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

typedef struct SMsg_ {
	uint16_t	id;
	uint8_t		*pData;
}SMsg;


typedef struct SDigitalInput_ {
	uint32_t 	phy[DIGITAL_INPUT_NUM_CHANNEL];
	bool 		value[DIGITAL_INPUT_NUM_CHANNEL];
}SDigitalInput;

typedef struct SDigitalOutput_ {
	uint32_t 	phy[DIGITAL_OUTPUT_NUM_CHANNEL];
	bool 		value[DIGITAL_OUTPUT_NUM_CHANNEL];
}SDigitalOutput;

typedef struct SAnalogInput_ {
	uint32_t 	phy[ANALOG_INPUT_NUM_CHANNEL];
	double 		value[ANALOG_INPUT_NUM_CHANNEL];
}SAnalogInput;


typedef enum ETagInputType_ {
	TIT_AI = 0,
	TIT_MB,
}ETagInputType;


typedef enum ECompType_ {
	CT_NONE = 0,
	CT_OXY,
	CT_TERMP_PRESS,
}ECompType;

typedef enum ETagStatus_ {
	TAG_STT_OK = 0,
	TAG_STT_MB_FAILED,
}ETagStatus;



typedef struct STag_ {
	uint16_t		id;
	ETagStatus		status;
	uint8_t			desc[15];
	bool			enable;
	bool			report;
	bool      		has_calib; //
	bool      		has_error; //
	bool      		alarm_enable; //
	ETagInputType	input_type;	// ai or modbus
	uint16_t		input_id; // slave address or ai index
	uint16_t		slave_reg_addr;
	ECompType		comp_type; // compose none/o2/temp-press
	uint8_t    		name[15]; //
	uint8_t    		raw_unit[15]; //
	uint8_t    		std_unit[15]; //
	uint16_t       	pin_calib; //
	uint16_t       	pin_error; //
	uint16_t       	input_o2; //
	uint16_t       	input_temp; //
	uint16_t       	input_press; //
	float    		o2_comp; //
	float    		temp_comp; //
	float    		press_comp; //
	float    		raw_min;    //
	float    		raw_max;    //
	float    		scratch_min; //
	float    		scratch_max; //
	float    		coef_a;  //
	float    		coef_b;  //
	float    		alarm_value; //
	float    		scratch_value; //
	float    		raw_value; //
	float    		std_value; //
}STag;




#endif /* APPLICATION_DEFINITION_H_ */
