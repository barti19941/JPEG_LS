#include "ans.h"

static unsigned char* c;												//cumulative probabilites		
static unsigned char* f;												//normalized probabilities p ~= f/M
static unsigned char* symbols;											//list of all symbols
static unsigned short int M;											//value to which probabilities are normalized, power of 2
static unsigned char n;													//M = 2^n
static unsigned char* c_inv;											//c_inv(x) = max{s, c[s] <= x}
static unsigned char k;													//quantity of bits read and wrote to bitstream
static unsigned int b;													//b = 2^k
static unsigned int l;													//renormalization value
static unsigned char d;													//renormalzation factor


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
**********************************************************************/
void tAnsInitialize(unsigned char* data, unsigned short int size)
{
	
}

/**********************************************************************
	rAnsInitialize:	This function initializes all values used during coding
				decoding rANS proccess.
				symbols - input vector with symbols to be encoded
				size - size of input vector
**********************************************************************/
void rAnsInitialize(unsigned char* data, unsigned short int size)
{
	int i, j;
	unsigned short int max = 0;
	k = BITS_TO_OUTPUT;
	b = (unsigned int)pow(2, (double)k);
	M = (unsigned short int)pow(2, ceil(log((double)size)/log(2.0)));
	l = RENORMALIZATION_FACTOR * M;
	n = (unsigned short int)log2((double)M);
	d = BITS_TO_OUTPUT;
	for (i = 0; i < size; i++)
	{
		if (data[i] > max)
		{
			max = data[i];
		}
	}
	max++;
	symbols = (unsigned char*)calloc(max, sizeof(unsigned char));
	for (i = 0; i < max; i++)
	{
		symbols[i] = i;
	}
	f = (unsigned char*)calloc(max, sizeof(unsigned char));
	for (i = 0; i < max; i++)
	{
		for (j = 0; j < size; j++)
		{
			if (data[j] == symbols[i])
			{
				f[i]++;
			}
		}
		f[i] = (unsigned char)round(((double)f[i]/(double)size) * (double)M);
	}
	c = (unsigned char*)calloc(max, sizeof(unsigned char));
	c[i] = 0;
	for (i = 1; i < max; i++)
	{
		for(j = 0; j < i; j++)
		{
			c[i] += f[j];
		}
	}
	c_inv = (unsigned char* )calloc(M, sizeof(unsigned char));
	for (i = 0; i < M; i++)
	{
		for (j = 0; j < max; j++)
		{
			if (c[symbols[j]] <= i)
			{
				c_inv[i] = symbols[j];
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
	n = 0;
	M = 0;
	k = 0;
	b = 0;
	l = 0;
	d = 0;
	free(symbols);
	free(c);
	free(f);
	free(c_inv);
	symbols = NULL;
	c = NULL;
	f = NULL;
	c_inv = NULL;
}

/**********************************************************************
	rAnsEncodingNumeralWhole: This function perform coding of symbols from
				data vector of size data_size.
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
				symbols of size data_size.
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
				data vector of size data_size to bitstream.
				data - pointer to data vector
				data_size - size of data to encode
				bitstream_size - size of bitstream (in bits)
				ret - bitstream to which data is encoded
**********************************************************************/
unsigned char* rAnsEncodingBinaryWhole(unsigned char* data, unsigned short int data_size, unsigned short int* bitstream_size)
{
	unsigned int x = l;
	unsigned char* bitstream;
	int i = 0;
	bitstream = (unsigned char*)calloc((unsigned short int)~(*bitstream_size), sizeof(unsigned char));
	for (i = 0; i < data_size; i++)
	{
		while(x >= (f[data[i]] << d))
		{
			AppendToBitstream(x & (b-1), k, bitstream);
			(*bitstream_size) += k;
			x >>= k;
		}	
		x = rAnsEncodingNumeralSingle(x, data[i]);
	}	
	while (x != 1)
	{
		AppendToBitstream(x & (b-1), k, bitstream);
		x >>= k;
		(*bitstream_size) += k;
	}
	return bitstream;	
}

/**********************************************************************
	rAnsDecodingBinaryWhole: This function perform decoding of bitstream
				of size bitstream_size.
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
	x |= (1 << n);
	for (i = 0; i < n; i++)
	{
		x |= ((unsigned int)GetFromBitstream(bitstream, bitstream_size) << n-(i+1));
	}
	for (i = 0; i < data_size; i++)
	{
		x = rAnsDecodingNumeralSingle(x, &(data[i]));
		while (x < l)
		{
			x =(x << k) + GetFromBitstream(bitstream, bitstream_size);
		}
	}
	return data;
}


/**********************************************************************
	rAnsEncodingNumeralSingle: This function perform coding of symbol s
				to natural number x.
				x - natural number to which symbol s will be encoded,
				s - symbol s to encode,
				ret - x' ~ x/p (p - probability of p), C(s,x) = x',
				this is new natural number
**********************************************************************/
unsigned long long int rAnsEncodingNumeralSingle(unsigned long long int x, unsigned char s)
{
	return ((x/f[s]) << n) + (x % f[s]) + c[s];	
}

/**********************************************************************
	rAnsDecodingNumeralSingle: This function perform decoding of symbol
				s from natural number x.
				x - natural number from which symbol s will be decoded,
				s - decoded symbol,
				ret - natural number after decoding process
**********************************************************************/
unsigned long long int rAnsDecodingNumeralSingle(unsigned long long int x, unsigned char* s)
{
	*s = c_inv[x & (M-1)];
	return f[*s] * (x >> n) + (x & (M-1)) - c[*s];
}

