#ifndef _date_h
#define _date_h

#include"stdlib.h"
#include"stdio.h"
#include"string.h"
#include"time.h"
#include"mystring.h"

typedef struct tm Date;
time_t Date_getTime();// 按照通用时间返回 Date 对象中自 1970 年 1 月 1 日午夜以来的毫秒数。 Date 
Date*Date_now();// 返回一个当前时间的数据结构
time_t Date_getTime();// 按照通用时间返回 Date 对象中自 1970 年 1 月 1 日午夜以来的毫秒数。 Date 
Date*Date_new(time_t *t);//根据毫秒数来设定时间
time_t Date_value(Date*date);// 按照通用时间返回 Date 对象中自 1970 年 1 月 1 日午夜以来的毫秒数。 
Date*Date_new2(int tm_year,int tm_mon,int tm_mday,int tm_hour,int tm_min,int tm_sec);//根据年,月,日,时,分,秒来设定时间
double Date_diff(Date* date1,Date* date2);//两个日期相差的毫秒树
Date * Date_newFrom1970(time_t days);// 1900.1.1=-25569;1904.1.2=-24106
time_t Date_getDaysFrom1970(Date * date);
#endif
