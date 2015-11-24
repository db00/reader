#include "ole.h"

static int Ole_getSectorPos(OleHeader* header,int sid)
{
	return 0x200 + sid*header->sectorSize;
}
static int Ole_getShortSectorPos(OleHeader* header,int sid)
{
	return 0x200 + sid*header->miniSsize;
}


unsigned int Ole_getEntryPostion(OleHeader * header,StorageEntry *entry)
{
	unsigned int position;
	if(entry->streamSize >= header->streamSize){
		position = Ole_getSectorPos(header,entry->sid0);
	}else{
		position = Ole_getShortSectorPos(header,entry->sid0);
	}
	return position;
}


StorageEntry * Ole_getEntryByName(OleHeader *header,char *name)
{
	int i = 0;
	while(i< header->numStorages)
	{
		StorageEntry * entry = header->storages[i];
		if(entry)
		{
			//printf("%s,",entry->name);
			if(strcmp(entry->name,name)==0)
			{
				return entry;
			}
		}
		++i;
	}
	return NULL;
}

static StorageEntry* Ole_readStorageFromBytearray(ByteArray* bytearray)
{
	//int position = bytearray->position;
	StorageEntry* storage = (StorageEntry*)malloc(sizeof(StorageEntry));
	//ByteArray_print16(bytearray,0);
	memset(storage,0,sizeof(StorageEntry));
	storage->name = ByteArray_readUtf16(bytearray,32);
	//bytearray->position += 64;
	storage->entryStrLen = ByteArray_readInt16(bytearray);
	storage->type= ByteArray_readByte(bytearray);
	storage->color= ByteArray_readByte(bytearray);
	storage->leftSonId = ByteArray_readInt32(bytearray);
	storage->rightSonId = ByteArray_readInt32(bytearray);
	storage->rootId = ByteArray_readInt32(bytearray);
	storage->id= bytearray->data+bytearray->position;
	bytearray->position+=16;
	storage->uid= ByteArray_readInt32(bytearray);
	bytearray->position+=16;
	storage->sid0= ByteArray_readInt32(bytearray);
	storage->streamSize= ByteArray_readInt32(bytearray);
	bytearray->position+=4;

	printf("name: %s,",storage->name);
	//printf("%d,",bytearray->position-position);
	//printf("storage id:%d\n",storage->id);
	switch(storage->type)
	{
		case 0:
			printf("type:%s,","invalid");
			return storage;
			break;
		case 1:
			printf("type:%s,","storage");
			break;
		case 2:
			printf("type:%s,","stream");
			break;
		case 3:
			printf("type:%s,","lockbytes");
			break;
		case 4:
			printf("type:%s,","property");
			break;
		case 5:
			printf("type:%s,","root");
			break;
	}
	/*
	   if(storage->color)
	   printf("storage color:%s\n","black");
	   else
	   printf("storage color:%s\n","red");
	   */

	if(storage->leftSonId!=-1) printf("left :%d ;",storage->leftSonId);
	if(storage->rightSonId !=-1) printf("right :%d ;",storage->rightSonId);
	if(storage->rootId !=-1) printf("treeId :%d ;",storage->rootId);

	printf("streamSize:0x%x ;",storage->streamSize);
	printf("sectorId :%d\n",storage->sid0);
	//printf("-----------------------\n");
	return storage;
}

static StorageEntry * Ole_readEntrys(OleHeader * header,int entryId)
{
	if(entryId>=0)
	{
		ByteArray * bytearray = header->bytearray;
		bytearray->position = Ole_getSectorPos(header,header->sid0) + 128 * entryId;
		printf("\n=======<0x%x>(%d)",bytearray->position,entryId);
		StorageEntry * entry = Ole_readStorageFromBytearray(bytearray);
		header->storages[entryId] = entry;
		entry->did = entryId;
		if(header->numStorages -1< entryId)
		{
			header->numStorages = entryId + 1;
		}

		/*
		   if(entry->sid0>=0){
		   bytearray->position = Ole_getSectorPos(header->header,entry->sid0);
	//printf("\n_=[0x%x]=(%d): ",bytearray->position,entryId);
	short int sid = ByteArray_readInt16(bytearray);
	while(sid){
	printf("%d,",sid);
	if(sid==-2)
	break;
	sid = ByteArray_readInt16(bytearray);
	}
	}
	*/

		if(entry->leftSonId>0){
			entry->left = Ole_readEntrys(header,entry->leftSonId);
		}
		if(entry->rightSonId>0){
			entry->right = Ole_readEntrys(header,entry->rightSonId);
		}
		if(entry->rootId>0){
			entry->root = Ole_readEntrys(header,entry->rootId);
		}
		return entry;
	}
	return NULL;
}


