/*
 * task_filesystem.h
 *
 *  Created on: Oct 26, 2018
 *      Author: PC
 */

#ifndef FILESYSTEM_TASK_FILESYSTEM_H_
#define FILESYSTEM_TASK_FILESYSTEM_H_

#include <board.h>
#include <includes.h>
#include <fsl_sdhc_card.h>
#include <ff.h>

typedef enum
{
    kTestResultPassed = 0U,
    kTestResultFailed,
    kTestResultInitFailed,
    kTestResultAborted,
} test_result_t;

#define TEST_BLOCK_NUM          4U
#define TEST_START_BLOCK        4U


bool check_file_existed(const char *path);

test_result_t demo_card_data_access(void);

#endif /* FILESYSTEM_TASK_FILESYSTEM_H_ */
