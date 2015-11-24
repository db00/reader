/**
 * @file pdf.c
 gcc -I"include" -D debug_pdf bytearray.c pdf.c adler32.c inftrees.c inffast.c crc32.c zutil.c inflate.c && ./a.out
 * @author db0@qq.com
 * @version 1.0.1
 * @date 2015-10-29
 */
#include "zlib.h"
#include "pdf.h"


//Keep this many previous recent characters for back reference:
#define oldchar 15

//Convert a recent set of characters into a number if there is one.
//Otherwise return -1:
float ExtractNumber(const char* search, int lastcharoffset)
{/*{{{*/
	int i = lastcharoffset;
	while (i>0 && search[i]==' ') i--;
	while (i>0 && (isdigit(search[i]) || search[i]=='.')) i--;
	float flt=-1.0;
	char buffer[oldchar+5];
	memset(buffer,0,sizeof(buffer));
	strncpy(buffer, search+i+1, lastcharoffset-i);
	if (buffer[0] && sscanf(buffer, "%f", &flt))
	{
		return flt;
	}
	return -1.0;
}/*}}}*/

//Check if a certain 2 character token just came along (e.g. BT):
int seen2(const char* search, char* recent)
{/*{{{*/
	if (    recent[oldchar-3]==search[0] 
			&& recent[oldchar-2]==search[1] 
			&& (recent[oldchar-1]==' ' || recent[oldchar-1]==0x0d || recent[oldchar-1]==0x0a) 
			&& (recent[oldchar-4]==' ' || recent[oldchar-4]==0x0d || recent[oldchar-4]==0x0a)
	   )
	{
		return 1;
	}
	return 0;
}/*}}}*/

//This method processes an uncompressed Adobe (text) object and extracts text.
void ProcessOutput(FILE* file, char* output, size_t len)
{/*{{{*/
	//Are we currently inside a text object?
	int intextobject = 0;

	//Is the next character literal (e.g. \\ to get a \ character or \( to get ( ):
	int nextliteral = 0;

	//() Bracket nesting level. Text appears inside ()
	int rbdepth = 0;

	//Keep previous chars to get extract numbers etc.:
	char oc[oldchar];
	int j=0;
	for (j=0; j<oldchar; j++) oc[j]=' ';

	for (size_t i=0; i<len; i++)
	{
		unsigned char c = output[i];
		if (intextobject)
		{
			if (rbdepth==0 && seen2("TD", oc))
			{
				//Positioning.
				//See if a new line has to start or just a tab:
				float num = ExtractNumber(oc,oldchar-5);
				if (num>1.0)
				{
					fputc(0x0d, file);
					fputc(0x0a, file);
				}
				if (num<1.0)
				{
					fputc('\t', file);
				}
			}
			if (rbdepth==0 && seen2("ET", oc))
			{
				//End of a text object, also go to a new line.
				intextobject = 0;
				fputc(0x0d, file);
				fputc(0x0a, file);
			}
			else if (c=='(' && rbdepth==0 && !nextliteral) 
			{
				//Start outputting text!
				rbdepth=1;
				//See if a space or tab (>1000) is called for by looking
				//at the number in front of (
				int num = ExtractNumber(oc,oldchar-1);
				if (num>0)
				{
					if (num>1000.0)
					{
						fputc('\t', file);
					}
					else if (num>100.0)
					{
						fputc(' ', file);
					}
				}
			}
			else if (c==')' && rbdepth==1 && !nextliteral) 
			{
				//Stop outputting text
				rbdepth=0;
			}
			else if (rbdepth==1) 
			{
				//Just a normal text character:
				if (c=='\\' && !nextliteral)
				{
					//Only print out next character no matter what. Do not interpret.
					nextliteral = 1;
				}
				else
				{
					nextliteral = 0;
					if ( ((c>=' ') && (c<='~')) || ((c>=128) && (c<255)) )
					{
						fputc(c, file);
					}
				}
			}
		}
		//Store the recent characters for when we have to go back for a number:
		for (j=0; j<oldchar-1; j++) oc[j]=oc[j+1];
		oc[oldchar-1]=c;
		if (!intextobject)
		{
			if (seen2("BT", oc))
			{
				//Start of a text object:
				intextobject = 1;
			}
		}
	}
}/*}}}*/






