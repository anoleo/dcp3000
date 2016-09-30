
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <dirent.h>
#include <ftw.h>

//#include <sys/types.h>


#include "utils.h"

#if 1
u_char calc_crc8(u_char *ptr, int len) 
{ 
	u_char i; 
	u_char crc=0; 
	u_char crcb;
	u_char x;
	for (x=0;x<len;x++) { 
		crcb = ptr[x];
		for(i=0; i<8; i++) {
 			if(((crc ^ crcb)&1)!=0) {
				crc ^=0x18;
				crc >>=1;
				crc |= 0x80;
			} 
			else 
				crc>>=1;
			
 			crcb >>= 1; 
 		} 
	} 
	return(crc); 
}


u_short docrc16(u_short cdata,u_short CRC)
{
	u_short CRC16 = CRC;
	u_short oddparity[16] = { 0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0 };
	cdata = (cdata ^ (CRC16 & 0xff)) & 0xff;
	CRC16 >>= 8;

	if (oddparity[cdata & 0xf] ^ oddparity[cdata >> 4])
		CRC16 ^= 0xc001;

	cdata <<= 6;
	CRC16 ^= cdata;
	cdata <<= 1;
	CRC16 ^= cdata;

	return CRC16;
}

u_short calc_crc16(unsigned char *ptr, int len) 
{ 
	u_char x;
	u_short crc=0; 
	
	for (x=0;x<len;x++) {
		crc = docrc16(ptr[x],crc);
		
	}
	
	return(crc); 
}
#endif


u_int CRCByte(u_int crc, u_char ch)
{
	u_char bb3;
	u_char i,temp,temp2;

	bb3 = 0;
	for (i = 0 ;i < 8; i++) {
		temp = (crc >> 24) & 0x000000ff;
		temp2 = ch ^ temp;

		ch = ch << 1;
		if (bb3) ch ++;

		if ((crc & 0x80000000)) bb3 = 1;
			else bb3 = 0;
		crc = crc << 1;
		if ((temp2 & 0x80)) crc ++;

		if ((temp2 & 0x80)) crc = crc ^ 0x04c11db6;
	}
	return crc;
}


u_int calc_crc32(u_char *point, int length)
{
	u_int crc = 0xffffffff;

	u_short ii;
	u_char bb3,ch;
	u_char i,temp,temp2;
	
	for (ii = 0; ii < length; ii++) 
	{
		bb3 = 0;
		ch = point[ii];
		for (i = 0 ;i < 8; i++) {
			temp = (crc >> 24) & 0x000000ff;
			temp2 = ch ^ temp;

			ch = ch << 1;
			if (bb3) ch ++;

			if ((crc & 0x80000000)) bb3 = 1;
				else bb3 = 0;
			crc = crc << 1;
			if ((temp2 & 0x80)) crc ++;

			if ((temp2 & 0x80)) crc = crc ^ 0x04c11db6;
		}
		//crc = CRCByte(crc,point[ii]);		
	}

	return crc;
}



void set_bits (u_char *buf, int byte_offset, int startbit, int bitlen,u_long data)
{
        u_char  *b;
        u_long  v;
        u_long  mask;
        u_long tmp_long;

        mask = (1ULL << bitlen) - 1;
        v = data & mask;

        b = &buf[byte_offset + (startbit / 8)];
        startbit %= 8;

        tmp_long = (u_long)( ((*b)<<24) + (*(b+1)<<16) + (*(b+2)<<8) + *(b+3) );

        startbit = 32 - startbit - bitlen;
        v= v << startbit;
        mask = mask << startbit;

        tmp_long &= ~mask;
        tmp_long |= v;

        *b = (tmp_long >> 24);
        *(b+1) = (tmp_long >> 16);
        *(b+2) = (tmp_long >> 8);
        *(b+3) = (tmp_long);
}


u_long get_bits(u_char *buf, int byte_offset, int startbit, int bitlen)
{
	u_char *b;
	u_long v;
	u_long mask;
	u_long tmp_long;

	b = &buf[byte_offset + (startbit / 8)];
	startbit %= 8;
	
	tmp_long = (u_long)(((*b) << 24) + (*(b+1) << 16) + (*(b + 2) << 8) + *(b + 3));
	startbit = 32 - startbit - bitlen;

	tmp_long = tmp_long >> startbit;

	mask = (1ULL << bitlen) - 1;

	v = tmp_long & mask;

	return v;
}

void mem_set(void *p_byte, int size, long long value, int count)
{
	int i;
	count *= size;
	for(i=0; i<count; i+=size){
		memcpy(p_byte+i, &value, size);
	}
}


