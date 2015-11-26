/**
 *
 gcc -Wall myregex.c mystring.c regex.c -lm -D _DEBUG_ -D STDC_HEADERS  && ./a.out
 gcc -I"../SDL2/include/" -Wall mystring.c dict.c myregex.c regex.c  -lSDL2 -lm -lSDL2 -Ddebug -DSTDC_HEADERS && ./a.out nude
 gcc -I"../SDL2/include/" -Wall mystring.c dict.c myregex.c regex.c  -lmingw32 -lSDL2main -lSDL2 -lm -lSDL2 -Ddebug -DSTDC_HEADERS && a nude
 gcc -Wall dict.c myregex.c regex.c -lm -D_DEBUG_ -DSTDC_HEADERS && a
 */

#include "myregex.h"

int regex_error(regex_t * re,int errno)
{/*{{{*/
	char errbuf [1024];
	//int len = 
	regerror(errno, re, errbuf, sizeof(errbuf));
	printf("error: %s\n", errbuf);
	regfree(re);
	free(re);
	return 0;
}/*}}}*/

static int regex_compile(regex_t * re ,const char *regString)
{/*{{{*/
	char * regstr = NULL;
	char * regEnd = NULL;
	int flag = REG_EXTENDED;
	if(*regString=='/'){
		regEnd = strrchr(regString,'/');
		int len = regEnd - (regString+1) ;
		int start = 1;
		regstr = getSubStr((char*)regString,start,len);

		char *flags = regEnd + 1;
		while(*flags){
			switch(*flags)
			{
				case 'i':
					flag |= REG_ICASE;
					break;
			}
			++flags;
		}
	}
	if(regstr == NULL){
		regstr = getSubStr((char*)regString,0,strlen(regString));
	}
	int err = regcomp(re, regstr, flag);
	free(regstr);
	return err;
}/*}}}*/

int regex_match(const char*s,const char*regString)
{/*{{{*/
	if(s==NULL || strlen(s)==0)
		return 0;
	int err;
	regex_t *re = malloc(sizeof(regex_t));            
	regmatch_t    subs [SUBSLEN];

	err = regex_compile(re,regString);
	if (err)
	{
		regex_error(re,err);
		re = NULL;
		return 0;
	}
	err = regexec(re, s, (size_t) SUBSLEN, subs, 0);
	if (err == REG_NOMATCH)
	{
		regex_error(re,err);
		re = NULL;
		return 0;
	}else if (err){  
		regex_error(re,err);
		re = NULL;
		return 0;
	}
	//printf("regex_match: %d\n", re->re_nsub);
	if(re){
		regfree(re);
		free(re);
	}
	return 1;
}/*}}}*/
/**
 *  
 *
 * @param s	the string to search
 * @param reg	the regex
 * @param callback	0:the first matched,
 * 					1:the first submatch,
 * 					2:the second submatch,
 * 					3:the third submatch,
 * 					...
 *
 * @return  
 */
char *regex_search(char * s,char * regString , int callback,int *dealed_len)
{
	size_t       len;
	regex_t *re = malloc(sizeof(regex_t));            
	regmatch_t    subs [SUBSLEN];
	int           err;
	if(dealed_len)*dealed_len = 0;

	err = regex_compile(re,regString);
	if (err)
	{
		regex_error(re,err);
		re = NULL;
		if(dealed_len) *dealed_len = strlen(s);
		return NULL;
	}

	//printf("re->re_nsub: %d\n", re->re_nsub);
	char *subsrc = s;
	if(dealed_len) subsrc += *dealed_len;
	err = regexec(re, subsrc, (size_t) SUBSLEN, subs, 0);
	if (err == REG_NOMATCH)
	{
		//printf("Sorry, no more match ...\n");
		if(dealed_len) *dealed_len = strlen(s);
		regex_error(re,err);
		re = NULL;
		return NULL;
	}else if (err){  
		regex_error(re,err);
		re = NULL;
		if(dealed_len) *dealed_len = strlen(s);
		return NULL;
	}

	char * matched = NULL;
	callback = (callback>re->re_nsub?re->re_nsub:callback);
	len = subs[callback].rm_eo - subs[callback].rm_so;
	if(len >0)
	{
		matched = getSubStr(subsrc,subs[callback].rm_so,len);
	}
	if(dealed_len) *dealed_len += subs[0].rm_eo;//当前匹配的结束地址
	if(re){
		regfree(re);
		free(re);
	}
	return matched;
}

/* -----------------------------------------------*/
/**
 *  replace the string with the call back search string array 
 *
 * @param str
 * @param str_arr
 * @param arr_len
 *
 * @return  
 */
