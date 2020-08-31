#include "decoder.h"

static decoder dec;

static uint8_t current_read_byte_regular;
static int8_t read_position_regular;
static int32_t byte_count_regular;
static uint8_t current_read_byte_run;
static int8_t read_position_run;
static int32_t byte_count_run;

static uint8_t GetFromBitStreamRun()
{
	uint8_t bit_set_mask[] = 
	{	
		0x01, 0x02, 0x04, 0x08,	0x10, 0x20, 0x40, 0x80
	};
  	if (read_position_run < 0)
    { 	
		byte_count_run = byte_count_run + 1;	
      	current_read_byte_run = dec.bitstream[byte_count_run];	    
   		read_position_run = 7;	
    }
  	if (current_read_byte_run & bit_set_mask[read_position_run--])
	{
    	return(1);
    }
    else
    {    	
  		return(0);
	}
}

static uint8_t GetFromBitStreamRegular()
{
	uint8_t bit_set_mask[] = 
	{	
		0x01, 0x02, 0x04, 0x08,	0x10, 0x20, 0x40, 0x80
	};
  	if (read_position_regular > 7)
    { 	
		byte_count_regular = byte_count_regular - 1;	
      	current_read_byte_regular = dec.bitstream[byte_count_regular];	    
   		read_position_regular = 0;	
    }
  	if (current_read_byte_regular & bit_set_mask[read_position_regular++])
	{
    	return(1);
    }
    else
    {    	
  		return(0);
	}
}

static void Initializations(uint8_t* data, uint16_t w, uint16_t h, uint32_t size, uint8_t coding_mode)
{
	uint32_t i = 0;	
	dec.coding_mode = coding_mode;	
	dec.w = w;
	dec.h = h;
	dec.bitstream = (uint8_t*)calloc(size, sizeof(uint8_t));
	memcpy(dec.bitstream, data, size);
	dec.extended_data = (uint8_t*)calloc((dec.w + 2) * (dec.h + 1) * 3, sizeof(uint8_t));
	current_read_byte_regular = 0;
	read_position_regular = 8;
	byte_count_regular = size;
	current_read_byte_run = 0;
	read_position_run = -1;
	byte_count_run = 0;
	dec.RANGE = 256;
	dec.MAXVAL = 255;
	dec.qbpp = 8;
	dec.bpp = 8;
	dec.LIMIT = 32; 
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
	dec.p.x = 0;
    dec.p.y = 1;	
	for(i = 0; i < 3; i++)
	{	
		dec.D[0][i] = 0;
		dec.D[1][i] = 0;
    	dec.D[2][i] = 0;
		dec.Q[i] = 0;
		dec.Ra[i] = 0;
		dec.Rb[i] = 0;
		dec.Rc[i] = 0;
		dec.Rd[i] = 0;
	}
	dec.J[0] = 0; 	dec.J[1] = 0; 	dec.J[2] = 0; 	dec.J[3] = 0; 	dec.J[4] = 1; 	dec.J[5] = 1; 	dec.J[6] = 1; 	dec.J[7] = 1; 
    dec.J[8] = 2; 	dec.J[9] = 2; 	dec.J[10] = 2; 	dec.J[11] = 2; 	dec.J[12] = 3; 	dec.J[13] = 3; 	dec.J[14] = 3; 	dec.J[15] = 3; 
    dec.J[16] = 4; 	dec.J[17] = 4; 	dec.J[18] = 5; 	dec.J[19] = 5; 	dec.J[20] = 6; 	dec.J[21] = 6; 	dec.J[22] = 7; 	dec.J[23] = 7;     
    dec.J[24] = 8; 	dec.J[25] = 9; 	dec.J[26] = 10; dec.J[27] = 11; dec.J[28] = 12; dec.J[29] = 13; dec.J[30] = 14; dec.J[31] = 15;  
	dec.Px = 0;
	dec.Rx = 0;
	dec.RUNindex = 0;
	dec.RUNindex_prev = 0;
	dec.RItype = 0;  	
	dec.SIGN = 0;
	dec.TEMP = 0;
	dec.Errval = 0;
	dec.EMErrval = 0;
	dec.MErrval = 0;
	dec.q = 0;
	dec.k = 0;
	dec.map = 0;	
    dec.RESET = 64;
    dec.T1 = 3;
    dec.T2 = 7;
    dec.T3 = 21;    
    dec.MAX_C = 127;
    dec.MIN_C = -128;
}

