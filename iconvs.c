/**
 *
 gcc iconvs.c && ./a.out
 */
#include "iconv.h"
#include "string.h"
#include "stdlib.h"
#include "stdio.h"

int main(){
	char * inbuf = "\xd6\xd0\xb9\xfa";
	int inlen=strlen(inbuf);
	iconv_t cd = iconv_open("UTF-8","UTF-16");
	//iconv_t cd = iconv_open("UTF-16","UTF-8");
	//iconv_t cd = iconv_open("GBK","UTF-8");
	char *outbuf = (char *)malloc(inlen*4+1);
	memset(outbuf,0,inlen*4+1);
	char *in = inbuf;
	char *out = outbuf;
	size_t outlen = inlen*4;
	iconv(cd, &in, (size_t *)&inlen, &out,&outlen);
	outlen = strlen(outbuf);
	//printf("%s,%s\n",inbuf,out);
	//printf("%s,%s,%s\n",inbuf,out,outbuf);
	printf("%s\n",outbuf);
	fflush(stdout);
	//free(outbuf);
	//return 0;
	iconv_close(cd); 
	return 0;
}
