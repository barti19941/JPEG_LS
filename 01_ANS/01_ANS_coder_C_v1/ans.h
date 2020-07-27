#ifndef _ANS_H_  
#define _ANS_H_

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define BITS_TO_OUTPUT 1
#define RENORMALIZATION_FACTOR 1

void tAnsInitialize(unsigned char* data, unsigned short int size);
void rAnsInitialize(unsigned char* data, unsigned short int size);
void rAnsDeinitialize(void);
unsigned long long int rAnsEncodingNumeralSingle(unsigned long long int x, unsigned char s);
unsigned long long int rAnsDecodingNumeralSingle(unsigned long long int x, unsigned char* s);
unsigned long long int rAnsEncodingNumeralWhole(unsigned char* data, unsigned short int data_size);
unsigned char* rAnsDecodingNumeralWhole(unsigned long long int x, unsigned short int data_size);
unsigned char* rAnsEncodingBinaryWhole(unsigned char* data, unsigned short int data_size, unsigned short int* bitstream_size);
unsigned char* rAnsDecodingBinaryWhole(unsigned char* bitstream, unsigned short int data_size, unsigned short int bitstream_size);
		
#endif
