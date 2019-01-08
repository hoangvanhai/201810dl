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

bool check_obj_existed(const char *path);
bool check_extension(const char *path, const char *exten);
bool obj_stat(const char* path);
int	show_content(char *path);
int scan_files(char *path);
int current_directory();
int remove_directory(char *path);
int delete_node (TCHAR* path, UINT sz_buff, FILINFO* fno);
int cat(char *path);
int show_content_recursive(char *path);

#endif /* FILESYSTEM_TASK_FILESYSTEM_H_ */
