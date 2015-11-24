/**
 *
 gcc date.c mystring.c -D debug_times && ./a.out 
 gcc date.c mystring.c -D debug_times && a 
 */

#include "date.h"

time_t Date_getDaysFrom1970(Date * date)
{
	int sum = 0;
	if(date){
		printf("\nLocal time is: %s\t",asctime(date));  
		printf("%d-",1900+date->tm_year);
		printf("%d-",1+date->tm_mon);
		printf("%d ",+date->tm_mday);
		printf("%d:",date->tm_hour-date->tm_isdst);
		printf("%d:",date->tm_min);
		printf("%d ",date->tm_sec);
		printf("wday:%d,",date->tm_wday);
		printf("day in Year:%d\n",date->tm_yday);
		//printf("isdst:%d,",date->tm_isdst);
		sum = Date_value(date)/(24*3600);
	}
	printf("%d\n",sum);
	return sum;
}

Date * Date_newFrom1970(time_t days)// 1900.1.1=-25569;1904.1.2=-24106
{
	Date * date = NULL;
	if(-24855<=days && days <= 24855){//from 1901.12.14 to 2038.1.19
		time_t t = days*24*3600;
		return Date_new(&t);
	}
	return date;
}

Date*Date_now()// 返回一个当前时间的数据结构
{
	time_t timer = Date_getTime();
	Date* date = localtime(&timer);
	return date;
}

time_t Date_getTime()// 按照通用时间返回 Date 对象中自 1970 年 1 月 1 日午夜以来的毫秒数。 Date 
{
	return time(NULL);
}

double Date_getTimer()
{
	return (double)clock()/CLOCKS_PER_SEC;
}

Date*Date_new(time_t *t)//根据秒数来设定时间
{
	Date *_date;
	Date * date = (Date*) malloc(sizeof(Date));
	memset(date,0,sizeof(Date));
	if(t){
		_date = gmtime(t);
		//_date = localtime(t);
	}else{
		_date = Date_now();
	}
	date->tm_year = _date->tm_year;
	date->tm_mon= _date->tm_mon;
	date->tm_mday= _date->tm_mday;
	date->tm_hour = _date->tm_hour-_date->tm_isdst;
	date->tm_min= _date->tm_min;
	date->tm_sec= _date->tm_sec;
	date->tm_wday= _date->tm_wday;
	date->tm_yday= _date->tm_yday;
	date->tm_isdst= 0;
	return date;
}

time_t Date_value(Date*date)// 按照通用时间返回 Date 对象中自 1970 年 1 月 1 日午夜以来的毫秒数。 
{
	return mktime(date);
}

Date*Date_new2(int tm_year,int tm_mon,int tm_mday,int tm_hour,int tm_min,int tm_sec)//根据年,月,日,时,分,秒来设定时间
{
	time_t t = 0;
	tm_year -= 1900;
	tm_mon -= 1;
	if(tm_year>=70 && tm_year<=138){
		Date * date = (Date*) malloc(sizeof(Date));
		memset(date,0,sizeof(Date));
		date->tm_year = tm_year;
		date->tm_mon= tm_mon;
		date->tm_mday= tm_mday;
		date->tm_hour = tm_hour;
		date->tm_min= tm_min;
		date->tm_sec= tm_sec;
		//date->tm_wday= 3;
		//date->tm_yday= tm_yday;
		//date->tm_isdst= 0;
		t = Date_value(date);
		if(date)free(date);
	}
	return Date_new(&t);
}

double Date_diff(Date* date1,Date* date2)//两个日期相差的毫秒树
{
	time_t t1 = Date_value(date1);
	time_t t2 = Date_value(date2);
	printf("t1:%ld t2:%ld\n",t1,t2);
	return difftime(t1,t2);
}

size_t Date_foramtout(char*str,size_t maxsize,const char*format,const Date* date)
{
	//return strftime(str,maxsize,"It is now %I %p",date);  
	return strftime(str,maxsize,format,date);  
}
/*
   January ['dʒænjuəri] 
   February ['februəri] 
   March [mɑ:tʃ] 
   April ['eiprəl] 
   May [mei] 
   June [dʒu:n] 
   July [dʒu:'lai] 
   August ['ɔ:ɡəst] 
   September [sep'tembə] 
   October [ɔk'təubə] 
   November [nəu'vembə] 
   December [di'sembə]
   */
