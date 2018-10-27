#ifndef TRANS_DEF_H_
#define TRANS_DEF_H_


/** @FILE NAME: TransDef.h
 *  @BRIEF:     This file for ...
 *
 *  Copyright (c) 2013 ViettelR&D.
 *  All Rights Reserved This program is the confidential and proprietary
 *  product of ViettelR&D. Any Unauthorized use, reproduction or transfer
 *  of this program is strictly prohibited.
 *
 *  @Author: kienpt12
 *  @NOTE:   No Note at the moment
 *  @BUG:    No known bugs.
 *
 *<pre>
 *  MODIFICATION HISTORY:
 *
 *  Ver   Who  	    Date          Changes
 *  ----- --------- ------------- -----------------------------------------------
 *  1.00  kienpt12  Jun 14, 2013  First release
 * 
 * 
 *</pre>
 ******************************************************************************/
#include "typedefs.h"
#include "mem.h"
#include "queue.h"
//#include "app_cfg.h"
/***************************** Include Files *********************************/

#define  CRCD_ALWAYS_CALCULATE     1
///* Frame format
// * Ver 1 - Short format (Maximum value of Data field is 256 bytes)
//   Idx:     0         1     2       3      4     5       6 	7       8                  8+DLEN
//        ----------------------------------------------------------------------------------------
//        | PREAM | SRCADR | DSTADR | SEQ | CTRL | DLEN | CRCD | CRCH | DATA[0] ... DATA[DLEN-1] |
//        ----------------------------------------------------------------------------------------
// *
/************************** Constant Definitions *****************************/
/* Frame format
 * Long format (Maximum value of Data field is 1024 bytes)
 * Idx:  0         1        2       3      4       5        6       7      8      9            9+DLEN
        -------------------------------------------------------------------------------------------------
        | PREAM | SRCADR | DSTADR | SEQ | CTRL | DLEN_H | DLEN_L | CRCD | CRCH | DATA[0] ...DATA[DLEN-1]|
        -------------------------------------------------------------------------------------------------
 * CTRL:
 *  Bit 7: ACK - REQ
 *  Bit 6: Not use
 *  Bit 5: Type: DATA or CMD frame
 *  Bit 4-0:Sub type
*/


#define FRM_MAX_DLEN		(240)

#define FRM_HDR_SIZE		8
#define FRM_MAX_SIZE		(FRM_MAX_DLEN + FRM_HDR_SIZE)



#define FORM_CTRL(ACKReq,FrmTpy) (((ACKReq) ? 0x80 : 0x00) | (FrmTpy & 0x1F))
#define IS_ACK_REQ(ctrl)	((ctrl) & 0x80 ? TRUE : FALSE)
#define IS_CRCD_REQ(ctrl)	((ctrl) & 0x40 ? TRUE : FALSE)
#define GET_FRM_TYPE(ctrl)	((ctrl) & 0x1F)
#define CLEAR_ACK_REQ_BIT(ctrl) ((ctrl) & 0x7F)
#define GET_CTRL(ctrl)          ((ctrl) & 0x3F)

#define SET_CRCD_REQ_BIT(ctrl)  ((ctrl) |= 0x40)
#define CLR_CRCD_REQ_BIT(ctrl)  ((ctrl) &= 0xBF)

#define MASK_DATA           0x20
#define CMD_NONE            0x00
#define	CMD_ACK             0x01
#define CMD_NACK            0x02
#define CMD_SLV_SEND_EN		0x03	//only for IBC
#define CMD_NOTDONE         0x04
#define CMD_ERROR           0x05

#define FRM_DATA            0x20
#define FRM_CMD             0x00

#define IS_DATA_FRM(fk)		((fk & MASK_DATA) ? TRUE : FALSE)
#define IS_CMD_FRM(fk)		((fk & MASK_DATA) ? FALSE : TRUE)

#define ADDR_BROADCAST		0xFF

#define MAX_NUM_SEND		(4)

//#define TRANS_TIMEOUT_TICK	(100L)	//in mili-ten second

/*---------------------------------------------------------------------------*/
/*Memory Pool for Trans Module*/

#define TOTAL_NUMBER_OF_LARGE_MEMS      (10)
#define TOTAL_NUMBER_OF_SMALL_MEMS		(10)



