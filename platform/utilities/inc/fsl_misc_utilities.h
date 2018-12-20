/*
 * Copyright (c) 2013 - 2014, Freescale Semiconductor, Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * o Redistributions of source code must retain the above copyright notice, this list
 *   of conditions and the following disclaimer.
 *
 * o Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * o Neither the name of Freescale Semiconductor, Inc. nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#ifndef __FSL_MISC_UTILITIES_H__
#define __FSL_MISC_UTILITIES_H__

#include <stdint.h>

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*! @brief Min/max macros */
#if !defined(MIN)
    #define MIN(a, b) ((a) < (b) ? (a) : (b))
#endif

#if !defined(MAX)
    #define MAX(a, b) ((a) > (b) ? (a) : (b))
#endif

/*! @brief Computes the number of elements in an array.*/
#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))

/*! @brief Byte swap macros */
#define BSWAP_16(x)   (uint16_t)((((x) & 0xFF00) >> 0x8) | (((x) & 0xFF) << 0x8))
#define BSWAP_32(val) (uint32_t)((BSWAP_16((uint32_t)(val) & (uint32_t)0xFFFF) << 0x10) |  \
                                 (BSWAP_16((uint32_t)((val) >> 0x10))))
/* A helper macro used by \ref TOSTRING.	*/
#define _TOSTRING(x) #x
/** Macro to convert a number to a string.	 */
#define TOSTRING(x) _TOSTRING(x)

#define DEC_TO_HEX_1BYTE(dec)  		(((dec / 10) << 4) & 0xF0) | (dec % 10)

#define COLOR_KNRM  "\x1B[0m"
#define COLOR_KRED  "\x1B[31m"
#define COLOR_KGRN  "\x1B[32m"
#define COLOR_KYEL  "\x1B[33m"
#define COLOR_KBLU  "\x1B[34m"
#define COLOR_KMAG  "\x1B[35m"
#define COLOR_KCYN  "\x1B[36m"
#define COLOR_KWHT  "\x1B[37m"

#ifdef DEBUG
	#define ASSERT_NONVOID(con,ret)	{if(!(con))	{LREP("\r\nASSERT in file " __FILE__ " at line "  TOSTRING(__LINE__) "\r\n"); return ret;}	}
	#define ASSERT_VOID(con)        {if(!(con))	{LREP("\r\nASSERT in file " __FILE__ " at line "  TOSTRING(__LINE__) "\r\n"); return;	 }	}
	#define ASSERT(con)				{if(!(con))	{LREP( COLOR_KYEL "\r\nASSERT in file " __FILE__ " at line "  TOSTRING(__LINE__) "\r\n" COLOR_KNRM);			 }	}
	#define ERR_LINE(s, args...) 		LREP(COLOR_KRED "[ERROR]:%s:%d " s "\r\n" COLOR_KNRM, __FILE__, __LINE__, ##args)
	#define ERR(s, args...)				LREP(COLOR_KRED "ERR " s "\r\n" COLOR_KNRM, ##args)
	#define WARN(s, args...)			LREP(COLOR_KYEL "WARN " s "\r\n" COLOR_KNRM, ##args)
	#define WARN_LINE(s, args...)		LREP(COLOR_KYEL "[WARN]:%s:%d " s "\r\n" COLOR_KNRM, __FILE__, __LINE__, ##args)

	#define RS485_DEBUG_TX_EN()		{_LATB8 = 1;}
	#define RS485_DEBUG_RX_EN()		{_LATB8 = 0;}

#else
	#define ASSERT_NONVOID(con,ret)
	#define ASSERT_VOID(con)
	#define ASSERT(con)
	#define LREP(...)
	#define ERR_LINE(...)
	#define ERR(...)
	#define WARN(...)
	#define WARN_LINE(...)
#endif



#ifdef NDEBUG           /* required by ANSI standard */
#define debug_assert(expression)  ((void)0)
#else
//#define debug_assert(expression)  ((expression) ? (void)0 : assert_func (__FILE__, __LINE__, NULL, #expression))

#endif

//#define ASSERT(x)   debug_assert(x)

/*!
 * @brief Print out failure messages.
 *
 * @param file A pointer to assert failure file.
 * @param line Assert failure line number.
 * @param func A pointer to assert failure function name.
 * @param failedExpr A pointer to assert failure expression.
 *
 */
void assert_func(const char *file, int line, const char *func, const char *failedExpr);

#endif /* __FSL_MISC_UTILITIES_H__ */
/*******************************************************************************
 * EOF
 ******************************************************************************/

