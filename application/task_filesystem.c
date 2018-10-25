
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
#include "fsl_debug_console.h"
#include <board.h>
#include <includes.h>
#include <fsl_sdhc_card.h>
#include <app.h>

/************************** Constant Definitions *****************************/
#define TEST_BLOCK_NUM          4U
#define TEST_START_BLOCK        4U
/**************************** Type Definitions *******************************/
typedef enum
{
    kTestResultPassed = 0U,
    kTestResultFailed,
    kTestResultInitFailed,
    kTestResultAborted,
} test_result_t;

/***************** Macros (Inline Functions) Definitions *********************/

/************************** Function Prototypes ******************************/
static test_result_t demo_card_data_access(void);
/************************** Variable Definitions *****************************/

/*****************************************************************************/

const char *monthStr[] = {
    "Jan", "Feb", "Mar", "Apr", "May", "Jun",
    "Jul", "Aug", "Sep", "Oct", "Nov", "Dec",
};
static uint8_t refData[FSL_SDHC_CARD_DEFAULT_BLOCK_SIZE]; /*!< Buffer to hold a reference data used for write/read/compare tests */
static uint8_t testData[FSL_SDHC_CARD_DEFAULT_BLOCK_SIZE]; /*!< Buffer to reac back data from card for write/read/compare tests */
static uint8_t refData2[FSL_SDHC_CARD_DEFAULT_BLOCK_SIZE*TEST_BLOCK_NUM]; /*!< Buffer to hold a reference data used for write/read/compare tests */
static uint8_t testData2[FSL_SDHC_CARD_DEFAULT_BLOCK_SIZE*TEST_BLOCK_NUM]; /*!< Buffer to reac back data from card for write/read/compare tests */
static volatile uint32_t cardInserted = 0; /*!< Flag to indicate a card has been inserted */
static volatile uint32_t cardInited = 0; /*!< Flag to indicate the card has been initialized successfully */

/*****************************************************************************/
/** @brief
 *
 *
 *  @param
 *  @return Void.
 *  @note
 */

void task_filesystem(task_param_t param)
{

	GPIO_DRV_Init(sdhcCdPin, NULL);

	OS_ERR err;
    while (1)
    {
		OSTaskSemPend(1000, OS_OPT_PEND_BLOCKING, 0, &err);
		if(err == OS_ERR_NONE) {
			LREP("sd card detect: %d\r\n", cardInserted);
			if(cardInserted == true) {
				demo_card_data_access();
			}
		}
    }
}


/*****************************************************************************/
/** @brief
 *
 *
 *  @param
 *  @return Void.
 *  @note
 */
static uint32_t fill_reference_data(uint8_t *pdata, uint8_t seed, uint32_t len)
{
    uint32_t i, j;

    // Check if either buffer pointer or length parameters are null
    if ((NULL == pdata) || (0 == len))
    {
        return 1U;
    }

    // clear buffer with all '0's
    memset(pdata, 0, len);

    // fill in the buffer processed with seed value
    for (i = 0; i < len; i++)
    {
        j = i % 4;
        switch(j)
        {
            case 0:
                pdata[i] = 0xA0 | (seed & 0x0FU);
                break;
            case 1:
                pdata[i] = 0x0B | (seed & 0xF0U);
                break;
            case 2:
                pdata[i] = 0xC0 | (seed & 0x0FU);
                break;
            case 3:
                pdata[i] = 0x0D | (seed & 0xF0U);
                break;
            default:
                break;
        }
    }
    return 0;
}

/*****************************************************************************/
/** @brief
 *
 *
 *  @param
 *  @return Void.
 *  @note
 */
static void show_card_cid(sdcard_cid_t *cid)
{
    int8_t temp[32];
    LREP("Manufacturer ID: 0x%X\r\n", cid->mid);
    LREP("OEM ID: 0x%X\r\n", cid->oid);
    LREP("Product name: %s\r\n", cid->pnm);
    LREP("Product serial number: 0x%X\r\n", (unsigned int)cid->psn);
    memset(temp, 0, sizeof(temp));
    snprintf((char *)temp, 4, "%u.%u", (cid->prv & 0xF0) >> 4, (cid->prv & 0xF));
    LREP("Product revision: %s\r\n", temp);
    memset(temp, 0, sizeof(temp));
    snprintf((char *)temp, sizeof(temp),
            "%s %u",
            monthStr[(cid->mdt & 0xF) - 1], 2000 + ((cid->mdt & 0xFF0) >> 4));
    LREP("Manufacturing data: %s\r\n", temp);
}

