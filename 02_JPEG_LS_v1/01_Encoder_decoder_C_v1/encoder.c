#include "encoder.h"

static encoder enc;
static int current_write_byte;
static int write_position;
static int byte_count;

/**********************************************************************
	AppendToBitStream	:	This function appends the non-negative number 
							i in binary form to the encoded bitstream 
							using j bits.
							b - number
							n - bits to append
**********************************************************************/
static void AppendToBitStream(int b, int n)
{
   	if((b >> n) > 0 || b < 0)
	{
       	printf("ERROR\t%d bits are not enough to represent the number %d\n", n, b);
       	return;
   	}
   	while(n--)
    {
       	if(b & bit_set_mask[n]) 
	   	{
			current_write_byte |= bit_set_mask[write_position--];			
			if (write_position < 0) 
			{
				enc.bitstream[byte_count] = current_write_byte;
				byte_count++;
 				if(current_write_byte == 255)
 				{
 					write_position = 6;
				}
				else
				{
					write_position = 7;
				}
				current_write_byte = 0;
			}
		}
       	else 
		{
			write_position--;
			if(write_position < 0)
			{
				enc.bitstream[byte_count] = current_write_byte;
				byte_count++;
 				write_position = 7;
 				current_write_byte = 0;
 			}
		}
    }
}

/**********************************************************************
	GolombCoding	:	Golomb code function.
						n - value to encode,
						k - golomb code variable
						limit - number of bits to which the length of
						 a Golomb code word is limited 
**********************************************************************/
void GolombCoding(int n, int l, int limit)
{
 	int val1;
 	int val2;
 	int val3;
 	int val4;
 	val1 = n >> l;
 	val2 = val1 << l;
 	val3 = limit - enc.qbpp - 1;
   	if(val1 < val3)
	{
    	AppendToBitStream(0, val1);
    	AppendToBitStream(1, 1);
    	val4 = n - val2;
    	AppendToBitStream(val4, l);
   	}   	
  	else
	{
    	AppendToBitStream(0, val3);
    	AppendToBitStream(1, 1);
    	AppendToBitStream(n - 1, enc.qbpp);
   	} 
}

/**********************************************************************
	GetNextSample	:	This gets new sample from data and set appropriate
						values.
**********************************************************************/
static void GetNextSample()
{
	/* calculate x and y coordinate */
	if(enc.x.x == enc.w)
	{
    	enc.x.y++;
    	enc.x.x = 1;
	}
 	else
 	{
 		enc.x.x++;
	}
	/* calculate values of casual template */
	enc.Ix = (unsigned char)enc.extended_data[enc.x.y][enc.x.x];	
	enc.Ra = (unsigned char)enc.extended_data[enc.x.y][enc.x.x - 1];
	enc.Rb = (unsigned char)enc.extended_data[enc.x.y - 1][enc.x.x];
	enc.Rc = (unsigned char)enc.extended_data[enc.x.y - 1][enc.x.x - 1];
	enc.Rd = (unsigned char)enc.extended_data[enc.x.y - 1][enc.x.x + 1];

	/* check if end of line */
	if(enc.x.x == enc.w)
	{		
		enc.EOline = 1;
	}
	else
	{		
		enc.EOline = 0;
	}
}