static void InitializeAns()
{
	uint32_t i = 0;		
	uint32_t j = 0;
	uint32_t k = 0;
	uint32_t X = 0;	
	double p_f = 0;	
	uint32_t sum = 0;	
    dec.L = 65536;    
	dec.x = 65536;	
    dec.max = 256;
    dec.step = 40963;
    dec.R = 16;
    dec.r = 17;
    dec.s = (uint16_t*)calloc(dec.max + 1, sizeof(uint16_t));
	for (i = 0; i < dec.max + 1; i++)
	{
		dec.s[i] = i;
	}	
	p_f = (double)dec.bitstream[0]/(double)100;
	dec.Ls = (uint16_t*)calloc(dec.max + 1, sizeof(uint16_t));
	for (i = 0; i < dec.max + 1; i++)
	{
		dec.Ls[i] = (uint16_t)((pow((1 - p_f), i) * p_f) * dec.L);
		if (dec.Ls[i] == 0)
		{
			dec.Ls[i] = 1;
		}
		sum += dec.Ls[i];
	}
	i = 0;
	if (sum > dec.L)
	{
		while (sum != dec.L)
		{
			sum = 0;
			if (dec.Ls[i] != 1)
			{
				dec.Ls[i] = dec.Ls[i] - 1;	
			}		
			for (j = 0; j < dec.max + 1; j++)
			{
				sum += dec.Ls[j];
			}
			i++;
			if (i == (dec.max + 1))
			{
				i = 0;
			}
		}
	}
	else if (sum < dec.L)
	{
		while (sum != dec.L)
		{			
			sum = 0;
			dec.Ls[i] = dec.Ls[i] + 1;	
			for (j = 0; j < dec.max + 1; j++)
			{
				sum += dec.Ls[j];
			}
			i++;
			if (i == (dec.max + 1))
			{
				i = 0;
			}
		}
	}
	dec.symbols = (uint16_t*)calloc(dec.L, sizeof(uint16_t));
	for (i = 0; i < dec.max + 1; i++)
	{
		for(j = k; j < dec.Ls[dec.s[i]] + k; j++)
		{
			dec.symbols[j] = dec.s[i];			
		}
		k += dec.Ls[dec.s[i]];
	}
	for (i = 0; i < dec.max + 1; i++)
	{
		for (j = 0; j < dec.Ls[dec.s[i]]; j++)
		{
			dec.symbols[X] = dec.s[i];
			X = (X + dec.step) & (dec.L - 1);
		}
	}
	dec.m = (uint8_t*)calloc(dec.max + 1, sizeof(uint8_t));
	dec.nb = (uint32_t*)calloc(dec.max + 1, sizeof(uint32_t));
	for (i = 0; i < dec.max + 1; i++)
	{
		dec.m[i] = dec.R - (uint8_t)floor((double)log2((double)dec.Ls[i]));
		dec.nb[i] = (dec.m[i] << dec.r) - (dec.Ls[i] << dec.m[i]);
	}
	dec.next = (uint32_t*)calloc(dec.max + 1, sizeof(uint32_t));
	for (i = 0; i < dec.max + 1; i++)
	{
		dec.next[i] = dec.Ls[i];
	}
	dec.decodingTable = (t*)calloc(dec.L, sizeof(t));
	for (i = 0; i < dec.L; i++)
	{
		dec.decodingTable[i].symbol = dec.symbols[i];
		j = dec.next[dec.decodingTable[i].symbol]++;
		dec.decodingTable[i].nbBits = dec.R - (uint16_t)(floor(log2(j)));
		dec.decodingTable[i].newX = (j << dec.decodingTable[i].nbBits);
	}
	dec.x = 0;
}

static void Close()
{	
	free(dec.decodingTable);	
	free(dec.next);
	free(dec.m);
	free(dec.nb);
	free(dec.symbols);
	free(dec.Ls);
	free(dec.s);
	free(dec.bitstream);
	free(dec.extended_data);
}

