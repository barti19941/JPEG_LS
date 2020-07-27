#include "ans.h"

static tANS tans;
static rANS rans;

/**********************************************************************
	GetFromBitstream	:	Read 1 bit from bitstream.
							bitstream - pointer to bitstream
							bitstream_size - size of bits to read
**********************************************************************/
static int GetFromBitstream(unsigned char* bitstream, int bitstream_size)
{
	static int read_position = -1;
	static int byte_count = 0;
	static int current_read_byte = 0;
	int bit_set_mask[] = { 	0x00000001, 0x00000002, 0x00000004, 0x00000008,
								0x00000010, 0x00000020, 0x00000040, 0x00000080};
	if (read_position == -1)
	{
		read_position = bitstream_size % 8;
		if (read_position == 0)
		{
			byte_count = (unsigned int)ceil(bitstream_size/8) - 1;
		}
		else
		{
			read_position = 8 - read_position;
			byte_count = (unsigned int)ceil(bitstream_size/8);	
		}		
		current_read_byte = bitstream[byte_count];	
	}
	else
	{
		if (read_position == 8)
    	{ 	
			byte_count = byte_count - 1;	
      		current_read_byte = bitstream[byte_count];	    
			read_position = 0;	      
    	}
	}
	if (current_read_byte & bit_set_mask[read_position++])
	{
	   	return(1);
    }
    return(0);
}

/**********************************************************************
	AppendToBitstream	:	This function appends the non-negative number 
							i in binary form to the encoded bitstream 
							using j bits.
							b - number
							n - bits to append
							bitstream - pointer to bitstream
**********************************************************************/
static void AppendToBitstream(int b, int n, unsigned char* bitstream)
{
	static int write_position = 7;
	static int current_write_byte = 0;
	static int byte_count = 0;
	int bit_set_mask[] = { 	0x00000001, 0x00000002, 0x00000004, 0x00000008,
								0x00000010, 0x00000020, 0x00000040, 0x00000080};
   	while(n--)
    {
       	if(b & bit_set_mask[n]) 
	   	{
	   		printf("1");
			current_write_byte |= bit_set_mask[write_position--];	
			bitstream[byte_count] = current_write_byte;		
			if (write_position < 0) 
			{				
				byte_count++;
				write_position = 7;
				current_write_byte = 0;
			}
		}
       	else 
		{
			printf("0");
			write_position--;
			bitstream[byte_count] = current_write_byte;
			if(write_position < 0)
			{
				byte_count++;
 				write_position = 7;
 				current_write_byte = 0;
 			}
		}
    }
}

