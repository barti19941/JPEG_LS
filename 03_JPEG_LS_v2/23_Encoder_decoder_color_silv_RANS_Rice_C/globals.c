#include "globals.h"

uint8_t Compare3D(uint8_t* data_1, uint8_t* data_2, uint16_t w, uint16_t h)
{
	uint32_t i = 0;
	uint32_t j = 0;	
	uint8_t k = 0;
	uint32_t cnt_1 = 0;
	uint32_t cnt_2 = 0;
	for (i = 0; i < h; i++)
	{
		for (j = 0; j < w; j++)
		{
			for (k = 0; k < 3; k++)
			{
				if (data_1[(i * w * 3) + (j * 3) + k] == data_2[(i * w * 3) + (j * 3) + k])
				{
					++cnt_1;
				}
				else
				{
					++cnt_2;
					printf("EXCEPTION: k = %d, i = %d, j = %d, data_1 = %d, data_2 = %d\n", k, i, j, data_1[(i * w * 3) + (j * 3) + k], data_2[(i * w * 3) + (j * 3) + k]);
					system("pause");
				}
			}
		}
	}
	printf("COMPARISON\n");
	printf("PASSED: %d\n", cnt_1);
	printf("FAILED: %d\n", cnt_2);
	printf("TOTAL: %d\n", 3 * w * h);	
	if (cnt_2 > 0)
	{
		return 255;
	}
	else
	{
		return 0;
	}
}

uint8_t Compare2D(uint8_t* data_1, uint8_t* data_2, uint16_t w, uint16_t h)
{
	uint32_t i = 0;
	uint32_t j = 0;	
	uint32_t cnt_1 = 0;
	uint32_t cnt_2 = 0;
	for (i = 0; i < h; i++)
	{
		for (j = 0; j < w; j++)
		{
			if (data_1[(i * w) + j] == data_2[(i * w) + j])
			{
				++cnt_1;
			}
			else
			{
				++cnt_2;
				printf("EXCEPTION: i = %d, j = %d, data_1 = %d, data_2 = %d\n", i, j, data_1[(i * w) + j], data_2[(i * w) + j]);
				system("pause");
			}
		}
	}
	printf("COMPARISON\n");
	printf("PASSED: %d\n", cnt_1);
	printf("FAILED: %d\n", cnt_2);
	printf("TOTAL: %d\n", w * h);	
	if (cnt_2 > 0)
	{
		return 255;
	}
	else
	{
		return 0;
	}
}

uint8_t Compare1D(uint8_t* data_1, uint8_t* data_2, uint32_t size)
{
	uint32_t i = 0;
	uint32_t cnt_1 = 0;
	uint32_t cnt_2 = 0;
	for (i = 0; i < size; i++)
	{
		if (data_1[i] == data_2[i])
		{
			++cnt_1;
		}
		else
		{
			++cnt_2;
			printf("EXCEPTION: i = %d, data_1 = %d, data_2 = %d\n", i, data_1[i], data_2[i]);
			system("pause");
		}
	}
	printf("COMPARISON\n");
	printf("PASSED: %d\n", cnt_1);
	printf("FAILED: %d\n", cnt_2);
	printf("TOTAL: %d\n", size);
	if (cnt_2 > 0)
	{
		return 255;
	}
	else
	{
		return 0;
	}
}

void Write1DDatWhole(uint8_t* data, uint32_t size, const char* name)
{
	FILE *outfile = fopen(name, "wb");
	fwrite(data, sizeof(uint8_t), size, outfile);
	fclose(outfile);
}

void Write1DDat(uint8_t* data, uint16_t w, uint16_t h, uint32_t size, const char* name)
{
	uint16_t tmp = 0;
	FILE *outfile = fopen(name, "wb");
	tmp = w;
	tmp >>= 8;	
	fwrite(&tmp, sizeof(uint8_t), 1, outfile);
	fwrite(&w, sizeof(uint8_t), 1, outfile);
	tmp = h;
	tmp >>= 8;	
	fwrite(&tmp, sizeof(uint8_t), 1, outfile);
	fwrite(&h, sizeof(uint8_t), 1, outfile);
	fwrite(data, sizeof(uint8_t), size, outfile);
	fclose(outfile);
}

