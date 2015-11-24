#include "bytearray.h"

#ifdef LITTLE_ENDIAN
int little_endian= 1; 
#else
int little_endian= 0; 
#endif
 
int isLittle_endian(){
	struct byteOrder16{
		char a:4;
		char b:4;
	} b16;
	b16.a = 1;
	b16.b = 0;
	return (*((char*)&b16)==1);//else == 16
}

char ByteArray_readByte(ByteArray * bytearray)
{
	if(bytearray->position < bytearray->length)
	{
		char c = *(bytearray->data+bytearray->position);
		//printf("%x\n",c);
		bytearray->position++;
		return c;
	}
	return 0;
}

void ByteArray_readBytes(ByteArray * bytearray,int size,char * bytes)
{
	if(bytes && size >0 && size + bytearray->position <= bytearray->length)
	{
		memcpy(bytes,bytearray->data+bytearray->position,size);
		bytearray->position+=size;
	}
}

char * ByteArray_readLine(ByteArray * bytearray)
{
	int len = 0;
	while(bytearray->position + len < bytearray->length)
	{
		char c = *(bytearray->data+bytearray->position+len);
		/*
		   if(lineEnd && strlen(lineEnd) && strlen(lineEnd) <= len){
		   int lineEnd_len = strlen(lineEnd);
		   if(strncmp(lineEnd,bytearray->data+bytearray->position+len-lineEnd_len,lineEnd_len))
		   break;
		   }else 
		   */
		if(c == '\r' || c == '\n')
		{
			if(c=='\r' && *(bytearray->data+bytearray->position+len+1)=='\n')
				len++;

			len++;
			break;
		}
		len++;
	}
	//printf("(%d)",len);
	if(len>0)
	{
		char * ret = malloc(len+1);
		memset(ret,0,len+1);
		ByteArray_readBytes(bytearray,len,ret);
		//printf("%s",ret);
		return ret;
	}
	return 0;
}
unsigned short int ByteArray_readInt16(ByteArray * bytearray)
{
	unsigned short c = 0;
	if(bytearray->position+1 < bytearray->length)
	{
		if(little_endian){
			c = *((unsigned short*)(bytearray->data+bytearray->position));
		}else{
			c = *(bytearray->data+bytearray->position + 1)&0xff ;
			c += (*(bytearray->data+bytearray->position)&0xff) <<8 ;
		}
		bytearray->position+=2;
	}
	return c;
}
unsigned int ByteArray_readInt32(ByteArray * bytearray)
{
	if(bytearray->position+3 < bytearray->length)
	{
		unsigned int c;
		if(little_endian){
			c = *((int*)(bytearray->data+bytearray->position));
		}else{
			c = (*(bytearray->data+bytearray->position+3))&0xff ;
			c += (*(bytearray->data+bytearray->position+2)&0xff) << 8;
			c += (*(bytearray->data+bytearray->position+1)&0xff) << 16;
			c += (*(bytearray->data+bytearray->position)&0xff) << 24;
		}
		bytearray->position+=4;
		return c;
	}
	return 0;
}
int u16ToUtf8(unsigned short w,char * ret)
{
	if(w==0xa0)
		w = ' ';
	if(w)
	{
		int length = 0;
		if(w < (1<<7)){//1 byte
			if(w == 0x0d)//结束符号
			{
				ret[length++]='\n';
			}else if(w==2){
			}else if(isprint(w)){
				ret[length++]= w;
			}
		}else if(w < (1<<11)){// 2 bytes
			ret[length] = (3<<6);//2 bytes
			ret[length++] = (w>>6) & 0x1f;
			ret[length] = (w) & 0x3f;
			ret[length++] += ((1<<7) & 0xff);
		}else if(w < (1<<15)){// 3 bytes
			// 微软 / 5fae
			ret[length] += (7<<5);
			ret[length++] += ((w>>12) & 0xf);
			ret[length] = ((w>>6) & 0x3f);
			ret[length++] += ((1<<7) & 0xff);
			ret[length] = ((w) & 0x3f);
			ret[length++] += ((1<<7) & 0xff);
			/*
		}else if(w < (1<<16)){// 3 bytes
			// 微软 / 5fae
			ret[length] += (7<<5);
			ret[length++] += ((w>>12) & 0xf);
			ret[length] = ((w>>6) & 0x3f);
			ret[length++] += ((1<<7) & 0xff);
			ret[length] = ((w) & 0x3f);
			ret[length++] += ((1<<7) & 0xff);
		}else if(w < (1<<21)){// 4 bytes
			ret[length] = (15<<4);
			ret[length++] += (w >> 18) & 0x7;
			ret[length] = (w >> 12) & 0x3f;
			ret[length++] += ((1<<7) & 0xff);
			ret[length] = (w >> 6) & 0x3f;
			ret[length++] += ((1<<7) & 0xff);
			ret[length] = (w) & 0x3f;
			ret[length++] += ((1<<7) & 0xff);
		}else if(w < (1<<26)){// 5 bytes
			ret[length] = (31<<3);//5 bytes
			ret[length++] = (w >> 18) & 0x3f;
			ret[length] = (w >> 12) & 0x3f;
			ret[length++] += ((1<<7) & 0xff);
			ret[length] = (w >> 6) & 0x3f;
			ret[length++] += ((1<<7) & 0xff);
			ret[length] = (w) & 0x3f;
			ret[length++] += ((1<<7) & 0xff);
		}else if(w < (1<<31)){// 6 bytes
			ret[length] = (63<<2);//6 bytes
			ret[length++] += (w >> 30) & 0x1;
			ret[length] = (w >> 24) & 0x3f;
			ret[length++] += ((1<<7) & 0xff);
			ret[length] = (w >> 18) & 0x3f;
			ret[length++] += ((1<<7) & 0xff);
			ret[length] = (w >> 12) & 0x3f;
			ret[length++] += ((1<<7) & 0xff);
			ret[length] = (w >> 6) & 0x3f;
			ret[length++] += ((1<<7) & 0xff);
			ret[length] = (w) & 0x3f;
			ret[length++] += ((1<<7) & 0xff);
		}else{
		*/
		}
		ret[length++]='\0';
		return length;
	}
	return 0;
}
char * ByteArray_readUtf16(ByteArray * bytearray,int n)//need free
{
	if(bytearray==NULL || n<=0){
		return NULL;
	}
	int position = bytearray->position;

	char * ret = malloc(n*3+1);
	memset(ret,0,n*3+1);
	int num = 0;
	unsigned short c = ByteArray_readInt16(bytearray);
	while(c)
	{
		char z[8];
		memset(z,0,8);
		u16ToUtf8(c,(char*)z);
		//printf("%s",z);
		sprintf(ret+strlen(ret),"%s",z);
		c = ByteArray_readInt16(bytearray);
		while(c == 0x0d)//结束符号
		{
			sprintf(ret+strlen(ret),"\n");
			c = ByteArray_readInt16(bytearray);
			++num;
		}
		++num;
		if(num>n && n>0)break;
	}
	//printf("(%d)\n",num);
	bytearray->position = position+n*2;
	return ret;
}

