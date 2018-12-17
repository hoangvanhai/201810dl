
#ifndef TRANSDEFS_H
#define	TRANSDEFS_H

#ifdef	__cplusplus
extern "C" {
#endif

/***************************** Include Files *********************************/
#include <stdint.h>
/************************** Constant Definitions *****************************/
/* Frame format
   Idx:     0         1     2       3      4     5       6 	7       8                  8+DLEN
        ----------------------------------------------------------------------------------------
        | PREAM | SRCADR | DSTADR | SEQ | CTRL | DLEN | CRCD | CRCH | DATA[0] ... DATA[DLEN-1] |
        ----------------------------------------------------------------------------------------
 * CTRL:
 *  Bit 7: ACK - REQ
 *  Bit 6: not Use
 *  Bit 5: Type: DATA or CMD frame
 *  Bit 4-0:Sub type
*/
// Frame structure and macros
#define	CST_PREAMBLE		'!'

// Byte Index
#define IDX_SFRM_PREAM		0
#define IDX_SFRM_SRCADR		1
#define IDX_SFRM_DSTADR		2
#define IDX_SFRM_SEQNUM		3
#define IDX_SFRM_CTRL		4

#define IDX_SFRM_DLEN		5
#define IDX_SFRM_CRCD		6
#define IDX_SFRM_CRCH		7
#define IDX_SFRM_DATA0		8

#define SFRM_MAX_DLEN			(240)   //Max. Val = 256
#define SFRM_HDR_SIZE			8
#define SFRM_MAX_SIZE			(SFRM_MAX_DLEN + SFRM_HDR_SIZE)
#define SFRAME_GET_DLEN(frm)    ((frm)[IDX_SFRM_DLEN])
#define SFRAME_SIZE(frm)		(SFRAME_GET_DLEN(frm) + SFRM_HDR_SIZE)

#define SFRAME_SET_DLEN(frm,val) ((frm)[IDX_SFRM_DLEN] = val)

#define SFRAME_GET_SRCADR(frm)   ((frm)[IDX_SFRM_SRCADR])
#define SFRAME_GET_DSTADR(frm)   ((frm)[IDX_SFRM_DSTADR])
#define SFRAME_GET_SEQ(frm)      ((frm)[IDX_SFRM_SEQNUM])
#define SFRAME_GET_CTRL(frm)     ((frm)[IDX_SFRM_CTRL])
#define SFRAME_GET_CRCD(frm)     ((frm)[IDX_SFRM_CRCD])
#define SFRAME_GET_CRCH(frm)     ((frm)[IDX_SFRM_CRCH])

#define SFRAME_GET_DATAPTR(frm)  (&(frm)[IDX_FRM_DATA0])

#define SFRAME_SET_SRCADR(frm,val)   ((frm)[IDX_SFRM_SRCADR] = val)
#define SFRAME_SET_DSTADR(frm,val)   ((frm)[IDX_SFRM_DSTADR] = val)
#define SFRAME_SET_SEQ(frm,val)      ((frm)[IDX_SFRM_SEQNUM] = val)
#define SFRAME_SET_CTRL(frm,val)     ((frm)[IDX_SFRM_CTRL]   = val)
#define SFRAME_SET_CRCD(frm,val)     ((frm)[IDX_SFRM_CRCD]   = val)
#define SFRAME_SET_CRCH(frm,val)     ((frm)[IDX_SFRM_CRCH]   = val)



#define FORM_CTRL(ACKReq,FrmTpy) (((ACKReq) ? 0x80 : 0x00) | (FrmTpy & 0x1F))
#define IS_ACK_REQ(ctrl)		((ctrl) & 0x80 ? TRUE : FALSE)
#define IS_CRCD_REQ(ctrl)		((ctrl) & 0x40 ? TRUE : FALSE)
#define GET_FRM_TYPE(ctrl)		((ctrl) & 0x1F)
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

#define DATA_FRM                    (0x20)
#define ACK_REQ                     (0x80)
#define CLR_ACK_FLAG                (0x7f)
/*UI Data Frame*/

#define MASTER_RESET_FOR_BOOTLOADER (0x00)
/******************* RRC Internal frame type *******************/

/*PTT in VOICE MODE*/
#define DATA_CTRL                (0x00 | DATA_FRM)


/*---------------------------------------------------------------------------*/
// Address of nodes
/*---------------------------------------------------------------------------*/


#define LARGE_FRAME_SIZE				(SFRM_MAX_SIZE)
#define LARGE_MEM_SIZE					((sizeof(SMem)) + (sizeof(SFrameInfo)) + LARGE_FRAME_SIZE)
#define CALCULATE_MEM_SIZE(DataSize)	((sizeof(SMem)) + (sizeof(SFrameInfo)) + (DataSize))

/**************************** Type Definitions *******************************/

typedef enum
{
    E_CMD_FRM   =   0,      //(CMD_ACK or CMD_NACK)
    E_DATA_FRM  =   1       //(DATA_FRM)
}EPackageType;

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


typedef enum
{
    /*General return values*/
	TRANS_SUCCESS 			= 	0,
	TRANS_FAILURE				=	1,

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


typedef struct _SFrameInfo
{
    /*Time out, in mili-second*/
    uint16_t		i16TimeOut;

    /*Number of send*/
    uint8_t			u8NumSend;

    /*Control*/
    uint8_t 		u8Ctrl;

    /*Sequence*/
    uint8_t			u8Seq;

    /*data length*/
    uint16_t 		u16DLen;

    /*Pointer to data field of IBC frame*/
    uint8_t 		*pu8Data;

    /*Pointer to frame*/
    uint8_t	 		*pFrame;

} SFrameInfo;




/************************** Constant Definitions *****************************/
#define TRANS_UPDATE_TIMER_RATE_MS   (1)    //20ms

#define TRANS_TIMEOUT_TICK           (50L)	//in mili-ten second
#define TRANS_MAX_NUM_SEND           (3)

#define TRANS_MAX_DLEN               (240)



/**************************** Type Definitions *******************************/
typedef union _S_TRANS_FLAG
{
    uint8_t	u8All;
    struct
    {
        unsigned bStarted                    :	1;
        unsigned bUpdateWaitingACKFrameState :	1;
        unsigned b2                          :	1;
        unsigned b3                          :	1;
        unsigned b4                          :	1;
        unsigned b5                          :	1;
        unsigned b6                          :	1;
        unsigned b7                          :	1;
    }Bits;

}STransFlag;

typedef enum E_TRANS_EVENT_
{
    TRANS_EVT_NONE           = 0,
    TRANS_EVT_RECV_DATA      = 1,	//Data indicate
    TRANS_EVT_SENT_DATA      = 2,	//Data confirm
    TRANS_EVT_ERROR          = 3

}EL3Event;

typedef void (*FClbL3Event)(void *pData, uint8_t u8Type);



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

/*****************************************************************************/


#ifdef	__cplusplus
}
#endif

#endif	/* TRANSDEFS_H */

