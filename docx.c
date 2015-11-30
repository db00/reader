/**
 * @file docx.c
 gcc -lxml2 -I"xml2" -Wall myregex.c mystring.c regex.c -lm -D test_docx -D STDC_HEADERS docx.c zip.c bytearray.c inflate.c crc32.c uncompr.c adler32.c inffast.c inftrees.c zutil.c && ./a.out
 * @author db0@qq.com
 * @version 1.0.1
 * @date 2015-11-02
 */

#include "zip.h"
#include "myregex.h"

#include <libxml/parser.h>
#include <libxml/tree.h>
#include <libxml/xpath.h>
#include <libxml/xpathInternals.h>

xmlXPathObjectPtr get_nodeset(xmlDocPtr doc, const xmlChar *xpath)
{
	xmlXPathContextPtr context;
	xmlXPathObjectPtr result;
	context = xmlXPathNewContext(doc);
	if (context == NULL)
	{
		printf("context is NULL\n");
		return NULL;
	}
	xmlXPathRegisterNs(context,"w","http://schemas.openxmlformats.org/wordprocessingml/2006/main");//  //默认ns :   @"//*[local-name()='price'] " 
	result = xmlXPathEvalExpression(xpath, context);
	xmlXPathFreeContext(context);
	if (result == NULL)
	{
		printf("xmlXPathEvalExpression return NULL\n");
		return NULL;
	}
	if (xmlXPathNodeSetIsEmpty(result->nodesetval))
	{
		xmlXPathFreeObject(result);
		printf("nodeset is empty\n");
		return NULL;
	}
	return result;
}


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

	/*
	//<w:p 分段
	char **matched_arr=(char**)malloc(strlen(out));
	memset(matched_arr,0,strlen(out));
	//<w:t>文本</w:t>
	int len = regex_search_all(out,"/<w:t>([^<>]*)<\\/w:t>/",matched_arr);
	printf("%d\n",len);
	char * connected = string_concat(matched_arr,len,"");
	printf("%s",regex_replace_all(connected,"/<[\\/]*w:t>/",""));
	fflush(stdout);
	regex_matchedarrClear(matched_arr,len);
	*/


	xmlDocPtr doc = xmlParseMemory(out, strlen(out));
	//for(i=0;i<MAXNS;i++) xmlXPathRegisterNs(context,NSNAME[i],NS[i]);//  //默认ns :   @"//*[local-name()='price'] " 
	/*检查解析文档是否成功，如果不成功，libxml将指一个注册的错误并停止。一个常见错误是不适当的编码。XML标准文档除了用UTF-8或UTF-16外还可用其它编码保存。如果文档是这样，libxml将自动地为你转换到UTF-8。更多关于XML编码信息包含在XML标准中。*/
	if (doc == NULL )
	{
		fprintf(stderr,"Document not parsed successfully. \n");
		return -1;
	}

	xmlChar *xpath = ("//w:p");
	xmlXPathObjectPtr app_result = get_nodeset(doc,xpath);
	if (app_result == NULL)
	{
		printf("app_result is NULL\n");
		return -4;
	}
	int i = 0;
	xmlChar *value;
	if(app_result)
	{
		xmlNodeSetPtr nodeset = app_result->nodesetval;
		for (i=0; i < nodeset->nodeNr; i++)
		{
			xmlNodePtr cur;  //定义结点指针(你需要它为了在各个结点间移动)
			cur = nodeset->nodeTab[i];   
			//printf("name: %s\n", (char *)cur->name);
			cur = cur->xmlChildrenNode; 
			while(cur!=NULL)
			{  
				value = xmlNodeGetContent(cur);
				if (value != NULL)
				{
					printf("%s", (char *)value);
					xmlFree(value);
				}
				cur = cur->next;
			}
			printf("\r\n");
		}
		printf("\r\n");
		xmlXPathFreeObject (app_result);
	}

	xmlFreeDoc(doc); 



	ZipFile_free(zipfile);
	return 0;
}
#endif
