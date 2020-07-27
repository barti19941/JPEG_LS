#include "encoder_color_sample_ilv.h"

static encoder_color_sample_ilv enc_color;
static int current_write_byte;
static int write_position;
static int byte_count;

/**********************************************************************
	GetNextSample	:	This gets new sample from data and set appropriate
						values.
**********************************************************************/
static void GetNextSample()
{
	int i;
	/* calculate x and y coordinate */
	if(enc_color.x.x == enc_color.w)
	{
    	enc_color.x.y++;
    	enc_color.x.x = 1;
	}
 	else
 	{
 		enc_color.x.x++;
	}
	/* check if end of line and set current component if necessary */
	if(enc_color.x.x == enc_color.w)
	{		
		enc_color.EOline = 1;
	}
	else if (enc_color.EOline == 1)
	{			
		enc_color.EOline = 0;
	}
	/* calculate values of casual template */
	for(i = 0; i < COMPONENTS_COUNT; i++)
	{
		enc_color.Ix[i] = (unsigned char)enc_color.extended_data[i][enc_color.x.y][enc_color.x.x];	
		enc_color.Ra[i] = (unsigned char)enc_color.extended_data[i][enc_color.x.y][enc_color.x.x - 1];
		enc_color.Rb[i] = (unsigned char)enc_color.extended_data[i][enc_color.x.y - 1][enc_color.x.x];
		enc_color.Rc[i] = (unsigned char)enc_color.extended_data[i][enc_color.x.y - 1][enc_color.x.x - 1];
		enc_color.Rd[i] = (unsigned char)enc_color.extended_data[i][enc_color.x.y - 1][enc_color.x.x + 1];
	}

}

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
				enc_color.bitstream[byte_count] = current_write_byte;
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
				enc_color.bitstream[byte_count] = current_write_byte;
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
 	val3 = limit - enc_color.qbpp - 1;
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
    	AppendToBitStream(n - 1, enc_color.qbpp);
   	} 
}

/**********************************************************************
	RunModeProcessing:	Run mode processing of samples.
**********************************************************************/
static void RunModeProcessing()
{
	int i;
	/* run-length determination for run mode */
	for (i = 0; i < COMPONENTS_COUNT; i++)
	{
		enc_color.RUNval[i] = enc_color.Ra[i];
	}
 	enc_color.RUNcnt = 0;
 	while((enc_color.Ix[0] == enc_color.RUNval[0]) && (enc_color.Ix[1] == enc_color.RUNval[1]) && (enc_color.Ix[2] == enc_color.RUNval[2]))
	{
       	enc_color.RUNcnt += 1;
       	if(enc_color.EOline == 1)
       	{
    		break;
	   	}
       	else
       	{       		
        	GetNextSample();
		}
 	}
 	/* encoding of run segments of length rg */
 	while(enc_color.RUNcnt >= (1 << enc_color.J[enc_color.RUNindex]))
	{
    	AppendToBitStream(1, 1);
    	enc_color.RUNcnt -= (1 << enc_color.J[enc_color.RUNindex]);
    	if(enc_color.RUNindex < 31)
		{
			enc_color.RUNindex_prev = enc_color.RUNindex;
      		enc_color.RUNindex++;
    	}
 	}
 	/* encoding of run segments of length less than rg */
 	if((enc_color.Ra[0] != enc_color.Ix[0]) && (enc_color.Ra[1] != enc_color.Ix[1]) && (enc_color.Ra[2] != enc_color.Ix[2]))
	{
   		AppendToBitStream(0, 1);
   		AppendToBitStream(enc_color.RUNcnt, enc_color.J[enc_color.RUNindex]);
   		if(enc_color.RUNindex > 0)
		{
     		enc_color.RUNindex_prev = enc_color.RUNindex;
     		enc_color.RUNindex = enc_color.RUNindex - 1;
   		}
 	}
 	else if(enc_color.RUNcnt > 0)
 	{ 		
    	AppendToBitStream(1, 1);
	}
	for (i = 0; i < COMPONENTS_COUNT; i++)
	{
		/* run interruption sample encoding */
		if(enc_color.Ra[i] != enc_color.Ix[i])
		{
			/* prediction error for a run interruption sample */
  			enc_color.Errval = enc_color.Ix[i] - enc_color.Rb[i];
	    	/* error computation for a run interruption sample */
			if(enc_color.Ra[i] > enc_color.Rb[i])
			{
	   			enc_color.Errval = -enc_color.Errval;
	   			enc_color.SIGN = -1;
	 		}
			else
			{			
	   			enc_color.SIGN = 1;
			}	
			enc_color.Errval = ModRange(enc_color.Errval, enc_color.RANGE);
			/* computation of the auxiliary variable TEMP */		
			enc_color.TEMP = enc_color.A[365];
			/* Q computation */
			enc_color.q = 365; 
			/* k computation */
			for(enc_color.k = 0; (enc_color.N[enc_color.q] << enc_color.k) < enc_color.TEMP; enc_color.k++);
			/* computation of map for Errval mapping */
			if((enc_color.k == 0) && (enc_color.Errval > 0) && (2*enc_color.Nn[enc_color.q] < enc_color.N[enc_color.q]))
			{
				enc_color.map = 1;
			}
			else if((enc_color.Errval < 0) && (2 * enc_color.Nn[enc_color.q] >= enc_color.N[enc_color.q]))
			{
				enc_color.map = 1;
			}
	  		else if((enc_color.Errval < 0) && (enc_color.k != 0))
	  		{
	  			enc_color.map = 1;
			}
			else 
			{
				enc_color.map = 0;
			}
			/* errval mapping for run interruption sample */
			enc_color.EMErrval = 2 * (ABS(enc_color.Errval)) - enc_color.RItype - enc_color.map;
			/* encoder EMErrval */
			enc_color.glimit = enc_color.LIMIT - enc_color.J[enc_color.RUNindex_prev] - 1;
	  		GolombCoding(enc_color.EMErrval, enc_color.k, enc_color.glimit); 
	  		/* update of parameters for run interruption sample */
	  		if(enc_color.Errval < 0)
	  		{  			
	   			enc_color.Nn[enc_color.q] = enc_color.Nn[enc_color.q] + 1;
			}
		 	enc_color.A[enc_color.q] = enc_color.A[enc_color.q] + ((enc_color.EMErrval + 1 - enc_color.RItype) >> 1);
	  		if(enc_color.N[enc_color.q] == enc_color.RESET)
			{
	       		enc_color.A[enc_color.q] = enc_color.A[enc_color.q] >> 1;
	       		enc_color.N[enc_color.q] = enc_color.N[enc_color.q] >> 1;
	       		enc_color.Nn[enc_color.q] = enc_color.Nn[enc_color.q] >> 1;
	  		}
	  		enc_color.N[enc_color.q] = enc_color.N[enc_color.q] + 1;
	 	}
	}

}