static void ModifyPixels(uint8_t* data, uint16_t w, uint16_t h)
{
	uint16_t i = 0;
	uint16_t j = 0;
	for (i = 0; i < h; i++)
	{
		for (j = 0; j < w; j++)
		{
			data[(i * w * 3) + (j * 3) + 1] = data[(i * w * 3) + (j * 3) + 0] - data[(i * w * 3) + (j * 3) + 1];
			data[(i * w * 3) + (j * 3) + 2] = data[(i * w * 3) + (j * 3) + 0] - data[(i * w * 3) + (j * 3) + 2];
		}
	}
}

static void GetNextSample()
{
	uint8_t i = 0;
	if(dec.p.x == dec.w)
	{
    	dec.p.y++;
    	dec.p.x = 1;
	}
 	else
 	{
 		dec.p.x++;
	}
	for (i = 0; i < 3; i++)
	{
		dec.Rb[i] = dec.extended_data[((dec.p.y - 1) * (dec.w + 2) * 3) + (dec.p.x * 3) + i];
		if(dec.p.x == 1 && dec.p.y > 1)
		{
		    dec.Ra[i] = dec.Rb[i];
		    dec.Rc[i] = dec.extended_data[((dec.p.y - 2) * (dec.w + 2) * 3) + (dec.p.x * 3) + i];
		    if (dec.w == 1)
		    {
		    	dec.Rd[i] = dec.Rb[i];
			}
			else
			{
				dec.Rd[i] = dec.extended_data[((dec.p.y - 1) * (dec.w + 2) * 3) + ((dec.p.x + 1) * 3) + i];
			}   
		}
	 	else if(dec.p.x == dec.w)
	 	{
	 		dec.Ra[i] = dec.extended_data[(dec.p.y * (dec.w + 2) * 3) + ((dec.p.x - 1) * 3) + i];
	 		dec.Rc[i] = dec.extended_data[((dec.p.y - 1) * (dec.w + 2) * 3) + ((dec.p.x - 1) * 3) + i];
		    dec.Rd[i] = dec.Rb[i];
		}
	 	else
		{ 
		 	dec.Ra[i] = dec.extended_data[(dec.p.y * (dec.w + 2) * 3) + ((dec.p.x - 1) * 3) + i];
		 	dec.Rc[i] = dec.extended_data[((dec.p.y - 1) * (dec.w + 2) * 3) + ((dec.p.x - 1) * 3) + i];
		 	dec.Rd[i] = dec.extended_data[((dec.p.y - 1) * (dec.w + 2) * 3) + ((dec.p.x + 1) * 3) + i];
		}	
	}
}

static int ContextModeling()
{
	uint8_t i = 0;
	for (i = 0; i < 3; i++)
	{
		dec.D[i][0] = dec.Rd[i] - dec.Rb[i];
    	dec.D[i][1] = dec.Rb[i] - dec.Rc[i];
    	dec.D[i][2] = dec.Rc[i] - dec.Ra[i];
	}
 	if (dec.D[0][0] == 0 && dec.D[0][1] == 0 && dec.D[0][2] == 0 && 
	 	dec.D[1][0] == 0 && dec.D[1][1] == 0 && dec.D[1][2] == 0 &&
		dec.D[2][0] == 0 && dec.D[2][1] == 0 && dec.D[2][2] == 0)
 	{
  		return 1;
	}
	else
	{			
  		return 0;
	}
}

static uint16_t tAnsDecoding()
{
	uint16_t data = 0;
	uint16_t i = 0;
	uint32_t tmp = 0;
	data = dec.decodingTable[dec.x - dec.L].symbol;
	tmp = 0;
	for (i = 0; i < dec.decodingTable[dec.x - dec.L].nbBits; i++)
	{
		tmp = (tmp << 1) + GetFromBitStreamRegular();
	}	
	dec.x = dec.decodingTable[dec.x - dec.L].newX + tmp;
	return data;
}

static void tAnsDecodingFirst()
{
	uint8_t i = 0;
	while(GetFromBitStreamRegular() == 0);
	dec.x = 1;
	for (i = 0; i < dec.R; i++)
	{
		dec.x = (dec.x << 1) + GetFromBitStreamRegular();
	}
}

