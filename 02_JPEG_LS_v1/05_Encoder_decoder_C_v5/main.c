#include "read_write.h"
#include "encoder_gray.h"
#include "encoder_color_line_ilv.h"
#include "encoder_color_no_ilv.h"
#include "encoder_color_sample_ilv.h"
#include "decoder_gray.h"
#include "decoder_color_line_ilv.h"
#include "decoder_color_no_ilv.h"
#include "decoder_color_sample_ilv.h"

/* 
	Title: 	Behavoiral code of JPEG-LS decoder and encoder.
	Author:	Dawid Bartosiak
	For more information refer to JPEG website (https://jpeg.org/)
	and JPEG-LS specification (Lossless and near-lossless compression 
	of continuous-tone still images - Baseline).
*/	
int main() 
{
	int option_choice = 1;
	int modify_choice = 1;
	int w, h;
	char* gray_data_jls;
	char** gray_data_decoded;	
	char* color_data_lilv_jls;
	char*** color_data_lilv_decoded;
	char*** color_data_nilv_decoded;
	char** color_data_nilv_jls;
	char*** color_data_silv_raw;
	char* color_data_silv_encoded;
	char*** color_data_nilv_raw;
	char** color_data_nilv_encoded;
	char*** color_data_lilv_raw;	
	char* color_data_lilv_encoded;
	char** gray_data_raw;
	char* gray_data_encoded;
	char* color_data_silv_jls;
	char*** color_data_silv_decoded;
	int bitstream_size[COMPONENTS_COUNT];
	while(option_choice)
	{
		system("cls");
		printf("===========================\n");
    	printf("(1). Encode gray *.pgm image (nilv, golomb coding).\n");
    	printf("(2). Encode color *.ppm image (lilv, golomb coding).\n");
    	printf("(3). Encode color *.ppm image (nilv, golomb coding).\n");
    	printf("(4). Encode color *.ppm image (silv, golomb coding).\n");
    	printf("(5). Decode gray *.jls image (nilv, golomb coding).\n");
    	printf("(6). Decode color *.jls image (lilv, golomb coding).\n");
    	printf("(7). Decode color *.jls image (nilv, golomb coding).\n");
    	printf("(8). Decode color *.jls image (silv, golomb coding).\n");
    	printf("(0). Exit.\n");
    	printf("===========================\n\n");
    	printf("What do you want to do?\n");
    	scanf("%d", &option_choice);
    	switch(option_choice)
    	{
    		case 1:	gray_data_raw = ReadPgmBinary(&w, &h);	
    				gray_data_encoded = EncodeGray(gray_data_raw, w, h, bitstream_size);
    				WriteJlsGray(gray_data_encoded, w, h, bitstream_size[0]);
    				Free2D(gray_data_raw);
    				Free1D(gray_data_encoded);
    				bitstream_size[0] = bitstream_size[1] = bitstream_size[2] = 0;
					gray_data_raw = NULL;
					gray_data_encoded = NULL;				
    				break;
    		case 2: color_data_lilv_raw = ReadPpmBinary(&w, &h);
    				printf("INFO\tDo you want to modify input data? (R=R, G=R-G, B = R-B) 1/0: ");
    				scanf("%d", &modify_choice);
    				if (modify_choice == 1)
    				{
    					ModifyInputData(color_data_lilv_raw, w, h);
					}
    				color_data_lilv_encoded = EncodeColorLineIlv(color_data_lilv_raw, w, h, bitstream_size);
    				WriteJlsColorLineIlv(color_data_lilv_encoded, w, h, bitstream_size[0]);
    				Free3D(color_data_lilv_raw);
    				Free1D(color_data_lilv_encoded);
    				bitstream_size[0] = bitstream_size[1] = bitstream_size[2] = 0;
    				color_data_lilv_raw = NULL;    
    				color_data_lilv_encoded = NULL;
					break;
    		case 3:	color_data_nilv_raw = ReadPpmBinary(&w, &h);
    		   		printf("INFO\tDo you want to modify input data? (R=R, G=R-G, B = R-B) 1/0: ");
    				scanf("%d", &modify_choice);
    				if (modify_choice == 1)
    				{
    					ModifyInputData(color_data_nilv_raw, w, h);
					}
    				color_data_nilv_encoded = EncodeColorNoIlv(color_data_nilv_raw, w, h, bitstream_size);
    				WriteJlsColorNoIlv(color_data_nilv_encoded, w, h, bitstream_size);
    				Free3D(color_data_nilv_raw);
    				Free1D(color_data_nilv_encoded[0]);
    				Free1D(color_data_nilv_encoded[1]);
    				Free1D(color_data_nilv_encoded[2]);
    				bitstream_size[0] = bitstream_size[1] = bitstream_size[2] = 0;
    				color_data_lilv_raw = NULL;
    				color_data_nilv_encoded[0] = color_data_nilv_encoded[1] = color_data_nilv_encoded[2] = NULL;    				
    				break;
    		case 4:	color_data_silv_raw = ReadPpmBinary(&w, &h);
    		 		printf("INFO\tDo you want to modify input data? (R=R, G=R-G, B = R-B) 1/0: ");
    				scanf("%d", &modify_choice);
    				if (modify_choice == 1)
    				{
    					ModifyInputData(color_data_silv_raw, w, h);
					}
    				color_data_silv_encoded = EncodeColorSampleIlv(color_data_silv_raw, w, h, bitstream_size);
    				WriteJlsColorSampleIlv(color_data_silv_encoded, w, h, bitstream_size[0]);
    				Free3D(color_data_silv_raw);
    				Free1D(color_data_silv_encoded);
    				bitstream_size[0] = bitstream_size[1] = bitstream_size[2] = 0;
    				color_data_silv_raw = NULL;
    				color_data_silv_encoded = NULL;
    				break;
    		case 5:	gray_data_jls = ReadJlsGray(&w, &h, bitstream_size);
    				gray_data_decoded = DecodeGray(gray_data_jls, w, h, bitstream_size[0]);		
    				WritePgmBinary(gray_data_decoded, w, h);
    				Free1D(gray_data_jls);
    				Free2D(gray_data_decoded);
    				gray_data_jls = NULL;
    				gray_data_decoded = NULL;
    				break;
    		case 6:	color_data_lilv_jls = ReadJlsColorLineIlv(&w, &h, bitstream_size);
    				color_data_lilv_decoded = DecodeColorLineIlv(color_data_lilv_jls, w, h, bitstream_size[0]);
    				printf("INFO\tDo you want to de-modify input data? (R=R, G=R-G, B = R-B) 1/0: ");
    				scanf("%d", &modify_choice);
    				if (modify_choice == 1)
    				{
    					DemodifyInputData(color_data_lilv_decoded, w, h);
					}			
    				WritePpmBinary(color_data_lilv_decoded, w, h);
    				Free1D(color_data_lilv_jls);
					Free3D(color_data_lilv_decoded);
					color_data_lilv_jls = NULL;
					color_data_lilv_decoded = NULL;
					break;			
			case 7:	color_data_nilv_jls = ReadJlsColorNoIlv(&w, &h, bitstream_size);
					color_data_nilv_decoded = DecodeColorNoIlv(color_data_nilv_jls, w, h, bitstream_size);
					printf("INFO\tDo you want to de-modify input data? (R=R, G=R-G, B = R-B) 1/0: ");
    				scanf("%d", &modify_choice);
    				if (modify_choice == 1)
    				{
    					DemodifyInputData(color_data_nilv_decoded, w, h);
					}			
					WritePpmBinary(color_data_nilv_decoded, w, h);
					Free1D(color_data_nilv_jls[0]);
					Free1D(color_data_nilv_jls[1]);
					Free1D(color_data_nilv_jls[2]);
					Free3D(color_data_nilv_decoded);
					color_data_nilv_jls = NULL;
					color_data_nilv_decoded = NULL;
					break;		
			case 8: color_data_silv_jls = ReadJlsColorSampleIlv(&w, &h, bitstream_size);
					color_data_silv_decoded = DecodeColorSampleIlv(color_data_silv_jls, w, h, bitstream_size[0]);
					printf("INFO\tDo you want to de-modify input data? (R=R, G=R-G, B = R-B) 1/0: ");
    				scanf("%d", &modify_choice);
    				if (modify_choice == 1)
    				{
    					DemodifyInputData(color_data_silv_decoded, w, h);
					}	
					WritePpmBinary(color_data_silv_decoded, w, h);
					Free1D(color_data_silv_jls);
					Free3D(color_data_silv_decoded);
					color_data_silv_jls = NULL;
					color_data_silv_decoded = NULL;
					break;
    		default:printf("GoodBye!\n");
    				break;
		}
	}
	return 0;
}