/**********************************************************************
	RunModeProcessing	:	Run mode processing of samples.
**********************************************************************/
static void RunModeProcessing()
{
	/* run-length determination for run mode */
	enc.RUNval = enc.Ra;
 	enc.RUNcnt = 0;
 	while(enc.Ix == enc.RUNval)
	{
       	enc.RUNcnt += 1;
       	if(enc.EOline == 1)
       	{
    		break;
	   	}
       	else
       	{       		
        	GetNextSample();
		}
 	}
 	/* encoding of run segments of length rg */
 	while(enc.RUNcnt >= (1 << enc.J[enc.RUNindex]))
	{
    	AppendToBitStream(1, 1);
    	enc.RUNcnt -= (1 << enc.J[enc.RUNindex]);
    	if(enc.RUNindex < 31)
		{
			enc.RUNindex_prev = enc.RUNindex;
      		enc.RUNindex++;
    	}
 	}
 	/* encoding of run segments of length less than rg */
 	if(enc.Ra != enc.Ix)
	{
   		AppendToBitStream(0, 1);
   		AppendToBitStream(enc.RUNcnt, enc.J[enc.RUNindex]);
   		if(enc.RUNindex > 0)
		{
     		enc.RUNindex_prev = enc.RUNindex;
     		enc.RUNindex = enc.RUNindex - 1;
   		}
 	}
 	else if(enc.RUNcnt > 0)
 	{ 		
    	AppendToBitStream(1, 1);
	}
	/* run interruption sample encoding */
	if(enc.Ra != enc.Ix)
	{
		/* index computation */	
  		if(enc.Ra == enc.Rb)
  		{
  			enc.RItype = 1;
		}
    	else
    	{    		
    		enc.RItype = 0;
		}
		/* prediction error for a run interruption sample */
		if(enc.RItype == 1)
		{
			enc.Errval = enc.Ix - enc.Ra;	
		}    
  		else
  		{
  			enc.Errval = enc.Ix - enc.Rb;
		}
    	/* error computation for a run interruption sample */
		if((enc.RItype == 0) && (enc.Ra > enc.Rb))
		{
   			enc.Errval = -enc.Errval;
   			enc.SIGN = -1;
 		}
		else
		{			
   			enc.SIGN = 1;
		}	
		enc.Errval = ModRange(enc.Errval, enc.RANGE);
		/* computation of the auxiliary variable TEMP */
		if (enc.RItype == 0)
		{			
			enc.TEMP = enc.A[365];
		}
		else
		{
			enc.TEMP = enc.A[366] + (enc.N[366] >> 1);
		}
		/* Q computation */
		enc.q = enc.RItype + 365; 
		/* k computation */
		for(enc.k = 0; (enc.N[enc.q] << enc.k) < enc.TEMP; enc.k++);
		/* computation of map for Errval mapping */
		if((enc.k == 0) && (enc.Errval > 0) && (2*enc.Nn[enc.q] < enc.N[enc.q]))
		{
			enc.map = 1;
		}
		else if((enc.Errval < 0) && (2 * enc.Nn[enc.q] >= enc.N[enc.q]))
		{
			enc.map = 1;
		}
  		else if((enc.Errval < 0) && (enc.k != 0))
  		{
  			enc.map = 1;
		}
		else 
		{
			enc.map = 0;
		}
		/* errval mapping for run interruption sample */
		enc.EMErrval = 2 * (ABS(enc.Errval)) - enc.RItype - enc.map;
		/* encoder EMErrval */
		enc.glimit = enc.LIMIT - enc.J[enc.RUNindex_prev] - 1;
  		GolombCoding(enc.EMErrval, enc.k, enc.glimit); 
  		/* update of parameters for run interruption sample */
  		if(enc.Errval < 0)
  		{  			
   			enc.Nn[enc.q] = enc.Nn[enc.q] + 1;
		}
	 	enc.A[enc.q] = enc.A[enc.q] + ((enc.EMErrval + 1 - enc.RItype) >> 1);
  		if(enc.N[enc.q] == enc.RESET)
		{
       		enc.A[enc.q] = enc.A[enc.q] >> 1;
       		enc.N[enc.q] = enc.N[enc.q] >> 1;
       		enc.Nn[enc.q] = enc.Nn[enc.q] >> 1;
  		}
  		enc.N[enc.q] = enc.N[enc.q] + 1;
 	}
}

