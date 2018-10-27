
#ifndef TRANSDEFS_H
#define	TRANSDEFS_H

#ifdef	__cplusplus
extern "C" {
#endif

/***************************** Include Files *********************************/

#include "TransDef.h"
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



#define DATA_FRM                    (0x20)
#define ACK_REQ                     (0x80)
#define CLR_ACK_FLAG                (0x7f)
/*UI Data Frame*/

#define MASTER_RESET_FOR_BOOTLOADER (0x00)

/******************* PC-RRC-Radio frame type *******************/
#define DATA_UI_CTRL                (0x00 | DATA_FRM)
#define DATA_RUI_CTRL               DATA_UI_CTRL
#define DATA_PC                     (0x03 | DATA_FRM)




/******************* RRC Internal frame type *******************/

/*PTT in VOICE MODE*/
#define DATA_RC_CTRL                (0x01 | DATA_FRM)

#define DATA_ONOFF_VOICE            (0x04 | DATA_FRM)   //PTT
/* PTT in CW MODE*/
#define DATA_CW_PACKET              (0x05 | DATA_FRM)
/* Check connection RUI-ADAPTER is properly working*/
#define DATA_CONN_CHECK             (0x10 | DATA_FRM)
/*Voice Frame*/
#define DATA_VOICE                  (0x08 | DATA_FRM)
/* Change Stage of system */
#define DATA_SYSTEM_STAGE_CONTROL   (0x09 | DATA_FRM)
/*Configure Transceiver Frame*/
#define DATA_TRANCEIVER_CONFIG      (0x0A | DATA_FRM)

#define DATA_BER_CHECK              (0x0B | DATA_FRM)

/*---------------------------------------------------------------------------*/
// Address of nodes
/*---------------------------------------------------------------------------*/
/* RRC-UI SIDE */

#define CHARGER_MY_ID             0x02
#define RRC_UI_UI_DEST_ID           0x01

/**************************** Type Definitions *******************************/

/***************** Macros (Inline Functions) Definitions *********************/


/************************** Function Prototypes ******************************/

/************************** Variable Definitions *****************************/

/*****************************************************************************/


#ifdef	__cplusplus
}
#endif

#endif	/* TRANSDEFS_H */

