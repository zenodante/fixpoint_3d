
#include "f3d_type.h"

#include "f3d_config.h"
#include "f3d.h"
#include "stdio.h"
#include "stdbool.h"

#ifndef __STATIC_FORCEINLINE
#define __STATIC_FORCEINLINE static inline __attribute__((always_inline)) 
#endif

#define SIN_TABLE_LENGTH  512
#define FAST_MATH_Q15_SHIFT   (16 - 10)
#define Q_15_FRACTION_BITS 15
#define SIN_TABLE_UNIT_STEP     ((1<<Q_15_FRACTION_BITS)/SIN_TABLE_LENGTH)

#ifndef _swap_i16
#define _swap_i16(a, b)                                                    \
  {                                                                            \
    i16 t = a;                                                             \
    a = b;                                                                     \
    b = t;                                                                     \
  }
#endif

#ifndef _swap_u16
#define _swap_u16(a, b)                                                    \
  {                                                                            \
    u16 t=a;                                                          \
    a = b;                                                                     \
    b = t;                                                                     \
  }
#endif

#ifndef _swap_i32
#define _swap_i32(a, b)                                                    \
  {                                                                            \
    i32 t = a;                                                             \
    a = b;                                                                     \
    b = t;                                                                     \
  }
#endif





static color_t *fb;
zbuff_t  zb[SCREEN_WIDTH * SCREEN_HEIGHT] __attribute__((aligned(4))) = { 0 };
static i16 vecBuff[MAX_VEC_NUM * 3] = { 0 };
camera_t camera;


static void ColorHlineWithZ(Q15_16 a, Q15_16 az, Q15_16 b, Q15_16 bz, i32 y, color_t color);
static void TextureHLineWithZ(Q15_16 a, Q15_16 au, Q15_16 av, Q15_16 az, Q15_16 b, Q15_16 bu, Q15_16 bv, Q15_16 bz, i32 y, u8 uvType, u32 uSize, u8* uvMap, color_t* lut);
static void MakeO2CMatrix(m34_Q3_12* pMat, m34_Q3_12* pMO, m34_Q3_12* pMC);
static void CreateObjMat(m34_Q3_12* pMat, v3_Q15* angle, v3_i16 *objPosition);
static void PerspTrans(i16* v, i16* result, u16 num, m34_Q3_12* mat);
static void MakeCameraMatrix(m34_Q3_12* pMat, v3_Q15* angle, v3_i16* position);
__STATIC_FORCEINLINE bool TriangleFaceToViewer(i32 x0, i32 y0, i32 x1, i32 y1, i32 x2, i32 y2);
__STATIC_FORCEINLINE bool ZTest(i32 z, zbuff_t currentZ);



const i16 boxVec[24+1] = {
    8,
    32,-32,-32,-32,-32,-32,
    32,32,-32,-32,32,-32,
    32,-32,32,-32,-32,32,
    32,32,32,-32,32,32
};

const u16 boxTriIdx[12 * 3+1] = {
    12,
    3,2,0,1,3,0,
    0,2,4,2,6,4,
    4,6,5,7,5,6,
    3,1,7,1,5,7,
    6,2,3,7,6,3,
    1,0,4,5,1,4
};

const u8 boxUVIdx[12 * 3 * 2] = {
    0,0,7,0,7,7,0,7,0,0,7,7,
    7,7,0,7,7,0,0,7,0,0,7,0,
    7,0,7,7,0,0,0,7,0,0,7,7,
    0,0,7,0,0,7,7,0,7,7,0,7,
    0,0,7,0,7,7,0,7,0,0,7,7,
    7,0,7,7,0,7,0,0,7,0,0,7
};

const color_t boxLUT[6] = {
    0x0000,0xC618,0x0600,0xC000,0x0018,0xFFFF,
};

const u8 uvMap[8 * 8 + 2] = {
1,7,
1,5,1,5,1,5,1,5,
5,1,2,1,2,1,2,1,
1,2,1,3,1,3,1,5,
5,1,3,5,4,1,2,1,
1,2,1,4,5,3,1,5,
5,1,3,1,3,1,2,1,
1,2,1,2,1,2,1,5,
5,1,5,1,5,1,5,1
};