/**********************************************************************
	RegularModeProcessing	:	Regural mode processing of samples.
**********************************************************************/
static void RegularModeProcessing()
{
	int i;
	/* quantization of the gradients */
	for(i = 0; i < 3; i++)
	{
		if(enc.D[i] <= -enc.T3)
		{
			enc.Q[i] = -4;	
		} 
		else if(enc.D[i] <= -enc.T2) 
		{
			enc.Q[i] = -3;	
		}
		else if(enc.D[i] <= -enc.T1) 
		{
			enc.Q[i] = -2;	
		}
		else if(enc.D[i] < -enc.NEAR )
		{
			enc.Q[i] = -1;	
		} 
		else if(enc.D[i] <= enc.NEAR) 
		{
			enc.Q[i] = 0;	
		}
		else if(enc.D[i] < enc.T1)
		{
			enc.Q[i] = 1;	
		} 
		else if(enc.D[i] < enc.T2) 
		{
			enc.Q[i] = 2;	
		}
		else if(enc.D[i] < enc.T3)
		{
			enc.Q[i] = 3;
		} 
		else
		{
			enc.Q[i] = 4;
		} 
	}
	/* quantized gradient merging and q mapping*/
	if(enc.Q[0] < 0)
	{
	    enc.Q[0] = -enc.Q[0];
	    enc.Q[1] = -enc.Q[1];
	    enc.Q[2] = -enc.Q[2];
	   	enc.SIGN = -1;
	}
  	else if(enc.Q[0] == 0)
	{
        if(enc.Q[1] < 0)
		{
            enc.Q[1] = -enc.Q[1];
        	enc.Q[2] = -enc.Q[2];
        	enc.SIGN = -1;
		}
        else if(enc.Q[1] == 0 && enc.Q[2] < 0)
		{
        	enc.Q[2] = -enc.Q[2];
            enc.SIGN = -1;
		}
        else
		{
			enc.SIGN = 1;	
		} 
	}
  	else 
	{
		enc.SIGN = 1;	
	}
	enc.q = 81 * enc.Q[0] + 9 * enc.Q[1] + enc.Q[2];
	/* edge-detecting predictor */
	if(enc.Rc >= MAX(enc.Ra, enc.Rb))
	{
		enc.Px = MIN(enc.Ra, enc.Rb);	
	}   		
 	else 
	{
   		if(enc.Rc <= MIN(enc.Ra, enc.Rb))
   		{   			
     		enc.Px = MAX(enc.Ra, enc.Rb);
		}
   		else
   		{   			
     		enc.Px = enc.Ra + enc.Rb - enc.Rc;
		}
  	}
  	/* prediction correction from the bias */
  	if(enc.SIGN == 1)
  	{
  		enc.Px = enc.Px + enc.C[enc.q];	
	}   		
 	else
 	{
		enc.Px = enc.Px - enc.C[enc.q];
	}
 	if(enc.Px > enc.MAXVAL)
 	{
 		enc.Px = enc.MAXVAL;
	}   		
 	else if(enc.Px < 0)
 	{
 		enc.Px = 0;
	}
	/* computation of prediction error */
	enc.Errval = enc.Ix - enc.Px;
 	if (enc.SIGN == -1)
 	{ 		
    	enc.Errval = -enc.Errval;
	}
    /* modulo reduction of the prediction error */
    enc.Errval = ModRange(enc.Errval, enc.RANGE);
    /* computation of the Golomg coding variable k */
    for(enc.k = 0; (enc.N[enc.q] << enc.k) < enc.A[enc.q]; enc.k++);
    /* error mapping to non-negative values */
    if((enc.NEAR == 0) && (enc.k==0) && (2 * enc.B[enc.q] <= -enc.N[enc.q]))
	{    
    	if(enc.Errval >= 0)
    	{
    		enc.MErrval = 2 * enc.Errval +1;
		}       
    	else
    	{
    		enc.MErrval = -2 * (enc.Errval + 1);	
		}       
  	}
 	else
	{
    	if(enc.Errval >= 0)
    	{
    		enc.MErrval = 2 * enc.Errval;
		}       		
    	else
    	{
    		enc.MErrval = -2 * enc.Errval - 1;	
		}
       	
  	}
  	/* mapped-error encoding */
  	GolombCoding(enc.MErrval, enc.k, enc.LIMIT);
  	/* variables update */
  	enc.B[enc.q] = enc.B[enc.q] + enc.Errval * (2 * enc.NEAR + 1);
	enc.A[enc.q] = enc.A[enc.q] + ABS(enc.Errval);
	if (enc.N[enc.q] == enc.RESET) 
	{
		enc.A[enc.q] = enc.A[enc.q] >> 1;
		if (enc.B[enc.q] >= 0)
		{
			enc.B[enc.q] = enc.B[enc.q] >> 1;
		}			
		else
		{
			enc.B[enc.q] = -((1-enc.B[enc.q]) >> 1);
		}			
		enc.N[enc.q] = enc.N[enc.q] >> 1;
	}
	enc.N[enc.q] = enc.N[enc.q] + 1;
	/* update of bias-related variables B[Q] and C[Q] */
	if(enc.B[enc.q] <= -enc.N[enc.q])
	{
  		enc.B[enc.q] = enc.B[enc.q] + enc.N[enc.q];
   		if(enc.C[enc.q] > enc.MIN_C)
   		{
   			enc.C[enc.q] = enc.C[enc.q] - 1;
		}     		
   		if(enc.B[enc.q] <= -enc.N[enc.q])
   		{   			
     		enc.B[enc.q] = -enc.N[enc.q] + 1;
		}
 	}
 	else if(enc.B[enc.q] > 0) 
	{
   		enc.B[enc.q] = enc.B[enc.q] - enc.N[enc.q];
   		if(enc.C[enc.q] < enc.MAX_C)
   		{
     		enc.C[enc.q] = enc.C[enc.q] + 1;
		}
   		if(enc.B[enc.q] > 0)
   		{   			
    		enc.B[enc.q] = 0;
		}
 	}
}

