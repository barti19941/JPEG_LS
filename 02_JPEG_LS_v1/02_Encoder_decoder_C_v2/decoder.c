#include "decoder.h"

static decoder dec;
static int byte_count;
static int count;
static int read_position;
static int current_read_byte;

/**********************************************************************
	GetFromBitStream	:	Read 1 bit from bitstream.
**********************************************************************/
int GetFromBitStream()
{
  	if (read_position < 0)
    { 	
		byte_count = byte_count + 1;	
      	current_read_byte = dec.bitstream[byte_count];	    
    	if(dec.bitstream[byte_count-1] == -1)
      	{
    		read_position = 6;
	  	}        
      	else
      	{
      		read_position = 7;	
		}      
    }
  	if (current_read_byte & bit_set_mask[read_position--])
	{
    	return(1);
    }
  	return(0);
}

/**********************************************************************
	GolombCoding	:	Function decoding value from bitstream.
						l - golomb code variable
						limit - number of bits to which the length of
						 a Golomb code word is limited 
**********************************************************************/
int GolombDecoding(int l, int limit)
{
	int val1 = 0;
 	int val2 = 0;
 	int val3;
 	int i;
  	val3 = limit - dec.qbpp - 1;
 	while(GetFromBitStream() != 1)
 	{
 		val1 += 1;
	}
   	if(val1 < val3)
	{
   		val2 = val1 << l;
   		for(i = 1; i <= l; i++)
   		{
   			val2 += (GetFromBitStream() << (l - i));
		}     		
 	}
 	else 
	{
    	for(i = 1; i <= dec.qbpp; i++)
    	{
    		val2 += (GetFromBitStream() << (dec.qbpp - i));
		}     		
    	val2 += 1;
 	}
 	return (unsigned char)val2;
}

/**********************************************************************
	GetNextSample	:	This gets new sample from data and set appropriate
						values.
**********************************************************************/
static void GetNextSample()
{
	/* calculate x and y coordinate */
	if(dec.x.x == dec.w)
	{
    	dec.x.y++;
    	dec.x.x = 1;
	}
 	else
 	{
 		dec.x.x++;
	}
	/* calculate values of casual template */
	dec.Rb = (unsigned char)dec.extended_data[dec.x.y - 1][dec.x.x];
	if(dec.x.x == 1 && dec.x.y > 1)
	{
    	dec.Ra = (unsigned char)dec.Rb;
    	dec.Rc = (unsigned char)dec.extended_data[dec.x.y - 1 - 1][dec.x.x];
    	dec.Rd = (unsigned char)dec.extended_data[dec.x.y - 1][dec.x.x + 1];
	}
 	else if(dec.x.x == dec.w)
 	{
		dec.Ra = (unsigned char)dec.extended_data[dec.x.y][dec.x.x - 1];
		dec.Rc = (unsigned char)dec.extended_data[dec.x.y - 1][dec.x.x - 1];
        dec.Rd = (unsigned char)dec.Rb;
	}
 	else
	{ 
		dec.Ra = (unsigned char)dec.extended_data[dec.x.y][dec.x.x - 1];
		dec.Rc = (unsigned char)dec.extended_data[dec.x.y - 1][dec.x.x - 1];
		dec.Rd = (unsigned char)dec.extended_data[dec.x.y - 1][dec.x.x + 1];
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
		if(dec.D[i] <= -dec.T3)
		{
			dec.Q[i] = -4;	
		} 
		else if(dec.D[i] <= -dec.T2) 
		{
			dec.Q[i] = -3;	
		}
		else if(dec.D[i] <= -dec.T1) 
		{
			dec.Q[i] = -2;	
		}
		else if(dec.D[i] < -dec.NEAR )
		{
			dec.Q[i] = -1;	
		} 
		else if(dec.D[i] <= dec.NEAR) 
		{
			dec.Q[i] = 0;	
		}
		else if(dec.D[i] < dec.T1)
		{
			dec.Q[i] = 1;	
		} 
		else if(dec.D[i] < dec.T2) 
		{
			dec.Q[i] = 2;	
		}
		else if(dec.D[i] < dec.T3)
		{
			dec.Q[i] = 3;
		} 
		else
		{
			dec.Q[i] = 4;
		} 
	}
	/* quantized gradient merging and q mapping*/
	if(dec.Q[0] < 0)
	{
	    dec.Q[0] = -dec.Q[0];
	    dec.Q[1] = -dec.Q[1];
	    dec.Q[2] = -dec.Q[2];
	   	dec.SIGN = -1;
	}
  	else if(dec.Q[0] == 0)
	{
        if(dec.Q[1] < 0)
		{
            dec.Q[1] = -dec.Q[1];
        	dec.Q[2] = -dec.Q[2];
        	dec.SIGN = -1;
		}
        else if(dec.Q[1] == 0 && dec.Q[2] < 0)
		{
        	dec.Q[2] = -dec.Q[2];
            dec.SIGN = -1;
		}
        else
		{
			dec.SIGN = 1;	
		} 
	}
  	else 
	{
		dec.SIGN = 1;	
	}
	dec.q = 81 * dec.Q[0] + 9 * dec.Q[1] + dec.Q[2];
	/* edge-detecting predictor */
	if(dec.Rc >= MAX(dec.Ra, dec.Rb))
	{
		dec.Px = MIN(dec.Ra, dec.Rb);	
	}   		
 	else 
	{
   		if(dec.Rc <= MIN(dec.Ra, dec.Rb))
   		{   			
     		dec.Px = MAX(dec.Ra, dec.Rb);
		}
   		else
   		{   			
     		dec.Px = dec.Ra + dec.Rb - dec.Rc;
		}
  	}
  	/* prediction correction from the bias */
  	if(dec.SIGN == 1)
  	{
  		dec.Px = dec.Px + dec.C[dec.q];	
	}   		
 	else
 	{
		dec.Px = dec.Px - dec.C[dec.q];
	}
 	if(dec.Px > dec.MAXVAL)
 	{
 		dec.Px = dec.MAXVAL;
	}   		
 	else if(dec.Px < 0)
 	{
 		dec.Px = 0;
	}	
}

