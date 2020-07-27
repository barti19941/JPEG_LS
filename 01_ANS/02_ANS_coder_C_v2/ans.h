#ifndef _ANS_H_  
#define _ANS_H_

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define BITS_TO_OUTPUT 1
#define RENORMALIZATION_FACTOR 1

typedef struct
{
	unsigned char symbol;
	unsigned char nbBits;
	unsigned char newX;	
}
t;

typedef struct 
{				
	unsigned short int L;
	unsigned short int max;
	unsigned char step;
	unsigned char R;
	unsigned char r;
	unsigned char* Ls;	
	unsigned char* s;	
	unsigned char* symbols;
	unsigned short int* k;
	unsigned short int* nb;
	short int* start;
	short int* next;
	unsigned char* encodingTable;
	t* decodingTable;
} 
tANS;

void tAnsInitialize(unsigned char* data, unsigned short int size, unsigned char step);
void tAnsDeinitialize(void);
unsigned char* tAnsEncodingBinaryWhole(unsigned char* data, unsigned short int data_size, unsigned short int* bitstream_size);
unsigned char* tAnsDecodingBinaryWhole(unsigned char* bitstream, unsigned short int data_size, unsigned short int bitstream_size);

void rAnsInitialize(unsigned char* data, unsigned short int size);
void rAnsDeinitialize(void);
unsigned long long int rAnsEncodingNumeralSingle(unsigned long long int x, unsigned char s);
unsigned long long int rAnsDecodingNumeralSingle(unsigned long long int x, unsigned char* s);
unsigned long long int rAnsEncodingNumeralWhole(unsigned char* data, unsigned short int data_size);
unsigned char* rAnsDecodingNumeralWhole(unsigned long long int x, unsigned short int data_size);
unsigned char* rAnsEncodingBinaryWhole(unsigned char* data, unsigned short int data_size, unsigned short int* bitstream_size);
unsigned char* rAnsDecodingBinaryWhole(unsigned char* bitstream, unsigned short int data_size, unsigned short int bitstream_size);
		
#endif
