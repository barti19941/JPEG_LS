#include "read_write.h"

/**********************************************************************
	ReadPpmBinary:	This function reads a color image into 3d matrix.
						w - width of read image
						h - height of read image
						return pointer to adress of allocated table
						return 0 if allocation failed
**********************************************************************/
char*** ReadPpmBinary(int* w, int* h)
{
	FILE* fp;
	int i, j;
	char read_data[BUFFER_LENGHT];
	char*** dout;	
	char** dr;
	char** dg;
	char** db;
	char* bitstream;
	printf("INFO\tEnter *.ppm filename to read: ");
	scanf("%s", read_data);
	fp = fopen(read_data,"rb");
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
	bitstream = Allocate1D((*w)*(*h)*COMPONENTS_COUNT);
	fread(bitstream, sizeof(char), (*w)*(*h)*COMPONENTS_COUNT, fp);
	dr = Allocate2D(*w, *h);
	dg = Allocate2D(*w, *h);
	db = Allocate2D(*w, *h);
	for (i = 0; i < (*h); i++)
	{
		for (j = 0; j < (*w); j++)
		{
			dr[i][j] = bitstream[((*w) * i * 3) + (3 * j)];
			dg[i][j] = bitstream[((*w) * i * 3) + ((3 * j) + 1)];
			db[i][j] = bitstream[((*w) * i * 3) + ((3 * j) + 2)];
		}
	}
	dout = Allocate3D(*w, *h);
	dout[0] = dr;
	dout[1] = dg;
	dout[2] = db;
	Free1D(bitstream);
	fclose(fp);
	return dout;
}

/**********************************************************************
	ReadPgmBinary: This function reads a binary gray image 
						into 2d matrix.
						w - width of read image
						h - height of read image
						return pointer to adress of allocated table
						return 0 if allocation failed
**********************************************************************/
char** ReadPgmBinary(int* w, int* h)
{
	FILE* fp;
	int i, j;
	char read_data[BUFFER_LENGHT];
	char** dout;	
	char* bitstream;
	printf("INFO\tEnter *.pgm filename to read: ");
	scanf("%s", read_data);
	fp = fopen(read_data,"rb");
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
	bitstream = Allocate1D((*w)*(*h));
	fread(bitstream, sizeof(char), (*w)*(*h), fp);
	dout = Allocate2D((*w), (*h));
	for (i = 0; i < (*h); i++)
	{
		for (j = 0; j < (*w); j++)
		{
			dout[i][j] = bitstream[(i*(*w))+j];
		}
	}
	Free1D(bitstream);
	fclose(fp);
	return dout;
}



/**********************************************************************
	WritePgmBinary:	This function save binary data to *.pgm file.
						data - pointer to data to save
						w - width of image
						h - height of image
**********************************************************************/
void WritePgmBinary(char** data, int w, int h)
{
	int i, j;
	char write_data[BUFFER_LENGHT];
	printf("INFO\tEnter *.pgm filename to write: ");
	scanf("%s", write_data);		
	FILE *fp = fopen(write_data, "wb");
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
			fwrite(&(data[i][j]), sizeof(char), 1, fp);
		}
	}
	fclose(fp);
}


/**********************************************************************
	WritePpmBinary:	This function save data to *.ppm file.
						data - pointer to data to save
						w - width of image
						h - height of image
**********************************************************************/
void WritePpmBinary(char*** data, int w, int h)
{
	int i, j, k;
	char write_data[BUFFER_LENGHT];
	printf("INFO\tEnter *.ppm filename to write: ");
	scanf("%s", write_data);		
	FILE *fp = fopen(write_data, "wb");
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
	for (i = 0; i < h; i++)
	{
		for (j = 0; j < w; j++)
		{
			for (k = 0; k < COMPONENTS_COUNT; k++)
			{				
				fwrite(&(data[k][i][j]), sizeof(char), 1, fp);
			}
		}
	}
	fclose(fp);
}

/**********************************************************************
	WriteJlsGrayPic	:	This function save gray encoded data to 
						*.jls file.
						data - pointer to data to save
						w - width of image
						h - height of image
						size - lenght of data bitstream
**********************************************************************/
void WriteJlsGray(char* data, int w, int h, int size)
{
	char* outdata;	
	char write_data[BUFFER_LENGHT];
	int i;
	printf("INFO\tEnter *.jls filename to write: ");
	scanf("%s", write_data);	
	FILE *outfile = fopen(write_data, "wb");
	outdata = Allocate1D(size+27);
	outdata[0] = 0xFF;
	outdata[1] = 0xD8;	
	outdata[2] = 0xFF;
	outdata[3] = 0xF7;
	outdata[4] = 0x00;
	outdata[5] = 0x0B;
	outdata[6] = 0x08;
	outdata[7] = (char)(h >> 8);	
	outdata[8] = (char)h;
	outdata[9] = (char)(w >> 8);
	outdata[10] = (char)w;
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
	Free1D(outdata);
}

