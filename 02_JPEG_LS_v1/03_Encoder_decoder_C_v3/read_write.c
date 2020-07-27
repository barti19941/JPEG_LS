#include "read_write.h"

/**********************************************************************
	GetLine		:	This function reads one line from text file.
						fp - pointer to file
						data - pointer to data buffer
						return 0 if no more data to read in file
						return 1 if data read succesfully
**********************************************************************/
static int GetLine(FILE* fp, char* data)
{
	int i = 0;
	char chr;
	for (i = 0; i > -1; i++)
	{		
		if (fread(&chr,sizeof(char),1,fp) == 0)
		{
			return 0;
		}
		else
		{			
			if (chr == '\n')
			{			
				data[i] = '\0';	
				break;
			}
			else
			{			
				data[i] = chr;
			}
		}
	}
	
	
	return 1;
}

/**********************************************************************
	GetValidLine	:	This function skip next lines which are 
						comment lines and return valid line.
						fp - pointer to file
						data - pointer to data buffer
**********************************************************************/
static void GetValidLine(FILE* fp, char* data)
{
	for (;;)
	{
		if (GetLine(fp,data) == 0)
		{
			break;
		}
		else
		{
			if (data[0] != '#')
			{
				break;
			}
		}
	}	
}

/**********************************************************************
	ReadPpmPicColor	:	This function reads a color image into 3d matrix.
						w - width of read image
						h - height of read image
						return pointer to adress of allocated table
						return 0 if allocation failed
**********************************************************************/
char*** ReadPpmPicColor(int* w, int* h)
{
	char*** dout;
	FILE* fp;
	char buffer[MAX_BUFFER_LENGHT];
	char data[MAX_BUFFER_LENGHT];
	char tmp[MIN_BUFFER_LENGHT];
	char** dr;
	char** dg;
	char** db;
	int i, j, k;
	/* enter file name */
	printf("INFO\tEnter *.ppm filename to read: ");
	scanf("%s", tmp);
	/* open file and check if opened properly */
	fp = fopen(tmp, "r");
	if (fp == NULL)
	{		
		printf("ERROR\tCan't read %s file!\n", tmp);
		return NULL;
	}	
	/* read first line with header */
	GetValidLine(fp, buffer);
	/* check header */
	if (!((buffer[0] == 'P') && (buffer[1] == '3')))
	{
		printf("ERROR\tInvalid header! Read header: %s, allowed header: P3.\n", buffer);
		return NULL;
	}		
	/* read second line with dimensions */
	GetValidLine(fp, buffer);
	/* save width */
	for (i = 0; i > -1; i++)
	{
		if(buffer[i] == ' ')
		{
			tmp[i] = '\0';
			break;
		}
		else
		{
			tmp[i] = buffer[i];
		}		
	}	
	
	*w = atoi((const char*)tmp);
	/* save height */
	for (j = i + 1; j > -1; j++)
	{
		if(buffer[j] == '\0')
		{
			tmp[i - j + 1] = '\0';
			break;
		}
		else
		{
			tmp[i - j + 1] = buffer[j];
		}
	}
	*h = atoi((const char*)tmp);
	if (*h == 0 || *w == 0)
	{
		printf("ERROR\tIvalid dimensions of picture, width = %d, height = %d.\n", *w, *h);
		return NULL;
	}	
	/* read max value */
	GetValidLine(fp, buffer);
	/* check precision */
	for (i = 0; i > -1; i++)
	{
		if(buffer[i] == '\0')
		{
			tmp[i] = '\0';
			break;
		}
		else
		{
			tmp[i] = buffer[i];
		}
	}	
	if (atoi(tmp) > 255)
	{
		printf("ERROR\tInvalid precision, read: %d, only allowed precision is less or equal to 255.\n", atoi(tmp));
		return NULL;
	}		
	GetValidLine(fp, buffer);
	/* read data from *.pgm file */
	i = 0;
	j = 0;
	k = 0;
	while (1)
	{
		for (j = 0; j > -1; j++)
		{			
			if(buffer[j] == ' ')
			{
				tmp[k] = '\0';
				k = 0;
				data[i]= (char)atoi((const char*)tmp);	
				i++;
				if (i == (3 * (*h) * (*w)))
				{	
					break;
				}
			}
			else if (buffer[j] == '\0')
			{		
				break;
			}
			else
			{
				tmp[k] = buffer[j];
				k++;
			}
		}
		if (i == (3 * (*h) * (*w)))
		{	
			break;
		}
		else
		{
			GetValidLine(fp, buffer);
		}
	}
	/* allocate data */
	dr = Allocate2D(*w, *h);
	dg = Allocate2D(*w, *h);
	db = Allocate2D(*w, *h);
	if (dr == NULL || dg == NULL || db == NULL) 
	{
		printf("ERROR\tCan't allocate data!\n");
		return NULL;
	}	
	/* red color */
	for (i = 0; i < (*h); i++)
	{
		for (j = 0; j < (*w); j++)
		{
			dr[i][j] = data[((*w) * i * 3) + (3 * j)];
		}
	}
	/* green color */
	for (i = 0; i < (*h); i++)
	{
		for (j = 0; j < (*w); j++)
		{
			dg[i][j] = data[((*w) * i * 3) + ((3 * j) + 1)];
		}
	}
	/* blue color */
	for (i = 0; i < (*h); i++)
	{
		for (j = 0; j < (*w); j++)
		{
			db[i][j] = data[((*w) * i * 3) + ((3 * j) + 2)];
		}
	}
	/* alocate data and assign pointers to 2D tables to 3D table values */
	dout = Allocate3D(*w, *h);
	dout[0] = dr;
	dout[1] = dg;
	dout[2] = db;
	return dout;
}