char *regex_replace2(char * src,const char ** str_arr , int arr_len)
{
	size_t       len;
	regex_t *re = malloc(sizeof(regex_t));            
	regmatch_t    subs [SUBSLEN];
	int           err, i,pos=0;
	char          pattern [] = "\\$[0-9]{1,2}";
	char *ret=NULL;

	//printf("String	: %s\nlen:%d\n", src,strlen(src));
	//printf("Pattern	: %s \n", pattern);


	err = regcomp(re, pattern, REG_EXTENDED);
	if (err)
	{
		regex_error(re,err);
		re = NULL;
		return src;
	}

	//printf("re->re_nsub: %d\n", re->re_nsub);
	do{
		char *subsrc = src + pos;
		err = regexec(re, subsrc, (size_t) SUBSLEN, subs, 0);
		if (err == REG_NOMATCH)
		{
			regex_error(re,err);
			re = NULL;
			break;
		}else if (err){  
			regex_error(re,err);
			re = NULL;
			break;
		}

		len = subs[0].rm_eo - subs[0].rm_so;
		char * matched = getSubStr(subsrc,subs[0].rm_so,len);
		if(matched){
			i = atoi(matched+1);
			free(matched);
		}else{
			return src;
		}
		if(i<arr_len){
			char * replace_str = (char*)(str_arr[i]);
			//printf("[%d]=%s\n",i,replace_str);
			if(subs[0].rm_so > 0){
				char * tmp = getSubStr(subsrc,0,subs[0].rm_so);
				if(tmp)
					ret = append_str(ret,"%s",tmp);
				free(tmp);
			}
			ret = append_str(ret,"%s",replace_str);
			pos += subs[0].rm_eo;//当前匹配的结束地址
			//printf("cur pos:%d\n",pos);
		}else{
			return src;
		}
	}while(pos < strlen(src));

	if(strlen(src)>pos){
		ret = append_str(ret,"%s",src+pos);
	}
	return ret;
}
/**
 *  
 *
 * @param s
 * @param reg
 * @param replace_str	replace the matched string with the replace_str
 *
 * @return  
 */
char *regex_replace(char * s, const char * regString , const char * replace_str,int *dealed_len)
{
	size_t       len;
	regex_t *re = malloc(sizeof(regex_t));            
	regmatch_t    subs [SUBSLEN];
	int           err, i;
	if(dealed_len) *dealed_len = 0;
	int bufsize= 0;
	//printf("src:		%s\n",s);
	//printf("regString:	%s\n",regString);
	//printf("replace_str:	%s\n",replace_str);

	err = regex_compile(re,regString);
	if (err)
	{
		regex_error(re,err);
		re = NULL;
		if(dealed_len) *dealed_len = strlen(s);
		return s;
	}

	char *ret = NULL;
	do{
		char *subsrc = s;
		if(dealed_len)
			subsrc += *dealed_len;

		err = regexec(re, subsrc, (size_t) SUBSLEN, subs, 0);
		if (err == REG_NOMATCH){
			regex_error(re,err);
			re = NULL;
			break;
		}else if (err){  
			regex_error(re,err);
			re = NULL;
			if(ret){
				free(ret);
				ret = NULL;
			}
			if(dealed_len) *dealed_len = strlen(s);
			return s;
		}

		//printf("re_nsub:%d\n",re->re_nsub);
		char **matched_arr=(char**)malloc(sizeof(char**)*(re->re_nsub+1));    
		for (i = 0; i <= re->re_nsub; i++)
		{
			len = subs[i].rm_eo - subs[i].rm_so;
			matched_arr[i] = getSubStr(subsrc,subs[i].rm_so,len);
			//printf("matched_arr[%d]: %s\n",i, (char*)matched_arr[i]);
		}
		bufsize += (subs[0].rm_so+1);
		char * tmp = getSubStr(subsrc,0,subs[0].rm_so);
		if(tmp)
			ret = append_str(ret,"%s",tmp);
		free(tmp);
		char * copy_str = NULL;
		if(re->re_nsub>0 && regex_match(replace_str,"/\\$[0-9]{1,2}/")>0){//替换字符串中含有回调,回调号功能
			copy_str = regex_replace2((char*)replace_str, (const char**)matched_arr, re->re_nsub+1);
			//printf("copy_str:%s\n",copy_str);
			if(copy_str && strlen(copy_str)>0){
				ret = append_str(ret,"%s",copy_str);
				if(copy_str != replace_str){
					free(copy_str);
					copy_str = NULL;
				}
			}
		}else{
			ret = append_str(ret,"%s",(char*)replace_str);
		}
		if(matched_arr){
			for (i = 0; i <= re->re_nsub; i++)
				free(matched_arr[i]);
			free(matched_arr);matched_arr = NULL;
		}
		if(dealed_len)
			*dealed_len += subs[0].rm_eo;//当前匹配的结束地址
		else
			break;
	}while(*dealed_len < strlen(s));

	if(dealed_len){
		if(strlen(s)>(int)(*dealed_len)) 
			ret = append_str(ret,"%s", s + (*dealed_len));
	} else {
		ret = append_str(ret,"%s", s + subs[0].rm_eo);
	}

	if(re){
		regfree(re);
		free(re);
	}
	//printf("result:%s\n",ret);
	return ret;
}
int regex_search_all(char * s,char * reg , char **matched_arr)
{
	int num = 0;
	int dealed_len=0;
	int pos = 0;
	//printf("%s(%d)\n",s,strlen(s));
	//printf("%s\n",reg);
	while(pos<strlen(s))
	{
		char * p = s + pos;
		matched_arr[num] = regex_search(p,reg , 0, &dealed_len);
		if(matched_arr[num] && strlen(matched_arr[num])>0){
			//printf(" matched:%s,dealed_len:%d\n",matched_arr[num],pos);
			//free(matched_arr[num]); matched_arr[num]=NULL;
			num+=1;
		}else{
			break;
		}
		pos += dealed_len; 
		//printf("dealed_len:%d\n",pos);
	}

	return num;
}

