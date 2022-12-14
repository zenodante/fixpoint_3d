#include "stdint.h"
const int16_t building_vec[73]={
	24,
	-48, 0, 48, 0, 64, 48, -48, 64, 48, 
	0, 0, 48, 48, 64, 48, 48, 0, 48, 
	-48, 0, 0, -48, 64, 0, -48, 0, -48, 
	-48, 64, -48, 48, 64, 0, 48, 0, 0, 
	48, 64, -48, 48, 0, -48, 0, 0, -48, 
	0, 64, -48, 0, 112, -48, 0, 112, 48, 
	-48, 64, 16, 0, 112, 16, 0, 112, -16, 
	-48, 64, -16, 48, 64, 16, 48, 64, -16, 
	};
const uint16_t building_face[97]={
	32,
	0, 2, 1, 0, 1, 3, 3, 1, 4, 3, 4, 5, 
	6, 7, 2, 6, 2, 0, 8, 9, 7, 8, 7, 6, 
	5, 4, 10, 5, 10, 11, 11, 10, 12, 11, 12, 13, 
	14, 15, 9, 14, 9, 8, 13, 12, 15, 13, 15, 14, 
	15, 16, 9, 12, 16, 15, 1, 17, 4, 2, 17, 1, 
	18, 19, 17, 18, 17, 2, 9, 16, 20, 9, 20, 21, 
	21, 20, 19, 21, 19, 18, 4, 17, 19, 4, 19, 22, 
	22, 19, 20, 22, 20, 23, 23, 20, 16, 23, 16, 12, 
	};
extern const uint8_t building_uv[192]={
	48, 63, 48, 31, 72, 31, 48, 63, 72, 31, 72, 63, 
	72, 63, 72, 31, 96, 31, 72, 63, 96, 31, 96, 63, 
	56, 127, 56, 95, 80, 95, 56, 127, 80, 95, 80, 127, 
	32, 127, 32, 95, 56, 95, 32, 127, 56, 95, 56, 127, 
	80, 127, 80, 95, 104, 95, 80, 127, 104, 95, 104, 127, 
	104, 127, 104, 95, 127, 95, 104, 127, 127, 95, 127, 127, 
	24, 63, 24, 31, 48, 31, 24, 63, 48, 31, 48, 63, 
	0, 63, 0, 31, 24, 31, 0, 63, 24, 31, 24, 63, 
	24, 31, 24, 7, 48, 31, 0, 31, 24, 7, 24, 31, 
	72, 31, 72, 7, 96, 31, 48, 31, 72, 7, 72, 31, 
	112, 31, 112, 7, 127, 7, 112, 31, 127, 7, 127, 31, 
	112, 31, 112, 7, 127, 7, 112, 31, 127, 7, 127, 31, 
	96, 31, 96, 7, 112, 7, 96, 31, 112, 7, 112, 31, 
	48, 95, 48, 71, 64, 71, 48, 95, 64, 71, 64, 95, 
	32, 95, 32, 71, 48, 71, 32, 95, 48, 71, 48, 95, 
	48, 95, 48, 71, 64, 71, 48, 95, 64, 71, 64, 95, 
	};