/**********************************************************************
	tAnsInitialize:	This function initializes all values used during coding
				decoding tANS proccess.
				symbols - input vector with symbols to be encoded
				size - size of input vector
				step - spread symbols step
**********************************************************************/
void tAnsInitialize(unsigned char* data, unsigned short int size, unsigned char step)
{
	int i = 0;
	int j = 0;
	int k = 0;
	int x = 0;
	int X = 0;
	tans.L = (unsigned short int)pow(2, ceil(log((double)size)/log(2.0)));
	tans.R = (unsigned short int)log2((double)tans.L);
	tans.r = tans.R + 1;
	if (step == 0)
	{
		tans.step = ((5*tans.L)/8)+3;
	}
	else
	{
		tans.step = step;
	}
	tans.max = 0;
	for (i = 0; i < size; i++)
	{
		if (data[i] > tans.max)
		{
			tans.max = data[i];
		}
	}
	tans.max++;
	tans.s = (unsigned char*)calloc(tans.max, sizeof(unsigned char));
	for (i = 0; i < tans.max; i++)
	{
		tans.s[i] = i;
	}
	tans.Ls = (unsigned char*)calloc(tans.max, sizeof(unsigned char));
	for (i = 0; i < tans.max; i++)
	{
		for (j = 0; j < size; j++)
		{
			if (data[j] == tans.s[i])
			{
				tans.Ls[i]++;
			}
		}
		tans.Ls[i] = (unsigned char)round(((double)tans.Ls[i]/(double)size) * (double)tans.L);
	}
	tans.symbols = (unsigned char*)calloc(tans.L, sizeof(unsigned char));
	k = 0;
	for (i = 0; i < tans.max; i++)
	{
		for(j = k; j < tans.Ls[tans.s[i]] + k; j++)
		{
			tans.symbols[j] = tans.s[i];			
		}
		k += tans.Ls[tans.s[i]];
	}
	for (i = 0; i < tans.max; i++)
	{
		for (j = 0; j < tans.Ls[tans.s[i]]; j++)
		{
			tans.symbols[X] = tans.s[i];
			X = (X + tans.step) & (tans.L - 1);
		}
	}
	tans.k = (unsigned short int*)calloc(tans.max, sizeof(unsigned short int));
	tans.nb = (unsigned short int*)calloc(tans.max, sizeof(unsigned short int));
	for (i = 0; i < tans.max; i++)
	{
		tans.k[i] = tans.R - floor((double)log2((double)tans.Ls[i]));
		tans.nb[i] = (tans.k[i] << tans.r) - (tans.Ls[i] << tans.k[i]);
	}
	tans.start = (unsigned short int*)calloc(tans.max, sizeof(unsigned short int));
	tans.next = (unsigned short int*)calloc(tans.max, sizeof(unsigned short int));
	for (i = 0; i < tans.max; i++)
	{
		tans.start[i] = -tans.Ls[i];
		for (j = 0; j < i; j++)
		{
			tans.start[i] += tans.Ls[j];
		}
		tans.next[i] = tans.Ls[i];
	}
	tans.encodingTable = (unsigned char*)calloc(tans.L, sizeof(unsigned char));
	for (i = tans.L; i < (2 * tans.L); i++)
	{
		j = tans.symbols[i - tans.L];
		tans.encodingTable[tans.start[j] + (tans.next[j]++)] = i;
	}
	for (i = 0; i < tans.max; i++)
	{
		tans.start[i] = -tans.Ls[i];
		for (j = 0; j < i; j++)
		{
			tans.start[i] += tans.Ls[j];
		}
		tans.next[i] = tans.Ls[i];
	}
	tans.decodingTable = (t*)calloc(tans.L, sizeof(t));
	for (i = 0; i < tans.max; i++)
	{
		tans.next[i] = tans.Ls[i];
	}
	for (i = 0; i < tans.L; i++)
	{
		tans.decodingTable[i].symbol = tans.symbols[i];
		j = tans.next[tans.decodingTable[i].symbol]++;
		tans.decodingTable[i].nbBits = tans.R - (unsigned char)(floor(log2(j)));
		tans.decodingTable[i].newX = (j << tans.decodingTable[i].nbBits);
	}
}

/**********************************************************************
	tAnsDeinitialize: This function clear all values used during coding,
				decoding tANS process.
**********************************************************************/
void tAnsDeinitialize(void)
{
	tans.L = 0;
	tans.R = 0;
	tans.r = 0;
	tans.max = 0;
	tans.step = 0;
	free(tans.Ls);
	free(tans.s);
	free(tans.symbols);
	free(tans.k);
	free(tans.nb);
	free(tans.next);
	free(tans.start);
	free(tans.encodingTable);
	free(tans.decodingTable);
}

/**********************************************************************
	tAnsEncodingBinaryWhole: This function perform coding of symbols from
				data vector of size data_size to bitstream using tans
				encoding/decoding table.
				data - pointer to data vector
				data_size - size of data to encode
				bitstream_size - size of bitstream (in bits)
				ret - bitstream to which data is encoded
**********************************************************************/
unsigned char* tAnsEncodingBinaryWhole(unsigned char* data, unsigned short int data_size, unsigned short int* bitstream_size)
{
	int i = 0;
	int j = 0;
	unsigned short int tmp = 0;
	unsigned char nbBits = 0;
	unsigned short int x = tans.L; 
	unsigned char* bitstream = NULL;
	bitstream = (unsigned char*)calloc((unsigned short int)~(*bitstream_size), sizeof(unsigned char));
	for (i = 0; i < data_size; i++)
	{
		nbBits = (x + tans.nb[data[i]]) >> tans.r;
		tmp = x;
		for (j = 0; j < nbBits; j++)
		{
			AppendToBitstream(tmp, 1, bitstream);
			tmp >>= 1;
			(*bitstream_size) += 1;
		}		
		x = tans.encodingTable[tans.start[data[i]] + (x >> nbBits)];
	}
	tmp = x;
	for (i = 0; i < tans.R; i++)
	{
		AppendToBitstream(tmp, 1, bitstream);
		tmp >>= 1;
		(*bitstream_size) += 1;	
	}
	return bitstream;
}

