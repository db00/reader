/**
 * @file zip.c
 gcc -D test_zip zip.c bytearray.c inflate.c crc32.c uncompr.c adler32.c inffast.c inftrees.c zutil.c && ./a.out
 * @author db0@qq.com
 * @version 1.0.1
 * @date 2015-11-02
 */


#include "zip.h"

typedef struct ZipDirHead{
	unsigned int sig;//must be 0x02014b50
	unsigned short ver;//pkware
	unsigned short verMinimum;//
	unsigned short flag;//
	unsigned short method;//
	unsigned short modifytime;//
	unsigned short modifydate;//
	unsigned int crc32;//
	unsigned int compressedSize;//
	unsigned int unCompressedSize;//
	unsigned short filenameLen;//
	unsigned short extraFieldLen;//
	unsigned short fileCommetLen;//
	unsigned short diskNum;//
	unsigned short internalAttrib;//
	unsigned int externalAttrib;//
	unsigned int headerOffset;//
	char * fileName;
	char * extraField;
	char * fileCommet;
}ZipDirHead;	

void ZipDirHead_free(ZipDirHead * header)
{
	if(header)
	{
		free(header);
	}
}
ZipDirHead * ZipDirHead_read(ByteArray * bytearray)
{
	ZipDirHead * zipdirhead = malloc(sizeof(ZipDirHead));
	memset(zipdirhead,0,sizeof(ZipDirHead));
	zipdirhead->sig = ByteArray_readInt32(bytearray);
	if(zipdirhead->sig!=0x02014b50)
	{
		printf("ZipDirHead Error!\n");
		ZipDirHead_free(zipdirhead);
		return 0;
	}
	zipdirhead->ver = ByteArray_readInt16(bytearray);
	char l = zipdirhead->ver & 0xff;
	printf("ver:%d.%d,",l/10,l%10);
	char h = (zipdirhead->ver>>8) & 0xff;
	switch(h)
	{
		case 0:
			printf("msdos,os/2 fat,");
			break;
		case 1:
			printf("amiga,");
			break;
		case 2:
			printf("vms,");
			break;
		case 3:
			printf("unix,");
			break;
		case 4:
			printf("vm/cms,");
			break;
		case 5:
			printf("atariSt,");
			break;
		case 6:
			printf("os2/ hpfs,");
			break;
		case 7:
			printf("macintosh,");
			break;
		case 8:
			printf("z-system,");
			break;
		case 9:
			printf("c p/m,");
			break;
		default:
			printf("unknow,");
			break;
	}
	zipdirhead->verMinimum = ByteArray_readInt16(bytearray);
	//if(zipdirhead->verMinimum) printf("verMinimum:%d,",zipdirhead->verMinimum);
	zipdirhead->flag= ByteArray_readInt16(bytearray);
	//if(zipdirhead->flag) printf("flag:0x%x,",zipdirhead->flag);

	zipdirhead->method= ByteArray_readInt16(bytearray);
	//if(zipdirhead->method) printf("method:%d,",zipdirhead->method);

	zipdirhead->modifytime= ByteArray_readInt16(bytearray);
	//if(zipdirhead->modifytime) printf("modifytime:%d,",zipdirhead->modifytime);
	zipdirhead->modifydate= ByteArray_readInt16(bytearray);
	//if(zipdirhead->modifydate) printf("modifydate:%d,",zipdirhead->modifydate);
	zipdirhead->crc32= ByteArray_readInt32(bytearray);
	//if(zipdirhead->crc32) printf("crc32:0x%x,",zipdirhead->crc32);

	zipdirhead->compressedSize = ByteArray_readInt32(bytearray);
	//if(zipdirhead->compressedSize) printf("compressedSize:0x%x,",zipdirhead->compressedSize);
	zipdirhead->unCompressedSize = ByteArray_readInt32(bytearray);
	//if(zipdirhead->unCompressedSize) printf("unCompressedSize:0x%x,",zipdirhead->unCompressedSize);
	zipdirhead->filenameLen = ByteArray_readInt16(bytearray);
	//if(zipdirhead->filenameLen) printf("filenameLen:%d,",zipdirhead->filenameLen);
	zipdirhead->extraFieldLen = ByteArray_readInt16(bytearray);
	//if(zipdirhead->extraFieldLen) printf("extraFieldLen:%d,",zipdirhead->extraFieldLen);
	zipdirhead->fileCommetLen= ByteArray_readInt16(bytearray);
	//if(zipdirhead->fileCommetLen) printf("fileCommetLen:%d,",zipdirhead->fileCommetLen);
	zipdirhead->diskNum = ByteArray_readInt16(bytearray);
	//if(zipdirhead->diskNum) printf("diskNum:%d,",zipdirhead->diskNum);
	zipdirhead->internalAttrib= ByteArray_readInt16(bytearray);
	//if(zipdirhead->internalAttrib) printf("internalAttrib:%d,",zipdirhead->internalAttrib);
	zipdirhead->externalAttrib = ByteArray_readInt32(bytearray);
	//if(zipdirhead->externalAttrib) printf("externalAttrib:%d,",zipdirhead->externalAttrib);
	zipdirhead->headerOffset= ByteArray_readInt32(bytearray);
	//if(zipdirhead->headerOffset) printf("headerOffset:%d,",zipdirhead->headerOffset);
	if(zipdirhead->filenameLen)
	{
		if(h==1)
		{
			zipdirhead->fileName = malloc(zipdirhead->filenameLen*3);
			memset(zipdirhead->fileName,0,zipdirhead->filenameLen*3);
			int i=0;
			while(i<zipdirhead->filenameLen)
			{
				char c = ByteArray_readByte(bytearray);
				if(((unsigned char)c<0x80) && isprint(c))
				{
					char b[2];
					b[0]=c;
					b[1]=0;
					strcat(zipdirhead->fileName,b);
					++i;
				}else{
					bytearray->position--;
					unsigned short int u = ByteArray_readInt16(bytearray);
					char b[8];
					memset(b,0,8);
					u16ToUtf8(u,b);
					strcat(zipdirhead->fileName,b);
					i+=2;
				}
			}
			//zipdirhead->fileName = ByteArray_readUtf16(bytearray,zipdirhead->filenameLen/2);
		}else{
			zipdirhead->fileName = malloc(zipdirhead->filenameLen+1);
			memset(zipdirhead->fileName,0,zipdirhead->filenameLen+1);
			ByteArray_readBytes(bytearray,zipdirhead->filenameLen,zipdirhead->fileName);
		}
		printf("fileName:%s,",zipdirhead->fileName);
	}
	if(zipdirhead->extraFieldLen)
	{
		zipdirhead->extraField = malloc(zipdirhead->extraFieldLen+1);
		memset(zipdirhead->extraField,0,zipdirhead->extraFieldLen+1);
		ByteArray_readBytes(bytearray,zipdirhead->extraFieldLen,zipdirhead->extraField);
		//printf(",extraField:%s,",zipdirhead->extraField);
	}
	if(zipdirhead->fileCommetLen)
	{
		zipdirhead->fileCommet = malloc(zipdirhead->fileCommetLen+1);
		memset(zipdirhead->fileCommet,0,zipdirhead->fileCommetLen+1);
		ByteArray_readBytes(bytearray,zipdirhead->fileCommetLen,zipdirhead->fileCommet);
		//printf(",fileCommet:%s,",zipdirhead->fileCommet);
	}
	printf("\n");
	return zipdirhead;
}
typedef struct ZipDirEnd{
	unsigned int sig;//must be 0x06054b50
	unsigned short diskNum;//
	unsigned short diskCentralStart;//
	unsigned short numDirs;//
	unsigned short numDirsCentral;//
	unsigned int size;//
	unsigned int offset;//
	unsigned short commetLen;//
	char * commet;
}ZipDirEnd;	
void ZipDirEnd_free(ZipDirEnd * zipdirend)
{
	if(zipdirend)
	{
		if(zipdirend->commet)
			free(zipdirend->commet);
		free(zipdirend);
	}
}
ZipDirEnd * ZipDirEnd_read(ByteArray * bytearray)
{
	ZipDirEnd * zipdirend = malloc(sizeof(zipdirend));
	memset(zipdirend,0,sizeof(*zipdirend));
	zipdirend->sig = ByteArray_readInt32(bytearray);
	if(zipdirend->sig != 0x06054b50)
	{
		printf("ZipDirEnd_read Error!\n");
		ZipDirEnd_free(zipdirend);
		return 0;
	}
	zipdirend->diskNum = ByteArray_readInt16(bytearray);
	zipdirend->diskCentralStart= ByteArray_readInt16(bytearray);
	zipdirend->numDirs= ByteArray_readInt16(bytearray);
	zipdirend->numDirsCentral= ByteArray_readInt16(bytearray);
	zipdirend->size= ByteArray_readInt32(bytearray);
	zipdirend->offset= ByteArray_readInt32(bytearray);
	zipdirend->commetLen= ByteArray_readInt16(bytearray);
	//if(zipdirend->diskNum) printf("diskNum:%d,",zipdirend->diskNum);
	//if(zipdirend->diskCentralStart) printf("diskCentralStart:%d,",zipdirend->diskCentralStart);
	//if(zipdirend->numDirs) printf("numDirs:%d,",zipdirend->numDirs);
	//if(zipdirend->numDirsCentral) printf("numDirsCentral:%d,",zipdirend->numDirsCentral);
	//if(zipdirend->size) printf("size:%d,",zipdirend->size);
	//if(zipdirend->offset) printf("offset:0x%x,",zipdirend->offset);
	//if(zipdirend->commetLen) printf("commetLen:%d,",zipdirend->commetLen);
	if(zipdirend->commetLen)
	{
		zipdirend->commet = malloc(zipdirend->commetLen);
		ByteArray_readBytes(bytearray,zipdirend->commetLen,zipdirend->commet);
		//printf(",commet:%s",zipdirend->commet);
	}
	//printf("\n");
	return zipdirend;
}