/**********************************************************************
	PredictionErrorDecoding	:	This function perform prediction
								error decoding and computation.
**********************************************************************/
static void PredictionErrorDecoding()
{
	/* computation of the Golomg coding variable k */
    for(dec.k = 0; (dec.N[dec.q] << dec.k) < dec.A[dec.q]; dec.k++);
    /* decode MErrval */
    dec.MErrval = GolombDecoding(dec.k, dec.LIMIT);
    /* inverse of error mapinng */
    if((dec.NEAR == 0) && (dec.k==0) && (2*dec.B[dec.q] <= -dec.N[dec.q])) 
	{
    	if((dec.MErrval & 1))
    	{
    		dec.Errval = (dec.MErrval - 1) >> 1;	
		}      
    	else
    	{
    		dec.Errval = -(dec.MErrval >> 1) - 1;
		}      
  	}
  	else
	{
    	if((dec.MErrval & 1))
   		{
    		dec.Errval = -((dec.MErrval + 1) >> 1);
		}      
    	else
    	{
    		dec.Errval = dec.MErrval >> 1;
		}
      
  	}
}

/**********************************************************************
	VariablesUpdate			:	This function performs variables update
								and sample value computation.
**********************************************************************/
static void VariablesUpdate()
{	
  	/* variables update */
  	dec.B[dec.q] = dec.B[dec.q] + dec.Errval * (2 * dec.NEAR + 1);
	dec.A[dec.q] = dec.A[dec.q] + ABS(dec.Errval);
	if (dec.N[dec.q] == dec.RESET) 
	{
		dec.A[dec.q] = dec.A[dec.q] >> 1;
		if (dec.B[dec.q] >= 0)
		{
			dec.B[dec.q] = dec.B[dec.q] >> 1;
		}			
		else
		{
			dec.B[dec.q] = -((1-dec.B[dec.q]) >> 1);
		}			
		dec.N[dec.q] = dec.N[dec.q] >> 1;
	}
	dec.N[dec.q] = dec.N[dec.q] + 1;
	/* invert sign if neccesary */
	if(dec.SIGN == -1)
	{
		dec.Errval = -dec.Errval;
	}
	dec.Rx = (dec.Errval + dec.Px) % (dec.RANGE);
  	/* compute sample value */
	if(dec.Rx < 0)
	{
		dec.Rx = dec.Rx + dec.RANGE;
	}    	
  	else if (dec.Rx > dec.MAXVAL)
  	{
  		dec.Rx = dec.Rx - dec.RANGE;
	}
  	if(dec.Rx < 0)
  	{
  		dec.Rx = 0;
	}    	
  	else if(dec.Rx > dec.MAXVAL)
  	{
  		dec.Rx = dec.MAXVAL;	
	}	
	/* update of bias-related variables B[Q] and C[Q] */
	if(dec.B[dec.q] <= -dec.N[dec.q])
	{
  		dec.B[dec.q] = dec.B[dec.q] + dec.N[dec.q];
   		if(dec.C[dec.q] > dec.MIN_C)
   		{
   			dec.C[dec.q] = dec.C[dec.q] - 1;
		}     		
   		if(dec.B[dec.q] <= -dec.N[dec.q])
   		{   			
     		dec.B[dec.q] = -dec.N[dec.q] + 1;
		}
 	}
 	else if(dec.B[dec.q] > 0) 
	{
   		dec.B[dec.q] = dec.B[dec.q] - dec.N[dec.q];
   		if(dec.C[dec.q] < dec.MAX_C)
   		{
     		dec.C[dec.q] = dec.C[dec.q] + 1;
		}
   		if(dec.B[dec.q] > 0)
   		{   			
    		dec.B[dec.q] = 0;
		}
 	}
 	dec.extended_data[dec.x.y][dec.x.x] = (unsigned char)dec.Rx;
}