/**********************************************************************
	ReadPgmPicGray	:	This function reads a gray image into 2d matrix.
						w - width of read image
						h - height of read image
						return pointer to adress of allocated table
						return 0 if allocation failed
**********************************************************************/
char** ReadPgmPicGray(int* w, int* h)
{
	FILE* fp;
	char data[MAX_BUFFER_LENGHT];
	char tmp[MIN_BUFFER_LENGHT];
	int i, j, k, l;
	char** dout;
	/* enter file name */
	printf("INFO\tEnter *.pgm filename to read: ");
	scanf("%s", data);		
	/* open file and check if opened properly */
	fp = fopen(data,"r");
	if (fp == NULL)
	{		
		printf("ERROR\tCan't read %s file!\n", data);
		return NULL;
	}	
	/* read first line with header */
	GetValidLine(fp,data);
	/* check header */
	if (!((data[0] == 'P') && (data[1] == '2')))
	{
		printf("ERROR\tInvalid header! Read header: %s, allowed header: P2.\n", data);
		return NULL;
	}	
	/* read second line with dimensions */
	GetValidLine(fp,data);
	/* save width */
	for (i = 0; i > -1; i++)
	{
		if(data[i] == ' ')
		{
			tmp[i] = '\0';
			break;
		}
		else
		{
			tmp[i] = data[i];
		}
	}	
	*w = atoi((const char*)tmp);
	/* save height */
	for (j = i + 1; j > -1; j++)
	{
		if(data[j] == '\0')
		{
			tmp[i - j + 1] = '\0';
			break;
		}
		else
		{
			tmp[i - j + 1] = data[j];
		}
	}
	*h = atoi((const char*)tmp);
	if (*h== 0 || *w == 0)
	{
		printf("ERROR\tIvalid dimensions of picture, width = %d, height = %d.\n", *w, *h);
		return NULL;
	}	
	/* read max value */
	GetValidLine(fp,data);
	/* check precision */
	for (i = 0; i > -1; i++)
	{
		if(data[i] == '\0')
		{
			tmp[i] = '\0';
			break;
		}
		else
		{
			tmp[i] = data[i];
		}
	}	
	if (atoi(tmp) > 255)
	{
		printf("ERROR\tInvalid precision, read: %d, only allowed precision is less or equal to 255.\n", atoi(tmp));
		return NULL;
	}		
	GetValidLine(fp,data);
	/* allocate data */
	dout = Allocate2D(*w,*h);
	if (dout == NULL)
	{
		printf("ERROR\tCan't allocate data!\n");
		return NULL;
	}	
	/* read data from *.pgm file */
	i = 0;
	j = 0;
	l = 0;
	while (1)
	{
		for (k = 0; k > -1; k++)
		{			
			if(data[k] == ' ')
			{
				if (i != *h)
				{				
					tmp[l] = '\0';
					l = 0;
					dout[i][j] = (char)atoi((const char*)tmp);	
					if (j == *w - 1)
					{
						j = 0;
						i++;
						if (i == *h)
						{	
							break;
						}
					}
					else
					{					
						j++;
					}
				}
				else
				{							
					break;
				}
			}
			else if (data[k] == '\0')
			{		
				GetValidLine(fp,data);
				break;
			}
			else
			{
				tmp[l] = data[k];
				l++;
			}
		}
		if (i == *h)
		{	
			break;
		}
	}
	fclose(fp);	
	return dout;	
}