int getNumChars(char * string,char sign)
{
	int num = 0;
	if(string)
	{
		int len = strlen(string);
		int i=0;
		while(i<len)
		{
			if(string[i]==sign)
				num++;
			++i;
		}
	}
	return num;
}

typedef struct PdfObj{
	int id;
	int modify;
	int length;
	char * properties;
	char * data;

	int numChildren;
	struct PdfObj ** children;
	struct PdfObj * parent;
}PdfObj;

typedef struct PdfTrailer{
	int size;
	PdfObj *root;
	PdfObj *info;
	char * ID;
}PdfTrailer;
typedef struct PdfHeader{
	char * version;
}PdfHeader;

typedef struct PdfXref{
	int start;
	int num;
	int * offsets;
}PdfXref;
typedef struct PdfFile{
	ByteArray *bytearray;
	PdfHeader * header;
	PdfXref * xref;
	PdfObj ** objs;
	int numObjs;
	PdfTrailer * trailer;
}PdfFile;
PdfTrailer * PdfTrailer_read(PdfFile * file,ByteArray * bytearray);
void startline(PdfFile *,ByteArray * bytearray);

PdfObj * PdfFile_getObjById(PdfFile * file,int id)
{
	if(file->objs)
	{
		int i = 0;
		while(i<file->numObjs)
		{
			PdfObj * obj = file->objs[i];
			if(obj && obj->id==id){
				return obj;
			}
			++i;
		}
	}
	return NULL;
}
void PdfXref_free(PdfXref * xref)
{
	if(xref)
	{
		free(xref);
	}
}
PdfXref * PdfXref_read(ByteArray * bytearray)
{
	PdfXref * pdfxref = malloc(sizeof(PdfXref));
	memset(pdfxref,0,sizeof(PdfXref));
	char * line = ByteArray_readLine(bytearray);
	printf("line:%s,",line);
	pdfxref->start = atoi(line);
	printf("start:%d,",pdfxref->start);
	int id = pdfxref->start;
	int len = 1;
	while(id >10){
		len++;
		id /= 10;
	}
	pdfxref->num = atoi(line+len);
	printf("num:%d,",pdfxref->num);
	free(line);
	int i = pdfxref->start;
	while(i<pdfxref->num)
	{
		line = ByteArray_readLine(bytearray);
		//printf("%s",line);
		free(line);
		++i;
	}
	return pdfxref;
}
PdfTrailer * PdfTrailer_read(PdfFile * file,ByteArray * bytearray)
{
	PdfTrailer * trailer = malloc(sizeof(*trailer));
	memset(trailer,0,sizeof(*trailer));
	char * line = NULL;
	int numL = 0;
	int numR = 0;
	while(numL-numR || numL==0)
	{
		if(line)free(line);
		line = ByteArray_readLine(bytearray);
		numL += getNumChars(line,'<');
		numR += getNumChars(line,'>');
		//printf("%s",line);
		char * per = NULL;

		if(trailer->size == 0)
		{
			per = strstr(line,"/Size ");
			if(per) {
				trailer->size = atoi(per+6);
				printf("size:%d,",trailer->size);
			}
		}

		if(trailer->root == NULL)
		{
			per = strstr(line,"/Root ");
			if(per) {
				int rootId = atoi(per+6);
				printf("rootId:%d,",rootId);
				trailer->root = PdfFile_getObjById(file,rootId);
				if(trailer->root)
				{
					char * s = trailer->root->properties;
					printf("\nroot obj:\n%s\n",s);
					char *p = strstr(s,"/Pages ");
					if(p)
					{
						PdfObj * pageObj = PdfFile_getObjById(file,atoi(p+7));
						if(pageObj)
						{
							char* properties = pageObj->properties;
							printf("pages found! %s\n",properties);
						}
					}
				}
			}
		}

		if(trailer->info==NULL)
		{
			per = strstr(line,"/Info ");
			if(per) {
				int infoId = atoi(per+6);
				printf("infoId:%d,",infoId);
				trailer->info = PdfFile_getObjById(file,infoId);
				if(trailer->info)
				{
					printf("info obj found!\n");
				}
			}
		}
		if(trailer->ID==NULL)
		{
			per = strstr(line,"/ID[< ");
			if(per) {
				trailer->ID = per+6;
				if(trailer->ID)
				{
					printf("ID:%s,",trailer->ID);
				}
			}
		}
	}
	printf("\n");
	//printf("%s",line);
	free(line);
	return trailer;
}
void PdfTrailer_free(PdfTrailer * trailer)
{
	if(trailer)
	{
		free(trailer);
	}
}