typedef struct ZipDataDesc{
	unsigned int sig;//must be 0x08074b50
	unsigned int crc32;//
	unsigned int compressedSize;//
	unsigned int unCompressedSize;//
}ZipDataDesc;
ZipDataDesc * ZipDataDesc_read(ByteArray * bytearray)
{
	ZipDataDesc * zipdatadesc = malloc(sizeof(*zipdatadesc));
	memset(zipdatadesc,0,sizeof(ZipDataDesc));
	ByteArray_readBytes(bytearray,16,(char*)zipdatadesc);
	return zipdatadesc;
}

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
void ZipHeader_free(ZipHeader * header)
{
	if(header)
	{
		free(header);
	}
}
ZipHeader * ZipHeader_read(ByteArray * bytearray,char * out)
{
	ZipHeader * header = malloc(sizeof(*header));
	memset(header,0,sizeof(*header));
	header->sig = ByteArray_readInt32(bytearray);
	//if("\x50\x4b\x3\x4",4))
	if(header->sig!=0x04034b50)
	{
		sprintf(out+strlen(out),"not a zip file");
		ZipHeader_free(header);
		return NULL;
	}
	header->verNeed = ByteArray_readInt16(bytearray);
	//sprintf(out+strlen(out),"verNeed:%d,",header->verNeed);
	header->flag= ByteArray_readInt16(bytearray);
	//sprintf(out+strlen(out),"flag:0x%x,",header->flag);
	header->method= ByteArray_readInt16(bytearray);
	//sprintf(out+strlen(out),"method:0x%x,",header->method);
	switch(header->method)
	{
		case 0:
			//sprintf(out+strlen(out),"不压缩");
			break;
		case 1:
			//sprintf(out+strlen(out),"缩小");
			break;
		case 2:
			//sprintf(out+strlen(out),"以压缩因素1缩小");
			break;
		case 3:
			//sprintf(out+strlen(out),"以压缩因素2缩小");
			break;
		case 4:
			//sprintf(out+strlen(out),"以压缩因素3缩小");
			break;
		case 5:
			//sprintf(out+strlen(out),"以压缩因素4缩小");
			break;
		case 6:
			//sprintf(out+strlen(out),"自展");
			break;
	}
	header->modifytime= ByteArray_readInt16(bytearray);
	//printf("modifytime:0x%x,",header->modifytime);
	header->modifydate= ByteArray_readInt16(bytearray);
	//printf("modifydate:0x%x,",header->modifydate);
	header->crc32= ByteArray_readInt32(bytearray);
	printf("crc32:0x%x,",header->crc32);
	header->size= ByteArray_readInt32(bytearray);
	//sprintf(out+strlen(out),"size:0x%x,",header->size);
	header->unCompressedSize= ByteArray_readInt32(bytearray);
	//sprintf(out+strlen(out),"unCompressedSize:0x%x,",header->unCompressedSize);
	header->filenameLen= ByteArray_readInt16(bytearray);
	//sprintf(out+strlen(out),"filenameLen:0x%x,",header->filenameLen);
	header->extraFieldLen= ByteArray_readInt16(bytearray);
	//if(header->extraFieldLen) sprintf(out+strlen(out),"extraFieldLen:0x%x,",header->extraFieldLen);

	if(header->filenameLen)
	{
		//unix
		header->fileName = malloc(header->filenameLen+1);
		memset(header->fileName,0,header->filenameLen+1);
		ByteArray_readBytes(bytearray,header->filenameLen,header->fileName);
		//header->fileName = ByteArray_readUtf16(bytearray,header->filenameLen);
		//sprintf(out+strlen(out),"fileName:%s,(%d),",header->fileName,header->filenameLen);
	}


	if(header->extraFieldLen)
	{
		header->extraField = malloc(header->extraFieldLen+1);
		memset(header->extraField,0,header->extraFieldLen+1);
		ByteArray_readBytes(bytearray,header->extraFieldLen,header->extraField);
		//printf("extraField:%s,",header->extraField);
	}

	if(header->flag & (1<<10))
	{
		ZipDataDesc_read(bytearray);
	}
	if(header->size)
	{
		printf("data:0x%x,",bytearray->position);
		header->data = malloc(header->size+2);
		header->data[1] = '\x9c';
		header->data[0] = '\x78';

		ByteArray_readBytes(bytearray,header->size,header->data+2);
		printf("position:0x%x,",bytearray->position);
	}

	printf("\n");

	return header;
}