#define PRINT_TYPES_BUF_LEN	(1024*3+1024/8+10)
void print_bytes(void *p_data, int len)
{
	char *p_buf;
	char buf[PRINT_TYPES_BUF_LEN];
	int quotient;
	int remainder;
	int buf_len;
	int loops, i = 0, j;
	int offset;

	if((p_data == NULL) || (len <= 0)){
		return;
	}
	
	p_buf = (char *)p_data;
	
	buf_len = 1024;

	loops = len/buf_len;
	
	
	printf("\n--------------print_bytes, len = %d-----------------\n", len);

	for(j=0; j<=loops; j++){

		if(j<loops){
			quotient = buf_len / 8;
			remainder = buf_len % 8;
		}else{
			quotient = len%buf_len / 8;
			remainder = len%buf_len % 8;
		}
		
		buf[0] = 0;
	
		for(i=0; i<quotient; i++){
			offset = i*8 + j*buf_len;
			sprintf(buf+strlen(buf), "%.2x\t%.2x\t%.2x\t%.2x\t%.2x\t%.2x\t%.2x\t%.2x\n", p_buf[offset+0], p_buf[offset+1]
				, p_buf[i*8+2], p_buf[offset+3], p_buf[offset+4], p_buf[offset+5], p_buf[offset+6], p_buf[offset+7]);
		}

		offset = quotient*8;
		for(i=0; i<remainder; i++){
			sprintf(buf+strlen(buf), "%.2x\t", p_buf[offset+i]);
		}
		
		printf("%s\n", buf);
		
	}
	
}


char hex2upper(char hexnum)
{
	char ch;
	
	if (hexnum>=0 && hexnum<=9){
		ch = hexnum + '0';
	}else if(hexnum>=0xa && hexnum<=0xf){
        ch = hexnum - 10 + 'A' ;
	}else{
		ch = hexnum;
	}

	return ch;
}

char hex2lower(char hexnum)
{
	char ch;
	
	if (hexnum>=0 && hexnum<=9){
		ch = hexnum + '0';
	}else if(hexnum>=0xa && hexnum<=0xf){
        ch = hexnum - 10 + 'a' ;
	}else{
		ch = hexnum;
	}

	return ch;
}

int ascii2num(char ascii)
{
	if((ascii >= '0') && (ascii <= '9')){
		return (ascii - '0');
	}else if((ascii >= 'A') && (ascii <= 'F')){
		return (ascii - 'A' + 10);
	}else if((ascii >= 'a') && (ascii <= 'f')){
		return (ascii - 'a' + 10);
	}

	return 0;
}

int byte2hex(char *p_byte, int len, char* p_hex)
{
    int  i;

	if(p_byte == NULL || len <= 0 || p_hex== NULL){
		return 0;
	}

    for (i=0; i<len; i++)
    {
        *p_hex++ = hex2upper((*p_byte >> 4) & 0xf);
        *p_hex++ = hex2upper(*p_byte & 0xf);
		p_byte++;
    }

    return (len << 1);
}

int hex2byte(char *p_hex, int len, char* p_byte)
{
    int  i;

	if(p_byte == NULL || len <= 0 || p_hex== NULL){
		return 0;
	}

	if(len%2){
		len--;
	}

    for (i=0; i<len; i+=2)
    {
        *p_byte = ascii2num(*p_hex++) << 4;
        *p_byte |= ascii2num(*p_hex++) & 0xf;
		p_byte++;
    }

    return (len/2);
}



u_long byte2int(char *p_byte, int size)
{
	int i;
	u_long num = 0;

	if(size < 0 || size > 8 || p_byte == NULL){
		return 0;
	}
	
	for(i=0; i<size; i++){
		num <<= 8;
		num |= p_byte[i]&0xff;
	}

	return num;
}


u_long hex2int(char *p_hex, int len)
{
    int  i;
	u_long val = 0;
	
	if(len <= 0 || p_hex== NULL){
		return 0;
	}


    for (i=0; i<len; i++)
    {
    	if(*p_hex == 0)break;
		val <<= 4;
        val |= (ascii2num(*p_hex++)& 0xf);
    }

    return val;
}


void clean_maskbit(void *maskbit, int bit_num)
{
	char *mask;
	int u8_bits;
	
	if(maskbit == NULL){
		return;
	}

	mask = (char *)maskbit;
	u8_bits = 8;
	mask[bit_num/u8_bits] &= ~(1<<(bit_num%u8_bits));
	
}