static const Q15 sinTable_q15[SIN_TABLE_LENGTH + 1] = {
    0, 402, 804, 1206, 1608, 2009, 2411, 2811, 3212, 3612, 4011, 4410, 4808,
    5205, 5602, 5998, 6393, 6787, 7180, 7571, 7962, 8351, 8740, 9127, 9512,
    9896, 10279, 10660, 11039, 11417, 11793, 12167, 12540, 12910, 13279,
    13646, 14010, 14373, 14733, 15091, 15447, 15800, 16151, 16500, 16846,
    17190, 17531, 17869, 18205, 18538, 18868, 19195, 19520, 19841, 20160,
    20475, 20788, 21097, 21403, 21706, 22006, 22302, 22595, 22884, 23170,
    23453, 23732, 24008, 24279, 24548, 24812, 25073, 25330, 25583, 25833,
    26078, 26320, 26557, 26791, 27020, 27246, 27467, 27684, 27897, 28106,
    28311, 28511, 28707, 28899, 29086, 29269, 29448, 29622, 29792, 29957,
    30118, 30274, 30425, 30572, 30715, 30853, 30986, 31114, 31238, 31357,
    31471, 31581, 31686, 31786, 31881, 31972, 32058, 32138, 32214, 32286,
    32352, 32413, 32470, 32522, 32568, 32610, 32647, 32679, 32706, 32729,
    32746, 32758, 32766, 32767, 32766, 32758, 32746, 32729, 32706, 32679,
    32647, 32610, 32568, 32522, 32470, 32413, 32352, 32286, 32214, 32138,
    32058, 31972, 31881, 31786, 31686, 31581, 31471, 31357, 31238, 31114,
    30986, 30853, 30715, 30572, 30425, 30274, 30118, 29957, 29792, 29622,
    29448, 29269, 29086, 28899, 28707, 28511, 28311, 28106, 27897, 27684,
    27467, 27246, 27020, 26791, 26557, 26320, 26078, 25833, 25583, 25330,
    25073, 24812, 24548, 24279, 24008, 23732, 23453, 23170, 22884, 22595,
    22302, 22006, 21706, 21403, 21097, 20788, 20475, 20160, 19841, 19520,
    19195, 18868, 18538, 18205, 17869, 17531, 17190, 16846, 16500, 16151,
    15800, 15447, 15091, 14733, 14373, 14010, 13646, 13279, 12910, 12540,
    12167, 11793, 11417, 11039, 10660, 10279, 9896, 9512, 9127, 8740, 8351,
    7962, 7571, 7180, 6787, 6393, 5998, 5602, 5205, 4808, 4410, 4011, 3612,
    3212, 2811, 2411, 2009, 1608, 1206, 804, 402, 0, -402, -804, -1206,
    -1608, -2009, -2411, -2811, -3212, -3612, -4011, -4410, -4808, -5205,
    -5602, -5998, -6393, -6787, -7180, -7571, -7962, -8351, -8740, -9127,
    -9512, -9896, -10279, -10660, -11039, -11417, -11793, -12167, -12540,
    -12910, -13279, -13646, -14010, -14373, -14733, -15091, -15447, -15800,
    -16151, -16500, -16846, -17190, -17531, -17869, -18205, -18538, -18868,
    -19195, -19520, -19841, -20160, -20475, -20788, -21097, -21403, -21706,
    -22006, -22302, -22595, -22884, -23170, -23453, -23732, -24008, -24279,
    -24548, -24812, -25073, -25330, -25583, -25833, -26078, -26320, -26557,
    -26791, -27020, -27246, -27467, -27684, -27897, -28106, -28311, -28511,
    -28707, -28899, -29086, -29269, -29448, -29622, -29792, -29957, -30118,
    -30274, -30425, -30572, -30715, -30853, -30986, -31114, -31238, -31357,
    -31471, -31581, -31686, -31786, -31881, -31972, -32058, -32138, -32214,
    -32286, -32352, -32413, -32470, -32522, -32568, -32610, -32647, -32679,
    -32706, -32729, -32746, -32758, -32766, -32768, -32766, -32758, -32746,
    -32729, -32706, -32679, -32647, -32610, -32568, -32522, -32470, -32413,
    -32352, -32286, -32214, -32138, -32058, -31972, -31881, -31786, -31686,
    -31581, -31471, -31357, -31238, -31114, -30986, -30853, -30715, -30572,
    -30425, -30274, -30118, -29957, -29792, -29622, -29448, -29269, -29086,
    -28899, -28707, -28511, -28311, -28106, -27897, -27684, -27467, -27246,
    -27020, -26791, -26557, -26320, -26078, -25833, -25583, -25330, -25073,
    -24812, -24548, -24279, -24008, -23732, -23453, -23170, -22884, -22595,
    -22302, -22006, -21706, -21403, -21097, -20788, -20475, -20160, -19841,
    -19520, -19195, -18868, -18538, -18205, -17869, -17531, -17190, -16846,
    -16500, -16151, -15800, -15447, -15091, -14733, -14373, -14010, -13646,
    -13279, -12910, -12540, -12167, -11793, -11417, -11039, -10660, -10279,
    -9896, -9512, -9127, -8740, -8351, -7962, -7571, -7180, -6787, -6393,
    -5998, -5602, -5205, -4808, -4410, -4011, -3612, -3212, -2811, -2411,
    -2009, -1608, -1206, -804, -402, 0
};

void SetFrameBuffAddr(color_t* pFb) {
    fb = pFb;
}

void InitCamera(void) {
    camera.angle.x = 0;
    camera.angle.y = 0;
    camera.angle.z = 0;
    camera.position.x = 0;
    camera.position.y = 0;
    camera.position.z = 0;
    MakeCameraMatrix(&(camera.mat), &(camera.angle), &(camera.position));


}

Q15 Qsin(Q15 x) {
    Q15 sinVal;                                  /* Temporary input, output variables */
    int32_t index;                                 /* Index variable */
    Q15 a, b;                                    /* Two nearest output values */
    Q15 fract;                                   /* Temporary values for fractional values */


    if (x < 0)
    { /* convert negative numbers to corresponding positive ones */
        x = (uint16_t)x + 0x8000;
    }

    /* Calculate the nearest index */
    index = (uint32_t)x >> FAST_MATH_Q15_SHIFT;

    /* Calculation of fractional value */
    fract = (x - (index << FAST_MATH_Q15_SHIFT)) << 9;

    /* Read two nearest values of input value from the sin table */
    a = sinTable_q15[index];
    b = sinTable_q15[index + 1];

    /* Linear interpolation process */
    sinVal = (Q31)(0x8000 - fract) * a >> 16;
    sinVal = (Q15)((((Q31)sinVal << 16) + ((Q31)fract * b)) >> 16);

    /* Return output value */
    return (sinVal << 1);
}

Q15 Qcos(Q15 x) {
    Q15 cosVal;                                  /* Temporary input, output variables */
    int32_t index;                                 /* Index variable */
    Q15 a, b;                                    /* Two nearest output values */
    Q15 fract;                                   /* Temporary values for fractional values */

    /* add 0.25 (pi/2) to read sine table */
    x = (uint16_t)x + 0x2000;
    if (x < 0)
    { /* convert negative numbers to corresponding positive ones */
        x = (uint16_t)x + 0x8000;
    }

    /* Calculate the nearest index */
    index = (uint32_t)x >> FAST_MATH_Q15_SHIFT;

    /* Calculation of fractional value */
    fract = (x - (index << FAST_MATH_Q15_SHIFT)) << 9;

    /* Read two nearest values of input value from the sin table */
    a = sinTable_q15[index];
    b = sinTable_q15[index + 1];

    /* Linear interpolation process */
    cosVal = (Q31)(0x8000 - fract) * a >> 16;
    cosVal = (Q15)((((Q31)cosVal << 16) + ((Q31)fract * b)) >> 16);

    /* Return output value */
    return (cosVal << 1);
}

