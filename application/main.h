/*
 * This is template for main module created by KSDK Project Generator. Enjoy!
 */

 /*
 * [File Name]     main.h
 * [Platform]      MK64FN1M0VLL12
 * [Project]       ucosiii_app
 * [Version]       1.00
 * [Author]        PC
 * [Date]          10/22/2018
 * [Language]      'C'
 * [History]       1.00 - Original Release
 *
 */

//-----------------------------------------------------------------------
// KSDK Includes
//-----------------------------------------------------------------------
#include "fsl_device_registers.h"
#include "fsl_debug_console.h"
#include "fsl_clock_manager.h"
#include "fsl_interrupt_manager.h"
#include "fsl_power_manager.h"
#include "fsl_os_abstraction.h"
#include "fsl_adc16_driver.h"
#include "fsl_cmp_driver.h"
#include "fsl_cmt_driver.h"
#include "fsl_crc_driver.h"
#include "fsl_dac_driver.h"
#include "fsl_dspi_edma_master_driver.h"
#include "fsl_dspi_edma_slave_driver.h"
#include "fsl_dspi_master_driver.h"
#include "fsl_dspi_slave_driver.h"
#include "fsl_edma_driver.h"
#include "fsl_enet_driver.h"
#include "fsl_ewm_driver.h"
#include "fsl_flexbus_driver.h"
#include "fsl_flexcan_driver.h"
#include "fsl_ftm_driver.h"
#include "fsl_gpio_driver.h"
#include "fsl_i2c_master_driver.h"
#include "fsl_i2c_slave_driver.h"
#include "fsl_lptmr_driver.h"
#include "fsl_mpu_driver.h"
#include "fsl_pdb_driver.h"
#include "fsl_pit_driver.h"
#include "fsl_rnga_driver.h"
#include "fsl_rtc_driver.h"
#include "fsl_sai_driver.h"
#include "fsl_sdhc_driver.h"
#include "fsl_smartcard_driver.h"
#if defined(FSL_FEATURE_SOC_EMVSIM_COUNT)
#if (FSL_FEATURE_SOC_EMVSIM_COUNT >= 1)
#include "fsl_smartcard_emvsim_driver.h"
#endif
#endif
#if defined(FSL_FEATURE_UART_HAS_SMART_CARD_SUPPORT)
#if (FSL_FEATURE_UART_HAS_SMART_CARD_SUPPORT == 1)
#include "fsl_smartcard_uart_driver.h"
#endif
#endif
#include "fsl_uart_driver.h"
#include "fsl_uart_edma_driver.h"
#include "fsl_vref_driver.h"
#include "fsl_wdog_driver.h"
#if USING_DIRECT_INTERFACE
#include "fsl_smartcard_direct_driver.h"
#endif
#if USING_NCN8025_INTERFACE
#include "fsl_smartcard_ncn8025_driver.h"
#endif

////////////////////////////////////////////////////////////////////////////////
// EOF
////////////////////////////////////////////////////////////////////////////////