int ByteArray_print16(ByteArray*bytearray,int n)
{
	if(bytearray==NULL || n<= 0 || bytearray->position+n*2>bytearray->length)
		return 0;
	int position = bytearray->position;
	char * ret = ByteArray_readUtf16(bytearray,n);
	if(ret){
		int length = strlen(ret);
		printf("%s\n",ret);
		free(ret);
		bytearray->position = position;
		return length;
	}
	bytearray->position = position;
	return 0;
}

char *ByteArray_readU8(ByteArray * bytearray,int n)//need free
{
	if(bytearray==NULL || bytearray->position>=bytearray->length)
		return 0;
	int len = n+1;
	char * ret = malloc(len);
	memset(ret,0,len);

	int i=0;
	while(i<n && bytearray->position < bytearray->length)
	{
		unsigned char c = ByteArray_readByte(bytearray);
		if(c == 0x0d)//结束符号
			c = '\n';
		unsigned short w = 0;
		switch(c)
		{
			case 0x82:
				w=0x201A;
				break;
			case 0x83:
				w=0x0192;
				break;
			case 0x84:
				w=0x201E;
				break;
			case 0x85:
				w=0x2026;
				break;
			case 0x86:
				w=0x2020;
				break;
			case 0x87:
				w=0x2021;
				break;
			case 0x88:
				w=0x02C6;
				break;
			case 0x89:
				w=0x2030;
				break;
			case 0x8A:
				w=0x0160;
				break;
			case 0x8B:
				w=0x2039;
				break;
			case 0x8C:
				w=0x0152;
				break;
			case 0x91:
				w=0x2018;
				break;
			case 0x92:
				w=0x2019;
				break;
			case 0x93:
				w=0x201C;
				break;
			case 0x94:
				w=0x201D;
				break;
			case 0x95:
				w=0x2022;
				break;
			case 0x96:
				w=0x2013;
				break;
			case 0x97:
				w=0x2014;
				break;
			case 0x98:
				w=0x02DC;
				break;
			case 0x99:
				w=0x2122;
				break;
			case 0x9A:
				w=0x0161;
				break;
			case 0x9B:
				w=0x203A;
				break;
			case 0x9C:
				w=0x0153;
				break;
			case 0x9F:
				w=0x0178;
				break;
			default:
				break;
		}
		if(w){
			char s[8];
			memset(ret,0,8);
			len += u16ToUtf8(w,(char*)s);
			ret = realloc(ret,len);
			sprintf(ret+strlen(ret),"%s",s);
		}else{
			//if(isprint(c))
			sprintf(ret+strlen(ret),"%c",c);
		}
		++i;
	}
	return ret;
}