Date * Date_fromString(char*_s)
{
	//"Thu, 27 Aug 2015 16:06:52 +0800"
	Date *date = NULL;
	int len;
	char**arr = string_split(_s," ",&len);
	if(len==6)
	{
		int i = 0;
		while(i<len)
		{
			printf("%d:%s:%d\n",i,arr[i],(int)strlen(arr[i]));
			++i;
		}
		int timelen = 0;
		char**time_arr = string_split(arr[4],":",&timelen);
		if(timelen == 3){
			int mon = 0;
			if(strncasecmp(arr[2],"January",strlen(arr[2]))==0){
				mon = 1;
			}else if(strncasecmp(arr[2],"February",strlen(arr[2]))==0){
				mon = 2;
			}else if(strncasecmp(arr[2],"March",strlen(arr[2]))==0){
				mon = 3;
			}else if(strncasecmp(arr[2],"April",strlen(arr[2]))==0){
				mon = 4;
			}else if(strncasecmp(arr[2],"May",strlen(arr[2]))==0){
				mon = 5;
			}else if(strncasecmp(arr[2],"June",strlen(arr[2]))==0){
				mon = 6;
			}else if(strncasecmp(arr[2],"July",strlen(arr[2]))==0){
				mon = 7;
			}else if(strncasecmp(arr[2],"August",strlen(arr[2]))==0){
				mon = 8;
			}else if(strncasecmp(arr[2],"September",strlen(arr[2]))==0){
				mon = 9;
			}else if(strncasecmp(arr[2],"October",strlen(arr[2]))==0){
				mon = 10;
			}else if(strncasecmp(arr[2],"November",strlen(arr[2]))==0){
				mon = 11;
			}else if(strncasecmp(arr[2],"December",strlen(arr[2]))==0){
				mon = 12;
			}
			printf("contact:%s\n",string_concat(arr,len,","));
			date = Date_new2(atoi(arr[3]),mon,atoi(arr[1]),atoi(time_arr[0]),atoi(time_arr[1]),atoi(time_arr[2]));
			printf("tm_hour:%d isdst:%d\n",date->tm_hour,date->tm_isdst);
			free(time_arr[0]);
			free(time_arr);
		}
		free(arr[0]);
		free(arr);
	}
	return date;
}

#ifdef debug_times
int main(int argc, char *argv[])
{
	/*
	   time_t timer;  
	   timer=time(NULL);  

	   Date *date = Date_now();  
	   printf("Local time is: %s\n",asctime(date));  
	   printf("Years:%d\n",1900+date->tm_year);
	   printf("month:%d\n",1+date->tm_mon);
	   printf("date:%d\n",date->tm_mday);
	   printf("Hours:%d\n",date->tm_hour);
	   printf("Minutes:%d\n",date->tm_min);
	   printf("Seconds:%d\n",date->tm_sec);
	   printf("wday:%d\n",date->tm_wday);
	   printf("day in Year:%d\n",date->tm_yday);
	   printf("isdst:%d\n",date->tm_isdst);
	   printf("\n");
	//delay(2000);
	printf("ctime is: %s\n",ctime(&timer));  
	printf("GMT is:%s\n", asctime(gmtime(&timer)));  
	printf("The number of seconds since January 1,1970 is %ld\n",time(NULL));  
	printf("The number of seconds since January 1,1970 is %ld\n",Date_value(Date_now()));  
	time_t t=0;
	Date *date1 = Date_new(NULL);
	Date *date2 = Date_new(&t);
	printf("Date_new(NULL) is:%s\n", asctime(date1));  
	printf("Date_new(&t) (t=0) is:%s\n", asctime(date2));  
	printf("Date_diff(now,1970) is:%f\n", Date_diff(date1,date2));  

	printf("Date_new2(1971,1,1,0,0,0) is:%s\n", asctime(Date_new2(1971,1,1,0,0,0)));  


	time_t td;  
	putenv("TZ=PST8PDT");  
	tzset();  
	time(&td);  
	printf("Current time=%s",asctime(localtime(&td)));  
	printf("Date_getTimer() is:%f\n", Date_getTimer());  

	printf("getdate:%s",asctime(Date_fromString("Thu, 27 Aug 2015 16:06:52 +0800")));
	//printf("getdate:%s",asctime(Date_fromString("Thu, 27 Jan 2015 16:06:52 +0800")));


	Date_getDaysFrom1970(date1);
	Date_getDaysFrom1970(date2);
	*/
	Date_getDaysFrom1970(Date_newFrom1970(-24855));
	Date_getDaysFrom1970(Date_newFrom1970(0));
	Date_getDaysFrom1970(Date_newFrom1970(33462-25569));
	Date_getDaysFrom1970(Date_newFrom1970(-24106));
	Date_getDaysFrom1970(Date_newFrom1970(33462-24106-365*5-1));
	return 0;
}
#endif
