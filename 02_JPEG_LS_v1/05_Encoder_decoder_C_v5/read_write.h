#ifndef _READ_WRITE_H_  
#define _READ_WRITE_H_

#include "globals.h"

char*** ReadPpmBinary(int* w, int* h);
char** ReadPgmBinary(int* w, int* h);
void WritePgmBinary(char** data, int w, int h);
void WritePpmBinary(char*** data, int w, int h);
void WriteJlsGray(char* data, int w, int h, int size);
char* ReadJlsGray(int* w, int* h, int* size);
void WriteJlsColorNoIlv(char** data, int w, int h, int* size);
char** ReadJlsColorNoIlv(int* w, int* h, int* size);
void WriteJlsColorLineIlv(char* data, int w, int h, int size);
char* ReadJlsColorLineIlv(int* w, int* h, int* size);
char* ReadJlsColorSampleIlv(int* w, int* h, int* size);
void WriteJlsColorSampleIlv(char* data, int w, int h, int size);
void WriteDat1D(char* data, int size);
void WriteDat2D(char** data, int w, int h);
#endif

