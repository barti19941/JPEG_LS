#include "encoder_gray.h"

static encoder_gray enc_gray;
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
				enc_gray.bitstream[byte_count] = current_write_byte;
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
				enc_gray.bitstream[byte_count] = current_write_byte;
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
static void GolombCoding(int n, int l, int limit)
{
 	int val1;
 	int val2;
 	int val3;
 	int val4;
 	val1 = n >> l;
 	val2 = val1 << l;
 	val3 = limit - enc_gray.qbpp - 1;
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
    	AppendToBitStream(n - 1, enc_gray.qbpp);
   	} 
}

/**********************************************************************
	GetNextSample	:	This gets new sample from data and set appropriate
						values.
**********************************************************************/
static void GetNextSample()
{
	/* calculate x and y coordinate */
	if(enc_gray.x.x == enc_gray.w)
	{
    	enc_gray.x.y++;
    	enc_gray.x.x = 1;
	}
 	else
 	{
 		enc_gray.x.x++;
	}
	/* calculate values of casual template */
	enc_gray.Ix = (unsigned char)enc_gray.extended_data[enc_gray.x.y][enc_gray.x.x];	
	enc_gray.Ra = (unsigned char)enc_gray.extended_data[enc_gray.x.y][enc_gray.x.x - 1];
	enc_gray.Rb = (unsigned char)enc_gray.extended_data[enc_gray.x.y - 1][enc_gray.x.x];
	enc_gray.Rc = (unsigned char)enc_gray.extended_data[enc_gray.x.y - 1][enc_gray.x.x - 1];
	enc_gray.Rd = (unsigned char)enc_gray.extended_data[enc_gray.x.y - 1][enc_gray.x.x + 1];

	/* check if end of line */
	if(enc_gray.x.x == enc_gray.w)
	{		
		enc_gray.EOline = 1;
	}
	else
	{		
		enc_gray.EOline = 0;
	}
}

