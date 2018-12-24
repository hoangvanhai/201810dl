/*
 *      File name:      debug.h
 *
 *      Created on:     Aug 20, 2018
 *      Author:         manhbt
 *      Brief:          Debug functions ....
 */


#ifndef __MODEM_DEBUG_H__
#define __MODEM_DEBUG_H__

#ifdef __cplusplus
extern "C" {
#endif


#include "stdio.h"
#include "fsl_debug_console.h"

#define MODEM_NAME	"UC15"
#define MODEM_DEBUG_PREFIX "[" MODEM_NAME "]"


#define MODEM_DEBUG_ENABLE 	1
#ifndef KNRM
#define KNRM  "\x1B[0m"
#endif

#ifndef KRED
#define KRED  "\x1B[31m"
#endif

#ifndef KGRN
#define KGRN  "\x1B[32m"
#endif

#ifndef KYEL
#define KYEL  "\x1B[33m"
#endif

#ifndef KBLU
#define KBLU  "\x1B[34m"
#endif

#ifndef KMAG
#define KMAG  "\x1B[35m"
#endif

#ifndef KCYN
#define KCYN  "\x1B[36m"
#endif

#ifndef KWHT
#define KWHT  "\x1B[37m"
#endif

#if MODEM_DEBUG_ENABLE
	#define MODEM_DEBUG_DUMP              	 	debug_dump
	#define MODEM_DEBUG(s, args...)				debug_printf(KGRN MODEM_DEBUG_PREFIX s KNRM "\r\n" , ##args)
	#define MODEM_DEBUG_RAW(s, args...)     	debug_printf(s, ##args)
	#define MODEM_DEBUG_CRITICAL(s, args...)   	debug_printf(KRED MODEM_DEBUG_PREFIX "[CRIT]:%d:%s " s KNRM "\r\n", __FILE__, __LINE__, ##args)
	#define MODEM_DEBUG_ERROR(s, args...)   	debug_printf(KRED MODEM_DEBUG_PREFIX "[ERROR]: " s KNRM "\r\n", ##args)
	#define MODEM_DEBUG_WARNING(s, args...) 	debug_printf(KYEL MODEM_DEBUG_PREFIX "[WARNING] " s KNRM "\r\n", ##args)
	#define MODEM_DEBUG_LINE(s, args...) 		debug_printf(KNRM MODEM_DEBUG_PREFIX"[%s:%d] " s KNRM "\r\n", __FILE__, __LINE__, ##args)

/* Define ASSERT MACROS */
#ifndef ASSERT_NONVOID
	#define ASSERT_NONVOID(con,ret)				{if(!(con))	{MODEM_DEBUG_ERROR("\r\nASSERT in file %s, line %d\r\n", __FILE__, __LINE__); return ret;}	}
#endif

#ifndef ASSERT_VOID
	#define ASSERT_VOID(con)        			{if(!(con))	{MODEM_DEBUG_ERROR("\r\nASSERT in file %s, line %d\r\n", __FILE__, __LINE__); return;	 }	}
#endif

#ifndef ASSERT
	#define ASSERT(con)							{if(!(con))	{MODEM_DEBUG_ERROR("\r\nASSERT in file %s, line %d\r\n", __FILE__, __LINE__);			 }	}
#endif

#else
	#define MODEM_DEBUG(s, args...)
	#define MODEM_DEBUG_RAW(s, args...)
	#define MODEM_DEBUG_CRITICAL(s, args...)
	#define MODEM_DEBUG_ERROR(s, args...)
	#define MODEM_DEBUG_WARNING(s, args...)
	#define MODEM_DEBUG_LINE(s, args...)
	#define MODEM_DEBUG_DUMP
#endif


#ifdef __cplusplus
}
#endif

#endif /* __MODEM_DEBUG_H__ */