/*****************************************************************************/
/** @brief
 *
 *
 *  @param
 *  @return Void.
 *  @note
 */
static float decode_taac(uint8_t taac)
{
    uint32_t t1, t2;
    float out = 0;

    // Below parsing done as per SD Physical Layer specification
    t1 = taac & 0x7;
    t2 = (taac & 0x78U) >> 3;
    switch(t1)
    {
        case 0:
            t1 = 1;
            break;
        case 1:
            t1 = 10;
            break;
        case 2:
            t1 = 100;
            break;
        case 3:
            t1 = 1000;
            break;
        case 4:
            t1 = 10000;
            break;
        case 5:
            t1 = 100000;
            break;
        case 6:
            t1 = 1000000;
            break;
        case 7:
            t1 = 10000000;
            break;
        default:
            break;
    }
    switch(t2)
    {
        case 1:
            out = 1;
            break;
        case 2:
            out = (float)1.2;
            break;
        case 3:
            out = (float)1.3;
            break;
        case 4:
            out = (float)1.5;
            break;
        case 5:
            out = (float)2.0;
            break;
        case 6:
            out = (float)2.5;
            break;
        case 7:
            out = (float)3.0;
            break;
        case 8:
            out = (float)3.5;
            break;
        case 9:
            out = (float)4.0;
            break;
        case 0xa:
            out = (float)4.5;
            break;
        case 0xb:
            out = (float)5.0;
            break;
        case 0xc:
            out = (float)5.5;
            break;
        case 0xd:
            out = (float)6.0;
            break;
        case 0xe:
            out = (float)7.0;
            break;
        case 0xf:
            out = (float)8.0;
            break;
        default:
            break;
    }
    return t1 * out;
}

/*****************************************************************************/
/** @brief
 *
 *
 *  @param
 *  @return Void.
 *  @note
 */
static float decode_transpeed(uint8_t ts)
{
    uint32_t t1, t2;
    float out = 0;

    // Parse card data transfer speed as per SD Physical Layer Specification
    t1 = ts & 0x7;
    t2 = (ts & 0x78U) >> 3;
    switch(t1)
    {
        case 0:
            t1 = 100;
            break;
        case 1:
            t1 = 1000000;
            break;
        case 2:
            t1 = 10000000;
            break;
        case 3:
            t1 = 100000000;
            break;
        default:
            break;
    }
    switch(t2)
    {
        case 1:
            out = 1;
            break;
        case 2:
            out = (float)1.2;
            break;
        case 3:
            out = (float)1.3;
            break;
        case 4:
            out = (float)1.5;
            break;
        case 5:
            out = (float)2.0;
            break;
        case 6:
            out = (float)2.5;
            break;
        case 7:
            out = (float)3.0;
            break;
        case 8:
            out = (float)3.5;
            break;
        case 9:
            out = (float)4.0;
            break;
        case 0xa:
            out = (float)4.5;
            break;
        case 0xb:
            out = (float)5.0;
            break;
        case 0xc:
            out = (float)5.5;
            break;
        case 0xd:
            out = (float)6.0;
            break;
        case 0xe:
            out = (float)7.0;
            break;
        case 0xf:
            out = (float)8.0;
            break;
        default:
            break;
    }
    return t1 * out;
}

/*****************************************************************************/
/** @brief
 *
 *
 *  @param
 *  @return Void.
 *  @note
 */
