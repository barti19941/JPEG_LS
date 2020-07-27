#ifndef _DECODER_H_  
#define _DECODER_H_

#include "globals.h"

typedef struct 
{				
	char bitstream[BITSTREAM_LENGHT];														
	int A[367], B[365], C[365], N[367], Nn[367];
	int J[32];
	int D[3];
	int Q[3];
	int Ra, Rb, Rc, Rd;
	int Rx;
	int Px;
	int RANGE, MAXVAL, qbpp, bpp, LIMIT;
	int RUNindex, RUNval, RUNcnt, RUNindex_prev;
	int RItype;	
	int w, h;
	int glimit;
	int MAX_C, MIN_C;
	int RESET;
	int NEAR;
	int T1, T2, T3;
	int SIGN;
	int TEMP;
	int Errval, EMErrval, MErrval;
	int q;
	int k;
	int map;
	char** raw_data;
	char** extended_data;
	point x;						
} 
decoder;

char** Decode(char* data, int w, int h, int size);

#endif
