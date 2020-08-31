#include "globals.h"
#include "encoder.h"
#include "decoder.h"

#define CODING_MODE 1
#define PIXELS_MODE 1

/* 
	Title: 	Behavoiral code of JPEG-LS decoder and encoder (Golomb).
	Author:	Dawid Bartosiak
	For more information refer to JPEG website (https://jpeg.org/)
	and JPEG-LS specification (Lossless and near-lossless compression 
	of continuous-tone still images - Baseline).
*/	

int main()
{
	FILE* f;
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
	uint16_t t[2];
	char name[12];	
	printf("Enter start: ");
	scanf("%d", t);
	printf("Enter stop: ");
	scanf("%d", t + 1);
	if (CODING_MODE == 1)
	{		
		printf("Coding variant: rANS + Golomb Rice.\n");
		printf("Coding mode: regular + run.\n");
		if (PIXELS_MODE == 1)
		{
			printf("Pixels mode: modified.\n");
			f = fopen("raport_color_nilv_rans_golomb_rice_regular_run_modified.txt", "wt");
		}
		else
		{
			printf("Pixels mode: no-modified.\n");
			f = fopen("raport_color_nilv_rans_golomb_rice_regular_run_nomodified.txt", "wt");
		}
	}
	else
	{
		printf("Coding variant: rANS.\n");
		printf("Coding mode: regular.\n");
		if (PIXELS_MODE == 1)
		{
			printf("Pixels mode: modified.\n");
			f = fopen("raport_color_nilv_rans_regular_modified.txt", "wt");
		}
		else
		{
			printf("Pixels mode: no-modified.\n");
			f = fopen("raport_color_nilv_rans_regular_nomodified.txt", "wt");
		}
	}
	for (i = t[0]; i <= t[1]; i++)
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
		printf("Color nilv image.\n");
		printf("Image: %s\n", name);
		data_3d_e = Read3DDat(&w, &h, name);
		start = clock();
		data_1d_e = Encode(data_3d_e, w, h, &size, CODING_MODE, PIXELS_MODE);
		end = clock();
		function_time = ((double) (end - start)) / CLOCKS_PER_SEC;
		printf("REPORT\n");
		printf("Input data size: %d bytes.\n", 3 * w * h);
		printf("Encoded data size: %d bytes.\n", size);
		printf("Encoding time: %f miliseconds.\n", function_time);
		fwrite(name, sizeof(char), strlen(name), f);
		fwrite("\t", sizeof(char), 1, f);
		itoa(3 * w * h, name, 10);
		fwrite(name, sizeof(char), strlen(name), f);
		fwrite("\t", sizeof(char), 1, f);
		itoa(size, name, 10);		
		fwrite(name, sizeof(char), strlen(name), f);
		fwrite("\t", sizeof(char), 1, f);
		sprintf(name, "%f", function_time);
		fwrite(name, sizeof(char), strlen(name), f);   
		start = clock();
		data_3d_d = Decode(data_1d_e, w, h, size, CODING_MODE, PIXELS_MODE);
		end = clock();
		function_time = ((double) (end - start)) / CLOCKS_PER_SEC;
		printf("REPORT\n");
		printf("Input data size: %d bytes.\n", size);
		printf("Decoded data size: %d bytes.\n", 3 * w * h);
		printf("Decoding time: %f miliseconds.\n", function_time);
		sprintf(name, "%f", function_time);
		fwrite("\t", sizeof(char), 1, f);
		fwrite(name, sizeof(char), strlen(name), f);
		fwrite("\t", sizeof(char), 1, f);
		free(data_3d_e);		
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
		data_3d_e = Read3DDat(&w, &h, name);
		if(Compare3D(data_3d_e, data_3d_d, w, h))
		{
			fwrite("FAIL", sizeof(char), strlen("FAIL"), f);
		}
		else
		{
			fwrite("PASS", sizeof(char), strlen("PASS"), f);
		}
		fwrite("\n", sizeof(char), 1, f);
		free(data_3d_e);
		free(data_1d_e);
		free(data_3d_d);
	}		
	fclose(f);
	system("pause");
	return 0;
}	