#define Q15_MIN   ((Q15)(0x8000))
#define Q15_MAX   ((Q15)(0x7FFF))
i32 Clamp_i32(i32 x, i32 min, i32 max) {
    return x >= min ? (x <= max ? x : max) : min;
}

Q15 Qasin(Q15 x) {
    //Q15 radius = (Q15)(Clamp_Q15_16(x,Q15_MIN<<1,Q15_MAX<<1)>>1);
    if (x == Q15_MIN) {
        x += 1;
    }
    int8_t sign = 1;

    if (x < 0) {
        sign = -1;
        x *= -1;
    }

    int16_t low = 0;
    int16_t high = SIN_TABLE_LENGTH >> 2 - 1;
    int16_t middle;

    while (low <= high) {
        middle = (low + high) / 2;

        Q15 v = sinTable_q15[middle];

        if (v > x)
            high = middle - 1;
        else if (v < x)
            low = middle + 1;
        else
            break;
    }

    middle *= SIN_TABLE_UNIT_STEP;

    return sign * middle;
}

i32 f3d_Sqrt(i32 value) {
    int8_t sign = 1;

    if (value < 0) {
        sign = -1;
        value *= -1;
    }

    u32 result = 0;
    u32 a = value;
    u32 b = 1u << 30;

    while (b > a)
        b >>= 2;

    while (b != 0) {
        if (a >= result + b)
        {
            a -= result + b;
            result = result + 2 * b;
        }

        b >>= 2;
        result >>= 1;
    }

    return ((i32)result) * sign;
}

i32 NonZero(i32 value) {
    return (value + (value == 0));
}

void LookAt(v3_i16* viewer, v3_i16* pointTo, v3_Q15* returnAngle) {
    i32 dx = pointTo->x - viewer->x;
    i32 dy = pointTo->z - viewer->z;
    i32 l = f3d_Sqrt(dx * dx + dy * dy);
    dx = (dx << Q_15_FRACTION_BITS) / NonZero(l);
    //dx = (dx<<16)>>16;
    dx = Clamp_i32(dx, Q15_MIN, Q15_MAX);
    returnAngle->y = -1 * Qasin((Q15)dx);

    if (dy < 0) {
        returnAngle->y = (1 << Q_15_FRACTION_BITS) >> 2 - returnAngle->y;
    }
    dx = pointTo->y - viewer->y;
    dy = l;
    l = f3d_Sqrt(dx * dx + dy * dy);
    dx = (dx << Q_15_FRACTION_BITS) / NonZero(l);
    //dx = (dx<<16)>>16;
    dx = Clamp_i32(dx, Q15_MIN, Q15_MAX);
    returnAngle->x = Qasin((Q15)dx);
}


