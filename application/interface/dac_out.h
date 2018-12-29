/*
 * dac_out.h
 *
 *  Created on: Dec 29, 2018
 *      Author: PC
 */

#ifndef APPLICATION_INTERFACE_DAC_OUT_H_
#define APPLICATION_INTERFACE_DAC_OUT_H_

#include <fsl_dac_driver.h>
#include <fsl_vref_driver.h>


void DAC_InterfaceInit();
void DAC_InterfaceSetLevel(uint16_t lev);

#endif /* APPLICATION_INTERFACE_DAC_OUT_H_ */