/**********************************************************************
	VariablesUpdate			:	This function performs variables update.
**********************************************************************/
static void VariablesUpdate()
{
	/* variables update */
  	enc_color.B[enc_color.q] = enc_color.B[enc_color.q] + enc_color.Errval * (2 * enc_color.NEAR + 1);
	enc_color.A[enc_color.q] = enc_color.A[enc_color.q] + ABS(enc_color.Errval);
	if (enc_color.N[enc_color.q] == enc_color.RESET) 
	{
		enc_color.A[enc_color.q] = enc_color.A[enc_color.q] >> 1;
		if (enc_color.B[enc_color.q] >= 0)
		{
			enc_color.B[enc_color.q] = enc_color.B[enc_color.q] >> 1;
		}			
		else
		{
			enc_color.B[enc_color.q] = -((1-enc_color.B[enc_color.q]) >> 1);
		}			
		enc_color.N[enc_color.q] = enc_color.N[enc_color.q] >> 1;
	}
	enc_color.N[enc_color.q] = enc_color.N[enc_color.q] + 1;
	/* update of bias-related variables B[Q] and C[Q] */
	if(enc_color.B[enc_color.q] <= -enc_color.N[enc_color.q])
	{
  		enc_color.B[enc_color.q] = enc_color.B[enc_color.q] + enc_color.N[enc_color.q];
   		if(enc_color.C[enc_color.q] > enc_color.MIN_C)
   		{
   			enc_color.C[enc_color.q] = enc_color.C[enc_color.q] - 1;
		}     		
   		if(enc_color.B[enc_color.q] <= -enc_color.N[enc_color.q])
   		{   			
     		enc_color.B[enc_color.q] = -enc_color.N[enc_color.q] + 1;
		}
 	}
 	else if(enc_color.B[enc_color.q] > 0) 
	{
   		enc_color.B[enc_color.q] = enc_color.B[enc_color.q] - enc_color.N[enc_color.q];
   		if(enc_color.C[enc_color.q] < enc_color.MAX_C)
   		{
     		enc_color.C[enc_color.q] = enc_color.C[enc_color.q] + 1;
		}
   		if(enc_color.B[enc_color.q] > 0)
   		{   			
    		enc_color.B[enc_color.q] = 0;
		}
 	}
}

