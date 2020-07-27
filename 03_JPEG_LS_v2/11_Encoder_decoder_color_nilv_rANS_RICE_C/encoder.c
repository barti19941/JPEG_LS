#include "encoder.h"

static encoder enc;
static uint8_t current_write_byte_regular;
static int8_t write_position_regular;
static uint32_t byte_count_regular;

static uint8_t current_write_byte_run;
static int8_t write_position_run;
static uint32_t byte_count_run;


static void Initializations(uint8_t* data, uint16_t w, uint16_t h, uint8_t component_id, uint8_t coding_mode)
{
	uint32_t i = 0;
	uint32_t j = 0;
	enc.coding_mode = coding_mode;
	enc.w = w;
    enc.h = h;
	enc.bitstream_regular = (uint8_t*)calloc(6 * enc.w * enc.h, sizeof(uint8_t));
	enc.bitstream_run = (uint8_t*)calloc(6 * enc.w * enc.h, sizeof(uint8_t));	
	enc.extended_data = (uint8_t*)calloc((enc.w + 2) * (enc.h + 1), sizeof(uint8_t));
	for (i = 0; i < enc.h; i++)
	{
		for (j = 0; j < enc.w; j++)
		{
			enc.extended_data[((i + 1) * (enc.w + 2)) + (j + 1)] = data[(i * enc.w * 3) + (j * 3) + component_id];
		}
	}
	for(j = 0; j < enc.w + 2; j++)
    {    	
		enc.extended_data[j] = 0;
	}
   	for(i = 1; i < enc.h + 1; i++)
	{
      	enc.extended_data[(i * (enc.w + 2))] = enc.extended_data[((i - 1) * (enc.w + 2)) + 1];
      	enc.extended_data[(i * (enc.w + 2)) + enc.w + 1] = enc.extended_data[(i * (enc.w + 2)) + enc.w];
	}
	enc.RANGE = 256;
	enc.MAXVAL = 255;
	enc.qbpp = 8;
	enc.bpp = 8;
	enc.LIMIT = 32;
	current_write_byte_regular = 0;
   	write_position_regular = 7;
   	byte_count_regular = 0;
   	current_write_byte_run = 0;
   	write_position_run = 7;
   	byte_count_run = 0;
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
	for(i = 0; i < 3; i++)
	{
		enc.D[i] = 0;
		enc.Q[i] = 0;
	}
    enc.J[0] = 0; 	enc.J[1] = 0; 	enc.J[2] = 0; 	enc.J[3] = 0; 	enc.J[4] = 1; 	enc.J[5] = 1; 	enc.J[6] = 1; 	enc.J[7] = 1; 
    enc.J[8] = 2; 	enc.J[9] = 2; 	enc.J[10] = 2; 	enc.J[11] = 2; 	enc.J[12] = 3; 	enc.J[13] = 3; 	enc.J[14] = 3; 	enc.J[15] = 3; 
    enc.J[16] = 4; 	enc.J[17] = 4; 	enc.J[18] = 5; 	enc.J[19] = 5; 	enc.J[20] = 6; 	enc.J[21] = 6; 	enc.J[22] = 7; 	enc.J[23] = 7;     
    enc.J[24] = 8; 	enc.J[25] = 9; 	enc.J[26] = 10; enc.J[27] = 11; enc.J[28] = 12; enc.J[29] = 13; enc.J[30] = 14; enc.J[31] = 15; 
	enc.Ra = 0;
	enc.Rb = 0;
	enc.Rc = 0;
	enc.Rd = 0;
	enc.Ix = 0;
	enc.Px = 0;
	enc.RUNindex = 0; 
	enc.RUNindex_prev = 0; 
    enc.RUNval = 0;  
	enc.RUNcnt = 0;     
	enc.RItype = 0;  
	enc.SIGN = 0;
	enc.TEMP = 0;
	enc.Errval = 0;
	enc.EMErrval = 0;
	enc.MErrval = 0;
	enc.q = 0;
	enc.k = 0;
	enc.map = 0;	
    enc.p.x = 0;
    enc.p.y = 1;	
	enc.EOline = 0;
    enc.RESET = 64;
    enc.T1 = 3;
    enc.T2 = 7;
    enc.T3 = 21;
    enc.MAX_C = 127;
    enc.MIN_C = -128;
    enc.x = 65536;
    enc.l = 65536;
    enc.max = 256;
    enc.b = 2;
    enc.d = 1;
    enc.n = 16;
    enc.m = 1;
    enc.errors_count = 0;
    enc.errors_calculated = (uint16_t*)calloc(10 * enc.w * enc.h, sizeof(uint16_t));
    enc.symbols = (uint16_t*)calloc(enc.max + 1, sizeof(uint16_t));
	for (i = 0; i < enc.max + 1; i++)
	{
		enc.symbols[i] = i;
	}
	enc.fs = (uint16_t*)calloc(enc.max + 1, sizeof(uint16_t));
	enc.fs[0] = 5876; enc.fs[1] = 5348; enc.fs[2] = 4866; enc.fs[3] = 4428; enc.fs[4] = 4030; enc.fs[5] = 3667; enc.fs[6] = 3337; enc.fs[7] = 3037;
	enc.fs[8] = 2764; enc.fs[9] = 2515; enc.fs[10] = 2289; enc.fs[11] = 2083; enc.fs[12] = 1896; enc.fs[13] = 1725; enc.fs[14] = 1570; enc.fs[15] = 1429;
	enc.fs[16] = 1300; enc.fs[17] = 1183; enc.fs[18] = 1077; enc.fs[19] = 980; enc.fs[20] = 892; enc.fs[21] = 812; enc.fs[22] = 739; enc.fs[23] = 672;
	enc.fs[24] = 612; enc.fs[25] = 557; enc.fs[26] = 507; enc.fs[27] = 461; enc.fs[28] = 420; enc.fs[29] = 382; enc.fs[30] = 348; enc.fs[31] = 317;
	enc.fs[32] = 288; enc.fs[33] = 262; enc.fs[34] = 239; enc.fs[35] = 218; enc.fs[36] = 198; enc.fs[37] = 180; enc.fs[38] = 164; enc.fs[39] = 149;
	enc.fs[40] = 136; enc.fs[41] = 124; enc.fs[42] = 113; enc.fs[43] = 103; enc.fs[44] = 94; enc.fs[45] = 85; enc.fs[46] = 78; enc.fs[47] = 71;
	enc.fs[48] = 65; enc.fs[49] = 59; enc.fs[50] = 54; enc.fs[51] = 49; enc.fs[52] = 45; enc.fs[53] = 41; enc.fs[54] = 37; enc.fs[55] = 34;
	enc.fs[56] = 31; enc.fs[57] = 28; enc.fs[58] = 26; enc.fs[59] = 24; enc.fs[60] = 21; enc.fs[61] = 20; enc.fs[62] = 18; enc.fs[63] = 16;
	enc.fs[64] = 15; enc.fs[65] = 14; enc.fs[66] = 13; enc.fs[67] = 12; enc.fs[68] = 11; enc.fs[69] = 10; enc.fs[70] = 9; enc.fs[71] = 8;
	enc.fs[72] = 8; enc.fs[73] = 7; enc.fs[74] = 6; enc.fs[75] = 6; enc.fs[76] = 6; enc.fs[77] = 5; enc.fs[78] = 5; enc.fs[79] = 4;
	enc.fs[80] = 4; enc.fs[81] = 4; enc.fs[82] = 4; enc.fs[83] = 3; enc.fs[84] = 3; enc.fs[85] = 3; enc.fs[86] = 3; enc.fs[87] = 3;
	enc.fs[88] = 2; enc.fs[89] = 2; enc.fs[90] = 2; enc.fs[91] = 2; enc.fs[92] = 2; enc.fs[93] = 2; enc.fs[94] = 2; enc.fs[95] = 2;
	enc.fs[96] = 2; enc.fs[97] = 2; enc.fs[98] = 2; enc.fs[99] = 2; enc.fs[100] = 1; enc.fs[101] = 1; enc.fs[102] = 1; enc.fs[103] = 1;
	enc.fs[104] = 1; enc.fs[105] = 1; enc.fs[106] = 1; enc.fs[107] = 1; enc.fs[108] = 1; enc.fs[109] = 1; enc.fs[110] = 1; enc.fs[111] = 1;
	enc.fs[112] = 1; enc.fs[113] = 1; enc.fs[114] = 1; enc.fs[115] = 1; enc.fs[116] = 1; enc.fs[117] = 1; enc.fs[118] = 1; enc.fs[119] = 1;
	enc.fs[120] = 1; enc.fs[121] = 1; enc.fs[122] = 1; enc.fs[123] = 1; enc.fs[124] = 1; enc.fs[125] = 1; enc.fs[126] = 1; enc.fs[127] = 1;
	enc.fs[128] = 1; enc.fs[129] = 1; enc.fs[130] = 1; enc.fs[131] = 1; enc.fs[132] = 1; enc.fs[133] = 1; enc.fs[134] = 1; enc.fs[135] = 1;
	enc.fs[136] = 1; enc.fs[137] = 1; enc.fs[138] = 1; enc.fs[139] = 1; enc.fs[140] = 1; enc.fs[141] = 1; enc.fs[142] = 1; enc.fs[143] = 1;
	enc.fs[144] = 1; enc.fs[145] = 1; enc.fs[146] = 1; enc.fs[147] = 1; enc.fs[148] = 1; enc.fs[149] = 1; enc.fs[150] = 1; enc.fs[151] = 1; 
	enc.fs[152] = 1; enc.fs[153] = 1; enc.fs[154] = 1; enc.fs[155] = 1; enc.fs[156] = 1; enc.fs[157] = 1; enc.fs[158] = 1; enc.fs[159] = 1;
	enc.fs[160] = 1; enc.fs[161] = 1; enc.fs[162] = 1; enc.fs[163] = 1; enc.fs[164] = 1; enc.fs[165] = 1; enc.fs[166] = 1; enc.fs[167] = 1;
	enc.fs[168] = 1; enc.fs[169] = 1; enc.fs[170] = 1; enc.fs[171] = 1; enc.fs[172] = 1; enc.fs[173] = 1; enc.fs[174] = 1; enc.fs[175] = 1;
	enc.fs[176] = 1; enc.fs[177] = 1; enc.fs[178] = 1; enc.fs[179] = 1; enc.fs[180] = 1; enc.fs[181] = 1; enc.fs[182] = 1; enc.fs[183] = 1;
	enc.fs[184] = 1; enc.fs[185] = 1; enc.fs[186] = 1; enc.fs[187] = 1; enc.fs[188] = 1; enc.fs[189] = 1; enc.fs[190] = 1; enc.fs[191] = 1;
	enc.fs[192] = 1; enc.fs[193] = 1; enc.fs[194] = 1; enc.fs[195] = 1; enc.fs[196] = 1; enc.fs[197] = 1; enc.fs[198] = 1; enc.fs[199] = 1;
	enc.fs[200] = 1; enc.fs[201] = 1; enc.fs[202] = 1; enc.fs[203] = 1; enc.fs[204] = 1; enc.fs[205] = 1; enc.fs[206] = 1; enc.fs[207] = 1;
	enc.fs[208] = 1; enc.fs[209] = 1; enc.fs[210] = 1; enc.fs[211] = 1; enc.fs[212] = 1; enc.fs[213] = 1; enc.fs[214] = 1; enc.fs[215] = 1;
	enc.fs[216] = 1; enc.fs[217] = 1; enc.fs[218] = 1; enc.fs[219] = 1; enc.fs[220] = 1; enc.fs[221] = 1; enc.fs[222] = 1; enc.fs[223] = 1;
	enc.fs[224] = 1; enc.fs[225] = 1; enc.fs[226] = 1; enc.fs[227] = 1; enc.fs[228] = 1; enc.fs[229] = 1; enc.fs[230] = 1; enc.fs[231] = 1;
	enc.fs[232] = 1; enc.fs[233] = 1; enc.fs[234] = 1; enc.fs[235] = 1; enc.fs[236] = 1; enc.fs[237] = 1; enc.fs[238] = 1; enc.fs[239] = 1;
	enc.fs[240] = 1; enc.fs[241] = 1; enc.fs[242] = 1; enc.fs[243] = 1; enc.fs[244] = 1; enc.fs[245] = 1; enc.fs[246] = 1; enc.fs[247] = 1;
	enc.fs[248] = 1; enc.fs[249] = 1; enc.fs[250] = 1; enc.fs[251] = 1; enc.fs[252] = 1; enc.fs[253] = 1; enc.fs[254] = 1; enc.fs[255] = 1;
	enc.fs[256] = 1; 
	enc.cs = (uint16_t*)calloc(enc.max + 1, sizeof(uint16_t));
	enc.cs[0] = 0;
	for (i = 1; i < enc.max + 1; i++)
	{
		for(j = 0; j < i; j++)
		{
			enc.cs[i] += enc.fs[j];
		}			
	}
}