/**********************************************************************
	WritePgmPic	:	This function save data to *.pgm file.
						data - pointer to data to save
						w - width of image
						h - height of image
**********************************************************************/
void WritePgmPic(char** data, int w, int h)
{
	char buff[MAX_BUFFER_LENGHT];
	int i, j;
	/* enter file name */
	printf("INFO\tEnter *.pgm filename to write: ");
	scanf("%s", buff);	
	FILE *outfile = fopen(buff, "wb");
	/* write header */
	buff[0] = 'P';
	buff[1] = '2';
	buff[2] = '\n';
	fwrite(buff, sizeof(char), 3, outfile);
	/* write dimensions width */
	itoa(w, buff, 10);
	if (w >= 100)
	{
		buff[3] = ' ';
		fwrite(buff, sizeof(char), 4, outfile);
	}
	else if (w >= 10)
	{
		buff[2] = ' ';
		fwrite(buff, sizeof(char), 3, outfile);
	}
	else
	{
		buff[1] = ' ';
		fwrite(buff, sizeof(char), 2, outfile);
	}
	/* write dimensions height */
	itoa(h, buff, 10);
	if (h >= 100)
	{
		buff[3] = '\n';
		fwrite(buff, sizeof(char), 4, outfile);
	}
	else if (h >= 10)
	{
		buff[2] = '\n';
		fwrite(buff, sizeof(char), 3, outfile);
	}
	else
	{
		buff[1] = '\n';
		fwrite(buff, sizeof(char), 2, outfile);
	}
	/* write precision */
	buff[0] = '2';
	buff[1] = '5';
	buff[2] = '5';
	buff[3] = '\n';
	fwrite(buff, sizeof(char), 4, outfile);
	/* write raw data */
	for (i = 0; i < h; i++)
	{
		for (j = 0; j < w; j++)
		{
			itoa((unsigned char)data[i][j], buff, 10);
			if ((unsigned char)data[i][j] >= 100)
			{
				buff[3] = ' ';
				fwrite(buff, sizeof(char), 4, outfile);
			}
			else if ((unsigned char)data[i][j] >= 10)
			{
				buff[2] = ' ';
				fwrite(buff, sizeof(char), 3, outfile);
			}
			else if ((unsigned char)data[i][j] < 10)
			{
				buff[1] = ' ';
				fwrite(buff, sizeof(char), 2, outfile);
			}
			if ( (h * i + j) % 12 == 0 && (h * i + j) != 0)
			{
				buff[0] = '\n';
				fwrite(buff, sizeof(char), 1, outfile);
			}
		}
	}
	fclose(outfile);
}