/**********************************************************************
	RunModeProcessing:	Run mode processing of samples.
**********************************************************************/
static void RunModeProcessing()
{
	/* run-length determination for run mode */
	enc_gray.RUNval = enc_gray.Ra;
 	enc_gray.RUNcnt = 0;
 	while(enc_gray.Ix == enc_gray.RUNval)
	{
       	enc_gray.RUNcnt += 1;
       	if(enc_gray.EOline == 1)
       	{
    		break;
	   	}
       	else
       	{       		
        	GetNextSample();
		}
 	}
 	/* encoding of run segments of length rg */
 	while(enc_gray.RUNcnt >= (1 << enc_gray.J[enc_gray.RUNindex]))
	{
    	AppendToBitStream(1, 1);
    	enc_gray.RUNcnt -= (1 << enc_gray.J[enc_gray.RUNindex]);
    	if(enc_gray.RUNindex < 31)
		{
			enc_gray.RUNindex_prev = enc_gray.RUNindex;
      		enc_gray.RUNindex++;
    	}
 	}
 	/* encoding of run segments of length less than rg */
 	if(enc_gray.Ra != enc_gray.Ix)
	{
   		AppendToBitStream(0, 1);
   		AppendToBitStream(enc_gray.RUNcnt, enc_gray.J[enc_gray.RUNindex]);
   		if(enc_gray.RUNindex > 0)
		{
     		enc_gray.RUNindex_prev = enc_gray.RUNindex;
     		enc_gray.RUNindex = enc_gray.RUNindex - 1;
   		}
 	}
 	else if(enc_gray.RUNcnt > 0)
 	{ 		
    	AppendToBitStream(1, 1);
	}
	/* run interruption sample encoding */
	if(enc_gray.Ra != enc_gray.Ix)
	{
		/* index computation */	
  		if(enc_gray.Ra == enc_gray.Rb)
  		{
  			enc_gray.RItype = 1;
		}
    	else
    	{    		
    		enc_gray.RItype = 0;
		}
		/* prediction error for a run interruption sample */
		if(enc_gray.RItype == 1)
		{
			enc_gray.Errval = enc_gray.Ix - enc_gray.Ra;	
		}    
  		else
  		{
  			enc_gray.Errval = enc_gray.Ix - enc_gray.Rb;
		}
    	/* error computation for a run interruption sample */
		if((enc_gray.RItype == 0) && (enc_gray.Ra > enc_gray.Rb))
		{
   			enc_gray.Errval = -enc_gray.Errval;
   			enc_gray.SIGN = -1;
 		}
		else
		{			
   			enc_gray.SIGN = 1;
		}	
		enc_gray.Errval = ModRange(enc_gray.Errval, enc_gray.RANGE);
		/* computation of the auxiliary variable TEMP */
		if (enc_gray.RItype == 0)
		{			
			enc_gray.TEMP = enc_gray.A[365];
		}
		else
		{
			enc_gray.TEMP = enc_gray.A[366] + (enc_gray.N[366] >> 1);
		}
		/* Q computation */
		enc_gray.q = enc_gray.RItype + 365; 
		/* k computation */
		for(enc_gray.k = 0; (enc_gray.N[enc_gray.q] << enc_gray.k) < enc_gray.TEMP; enc_gray.k++);
		/* computation of map for Errval mapping */
		if((enc_gray.k == 0) && (enc_gray.Errval > 0) && (2*enc_gray.Nn[enc_gray.q] < enc_gray.N[enc_gray.q]))
		{
			enc_gray.map = 1;
		}
		else if((enc_gray.Errval < 0) && (2 * enc_gray.Nn[enc_gray.q] >= enc_gray.N[enc_gray.q]))
		{
			enc_gray.map = 1;
		}
  		else if((enc_gray.Errval < 0) && (enc_gray.k != 0))
  		{
  			enc_gray.map = 1;
		}
		else 
		{
			enc_gray.map = 0;
		}
		/* errval mapping for run interruption sample */
		enc_gray.EMErrval = 2 * (ABS(enc_gray.Errval)) - enc_gray.RItype - enc_gray.map;
		/* encoder EMErrval */
		enc_gray.glimit = enc_gray.LIMIT - enc_gray.J[enc_gray.RUNindex_prev] - 1;
  		GolombCoding(enc_gray.EMErrval, enc_gray.k, enc_gray.glimit); 
  		/* update of parameters for run interruption sample */
  		if(enc_gray.Errval < 0)
  		{  			
   			enc_gray.Nn[enc_gray.q] = enc_gray.Nn[enc_gray.q] + 1;
		}
	 	enc_gray.A[enc_gray.q] = enc_gray.A[enc_gray.q] + ((enc_gray.EMErrval + 1 - enc_gray.RItype) >> 1);
  		if(enc_gray.N[enc_gray.q] == enc_gray.RESET)
		{
       		enc_gray.A[enc_gray.q] = enc_gray.A[enc_gray.q] >> 1;
       		enc_gray.N[enc_gray.q] = enc_gray.N[enc_gray.q] >> 1;
       		enc_gray.Nn[enc_gray.q] = enc_gray.Nn[enc_gray.q] >> 1;
  		}
  		enc_gray.N[enc_gray.q] = enc_gray.N[enc_gray.q] + 1;
 	}
}