/**********************************************************************
	PredictionErrorEncoding	:	This function perform prediction
								error encoding.								
**********************************************************************/
static void PredictionErrorEncoding()
{
	/* computation of the Golomg coding variable k */
    for(enc_color.k = 0; (enc_color.N[enc_color.q] << enc_color.k) < enc_color.A[enc_color.q]; enc_color.k++);
    /* error mapping to non-negative values */
    if((enc_color.NEAR == 0) && (enc_color.k == 0) && (2 * enc_color.B[enc_color.q] <= -enc_color.N[enc_color.q]))
	{    
    	if(enc_color.Errval >= 0)
    	{
    		enc_color.MErrval = 2 * enc_color.Errval + 1;
		}       
    	else
    	{
    		enc_color.MErrval = -2 * (enc_color.Errval + 1);	
		}       
  	}
 	else
	{
    	if(enc_color.Errval >= 0)
    	{
    		enc_color.MErrval = 2 * enc_color.Errval;
		}       		
    	else
    	{
    		enc_color.MErrval = -2 * enc_color.Errval - 1;	
		}
       	
  	}
  	/* mapped-error encoding */
  	GolombCoding(enc_color.MErrval, enc_color.k, enc_color.LIMIT);
}

/**********************************************************************
	Prediction		:	This function perform prediction.
						cmp - current component
**********************************************************************/
static void Prediction(int cmp)
{
	int i;
	/* quantization of the gradients */
	for(i = 0; i < 3; i++)
	{
		if(enc_color.D[cmp][i] <= -enc_color.T3)
		{
			enc_color.Q[i] = -4;	
		} 
		else if(enc_color.D[cmp][i] <= -enc_color.T2) 
		{
			enc_color.Q[i] = -3;	
		}
		else if(enc_color.D[cmp][i] <= -enc_color.T1) 
		{
			enc_color.Q[i] = -2;	
		}
		else if(enc_color.D[cmp][i] < -enc_color.NEAR )
		{
			enc_color.Q[i] = -1;	
		} 
		else if(enc_color.D[cmp][i] <= enc_color.NEAR) 
		{
			enc_color.Q[i] = 0;	
		}
		else if(enc_color.D[cmp][i] < enc_color.T1)
		{
			enc_color.Q[i] = 1;	
		} 
		else if(enc_color.D[cmp][i] < enc_color.T2) 
		{
			enc_color.Q[i] = 2;	
		}
		else if(enc_color.D[cmp][i] < enc_color.T3)
		{
			enc_color.Q[i] = 3;
		} 
		else
		{
			enc_color.Q[i] = 4;
		} 
	}
	/* quantized gradient merging and q mapping*/
	if(enc_color.Q[0] < 0)
	{
	    enc_color.Q[0] = -enc_color.Q[0];
	    enc_color.Q[1] = -enc_color.Q[1];
	    enc_color.Q[2] = -enc_color.Q[2];
	   	enc_color.SIGN = -1;
	}
  	else if(enc_color.Q[0] == 0)
	{
        if(enc_color.Q[1] < 0)
		{
            enc_color.Q[1] = -enc_color.Q[1];
        	enc_color.Q[2] = -enc_color.Q[2];
        	enc_color.SIGN = -1;
		}
        else if(enc_color.Q[1] == 0 && enc_color.Q[2] < 0)
		{
        	enc_color.Q[2] = -enc_color.Q[2];
            enc_color.SIGN = -1;
		}
        else
		{
			enc_color.SIGN = 1;	
		} 
	}
  	else 
	{
		enc_color.SIGN = 1;	
	}
	enc_color.q = 81 * enc_color.Q[0] + 9 * enc_color.Q[1] + enc_color.Q[2];
	/* edge-detecting predictor */
	if(enc_color.Rc[cmp] >= MAX(enc_color.Ra[cmp], enc_color.Rb[cmp]))
	{
		enc_color.Px = MIN(enc_color.Ra[cmp], enc_color.Rb[cmp]);	
	}   		
 	else 
	{
   		if(enc_color.Rc[cmp] <= MIN(enc_color.Ra[cmp], enc_color.Rb[cmp]))
   		{   			
     		enc_color.Px = MAX(enc_color.Ra[cmp], enc_color.Rb[cmp]);
		}
   		else
   		{   			
     		enc_color.Px = enc_color.Ra[cmp] + enc_color.Rb[cmp] - enc_color.Rc[cmp];
		}
  	}
  	/* prediction correction from the bias */
  	if(enc_color.SIGN == 1)
  	{
  		enc_color.Px = enc_color.Px + enc_color.C[enc_color.q];	
	}   		
 	else
 	{
		enc_color.Px = enc_color.Px - enc_color.C[enc_color.q];
	}
 	if(enc_color.Px > enc_color.MAXVAL)
 	{
 		enc_color.Px = enc_color.MAXVAL;
	}   		
 	else if(enc_color.Px < 0)
 	{
 		enc_color.Px = 0;
	}
	/* computation of prediction error */
	enc_color.Errval = enc_color.Ix[cmp] - enc_color.Px;
 	if (enc_color.SIGN == -1)
 	{ 		
    	enc_color.Errval = -enc_color.Errval;
	}
    /* modulo reduction of the prediction error */
    enc_color.Errval = ModRange(enc_color.Errval, enc_color.RANGE);
}

