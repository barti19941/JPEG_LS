#include "read_write.h"
#include "encoder_gray.h"
#include "encoder_color_line_ilv.h"
#include "encoder_color_nilv.h"
#include "encoder_color_sample_ilv.h"
#include "decoder_gray.h"
/* 
	Title: 	Behavoiral code of JPEG-LS decoder and encoder.
	Author:	Dawid Bartosiak
	For more information refer to JPEG website (https://jpeg.org/)
	and JPEG-LS specification (Lossless and near-lossless compression 
	of continuous-tone still images - Baseline).
*/
int main() 
{
	int choice = 1;
	int w, h;
	char* gray_data_jls;
	char** gray_data_decoded;
	char*** color_data_silv_raw;
	char* color_data_silv_encoded;
	char*** color_data_nilv_raw;
	char** color_data_nilv_encoded;
	char*** color_data_lilv_raw;	
	char* color_data_lilv_encoded;
	char** gray_data_raw;
	char* gray_data_encoded;
	int bitstream_size[COMPONENTS_COUNT];
	while(choice)
	{
		system("cls");
		printf("===========================\n");
    	printf("(1). Encode gray *.pgm image (nilv, golomb coding).\n");
    	printf("(2). Encode color *.ppm image (line ilv, golomb coding).\n");
    	printf("(3). Encode color *.ppm image (nilv, golomb coding).\n");
    	printf("(4). Encode color *.ppm image (silv, golomb coding).\n");
    	printf("(5). Decode gray *.pgm image (nilv, golomb coding).\n");
    	printf("(0). Exit.\n");
    	printf("===========================\n\n");
    	printf("What do you want to do?\n");
    	scanf("%d", &choice);
    	switch(choice)
    	{
    		case 1:	gray_data_raw = ReadPgmPicGray(&w, &h);	
    				gray_data_encoded = EncodeGray(gray_data_raw, w, h, bitstream_size);
    				WriteJlsGrayPic(gray_data_encoded, w, h, bitstream_size[0]);
    				Free2D(gray_data_raw);
    				Free1D(gray_data_encoded);
    				bitstream_size[0] = bitstream_size[1] = bitstream_size[2] = 0;
    				gray_data_raw = NULL;
					gray_data_encoded = NULL;
    				break;
    		case 2: color_data_lilv_raw = ReadPpmPicColor(&w, &h);
    				color_data_lilv_encoded = EncodeColorLineIlv(color_data_lilv_raw, w, h, bitstream_size);
    				WriteJlsColorLineIlvPic(color_data_lilv_encoded, w, h, bitstream_size[0]);
    				Free3D(color_data_lilv_raw);
    				Free1D(color_data_lilv_encoded);
    				bitstream_size[0] = bitstream_size[1] = bitstream_size[2] = 0;
    				color_data_lilv_raw = NULL;    
    				color_data_lilv_encoded = NULL;
					break;
    		case 3:	color_data_nilv_raw = ReadPpmPicColor(&w, &h);
    				color_data_nilv_encoded = EncodeColorNIlv(color_data_nilv_raw, w, h, bitstream_size);
    				WriteJlsColorNIlvPic(color_data_nilv_encoded, w, h, bitstream_size);
    				Free3D(color_data_nilv_raw);
    				Free1D(color_data_nilv_encoded[0]);
    				Free1D(color_data_nilv_encoded[1]);
    				Free1D(color_data_nilv_encoded[2]);
    				bitstream_size[0] = bitstream_size[1] = bitstream_size[2] = 0;
    				color_data_lilv_raw = NULL;
    				color_data_nilv_encoded[0] = color_data_nilv_encoded[1] = color_data_nilv_encoded[2] = NULL;    				
    				break;
    		case 4:	color_data_silv_raw = ReadPpmPicColor(&w, &h);
    				color_data_silv_encoded = EncodeColorSampleIlv(color_data_silv_raw, w, h, bitstream_size);
    				WriteJlsColorSIlvPic(color_data_silv_encoded, w, h, bitstream_size[0]);
    				Free3D(color_data_silv_raw);
    				Free1D(color_data_silv_encoded);
    				bitstream_size[0] = bitstream_size[1] = bitstream_size[2] = 0;
    				color_data_silv_raw = NULL;
    				color_data_silv_encoded = NULL;
    				break;
    		case 5:	gray_data_jls = ReadJlsPicGray(&w, &h, bitstream_size);
    				gray_data_decoded = DecodeGray(gray_data_jls, w, h, bitstream_size[0]);
    				Free1D(gray_data_jls);
    				gray_data_jls = NULL;
    		default:printf("GoodBye!\n");
    				break;
		}
	}
	return 0;
}