static void show_card_csd(sdcard_csd_t *csd)
{
    uint32_t i = 0;

    // Show CSD structure value
    LREP("CSD Structure: 0x%X\r\n", csd->csdStructure);

    // decode and show card data read access time 1 value
    LREP("taac: %.02f ns\r\n", decode_taac(csd->taac));

    // decode and show card data read access time 2 in clock cycles value
    LREP("nsac: %d clks\r\n", csd->nsac);

    // decode and show card data transfer speed
    LREP("tran speed: %.2f kbps\r\n", decode_transpeed(csd->tranSpeed));

    // parse and show command classes supported by the card
    LREP("ccc: class ");
    while ((0 != csd->ccc) && (i < 12))
    {
        if (csd->ccc & 0x1)
        {
            LREP("%d ", (int)i);
        }
        i++;
        csd->ccc >>= 1;
    }
    LREP("\r\n");

    // show Max. read data block length
    LREP("max read block length: %d Bytes\r\n", (int)((uint32_t)1 << csd->readBlkLen));

    // parse and display card CSD flags
    if (csd->flags & SDCARD_CSD_READ_BL_PARTIAL)
    {
        LREP("Support partial read\r\n");
    }
    if (csd->flags & SDCARD_CSD_WRITE_BLK_MISALIGN)
    {
        LREP("Support crossing physical block boundaries writing is allowed\r\n");
    }
    if (csd->flags & SDCARD_CSD_READ_BLK_MISALIGN)
    {
        LREP("Support crossing physical block boundaries reading is allowed\r\n");
    }
    if (csd->flags & SDCARD_CSD_DSR_IMP)
    {
        LREP("DSR is implemented\r\n");
    }

    // show device size
    LREP("c_size: %d\r\n", (int)csd->cSize);

    // Show read, write Max, Min current ratings in case CSD structure indicated 'Standard Capacity' card.
    if (csd->csdStructure == 0)
    {
        LREP("VDD_R_CURR_MIN: 0x%X\r\n", csd->vddRCurrMin);
        LREP("VDD_R_CURR_MAX: 0x%X\r\n", csd->vddRCurrMax);
        LREP("VDD_W_CURR_MIN: 0x%X\r\n", csd->vddWCurrMin);
        LREP("VDD_W_CURR_MAX: 0x%X\r\n", csd->vddWCurrMax);
        LREP("c_size_mult: %d\r\n", csd->cSizeMult);
    }

    // parse CSD flag and display if 'Erase single block' is enabled
    if (csd->flags & SDCARD_CSD_ERASE_BLK_ENABLED)
    {
        LREP("Erase unit size is one or multiple units of 512 bytes\r\n");
    }
    else
    {
        LREP("Erase unit size is one or multiple units of %d blocks\r\n", csd->sectorSize + 2);
    }

    // parse and show write protected group size
    LREP("The size of write protected group is %d blocks\r\n", csd->wpGrpSize + 2);

    // parse and show if 'write protection group' is enabled
    if (csd->flags & SDCARD_CSD_WP_GRP_ENABLED)
    {
        LREP("Write protection group is defined\r\n");
    }

    // display Write speed factor
    LREP("R2W_Factor: %d\r\n", csd->r2wFactor);

    // display Max. write data block length
    LREP("max write block length: %d\r\n", csd->writeBlkLen);

    // parse CSD flags and display whether 'Partial blocks for write' is allowed
    if (csd->flags & SDCARD_CSD_WRITE_BL_PARTIAL)
    {
        LREP("Smaller blocks can be used to write\r\n");
    }

    // parse CSD flags and display whether card content is a COPY (not original)
    if (csd->flags & SDCARD_CSD_COPY)
    {
        LREP("The content is copied\r\n");
    }
    // parse CSD flags and display if the card content is write protected permanently
    if (csd->flags & SDCARD_CSD_PERM_WRITE_PROTECT)
    {
        LREP("The content is write protected permanently\r\n");
    }

    // parse CSD flags and display if the card content is write protected temporarily
    if (csd->flags & SDCARD_CSD_TMP_WRITE_PROTECT)
    {
        LREP("The content is write protected temporarily\r\n");
    }

    // parse CSD file format and display
    if (!(csd->flags & SDCARD_CSD_FILE_FORMAT_GROUP))
    {
        switch(csd->fileFormat)
        {
            case 0:
                LREP("Hard disk-like file system with partition table\r\n");
                break;
            case 1:
                LREP("DOS FAT (floppy-like) with boot sector only\r\n");
                break;
            case 2:
                LREP("Universla file format\r\n");
                break;
            case 3:
                LREP("Others/Unknown\r\n");
                break;
            default:
                break;
        }
    }
}

/*****************************************************************************/
/** @brief
 *
 *
 *  @param
 *  @return Void.
 *  @note
 */
