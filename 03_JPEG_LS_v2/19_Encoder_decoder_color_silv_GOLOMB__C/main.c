#include "globals.h"
#include "encoder.h"
#include "decoder.h"

/* 
	Title: 	Behavoiral code of JPEG-LS decoder and encoder (Golomb).
	Author:	Dawid Bartosiak
	For more information refer to JPEG website (https://jpeg.org/)
	and JPEG-LS specification (Lossless and near-lossless compression 
	of continuous-tone still images - Baseline).
*/	

int main()
{
	uint16_t i = 0;
	uint16_t j = 0;
	clock_t start = 0;
	clock_t end = 0;
	double function_time = 0;
	uint16_t w = 0;
	uint16_t h = 0;
	uint32_t size = 0;
	uint8_t* data_3d_e;
	uint8_t* data_1d_e;
	uint8_t* data_3d_d;
	char name[12];
	for (i = 1; i <= 1000; i++)
	{
		itoa(i, name, 10); 
		j = 0;
		while (name[j] != '\0')
		{
			j++;
		}
		name[j] = '.';
		name[j + 1] = 'd';
		name[j + 2] = 'a';
		name[j + 3] = 't';
		name[j + 4] = '\0';
		printf("%s\n", name);
		data_3d_e = Read3DDat(&w, &h, name);
		start = clock();
		data_1d_e = Encode(data_3d_e, w, h, &size, 1);
		end = clock();
		function_time = ((double) (end - start)) / CLOCKS_PER_SEC;
		printf("REPORT\n");
		printf("Input data size: %d bytes.\n", w * h);
		printf("Encoded data size: %d bytes.\n", size);
		printf("Encoding time: %f miliseconds.\n", function_time);   
		start = clock();
		data_3d_d = Decode(data_1d_e, w, h, size, 1);
		end = clock();
		function_time = ((double) (end - start)) / CLOCKS_PER_SEC;
		printf("REPORT\n");
		printf("Input data size: %d bytes.\n", size);
		printf("Decoded data size: %d bytes.\n", w * h);
		printf("Decoding time: %f miliseconds.\n", function_time);
		if(Compare3D(data_3d_e, data_3d_d, w, h))
		{
			break;
		}
		free(data_3d_e);
		free(data_1d_e);
		free(data_3d_d);
	}	
	system("pause");
	return 0;
}	
