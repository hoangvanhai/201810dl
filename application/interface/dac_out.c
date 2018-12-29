/*
 * dac_out.c
 *
 *  Created on: Dec 29, 2018
 *      Author: PC
 */

#include <dac_out.h>

dac_converter_config_t  dacUserConfig;
vref_user_config_t		vrefUserConfig;

void DAC_InterfaceInit() {
//    vrefUserConfig.trimValue = 0x63;
//    VREF_DRV_Init(0, &vrefUserConfig);

//	dacUserConfig.dacRefVoltSrc = kDacRefVoltSrcOfVref1;
	dacUserConfig.lowPowerEnable = false;
    // Fill the structure with configuration of software trigger.
    DAC_DRV_StructInitUserConfigNormal(&dacUserConfig);
    // Initialize the DAC Converter.
    DAC_DRV_Init(0, &dacUserConfig);
}




void DAC_InterfaceSetLevel(uint16_t lev) {

	DAC_DRV_Output(0, (uint16_t)lev);
}