void M34_Mul(m34_Q3_12* pA, m34_Q3_12* pB, m34_Q3_12* pResult) {
    i32 t0, t1, t2;
    i32 s0, s1, s2;
#define M(x,y) ((pA)->m##x##y)
#define N(x,y) ((pB)->m##x##y)
#define O(x,y) ((pResult)->m##x##y)
    s0 = (i32)M(0, 0); s1 = (i32)M(1, 0); s2 = (i32)M(2, 0);
    t0 = (s0 * ((i32)N(0, 0)) + s1 * ((i32)N(0, 1)) + s2 * ((i32)N(0, 2))) >> 12;
    t1 = (s0 * ((i32)N(1, 0)) + s1 * ((i32)N(1, 1)) + s2 * ((i32)N(1, 2))) >> 12;
    t2 = (s0 * ((i32)N(2, 0)) + s1 * ((i32)N(2, 1)) + s2 * ((i32)N(2, 2))) >> 12;
    O(0, 0) = (Q3_12)t0; O(1, 0) = (Q3_12)t1; O(2, 0) = (Q3_12)t2;
    s0 = (i32)M(0, 1); s1 = (i32)M(1, 1); s2 = (i32)M(2, 1);
    t0 = (s0 * ((i32)N(0, 0)) + s1 * ((i32)N(0, 1)) + s2 * ((i32)N(0, 2))) >> 12;
    t1 = (s0 * ((i32)N(1, 0)) + s1 * ((i32)N(1, 1)) + s2 * ((i32)N(1, 2))) >> 12;
    t2 = (s0 * ((i32)N(2, 0)) + s1 * ((i32)N(2, 1)) + s2 * ((i32)N(2, 2))) >> 12;
    O(0, 1) = (Q3_12)t0; O(1, 1) = (Q3_12)t1; O(2, 1) = (Q3_12)t2;
    s0 = (i32)M(0, 2); s1 = (i32)M(1, 2); s2 = (i32)M(2, 2);
    t0 = (s0 * ((i32)N(0, 0)) + s1 * ((i32)N(0, 1)) + s2 * ((i32)N(0, 2))) >> 12;
    t1 = (s0 * ((i32)N(1, 0)) + s1 * ((i32)N(1, 1)) + s2 * ((i32)N(1, 2))) >> 12;
    t2 = (s0 * ((i32)N(2, 0)) + s1 * ((i32)N(2, 1)) + s2 * ((i32)N(2, 2))) >> 12;
    O(0, 2) = (Q3_12)t0; O(1, 2) = (Q3_12)t1; O(2, 2) = (Q3_12)t2;
#undef M
#undef N
#undef O
    i32 x = pA->m03;
    i32 y = pA->m13;
    i32 z = pA->m23;
    pResult->m03 = (i16)((((x * pB->m00) >> 2 + (y * pB->m01) >> 2 + (z * pB->m02) >> 2) >> 10)+pB->m03);
    pResult->m13 = (i16)((((x * pB->m10) >> 2 + (y * pB->m11) >> 2 + (z * pB->m12) >> 2) >> 10)+pB->m13);
    pResult->m23 = (i16)((((x * pB->m20) >> 2 + (y * pB->m21) >> 2 + (z * pB->m22) >> 2) >> 10)+pB->m23);

}
/*
void M33_Q3_12_Mul(m33_Q3_12* pA, m33_Q3_12* pB, m33_Q3_12* pResult) {

    i32 t0, t1, t2;
    i32 s0, s1, s2;
#define M(x,y) ((pA)->m##x##y)
#define N(x,y) ((pB)->m##x##y)
#define O(x,y) ((pResult)->m##x##y)
    s0 = (i32)M(0, 0); s1 = (i32)M(1, 0); s2 = (i32)M(2, 0);
    t0 = (s0 * ((i32)N(0, 0)) + s1 * ((i32)N(0, 1)) + s2 * ((i32)N(0, 2))) >> 12;
    t1 = (s0 * ((i32)N(1, 0)) + s1 * ((i32)N(1, 1)) + s2 * ((i32)N(1, 2))) >> 12;
    t2 = (s0 * ((i32)N(2, 0)) + s1 * ((i32)N(2, 1)) + s2 * ((i32)N(2, 2))) >> 12;
    O(0, 0) = (Q3_12)t0; O(1, 0) = (Q3_12)t1; O(2, 0) = (Q3_12)t2;
    s0 = (i32)M(0, 1); s1 = (i32)M(1, 1); s2 = (i32)M(2, 1);
    t0 = (s0 * ((i32)N(0, 0)) + s1 * ((i32)N(0, 1)) + s2 * ((i32)N(0, 2))) >> 12;
    t1 = (s0 * ((i32)N(1, 0)) + s1 * ((i32)N(1, 1)) + s2 * ((i32)N(1, 2))) >> 12;
    t2 = (s0 * ((i32)N(2, 0)) + s1 * ((i32)N(2, 1)) + s2 * ((i32)N(2, 2))) >> 12;
    O(0, 1) = (Q3_12)t0; O(1, 1) = (Q3_12)t1; O(2, 1) = (Q3_12)t2;
    s0 = (i32)M(0, 2); s1 = (i32)M(1, 2); s2 = (i32)M(2, 2);
    t0 = (s0 * ((i32)N(0, 0)) + s1 * ((i32)N(0, 1)) + s2 * ((i32)N(0, 2))) >> 12;
    t1 = (s0 * ((i32)N(1, 0)) + s1 * ((i32)N(1, 1)) + s2 * ((i32)N(1, 2))) >> 12;
    t2 = (s0 * ((i32)N(2, 0)) + s1 * ((i32)N(2, 1)) + s2 * ((i32)N(2, 2))) >> 12;
    O(0, 2) = (Q3_12)t0; O(1, 2) = (Q3_12)t1; O(2, 2) = (Q3_12)t2;
#undef M
#undef N
#undef O
}
*/
void V3_MUL_Mat(v3_i16 *pV, m34_Q3_12 *pM, v3_i16 *pResult){
    i32 x = pV->x;
    i32 y = pV->y;
    i32 z = pV->z;
    pResult->x = (i16)(((((x * pM->m00) >> 2) + ((y * pM->m01) >> 2) + ((z * pM->m02) >> 2))>>10)+pM->m03);
    pResult->y = (i16)(((((x * pM->m10) >> 2) + ((y * pM->m11) >> 2) + ((z * pM->m12) >> 2))>>10)+pM->m13);
    pResult->z = (i16)(((((x * pM->m20) >> 2) + ((y * pM->m21) >> 2) + ((z * pM->m22) >> 2))>>10)+pM->m23);
}

//Rotation Order: ZXY
/*
void CreateRMat(Q15 x, Q15 y, Q15 z, m33_Q3_12* pR) {
    i32 sx, sy, sz, cx, cy, cz;
    x *= -1;
    y *= -1;
    z *= -1;
    sx = (i32)Qsin(x);
    cx = (i32)Qcos(x);
    sy = (i32)Qsin(y);
    cy = (i32)Qcos(y);
    sz = (i32)Qsin(z);
    cz = (i32)Qcos(z);

    i32 temp = (sz * sx) >> 15;
    pR->m00 = (Q3_12)(((temp * sy) + (cy * cz)) >> 18);
    pR->m20 = (Q3_12)(((temp * cy) - (cz * sy)) >> 18);
    pR->m10 = (Q3_12)((cx * sz) >> 18);
    
    temp = (cz * sx) >> 15;
    pR->m01 = (Q3_12)(((temp * sy) - (cy * sz)) >> 18);
    pR->m21 = (Q3_12)(((temp * cy) + (sy * sz)) >> 18);
    pR->m11 = (Q3_12)((cx * cz) >> 18);
    

    pR->m02 = (Q3_12)((cx * sy) >> 18);
    pR->m12 = (Q3_12)((-1*sx)>>3);
    pR->m22 = (Q3_12)((cy * cx) >> 18);

}
*/
//Rotation Order: ZXY
void CreateRMat34(v3_Q15* angle, m34_Q3_12* pR) {
    i32 sx, sy, sz, cx, cy, cz;
    Q15 x =  -1*angle->x;
    Q15 y  = -1*angle->y;
    Q15 z  = -1*angle->z;
    sx = (i32)Qsin(x);
    cx = (i32)Qcos(x);
    sy = (i32)Qsin(y);
    cy = (i32)Qcos(y);
    sz = (i32)Qsin(z);
    cz = (i32)Qcos(z);

    i32 temp = (sz * sx) >> 15;
    pR->m00 = (Q3_12)(((temp * sy) + (cy * cz)) >> 18);
    pR->m20 = (Q3_12)(((temp * cy) - (cz * sy)) >> 18);
    pR->m10 = (Q3_12)((cx * sz) >> 18);

    temp = (cz * sx) >> 15;
    pR->m01 = (Q3_12)(((temp * sy) - (cy * sz)) >> 18);
    pR->m21 = (Q3_12)(((temp * cy) + (sy * sz)) >> 18);
    pR->m11 = (Q3_12)((cx * cz) >> 18);


    pR->m02 = (Q3_12)((cx * sy) >> 18);
    pR->m12 = (Q3_12)((-1 * sx) >> 3);
    pR->m22 = (Q3_12)((cy * cx) >> 18);
    pR->m03 = 0;
    pR->m13 = 0;
    pR->m23 = 0;

}