/**********************************************************************
	RegularModeProcessing	:	Regural mode processing of samples.
**********************************************************************/
static void RegularModeProcessing()
{
	int i;
	for (i = 0; i < COMPONENTS_COUNT; i++)
	{		
		/* Prediction */
		Prediction(i);	
		/* Prediction error encoding */	
		PredictionErrorEncoding();
		/* Variables update */
		VariablesUpdate();
	}
}


/**********************************************************************
	ContextModeling	:	This function performs context modelling 
						procedure.
**********************************************************************/
static int ContextModeling()
{
	int i;
	/* local gradient computation */
	for (i = 0; i < COMPONENTS_COUNT; i++)
	{
		enc_color.D[0][i] = enc_color.Rd[i] - enc_color.Rb[i];
    	enc_color.D[1][i] = enc_color.Rb[i] - enc_color.Rc[i];
    	enc_color.D[2][i] = enc_color.Rc[i] - enc_color.Ra[i];
	}
    /* mode selection */
 	if (enc_color.D[0][0] == 0 && enc_color.D[0][1] == 0 && enc_color.D[0][2] == 0 && 
	 	enc_color.D[1][0] == 0 && enc_color.D[1][1] == 0 && enc_color.D[1][2] == 0 &&
		enc_color.D[2][0] == 0 && enc_color.D[2][1] == 0 && enc_color.D[2][2] == 0)
 	{
  		return 1;
	}
	else
	{			
  		return 0;
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
static char*** InitializeNonDefinedSamples(char*** data, int w, int h)
{
	int i, j;
	char*** dout;
	/* allocate data for each component (with non defined samples) */
	dout = Allocate3D(w + 2, h + 1);
	/* append raw red data to extended data (red, green, blue samples) */
	for (i = 0; i < h; i++)
	{
		for (j = 0; j < w; j++)
		{
			dout[0][i + 1][j + 1] = data[0][i][j];
			dout[1][i + 1][j + 1] = data[1][i][j];
			dout[2][i + 1][j + 1] = data[2][i][j];
		}
	}
	/* set data for first row  (red, green, blue data) */
	for(i = 0; i < w + 2; i++)
    {    	
		dout[0][0][i] = 0;
		dout[1][0][i] = 0;
		dout[2][0][i] = 0;
	}
	/* set first and last column (red, green, blue data) */
   	for(i = 1; i < h + 1; i++)
	{
     	dout[0][i][0] 			= 	dout[0][i - 1][1];
      	dout[0][i][w + 2 - 1] 	= 	dout[0][i][w + 2 - 2];
      	dout[1][i][0] 			= 	dout[1][i - 1][1];
      	dout[1][i][w + 2 - 1] 	= 	dout[1][i][w + 2 -2];
      	dout[2][i][0] 			= 	dout[2][i - 1][1];
      	dout[2][i][w + 2 - 1] 	= 	dout[2][i][w + 2 -2];
	}
	return dout;
}


/**********************************************************************
	Initializations	:	This function initialize basic parameters
						of encoder as well as auxillary variables.
						data - pointer to input data
						w - width of input data
						h - height of input data
						size - size of output bitstream
**********************************************************************/
static void Initializations(char*** data, int w, int h)
{
	int i;
	/* clear bitstream */
	for (i = 0; i < BITSTREAM_LENGHT; i++)
	{
		enc_color.bitstream[i] = 0;
	}
	/* assign auxillary values used in AppendToBitStream function */
	current_write_byte = 0;
   	write_position = 7;
   	byte_count = 0;
	/* initialisations of appropriate values */
	enc_color.raw_data = data;
	enc_color.RANGE = 256;
	enc_color.MAXVAL = 255;
	enc_color.qbpp = 8;
	enc_color.bpp = 8;
	enc_color.LIMIT = 32; 
	enc_color.RESET = 64;
	enc_color.T1 = 3;
    enc_color.T2 = 7;
    enc_color.T3 = 21;    
    enc_color.NEAR = 0;
    enc_color.MAX_C = 127;
    enc_color.MIN_C = -128;
    enc_color.J[0] = 0; enc_color.J[1] = 0; enc_color.J[2] = 0; enc_color.J[3] = 0; enc_color.J[4] = 1; enc_color.J[5] = 1; enc_color.J[6] = 1; enc_color.J[7] = 1; 
    enc_color.J[8] = 2; enc_color.J[9] = 2; enc_color.J[10] = 2; enc_color.J[11] = 2; enc_color.J[12] = 3; enc_color.J[13] = 3; enc_color.J[14] = 3; enc_color.J[15] = 3; 
    enc_color.J[16] = 4; enc_color.J[17] = 4; enc_color.J[18] = 5; enc_color.J[19] = 5; enc_color.J[20] = 6; enc_color.J[21] = 6; enc_color.J[22] = 7; enc_color.J[23] = 7;     
    enc_color.J[24] = 8; enc_color.J[25] = 9; enc_color.J[26] = 10; enc_color.J[27] = 11; enc_color.J[28] = 12; enc_color.J[29] = 13; enc_color.J[30] = 14; enc_color.J[31] = 15; 
	enc_color.w = w;
	enc_color.h = h;
	for(i = 0; i < 365; i++)
	{
    	enc_color.A[i] = 4;
      	enc_color.N[i] = 1;
      	enc_color.B[i] = 0;
      	enc_color.C[i] = 0;
      	enc_color.Nn[i] = 0;
	}
	for (i = 0; i < COMPONENTS_COUNT; i++)  
	{
		enc_color.Ra[i] = 0;
		enc_color.Rb[i] = 0;
		enc_color.Rc[i] = 0;
		enc_color.Rd[i] = 0;
		enc_color.Ix[i] = 0;
		enc_color.Q[i] = 0;
		enc_color.D[0][i] = 0;
		enc_color.D[1][i] = 0;
		enc_color.D[2][i] = 0;
		enc_color.RUNval[i] = 0;
	}	
	enc_color.x.x = 0;
    enc_color.x.y = 1;	
    enc_color.EOline = 0;
	enc_color.Px = 0;	
	enc_color.q = 0;
	enc_color.A[365] = 4;
    enc_color.A[366] = 4;
    enc_color.N[365] = 1;
    enc_color.N[366] = 1;
    enc_color.Nn[365] = 0;
    enc_color.Nn[366] = 0;  
    enc_color.RItype = 0;
	enc_color.glimit = 0;
	enc_color.TEMP = 0;
	enc_color.map = 0;	
	enc_color.SIGN = 0;
	enc_color.Errval = 0;
	enc_color.EMErrval = 0;
	enc_color.MErrval;
	enc_color.k = 0;
	enc_color.RUNcnt = 0;    
    enc_color.RUNindex = 0;
    enc_color.RUNindex_prev = 0;
    /* initialize non defined samples */
	enc_color.extended_data = InitializeNonDefinedSamples(enc_color.raw_data, w, h);
}

/**********************************************************************
	EncodeColorSampleIlv	:	This function encode color 3D raw data into jpegls
						comporessed data.
						data - pointer to 3D input data
						w - width of input data
						h - height of input data
						size - size of output bitstream
						return NULL if cannot encode input data
						return pointer to compressed data
**********************************************************************/
char* EncodeColorSampleIlv(char*** data, int w, int h, int* size)
{
	int i, j;
	char *dout;
	/* Initializations */
	Initializations(data, w, h);
	/* for each sample */
	for(;;)
    {
    	/* check if all sample proceeded */
		if(enc_color.x.x == w && enc_color.x.y == h)
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
		enc_color.bitstream[byte_count] = current_write_byte;
		byte_count = byte_count + 1;
	}
	/* allocate data for output bitstream */
	dout = Allocate1D(byte_count);
	/* rewrite bitstream */
	for (i = 0; i < byte_count; i++)
	{
		dout[i] = enc_color.bitstream[i];	
	}
	*size = byte_count;    	
	Free3D(enc_color.extended_data);
	enc_color.extended_data = NULL;
	enc_color.raw_data = NULL;
	return dout;
}