void Write1DJlsGray(uint8_t* data, uint16_t w, uint16_t h, uint32_t size, const char* name)
{
	char* outdata;	
	char write_data[10];
	uint32_t i = 0;	
	FILE *outfile = fopen(name, "wb");
	outdata = (char*)calloc(size+27, sizeof(char));
	outdata[0] = 0xFF;
	outdata[1] = 0xD8;	
	outdata[2] = 0xFF;
	outdata[3] = 0xF7;
	outdata[4] = 0x00;
	outdata[5] = 0x0B;
	outdata[6] = 0x08;
	outdata[7] = (uint8_t)(h >> 8);	
	outdata[8] = (uint8_t)h;
	outdata[9] = (uint8_t)(w >> 8);
	outdata[10] = (uint8_t)w;
	outdata[11] = 0x01;
	outdata[12] = 0x01;
	outdata[13] = 0x11;
	outdata[14] = 0x00;
	outdata[15] = 0xFF;
	outdata[16] = 0xDA;
	outdata[17] = 0x00;
	outdata[18] = 0x08;
	outdata[19] = 0x01;
	outdata[20] = 0x01;
	outdata[21] = 0x00;
	outdata[22] = 0x00;
	outdata[23] = 0x00;
	outdata[24] = 0x00;	
	for (i = 0; i < size; i++)
	{
		outdata[25+i] = data[i];		
	}
	outdata[25+i] = 0xFF;
	outdata[25+i+1] = 0xD9;
	fwrite(outdata, sizeof(*outdata), 27+size, outfile);
	fclose(outfile);
	free(outdata);
}

void Write1DJlsColorNoInterleaved(uint8_t* data, uint16_t w, uint16_t h, uint32_t size, const char* name)
{	
	uint8_t* outdata;	
	char write_data[10];
	uint32_t i = 0;
	FILE *outfile = fopen(name, "wb");
	outdata = (uint8_t*)calloc(size + 21 + 2, sizeof(uint8_t));
	outdata[0] = 0xFF;
	outdata[1] = 0xD8;	
	outdata[2] = 0xFF;
	outdata[3] = 0xF7;
	outdata[4] = 0x00;
	outdata[5] = 0x11;
	outdata[6] = 0x08;
	outdata[7] = (uint8_t)(h >> 8);	
	outdata[8] = (uint8_t)h;
	outdata[9] = (uint8_t)(w >> 8);
	outdata[10] = (uint8_t)w;
	outdata[11] = 0x03;
	outdata[12] = 0x01;
	outdata[13] = 0x11;
	outdata[14] = 0x00;	
	outdata[15] = 0x02;
	outdata[16] = 0x11;
	outdata[17] = 0x00;	
	outdata[18] = 0x03;
	outdata[19] = 0x11;
	outdata[20] = 0x00;
	for (i = 0; i < size; i++)
	{
		outdata[21 + i] = data[i];	
	}
	outdata[21 + i] = 0xFF;
	outdata[22 + i] = 0xD9;
	fwrite(outdata, sizeof(*outdata), size + 21 + 2, outfile);
	fclose(outfile);
	free(outdata);
}

void Write1DJlsColorLineInterleaved(uint8_t* data, uint16_t w, uint16_t h, uint32_t size, const char* name)
{
	uint8_t* outdata;	
	char write_data[10];
	uint32_t i = 0;
	FILE *outfile = fopen(name, "wb");
	outdata = (uint8_t*)calloc(size + 37, sizeof(uint8_t));
	outdata[0] = 0xFF;
	outdata[1] = 0xD8;	
	outdata[2] = 0xFF;
	outdata[3] = 0xF7;
	outdata[4] = 0x00;
	outdata[5] = 0x11;
	outdata[6] = 0x08;
	outdata[7] = (uint8_t)(h >> 8);	
	outdata[8] = (uint8_t)h;
	outdata[9] = (uint8_t)(w >> 8);
	outdata[10] = (uint8_t)w;
	outdata[11] = 0x03;
	outdata[12] = 0x01;
	outdata[13] = 0x11;
	outdata[14] = 0x00;	
	outdata[15] = 0x02;
	outdata[16] = 0x11;
	outdata[17] = 0x00;	
	outdata[18] = 0x03;
	outdata[19] = 0x11;
	outdata[20] = 0x00;
	outdata[21] = 0xFF;
	outdata[22] = 0xDA;
	outdata[23] = 0x00;
	outdata[24] = 0x0C;	
	outdata[25] = 0x03;
	outdata[26] = 0x01;
	outdata[27] = 0x00;
	outdata[28] = 0x02;
	outdata[29] = 0x00;
	outdata[30] = 0x03;
	outdata[31] = 0x00;
	outdata[32] = 0x00;
	outdata[33] = 0x01;
	outdata[34] = 0x00;
	for (i = 0; i < size; i++)
	{
		outdata[35+i] = data[i];		
	}
	outdata[35+i] = 0xFF;
	outdata[35+i+1] = 0xD9;
	fwrite(outdata, sizeof(*outdata), size + 37, outfile);
	fclose(outfile);
	free(outdata);
}