/**********************************************************************
	WriteJlsColorSampleIlv	:	This function save color encoded data 
						with sample interlaved to *.jls file.
						data - pointer to data to save
						w - width of image
						h - height of image
						size - pointer to data with size for each component
**********************************************************************/ 
void WriteJlsColorSampleIlv(char* data, int w, int h, int size)
{
	char* outdata;	
	char write_data[BUFFER_LENGHT];
	int i;
	printf("INFO\tEnter *.jls filename to write: ");
	scanf("%s", write_data);	
	FILE *outfile = fopen(write_data, "wb");
	outdata = Allocate1D(size+37);
	/* write all markers */
	outdata[0] = 0xFF;
	outdata[1] = 0xD8;	
	outdata[2] = 0xFF;
	outdata[3] = 0xF7;
	outdata[4] = 0x00;
	outdata[5] = 0x11;
	outdata[6] = 0x08;
	outdata[7] = (char)(h >> 8);	
	outdata[8] = (char)h;
	outdata[9] = (char)(w >> 8);
	outdata[10] = (char)w;
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
	fwrite(outdata, sizeof(*outdata), 37+size, outfile);
	fclose(outfile);
	Free1D(outdata);
}

/**********************************************************************
	WriteJlsColorNoIlv	:	This function save color encoded data 
						with no line interlaved to *.jls file.
						data - pointer to data to save
						w - width of image
						h - height of image
						size - pointer to data with size for each component
**********************************************************************/
void WriteJlsColorNoIlv(char** data, int w, int h, int* size)
{
	char* outdata;	
	char write_data[BUFFER_LENGHT];
	int i, j, k;
	printf("INFO\tEnter *.jls filename to write: ");
	scanf("%s", write_data);	
	FILE *outfile = fopen(write_data, "wb");
	outdata = Allocate1D(size[0]+size[1]+size[2]+53);
	outdata[0] = 0xFF;
	outdata[1] = 0xD8;	
	outdata[2] = 0xFF;
	outdata[3] = 0xF7;
	outdata[4] = 0x00;
	outdata[5] = 0x11;
	outdata[6] = 0x08;
	outdata[7] = (char)(h >> 8);	
	outdata[8] = (char)h;
	outdata[9] = (char)(w >> 8);
	outdata[10] = (char)w;
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
	outdata[24] = 0x08;	
	outdata[25] = 0x01;
	outdata[26] = 0x01;
	outdata[27] = 0x00;
	outdata[28] = 0x00;
	outdata[29] = 0x00;
	outdata[30] = 0x00;
	for (i = 0; i < size[0]; i++)
	{
		outdata[31+i] = data[0][i];		
	}
	outdata[31+i] = 0xFF;
	outdata[32+i] = 0xDA;
	outdata[33+i] = 0x00;
	outdata[34+i] = 0x08;	
	outdata[35+i] = 0x01;
	outdata[36+i] = 0x01;
	outdata[37+i] = 0x00;
	outdata[38+i] = 0x00;
	outdata[39+i] = 0x00;
	outdata[40+i] = 0x00;
	for (j = 0; j < size[1]; j++)
	{
		outdata[41+i+j] = data[1][j];		
	}
	outdata[41+i+j] = 0xFF;
	outdata[42+i+j] = 0xDA;
	outdata[43+i+j] = 0x00;
	outdata[44+i+j] = 0x08;	
	outdata[45+i+j] = 0x01;
	outdata[46+i+j] = 0x01;
	outdata[47+i+j] = 0x00;
	outdata[48+i+j] = 0x00;
	outdata[49+i+j] = 0x00;
	outdata[50+i+j] = 0x00;
	for (k = 0; k < size[2]; k++)
	{
		outdata[51+i+j+k] = data[2][k];		
	}
	outdata[51+i+j+k] = 0xFF;
	outdata[52+i+j+k] = 0xD9;
	fwrite(outdata, sizeof(*outdata), size[0]+size[1]+size[2]+53, outfile);
	fclose(outfile);
	Free1D(outdata);
}
/**********************************************************************
	WriteJlsColorLineIlv	:	This function save color encoded data 
						with line interlaved to *.jls file.
						data - pointer to data to save
						w - width of image
						h - height of image
						size - lenght of data bitstream
**********************************************************************/
void WriteJlsColorLineIlv(char* data, int w, int h, int size)
{
	char* outdata;	
	char write_data[BUFFER_LENGHT];
	int i;
	printf("INFO\tEnter *.jls filename to write: ");
	scanf("%s", write_data);	
	FILE *outfile = fopen(write_data, "wb");	
	outdata = Allocate1D(size+37);
	outdata[0] = 0xFF;
	outdata[1] = 0xD8;	
	outdata[2] = 0xFF;
	outdata[3] = 0xF7;
	outdata[4] = 0x00;
	outdata[5] = 0x11;
	outdata[6] = 0x08;
	outdata[7] = (char)(h >> 8);	
	outdata[8] = (char)h;
	outdata[9] = (char)(w >> 8);
	outdata[10] = (char)w;
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
	fwrite(outdata, sizeof(*outdata), 37+size, outfile);
	fclose(outfile);
	Free1D(outdata);
}

