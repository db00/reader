/**
 * @file docx.c
 gcc -Wall myregex.c mystring.c regex.c -lm -D test_docx -D STDC_HEADERS docx.c zip.c bytearray.c inflate.c crc32.c uncompr.c adler32.c inffast.c inftrees.c zutil.c && ./a.out
 * @author db0@qq.com
 * @version 1.0.1
 * @date 2015-11-02
 */

#include "zip.h"
#include "myregex.h"


#ifdef test_docx
int main(int argc,char **argv)
{
	char * filename;
	if(argc>1)
	{
		filename = argv[argc-1];
	}else{
		filename = "RAR.docx";
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
	ZipFile * zipfile = ZipFile_parser(bytearray,"word/document.xml",out,&outlen);
	//printf("\n%s",out);

	//<w:p ·Ö¶Î
	char **matched_arr=(char**)malloc(strlen(out));
	memset(matched_arr,0,strlen(out));
	//<w:t>ÎÄ±¾</w:t>
	int len = regex_search_all(out,"/<w:t>([^<>]*)<\\/w:t>/",matched_arr);
	printf("%d\n",len);
	char * connected = string_concat(matched_arr,len,"");
	printf("%s",regex_replace_all(connected,"/<[\\/]*w:t>/",""));
	fflush(stdout);


	regex_matchedarrClear(matched_arr,len);

	ZipFile_free(zipfile);
	return 0;
}
#endif