typedef struct ZipFile{
	ZipHeader * header;
	ByteArray * bytearray;
}ZipFile;
void ZipFile_free(ZipFile * file)
{
	if(file)
	{
		if(file->header)
			ZipHeader_free(file->header);
		free(file);
	}
}

ZipFile * ZipFile_parser(ByteArray * bytearray,char * fileName,char * out,int *outlen)
{
	little_endian = isLittle_endian();
	ZipFile * zipfile = malloc(sizeof(ZipFile));
	memset(zipfile,0,sizeof(ZipFile));
	zipfile->bytearray = bytearray;
	while(bytearray->position<bytearray->length-4)
	{
		int sig = ByteArray_readInt32(bytearray);
		//sprintf(out+strlen(out),"sig:0x%x\n",sig);
		switch(sig)
		{
			case 0x04034b50:
				bytearray->position-= 4;
				ZipHeader * header = ZipHeader_read(bytearray,out);
				if(header==0)
					break;
				if(header->data && strcmp(fileName,header->fileName)==0)
				{
					uLong len = header->unCompressedSize;
					uncompress((Bytef*)out+strlen(out),&len,(const Bytef*)header->data,(uLong)header->size+2);
					//printf("(len:%d==%d)\n----------------\n%s\n------------------------\n",header->unCompressedSize,strlen(out),out);
					//fflush(stdout);
					*outlen = header->unCompressedSize;

					return zipfile;
					//memset(out,0,len);
				}else if(header->data && strlen(fileName)==0){
					sprintf(out+strlen(out),"%s",header->fileName);
					sprintf(out+strlen(out),",%d\n",header->unCompressedSize);
					*outlen = strlen(out);
				}else{
					//sprintf(out+strlen(out),"no data!\n");
				}
				break;
			case 0x02014b50:
				bytearray->position-= 4;
				ZipDirHead_read(bytearray);
				break;
			case 0x06054b50:
				bytearray->position-= 4;
				ZipDirEnd_read(bytearray);
				break;
			default:
				printf("Error!");
				ZipFile_free(zipfile);
				return 0;
		}
	}
	return zipfile;
}

