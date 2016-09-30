#ifndef CHARSET_CH_
#define CHARSET_CH_

#include "./include/iconv.h" 

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define ISO_IEC_8859_5				0x1
#define ISO_IEC_8859_6				0x2
#define ISO_IEC_8859_7				0x3
#define ISO_IEC_8859_8				0x4
#define ISO_IEC_8859_9				0x5
#define ISO_IEC_8859_10				0x6
#define ISO_IEC_8859_11				0x7
#define ISO_IEC_8859_12				0x8
#define ISO_IEC_8859_13				0x9
#define ISO_IEC_8859_14				0xa
#define ISO_IEC_8859_15				0xb
#define ISO_IEC_8859				0x10
#define ISO_IEC_10646_1				0x11
#define KSC5601_1987				0x12
#define GB_2312_1980				0x13
#define ISO_IEC_10646_1_BIG5		0x14
#define ISO_IEC_10646_1_UTF_8		0x15

typedef struct {
	char* intype;
	char* outtype;
	char *inbuf;
	int inlen;
	char *outbuf;
	int outlen;
}charset_par_t;


//int code_convert(char *from_charset,char *to_charset,char *inbuf,int inlen,char *outbuf,int outlen) ;
int charset_r_fun(charset_par_t * p_charset_par);
int charset_w_fun(charset_par_t * p_charset_par);
#endif