/**********************************************************************
	WriteJlsColorSIlvPic	:	This function save color encoded data 
						with sample interlaved to *.jls file.
						data - pointer to data to save
						w - width of image
						h - height of image
						size - pointer to data with size for each component
**********************************************************************/ 
void WriteJlsColorSIlvPic(char* data, int w, int h, int size)
{
	char* outdata;	
	char buff[MAX_BUFFER_LENGHT];
	int i;
	/* enter file name */
	printf("INFO\tEnter *.jls filename to write: ");
	scanf("%s", buff);	
	FILE *outfile = fopen(buff, "wb");
	outdata = Allocate1D(size+37);
	/* write all markers */
	outdata[0] = 0xFF;
	outdata[1] = 0xD8;	
	outdata[2] = 0xFF;
	outdata[3] = 0xF7;
	outdata[4] = 0x00;
	outdata[5] = 0x11;
	outdata[6] = 0x08;
	outdata[7] = (char)(w >> 8);	
	outdata[8] = (char)w;
	outdata[9] = (char)(h >> 8);
	outdata[10] = (char)h;
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
	outdata[33] = 0x02;	//LINE ILV
	outdata[34] = 0x00;
	/* write data */
	for (i = 0; i < size; i++)
	{
		outdata[35+i] = data[i];		
	}
	/* write last markers */
	outdata[35+i] = 0xFF;
	outdata[35+i+1] = 0xD9;
	fwrite(outdata, sizeof(*outdata), 37+size, outfile);
	fclose(outfile);
	Free1D(outdata);
}

