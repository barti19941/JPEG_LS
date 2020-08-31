#ifndef _GLOBALS_H_  
#define _GLOBALS_H_

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdint.h>
#include <time.h>
#include <string.h>

#define MAX(x,y) ((x > y) ? x:y)
#define MIN(x,y) ((x > y) ? y:x)
#define ABS(x) ((x < 0) ? -x:x)	

typedef struct
{
	uint16_t x;
	uint16_t y;
}
point;

typedef struct
{
	uint16_t symbol;
	uint16_t nbBits;
	uint32_t newX;	
} 
t;

uint8_t Compare3D(uint8_t* data_1, uint8_t* data_2, uint16_t w, uint16_t h);
uint8_t Compare2D(uint8_t* data_1, uint8_t* data_2, uint16_t w, uint16_t h);
uint8_t Compare1D(uint8_t* data_1, uint8_t* data_2, uint32_t size);

void Write1DDatWhole(uint8_t* data, uint32_t size, const char* name);
void Write1DDat(uint8_t* data, uint16_t w, uint16_t h, uint32_t size, const char* name);
void Write1DJlsGray(uint8_t* data, uint16_t w, uint16_t h, uint32_t size, const char* name);
void Write1DJlsColorNoInterleaved(uint8_t* data, uint16_t w, uint16_t h, uint32_t size, const char* name);
void Write1DJlsColorLineInterleaved(uint8_t* data, uint16_t w, uint16_t h, uint32_t size, const char* name);
void Write1DJlsColorSampleInterleaved(uint8_t* data, uint16_t w, uint16_t h, uint32_t size, const char* name);
void Write2DDat(uint8_t* data, uint16_t w, uint16_t h, const char* name);
void Write2DPgm(uint8_t* data, uint16_t w, uint16_t h, const char* name);
void Write3DDat(uint8_t* data, uint16_t w, uint16_t h, const char* name);
void Write3DPpm(uint8_t* data, uint16_t w, uint16_t h, const char* name);

uint8_t* Read1DDatWhole(uint32_t* size, const char* name);
uint8_t* Read1DDat(uint16_t* w, uint16_t* h, uint32_t* size, const char* name);
uint8_t* Read1DJlsGray(uint16_t* w, uint16_t* h, uint32_t* size, const char* name);
uint8_t* Read1DJlsColorNoInterleaved(uint16_t* w, uint16_t* h, uint32_t* size_table, const char* name);
uint8_t* Read1DJlsColorLineInterleaved(uint16_t* w, uint16_t* h, uint32_t* size_table, const char* name);
uint8_t* Read1DJlsColorSampleInterleaved(uint16_t* w, uint16_t* h, uint32_t* size_table, const char* name);
uint8_t* Read2DDat(uint16_t* w, uint16_t* h, const char* name);
uint8_t* Read2DPgm(uint16_t* w, uint16_t* h, const char* name);
uint8_t* Read3DDat(uint16_t* w, uint16_t* h, const char* name);
uint8_t* Read3DPpm(uint16_t* w, uint16_t* h, const char* name);				
	
#endif