/**********************************************************************
	tAnsDecodingBinaryWhole: This function perform decoding of bitstream
				of size bitstream_size using tans encoding/decoding table.
				bitstream_size - size of bitstream (in bits)
				data_size - size of data to decode
				bitstream - pointer to bitstream
				ret - pointer to vector with decoded symbols
**********************************************************************/
unsigned char* tAnsDecodingBinaryWhole(unsigned char* bitstream, unsigned short int data_size, unsigned short int bitstream_size)
{
	unsigned char* data = (unsigned char*)calloc(data_size, sizeof(unsigned char));
	unsigned short int x = 0;
	int i = 0;
	int j = 0;
	unsigned short int tmp = 0;
	x |= (1 << tans.R);
	for (i = 0; i < tans.R; i++)
	{
		x |= ((unsigned int)GetFromBitstream(bitstream, bitstream_size) << tans.R-(i+1));
	}
	for (i = 0; i < data_size; i++)
	{
		data[i] = tans.decodingTable[x - tans.L].symbol;
		tmp = 0;
		for (j = 0; j < tans.decodingTable[x-tans.L].nbBits; j++)
		{
			tmp = (tmp << 1) + GetFromBitstream(bitstream, bitstream_size);
		}	
		x = tans.decodingTable[x-tans.L].newX + tmp;
	}
	return data;
}

/**********************************************************************
	rAnsInitialize:	This function initializes all values used during coding
				decoding rANS proccess.
				symbols - input vector with symbols to be encoded
				size - size of input vector
**********************************************************************/
void rAnsInitialize(unsigned char* data, unsigned short int size)
{
	int i = 0;
	int j = 0; 
	rans.l = (unsigned short int)pow(2, ceil(log((double)size)/log(2.0)));
	rans.n = (unsigned short int)log2((double)rans.l);
	rans.k = 1;
	rans.b = (unsigned int)pow(2, (double)rans.k);
	rans.d = rans.k + rans.n - rans.n;
	for (i = 0; i < size; i++)
	{
		if (data[i] > rans.max)
		{
			rans.max = data[i];
		}
	}
	rans.max++;
	rans.symbols = (unsigned char*)calloc(rans.max, sizeof(unsigned char));
	for (i = 0; i < rans.max; i++)
	{
		rans.symbols[i] = i;
	}
	rans.fs = (unsigned char*)calloc(rans.max, sizeof(unsigned char));
	for (i = 0; i < rans.max; i++)
	{
		for (j = 0; j < size; j++)
		{
			if (data[j] == rans.symbols[i])
			{
				rans.fs[i]++;
			}
		}
		rans.fs[i] = (unsigned char)round(((double)rans.fs[i]/(double)size) * (double)rans.l);
	}
	rans.cs = (unsigned char*)calloc(rans.max, sizeof(unsigned char));
	rans.cs[0] = 0;
	for (i = 1; i < rans.max; i++)
	{
		for(j = 0; j < i; j++)
		{
			rans.cs[i] += rans.fs[j];
		}			
	}
	rans.symbols_inv = (unsigned char* )calloc(rans.l, sizeof(unsigned char));
	for (i = 0; i < rans.l; i++)
	{
		for (j = 0; j < rans.max; j++)
		{
			if (rans.cs[rans.symbols[j]] <= i)
			{
				rans.symbols_inv[i] = rans.symbols[j];
			}
		}
	}
}

/**********************************************************************
	rAnsDeinitialize: This function clear all values used during coding,
				decoding rANS process.
**********************************************************************/
void rAnsDeinitialize(void)
{
	rans.max = 0;
	rans.n = 0;
	rans.l = 0;
	rans.k = 0;
	rans.b = 0;
	rans.d = 0;
	free(rans.symbols);
	free(rans.fs);
	free(rans.cs);
	free(rans.symbols_inv);
}

/**********************************************************************
	rAnsEncodingNumeralWhole: This function perform coding of symbols from
				data vector of size data_size using rans equations.
				data - pointer to data vector
				data_size - size of data to encode
				ret - natural number to which data is encoded
**********************************************************************/
unsigned long long int rAnsEncodingNumeralWhole(unsigned char* data, unsigned short int data_size)
{
	unsigned long long int x = 0;
	int i = 0;
	for (i = 0; i < data_size; i++)
	{
		x = rAnsEncodingNumeralSingle(x, data[i]);
	}
	return x;
}

