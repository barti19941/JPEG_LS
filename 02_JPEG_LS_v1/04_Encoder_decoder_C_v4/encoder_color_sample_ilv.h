#ifndef _ENCODER_COLOR_SAMPLE_ILV_H_  
#define _ENCODER_COLOR_SAMPLE_ILV_H_

#include "globals.h"

typedef struct 
{				
	char bitstream[BITSTREAM_LENGHT];														
	int A[367], B[365], C[365], N[367], Nn[367];
	int J[32];
	int D[COMPONENTS_COUNT][COMPONENTS_COUNT];
	int Q[COMPONENTS_COUNT];
	int Ra[COMPONENTS_COUNT];
	int Rb[COMPONENTS_COUNT];
	int Rc[COMPONENTS_COUNT];
	int Rd[COMPONENTS_COUNT];
	int Ix[COMPONENTS_COUNT];
	int Px;
	int RANGE, MAXVAL, qbpp, bpp, LIMIT;	
	int RUNval[COMPONENTS_COUNT];
	int RUNindex;
	int RUNindex_prev;
	int RUNcnt;
	int RItype;	
	int w, h;
	int map;
	int TEMP;
	int glimit;
	int EOline;		
	int MAX_C, MIN_C;
	int RESET;
	int NEAR;
	int T1, T2, T3;
	int SIGN;
	int Errval, EMErrval, MErrval;
	int q;
	int k;
	char*** raw_data;
	char*** extended_data;
	point x;						
} 
encoder_color_sample_ilv;

char* EncodeColorSampleIlv(char*** data, int w, int h, int* size);

#endif

