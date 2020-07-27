#include "decoder.h"

static decoder dec;

static uint8_t current_read_byte_regular;
static int8_t read_position_regular;
static int32_t byte_count_regular;

static uint8_t current_read_byte_run;
static int8_t read_position_run;
static int32_t byte_count_run;

static void Initializations(uint8_t* data, uint16_t w, uint16_t h, uint32_t size, uint8_t coding_mode)
{
	uint32_t i = 0;		
	uint32_t j = 0;
	uint32_t k = 0;
	uint32_t X = 0;
	dec.coding_mode = coding_mode;	
	dec.w = w;
	dec.h = h;
	dec.bitstream = (uint8_t*)calloc(size, sizeof(uint8_t));
	memcpy(dec.bitstream, data, size);
	dec.extended_data = (uint8_t*)calloc((dec.w + 2) * (dec.h + 1) * 3, sizeof(uint8_t));
	current_read_byte_regular = 0;
	read_position_regular = -1;
	byte_count_regular = -1;
	current_read_byte_run = 0;
	read_position_run = 8;
	byte_count_run = size;
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
	dec.Ls = (uint16_t*)calloc(dec.max + 1, sizeof(uint16_t));
	dec.Ls[0] = 5876; dec.Ls[1] = 5348; dec.Ls[2] = 4866; dec.Ls[3] = 4428; dec.Ls[4] = 4030; dec.Ls[5] = 3667; dec.Ls[6] = 3337; dec.Ls[7] = 3037;
	dec.Ls[8] = 2764; dec.Ls[9] = 2515; dec.Ls[10] = 2289; dec.Ls[11] = 2083; dec.Ls[12] = 1896; dec.Ls[13] = 1725; dec.Ls[14] = 1570; dec.Ls[15] = 1429;
	dec.Ls[16] = 1300; dec.Ls[17] = 1183; dec.Ls[18] = 1077; dec.Ls[19] = 980; dec.Ls[20] = 892; dec.Ls[21] = 812; dec.Ls[22] = 739; dec.Ls[23] = 672;
	dec.Ls[24] = 612; dec.Ls[25] = 557; dec.Ls[26] = 507; dec.Ls[27] = 461; dec.Ls[28] = 420; dec.Ls[29] = 382; dec.Ls[30] = 348; dec.Ls[31] = 317;
	dec.Ls[32] = 288; dec.Ls[33] = 262; dec.Ls[34] = 239; dec.Ls[35] = 218; dec.Ls[36] = 198; dec.Ls[37] = 180; dec.Ls[38] = 164; dec.Ls[39] = 149;
	dec.Ls[40] = 136; dec.Ls[41] = 124; dec.Ls[42] = 113; dec.Ls[43] = 103; dec.Ls[44] = 94; dec.Ls[45] = 85; dec.Ls[46] = 78; dec.Ls[47] = 71;
	dec.Ls[48] = 65; dec.Ls[49] = 59; dec.Ls[50] = 54; dec.Ls[51] = 49; dec.Ls[52] = 45; dec.Ls[53] = 41; dec.Ls[54] = 37; dec.Ls[55] = 34;
	dec.Ls[56] = 31; dec.Ls[57] = 28; dec.Ls[58] = 26; dec.Ls[59] = 24; dec.Ls[60] = 21; dec.Ls[61] = 20; dec.Ls[62] = 18; dec.Ls[63] = 16;
	dec.Ls[64] = 15; dec.Ls[65] = 14; dec.Ls[66] = 13; dec.Ls[67] = 12; dec.Ls[68] = 11; dec.Ls[69] = 10; dec.Ls[70] = 9; dec.Ls[71] = 8;
	dec.Ls[72] = 8; dec.Ls[73] = 7; dec.Ls[74] = 6; dec.Ls[75] = 6; dec.Ls[76] = 6; dec.Ls[77] = 5; dec.Ls[78] = 5; dec.Ls[79] = 4;
	dec.Ls[80] = 4; dec.Ls[81] = 4; dec.Ls[82] = 4; dec.Ls[83] = 3; dec.Ls[84] = 3; dec.Ls[85] = 3; dec.Ls[86] = 3; dec.Ls[87] = 3;
	dec.Ls[88] = 2; dec.Ls[89] = 2; dec.Ls[90] = 2; dec.Ls[91] = 2; dec.Ls[92] = 2; dec.Ls[93] = 2; dec.Ls[94] = 2; dec.Ls[95] = 2;
	dec.Ls[96] = 2; dec.Ls[97] = 2; dec.Ls[98] = 2; dec.Ls[99] = 2; dec.Ls[100] = 1; dec.Ls[101] = 1; dec.Ls[102] = 1; dec.Ls[103] = 1;
	dec.Ls[104] = 1; dec.Ls[105] = 1; dec.Ls[106] = 1; dec.Ls[107] = 1; dec.Ls[108] = 1; dec.Ls[109] = 1; dec.Ls[110] = 1; dec.Ls[111] = 1;
	dec.Ls[112] = 1; dec.Ls[113] = 1; dec.Ls[114] = 1; dec.Ls[115] = 1; dec.Ls[116] = 1; dec.Ls[117] = 1; dec.Ls[118] = 1; dec.Ls[119] = 1;
	dec.Ls[120] = 1; dec.Ls[121] = 1; dec.Ls[122] = 1; dec.Ls[123] = 1; dec.Ls[124] = 1; dec.Ls[125] = 1; dec.Ls[126] = 1; dec.Ls[127] = 1;
	dec.Ls[128] = 1; dec.Ls[129] = 1; dec.Ls[130] = 1; dec.Ls[131] = 1; dec.Ls[132] = 1; dec.Ls[133] = 1; dec.Ls[134] = 1; dec.Ls[135] = 1;
	dec.Ls[136] = 1; dec.Ls[137] = 1; dec.Ls[138] = 1; dec.Ls[139] = 1; dec.Ls[140] = 1; dec.Ls[141] = 1; dec.Ls[142] = 1; dec.Ls[143] = 1;
	dec.Ls[144] = 1; dec.Ls[145] = 1; dec.Ls[146] = 1; dec.Ls[147] = 1; dec.Ls[148] = 1; dec.Ls[149] = 1; dec.Ls[150] = 1; dec.Ls[151] = 1;
	dec.Ls[152] = 1; dec.Ls[153] = 1; dec.Ls[154] = 1; dec.Ls[155] = 1; dec.Ls[156] = 1; dec.Ls[157] = 1; dec.Ls[158] = 1; dec.Ls[159] = 1;
	dec.Ls[160] = 1; dec.Ls[161] = 1; dec.Ls[162] = 1; dec.Ls[163] = 1; dec.Ls[164] = 1; dec.Ls[165] = 1; dec.Ls[166] = 1; dec.Ls[167] = 1;
	dec.Ls[168] = 1; dec.Ls[169] = 1; dec.Ls[170] = 1; dec.Ls[171] = 1; dec.Ls[172] = 1; dec.Ls[173] = 1; dec.Ls[174] = 1; dec.Ls[175] = 1;
	dec.Ls[176] = 1; dec.Ls[177] = 1; dec.Ls[178] = 1; dec.Ls[179] = 1; dec.Ls[180] = 1; dec.Ls[181] = 1; dec.Ls[182] = 1; dec.Ls[183] = 1;
	dec.Ls[184] = 1; dec.Ls[185] = 1; dec.Ls[186] = 1; dec.Ls[187] = 1; dec.Ls[188] = 1; dec.Ls[189] = 1; dec.Ls[190] = 1; dec.Ls[191] = 1;
	dec.Ls[192] = 1; dec.Ls[193] = 1; dec.Ls[194] = 1; dec.Ls[195] = 1; dec.Ls[196] = 1; dec.Ls[197] = 1; dec.Ls[198] = 1; dec.Ls[199] = 1;
	dec.Ls[200] = 1; dec.Ls[201] = 1; dec.Ls[202] = 1; dec.Ls[203] = 1; dec.Ls[204] = 1; dec.Ls[205] = 1; dec.Ls[206] = 1; dec.Ls[207] = 1;
	dec.Ls[208] = 1; dec.Ls[209] = 1; dec.Ls[210] = 1; dec.Ls[211] = 1; dec.Ls[212] = 1; dec.Ls[213] = 1; dec.Ls[214] = 1; dec.Ls[215] = 1;
	dec.Ls[216] = 1; dec.Ls[217] = 1; dec.Ls[218] = 1; dec.Ls[219] = 1; dec.Ls[220] = 1; dec.Ls[221] = 1; dec.Ls[222] = 1; dec.Ls[223] = 1;
	dec.Ls[224] = 1; dec.Ls[225] = 1; dec.Ls[226] = 1; dec.Ls[227] = 1; dec.Ls[228] = 1; dec.Ls[229] = 1; dec.Ls[230] = 1; dec.Ls[231] = 1;
	dec.Ls[232] = 1; dec.Ls[233] = 1; dec.Ls[234] = 1; dec.Ls[235] = 1; dec.Ls[236] = 1; dec.Ls[237] = 1; dec.Ls[238] = 1; dec.Ls[239] = 1;
	dec.Ls[240] = 1; dec.Ls[241] = 1; dec.Ls[242] = 1; dec.Ls[243] = 1; dec.Ls[244] = 1; dec.Ls[245] = 1; dec.Ls[246] = 1; dec.Ls[247] = 1;
	dec.Ls[248] = 1; dec.Ls[249] = 1; dec.Ls[250] = 1; dec.Ls[251] = 1; dec.Ls[252] = 1; dec.Ls[253] = 1; dec.Ls[254] = 1; dec.Ls[255] = 1;
	dec.Ls[256] = 1; 
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
	dec.next = (uint16_t*)calloc(dec.max + 1, sizeof(uint16_t));
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


static uint8_t GetFromBitStreamRegular()
{
	uint8_t bit_set_mask[] = 
	{	
		0x01, 0x02, 0x04, 0x08,	0x10, 0x20, 0x40, 0x80
	};
  	if (read_position_regular < 0)
    { 	
		byte_count_regular = byte_count_regular + 1;	
      	current_read_byte_regular = dec.bitstream[byte_count_regular];	    
   		read_position_regular = 7;	
    }
  	if (current_read_byte_regular & bit_set_mask[read_position_regular--])
	{
    	return(1);
    }
    else
    {    	
  		return(0);
	}
}

static uint8_t GetFromBitStreamRun()
{
	uint8_t bit_set_mask[] = 
	{	
		0x01, 0x02, 0x04, 0x08,	0x10, 0x20, 0x40, 0x80
	};
  	if (read_position_run > 7)
    { 	
		byte_count_run = byte_count_run - 1;	
      	current_read_byte_run = dec.bitstream[byte_count_run];	    
   		read_position_run = 0;	
    }
  	if (current_read_byte_run & bit_set_mask[read_position_run++])
	{
    	return(1);
    }
    else
    {    	
  		return(0);
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

uint8_t* Decode(uint8_t* data, uint16_t w, uint16_t h, uint32_t size, uint8_t coding_mode)
{	
	uint8_t* dout;
	uint32_t i = 0;
	uint32_t j = 0;
	uint8_t k = 0;
	dout = (uint8_t*)calloc(3 * w * h, sizeof(uint8_t));
	Initializations(data, w, h, size, coding_mode);
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
	return dout;
}