static void Close()
{	
	free(enc.fs);
	free(enc.cs);
	free(enc.symbols);	
	free(enc.bitstream_run);
	free(enc.bitstream_regular);
	free(enc.extended_data);
	free(enc.errors_calculated);
}

static void ReverseErrorTable()
{
	uint16_t* dout;
	uint32_t i = 0;
	dout = (uint16_t*)calloc(enc.errors_count, sizeof(uint16_t));
	for (i = 0; i < enc.errors_count; i++)
	{
		dout[i] = enc.errors_calculated[enc.errors_count - i - 1];
	}
	free(enc.errors_calculated);
	enc.errors_calculated = dout;
}

static void GetNextSample()
{
	if(enc.p.x == enc.w)
	{
    	enc.p.y++;
    	enc.p.x = 1;
	}
 	else
 	{
 		enc.p.x++;
	}
	enc.Ix = enc.extended_data[enc.p.y * (enc.w + 2) + enc.p.x];	
	enc.Ra = enc.extended_data[enc.p.y * (enc.w + 2) + enc.p.x - 1];	
	enc.Rb = enc.extended_data[(enc.p.y - 1) * (enc.w + 2) + enc.p.x];	
	enc.Rc = enc.extended_data[(enc.p.y - 1) * (enc.w + 2) + enc.p.x - 1];
	enc.Rd = enc.extended_data[(enc.p.y - 1) * (enc.w + 2) + enc.p.x + 1];		
	if(enc.p.x == enc.w)
	{		
		enc.EOline = 1;
	}
	else
	{		
		enc.EOline = 0;
	}
}

