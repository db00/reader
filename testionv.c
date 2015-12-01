/**
 * @file testionv.c
 gcc -Wall testionv.c && ./a.out
 This file code page is gbk ,
 *  
 * @author db0@qq.com
 * @version 1.0.1
 * @date 2015-12-01
 */
#include "iconv.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"

int main()
{    
	iconv_t conveter = iconv_open("gbk","UTF-8");
	if( conveter == (iconv_t)-1){
		return -1;    
	}

	const char *input = "hello中文";
	size_t insize = strlen(input);

	size_t outsize = insize * 3+1;
	char *output = malloc(outsize);
	memset(output,0,outsize);
	char *output_old = output;

	size_t ret = iconv(conveter,(char**)&input,&insize,(char**)&output_old,&outsize);
	if(ret==(size_t)-1){
		printf("converting failed\n");
	}

	printf("%s,%d",output,(int)outsize);
	free(output);
	iconv_close(conveter);
	return 0;
}
