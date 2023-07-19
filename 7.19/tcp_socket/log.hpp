#pragma once
#include <iostream>
#include <cstdio>
#include <stdarg.h>
#include <string>
#include <unistd.h>
#include <ctime>
using namespace std;
string filename = "log.txt";
enum LEVEL
{
    Debug = 0,
    Info,
    Warning,
    Error,
    Fatal,
    Unknown
};
// int tm_sec;			/* Seconds.	[0-60] (1 leap second) */
//   int tm_min;			/* Minutes.	[0-59] */
//   int tm_hour;			/* Hours.	[0-23] */
//   int tm_mday;			/* Day.		[1-31] */
//   int tm_mon;			/* Month.	[0-11] */
//   int tm_year;			/* Year	- 1900.  */
//   int tm_wday;			/* Day of week.	[0-6] */
//   int tm_yday;			/* Days in year.[0-365]	*/
//   int tm_isdst;			/* DST.		[-1/0/1]*/
string get_time()
{
    time_t cur_time = time(nullptr);
    struct tm *s_tm = localtime(&cur_time);
    char buffer[128];
    snprintf(buffer, sizeof(buffer), "%d-%d-%d %d:%d:%d", s_tm->tm_year + 1900,
             s_tm->tm_mon + 1, s_tm->tm_mday, s_tm->tm_hour, s_tm->tm_min, s_tm->tm_sec);

    return buffer;
}

string get_level(int level)
{
    switch (level)
    {
    case 0:
        return "Debug";
    case 1:
        return "Info";
    case 2:
        return "Warning";
    case 3:
        return "Error";
    case 4:
        return "Fatal";
    default:
        return "Unknown";
    }
}

void logMessage(int level, const char *format, ...)
{
    // 可变参数列表
    char logleft[1024];
    // 向左半部分写入日志等级,时间,进程id
    string level_str = get_level(level);
    string cur_time = get_time();
    snprintf(logleft, sizeof(logleft), "[%s][%s][%d]", level_str.c_str(), cur_time.c_str(), getpid());

    char logright[1024];
    // 生命一个使用可变参数的指针
    va_list p;
    // 让p指针指向可变参数的开始位置
    va_start(p, format);
    vsnprintf(logright, sizeof(logright), format, p); // 向缓冲区输入
    va_end(p);                                        //    结束使用p指针

    // printf("%s%s\n", logleft, logright);
    FILE *fp = fopen(filename.c_str(), "a");
    if (fp == nullptr)
        return;
    fprintf(fp, "%s%s\n", logleft, logright);
    fflush(fp); // 可写也可以不写
    fclose(fp);
}