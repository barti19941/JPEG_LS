#include "decoder_color_line_ilv.h"

static decoder_color_line_ilv dec_color;
static int byte_count;
static int current_component;
static int read_position;
static int current_read_byte;

/**********************************************************************
	GetNextSample	:	This gets new sample from data and set appropriate
						values.
**********************************************************************/
static void GetNextSample()
{
	/* change component if necessary */
	if (dec_color.x[current_component].x == dec_color.w)
	{
		current_component++;			
		if (current_component == 3)
		{
			current_component = 0;
		}
	}	
	/* calculate x and y coordinate */
	if(dec_color.x[current_component].x == dec_color.w)
	{
    	dec_color.x[current_component].y++;
    	dec_color.x[current_component].x = 1;
	}
 	else
 	{
 		dec_color.x[current_component].x++;
	}
	/* calculate values of casual template */
	dec_color.Rb = (unsigned char)dec_color.extended_data[current_component][dec_color.x[current_component].y - 1][dec_color.x[current_component].x];
	if(dec_color.x[current_component].x == 1 && dec_color.x[current_component].y > 1)
	{
    	dec_color.Ra = (unsigned char)dec_color.Rb;
    	dec_color.Rc = (unsigned char)dec_color.extended_data[current_component][dec_color.x[current_component].y - 1 - 1][dec_color.x[current_component].x];
    	dec_color.Rd = (unsigned char)dec_color.extended_data[current_component][dec_color.x[current_component].y - 1][dec_color.x[current_component].x + 1];
	}
 	else if(dec_color.x[current_component].x == dec_color.w)
 	{
		dec_color.Ra = (unsigned char)dec_color.extended_data[current_component][dec_color.x[current_component].y][dec_color.x[current_component].x - 1];
		dec_color.Rc = (unsigned char)dec_color.extended_data[current_component][dec_color.x[current_component].y - 1][dec_color.x[current_component].x - 1];
        dec_color.Rd = (unsigned char)dec_color.Rb;
	}
 	else
	{ 
		dec_color.Ra = (unsigned char)dec_color.extended_data[current_component][dec_color.x[current_component].y][dec_color.x[current_component].x - 1];
		dec_color.Rc = (unsigned char)dec_color.extended_data[current_component][dec_color.x[current_component].y - 1][dec_color.x[current_component].x - 1];
		dec_color.Rd = (unsigned char)dec_color.extended_data[current_component][dec_color.x[current_component].y - 1][dec_color.x[current_component].x + 1];
	}
}

/**********************************************************************
	GetFromBitStream	:	Read 1 bit from bitstream.
**********************************************************************/
static int GetFromBitStream()
{
  	if (read_position < 0)
    { 	
		byte_count = byte_count + 1;	
      	current_read_byte = dec_color.bitstream[byte_count];	    
    	if(dec_color.bitstream[byte_count-1] == -1)
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
 	int val3;
 	int i;
  	val3 = limit - dec_color.qbpp - 1;
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
    	for(i = 1; i <= dec_color.qbpp; i++)
    	{
    		val2 += (GetFromBitStream() << (dec_color.qbpp - i));
		}     		
    	val2 += 1;
 	}
 	return (unsigned char)val2;
}

