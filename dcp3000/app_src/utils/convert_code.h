
#ifndef __CONVERT_H__
#define __CONVERT_H__
/*
*code_convert.h
*Authors:Zheng Lixue
*Function : The mutual conversion between coding.
*Modification log:
*/

#include <sys/types.h>

/*************************************************************
*功能: ucs2_gb2312_table码表
**************************************************************/
//static const unsigned short ucs2_gb2312_table[][2];

/*************************************************************
*功能: ucs2_gb2312_table码表
**************************************************************/

//int code_convert(char *from_charset,char *to_charset,char *inbuf,int inlen,char *outbuf,int outlen);
 
/***_hanyi_2013/07/08 build
**filtrateCharacter to web 
*/
void filtrateCharacter(char *name,char *buf);

/*********************************************************************************************
*Authors & Time:_lixuezheng_2015/01/04_make function_
*Function :字符变成16进制
*Parameter:字符
*Return :整数
*********************************************************************************************/
 int String_into_Sixteen(u_char c);


/*********************************************************************************************
*Authors & Time:_lixuezheng_2015/01/04_make function_
*Function : 对字符串URL解码,编码的逆过程.
*Parameter:原字符串、原字符串大小（不包括最后的\0）、
结果字符串缓存区、结果地址的缓冲区大小(包括最后的\0)
*Return :实际有效的字符串长度   (0 解码失败)
*********************************************************************************************/
int URLDecode(const u_char* str, const int strSize, u_char* result, const int resultSize);


/*********************************************************************************************
*Authors & Time:_lixuezheng_2014/11/22_make function_
*Function : gb2312转换为ucs2
*Parameter:gb2312
*Return :ucs2编码
*********************************************************************************************/
unsigned short gb2312_to_ucs2(unsigned short gb2312);


/*********************************************************************************************
*Authors & Time:_lixuezheng_2014/11/22_make function_
*Function : gb2312字转换为utf-8字
*Parameter:from_name,to_utf8_name
*Return :
*********************************************************************************************/
unsigned char *gb2312_to_utf8(const unsigned char *gb2312, unsigned char *utf);

/*************************************************************
*Authors & Time:_lixuezheng_2014/12/24 function_
* 说明:
* UTF-8以字节为单位对Unicode进行编码。
* 从Unicode到UTF-8的编码方式如下：
* Unicode编码(16进制) --> UTF-8 字节流(二进制) 
* U-00000000 ~ U-0000007F --> 0xxxxxxx  
* U-00000080 ~ U-000007FF --> 110xxxxx 10xxxxxx  
* U-00000800 ~ U-0000FFFF --> 1110xxxx 10xxxxxx 10xxxxxx  
* U-00010000 ~ U-001FFFFF --> 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx  
* U-00200000 ~ U-03FFFFFF --> 111110xx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx  
* U-04000000 ~ U-7FFFFFFF --> 1111110x 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx 
* 故根据每个汉字的第一个字节，即可以知道utf-8汉字由几个字节组成
*************************************************************/
int get_utf8_nbytes(unsigned char first_char);

/*************************************************************
* 语法格式：unsigned short ucs2_to_gb2312(unsigned short ucs2)
* 实现功能：ucs2转gb2312
* 参数：	ucs2: 待转换的ucs2编码
* 返回值：	gb2312编码
**************************************************************/
unsigned short ucs2_to_gb2312(unsigned short ucs2);

/*********************************************************************************************
*Authors & Time:_lixuezheng_2014/12/24 function_
*Function : utf-8字转换为gb2312字 .
*Parameter:utf,gb2312
*Return : 0
*********************************************************************************************/
unsigned char *utf8_to_gb2312(const unsigned char *utf, unsigned char *gb2312);


/*********************************************************************************************
*Authors & Time:_lixuezheng_2014/12/24 function_
*Function : utf-8字转换为iso8859_1字 .
*Parameter:utf,iso8859_1
*Return : 0
*********************************************************************************************/
unsigned char *utf8_to_iso8859_1(const unsigned char *utf, unsigned char *iso8859_1);


/*********************************************************************************************
*Authors & Time:_lixuezheng_2014/11/22_make function_
*Function : iso8859-1字转换为utf-8  字
*Parameter:from_name,to_utf8_name
*Return :
*********************************************************************************************/
unsigned char *iso8859_1_to_utf8(const unsigned char *iso8859_1, unsigned char *utf);

unsigned char *iso8859_5_to_utf8(const unsigned char *iso8859_5, unsigned char *utf);

/*********************************************************************************************
*Authors & Time:_lixuezheng_2014/10/08_make function_
*Function : Special_Symbol_Convert for Encoder Top_menu OEM and Device.
*Parameter:Maybe need  covert  name
*Return : 0
*********************************************************************************************/
void  Special_Symbol_Convert(u_char *need_covert_name,u_char *after_covert_name,int num);


/*********************************************************************************************
*Authors & Time:_lixuezheng_2014/09/22_make function_
*Function : name_convert_to_utf8
*Parameter:from_name,to_utf8_name
*Return : 0
*********************************************************************************************/
void name_convert_to_utf8(u_char *from_name,u_char *to_utf8_name);

/*********************************************************************************************
*Authors & Time:_lixuezheng_2014/09/22_make function_
*Function : utf8_to_other_convert for Encoder
*Parameter:character_selection,from_utf8_name,to_other_name
*Return : 0
*********************************************************************************************/
void utf8_to_other_convert(int character_selection,u_char *from_utf8_name,u_char *to_other_name);

int get_codetype(u_char *str);

#endif