#ifdef test_zip
int main(int argc,char ** argv)
{
	printf("hello world!\n");

	char * filename;
	if(argc>1)
	{
		filename = argv[argc-1];
	}else{
		//filename = "/home/libiao/ExtractPDFText_src._zip";
		//filename = "/home/libiao/unzip-5.52/testmake.zip";
		//filename = "/home/libiao/test/as3_filemanager/bin/docx.docx";
		//filename = "/run/media/libiao/back/os/璀璨华星人.air";
		filename = "/run/media/libiao/back/os/_璀璨华星人.zip";
	}

	FILE * _file = fopen(filename,"rb");
	int fileLen = fseek(_file,0,SEEK_END);
	fileLen = ftell(_file);
	rewind(_file);
	printf("%s,%d\n",filename,fileLen);

	ByteArray * bytearray = ByteArray_new(fileLen);
	fread(bytearray->data,1,fileLen,_file);
	fclose(_file);

	char * out = malloc(fileLen*10);
	memset(out,0,fileLen*10);
	int outlen=0;
	ZipFile_free(ZipFile_parser(bytearray,"",out,&outlen));
	//printf("\n%s",out);

	//memset(out,0,fileLen*10);
	//ZipFile_free(ZipFile_parser(bytearray,"word/document.xml",out,&outlen)); printf("\n%s",out);

	return 0;
}
#endif