/**********************************************************************
	ReadJlsPicColorNoIlv	:	This function read raw_data from *.jls
						color no ilv file.
						w - width of image
						h - height of image
						size - lenght of data bitstream						
						return pointer to adress of allocated raw data
						return 0 if allocation failed
**********************************************************************/
char** ReadJlsColorNoIlv(int* w, int* h, int* size)
{
	int i;
	char read_data[BUFFER_LENGHT];
	char** dout;
	char tmp_1 = 0x00;
	char tmp_2 = 0x00;
	size[0] = 0;
	size[1] = 0;
	size[2] = 0;
	dout = (char**)Allocate1D(COMPONENTS_COUNT);
	FILE* fp;
	printf("INFO\tEnter *.jls filename to read: ");
	scanf("%s", read_data);	
	fp = fopen(read_data, "rb");
	fseek(fp, 7, 0);
	fread(read_data, sizeof(char), 2, fp);
	*h = (unsigned char)read_data[0] << 8 | (unsigned char)read_data[1];
	fread(read_data, sizeof(char), 2, fp);
	*w = (unsigned char)read_data[0] << 8 | (unsigned char)read_data[1];
	fseek(fp, 31, 0);
	for(;;)
	{
		fread(&tmp_1, sizeof(char), 1, fp);
		size[0]++;
		if (((unsigned char)tmp_1 == 0xda) && ((unsigned char)tmp_2 == 0xff))
		{
			size[0] -= 2;
			break;
		}
		else
		{
			tmp_2 = tmp_1;
		}	
	}
	fseek(fp, 31, 0);
	dout[0] = Allocate1D(size[0]);
	for (i = 0; i < size[0]; i++)
	{
		fread(&(dout[0][i]), sizeof(char), 1, fp);
	}
	fseek(fp, 41+size[0], 0);
	tmp_1 = 0x00;
	tmp_2 = 0x00;
	for(;;)
	{
		fread(&tmp_1, sizeof(char), 1, fp);
		size[1]++;
		if (((unsigned char)tmp_1 == 0xda) && ((unsigned char)tmp_2 == 0xff))
		{
			size[1] -= 2;
			break;
		}
		else
		{
			tmp_2 = tmp_1;
		}	
	}
	fseek(fp, 41+size[0], 0);
	dout[1] = Allocate1D(size[1]);
	for (i = 0; i < size[1]; i++)
	{
		fread(&(dout[1][i]), sizeof(char), 1, fp);
	}
	fseek(fp, 51+size[0]+size[1], 0);
	tmp_1 = 0x00;
	tmp_2 = 0x00;
	for(;;)
	{
		fread(&tmp_1, sizeof(char), 1, fp);
		size[2]++;
		if (((unsigned char)tmp_1 == 0xd9) && ((unsigned char)tmp_2 == 0xff))
		{
			size[2] -= 2;
			break;
		}
		else
		{
			tmp_2 = tmp_1;
		}	
	}
	fseek(fp, 51+size[0]+size[1], 0);
	dout[2] = Allocate1D(size[2]);
	for (i = 0; i < size[2]; i++)
	{
		fread(&(dout[2][i]), sizeof(char), 1, fp);
	}
	fclose(fp);
	return dout;
}