static uint8_t ContextModeling()
{
    enc.D[0] = enc.Rd - enc.Rb;
    enc.D[1] = enc.Rb - enc.Rc;
    enc.D[2] = enc.Rc - enc.Ra;
 	if (enc.D[0] == 0 && enc.D[1] == 0 && enc.D[2] == 0)
 	{
  		return 1;
	}
	else
	{			
  		return 0;
	}
}

static void AppendToBitStreamRun(uint16_t b, uint8_t n)
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
			current_write_byte_run |= bit_set_mask[write_position_run--];			
			if (write_position_run < 0) 
			{
				enc.bitstream_run[byte_count_run] = current_write_byte_run;
				byte_count_run++;
 				if(current_write_byte_run == 255)
 				{
 					write_position_run = 6;
				}
				else
				{
					write_position_run = 7;
				}
				current_write_byte_run = 0;
			}
		}
       	else 
		{
			write_position_run--;
			if(write_position_run < 0)
			{
				enc.bitstream_run[byte_count_run] = current_write_byte_run;
				byte_count_run++;
 				write_position_run = 7;
 				current_write_byte_run = 0;
 			}
		}
    }
}

static void AppendToBitStreamRegular(uint16_t b, uint8_t n)
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
			current_write_byte_regular |= bit_set_mask[write_position_regular--];			
			if (write_position_regular < 0) 
			{
				enc.bitstream_regular[byte_count_regular] = current_write_byte_regular;
				byte_count_regular++;
 				if(current_write_byte_regular == 255)
 				{
 					write_position_regular = 6;
				}
				else
				{
					write_position_regular = 7;
				}
				current_write_byte_regular = 0;
			}
		}
       	else 
		{
			write_position_regular--;
			if(write_position_regular < 0)
			{
				enc.bitstream_regular[byte_count_regular] = current_write_byte_regular;
				byte_count_regular++;
 				write_position_regular = 7;
 				current_write_byte_regular = 0;
 			}
		}
    }
}

