#ifndef __F3d__H__
#define __F3d__H__


#ifdef __cplusplus
extern "C" {
#endif

#include "f3d_type.h"
#include "f3d_config.h"

#ifdef RP2040

#include "pico/divider.h"
#define f3d_div(a,b)	div_s32s32_unsafe(a,b)
#else
#define f3d_div(a,b) (a/b)
#endif
	extern zbuff_t  zb[SCREEN_WIDTH * SCREEN_HEIGHT];
	//Test obj
	extern const u8 uvMap[8 * 8 + 2];
	extern const color_t boxLUT[6];
	extern const i16 boxVec[24+1];
	extern const u16 boxTriIdx[12 * 3+1];
	extern const u8 boxUVIdx[12 * 3 * 2];
	extern camera_t camera;

	//camera functions
	extern void InitCamera(void);
	extern void LookAt(v3_i16* viewer, v3_i16* pointTo, v3_Q15* returnAngle);
	extern void UpdateCameraMatrix(void);

	//init functions
	extern void SetFrameBuffAddr(color_t* pFb);
	extern void NewFrame(void);
	extern void ClearFBuff(void);
	extern void ClearZBuff(void);
	//math functions
	extern i32 Clamp_i32(i32 x, i32 min, i32 max);
	extern i32 f3d_Sqrt(i32 value);
	extern i32 NonZero(i32 value);
	extern Q15 Qsin(Q15 x);
	extern Q15 Qcos(Q15 x);
	extern Q15 Qasin(Q15 x);
	extern void M34_Mul(m34_Q3_12* pA, m34_Q3_12* pB, m34_Q3_12* pResult);
	extern void V3_MUL_Mat(v3_i16* pV, m34_Q3_12* pM, v3_i16* pResult);
	extern void CreateRMat34(v3_Q15* angle, m34_Q3_12* pR);
	extern void TransposeMat3(m34_Q3_12* pMat);
	
	//render functions
	extern void RenderTextureObjNoLight(v3_Q15* angle, v3_i16* objPosition, i16* vec, u16* triIdx, u8* uvIdx, u8* uvmap, color_t* lut);

	//draw functions
	extern void TriColorZ(i32 x0, i32 y0, i32 z0, i32 x1, i32 y1, i32 z1, i32 x2, i32 y2, i32 z2, color_t color);
	extern void TriTextureZ(i32 x0, i32 y0, i32 z0, i32 u0, i32 v0,
							i32 x1, i32 y1, i32 z1, i32 u1, i32 v1,
							i32 x2, i32 y2, i32 z2, i32 u2, i32 v2, u8* uvmap, color_t* lut);	
	
	extern void PutColorTestZ(color_t* pixel, zbuff_t* zbuff, color_t color, i32 z);
	extern void TLine4bitTiltmap(i16 x0, i16 y0,  i16 u0, i16 v0, i16 x1, i16 y1, i16 u1, i16 v1, u8* pMap, u8* pTiltmap, color_t* lut);
#ifdef __cplusplus
}
#endif
#endif

//extern void M33_Q3_12_Mul(m33_Q3_12* pA, m33_Q3_12* pB, m33_Q3_12* pResult);
//extern void CreateRMat(Q15 x, Q15 y, Q15 z, m33_Q3_12* pR);
//extern void PerspTrans(i16* v, i16* result, u16 num, m34_Q3_12* mat);