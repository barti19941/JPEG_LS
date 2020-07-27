#ifndef _ENCODER_COLOR_NO_ILV_H_  
#define _ENCODER_COLOR_NO_ILV_H_

#include "globals.h"

typedef struct 
{				
	char* bitstream;														
	int A[367], B[365], C[365], N[367], Nn[367];
	int J[32];
	int D[COMPONENTS_COUNT];
	int Q[COMPONENTS_COUNT];
	int Ra, Rb, Rc, Rd;
	int Ix;
	int Px;
	int RANGE, MAXVAL, qbpp, bpp, LIMIT;
	int RUNindex;
	int RUNindex_prev;
	int RUNval, RUNcnt;
	int RItype;
	int EOline;	
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
encoder_color_no_ilv;

char** EncodeColorNoIlv(char*** data, int w, int h, int* size);

#endif