void Write1DJlsColorSampleInterleaved(uint8_t* data, uint16_t w, uint16_t h, uint32_t size, const char* name)
{
	uint8_t* outdata;	
	char write_data[10];
	uint32_t i = 0;
	FILE *outfile = fopen(name, "wb");
	outdata = (uint8_t*)calloc(size + 37, sizeof(uint8_t));
	outdata[0] = 0xFF;
	outdata[1] = 0xD8;	
	outdata[2] = 0xFF;
	outdata[3] = 0xF7;
	outdata[4] = 0x00;
	outdata[5] = 0x11;
	outdata[6] = 0x08;
	outdata[7] = (uint8_t)(h >> 8);	
	outdata[8] = (uint8_t)h;
	outdata[9] = (uint8_t)(w >> 8);
	outdata[10] = (uint8_t)w;
	outdata[11] = 0x03;
	outdata[12] = 0x01;
	outdata[13] = 0x11;
	outdata[14] = 0x00;	
	outdata[15] = 0x02;
	outdata[16] = 0x11;
	outdata[17] = 0x00;	
	outdata[18] = 0x03;
	outdata[19] = 0x11;
	outdata[20] = 0x00;
	outdata[21] = 0xFF;
	outdata[22] = 0xDA;
	outdata[23] = 0x00;
	outdata[24] = 0x0C;	
	outdata[25] = 0x03;
	outdata[26] = 0x01;
	outdata[27] = 0x00;
	outdata[28] = 0x02;
	outdata[29] = 0x00;
	outdata[30] = 0x03;
	outdata[31] = 0x00;
	outdata[32] = 0x00;
	outdata[33] = 0x02;
	outdata[34] = 0x00;
	for (i = 0; i < size; i++)
	{
		outdata[35+i] = data[i];		
	}
	outdata[35+i] = 0xFF;
	outdata[35+i+1] = 0xD9;
	fwrite(outdata, sizeof(*outdata), size + 37, outfile);
	fclose(outfile);
	free(outdata);
}

void Write2DDat(uint8_t* data, uint16_t w, uint16_t h, const char* name)
{
	uint16_t tmp = 0;
	FILE *outfile = fopen(name, "wb");
	tmp = w;
	tmp >>= 8;	
	fwrite(&tmp, sizeof(uint8_t), 1, outfile);
	fwrite(&w, sizeof(uint8_t), 1, outfile);
	tmp = h;
	tmp >>= 8;	
	fwrite(&tmp, sizeof(uint8_t), 1, outfile);
	fwrite(&h, sizeof(uint8_t), 1, outfile);
	fwrite(data, sizeof(uint8_t), w * h, outfile);
	fclose(outfile);	
}

void Write2DPgm(uint8_t* data, uint16_t w, uint16_t h, const char* name)
{
	int32_t i = 0;
	int32_t j = 0;
	char write_data[10];	
	FILE *fp = fopen(name, "wb");
	write_data[0] = 'P';
	write_data[1] = '5';
	write_data[2] = '\n';
	fwrite(write_data, sizeof(char), 3, fp);
	itoa(w, write_data, 10);
	if (w >= 1000)
	{
		write_data[4] = ' ';
		fwrite(write_data, sizeof(char), 5, fp);
	}
	else if (w >= 100)
	{
		write_data[3] = ' ';
		fwrite(write_data, sizeof(char), 4, fp);
	}
	else if (w >= 10)
	{
		write_data[2] = ' ';
		fwrite(write_data, sizeof(char), 3, fp);
	}
	else
	{
		write_data[1] = ' ';
		fwrite(write_data, sizeof(char), 2, fp);
	}
	itoa(h, write_data, 10);
	if (h >= 1000)
	{
		write_data[4] = '\n';
		fwrite(write_data, sizeof(char), 5, fp);
	}
	else if (h >= 100)
	{
		write_data[3] = '\n';
		fwrite(write_data, sizeof(char), 4, fp);
	}
	else if (h >= 10)
	{
		write_data[2] = '\n';
		fwrite(write_data, sizeof(char), 3, fp);
	}
	else
	{
		write_data[1] = '\n';
		fwrite(write_data, sizeof(char), 2, fp);
	}
	write_data[0] = '2';
	write_data[1] = '5';
	write_data[2] = '5';
	write_data[3] = '\n';
	fwrite(write_data, sizeof(char), 4, fp);
	for (i = 0; i < h; i++)
	{
		for (j = 0; j < w; j++)
		{
			fwrite(&(data[(i * w) + j]), sizeof(char), 1, fp);
		}
	}
	fclose(fp);
}