/**********************************************************************
	rAnsDecodingNumeralWhole: This function perform decoding sequence of
				symbols of size data_size using rans equations.
				x - natural number to decode
				data_size - size of data to decode
				ret - pointer to vector with decoded symbols
**********************************************************************/
unsigned char* rAnsDecodingNumeralWhole(unsigned long long int x, unsigned short int data_size)
{
	unsigned char* data = (unsigned char*)calloc(data_size, sizeof(unsigned char));
	int i = 0;
	for (i = 0; i < data_size; i++)
	{
		x = rAnsDecodingNumeralSingle(x, &(data[i]));
	}
	return data;
}

/**********************************************************************
	rAnsEncodingBinaryWhole: This function perform coding of symbols from
				data vector of size data_size to bitstream using rans equations.
				data - pointer to data vector
				data_size - size of data to encode
				bitstream_size - size of bitstream (in bits)
				ret - bitstream to which data is encoded
**********************************************************************/
unsigned char* rAnsEncodingBinaryWhole(unsigned char* data, unsigned short int data_size, unsigned short int* bitstream_size)
{
	unsigned int x = rans.l;
	unsigned char* bitstream = NULL;
	unsigned short int tmp = 0;
	int i = 0;
	int j = 0;
	bitstream = (unsigned char*)calloc((unsigned short int)~(*bitstream_size), sizeof(unsigned char));
	for (i = 0; i < data_size; i++)
	{
		while(x >= (rans.fs[data[i]] << rans.d))
		{	
			tmp = x & (rans.b-1);
			for (j = 0; j < rans.k; j++)
			{
				AppendToBitstream(tmp, 1, bitstream);
				(*bitstream_size) += 1;
				tmp >>= 1;
			}			
			x >>= rans.k;
		}
		x = rAnsEncodingNumeralSingle(x, data[i]);
	}	
	for (i = 0; i < (rans.n + rans.k); i++)
	{
		AppendToBitstream(x, 1, bitstream);
		x >>= 1;
		(*bitstream_size) += 1;	
	}
	return bitstream;	
}

/**********************************************************************
	rAnsDecodingBinaryWhole: This function perform decoding of bitstream
				of size bitstream_size using rans equations.
				bitstream_size - size of bitstream (in bits)
				data_size - size of data to decode
				bitstream - pointer to bitstream
				ret - pointer to vector with decoded symbols
**********************************************************************/
unsigned char* rAnsDecodingBinaryWhole(unsigned char* bitstream, unsigned short int data_size, unsigned short int bitstream_size)
{
	unsigned char* data = (unsigned char*)calloc(data_size, sizeof(unsigned char));
	unsigned int x = 0;
	int i = 0;
	int j = 0;
	for (i = 0; i < (rans.n + rans.k); i++)
	{
		x |= ((unsigned int)GetFromBitstream(bitstream, bitstream_size) << ((rans.k + rans.n)-(i+1)));
	}
	for (i = 0; i < data_size; i++)
	{
		x = rAnsDecodingNumeralSingle(x, &(data[i]));	
		while (x < rans.l)
		{
			for (j = 0; j < rans.k; j++)
			{
				x = (x << 1) + GetFromBitstream(bitstream, bitstream_size);
			}					
		}
	}
	return data;
}

/**********************************************************************
	rAnsEncodingNumeralSingle: This function perform coding of symbol s
				to natural number x using rans equations.
				x - natural number to which symbol s will be encoded,
				s - symbol s to encode,
				ret - x' ~ x/p (p - probability of p), C(s,x) = x',
				this is new natural number
**********************************************************************/
unsigned long long int rAnsEncodingNumeralSingle(unsigned long long int x, unsigned char s)
{
	return ((unsigned short int)floor(x/rans.fs[s]) << rans.n) + (x % rans.fs[s]) + rans.cs[s];	
}

/**********************************************************************
	rAnsDecodingNumeralSingle: This function perform decoding of symbol
				s from natural number x using rans equations.
				x - natural number from which symbol s will be decoded,
				s - decoded symbol,
				ret - natural number after decoding process
**********************************************************************/

unsigned long long int rAnsDecodingNumeralSingle(unsigned long long int x, unsigned char* s)
{
	*s = rans.symbols_inv[x & (rans.l - 1)];
	return rans.fs[*s] * (x >> rans.n) + (x & (rans.l - 1)) - rans.cs[*s];
}

