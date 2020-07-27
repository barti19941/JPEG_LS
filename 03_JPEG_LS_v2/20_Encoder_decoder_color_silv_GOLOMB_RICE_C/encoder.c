#include "encoder.h"

static encoder enc;
static uint8_t current_write_byte;
static int8_t write_position;
static uint32_t byte_count;

static void Initializations(uint8_t* data, uint16_t w, uint16_t h, uint8_t coding_mode)
{
	uint32_t i = 0;
	uint32_t j = 0;
	uint8_t k = 0;
	enc.coding_mode = coding_mode;
	enc.w = w;
    enc.h = h;
	enc.bitstream = (uint8_t*)calloc(6 * enc.w * enc.h, sizeof(uint8_t));	
	enc.extended_data = (uint8_t*)calloc((enc.w + 2) * (enc.h + 1) * 3, sizeof(uint8_t));
	for (k = 0; k < 3; k++)
	{
		for (i = 0; i < enc.h; i++)
		{
			for (j = 0; j < enc.w; j++)
			{
				enc.extended_data[((i + 1) * (enc.w + 2) * 3) + ((j + 1) * 3) + k] = data[(i * enc.w * 3) + (j * 3) + k];
			}
		}
		for(j = 0; j < enc.w + 2; j++)
	    {    	
			enc.extended_data[(j * 3) + k] = 0;
		}
	   	for(i = 1; i < enc.h + 1; i++)
		{
	      	enc.extended_data[(i * (enc.w + 2) * 3) + k] = enc.extended_data[((i - 1) * (enc.w + 2) * 3) + (1 * 3) + k];
	      	enc.extended_data[(i * (enc.w + 2) * 3) + ((enc.w + 1) * 3) + k] = enc.extended_data[(i * (enc.w + 2) * 3) + (enc.w * 3) + k];
		}
	}	
	enc.RANGE = 256;
	enc.MAXVAL = 255;
	enc.qbpp = 8;
	enc.bpp = 8;
	enc.LIMIT = 32;
	current_write_byte = 0;
   	write_position = 7;
   	byte_count = 0;
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
	enc.p.x = 0;
	enc.p.y = 1;
   	enc.RUNindex = 0;
   	enc.RUNindex_prev = 0;
	enc.EOline = 0;
	for(i = 0; i < 3; i++)
	{
		enc.D[0][i] = 0;
		enc.D[1][i] = 0;
		enc.D[2][i] = 0;
		enc.Q[i] = 0;
		enc.Ra[i] = 0;
		enc.Rb[i] = 0;
		enc.Rc[i] = 0;
		enc.Rd[i] = 0;
		enc.Ix[i] = 0;
    	enc.RUNval[i] = 0;  
	}
    enc.J[0] = 0; 	enc.J[1] = 0; 	enc.J[2] = 0; 	enc.J[3] = 0; 	enc.J[4] = 1; 	enc.J[5] = 1; 	enc.J[6] = 1; 	enc.J[7] = 1; 
    enc.J[8] = 2; 	enc.J[9] = 2; 	enc.J[10] = 2; 	enc.J[11] = 2; 	enc.J[12] = 3; 	enc.J[13] = 3; 	enc.J[14] = 3; 	enc.J[15] = 3; 
    enc.J[16] = 4; 	enc.J[17] = 4; 	enc.J[18] = 5; 	enc.J[19] = 5; 	enc.J[20] = 6; 	enc.J[21] = 6; 	enc.J[22] = 7; 	enc.J[23] = 7;     
    enc.J[24] = 8; 	enc.J[25] = 9; 	enc.J[26] = 10; enc.J[27] = 11; enc.J[28] = 12; enc.J[29] = 13; enc.J[30] = 14; enc.J[31] = 15; 

	enc.Px = 0;
	enc.RUNcnt = 0;    
	enc.RItype = 0;  
	enc.glimit = 0;
	enc.SIGN = 0;
	enc.TEMP = 0;
	enc.Errval = 0;
	enc.EMErrval = 0;
	enc.MErrval = 0;
	enc.q = 0;
	enc.k = 0;
	enc.map = 0;	
    enc.RESET = 64;
    enc.T1 = 3;
    enc.T2 = 7;
    enc.T3 = 21;
    enc.MAX_C = 127;
    enc.MIN_C = -128;
}

static void Close()
{	
	free(enc.bitstream);
	free(enc.extended_data);
}