void Write3DDat(uint8_t* data, uint16_t w, uint16_t h, const char* name)
{
	uint16_t tmp = 0;
	FILE *outfile = fopen(name, "wb");
	tmp = w;
	tmp >>= 8;	
	fwrite(&tmp, sizeof(uint8_t), 1, outfile);
	fwrite(&w, sizeof(uint8_t), 1, outfile);
	tmp = h;
	tmp >>= 8;	
	fwrite(&tmp, sizeof(uint8_t), 1, outfile);
	fwrite(&h, sizeof(uint8_t), 1, outfile);
	fwrite(data, sizeof(uint8_t), 3 * w * h, outfile);
	fclose(outfile);	
}

void Write3DPpm(uint8_t* data, uint16_t w, uint16_t h, const char* name)
{
	char write_data[10];		
	FILE *fp = fopen(name, "wb");
	write_data[0] = 'P';
	write_data[1] = '6';
	write_data[2] = '\n';
	fwrite(write_data, sizeof(char), 3, fp);
	itoa(w, write_data, 10);
	if (w >= 1000)
	{
		write_data[4] = ' ';
		fwrite(write_data, sizeof(char), 5, fp);
	}
	else if (w >= 100)
	{
		write_data[3] = ' ';
		fwrite(write_data, sizeof(char), 4, fp);
	}
	else if (w >= 10)
	{
		write_data[2] = ' ';
		fwrite(write_data, sizeof(char), 3, fp);
	}
	else
	{
		write_data[1] = ' ';
		fwrite(write_data, sizeof(char), 2, fp);
	}
	itoa(h, write_data, 10);
	if (h >= 1000)
	{
		write_data[4] = '\n';
		fwrite(write_data, sizeof(char), 5, fp);
	}
	else if (h >= 100)
	{
		write_data[3] = '\n';
		fwrite(write_data, sizeof(char), 4, fp);
	}
	else if (h >= 10)
	{
		write_data[2] = '\n';
		fwrite(write_data, sizeof(char), 3, fp);
	}
	else
	{
		write_data[1] = '\n';
		fwrite(write_data, sizeof(char), 2, fp);
	}
	write_data[0] = '2';
	write_data[1] = '5';
	write_data[2] = '5';
	write_data[3] = '\n';
	fwrite(write_data, sizeof(char), 4, fp);
	fwrite(data, sizeof(uint8_t), 3 * w * h, fp);
	fclose(fp);
}

uint8_t* Read1DDatWhole(uint32_t* size, const char* name)
{
	uint8_t* data;
	FILE *outfile = fopen(name, "rb");
	fseek (outfile, 0, SEEK_END);   
	(*size) = 0;
    (*size) = ftell(outfile);
    data = (uint8_t*)calloc((*size), sizeof(uint8_t));
    fseek (outfile, 0, SEEK_SET);   
    fread(data, sizeof(uint8_t), (*size), outfile);
    fclose(outfile);
    return data;
}

uint8_t* Read1DDat(uint16_t* w, uint16_t* h, uint32_t* size, const char* name)
{
	uint8_t* data;
	FILE *outfile = fopen(name, "rb");
	fread(w, sizeof(uint8_t), 1, outfile);
	(*w) <<= 8;
	fread(w, sizeof(uint8_t), 1, outfile);
	fread(h, sizeof(uint8_t), 1, outfile);
	(*h) <<= 8;
	fread(h, sizeof(uint8_t), 1, outfile);
	fseek(outfile, 0, SEEK_END);
	*size = ftell(outfile) - 4;
	fseek(outfile, 4, SEEK_SET);
	data = (uint8_t*)calloc((*size), sizeof(uint8_t));
	fread(data, sizeof(uint8_t), *size, outfile);
	fclose(outfile);
	return data;
}