#ifdef as_api
#include "AS3.h"

//Method exposed to ActionScript
//Takes a String and echos it
static AS3_Val zipList(void* self, AS3_Val args)
{
	AS3_Val byteArray;
	AS3_Val string;
	AS3_ArrayValue(args, "AS3ValType,AS3ValType", &byteArray,&string);
	AS3_Val length = AS3_GetS(byteArray, "length");  

	//sztrace("length getted!");
	int len = AS3_IntValue(length);
	char *s =  AS3_StringValue(string);
	//if(len>0) return length;

	AS3_Release(length);

	char *data = malloc(len);
	memset(data,0,len);
	int fileLen = AS3_ByteArray_readBytes(data,byteArray, len);
	char *out= NULL;
	if(strlen(s)==0)
	{
		out= malloc(fileLen);
	}else{
		out= malloc(fileLen*10);
	}
	memset(out,0,fileLen);


	ByteArray * bytearray = ByteArray_new(fileLen);
	bytearray->data = data;


	int outlen;
	ZipFile_free(ZipFile_parser(bytearray,s,out,&outlen));
	ByteArray_free(bytearray);


	AS3_Val baNS = AS3_String("flash.utils");
	AS3_Val baClass = AS3_NSGetS(baNS, "ByteArray");
	AS3_Val emptyParams = AS3_Array("");
	AS3_Val ba = AS3_New(baClass, emptyParams);

	//sprintf(out+strlen(out),"[%d]%d,%d",fileLen,strlen(out),outlen);
	AS3_ByteArray_writeBytes(ba,out,outlen);

	free(out);
	AS3_Release(baNS);
	AS3_Release(baClass);
	AS3_Release(emptyParams);

	return ba;
}

int main()
{
	//define the methods exposed to ActionScript
	//typed as an ActionScript Function instance
	AS3_Val echoMethod = AS3_Function( NULL, zipList);

	// construct an object that holds references to the functions
	AS3_Val result = AS3_Object( "zipList: AS3ValType", echoMethod );

	// Release
	AS3_Release( echoMethod );

	// notify that we initialized -- THIS DOES NOT RETURN!
	AS3_LibInit( result );

	// should never get here!
	return 0;
}
#endif

