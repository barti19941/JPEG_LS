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

/* WORKING BINARY tANS */
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
	printf("\nbitstream size = %d\n", bitstream_size);
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


/* WORKING NUMERAL rANS 
int main() 
{
	unsigned char* data_1 = NULL;
	unsigned char* data_2 = NULL;
	unsigned char* bitstream = NULL;
	unsigned short int data_size = 0;
	unsigned long long int x = 0;
	int i = 0;
	data_size = read_symbols(&data_1);
	rAnsInitialize(data_1, data_size);
	x = rAnsEncodingNumeralWhole(data_1, data_size);
	data_2 = rAnsDecodingNumeralWhole(x, data_size);
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

/* WORKING BINARY rANS 
int main() 
{
	unsigned char* data_1 = NULL;
	unsigned char* data_2 = NULL;
	unsigned char* bitstream = NULL;
	unsigned short int data_size = 0;
	unsigned short int bitstream_size = 0;
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
