/*HEADER**********************************************************************
*
* Copyright 2011 Freescale Semiconductor, Inc.
* Copyright 1989-2008 ARC International
*
* This software is owned or controlled by Freescale Semiconductor.
* Use of this software is governed by the Freescale MQX RTOS License
* distributed with this Material.
* See the MQX_RTOS_LICENSE file distributed for more details.
*
* Brief License Summary:
* This software is provided in source form for you to use free of charge,
* but it is not open source software. You are allowed to use this software
* but you cannot redistribute it or derivative works of it in source form.
* The software may be used only in connection with a product containing
* a Freescale microprocessor, microcontroller, or digital signal processor.
* See license agreement file for full license terms including other
* restrictions.
*****************************************************************************
*
* Comments:
Provide MFS file system on external RAM
*
*   
*
*
*END************************************************************************/

#include <stdint.h>
#include <string.h>
#include <mqx.h>
#include <bsp.h>
#include <nio.h>
#include <nio_mem.h>
#include <stdio.h>
#include <mfs.h>
#include <part_mgr.h>
#include <shell.h>
#include <fcntl.h>
#include <fs_supp.h>

#include <mfs_usb.h>

void Shell_Task(uint32_t);
void Ram_disk_start(void);

const SHELL_COMMAND_STRUCT Shell_commands[] = {   
   { "cd",        Shell_cd },      
   { "copy",      Shell_copy },
   { "create",    Shell_create },
   { "del",       Shell_del },       
   { "disect",    Shell_disect},      
   { "dir",       Shell_dir },      
   { "df",        Shell_df },      
   { "exit",      Shell_exit }, 
   { "format",    Shell_format },
   { "help",      Shell_help }, 
   { "mkdir",     Shell_mkdir },     
   { "pwd",       Shell_pwd },       
   { "read",      Shell_read },      
   { "ren",       Shell_rename },    
   { "rmdir",     Shell_rmdir },
   { "sh",        Shell_sh },
   { "type",      Shell_type },
   { "write",     Shell_write },     
   { "?",         Shell_command_list },     
   
   { NULL,        NULL } 
};

/*TASK*-----------------------------------------------------------------
*
* Function Name  : Shell_Task
* Returned Value : void
* Comments       :
*
*END------------------------------------------------------------------*/
void Shell_Task(uint32_t temp)
{ 
   (void)temp; /* suppress 'unused variable' warning */

   /* Run the shell on the serial port */
   printf("Demo start\n\r");
   for(;;)  {
      Shell(Shell_commands, NULL);
      printf("Shell exited, restarting...\n");
   }
}

/* EOF */
