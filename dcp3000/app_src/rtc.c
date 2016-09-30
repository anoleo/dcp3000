#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <fcntl.h>

#include <linux/rtc.h>

#include <sys/time.h>
#include <time.h>


#include "rtc.h"

#ifndef USE_HWCLOCK_TOOLS
static int rtc_fd;
#endif


/*
hwclock –r         显示硬件时钟与日期
hwclock –s         将系统时钟调整为与目前的硬件时钟一致。
hwclock –w        将硬件时钟调整为与目前的系统时钟一致。
//*/
#define hwclock_r()	system("hwclock -r")
#define hwclock_s()	system("hwclock -s")
#define hwclock_w()	system("hwclock -w")
#define view_rtctime()	hwclock_r()
#define sync_systime()	hwclock_s()
#define sync_rtctime()	hwclock_w()

static int set_systime(struct tm *stm)
{
	char cmd[40];
	
	sprintf(cmd, "date -s \"%04d-%02d-%02d %02d:%02d:%02d\"",
			stm->tm_year, stm->tm_mon, stm->tm_mday,  stm->tm_hour, stm->tm_min, stm->tm_sec);

	return system(cmd);
}

static struct tm *get_systime(void)
{
	time_t cur_time;
	struct tm *pst_time;

	cur_time = time(NULL);
	pst_time = localtime(&cur_time);

	pst_time->tm_year += 1900;
	pst_time->tm_mon += 1;

	
	return pst_time;
}

static char is_leap_year_zmy(unsigned int y){	return y % 4 == 0 && (y % 100 != 0 || y % 400 == 0);}
int check_tm_valid(struct tm *stm)
{

	if((stm->tm_year<2000)||(stm->tm_year>2099))
		return -1;
	if((stm->tm_mon<1)||(stm->tm_mon>12))
		return -1;
	if((stm->tm_mon==1)||(stm->tm_mon==3)||(stm->tm_mon==5)||(stm->tm_mon==7)||(stm->tm_mon==8)||(stm->tm_mon==10)||(stm->tm_mon==12))
	{
		if((stm->tm_mday<1)||(stm->tm_mday>31))
			return -1;
	}
	else if(stm->tm_mon==2)
	{
		if(is_leap_year_zmy(stm->tm_year))
		{
			if((stm->tm_mday<1)||(stm->tm_mday>29))
				return -1;
		}
		else
		{
			if((stm->tm_mday<1)||(stm->tm_mday>28))
				return -1;
		}
	}
	else
	{
		if((stm->tm_mday<1)||(stm->tm_mday>30))
			return -1;
	}
	if((stm->tm_hour<0)||(stm->tm_hour>23))
		return -1;
	if((stm->tm_min<0)||(stm->tm_min>59))
		return -1;
	if((stm->tm_sec<0)||(stm->tm_sec>59))
		return -1;

	return 0;
}

/*
struct rtc_time {
	int tm_sec;
	int tm_min;
	int tm_hour;
	int tm_mday;
	int tm_mon;
	int tm_year;
	int tm_wday;
	int tm_yday;
	int tm_isdst;
};

struct tm{
    int tm_sec; // 秒，0～61 
    int tm_min; // 分，0～59 
    int tm_hour; // 时，0～23
    int tm_mday; // 日，1～31
    int tm_mon; // 月(从1月开始)，0～11
    int tm_year; // 年(从1900年开始)
    int tm_wday; // 星期(从周日开始)，0～6
    int tm_yday; // 天数(从1月1日开始)，0～365
    int tm_isdst; // 夏令时标记
}
//*/

//struct rtc_time 和struct tm 结构完全一样
struct tm * get_rtc_datetime(struct tm *rtm)
{

	#ifndef USE_HWCLOCK_TOOLS
	static struct tm stm;
	if(rtm == NULL){rtm = &stm;}
	
	ioctl(rtc_fd, RTC_RD_TIME, rtm);
	rtm->tm_year += 1900;
	rtm->tm_mon += 1;

	set_systime(rtm);
	
	#else
	sync_systime();
	rtm = get_systime();
	#endif

	
	//printf("get rtc datatime : %d-%d-%d %.2d:%.2d:%.2d \n", rtm->tm_year, rtm->tm_mon, rtm->tm_mday, rtm->tm_hour, rtm->tm_min, rtm->tm_sec);
	//printf("get rtc datatime : %d isdst, %d ydata, %d wdata \n", rtm->tm_isdst, rtm->tm_yday, rtm->tm_wday);

	return rtm;
}

int set_rtc_datetime(struct tm *rtm)
{

	if(rtm){
		set_systime(rtm);
	}
		
	//printf("set rtc datatime : %d-%d-%d %.2d:%.2d:%.2d \n", rtm->tm_year, rtm->tm_mon, rtm->tm_mday, rtm->tm_hour, rtm->tm_min, rtm->tm_sec);
	//printf("set rtc datatime : %d isdst, %d ydata, %d wdata \n", rtm->tm_isdst, rtm->tm_yday, rtm->tm_wday);
	
	#ifndef USE_HWCLOCK_TOOLS
	time_t cur_time;
	cur_time  = time(NULL);
	rtm = localtime(&cur_time);

	return ioctl(rtc_fd, RTC_SET_TIME, rtm);

	#else
	return sync_rtctime();
	#endif
	
}

int open_rtc (void)
{ 	
	#ifndef USE_HWCLOCK_TOOLS
	rtc_fd = open("/dev/rtc0", O_RDWR);

	if(rtc_fd < 0){
		printf("***Can't open the  rtc0  driver handle!\r\n");
		return -1;
	}
	#endif
	
	//get_rtc_datetime(NULL);
	return 0;
}

void close_rtc(void)
{
	#ifndef USE_HWCLOCK_TOOLS
	close(rtc_fd);
	#endif

}
