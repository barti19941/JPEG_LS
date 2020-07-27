#include "decoder.h"

static decoder dec;
static uint8_t current_read_byte;
static int8_t read_position;
static int32_t byte_count;
static uint8_t current_component;

static void Initializations(uint8_t* data, uint16_t w, uint16_t h, uint32_t size, uint8_t coding_mode)
{
	uint32_t i = 0;		
	uint32_t j = 0;
	dec.coding_mode = coding_mode;	
	dec.w = w;
	dec.h = h;
	dec.bitstream = (uint8_t*)calloc(size, sizeof(uint8_t));
	memcpy(dec.bitstream, data, size);
	dec.extended_data = (uint8_t*)calloc((dec.w + 2) * (dec.h + 1) * 3, sizeof(uint8_t));
	current_read_byte = 0;
	read_position = -1;
	byte_count = -1;
	current_component = 0;
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
	for(i = 0; i < 3; i++)
	{
	   	dec.p[i].x = 0;
    	dec.p[i].y = 1;	
    	dec.D[i] = 0;
		dec.Q[i] = 0;
	}
	dec.J[0] = 0; 	dec.J[1] = 0; 	dec.J[2] = 0; 	dec.J[3] = 0; 	dec.J[4] = 1; 	dec.J[5] = 1; 	dec.J[6] = 1; 	dec.J[7] = 1; 
    dec.J[8] = 2; 	dec.J[9] = 2; 	dec.J[10] = 2; 	dec.J[11] = 2; 	dec.J[12] = 3; 	dec.J[13] = 3; 	dec.J[14] = 3; 	dec.J[15] = 3; 
    dec.J[16] = 4; 	dec.J[17] = 4; 	dec.J[18] = 5; 	dec.J[19] = 5; 	dec.J[20] = 6; 	dec.J[21] = 6; 	dec.J[22] = 7; 	dec.J[23] = 7;     
    dec.J[24] = 8; 	dec.J[25] = 9; 	dec.J[26] = 10; dec.J[27] = 11; dec.J[28] = 12; dec.J[29] = 13; dec.J[30] = 14; dec.J[31] = 15;  
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
    dec.RESET = 64;
    dec.T1 = 3;
    dec.T2 = 7;
    dec.T3 = 21;    
    dec.MAX_C = 127;
    dec.MIN_C = -128;
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
	dec.fs = (uint16_t*)calloc(dec.max + 1, sizeof(uint16_t));
	dec.fs[0] = 5876; dec.fs[1] = 5348; dec.fs[2] = 4866; dec.fs[3] = 4428; dec.fs[4] = 4030; dec.fs[5] = 3667; dec.fs[6] = 3337; dec.fs[7] = 3037;
	dec.fs[8] = 2764; dec.fs[9] = 2515; dec.fs[10] = 2289; dec.fs[11] = 2083; dec.fs[12] = 1896; dec.fs[13] = 1725; dec.fs[14] = 1570; dec.fs[15] = 1429;
	dec.fs[16] = 1300; dec.fs[17] = 1183; dec.fs[18] = 1077; dec.fs[19] = 980; dec.fs[20] = 892; dec.fs[21] = 812; dec.fs[22] = 739; dec.fs[23] = 672;
	dec.fs[24] = 612; dec.fs[25] = 557; dec.fs[26] = 507; dec.fs[27] = 461; dec.fs[28] = 420; dec.fs[29] = 382; dec.fs[30] = 348; dec.fs[31] = 317;
	dec.fs[32] = 288; dec.fs[33] = 262; dec.fs[34] = 239; dec.fs[35] = 218; dec.fs[36] = 198; dec.fs[37] = 180; dec.fs[38] = 164; dec.fs[39] = 149;
	dec.fs[40] = 136; dec.fs[41] = 124; dec.fs[42] = 113; dec.fs[43] = 103; dec.fs[44] = 94; dec.fs[45] = 85; dec.fs[46] = 78; dec.fs[47] = 71;
	dec.fs[48] = 65; dec.fs[49] = 59; dec.fs[50] = 54; dec.fs[51] = 49; dec.fs[52] = 45; dec.fs[53] = 41; dec.fs[54] = 37; dec.fs[55] = 34;
	dec.fs[56] = 31; dec.fs[57] = 28; dec.fs[58] = 26; dec.fs[59] = 24; dec.fs[60] = 21; dec.fs[61] = 20; dec.fs[62] = 18; dec.fs[63] = 16;
	dec.fs[64] = 15; dec.fs[65] = 14; dec.fs[66] = 13; dec.fs[67] = 12; dec.fs[68] = 11; dec.fs[69] = 10; dec.fs[70] = 9; dec.fs[71] = 8;
	dec.fs[72] = 8; dec.fs[73] = 7; dec.fs[74] = 6; dec.fs[75] = 6; dec.fs[76] = 6; dec.fs[77] = 5; dec.fs[78] = 5; dec.fs[79] = 4;
	dec.fs[80] = 4; dec.fs[81] = 4; dec.fs[82] = 4; dec.fs[83] = 3; dec.fs[84] = 3; dec.fs[85] = 3; dec.fs[86] = 3; dec.fs[87] = 3;
	dec.fs[88] = 2; dec.fs[89] = 2; dec.fs[90] = 2; dec.fs[91] = 2; dec.fs[92] = 2; dec.fs[93] = 2; dec.fs[94] = 2; dec.fs[95] = 2;
	dec.fs[96] = 2; dec.fs[97] = 2; dec.fs[98] = 2; dec.fs[99] = 2; dec.fs[100] = 1; dec.fs[101] = 1; dec.fs[102] = 1; dec.fs[103] = 1;
	dec.fs[104] = 1; dec.fs[105] = 1; dec.fs[106] = 1; dec.fs[107] = 1; dec.fs[108] = 1; dec.fs[109] = 1; dec.fs[110] = 1; dec.fs[111] = 1;
	dec.fs[112] = 1; dec.fs[113] = 1; dec.fs[114] = 1; dec.fs[115] = 1; dec.fs[116] = 1; dec.fs[117] = 1; dec.fs[118] = 1; dec.fs[119] = 1;
	dec.fs[120] = 1; dec.fs[121] = 1; dec.fs[122] = 1; dec.fs[123] = 1; dec.fs[124] = 1; dec.fs[125] = 1; dec.fs[126] = 1; dec.fs[127] = 1;
	dec.fs[128] = 1; dec.fs[129] = 1; dec.fs[130] = 1; dec.fs[131] = 1; dec.fs[132] = 1; dec.fs[133] = 1; dec.fs[134] = 1; dec.fs[135] = 1;
	dec.fs[136] = 1; dec.fs[137] = 1; dec.fs[138] = 1; dec.fs[139] = 1; dec.fs[140] = 1; dec.fs[141] = 1; dec.fs[142] = 1; dec.fs[143] = 1;
	dec.fs[144] = 1; dec.fs[145] = 1; dec.fs[146] = 1; dec.fs[147] = 1; dec.fs[148] = 1; dec.fs[149] = 1; dec.fs[150] = 1; dec.fs[151] = 1; 
	dec.fs[152] = 1; dec.fs[153] = 1; dec.fs[154] = 1; dec.fs[155] = 1; dec.fs[156] = 1; dec.fs[157] = 1; dec.fs[158] = 1; dec.fs[159] = 1;
	dec.fs[160] = 1; dec.fs[161] = 1; dec.fs[162] = 1; dec.fs[163] = 1; dec.fs[164] = 1; dec.fs[165] = 1; dec.fs[166] = 1; dec.fs[167] = 1;
	dec.fs[168] = 1; dec.fs[169] = 1; dec.fs[170] = 1; dec.fs[171] = 1; dec.fs[172] = 1; dec.fs[173] = 1; dec.fs[174] = 1; dec.fs[175] = 1;
	dec.fs[176] = 1; dec.fs[177] = 1; dec.fs[178] = 1; dec.fs[179] = 1; dec.fs[180] = 1; dec.fs[181] = 1; dec.fs[182] = 1; dec.fs[183] = 1;
	dec.fs[184] = 1; dec.fs[185] = 1; dec.fs[186] = 1; dec.fs[187] = 1; dec.fs[188] = 1; dec.fs[189] = 1; dec.fs[190] = 1; dec.fs[191] = 1;
	dec.fs[192] = 1; dec.fs[193] = 1; dec.fs[194] = 1; dec.fs[195] = 1; dec.fs[196] = 1; dec.fs[197] = 1; dec.fs[198] = 1; dec.fs[199] = 1;
	dec.fs[200] = 1; dec.fs[201] = 1; dec.fs[202] = 1; dec.fs[203] = 1; dec.fs[204] = 1; dec.fs[205] = 1; dec.fs[206] = 1; dec.fs[207] = 1;
	dec.fs[208] = 1; dec.fs[209] = 1; dec.fs[210] = 1; dec.fs[211] = 1; dec.fs[212] = 1; dec.fs[213] = 1; dec.fs[214] = 1; dec.fs[215] = 1;
	dec.fs[216] = 1; dec.fs[217] = 1; dec.fs[218] = 1; dec.fs[219] = 1; dec.fs[220] = 1; dec.fs[221] = 1; dec.fs[222] = 1; dec.fs[223] = 1;
	dec.fs[224] = 1; dec.fs[225] = 1; dec.fs[226] = 1; dec.fs[227] = 1; dec.fs[228] = 1; dec.fs[229] = 1; dec.fs[230] = 1; dec.fs[231] = 1;
	dec.fs[232] = 1; dec.fs[233] = 1; dec.fs[234] = 1; dec.fs[235] = 1; dec.fs[236] = 1; dec.fs[237] = 1; dec.fs[238] = 1; dec.fs[239] = 1;
	dec.fs[240] = 1; dec.fs[241] = 1; dec.fs[242] = 1; dec.fs[243] = 1; dec.fs[244] = 1; dec.fs[245] = 1; dec.fs[246] = 1; dec.fs[247] = 1;
	dec.fs[248] = 1; dec.fs[249] = 1; dec.fs[250] = 1; dec.fs[251] = 1; dec.fs[252] = 1; dec.fs[253] = 1; dec.fs[254] = 1; dec.fs[255] = 1;
	dec.fs[256] = 1; 
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
	if (dec.p[current_component].x == dec.w)
	{
		current_component++;			
		if (current_component == 3)
		{
			current_component = 0;
		}
	}		
	if(dec.p[current_component].x == dec.w)
	{
    	dec.p[current_component].y++;
    	dec.p[current_component].x = 1;
	}
 	else
 	{
 		dec.p[current_component].x++;
	}
	dec.Rb = dec.extended_data[((dec.p[current_component].y - 1) * (dec.w + 2) * 3) + (dec.p[current_component].x * 3) + current_component];
	if(dec.p[current_component].x == 1 && dec.p[current_component].y > 1)
	{
	    dec.Ra = dec.Rb;
	    dec.Rc = dec.extended_data[((dec.p[current_component].y - 2) * (dec.w + 2) * 3) + (dec.p[current_component].x * 3) + current_component];
	    if (dec.w == 1)
	    {
	    	dec.Rd = dec.Rb;
		}
		else
		{
			dec.Rd = dec.extended_data[((dec.p[current_component].y - 1) * (dec.w + 2) * 3) + ((dec.p[current_component].x + 1) * 3) + current_component];
		}   
	}
 	else if(dec.p[current_component].x == dec.w)
 	{
 		dec.Ra = dec.extended_data[(dec.p[current_component].y * (dec.w + 2) * 3) + ((dec.p[current_component].x - 1) * 3) + current_component];
 		dec.Rc = dec.extended_data[((dec.p[current_component].y - 1) * (dec.w + 2) * 3) + ((dec.p[current_component].x - 1) * 3) + current_component];
	    dec.Rd = dec.Rb;
	}
 	else
	{ 
	 	dec.Ra = dec.extended_data[(dec.p[current_component].y * (dec.w + 2) * 3) + ((dec.p[current_component].x - 1) * 3) + current_component];
	 	dec.Rc = dec.extended_data[((dec.p[current_component].y - 1) * (dec.w + 2) * 3) + ((dec.p[current_component].x - 1) * 3) + current_component];
	 	dec.Rd = dec.extended_data[((dec.p[current_component].y - 1) * (dec.w + 2) * 3) + ((dec.p[current_component].x + 1) * 3) + current_component];
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

static uint8_t GetFromBitStream()
{
	uint8_t bit_set_mask[] = 
	{	
		0x01, 0x02, 0x04, 0x08,	0x10, 0x20, 0x40, 0x80
	};
  	if (read_position < 0)
    { 	
		byte_count = byte_count + 1;	
      	current_read_byte = dec.bitstream[byte_count];	    
   		read_position = 7;	
    }
  	if (current_read_byte & bit_set_mask[read_position--])
	{
    	return(1);
    }
    else
    {    	
  		return(0);
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
			dec.extended_data[(dec.p[current_component].y * (dec.w + 2) * 3) + ((dec.p[current_component].x + i) * 3) + current_component] = dec.Ra;	
		}
		dec.p[current_component].x += cnt;
	} 
	while (cnt == dec.RANGE);
	if (dec.p[current_component].x > dec.w)
	{
		dec.p[current_component].x = dec.w;
		run_flag = 0;
	}
	else
	{
		run_flag = 1;
	}
	if (run_flag)
	{
		dec.p[current_component].x--;
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
		dec.extended_data[(dec.p[current_component].y * (dec.w + 2) * 3) + (dec.p[current_component].x * 3) + current_component] = (uint8_t)dec.Rx;		
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
 	dec.extended_data[(dec.p[current_component].y * (dec.w + 2) * 3) + (dec.p[current_component].x * 3) + current_component] = dec.Rx;
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
	uint32_t i = 0;
	uint32_t j = 0;
	uint8_t k = 0;
	dout = (uint8_t*)calloc(3 * w * h, sizeof(uint8_t));
	Initializations(data, w, h, size, coding_mode);
	rAnsDecodingFirst();
	for(;;)
	{
		if(dec.p[2].x == w && dec.p[2].y == h)
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
	return dout;
}