static void GetNextSample()
{
	uint8_t i = 0;
	if(enc.p.x == enc.w)
	{
    	enc.p.y++;
    	enc.p.x = 1;
	}
 	else
 	{
 		enc.p.x++;
	}
	
	if(enc.p.x == enc.w)
	{		
		enc.EOline = 1;
	}
	else if (enc.EOline == 1)
	{			
		enc.EOline = 0;
	}	
	for (i = 0; i < 3; i++)
	{
		enc.Ix[i] = enc.extended_data[(enc.p.y * (enc.w + 2) * 3) + (enc.p.x * 3) + i];	
		enc.Ra[i] = enc.extended_data[(enc.p.y * (enc.w + 2) * 3) + ((enc.p.x - 1) * 3) + i];	
		enc.Rb[i] = enc.extended_data[((enc.p.y - 1) * (enc.w + 2) * 3) + (enc.p.x * 3) + i];	
		enc.Rc[i] = enc.extended_data[((enc.p.y - 1) * (enc.w + 2) * 3) + ((enc.p.x - 1) * 3) + i];
		enc.Rd[i] = enc.extended_data[((enc.p.y - 1) * (enc.w + 2) * 3) + ((enc.p.x + 1) * 3) + i];
	}	
}

static uint8_t ContextModeling()
{
	uint8_t i = 0;
	for (i = 0; i < 3; i++)
	{
		enc.D[i][0] = enc.Rd[i] - enc.Rb[i];
    	enc.D[i][1] = enc.Rb[i] - enc.Rc[i];
    	enc.D[i][2] = enc.Rc[i] - enc.Ra[i];
	}
 	if (enc.D[0][0] == 0 && enc.D[0][1] == 0 && enc.D[0][2] == 0 && 
	 	enc.D[1][0] == 0 && enc.D[1][1] == 0 && enc.D[1][2] == 0 &&
		enc.D[2][0] == 0 && enc.D[2][1] == 0 && enc.D[2][2] == 0)
 	{
  		return 1;
	}
	else
	{			
  		return 0;
	}
}

