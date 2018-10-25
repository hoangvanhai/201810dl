

#ifndef _GENERIC_H_
#define _GENERIC_H_



/***************************** Include Files *********************************/
#include <stdint.h>
#include <stdbool.h>
/************************** Constant Definitions *****************************/

/**************************** Type Definitions *******************************/


/* Other definitions	*/
#ifndef BYTE
#define BYTE		uint8_t
#define BYTE_MAX (	255)
#endif

#ifndef WORD
#define WORD		uint16_t
#define WORD_MAX 	(65535U)
#endif

#ifndef DWORD
#define DWORD		uint32_t
#define DWORD_MAX 	(4294967295UL)
#endif

#ifndef LPVOID	
#define LPVOID	void*
#endif

#ifndef LPBYTE
#define LPBYTE  BYTE*
#endif

#ifndef LPWORD
#define LPWORD  WORD*
#endif

#ifndef LPDWORD
#define LPDWORD  DWORD*
#endif

typedef uint8_t BOOL;


/*Two types compatible with Texas Instrument's DSP*/
typedef long LDATA;	/* 32 bit "accumulator" (L_*) */
typedef short DATA;	/* 16 bit "register"  (sw*) */

/* A union type for byte or word access for 16 bit values.	*/
typedef union _union16 {
  uint16_t u16;
  uint8_t u8[2];
} union16;

/* A union type for byte, word, or dword access for 32 bit values.	*/
typedef union _union32 {
  uint32_t u32;

  struct {
    uint16_t ls16;
    uint16_t ms16;
  } u16;

  uint8_t u8[4];
} union32;

/* A union type for byte, word, or dword access for 64 bit values.	*/
typedef union _union64 {
  uint32_t u32[2];
  uint16_t u16[4];
  uint8_t u8[8];
} union64;

typedef union _unionfloat32{
	float f32;
	
	uint8_t u8[4];
} unionfloat32;

typedef union _unionfloat64{
	float f64;
	
	uint8_t u8[8];
} unionfloat64;

/*----------------------------------------------------------------------*/

typedef union _UINT8 {

	  uint8_t _uint8;			//uint8_t viewed as an uint8_t

	  uint8_t u8;				//uint8_t viewed as an uint8_t
	  struct {
			unsigned b0:		//bit 0 (the LSb) of the uint8_t
			    1;
			unsigned b1:		//bit 1 of the uint8_t
			    1;
			unsigned b2:		//bit 2 of the uint8_t
			    1;
			unsigned b3:		//bit 3  of the uint8_t
			    1;
			unsigned b4:		//bit 4  of the uint8_t
			    1;
			unsigned b5:		//bit 5  of the uint8_t
			    1;
			unsigned b6:		//bit 6  of the uint8_t
			    1;
			unsigned b7:		//bit 7 (MSb) of the uint8_t
			    1;
	  };
} UINT8;


typedef union _UINT16 {

	  uint16_t _uint16;					//uint16_t viewed as an uint16_t
	
	  uint16_t u16;						//uint16_t viewed as an uint16_t
	  struct {
		    uint8_t u8Lsb;				//LSB (uint8_t) of the uint16_t	
		    uint8_t u8Msb;				//MSB (uint8_t) of the uint16_t
	  };
	  struct {
	    	uint8_t u8Bytes[2];			//The uint16_t viewed as an array of two (2) uint8s
	  };
	  struct {

			unsigned b0:				//bit 0 (the LSb) of the uint16_t
			    1;
			unsigned b1:				//bit 1 of the uint16_t
			    1;
			unsigned b2:				//bit 2 of the uint16_t
			    1;
			unsigned b3:				//bit 3 of the uint16_t
			    1;
			unsigned b4:				//bit 4 of the uint16_t
			    1;
			unsigned b5:				//bit 5 of the uint16_t
			    1;
			unsigned b6:				//bit 6 of the uint16_t
			    1;
			unsigned b7:				//bit 7 of the uint16_t
			    1;
			unsigned b8:				//bit 8 of the uint16_t
			    1;
			unsigned b9:				//bit 9 of the uint16_t
			    1;
			unsigned b10:				//bit 10 of the uint16_t
			    1;
			unsigned b11:				//bit 11 of the uint16_t
			    1;
			unsigned b12:				//bit 12 of the uint16_t
			    1;
			unsigned b13:				//bit 13 of the uint16_t
			    1;
			unsigned b14:				//bit 14 of the uint16_t
			    1;
			unsigned b15:				//bit 15 of the uint16_t
			    1;
	  };
} UINT16;

#define LSB(a)      ((a).u8Bytes[0])

