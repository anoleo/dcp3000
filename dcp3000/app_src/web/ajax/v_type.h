/* 
* Copyright (c) 2012,victor 
* All rights reserved. 
*  
* 文件名称：victor_type 
* 文件标识：自定义变量类型 
* 摘    要：用位数识别变量
*
*  
* 当前版本：1.0  
* 作    者：victor 
* 完成日期：2012年11月30日 
* 
* 取代版本：----
* 原作者  ：---- 
* 完成日期：----年--月--日 
*/ 
/*1*/
#ifndef V_TYPE_
#define V_TYPE_

typedef signed char  v_8;
typedef unsigned char  v_u8;
typedef signed short v_16;
typedef unsigned short v_u16;
typedef signed long v_32;
typedef unsigned long v_u32;
typedef long long v_64;
typedef unsigned long long v_u64;
typedef unsigned char  u_char;



#define IPCMSG_INFO		0
#define IPCMSG_DEBUG		0

#define MSG_DEBUG				0
#if		IPCMSG_INFO
#define DEBUG_WM_PRINT(fmt,...)			fprintf(fp_uart,fmt ,__VA_ARGS__)
#define DEBUG_NWM_PRINT(fmt)			fprintf(fp_uart,fmt)
#else
#define DEBUG_WM_PRINT(fmt,...)	
#define DEBUG_NWM_PRINT(fmt)			
#endif

#endif