uint8_t* Read1DJlsGray(uint16_t* w, uint16_t* h, uint32_t* size, const char* name)
{
	char read_data[10];
	char tmp;
	uint8_t *dout;
	FILE* fp = fopen(name, "rb");
	fseek(fp, 7, 0);
	fread(read_data, sizeof(char), 2, fp);
	*h = (unsigned char)read_data[0] << 8 | (unsigned char)read_data[1];
	fread(read_data, sizeof(char), 2, fp);
	*w = (unsigned char)read_data[0] << 8 | (unsigned char)read_data[1];
	fseek(fp, 25, 0);
	*size = 0;
	for (;;)
	{
		fread(read_data, sizeof(char), 1, fp);
		(*size)++;
		if (((unsigned char)*read_data == 0xd9) && ((unsigned char)tmp == 0xff))
		{
			(*size) -= 2;
			break;
		}
		else
		{
			tmp = *read_data;
		}	
	}
	fseek(fp, 25, 0);
	dout = (uint8_t*)calloc(*size, sizeof(uint8_t));
	fread(dout, (*size)*sizeof(uint8_t), 1, fp);
	fclose(fp);
	return dout;
}

uint8_t* Read1DJlsColorNoInterleaved(uint16_t* w, uint16_t* h, uint32_t* size, const char* name)
{
	char read_data[10];
	uint8_t* dout;
	char tmp = 0x00;
	FILE* fp = fopen(name, "rb");
	fseek(fp, 7, 0);
	fread(read_data, sizeof(char), 2, fp);
	*h = (unsigned char)read_data[0] << 8 | (unsigned char)read_data[1];
	fread(read_data, sizeof(char), 2, fp);
	*w = (unsigned char)read_data[0] << 8 | (unsigned char)read_data[1];
	fseek(fp, 21, 0);
	*size = 0;
	for(;;)
	{
		fread(read_data, sizeof(char), 1, fp);
		(*size)++;
		if (((unsigned char)*read_data == 0xd9) && ((unsigned char)tmp == 0xff))
		{
			(*size) -= 2;
			break;
		}
		else
		{
			tmp = *read_data;
		}	
	}
	fseek(fp, 21, 0);
	dout = (uint8_t*)calloc(*size, sizeof(uint8_t));
	fread(dout, (*size)*sizeof(uint8_t), 1, fp);
	fclose(fp);
	return dout;	
}

uint8_t* Read1DJlsColorLineInterleaved(uint16_t* w, uint16_t* h, uint32_t* size, const char* name)
{
	char read_data[10];
	uint8_t* dout;
	char tmp = 0x00;
	FILE* fp = fopen(name, "rb");
	fseek(fp, 7, 0);
	fread(read_data, sizeof(char), 2, fp);
	*h = (unsigned char)read_data[0] << 8 | (unsigned char)read_data[1];
	fread(read_data, sizeof(char), 2, fp);
	*w = (unsigned char)read_data[0] << 8 | (unsigned char)read_data[1];
	fseek(fp, 35, 0);
	*size = 0;
	for(;;)
	{
		fread(read_data, sizeof(char), 1, fp);
		(*size)++;
		if (((unsigned char)*read_data == 0xd9) && ((unsigned char)tmp == 0xff))
		{
			(*size) -= 2;
			break;
		}
		else
		{
			tmp = *read_data;
		}	
	}
	fseek(fp, 35, 0);
	dout = (uint8_t*)calloc(*size, sizeof(uint8_t));
	fread(dout, (*size)*sizeof(uint8_t), 1, fp);
	fclose(fp);
	return dout;
}

uint8_t* Read1DJlsColorSampleInterleaved(uint16_t* w, uint16_t* h, uint32_t* size, const char* name)
{
	char read_data[10];
	uint8_t* dout;
	char tmp = 0x00;
	FILE* fp = fopen(name, "rb");
	fseek(fp, 7, 0);
	fread(read_data, sizeof(char), 2, fp);
	*h = (unsigned char)read_data[0] << 8 | (unsigned char)read_data[1];
	fread(read_data, sizeof(char), 2, fp);
	*w = (unsigned char)read_data[0] << 8 | (unsigned char)read_data[1];
	fseek(fp, 35, 0);
	*size = 0;
	for(;;)
	{
		fread(read_data, sizeof(char), 1, fp);
		(*size)++;
		if (((unsigned char)*read_data == 0xd9) && ((unsigned char)tmp == 0xff))
		{
			(*size) -= 2;
			break;
		}
		else
		{
			tmp = *read_data;
		}	
	}
	fseek(fp, 35, 0);
	dout = (uint8_t*)calloc(*size, sizeof(uint8_t));
	fread(dout, (*size)*sizeof(uint8_t), 1, fp);
	fclose(fp);
	return dout;
}