static void show_card_scr(sdcard_scr_t *scr)
{
    // print SCR structure value
    LREP("SCR Structure: 0x%X\r\n", scr->scrStructure);

    // print SD specs number
    LREP("SD Spec: 0x%X\r\n", scr->sdSpec);
    if (scr->sdSpec)
    {
        if (scr->flags & SDCARD_SCR_SD_SPEC3)
        {
            LREP("SD Spec 3.0\r\n");
        }
        else
        {
            LREP("SD Spec 2.0\r\n");
        }
    }

    // parse and show 'Data status after erases'
    if (scr->flags & SDCARD_SCR_DATA_STAT_AFTER_ERASE)
    {
        LREP("Data status after erase is Set\r\n");
    }

    // parse and show SD security and version
    switch(scr->sdSecurity)
    {
        case 0:
            LREP("No security\r\n");
            break;
        case 1:
            LREP("Not used\r\n");
            break;
        case 2:
            LREP("SDSC Card(Security Version 1.01)\r\n");
            break;
        case 3:
            LREP("SDHC Card(Security Version 2.00)\r\n");
            break;
        case 4:
            LREP("SDXC Card(Security Version 3.xx)\r\n");
            break;
        default:
            break;
    }

    // parse and show SD card supported bus width
    if (scr->sdBusWidths & 0x1)
    {
        LREP("Card supports 1-bit bus width\r\n");
    }
    if (scr->sdBusWidths & 0x4)
    {
        LREP("Card supports 4-bit bus width\r\n");
    }

    // parse and show whether Extended security is supported
    if (scr->exSecurity)
    {
        LREP("Extended security is supported\r\n");
    }

    // parse and display Command support bits
    if (scr->cmdSupport & 0x1)
    {
        LREP("Support set block count command\r\n");
    }
    if (scr->cmdSupport & 0x2)
    {
        LREP("Support speed class control\r\n");
    }
}

/*****************************************************************************/
/** @brief
 *
 *
 *  @param
 *  @return Void.
 *  @note
 */
static void show_card_info(sdhc_card_t *card, bool showDetail)
{
    double temp;

    LREP("\r\n------- Card Information -------\r\n");

    // show card type
    LREP("Card Type: ");
    switch(card->cardType)
    {
        case kCardTypeMmc:
            LREP("MMC");
            break;
        case kCardTypeSd:
            if (card->caps & SDMMC_CARD_CAPS_SDHC)
            {
                LREP("SDHC");
            }
            else if (card->caps & SDMMC_CARD_CAPS_SDXC)
            {
                LREP("SDXC");
            }
            else
            {
                LREP("SDSC");
            }
            break;
        case kCardTypeSdio:
            LREP("SDIO");
            break;
        default:
            LREP("Unknown");
            break;
    }
    LREP("\r\n");

    // calculate and show capacity of the card
    LREP("\r\nCard Capacity: ");
    temp = 1000000000 / card->blockSize;
    temp = ((float)card->blockCount / (float)temp);
    if (temp > 1.0)
    {
        LREP("%.02f GB\r\n", temp);
    }
    else
    {
        LREP("%.02f MB\r\n", temp * 1000);
    }

    // show Max. clock speed supported by the host
    LREP("Host Clock Max Rate: %d MHz\r\n", (int)(card->host->maxClock / 1000000));

    // show current clock speed of operation
    LREP("Clock Rate: %d MHz\r\n", (int)(card->host->clock / 1000000));

    // show Card Identification (CID)
    show_card_cid(&(card->cid));

    // check if it's a SD card and if Yes, show CSD and SCR specific values
    if (IS_SD_CARD(card) && showDetail)
    {
        show_card_csd(&(card->csd));
        show_card_scr(&(card->scr));
    }
}

void sdhc_card_detection(void)
{
	OS_ERR err;
	cardInserted = BOARD_IsSDCardDetected();
    OSTaskSemPost(&TCB_task_filesystem, 0, &err);
}


/*****************************************************************************/
/** @brief
 *
 *
 *  @param
 *  @return Void.
 *  @note
 */