/**********************************************************************
	VariablesUpdate			:	This function performs variables update
								and sample value computation.
**********************************************************************/
static void VariablesUpdate()
{	
  	/* variables update */
  	dec_color.B[dec_color.q] = dec_color.B[dec_color.q] + dec_color.Errval * (2 * dec_color.NEAR + 1);
	dec_color.A[dec_color.q] = dec_color.A[dec_color.q] + ABS(dec_color.Errval);
	if (dec_color.N[dec_color.q] == dec_color.RESET) 
	{
		dec_color.A[dec_color.q] = dec_color.A[dec_color.q] >> 1;
		if (dec_color.B[dec_color.q] >= 0)
		{
			dec_color.B[dec_color.q] = dec_color.B[dec_color.q] >> 1;
		}			
		else
		{
			dec_color.B[dec_color.q] = -((1-dec_color.B[dec_color.q]) >> 1);
		}			
		dec_color.N[dec_color.q] = dec_color.N[dec_color.q] >> 1;
	}
	dec_color.N[dec_color.q] = dec_color.N[dec_color.q] + 1;
	/* invert sign if neccesary */
	if(dec_color.SIGN == -1)
	{
		dec_color.Errval = -dec_color.Errval;
	}
	dec_color.Rx = (dec_color.Errval + dec_color.Px) % (dec_color.RANGE);
  	/* compute sample value */
	if(dec_color.Rx < 0)
	{
		dec_color.Rx = dec_color.Rx + dec_color.RANGE;
	}    	
  	else if (dec_color.Rx > dec_color.MAXVAL)
  	{
  		dec_color.Rx = dec_color.Rx - dec_color.RANGE;
	}
  	if(dec_color.Rx < 0)
  	{
  		dec_color.Rx = 0;
	}    	
  	else if(dec_color.Rx > dec_color.MAXVAL)
  	{
  		dec_color.Rx = dec_color.MAXVAL;	
	}	
	/* update of bias-related variables B[Q] and C[Q] */
	if(dec_color.B[dec_color.q] <= -dec_color.N[dec_color.q])
	{
  		dec_color.B[dec_color.q] = dec_color.B[dec_color.q] + dec_color.N[dec_color.q];
   		if(dec_color.C[dec_color.q] > dec_color.MIN_C)
   		{
   			dec_color.C[dec_color.q] = dec_color.C[dec_color.q] - 1;
		}     		
   		if(dec_color.B[dec_color.q] <= -dec_color.N[dec_color.q])
   		{   			
     		dec_color.B[dec_color.q] = -dec_color.N[dec_color.q] + 1;
		}
 	}
 	else if(dec_color.B[dec_color.q] > 0) 
	{
   		dec_color.B[dec_color.q] = dec_color.B[dec_color.q] - dec_color.N[dec_color.q];
   		if(dec_color.C[dec_color.q] < dec_color.MAX_C)
   		{
     		dec_color.C[dec_color.q] = dec_color.C[dec_color.q] + 1;
		}
   		if(dec_color.B[dec_color.q] > 0)
   		{   			
    		dec_color.B[dec_color.q] = 0;
		}
 	}
 	dec_color.extended_data[current_component][dec_color.x[current_component].y][dec_color.x[current_component].x] = (unsigned char)dec_color.Rx;
}