void TransposeMat3(m34_Q3_12* pMat){
    Q3_12 temp;
    Q3_12* m = (Q3_12*)pMat;
    temp = m[1];
    m[1] = m[4];
    m[4] = temp;

    temp = m[2];
    m[2] = m[8];
    m[8] = temp;

    temp = m[6];
    m[6] = m[9];
    m[9] = temp;
}



    



void UpdateCameraMatrix(void) {
    MakeCameraMatrix(&(camera.mat), &(camera.angle), &(camera.position));
}





void RenderTextureObjNoLight(v3_Q15* angle, v3_i16* objPosition, i16* vec, u16* triIdx, u8* uvIdx, u8* uvmap, color_t* lut) {

    i32 vecNum = (i32)(vec[0]);
    vec++;
    i32 triNum = (i32)(triIdx[0]);
    triIdx++;
    u32 idx;
    //create o2c matrix and shift


    m34_Q3_12 mat;
    //v3_i16 objShift;
    //CreateRMat(angle->x, angle->y, angle->z, &mat);
    CreateObjMat(&mat, angle, objPosition);

    MakeO2CMatrix(&mat,&mat, &(camera.mat));
    //generate vec buff
    PerspTrans(vec, vecBuff, vecNum, &mat);
    uint32_t i;
    //printf("vec buff:\n");
    //for (i = 0; i < 8; i++) {

    //    printf("x: %d,y:%d,z:%d\n", vecBuff[i * 3], vecBuff[i * 3 + 1], vecBuff[i * 3 + 2]);
    //}
    //draw triangle
    u32 v0, v1, v2;
    i32 x0, x1, x2, y0, y1, y2, z0, z1, z2, u0, u1, u2;

    while (triNum--) {
        v0 = *triIdx++;
        v0 *= 3;
        v1 = *triIdx++;
        v1 *= 3;
        v2 = *triIdx++;
        v2 *= 3;

        x0 = (i32)(vecBuff[v0]);
        y0 = (i32)(vecBuff[v0 + 1]);
        z0 = (i32)(vecBuff[v0 + 2]);
        u0 = (i32)(*uvIdx++);
        v0 = (i32)(*uvIdx++);
        x1 = (i32)(vecBuff[v1]);
        y1 = (i32)(vecBuff[v1 + 1]);
        z1 = (i32)(vecBuff[v1 + 2]);
        u1 = (i32)(*uvIdx++);
        v1 = (i32)(*uvIdx++);
        x2 = (i32)(vecBuff[v2]);
        y2 = (i32)(vecBuff[v2 + 1]);
        z2 = (i32)(vecBuff[v2 + 2]);
        u2 = (i32)(*uvIdx++);
        v2 = (i32)(*uvIdx++);
        //if ((z0<=0)|| (z1 <= 0)||(z2 <= 0)){
        if ((z0 < NEAR_PLANE) || (z0 > FAR_PLANE) || (z1 < NEAR_PLANE) || (z1 > FAR_PLANE) || (z2 < NEAR_PLANE) || (z2 > FAR_PLANE)) {
        //if ((z0 < NEAR_PLANE)  || (z1 < NEAR_PLANE) || (z2 < NEAR_PLANE) ) {
            //printf("%d, %d, %d\n", z0, z1, z2);
            continue;
        }
        //if (((x0 < SCREEN_WIDTH) && (x0 >= 0) && (y0 < SCREEN_HEIGHT) && (y0 >= 0)) || ((x1 < SCREEN_WIDTH) && (x1 >= 0) && (y1 < SCREEN_HEIGHT) && (y1 >= 0)) || ((x2 < SCREEN_WIDTH) && (x2 >= 0) && (y2 < SCREEN_HEIGHT) && (y2 >= 0))) {
            if (TriangleFaceToViewer(x0, y0, x1, y1, x2, y2)) {
                TriTextureZ(x0, y0, z0, u0, v0,
                    x1, y1, z1, u1, v1,
                    x2, y2, z2, u2, v2, uvmap, lut);
            }
       // }
    }
}


void NewFrame(void) {
    ClearFBuff();
    ClearZBuff();
}

void ClearFBuff(void) {
    u32 i = CLEAR_F_BUFF_LENGTH;
    u32 *addr = fb;
    while (i--) {
        *addr++ = CLEAR_F_BUFF_WORD;
    }
}

void ClearZBuff(void) {
    u32 i = CLEAR_Z_BUFF_LENGTH;
    u32* addr = zb;
    while (i--) {
        *addr++ = CLEAR_Z_BUFF_WORD;
    }
}