static test_result_t demo_card_data_access(void)
{
    sdhc_card_t card = {0};
    sdhc_host_t host = {0};
    sdhc_user_config_t config = {0};
    uint32_t i, status;
    uint8_t proceed;

    // initialize user sdhc configuration structure
    config.transMode = kSdhcTransModeAdma2;
    config.clock = SDMMC_CLK_100KHZ;
    config.cdType = kSdhcCardDetectGpio;

    LREP("This demo is going to access data on card\r\n");

    // initialize the SDHC driver with the user configuration
    if (SDHC_DRV_Init(BOARD_SDHC_INSTANCE, &host, &config) != kStatus_SDHC_NoError)
    {
        return kTestResultFailed;
    }

    // wait for a card detection
    sdhc_card_detection();

    if (cardInserted) {
        LREP("A card is detected\r\n");
    } else {
        LREP("A card is removed\r\n");
        return kTestResultFailed;
    }

    // A card is detected, now initialize the card driver
    if (kStatus_SDHC_NoError != SDCARD_DRV_Init(&host, &card))
    {
        LREP("SDCARD_DRV_Init failed\r\n");
        SDHC_DRV_Shutdown(card.hostInstance);
        return kTestResultFailed;
    }

    LREP("sdcard initialized\r\n");

    // show card information
    show_card_info(&card, true);

    // Check if the card is read only or not
    // It checks the write protection information from the card, not from on-board WP signal.
    // High Capacity SD cards (More than 2GB and up to and including 32GB) doesn't support write protection.
    // Standard Capacity (Up to and including 2 GBytes) SD cards may optionally support write protection.
    if (SDCARD_DRV_CheckReadOnly(&card))
    {
        LREP("Card is write-protected, skip writing tests\r\n");

        // clear buffer refData before reading data from card
        memset(refData, 0, sizeof(refData));

        // read 1 block of data from block no '2'
        if (kStatus_SDHC_NoError != SDCARD_DRV_ReadBlocks(&card, testData, 2, 1))
        {
            LREP("ERROR: SDCARD_DRV_ReadBlocks failed, line %d\r\n", __LINE__);
            SDCARD_DRV_Shutdown(&card);
            SDHC_DRV_Shutdown(card.hostInstance);
            return kTestResultFailed;
        }
        LREP("Single block read test passed!\r\n");

        // clear buffer refData2 before reading data from card
        memset(refData2, 0, sizeof(refData2));

        // read multiple blocks of data starting from TEST_START_BLOCK
        if (kStatus_SDHC_NoError != SDCARD_DRV_ReadBlocks(&card, testData2, TEST_START_BLOCK, sizeof(refData2)/FSL_SDHC_CARD_DEFAULT_BLOCK_SIZE))
        {
            LREP("ERROR: SDCARD_DRV_ReadBlocks failed, line %d\r\n", __LINE__);
            SDCARD_DRV_Shutdown(&card);
            SDHC_DRV_Shutdown(card.hostInstance);
            return kTestResultFailed;
        }
        LREP("multiple block read test passed!\r\n");
        SDCARD_DRV_Shutdown(&card);
        SDHC_DRV_Shutdown(card.hostInstance);
        return kTestResultFailed;
    }

    // Prompt user to take data backup
    LREP("\r\n");
    LREP("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! CAUTION  !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\r\n");
    LREP("THIS DEMO IS GOING TO ERASE AND WRITE RAW DATA TO THE CARD,\r\n");
    LREP("MAKE SURE YOU TAKE BACKUP OF ANY VALUEABLE DATA PRESENT IN THE CARD\r\n");
    LREP("BEFORE PROCEEDING.\r\n");
    LREP("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\r\n");
    LREP("\r\n");

    LREP("Do you want to proceed? (Enter 'y' IF yes) :");
//    proceed = GETCHAR();
//    PUTCHAR(proceed);
    LREP("\r\n");
    if(proceed != 'y')
    {
        // the demo is aborted
        return kTestResultAborted;
    }

    // Erase a group of 20 blocks
    if (kStatus_SDHC_NoError != SDCARD_DRV_EraseBlocks(&card, 0, 20))
    {
        LREP("ERROR:SDCARD_DRV_EraseBlocks failed, line %d\r\n", __LINE__);
        SDCARD_DRV_Shutdown(&card);
        SDHC_DRV_Shutdown(card.hostInstance);
        return kTestResultFailed;
    }

    LREP("\r\nStart write/read/compare demo...\r\n");

    // clear refData with '0's
    memset(refData, 0, sizeof(refData));

    // fill refData buffer with reference data with a seed value '0x11'
    if (fill_reference_data(refData, 0x11, sizeof(refData))) {
        LREP("ERROR: prepare reference data failed\r\n");
        SDCARD_DRV_Shutdown(&card);
        SDHC_DRV_Shutdown(card.hostInstance);
        return kTestResultInitFailed;
    }

    // write 1 block of refData buffer data to card block no '2'
    if (kStatus_SDHC_NoError != SDCARD_DRV_WriteBlocks(&card, refData, 2, 1))
    {
        LREP("ERROR: SDCARD_DRV_WriteBlocks failed, line %d\r\n", __LINE__);
        SDCARD_DRV_Shutdown(&card);
        SDHC_DRV_Shutdown(card.hostInstance);
        return kTestResultFailed;
    }

    // read back 1 block of data from card block no '2' to buffer testData
    if (kStatus_SDHC_NoError != SDCARD_DRV_ReadBlocks(&card, testData, 2, 1))
    {
        LREP("ERROR: SDCARD_DRV_ReadBlocks failed, line %d\r\n", __LINE__);
        SDCARD_DRV_Shutdown(&card);
        SDHC_DRV_Shutdown(card.hostInstance);
        return kTestResultFailed;
    }

    // compare values match between buffers testData and refData
    for (i = 0; i < sizeof(testData); i++)
    {
        if (testData[i] != refData[i])
        {
            LREP("ERROR: comparison failed, line %d\r\n", __LINE__);
            SDCARD_DRV_Shutdown(&card);
            SDHC_DRV_Shutdown(card.hostInstance);
            return kTestResultFailed;
        }
    }
    LREP("Single block write/read/compare demo passed!\r\n");

    // clear refData2 with '0's
    memset(refData2, 0, sizeof(refData2));

    // fill refData2 buffer with reference data with different seed values
    for (i = 0; i < sizeof(refData2)/FSL_SDHC_CARD_DEFAULT_BLOCK_SIZE; i++)
    {
        if (fill_reference_data(&refData2[i * FSL_SDHC_CARD_DEFAULT_BLOCK_SIZE], 0x44 + i, FSL_SDHC_CARD_DEFAULT_BLOCK_SIZE))
        {
            LREP("ERROR: fill data failed, line %d\r\n", __LINE__);
            SDCARD_DRV_Shutdown(&card);
            SDHC_DRV_Shutdown(card.hostInstance);
            return kTestResultFailed;
        }
    }

    // write multiple blocks from refData buffer data to card block no TEST_START_BLOCK
    if (kStatus_SDHC_NoError != SDCARD_DRV_WriteBlocks(&card, refData2, TEST_START_BLOCK, sizeof(refData2)/FSL_SDHC_CARD_DEFAULT_BLOCK_SIZE))
    {
        LREP("ERROR: SDCARD_DRV_WriteBlocks failed, line %d\r\n", __LINE__);
        SDCARD_DRV_Shutdown(&card);
        SDHC_DRV_Shutdown(card.hostInstance);
        return kTestResultFailed;
    }

    // read back multiple blocks of data from card block no TEST_START_BLOCK to buffer testData2
    if (kStatus_SDHC_NoError != SDCARD_DRV_ReadBlocks(&card, testData2, TEST_START_BLOCK, sizeof(refData2)/FSL_SDHC_CARD_DEFAULT_BLOCK_SIZE))
    {
        LREP("ERROR: SDCARD_DRV_ReadBlocks failed, line %d\r\n", __LINE__);
        SDCARD_DRV_Shutdown(&card);
        SDHC_DRV_Shutdown(card.hostInstance);
        return kTestResultFailed;
    }

    // compare values match between buffers testData2 and refData2
    for (i = 0; i < sizeof(refData2); i++)
    {
        if (testData2[i] != refData2[i])
        {
            LREP("ERROR: data comparison failed, line %d\r\n", __LINE__);
            SDCARD_DRV_Shutdown(&card);
            SDHC_DRV_Shutdown(card.hostInstance);
            return kTestResultFailed;
        }
    }
    LREP("Multi-block write/read/compare demo passed!\r\n");

    // Erase 1 block at TEST_START_BLOCK
    if (kStatus_SDHC_NoError != SDCARD_DRV_EraseBlocks(&card, TEST_START_BLOCK, 1))
    {
        LREP("ERROR: SDCARD_DRV_EraseBlocks failed, line %d\r\n", __LINE__);
        SDCARD_DRV_Shutdown(&card);
        SDHC_DRV_Shutdown(card.hostInstance);
        return kTestResultFailed;
    }
    LREP("Erase blocks demo passed!\r\n");

    // shut down SD Card driver instance
    SDCARD_DRV_Shutdown(&card);

    // shut down SDHC host driver instance
    SDHC_DRV_Shutdown(card.hostInstance);

    return kTestResultPassed;
}