#define MSB(a)      ((a).u8Bytes[1])
/*----------------------------------------------------------------------*/
typedef union _UINT32 {
	
	  uint32_t _uint32;							//uint32_t viewed as an uint32_t
	
	  uint32_t u32;								//uint32_t viewed as an uint32_t
	  struct {
		    uint8_t u8LoLsb;					//The LSB of the least-signficant uint16_t in the 32-bit data
		    uint8_t u8LoMsb;					//The MSB of the least-signficant uint16_t in the 32-bit data  
		    uint8_t u8HiLsb;					//The LSB of the most-signficant uint16_t in the 32-bit data  
		    uint8_t u8HiMsb;					//The MSB of the most-signficant uint16_t in the 32-bit data
	  };
	  struct {
		    uint16_t u16LoWord;					//The least-significant uint16_t in the 32-bit data	
		    uint16_t u16HiWord;					//The most-significant uint16_t in the 32-bit data
	  };
	  struct {
	    	uint16_t u16Words[2];				//The uint32_t viewed as an array of two (2) uint16s
	  };
	  struct {
	    	uint8_t u8Bytes[4];					//The uint32_t viewed as an array of four (4) uint8s
	  };	
	  struct {
			unsigned b0:
			    1;
			unsigned b1:
			    1;
			unsigned b2:
			    1;
			unsigned b3:
			    1;
			unsigned b4:
			    1;
			unsigned b5:
			    1;
			unsigned b6:
			    1;
			unsigned b7:
			    1;
			unsigned b8:
			    1;
			unsigned b9:
			    1;
			unsigned b10:
			    1;
			unsigned b11:
			    1;
			unsigned b12:
			    1;
			unsigned b13:
			    1;
			unsigned b14:
			    1;
			unsigned b15:
			    1;
			unsigned b16:
			    1;
			unsigned b17:
			    1;
			unsigned b18:
			    1;
			unsigned b19:
			    1;
			unsigned b20:
			    1;
			unsigned b21:
			    1;
			unsigned b22:
			    1;
			unsigned b23:
			    1;
			unsigned b24:
			    1;
			unsigned b25:
			    1;
			unsigned b26:
			    1;
			unsigned b27:
			    1;
			unsigned b28:
			    1;
			unsigned b29:
			    1;
			unsigned b30:
			    1;
			unsigned b31:
			    1;
	  };
} UINT32;

#define LOWER_WORD(a)   ((a).u16Words[0])

#define UPPER_WORD(a)   ((a).u16Words[1])

#define LOWER_LSB(a)    ((a).u8Bytes[0])

#define LOWER_MSB(a)    ((a).u8Bytes[1])

#define UPPER_LSB(a)    ((a).u8Bytes[2])

#define UPPER_MSB(a)    ((a).u8Bytes[3])

/*----------------------------------------------------------------------*/
typedef enum {
	  FALSE = 0,
	  TRUE
}BOOLEAN;


#ifndef bool
typedef enum _bool {
	  false = 0,
	  true
} bool;	
#endif

#define OK              TRUE
#define FAIL            FALSE

#ifndef NULL
#define NULL			0
#endif

#define NULLPTR         0				//An uninitialized pointer

#define NULLIDX         0xFF			//An unitialized index value

#define BIT0            0x01			//Mask to represent bit 0 (the LSb)
#define BIT1            0x02			//Mask to represent bit 1
#define BIT2            0x04			//Mask to represent bit 2
#define BIT3            0x08			//Mask to represent bit 3		
#define BIT4            0x10			//Mask to represent bit 4
#define BIT5            0x20			//Mask to represent bit 5
#define BIT6            0x40			//Mask to represent bit 6
#define BIT7            0x80			//Mask to represent bit 7 (the MSb of a uint8_t)


#define BIT8            0x0100			//Mask to represent bit 8 (the LSb of the most-significant byte in an uint8_t)
#define BIT9            0x0200			//Mask to represent bit 9
#define BIT10           0x0400			//Mask to represent bit 10
#define BIT11           0x0800			//Mask to represent bit 11
#define BIT12           0x1000			//Mask to represent bit 12
#define BIT13           0x2000			//Mask to represent bit 13
#define BIT14           0x4000			//Mask to represent bit 14
#define BIT15           0x8000			//Mask to represent bit 15 (the MSb of an uint16_t)

/*----------------------------------------------------------------------*/

/***************** Macros (Inline Functions) Definitions *********************/


/************************** Function Prototypes ******************************/

/************************** Variable Definitions *****************************/

/*****************************************************************************/



#endif	//_GENERIC_H_