void set_maskbit(void *maskbit, int bit_num)
{
	char *mask;
	int u8_bits;
	
	if(maskbit == NULL){
		return;
	}

	mask = (char *)maskbit;
	u8_bits = 8;
	mask[bit_num/u8_bits] |= (1<<(bit_num%u8_bits));
	
}

int check_maskbit(void *maskbit, int bit_num)
{
	char *mask;
	int u8_bits;

	if(maskbit == NULL){
		return 0;
	}

	mask = (char *)maskbit;
	u8_bits = 8;

	return mask[bit_num/u8_bits] & (1<<(bit_num%u8_bits));
	
}


//bigend and littleend convert
u_int endian_reverse(u_int o_int)
{
	u_int res;

	res = (o_int & 0xff) << 24;
	res |= (o_int & 0xff00) << 8;
	res |= (o_int >> 8) & 0xff00;
	res |= (o_int >> 24) & 0xff;

	return res;
}

//if the cpu is littleend then the endian convert
u_int big_endian(u_int o_int)
{
	u_char *p_ch;
	u_int res;

	p_ch = (u_char *)(&res);

	*p_ch++ = (o_int >> 24)  & 0xff;
	*p_ch++ = (o_int >> 16)  & 0xff;
	*p_ch++ = (o_int >> 8)  & 0xff;
	*p_ch++ = (o_int >> 0)  & 0xff;

	return res;
}

//if the cpu is bigend then the endian convert
u_int little_endian(u_int o_int)
{
	u_char *p_ch;
	u_int res;

	p_ch = (u_char *)(&res);

	*p_ch++ = (o_int >> 0)  & 0xff;
	*p_ch++ = (o_int >> 8)  & 0xff;
	*p_ch++ = (o_int >> 16)  & 0xff;
	*p_ch++ = (o_int >> 24)  & 0xff;

	return res;
}

int is_space(char ch){
	return (ch)&& !(ch >= 33 && ch <= 126);
}

char *strtrim(char *str)
{
	char * tail;
	
	while(is_space(*str))str++;
	if(*str == 0){
		return str;
	}
	tail = str;
	while(*++tail);
	while(is_space(*--tail))*tail = '\0';
	
	return str;
}

int sameStarts(char *str1,char *str2)
{
	int count = 0;
	if(str1 == 0 || str2 == 0)
		return 0;
	while(1){
		if(*str1++ == *str2++)
			count++;
		else
			break;
	}
	return count;
}

int startsWith(char *str1, char *str2)
{

	while(1){
		if(!*str2 && *str1){
			return 1;
		}else if(!*str1 && *str2){
			return 0;
		}
		if(*str2++ != *str1++){
			return 0;
		}
	}
	return 0;
}

void UsDelay(unsigned int count)
{
	//#define CALC_SPEND_TIMES
	unsigned int i;
	#ifdef CALC_SPEND_TIMES
	struct timeval t_start,t_end; 
	long long spend_time,  spend_time2= 0;

	gettimeofday(&t_start, NULL); 
	#endif
	
	count *= 115;	//实测经验值
	for(i = 0;i < count; i++){;}
	
	#ifdef CALC_SPEND_TIMES
	gettimeofday(&t_end, NULL); 
	
	//spend_time = ((long)t_end.tv_sec - (long)t_start.tv_sec)*1000+(long)(t_end.tv_usec - (long)t_start.tv_usec)/1000; 
	spend_time = ((long)t_end.tv_sec - (long)t_start.tv_sec)*1000000+(long)(t_end.tv_usec - (long)t_start.tv_usec); 
	
	printf("UsDelay(%d) ,spend time: %Ld us \n", count, spend_time); 
	#endif

}


void wait_second(int sec)
{
	time_t time_now,time_pre;

	time(&time_now);
	time_pre = time_now;
	while(abs(time_now - time_pre)<sec)
	{
		time(&time_now);
	}
	return;
}


void print_time(time_t cur_time)
{
	struct tm *time_temp;

	time_temp = localtime(&cur_time);
	
	printf("%04d-%02d-%02d %02d:%02d:%02d\n",1900 + time_temp->tm_year, 1 + time_temp->tm_mon,
		time_temp->tm_mday,time_temp->tm_hour,time_temp->tm_min,time_temp->tm_sec);
		
}

char *show_curtime(void)
{
	static char str_buf[20];

	time_t cur_time = time(NULL);

	time2str(&cur_time, str_buf);
	
	return str_buf;
}

