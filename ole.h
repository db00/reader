#ifndef ole_h
#define ole_h

#include<math.h>
#include "bytearray.h"
extern int little_endian;

typedef unsigned long ULONG;
typedef unsigned short USHORT;
typedef short OFFSET;
typedef ULONG SECT;
typedef ULONG FSINDEX;
typedef USHORT FSOFFSET;
typedef ULONG DFSIGNATURE;
typedef unsigned char BYTE;
typedef unsigned short WORD;
typedef unsigned long DWORD;
typedef WORD DFPROPTYPE;
typedef ULONG SID;
//typedef CLSID GUID;
typedef struct tagFILETIME 
{
	DWORD dwLowDateTime;
	DWORD dwHighDateTime;
} FILETIME, TIME_T;
typedef enum tagSTGTY
{
	STGTY_INVALID,
	STGTY_STORAGE,
	STGTY_STREAM,
	STGTY_LOCKBYTES,
	STGTY_PROPERTY,
	STGTY_ROOT,
} STGTY;

typedef struct StorageEntry
{
	char * name;//u16 string 64 bytes;
	unsigned short entryStrLen;//2 bytes
	char type;//1 byte; 0:empty,1:Storage,2:stream,3:lockbytes,4:property,t,root Storage
	char color;//1 byte; 0:red,1:black
	int leftSonId;//4 bytes; -1:nothing;
	int rightSonId;//4 bytes; -1:nothing
	int rootId;// 4 bytes;
	char *id;//16 bytes
	int uid;//4 bytes
	char * createTime;//8 bytes;from 1601.1.1 00:00:00, ms
	char * modifyTime;//8 bytes
	int sid0;//4 bytes
	int streamSize;//4 bytes
	short int noUse;//4 bytes

	int did;
	struct StorageEntry * root;
	struct StorageEntry * left;
	struct StorageEntry * right;
}StorageEntry;


typedef struct OleHeader
{
	ByteArray *bytearray;
	char *headstr;
	int little_endian;
	int sid0;
	unsigned int sectorSize;
	unsigned int miniSsize;
	unsigned int numS;
	unsigned int streamSize;
	int miniSid0;
	unsigned int numMiniS;
	int msid0;
	unsigned int numMiniMS;
	int MSectorIDs[109];


	int numStorages;
	StorageEntry *rootStorage;
	StorageEntry *storages[16];
}OleHeader;


unsigned int Ole_getEntryPostion(OleHeader * header,StorageEntry *entry);
StorageEntry * Ole_getEntryByName(OleHeader *header,char *name);
OleHeader * Ole_read(OleHeader* header,ByteArray * bytearray);
void Ole_free(OleHeader * header);
#endif
