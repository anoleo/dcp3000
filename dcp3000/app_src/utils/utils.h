#ifndef _UTILS_H_
#define _UTILS_H_

#include <sys/types.h>
#include <sys/time.h>
#include <time.h>



#define PRINT_LEVLE		0


#undef	PRINTF
#define PRINTF(fmt, ...)	printf(fmt, ##__VA_ARGS__);fflush(stdout)


#if	(PRINT_LEVLE & 3)
#define InfoPrintf(fmt, ...)	PRINTF("%s : "fmt, __FUNCTION__, ##__VA_ARGS__)
#else
#define InfoPrintf(fmt, ...)
#endif

#if	(PRINT_LEVLE & 2)
#define DebugPrintf(fmt, ...)	PRINTF("---%s  [line %d]--- : "fmt, __FUNCTION__, __LINE__, ##__VA_ARGS__)
#else
#define DebugPrintf(fmt, ...)
#endif

#if	(PRINT_LEVLE & 4)
#define TagPrintf(idx)		PRINTF("---%s  [line %d]--- :  %d\n", __FUNCTION__, __LINE__, idx)
#else
#define TagPrintf(idx)
#endif

#define printchar(ch) do{putchar(ch);fflush(stdout);}while(0)

u_char calc_crc8(u_char *ptr, int len) ;
u_short calc_crc16(u_char *ptr, int len) ;
u_int calc_crc32(u_char *point, int length);
u_int generateCRC(u_int crc, u_char *point, int length);

void set_bits (u_char *buf, int byte_offset, int startbit, int bitlen,u_long data);
u_long get_bits(u_char *buf, int byte_offset, int startbit, int bitlen);

void print_bytes(void *p_buf, int len);

void mem_set(void *p_byte, int size, long long value, int count);
u_long hex2int(char *p_hex, int len);
u_long byte2int(char *p_byte, int size);
int hex2byte(char *p_hex, int len, char* p_byte);
int byte2hex(char *p_byte, int len, char* p_hex);

int ascii2num(char ascii);


void clean_maskbit(void *maskbit, int bit_num);
void set_maskbit(void *maskbit, int bit_num);
int	check_maskbit(void *maskbit, int bit_num);


char *strtrim(char *str);
int sameStarts(char *str1,char *str2);
int startsWith(char *str1, char *str2);

void *xMalloc (size_t bytes);
char *cpstring (const char *s);
char *cpnstring (const char *s, int n);

int calc_dirsize(char *dir);
int calc_filesize(char *file);
int calc_wildcardfiles_size(char *wildcard);

int read_from_file(char *filename, char *databuf);
int write_to_file(char *filename, char *databuf, int len);

#define  calc_exectime(begintime, endtime)	(((endtime)->tv_sec - (begintime)->tv_sec)+(double)((endtime)->tv_usec - (begintime)->tv_usec)/1000000)
//double calc_exectime(struct timeval *begintime, struct timeval *endtime);
double show_exectime(struct timeval *begintime, char *prompt);

int floatcmp(float *pdata1, float *pdata2);
int doublecmp(double *pdata1, double *pdata2);

#define  time2str(p_time, str_buf)		strftime(str_buf, 20, "%Y-%m-%d %H:%M:%S", localtime((time_t *)(p_time)))

#define print_stime(stm)		\
	printf("%04d-%02d-%02d %02d:%02d:%02d\n",1900 + (stm)->tm_year, 1 + (stm)->tm_mon,	\
		(stm)->tm_mday,(stm)->tm_hour,(stm)->tm_min,(stm)->tm_sec);

void print_time(time_t cur_time);
char *show_curtime(void);

struct tm *get_systime(void);
int set_systime(struct tm *);

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

void wait_second(int sec);
void UsDelay(u_int count);

u_int little_endian(u_int o_int);
u_int big_endian(u_int o_int);
u_int endian_reverse(u_int o_int);


#endif