/**********************************************************************
	RegularModeProcessing	:	Regural mode processing of samples.
**********************************************************************/
static void RegularModeProcessing()
{
	/* Prediction */
	Prediction();
	/* Prediction error decoding and computatation */
	PredictionErrorDecoding();
	/* Sample value computation and variables update */
	VariablesUpdate();
}

/**********************************************************************
	RunModeProcessing	:	Run mode processing of samples.
**********************************************************************/
static void RunModeProcessing()
{
	int R;
	int cnt = 0;
	int i;
	int run_flag;
	/* read R bit from bitstream */
	for (;;)
	{
		run_flag = 0;
		R = GetFromBitStream();
 		if(R)
		{			
     		if((dec.w - dec.x.x) + 1 >= (1 << dec.J[dec.RUNindex]))
			{
        		for(i = 0; i < (1 << dec.J[dec.RUNindex]); i++)
        		{
        			dec.extended_data[dec.x.y][dec.x.x + i] = (unsigned char)dec.Ra;	
				}
            	dec.x.x += (1 << dec.J[dec.RUNindex]);        
        		if(dec.RUNindex < 31)
				{
          			dec.RUNindex_prev = dec.RUNindex;
          			dec.RUNindex++;
				}
     		}
    	 	else
			{
        		for(i = dec.x.x; i <= dec.w; i++)
            	{
            		dec.extended_data[dec.x.y][i] = (unsigned char)dec.Ra;
				}			
        		dec.x.x = dec.w;
       			break;
	   		}
 		}
 		else
 		{ 			
  			for(i = 1; i <= dec.J[dec.RUNindex]; i++)
  			{
  				cnt += (unsigned char)(GetFromBitStream() << (dec.J[dec.RUNindex] - i));
			}     
  			for(i = 0; i < cnt; i++)
  			{
  				dec.extended_data[dec.x.y][dec.x.x + i] = (unsigned char)dec.Ra;
			}     
  			dec.x.x += cnt;
  			if(dec.RUNindex > 0)
			{
          		dec.RUNindex_prev = dec.RUNindex;
          		dec.RUNindex--;
			}
  			run_flag = 1;
  			break;
  		}
 		if(dec.x.x > dec.w)
		{
        	dec.x.x = dec.w;
          	break;
		}
	}
	if (run_flag)
	{
		dec.x.x--;
		GetNextSample(); 
		/* index computation */	
  		if(dec.Ra == dec.Rb)
  		{
  			dec.RItype = 1;
		}
    	else
    	{    		
    		dec.RItype = 0;
		}
		/* computation of the auxiliary variable TEMP */
		if (dec.RItype == 0)
		{			
			dec.TEMP = dec.A[365];
		}
		else
		{
			dec.TEMP = dec.A[366] + (dec.N[366] >> 1);
		}
		/* Q computation */
		dec.q = dec.RItype + 365; 
		/* k computation */
		for(dec.k = 0; (dec.N[dec.q] << dec.k) < dec.TEMP; dec.k++);
		/* encoder EMErrval */
		dec.glimit = dec.LIMIT - dec.J[dec.RUNindex_prev] - 1;
  		dec.EMErrval = GolombDecoding(dec.k, dec.glimit);
  		/* Errval computation */
  		if(dec.EMErrval & 1)
		{
			if(dec.RItype == 1)	
			{
				dec.map = 0;	
			}
			else
			{
				dec.map = 1;	
			}
			dec.Errval = (dec.EMErrval + 1) >> 1;
       	}
  		else
		{
    		if(dec.RItype == 1)
			{
      			dec.Errval = (dec.EMErrval + 2) >> 1;
      			dec.map = 1;
			}
    		else
			{
     	 		dec.Errval = dec.EMErrval >> 1;
      			dec.map = 0;
	  		}
		}
		/* correct the sign of the map fun. */
		if((dec.k == 0) && (dec.map == 0) && (2*dec.Nn[dec.q] < dec.N[dec.q]))
		{
			dec.Errval = -dec.Errval;
		}
		else if((dec.map == 1) && (2*dec.Nn[dec.q] >= dec.N[dec.q]))
		{
			dec.Errval = -dec.Errval;
		}		    
		else if((dec.map == 1) && (dec.k != 0))
		{
			dec.Errval = -dec.Errval;
		}
		/* Error computation for a run interruption sample */
 		if((dec.RItype == 0) && (dec.Rb < dec.Ra))
 		{
 			dec.Errval = -dec.Errval;
		}
		/* Adding prediction error for a run interruption sample */
		if(dec.RItype == 1)
		{
			dec.Rx = (dec.Errval + dec.Ra) % (dec.RANGE);
		}
		else
		{
			dec.Rx = (dec.Errval + dec.Rb) % (dec.RANGE);
		}
		if(dec.Rx < 0)
		{
			dec.Rx = dec.Rx + dec.RANGE;
		}    	
	  	else if (dec.Rx > dec.MAXVAL)
	  	{
	  		dec.Rx = dec.Rx - dec.RANGE;
		}
	  	if(dec.Rx < 0)
	  	{
	  		dec.Rx = 0;
		}    	
	  	else if(dec.Rx > dec.MAXVAL)
	  	{
	  		dec.Rx = dec.MAXVAL;	
		}	
		dec.extended_data[dec.x.y][dec.x.x] = (unsigned char)dec.Rx;		
		/* update of parameters for run interruption sample */
  		if(dec.Errval < 0)
  		{  			
   			dec.Nn[dec.q] = dec.Nn[dec.q] + 1;
		}
	 	dec.A[dec.q] = dec.A[dec.q] + ((dec.EMErrval + 1 - dec.RItype) >> 1);
  		if(dec.N[dec.q] == dec.RESET)
		{
       		dec.A[dec.q] = dec.A[dec.q] >> 1;
       		dec.N[dec.q] = dec.N[dec.q] >> 1;
       		dec.Nn[dec.q] = dec.Nn[dec.q] >> 1;
  		}
  		dec.N[dec.q] = dec.N[dec.q] + 1;
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
	Initializations	:	This function initialize basic parameters
						of decoder as well as auxillary variables.
						data - pointer to input data
						w - width of input data
						h - height of input data
						size - size of input bitstream
**********************************************************************/
static void Initializations(char* data, int w, int h, int size)
{
	int i;
	/* clear bitstream */
	for (i = 0; i < BITSTREAM_LENGHT; i++)
	{
		dec.bitstream[i] = 0;
	}
   	byte_count = size;
   	for (i = 0; i < byte_count; i++)
   	{
   		dec.bitstream[i] = data[i];
	}
	read_position = -1;
	byte_count = -1;
	/* initialisations of appropriate values */
	dec.RANGE = 256;
	dec.MAXVAL = 255;
	dec.qbpp = 8;
	dec.bpp = 8;
	dec.LIMIT = 32; 
	dec.w = w;
	dec.h = h;
	for(i = 0; i < 365; i++)
	{
    	dec.A[i] = 4;
      	dec.N[i] = 1;
      	dec.B[i] = 0;
      	dec.C[i] = 0;
      	dec.Nn[i] = 0;
	}
	dec.A[365] = 4;
    dec.A[366] = 4;
    dec.N[365] = 1;
    dec.N[366] = 1;
    dec.Nn[365] = 0;
    dec.Nn[366] = 0;
    dec.RUNindex = 0;    
    dec.x.x = 0;
    dec.x.y = 1;	
    count = 0;
    dec.RESET = 64;
    dec.T1 = 3;
    dec.T2 = 7;
    dec.T3 = 21;
    dec.MAX_C = 127;
    dec.MIN_C = -128;
    dec.NEAR = 0;
    dec.J[0] = 0; dec.J[1] = 0; dec.J[2] = 0; dec.J[3] = 0; dec.J[4] = 1; dec.J[5] = 1; dec.J[6] = 1; dec.J[7] = 1; 
    dec.J[8] = 2; dec.J[9] = 2; dec.J[10] = 2; dec.J[11] = 2; dec.J[12] = 3; dec.J[13] = 3; dec.J[14] = 3; dec.J[15] = 3; 
    dec.J[16] = 4; dec.J[17] = 4; dec.J[18] = 5; dec.J[19] = 5; dec.J[20] = 6; dec.J[21] = 6; dec.J[22] = 7; dec.J[23] = 7;     
    dec.J[24] = 8; dec.J[25] = 9; dec.J[26] = 10; dec.J[27] = 11; dec.J[28] = 12; dec.J[29] = 13; dec.J[30] = 14; dec.J[31] = 15;    
	/* allocate data for non defined samples */	
    dec.raw_data = Allocate2D(w, h); 
    /* initialize non defined samples */
    dec.extended_data = InitializeNonDefinedSamples(dec.raw_data, w, h);
}

/**********************************************************************
	ContextModeling	:	This function performs context modelling 
						procedure.
**********************************************************************/
static int ContextModeling()
{
	/* local gradient computation */
    dec.D[0] = dec.Rd - dec.Rb;
    dec.D[1] = dec.Rb - dec.Rc;
    dec.D[2] = dec.Rc - dec.Ra;
    /* mode selection */
 	if (dec.D[0] == 0 && dec.D[1] == 0 && dec.D[2] == 0)
 	{
  		return 1;
	}
	else
	{			
  		return 0;
	}
}


/**********************************************************************
	Decode			:	This function decode input bitstream into raw 
						data.
						data - pointer to input data
						w - width of decoded data
						h - height of decoded data
						size - bitstream size
						return NULL if cannot encode input data
						return pointer to compressed data
**********************************************************************/
char** Decode(char* data, int w, int h, int size)
{
	
	int i, j;
	char** dout;
	dout = Allocate2D(w, h);
	if (data == NULL)
	{
		printf("ERROR\tWrong input data!\n");
		return NULL;
	}
	/* Initializations */
	Initializations(data, w, h, size);
    /* for each sample */
    for(;;)
    {
    	/* check if all sample proceeded */
		if(dec.x.x == w && dec.x.y == h)
    	{    		
        	break;
		} 
		/* set sample */ 
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
	/* rewrite decoded data */
	for (i = 0; i < h; i++)
	{
		for (j = 0; j < w; j++)
		{
			dout[i][j] = dec.extended_data[i + 1][j + 1];
		}
	}
	Free2D(dec.extended_data);
	Free2D(dec.raw_data);
	return dout;
}