static void AppendToBitStream(uint16_t b, uint8_t n)
{
	uint16_t bit_set_mask[] = 
	{	
		0x0001, 0x0002, 0x0004, 0x0008,	0x0010, 0x0020, 0x0040, 0x0080, 
		0x0100, 0x0200, 0x0400, 0x0800, 0x1000, 0x2000, 0x4000, 0x8000
	};
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

static void GolombCoding(uint16_t n, uint8_t l, uint8_t limit)
{
 	uint16_t val1;
 	uint16_t val2;
 	uint8_t val3;
 	uint8_t val4;
 	val1 = n >> l;
 	val2 = val1 << l;
 	val3 = limit - enc.qbpp - 1;
   	if(val1 < val3)
	{
    	AppendToBitStream(0, (uint8_t)val1);
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

static void RunModeProcessing()
{
	uint8_t i = 0;
	for (i = 0; i < 3; i++)
	{
		enc.RUNval[i] = enc.Ra[i];
	}	
 	enc.RUNcnt = 0;
 	while((enc.Ix[0] == enc.RUNval[0]) && (enc.Ix[1] == enc.RUNval[1]) && (enc.Ix[2] == enc.RUNval[2]))
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
 	if((enc.Ra[0] != enc.Ix[0]) || (enc.Ra[1] != enc.Ix[1]) || (enc.Ra[2] != enc.Ix[2]))
	{
   		AppendToBitStream(0, 1);
   		AppendToBitStream(enc.RUNcnt, enc.J[enc.RUNindex]);
   		if(enc.RUNindex > 0)
		{
     		enc.RUNindex_prev = enc.RUNindex;
     		enc.RUNindex = enc.RUNindex - 1;
   		}
   		for (i = 0; i < 3; i++)
   		{
	    	enc.RItype = 0;		
			if(enc.RItype == 1)
			{
				enc.Errval = enc.Ix[i] - enc.Ra[i];	
			}    
	  		else
	  		{
	  			enc.Errval = enc.Ix[i] - enc.Rb[i];
			}
			if((enc.RItype == 0) && (enc.Ra[i] > enc.Rb[i]))
			{
	   			enc.Errval = -enc.Errval;
	   			enc.SIGN = -1;
	 		}
			else
			{			
	   			enc.SIGN = 1;
			}	
			if(enc.Errval < 0)
	 		{ 	
	   			enc.Errval = enc.Errval + enc.RANGE;
	 		}
	 		if(enc.Errval >= ((enc.RANGE+1)/2))
	 		{
	 			enc.Errval = enc.Errval - enc.RANGE;
			}
			if (enc.RItype == 0)
			{			
				enc.TEMP = enc.A[365];
			}
			else
			{
				enc.TEMP = enc.A[366] + (enc.N[366] >> 1);
			}
			enc.q = enc.RItype + 365; 
			for(enc.k = 0; (enc.N[enc.q] << enc.k) < enc.TEMP; enc.k++);
			if((enc.k == 0) && (enc.Errval > 0) && (2 * enc.Nn[enc.q] < enc.N[enc.q]))
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
			enc.EMErrval = 2 * (ABS(enc.Errval)) - enc.RItype - enc.map;
			enc.glimit = enc.LIMIT - enc.J[enc.RUNindex_prev] - 1;
	  		GolombCoding(enc.EMErrval, enc.k, enc.glimit); 
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
 	else if(enc.RUNcnt > 0)
 	{ 		
    	AppendToBitStream(1, 1);
	}
}

static void Prediction(uint8_t cmp)
{
	uint8_t i = 0;
	for(i = 0; i < 3; i++)
	{
		if(enc.D[cmp][i] <= -enc.T3)
		{
			enc.Q[i] = -4;	
		} 
		else if(enc.D[cmp][i] <= -enc.T2) 
		{
			enc.Q[i] = -3;	
		}
		else if(enc.D[cmp][i] <= -enc.T1) 
		{
			enc.Q[i] = -2;	
		}
		else if(enc.D[cmp][i] < -0)
		{
			enc.Q[i] = -1;	
		} 
		else if(enc.D[cmp][i] <= 0) 
		{
			enc.Q[i] = 0;	
		}
		else if(enc.D[cmp][i] < enc.T1)
		{
			enc.Q[i] = 1;	
		} 
		else if(enc.D[cmp][i] < enc.T2) 
		{
			enc.Q[i] = 2;	
		}
		else if(enc.D[cmp][i] < enc.T3)
		{
			enc.Q[i] = 3;
		} 
		else
		{
			enc.Q[i] = 4;
		} 
	}
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
	if(enc.Rc[cmp] >= MAX(enc.Ra[cmp], enc.Rb[cmp]))
	{
		enc.Px = MIN(enc.Ra[cmp], enc.Rb[cmp]);	
	}   		
 	else 
	{
   		if(enc.Rc[cmp] <= MIN(enc.Ra[cmp], enc.Rb[cmp]))
   		{   			
     		enc.Px = MAX(enc.Ra[cmp], enc.Rb[cmp]);
		}
   		else
   		{   			
     		enc.Px = enc.Ra[cmp] + enc.Rb[cmp] - enc.Rc[cmp];
		}
  	}
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
	enc.Errval = enc.Ix[cmp] - enc.Px;
 	if (enc.SIGN == -1)
 	{ 		
    	enc.Errval = -enc.Errval;
	}    
	if(enc.Errval < 0)
 	{ 	
   		enc.Errval = enc.Errval + enc.RANGE;
 	}
 	if(enc.Errval >= ((enc.RANGE+1)/2))
 	{
 		enc.Errval = enc.Errval - enc.RANGE;
	}
}

static void PredictionErrorEncoding()
{
    for(enc.k = 0; (enc.N[enc.q] << enc.k) < enc.A[enc.q]; enc.k++);
    if((enc.k == 0) && (2 * enc.B[enc.q] <= -enc.N[enc.q]))
	{    
    	if(enc.Errval >= 0)
    	{
    		enc.MErrval = 2 * enc.Errval + 1;
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
	GolombCoding(enc.MErrval, enc.k, enc.LIMIT);
}

static void VariablesUpdate()
{
  	enc.B[enc.q] = enc.B[enc.q] + enc.Errval;
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

static void RegularModeProcessing()
{
	uint8_t i = 0;
	for (i = 0; i < 3; i++)
	{
		Prediction(i);
		PredictionErrorEncoding();
		VariablesUpdate();
	}
}

uint8_t* Encode(uint8_t* data, uint16_t w, uint16_t h, uint32_t* size, uint8_t coding_mode)
{	
	uint8_t *dout;
	Initializations(data, w, h, coding_mode);
    for(;;)
    {
		if(enc.p.x == enc.w && enc.p.y == enc.h)
    	{    		
        	break;
		} 
		GetNextSample();
		if (ContextModeling())
		{
			if (enc.coding_mode == 1)
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
	if (write_position >= 0)
	{
		enc.bitstream[byte_count] = current_write_byte;
		byte_count = byte_count + 1;
	}
	dout = (uint8_t*)calloc(byte_count, sizeof(uint8_t));
	*size = byte_count;
	memcpy(dout, enc.bitstream, byte_count);	
	Close();
	return dout;
}
