#ifndef _ENCODER_H_  
#define _ENCODER_H_

#include "globals.h"

typedef struct 
{				
	uint8_t* bitstream_regular;	
	uint8_t* bitstream_run;		
	uint8_t* extended_data;
	uint16_t w;
	uint16_t h;
	uint16_t RANGE;
	uint8_t MAXVAL;
	uint8_t qbpp;
	uint8_t bpp;
	uint8_t LIMIT;
	uint16_t A[367];
	int16_t B[365];
	int16_t C[365];
	uint16_t N[367];
	uint16_t Nn[367];							
	uint8_t J[32];
	int16_t D[3];
	int8_t Q[3];
	uint8_t Ra;
	uint8_t Rb;
	uint8_t Rc;
	uint8_t Rd;
	uint8_t Ix;
	int16_t Px;
	int8_t RUNindex[3];
	int8_t RUNindex_prev[3];
	uint8_t RUNval;
	uint16_t RUNcnt;
	uint8_t RItype;
	int8_t SIGN;
	uint16_t TEMP;
	int16_t Errval;
	uint16_t EMErrval;
	uint16_t MErrval;
	uint16_t q;
	uint8_t k;
	uint8_t map;
	uint8_t EOline[3];		
	point p[3];
	int8_t MAX_C;
	int8_t MIN_C;
	uint8_t RESET;	
	uint8_t T1;
	uint8_t T2;
	uint8_t T3;		
	uint32_t x;
	uint8_t b;
	uint8_t d;
	uint16_t max;
	uint32_t l;
	uint8_t n;
	uint8_t m;
	uint16_t* symbols;
	uint16_t* fs;
	uint16_t* cs;
	uint16_t* errors_calculated;
	uint32_t errors_count;
	uint8_t coding_mode;		
} 
encoder;

uint8_t* Encode(uint8_t* data, uint16_t w, uint16_t h, uint32_t* size, uint8_t coding_mode, uint8_t pixels_modify);

#endif