char *regex_replace_all(char * src, const char * reg , const char * replace_str)
{
	int dealed_len=0;
	return  regex_replace(src,reg,replace_str,&dealed_len);
}

void regex_matchedarrClear(char ** matched_arr,int len)
{
	int i;
	for(i=0;i<len;++i) {
		if(matched_arr[i])
		{
			free(matched_arr[i]);
			matched_arr[i] = 0;
		}
	}
	if(matched_arr)
		free(matched_arr);
}

#ifdef _DEBUG_
int main()
{
	printf("%d\n",regex_match("tests","/TESTS/i"));

	size_t       len;
	regmatch_t    subs [SUBSLEN];
	int           err, i,pos=0;
	char          src    [] = "++<title>12345+9876</title>";

	//printf("%s\n",regex_replace_all(src,"/<([^<>]+)>/im",""));

	printf("\ntest regex_search_all:\n");
	char **matched_arr=(char**)malloc(strlen(src));
	memset(matched_arr,0,strlen(src));
	len = regex_search_all(src,"/<([^<>]*)>/",matched_arr);
	printf("%d\n",len);
	int d=0;
	while(d<len)
	{
		printf("%s",matched_arr[d]);
		++d;
	}
	regex_matchedarrClear(matched_arr,len);

	printf("\ntest regex_search:\n");
	int dealed_len=0;
	char *replaced = regex_search(src,"/([0-9]+)/",0,&dealed_len);
	printf("%s,%d",replaced,dealed_len);
	if(src != replaced)free(replaced);//free
	printf("\n-----------\n");

	printf("\ntest regex_replace:\n");
	replaced = regex_replace(src,"/([0-9]+)/","/($1)/",&dealed_len);
	printf("regex_replace:%s,%d",replaced,dealed_len);
	if(src != replaced)free(replaced);//free
	printf("\n-----------\n");


	regex_t *re = malloc(sizeof(regex_t));            
	regex_compile(re,"/(\\+)/");
	//printf("re->re_nsub: %d\n", re->re_nsub);
	do{
		char *subsrc = src+pos;
		err = regexec(re, subsrc, (size_t) SUBSLEN, subs, 0);
		if (err == REG_NOMATCH)
		{
			regex_error(re,err);
			re = NULL;
			break;
		}else if (err){  
			regex_error(re,err);
			re = NULL;
			break;
		}

		for (i = 0; i <= re->re_nsub; i++)
		{
			len = subs[i].rm_eo - subs[i].rm_so;
			if (i == 0)
			{//当前匹配
				printf ("first match : %d -> %d , len : %d , ",pos + subs[i].rm_so,pos + subs[i].rm_eo,len);
			}
			else
			{//子匹配
				printf("sub match %d : %d -> %d , len : %d , ", i, pos + subs[i].rm_so,pos + subs[i].rm_eo, len);
			}

			char * matched = getSubStr(subsrc,subs[i].rm_so,len);
			printf("match: %s\n", matched);
			if(matched)free(matched);
		}
		pos += subs[0].rm_eo;//当前匹配的结束地址
		//printf("cur pos:%d\n",pos);
	}while(pos < strlen(src));

	if(re){
		regfree(re);
		free(re);
	}
	return (0);
}
#endif
