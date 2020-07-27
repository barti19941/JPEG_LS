#include "read_write.h"
#include "encoder.h"
#include "decoder.h"

int main() 
{
	int w, h;
	int i, j;
	int bitstream_size;
	char** data_in_encoder;
	char* data_out_encoder;
	char* data_in_decoder;
	char** data_out_decoder;
	data_in_encoder = ReadPgmPic(&w, &h);	
	data_out_encoder = Encode(data_in_encoder, w, h, &bitstream_size);
	WriteJlsPic(data_out_encoder, w, h, bitstream_size);	
	data_in_decoder = ReadJlsPic(&w, &h, &bitstream_size);
	data_out_decoder = Decode(data_in_decoder, w, h, bitstream_size);
	WritePgmPic(data_out_decoder, w, h);
	Free1D(data_out_encoder);
	Free1D(data_in_decoder);
	Free2D(data_out_decoder);
	Free2D(data_in_encoder);
	return 0;
}