/*
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
struct tm *get_systime(void)
{
	time_t cur_time;
	struct tm *pst_time;

	cur_time = time(NULL);
	pst_time = localtime(&cur_time);

	pst_time->tm_year += 1900;
	pst_time->tm_mon += 1;

	
	return pst_time;
}

int set_systime(struct tm *stm)
{
	char cmd[40];
	
	sprintf(cmd, "date -s \"%04d-%02d-%02d %02d:%02d:%02d\"",
			stm->tm_year, stm->tm_mon, stm->tm_mday,  stm->tm_hour, stm->tm_min, stm->tm_sec);

	return system(cmd);
}

//return double format : ###.###### s
/*
double calc_exectime(struct timeval *begintime, struct timeval *endtime)
{
	int sec_used, usec_used;

	sec_used = endtime->tv_sec - begintime->tv_sec;
	usec_used = endtime->tv_usec - begintime->tv_usec;

	return (sec_used+(double)usec_used/1000000);
}
//*/

//return double format : ###.###### s
double show_exectime(struct timeval *begintime, char *prompt)
{
	struct timeval endtime;

	double rtime;
	
	gettimeofday(&endtime, NULL);

	rtime = calc_exectime(begintime, &endtime);
	
	printf("%s : used %8.6f second\n", prompt, rtime);

	return rtime;
}

#define DECIMALS_COMPARE(pdata1, pdata2)	({	\
	if((*(pdata1) > *(pdata2) + EP)){	\
		return 1;	\
	}else if((*(pdata1) < *(pdata2) -EP)){	\
		return -1;	\
	}else{ 	\
		return 0;	\
	}	\
})

int floatcmp(float *pdata1, float *pdata2)
{
	#define	EP	1e-6
	
	#if 1
	if((*(pdata1) > *(pdata2) + EP)){
		return 1;
	}else if((*(pdata1) < *(pdata2) -EP)){
		return -1;
	}else{ 
		return 0;
	}
	#else
	DECIMALS_COMPARE(pdata1, pdata2);
	#endif
}

int doublecmp(double *pdata1, double *pdata2)
{
	#define	EP	1e-6

	#if 1
	if((*(pdata1) > *(pdata2) + EP)){
		return 1;
	}else if((*(pdata1) < *(pdata2) -EP)){
		return -1;
	}else{ 
		return 0;
	}
	#else
	DECIMALS_COMPARE(pdata1, pdata2);
	#endif
}


static void memory_error_and_abort (char *fname)
{
	fprintf (stderr, "%s: out of virtual memory\n", fname);
	exit (2);
}

void *xMalloc (size_t bytes)
{
	void *temp;

	temp = malloc (bytes);
	if (temp == 0)
		memory_error_and_abort ("xMalloc");
	
	return (temp);
}

char *cpstring (const char *s)
{
	char *ret;

	ret = (char *)xMalloc (strlen (s) + 1);
	strcpy (ret, s);

	return ret;
}

char *cpnstring (const char *s, int n)
{
	char *ret;
	int len;

	len = strlen(s);
	if(n>len){
		n = len;
	}

	ret = (char *)xMalloc (n + 1);
	memcpy (ret, s, n);
	ret[n] = 0;

	return ret;
}


int read_from_file(char *filename, char *databuf)
{
	//目前需要下载的参数有
	//下载的参数应包括参数的版本
	int len, rdlen;
	int fd;
	char *pdata;
	
	if(databuf == NULL){
		return 0;
	}

	pdata = databuf;

	fd = open(filename, O_RDONLY);
	if(fd<0){
		printf(" open %s failed \n", filename);
		return 0;
	}
	len = 0;
	do{
		rdlen = read(fd, pdata, 1024);
		//printf("rdlen = %d \n", rdlen);
		if(rdlen>0){
			pdata += rdlen;
			len += rdlen;
		}
	}while(rdlen>0);

	close(fd);
		
	return len;
}

int write_to_file(char *filename, char *databuf, int len)
{
	int fd;
	int wrlen, ret;
	char *pdata = databuf;

	if((filename == NULL) || (databuf == NULL) || (len<=0)){
		return -1;
	}
	
	fd = open(filename, O_RDWR | O_TRUNC| O_CREAT, 0644);
	if(fd<0){
		printf(" open %s failed \n", filename);
		return 0;
	}

	while(len>0){
		wrlen = write(fd, pdata, len);
		//printf("wrlen = %d \n", wrlen);
		if(wrlen>0){
			pdata += wrlen;
			len -= wrlen;
		}else{
			ret = -1;
			break;
		}
	}

	close(fd);

	return ret;
}

static int ftw_dir_total_size = 0;