/**********************************************************************
	InitializeNonDefinedSamples	:	This function initialize non
						defined samples of casual template.
						data - pointer to input data
						w - width of input data
						h - height of input data
						return pointer to extended data
**********************************************************************/
static char** InitializeNonDefinedSamples(char** data, int w, int h)
{
	int i, j;
	char** rdata;
	/* allocate data for new array (with non defined samples) */
	rdata = Allocate2D(w + 2,h + 1);
	/* append raw data to extended data (with non defined samples) */
	for (i = 0; i < h; i++)
	{
		for (j = 0; j < w; j++)
		{
			rdata[i + 1][j + 1] = data[i][j];
		}
	}
	/* set data for first row */
	for(i = 0; i < w + 2; i++)
    {    	
		rdata[0][i] = 0;
	}
	/* set first and last column */
   	for(i = 1; i < h + 1; i++)
	{
     	rdata[i][0] = rdata[i - 1][1];
      	rdata[i][w + 2 - 1] = rdata[i][w + 2 -2];
	}
	return rdata;
}

/**********************************************************************
	Encode			:	This function encode raw data into jpegls
						comporessed data.
						data - pointer to input data
						w - width of input data
						h - height of input data
						size - size of output bitstream
						return NULL if cannot encode input data
						return pointer to compressed data
**********************************************************************/
char* Encode(char** data, int w, int h, int* size)
{
	int i, j;
	char *dout;
	if (data == NULL)
	{
		printf("ERROR\tWrong input data!\n");
		return NULL;
	}
	for (i = 0; i < BITSTREAM_LENGHT; i++)
	{
		enc.bitstream[i] = 0;
	}
	current_write_byte = 0;
   	write_position = 7;
   	byte_count = 0;
	/* initialisations of appropriate values */
	enc.RANGE = 256;
	enc.MAXVAL = 255;
	enc.qbpp = 8;
	enc.bpp = 8;
	enc.LIMIT = 32; 
	enc.w = w;
	enc.h = h;
	for(i = 0; i < 365; i++)
	{
    	enc.A[i] = 4;
      	enc.N[i] = 1;
      	enc.B[i] = 0;
      	enc.C[i] = 0;
      	enc.Nn[i] = 0;
	}
	enc.A[365] = 4;
    enc.A[366] = 4;
    enc.N[365] = 1;
    enc.N[366] = 1;
    enc.Nn[365] = 0;
    enc.Nn[366] = 0;
    enc.RUNindex = 0;    
    enc.x.x = 0;
    enc.x.y = 1;	
	enc.EOline = 0;
    enc.raw_data = data;
    enc.RESET = 64;
    enc.T1 = 3;
    enc.T2 = 7;
    enc.T3 = 21;
    enc.MAX_C = 127;
    enc.MIN_C = -128;
    enc.NEAR = 0;
    enc.J[0] = 0; enc.J[1] = 0; enc.J[2] = 0; enc.J[3] = 0; enc.J[4] = 1; enc.J[5] = 1; enc.J[6] = 1; enc.J[7] = 1; 
    enc.J[8] = 2; enc.J[9] = 2; enc.J[10] = 2; enc.J[11] = 2; enc.J[12] = 3; enc.J[13] = 3; enc.J[14] = 3; enc.J[15] = 3; 
    enc.J[16] = 4; enc.J[17] = 4; enc.J[18] = 5; enc.J[19] = 5; enc.J[20] = 6; enc.J[21] = 6; enc.J[22] = 7; enc.J[23] = 7;     
    enc.J[24] = 8; enc.J[25] = 9; enc.J[26] = 10; enc.J[27] = 11; enc.J[28] = 12; enc.J[29] = 13; enc.J[30] = 14; enc.J[31] = 15; 
    /* initialize non defined samples */
    enc.extended_data = InitializeNonDefinedSamples(enc.raw_data, w, h);
    /* for each sample */
    for(;;)
    {
    	/* check if all sample proceeded */
		if(enc.x.x == w && enc.x.y == h)
    	{    		
        	break;
		} 
		/* get next sample */ 
		GetNextSample();
		/* local gradient computation */
        enc.D[0] = enc.Rd - enc.Rb;
        enc.D[1] = enc.Rb - enc.Rc;
        enc.D[2] = enc.Rc - enc.Ra;
        /* mode selection */
     	if (enc.D[0] == 0 && enc.D[1] == 0 && enc.D[2] == 0)
     	{
      		RunModeProcessing();
		}
		else
		{			
      		RegularModeProcessing();
		}
	}
	/* save last byte if neccesary */
	if (write_position >= 0)
	{
		enc.bitstream[byte_count] = current_write_byte;
		byte_count = byte_count + 1;
	}
	/* allocate data for output bitstream */
	dout = Allocate1D(byte_count);
	if (dout == NULL)
	{
		printf("ERROR\tCan't allocate data for output bitstream!\n");
		return NULL;
	}
	/* rewrite bitstream */
	for (i = 0; i < byte_count; i++)
	{
		dout[i] = enc.bitstream[i];	
	}
	*size = byte_count;    	
	Free2D(enc.extended_data);
	return dout;
}

