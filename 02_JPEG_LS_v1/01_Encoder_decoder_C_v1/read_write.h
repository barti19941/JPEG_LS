#ifndef _READ_WRITE_H_  
#define _READ_WRITE_H_

#include "globals.h"

char** ReadPgmPic(int* w, int* h);
char* ReadJlsPic(int* w, int* h, int* size);
void WriteJlsPic(char* data, int w, int h, int size);
void WritePgmPic(char** data, int w, int h);

#endif
