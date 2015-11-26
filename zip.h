#ifndef zip_h
#define zip_h

#include "zlib.h"
#include "bytearray.h"

typedef struct ZipHeader{
	unsigned int sig;//must be 0x04034b50
	unsigned short verNeed;
	unsigned short flag;
	unsigned short method;
	unsigned short modifytime;
	unsigned short modifydate;
	unsigned int crc32;
	unsigned int size;
	unsigned int unCompressedSize;
	unsigned short filenameLen;
	unsigned short extraFieldLen;
	char * fileName;
	char * extraField;
	char * data;
}ZipHeader;

typedef struct ZipFile{
	ZipHeader * header;
	ByteArray * bytearray;
}ZipFile;

extern int little_endian;
void ZipFile_free(ZipFile * file);
ZipFile * ZipFile_parser(ByteArray * bytearray,char * fileName,char * out,int *outlen);
#endif
