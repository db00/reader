#ifndef bytearray_h
#define bytearray_h
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#define FREE_SID -1
#define END_SID -2
#define SAT_SID -3
#define MSAT_SID -4

typedef struct ByteArray{
	char * data;
	int position;
	int length;
} ByteArray;






char ByteArray_readByte(ByteArray * bytearray);
void ByteArray_readBytes(ByteArray * bytearray,int size,char * bytes);
unsigned short int ByteArray_readInt16(ByteArray * bytearray);
unsigned int ByteArray_readInt32(ByteArray * bytearray);
int ByteArray_print16(ByteArray*bytearray,int n);
int ByteArray_prints(ByteArray*bytearray,int n);
void ByteArray_printx(ByteArray*bytearray,int num);


ByteArray * ByteArray_new(int size);
void ByteArray_free(ByteArray*bytearray);
ByteArray*ByteArray_resize(ByteArray*bytearray,int size);
ByteArray * ByteArray_writeBytes(ByteArray*bytearray,char*bytes,int length);
void ByteArray_rewind(ByteArray*bytearray);
char * ByteArray_readUtf16(ByteArray * bytearray,int n);//need free
char * ByteArray_readLine(ByteArray * bytearray);

int isLittle_endian();
int u16ToUtf8(unsigned short w,char * ret);

#endif
