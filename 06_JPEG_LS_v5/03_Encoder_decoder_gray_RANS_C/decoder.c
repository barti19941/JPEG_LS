#include "decoder.h"

static decoder dec;
static uint8_t current_read_byte;
static int8_t read_position;
static int32_t byte_count;

static uint8_t GetFromBitStream()
{
	uint8_t bit_set_mask[] = 
	{	
		0x01, 0x02, 0x04, 0x08,	0x10, 0x20, 0x40, 0x80
	};
  	if (read_position > 7)
    { 	
		byte_count = byte_count - 1;
      	current_read_byte = dec.bitstream[byte_count];
		read_position = 0; 	
    }
  	if (current_read_byte & bit_set_mask[read_position++])
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
	dec.w = w;
	dec.h = h;
	dec.coding_mode = coding_mode;
	dec.bitstream = (uint8_t*)calloc(size, sizeof(uint8_t));
	memcpy(dec.bitstream, data, size);
	dec.extended_data = (uint8_t*)calloc((dec.w + 2) * (dec.h + 1), sizeof(uint8_t));
	current_read_byte = 0;
	read_position = 8;
	byte_count = size;
	dec.RANGE = 256;
	dec.MAXVAL = 255;
	dec.qbpp = 8;
	dec.bpp = 8;
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
	for(i = 0; i < 3; i++)
	{
		dec.D[i] = 0;
		dec.Q[i] = 0;
	}
    dec.Ra = 0;
	dec.Rb = 0;
	dec.Rc = 0;
	dec.Rd = 0;
	dec.Px = 0;
	dec.Rx = 0; 
	dec.RItype = 0;  	
	dec.SIGN = 0;
	dec.TEMP = 0;
	dec.Errval = 0;
	dec.EMErrval = 0;
	dec.MErrval = 0;
	dec.q = 0;
	dec.k = 0;
	dec.map = 0;	
    dec.p.x = 0;
    dec.p.y = 1;	
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
	double p_f = 0;	
	uint32_t sum = 0;	
	dec.x = 65536;
    dec.l = 65536;
    dec.max = 256;
    dec.b = 2;
    dec.d = 1;
    dec.n = 16;
    dec.m = 1;
    dec.symbols = (uint16_t*)calloc(dec.max + 1, sizeof(uint16_t));
	for (i = 0; i < dec.max + 1; i++)
	{
		dec.symbols[i] = i;
	}
	p_f = (double)dec.bitstream[0]/(double)100;
	dec.fs = (uint16_t*)calloc(dec.max + 1, sizeof(uint16_t));
	for (i = 0; i < dec.max + 1; i++)
	{
		dec.fs[i] = (uint16_t)((pow((1 - p_f), i) * p_f) * dec.l);
		if (dec.fs[i] == 0)
		{
			dec.fs[i] = 1;
		}
		sum += dec.fs[i];
	}
	i = 0;
	if (sum > dec.l)
	{
		while (sum != dec.l)
		{
			sum = 0;
			if (dec.fs[i] != 1)
			{
				dec.fs[i] = dec.fs[i] - 1;	
			}		
			for (j = 0; j < dec.max + 1; j++)
			{
				sum += dec.fs[j];
			}
			i++;
			if (i == (dec.max + 1))
			{
				i = 0;
			}
		}
	}
	else if (sum < dec.l)
	{
		while (sum != dec.l)
		{			
			sum = 0;
			dec.fs[i] = dec.fs[i] + 1;	
			for (j = 0; j < dec.max + 1; j++)
			{
				sum += dec.fs[j];
			}
			i++;
			if (i == (dec.max + 1))
			{
				i = 0;
			}
		}
	}
	dec.cs = (uint16_t*)calloc(dec.max + 1, sizeof(uint16_t));
	dec.cs[0] = 0;
	for (i = 1; i < dec.max + 1; i++)
	{
		for(j = 0; j < i; j++)
		{
			dec.cs[i] += dec.fs[j];
		}			
	}
	dec.symbols_inv = (uint16_t*)calloc(dec.l, sizeof(uint16_t));
	for (i = 0; i < dec.l; i++)
	{
		for (j = 0; j < dec.max + 1; j++)
		{
			if (dec.cs[dec.symbols[j]] <= i)
			{
				dec.symbols_inv[i] = dec.symbols[j];
			}
		}
	}
}

static void Close()
{	
	free(dec.fs);
	free(dec.cs);
	free(dec.symbols);
	free(dec.symbols_inv);
	free(dec.bitstream);
	free(dec.extended_data);
}

