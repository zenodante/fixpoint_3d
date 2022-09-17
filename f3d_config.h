
#ifndef __F3d_CONFIG_H__
#define __F3d_CONFIG_H__


#ifdef __cplusplus
extern "C" {
#endif
#include "stdint.h"


#define DESKTOP
//#define RP2040

#define	USING_16BIT_FRAMEBUFF
#define USING_16BIT_ZBUFF





#define SCREEN_WIDTH   120
#define SCREEN_HEIGHT  120
#define HALF_SCREEN_WIDTH  60
#define HALF_SCREEN_HEIGHT 60

#define VIEW_PLANE      60
#define PROJECT_X_FACTOR    60
#define PROJECT_Y_FACTOR    60


#define NEAR_PLANE      5
#define FAR_PLANE       1000

#define MAX_VEC_NUM     1024

#ifdef DESKTOP 
#define __attribute__(A)
#endif


#ifdef __cplusplus
}
#endif
#endif