void TriColorZ(i32 x0, i32 y0, i32 z0, i32 x1, i32 y1, i32 z1, i32 x2, i32 y2, i32 z2, color_t color) {
    i32 a, b, y, top, last;
    i32 divTemp01 = 0, divTemp12 = 0, divTemp02 = 0;
    i32 az, bu, bv, bz;
    i32 distance;
    if (y0 > y1) {
        _swap_i32(y0, y1); _swap_i32(x0, x1);  _swap_i32(z0, z1);
    }
    if (y1 > y2) {
        _swap_i32(y2, y1); _swap_i32(x2, x1);  _swap_i32(z2, z1);
    }
    if (y0 > y1) {
        _swap_i32(y0, y1); _swap_i32(x0, x1);  _swap_i32(z0, z1);
    }
    if (y0 == y2) { // Handle awkward all-on-same-line case as its own thing
        return;
    }
    i32 dx01 = x1 - x0;
    i32 dy01 = y1 - y0;
    i32 dz01 = z1 - z0;
    i32 dx02 = x2 - x0;
    i32 dy02 = y2 - y0;
    i32 dz02 = z2 - z0;
    i32 dx12 = x2 - x1;
    i32 dy12 = y2 - y1;
    i32 dz12 = z2 - z1;
    x0 = x0 << 16;
    z0 = z0 << 16;
    x1 = x1 << 16;
    z1 = z1 << 16;
    x2 = x2 << 16;
    z2 = z2 << 16;
    if (dy01 != 0) {
        divTemp01 = f3d_div((1 << 16), dy01);
    }
    if (y1 == y2) {
        last = y1; // Include y1 
    }
    else {
        divTemp12 = f3d_div((1 << 16), dy12);
        last = y1 - 1; // Skip it
    }
    divTemp02 = f3d_div((1 << 16), dy02);
    i32 sa = 0;
    i32 saZ = 0;
    i32 sb = 0;
    i32 sbZ = 0;

    if (y0 < 0) {
        i32 tempy = y0 * -1;
        //y0 *= -1;
        sa = tempy * dx01;
        saZ = tempy * dz01;
        sb = tempy * dx02;
        sbZ = tempy * dz02;
        top = 0;
    }
    else {
        top = y0;
    }
    if (last >= SCREEN_HEIGHT) {
        last = SCREEN_HEIGHT - 1;
    }
    for (y = top; y <= last; y++) {
        a = x0 + ((sa * divTemp01));
        az = z0 + ((saZ * divTemp01));
        b = x0 + ((sb * divTemp02));
        bz = z0 + ((sbZ * divTemp02));
        sa += dx01;
        sb += dx02;
        saZ += dz01;
        sbZ += dz02;

        if (a < b) {
            ColorHlineWithZ(a, az, b, bz, y, color);
        }
        else {
            ColorHlineWithZ(b, bz, a, az, y, color);
        }

    }
    distance = y - y1;
    sa = (i32)dx12 * distance;
    saZ = (i32)dz12 * distance;
    distance = y - y0;
    sb = (i32)dx02 * distance;
    sbZ = (i32)dz02 * distance;

    if (y2 >= SCREEN_HEIGHT) {
        last = SCREEN_HEIGHT - 1;
    }
    else {
        last = y2;
    }
    for (; y <= last; y++) {

        a = x1 + ((sa * divTemp12));
        az = z1 + ((saZ * divTemp12));
        b = x0 + ((sb * divTemp02));
        bz = z0 + ((sbZ * divTemp02));
        sa += dx12;
        saZ += dz12;
        sb += dx02;
        sbZ += dz02;
        if (a < b) {
            ColorHlineWithZ(a, az, b, bz, y, color);
        }
        else {
            ColorHlineWithZ(b, bz, a, az, y, color);
        }
    }
}