static int ftw_stat_file(const char * file, const struct stat *sb, int flag)
{
	//printf("%s : mode = %#x, size = %ld, flag = %#x \n", file, sb->st_mode, sb->st_size, flag);
	if(flag == FTW_F){
		ftw_dir_total_size += sb->st_size;
	} 
	return 0;
}

int calc_dirsize(char *dir)
{
	ftw_dir_total_size = 0;
	
	if(ftw(dir, ftw_stat_file, 10000) < 0){
		return -1;
	}

	return ftw_dir_total_size;
}

static int traverse_files(char *dir, char *prefix, char *subfix)
{
	char name[256];
	struct dirent *dp;
	DIR *dfd;
	//int path_len = 0;

	struct stat stbuf;

	if (stat(dir, &stbuf) == -1) {
		fprintf(stderr, "stat: can't access %s\n", dir);
		return -1;
	}
	
	if ((stbuf.st_mode & S_IFMT) != S_IFDIR){
		return -2;
	}

	if ((dfd = opendir(dir)) == NULL) {
		fprintf(stderr, "dirwalk: can't open %s\n", dir);
		return -3;
	}
	
	while ((dp = readdir(dfd)) != NULL) {

		if (strcmp(dp->d_name, ".") == 0 || strcmp(dp->d_name, "..") == 0)
			continue;    /* skip self and parent */
		//p_printf(bin_pid,"traverse_dir : dp->d_name :  %s\n", dp->d_name);
		if(strstr(dp->d_name, prefix) && strstr(dp->d_name, subfix)){

			//memset(name, 0, 256);
			sprintf(name, "%s/%s", dir, dp->d_name);
			//p_printf(bin_pid,"traverse_dir : %s\n", name);
			if (stat(name, &stbuf) == -1) {
				fprintf(stderr, "fsize: can't access %s\n", name);
				continue;
			}		
			
		}
	}
	
	closedir(dfd);

	return 0;
	
}


int calc_wildcardfiles_size(char *wildcard)
{
	
	char name[256];
	struct dirent *dp;
	DIR *dfd;
	struct stat stbuf;
	char *filename, *dir;
	int filesize = 0;
	char *prefix, *subfix;

	strcpy(name, wildcard);

	filename = strrchr(name, '/');


	if(filename){
		dir = name;
		*filename = 0;
		filename++;
	}else{
		dir = ".";
		filename = name;
	}

	subfix = strrchr(filename, '*');
	if(subfix){
		*subfix = 0;
		subfix++;
		prefix = filename;
	}else{
		if (stat(name, &stbuf) == -1) {
			fprintf(stderr, "fsize: can't access %s\n", name);
		}else{
			filesize += stbuf.st_size;
		}

		return filesize;
	}

	if (stat(dir, &stbuf) == -1) {
		fprintf(stderr, "stat: can't access %s\n", dir);
		return -1;
	}
	
	if ((stbuf.st_mode & S_IFMT) != S_IFDIR){
		return -2;
	}

	if ((dfd = opendir(dir)) == NULL) {
		fprintf(stderr, "dirwalk: can't open %s\n", dir);
		return -3;
	}
	
	while ((dp = readdir(dfd)) != NULL) {

		if (strcmp(dp->d_name, ".") == 0 || strcmp(dp->d_name, "..") == 0)
			continue;    /* skip self and parent */
		//p_printf(bin_pid,"traverse_dir : dp->d_name :  %s\n", dp->d_name);
		if(strstr(dp->d_name, prefix) && strstr(dp->d_name, subfix)){

			//memset(name, 0, 256);
			sprintf(name, "%s/%s", dir, dp->d_name);
			//p_printf(bin_pid,"traverse_dir : %s\n", name);
			if (stat(name, &stbuf) == -1) {
				fprintf(stderr, "fsize: can't access %s\n", name);
				continue;
			}else{
				filesize += stbuf.st_size;
			}		
			
		}
	}
	
	closedir(dfd);

	return filesize;
}

#if 1
int calc_filesize(char *file)
{
	struct stat stbuf;

	if (stat(file, &stbuf) == -1) {
		//fprintf(stderr, "stat: can't access %s\n", file);
		return -1;
	}

	return stbuf.st_size;
}
#else
int calc_filesize(char *name)
{
	int ret;
	FILE *p_file;

	p_file = fopen(name, "rb");
	if(p_file == NULL){
		return -1;
	}
	
	ret = fseek(p_file, 0, SEEK_END);
	if(ret){
		fclose(p_file);
		return -1;
	}
	
	ret = ftell(p_file);

	fclose(p_file);

	//printf("calc file : %s , size = %d \n", name, ret);

	return ret;
}
#endif
