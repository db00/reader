/**
 * @file docx.c
 gcc -Wall myregex.c mystring.c regex.c -lm -D test_xlsx -D STDC_HEADERS xlsx.c zip.c bytearray.c inflate.c crc32.c uncompr.c adler32.c inffast.c inftrees.c zutil.c && ./a.out
 * @author db0@qq.com
 * @version 1.0.1
 * @date 2015-11-02
 */

#include "zip.h"
#include "myregex.h"


#ifdef test_xlsx
int main(int argc,char **argv)
{
	char * filename;
	if(argc>1)
	{
		filename = argv[argc-1];
	}else{
		filename = "xls.xlsx";
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
	//ZipFile_free(ZipFile_parser(bytearray,"",out,&outlen));

	ZipFile * zipfile = ZipFile_parser(bytearray,"xl/sharedStrings.xml",out,&outlen);
	printf("\n%s",out);
	ZipFile_free(zipfile);

	char **matched_arr=(char**)malloc(strlen(out));
	memset(matched_arr,0,strlen(out));
	//<w:t></w:t>
	int len = regex_search_all(out,"/<t>([^<>]*)<\\/t>/",matched_arr);
	printf("%d\n",len);
	char * connected = string_concat(matched_arr,len,"");
	printf("%s",regex_replace_all(connected,"/<[\\/]*w:t>/",""));
	fflush(stdout);
	
	memset(out,0,fileLen*10);
	// xl/worksheets/sheet1.xml
	char sheetName[64];
	memset(sheetName,0,64);
	int i=1;
	sprintf(sheetName,"xl/worksheets/sheet%d.xml",i);
	bytearray->position=0;
	ZipFile_free(ZipFile_parser(bytearray,sheetName,out,&outlen));
	printf("\n%s",out);
	return 0;


	regex_matchedarrClear(matched_arr,len);

	ZipFile_free(zipfile);
	return 0;
}
#endif
