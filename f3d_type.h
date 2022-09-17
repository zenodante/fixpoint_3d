#ifndef __F3d_TYPE_H__
#define __F3d_TYPE_H__


#ifdef __cplusplus
extern "C" {
#endif

#include "stdint.h"
#include "f3d_config.h"
	typedef int16_t    Q7_8;
	typedef int16_t    Q3_12;
	typedef int16_t    i16;
	typedef int16_t    Q15;
	typedef int32_t    Q15_16;
	typedef int32_t    i32;
	typedef int32_t    Q31;
	typedef uint16_t   u16;
	typedef uint32_t   u32;
	typedef uint8_t    u8;

#ifdef USING_16BIT_FRAMEBUFF
	typedef uint16_t color_t;
	#define CLEAR_F_BUFF_LENGTH     120*60
	#define CLEAR_F_BUFF_WORD       0x00000000UL
#endif

#ifdef USING_16BIT_ZBUFF
	typedef uint16_t  zbuff_t;
	#define CLEAR_Z_BUFF_LENGTH     120*60
	#define CLEAR_Z_BUFF_WORD       0xFFFFFFFFUL
#endif

	typedef struct {
		//row0  row1    row2    row3    
		Q3_12 m00; Q3_12 m01; Q3_12 m02;//column 0
		Q3_12 m10; Q3_12 m11; Q3_12 m12;//column 1
		Q3_12 m20; Q3_12 m21; Q3_12 m22;//column 2
	}m33_Q3_12;

	typedef struct {
		//row0  row1    row2    row3    
		Q3_12 m00; Q3_12 m01; Q3_12 m02; i16 m03;//column 0
		Q3_12 m10; Q3_12 m11; Q3_12 m12; i16 m13;//column 1
		Q3_12 m20; Q3_12 m21; Q3_12 m22; i16 m23;//column 2
	}m34_Q3_12;


	typedef struct {
		i16 x;
		i16 y;
		i16 z;
	}v3_i16;

	typedef struct {
		Q15 x;
		Q15 y;
		Q15 z;
	}v3_Q15;

	typedef struct {
		i32 x;
		i32 y;
		i32 z;
	}v3_i32;


	typedef struct {
		v3_Q15 angle;
		v3_i16 position;
		m34_Q3_12 mat;
	}camera_t;
#ifdef __cplusplus
}
#endif
#endif