uint8_t* Read2DDat(uint16_t* w, uint16_t* h, const char* name)
{
	uint8_t* data;
	uint32_t size = 0;
	FILE *outfile = fopen(name, "rb");
	fread(w, sizeof(uint8_t), 1, outfile);
	(*w) <<= 8;
	fread(w, sizeof(uint8_t), 1, outfile);
	fread(h, sizeof(uint8_t), 1, outfile);
	(*h) <<= 8;
	fread(h, sizeof(uint8_t), 1, outfile);
	data = (uint8_t*)calloc((*w) * (*h), sizeof(uint8_t));
	fread(data, sizeof(uint8_t), (*w) * (*h), outfile);
	fclose(outfile);
	return data;
}

uint8_t* Read2DPgm(uint16_t* w, uint16_t* h, const char* name)
{
	FILE* fp = fopen(name,"rb");
	int32_t i = 0;
	int32_t j = 0;
	char read_data[10];
	uint8_t* dout;	
	fread(&(read_data[0]), sizeof(char), 1, fp);
	fread(&(read_data[1]), sizeof(char), 1, fp);
	fread(&(read_data[2]), sizeof(char), 1, fp);
	for (i = 0; i > -1; i++)
	{
		fread(&(read_data[i]), sizeof(char), 1, fp);
		if (read_data[i] == 0x20)
		{
			read_data[i] = '\0';
			break;
		}
	}	
	*w = atoi((const char*)read_data);
	for (i = 0; i > -1; i++)
	{
		fread(&(read_data[i]), sizeof(char), 1, fp);
		if (read_data[i] == 0x0a)
		{
			read_data[i] = '\0';
			break;
		}	
	}	
	*h = atoi((const char*)read_data);
	for (i = 0; i > -1; i++)
	{
		fread(&(read_data[i]), sizeof(char), 1, fp);
		if (read_data[i] == 0x0a)
		{
			read_data[i] = '\0';
			break;
		}	
	}		
	dout = (uint8_t*)calloc((*w)*(*h), sizeof(uint8_t));
	fread(dout, sizeof(char), (*w)*(*h), fp);
	fclose(fp);
	return dout;
}

uint8_t* Read3DDat(uint16_t* w, uint16_t* h, const char* name)
{
	uint8_t* data;
	uint32_t size = 0;
	FILE *outfile = fopen(name, "rb");
	fread(w, sizeof(uint8_t), 1, outfile);
	(*w) <<= 8;
	fread(w, sizeof(uint8_t), 1, outfile);
	fread(h, sizeof(uint8_t), 1, outfile);
	(*h) <<= 8;
	fread(h, sizeof(uint8_t), 1, outfile);
	data = (uint8_t*)calloc(3 * (*w) * (*h), sizeof(uint8_t));
	fread(data, sizeof(uint8_t), 3 * (*w) * (*h), outfile);
	fclose(outfile);
	return data;
}

uint8_t* Read3DPpm(uint16_t* w, uint16_t* h, const char* name)
{
	FILE* fp = fopen(name,"rb");
	int32_t i = 0;
	char read_data[10];
	uint8_t* dout;	
	fread(&(read_data[0]), sizeof(char), 1, fp);
	fread(&(read_data[1]), sizeof(char), 1, fp);
	fread(&(read_data[2]), sizeof(char), 1, fp);
	for (i = 0; i > -1; i++)
	{
		fread(&(read_data[i]), sizeof(char), 1, fp);
		if (read_data[i] == 0x20)
		{
			read_data[i] = '\0';
			break;
		}
	}	
	*w = atoi((const char*)read_data);
	for (i = 0; i > -1; i++)
	{
		fread(&(read_data[i]), sizeof(char), 1, fp);
		if (read_data[i] == 0x0a)
		{
			read_data[i] = '\0';
			break;
		}	
	}	
	*h = atoi((const char*)read_data);
	for (i = 0; i > -1; i++)
	{
		fread(&(read_data[i]), sizeof(char), 1, fp);
		if (read_data[i] == 0x0a)
		{
			read_data[i] = '\0';
			break;
		}	
	}		
	dout = (uint8_t*)calloc((*w) * (*h) * 3, sizeof(uint8_t));
	fread(dout, sizeof(uint8_t), (*w) * (*h) * 3, fp);
	fclose(fp);
	return dout;
}