static void GetNextSample()
{
	if(dec.p.x == dec.w)
	{
    	dec.p.y++;
    	dec.p.x = 1;
	}
 	else
 	{
 		dec.p.x++;
	}
	dec.Rb = dec.extended_data[(dec.p.y - 1) * (dec.w + 2) + dec.p.x];
	if(dec.p.x == 1 && dec.p.y > 1)
	{
	    dec.Ra = dec.Rb;
	    dec.Rc = dec.extended_data[(dec.p.y - 2) * (dec.w + 2) + dec.p.x];
	    if (dec.w == 1)
	    {
	    	dec.Rd = dec.Rb;
		}
		else
		{
			dec.Rd = dec.extended_data[(dec.p.y - 1) * (dec.w + 2) + dec.p.x + 1];
		}   
	}
 	else if(dec.p.x == dec.w)
 	{
 		dec.Ra = dec.extended_data[dec.p.y * (dec.w + 2) + dec.p.x - 1];
 		dec.Rc = dec.extended_data[(dec.p.y - 1) * (dec.w + 2) + dec.p.x - 1];
	    dec.Rd = dec.Rb;
	}
 	else
	{ 
	 	dec.Ra = dec.extended_data[dec.p.y * (dec.w + 2) + dec.p.x - 1];
	 	dec.Rc = dec.extended_data[(dec.p.y - 1) * (dec.w + 2) + dec.p.x - 1];
	 	dec.Rd = dec.extended_data[(dec.p.y - 1) * (dec.w + 2) + dec.p.x + 1];
	}
}

static int ContextModeling()
{
    dec.D[0] = dec.Rd - dec.Rb;
    dec.D[1] = dec.Rb - dec.Rc;
    dec.D[2] = dec.Rc - dec.Ra;
 	if (dec.D[0] == 0 && dec.D[1] == 0 && dec.D[2] == 0)
 	{
  		return 1;
	}
	else
	{			
  		return 0;
	}
}

static uint16_t rAnsDecoding()
{
	uint16_t data = 0;
	data = dec.symbols_inv[dec.x & (dec.l - 1)];
	dec.x = dec.fs[data] * (dec.x >> dec.n) + (dec.x & (dec.l - 1)) - dec.cs[data];
	while (dec.x < dec.l)
	{
		dec.x = (dec.x << 1) + GetFromBitStream();	
	}
	return data;
}

static void rAnsDecodingFirst()
{
	uint8_t i = 0;
	while(GetFromBitStream() == 0);
	dec.x = 1;
	for (i = 0; i < dec.n; i++)
	{
		dec.x = (dec.x << 1) + GetFromBitStream();
	}
}

static void RunModeProcessing()
{
	uint8_t R = 0;
	uint16_t cnt = 0;
	uint16_t i = 0;
	uint8_t run_flag = 0;
	do
	{
		cnt = rAnsDecoding();
		for(i = 0; i < cnt; i++)
		{
			dec.extended_data[dec.p.y * (dec.w + 2) + dec.p.x + i] = dec.Ra;	
		}
		dec.p.x += cnt;
	} 
	while (cnt == dec.RANGE);
	if (dec.p.x > dec.w)
	{
		dec.p.x = dec.w;
		run_flag = 0;
	}
	else
	{
		run_flag = 1;
	}
	if (run_flag)
	{
		dec.p.x--;
		GetNextSample(); 
		if(dec.Ra == dec.Rb)
		{
			dec.RItype = 1;
		}
		else
		{    		
			dec.RItype = 0;
		}
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
		dec.EMErrval = rAnsDecoding();
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
		if((dec.RItype == 0) && (dec.Rb < dec.Ra))
		{
			dec.Errval = -dec.Errval;
		}
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
		dec.extended_data[dec.p.y * (dec.w + 2) + dec.p.x] = (uint8_t)dec.Rx;		
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

static void Prediction()
{
	uint8_t i = 0;
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
		else if(dec.D[i] < -0 )
		{
			dec.Q[i] = -1;	
		} 
		else if(dec.D[i] <= 0) 
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
	dec.MErrval = rAnsDecoding();
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

static void VariablesUpdate()
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
 	dec.extended_data[dec.p.y * (dec.w + 2) + dec.p.x] = dec.Rx;
}

static void RegularModeProcessing()
{
	Prediction();
	PredictionErrorDecoding();
	VariablesUpdate();
}

uint8_t* Decode(uint8_t* data, uint16_t w, uint16_t h, uint32_t size, uint8_t coding_mode)
{	
	uint8_t* dout;
	uint16_t i = 0;
	uint16_t j = 0;
	dout = (uint8_t*)calloc(w * h, sizeof(uint8_t));
	Initializations(data, w, h, size, coding_mode);
	InitializeAns();
	rAnsDecodingFirst();
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
	for (i = 0; i < h; i++)
	{
		for (j = 0; j < w; j++)
		{
			dout[(i * w) + j] = dec.extended_data[(i + 1) * (w + 2) + j + 1];
		}
	}
	Close();
	return dout;
}
