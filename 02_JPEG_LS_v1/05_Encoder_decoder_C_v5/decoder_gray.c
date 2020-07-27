#include "decoder_gray.h"

static decoder_gray dec_gray;
static int byte_count;
static int read_position;
static int current_read_byte;

/**********************************************************************
	ModRange		:	Modulo reduction of error.
						val - value to reduct
						range - range
**********************************************************************/
static int ModRange(int val, int range)
{
 	if(val < 0)
 	{ 	
   		val = val + range;
 	}
 	if(val >= ((range+1)/2))
 	{
 		val = val - range;
	}
 	return val;
}

/**********************************************************************
	GetFromBitStream	:	Read 1 bit from bitstream.
**********************************************************************/
static int GetFromBitStream()
{
  	if (read_position < 0)
    { 	
		byte_count = byte_count + 1;	
      	current_read_byte = dec_gray.bitstream[byte_count];	    
    	if(dec_gray.bitstream[byte_count-1] == -1)
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
static int GolombDecoding(int l, int limit)
{
	int val1 = 0;
 	int val2 = 0;
 	int val3 = 0;
 	int i;
  	val3 = limit - dec_gray.qbpp - 1;
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
    	for(i = 1; i <= dec_gray.qbpp; i++)
    	{
    		val2 += (GetFromBitStream() << (dec_gray.qbpp - i));
		}     		
    	val2 += 1;
 	}
 	if (val2 == dec_gray.RANGE)
 	{
 		return dec_gray.RANGE;
	} 		
 	else
 	{
 		return (unsigned char)val2;		
	} 		
}

/**********************************************************************
	GetNextSample	:	This gets new sample from data and set appropriate
						values.
**********************************************************************/
static void GetNextSample()
{
	/* calculate x and y coordinate */
	if(dec_gray.x.x == dec_gray.w)
	{
    	dec_gray.x.y++;
    	dec_gray.x.x = 1;
	}
 	else
 	{
 		dec_gray.x.x++;
	}
	/* calculate values of casual template */
	dec_gray.Rb = (unsigned char)dec_gray.extended_data[dec_gray.x.y - 1][dec_gray.x.x];
	if(dec_gray.x.x == 1 && dec_gray.x.y > 1)
	{
    	dec_gray.Ra = dec_gray.Rb;
    	dec_gray.Rc = (unsigned char)dec_gray.extended_data[dec_gray.x.y - 1 - 1][dec_gray.x.x];
    	if (dec_gray.w == 1)
    	{
    		dec_gray.Rd = dec_gray.Rb;
		}
		else
		{
			dec_gray.Rd = (unsigned char)dec_gray.extended_data[dec_gray.x.y - 1][dec_gray.x.x + 1];
		}   
	}
 	else if(dec_gray.x.x == dec_gray.w)
 	{
		dec_gray.Ra = (unsigned char)dec_gray.extended_data[dec_gray.x.y][dec_gray.x.x - 1];
		dec_gray.Rc = (unsigned char)dec_gray.extended_data[dec_gray.x.y - 1][dec_gray.x.x - 1];
        dec_gray.Rd = dec_gray.Rb;
	}
 	else
	{ 
		dec_gray.Ra = (unsigned char)dec_gray.extended_data[dec_gray.x.y][dec_gray.x.x - 1];
		dec_gray.Rc = (unsigned char)dec_gray.extended_data[dec_gray.x.y - 1][dec_gray.x.x - 1];
		dec_gray.Rd = (unsigned char)dec_gray.extended_data[dec_gray.x.y - 1][dec_gray.x.x + 1];
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
		if(dec_gray.D[i] <= -dec_gray.T3)
		{
			dec_gray.Q[i] = -4;	
		} 
		else if(dec_gray.D[i] <= -dec_gray.T2) 
		{
			dec_gray.Q[i] = -3;	
		}
		else if(dec_gray.D[i] <= -dec_gray.T1) 
		{
			dec_gray.Q[i] = -2;	
		}
		else if(dec_gray.D[i] < -dec_gray.NEAR )
		{
			dec_gray.Q[i] = -1;	
		} 
		else if(dec_gray.D[i] <= dec_gray.NEAR) 
		{
			dec_gray.Q[i] = 0;	
		}
		else if(dec_gray.D[i] < dec_gray.T1)
		{
			dec_gray.Q[i] = 1;	
		} 
		else if(dec_gray.D[i] < dec_gray.T2) 
		{
			dec_gray.Q[i] = 2;	
		}
		else if(dec_gray.D[i] < dec_gray.T3)
		{
			dec_gray.Q[i] = 3;
		} 
		else
		{
			dec_gray.Q[i] = 4;
		} 
	}
	/* quantized gradient merging and q mapping*/
	if(dec_gray.Q[0] < 0)
	{
	    dec_gray.Q[0] = -dec_gray.Q[0];
	    dec_gray.Q[1] = -dec_gray.Q[1];
	    dec_gray.Q[2] = -dec_gray.Q[2];
	   	dec_gray.SIGN = -1;
	}
  	else if(dec_gray.Q[0] == 0)
	{
        if(dec_gray.Q[1] < 0)
		{
            dec_gray.Q[1] = -dec_gray.Q[1];
        	dec_gray.Q[2] = -dec_gray.Q[2];
        	dec_gray.SIGN = -1;
		}
        else if(dec_gray.Q[1] == 0 && dec_gray.Q[2] < 0)
		{
        	dec_gray.Q[2] = -dec_gray.Q[2];
            dec_gray.SIGN = -1;
		}
        else
		{
			dec_gray.SIGN = 1;	
		} 
	}
  	else 
	{
		dec_gray.SIGN = 1;	
	}
	dec_gray.q = 81 * dec_gray.Q[0] + 9 * dec_gray.Q[1] + dec_gray.Q[2];
	/* edge-detecting predictor */
	if(dec_gray.Rc >= MAX(dec_gray.Ra, dec_gray.Rb))
	{
		dec_gray.Px = MIN(dec_gray.Ra, dec_gray.Rb);	
	}   		
 	else 
	{
   		if(dec_gray.Rc <= MIN(dec_gray.Ra, dec_gray.Rb))
   		{   			
     		dec_gray.Px = MAX(dec_gray.Ra, dec_gray.Rb);
		}
   		else
   		{   			
     		dec_gray.Px = dec_gray.Ra + dec_gray.Rb - dec_gray.Rc;
		}
  	}
  	/* prediction correction from the bias */
  	if(dec_gray.SIGN == 1)
  	{
  		dec_gray.Px = dec_gray.Px + dec_gray.C[dec_gray.q];	
	}   		
 	else
 	{
		dec_gray.Px = dec_gray.Px - dec_gray.C[dec_gray.q];
	}
 	if(dec_gray.Px > dec_gray.MAXVAL)
 	{
 		dec_gray.Px = dec_gray.MAXVAL;
	}   		
 	else if(dec_gray.Px < 0)
 	{
 		dec_gray.Px = 0;
	}	
}

/**********************************************************************
	PredictionErrorDecoding	:	This function perform prediction
								error decoding and computation.
**********************************************************************/
static void PredictionErrorDecoding()
{
	/* computation of the Golomg coding variable k */
    for(dec_gray.k = 0; (dec_gray.N[dec_gray.q] << dec_gray.k) < dec_gray.A[dec_gray.q]; dec_gray.k++)
    {
    	//printf("x");
	}
    /* decode MErrval */
    dec_gray.MErrval = GolombDecoding(dec_gray.k, dec_gray.LIMIT);
    /* inverse of error mapinng */
    if((dec_gray.NEAR == 0) && (dec_gray.k==0) && (2*dec_gray.B[dec_gray.q] <= -dec_gray.N[dec_gray.q])) 
	{
    	if((dec_gray.MErrval & 1))
    	{
    		dec_gray.Errval = (dec_gray.MErrval - 1) >> 1;	
		}      
    	else
    	{
    		dec_gray.Errval = -(dec_gray.MErrval >> 1) - 1;
		}      
  	}
  	else
	{
    	if((dec_gray.MErrval & 1))
   		{
    		dec_gray.Errval = -((dec_gray.MErrval + 1) >> 1);
		}      
    	else
    	{
    		dec_gray.Errval = dec_gray.MErrval >> 1;
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
  	dec_gray.B[dec_gray.q] = dec_gray.B[dec_gray.q] + dec_gray.Errval * (2 * dec_gray.NEAR + 1);
	dec_gray.A[dec_gray.q] = dec_gray.A[dec_gray.q] + ABS(dec_gray.Errval);
	if (dec_gray.N[dec_gray.q] == dec_gray.RESET) 
	{
		dec_gray.A[dec_gray.q] = dec_gray.A[dec_gray.q] >> 1;
		if (dec_gray.B[dec_gray.q] >= 0)
		{
			dec_gray.B[dec_gray.q] = dec_gray.B[dec_gray.q] >> 1;
		}			
		else
		{
			dec_gray.B[dec_gray.q] = -((1-dec_gray.B[dec_gray.q]) >> 1);
		}			
		dec_gray.N[dec_gray.q] = dec_gray.N[dec_gray.q] >> 1;
	}
	dec_gray.N[dec_gray.q] = dec_gray.N[dec_gray.q] + 1;
	/* invert sign if neccesary */
	if(dec_gray.SIGN == -1)
	{
		dec_gray.Errval = -dec_gray.Errval;
	}
	dec_gray.Rx = (dec_gray.Errval + dec_gray.Px) % (dec_gray.RANGE);
  	/* compute sample value */
	if(dec_gray.Rx < 0)
	{
		dec_gray.Rx = dec_gray.Rx + dec_gray.RANGE;
	}    	
  	else if (dec_gray.Rx > dec_gray.MAXVAL)
  	{
  		dec_gray.Rx = dec_gray.Rx - dec_gray.RANGE;
	}
  	if(dec_gray.Rx < 0)
  	{
  		dec_gray.Rx = 0;
	}    	
  	else if(dec_gray.Rx > dec_gray.MAXVAL)
  	{
  		dec_gray.Rx = dec_gray.MAXVAL;	
	}	
	/* update of bias-related variables B[Q] and C[Q] */
	if(dec_gray.B[dec_gray.q] <= -dec_gray.N[dec_gray.q])
	{
  		dec_gray.B[dec_gray.q] = dec_gray.B[dec_gray.q] + dec_gray.N[dec_gray.q];
   		if(dec_gray.C[dec_gray.q] > dec_gray.MIN_C)
   		{
   			dec_gray.C[dec_gray.q] = dec_gray.C[dec_gray.q] - 1;
		}     		
   		if(dec_gray.B[dec_gray.q] <= -dec_gray.N[dec_gray.q])
   		{   			
     		dec_gray.B[dec_gray.q] = -dec_gray.N[dec_gray.q] + 1;
		}
 	}
 	else if(dec_gray.B[dec_gray.q] > 0) 
	{
   		dec_gray.B[dec_gray.q] = dec_gray.B[dec_gray.q] - dec_gray.N[dec_gray.q];
   		if(dec_gray.C[dec_gray.q] < dec_gray.MAX_C)
   		{
     		dec_gray.C[dec_gray.q] = dec_gray.C[dec_gray.q] + 1;
		}
   		if(dec_gray.B[dec_gray.q] > 0)
   		{   			
    		dec_gray.B[dec_gray.q] = 0;
		}
 	}
 	dec_gray.extended_data[dec_gray.x.y][dec_gray.x.x] = dec_gray.Rx;
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
     		if((dec_gray.w - dec_gray.x.x) + 1 >= (1 << dec_gray.J[dec_gray.RUNindex]))
			{
        		for(i = 0; i < (1 << dec_gray.J[dec_gray.RUNindex]); i++)
        		{
        			dec_gray.extended_data[dec_gray.x.y][dec_gray.x.x + i] = dec_gray.Ra;	
				}
            	dec_gray.x.x += (1 << dec_gray.J[dec_gray.RUNindex]);        
        		if(dec_gray.RUNindex < 31)
				{
          			dec_gray.RUNindex_prev = dec_gray.RUNindex;
          			dec_gray.RUNindex++;
				}
     		}
    	 	else
			{
        		for(i = dec_gray.x.x; i <= dec_gray.w; i++)
            	{
            		dec_gray.extended_data[dec_gray.x.y][i] = dec_gray.Ra;
				}			
        		dec_gray.x.x = dec_gray.w;
       			break;
	   		}
 		}
 		else
 		{ 			
  			for(i = 1; i <= dec_gray.J[dec_gray.RUNindex]; i++)
  			{
  				cnt += (GetFromBitStream() << (dec_gray.J[dec_gray.RUNindex] - i));
			}     
  			for(i = 0; i < cnt; i++)
  			{
  				dec_gray.extended_data[dec_gray.x.y][dec_gray.x.x + i] = dec_gray.Ra;
			}     
  			dec_gray.x.x += cnt;
  			if(dec_gray.RUNindex > 0)
			{
          		dec_gray.RUNindex_prev = dec_gray.RUNindex;
          		dec_gray.RUNindex--;
			}
  			run_flag = 1;
  			break;
  		}
 		if(dec_gray.x.x > dec_gray.w)
		{
        	dec_gray.x.x = dec_gray.w;
          	break;
		}
	}
	if (run_flag)
	{
		dec_gray.x.x--;
		GetNextSample(); 
		/* index computation */	
  		if(dec_gray.Ra == dec_gray.Rb)
  		{
  			dec_gray.RItype = 1;
		}
    	else
    	{    		
    		dec_gray.RItype = 0;
		}
		/* computation of the auxiliary variable TEMP */
		if (dec_gray.RItype == 0)
		{			
			dec_gray.TEMP = dec_gray.A[365];
		}
		else
		{
			dec_gray.TEMP = dec_gray.A[366] + (dec_gray.N[366] >> 1);
		}
		/* Q computation */
		dec_gray.q = dec_gray.RItype + 365; 
		/* k computation */
		for(dec_gray.k = 0; (dec_gray.N[dec_gray.q] << dec_gray.k) < dec_gray.TEMP; dec_gray.k++);
		/* encoder EMErrval */
		dec_gray.glimit = dec_gray.LIMIT - dec_gray.J[dec_gray.RUNindex_prev] - 1;
  		dec_gray.EMErrval = GolombDecoding(dec_gray.k, dec_gray.glimit);
  		/* Errval computation */
  		if(dec_gray.EMErrval & 1)
		{
			if(dec_gray.RItype == 1)	
			{
				dec_gray.map = 0;	
			}
			else
			{
				dec_gray.map = 1;	
			}
			dec_gray.Errval = (dec_gray.EMErrval + 1) >> 1;
       	}
  		else
		{
    		if(dec_gray.RItype == 1)
			{
      			dec_gray.Errval = (dec_gray.EMErrval + 2) >> 1;
      			dec_gray.map = 1;
			}
    		else
			{
     	 		dec_gray.Errval = dec_gray.EMErrval >> 1;
      			dec_gray.map = 0;
	  		}
		}
		/* correct the sign of the map fun. */
		if((dec_gray.k == 0) && (dec_gray.map == 0) && (2*dec_gray.Nn[dec_gray.q] < dec_gray.N[dec_gray.q]))
		{
			dec_gray.Errval = -dec_gray.Errval;
		}
		else if((dec_gray.map == 1) && (2 * dec_gray.Nn[dec_gray.q] >= dec_gray.N[dec_gray.q]))
		{
			dec_gray.Errval = -dec_gray.Errval;
		}		    
		else if((dec_gray.map == 1) && (dec_gray.k != 0))
		{
			dec_gray.Errval = -dec_gray.Errval;
		}
		if(dec_gray.Errval < 0)
  		{  			
   			dec_gray.Nn[dec_gray.q] = dec_gray.Nn[dec_gray.q] + 1;
		}
		/* Error computation for a run interruption sample */
 		if((dec_gray.RItype == 0) && (dec_gray.Rb < dec_gray.Ra))
 		{
 			dec_gray.Errval = -dec_gray.Errval;
		}
		/* Adding prediction error for a run interruption sample */
		if(dec_gray.RItype == 1)
		{
			dec_gray.Rx = (dec_gray.Errval + dec_gray.Ra) % (dec_gray.RANGE);
		}
		else
		{
			dec_gray.Rx = (dec_gray.Errval + dec_gray.Rb) % (dec_gray.RANGE);
		}
		if(dec_gray.Rx < 0)
		{
			dec_gray.Rx = dec_gray.Rx + dec_gray.RANGE;
		}    	
	  	else if (dec_gray.Rx > dec_gray.MAXVAL)
	  	{
	  		dec_gray.Rx = dec_gray.Rx - dec_gray.RANGE;
		}
	  	if(dec_gray.Rx < 0)
	  	{
	  		dec_gray.Rx = 0;
		}    	
	  	else if(dec_gray.Rx > dec_gray.MAXVAL)
	  	{
	  		dec_gray.Rx = dec_gray.MAXVAL;	
		}	
		dec_gray.extended_data[dec_gray.x.y][dec_gray.x.x] = dec_gray.Rx;		
		/* update of parameters for run interruption sample */
	 	dec_gray.A[dec_gray.q] = dec_gray.A[dec_gray.q] + ((dec_gray.EMErrval + 1 - dec_gray.RItype) >> 1);
  		if(dec_gray.N[dec_gray.q] == dec_gray.RESET)
		{
       		dec_gray.A[dec_gray.q] = dec_gray.A[dec_gray.q] >> 1;
       		dec_gray.N[dec_gray.q] = dec_gray.N[dec_gray.q] >> 1;
       		dec_gray.Nn[dec_gray.q] = dec_gray.Nn[dec_gray.q] >> 1;
  		}
  		dec_gray.N[dec_gray.q] = dec_gray.N[dec_gray.q] + 1;
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
	dec_gray.bitstream = Allocate1D(size);
   	byte_count = size;
   	for (i = 0; i < byte_count; i++)
   	{
   		dec_gray.bitstream[i] = data[i];
	}
	read_position = -1;
	byte_count = -1;
	/* initialisations of appropriate values */
	dec_gray.RANGE = 256;
	dec_gray.MAXVAL = 255;
	dec_gray.qbpp = 8;
	dec_gray.bpp = 8;
	dec_gray.LIMIT = 32; 
	dec_gray.w = w;
	dec_gray.h = h;
	for(i = 0; i < 365; i++)
	{
    	dec_gray.A[i] = 4;
      	dec_gray.N[i] = 1;
      	dec_gray.B[i] = 0;
      	dec_gray.C[i] = 0;
      	dec_gray.Nn[i] = 0;
	}
	for(i = 0; i < COMPONENTS_COUNT; i++)
	{
		dec_gray.D[i] = 0;
		dec_gray.Q[i] = 0;
	}
	dec_gray.A[365] = 4;
    dec_gray.A[366] = 4;
    dec_gray.N[365] = 1;
    dec_gray.N[366] = 1;
    dec_gray.Nn[365] = 0;
    dec_gray.Nn[366] = 0;
    dec_gray.Ra = 0;
	dec_gray.Rb = 0;
	dec_gray.Rc = 0;
	dec_gray.Rd = 0;
	dec_gray.Px = 0;
	dec_gray.Rx = 0;
    dec_gray.RUNindex = 0;    
    dec_gray.RUNval = 0;  
	dec_gray.RUNcnt = 0;    
	dec_gray.RUNindex_prev = 0;  
	dec_gray.RItype = 0;  
	dec_gray.glimit = 0;
	dec_gray.SIGN = 0;
	dec_gray.TEMP = 0;
	dec_gray.Errval = 0;
	dec_gray.EMErrval = 0;
	dec_gray.MErrval;
	dec_gray.q = 0;
	dec_gray.k = 0;
	dec_gray.map = 0;	
    dec_gray.x.x = 0;
    dec_gray.x.y = 1;	
    dec_gray.RESET = 64;
    dec_gray.T1 = 3;
    dec_gray.T2 = 7;
    dec_gray.T3 = 21;
    dec_gray.MAX_C = 127;
    dec_gray.MIN_C = -128;
    dec_gray.NEAR = 0;
    dec_gray.J[0] = 0; dec_gray.J[1] = 0; dec_gray.J[2] = 0; dec_gray.J[3] = 0; dec_gray.J[4] = 1; dec_gray.J[5] = 1; dec_gray.J[6] = 1; dec_gray.J[7] = 1; 
    dec_gray.J[8] = 2; dec_gray.J[9] = 2; dec_gray.J[10] = 2; dec_gray.J[11] = 2; dec_gray.J[12] = 3; dec_gray.J[13] = 3; dec_gray.J[14] = 3; dec_gray.J[15] = 3; 
    dec_gray.J[16] = 4; dec_gray.J[17] = 4; dec_gray.J[18] = 5; dec_gray.J[19] = 5; dec_gray.J[20] = 6; dec_gray.J[21] = 6; dec_gray.J[22] = 7; dec_gray.J[23] = 7;     
    dec_gray.J[24] = 8; dec_gray.J[25] = 9; dec_gray.J[26] = 10; dec_gray.J[27] = 11; dec_gray.J[28] = 12; dec_gray.J[29] = 13; dec_gray.J[30] = 14; dec_gray.J[31] = 15;    
	/* allocate data for non defined samples */	
    dec_gray.raw_data = Allocate2D(w, h); 
    /* initialize non defined samples */
    dec_gray.extended_data = InitializeNonDefinedSamples(dec_gray.raw_data, w, h);
}

/**********************************************************************
	ContextModeling	:	This function performs context modelling 
						procedure.
**********************************************************************/
static int ContextModeling()
{
	/* local gradient computation */
    dec_gray.D[0] = dec_gray.Rd - dec_gray.Rb;
    dec_gray.D[1] = dec_gray.Rb - dec_gray.Rc;
    dec_gray.D[2] = dec_gray.Rc - dec_gray.Ra;
    /* mode selection */
 	if (dec_gray.D[0] == 0 && dec_gray.D[1] == 0 && dec_gray.D[2] == 0)
 	{
  		return 1;
	}
	else
	{			
  		return 0;
	}
}


/**********************************************************************
	DecodeGray		:	This function decode input gray bitstream 
						into raw data.
						data - pointer to input data
						w - width of decoded data
						h - height of decoded data
						size - bitstream size
						return NULL if cannot encode input data
						return pointer to compressed data
**********************************************************************/
char** DecodeGray(char* data, int w, int h, int size)
{
	
	int i, j;
	char** dout;
	dout = Allocate2D(w, h);
	/* Initializations */
	Initializations(data, w, h, size);
    /* for each sample */
    for(;;)
    {
    	/* check if all sample proceeded */
		if(dec_gray.x.x == w && dec_gray.x.y == h)
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
			dout[i][j] = dec_gray.extended_data[i + 1][j + 1];
		}
	}
	Free1D(dec_gray.bitstream);
	Free2D(dec_gray.extended_data);
	Free2D(dec_gray.raw_data);
	dec_gray.bitstream = NULL;
	dec_gray.extended_data = NULL;
	dec_gray.raw_data;
	return dout;
}