static void RunModeProcessing()
{
	uint8_t R = 0;
	uint16_t cnt = 0;
	uint16_t i = 0;
	uint8_t run_flag = 0;
	for (;;)
	{
		run_flag = 0;
		R = GetFromBitStreamRun();
 		if(R)
		{			
     		if((dec.w - dec.p.x) + 1 >= (1 << dec.J[dec.RUNindex]))
			{
        		for(i = 0; i < (1 << dec.J[dec.RUNindex]); i++)
        		{
        			dec.extended_data[(dec.p.y * (dec.w + 2) * 3) + ((dec.p.x + i) * 3) + 0] = dec.Ra[0];
					dec.extended_data[(dec.p.y * (dec.w + 2) * 3) + ((dec.p.x + i) * 3) + 1] = dec.Ra[1];
					dec.extended_data[(dec.p.y * (dec.w + 2) * 3) + ((dec.p.x + i) * 3) + 2] = dec.Ra[2];	
				}
            	dec.p.x += (1 << dec.J[dec.RUNindex]);        
        		if(dec.RUNindex < 31)
				{
          			dec.RUNindex_prev = dec.RUNindex;
          			dec.RUNindex++;
				}
     		}
    	 	else
			{
        		for(i = dec.p.x; i <= dec.w; i++)
            	{
            		dec.extended_data[(dec.p.y * (dec.w + 2) * 3) + (i * 3) + 0] = dec.Ra[0];
            		dec.extended_data[(dec.p.y * (dec.w + 2) * 3) + (i * 3) + 1] = dec.Ra[1];
            		dec.extended_data[(dec.p.y * (dec.w + 2) * 3) + (i * 3) + 2] = dec.Ra[2];
				}			
        		dec.p.x = dec.w;
       			break;
	   		}
 		}
 		else
 		{ 			
  			for(i = 1; i <= dec.J[dec.RUNindex]; i++)
  			{
  				cnt += (GetFromBitStreamRun() << (dec.J[dec.RUNindex] - i));
			}     
  			for(i = 0; i < cnt; i++)
  			{
  				dec.extended_data[(dec.p.y * (dec.w + 2) * 3) + ((dec.p.x + i) * 3) + 0] = dec.Ra[0];
  				dec.extended_data[(dec.p.y * (dec.w + 2) * 3) + ((dec.p.x + i) * 3) + 1] = dec.Ra[1];
  				dec.extended_data[(dec.p.y * (dec.w + 2) * 3) + ((dec.p.x + i) * 3) + 2] = dec.Ra[2];
			}     
  			dec.p.x += cnt;
  			if(dec.RUNindex > 0)
			{
          		dec.RUNindex_prev = dec.RUNindex;
          		dec.RUNindex--;
			}
  			run_flag = 1;
  			break;
  		}
 		if(dec.p.x > dec.w)
		{
        	dec.p.x = dec.w;
          	break;
		}
	}
	if (run_flag)
	{
		dec.p.x--;
		GetNextSample(); 
		for (i = 0; i < 3; i++)
		{
	    	dec.RItype = 0;
			if (dec.RItype == 0)
			{			
				dec.TEMP = dec.A[365];
			}
			else
			{
				dec.TEMP = dec.A[366] + (dec.N[366] >> 1);
			}
			dec.q = dec.RItype + 365; 
			for(dec.k = 0; (dec.N[dec.q] << dec.k) < dec.TEMP; dec.k++);
			dec.EMErrval = tAnsDecoding();
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
			if((dec.k == 0) && (dec.map == 0) && (2 * dec.Nn[dec.q] < dec.N[dec.q]))
			{
				dec.Errval = -dec.Errval;
			}
			else if((dec.map == 1) && (2 * dec.Nn[dec.q] >= dec.N[dec.q]))
			{
				dec.Errval = -dec.Errval;
			}		    
			else if((dec.map == 1) && (dec.k != 0))
			{
				dec.Errval = -dec.Errval;
			}
			if(dec.Errval < 0)
	  		{  			
	   			dec.Nn[dec.q] = dec.Nn[dec.q] + 1;
			}
	 		if((dec.RItype == 0) && (dec.Rb[i] < dec.Ra[i]))
	 		{
	 			dec.Errval = -dec.Errval;
			}
			if(dec.RItype == 1)
			{
				dec.Rx = (dec.Errval + dec.Ra[i]) % (dec.RANGE);
			}
			else
			{
				dec.Rx = (dec.Errval + dec.Rb[i]) % (dec.RANGE);
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
			dec.extended_data[(dec.p.y * (dec.w + 2) * 3) + (dec.p.x * 3) + i] = (uint8_t)dec.Rx;		
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
}

static void Prediction(uint8_t cmp)
{
	uint8_t i = 0;
	for(i = 0; i < 3; i++)
	{
		if(dec.D[cmp][i] <= -dec.T3)
		{
			dec.Q[i] = -4;	
		} 
		else if(dec.D[cmp][i] <= -dec.T2) 
		{
			dec.Q[i] = -3;	
		}
		else if(dec.D[cmp][i] <= -dec.T1) 
		{
			dec.Q[i] = -2;	
		}
		else if(dec.D[cmp][i] < -0 )
		{
			dec.Q[i] = -1;	
		} 
		else if(dec.D[cmp][i] <= 0) 
		{
			dec.Q[i] = 0;	
		}
		else if(dec.D[cmp][i] < dec.T1)
		{
			dec.Q[i] = 1;	
		} 
		else if(dec.D[cmp][i] < dec.T2) 
		{
			dec.Q[i] = 2;	
		}
		else if(dec.D[cmp][i] < dec.T3)
		{
			dec.Q[i] = 3;
		} 
		else
		{
			dec.Q[i] = 4;
		} 
	}
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
	if(dec.Rc[cmp] >= MAX(dec.Ra[cmp], dec.Rb[cmp]))
	{
		dec.Px = MIN(dec.Ra[cmp], dec.Rb[cmp]);	
	}   		
 	else 
	{
   		if(dec.Rc[cmp] <= MIN(dec.Ra[cmp], dec.Rb[cmp]))
   		{   			
     		dec.Px = MAX(dec.Ra[cmp], dec.Rb[cmp]);
		}
   		else
   		{   			
     		dec.Px = dec.Ra[cmp] + dec.Rb[cmp] - dec.Rc[cmp];
		}
  	}
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

static void PredictionErrorDecoding()
{
    for(dec.k = 0; (dec.N[dec.q] << dec.k) < dec.A[dec.q]; dec.k++);
	dec.MErrval = tAnsDecoding();
    if((dec.k == 0) && (2 * dec.B[dec.q] <= -dec.N[dec.q])) 
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

static void VariablesUpdate(uint8_t cmp)
{	
  	dec.B[dec.q] = dec.B[dec.q] + dec.Errval;
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
			dec.B[dec.q] = -((1 - dec.B[dec.q]) >> 1);
		}			
		dec.N[dec.q] = dec.N[dec.q] >> 1;
	}
	dec.N[dec.q] = dec.N[dec.q] + 1;
	if(dec.SIGN == -1)
	{
		dec.Errval = -dec.Errval;
	}
	dec.Rx = (dec.Errval + dec.Px) % (dec.RANGE);
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
 	dec.extended_data[(dec.p.y * (dec.w + 2) * 3) + (dec.p.x * 3) + cmp] = dec.Rx;
}

static void RegularModeProcessing()
{
	uint8_t i = 0;
	for (i = 0; i < 3; i++)
	{
		Prediction(i);
		PredictionErrorDecoding();
		VariablesUpdate(i);
	}
}

uint8_t* Decode(uint8_t* data, uint16_t w, uint16_t h, uint32_t size, uint8_t coding_mode, uint8_t pixels_modify)
{	
	uint8_t* dout;
	uint32_t i = 0;
	uint32_t j = 0;
	uint8_t k = 0;
	dout = (uint8_t*)calloc(3 * w * h, sizeof(uint8_t));
	Initializations(data, w, h, size, coding_mode);
	InitializeAns();
	tAnsDecodingFirst();
	for(;;)
	{
		if(dec.p.x == w && dec.p.y == h)
    	{    			    	
        	break;
		} 			
		GetNextSample();
		if (ContextModeling())
		{
			if (dec.coding_mode == 1)
			{
				RunModeProcessing();
			}
			else
			{
				RegularModeProcessing();
			}			
		}
		else
		{
			RegularModeProcessing();
		}	
	}
	for (k = 0; k < 3; k++)
	{
		for (i = 0; i < h; i++)
		{
			for (j = 0; j < w; j++)
			{
				dout[(i * w * 3) + (j * 3) + k] = dec.extended_data[((i + 1) * (w + 2) * 3) + ((j + 1) * 3) + k];
			}
		}
	}
	Close();
	if (pixels_modify == 1)
	{
		ModifyPixels(dout, w, h);
	}	
	return dout;
}