/**********************************************************************
	WriteJlsColorNIlvPic	:	This function save color encoded data 
						with no line interlaved to *.jls file.
						data - pointer to data to save
						w - width of image
						h - height of image
						size - pointer to data with size for each component
**********************************************************************/
void WriteJlsColorNIlvPic(char** data, int w, int h, int* size)
{
	char* outdata;	
	char buff[MAX_BUFFER_LENGHT];
	int i, j, k;
	/* enter file name */
	printf("INFO\tEnter *.jls filename to write: ");
	scanf("%s", buff);	
	FILE *outfile = fopen(buff, "wb");
	outdata = Allocate1D(size[0]+size[1]+size[2]+53);
	/* write image segment */
	outdata[0] = 0xFF;
	outdata[1] = 0xD8;	
	outdata[2] = 0xFF;
	outdata[3] = 0xF7;
	outdata[4] = 0x00;
	outdata[5] = 0x11;
	outdata[6] = 0x08;
	outdata[7] = (char)(w >> 8);	
	outdata[8] = (char)w;
	outdata[9] = (char)(h >> 8);
	outdata[10] = (char)h;
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
	/* write scan segment for red component */
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
	/* write scan segment for green component */
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
	/* write scan segment for blue component */
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
	WriteJlsColorLineIlvPic	:	This function save color encoded data 
						with line interlaved to *.jls file.
						data - pointer to data to save
						w - width of image
						h - height of image
						size - lenght of data bitstream
**********************************************************************/
void WriteJlsColorLineIlvPic(char* data, int w, int h, int size)
{
	char* outdata;	
	char buff[MAX_BUFFER_LENGHT];
	int i;
	/* enter file name */
	printf("INFO\tEnter *.jls filename to write: ");
	scanf("%s", buff);	
	FILE *outfile = fopen(buff, "wb");
	outdata = Allocate1D(size+37);
	/* write all markers */
	outdata[0] = 0xFF;
	outdata[1] = 0xD8;	
	outdata[2] = 0xFF;
	outdata[3] = 0xF7;
	outdata[4] = 0x00;
	outdata[5] = 0x11;
	outdata[6] = 0x08;
	outdata[7] = (char)(w >> 8);	
	outdata[8] = (char)w;
	outdata[9] = (char)(h >> 8);
	outdata[10] = (char)h;
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
	outdata[33] = 0x01;	//LINE ILV
	outdata[34] = 0x00;
	/* write data */
	for (i = 0; i < size; i++)
	{
		outdata[35+i] = data[i];		
	}
	/* write last markers */
	outdata[35+i] = 0xFF;
	outdata[35+i+1] = 0xD9;
	fwrite(outdata, sizeof(*outdata), 37+size, outfile);
	fclose(outfile);
	Free1D(outdata);
}

/**********************************************************************
	WriteJlsGrayPic	:	This function save gray encoded data to 
						*.jls file.
						data - pointer to data to save
						w - width of image
						h - height of image
						size - lenght of data bitstream
**********************************************************************/
void WriteJlsGrayPic(char* data, int w, int h, int size)
{
	char* outdata;	
	char buff[MAX_BUFFER_LENGHT];
	int i;
	/* enter file name */
	printf("INFO\tEnter *.jls filename to write: ");
	scanf("%s", buff);	
	FILE *outfile = fopen(buff, "wb");
	outdata = Allocate1D(size+27);
	/* write all markers */
	outdata[0] = 0xFF;
	outdata[1] = 0xD8;	
	outdata[2] = 0xFF;
	outdata[3] = 0xF7;
	outdata[4] = 0x00;
	outdata[5] = 0x0B;
	outdata[6] = 0x08;
	outdata[7] = (char)(w >> 8);	
	outdata[8] = (char)w;
	outdata[9] = (char)(h >> 8);
	outdata[10] = (char)h;
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
	/* write data */
	for (i = 0; i < size; i++)
	{
		outdata[25+i] = data[i];		
	}
	/* write last markers */
	outdata[25+i] = 0xFF;
	outdata[25+i+1] = 0xD9;
	fwrite(outdata, sizeof(*outdata), 27+size, outfile);
	fclose(outfile);
	Free1D(outdata);
}

/**********************************************************************
	ReadJlsPicGray	:	This function read raw_data from .
						w - width of image
						h - height of image
						size - lenght of data bitstream						
						return pointer to adress of allocated raw data
						return 0 if allocation failed
**********************************************************************/
char* ReadJlsPicGray(int* w, int* h, int* size)
{
	char buff[BITSTREAM_LENGHT];
	char tmp;
	char *dout;
	int i;
	FILE* fp;
	printf("INFO\tEnter *.jls filename to read: ");
	scanf("%s", buff);	
	fp = fopen(buff, "rb");
	if (fp == NULL)
	{		
		printf("ERROR\tCan't read %s file!\n", buff);
		return NULL;
	}		
	/* read height */
	fseek(fp, 7, 0);
	fread(buff, sizeof(char), 2, fp);
	*h = (int)buff[0] << 8 | (int)buff[1];
	/* read width */
	fread(buff, sizeof(char), 2, fp);
	*w = (int)buff[0] << 8 | (int)buff[1];
	/* check how much bytes bitstream contain */
	fseek(fp, 25, 0);
	*size = 0;
	for (;;)
	{
		fread(buff, sizeof(char), 1, fp);
		(*size)++;
		if (((unsigned char)*buff == 0xd9) && ((unsigned char)tmp == 0xff))
		{
			(*size) -= 2;
			break;
		}
		else
		{
			tmp = *buff;
		}	
	}
	/* allocate buffer for bitstream */
	fseek(fp, 25, 0);
	dout = Allocate1D(*size);
	/* append data to */
	for (i = 0; i < *size; i++)
	{
		fread(&(dout[i]), sizeof(char), 1, fp);
	}
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
	char buff[MAX_BUFFER_LENGHT];
	/* enter file name */
	printf("INFO\tEnter *.dat filename to write: ");
	scanf("%s", buff);	
	FILE *outfile = fopen(buff, "wb");
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
	char buff[MAX_BUFFER_LENGHT];
	char* out_data = Allocate1D(w * h);
	/* enter file name */
	printf("INFO\tEnter *.dat filename to write: ");
	scanf("%s", buff);	
	FILE *outfile = fopen(buff, "wb");
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