/**********************************************************************
	ReadJlsColorLineIlv	:	This function read raw_data from *.jls
						color line ilv file.
						w - width of image
						h - height of image
						size - lenght of data bitstream						
						return pointer to adress of allocated raw data
						return 0 if allocation failed
**********************************************************************/
char* ReadJlsColorLineIlv(int* w, int* h, int* size)
{
	char read_data[BUFFER_LENGHT];
	char tmp;
	char *dout;
	int i;	
	FILE* fp;
	printf("INFO\tEnter *.jls filename to read: ");
	scanf("%s", read_data);		
	fp = fopen(read_data, "rb");
	fseek(fp, 7, 0);
	fread(read_data, sizeof(char), 2, fp);
	*h = (unsigned char)read_data[0] << 8 | (unsigned char)read_data[1];
	fread(read_data, sizeof(char), 2, fp);
	*w = (unsigned char)read_data[0] << 8 | (unsigned char)read_data[1];
	fseek(fp, 35, 0);
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
	fseek(fp, 35, 0);
	dout = Allocate1D(*size);
	for (i = 0; i < *size; i++)
	{
		fread(&(dout[i]), sizeof(char), 1, fp);
	}
	fclose(fp);
	return dout;	
}

/**********************************************************************
	ReadJlsPicColorSampleIlv:	This function read raw_data from *.jls
						color sample ilv file.
						w - width of image
						h - height of image
						size - lenght of data bitstream						
						return pointer to adress of allocated raw data
						return 0 if allocation failed
**********************************************************************/
char* ReadJlsColorSampleIlv(int* w, int* h, int* size)
{
	char read_data[BUFFER_LENGHT];
	char tmp;
	char *dout;
	int i;
	FILE* fp;
	printf("INFO\tEnter *.jls filename to read: ");
	scanf("%s", read_data);	
	fp = fopen(read_data, "rb");
	fseek(fp, 7, 0);
	fread(read_data, sizeof(char), 2, fp);
	*h = (unsigned char)read_data[0] << 8 | (unsigned char)read_data[1];
	fread(read_data, sizeof(char), 2, fp);
	*w = (unsigned char)read_data[0] << 8 | (unsigned char)read_data[1];
	fseek(fp, 35, 0);
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
	fseek(fp, 35, 0);
	dout = Allocate1D(*size);
	for (i = 0; i < *size; i++)
	{
		fread(&(dout[i]), sizeof(char), 1, fp);
	}
	fclose(fp);
	return dout;	
}

/**********************************************************************
	ReadJlsGray		:	This function read raw_data from *.jls gray
						file.
						w - width of image
						h - height of image
						size - lenght of data bitstream						
						return pointer to adress of allocated raw data
						return 0 if allocation failed
**********************************************************************/
char* ReadJlsGray(int* w, int* h, int* size)
{
	char read_data[BUFFER_LENGHT];
	char tmp;
	char *dout;
	int i;
	FILE* fp;
	printf("INFO\tEnter *.jls filename to read: ");
	scanf("%s", read_data);	
	fp = fopen(read_data, "rb");
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
	dout = Allocate1D(*size);
	fread(dout, (*size)*sizeof(char), 1, fp);
	fclose(fp);
	return dout;
}

/**********************************************************************
	WriteDat1D	:	This function store 1D data into *.dat file.
						data - pointer to data 
						size - size of data
**********************************************************************/
void WriteDat1D(char* data, int size)
{
	char write_data[BUFFER_LENGHT];
	printf("INFO\tEnter *.dat filename to write: ");
	scanf("%s", write_data);	
	FILE *outfile = fopen(write_data, "wb");
	fwrite(data, sizeof(*data), size, outfile);
	fclose(outfile);
}

/**********************************************************************
	WriteDat2D	:	This function store 2D data into *.dat file.
						data - pointer to data 
						w - width of image
						h - height of image
**********************************************************************/
void WriteDat2D(char** data, int w, int h)
{
	int i, j;
	char write_data[BUFFER_LENGHT];
	char* out_data = Allocate1D(w * h);
	printf("INFO\tEnter *.dat filename to write: ");
	scanf("%s", write_data);	
	FILE *outfile = fopen(write_data, "wb");
	for (i = 0; i < h; i++)
	{
		for (j = 0; j < w; j++)
		{
			out_data[i * w + j] = data[i][j];
		}
	}
	fwrite(out_data, sizeof(*out_data), w * h, outfile);
	fclose(outfile);
	Free1D(out_data);
}