void PdfObj_free(PdfObj * obj)
{
	if(obj)
	{
		if(obj->data) free(obj->data);
		if(obj->properties) free(obj->properties);
		free(obj);
	}
}


void Text_printf(char * data,int length,char * out,int outsize)
{
	z_stream zstrm;
	memset(&zstrm, 0,sizeof(zstrm));

	zstrm.avail_in = length + 1;
	zstrm.avail_out = outsize;
	zstrm.next_in = (Bytef*)data;
	zstrm.next_out = (Bytef*)out;

	int rsti = inflateInit(&zstrm);
	if (rsti == Z_OK)
	{
		int rst2 = inflate (&zstrm, Z_FINISH);
		if (rst2 >= 0)
		{
			size_t totout = zstrm.total_out;
			ProcessOutput(stdout, out, totout);
		}else{
			printf("error\n");
		}
	}else{
		printf("error\n");
	}
}

PdfObj * PdfObj_read(PdfFile * file,char *line)
{
	ByteArray * bytearray = file->bytearray;
	PdfObj * obj = malloc(sizeof(*obj));
	memset(obj,0,sizeof(*obj));

	obj->id = atoi(line);
	printf("id:%d,",obj->id);

	if(file->numObjs < obj->id)
	{
		if(file->objs==NULL)
		{
			file->objs = malloc(obj->id*sizeof(PdfObj*));
			memset(file->objs,0,obj->id*sizeof(PdfObj*));
		}else{
			int len = obj->id*sizeof(PdfObj*);
			file->objs = realloc(file->objs,len);
			int dealed = file->numObjs*sizeof(PdfObj*);
			memset((char*)file->objs+dealed,0,len-dealed);
		}
		file->numObjs = obj->id;
	}
	file->objs[obj->id-1] = obj;



	int position = bytearray->position - strlen(line);
	int numL = getNumChars(line,'<');
	int numR = getNumChars(line,'>');

	while(numL-numR || numL==0)
	{
		if(line)free(line);line = NULL;
		line = ByteArray_readLine(bytearray);
		numL += getNumChars(line,'<');
		numR += getNumChars(line,'>');
	}

	int propertiesLen = bytearray->position - position;
	obj->properties = malloc(propertiesLen);
	bytearray->position = position;
	ByteArray_readBytes(bytearray,propertiesLen,obj->properties);

	if(obj->length==0){
		char * length = strstr(obj->properties,"/Length ");
		if(length){
			obj->length = atoi(length+strlen("/Length "));
			printf("(%d)",obj->length);
		}
	}

	if(obj->length>0)
	{
		if((line+strlen(line)-strrchr(line,'>'))<3)
		{
			if(line)free(line);line=NULL;
			line = ByteArray_readLine(bytearray);
		}

		obj->data = malloc(obj->length);
		ByteArray_readBytes(bytearray,obj->length,obj->data);
		//printf("data:%s,",obj->data);
		if(line)free(line);line=NULL;
		line = ByteArray_readLine(bytearray);
		while(strlen(line)<3){
			//printf("line:%s,",line);
			if(line)free(line);line = NULL;
			line = ByteArray_readLine(bytearray);
		}
		if(line && strncmp(line,"endstream",9)){
			printf("\nXXXXXXXXXXXXxx type_end:%s",line);
		}
	}
	if(line)free(line);line = NULL;
	line = (ByteArray_readLine(bytearray));
	if(strncmp(line,"endobj",6)){
		printf("\nXXXXXXXXXXXXxx endobj:%s",line);
	}
	//printf("endobj:%s",line);
	free(line);
	//printf("\n----------------------------------------,\n");

	if(obj->data)
	{
		size_t outsize = obj->length*10;
		char out[outsize];
		memset(out, 0,outsize);
		Text_printf(obj->data,obj->length,out,outsize);
	}

	return obj;
}