/**********************************************************************
	Prediction		:	This function perform prediction.
**********************************************************************/
static void Prediction()
{
	int i;
	/* quantization of the gradients */
	for(i = 0; i < 3; i++)
	{
		if(enc_gray.D[i] <= -enc_gray.T3)
		{
			enc_gray.Q[i] = -4;	
		} 
		else if(enc_gray.D[i] <= -enc_gray.T2) 
		{
			enc_gray.Q[i] = -3;	
		}
		else if(enc_gray.D[i] <= -enc_gray.T1) 
		{
			enc_gray.Q[i] = -2;	
		}
		else if(enc_gray.D[i] < -enc_gray.NEAR )
		{
			enc_gray.Q[i] = -1;	
		} 
		else if(enc_gray.D[i] <= enc_gray.NEAR) 
		{
			enc_gray.Q[i] = 0;	
		}
		else if(enc_gray.D[i] < enc_gray.T1)
		{
			enc_gray.Q[i] = 1;	
		} 
		else if(enc_gray.D[i] < enc_gray.T2) 
		{
			enc_gray.Q[i] = 2;	
		}
		else if(enc_gray.D[i] < enc_gray.T3)
		{
			enc_gray.Q[i] = 3;
		} 
		else
		{
			enc_gray.Q[i] = 4;
		} 
	}
	/* quantized gradient merging and q mapping*/
	if(enc_gray.Q[0] < 0)
	{
	    enc_gray.Q[0] = -enc_gray.Q[0];
	    enc_gray.Q[1] = -enc_gray.Q[1];
	    enc_gray.Q[2] = -enc_gray.Q[2];
	   	enc_gray.SIGN = -1;
	}
  	else if(enc_gray.Q[0] == 0)
	{
        if(enc_gray.Q[1] < 0)
		{
            enc_gray.Q[1] = -enc_gray.Q[1];
        	enc_gray.Q[2] = -enc_gray.Q[2];
        	enc_gray.SIGN = -1;
		}
        else if(enc_gray.Q[1] == 0 && enc_gray.Q[2] < 0)
		{
        	enc_gray.Q[2] = -enc_gray.Q[2];
            enc_gray.SIGN = -1;
		}
        else
		{
			enc_gray.SIGN = 1;	
		} 
	}
  	else 
	{
		enc_gray.SIGN = 1;	
	}
	enc_gray.q = 81 * enc_gray.Q[0] + 9 * enc_gray.Q[1] + enc_gray.Q[2];
	/* edge-detecting predictor */
	if(enc_gray.Rc >= MAX(enc_gray.Ra, enc_gray.Rb))
	{
		enc_gray.Px = MIN(enc_gray.Ra, enc_gray.Rb);	
	}   		
 	else 
	{
   		if(enc_gray.Rc <= MIN(enc_gray.Ra, enc_gray.Rb))
   		{   			
     		enc_gray.Px = MAX(enc_gray.Ra, enc_gray.Rb);
		}
   		else
   		{   			
     		enc_gray.Px = enc_gray.Ra + enc_gray.Rb - enc_gray.Rc;
		}
  	}
  	/* prediction correction from the bias */
  	if(enc_gray.SIGN == 1)
  	{
  		enc_gray.Px = enc_gray.Px + enc_gray.C[enc_gray.q];	
	}   		
 	else
 	{
		enc_gray.Px = enc_gray.Px - enc_gray.C[enc_gray.q];
	}
 	if(enc_gray.Px > enc_gray.MAXVAL)
 	{
 		enc_gray.Px = enc_gray.MAXVAL;
	}   		
 	else if(enc_gray.Px < 0)
 	{
 		enc_gray.Px = 0;
	}
	/* computation of prediction error */
	enc_gray.Errval = enc_gray.Ix - enc_gray.Px;
 	if (enc_gray.SIGN == -1)
 	{ 		
    	enc_gray.Errval = -enc_gray.Errval;
	}
    /* modulo reduction of the prediction error */
    enc_gray.Errval = ModRange(enc_gray.Errval, enc_gray.RANGE);
}

/**********************************************************************
	PredictionErrorEncoding	:	This function perform prediction
								error encoding.
**********************************************************************/
static void PredictionErrorEncoding()
{
	/* computation of the Golomg coding variable k */
    for(enc_gray.k = 0; (enc_gray.N[enc_gray.q] << enc_gray.k) < enc_gray.A[enc_gray.q]; enc_gray.k++);
    /* error mapping to non-negative values */
    if((enc_gray.NEAR == 0) && (enc_gray.k==0) && (2 * enc_gray.B[enc_gray.q] <= -enc_gray.N[enc_gray.q]))
	{    
    	if(enc_gray.Errval >= 0)
    	{
    		enc_gray.MErrval = 2 * enc_gray.Errval + 1;
		}       
    	else
    	{
    		enc_gray.MErrval = -2 * (enc_gray.Errval + 1);	
		}       
  	}
 	else
	{
    	if(enc_gray.Errval >= 0)
    	{
    		enc_gray.MErrval = 2 * enc_gray.Errval;
		}       		
    	else
    	{
    		enc_gray.MErrval = -2 * enc_gray.Errval - 1;	
		}
       	
  	}
  	/* mapped-error encoding */
  	GolombCoding(enc_gray.MErrval, enc_gray.k, enc_gray.LIMIT);
}