OleHeader * Ole_read(OleHeader * header,ByteArray * bytearray)
{
	if(header)
		return header;

	bytearray->position = 0;
	int i=0;
	char _header[8];
	header = malloc(sizeof(OleHeader));
	memset(header,0,sizeof(OleHeader));
	header->bytearray = bytearray;
	bytearray->position = 0;
	ByteArray_readBytes(bytearray,8,_header);

	if(strncmp(_header,"\xd0\xcf\x11\xe0\xa1\xb1\x1a\xe1",8)==0){
		//printf("headstr\n");
	}else if(strncmp(_header,"\x0e\x11\xfc\x0d\xd0\xcf\x11\xe0",8)==0){
		//printf("old headstr\n");
	}else{
		printf("not a doc file\n");
		free(header);
		return NULL;
	}


	struct byteOrder16{
		char a:4;
		char b:4;
	} b16;
	b16.a = 1;
	b16.b = 0;
	little_endian = (*((char*)&b16)==1);//else == 16

	bytearray->position = 0x1c;
	unsigned int byteOrder=(unsigned int)ByteArray_readInt16(bytearray);
	if(byteOrder==0xfffe)
	{
		little_endian = 1;
		printf("little_endian\n");
	}else if(byteOrder==0xfeff){
		little_endian= 0;
		printf("big_endian\n");
	}else{
		printf("unknow byteOrder\n");
		free(header);
		return NULL;
	}
	bytearray->position = 0x1e;
	header->sectorSize= pow(2,ByteArray_readInt16(bytearray));
	bytearray->position = 0x20;
	header->miniSsize= pow(2,ByteArray_readInt16(bytearray));
	bytearray->position = 0x2c;
	header->numS= ByteArray_readInt32(bytearray);
	bytearray->position = 0x30;
	header->sid0= (int)ByteArray_readInt32(bytearray);

	bytearray->position = 0x38;
	header->streamSize = ByteArray_readInt32(bytearray);//maximum size for mini-streams: typically 4096 bytes
	bytearray->position = 0x3c;
	header->miniSid0= (int)ByteArray_readInt32(bytearray);
	bytearray->position = Ole_getSectorPos(header,header->miniSid0);
	//ByteArray_print16(bytearray,0);
	bytearray->position = 0x40;
	header->numMiniS= ByteArray_readInt32(bytearray);
	bytearray->position = 0x44;
	header->msid0= ByteArray_readInt32(bytearray);
	bytearray->position = 0x48;
	header->numMiniMS= ByteArray_readInt32(bytearray);


	bytearray->position = 0x4c;
	i=0;
	while(i< header->numS){
		header->MSectorIDs[i]= ByteArray_readInt32(bytearray);
		printf("%d,",header->MSectorIDs[i]);
		if(header->MSectorIDs[i]== -2)
			break;
		++i;
	}
	i=0;
	while(i< header->numS){
		int sid= header->MSectorIDs[i];
		if(sid>0){
			bytearray->position = Ole_getSectorPos(header,sid);
			int _sid = ByteArray_readInt32(bytearray);
			printf("\narr[%d]%d:",i,sid);
			while(_sid>=-4){
				if(_sid==-2)
					break;
				printf("%d,",_sid);
				_sid = ByteArray_readInt32(bytearray);
			}
		}
		if(sid== -2)
			break;
		++i;
	}

	printf("\n");


	printf("sid0 : %d, " ,header->sid0);
	printf("sectorSize:%d, " ,header->sectorSize);
	printf("miniSsize :%d, " ,header->miniSsize);
	printf("numS :%d, " ,header->numS);
	printf("numMiniS :%d, " ,header->numMiniS);
	printf("numMiniMS:%d, " ,header->numMiniMS);
	printf("streamSize : %d, " ,header->streamSize);
	printf("msid0 : %d, " ,header->msid0);
	printf("miniSid0 : %d, " ,header->miniSid0);

	printf("==========\n");

	//return 0;
	header->rootStorage = Ole_readEntrys(header,0);
	if(header->rootStorage == NULL)
		return 0;


	return header;
}

void StorageEntry_free(StorageEntry*entry)
{
	if(entry)
	{
		if(entry->name)
			free(entry->name);
		free(entry);
	}
}
void Ole_free(OleHeader * header)
{
	if(header)
	{
		if(header->rootStorage)
		{
			int i=0;
			while(i<header->numStorages)
			{
				StorageEntry_free(header->storages[i]);
				++i;
			}
		}

		free(header);
	}
}