/**********************************************************************
	PredictionErrorDecoding	:	This function perform prediction
								error decoding and computation.
**********************************************************************/
static void PredictionErrorDecoding()
{
	/* computation of the Golomg coding variable k */
    for(dec_color.k = 0; (dec_color.N[dec_color.q] << dec_color.k) < dec_color.A[dec_color.q]; dec_color.k++);
    /* decode MErrval */
    dec_color.MErrval = GolombDecoding(dec_color.k, dec_color.LIMIT);
    /* inverse of error mapinng */
    if((dec_color.NEAR == 0) && (dec_color.k==0) && (2*dec_color.B[dec_color.q] <= -dec_color.N[dec_color.q])) 
	{
    	if((dec_color.MErrval & 1))
    	{
    		dec_color.Errval = (dec_color.MErrval - 1) >> 1;	
		}      
    	else
    	{
    		dec_color.Errval = -(dec_color.MErrval >> 1) - 1;
		}      
  	}
  	else
	{
    	if((dec_color.MErrval & 1))
   		{
    		dec_color.Errval = -((dec_color.MErrval + 1) >> 1);
		}      
    	else
    	{
    		dec_color.Errval = dec_color.MErrval >> 1;
		}      
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
		if(dec_color.D[i] <= -dec_color.T3)
		{
			dec_color.Q[i] = -4;	
		} 
		else if(dec_color.D[i] <= -dec_color.T2) 
		{
			dec_color.Q[i] = -3;	
		}
		else if(dec_color.D[i] <= -dec_color.T1) 
		{
			dec_color.Q[i] = -2;	
		}
		else if(dec_color.D[i] < -dec_color.NEAR )
		{
			dec_color.Q[i] = -1;	
		} 
		else if(dec_color.D[i] <= dec_color.NEAR) 
		{
			dec_color.Q[i] = 0;	
		}
		else if(dec_color.D[i] < dec_color.T1)
		{
			dec_color.Q[i] = 1;	
		} 
		else if(dec_color.D[i] < dec_color.T2) 
		{
			dec_color.Q[i] = 2;	
		}
		else if(dec_color.D[i] < dec_color.T3)
		{
			dec_color.Q[i] = 3;
		} 
		else
		{
			dec_color.Q[i] = 4;
		} 
	}
	/* quantized gradient merging and q mapping*/
	if(dec_color.Q[0] < 0)
	{
	    dec_color.Q[0] = -dec_color.Q[0];
	    dec_color.Q[1] = -dec_color.Q[1];
	    dec_color.Q[2] = -dec_color.Q[2];
	   	dec_color.SIGN = -1;
	}
  	else if(dec_color.Q[0] == 0)
	{
        if(dec_color.Q[1] < 0)
		{
            dec_color.Q[1] = -dec_color.Q[1];
        	dec_color.Q[2] = -dec_color.Q[2];
        	dec_color.SIGN = -1;
		}
        else if(dec_color.Q[1] == 0 && dec_color.Q[2] < 0)
		{
        	dec_color.Q[2] = -dec_color.Q[2];
            dec_color.SIGN = -1;
		}
        else
		{
			dec_color.SIGN = 1;	
		} 
	}
  	else 
	{
		dec_color.SIGN = 1;	
	}
	dec_color.q = 81 * dec_color.Q[0] + 9 * dec_color.Q[1] + dec_color.Q[2];
	/* edge-detecting predictor */
	if(dec_color.Rc >= MAX(dec_color.Ra, dec_color.Rb))
	{
		dec_color.Px = MIN(dec_color.Ra, dec_color.Rb);	
	}   		
 	else 
	{
   		if(dec_color.Rc <= MIN(dec_color.Ra, dec_color.Rb))
   		{   			
     		dec_color.Px = MAX(dec_color.Ra, dec_color.Rb);
		}
   		else
   		{   			
     		dec_color.Px = dec_color.Ra + dec_color.Rb - dec_color.Rc;
		}
  	}
  	/* prediction correction from the bias */
  	if(dec_color.SIGN == 1)
  	{
  		dec_color.Px = dec_color.Px + dec_color.C[dec_color.q];	
	}   		
 	else
 	{
		dec_color.Px = dec_color.Px - dec_color.C[dec_color.q];
	}
 	if(dec_color.Px > dec_color.MAXVAL)
 	{
 		dec_color.Px = dec_color.MAXVAL;
	}   		
 	else if(dec_color.Px < 0)
 	{
 		dec_color.Px = 0;
	}	
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
     		if((dec_color.w - dec_color.x[current_component].x) + 1 >= (1 << dec_color.J[dec_color.RUNindex[current_component]]))
			{
        		for(i = 0; i < (1 << dec_color.J[dec_color.RUNindex[current_component]]); i++)
        		{
        			dec_color.extended_data[current_component][dec_color.x[current_component].y][dec_color.x[current_component].x + i] = (unsigned char)dec_color.Ra;	
				}
            	dec_color.x[current_component].x += (1 << dec_color.J[dec_color.RUNindex[current_component]]);        
        		if(dec_color.RUNindex[current_component] < 31)
				{
          			dec_color.RUNindex_prev[current_component] = dec_color.RUNindex[current_component];
          			dec_color.RUNindex[current_component]++;
				}
     		}
    	 	else
			{
        		for(i = dec_color.x[current_component].x; i <= dec_color.w; i++)
            	{
            		dec_color.extended_data[current_component][dec_color.x[current_component].y][i] = (unsigned char)dec_color.Ra;
				}			
        		dec_color.x[current_component].x = dec_color.w;
       			break;
	   		}
 		}
 		else
 		{ 			
  			for(i = 1; i <= dec_color.J[dec_color.RUNindex[current_component]]; i++)
  			{
  				cnt += (unsigned char)(GetFromBitStream() << (dec_color.J[dec_color.RUNindex[current_component]] - i));
			}     
  			for(i = 0; i < cnt; i++)
  			{
  				dec_color.extended_data[current_component][dec_color.x[current_component].y][dec_color.x[current_component].x + i] = (unsigned char)dec_color.Ra;
			}     
  			dec_color.x[current_component].x += cnt;
  			if(dec_color.RUNindex[current_component] > 0)
			{
          		dec_color.RUNindex_prev[current_component] = dec_color.RUNindex[current_component];
          		dec_color.RUNindex[current_component]--;
			}
  			run_flag = 1;
  			break;
  		}
 		if(dec_color.x[current_component].x > dec_color.w)
		{
        	dec_color.x[current_component].x = dec_color.w;
          	break;
		}
	}
	if (run_flag)
	{
		dec_color.x[current_component].x--;
		GetNextSample(); 
		/* index computation */	
  		if(dec_color.Ra == dec_color.Rb)
  		{
  			dec_color.RItype = 1;
		}
    	else
    	{    		
    		dec_color.RItype = 0;
		}
		/* computation of the auxiliary variable TEMP */
		if (dec_color.RItype == 0)
		{			
			dec_color.TEMP = dec_color.A[365];
		}
		else
		{
			dec_color.TEMP = dec_color.A[366] + (dec_color.N[366] >> 1);
		}
		/* Q computation */
		dec_color.q = dec_color.RItype + 365; 
		/* k computation */
		for(dec_color.k = 0; (dec_color.N[dec_color.q] << dec_color.k) < dec_color.TEMP; dec_color.k++);
		/* encoder EMErrval */
		dec_color.glimit = dec_color.LIMIT - dec_color.J[dec_color.RUNindex_prev[current_component]] - 1;
  		dec_color.EMErrval = GolombDecoding(dec_color.k, dec_color.glimit);
  		/* Errval computation */
  		if(dec_color.EMErrval & 1)
		{
			if(dec_color.RItype == 1)	
			{
				dec_color.map = 0;	
			}
			else
			{
				dec_color.map = 1;	
			}
			dec_color.Errval = (dec_color.EMErrval + 1) >> 1;
       	}
  		else
		{
    		if(dec_color.RItype == 1)
			{
      			dec_color.Errval = (dec_color.EMErrval + 2) >> 1;
      			dec_color.map = 1;
			}
    		else
			{
     	 		dec_color.Errval = dec_color.EMErrval >> 1;
      			dec_color.map = 0;
	  		}
		}
		/* correct the sign of the map fun. */
		if((dec_color.k == 0) && (dec_color.map == 0) && (2*dec_color.Nn[dec_color.q] < dec_color.N[dec_color.q]))
		{
			dec_color.Errval = -dec_color.Errval;
		}
		else if((dec_color.map == 1) && (2*dec_color.Nn[dec_color.q] >= dec_color.N[dec_color.q]))
		{
			dec_color.Errval = -dec_color.Errval;
		}		    
		else if((dec_color.map == 1) && (dec_color.k != 0))
		{
			dec_color.Errval = -dec_color.Errval;
		}
		/* Error computation for a run interruption sample */
 		if((dec_color.RItype == 0) && (dec_color.Rb < dec_color.Ra))
 		{
 			dec_color.Errval = -dec_color.Errval;
		}
		/* Adding prediction error for a run interruption sample */
		if(dec_color.RItype == 1)
		{
			dec_color.Rx = (dec_color.Errval + dec_color.Ra) % (dec_color.RANGE);
		}
		else
		{
			dec_color.Rx = (dec_color.Errval + dec_color.Rb) % (dec_color.RANGE);
		}
		if(dec_color.Rx < 0)
		{
			dec_color.Rx = dec_color.Rx + dec_color.RANGE;
		}    	
	  	else if (dec_color.Rx > dec_color.MAXVAL)
	  	{
	  		dec_color.Rx = dec_color.Rx - dec_color.RANGE;
		}
	  	if(dec_color.Rx < 0)
	  	{
	  		dec_color.Rx = 0;
		}    	
	  	else if(dec_color.Rx > dec_color.MAXVAL)
	  	{
	  		dec_color.Rx = dec_color.MAXVAL;	
		}	
		dec_color.extended_data[current_component][dec_color.x[current_component].y][dec_color.x[current_component].x] = (unsigned char)dec_color.Rx;		
		/* update of parameters for run interruption sample */
  		if(dec_color.Errval < 0)
  		{  			
   			dec_color.Nn[dec_color.q] = dec_color.Nn[dec_color.q] + 1;
		}
	 	dec_color.A[dec_color.q] = dec_color.A[dec_color.q] + ((dec_color.EMErrval + 1 - dec_color.RItype) >> 1);
  		if(dec_color.N[dec_color.q] == dec_color.RESET)
		{
       		dec_color.A[dec_color.q] = dec_color.A[dec_color.q] >> 1;
       		dec_color.N[dec_color.q] = dec_color.N[dec_color.q] >> 1;
       		dec_color.Nn[dec_color.q] = dec_color.Nn[dec_color.q] >> 1;
  		}
  		dec_color.N[dec_color.q] = dec_color.N[dec_color.q] + 1;
	}
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
	ContextModeling	:	This function performs context modelling 
						procedure.