/**********************************************************************
	VariablesUpdate			:	This function performs variables update.
**********************************************************************/
static void VariablesUpdate()
{
	/* variables update */
  	enc_gray.B[enc_gray.q] = enc_gray.B[enc_gray.q] + enc_gray.Errval * (2 * enc_gray.NEAR + 1);
	enc_gray.A[enc_gray.q] = enc_gray.A[enc_gray.q] + ABS(enc_gray.Errval);
	if (enc_gray.N[enc_gray.q] == enc_gray.RESET) 
	{
		enc_gray.A[enc_gray.q] = enc_gray.A[enc_gray.q] >> 1;
		if (enc_gray.B[enc_gray.q] >= 0)
		{
			enc_gray.B[enc_gray.q] = enc_gray.B[enc_gray.q] >> 1;
		}			
		else
		{
			enc_gray.B[enc_gray.q] = -((1-enc_gray.B[enc_gray.q]) >> 1);
		}			
		enc_gray.N[enc_gray.q] = enc_gray.N[enc_gray.q] >> 1;
	}
	enc_gray.N[enc_gray.q] = enc_gray.N[enc_gray.q] + 1;
	/* update of bias-related variables B[Q] and C[Q] */
	if(enc_gray.B[enc_gray.q] <= -enc_gray.N[enc_gray.q])
	{
  		enc_gray.B[enc_gray.q] = enc_gray.B[enc_gray.q] + enc_gray.N[enc_gray.q];
   		if(enc_gray.C[enc_gray.q] > enc_gray.MIN_C)
   		{
   			enc_gray.C[enc_gray.q] = enc_gray.C[enc_gray.q] - 1;
		}     		
   		if(enc_gray.B[enc_gray.q] <= -enc_gray.N[enc_gray.q])
   		{   			
     		enc_gray.B[enc_gray.q] = -enc_gray.N[enc_gray.q] + 1;
		}
 	}
 	else if(enc_gray.B[enc_gray.q] > 0) 
	{
   		enc_gray.B[enc_gray.q] = enc_gray.B[enc_gray.q] - enc_gray.N[enc_gray.q];
   		if(enc_gray.C[enc_gray.q] < enc_gray.MAX_C)
   		{
     		enc_gray.C[enc_gray.q] = enc_gray.C[enc_gray.q] + 1;
		}
   		if(enc_gray.B[enc_gray.q] > 0)
   		{   			
    		enc_gray.B[enc_gray.q] = 0;
		}
 	}
}

