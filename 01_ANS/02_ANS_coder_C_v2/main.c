#include "ans.h"

#define BUFFER_LENGHT 16

unsigned short int read_symbols(unsigned char** sym)
{
	FILE* file = NULL;
	char read_data[BUFFER_LENGHT];
	unsigned char size = 0;
	printf("INFO\tEnter *.dat filename to read: ");
	scanf("%s", read_data);
	file = fopen(read_data, "rb");
	fseek(file, 0, SEEK_END);	
	size = ftell(file);
	*sym = (unsigned char*)calloc(size, sizeof(unsigned char));
	fseek(file, 0, SEEK_SET);
	fread(*sym, sizeof(unsigned char), size, file);
	fclose(file);
	return size;
}

int main(void)
{
	int i = 0;
	unsigned char* data_1 = NULL;
	unsigned char* data_2 = NULL;
	unsigned char* bitstream = NULL;
	unsigned short int data_size = 0;
	unsigned short int bitstream_size = 0;
	data_size = read_symbols(&data_1);
	tAnsInitialize(data_1, data_size, 5);
	bitstream = tAnsEncodingBinaryWhole(data_1, data_size, &bitstream_size);
	data_2 = tAnsDecodingBinaryWhole(bitstream, data_size, bitstream_size);
	for (i = 0; i < data_size; i++)
	{
		printf("%d. Encoded = %d, Decoded = %d\n", i+1, data_1[i], data_2[data_size-1-i]);
	}
	tAnsDeinitialize();
	free(data_1);
	free(data_2);
	free(bitstream);
	return 0;
}

/*
int main() 
{
	unsigned char* data_1 = NULL;
	unsigned char* data_2 = NULL;
	unsigned char* bitstream = NULL;
	unsigned short int data_size = 0;
	unsigned short int bitstream_size = 0;
	unsigned long long int x = 0;
	int i = 0;
	data_size = read_symbols(&data_1);
	rAnsInitialize(data_1, data_size);
	bitstream = rAnsEncodingBinaryWhole(data_1, data_size, &bitstream_size);
	data_2 = rAnsDecodingBinaryWhole(bitstream, data_size, bitstream_size);
	for (i = 0; i < data_size; i++)
	{
		printf("%d. Encoded = %d, Decoded = %d\n", i+1, data_1[i], data_2[data_size-1-i]);
	}
	rAnsDeinitialize();
	free(data_1);
	free(data_2);
	free(bitstream);
	return 0;
}
*/
/* WORKING SPREAD
int main()
{
	int X = 0;
	//int m = 4;
	int m = 3;
	int L = 16;
	int s = 0;
	int i = 0;
	int step = 5;
	//int step = (L >> 1) + (L >> 3) + 3;
	int symbols[] = {0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2};
	//int symbols[] = {0, 1, 1, 1, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3};
	int ls[] = {3, 8, 5};
	//int ls[] = {1, 3, 2, 10};
	for (s = 0; s < m; s++)
	{
		for (i = 0; i < ls[s]; i++)
		{
			symbols[X] = s;
			X = (X + step) & (L - 1);
		}
	}
	for (i = 0; i < L; i++)
	{
		printf("symbols[%d] = %d\n", i, symbols[i]);
	}
	return 0;
}
*/

/* ENCODING, DECODING TABLE PREPARATION TRY
int main()
{
	int symbols[] = {1, 1, 0, 2, 2, 1, 0, 2, 1, 0, 2, 1, 2, 1, 1, 1};
	int ls[] = {3, 8, 5};
	int k[3] = {0, 0, 0};
	int nb[3] = {0, 0, 0};
	int nbbits[16];
	int L = 16;
	int R = 4;
	int r = R + 1;
	int m = 3;
	int s = 0;
	int x = 0;
	for (s = 0; s < m; s++)
	{
		k[s] = R - floor(log2(ls[s]));
		nb[s] = (k[s] << r) - (ls[s] << k[s]);
		printf("k[%d] = %d\n", s, k[s]);
		printf("nb[%d] = %d\n", s, nb[s]);
	}
	x = m++;
	printf("val = %d\n", m);
	return 0;
}
*/

/* SPREAD KORYCIAK
int main()
{
	int X = 0;
	//int m = 4;
	int m = 4;
	int L = 64;
	int s = 0;
	int i = 0;
	int step = 7;
	//int step = (L >> 1) + (L >> 3) + 3;
	int symbols[] = {	0, 1, 0, 1, 0, 1, 1, 2, 
						0, 3, 1, 0, 1, 0, 0, 0,
						0, 0, 3, 1, 3, 1, 2, 0,
						0, 2, 3, 3, 2, 0, 0, 3,
						2, 2, 3, 0, 1, 0, 3, 2,
						2, 3, 1, 1, 0, 3, 0, 1,
						1, 3, 2, 0, 1, 0, 0, 2,
						1, 3, 0, 1, 0, 2, 0, 3
					};
	//int symbols[] = {0, 1, 1, 1, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3};
	int ls[] = {24, 16, 11, 13};
	//int ls[] = {1, 3, 2, 10};
	for (s = 0; s < m; s++)
	{
		for (i = 0; i < ls[s]; i++)
		{
			symbols[X] = s;
			X = (X + step) & (L - 1);
		}
	}
	for (i = 0; i < L; i++)
	{
		printf("%d\t", symbols[i]);
		if ((i + 1) % 8 == 0 && i != 0)
		{
			printf("\n");
		}
	}
	return 0;
}
*/