**********************************************************************/
static int ContextModeling()
{
	/* local gradient computation */
    dec_color.D[0] = dec_color.Rd - dec_color.Rb;
    dec_color.D[1] = dec_color.Rb - dec_color.Rc;
    dec_color.D[2] = dec_color.Rc - dec_color.Ra;
    /* mode selection */
 	if (dec_color.D[0] == 0 && dec_color.D[1] == 0 && dec_color.D[2] == 0)
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
		dec_color.bitstream[i] = 0;
	}
   	byte_count = size;
   	for (i = 0; i < byte_count; i++)
   	{
   		dec_color.bitstream[i] = data[i];
	}	
   	current_component = 0;
	read_position = -1;
	byte_count = -1;
	/* initialisations of appropriate values */
	dec_color.RANGE = 256;
	dec_color.MAXVAL = 255;
	dec_color.qbpp = 8;
	dec_color.bpp = 8;
	dec_color.LIMIT = 32; 
	dec_color.w = w;
	dec_color.h = h;
	for(i = 0; i < 365; i++)
	{
    	dec_color.A[i] = 4;
      	dec_color.N[i] = 1;
      	dec_color.B[i] = 0;
      	dec_color.C[i] = 0;
      	dec_color.Nn[i] = 0;
	}
	for(i = 0; i < COMPONENTS_COUNT; i++)
	{
	   	dec_color.x[i].x = 0;
    	dec_color.x[i].y = 1;	
    	dec_color.D[i] = 0;
		dec_color.Q[i] = 0;
		dec_color.RUNindex[i] = 0;
		dec_color.RUNindex_prev[i] = 0;
	}
	dec_color.A[365] = 4;
    dec_color.A[366] = 4;
    dec_color.N[365] = 1;
    dec_color.N[366] = 1;
    dec_color.Nn[365] = 0;
    dec_color.Nn[366] = 0;
    dec_color.Ra = 0;
	dec_color.Rb = 0;
	dec_color.Rc = 0;
	dec_color.Rd = 0;
	dec_color.Px = 0;
	dec_color.Rx = 0; 
    dec_color.RUNval = 0;  
	dec_color.RUNcnt = 0;     
	dec_color.RItype = 0;  
	dec_color.glimit = 0;
	dec_color.SIGN = 0;
	dec_color.TEMP = 0;
	dec_color.Errval = 0;
	dec_color.EMErrval = 0;
	dec_color.MErrval;
	dec_color.q = 0;
	dec_color.k = 0;
	dec_color.map = 0;	
    dec_color.RESET = 64;
    dec_color.T1 = 3;
    dec_color.T2 = 7;
    dec_color.T3 = 21;
    dec_color.MAX_C = 127;
    dec_color.MIN_C = -128;
    dec_color.NEAR = 0;
    dec_color.J[0] = 0; dec_color.J[1] = 0; dec_color.J[2] = 0; dec_color.J[3] = 0; dec_color.J[4] = 1; dec_color.J[5] = 1; dec_color.J[6] = 1; dec_color.J[7] = 1; 
    dec_color.J[8] = 2; dec_color.J[9] = 2; dec_color.J[10] = 2; dec_color.J[11] = 2; dec_color.J[12] = 3; dec_color.J[13] = 3; dec_color.J[14] = 3; dec_color.J[15] = 3; 
    dec_color.J[16] = 4; dec_color.J[17] = 4; dec_color.J[18] = 5; dec_color.J[19] = 5; dec_color.J[20] = 6; dec_color.J[21] = 6; dec_color.J[22] = 7; dec_color.J[23] = 7;     
    dec_color.J[24] = 8; dec_color.J[25] = 9; dec_color.J[26] = 10; dec_color.J[27] = 11; dec_color.J[28] = 12; dec_color.J[29] = 13; dec_color.J[30] = 14; dec_color.J[31] = 15;    
	/* allocate data for non defined samples */	
    dec_color.raw_data = Allocate3D(w, h); 
    /* initialize non defined samples */
    dec_color.extended_data = InitializeNonDefinedSamples(dec_color.raw_data, w, h);
}

/**********************************************************************
	DecodeColorLineIlv	:	This function decode input color bitstream 
						line interleaved into raw data.
						data - pointer to input data
						w - width of decoded data
						h - height of decoded data
						size - bitstream size
						return NULL if cannot encode input data
						return pointer to compressed data
**********************************************************************/
char*** DecodeColorLineIlv(char* data, int w, int h, int size)
{
	int i, j, k;
	char*** dout;
	dout = Allocate3D(w, h);
	/* Initializations */
	Initializations(data, w, h, size);
    /* for each sample */
    for(;;)
    {
    	/* check if all sample proceeded */
		if(dec_color.x[COMPONENTS_COUNT - 1].x == w && dec_color.x[COMPONENTS_COUNT - 1].y == h)
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
	for (k = 0; k < COMPONENTS_COUNT; k++)
	{		
		for (i = 0; i < h; i++)
		{
			for (j = 0; j < w; j++)
			{
				dout[k][i][j] = dec_color.extended_data[k][i + 1][j + 1];
			}
		}
	}
	Free3D(dec_color.extended_data);
	Free3D(dec_color.raw_data);
	dec_color.extended_data = NULL;
	dec_color.raw_data = NULL;
	return dout;
}