void TriTextureZ(i32 x0, i32 y0, i32 z0, i32 u0, i32 v0,
    i32 x1, i32 y1, i32 z1, i32 u1, i32 v1,
    i32 x2, i32 y2, i32 z2, i32 u2, i32 v2, u8* uvmap, color_t* lut) {
    u8  uvType = uvmap[0];
    u32 uSize = (u32)(uvmap[1])+1;
    i32 a, b, y, top, last;
    i32 divTemp01 = 0, divTemp12 = 0, divTemp02 = 0;
    i32 au, av, az, bu, bv, bz;
    i32 distance;
    if (y0 > y1) {
        _swap_i32(y0, y1); _swap_i32(x0, x1); _swap_i32(u0, u1); _swap_i32(v0, v1); _swap_i32(z0, z1);
    }
    if (y1 > y2) {
        _swap_i32(y2, y1); _swap_i32(x2, x1); _swap_i32(u2, u1); _swap_i32(v2, v1); _swap_i32(z2, z1);
    }
    if (y0 > y1) {
        _swap_i32(y0, y1); _swap_i32(x0, x1); _swap_i32(u0, u1); _swap_i32(v0, v1); _swap_i32(z0, z1);
    }
    if (y0 == y2) { // Handle awkward all-on-same-line case as its own thing
        return;
    }
    i32 dx01 = x1 - x0;
    i32 dy01 = y1 - y0;
    i32 dz01 = z1 - z0;
    i32 dx02 = x2 - x0;
    i32 dy02 = y2 - y0;
    i32 dz02 = z2 - z0;
    i32 dx12 = x2 - x1;
    i32 dy12 = y2 - y1;
    i32 dz12 = z2 - z1;
    u0 = u0 << 22;
    u1 = u1 << 22;
    u2 = u2 << 22;
    v0 = v0 << 22;
    v1 = v1 << 22;
    v2 = v2 << 22;
    x0 = x0 << 16;
    z0 = z0 << 16;
    x1 = x1 << 16;
    z1 = z1 << 16;
    x2 = x2 << 16;
    z2 = z2 << 16;
    au = u0 ;
    bu = u0 ;
    av = v0 ;
    bv = v0 ;

    i32 du01=0, du02=0, du12=0, dv01=0, dv02=0, dv12=0;
    if (dy01 != 0) {
        divTemp01 = f3d_div((1 << 16), dy01);
        du01 = f3d_div((u1 - u0),dy01);
        dv01 = f3d_div((v1 - v0),dy01);
    }
    if (y1 == y2) {
        last = y1; // Include y1 
    }else {
        divTemp12 = f3d_div((1 << 16),dy12);
        du12 = f3d_div((u2 - u1),dy12);
        dv12 = f3d_div((v2 - v1),dy12);//Q9_22
        last = y1 - 1; // Skip it
    }

    divTemp02 = f3d_div((1 << 16), dy02);
    du02 = f3d_div((u2 - u0), dy02);
    dv02 = f3d_div((v2 - v0), dy02);




    i32 sa = 0;
    i32 saZ = 0;
    i32 sb = 0;
    i32 sbZ = 0;

    if (y0 < 0) {
        i32 tempy = y0 * -1;
        //y0 *= -1;
        sa = tempy * dx01;
        au += tempy * du01;
        av += tempy * dv01;
        saZ = tempy * dz01;
        sb = tempy * dx02;
        bu += tempy * du02;
        bv += tempy * dv02;
        sbZ = tempy * dz02;
        top = 0;
    }
    else {
        top = y0;
    }
    u8* uvmapStart = &(uvmap[2]);
    if (last >= SCREEN_HEIGHT) {
        last = SCREEN_HEIGHT - 1;
    }

    for (y = top; y <= last; y++) {
        a = x0 + ((sa * divTemp01));
        az = z0 + ((saZ * divTemp01));
        b = x0 + ((sb * divTemp02)); 
        bz = z0 + ((sbZ * divTemp02));
        sa += dx01;
        sb += dx02;     
        saZ += dz01;
        sbZ += dz02;

        if (a < b) {
            TextureHLineWithZ(a, au, av, az, b, bu, bv, bz, y, uvType, uSize, uvmapStart, lut);
        }
        else {
            TextureHLineWithZ(b, bu, bv, bz, a, au, av, az, y, uvType, uSize, uvmapStart, lut);
        }
        au += du01;
        av += dv01;
        bu += du02;
        bv += dv02;
    }
    distance = y - y1;
    sa = (i32)dx12 * distance;
    au = u1+ (i32)du12 * distance;
    av = v1+ (i32)dv12 * distance;
    saZ = (i32)dz12 * distance;
    distance = y - y0;
    sb = (i32)dx02 * distance;
    bu =u0+ (i32)du02 * distance;
    bv =v0+ (i32)dv02 * distance;
    sbZ = (i32)dz02 * distance;

    if (y2 >= SCREEN_HEIGHT) {
        last = SCREEN_HEIGHT - 1;
    }
    else {
        last = y2;
    }
    for (; y <= last; y++) {

        a = x1 + ((sa * divTemp12));
        az = z1 + ((saZ * divTemp12));
        b = x0 + ((sb * divTemp02));
        bz = z0 + ((sbZ * divTemp02));
        sa += dx12;
        saZ += dz12;
        sb += dx02;
        sbZ += dz02;
        if (a < b) {
            TextureHLineWithZ(a, au, av, az, b, bu, bv, bz, y, uvType, uSize, uvmapStart, lut);
        }
        else {
            TextureHLineWithZ(b, bu, bv, bz, a, au, av, az, y, uvType, uSize, uvmapStart, lut);
        }
        au += du12;
        av += dv12;
        bu += du02;
        bv += dv02;

    }
}

void PutColorTestZ(color_t* pixel, zbuff_t* zbuff, color_t color, i32 z) {
    zbuff_t currentZ = *zbuff;
    if (z <= ((i32)currentZ)) {
        *zbuff = (zbuff_t)z;
        *pixel = color;
    }
}

void TLine4bitTiltmap(i16 x0, i16 y0, i16 u0, i16 v0, i16 x1, i16 y1, i16 u1, i16 v1, u8* pMap, u8* pTiltmap, color_t* lut) {

}
/*---------------------------------------Static Functions------------------------------------------------------------*/
static void ColorHlineWithZ(Q15_16 a,  Q15_16 az, Q15_16 b,  Q15_16 bz, i32 y, color_t color) {

    i32 ib = (b + (1 << 15)) >> 16;
    i32 ia = (a + (1 << 15)) >> 16;
    Q15_16 delta = ib - ia;
    i32 step = delta + 1;
    if (delta == 0) {
        delta = 1;
    }
    delta = f3d_div((1 << 16), delta);
    Q15_16 dz = bz - az;
    Q15_16 sz = 0;
    i32  z;

    if (ia < 0) {
        ia *= -1;
        sz = ia * dz;
        step -= ia;
        ia = 0;
    }
    if (ib >= SCREEN_WIDTH) {
        ib -= (SCREEN_WIDTH);
        step -= (ib + 1);
    }
    if (step <= 0) {
        return;
    }
    color_t* pixel = &(fb[SCREEN_WIDTH * y + ia]);
    zbuff_t* zbuff = &(zb[SCREEN_WIDTH * y + ia]);

    az += (1 << 15);
    for (step; step > 0; step--) {
        z = (az + ((sz >> 16) * delta)) >> 16;
        sz += dz;
        if (ZTest(z, *zbuff)) {
            *zbuff = (zbuff_t)z;
            *pixel = color;
        }
        pixel++;
        zbuff++;

    }


}


