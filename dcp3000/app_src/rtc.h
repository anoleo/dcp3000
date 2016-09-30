#ifndef __RTC_H_
#define __RTC_H_

#include <time.h>

#define USE_HWCLOCK_TOOLS

int check_tm_valid(struct tm *stm);

struct tm * get_rtc_datetime(struct tm *rtm);
int set_rtc_datetime(struct tm *rtm);

int open_rtc (void);
void close_rtc(void);
#endif