/**********************************************************************
	RegularModeProcessing	:	Regural mode processing of samples.
**********************************************************************/
static void RegularModeProcessing()
{
	/* Prediction */
	Prediction();
	/* Prediction error encoding */
	PredictionErrorEncoding();
	/* Variables update */
	VariablesUpdate();
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
	Initializations	:	This function initialize basic parameters
						of encoder as well as auxillary variables.
						data - pointer to input data
						w - width of input data
						h - height of input data
						size - size of output bitstream
**********************************************************************/
static void Initializations(char** data, int w, int h)
{
	int i;
	/* clear bitstream */
	for (i = 0; i < BITSTREAM_LENGHT; i++)
	{
		enc_gray.bitstream[i] = 0;
	}
	/* assign auxillary values used in AppendToBitStream function */
	current_write_byte = 0;
   	write_position = 7;
   	byte_count = 0;
	/* initialisations of appropriate values */	
    enc_gray.raw_data = data;
	enc_gray.RANGE = 256;
	enc_gray.MAXVAL = 255;
	enc_gray.qbpp = 8;
	enc_gray.bpp = 8;
	enc_gray.LIMIT = 32; 
	enc_gray.w = w;
	enc_gray.h = h;
	for(i = 0; i < 365; i++)
	{
    	enc_gray.A[i] = 4;
      	enc_gray.N[i] = 1;
      	enc_gray.B[i] = 0;
      	enc_gray.C[i] = 0;
      	enc_gray.Nn[i] = 0;
	}
	for(i = 0; i < COMPONENTS_COUNT; i++)
	{
		enc_gray.D[i] = 0;
		enc_gray.Q[i] = 0;
	}
	enc_gray.Ra = 0;
	enc_gray.Rb = 0;
	enc_gray.Rc = 0;
	enc_gray.Rd = 0;
	enc_gray.Ix = 0;
	enc_gray.Px = 0;
	enc_gray.A[365] = 4;
    enc_gray.A[366] = 4;
    enc_gray.N[365] = 1;
    enc_gray.N[366] = 1;
    enc_gray.Nn[365] = 0;
    enc_gray.Nn[366] = 0;
    enc_gray.RUNindex = 0;    
    enc_gray.RUNval = 0;  
	enc_gray.RUNcnt = 0;    
	enc_gray.RUNindex_prev = 0;  
	enc_gray.RItype = 0;  
	enc_gray.glimit = 0;
	enc_gray.SIGN = 0;
	enc_gray.TEMP = 0;
	enc_gray.Errval = 0;
	enc_gray.EMErrval = 0;
	enc_gray.MErrval;
	enc_gray.q = 0;
	enc_gray.k = 0;
	enc_gray.map = 0;	
    enc_gray.x.x = 0;
    enc_gray.x.y = 1;	
	enc_gray.EOline = 0;
    enc_gray.RESET = 64;
    enc_gray.T1 = 3;
    enc_gray.T2 = 7;
    enc_gray.T3 = 21;
    enc_gray.MAX_C = 127;
    enc_gray.MIN_C = -128;
    enc_gray.NEAR = 0;
    enc_gray.J[0] = 0; enc_gray.J[1] = 0; enc_gray.J[2] = 0; enc_gray.J[3] = 0; enc_gray.J[4] = 1; enc_gray.J[5] = 1; enc_gray.J[6] = 1; enc_gray.J[7] = 1; 
    enc_gray.J[8] = 2; enc_gray.J[9] = 2; enc_gray.J[10] = 2; enc_gray.J[11] = 2; enc_gray.J[12] = 3; enc_gray.J[13] = 3; enc_gray.J[14] = 3; enc_gray.J[15] = 3; 
    enc_gray.J[16] = 4; enc_gray.J[17] = 4; enc_gray.J[18] = 5; enc_gray.J[19] = 5; enc_gray.J[20] = 6; enc_gray.J[21] = 6; enc_gray.J[22] = 7; enc_gray.J[23] = 7;     
    enc_gray.J[24] = 8; enc_gray.J[25] = 9; enc_gray.J[26] = 10; enc_gray.J[27] = 11; enc_gray.J[28] = 12; enc_gray.J[29] = 13; enc_gray.J[30] = 14; enc_gray.J[31] = 15; 
    /* initialize non defined samples */
    enc_gray.extended_data = InitializeNonDefinedSamples(enc_gray.raw_data, w, h);
}

/**********************************************************************
	ContextModeling	:	This function performs context modelling 
						procedure.
**********************************************************************/
static int ContextModeling()
{
	/* local gradient computation */
    enc_gray.D[0] = enc_gray.Rd - enc_gray.Rb;
    enc_gray.D[1] = enc_gray.Rb - enc_gray.Rc;
    enc_gray.D[2] = enc_gray.Rc - enc_gray.Ra;
    /* mode selection */
 	if (enc_gray.D[0] == 0 && enc_gray.D[1] == 0 && enc_gray.D[2] == 0)
 	{
  		return 1;
	}
	else
	{			
  		return 0;
	}
}

/**********************************************************************
	EncodeGray		:	This function encode gray raw data into jpegls
						comporessed data.
						data - pointer to 2D input data
						w - width of input data
						h - height of input data
						size - size of output bitstream
						return NULL if cannot encode input data
						return pointer to compressed data
**********************************************************************/
char* EncodeGray(char** data, int w, int h, int* size)
{
	int i;
	char *dout;
	/* Initializations */
	Initializations(data, w, h);
    /* for each sample */
    for(;;)
    {
    	/* check if all sample proceeded */
		if(enc_gray.x.x == w && enc_gray.x.y == h)
    	{    		
        	break;
		} 
		/* get next sample */ 
		GetNextSample();
		/* Prediction */
		if (ContextModeling())
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
		enc_gray.bitstream[byte_count] = current_write_byte;
		byte_count = byte_count + 1;
	}
	/* allocate data for output bitstream */
	dout = Allocate1D(byte_count);
	/* rewrite bitstream */
	for (i = 0; i < byte_count; i++)
	{
		dout[i] = enc_gray.bitstream[i];	
	}
	*size = byte_count;    	
	Free2D(enc_gray.extended_data);
	enc_gray.extended_data = NULL;
	enc_gray.raw_data = NULL;
	return dout;
}