static void TextureHLineWithZ(Q15_16 a, Q15_16 au, Q15_16 av, Q15_16 az, Q15_16 b, Q15_16 bu, Q15_16 bv, Q15_16 bz, i32 y, u8 uvType, u32 uSize, u8* uvMap, color_t* lut) {

    i32 ib = (b + (1 << 15)) >> 16;
    i32 ia = (a + (1 << 15)) >> 16;
    Q15_16 delta = ib - ia;
    i32 step = delta + 1;
    if (delta == 0) {
        delta = 1;
    }
    Q15_16 du = f3d_div((bu - au),delta);//max 255
    Q15_16 dv = f3d_div((bv - av),delta);
    delta = f3d_div((1 << 16), delta);
    Q15_16 dz = bz - az;
    Q15_16 sz = 0;
    color_t color;
    i32 u, v, z;
    u = au+(1 << 21);
    v = av + (1 << 21);
    if (ia < 0) {
        ia *= -1;
        u += ia * du;
        v += ia * dv;
        sz = ia * dz;
        step -= ia;
        ia = 0;
    }
    if (ib >= SCREEN_WIDTH) {
        ib -= (SCREEN_WIDTH);
        step -= (ib + 1);
    }
    if (step <= 0) {
        return;
    }
    color_t* pixel = &(fb[SCREEN_WIDTH * y + ia]);
    zbuff_t* zbuff = &(zb[SCREEN_WIDTH * y + ia]);
    u8 index;

    az += (1 << 15);


    switch (uvType) {
    case 0:

        for (step; step > 0; step--) {
            z = (az + (sz >> 16) * delta) >> 16;
            sz += dz;
            index = uvMap[(uSize * (v>>22) + (u>>22)) >> 1];
            if (u & 0x01) {
                index = (index & 0xf0) >> 4;
            }
            else {
                index = index & 0x0f;
            }
            if (index != 0) {
                color = lut[index];
                if (ZTest(z, *zbuff)) {
                    *zbuff = (zbuff_t)z;
                    *pixel = color;
                }
            }
            //PutColorTestZ(pixel, zbuff, color, z);
            //*pixel = color;
            pixel++;
            zbuff++;
            u += du;
            v += dv;

        }
        return;
    case 1:
        for (step; step > 0; step--) {
            z = (az + ((sz >> 16) * delta)) >> 16;
            sz += dz;
            index = uvMap[uSize * (v >> 22) + (u >> 22)];
            if (index != 0) {
                color = lut[index];
                if (ZTest(z, *zbuff)) {
                    *zbuff = (zbuff_t)z;
                    *pixel = color;
                }
            }
            //PutColorTestZ(pixel, zbuff, color, z);
            pixel++;
            zbuff++;
            u += du;
            v += dv;
        }
        return;
    }

}

static void MakeO2CMatrix(m34_Q3_12* pMat, m34_Q3_12* pMO, m34_Q3_12* pMC) {
    M34_Mul(pMO, pMC, pMat);
}


static void CreateObjMat(m34_Q3_12* pMat, v3_Q15* angle, v3_i16* objPosition) {
    CreateRMat34(angle, pMat);
    pMat->m03 = objPosition->x;
    pMat->m13 = objPosition->y;
    pMat->m23 = objPosition->z;
}




static void PerspTrans(i16* v, i16* result, u16 num, m34_Q3_12* mat) {
    i32 x, y, z;
    i32 ox, oy, oz, temp;
    i32 sx = ((i32)(mat->m03)) << 12;
    i32 sy = ((i32)(mat->m13)) << 12;
    i32 sz = ((i32)(mat->m23)) << 12;
    while (num--) {
        x = *v++;
        y = *v++;
        z = *v++;

        ox = ((x * mat->m00 + y * mat->m01 + z * mat->m02) + sx) >> 2;//Q21_10
        oy = ((x * mat->m10 + y * mat->m11 + z * mat->m12) + sy) >> 2;//Q21_10
        oz = ((x * mat->m20 + y * mat->m21 + z * mat->m22) + sz);//Q19_12
        if (oz < (NEAR_PLANE << 12)) {
            *result++ = 0;
            *result++ = 0;
            *result++ = 0;
        }
        else {
            temp = (f3d_div((1 << 28), oz));// Q15_16
            *result++ = (i16)(((((temp * ox) >> 6) * PROJECT_X_FACTOR + (1 << 19)) >> 20) + HALF_SCREEN_WIDTH);
            *result++ = (i16)(HALF_SCREEN_HEIGHT - ((((temp * oy) >> 6) * PROJECT_Y_FACTOR + (1 << 19)) >> 20));
            *result++ = (i16)(oz >> 12);
            //printf("oz %X,%X\n", oz, oz >> 12);
        }
    }
}

static void MakeCameraMatrix(m34_Q3_12* pMat, v3_Q15* angle, v3_i16* position) {

    CreateRMat34(angle, pMat);
    TransposeMat3(pMat); // transposing creates an inverse transform
    i32 x = -1 * (i32)(position->x);
    i32 y = -1 * (i32)(position->y);
    i32 z = -1 * (i32)(position->z);


    pMat->m03 = (i16)((((x * pMat->m00) >> 2) + ((y * pMat->m01) >> 2) + ((z * pMat->m02) >> 2)) >> 10);
    pMat->m13 = (i16)((((x * pMat->m10) >> 2) + ((y * pMat->m11) >> 2) + ((z * pMat->m12) >> 2)) >> 10);
    pMat->m23 = (i16)((((x * pMat->m20) >> 2) + ((y * pMat->m21) >> 2) + ((z * pMat->m22) >> 2)) >> 10);
}


__STATIC_FORCEINLINE bool ZTest(i32 z, zbuff_t currentZ) {
    if (z <= ((i32)currentZ)) {
        return true;
    }
    else {
        return false;
    }
}

__STATIC_FORCEINLINE bool TriangleFaceToViewer(i32 x0, i32 y0, i32 x1, i32 y1, i32 x2, i32 y2) {
    i32 winding =
        (x1 - x0) * (y2 - y0) - (y1 - y0) * (x2 - x0);
    // ^ cross product for points with z == 0
    return winding <= 0 ? true : false;
}