/*NOTE MEM_SIZE must be a event number*/
#define LARGE_FRAME_SIZE				(FRM_MAX_SIZE)

#define LARGE_MEM_SIZE					((sizeof(SFrameInfo)) + LARGE_FRAME_SIZE)

#define SMALL_FRAME_SIZE				(60)//(32)
#define SMALL_MEM_SIZE					((sizeof(SFrameInfo)) + SMALL_FRAME_SIZE)

#define TOTAL_NUMBER_OF_MEMS			(TOTAL_NUMBER_OF_LARGE_MEMS + TOTAL_NUMBER_OF_SMALL_MEMS)

#define CALCULATE_MEM_SIZE(DataSize)	((sizeof(SFrameInfo)) + (DataSize))
/************************** Constant Definitions *****************************/

/**************************** Type Definitions *******************************/
typedef enum
{
    /*General return values*/
    SUCCESS 			= 	0,
    FAILURE				=	1,

    TRANS_ERR_MEM		=  	2,

    /*return value and error code for TransL1 & TransL2*/
    TRANS_ERR_BUSY		=	10,
    TRANS_ERR_NOT_STARTED 	= 	11,
    TRANS_ERR_L1_TIMEOUT	=	12,
    TRANS_ERR_L2_TIMEOUT	=	13,
    TRANS_ERR_INVALID_PTR	=	14,
    TRANS_ERR_INVALID_DATA 	= 	15,
    TRANS_ERR_CRCH		=	16,
    TRANS_ERR_CRCD		=	17,
    TRANS_ERR_PARAM		=	18,
    TRANS_ERR_REMOTE		= 	19,
    TRANS_ERR_REMOTE_DLEN	= 	20,
    TRANS_ERR_REMOTE_CRCH	= 	21,
    TRANS_ERR_REMOTE_CRCD	= 	22,
                
    TRANS_ERR_UNKNOWN		=	30

}ETransReturn, ETransErrorCode, ETransStatus;


/**
 * @brief Globally used frame information structure
 *
 * @ingroup Trans
 */
typedef struct _SFrameInfo
{
    /*Time out, in mili-second*/
    uint16_t			i16TimeOut;

    /*Number of send*/
    uint8_t			u8NumSend;

    /*Control*/
    uint8_t 			u8Ctrl;

    /*Sequence*/
    uint8_t			u8Seq;
	
    /*data length*/
    uint16_t 			u16DLen;

    /*Pointer to data field of IBC frame*/
    uint8_t 			*pu8Data;

    /*Pointer to frame*/
    uint8_t	 		*pFrame;

} SFrameInfo;

/*---------------------------------------------------------------------------*/
typedef enum
{
    TRANSL2_ACT_NO        =   0,

    TRANSL2_RCV_ACK       =   1,  //receive ACK frame from master
    TRANSL2_RCV_NACK      =   2,  //receive ACK frame from master

    TRANSL2_REQ_ACK       =   3,  //recieve data frame requireing ACK
    TRANSL2_REQ_NACK      =   4   //receive a wrong frame

}EL2RecvAction;

typedef enum E_TRANSL2_EVENT_
{
    TRANSL2_EVT_NONE        = 0,
    TRANSL2_EVT_SEND_DONE   = 1,	//Data indicate
    TRANSL2_EVT_RECV_DATA   = 2,	//Data confirm
    TRANSL2_EVT_ERROR       = 3

}EL2Event;

typedef void (*FClbL2Event)(EL2Event eEventCode, ETransStatus eStatus, SMem *pMem, void *pClbParam);

/***************** Macros (Inline Functions) Definitions *********************/

#define Frame_SetInfor(pFrame, eMsgType, i16TimeOut, u8NumSend, u8Ctrl, u8Seq, u16DLen) \
{\
	pFrame->eMsgType 	= eMsgType;	\
	pFrame->i16TimeOut 	= i16TimeOut;	\
	pFrame->u8NumSend	= u8NumSend;	\
	pFrame->u8Ctrl		= u8Ctrl;	\
	pFrame->u8Seq		= u8Seq;	\
	pFrame->u16DLen		= u16DLen;	\
}	

/************************** Function Prototypes ******************************/

/************************** Variable Definitions *****************************/







#endif /* TRANS_DEF_H_ */