int ByteArray_prints(ByteArray*bytearray,int num)
{
	if(bytearray==NULL || bytearray->position>=bytearray->length)
		return 0;
	char * p = bytearray->data + bytearray->position;
	char * ret = ByteArray_readU8(bytearray,num);
	if(num>0 && num<0x1000)
	{
		printf("%s",ret);
		free(ret);
	}else if(strlen(p)){
		printf("%s",p);
	}
	return 0;
}

void ByteArray_printx(ByteArray*bytearray,int num)
{
	int i = 0;
	char *bytes = NULL;
	if(num > 0){
		bytes = malloc(num);
		ByteArray_readBytes(bytearray,num,bytes);
	}
	while(i < num)
	{
		char c = bytes[i];
		printf("%x ",(char)c & 0xff);
		++i;
	}
	if(bytes)
		free(bytes);
}


ByteArray * ByteArray_new(int size)
{
	ByteArray * bytearray = malloc(sizeof(ByteArray));
	memset(bytearray,0,sizeof(ByteArray));
	if(size>0){
		bytearray->length = size;
		bytearray->data = malloc(size);
	}
	return bytearray;
}
void ByteArray_free(ByteArray*bytearray)
{
	if(bytearray)
	{
		if(bytearray->length>0)
			free(bytearray->data);
		free(bytearray);
	}
}
ByteArray*ByteArray_resize(ByteArray*bytearray,int size)
{
	if(size<=0)
	{
		ByteArray_free(bytearray);
		return NULL;
	}

	if(bytearray==NULL)
		bytearray = ByteArray_new(size);

	if(bytearray->length==0){
		bytearray->data = malloc(size);
	}else{
		bytearray->data = realloc(bytearray->data,size);
	}
	bytearray->length = size;
	return bytearray;
}
ByteArray * ByteArray_writeBytes(ByteArray*bytearray,char*bytes,int length)
{
	if(length<=0 || bytes==NULL)
		return bytearray;
	if(bytearray==NULL)
		bytearray = ByteArray_new(length);
	if(bytearray->position+length>bytearray->length){
		bytearray = ByteArray_resize(bytearray,bytearray->length+length);
	}
	memcpy(bytearray->data+bytearray->position,bytes,length);
	bytearray->position += length;
	return bytearray;
}
void ByteArray_rewind(ByteArray*bytearray)
{
	if(bytearray)
		bytearray->position=0;
}