static void rAnsEncoding(uint16_t val)
{
	while(enc.x >= (enc.fs[val] << enc.d))
	{	
		AppendToBitStreamRegular((uint8_t)enc.x & (uint8_t)(enc.b - 1), 1);
		enc.x >>= 1;
	}
	enc.x = ((uint32_t)floor(enc.x/enc.fs[val]) << enc.n) + (enc.x % enc.fs[val]) + enc.cs[val];	
}

static void rAnsEncodingLast()
{
	uint8_t i = 0;
	for (i = 0; i < (enc.n + enc.m); i++)
	{
		AppendToBitStreamRegular(enc.x, 1);
		enc.x >>= 1;
	}
}

static void RunModeProcessing()
{
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
 	while(enc.RUNcnt >= (1 << enc.J[enc.RUNindex]))
	{
    	AppendToBitStreamRun(1, 1);
    	enc.RUNcnt -= (1 << enc.J[enc.RUNindex]);
    	if(enc.RUNindex < 31)
		{
			enc.RUNindex_prev = enc.RUNindex;
      		enc.RUNindex++;
    	}
 	}
 	if(enc.Ra != enc.Ix)
	{
   		AppendToBitStreamRun(0, 1);
   		AppendToBitStreamRun(enc.RUNcnt, enc.J[enc.RUNindex]);
   		if(enc.RUNindex > 0)
		{
     		enc.RUNindex_prev = enc.RUNindex;
     		enc.RUNindex = enc.RUNindex - 1;
   		}
  		if(enc.Ra == enc.Rb)
  		{
  			enc.RItype = 1;
		}
    	else
    	{    		
    		enc.RItype = 0;
		}
		if(enc.RItype == 1)
		{
			enc.Errval = enc.Ix - enc.Ra;	
		}    
  		else
  		{
  			enc.Errval = enc.Ix - enc.Rb;
		}
		if((enc.RItype == 0) && (enc.Ra > enc.Rb))
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
		enc.errors_calculated[enc.errors_count] = enc.EMErrval;
  		enc.errors_count++;
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
 	else if(enc.RUNcnt > 0)
 	{ 		
    	AppendToBitStreamRun(1, 1);
	}
}

static void Prediction()
{
	uint8_t i = 0;
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
		else if(enc.D[i] < -0)
		{
			enc.Q[i] = -1;	
		} 
		else if(enc.D[i] <= 0) 
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
	enc.Errval = enc.Ix - enc.Px;
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
  	enc.errors_calculated[enc.errors_count] = enc.MErrval;
  	enc.errors_count++;
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
	Prediction();
	PredictionErrorEncoding();
	VariablesUpdate();
}

uint8_t* Encode(uint8_t* data, uint16_t w, uint16_t h, uint32_t* size, uint8_t coding_mode)
{	
	uint8_t *dout;
	uint8_t *data_pointers[3];
	uint8_t k = 0;
	uint32_t i = 0;
	uint32_t j = 0;
	uint32_t size_bytes[3];
	for (k = 0; k < 3; k++)
	{
		Initializations(data, w, h, k, coding_mode);
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
		ReverseErrorTable();
		for (i = 0; i < enc.errors_count; i++)
		{
			rAnsEncoding(enc.errors_calculated[i]);
		}
		rAnsEncodingLast();
		if (write_position_regular >= 0)
		{
			enc.bitstream_regular[byte_count_regular] = current_write_byte_regular;
			byte_count_regular = byte_count_regular + 1;
		}
		if (write_position_run >= 0)
		{
			enc.bitstream_run[byte_count_run] = current_write_byte_run;
			byte_count_run = byte_count_run + 1;
		}
		size_bytes[k] = byte_count_regular + byte_count_run;
		data_pointers[k] = (uint8_t*)calloc(size_bytes[k], sizeof(uint8_t));
		memcpy(data_pointers[k], enc.bitstream_run, byte_count_run);	
		memcpy(data_pointers[k] + byte_count_run, enc.bitstream_regular, byte_count_regular);
		Close();
	} 	
	*size = size_bytes[0] + size_bytes[1] + size_bytes[2] + 10 + 10 + 10;
	dout = (uint8_t*)calloc(*size, sizeof(uint8_t));
	for (k = 0; k < 3; k++)
	{
		dout[0 + j] = 0xFF;
		dout[1 + j] = 0xDA;
		dout[2 + j] = 0x00;
		dout[3 + j] = 0x08;
		dout[4 + j] = 0x01;
		dout[5 + j] = k;
		dout[6 + j] = 0x00;
		dout[7 + j] = 0x00;
		dout[8 + j] = 0x00;
		dout[9 + j] = 0x00;
		for (i = 0; i < size_bytes[k]; i++)
		{
			dout[10 + i + j] = data_pointers[k][i];
		}		
		j += (10 + size_bytes[k]);
		free(data_pointers[k]);
	}
	return dout;
}
