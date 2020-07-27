#ifndef _READ_WRITE_H_  
#define _READ_WRITE_H_

#include "globals.h"

char*** ReadPpmPicColor(int* w, int* h);
char** ReadPgmPicGray(int* w, int* h);
char* ReadJlsPicGray(int* w, int* h, int* size);
void WriteJlsGrayPic(char* data, int w, int h, int size);
void WriteJlsColorLineIlvPic(char* data, int w, int h, int size);
void WriteJlsColorNIlvPic(char** data, int w, int h, int* size);
void WriteJlsColorSIlvPic(char* data, int w, int h, int size);
void WritePgmPic(char** data, int w, int h);
void WriteDat1D(char* data, int size);
void WriteDat2D(char** data, int w, int h);
#endif
