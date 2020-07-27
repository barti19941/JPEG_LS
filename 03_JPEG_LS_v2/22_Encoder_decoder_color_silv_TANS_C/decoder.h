#ifndef _DECODER_H_  
#define _DECODER_H_

#include "globals.h"

typedef struct 
{	
	uint8_t* bitstream;	
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
	int16_t D[3][3];
	int8_t Q[3];	
	uint8_t Ra[3];
	uint8_t Rb[3];
	uint8_t Rc[3];
	uint8_t Rd[3];
	int16_t Rx;
	int16_t Px;
	uint8_t RItype;	
	int8_t SIGN;
	uint16_t TEMP;
	int16_t Errval;
	uint16_t EMErrval;
	uint16_t MErrval;	
	uint16_t q;
	uint8_t k;
	uint8_t map;	
	point p;
	int8_t MAX_C;
	int8_t MIN_C;
	uint8_t RESET;	
	uint8_t T1;
	uint8_t T2;
	uint8_t T3;	
	uint32_t L;	
	uint16_t max;
	uint16_t step;
	uint8_t R;
	uint8_t r;
	uint16_t* s;
	uint16_t* Ls;
	uint16_t* symbols;
	uint8_t* m;
	uint32_t* nb;
	uint16_t* next;
	t* decodingTable;
	uint32_t x;			
	uint8_t coding_mode;
} 
decoder;

uint8_t* Decode(uint8_t* data, uint16_t w, uint16_t h, uint32_t size, uint8_t coding_mode);

#endif