void PdfHeader_free(PdfHeader * header)
{
	if(header)
	{
		if(header->version)
			free(header->version);
		free(header);
	}
}
PdfHeader * PdfHeader_read(ByteArray * bytearray)
{
	PdfHeader * header = malloc(sizeof(*header));
	memset(header,0,sizeof(*header));
	header->version = ByteArray_readLine(bytearray);
	if(strncmp(header->version,"%PDF-1.",7))
	{
		printf("not a pdf file!\n");
		PdfHeader_free(header);
		return 0;
	}
	free(ByteArray_readLine(bytearray));
	//printf("--------------------");

	fflush(stdout);
	return header;
}
void PdfFile_free(PdfFile * file)
{
	if(file)
	{
		if(file->header)
			PdfHeader_free(file->header);
		if(file->objs)
		{
			int i=0;
			while(i<file->numObjs)
			{
				if(file->objs[i])
				{
					//printf("0x%x,",(int)file->objs[i]);
					PdfObj_free(file->objs[i]);
				}
				++i;
			}
			free(file->objs);
		}
		if(file->xref)
			PdfXref_free(file->xref);
		if(file->trailer)
			PdfTrailer_free(file->trailer);
		free(file);
	}
}

void startline(PdfFile * file,ByteArray * bytearray)
{
	char * line = ByteArray_readLine(bytearray);
	if(strstr(line," obj"))
	{
		PdfObj_read(file,line);
		line = NULL;
	}else if(strncmp(line,"xref",4)==0){
		printf("xref!\n");
		PdfXref_read(bytearray);
	}else if(strncmp(line,"trailer",7)==0){
		printf("trailer!\n");
		PdfTrailer_read(file,bytearray);
	}else if(strncmp(line,"startxref",7)==0){
		printf("startxref!\n");
		free(line);
		line = ByteArray_readLine(bytearray);
		printf("%d,",atoi(line));
	}else if(strncmp(line,"%%EOF",5)==0){
		printf("\nfile end!\n");
		//free(line); line = NULL;
	}
	if(line){
		free(line);
	}
	if(bytearray->position < bytearray->length) startline(file,bytearray);
}
PdfFile * PdfFile_parse(ByteArray * bytearray,char * out)
{
	PdfFile * file = malloc(sizeof(PdfFile));
	memset(file,0,sizeof(PdfFile));
	file->bytearray = bytearray;
	file->header = PdfHeader_read(bytearray);
	if(file->header==NULL)
	{
		PdfFile_free(file);
		return 0;
	}
	startline(file,bytearray);
	return file;
}



#ifdef debug_pdf
int main(int argc,char**argv)
{
	char * filename;
	if(argc>1)
	{
		filename = argv[argc-1];
	}else{
		filename = "pdf.pdf";
	}

	FILE * _file = fopen(filename,"rb");
	int fileLen = fseek(_file,0,SEEK_END);
	fileLen = ftell(_file);
	rewind(_file);
	printf("%s,%d\n",filename,fileLen);

	ByteArray * bytearray = ByteArray_new(fileLen);
	fread(bytearray->data,1,fileLen,_file);
	fclose(_file);

	char * out = malloc(fileLen);
	memset(out,0,fileLen);
	PdfFile_free(PdfFile_parse(bytearray,out));
	printf("%s\n",out);
	return 0;
}
#endif


#ifdef as_api
#include "AS3.h"

//Method exposed to ActionScript
//Takes a String and echos it
static AS3_Val echo(void* self, AS3_Val args)
{
	AS3_Val byteArray;
	AS3_ArrayValue( args, "AS3ValType", &byteArray);
	AS3_Val length = AS3_GetS(byteArray, "length");  

	sztrace("length getted!");
	int len = AS3_IntValue(length);
	//if(len>0) return length;

	char *data=NULL;
	data=malloc(len);
	memset(data,0,len);
	int fileLen = AS3_ByteArray_readBytes(data,byteArray, len);
	char *out= NULL;
	out = malloc(fileLen);
	memset(out,0,fileLen);



	ByteArray * bytearray = ByteArray_new(fileLen);
	bytearray->data = data;

	PdfFile_free(PdfFile_parse(bytearray,out));
	ByteArray_free(bytearray);

	return AS3_String(out);
}

int main()
{
	//define the methods exposed to ActionScript
	//typed as an ActionScript Function instance
	AS3_Val echoMethod = AS3_Function( NULL, echo );

	// construct an object that holds references to the functions
	AS3_Val result = AS3_Object( "echo: AS3ValType", echoMethod );

	// Release
	AS3_Release( echoMethod );

	// notify that we initialized -- THIS DOES NOT RETURN!
	AS3_LibInit( result );

	// should never get here!
	return 0;
}
#endif

