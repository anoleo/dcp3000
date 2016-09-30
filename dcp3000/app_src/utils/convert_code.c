/*
*code_convert.c
*Authors:Zheng Lixue
*Function : The mutual conversion between coding.
*Modification log:
*/

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <iconv.h>

//#include "../general/cgic.h"
//#include "../general/general.h"
#include "convert_code.h"

#define NON_NUM '0'
const unsigned short MAX_UNI_INDEX = 6808; /*6768*/
/*************************************************************
*����: ucs2_gb2312_table���
**************************************************************/
//static const unsigned short ucs2_gb2312_table[][2];

/*************************************************************
*����: ucs2_gb2312_table���
**************************************************************/
static const unsigned short ucs2_gb2312_table[][2]={
	/*unicode,GB2312*/
    {0x4E00,0xBBD2}, /* һ: GB2312 Code: 0xD2BB ==> Row:50 Col:27 */
    {0x4E01,0xA1B6}, /* ��: GB2312 Code: 0xB6A1 ==> Row:22 Col:01 */
    {0x4E03,0xDFC6}, /* ��: GB2312 Code: 0xC6DF ==> Row:38 Col:63 */
    {0x4E07,0xF2CD}, /* ��: GB2312 Code: 0xCDF2 ==> Row:45 Col:82 */
    {0x4E08,0xC9D5}, /* ��: GB2312 Code: 0xD5C9 ==> Row:53 Col:41 */
    {0x4E09,0xFDC8}, /* ��: GB2312 Code: 0xC8FD ==> Row:40 Col:93 */
    {0x4E0A,0xCFC9}, /* ��: GB2312 Code: 0xC9CF ==> Row:41 Col:47 */
    {0x4E0B,0xC2CF}, /* ��: GB2312 Code: 0xCFC2 ==> Row:47 Col:34 */
    {0x4E0C,0xA2D8}, /* آ: GB2312 Code: 0xD8A2 ==> Row:56 Col:02 */
    {0x4E0D,0xBBB2}, /* ��: GB2312 Code: 0xB2BB ==> Row:18 Col:27 */
    {0x4E0E,0xEBD3}, /* ��: GB2312 Code: 0xD3EB ==> Row:51 Col:75 */
    {0x4E10,0xA4D8}, /* ؤ: GB2312 Code: 0xD8A4 ==> Row:56 Col:04 */
    {0x4E11,0xF3B3}, /* ��: GB2312 Code: 0xB3F3 ==> Row:19 Col:83 */
    {0x4E13,0xA8D7}, /* ר: GB2312 Code: 0xD7A8 ==> Row:55 Col:08 */
    {0x4E14,0xD2C7}, /* ��: GB2312 Code: 0xC7D2 ==> Row:39 Col:50 */
    {0x4E15,0xA7D8}, /* ا: GB2312 Code: 0xD8A7 ==> Row:56 Col:07 */
    {0x4E16,0xC0CA}, /* ��: GB2312 Code: 0xCAC0 ==> Row:42 Col:32 */
    {0x4E18,0xF0C7}, /* ��: GB2312 Code: 0xC7F0 ==> Row:39 Col:80 */
    {0x4E19,0xFBB1}, /* ��: GB2312 Code: 0xB1FB ==> Row:17 Col:91 */
    {0x4E1A,0xB5D2}, /* ҵ: GB2312 Code: 0xD2B5 ==> Row:50 Col:21 */
    {0x4E1B,0xD4B4}, /* ��: GB2312 Code: 0xB4D4 ==> Row:20 Col:52 */
    {0x4E1C,0xABB6}, /* ��: GB2312 Code: 0xB6AB ==> Row:22 Col:11 */
    {0x4E1D,0xBFCB}, /* ˿: GB2312 Code: 0xCBBF ==> Row:43 Col:31 */
    {0x4E1E,0xA9D8}, /* ة: GB2312 Code: 0xD8A9 ==> Row:56 Col:09 */
    {0x4E22,0xAAB6}, /* ��: GB2312 Code: 0xB6AA ==> Row:22 Col:10 */
    {0x4E24,0xBDC1}, /* ��: GB2312 Code: 0xC1BD ==> Row:33 Col:29 */
    {0x4E25,0xCFD1}, /* ��: GB2312 Code: 0xD1CF ==> Row:49 Col:47 */
    {0x4E27,0xA5C9}, /* ɥ: GB2312 Code: 0xC9A5 ==> Row:41 Col:05 */
    {0x4E28,0xADD8}, /* ح: GB2312 Code: 0xD8AD ==> Row:56 Col:13 */
    {0x4E2A,0xF6B8}, /* ��: GB2312 Code: 0xB8F6 ==> Row:24 Col:86 */
    {0x4E2B,0xBED1}, /* Ѿ: GB2312 Code: 0xD1BE ==> Row:49 Col:30 */
    {0x4E2C,0xDCE3}, /* ��: GB2312 Code: 0xE3DC ==> Row:67 Col:60 */
    {0x4E2D,0xD0D6}, /* ��: GB2312 Code: 0xD6D0 ==> Row:54 Col:48 */
    {0x4E30,0xE1B7}, /* ��: GB2312 Code: 0xB7E1 ==> Row:23 Col:65 */
    {0x4E32,0xAEB4}, /* ��: GB2312 Code: 0xB4AE ==> Row:20 Col:14 */
    {0x4E34,0xD9C1}, /* ��: GB2312 Code: 0xC1D9 ==> Row:33 Col:57 */
    {0x4E36,0xBCD8}, /* ؼ: GB2312 Code: 0xD8BC ==> Row:56 Col:28 */
    {0x4E38,0xE8CD}, /* ��: GB2312 Code: 0xCDE8 ==> Row:45 Col:72 */
    {0x4E39,0xA4B5}, /* ��: GB2312 Code: 0xB5A4 ==> Row:21 Col:04 */
    {0x4E3A,0xAACE}, /* Ϊ: GB2312 Code: 0xCEAA ==> Row:46 Col:10 */
    {0x4E3B,0xF7D6}, /* ��: GB2312 Code: 0xD6F7 ==> Row:54 Col:87 */
    {0x4E3D,0xF6C0}, /* ��: GB2312 Code: 0xC0F6 ==> Row:32 Col:86 */
    {0x4E3E,0xD9BE}, /* ��: GB2312 Code: 0xBED9 ==> Row:30 Col:57 */
    {0x4E3F,0xAFD8}, /* د: GB2312 Code: 0xD8AF ==> Row:56 Col:15 */
    {0x4E43,0xCBC4}, /* ��: GB2312 Code: 0xC4CB ==> Row:36 Col:43 */
    {0x4E45,0xC3BE}, /* ��: GB2312 Code: 0xBEC3 ==> Row:30 Col:35 */
    {0x4E47,0xB1D8}, /* ر: GB2312 Code: 0xD8B1 ==> Row:56 Col:17 */
    {0x4E48,0xB4C3}, /* ô: GB2312 Code: 0xC3B4 ==> Row:35 Col:20 */
    {0x4E49,0xE5D2}, /* ��: GB2312 Code: 0xD2E5 ==> Row:50 Col:69 */
    {0x4E4B,0xAED6}, /* ֮: GB2312 Code: 0xD6AE ==> Row:54 Col:14 */
    {0x4E4C,0xDACE}, /* ��: GB2312 Code: 0xCEDA ==> Row:46 Col:58 */
    {0x4E4D,0xA7D5}, /* է: GB2312 Code: 0xD5A7 ==> Row:53 Col:07 */
    {0x4E4E,0xF5BA}, /* ��: GB2312 Code: 0xBAF5 ==> Row:26 Col:85 */
    {0x4E4F,0xA6B7}, /* ��: GB2312 Code: 0xB7A6 ==> Row:23 Col:06 */
    {0x4E50,0xD6C0}, /* ��: GB2312 Code: 0xC0D6 ==> Row:32 Col:54 */
    {0x4E52,0xB9C6}, /* ƹ: GB2312 Code: 0xC6B9 ==> Row:38 Col:25 */
    {0x4E53,0xD2C5}, /* ��: GB2312 Code: 0xC5D2 ==> Row:37 Col:50 */
    {0x4E54,0xC7C7}, /* ��: GB2312 Code: 0xC7C7 ==> Row:39 Col:39 */
    {0x4E56,0xD4B9}, /* ��: GB2312 Code: 0xB9D4 ==> Row:25 Col:52 */
    {0x4E58,0xCBB3}, /* ��: GB2312 Code: 0xB3CB ==> Row:19 Col:43 */
    {0x4E59,0xD2D2}, /* ��: GB2312 Code: 0xD2D2 ==> Row:50 Col:50 */
    {0x4E5C,0xBFD8}, /* ؿ: GB2312 Code: 0xD8BF ==> Row:56 Col:31 */
    {0x4E5D,0xC5BE}, /* ��: GB2312 Code: 0xBEC5 ==> Row:30 Col:37 */
    {0x4E5E,0xF2C6}, /* ��: GB2312 Code: 0xC6F2 ==> Row:38 Col:82 */
    {0x4E5F,0xB2D2}, /* Ҳ: GB2312 Code: 0xD2B2 ==> Row:50 Col:18 */
    {0x4E60,0xB0CF}, /* ϰ: GB2312 Code: 0xCFB0 ==> Row:47 Col:16 */
    {0x4E61,0xE7CF}, /* ��: GB2312 Code: 0xCFE7 ==> Row:47 Col:71 */
    {0x4E66,0xE9CA}, /* ��: GB2312 Code: 0xCAE9 ==> Row:42 Col:73 */
    {0x4E69,0xC0D8}, /* ��: GB2312 Code: 0xD8C0 ==> Row:56 Col:32 */
    {0x4E70,0xF2C2}, /* ��: GB2312 Code: 0xC2F2 ==> Row:34 Col:82 */
    {0x4E71,0xD2C2}, /* ��: GB2312 Code: 0xC2D2 ==> Row:34 Col:50 */
    {0x4E73,0xE9C8}, /* ��: GB2312 Code: 0xC8E9 ==> Row:40 Col:73 */
    {0x4E7E,0xACC7}, /* Ǭ: GB2312 Code: 0xC7AC ==> Row:39 Col:12 */
    {0x4E86,0xCBC1}, /* ��: GB2312 Code: 0xC1CB ==> Row:33 Col:43 */
    {0x4E88,0xE8D3}, /* ��: GB2312 Code: 0xD3E8 ==> Row:51 Col:72 */
    {0x4E89,0xF9D5}, /* ��: GB2312 Code: 0xD5F9 ==> Row:53 Col:89 */
    {0x4E8B,0xC2CA}, /* ��: GB2312 Code: 0xCAC2 ==> Row:42 Col:34 */
    {0x4E8C,0xFEB6}, /* ��: GB2312 Code: 0xB6FE ==> Row:22 Col:94 */
    {0x4E8D,0xA1D8}, /* ء: GB2312 Code: 0xD8A1 ==> Row:56 Col:01 */
    {0x4E8E,0xDAD3}, /* ��: GB2312 Code: 0xD3DA ==> Row:51 Col:58 */
    {0x4E8F,0xF7BF}, /* ��: GB2312 Code: 0xBFF7 ==> Row:31 Col:87 */
    {0x4E91,0xC6D4}, /* ��: GB2312 Code: 0xD4C6 ==> Row:52 Col:38 */
    {0x4E92,0xA5BB}, /* ��: GB2312 Code: 0xBBA5 ==> Row:27 Col:05 */
    {0x4E93,0xC1D8}, /* ��: GB2312 Code: 0xD8C1 ==> Row:56 Col:33 */
    {0x4E94,0xE5CE}, /* ��: GB2312 Code: 0xCEE5 ==> Row:46 Col:69 */
    {0x4E95,0xAEBE}, /* ��: GB2312 Code: 0xBEAE ==> Row:30 Col:14 */
    {0x4E98,0xA8D8}, /* ب: GB2312 Code: 0xD8A8 ==> Row:56 Col:08 */
    {0x4E9A,0xC7D1}, /* ��: GB2312 Code: 0xD1C7 ==> Row:49 Col:39 */
    {0x4E9B,0xA9D0}, /* Щ: GB2312 Code: 0xD0A9 ==> Row:48 Col:09 */
    {0x4E9F,0xBDD8}, /* ؽ: GB2312 Code: 0xD8BD ==> Row:56 Col:29 */
    {0x4EA0,0xEFD9}, /* ��: GB2312 Code: 0xD9EF ==> Row:57 Col:79 */
    {0x4EA1,0xF6CD}, /* ��: GB2312 Code: 0xCDF6 ==> Row:45 Col:86 */
    {0x4EA2,0xBABF}, /* ��: GB2312 Code: 0xBFBA ==> Row:31 Col:26 */
    {0x4EA4,0xBBBD}, /* ��: GB2312 Code: 0xBDBB ==> Row:29 Col:27 */
    {0x4EA5,0xA5BA}, /* ��: GB2312 Code: 0xBAA5 ==> Row:26 Col:05 */
    {0x4EA6,0xE0D2}, /* ��: GB2312 Code: 0xD2E0 ==> Row:50 Col:64 */
    {0x4EA7,0xFAB2}, /* ��: GB2312 Code: 0xB2FA ==> Row:18 Col:90 */
    {0x4EA8,0xE0BA}, /* ��: GB2312 Code: 0xBAE0 ==> Row:26 Col:64 */
    {0x4EA9,0xB6C4}, /* Ķ: GB2312 Code: 0xC4B6 ==> Row:36 Col:22 */
    {0x4EAB,0xEDCF}, /* ��: GB2312 Code: 0xCFED ==> Row:47 Col:77 */
    {0x4EAC,0xA9BE}, /* ��: GB2312 Code: 0xBEA9 ==> Row:30 Col:09 */
    {0x4EAD,0xA4CD}, /* ͤ: GB2312 Code: 0xCDA4 ==> Row:45 Col:04 */
    {0x4EAE,0xC1C1}, /* ��: GB2312 Code: 0xC1C1 ==> Row:33 Col:33 */
    {0x4EB2,0xD7C7}, /* ��: GB2312 Code: 0xC7D7 ==> Row:39 Col:55 */
    {0x4EB3,0xF1D9}, /* ��: GB2312 Code: 0xD9F1 ==> Row:57 Col:81 */
    {0x4EB5,0xF4D9}, /* ��: GB2312 Code: 0xD9F4 ==> Row:57 Col:84 */
    {0x4EBA,0xCBC8}, /* ��: GB2312 Code: 0xC8CB ==> Row:40 Col:43 */
    {0x4EBB,0xE9D8}, /* ��: GB2312 Code: 0xD8E9 ==> Row:56 Col:73 */
    {0x4EBF,0xDAD2}, /* ��: GB2312 Code: 0xD2DA ==> Row:50 Col:58 */
    {0x4EC0,0xB2CA}, /* ʲ: GB2312 Code: 0xCAB2 ==> Row:42 Col:18 */
    {0x4EC1,0xCAC8}, /* ��: GB2312 Code: 0xC8CA ==> Row:40 Col:42 */
    {0x4EC2,0xECD8}, /* ��: GB2312 Code: 0xD8EC ==> Row:56 Col:76 */
    {0x4EC3,0xEAD8}, /* ��: GB2312 Code: 0xD8EA ==> Row:56 Col:74 */
    {0x4EC4,0xC6D8}, /* ��: GB2312 Code: 0xD8C6 ==> Row:56 Col:38 */
    {0x4EC5,0xF6BD}, /* ��: GB2312 Code: 0xBDF6 ==> Row:29 Col:86 */
    {0x4EC6,0xCDC6}, /* ��: GB2312 Code: 0xC6CD ==> Row:38 Col:45 */
    {0x4EC7,0xF0B3}, /* ��: GB2312 Code: 0xB3F0 ==> Row:19 Col:80 */
    {0x4EC9,0xEBD8}, /* ��: GB2312 Code: 0xD8EB ==> Row:56 Col:75 */
    {0x4ECA,0xF1BD}, /* ��: GB2312 Code: 0xBDF1 ==> Row:29 Col:81 */
    {0x4ECB,0xE9BD}, /* ��: GB2312 Code: 0xBDE9 ==> Row:29 Col:73 */
    {0x4ECD,0xD4C8}, /* ��: GB2312 Code: 0xC8D4 ==> Row:40 Col:52 */
    {0x4ECE,0xD3B4}, /* ��: GB2312 Code: 0xB4D3 ==> Row:20 Col:51 */
    {0x4ED1,0xD8C2}, /* ��: GB2312 Code: 0xC2D8 ==> Row:34 Col:56 */
    {0x4ED3,0xD6B2}, /* ��: GB2312 Code: 0xB2D6 ==> Row:18 Col:54 */
    {0x4ED4,0xD0D7}, /* ��: GB2312 Code: 0xD7D0 ==> Row:55 Col:48 */
    {0x4ED5,0xCBCA}, /* ��: GB2312 Code: 0xCACB ==> Row:42 Col:43 */
    {0x4ED6,0xFBCB}, /* ��: GB2312 Code: 0xCBFB ==> Row:43 Col:91 */
    {0x4ED7,0xCCD5}, /* ��: GB2312 Code: 0xD5CC ==> Row:53 Col:44 */
    {0x4ED8,0xB6B8}, /* ��: GB2312 Code: 0xB8B6 ==> Row:24 Col:22 */
    {0x4ED9,0xC9CF}, /* ��: GB2312 Code: 0xCFC9 ==> Row:47 Col:41 */
    {0x4EDD,0xDAD9}, /* ��: GB2312 Code: 0xD9DA ==> Row:57 Col:58 */
    {0x4EDE,0xF0D8}, /* ��: GB2312 Code: 0xD8F0 ==> Row:56 Col:80 */
    {0x4EDF,0xAAC7}, /* Ǫ: GB2312 Code: 0xC7AA ==> Row:39 Col:10 */
    {0x4EE1,0xEED8}, /* ��: GB2312 Code: 0xD8EE ==> Row:56 Col:78 */
    {0x4EE3,0xFAB4}, /* ��: GB2312 Code: 0xB4FA ==> Row:20 Col:90 */
    {0x4EE4,0xEEC1}, /* ��: GB2312 Code: 0xC1EE ==> Row:33 Col:78 */
    {0x4EE5,0xD4D2}, /* ��: GB2312 Code: 0xD2D4 ==> Row:50 Col:52 */
    {0x4EE8,0xEDD8}, /* ��: GB2312 Code: 0xD8ED ==> Row:56 Col:77 */
    {0x4EEA,0xC7D2}, /* ��: GB2312 Code: 0xD2C7 ==> Row:50 Col:39 */
    {0x4EEB,0xEFD8}, /* ��: GB2312 Code: 0xD8EF ==> Row:56 Col:79 */
    {0x4EEC,0xC7C3}, /* ��: GB2312 Code: 0xC3C7 ==> Row:35 Col:39 */
    {0x4EF0,0xF6D1}, /* ��: GB2312 Code: 0xD1F6 ==> Row:49 Col:86 */
    {0x4EF2,0xD9D6}, /* ��: GB2312 Code: 0xD6D9 ==> Row:54 Col:57 */
    {0x4EF3,0xF2D8}, /* ��: GB2312 Code: 0xD8F2 ==> Row:56 Col:82 */
    {0x4EF5,0xF5D8}, /* ��: GB2312 Code: 0xD8F5 ==> Row:56 Col:85 */
    {0x4EF6,0xFEBC}, /* ��: GB2312 Code: 0xBCFE ==> Row:28 Col:94 */
    {0x4EF7,0xDBBC}, /* ��: GB2312 Code: 0xBCDB ==> Row:28 Col:59 */
    {0x4EFB,0xCEC8}, /* ��: GB2312 Code: 0xC8CE ==> Row:40 Col:46 */
    {0x4EFD,0xDDB7}, /* ��: GB2312 Code: 0xB7DD ==> Row:23 Col:61 */
    {0x4EFF,0xC2B7}, /* ��: GB2312 Code: 0xB7C2 ==> Row:23 Col:34 */
    {0x4F01,0xF3C6}, /* ��: GB2312 Code: 0xC6F3 ==> Row:38 Col:83 */
    {0x4F09,0xF8D8}, /* ��: GB2312 Code: 0xD8F8 ==> Row:56 Col:88 */
    {0x4F0A,0xC1D2}, /* ��: GB2312 Code: 0xD2C1 ==> Row:50 Col:33 */
    {0x4F0D,0xE9CE}, /* ��: GB2312 Code: 0xCEE9 ==> Row:46 Col:73 */
    {0x4F0E,0xBFBC}, /* ��: GB2312 Code: 0xBCBF ==> Row:28 Col:31 */
    {0x4F0F,0xFCB7}, /* ��: GB2312 Code: 0xB7FC ==> Row:23 Col:92 */
    {0x4F10,0xA5B7}, /* ��: GB2312 Code: 0xB7A5 ==> Row:23 Col:05 */
    {0x4F11,0xDDD0}, /* ��: GB2312 Code: 0xD0DD ==> Row:48 Col:61 */
    {0x4F17,0xDAD6}, /* ��: GB2312 Code: 0xD6DA ==> Row:54 Col:58 */
    {0x4F18,0xC5D3}, /* ��: GB2312 Code: 0xD3C5 ==> Row:51 Col:37 */
    {0x4F19,0xEFBB}, /* ��: GB2312 Code: 0xBBEF ==> Row:27 Col:79 */
    {0x4F1A,0xE1BB}, /* ��: GB2312 Code: 0xBBE1 ==> Row:27 Col:65 */
    {0x4F1B,0xF1D8}, /* ��: GB2312 Code: 0xD8F1 ==> Row:56 Col:81 */
    {0x4F1E,0xA1C9}, /* ɡ: GB2312 Code: 0xC9A1 ==> Row:41 Col:01 */
    {0x4F1F,0xB0CE}, /* ΰ: GB2312 Code: 0xCEB0 ==> Row:46 Col:16 */
    {0x4F20,0xABB4}, /* ��: GB2312 Code: 0xB4AB ==> Row:20 Col:11 */
    {0x4F22,0xF3D8}, /* ��: GB2312 Code: 0xD8F3 ==> Row:56 Col:83 */
    {0x4F24,0xCBC9}, /* ��: GB2312 Code: 0xC9CB ==> Row:41 Col:43 */
    {0x4F25,0xF6D8}, /* ��: GB2312 Code: 0xD8F6 ==> Row:56 Col:86 */
    {0x4F26,0xD7C2}, /* ��: GB2312 Code: 0xC2D7 ==> Row:34 Col:55 */
    {0x4F27,0xF7D8}, /* ��: GB2312 Code: 0xD8F7 ==> Row:56 Col:87 */
    {0x4F2A,0xB1CE}, /* α: GB2312 Code: 0xCEB1 ==> Row:46 Col:17 */
    {0x4F2B,0xF9D8}, /* ��: GB2312 Code: 0xD8F9 ==> Row:56 Col:89 */
    {0x4F2F,0xAEB2}, /* ��: GB2312 Code: 0xB2AE ==> Row:18 Col:14 */
    {0x4F30,0xC0B9}, /* ��: GB2312 Code: 0xB9C0 ==> Row:25 Col:32 */
    {0x4F32,0xA3D9}, /* ٣: GB2312 Code: 0xD9A3 ==> Row:57 Col:03 */
    {0x4F34,0xE9B0}, /* ��: GB2312 Code: 0xB0E9 ==> Row:16 Col:73 */
    {0x4F36,0xE6C1}, /* ��: GB2312 Code: 0xC1E6 ==> Row:33 Col:70 */
    {0x4F38,0xECC9}, /* ��: GB2312 Code: 0xC9EC ==> Row:41 Col:76 */
    {0x4F3A,0xC5CB}, /* ��: GB2312 Code: 0xCBC5 ==> Row:43 Col:37 */
    {0x4F3C,0xC6CB}, /* ��: GB2312 Code: 0xCBC6 ==> Row:43 Col:38 */
    {0x4F3D,0xA4D9}, /* ٤: GB2312 Code: 0xD9A4 ==> Row:57 Col:04 */
    {0x4F43,0xE8B5}, /* ��: GB2312 Code: 0xB5E8 ==> Row:21 Col:72 */
    {0x4F46,0xABB5}, /* ��: GB2312 Code: 0xB5AB ==> Row:21 Col:11 */
    {0x4F4D,0xBBCE}, /* λ: GB2312 Code: 0xCEBB ==> Row:46 Col:27 */
    {0x4F4E,0xCDB5}, /* ��: GB2312 Code: 0xB5CD ==> Row:21 Col:45 */
    {0x4F4F,0xA1D7}, /* ס: GB2312 Code: 0xD7A1 ==> Row:55 Col:01 */
    {0x4F50,0xF4D7}, /* ��: GB2312 Code: 0xD7F4 ==> Row:55 Col:84 */
    {0x4F51,0xD3D3}, /* ��: GB2312 Code: 0xD3D3 ==> Row:51 Col:51 */
    {0x4F53,0xE5CC}, /* ��: GB2312 Code: 0xCCE5 ==> Row:44 Col:69 */
    {0x4F55,0xCEBA}, /* ��: GB2312 Code: 0xBACE ==> Row:26 Col:46 */
    {0x4F57,0xA2D9}, /* ٢: GB2312 Code: 0xD9A2 ==> Row:57 Col:02 */
    {0x4F58,0xDCD9}, /* ��: GB2312 Code: 0xD9DC ==> Row:57 Col:60 */
    {0x4F59,0xE0D3}, /* ��: GB2312 Code: 0xD3E0 ==> Row:51 Col:64 */
    {0x4F5A,0xFDD8}, /* ��: GB2312 Code: 0xD8FD ==> Row:56 Col:93 */
    {0x4F5B,0xF0B7}, /* ��: GB2312 Code: 0xB7F0 ==> Row:23 Col:80 */
    {0x4F5C,0xF7D7}, /* ��: GB2312 Code: 0xD7F7 ==> Row:55 Col:87 */
    {0x4F5D,0xFED8}, /* ��: GB2312 Code: 0xD8FE ==> Row:56 Col:94 */
    {0x4F5E,0xFAD8}, /* ��: GB2312 Code: 0xD8FA ==> Row:56 Col:90 */
    {0x4F5F,0xA1D9}, /* ١: GB2312 Code: 0xD9A1 ==> Row:57 Col:01 */
    {0x4F60,0xE3C4}, /* ��: GB2312 Code: 0xC4E3 ==> Row:36 Col:67 */
    {0x4F63,0xB6D3}, /* Ӷ: GB2312 Code: 0xD3B6 ==> Row:51 Col:22 */
    {0x4F64,0xF4D8}, /* ��: GB2312 Code: 0xD8F4 ==> Row:56 Col:84 */
    {0x4F65,0xDDD9}, /* ��: GB2312 Code: 0xD9DD ==> Row:57 Col:61 */
    {0x4F67,0xFBD8}, /* ��: GB2312 Code: 0xD8FB ==> Row:56 Col:91 */
    {0x4F69,0xE5C5}, /* ��: GB2312 Code: 0xC5E5 ==> Row:37 Col:69 */
    {0x4F6C,0xD0C0}, /* ��: GB2312 Code: 0xC0D0 ==> Row:32 Col:48 */
    {0x4F6F,0xF0D1}, /* ��: GB2312 Code: 0xD1F0 ==> Row:49 Col:80 */
    {0x4F70,0xDBB0}, /* ��: GB2312 Code: 0xB0DB ==> Row:16 Col:59 */
    {0x4F73,0xD1BC}, /* ��: GB2312 Code: 0xBCD1 ==> Row:28 Col:49 */
    {0x4F74,0xA6D9}, /* ٦: GB2312 Code: 0xD9A6 ==> Row:57 Col:06 */
    {0x4F76,0xA5D9}, /* ٥: GB2312 Code: 0xD9A5 ==> Row:57 Col:05 */
    {0x4F7B,0xACD9}, /* ٬: GB2312 Code: 0xD9AC ==> Row:57 Col:12 */
    {0x4F7C,0xAED9}, /* ٮ: GB2312 Code: 0xD9AE ==> Row:57 Col:14 */
    {0x4F7E,0xABD9}, /* ٫: GB2312 Code: 0xD9AB ==> Row:57 Col:11 */
    {0x4F7F,0xB9CA}, /* ʹ: GB2312 Code: 0xCAB9 ==> Row:42 Col:25 */
    {0x4F83,0xA9D9}, /* ٩: GB2312 Code: 0xD9A9 ==> Row:57 Col:09 */
    {0x4F84,0xB6D6}, /* ֶ: GB2312 Code: 0xD6B6 ==> Row:54 Col:22 */
    {0x4F88,0xDEB3}, /* ��: GB2312 Code: 0xB3DE ==> Row:19 Col:62 */
    {0x4F89,0xA8D9}, /* ٨: GB2312 Code: 0xD9A8 ==> Row:57 Col:08 */
    {0x4F8B,0xFDC0}, /* ��: GB2312 Code: 0xC0FD ==> Row:32 Col:93 */
    {0x4F8D,0xCCCA}, /* ��: GB2312 Code: 0xCACC ==> Row:42 Col:44 */
    {0x4F8F,0xAAD9}, /* ٪: GB2312 Code: 0xD9AA ==> Row:57 Col:10 */
    {0x4F91,0xA7D9}, /* ٧: GB2312 Code: 0xD9A7 ==> Row:57 Col:07 */
    {0x4F94,0xB0D9}, /* ٰ: GB2312 Code: 0xD9B0 ==> Row:57 Col:16 */
    {0x4F97,0xB1B6}, /* ��: GB2312 Code: 0xB6B1 ==> Row:22 Col:17 */
    {0x4F9B,0xA9B9}, /* ��: GB2312 Code: 0xB9A9 ==> Row:25 Col:09 */
    {0x4F9D,0xC0D2}, /* ��: GB2312 Code: 0xD2C0 ==> Row:50 Col:32 */
    {0x4FA0,0xC0CF}, /* ��: GB2312 Code: 0xCFC0 ==> Row:47 Col:32 */
    {0x4FA3,0xC2C2}, /* ��: GB2312 Code: 0xC2C2 ==> Row:34 Col:34 */
    {0x4FA5,0xC4BD}, /* ��: GB2312 Code: 0xBDC4 ==> Row:29 Col:36 */
    {0x4FA6,0xECD5}, /* ��: GB2312 Code: 0xD5EC ==> Row:53 Col:76 */
    {0x4FA7,0xE0B2}, /* ��: GB2312 Code: 0xB2E0 ==> Row:18 Col:64 */
    {0x4FA8,0xC8C7}, /* ��: GB2312 Code: 0xC7C8 ==> Row:39 Col:40 */
    {0x4FA9,0xEBBF}, /* ��: GB2312 Code: 0xBFEB ==> Row:31 Col:75 */
    {0x4FAA,0xADD9}, /* ٭: GB2312 Code: 0xD9AD ==> Row:57 Col:13 */
    {0x4FAC,0xAFD9}, /* ٯ: GB2312 Code: 0xD9AF ==> Row:57 Col:15 */
    {0x4FAE,0xEACE}, /* ��: GB2312 Code: 0xCEEA ==> Row:46 Col:74 */
    {0x4FAF,0xEEBA}, /* ��: GB2312 Code: 0xBAEE ==> Row:26 Col:78 */
    {0x4FB5,0xD6C7}, /* ��: GB2312 Code: 0xC7D6 ==> Row:39 Col:54 */
    {0x4FBF,0xE3B1}, /* ��: GB2312 Code: 0xB1E3 ==> Row:17 Col:67 */
    {0x4FC3,0xD9B4}, /* ��: GB2312 Code: 0xB4D9 ==> Row:20 Col:57 */
    {0x4FC4,0xEDB6}, /* ��: GB2312 Code: 0xB6ED ==> Row:22 Col:77 */
    {0x4FC5,0xB4D9}, /* ٴ: GB2312 Code: 0xD9B4 ==> Row:57 Col:20 */
    {0x4FCA,0xA1BF}, /* ��: GB2312 Code: 0xBFA1 ==> Row:31 Col:01 */
    {0x4FCE,0xDED9}, /* ��: GB2312 Code: 0xD9DE ==> Row:57 Col:62 */
    {0x4FCF,0xCEC7}, /* ��: GB2312 Code: 0xC7CE ==> Row:39 Col:46 */
    {0x4FD0,0xFEC0}, /* ��: GB2312 Code: 0xC0FE ==> Row:32 Col:94 */
    {0x4FD1,0xB8D9}, /* ٸ: GB2312 Code: 0xD9B8 ==> Row:57 Col:24 */
    {0x4FD7,0xD7CB}, /* ��: GB2312 Code: 0xCBD7 ==> Row:43 Col:55 */
    {0x4FD8,0xFDB7}, /* ��: GB2312 Code: 0xB7FD ==> Row:23 Col:93 */
    {0x4FDA,0xB5D9}, /* ٵ: GB2312 Code: 0xD9B5 ==> Row:57 Col:21 */
    {0x4FDC,0xB7D9}, /* ٷ: GB2312 Code: 0xD9B7 ==> Row:57 Col:23 */
    {0x4FDD,0xA3B1}, /* ��: GB2312 Code: 0xB1A3 ==> Row:17 Col:03 */
    {0x4FDE,0xE1D3}, /* ��: GB2312 Code: 0xD3E1 ==> Row:51 Col:65 */
    {0x4FDF,0xB9D9}, /* ٹ: GB2312 Code: 0xD9B9 ==> Row:57 Col:25 */
    {0x4FE1,0xC5D0}, /* ��: GB2312 Code: 0xD0C5 ==> Row:48 Col:37 */
    {0x4FE3,0xB6D9}, /* ٶ: GB2312 Code: 0xD9B6 ==> Row:57 Col:22 */
    {0x4FE6,0xB1D9}, /* ٱ: GB2312 Code: 0xD9B1 ==> Row:57 Col:17 */
    {0x4FE8,0xB2D9}, /* ٲ: GB2312 Code: 0xD9B2 ==> Row:57 Col:18 */
    {0x4FE9,0xA9C1}, /* ��: GB2312 Code: 0xC1A9 ==> Row:33 Col:09 */
    {0x4FEA,0xB3D9}, /* ٳ: GB2312 Code: 0xD9B3 ==> Row:57 Col:19 */
    {0x4FED,0xF3BC}, /* ��: GB2312 Code: 0xBCF3 ==> Row:28 Col:83 */
    {0x4FEE,0xDED0}, /* ��: GB2312 Code: 0xD0DE ==> Row:48 Col:62 */
    {0x4FEF,0xA9B8}, /* ��: GB2312 Code: 0xB8A9 ==> Row:24 Col:09 */
    {0x4FF1,0xE3BE}, /* ��: GB2312 Code: 0xBEE3 ==> Row:30 Col:67 */
    {0x4FF3,0xBDD9}, /* ٽ: GB2312 Code: 0xD9BD ==> Row:57 Col:29 */
    {0x4FF8,0xBAD9}, /* ٺ: GB2312 Code: 0xD9BA ==> Row:57 Col:26 */
    {0x4FFA,0xB3B0}, /* ��: GB2312 Code: 0xB0B3 ==> Row:16 Col:19 */
    {0x4FFE,0xC2D9}, /* ��: GB2312 Code: 0xD9C2 ==> Row:57 Col:34 */
    {0x500C,0xC4D9}, /* ��: GB2312 Code: 0xD9C4 ==> Row:57 Col:36 */
    {0x500D,0xB6B1}, /* ��: GB2312 Code: 0xB1B6 ==> Row:17 Col:22 */
    {0x500F,0xBFD9}, /* ٿ: GB2312 Code: 0xD9BF ==> Row:57 Col:31 */
    {0x5012,0xB9B5}, /* ��: GB2312 Code: 0xB5B9 ==> Row:21 Col:25 */
    {0x5014,0xF3BE}, /* ��: GB2312 Code: 0xBEF3 ==> Row:30 Col:83 */
    {0x5018,0xC8CC}, /* ��: GB2312 Code: 0xCCC8 ==> Row:44 Col:40 */
    {0x5019,0xF2BA}, /* ��: GB2312 Code: 0xBAF2 ==> Row:26 Col:82 */
    {0x501A,0xD0D2}, /* ��: GB2312 Code: 0xD2D0 ==> Row:50 Col:48 */
    {0x501C,0xC3D9}, /* ��: GB2312 Code: 0xD9C3 ==> Row:57 Col:35 */
    {0x501F,0xE8BD}, /* ��: GB2312 Code: 0xBDE8 ==> Row:29 Col:72 */
    {0x5021,0xABB3}, /* ��: GB2312 Code: 0xB3AB ==> Row:19 Col:11 */
    {0x5025,0xC5D9}, /* ��: GB2312 Code: 0xD9C5 ==> Row:57 Col:37 */
    {0x5026,0xEBBE}, /* ��: GB2312 Code: 0xBEEB ==> Row:30 Col:75 */
    {0x5028,0xC6D9}, /* ��: GB2312 Code: 0xD9C6 ==> Row:57 Col:38 */
    {0x5029,0xBBD9}, /* ٻ: GB2312 Code: 0xD9BB ==> Row:57 Col:27 */
    {0x502A,0xDFC4}, /* ��: GB2312 Code: 0xC4DF ==> Row:36 Col:63 */
    {0x502C,0xBED9}, /* پ: GB2312 Code: 0xD9BE ==> Row:57 Col:30 */
    {0x502D,0xC1D9}, /* ��: GB2312 Code: 0xD9C1 ==> Row:57 Col:33 */
    {0x502E,0xC0D9}, /* ��: GB2312 Code: 0xD9C0 ==> Row:57 Col:32 */
    {0x503A,0xAED5}, /* ծ: GB2312 Code: 0xD5AE ==> Row:53 Col:14 */
    {0x503C,0xB5D6}, /* ֵ: GB2312 Code: 0xD6B5 ==> Row:54 Col:21 */
    {0x503E,0xE3C7}, /* ��: GB2312 Code: 0xC7E3 ==> Row:39 Col:67 */
    {0x5043,0xC8D9}, /* ��: GB2312 Code: 0xD9C8 ==> Row:57 Col:40 */
    {0x5047,0xD9BC}, /* ��: GB2312 Code: 0xBCD9 ==> Row:28 Col:57 */
    {0x5048,0xCAD9}, /* ��: GB2312 Code: 0xD9CA ==> Row:57 Col:42 */
    {0x504C,0xBCD9}, /* ټ: GB2312 Code: 0xD9BC ==> Row:57 Col:28 */
    {0x504E,0xCBD9}, /* ��: GB2312 Code: 0xD9CB ==> Row:57 Col:43 */
    {0x504F,0xABC6}, /* ƫ: GB2312 Code: 0xC6AB ==> Row:38 Col:11 */
    {0x5055,0xC9D9}, /* ��: GB2312 Code: 0xD9C9 ==> Row:57 Col:41 */
    {0x505A,0xF6D7}, /* ��: GB2312 Code: 0xD7F6 ==> Row:55 Col:86 */
    {0x505C,0xA3CD}, /* ͣ: GB2312 Code: 0xCDA3 ==> Row:45 Col:03 */
    {0x5065,0xA1BD}, /* ��: GB2312 Code: 0xBDA1 ==> Row:29 Col:01 */
    {0x506C,0xCCD9}, /* ��: GB2312 Code: 0xD9CC ==> Row:57 Col:44 */
    {0x5076,0xBCC5}, /* ż: GB2312 Code: 0xC5BC ==> Row:37 Col:28 */
    {0x5077,0xB5CD}, /* ͵: GB2312 Code: 0xCDB5 ==> Row:45 Col:21 */
    {0x507B,0xCDD9}, /* ��: GB2312 Code: 0xD9CD ==> Row:57 Col:45 */
    {0x507E,0xC7D9}, /* ��: GB2312 Code: 0xD9C7 ==> Row:57 Col:39 */
    {0x507F,0xA5B3}, /* ��: GB2312 Code: 0xB3A5 ==> Row:19 Col:05 */
    {0x5080,0xFEBF}, /* ��: GB2312 Code: 0xBFFE ==> Row:31 Col:94 */
    {0x5085,0xB5B8}, /* ��: GB2312 Code: 0xB8B5 ==> Row:24 Col:21 */
    {0x5088,0xFCC0}, /* ��: GB2312 Code: 0xC0FC ==> Row:32 Col:92 */
    {0x508D,0xF8B0}, /* ��: GB2312 Code: 0xB0F8 ==> Row:16 Col:88 */
    {0x50A3,0xF6B4}, /* ��: GB2312 Code: 0xB4F6 ==> Row:20 Col:86 */
    {0x50A5,0xCED9}, /* ��: GB2312 Code: 0xD9CE ==> Row:57 Col:46 */
    {0x50A7,0xCFD9}, /* ��: GB2312 Code: 0xD9CF ==> Row:57 Col:47 */
    {0x50A8,0xA2B4}, /* ��: GB2312 Code: 0xB4A2 ==> Row:20 Col:02 */
    {0x50A9,0xD0D9}, /* ��: GB2312 Code: 0xD9D0 ==> Row:57 Col:48 */
    {0x50AC,0xDFB4}, /* ��: GB2312 Code: 0xB4DF ==> Row:20 Col:63 */
    {0x50B2,0xC1B0}, /* ��: GB2312 Code: 0xB0C1 ==> Row:16 Col:33 */
    {0x50BA,0xD1D9}, /* ��: GB2312 Code: 0xD9D1 ==> Row:57 Col:49 */
    {0x50BB,0xB5C9}, /* ɵ: GB2312 Code: 0xC9B5 ==> Row:41 Col:21 */
    {0x50CF,0xF1CF}, /* ��: GB2312 Code: 0xCFF1 ==> Row:47 Col:81 */
    {0x50D6,0xD2D9}, /* ��: GB2312 Code: 0xD9D2 ==> Row:57 Col:50 */
    {0x50DA,0xC5C1}, /* ��: GB2312 Code: 0xC1C5 ==> Row:33 Col:37 */
    {0x50E6,0xD6D9}, /* ��: GB2312 Code: 0xD9D6 ==> Row:57 Col:54 */
    {0x50E7,0xAEC9}, /* ɮ: GB2312 Code: 0xC9AE ==> Row:41 Col:14 */
    {0x50EC,0xD5D9}, /* ��: GB2312 Code: 0xD9D5 ==> Row:57 Col:53 */
    {0x50ED,0xD4D9}, /* ��: GB2312 Code: 0xD9D4 ==> Row:57 Col:52 */
    {0x50EE,0xD7D9}, /* ��: GB2312 Code: 0xD9D7 ==> Row:57 Col:55 */
    {0x50F3,0xDBCB}, /* ��: GB2312 Code: 0xCBDB ==> Row:43 Col:59 */
    {0x50F5,0xA9BD}, /* ��: GB2312 Code: 0xBDA9 ==> Row:29 Col:09 */
    {0x50FB,0xA7C6}, /* Ƨ: GB2312 Code: 0xC6A7 ==> Row:38 Col:07 */
    {0x5106,0xD3D9}, /* ��: GB2312 Code: 0xD9D3 ==> Row:57 Col:51 */
    {0x5107,0xD8D9}, /* ��: GB2312 Code: 0xD9D8 ==> Row:57 Col:56 */
    {0x510B,0xD9D9}, /* ��: GB2312 Code: 0xD9D9 ==> Row:57 Col:57 */
    {0x5112,0xE5C8}, /* ��: GB2312 Code: 0xC8E5 ==> Row:40 Col:69 */
    {0x5121,0xDCC0}, /* ��: GB2312 Code: 0xC0DC ==> Row:32 Col:60 */
    {0x513F,0xF9B6}, /* ��: GB2312 Code: 0xB6F9 ==> Row:22 Col:89 */
    {0x5140,0xA3D8}, /* أ: GB2312 Code: 0xD8A3 ==> Row:56 Col:03 */
    {0x5141,0xCAD4}, /* ��: GB2312 Code: 0xD4CA ==> Row:52 Col:42 */
    {0x5143,0xAAD4}, /* Ԫ: GB2312 Code: 0xD4AA ==> Row:52 Col:10 */
    {0x5144,0xD6D0}, /* ��: GB2312 Code: 0xD0D6 ==> Row:48 Col:54 */
    {0x5145,0xE4B3}, /* ��: GB2312 Code: 0xB3E4 ==> Row:19 Col:68 */
    {0x5146,0xD7D5}, /* ��: GB2312 Code: 0xD5D7 ==> Row:53 Col:55 */
    {0x5148,0xC8CF}, /* ��: GB2312 Code: 0xCFC8 ==> Row:47 Col:40 */
    {0x5149,0xE2B9}, /* ��: GB2312 Code: 0xB9E2 ==> Row:25 Col:66 */
    {0x514B,0xCBBF}, /* ��: GB2312 Code: 0xBFCB ==> Row:31 Col:43 */
    {0x514D,0xE2C3}, /* ��: GB2312 Code: 0xC3E2 ==> Row:35 Col:66 */
    {0x5151,0xD2B6}, /* ��: GB2312 Code: 0xB6D2 ==> Row:22 Col:50 */
    {0x5154,0xC3CD}, /* ��: GB2312 Code: 0xCDC3 ==> Row:45 Col:35 */
    {0x5155,0xEED9}, /* ��: GB2312 Code: 0xD9EE ==> Row:57 Col:78 */
    {0x5156,0xF0D9}, /* ��: GB2312 Code: 0xD9F0 ==> Row:57 Col:80 */
    {0x515A,0xB3B5}, /* ��: GB2312 Code: 0xB5B3 ==> Row:21 Col:19 */
    {0x515C,0xB5B6}, /* ��: GB2312 Code: 0xB6B5 ==> Row:22 Col:21 */
    {0x5162,0xA4BE}, /* ��: GB2312 Code: 0xBEA4 ==> Row:30 Col:04 */
    {0x5165,0xEBC8}, /* ��: GB2312 Code: 0xC8EB ==> Row:40 Col:75 */
    {0x5168,0xABC8}, /* ȫ: GB2312 Code: 0xC8AB ==> Row:40 Col:11 */
    {0x516B,0xCBB0}, /* ��: GB2312 Code: 0xB0CB ==> Row:16 Col:43 */
    {0x516C,0xABB9}, /* ��: GB2312 Code: 0xB9AB ==> Row:25 Col:11 */
    {0x516D,0xF9C1}, /* ��: GB2312 Code: 0xC1F9 ==> Row:33 Col:89 */
    {0x516E,0xE2D9}, /* ��: GB2312 Code: 0xD9E2 ==> Row:57 Col:66 */
    {0x5170,0xBCC0}, /* ��: GB2312 Code: 0xC0BC ==> Row:32 Col:28 */
    {0x5171,0xB2B9}, /* ��: GB2312 Code: 0xB9B2 ==> Row:25 Col:18 */
    {0x5173,0xD8B9}, /* ��: GB2312 Code: 0xB9D8 ==> Row:25 Col:56 */
    {0x5174,0xCBD0}, /* ��: GB2312 Code: 0xD0CB ==> Row:48 Col:43 */
    {0x5175,0xF8B1}, /* ��: GB2312 Code: 0xB1F8 ==> Row:17 Col:88 */
    {0x5176,0xE4C6}, /* ��: GB2312 Code: 0xC6E4 ==> Row:38 Col:68 */
    {0x5177,0xDFBE}, /* ��: GB2312 Code: 0xBEDF ==> Row:30 Col:63 */
    {0x5178,0xE4B5}, /* ��: GB2312 Code: 0xB5E4 ==> Row:21 Col:68 */
    {0x5179,0xC8D7}, /* ��: GB2312 Code: 0xD7C8 ==> Row:55 Col:40 */
    {0x517B,0xF8D1}, /* ��: GB2312 Code: 0xD1F8 ==> Row:49 Col:88 */
    {0x517C,0xE6BC}, /* ��: GB2312 Code: 0xBCE6 ==> Row:28 Col:70 */
    {0x517D,0xDECA}, /* ��: GB2312 Code: 0xCADE ==> Row:42 Col:62 */
    {0x5180,0xBDBC}, /* ��: GB2312 Code: 0xBCBD ==> Row:28 Col:29 */
    {0x5181,0xE6D9}, /* ��: GB2312 Code: 0xD9E6 ==> Row:57 Col:70 */
    {0x5182,0xE7D8}, /* ��: GB2312 Code: 0xD8E7 ==> Row:56 Col:71 */
    {0x5185,0xDAC4}, /* ��: GB2312 Code: 0xC4DA ==> Row:36 Col:58 */
    {0x5188,0xD4B8}, /* ��: GB2312 Code: 0xB8D4 ==> Row:24 Col:52 */
    {0x5189,0xBDC8}, /* Ƚ: GB2312 Code: 0xC8BD ==> Row:40 Col:29 */
    {0x518C,0xE1B2}, /* ��: GB2312 Code: 0xB2E1 ==> Row:18 Col:65 */
    {0x518D,0xD9D4}, /* ��: GB2312 Code: 0xD4D9 ==> Row:52 Col:57 */
    {0x5192,0xB0C3}, /* ð: GB2312 Code: 0xC3B0 ==> Row:35 Col:16 */
    {0x5195,0xE1C3}, /* ��: GB2312 Code: 0xC3E1 ==> Row:35 Col:65 */
    {0x5196,0xA2DA}, /* ڢ: GB2312 Code: 0xDAA2 ==> Row:58 Col:02 */
    {0x5197,0xDFC8}, /* ��: GB2312 Code: 0xC8DF ==> Row:40 Col:63 */
    {0x5199,0xB4D0}, /* д: GB2312 Code: 0xD0B4 ==> Row:48 Col:20 */
    {0x519B,0xFCBE}, /* ��: GB2312 Code: 0xBEFC ==> Row:30 Col:92 */
    {0x519C,0xA9C5}, /* ũ: GB2312 Code: 0xC5A9 ==> Row:37 Col:09 */
    {0x51A0,0xDAB9}, /* ��: GB2312 Code: 0xB9DA ==> Row:25 Col:58 */
    {0x51A2,0xA3DA}, /* ڣ: GB2312 Code: 0xDAA3 ==> Row:58 Col:03 */
    {0x51A4,0xA9D4}, /* ԩ: GB2312 Code: 0xD4A9 ==> Row:52 Col:09 */
    {0x51A5,0xA4DA}, /* ڤ: GB2312 Code: 0xDAA4 ==> Row:58 Col:04 */
    {0x51AB,0xFBD9}, /* ��: GB2312 Code: 0xD9FB ==> Row:57 Col:91 */
    {0x51AC,0xACB6}, /* ��: GB2312 Code: 0xB6AC ==> Row:22 Col:12 */
    {0x51AF,0xEBB7}, /* ��: GB2312 Code: 0xB7EB ==> Row:23 Col:75 */
    {0x51B0,0xF9B1}, /* ��: GB2312 Code: 0xB1F9 ==> Row:17 Col:89 */
    {0x51B1,0xFCD9}, /* ��: GB2312 Code: 0xD9FC ==> Row:57 Col:92 */
    {0x51B2,0xE5B3}, /* ��: GB2312 Code: 0xB3E5 ==> Row:19 Col:69 */
    {0x51B3,0xF6BE}, /* ��: GB2312 Code: 0xBEF6 ==> Row:30 Col:86 */
    {0x51B5,0xF6BF}, /* ��: GB2312 Code: 0xBFF6 ==> Row:31 Col:86 */
    {0x51B6,0xB1D2}, /* ұ: GB2312 Code: 0xD2B1 ==> Row:50 Col:17 */
    {0x51B7,0xE4C0}, /* ��: GB2312 Code: 0xC0E4 ==> Row:32 Col:68 */
    {0x51BB,0xB3B6}, /* ��: GB2312 Code: 0xB6B3 ==> Row:22 Col:19 */
    {0x51BC,0xFED9}, /* ��: GB2312 Code: 0xD9FE ==> Row:57 Col:94 */
    {0x51BD,0xFDD9}, /* ��: GB2312 Code: 0xD9FD ==> Row:57 Col:93 */
    {0x51C0,0xBBBE}, /* ��: GB2312 Code: 0xBEBB ==> Row:30 Col:27 */
    {0x51C4,0xE0C6}, /* ��: GB2312 Code: 0xC6E0 ==> Row:38 Col:64 */
    {0x51C6,0xBCD7}, /* ׼: GB2312 Code: 0xD7BC ==> Row:55 Col:28 */
    {0x51C7,0xA1DA}, /* ڡ: GB2312 Code: 0xDAA1 ==> Row:58 Col:01 */
    {0x51C9,0xB9C1}, /* ��: GB2312 Code: 0xC1B9 ==> Row:33 Col:25 */
    {0x51CB,0xF2B5}, /* ��: GB2312 Code: 0xB5F2 ==> Row:21 Col:82 */
    {0x51CC,0xE8C1}, /* ��: GB2312 Code: 0xC1E8 ==> Row:33 Col:72 */
    {0x51CF,0xF5BC}, /* ��: GB2312 Code: 0xBCF5 ==> Row:28 Col:85 */
    {0x51D1,0xD5B4}, /* ��: GB2312 Code: 0xB4D5 ==> Row:20 Col:53 */
    {0x51DB,0xDDC1}, /* ��: GB2312 Code: 0xC1DD ==> Row:33 Col:61 */
    {0x51DD,0xFDC4}, /* ��: GB2312 Code: 0xC4FD ==> Row:36 Col:93 */
    {0x51E0,0xB8BC}, /* ��: GB2312 Code: 0xBCB8 ==> Row:28 Col:24 */
    {0x51E1,0xB2B7}, /* ��: GB2312 Code: 0xB7B2 ==> Row:23 Col:18 */
    {0x51E4,0xEFB7}, /* ��: GB2312 Code: 0xB7EF ==> Row:23 Col:79 */
    {0x51EB,0xECD9}, /* ��: GB2312 Code: 0xD9EC ==> Row:57 Col:76 */
    {0x51ED,0xBEC6}, /* ƾ: GB2312 Code: 0xC6BE ==> Row:38 Col:30 */
    {0x51EF,0xADBF}, /* ��: GB2312 Code: 0xBFAD ==> Row:31 Col:13 */
    {0x51F0,0xCBBB}, /* ��: GB2312 Code: 0xBBCB ==> Row:27 Col:43 */
    {0x51F3,0xCAB5}, /* ��: GB2312 Code: 0xB5CA ==> Row:21 Col:42 */
    {0x51F5,0xC9DB}, /* ��: GB2312 Code: 0xDBC9 ==> Row:59 Col:41 */
    {0x51F6,0xD7D0}, /* ��: GB2312 Code: 0xD0D7 ==> Row:48 Col:55 */
    {0x51F8,0xB9CD}, /* ͹: GB2312 Code: 0xCDB9 ==> Row:45 Col:25 */
    {0x51F9,0xBCB0}, /* ��: GB2312 Code: 0xB0BC ==> Row:16 Col:28 */
    {0x51FA,0xF6B3}, /* ��: GB2312 Code: 0xB3F6 ==> Row:19 Col:86 */
    {0x51FB,0xF7BB}, /* ��: GB2312 Code: 0xBBF7 ==> Row:27 Col:87 */
    {0x51FC,0xCADB}, /* ��: GB2312 Code: 0xDBCA ==> Row:59 Col:42 */
    {0x51FD,0xAFBA}, /* ��: GB2312 Code: 0xBAAF ==> Row:26 Col:15 */
    {0x51FF,0xE4D4}, /* ��: GB2312 Code: 0xD4E4 ==> Row:52 Col:68 */
    {0x5200,0xB6B5}, /* ��: GB2312 Code: 0xB5B6 ==> Row:21 Col:22 */
    {0x5201,0xF3B5}, /* ��: GB2312 Code: 0xB5F3 ==> Row:21 Col:83 */
    {0x5202,0xD6D8}, /* ��: GB2312 Code: 0xD8D6 ==> Row:56 Col:54 */
    {0x5203,0xD0C8}, /* ��: GB2312 Code: 0xC8D0 ==> Row:40 Col:48 */
    {0x5206,0xD6B7}, /* ��: GB2312 Code: 0xB7D6 ==> Row:23 Col:54 */
    {0x5207,0xD0C7}, /* ��: GB2312 Code: 0xC7D0 ==> Row:39 Col:48 */
    {0x5208,0xD7D8}, /* ��: GB2312 Code: 0xD8D7 ==> Row:56 Col:55 */
    {0x520A,0xAFBF}, /* ��: GB2312 Code: 0xBFAF ==> Row:31 Col:15 */
    {0x520D,0xBBDB}, /* ۻ: GB2312 Code: 0xDBBB ==> Row:59 Col:27 */
    {0x520E,0xD8D8}, /* ��: GB2312 Code: 0xD8D8 ==> Row:56 Col:56 */
    {0x5211,0xCCD0}, /* ��: GB2312 Code: 0xD0CC ==> Row:48 Col:44 */
    {0x5212,0xAEBB}, /* ��: GB2312 Code: 0xBBAE ==> Row:27 Col:14 */
    {0x5216,0xBEEB}, /* �: GB2312 Code: 0xEBBE ==> Row:75 Col:30 */
    {0x5217,0xD0C1}, /* ��: GB2312 Code: 0xC1D0 ==> Row:33 Col:48 */
    {0x5218,0xF5C1}, /* ��: GB2312 Code: 0xC1F5 ==> Row:33 Col:85 */
    {0x5219,0xF2D4}, /* ��: GB2312 Code: 0xD4F2 ==> Row:52 Col:82 */
    {0x521A,0xD5B8}, /* ��: GB2312 Code: 0xB8D5 ==> Row:24 Col:53 */
    {0x521B,0xB4B4}, /* ��: GB2312 Code: 0xB4B4 ==> Row:20 Col:20 */
    {0x521D,0xF5B3}, /* ��: GB2312 Code: 0xB3F5 ==> Row:19 Col:85 */
    {0x5220,0xBEC9}, /* ɾ: GB2312 Code: 0xC9BE ==> Row:41 Col:30 */
    {0x5224,0xD0C5}, /* ��: GB2312 Code: 0xC5D0 ==> Row:37 Col:48 */
    {0x5228,0xD9C5}, /* ��: GB2312 Code: 0xC5D9 ==> Row:37 Col:57 */
    {0x5229,0xFBC0}, /* ��: GB2312 Code: 0xC0FB ==> Row:32 Col:91 */
    {0x522B,0xF0B1}, /* ��: GB2312 Code: 0xB1F0 ==> Row:17 Col:80 */
    {0x522D,0xD9D8}, /* ��: GB2312 Code: 0xD8D9 ==> Row:56 Col:57 */
    {0x522E,0xCEB9}, /* ��: GB2312 Code: 0xB9CE ==> Row:25 Col:46 */
    {0x5230,0xBDB5}, /* ��: GB2312 Code: 0xB5BD ==> Row:21 Col:29 */
    {0x5233,0xDAD8}, /* ��: GB2312 Code: 0xD8DA ==> Row:56 Col:58 */
    {0x5236,0xC6D6}, /* ��: GB2312 Code: 0xD6C6 ==> Row:54 Col:38 */
    {0x5237,0xA2CB}, /* ˢ: GB2312 Code: 0xCBA2 ==> Row:43 Col:02 */
    {0x5238,0xAFC8}, /* ȯ: GB2312 Code: 0xC8AF ==> Row:40 Col:15 */
    {0x5239,0xB2C9}, /* ɲ: GB2312 Code: 0xC9B2 ==> Row:41 Col:18 */
    {0x523A,0xCCB4}, /* ��: GB2312 Code: 0xB4CC ==> Row:20 Col:44 */
    {0x523B,0xCCBF}, /* ��: GB2312 Code: 0xBFCC ==> Row:31 Col:44 */
    {0x523D,0xF4B9}, /* ��: GB2312 Code: 0xB9F4 ==> Row:25 Col:84 */
    {0x523F,0xDBD8}, /* ��: GB2312 Code: 0xD8DB ==> Row:56 Col:59 */
    {0x5240,0xDCD8}, /* ��: GB2312 Code: 0xD8DC ==> Row:56 Col:60 */
    {0x5241,0xE7B6}, /* ��: GB2312 Code: 0xB6E7 ==> Row:22 Col:71 */
    {0x5242,0xC1BC}, /* ��: GB2312 Code: 0xBCC1 ==> Row:28 Col:33 */
    {0x5243,0xEACC}, /* ��: GB2312 Code: 0xCCEA ==> Row:44 Col:74 */
    {0x524A,0xF7CF}, /* ��: GB2312 Code: 0xCFF7 ==> Row:47 Col:87 */
    {0x524C,0xDDD8}, /* ��: GB2312 Code: 0xD8DD ==> Row:56 Col:61 */
    {0x524D,0xB0C7}, /* ǰ: GB2312 Code: 0xC7B0 ==> Row:39 Col:16 */
    {0x5250,0xD0B9}, /* ��: GB2312 Code: 0xB9D0 ==> Row:25 Col:48 */
    {0x5251,0xA3BD}, /* ��: GB2312 Code: 0xBDA3 ==> Row:29 Col:03 */
    {0x5254,0xDECC}, /* ��: GB2312 Code: 0xCCDE ==> Row:44 Col:62 */
    {0x5256,0xCAC6}, /* ��: GB2312 Code: 0xC6CA ==> Row:38 Col:42 */
    {0x525C,0xE0D8}, /* ��: GB2312 Code: 0xD8E0 ==> Row:56 Col:64 */
    {0x525E,0xDED8}, /* ��: GB2312 Code: 0xD8DE ==> Row:56 Col:62 */
    {0x5261,0xDFD8}, /* ��: GB2312 Code: 0xD8DF ==> Row:56 Col:63 */
    {0x5265,0xFEB0}, /* ��: GB2312 Code: 0xB0FE ==> Row:16 Col:94 */
    {0x5267,0xE7BE}, /* ��: GB2312 Code: 0xBEE7 ==> Row:30 Col:71 */
    {0x5269,0xA3CA}, /* ʣ: GB2312 Code: 0xCAA3 ==> Row:42 Col:03 */
    {0x526A,0xF4BC}, /* ��: GB2312 Code: 0xBCF4 ==> Row:28 Col:84 */
    {0x526F,0xB1B8}, /* ��: GB2312 Code: 0xB8B1 ==> Row:24 Col:17 */
    {0x5272,0xEEB8}, /* ��: GB2312 Code: 0xB8EE ==> Row:24 Col:78 */
    {0x527D,0xE2D8}, /* ��: GB2312 Code: 0xD8E2 ==> Row:56 Col:66 */
    {0x527F,0xCBBD}, /* ��: GB2312 Code: 0xBDCB ==> Row:29 Col:43 */
    {0x5281,0xE4D8}, /* ��: GB2312 Code: 0xD8E4 ==> Row:56 Col:68 */
    {0x5282,0xE3D8}, /* ��: GB2312 Code: 0xD8E3 ==> Row:56 Col:67 */
    {0x5288,0xFCC5}, /* ��: GB2312 Code: 0xC5FC ==> Row:37 Col:92 */
    {0x5290,0xE5D8}, /* ��: GB2312 Code: 0xD8E5 ==> Row:56 Col:69 */
    {0x5293,0xE6D8}, /* ��: GB2312 Code: 0xD8E6 ==> Row:56 Col:70 */
    {0x529B,0xA6C1}, /* ��: GB2312 Code: 0xC1A6 ==> Row:33 Col:06 */
    {0x529D,0xB0C8}, /* Ȱ: GB2312 Code: 0xC8B0 ==> Row:40 Col:16 */
    {0x529E,0xECB0}, /* ��: GB2312 Code: 0xB0EC ==> Row:16 Col:76 */
    {0x529F,0xA6B9}, /* ��: GB2312 Code: 0xB9A6 ==> Row:25 Col:06 */
    {0x52A0,0xD3BC}, /* ��: GB2312 Code: 0xBCD3 ==> Row:28 Col:51 */
    {0x52A1,0xF1CE}, /* ��: GB2312 Code: 0xCEF1 ==> Row:46 Col:81 */
    {0x52A2,0xBDDB}, /* ۽: GB2312 Code: 0xDBBD ==> Row:59 Col:29 */
    {0x52A3,0xD3C1}, /* ��: GB2312 Code: 0xC1D3 ==> Row:33 Col:51 */
    {0x52A8,0xAFB6}, /* ��: GB2312 Code: 0xB6AF ==> Row:22 Col:15 */
    {0x52A9,0xFAD6}, /* ��: GB2312 Code: 0xD6FA ==> Row:54 Col:90 */
    {0x52AA,0xACC5}, /* Ŭ: GB2312 Code: 0xC5AC ==> Row:37 Col:12 */
    {0x52AB,0xD9BD}, /* ��: GB2312 Code: 0xBDD9 ==> Row:29 Col:57 */
    {0x52AC,0xBEDB}, /* ۾: GB2312 Code: 0xDBBE ==> Row:59 Col:30 */
    {0x52AD,0xBFDB}, /* ۿ: GB2312 Code: 0xDBBF ==> Row:59 Col:31 */
    {0x52B1,0xF8C0}, /* ��: GB2312 Code: 0xC0F8 ==> Row:32 Col:88 */
    {0x52B2,0xA2BE}, /* ��: GB2312 Code: 0xBEA2 ==> Row:30 Col:02 */
    {0x52B3,0xCDC0}, /* ��: GB2312 Code: 0xC0CD ==> Row:32 Col:45 */
    {0x52BE,0xC0DB}, /* ��: GB2312 Code: 0xDBC0 ==> Row:59 Col:32 */
    {0x52BF,0xC6CA}, /* ��: GB2312 Code: 0xCAC6 ==> Row:42 Col:38 */
    {0x52C3,0xAAB2}, /* ��: GB2312 Code: 0xB2AA ==> Row:18 Col:10 */
    {0x52C7,0xC2D3}, /* ��: GB2312 Code: 0xD3C2 ==> Row:51 Col:34 */
    {0x52C9,0xE3C3}, /* ��: GB2312 Code: 0xC3E3 ==> Row:35 Col:67 */
    {0x52CB,0xABD1}, /* ѫ: GB2312 Code: 0xD1AB ==> Row:49 Col:11 */
    {0x52D0,0xC2DB}, /* ��: GB2312 Code: 0xDBC2 ==> Row:59 Col:34 */
    {0x52D2,0xD5C0}, /* ��: GB2312 Code: 0xC0D5 ==> Row:32 Col:53 */
    {0x52D6,0xC3DB}, /* ��: GB2312 Code: 0xDBC3 ==> Row:59 Col:35 */
    {0x52D8,0xB1BF}, /* ��: GB2312 Code: 0xBFB1 ==> Row:31 Col:17 */
    {0x52DF,0xBCC4}, /* ļ: GB2312 Code: 0xC4BC ==> Row:36 Col:28 */
    {0x52E4,0xDAC7}, /* ��: GB2312 Code: 0xC7DA ==> Row:39 Col:58 */
    {0x52F0,0xC4DB}, /* ��: GB2312 Code: 0xDBC4 ==> Row:59 Col:36 */
    {0x52F9,0xE8D9}, /* ��: GB2312 Code: 0xD9E8 ==> Row:57 Col:72 */
    {0x52FA,0xD7C9}, /* ��: GB2312 Code: 0xC9D7 ==> Row:41 Col:55 */
    {0x52FE,0xB4B9}, /* ��: GB2312 Code: 0xB9B4 ==> Row:25 Col:20 */
    {0x52FF,0xF0CE}, /* ��: GB2312 Code: 0xCEF0 ==> Row:46 Col:80 */
    {0x5300,0xC8D4}, /* ��: GB2312 Code: 0xD4C8 ==> Row:52 Col:40 */
    {0x5305,0xFCB0}, /* ��: GB2312 Code: 0xB0FC ==> Row:16 Col:92 */
    {0x5306,0xD2B4}, /* ��: GB2312 Code: 0xB4D2 ==> Row:20 Col:50 */
    {0x5308,0xD9D0}, /* ��: GB2312 Code: 0xD0D9 ==> Row:48 Col:57 */
    {0x530D,0xE9D9}, /* ��: GB2312 Code: 0xD9E9 ==> Row:57 Col:73 */
    {0x530F,0xCBDE}, /* ��: GB2312 Code: 0xDECB ==> Row:62 Col:43 */
    {0x5310,0xEBD9}, /* ��: GB2312 Code: 0xD9EB ==> Row:57 Col:75 */
    {0x5315,0xB0D8}, /* ذ: GB2312 Code: 0xD8B0 ==> Row:56 Col:16 */
    {0x5316,0xAFBB}, /* ��: GB2312 Code: 0xBBAF ==> Row:27 Col:15 */
    {0x5317,0xB1B1}, /* ��: GB2312 Code: 0xB1B1 ==> Row:17 Col:17 */
    {0x5319,0xD7B3}, /* ��: GB2312 Code: 0xB3D7 ==> Row:19 Col:55 */
    {0x531A,0xCED8}, /* ��: GB2312 Code: 0xD8CE ==> Row:56 Col:46 */
    {0x531D,0xD1D4}, /* ��: GB2312 Code: 0xD4D1 ==> Row:52 Col:49 */
    {0x5320,0xB3BD}, /* ��: GB2312 Code: 0xBDB3 ==> Row:29 Col:19 */
    {0x5321,0xEFBF}, /* ��: GB2312 Code: 0xBFEF ==> Row:31 Col:79 */
    {0x5323,0xBBCF}, /* ϻ: GB2312 Code: 0xCFBB ==> Row:47 Col:27 */
    {0x5326,0xD0D8}, /* ��: GB2312 Code: 0xD8D0 ==> Row:56 Col:48 */
    {0x532A,0xCBB7}, /* ��: GB2312 Code: 0xB7CB ==> Row:23 Col:43 */
    {0x532E,0xD1D8}, /* ��: GB2312 Code: 0xD8D1 ==> Row:56 Col:49 */
    {0x5339,0xA5C6}, /* ƥ: GB2312 Code: 0xC6A5 ==> Row:38 Col:05 */
    {0x533A,0xF8C7}, /* ��: GB2312 Code: 0xC7F8 ==> Row:39 Col:88 */
    {0x533B,0xBDD2}, /* ҽ: GB2312 Code: 0xD2BD ==> Row:50 Col:29 */
    {0x533E,0xD2D8}, /* ��: GB2312 Code: 0xD8D2 ==> Row:56 Col:50 */
    {0x533F,0xE4C4}, /* ��: GB2312 Code: 0xC4E4 ==> Row:36 Col:68 */
    {0x5341,0xAECA}, /* ʮ: GB2312 Code: 0xCAAE ==> Row:42 Col:14 */
    {0x5343,0xA7C7}, /* ǧ: GB2312 Code: 0xC7A7 ==> Row:39 Col:07 */
    {0x5345,0xA6D8}, /* ئ: GB2312 Code: 0xD8A6 ==> Row:56 Col:06 */
    {0x5347,0xFDC9}, /* ��: GB2312 Code: 0xC9FD ==> Row:41 Col:93 */
    {0x5348,0xE7CE}, /* ��: GB2312 Code: 0xCEE7 ==> Row:46 Col:71 */
    {0x5349,0xDCBB}, /* ��: GB2312 Code: 0xBBDC ==> Row:27 Col:60 */
    {0x534A,0xEBB0}, /* ��: GB2312 Code: 0xB0EB ==> Row:16 Col:75 */
    {0x534E,0xAABB}, /* ��: GB2312 Code: 0xBBAA ==> Row:27 Col:10 */
    {0x534F,0xADD0}, /* Э: GB2312 Code: 0xD0AD ==> Row:48 Col:13 */
    {0x5351,0xB0B1}, /* ��: GB2312 Code: 0xB1B0 ==> Row:17 Col:16 */
    {0x5352,0xE4D7}, /* ��: GB2312 Code: 0xD7E4 ==> Row:55 Col:68 */
    {0x5353,0xBFD7}, /* ׿: GB2312 Code: 0xD7BF ==> Row:55 Col:31 */
    {0x5355,0xA5B5}, /* ��: GB2312 Code: 0xB5A5 ==> Row:21 Col:05 */
    {0x5356,0xF4C2}, /* ��: GB2312 Code: 0xC2F4 ==> Row:34 Col:84 */
    {0x5357,0xCFC4}, /* ��: GB2312 Code: 0xC4CF ==> Row:36 Col:47 */
    {0x535A,0xA9B2}, /* ��: GB2312 Code: 0xB2A9 ==> Row:18 Col:09 */
    {0x535C,0xB7B2}, /* ��: GB2312 Code: 0xB2B7 ==> Row:18 Col:23 */
    {0x535E,0xE5B1}, /* ��: GB2312 Code: 0xB1E5 ==> Row:17 Col:69 */
    {0x535F,0xB2DF}, /* ߲: GB2312 Code: 0xDFB2 ==> Row:63 Col:18 */
    {0x5360,0xBCD5}, /* ռ: GB2312 Code: 0xD5BC ==> Row:53 Col:28 */
    {0x5361,0xA8BF}, /* ��: GB2312 Code: 0xBFA8 ==> Row:31 Col:08 */
    {0x5362,0xACC2}, /* ¬: GB2312 Code: 0xC2AC ==> Row:34 Col:12 */
    {0x5363,0xD5D8}, /* ��: GB2312 Code: 0xD8D5 ==> Row:56 Col:53 */
    {0x5364,0xB1C2}, /* ±: GB2312 Code: 0xC2B1 ==> Row:34 Col:17 */
    {0x5366,0xD4D8}, /* ��: GB2312 Code: 0xD8D4 ==> Row:56 Col:52 */
    {0x5367,0xD4CE}, /* ��: GB2312 Code: 0xCED4 ==> Row:46 Col:52 */
    {0x5369,0xE0DA}, /* ��: GB2312 Code: 0xDAE0 ==> Row:58 Col:64 */
    {0x536B,0xC0CE}, /* ��: GB2312 Code: 0xCEC0 ==> Row:46 Col:32 */
    {0x536E,0xB4D8}, /* ش: GB2312 Code: 0xD8B4 ==> Row:56 Col:20 */
    {0x536F,0xAEC3}, /* î: GB2312 Code: 0xC3AE ==> Row:35 Col:14 */
    {0x5370,0xA1D3}, /* ӡ: GB2312 Code: 0xD3A1 ==> Row:51 Col:01 */
    {0x5371,0xA3CE}, /* Σ: GB2312 Code: 0xCEA3 ==> Row:46 Col:03 */
    {0x5373,0xB4BC}, /* ��: GB2312 Code: 0xBCB4 ==> Row:28 Col:20 */
    {0x5374,0xB4C8}, /* ȴ: GB2312 Code: 0xC8B4 ==> Row:40 Col:20 */
    {0x5375,0xD1C2}, /* ��: GB2312 Code: 0xC2D1 ==> Row:34 Col:49 */
    {0x5377,0xEDBE}, /* ��: GB2312 Code: 0xBEED ==> Row:30 Col:77 */
    {0x5378,0xB6D0}, /* ж: GB2312 Code: 0xD0B6 ==> Row:48 Col:22 */
    {0x537A,0xE1DA}, /* ��: GB2312 Code: 0xDAE1 ==> Row:58 Col:65 */
    {0x537F,0xE4C7}, /* ��: GB2312 Code: 0xC7E4 ==> Row:39 Col:68 */
    {0x5382,0xA7B3}, /* ��: GB2312 Code: 0xB3A7 ==> Row:19 Col:07 */
    {0x5384,0xF2B6}, /* ��: GB2312 Code: 0xB6F2 ==> Row:22 Col:82 */
    {0x5385,0xFCCC}, /* ��: GB2312 Code: 0xCCFC ==> Row:44 Col:92 */
    {0x5386,0xFAC0}, /* ��: GB2312 Code: 0xC0FA ==> Row:32 Col:90 */
    {0x5389,0xF7C0}, /* ��: GB2312 Code: 0xC0F7 ==> Row:32 Col:87 */
    {0x538B,0xB9D1}, /* ѹ: GB2312 Code: 0xD1B9 ==> Row:49 Col:25 */
    {0x538C,0xE1D1}, /* ��: GB2312 Code: 0xD1E1 ==> Row:49 Col:65 */
    {0x538D,0xC7D8}, /* ��: GB2312 Code: 0xD8C7 ==> Row:56 Col:39 */
    {0x5395,0xDEB2}, /* ��: GB2312 Code: 0xB2DE ==> Row:18 Col:62 */
    {0x5398,0xE5C0}, /* ��: GB2312 Code: 0xC0E5 ==> Row:32 Col:69 */
    {0x539A,0xF1BA}, /* ��: GB2312 Code: 0xBAF1 ==> Row:26 Col:81 */
    {0x539D,0xC8D8}, /* ��: GB2312 Code: 0xD8C8 ==> Row:56 Col:40 */
    {0x539F,0xADD4}, /* ԭ: GB2312 Code: 0xD4AD ==> Row:52 Col:13 */
    {0x53A2,0xE1CF}, /* ��: GB2312 Code: 0xCFE1 ==> Row:47 Col:65 */
    {0x53A3,0xC9D8}, /* ��: GB2312 Code: 0xD8C9 ==> Row:56 Col:41 */
    {0x53A5,0xCAD8}, /* ��: GB2312 Code: 0xD8CA ==> Row:56 Col:42 */
    {0x53A6,0xC3CF}, /* ��: GB2312 Code: 0xCFC3 ==> Row:47 Col:35 */
    {0x53A8,0xF8B3}, /* ��: GB2312 Code: 0xB3F8 ==> Row:19 Col:88 */
    {0x53A9,0xC7BE}, /* ��: GB2312 Code: 0xBEC7 ==> Row:30 Col:39 */
    {0x53AE,0xCBD8}, /* ��: GB2312 Code: 0xD8CB ==> Row:56 Col:43 */
    {0x53B6,0xCCDB}, /* ��: GB2312 Code: 0xDBCC ==> Row:59 Col:44 */
    {0x53BB,0xA5C8}, /* ȥ: GB2312 Code: 0xC8A5 ==> Row:40 Col:05 */
    {0x53BF,0xD8CF}, /* ��: GB2312 Code: 0xCFD8 ==> Row:47 Col:56 */
    {0x53C1,0xFEC8}, /* ��: GB2312 Code: 0xC8FE ==> Row:40 Col:94 */
    {0x53C2,0xCEB2}, /* ��: GB2312 Code: 0xB2CE ==> Row:18 Col:46 */
    {0x53C8,0xD6D3}, /* ��: GB2312 Code: 0xD3D6 ==> Row:51 Col:54 */
    {0x53C9,0xE6B2}, /* ��: GB2312 Code: 0xB2E6 ==> Row:18 Col:70 */
    {0x53CA,0xB0BC}, /* ��: GB2312 Code: 0xBCB0 ==> Row:28 Col:16 */
    {0x53CB,0xD1D3}, /* ��: GB2312 Code: 0xD3D1 ==> Row:51 Col:49 */
    {0x53CC,0xABCB}, /* ˫: GB2312 Code: 0xCBAB ==> Row:43 Col:11 */
    {0x53CD,0xB4B7}, /* ��: GB2312 Code: 0xB7B4 ==> Row:23 Col:20 */
    {0x53D1,0xA2B7}, /* ��: GB2312 Code: 0xB7A2 ==> Row:23 Col:02 */
    {0x53D4,0xE5CA}, /* ��: GB2312 Code: 0xCAE5 ==> Row:42 Col:69 */
    {0x53D6,0xA1C8}, /* ȡ: GB2312 Code: 0xC8A1 ==> Row:40 Col:01 */
    {0x53D7,0xDCCA}, /* ��: GB2312 Code: 0xCADC ==> Row:42 Col:60 */
    {0x53D8,0xE4B1}, /* ��: GB2312 Code: 0xB1E4 ==> Row:17 Col:68 */
    {0x53D9,0xF0D0}, /* ��: GB2312 Code: 0xD0F0 ==> Row:48 Col:80 */
    {0x53DB,0xD1C5}, /* ��: GB2312 Code: 0xC5D1 ==> Row:37 Col:49 */
    {0x53DF,0xC5DB}, /* ��: GB2312 Code: 0xDBC5 ==> Row:59 Col:37 */
    {0x53E0,0xFEB5}, /* ��: GB2312 Code: 0xB5FE ==> Row:21 Col:94 */
    {0x53E3,0xDABF}, /* ��: GB2312 Code: 0xBFDA ==> Row:31 Col:58 */
    {0x53E4,0xC5B9}, /* ��: GB2312 Code: 0xB9C5 ==> Row:25 Col:37 */
    {0x53E5,0xE4BE}, /* ��: GB2312 Code: 0xBEE4 ==> Row:30 Col:68 */
    {0x53E6,0xEDC1}, /* ��: GB2312 Code: 0xC1ED ==> Row:33 Col:77 */
    {0x53E8,0xB6DF}, /* ߶: GB2312 Code: 0xDFB6 ==> Row:63 Col:22 */
    {0x53E9,0xB5DF}, /* ߵ: GB2312 Code: 0xDFB5 ==> Row:63 Col:21 */
    {0x53EA,0xBBD6}, /* ֻ: GB2312 Code: 0xD6BB ==> Row:54 Col:27 */
    {0x53EB,0xD0BD}, /* ��: GB2312 Code: 0xBDD0 ==> Row:29 Col:48 */
    {0x53EC,0xD9D5}, /* ��: GB2312 Code: 0xD5D9 ==> Row:53 Col:57 */
    {0x53ED,0xC8B0}, /* ��: GB2312 Code: 0xB0C8 ==> Row:16 Col:40 */
    {0x53EE,0xA3B6}, /* ��: GB2312 Code: 0xB6A3 ==> Row:22 Col:03 */
    {0x53EF,0xC9BF}, /* ��: GB2312 Code: 0xBFC9 ==> Row:31 Col:41 */
    {0x53F0,0xA8CC}, /* ̨: GB2312 Code: 0xCCA8 ==> Row:44 Col:08 */
    {0x53F1,0xB3DF}, /* ߳: GB2312 Code: 0xDFB3 ==> Row:63 Col:19 */
    {0x53F2,0xB7CA}, /* ʷ: GB2312 Code: 0xCAB7 ==> Row:42 Col:23 */
    {0x53F3,0xD2D3}, /* ��: GB2312 Code: 0xD3D2 ==> Row:51 Col:50 */
    {0x53F5,0xCFD8}, /* ��: GB2312 Code: 0xD8CF ==> Row:56 Col:47 */
    {0x53F6,0xB6D2}, /* Ҷ: GB2312 Code: 0xD2B6 ==> Row:50 Col:22 */
    {0x53F7,0xC5BA}, /* ��: GB2312 Code: 0xBAC5 ==> Row:26 Col:37 */
    {0x53F8,0xBECB}, /* ˾: GB2312 Code: 0xCBBE ==> Row:43 Col:30 */
    {0x53F9,0xBECC}, /* ̾: GB2312 Code: 0xCCBE ==> Row:44 Col:30 */
    {0x53FB,0xB7DF}, /* ߷: GB2312 Code: 0xDFB7 ==> Row:63 Col:23 */
    {0x53FC,0xF0B5}, /* ��: GB2312 Code: 0xB5F0 ==> Row:21 Col:80 */
    {0x53FD,0xB4DF}, /* ߴ: GB2312 Code: 0xDFB4 ==> Row:63 Col:20 */
    {0x5401,0xF5D3}, /* ��: GB2312 Code: 0xD3F5 ==> Row:51 Col:85 */
    {0x5403,0xD4B3}, /* ��: GB2312 Code: 0xB3D4 ==> Row:19 Col:52 */
    {0x5404,0xF7B8}, /* ��: GB2312 Code: 0xB8F7 ==> Row:24 Col:87 */
    {0x5406,0xBADF}, /* ߺ: GB2312 Code: 0xDFBA ==> Row:63 Col:26 */
    {0x5408,0xCFBA}, /* ��: GB2312 Code: 0xBACF ==> Row:26 Col:47 */
    {0x5409,0xAABC}, /* ��: GB2312 Code: 0xBCAA ==> Row:28 Col:10 */
    {0x540A,0xF5B5}, /* ��: GB2312 Code: 0xB5F5 ==> Row:21 Col:85 */
    {0x540C,0xACCD}, /* ͬ: GB2312 Code: 0xCDAC ==> Row:45 Col:12 */
    {0x540D,0xFBC3}, /* ��: GB2312 Code: 0xC3FB ==> Row:35 Col:91 */
    {0x540E,0xF3BA}, /* ��: GB2312 Code: 0xBAF3 ==> Row:26 Col:83 */
    {0x540F,0xF4C0}, /* ��: GB2312 Code: 0xC0F4 ==> Row:32 Col:84 */
    {0x5410,0xC2CD}, /* ��: GB2312 Code: 0xCDC2 ==> Row:45 Col:34 */
    {0x5411,0xF2CF}, /* ��: GB2312 Code: 0xCFF2 ==> Row:47 Col:82 */
    {0x5412,0xB8DF}, /* ߸: GB2312 Code: 0xDFB8 ==> Row:63 Col:24 */
    {0x5413,0xC5CF}, /* ��: GB2312 Code: 0xCFC5 ==> Row:47 Col:37 */
    {0x5415,0xC0C2}, /* ��: GB2312 Code: 0xC2C0 ==> Row:34 Col:32 */
    {0x5416,0xB9DF}, /* ߹: GB2312 Code: 0xDFB9 ==> Row:63 Col:25 */
    {0x5417,0xF0C2}, /* ��: GB2312 Code: 0xC2F0 ==> Row:34 Col:80 */
    {0x541B,0xFDBE}, /* ��: GB2312 Code: 0xBEFD ==> Row:30 Col:93 */
    {0x541D,0xDFC1}, /* ��: GB2312 Code: 0xC1DF ==> Row:33 Col:63 */
    {0x541E,0xCCCD}, /* ��: GB2312 Code: 0xCDCC ==> Row:45 Col:44 */
    {0x541F,0xF7D2}, /* ��: GB2312 Code: 0xD2F7 ==> Row:50 Col:87 */
    {0x5420,0xCDB7}, /* ��: GB2312 Code: 0xB7CD ==> Row:23 Col:45 */
    {0x5421,0xC1DF}, /* ��: GB2312 Code: 0xDFC1 ==> Row:63 Col:33 */
    {0x5423,0xC4DF}, /* ��: GB2312 Code: 0xDFC4 ==> Row:63 Col:36 */
    {0x5426,0xF1B7}, /* ��: GB2312 Code: 0xB7F1 ==> Row:23 Col:81 */
    {0x5427,0xC9B0}, /* ��: GB2312 Code: 0xB0C9 ==> Row:16 Col:41 */
    {0x5428,0xD6B6}, /* ��: GB2312 Code: 0xB6D6 ==> Row:22 Col:54 */
    {0x5429,0xD4B7}, /* ��: GB2312 Code: 0xB7D4 ==> Row:23 Col:52 */
    {0x542B,0xACBA}, /* ��: GB2312 Code: 0xBAAC ==> Row:26 Col:12 */
    {0x542C,0xFDCC}, /* ��: GB2312 Code: 0xCCFD ==> Row:44 Col:93 */
    {0x542D,0xD4BF}, /* ��: GB2312 Code: 0xBFD4 ==> Row:31 Col:52 */
    {0x542E,0xB1CB}, /* ˱: GB2312 Code: 0xCBB1 ==> Row:43 Col:17 */
    {0x542F,0xF4C6}, /* ��: GB2312 Code: 0xC6F4 ==> Row:38 Col:84 */
    {0x5431,0xA8D6}, /* ֨: GB2312 Code: 0xD6A8 ==> Row:54 Col:08 */
    {0x5432,0xC5DF}, /* ��: GB2312 Code: 0xDFC5 ==> Row:63 Col:37 */
    {0x5434,0xE2CE}, /* ��: GB2312 Code: 0xCEE2 ==> Row:46 Col:66 */
    {0x5435,0xB3B3}, /* ��: GB2312 Code: 0xB3B3 ==> Row:19 Col:19 */
    {0x5438,0xFCCE}, /* ��: GB2312 Code: 0xCEFC ==> Row:46 Col:92 */
    {0x5439,0xB5B4}, /* ��: GB2312 Code: 0xB4B5 ==> Row:20 Col:21 */
    {0x543B,0xC7CE}, /* ��: GB2312 Code: 0xCEC7 ==> Row:46 Col:39 */
    {0x543C,0xF0BA}, /* ��: GB2312 Code: 0xBAF0 ==> Row:26 Col:80 */
    {0x543E,0xE1CE}, /* ��: GB2312 Code: 0xCEE1 ==> Row:46 Col:65 */
    {0x5440,0xBDD1}, /* ѽ: GB2312 Code: 0xD1BD ==> Row:49 Col:29 */
    {0x5443,0xC0DF}, /* ��: GB2312 Code: 0xDFC0 ==> Row:63 Col:32 */
    {0x5446,0xF4B4}, /* ��: GB2312 Code: 0xB4F4 ==> Row:20 Col:84 */
    {0x5448,0xCAB3}, /* ��: GB2312 Code: 0xB3CA ==> Row:19 Col:42 */
    {0x544A,0xE6B8}, /* ��: GB2312 Code: 0xB8E6 ==> Row:24 Col:70 */
    {0x544B,0xBBDF}, /* ߻: GB2312 Code: 0xDFBB ==> Row:63 Col:27 */
    {0x5450,0xC5C4}, /* ��: GB2312 Code: 0xC4C5 ==> Row:36 Col:37 */
    {0x5452,0xBCDF}, /* ߼: GB2312 Code: 0xDFBC ==> Row:63 Col:28 */
    {0x5453,0xBDDF}, /* ߽: GB2312 Code: 0xDFBD ==> Row:63 Col:29 */
    {0x5454,0xBEDF}, /* ߾: GB2312 Code: 0xDFBE ==> Row:63 Col:30 */
    {0x5455,0xBBC5}, /* Ż: GB2312 Code: 0xC5BB ==> Row:37 Col:27 */
    {0x5456,0xBFDF}, /* ߿: GB2312 Code: 0xDFBF ==> Row:63 Col:31 */
    {0x5457,0xC2DF}, /* ��: GB2312 Code: 0xDFC2 ==> Row:63 Col:34 */
    {0x5458,0xB1D4}, /* Ա: GB2312 Code: 0xD4B1 ==> Row:52 Col:17 */
    {0x5459,0xC3DF}, /* ��: GB2312 Code: 0xDFC3 ==> Row:63 Col:35 */
    {0x545B,0xBAC7}, /* Ǻ: GB2312 Code: 0xC7BA ==> Row:39 Col:26 */
    {0x545C,0xD8CE}, /* ��: GB2312 Code: 0xCED8 ==> Row:46 Col:56 */
    {0x5462,0xD8C4}, /* ��: GB2312 Code: 0xC4D8 ==> Row:36 Col:56 */
    {0x5464,0xCADF}, /* ��: GB2312 Code: 0xDFCA ==> Row:63 Col:42 */
    {0x5466,0xCFDF}, /* ��: GB2312 Code: 0xDFCF ==> Row:63 Col:47 */
    {0x5468,0xDCD6}, /* ��: GB2312 Code: 0xD6DC ==> Row:54 Col:60 */
    {0x5471,0xC9DF}, /* ��: GB2312 Code: 0xDFC9 ==> Row:63 Col:41 */
    {0x5472,0xDADF}, /* ��: GB2312 Code: 0xDFDA ==> Row:63 Col:58 */
    {0x5473,0xB6CE}, /* ζ: GB2312 Code: 0xCEB6 ==> Row:46 Col:22 */
    {0x5475,0xC7BA}, /* ��: GB2312 Code: 0xBAC7 ==> Row:26 Col:39 */
    {0x5476,0xCEDF}, /* ��: GB2312 Code: 0xDFCE ==> Row:63 Col:46 */
    {0x5477,0xC8DF}, /* ��: GB2312 Code: 0xDFC8 ==> Row:63 Col:40 */
    {0x5478,0xDEC5}, /* ��: GB2312 Code: 0xC5DE ==> Row:37 Col:62 */
    {0x547B,0xEBC9}, /* ��: GB2312 Code: 0xC9EB ==> Row:41 Col:75 */
    {0x547C,0xF4BA}, /* ��: GB2312 Code: 0xBAF4 ==> Row:26 Col:84 */
    {0x547D,0xFCC3}, /* ��: GB2312 Code: 0xC3FC ==> Row:35 Col:92 */
    {0x5480,0xD7BE}, /* ��: GB2312 Code: 0xBED7 ==> Row:30 Col:55 */
    {0x5482,0xC6DF}, /* ��: GB2312 Code: 0xDFC6 ==> Row:63 Col:38 */
    {0x5484,0xCDDF}, /* ��: GB2312 Code: 0xDFCD ==> Row:63 Col:45 */
    {0x5486,0xD8C5}, /* ��: GB2312 Code: 0xC5D8 ==> Row:37 Col:56 */
    {0x548B,0xA6D5}, /* զ: GB2312 Code: 0xD5A6 ==> Row:53 Col:06 */
    {0x548C,0xCDBA}, /* ��: GB2312 Code: 0xBACD ==> Row:26 Col:45 */
    {0x548E,0xCCBE}, /* ��: GB2312 Code: 0xBECC ==> Row:30 Col:44 */
    {0x548F,0xBDD3}, /* ӽ: GB2312 Code: 0xD3BD ==> Row:51 Col:29 */
    {0x5490,0xC0B8}, /* ��: GB2312 Code: 0xB8C0 ==> Row:24 Col:32 */
    {0x5492,0xE4D6}, /* ��: GB2312 Code: 0xD6E4 ==> Row:54 Col:68 */
    {0x5494,0xC7DF}, /* ��: GB2312 Code: 0xDFC7 ==> Row:63 Col:39 */
    {0x5495,0xBEB9}, /* ��: GB2312 Code: 0xB9BE ==> Row:25 Col:30 */
    {0x5496,0xA7BF}, /* ��: GB2312 Code: 0xBFA7 ==> Row:31 Col:07 */
    {0x5499,0xFCC1}, /* ��: GB2312 Code: 0xC1FC ==> Row:33 Col:92 */
    {0x549A,0xCBDF}, /* ��: GB2312 Code: 0xDFCB ==> Row:63 Col:43 */
    {0x549B,0xCCDF}, /* ��: GB2312 Code: 0xDFCC ==> Row:63 Col:44 */
    {0x549D,0xD0DF}, /* ��: GB2312 Code: 0xDFD0 ==> Row:63 Col:48 */
    {0x54A3,0xDBDF}, /* ��: GB2312 Code: 0xDFDB ==> Row:63 Col:59 */
    {0x54A4,0xE5DF}, /* ��: GB2312 Code: 0xDFE5 ==> Row:63 Col:69 */
    {0x54A6,0xD7DF}, /* ��: GB2312 Code: 0xDFD7 ==> Row:63 Col:55 */
    {0x54A7,0xD6DF}, /* ��: GB2312 Code: 0xDFD6 ==> Row:63 Col:54 */
    {0x54A8,0xC9D7}, /* ��: GB2312 Code: 0xD7C9 ==> Row:55 Col:41 */
    {0x54A9,0xE3DF}, /* ��: GB2312 Code: 0xDFE3 ==> Row:63 Col:67 */
    {0x54AA,0xE4DF}, /* ��: GB2312 Code: 0xDFE4 ==> Row:63 Col:68 */
    {0x54AB,0xEBE5}, /* ��: GB2312 Code: 0xE5EB ==> Row:69 Col:75 */
    {0x54AC,0xA7D2}, /* ҧ: GB2312 Code: 0xD2A7 ==> Row:50 Col:07 */
    {0x54AD,0xD2DF}, /* ��: GB2312 Code: 0xDFD2 ==> Row:63 Col:50 */
    {0x54AF,0xA9BF}, /* ��: GB2312 Code: 0xBFA9 ==> Row:31 Col:09 */
    {0x54B1,0xDBD4}, /* ��: GB2312 Code: 0xD4DB ==> Row:52 Col:59 */
    {0x54B3,0xC8BF}, /* ��: GB2312 Code: 0xBFC8 ==> Row:31 Col:40 */
    {0x54B4,0xD4DF}, /* ��: GB2312 Code: 0xDFD4 ==> Row:63 Col:52 */
    {0x54B8,0xCCCF}, /* ��: GB2312 Code: 0xCFCC ==> Row:47 Col:44 */
    {0x54BB,0xDDDF}, /* ��: GB2312 Code: 0xDFDD ==> Row:63 Col:61 */
    {0x54BD,0xCAD1}, /* ��: GB2312 Code: 0xD1CA ==> Row:49 Col:42 */
    {0x54BF,0xDEDF}, /* ��: GB2312 Code: 0xDFDE ==> Row:63 Col:62 */
    {0x54C0,0xA7B0}, /* ��: GB2312 Code: 0xB0A7 ==> Row:16 Col:07 */
    {0x54C1,0xB7C6}, /* Ʒ: GB2312 Code: 0xC6B7 ==> Row:38 Col:23 */
    {0x54C2,0xD3DF}, /* ��: GB2312 Code: 0xDFD3 ==> Row:63 Col:51 */
    {0x54C4,0xE5BA}, /* ��: GB2312 Code: 0xBAE5 ==> Row:26 Col:69 */
    {0x54C6,0xDFB6}, /* ��: GB2312 Code: 0xB6DF ==> Row:22 Col:63 */
    {0x54C7,0xDBCD}, /* ��: GB2312 Code: 0xCDDB ==> Row:45 Col:59 */
    {0x54C8,0xFEB9}, /* ��: GB2312 Code: 0xB9FE ==> Row:25 Col:94 */
    {0x54C9,0xD5D4}, /* ��: GB2312 Code: 0xD4D5 ==> Row:52 Col:53 */
    {0x54CC,0xDFDF}, /* ��: GB2312 Code: 0xDFDF ==> Row:63 Col:63 */
    {0x54CD,0xECCF}, /* ��: GB2312 Code: 0xCFEC ==> Row:47 Col:76 */
    {0x54CE,0xA5B0}, /* ��: GB2312 Code: 0xB0A5 ==> Row:16 Col:05 */
    {0x54CF,0xE7DF}, /* ��: GB2312 Code: 0xDFE7 ==> Row:63 Col:71 */
    {0x54D0,0xD1DF}, /* ��: GB2312 Code: 0xDFD1 ==> Row:63 Col:49 */
    {0x54D1,0xC6D1}, /* ��: GB2312 Code: 0xD1C6 ==> Row:49 Col:38 */
    {0x54D2,0xD5DF}, /* ��: GB2312 Code: 0xDFD5 ==> Row:63 Col:53 */
    {0x54D3,0xD8DF}, /* ��: GB2312 Code: 0xDFD8 ==> Row:63 Col:56 */
    {0x54D4,0xD9DF}, /* ��: GB2312 Code: 0xDFD9 ==> Row:63 Col:57 */
    {0x54D5,0xDCDF}, /* ��: GB2312 Code: 0xDFDC ==> Row:63 Col:60 */
    {0x54D7,0xA9BB}, /* ��: GB2312 Code: 0xBBA9 ==> Row:27 Col:09 */
    {0x54D9,0xE0DF}, /* ��: GB2312 Code: 0xDFE0 ==> Row:63 Col:64 */
    {0x54DA,0xE1DF}, /* ��: GB2312 Code: 0xDFE1 ==> Row:63 Col:65 */
    {0x54DC,0xE2DF}, /* ��: GB2312 Code: 0xDFE2 ==> Row:63 Col:66 */
    {0x54DD,0xE6DF}, /* ��: GB2312 Code: 0xDFE6 ==> Row:63 Col:70 */
    {0x54DE,0xE8DF}, /* ��: GB2312 Code: 0xDFE8 ==> Row:63 Col:72 */
    {0x54DF,0xB4D3}, /* Ӵ: GB2312 Code: 0xD3B4 ==> Row:51 Col:20 */
    {0x54E5,0xE7B8}, /* ��: GB2312 Code: 0xB8E7 ==> Row:24 Col:71 */
    {0x54E6,0xB6C5}, /* Ŷ: GB2312 Code: 0xC5B6 ==> Row:37 Col:22 */
    {0x54E7,0xEADF}, /* ��: GB2312 Code: 0xDFEA ==> Row:63 Col:74 */
    {0x54E8,0xDAC9}, /* ��: GB2312 Code: 0xC9DA ==> Row:41 Col:58 */
    {0x54E9,0xA8C1}, /* ��: GB2312 Code: 0xC1A8 ==> Row:33 Col:08 */
    {0x54EA,0xC4C4}, /* ��: GB2312 Code: 0xC4C4 ==> Row:36 Col:36 */
    {0x54ED,0xDEBF}, /* ��: GB2312 Code: 0xBFDE ==> Row:31 Col:62 */
    {0x54EE,0xF8CF}, /* ��: GB2312 Code: 0xCFF8 ==> Row:47 Col:88 */
    {0x54F2,0xDCD5}, /* ��: GB2312 Code: 0xD5DC ==> Row:53 Col:60 */
    {0x54F3,0xEEDF}, /* ��: GB2312 Code: 0xDFEE ==> Row:63 Col:78 */
    {0x54FA,0xB8B2}, /* ��: GB2312 Code: 0xB2B8 ==> Row:18 Col:24 */
    {0x54FC,0xDFBA}, /* ��: GB2312 Code: 0xBADF ==> Row:26 Col:63 */
    {0x54FD,0xECDF}, /* ��: GB2312 Code: 0xDFEC ==> Row:63 Col:76 */
    {0x54FF,0xC1DB}, /* ��: GB2312 Code: 0xDBC1 ==> Row:59 Col:33 */
    {0x5501,0xE4D1}, /* ��: GB2312 Code: 0xD1E4 ==> Row:49 Col:68 */
    {0x5506,0xF4CB}, /* ��: GB2312 Code: 0xCBF4 ==> Row:43 Col:84 */
    {0x5507,0xBDB4}, /* ��: GB2312 Code: 0xB4BD ==> Row:20 Col:29 */
    {0x5509,0xA6B0}, /* ��: GB2312 Code: 0xB0A6 ==> Row:16 Col:06 */
    {0x550F,0xF1DF}, /* ��: GB2312 Code: 0xDFF1 ==> Row:63 Col:81 */
    {0x5510,0xC6CC}, /* ��: GB2312 Code: 0xCCC6 ==> Row:44 Col:38 */
    {0x5511,0xF2DF}, /* ��: GB2312 Code: 0xDFF2 ==> Row:63 Col:82 */
    {0x5514,0xEDDF}, /* ��: GB2312 Code: 0xDFED ==> Row:63 Col:77 */
    {0x551B,0xE9DF}, /* ��: GB2312 Code: 0xDFE9 ==> Row:63 Col:73 */
    {0x5520,0xEBDF}, /* ��: GB2312 Code: 0xDFEB ==> Row:63 Col:75 */
    {0x5522,0xEFDF}, /* ��: GB2312 Code: 0xDFEF ==> Row:63 Col:79 */
    {0x5523,0xF0DF}, /* ��: GB2312 Code: 0xDFF0 ==> Row:63 Col:80 */
    {0x5524,0xBDBB}, /* ��: GB2312 Code: 0xBBBD ==> Row:27 Col:29 */
    {0x5527,0xF3DF}, /* ��: GB2312 Code: 0xDFF3 ==> Row:63 Col:83 */
    {0x552A,0xF4DF}, /* ��: GB2312 Code: 0xDFF4 ==> Row:63 Col:84 */
    {0x552C,0xA3BB}, /* ��: GB2312 Code: 0xBBA3 ==> Row:27 Col:03 */
    {0x552E,0xDBCA}, /* ��: GB2312 Code: 0xCADB ==> Row:42 Col:59 */
    {0x552F,0xA8CE}, /* Ψ: GB2312 Code: 0xCEA8 ==> Row:46 Col:08 */
    {0x5530,0xA7E0}, /* �: GB2312 Code: 0xE0A7 ==> Row:64 Col:07 */
    {0x5531,0xAAB3}, /* ��: GB2312 Code: 0xB3AA ==> Row:19 Col:10 */
    {0x5533,0xA6E0}, /* �: GB2312 Code: 0xE0A6 ==> Row:64 Col:06 */
    {0x5537,0xA1E0}, /* �: GB2312 Code: 0xE0A1 ==> Row:64 Col:01 */
    {0x553C,0xFEDF}, /* ��: GB2312 Code: 0xDFFE ==> Row:63 Col:94 */
    {0x553E,0xD9CD}, /* ��: GB2312 Code: 0xCDD9 ==> Row:45 Col:57 */
    {0x553F,0xFCDF}, /* ��: GB2312 Code: 0xDFFC ==> Row:63 Col:92 */
    {0x5541,0xFADF}, /* ��: GB2312 Code: 0xDFFA ==> Row:63 Col:90 */
    {0x5543,0xD0BF}, /* ��: GB2312 Code: 0xBFD0 ==> Row:31 Col:48 */
    {0x5544,0xC4D7}, /* ��: GB2312 Code: 0xD7C4 ==> Row:55 Col:36 */
    {0x5546,0xCCC9}, /* ��: GB2312 Code: 0xC9CC ==> Row:41 Col:44 */
    {0x5549,0xF8DF}, /* ��: GB2312 Code: 0xDFF8 ==> Row:63 Col:88 */
    {0x554A,0xA1B0}, /* ��: GB2312 Code: 0xB0A1 ==> Row:16 Col:01 */
    {0x5550,0xFDDF}, /* ��: GB2312 Code: 0xDFFD ==> Row:63 Col:93 */
    {0x5555,0xFBDF}, /* ��: GB2312 Code: 0xDFFB ==> Row:63 Col:91 */
    {0x5556,0xA2E0}, /* �: GB2312 Code: 0xE0A2 ==> Row:64 Col:02 */
    {0x555C,0xA8E0}, /* �: GB2312 Code: 0xE0A8 ==> Row:64 Col:08 */
    {0x5561,0xC8B7}, /* ��: GB2312 Code: 0xB7C8 ==> Row:23 Col:40 */
    {0x5564,0xA1C6}, /* ơ: GB2312 Code: 0xC6A1 ==> Row:38 Col:01 */
    {0x5565,0xB6C9}, /* ɶ: GB2312 Code: 0xC9B6 ==> Row:41 Col:22 */
    {0x5566,0xB2C0}, /* ��: GB2312 Code: 0xC0B2 ==> Row:32 Col:18 */
    {0x5567,0xF5DF}, /* ��: GB2312 Code: 0xDFF5 ==> Row:63 Col:85 */
    {0x556A,0xBEC5}, /* ž: GB2312 Code: 0xC5BE ==> Row:37 Col:30 */
    {0x556C,0xC4D8}, /* ��: GB2312 Code: 0xD8C4 ==> Row:56 Col:36 */
    {0x556D,0xF9DF}, /* ��: GB2312 Code: 0xDFF9 ==> Row:63 Col:89 */
    {0x556E,0xF6C4}, /* ��: GB2312 Code: 0xC4F6 ==> Row:36 Col:86 */
    {0x5575,0xA3E0}, /* �: GB2312 Code: 0xE0A3 ==> Row:64 Col:03 */
    {0x5576,0xA4E0}, /* �: GB2312 Code: 0xE0A4 ==> Row:64 Col:04 */
    {0x5577,0xA5E0}, /* �: GB2312 Code: 0xE0A5 ==> Row:64 Col:05 */
    {0x5578,0xA5D0}, /* Х: GB2312 Code: 0xD0A5 ==> Row:48 Col:05 */
    {0x557B,0xB4E0}, /* �: GB2312 Code: 0xE0B4 ==> Row:64 Col:20 */
    {0x557C,0xE4CC}, /* ��: GB2312 Code: 0xCCE4 ==> Row:44 Col:68 */
    {0x557E,0xB1E0}, /* �: GB2312 Code: 0xE0B1 ==> Row:64 Col:17 */
    {0x5580,0xA6BF}, /* ��: GB2312 Code: 0xBFA6 ==> Row:31 Col:06 */
    {0x5581,0xAFE0}, /* �: GB2312 Code: 0xE0AF ==> Row:64 Col:15 */
    {0x5582,0xB9CE}, /* ι: GB2312 Code: 0xCEB9 ==> Row:46 Col:25 */
    {0x5583,0xABE0}, /* �: GB2312 Code: 0xE0AB ==> Row:64 Col:11 */
    {0x5584,0xC6C9}, /* ��: GB2312 Code: 0xC9C6 ==> Row:41 Col:38 */
    {0x5587,0xAEC0}, /* ��: GB2312 Code: 0xC0AE ==> Row:32 Col:14 */
    {0x5588,0xAEE0}, /* �: GB2312 Code: 0xE0AE ==> Row:64 Col:14 */
    {0x5589,0xEDBA}, /* ��: GB2312 Code: 0xBAED ==> Row:26 Col:77 */
    {0x558A,0xB0BA}, /* ��: GB2312 Code: 0xBAB0 ==> Row:26 Col:16 */
    {0x558B,0xA9E0}, /* �: GB2312 Code: 0xE0A9 ==> Row:64 Col:09 */
    {0x558F,0xF6DF}, /* ��: GB2312 Code: 0xDFF6 ==> Row:63 Col:86 */
    {0x5591,0xB3E0}, /* �: GB2312 Code: 0xE0B3 ==> Row:64 Col:19 */
    {0x5594,0xB8E0}, /* �: GB2312 Code: 0xE0B8 ==> Row:64 Col:24 */
    {0x5598,0xADB4}, /* ��: GB2312 Code: 0xB4AD ==> Row:20 Col:13 */
    {0x5599,0xB9E0}, /* �: GB2312 Code: 0xE0B9 ==> Row:64 Col:25 */
    {0x559C,0xB2CF}, /* ϲ: GB2312 Code: 0xCFB2 ==> Row:47 Col:18 */
    {0x559D,0xC8BA}, /* ��: GB2312 Code: 0xBAC8 ==> Row:26 Col:40 */
    {0x559F,0xB0E0}, /* �: GB2312 Code: 0xE0B0 ==> Row:64 Col:16 */
    {0x55A7,0xFAD0}, /* ��: GB2312 Code: 0xD0FA ==> Row:48 Col:90 */
    {0x55B1,0xACE0}, /* �: GB2312 Code: 0xE0AC ==> Row:64 Col:12 */
    {0x55B3,0xFBD4}, /* ��: GB2312 Code: 0xD4FB ==> Row:52 Col:91 */
    {0x55B5,0xF7DF}, /* ��: GB2312 Code: 0xDFF7 ==> Row:63 Col:87 */
    {0x55B7,0xE7C5}, /* ��: GB2312 Code: 0xC5E7 ==> Row:37 Col:71 */
    {0x55B9,0xADE0}, /* �: GB2312 Code: 0xE0AD ==> Row:64 Col:13 */
    {0x55BB,0xF7D3}, /* ��: GB2312 Code: 0xD3F7 ==> Row:51 Col:87 */
    {0x55BD,0xB6E0}, /* �: GB2312 Code: 0xE0B6 ==> Row:64 Col:22 */
    {0x55BE,0xB7E0}, /* �: GB2312 Code: 0xE0B7 ==> Row:64 Col:23 */
    {0x55C4,0xC4E0}, /* ��: GB2312 Code: 0xE0C4 ==> Row:64 Col:36 */
    {0x55C5,0xE1D0}, /* ��: GB2312 Code: 0xD0E1 ==> Row:48 Col:65 */
    {0x55C9,0xBCE0}, /* �: GB2312 Code: 0xE0BC ==> Row:64 Col:28 */
    {0x55CC,0xC9E0}, /* ��: GB2312 Code: 0xE0C9 ==> Row:64 Col:41 */
    {0x55CD,0xCAE0}, /* ��: GB2312 Code: 0xE0CA ==> Row:64 Col:42 */
    {0x55D1,0xBEE0}, /* �: GB2312 Code: 0xE0BE ==> Row:64 Col:30 */
    {0x55D2,0xAAE0}, /* �: GB2312 Code: 0xE0AA ==> Row:64 Col:10 */
    {0x55D3,0xA4C9}, /* ɤ: GB2312 Code: 0xC9A4 ==> Row:41 Col:04 */
    {0x55D4,0xC1E0}, /* ��: GB2312 Code: 0xE0C1 ==> Row:64 Col:33 */
    {0x55D6,0xB2E0}, /* �: GB2312 Code: 0xE0B2 ==> Row:64 Col:18 */
    {0x55DC,0xC8CA}, /* ��: GB2312 Code: 0xCAC8 ==> Row:42 Col:40 */
    {0x55DD,0xC3E0}, /* ��: GB2312 Code: 0xE0C3 ==> Row:64 Col:35 */
    {0x55DF,0xB5E0}, /* �: GB2312 Code: 0xE0B5 ==> Row:64 Col:21 */
    {0x55E1,0xCBCE}, /* ��: GB2312 Code: 0xCECB ==> Row:46 Col:43 */
    {0x55E3,0xC3CB}, /* ��: GB2312 Code: 0xCBC3 ==> Row:43 Col:35 */
    {0x55E4,0xCDE0}, /* ��: GB2312 Code: 0xE0CD ==> Row:64 Col:45 */
    {0x55E5,0xC6E0}, /* ��: GB2312 Code: 0xE0C6 ==> Row:64 Col:38 */
    {0x55E6,0xC2E0}, /* ��: GB2312 Code: 0xE0C2 ==> Row:64 Col:34 */
    {0x55E8,0xCBE0}, /* ��: GB2312 Code: 0xE0CB ==> Row:64 Col:43 */
    {0x55EA,0xBAE0}, /* �: GB2312 Code: 0xE0BA ==> Row:64 Col:26 */
    {0x55EB,0xBFE0}, /* �: GB2312 Code: 0xE0BF ==> Row:64 Col:31 */
    {0x55EC,0xC0E0}, /* ��: GB2312 Code: 0xE0C0 ==> Row:64 Col:32 */
    {0x55EF,0xC5E0}, /* ��: GB2312 Code: 0xE0C5 ==> Row:64 Col:37 */
    {0x55F2,0xC7E0}, /* ��: GB2312 Code: 0xE0C7 ==> Row:64 Col:39 */
    {0x55F3,0xC8E0}, /* ��: GB2312 Code: 0xE0C8 ==> Row:64 Col:40 */
    {0x55F5,0xCCE0}, /* ��: GB2312 Code: 0xE0CC ==> Row:64 Col:44 */
    {0x55F7,0xBBE0}, /* �: GB2312 Code: 0xE0BB ==> Row:64 Col:27 */
    {0x55FD,0xD4CB}, /* ��: GB2312 Code: 0xCBD4 ==> Row:43 Col:52 */
    {0x55FE,0xD5E0}, /* ��: GB2312 Code: 0xE0D5 ==> Row:64 Col:53 */
    {0x5600,0xD6E0}, /* ��: GB2312 Code: 0xE0D6 ==> Row:64 Col:54 */
    {0x5601,0xD2E0}, /* ��: GB2312 Code: 0xE0D2 ==> Row:64 Col:50 */
    {0x5608,0xD0E0}, /* ��: GB2312 Code: 0xE0D0 ==> Row:64 Col:48 */
    {0x5609,0xCEBC}, /* ��: GB2312 Code: 0xBCCE ==> Row:28 Col:46 */
    {0x560C,0xD1E0}, /* ��: GB2312 Code: 0xE0D1 ==> Row:64 Col:49 */
    {0x560E,0xC2B8}, /* ��: GB2312 Code: 0xB8C2 ==> Row:24 Col:34 */
    {0x560F,0xC5D8}, /* ��: GB2312 Code: 0xD8C5 ==> Row:56 Col:37 */
    {0x5618,0xEAD0}, /* ��: GB2312 Code: 0xD0EA ==> Row:48 Col:74 */
    {0x561B,0xEFC2}, /* ��: GB2312 Code: 0xC2EF ==> Row:34 Col:79 */
    {0x561E,0xCFE0}, /* ��: GB2312 Code: 0xE0CF ==> Row:64 Col:47 */
    {0x561F,0xBDE0}, /* �: GB2312 Code: 0xE0BD ==> Row:64 Col:29 */
    {0x5623,0xD4E0}, /* ��: GB2312 Code: 0xE0D4 ==> Row:64 Col:52 */
    {0x5624,0xD3E0}, /* ��: GB2312 Code: 0xE0D3 ==> Row:64 Col:51 */
    {0x5627,0xD7E0}, /* ��: GB2312 Code: 0xE0D7 ==> Row:64 Col:55 */
    {0x562C,0xDCE0}, /* ��: GB2312 Code: 0xE0DC ==> Row:64 Col:60 */
    {0x562D,0xD8E0}, /* ��: GB2312 Code: 0xE0D8 ==> Row:64 Col:56 */
    {0x5631,0xF6D6}, /* ��: GB2312 Code: 0xD6F6 ==> Row:54 Col:86 */
    {0x5632,0xB0B3}, /* ��: GB2312 Code: 0xB3B0 ==> Row:19 Col:16 */
    {0x5634,0xECD7}, /* ��: GB2312 Code: 0xD7EC ==> Row:55 Col:76 */
    {0x5636,0xBBCB}, /* ˻: GB2312 Code: 0xCBBB ==> Row:43 Col:27 */
    {0x5639,0xDAE0}, /* ��: GB2312 Code: 0xE0DA ==> Row:64 Col:58 */
    {0x563B,0xFBCE}, /* ��: GB2312 Code: 0xCEFB ==> Row:46 Col:91 */
    {0x563F,0xD9BA}, /* ��: GB2312 Code: 0xBAD9 ==> Row:26 Col:57 */
    {0x564C,0xE1E0}, /* ��: GB2312 Code: 0xE0E1 ==> Row:64 Col:65 */
    {0x564D,0xDDE0}, /* ��: GB2312 Code: 0xE0DD ==> Row:64 Col:61 */
    {0x564E,0xADD2}, /* ҭ: GB2312 Code: 0xD2AD ==> Row:50 Col:13 */
    {0x5654,0xE2E0}, /* ��: GB2312 Code: 0xE0E2 ==> Row:64 Col:66 */
    {0x5657,0xDBE0}, /* ��: GB2312 Code: 0xE0DB ==> Row:64 Col:59 */
    {0x5658,0xD9E0}, /* ��: GB2312 Code: 0xE0D9 ==> Row:64 Col:57 */
    {0x5659,0xDFE0}, /* ��: GB2312 Code: 0xE0DF ==> Row:64 Col:63 */
    {0x565C,0xE0E0}, /* ��: GB2312 Code: 0xE0E0 ==> Row:64 Col:64 */
    {0x5662,0xDEE0}, /* ��: GB2312 Code: 0xE0DE ==> Row:64 Col:62 */
    {0x5664,0xE4E0}, /* ��: GB2312 Code: 0xE0E4 ==> Row:64 Col:68 */
    {0x5668,0xF7C6}, /* ��: GB2312 Code: 0xC6F7 ==> Row:38 Col:87 */
    {0x5669,0xACD8}, /* ج: GB2312 Code: 0xD8AC ==> Row:56 Col:12 */
    {0x566A,0xEBD4}, /* ��: GB2312 Code: 0xD4EB ==> Row:52 Col:75 */
    {0x566B,0xE6E0}, /* ��: GB2312 Code: 0xE0E6 ==> Row:64 Col:70 */
    {0x566C,0xC9CA}, /* ��: GB2312 Code: 0xCAC9 ==> Row:42 Col:41 */
    {0x5671,0xE5E0}, /* ��: GB2312 Code: 0xE0E5 ==> Row:64 Col:69 */
    {0x5676,0xC1B8}, /* ��: GB2312 Code: 0xB8C1 ==> Row:24 Col:33 */
    {0x567B,0xE7E0}, /* ��: GB2312 Code: 0xE0E7 ==> Row:64 Col:71 */
    {0x567C,0xE8E0}, /* ��: GB2312 Code: 0xE0E8 ==> Row:64 Col:72 */
    {0x5685,0xE9E0}, /* ��: GB2312 Code: 0xE0E9 ==> Row:64 Col:73 */
    {0x5686,0xE3E0}, /* ��: GB2312 Code: 0xE0E3 ==> Row:64 Col:67 */
    {0x568E,0xBFBA}, /* ��: GB2312 Code: 0xBABF ==> Row:26 Col:31 */
    {0x568F,0xE7CC}, /* ��: GB2312 Code: 0xCCE7 ==> Row:44 Col:71 */
    {0x5693,0xEAE0}, /* ��: GB2312 Code: 0xE0EA ==> Row:64 Col:74 */
    {0x56A3,0xF9CF}, /* ��: GB2312 Code: 0xCFF9 ==> Row:47 Col:89 */
    {0x56AF,0xEBE0}, /* ��: GB2312 Code: 0xE0EB ==> Row:64 Col:75 */
    {0x56B7,0xC2C8}, /* ��: GB2312 Code: 0xC8C2 ==> Row:40 Col:34 */
    {0x56BC,0xC0BD}, /* ��: GB2312 Code: 0xBDC0 ==> Row:29 Col:32 */
    {0x56CA,0xD2C4}, /* ��: GB2312 Code: 0xC4D2 ==> Row:36 Col:50 */
    {0x56D4,0xECE0}, /* ��: GB2312 Code: 0xE0EC ==> Row:64 Col:76 */
    {0x56D7,0xEDE0}, /* ��: GB2312 Code: 0xE0ED ==> Row:64 Col:77 */
    {0x56DA,0xF4C7}, /* ��: GB2312 Code: 0xC7F4 ==> Row:39 Col:84 */
    {0x56DB,0xC4CB}, /* ��: GB2312 Code: 0xCBC4 ==> Row:43 Col:36 */
    {0x56DD,0xEEE0}, /* ��: GB2312 Code: 0xE0EE ==> Row:64 Col:78 */
    {0x56DE,0xD8BB}, /* ��: GB2312 Code: 0xBBD8 ==> Row:27 Col:56 */
    {0x56DF,0xB6D8}, /* ض: GB2312 Code: 0xD8B6 ==> Row:56 Col:22 */
    {0x56E0,0xF2D2}, /* ��: GB2312 Code: 0xD2F2 ==> Row:50 Col:82 */
    {0x56E1,0xEFE0}, /* ��: GB2312 Code: 0xE0EF ==> Row:64 Col:79 */
    {0x56E2,0xC5CD}, /* ��: GB2312 Code: 0xCDC5 ==> Row:45 Col:37 */
    {0x56E4,0xDAB6}, /* ��: GB2312 Code: 0xB6DA ==> Row:22 Col:58 */
    {0x56EB,0xF1E0}, /* ��: GB2312 Code: 0xE0F1 ==> Row:64 Col:81 */
    {0x56ED,0xB0D4}, /* ԰: GB2312 Code: 0xD4B0 ==> Row:52 Col:16 */
    {0x56F0,0xA7C0}, /* ��: GB2312 Code: 0xC0A7 ==> Row:32 Col:07 */
    {0x56F1,0xD1B4}, /* ��: GB2312 Code: 0xB4D1 ==> Row:20 Col:49 */
    {0x56F4,0xA7CE}, /* Χ: GB2312 Code: 0xCEA7 ==> Row:46 Col:07 */
    {0x56F5,0xF0E0}, /* ��: GB2312 Code: 0xE0F0 ==> Row:64 Col:80 */
    {0x56F9,0xF2E0}, /* ��: GB2312 Code: 0xE0F2 ==> Row:64 Col:82 */
    {0x56FA,0xCCB9}, /* ��: GB2312 Code: 0xB9CC ==> Row:25 Col:44 */
    {0x56FD,0xFAB9}, /* ��: GB2312 Code: 0xB9FA ==> Row:25 Col:90 */
    {0x56FE,0xBCCD}, /* ͼ: GB2312 Code: 0xCDBC ==> Row:45 Col:28 */
    {0x56FF,0xF3E0}, /* ��: GB2312 Code: 0xE0F3 ==> Row:64 Col:83 */
    {0x5703,0xD4C6}, /* ��: GB2312 Code: 0xC6D4 ==> Row:38 Col:52 */
    {0x5704,0xF4E0}, /* ��: GB2312 Code: 0xE0F4 ==> Row:64 Col:84 */
    {0x5706,0xB2D4}, /* Բ: GB2312 Code: 0xD4B2 ==> Row:52 Col:18 */
    {0x5708,0xA6C8}, /* Ȧ: GB2312 Code: 0xC8A6 ==> Row:40 Col:06 */
    {0x5709,0xF6E0}, /* ��: GB2312 Code: 0xE0F6 ==> Row:64 Col:86 */
    {0x570A,0xF5E0}, /* ��: GB2312 Code: 0xE0F5 ==> Row:64 Col:85 */
    {0x571C,0xF7E0}, /* ��: GB2312 Code: 0xE0F7 ==> Row:64 Col:87 */
    {0x571F,0xC1CD}, /* ��: GB2312 Code: 0xCDC1 ==> Row:45 Col:33 */
    {0x5723,0xA5CA}, /* ʥ: GB2312 Code: 0xCAA5 ==> Row:42 Col:05 */
    {0x5728,0xDAD4}, /* ��: GB2312 Code: 0xD4DA ==> Row:52 Col:58 */
    {0x5729,0xD7DB}, /* ��: GB2312 Code: 0xDBD7 ==> Row:59 Col:55 */
    {0x572A,0xD9DB}, /* ��: GB2312 Code: 0xDBD9 ==> Row:59 Col:57 */
    {0x572C,0xD8DB}, /* ��: GB2312 Code: 0xDBD8 ==> Row:59 Col:56 */
    {0x572D,0xE7B9}, /* ��: GB2312 Code: 0xB9E7 ==> Row:25 Col:71 */
    {0x572E,0xDCDB}, /* ��: GB2312 Code: 0xDBDC ==> Row:59 Col:60 */
    {0x572F,0xDDDB}, /* ��: GB2312 Code: 0xDBDD ==> Row:59 Col:61 */
    {0x5730,0xD8B5}, /* ��: GB2312 Code: 0xB5D8 ==> Row:21 Col:56 */
    {0x5733,0xDADB}, /* ��: GB2312 Code: 0xDBDA ==> Row:59 Col:58 */
    {0x5739,0xDBDB}, /* ��: GB2312 Code: 0xDBDB ==> Row:59 Col:59 */
    {0x573A,0xA1B3}, /* ��: GB2312 Code: 0xB3A1 ==> Row:19 Col:01 */
    {0x573B,0xDFDB}, /* ��: GB2312 Code: 0xDBDF ==> Row:59 Col:63 */
    {0x573E,0xF8BB}, /* ��: GB2312 Code: 0xBBF8 ==> Row:27 Col:88 */
    {0x5740,0xB7D6}, /* ַ: GB2312 Code: 0xD6B7 ==> Row:54 Col:23 */
    {0x5742,0xE0DB}, /* ��: GB2312 Code: 0xDBE0 ==> Row:59 Col:64 */
    {0x5747,0xF9BE}, /* ��: GB2312 Code: 0xBEF9 ==> Row:30 Col:89 */
    {0x574A,0xBBB7}, /* ��: GB2312 Code: 0xB7BB ==> Row:23 Col:27 */
    {0x574C,0xD0DB}, /* ��: GB2312 Code: 0xDBD0 ==> Row:59 Col:48 */
    {0x574D,0xAECC}, /* ̮: GB2312 Code: 0xCCAE ==> Row:44 Col:14 */
    {0x574E,0xB2BF}, /* ��: GB2312 Code: 0xBFB2 ==> Row:31 Col:18 */
    {0x574F,0xB5BB}, /* ��: GB2312 Code: 0xBBB5 ==> Row:27 Col:21 */
    {0x5750,0xF8D7}, /* ��: GB2312 Code: 0xD7F8 ==> Row:55 Col:88 */
    {0x5751,0xD3BF}, /* ��: GB2312 Code: 0xBFD3 ==> Row:31 Col:51 */
    {0x5757,0xE9BF}, /* ��: GB2312 Code: 0xBFE9 ==> Row:31 Col:73 */
    {0x575A,0xE1BC}, /* ��: GB2312 Code: 0xBCE1 ==> Row:28 Col:65 */
    {0x575B,0xB3CC}, /* ̳: GB2312 Code: 0xCCB3 ==> Row:44 Col:19 */
    {0x575C,0xDEDB}, /* ��: GB2312 Code: 0xDBDE ==> Row:59 Col:62 */
    {0x575D,0xD3B0}, /* ��: GB2312 Code: 0xB0D3 ==> Row:16 Col:51 */
    {0x575E,0xEBCE}, /* ��: GB2312 Code: 0xCEEB ==> Row:46 Col:75 */
    {0x575F,0xD8B7}, /* ��: GB2312 Code: 0xB7D8 ==> Row:23 Col:56 */
    {0x5760,0xB9D7}, /* ׹: GB2312 Code: 0xD7B9 ==> Row:55 Col:25 */
    {0x5761,0xC2C6}, /* ��: GB2312 Code: 0xC6C2 ==> Row:38 Col:34 */
    {0x5764,0xA4C0}, /* ��: GB2312 Code: 0xC0A4 ==> Row:32 Col:04 */
    {0x5766,0xB9CC}, /* ̹: GB2312 Code: 0xCCB9 ==> Row:44 Col:25 */
    {0x5768,0xE7DB}, /* ��: GB2312 Code: 0xDBE7 ==> Row:59 Col:71 */
    {0x5769,0xE1DB}, /* ��: GB2312 Code: 0xDBE1 ==> Row:59 Col:65 */
    {0x576A,0xBAC6}, /* ƺ: GB2312 Code: 0xC6BA ==> Row:38 Col:26 */
    {0x576B,0xE3DB}, /* ��: GB2312 Code: 0xDBE3 ==> Row:59 Col:67 */
    {0x576D,0xE8DB}, /* ��: GB2312 Code: 0xDBE8 ==> Row:59 Col:72 */
    {0x576F,0xF7C5}, /* ��: GB2312 Code: 0xC5F7 ==> Row:37 Col:87 */
    {0x5773,0xEADB}, /* ��: GB2312 Code: 0xDBEA ==> Row:59 Col:74 */
    {0x5776,0xE9DB}, /* ��: GB2312 Code: 0xDBE9 ==> Row:59 Col:73 */
    {0x5777,0xC0BF}, /* ��: GB2312 Code: 0xBFC0 ==> Row:31 Col:32 */
    {0x577B,0xE6DB}, /* ��: GB2312 Code: 0xDBE6 ==> Row:59 Col:70 */
    {0x577C,0xE5DB}, /* ��: GB2312 Code: 0xDBE5 ==> Row:59 Col:69 */
    {0x5782,0xB9B4}, /* ��: GB2312 Code: 0xB4B9 ==> Row:20 Col:25 */
    {0x5783,0xACC0}, /* ��: GB2312 Code: 0xC0AC ==> Row:32 Col:12 */
    {0x5784,0xA2C2}, /* ¢: GB2312 Code: 0xC2A2 ==> Row:34 Col:02 */
    {0x5785,0xE2DB}, /* ��: GB2312 Code: 0xDBE2 ==> Row:59 Col:66 */
    {0x5786,0xE4DB}, /* ��: GB2312 Code: 0xDBE4 ==> Row:59 Col:68 */
    {0x578B,0xCDD0}, /* ��: GB2312 Code: 0xD0CD ==> Row:48 Col:45 */
    {0x578C,0xEDDB}, /* ��: GB2312 Code: 0xDBED ==> Row:59 Col:77 */
    {0x5792,0xDDC0}, /* ��: GB2312 Code: 0xC0DD ==> Row:32 Col:61 */
    {0x5793,0xF2DB}, /* ��: GB2312 Code: 0xDBF2 ==> Row:59 Col:82 */
    {0x579B,0xE2B6}, /* ��: GB2312 Code: 0xB6E2 ==> Row:22 Col:66 */
    {0x57A0,0xF3DB}, /* ��: GB2312 Code: 0xDBF3 ==> Row:59 Col:83 */
    {0x57A1,0xD2DB}, /* ��: GB2312 Code: 0xDBD2 ==> Row:59 Col:50 */
    {0x57A2,0xB8B9}, /* ��: GB2312 Code: 0xB9B8 ==> Row:25 Col:24 */
    {0x57A3,0xABD4}, /* ԫ: GB2312 Code: 0xD4AB ==> Row:52 Col:11 */
    {0x57A4,0xECDB}, /* ��: GB2312 Code: 0xDBEC ==> Row:59 Col:76 */
    {0x57A6,0xD1BF}, /* ��: GB2312 Code: 0xBFD1 ==> Row:31 Col:49 */
    {0x57A7,0xF0DB}, /* ��: GB2312 Code: 0xDBF0 ==> Row:59 Col:80 */
    {0x57A9,0xD1DB}, /* ��: GB2312 Code: 0xDBD1 ==> Row:59 Col:49 */
    {0x57AB,0xE6B5}, /* ��: GB2312 Code: 0xB5E6 ==> Row:21 Col:70 */
    {0x57AD,0xEBDB}, /* ��: GB2312 Code: 0xDBEB ==> Row:59 Col:75 */
    {0x57AE,0xE5BF}, /* ��: GB2312 Code: 0xBFE5 ==> Row:31 Col:69 */
    {0x57B2,0xEEDB}, /* ��: GB2312 Code: 0xDBEE ==> Row:59 Col:78 */
    {0x57B4,0xF1DB}, /* ��: GB2312 Code: 0xDBF1 ==> Row:59 Col:81 */
    {0x57B8,0xF9DB}, /* ��: GB2312 Code: 0xDBF9 ==> Row:59 Col:89 */
    {0x57C2,0xA1B9}, /* ��: GB2312 Code: 0xB9A1 ==> Row:25 Col:01 */
    {0x57C3,0xA3B0}, /* ��: GB2312 Code: 0xB0A3 ==> Row:16 Col:03 */
    {0x57CB,0xF1C2}, /* ��: GB2312 Code: 0xC2F1 ==> Row:34 Col:81 */
    {0x57CE,0xC7B3}, /* ��: GB2312 Code: 0xB3C7 ==> Row:19 Col:39 */
    {0x57CF,0xEFDB}, /* ��: GB2312 Code: 0xDBEF ==> Row:59 Col:79 */
    {0x57D2,0xF8DB}, /* ��: GB2312 Code: 0xDBF8 ==> Row:59 Col:88 */
    {0x57D4,0xD2C6}, /* ��: GB2312 Code: 0xC6D2 ==> Row:38 Col:50 */
    {0x57D5,0xF4DB}, /* ��: GB2312 Code: 0xDBF4 ==> Row:59 Col:84 */
    {0x57D8,0xF5DB}, /* ��: GB2312 Code: 0xDBF5 ==> Row:59 Col:85 */
    {0x57D9,0xF7DB}, /* ��: GB2312 Code: 0xDBF7 ==> Row:59 Col:87 */
    {0x57DA,0xF6DB}, /* ��: GB2312 Code: 0xDBF6 ==> Row:59 Col:86 */
    {0x57DD,0xFEDB}, /* ��: GB2312 Code: 0xDBFE ==> Row:59 Col:94 */
    {0x57DF,0xF2D3}, /* ��: GB2312 Code: 0xD3F2 ==> Row:51 Col:82 */
    {0x57E0,0xBAB2}, /* ��: GB2312 Code: 0xB2BA ==> Row:18 Col:26 */
    {0x57E4,0xFDDB}, /* ��: GB2312 Code: 0xDBFD ==> Row:59 Col:93 */
    {0x57ED,0xA4DC}, /* ܤ: GB2312 Code: 0xDCA4 ==> Row:60 Col:04 */
    {0x57EF,0xFBDB}, /* ��: GB2312 Code: 0xDBFB ==> Row:59 Col:91 */
    {0x57F4,0xFADB}, /* ��: GB2312 Code: 0xDBFA ==> Row:59 Col:90 */
    {0x57F8,0xFCDB}, /* ��: GB2312 Code: 0xDBFC ==> Row:59 Col:92 */
    {0x57F9,0xE0C5}, /* ��: GB2312 Code: 0xC5E0 ==> Row:37 Col:64 */
    {0x57FA,0xF9BB}, /* ��: GB2312 Code: 0xBBF9 ==> Row:27 Col:89 */
    {0x57FD,0xA3DC}, /* ܣ: GB2312 Code: 0xDCA3 ==> Row:60 Col:03 */
    {0x5800,0xA5DC}, /* ܥ: GB2312 Code: 0xDCA5 ==> Row:60 Col:05 */
    {0x5802,0xC3CC}, /* ��: GB2312 Code: 0xCCC3 ==> Row:44 Col:35 */
    {0x5806,0xD1B6}, /* ��: GB2312 Code: 0xB6D1 ==> Row:22 Col:49 */
    {0x5807,0xC0DD}, /* ��: GB2312 Code: 0xDDC0 ==> Row:61 Col:32 */
    {0x580B,0xA1DC}, /* ܡ: GB2312 Code: 0xDCA1 ==> Row:60 Col:01 */
    {0x580D,0xA2DC}, /* ܢ: GB2312 Code: 0xDCA2 ==> Row:60 Col:02 */
    {0x5811,0xB5C7}, /* ǵ: GB2312 Code: 0xC7B5 ==> Row:39 Col:21 */
    {0x5815,0xE9B6}, /* ��: GB2312 Code: 0xB6E9 ==> Row:22 Col:73 */
    {0x5819,0xA7DC}, /* ܧ: GB2312 Code: 0xDCA7 ==> Row:60 Col:07 */
    {0x581E,0xA6DC}, /* ܦ: GB2312 Code: 0xDCA6 ==> Row:60 Col:06 */
    {0x5820,0xA9DC}, /* ܩ: GB2312 Code: 0xDCA9 ==> Row:60 Col:09 */
    {0x5821,0xA4B1}, /* ��: GB2312 Code: 0xB1A4 ==> Row:17 Col:04 */
    {0x5824,0xCCB5}, /* ��: GB2312 Code: 0xB5CC ==> Row:21 Col:44 */
    {0x582A,0xB0BF}, /* ��: GB2312 Code: 0xBFB0 ==> Row:31 Col:16 */
    {0x5830,0xDFD1}, /* ��: GB2312 Code: 0xD1DF ==> Row:49 Col:63 */
    {0x5835,0xC2B6}, /* ��: GB2312 Code: 0xB6C2 ==> Row:22 Col:34 */
    {0x5844,0xA8DC}, /* ܨ: GB2312 Code: 0xDCA8 ==> Row:60 Col:08 */
    {0x584C,0xFACB}, /* ��: GB2312 Code: 0xCBFA ==> Row:43 Col:90 */
    {0x584D,0xF3EB}, /* ��: GB2312 Code: 0xEBF3 ==> Row:75 Col:83 */
    {0x5851,0xDCCB}, /* ��: GB2312 Code: 0xCBDC ==> Row:43 Col:60 */
    {0x5854,0xFECB}, /* ��: GB2312 Code: 0xCBFE ==> Row:43 Col:94 */
    {0x5858,0xC1CC}, /* ��: GB2312 Code: 0xCCC1 ==> Row:44 Col:33 */
    {0x585E,0xFBC8}, /* ��: GB2312 Code: 0xC8FB ==> Row:40 Col:91 */
    {0x5865,0xAADC}, /* ܪ: GB2312 Code: 0xDCAA ==> Row:60 Col:10 */
    {0x586B,0xEECC}, /* ��: GB2312 Code: 0xCCEE ==> Row:44 Col:78 */
    {0x586C,0xABDC}, /* ܫ: GB2312 Code: 0xDCAB ==> Row:60 Col:11 */
    {0x587E,0xD3DB}, /* ��: GB2312 Code: 0xDBD3 ==> Row:59 Col:51 */
    {0x5880,0xAFDC}, /* ܯ: GB2312 Code: 0xDCAF ==> Row:60 Col:15 */
    {0x5881,0xACDC}, /* ܬ: GB2312 Code: 0xDCAC ==> Row:60 Col:12 */
    {0x5883,0xB3BE}, /* ��: GB2312 Code: 0xBEB3 ==> Row:30 Col:19 */
    {0x5885,0xFBCA}, /* ��: GB2312 Code: 0xCAFB ==> Row:42 Col:91 */
    {0x5889,0xADDC}, /* ܭ: GB2312 Code: 0xDCAD ==> Row:60 Col:13 */
    {0x5892,0xCAC9}, /* ��: GB2312 Code: 0xC9CA ==> Row:41 Col:42 */
    {0x5893,0xB9C4}, /* Ĺ: GB2312 Code: 0xC4B9 ==> Row:36 Col:25 */
    {0x5899,0xBDC7}, /* ǽ: GB2312 Code: 0xC7BD ==> Row:39 Col:29 */
    {0x589A,0xAEDC}, /* ܮ: GB2312 Code: 0xDCAE ==> Row:60 Col:14 */
    {0x589E,0xF6D4}, /* ��: GB2312 Code: 0xD4F6 ==> Row:52 Col:86 */
    {0x589F,0xE6D0}, /* ��: GB2312 Code: 0xD0E6 ==> Row:48 Col:70 */
    {0x58A8,0xABC4}, /* ī: GB2312 Code: 0xC4AB ==> Row:36 Col:11 */
    {0x58A9,0xD5B6}, /* ��: GB2312 Code: 0xB6D5 ==> Row:22 Col:53 */
    {0x58BC,0xD4DB}, /* ��: GB2312 Code: 0xDBD4 ==> Row:59 Col:52 */
    {0x58C1,0xDAB1}, /* ��: GB2312 Code: 0xB1DA ==> Row:17 Col:58 */
    {0x58C5,0xD5DB}, /* ��: GB2312 Code: 0xDBD5 ==> Row:59 Col:53 */
    {0x58D1,0xD6DB}, /* ��: GB2312 Code: 0xDBD6 ==> Row:59 Col:54 */
    {0x58D5,0xBEBA}, /* ��: GB2312 Code: 0xBABE ==> Row:26 Col:30 */
    {0x58E4,0xC0C8}, /* ��: GB2312 Code: 0xC8C0 ==> Row:40 Col:32 */
    {0x58EB,0xBFCA}, /* ʿ: GB2312 Code: 0xCABF ==> Row:42 Col:31 */
    {0x58EC,0xC9C8}, /* ��: GB2312 Code: 0xC8C9 ==> Row:40 Col:41 */
    {0x58EE,0xB3D7}, /* ׳: GB2312 Code: 0xD7B3 ==> Row:55 Col:19 */
    {0x58F0,0xF9C9}, /* ��: GB2312 Code: 0xC9F9 ==> Row:41 Col:89 */
    {0x58F3,0xC7BF}, /* ��: GB2312 Code: 0xBFC7 ==> Row:31 Col:39 */
    {0x58F6,0xF8BA}, /* ��: GB2312 Code: 0xBAF8 ==> Row:26 Col:88 */
    {0x58F9,0xBCD2}, /* Ҽ: GB2312 Code: 0xD2BC ==> Row:50 Col:28 */
    {0x5902,0xBAE2}, /* �: GB2312 Code: 0xE2BA ==> Row:66 Col:26 */
    {0x5904,0xA6B4}, /* ��: GB2312 Code: 0xB4A6 ==> Row:20 Col:06 */
    {0x5907,0xB8B1}, /* ��: GB2312 Code: 0xB1B8 ==> Row:17 Col:24 */
    {0x590D,0xB4B8}, /* ��: GB2312 Code: 0xB8B4 ==> Row:24 Col:20 */
    {0x590F,0xC4CF}, /* ��: GB2312 Code: 0xCFC4 ==> Row:47 Col:36 */
    {0x5914,0xE7D9}, /* ��: GB2312 Code: 0xD9E7 ==> Row:57 Col:71 */
    {0x5915,0xA6CF}, /* Ϧ: GB2312 Code: 0xCFA6 ==> Row:47 Col:06 */
    {0x5916,0xE2CD}, /* ��: GB2312 Code: 0xCDE2 ==> Row:45 Col:66 */
    {0x5919,0xEDD9}, /* ��: GB2312 Code: 0xD9ED ==> Row:57 Col:77 */
    {0x591A,0xE0B6}, /* ��: GB2312 Code: 0xB6E0 ==> Row:22 Col:64 */
    {0x591C,0xB9D2}, /* ҹ: GB2312 Code: 0xD2B9 ==> Row:50 Col:25 */
    {0x591F,0xBBB9}, /* ��: GB2312 Code: 0xB9BB ==> Row:25 Col:27 */
    {0x5924,0xB9E2}, /* �: GB2312 Code: 0xE2B9 ==> Row:66 Col:25 */
    {0x5925,0xB7E2}, /* �: GB2312 Code: 0xE2B7 ==> Row:66 Col:23 */
    {0x5927,0xF3B4}, /* ��: GB2312 Code: 0xB4F3 ==> Row:20 Col:83 */
    {0x5929,0xECCC}, /* ��: GB2312 Code: 0xCCEC ==> Row:44 Col:76 */
    {0x592A,0xABCC}, /* ̫: GB2312 Code: 0xCCAB ==> Row:44 Col:11 */
    {0x592B,0xF2B7}, /* ��: GB2312 Code: 0xB7F2 ==> Row:23 Col:82 */
    {0x592D,0xB2D8}, /* ز: GB2312 Code: 0xD8B2 ==> Row:56 Col:18 */
    {0x592E,0xEBD1}, /* ��: GB2312 Code: 0xD1EB ==> Row:49 Col:75 */
    {0x592F,0xBBBA}, /* ��: GB2312 Code: 0xBABB ==> Row:26 Col:27 */
    {0x5931,0xA7CA}, /* ʧ: GB2312 Code: 0xCAA7 ==> Row:42 Col:07 */
    {0x5934,0xB7CD}, /* ͷ: GB2312 Code: 0xCDB7 ==> Row:45 Col:23 */
    {0x5937,0xC4D2}, /* ��: GB2312 Code: 0xD2C4 ==> Row:50 Col:36 */
    {0x5938,0xE4BF}, /* ��: GB2312 Code: 0xBFE4 ==> Row:31 Col:68 */
    {0x5939,0xD0BC}, /* ��: GB2312 Code: 0xBCD0 ==> Row:28 Col:48 */
    {0x593A,0xE1B6}, /* ��: GB2312 Code: 0xB6E1 ==> Row:22 Col:65 */
    {0x593C,0xC5DE}, /* ��: GB2312 Code: 0xDEC5 ==> Row:62 Col:37 */
    {0x5941,0xC6DE}, /* ��: GB2312 Code: 0xDEC6 ==> Row:62 Col:38 */
    {0x5942,0xBCDB}, /* ۼ: GB2312 Code: 0xDBBC ==> Row:59 Col:28 */
    {0x5944,0xD9D1}, /* ��: GB2312 Code: 0xD1D9 ==> Row:49 Col:57 */
    {0x5947,0xE6C6}, /* ��: GB2312 Code: 0xC6E6 ==> Row:38 Col:70 */
    {0x5948,0xCEC4}, /* ��: GB2312 Code: 0xC4CE ==> Row:36 Col:46 */
    {0x5949,0xEEB7}, /* ��: GB2312 Code: 0xB7EE ==> Row:23 Col:78 */
    {0x594B,0xDCB7}, /* ��: GB2312 Code: 0xB7DC ==> Row:23 Col:60 */
    {0x594E,0xFCBF}, /* ��: GB2312 Code: 0xBFFC ==> Row:31 Col:92 */
    {0x594F,0xE0D7}, /* ��: GB2312 Code: 0xD7E0 ==> Row:55 Col:64 */
    {0x5951,0xF5C6}, /* ��: GB2312 Code: 0xC6F5 ==> Row:38 Col:85 */
    {0x5954,0xBCB1}, /* ��: GB2312 Code: 0xB1BC ==> Row:17 Col:28 */
    {0x5955,0xC8DE}, /* ��: GB2312 Code: 0xDEC8 ==> Row:62 Col:40 */
    {0x5956,0xB1BD}, /* ��: GB2312 Code: 0xBDB1 ==> Row:29 Col:17 */
    {0x5957,0xD7CC}, /* ��: GB2312 Code: 0xCCD7 ==> Row:44 Col:55 */
    {0x5958,0xCADE}, /* ��: GB2312 Code: 0xDECA ==> Row:62 Col:42 */
    {0x595A,0xC9DE}, /* ��: GB2312 Code: 0xDEC9 ==> Row:62 Col:41 */
    {0x5960,0xECB5}, /* ��: GB2312 Code: 0xB5EC ==> Row:21 Col:76 */
    {0x5962,0xDDC9}, /* ��: GB2312 Code: 0xC9DD ==> Row:41 Col:61 */
    {0x5965,0xC2B0}, /* ��: GB2312 Code: 0xB0C2 ==> Row:16 Col:34 */
    {0x5973,0xAEC5}, /* Ů: GB2312 Code: 0xC5AE ==> Row:37 Col:14 */
    {0x5974,0xABC5}, /* ū: GB2312 Code: 0xC5AB ==> Row:37 Col:11 */
    {0x5976,0xCCC4}, /* ��: GB2312 Code: 0xC4CC ==> Row:36 Col:44 */
    {0x5978,0xE9BC}, /* ��: GB2312 Code: 0xBCE9 ==> Row:28 Col:73 */
    {0x5979,0xFDCB}, /* ��: GB2312 Code: 0xCBFD ==> Row:43 Col:93 */
    {0x597D,0xC3BA}, /* ��: GB2312 Code: 0xBAC3 ==> Row:26 Col:35 */
    {0x5981,0xF9E5}, /* ��: GB2312 Code: 0xE5F9 ==> Row:69 Col:89 */
    {0x5982,0xE7C8}, /* ��: GB2312 Code: 0xC8E7 ==> Row:40 Col:71 */
    {0x5983,0xFAE5}, /* ��: GB2312 Code: 0xE5FA ==> Row:69 Col:90 */
    {0x5984,0xFDCD}, /* ��: GB2312 Code: 0xCDFD ==> Row:45 Col:93 */
    {0x5986,0xB1D7}, /* ױ: GB2312 Code: 0xD7B1 ==> Row:55 Col:17 */
    {0x5987,0xBEB8}, /* ��: GB2312 Code: 0xB8BE ==> Row:24 Col:30 */
    {0x5988,0xE8C2}, /* ��: GB2312 Code: 0xC2E8 ==> Row:34 Col:72 */
    {0x598A,0xD1C8}, /* ��: GB2312 Code: 0xC8D1 ==> Row:40 Col:49 */
    {0x598D,0xFBE5}, /* ��: GB2312 Code: 0xE5FB ==> Row:69 Col:91 */
    {0x5992,0xCAB6}, /* ��: GB2312 Code: 0xB6CA ==> Row:22 Col:42 */
    {0x5993,0xCBBC}, /* ��: GB2312 Code: 0xBCCB ==> Row:28 Col:43 */
    {0x5996,0xFDD1}, /* ��: GB2312 Code: 0xD1FD ==> Row:49 Col:93 */
    {0x5997,0xA1E6}, /* �: GB2312 Code: 0xE6A1 ==> Row:70 Col:01 */
    {0x5999,0xEEC3}, /* ��: GB2312 Code: 0xC3EE ==> Row:35 Col:78 */
    {0x599E,0xA4E6}, /* �: GB2312 Code: 0xE6A4 ==> Row:70 Col:04 */
    {0x59A3,0xFEE5}, /* ��: GB2312 Code: 0xE5FE ==> Row:69 Col:94 */
    {0x59A4,0xA5E6}, /* �: GB2312 Code: 0xE6A5 ==> Row:70 Col:05 */
    {0x59A5,0xD7CD}, /* ��: GB2312 Code: 0xCDD7 ==> Row:45 Col:55 */
    {0x59A8,0xC1B7}, /* ��: GB2312 Code: 0xB7C1 ==> Row:23 Col:33 */
    {0x59A9,0xFCE5}, /* ��: GB2312 Code: 0xE5FC ==> Row:69 Col:92 */
    {0x59AA,0xFDE5}, /* ��: GB2312 Code: 0xE5FD ==> Row:69 Col:93 */
    {0x59AB,0xA3E6}, /* �: GB2312 Code: 0xE6A3 ==> Row:70 Col:03 */
    {0x59AE,0xDDC4}, /* ��: GB2312 Code: 0xC4DD ==> Row:36 Col:61 */
    {0x59AF,0xA8E6}, /* �: GB2312 Code: 0xE6A8 ==> Row:70 Col:08 */
    {0x59B2,0xA7E6}, /* �: GB2312 Code: 0xE6A7 ==> Row:70 Col:07 */
    {0x59B9,0xC3C3}, /* ��: GB2312 Code: 0xC3C3 ==> Row:35 Col:35 */
    {0x59BB,0xDEC6}, /* ��: GB2312 Code: 0xC6DE ==> Row:38 Col:62 */
    {0x59BE,0xAAE6}, /* �: GB2312 Code: 0xE6AA ==> Row:70 Col:10 */
    {0x59C6,0xB7C4}, /* ķ: GB2312 Code: 0xC4B7 ==> Row:36 Col:23 */
    {0x59CA,0xA2E6}, /* �: GB2312 Code: 0xE6A2 ==> Row:70 Col:02 */
    {0x59CB,0xBCCA}, /* ʼ: GB2312 Code: 0xCABC ==> Row:42 Col:28 */
    {0x59D0,0xE3BD}, /* ��: GB2312 Code: 0xBDE3 ==> Row:29 Col:67 */
    {0x59D1,0xC3B9}, /* ��: GB2312 Code: 0xB9C3 ==> Row:25 Col:35 */
    {0x59D2,0xA6E6}, /* �: GB2312 Code: 0xE6A6 ==> Row:70 Col:06 */
    {0x59D3,0xD5D0}, /* ��: GB2312 Code: 0xD0D5 ==> Row:48 Col:53 */
    {0x59D4,0xAFCE}, /* ί: GB2312 Code: 0xCEAF ==> Row:46 Col:15 */
    {0x59D7,0xA9E6}, /* �: GB2312 Code: 0xE6A9 ==> Row:70 Col:09 */
    {0x59D8,0xB0E6}, /* �: GB2312 Code: 0xE6B0 ==> Row:70 Col:16 */
    {0x59DA,0xA6D2}, /* Ҧ: GB2312 Code: 0xD2A6 ==> Row:50 Col:06 */
    {0x59DC,0xAABD}, /* ��: GB2312 Code: 0xBDAA ==> Row:29 Col:10 */
    {0x59DD,0xADE6}, /* �: GB2312 Code: 0xE6AD ==> Row:70 Col:13 */
    {0x59E3,0xAFE6}, /* �: GB2312 Code: 0xE6AF ==> Row:70 Col:15 */
    {0x59E5,0xD1C0}, /* ��: GB2312 Code: 0xC0D1 ==> Row:32 Col:49 */
    {0x59E8,0xCCD2}, /* ��: GB2312 Code: 0xD2CC ==> Row:50 Col:44 */
    {0x59EC,0xA7BC}, /* ��: GB2312 Code: 0xBCA7 ==> Row:28 Col:07 */
    {0x59F9,0xB1E6}, /* �: GB2312 Code: 0xE6B1 ==> Row:70 Col:17 */
    {0x59FB,0xF6D2}, /* ��: GB2312 Code: 0xD2F6 ==> Row:50 Col:86 */
    {0x59FF,0xCBD7}, /* ��: GB2312 Code: 0xD7CB ==> Row:55 Col:43 */
    {0x5A01,0xFECD}, /* ��: GB2312 Code: 0xCDFE ==> Row:45 Col:94 */
    {0x5A03,0xDECD}, /* ��: GB2312 Code: 0xCDDE ==> Row:45 Col:62 */
    {0x5A04,0xA6C2}, /* ¦: GB2312 Code: 0xC2A6 ==> Row:34 Col:06 */
    {0x5A05,0xABE6}, /* �: GB2312 Code: 0xE6AB ==> Row:70 Col:11 */
    {0x5A06,0xACE6}, /* �: GB2312 Code: 0xE6AC ==> Row:70 Col:12 */
    {0x5A07,0xBFBD}, /* ��: GB2312 Code: 0xBDBF ==> Row:29 Col:31 */
    {0x5A08,0xAEE6}, /* �: GB2312 Code: 0xE6AE ==> Row:70 Col:14 */
    {0x5A09,0xB3E6}, /* �: GB2312 Code: 0xE6B3 ==> Row:70 Col:19 */
    {0x5A0C,0xB2E6}, /* �: GB2312 Code: 0xE6B2 ==> Row:70 Col:18 */
    {0x5A11,0xB6E6}, /* �: GB2312 Code: 0xE6B6 ==> Row:70 Col:22 */
    {0x5A13,0xB8E6}, /* �: GB2312 Code: 0xE6B8 ==> Row:70 Col:24 */
    {0x5A18,0xEFC4}, /* ��: GB2312 Code: 0xC4EF ==> Row:36 Col:79 */
    {0x5A1C,0xC8C4}, /* ��: GB2312 Code: 0xC4C8 ==> Row:36 Col:40 */
    {0x5A1F,0xEABE}, /* ��: GB2312 Code: 0xBEEA ==> Row:30 Col:74 */
    {0x5A20,0xEFC9}, /* ��: GB2312 Code: 0xC9EF ==> Row:41 Col:79 */
    {0x5A23,0xB7E6}, /* �: GB2312 Code: 0xE6B7 ==> Row:70 Col:23 */
    {0x5A25,0xF0B6}, /* ��: GB2312 Code: 0xB6F0 ==> Row:22 Col:80 */
    {0x5A29,0xE4C3}, /* ��: GB2312 Code: 0xC3E4 ==> Row:35 Col:68 */
    {0x5A31,0xE9D3}, /* ��: GB2312 Code: 0xD3E9 ==> Row:51 Col:73 */
    {0x5A32,0xB4E6}, /* �: GB2312 Code: 0xE6B4 ==> Row:70 Col:20 */
    {0x5A34,0xB5E6}, /* �: GB2312 Code: 0xE6B5 ==> Row:70 Col:21 */
    {0x5A36,0xA2C8}, /* Ȣ: GB2312 Code: 0xC8A2 ==> Row:40 Col:02 */
    {0x5A3C,0xBDE6}, /* �: GB2312 Code: 0xE6BD ==> Row:70 Col:29 */
    {0x5A40,0xB9E6}, /* �: GB2312 Code: 0xE6B9 ==> Row:70 Col:25 */
    {0x5A46,0xC5C6}, /* ��: GB2312 Code: 0xC6C5 ==> Row:38 Col:37 */
    {0x5A49,0xF1CD}, /* ��: GB2312 Code: 0xCDF1 ==> Row:45 Col:81 */
    {0x5A4A,0xBBE6}, /* �: GB2312 Code: 0xE6BB ==> Row:70 Col:27 */
    {0x5A55,0xBCE6}, /* �: GB2312 Code: 0xE6BC ==> Row:70 Col:28 */
    {0x5A5A,0xE9BB}, /* ��: GB2312 Code: 0xBBE9 ==> Row:27 Col:73 */
    {0x5A62,0xBEE6}, /* �: GB2312 Code: 0xE6BE ==> Row:70 Col:30 */
    {0x5A67,0xBAE6}, /* �: GB2312 Code: 0xE6BA ==> Row:70 Col:26 */
    {0x5A6A,0xB7C0}, /* ��: GB2312 Code: 0xC0B7 ==> Row:32 Col:23 */
    {0x5A74,0xA4D3}, /* Ӥ: GB2312 Code: 0xD3A4 ==> Row:51 Col:04 */
    {0x5A75,0xBFE6}, /* �: GB2312 Code: 0xE6BF ==> Row:70 Col:31 */
    {0x5A76,0xF4C9}, /* ��: GB2312 Code: 0xC9F4 ==> Row:41 Col:84 */
    {0x5A77,0xC3E6}, /* ��: GB2312 Code: 0xE6C3 ==> Row:70 Col:35 */
    {0x5A7A,0xC4E6}, /* ��: GB2312 Code: 0xE6C4 ==> Row:70 Col:36 */
    {0x5A7F,0xF6D0}, /* ��: GB2312 Code: 0xD0F6 ==> Row:48 Col:86 */
    {0x5A92,0xBDC3}, /* ý: GB2312 Code: 0xC3BD ==> Row:35 Col:29 */
    {0x5A9A,0xC4C3}, /* ��: GB2312 Code: 0xC3C4 ==> Row:35 Col:36 */
    {0x5A9B,0xC2E6}, /* ��: GB2312 Code: 0xE6C2 ==> Row:70 Col:34 */
    {0x5AAA,0xC1E6}, /* ��: GB2312 Code: 0xE6C1 ==> Row:70 Col:33 */
    {0x5AB2,0xC7E6}, /* ��: GB2312 Code: 0xE6C7 ==> Row:70 Col:39 */
    {0x5AB3,0xB1CF}, /* ϱ: GB2312 Code: 0xCFB1 ==> Row:47 Col:17 */
    {0x5AB5,0xF4EB}, /* ��: GB2312 Code: 0xEBF4 ==> Row:75 Col:84 */
    {0x5AB8,0xCAE6}, /* ��: GB2312 Code: 0xE6CA ==> Row:70 Col:42 */
    {0x5ABE,0xC5E6}, /* ��: GB2312 Code: 0xE6C5 ==> Row:70 Col:37 */
    {0x5AC1,0xDEBC}, /* ��: GB2312 Code: 0xBCDE ==> Row:28 Col:62 */
    {0x5AC2,0xA9C9}, /* ɩ: GB2312 Code: 0xC9A9 ==> Row:41 Col:09 */
    {0x5AC9,0xB5BC}, /* ��: GB2312 Code: 0xBCB5 ==> Row:28 Col:21 */
    {0x5ACC,0xD3CF}, /* ��: GB2312 Code: 0xCFD3 ==> Row:47 Col:51 */
    {0x5AD2,0xC8E6}, /* ��: GB2312 Code: 0xE6C8 ==> Row:70 Col:40 */
    {0x5AD4,0xC9E6}, /* ��: GB2312 Code: 0xE6C9 ==> Row:70 Col:41 */
    {0x5AD6,0xCEE6}, /* ��: GB2312 Code: 0xE6CE ==> Row:70 Col:46 */
    {0x5AD8,0xD0E6}, /* ��: GB2312 Code: 0xE6D0 ==> Row:70 Col:48 */
    {0x5ADC,0xD1E6}, /* ��: GB2312 Code: 0xE6D1 ==> Row:70 Col:49 */
    {0x5AE0,0xCBE6}, /* ��: GB2312 Code: 0xE6CB ==> Row:70 Col:43 */
    {0x5AE1,0xD5B5}, /* ��: GB2312 Code: 0xB5D5 ==> Row:21 Col:53 */
    {0x5AE3,0xCCE6}, /* ��: GB2312 Code: 0xE6CC ==> Row:70 Col:44 */
    {0x5AE6,0xCFE6}, /* ��: GB2312 Code: 0xE6CF ==> Row:70 Col:47 */
    {0x5AE9,0xDBC4}, /* ��: GB2312 Code: 0xC4DB ==> Row:36 Col:59 */
    {0x5AEB,0xC6E6}, /* ��: GB2312 Code: 0xE6C6 ==> Row:70 Col:38 */
    {0x5AF1,0xCDE6}, /* ��: GB2312 Code: 0xE6CD ==> Row:70 Col:45 */
    {0x5B09,0xD2E6}, /* ��: GB2312 Code: 0xE6D2 ==> Row:70 Col:50 */
    {0x5B16,0xD4E6}, /* ��: GB2312 Code: 0xE6D4 ==> Row:70 Col:52 */
    {0x5B17,0xD3E6}, /* ��: GB2312 Code: 0xE6D3 ==> Row:70 Col:51 */
    {0x5B32,0xD5E6}, /* ��: GB2312 Code: 0xE6D5 ==> Row:70 Col:53 */
    {0x5B34,0xF8D9}, /* ��: GB2312 Code: 0xD9F8 ==> Row:57 Col:88 */
    {0x5B37,0xD6E6}, /* ��: GB2312 Code: 0xE6D6 ==> Row:70 Col:54 */
    {0x5B40,0xD7E6}, /* ��: GB2312 Code: 0xE6D7 ==> Row:70 Col:55 */
    {0x5B50,0xD3D7}, /* ��: GB2312 Code: 0xD7D3 ==> Row:55 Col:51 */
    {0x5B51,0xDDE6}, /* ��: GB2312 Code: 0xE6DD ==> Row:70 Col:61 */
    {0x5B53,0xDEE6}, /* ��: GB2312 Code: 0xE6DE ==> Row:70 Col:62 */
    {0x5B54,0xD7BF}, /* ��: GB2312 Code: 0xBFD7 ==> Row:31 Col:55 */
    {0x5B55,0xD0D4}, /* ��: GB2312 Code: 0xD4D0 ==> Row:52 Col:48 */
    {0x5B57,0xD6D7}, /* ��: GB2312 Code: 0xD7D6 ==> Row:55 Col:54 */
    {0x5B58,0xE6B4}, /* ��: GB2312 Code: 0xB4E6 ==> Row:20 Col:70 */
    {0x5B59,0xEFCB}, /* ��: GB2312 Code: 0xCBEF ==> Row:43 Col:79 */
    {0x5B5A,0xDAE6}, /* ��: GB2312 Code: 0xE6DA ==> Row:70 Col:58 */
    {0x5B5B,0xC3D8}, /* ��: GB2312 Code: 0xD8C3 ==> Row:56 Col:35 */
    {0x5B5C,0xCED7}, /* ��: GB2312 Code: 0xD7CE ==> Row:55 Col:46 */
    {0x5B5D,0xA2D0}, /* Т: GB2312 Code: 0xD0A2 ==> Row:48 Col:02 */
    {0x5B5F,0xCFC3}, /* ��: GB2312 Code: 0xC3CF ==> Row:35 Col:47 */
    {0x5B62,0xDFE6}, /* ��: GB2312 Code: 0xE6DF ==> Row:70 Col:63 */
    {0x5B63,0xBEBC}, /* ��: GB2312 Code: 0xBCBE ==> Row:28 Col:30 */
    {0x5B64,0xC2B9}, /* ��: GB2312 Code: 0xB9C2 ==> Row:25 Col:34 */
    {0x5B65,0xDBE6}, /* ��: GB2312 Code: 0xE6DB ==> Row:70 Col:59 */
    {0x5B66,0xA7D1}, /* ѧ: GB2312 Code: 0xD1A7 ==> Row:49 Col:07 */
    {0x5B69,0xA2BA}, /* ��: GB2312 Code: 0xBAA2 ==> Row:26 Col:02 */
    {0x5B6A,0xCFC2}, /* ��: GB2312 Code: 0xC2CF ==> Row:34 Col:47 */
    {0x5B6C,0xABD8}, /* ث: GB2312 Code: 0xD8AB ==> Row:56 Col:11 */
    {0x5B70,0xEBCA}, /* ��: GB2312 Code: 0xCAEB ==> Row:42 Col:75 */
    {0x5B71,0xEEE5}, /* ��: GB2312 Code: 0xE5EE ==> Row:69 Col:78 */
    {0x5B73,0xDCE6}, /* ��: GB2312 Code: 0xE6DC ==> Row:70 Col:60 */
    {0x5B75,0xF5B7}, /* ��: GB2312 Code: 0xB7F5 ==> Row:23 Col:85 */
    {0x5B7A,0xE6C8}, /* ��: GB2312 Code: 0xC8E6 ==> Row:40 Col:70 */
    {0x5B7D,0xF5C4}, /* ��: GB2312 Code: 0xC4F5 ==> Row:36 Col:85 */
    {0x5B80,0xB2E5}, /* �: GB2312 Code: 0xE5B2 ==> Row:69 Col:18 */
    {0x5B81,0xFEC4}, /* ��: GB2312 Code: 0xC4FE ==> Row:36 Col:94 */
    {0x5B83,0xFCCB}, /* ��: GB2312 Code: 0xCBFC ==> Row:43 Col:92 */
    {0x5B84,0xB3E5}, /* �: GB2312 Code: 0xE5B3 ==> Row:69 Col:19 */
    {0x5B85,0xACD5}, /* լ: GB2312 Code: 0xD5AC ==> Row:53 Col:12 */
    {0x5B87,0xEED3}, /* ��: GB2312 Code: 0xD3EE ==> Row:51 Col:78 */
    {0x5B88,0xD8CA}, /* ��: GB2312 Code: 0xCAD8 ==> Row:42 Col:56 */
    {0x5B89,0xB2B0}, /* ��: GB2312 Code: 0xB0B2 ==> Row:16 Col:18 */
    {0x5B8B,0xCECB}, /* ��: GB2312 Code: 0xCBCE ==> Row:43 Col:46 */
    {0x5B8C,0xEACD}, /* ��: GB2312 Code: 0xCDEA ==> Row:45 Col:74 */
    {0x5B8F,0xEABA}, /* ��: GB2312 Code: 0xBAEA ==> Row:26 Col:74 */
    {0x5B93,0xB5E5}, /* �: GB2312 Code: 0xE5B5 ==> Row:69 Col:21 */
    {0x5B95,0xB4E5}, /* �: GB2312 Code: 0xE5B4 ==> Row:69 Col:20 */
    {0x5B97,0xDAD7}, /* ��: GB2312 Code: 0xD7DA ==> Row:55 Col:58 */
    {0x5B98,0xD9B9}, /* ��: GB2312 Code: 0xB9D9 ==> Row:25 Col:57 */
    {0x5B99,0xE6D6}, /* ��: GB2312 Code: 0xD6E6 ==> Row:54 Col:70 */
    {0x5B9A,0xA8B6}, /* ��: GB2312 Code: 0xB6A8 ==> Row:22 Col:08 */
    {0x5B9B,0xF0CD}, /* ��: GB2312 Code: 0xCDF0 ==> Row:45 Col:80 */
    {0x5B9C,0xCBD2}, /* ��: GB2312 Code: 0xD2CB ==> Row:50 Col:43 */
    {0x5B9D,0xA6B1}, /* ��: GB2312 Code: 0xB1A6 ==> Row:17 Col:06 */
    {0x5B9E,0xB5CA}, /* ʵ: GB2312 Code: 0xCAB5 ==> Row:42 Col:21 */
    {0x5BA0,0xE8B3}, /* ��: GB2312 Code: 0xB3E8 ==> Row:19 Col:72 */
    {0x5BA1,0xF3C9}, /* ��: GB2312 Code: 0xC9F3 ==> Row:41 Col:83 */
    {0x5BA2,0xCDBF}, /* ��: GB2312 Code: 0xBFCD ==> Row:31 Col:45 */
    {0x5BA3,0xFBD0}, /* ��: GB2312 Code: 0xD0FB ==> Row:48 Col:91 */
    {0x5BA4,0xD2CA}, /* ��: GB2312 Code: 0xCAD2 ==> Row:42 Col:50 */
    {0x5BA5,0xB6E5}, /* �: GB2312 Code: 0xE5B6 ==> Row:69 Col:22 */
    {0x5BA6,0xC2BB}, /* ��: GB2312 Code: 0xBBC2 ==> Row:27 Col:34 */
    {0x5BAA,0xDCCF}, /* ��: GB2312 Code: 0xCFDC ==> Row:47 Col:60 */
    {0x5BAB,0xACB9}, /* ��: GB2312 Code: 0xB9AC ==> Row:25 Col:12 */
    {0x5BB0,0xD7D4}, /* ��: GB2312 Code: 0xD4D7 ==> Row:52 Col:55 */
    {0x5BB3,0xA6BA}, /* ��: GB2312 Code: 0xBAA6 ==> Row:26 Col:06 */
    {0x5BB4,0xE7D1}, /* ��: GB2312 Code: 0xD1E7 ==> Row:49 Col:71 */
    {0x5BB5,0xFCCF}, /* ��: GB2312 Code: 0xCFFC ==> Row:47 Col:92 */
    {0x5BB6,0xD2BC}, /* ��: GB2312 Code: 0xBCD2 ==> Row:28 Col:50 */
    {0x5BB8,0xB7E5}, /* �: GB2312 Code: 0xE5B7 ==> Row:69 Col:23 */
    {0x5BB9,0xDDC8}, /* ��: GB2312 Code: 0xC8DD ==> Row:40 Col:61 */
    {0x5BBD,0xEDBF}, /* ��: GB2312 Code: 0xBFED ==> Row:31 Col:77 */
    {0x5BBE,0xF6B1}, /* ��: GB2312 Code: 0xB1F6 ==> Row:17 Col:86 */
    {0x5BBF,0xDECB}, /* ��: GB2312 Code: 0xCBDE ==> Row:43 Col:62 */
    {0x5BC2,0xC5BC}, /* ��: GB2312 Code: 0xBCC5 ==> Row:28 Col:37 */
    {0x5BC4,0xC4BC}, /* ��: GB2312 Code: 0xBCC4 ==> Row:28 Col:36 */
    {0x5BC5,0xFAD2}, /* ��: GB2312 Code: 0xD2FA ==> Row:50 Col:90 */
    {0x5BC6,0xDCC3}, /* ��: GB2312 Code: 0xC3DC ==> Row:35 Col:60 */
    {0x5BC7,0xDCBF}, /* ��: GB2312 Code: 0xBFDC ==> Row:31 Col:60 */
    {0x5BCC,0xBBB8}, /* ��: GB2312 Code: 0xB8BB ==> Row:24 Col:27 */
    {0x5BD0,0xC2C3}, /* ��: GB2312 Code: 0xC3C2 ==> Row:35 Col:34 */
    {0x5BD2,0xAEBA}, /* ��: GB2312 Code: 0xBAAE ==> Row:26 Col:14 */
    {0x5BD3,0xA2D4}, /* Ԣ: GB2312 Code: 0xD4A2 ==> Row:52 Col:02 */
    {0x5BDD,0xDEC7}, /* ��: GB2312 Code: 0xC7DE ==> Row:39 Col:62 */
    {0x5BDE,0xAFC4}, /* į: GB2312 Code: 0xC4AF ==> Row:36 Col:15 */
    {0x5BDF,0xECB2}, /* ��: GB2312 Code: 0xB2EC ==> Row:18 Col:76 */
    {0x5BE1,0xD1B9}, /* ��: GB2312 Code: 0xB9D1 ==> Row:25 Col:49 */
    {0x5BE4,0xBBE5}, /* �: GB2312 Code: 0xE5BB ==> Row:69 Col:27 */
    {0x5BE5,0xC8C1}, /* ��: GB2312 Code: 0xC1C8 ==> Row:33 Col:40 */
    {0x5BE8,0xAFD5}, /* կ: GB2312 Code: 0xD5AF ==> Row:53 Col:15 */
    {0x5BEE,0xBCE5}, /* �: GB2312 Code: 0xE5BC ==> Row:69 Col:28 */
    {0x5BF0,0xBEE5}, /* �: GB2312 Code: 0xE5BE ==> Row:69 Col:30 */
    {0x5BF8,0xE7B4}, /* ��: GB2312 Code: 0xB4E7 ==> Row:20 Col:71 */
    {0x5BF9,0xD4B6}, /* ��: GB2312 Code: 0xB6D4 ==> Row:22 Col:52 */
    {0x5BFA,0xC2CB}, /* ��: GB2312 Code: 0xCBC2 ==> Row:43 Col:34 */
    {0x5BFB,0xB0D1}, /* Ѱ: GB2312 Code: 0xD1B0 ==> Row:49 Col:16 */
    {0x5BFC,0xBCB5}, /* ��: GB2312 Code: 0xB5BC ==> Row:21 Col:28 */
    {0x5BFF,0xD9CA}, /* ��: GB2312 Code: 0xCAD9 ==> Row:42 Col:57 */
    {0x5C01,0xE2B7}, /* ��: GB2312 Code: 0xB7E2 ==> Row:23 Col:66 */
    {0x5C04,0xE4C9}, /* ��: GB2312 Code: 0xC9E4 ==> Row:41 Col:68 */
    {0x5C06,0xABBD}, /* ��: GB2312 Code: 0xBDAB ==> Row:29 Col:11 */
    {0x5C09,0xBECE}, /* ξ: GB2312 Code: 0xCEBE ==> Row:46 Col:30 */
    {0x5C0A,0xF0D7}, /* ��: GB2312 Code: 0xD7F0 ==> Row:55 Col:80 */
    {0x5C0F,0xA1D0}, /* С: GB2312 Code: 0xD0A1 ==> Row:48 Col:01 */
    {0x5C11,0xD9C9}, /* ��: GB2312 Code: 0xC9D9 ==> Row:41 Col:57 */
    {0x5C14,0xFBB6}, /* ��: GB2312 Code: 0xB6FB ==> Row:22 Col:91 */
    {0x5C15,0xD8E6}, /* ��: GB2312 Code: 0xE6D8 ==> Row:70 Col:56 */
    {0x5C16,0xE2BC}, /* ��: GB2312 Code: 0xBCE2 ==> Row:28 Col:66 */
    {0x5C18,0xBEB3}, /* ��: GB2312 Code: 0xB3BE ==> Row:19 Col:30 */
    {0x5C1A,0xD0C9}, /* ��: GB2312 Code: 0xC9D0 ==> Row:41 Col:48 */
    {0x5C1C,0xD9E6}, /* ��: GB2312 Code: 0xE6D9 ==> Row:70 Col:57 */
    {0x5C1D,0xA2B3}, /* ��: GB2312 Code: 0xB3A2 ==> Row:19 Col:02 */
    {0x5C22,0xCCDE}, /* ��: GB2312 Code: 0xDECC ==> Row:62 Col:44 */
    {0x5C24,0xC8D3}, /* ��: GB2312 Code: 0xD3C8 ==> Row:51 Col:40 */
    {0x5C25,0xCDDE}, /* ��: GB2312 Code: 0xDECD ==> Row:62 Col:45 */
    {0x5C27,0xA2D2}, /* Ң: GB2312 Code: 0xD2A2 ==> Row:50 Col:02 */
    {0x5C2C,0xCEDE}, /* ��: GB2312 Code: 0xDECE ==> Row:62 Col:46 */
    {0x5C31,0xCDBE}, /* ��: GB2312 Code: 0xBECD ==> Row:30 Col:45 */
    {0x5C34,0xCFDE}, /* ��: GB2312 Code: 0xDECF ==> Row:62 Col:47 */
    {0x5C38,0xACCA}, /* ʬ: GB2312 Code: 0xCAAC ==> Row:42 Col:12 */
    {0x5C39,0xFCD2}, /* ��: GB2312 Code: 0xD2FC ==> Row:50 Col:92 */
    {0x5C3A,0xDFB3}, /* ��: GB2312 Code: 0xB3DF ==> Row:19 Col:63 */
    {0x5C3B,0xEAE5}, /* ��: GB2312 Code: 0xE5EA ==> Row:69 Col:74 */
    {0x5C3C,0xE1C4}, /* ��: GB2312 Code: 0xC4E1 ==> Row:36 Col:65 */
    {0x5C3D,0xA1BE}, /* ��: GB2312 Code: 0xBEA1 ==> Row:30 Col:01 */
    {0x5C3E,0xB2CE}, /* β: GB2312 Code: 0xCEB2 ==> Row:46 Col:18 */
    {0x5C3F,0xF2C4}, /* ��: GB2312 Code: 0xC4F2 ==> Row:36 Col:82 */
    {0x5C40,0xD6BE}, /* ��: GB2312 Code: 0xBED6 ==> Row:30 Col:54 */
    {0x5C41,0xA8C6}, /* ƨ: GB2312 Code: 0xC6A8 ==> Row:38 Col:08 */
    {0x5C42,0xE3B2}, /* ��: GB2312 Code: 0xB2E3 ==> Row:18 Col:67 */
    {0x5C45,0xD3BE}, /* ��: GB2312 Code: 0xBED3 ==> Row:30 Col:51 */
    {0x5C48,0xFCC7}, /* ��: GB2312 Code: 0xC7FC ==> Row:39 Col:92 */
    {0x5C49,0xEBCC}, /* ��: GB2312 Code: 0xCCEB ==> Row:44 Col:75 */
    {0x5C4A,0xECBD}, /* ��: GB2312 Code: 0xBDEC ==> Row:29 Col:76 */
    {0x5C4B,0xDDCE}, /* ��: GB2312 Code: 0xCEDD ==> Row:46 Col:61 */
    {0x5C4E,0xBACA}, /* ʺ: GB2312 Code: 0xCABA ==> Row:42 Col:26 */
    {0x5C4F,0xC1C6}, /* ��: GB2312 Code: 0xC6C1 ==> Row:38 Col:33 */
    {0x5C50,0xECE5}, /* ��: GB2312 Code: 0xE5EC ==> Row:69 Col:76 */
    {0x5C51,0xBCD0}, /* м: GB2312 Code: 0xD0BC ==> Row:48 Col:28 */
    {0x5C55,0xB9D5}, /* չ: GB2312 Code: 0xD5B9 ==> Row:53 Col:25 */
    {0x5C59,0xEDE5}, /* ��: GB2312 Code: 0xE5ED ==> Row:69 Col:77 */
    {0x5C5E,0xF4CA}, /* ��: GB2312 Code: 0xCAF4 ==> Row:42 Col:84 */
    {0x5C60,0xC0CD}, /* ��: GB2312 Code: 0xCDC0 ==> Row:45 Col:32 */
    {0x5C61,0xC5C2}, /* ��: GB2312 Code: 0xC2C5 ==> Row:34 Col:37 */
    {0x5C63,0xEFE5}, /* ��: GB2312 Code: 0xE5EF ==> Row:69 Col:79 */
    {0x5C65,0xC4C2}, /* ��: GB2312 Code: 0xC2C4 ==> Row:34 Col:36 */
    {0x5C66,0xF0E5}, /* ��: GB2312 Code: 0xE5F0 ==> Row:69 Col:80 */
    {0x5C6E,0xF8E5}, /* ��: GB2312 Code: 0xE5F8 ==> Row:69 Col:88 */
    {0x5C6F,0xCDCD}, /* ��: GB2312 Code: 0xCDCD ==> Row:45 Col:45 */
    {0x5C71,0xBDC9}, /* ɽ: GB2312 Code: 0xC9BD ==> Row:41 Col:29 */
    {0x5C79,0xD9D2}, /* ��: GB2312 Code: 0xD2D9 ==> Row:50 Col:57 */
    {0x5C7A,0xA8E1}, /* �: GB2312 Code: 0xE1A8 ==> Row:65 Col:08 */
    {0x5C7F,0xECD3}, /* ��: GB2312 Code: 0xD3EC ==> Row:51 Col:76 */
    {0x5C81,0xEACB}, /* ��: GB2312 Code: 0xCBEA ==> Row:43 Col:74 */
    {0x5C82,0xF1C6}, /* ��: GB2312 Code: 0xC6F1 ==> Row:38 Col:81 */
    {0x5C88,0xACE1}, /* �: GB2312 Code: 0xE1AC ==> Row:65 Col:12 */
    {0x5C8C,0xA7E1}, /* �: GB2312 Code: 0xE1A7 ==> Row:65 Col:07 */
    {0x5C8D,0xA9E1}, /* �: GB2312 Code: 0xE1A9 ==> Row:65 Col:09 */
    {0x5C90,0xAAE1}, /* �: GB2312 Code: 0xE1AA ==> Row:65 Col:10 */
    {0x5C91,0xAFE1}, /* �: GB2312 Code: 0xE1AF ==> Row:65 Col:15 */
    {0x5C94,0xEDB2}, /* ��: GB2312 Code: 0xB2ED ==> Row:18 Col:77 */
    {0x5C96,0xABE1}, /* �: GB2312 Code: 0xE1AB ==> Row:65 Col:11 */
    {0x5C97,0xDAB8}, /* ��: GB2312 Code: 0xB8DA ==> Row:24 Col:58 */
    {0x5C98,0xADE1}, /* �: GB2312 Code: 0xE1AD ==> Row:65 Col:13 */
    {0x5C99,0xAEE1}, /* �: GB2312 Code: 0xE1AE ==> Row:65 Col:14 */
    {0x5C9A,0xB0E1}, /* �: GB2312 Code: 0xE1B0 ==> Row:65 Col:16 */
    {0x5C9B,0xBAB5}, /* ��: GB2312 Code: 0xB5BA ==> Row:21 Col:26 */
    {0x5C9C,0xB1E1}, /* �: GB2312 Code: 0xE1B1 ==> Row:65 Col:17 */
    {0x5CA2,0xB3E1}, /* �: GB2312 Code: 0xE1B3 ==> Row:65 Col:19 */
    {0x5CA3,0xB8E1}, /* �: GB2312 Code: 0xE1B8 ==> Row:65 Col:24 */
    {0x5CA9,0xD2D1}, /* ��: GB2312 Code: 0xD1D2 ==> Row:49 Col:50 */
    {0x5CAB,0xB6E1}, /* �: GB2312 Code: 0xE1B6 ==> Row:65 Col:22 */
    {0x5CAC,0xB5E1}, /* �: GB2312 Code: 0xE1B5 ==> Row:65 Col:21 */
    {0x5CAD,0xEBC1}, /* ��: GB2312 Code: 0xC1EB ==> Row:33 Col:75 */
    {0x5CB1,0xB7E1}, /* �: GB2312 Code: 0xE1B7 ==> Row:65 Col:23 */
    {0x5CB3,0xC0D4}, /* ��: GB2312 Code: 0xD4C0 ==> Row:52 Col:32 */
    {0x5CB5,0xB2E1}, /* �: GB2312 Code: 0xE1B2 ==> Row:65 Col:18 */
    {0x5CB7,0xBAE1}, /* �: GB2312 Code: 0xE1BA ==> Row:65 Col:26 */
    {0x5CB8,0xB6B0}, /* ��: GB2312 Code: 0xB0B6 ==> Row:16 Col:22 */
    {0x5CBD,0xB4E1}, /* �: GB2312 Code: 0xE1B4 ==> Row:65 Col:20 */
    {0x5CBF,0xF9BF}, /* ��: GB2312 Code: 0xBFF9 ==> Row:31 Col:89 */
    {0x5CC1,0xB9E1}, /* �: GB2312 Code: 0xE1B9 ==> Row:65 Col:25 */
    {0x5CC4,0xBBE1}, /* �: GB2312 Code: 0xE1BB ==> Row:65 Col:27 */
    {0x5CCB,0xBEE1}, /* �: GB2312 Code: 0xE1BE ==> Row:65 Col:30 */
    {0x5CD2,0xBCE1}, /* �: GB2312 Code: 0xE1BC ==> Row:65 Col:28 */
    {0x5CD9,0xC5D6}, /* ��: GB2312 Code: 0xD6C5 ==> Row:54 Col:37 */
    {0x5CE1,0xBFCF}, /* Ͽ: GB2312 Code: 0xCFBF ==> Row:47 Col:31 */
    {0x5CE4,0xBDE1}, /* �: GB2312 Code: 0xE1BD ==> Row:65 Col:29 */
    {0x5CE5,0xBFE1}, /* �: GB2312 Code: 0xE1BF ==> Row:65 Col:31 */
    {0x5CE6,0xCDC2}, /* ��: GB2312 Code: 0xC2CD ==> Row:34 Col:45 */
    {0x5CE8,0xEBB6}, /* ��: GB2312 Code: 0xB6EB ==> Row:22 Col:75 */
    {0x5CEA,0xF8D3}, /* ��: GB2312 Code: 0xD3F8 ==> Row:51 Col:88 */
    {0x5CED,0xCDC7}, /* ��: GB2312 Code: 0xC7CD ==> Row:39 Col:45 */
    {0x5CF0,0xE5B7}, /* ��: GB2312 Code: 0xB7E5 ==> Row:23 Col:69 */
    {0x5CFB,0xFEBE}, /* ��: GB2312 Code: 0xBEFE ==> Row:30 Col:94 */
    {0x5D02,0xC0E1}, /* ��: GB2312 Code: 0xE1C0 ==> Row:65 Col:32 */
    {0x5D03,0xC1E1}, /* ��: GB2312 Code: 0xE1C1 ==> Row:65 Col:33 */
    {0x5D06,0xC7E1}, /* ��: GB2312 Code: 0xE1C7 ==> Row:65 Col:39 */
    {0x5D07,0xE7B3}, /* ��: GB2312 Code: 0xB3E7 ==> Row:19 Col:71 */
    {0x5D0E,0xE9C6}, /* ��: GB2312 Code: 0xC6E9 ==> Row:38 Col:73 */
    {0x5D14,0xDEB4}, /* ��: GB2312 Code: 0xB4DE ==> Row:20 Col:62 */
    {0x5D16,0xC2D1}, /* ��: GB2312 Code: 0xD1C2 ==> Row:49 Col:34 */
    {0x5D1B,0xC8E1}, /* ��: GB2312 Code: 0xE1C8 ==> Row:65 Col:40 */
    {0x5D1E,0xC6E1}, /* ��: GB2312 Code: 0xE1C6 ==> Row:65 Col:38 */
    {0x5D24,0xC5E1}, /* ��: GB2312 Code: 0xE1C5 ==> Row:65 Col:37 */
    {0x5D26,0xC3E1}, /* ��: GB2312 Code: 0xE1C3 ==> Row:65 Col:35 */
    {0x5D27,0xC2E1}, /* ��: GB2312 Code: 0xE1C2 ==> Row:65 Col:34 */
    {0x5D29,0xC0B1}, /* ��: GB2312 Code: 0xB1C0 ==> Row:17 Col:32 */
    {0x5D2D,0xB8D5}, /* ո: GB2312 Code: 0xD5B8 ==> Row:53 Col:24 */
    {0x5D2E,0xC4E1}, /* ��: GB2312 Code: 0xE1C4 ==> Row:65 Col:36 */
    {0x5D34,0xCBE1}, /* ��: GB2312 Code: 0xE1CB ==> Row:65 Col:43 */
    {0x5D3D,0xCCE1}, /* ��: GB2312 Code: 0xE1CC ==> Row:65 Col:44 */
    {0x5D3E,0xCAE1}, /* ��: GB2312 Code: 0xE1CA ==> Row:65 Col:42 */
    {0x5D47,0xFAEF}, /* ��: GB2312 Code: 0xEFFA ==> Row:79 Col:90 */
    {0x5D4A,0xD3E1}, /* ��: GB2312 Code: 0xE1D3 ==> Row:65 Col:51 */
    {0x5D4B,0xD2E1}, /* ��: GB2312 Code: 0xE1D2 ==> Row:65 Col:50 */
    {0x5D4C,0xB6C7}, /* Ƕ: GB2312 Code: 0xC7B6 ==> Row:39 Col:22 */
    {0x5D58,0xC9E1}, /* ��: GB2312 Code: 0xE1C9 ==> Row:65 Col:41 */
    {0x5D5B,0xCEE1}, /* ��: GB2312 Code: 0xE1CE ==> Row:65 Col:46 */
    {0x5D5D,0xD0E1}, /* ��: GB2312 Code: 0xE1D0 ==> Row:65 Col:48 */
    {0x5D69,0xD4E1}, /* ��: GB2312 Code: 0xE1D4 ==> Row:65 Col:52 */
    {0x5D6B,0xD1E1}, /* ��: GB2312 Code: 0xE1D1 ==> Row:65 Col:49 */
    {0x5D6C,0xCDE1}, /* ��: GB2312 Code: 0xE1CD ==> Row:65 Col:45 */
    {0x5D6F,0xCFE1}, /* ��: GB2312 Code: 0xE1CF ==> Row:65 Col:47 */
    {0x5D74,0xD5E1}, /* ��: GB2312 Code: 0xE1D5 ==> Row:65 Col:53 */
    {0x5D82,0xD6E1}, /* ��: GB2312 Code: 0xE1D6 ==> Row:65 Col:54 */
    {0x5D99,0xD7E1}, /* ��: GB2312 Code: 0xE1D7 ==> Row:65 Col:55 */
    {0x5D9D,0xD8E1}, /* ��: GB2312 Code: 0xE1D8 ==> Row:65 Col:56 */
    {0x5DB7,0xDAE1}, /* ��: GB2312 Code: 0xE1DA ==> Row:65 Col:58 */
    {0x5DC5,0xDBE1}, /* ��: GB2312 Code: 0xE1DB ==> Row:65 Col:59 */
    {0x5DCD,0xA1CE}, /* Ρ: GB2312 Code: 0xCEA1 ==> Row:46 Col:01 */
    {0x5DDB,0xDDE7}, /* ��: GB2312 Code: 0xE7DD ==> Row:71 Col:61 */
    {0x5DDD,0xA8B4}, /* ��: GB2312 Code: 0xB4A8 ==> Row:20 Col:08 */
    {0x5DDE,0xDDD6}, /* ��: GB2312 Code: 0xD6DD ==> Row:54 Col:61 */
    {0x5DE1,0xB2D1}, /* Ѳ: GB2312 Code: 0xD1B2 ==> Row:49 Col:18 */
    {0x5DE2,0xB2B3}, /* ��: GB2312 Code: 0xB3B2 ==> Row:19 Col:18 */
    {0x5DE5,0xA4B9}, /* ��: GB2312 Code: 0xB9A4 ==> Row:25 Col:04 */
    {0x5DE6,0xF3D7}, /* ��: GB2312 Code: 0xD7F3 ==> Row:55 Col:83 */
    {0x5DE7,0xC9C7}, /* ��: GB2312 Code: 0xC7C9 ==> Row:39 Col:41 */
    {0x5DE8,0xDEBE}, /* ��: GB2312 Code: 0xBEDE ==> Row:30 Col:62 */
    {0x5DE9,0xAEB9}, /* ��: GB2312 Code: 0xB9AE ==> Row:25 Col:14 */
    {0x5DEB,0xD7CE}, /* ��: GB2312 Code: 0xCED7 ==> Row:46 Col:55 */
    {0x5DEE,0xEEB2}, /* ��: GB2312 Code: 0xB2EE ==> Row:18 Col:78 */
    {0x5DEF,0xCFDB}, /* ��: GB2312 Code: 0xDBCF ==> Row:59 Col:47 */
    {0x5DF1,0xBABC}, /* ��: GB2312 Code: 0xBCBA ==> Row:28 Col:26 */
    {0x5DF2,0xD1D2}, /* ��: GB2312 Code: 0xD2D1 ==> Row:50 Col:49 */
    {0x5DF3,0xC8CB}, /* ��: GB2312 Code: 0xCBC8 ==> Row:43 Col:40 */
    {0x5DF4,0xCDB0}, /* ��: GB2312 Code: 0xB0CD ==> Row:16 Col:45 */
    {0x5DF7,0xEFCF}, /* ��: GB2312 Code: 0xCFEF ==> Row:47 Col:79 */
    {0x5DFD,0xE3D9}, /* ��: GB2312 Code: 0xD9E3 ==> Row:57 Col:67 */
    {0x5DFE,0xEDBD}, /* ��: GB2312 Code: 0xBDED ==> Row:29 Col:77 */
    {0x5E01,0xD2B1}, /* ��: GB2312 Code: 0xB1D2 ==> Row:17 Col:50 */
    {0x5E02,0xD0CA}, /* ��: GB2312 Code: 0xCAD0 ==> Row:42 Col:48 */
    {0x5E03,0xBCB2}, /* ��: GB2312 Code: 0xB2BC ==> Row:18 Col:28 */
    {0x5E05,0xA7CB}, /* ˧: GB2312 Code: 0xCBA7 ==> Row:43 Col:07 */
    {0x5E06,0xABB7}, /* ��: GB2312 Code: 0xB7AB ==> Row:23 Col:11 */
    {0x5E08,0xA6CA}, /* ʦ: GB2312 Code: 0xCAA6 ==> Row:42 Col:06 */
    {0x5E0C,0xA3CF}, /* ϣ: GB2312 Code: 0xCFA3 ==> Row:47 Col:03 */
    {0x5E0F,0xF8E0}, /* ��: GB2312 Code: 0xE0F8 ==> Row:64 Col:88 */
    {0x5E10,0xCAD5}, /* ��: GB2312 Code: 0xD5CA ==> Row:53 Col:42 */
    {0x5E11,0xFBE0}, /* ��: GB2312 Code: 0xE0FB ==> Row:64 Col:91 */
    {0x5E14,0xFAE0}, /* ��: GB2312 Code: 0xE0FA ==> Row:64 Col:90 */
    {0x5E15,0xC1C5}, /* ��: GB2312 Code: 0xC5C1 ==> Row:37 Col:33 */
    {0x5E16,0xFBCC}, /* ��: GB2312 Code: 0xCCFB ==> Row:44 Col:91 */
    {0x5E18,0xB1C1}, /* ��: GB2312 Code: 0xC1B1 ==> Row:33 Col:17 */
    {0x5E19,0xF9E0}, /* ��: GB2312 Code: 0xE0F9 ==> Row:64 Col:89 */
    {0x5E1A,0xE3D6}, /* ��: GB2312 Code: 0xD6E3 ==> Row:54 Col:67 */
    {0x5E1B,0xAFB2}, /* ��: GB2312 Code: 0xB2AF ==> Row:18 Col:15 */
    {0x5E1C,0xC4D6}, /* ��: GB2312 Code: 0xD6C4 ==> Row:54 Col:36 */
    {0x5E1D,0xDBB5}, /* ��: GB2312 Code: 0xB5DB ==> Row:21 Col:59 */
    {0x5E26,0xF8B4}, /* ��: GB2312 Code: 0xB4F8 ==> Row:20 Col:88 */
    {0x5E27,0xA1D6}, /* ֡: GB2312 Code: 0xD6A1 ==> Row:54 Col:01 */
    {0x5E2D,0xAFCF}, /* ϯ: GB2312 Code: 0xCFAF ==> Row:47 Col:15 */
    {0x5E2E,0xEFB0}, /* ��: GB2312 Code: 0xB0EF ==> Row:16 Col:79 */
    {0x5E31,0xFCE0}, /* ��: GB2312 Code: 0xE0FC ==> Row:64 Col:92 */
    {0x5E37,0xA1E1}, /* �: GB2312 Code: 0xE1A1 ==> Row:65 Col:01 */
    {0x5E38,0xA3B3}, /* ��: GB2312 Code: 0xB3A3 ==> Row:19 Col:03 */
    {0x5E3B,0xFDE0}, /* ��: GB2312 Code: 0xE0FD ==> Row:64 Col:93 */
    {0x5E3C,0xFEE0}, /* ��: GB2312 Code: 0xE0FE ==> Row:64 Col:94 */
    {0x5E3D,0xB1C3}, /* ñ: GB2312 Code: 0xC3B1 ==> Row:35 Col:17 */
    {0x5E42,0xDDC3}, /* ��: GB2312 Code: 0xC3DD ==> Row:35 Col:61 */
    {0x5E44,0xA2E1}, /* �: GB2312 Code: 0xE1A2 ==> Row:65 Col:02 */
    {0x5E45,0xF9B7}, /* ��: GB2312 Code: 0xB7F9 ==> Row:23 Col:89 */
    {0x5E4C,0xCFBB}, /* ��: GB2312 Code: 0xBBCF ==> Row:27 Col:47 */
    {0x5E54,0xA3E1}, /* �: GB2312 Code: 0xE1A3 ==> Row:65 Col:03 */
    {0x5E55,0xBBC4}, /* Ļ: GB2312 Code: 0xC4BB ==> Row:36 Col:27 */
    {0x5E5B,0xA4E1}, /* �: GB2312 Code: 0xE1A4 ==> Row:65 Col:04 */
    {0x5E5E,0xA5E1}, /* �: GB2312 Code: 0xE1A5 ==> Row:65 Col:05 */
    {0x5E61,0xA6E1}, /* �: GB2312 Code: 0xE1A6 ==> Row:65 Col:06 */
    {0x5E62,0xB1B4}, /* ��: GB2312 Code: 0xB4B1 ==> Row:20 Col:17 */
    {0x5E72,0xC9B8}, /* ��: GB2312 Code: 0xB8C9 ==> Row:24 Col:41 */
    {0x5E73,0xBDC6}, /* ƽ: GB2312 Code: 0xC6BD ==> Row:38 Col:29 */
    {0x5E74,0xEAC4}, /* ��: GB2312 Code: 0xC4EA ==> Row:36 Col:74 */
    {0x5E76,0xA2B2}, /* ��: GB2312 Code: 0xB2A2 ==> Row:18 Col:02 */
    {0x5E78,0xD2D0}, /* ��: GB2312 Code: 0xD0D2 ==> Row:48 Col:50 */
    {0x5E7A,0xDBE7}, /* ��: GB2312 Code: 0xE7DB ==> Row:71 Col:59 */
    {0x5E7B,0xC3BB}, /* ��: GB2312 Code: 0xBBC3 ==> Row:27 Col:35 */
    {0x5E7C,0xD7D3}, /* ��: GB2312 Code: 0xD3D7 ==> Row:51 Col:55 */
    {0x5E7D,0xC4D3}, /* ��: GB2312 Code: 0xD3C4 ==> Row:51 Col:36 */
    {0x5E7F,0xE3B9}, /* ��: GB2312 Code: 0xB9E3 ==> Row:25 Col:67 */
    {0x5E80,0xCFE2}, /* ��: GB2312 Code: 0xE2CF ==> Row:66 Col:47 */
    {0x5E84,0xAFD7}, /* ׯ: GB2312 Code: 0xD7AF ==> Row:55 Col:15 */
    {0x5E86,0xECC7}, /* ��: GB2312 Code: 0xC7EC ==> Row:39 Col:76 */
    {0x5E87,0xD3B1}, /* ��: GB2312 Code: 0xB1D3 ==> Row:17 Col:51 */
    {0x5E8A,0xB2B4}, /* ��: GB2312 Code: 0xB4B2 ==> Row:20 Col:18 */
    {0x5E8B,0xD1E2}, /* ��: GB2312 Code: 0xE2D1 ==> Row:66 Col:49 */
    {0x5E8F,0xF2D0}, /* ��: GB2312 Code: 0xD0F2 ==> Row:48 Col:82 */
    {0x5E90,0xAEC2}, /* ®: GB2312 Code: 0xC2AE ==> Row:34 Col:14 */
    {0x5E91,0xD0E2}, /* ��: GB2312 Code: 0xE2D0 ==> Row:66 Col:48 */
    {0x5E93,0xE2BF}, /* ��: GB2312 Code: 0xBFE2 ==> Row:31 Col:66 */
    {0x5E94,0xA6D3}, /* Ӧ: GB2312 Code: 0xD3A6 ==> Row:51 Col:06 */
    {0x5E95,0xD7B5}, /* ��: GB2312 Code: 0xB5D7 ==> Row:21 Col:55 */
    {0x5E96,0xD2E2}, /* ��: GB2312 Code: 0xE2D2 ==> Row:66 Col:50 */
    {0x5E97,0xEAB5}, /* ��: GB2312 Code: 0xB5EA ==> Row:21 Col:74 */
    {0x5E99,0xEDC3}, /* ��: GB2312 Code: 0xC3ED ==> Row:35 Col:77 */
    {0x5E9A,0xFDB8}, /* ��: GB2312 Code: 0xB8FD ==> Row:24 Col:93 */
    {0x5E9C,0xAEB8}, /* ��: GB2312 Code: 0xB8AE ==> Row:24 Col:14 */
    {0x5E9E,0xD3C5}, /* ��: GB2312 Code: 0xC5D3 ==> Row:37 Col:51 */
    {0x5E9F,0xCFB7}, /* ��: GB2312 Code: 0xB7CF ==> Row:23 Col:47 */
    {0x5EA0,0xD4E2}, /* ��: GB2312 Code: 0xE2D4 ==> Row:66 Col:52 */
    {0x5EA5,0xD3E2}, /* ��: GB2312 Code: 0xE2D3 ==> Row:66 Col:51 */
    {0x5EA6,0xC8B6}, /* ��: GB2312 Code: 0xB6C8 ==> Row:22 Col:40 */
    {0x5EA7,0xF9D7}, /* ��: GB2312 Code: 0xD7F9 ==> Row:55 Col:89 */
    {0x5EAD,0xA5CD}, /* ͥ: GB2312 Code: 0xCDA5 ==> Row:45 Col:05 */
    {0x5EB3,0xD8E2}, /* ��: GB2312 Code: 0xE2D8 ==> Row:66 Col:56 */
    {0x5EB5,0xD6E2}, /* ��: GB2312 Code: 0xE2D6 ==> Row:66 Col:54 */
    {0x5EB6,0xFCCA}, /* ��: GB2312 Code: 0xCAFC ==> Row:42 Col:92 */
    {0x5EB7,0xB5BF}, /* ��: GB2312 Code: 0xBFB5 ==> Row:31 Col:21 */
    {0x5EB8,0xB9D3}, /* ӹ: GB2312 Code: 0xD3B9 ==> Row:51 Col:25 */
    {0x5EB9,0xD5E2}, /* ��: GB2312 Code: 0xE2D5 ==> Row:66 Col:53 */
    {0x5EBE,0xD7E2}, /* ��: GB2312 Code: 0xE2D7 ==> Row:66 Col:55 */
    {0x5EC9,0xAEC1}, /* ��: GB2312 Code: 0xC1AE ==> Row:33 Col:14 */
    {0x5ECA,0xC8C0}, /* ��: GB2312 Code: 0xC0C8 ==> Row:32 Col:40 */
    {0x5ED1,0xDBE2}, /* ��: GB2312 Code: 0xE2DB ==> Row:66 Col:59 */
    {0x5ED2,0xDAE2}, /* ��: GB2312 Code: 0xE2DA ==> Row:66 Col:58 */
    {0x5ED3,0xAAC0}, /* ��: GB2312 Code: 0xC0AA ==> Row:32 Col:10 */
    {0x5ED6,0xCEC1}, /* ��: GB2312 Code: 0xC1CE ==> Row:33 Col:46 */
    {0x5EDB,0xDCE2}, /* ��: GB2312 Code: 0xE2DC ==> Row:66 Col:60 */
    {0x5EE8,0xDDE2}, /* ��: GB2312 Code: 0xE2DD ==> Row:66 Col:61 */
    {0x5EEA,0xDEE2}, /* ��: GB2312 Code: 0xE2DE ==> Row:66 Col:62 */
    {0x5EF4,0xC8DB}, /* ��: GB2312 Code: 0xDBC8 ==> Row:59 Col:40 */
    {0x5EF6,0xD3D1}, /* ��: GB2312 Code: 0xD1D3 ==> Row:49 Col:51 */
    {0x5EF7,0xA2CD}, /* ͢: GB2312 Code: 0xCDA2 ==> Row:45 Col:02 */
    {0x5EFA,0xA8BD}, /* ��: GB2312 Code: 0xBDA8 ==> Row:29 Col:08 */
    {0x5EFE,0xC3DE}, /* ��: GB2312 Code: 0xDEC3 ==> Row:62 Col:35 */
    {0x5EFF,0xA5D8}, /* إ: GB2312 Code: 0xD8A5 ==> Row:56 Col:05 */
    {0x5F00,0xAABF}, /* ��: GB2312 Code: 0xBFAA ==> Row:31 Col:10 */
    {0x5F01,0xCDDB}, /* ��: GB2312 Code: 0xDBCD ==> Row:59 Col:45 */
    {0x5F02,0xECD2}, /* ��: GB2312 Code: 0xD2EC ==> Row:50 Col:76 */
    {0x5F03,0xFAC6}, /* ��: GB2312 Code: 0xC6FA ==> Row:38 Col:90 */
    {0x5F04,0xAAC5}, /* Ū: GB2312 Code: 0xC5AA ==> Row:37 Col:10 */
    {0x5F08,0xC4DE}, /* ��: GB2312 Code: 0xDEC4 ==> Row:62 Col:36 */
    {0x5F0A,0xD7B1}, /* ��: GB2312 Code: 0xB1D7 ==> Row:17 Col:55 */
    {0x5F0B,0xAEDF}, /* ߮: GB2312 Code: 0xDFAE ==> Row:63 Col:14 */
    {0x5F0F,0xBDCA}, /* ʽ: GB2312 Code: 0xCABD ==> Row:42 Col:29 */
    {0x5F11,0xB1DF}, /* ߱: GB2312 Code: 0xDFB1 ==> Row:63 Col:17 */
    {0x5F13,0xADB9}, /* ��: GB2312 Code: 0xB9AD ==> Row:25 Col:13 */
    {0x5F15,0xFDD2}, /* ��: GB2312 Code: 0xD2FD ==> Row:50 Col:93 */
    {0x5F17,0xA5B8}, /* ��: GB2312 Code: 0xB8A5 ==> Row:24 Col:05 */
    {0x5F18,0xEBBA}, /* ��: GB2312 Code: 0xBAEB ==> Row:26 Col:75 */
    {0x5F1B,0xDAB3}, /* ��: GB2312 Code: 0xB3DA ==> Row:19 Col:58 */
    {0x5F1F,0xDCB5}, /* ��: GB2312 Code: 0xB5DC ==> Row:21 Col:60 */
    {0x5F20,0xC5D5}, /* ��: GB2312 Code: 0xD5C5 ==> Row:53 Col:37 */
    {0x5F25,0xD6C3}, /* ��: GB2312 Code: 0xC3D6 ==> Row:35 Col:54 */
    {0x5F26,0xD2CF}, /* ��: GB2312 Code: 0xCFD2 ==> Row:47 Col:50 */
    {0x5F27,0xA1BB}, /* ��: GB2312 Code: 0xBBA1 ==> Row:27 Col:01 */
    {0x5F29,0xF3E5}, /* ��: GB2312 Code: 0xE5F3 ==> Row:69 Col:83 */
    {0x5F2A,0xF2E5}, /* ��: GB2312 Code: 0xE5F2 ==> Row:69 Col:82 */
    {0x5F2D,0xF4E5}, /* ��: GB2312 Code: 0xE5F4 ==> Row:69 Col:84 */
    {0x5F2F,0xE4CD}, /* ��: GB2312 Code: 0xCDE4 ==> Row:45 Col:68 */
    {0x5F31,0xF5C8}, /* ��: GB2312 Code: 0xC8F5 ==> Row:40 Col:85 */
    {0x5F39,0xAFB5}, /* ��: GB2312 Code: 0xB5AF ==> Row:21 Col:15 */
    {0x5F3A,0xBFC7}, /* ǿ: GB2312 Code: 0xC7BF ==> Row:39 Col:31 */
    {0x5F3C,0xF6E5}, /* ��: GB2312 Code: 0xE5F6 ==> Row:69 Col:86 */
    {0x5F40,0xB0EC}, /* �: GB2312 Code: 0xECB0 ==> Row:76 Col:16 */
    {0x5F50,0xE6E5}, /* ��: GB2312 Code: 0xE5E6 ==> Row:69 Col:70 */
    {0x5F52,0xE9B9}, /* ��: GB2312 Code: 0xB9E9 ==> Row:25 Col:73 */
    {0x5F53,0xB1B5}, /* ��: GB2312 Code: 0xB5B1 ==> Row:21 Col:17 */
    {0x5F55,0xBCC2}, /* ¼: GB2312 Code: 0xC2BC ==> Row:34 Col:28 */
    {0x5F56,0xE8E5}, /* ��: GB2312 Code: 0xE5E8 ==> Row:69 Col:72 */
    {0x5F57,0xE7E5}, /* ��: GB2312 Code: 0xE5E7 ==> Row:69 Col:71 */
    {0x5F58,0xE9E5}, /* ��: GB2312 Code: 0xE5E9 ==> Row:69 Col:73 */
    {0x5F5D,0xCDD2}, /* ��: GB2312 Code: 0xD2CD ==> Row:50 Col:45 */
    {0x5F61,0xEAE1}, /* ��: GB2312 Code: 0xE1EA ==> Row:65 Col:74 */
    {0x5F62,0xCED0}, /* ��: GB2312 Code: 0xD0CE ==> Row:48 Col:46 */
    {0x5F64,0xAECD}, /* ͮ: GB2312 Code: 0xCDAE ==> Row:45 Col:14 */
    {0x5F66,0xE5D1}, /* ��: GB2312 Code: 0xD1E5 ==> Row:49 Col:69 */
    {0x5F69,0xCAB2}, /* ��: GB2312 Code: 0xB2CA ==> Row:18 Col:42 */
    {0x5F6A,0xEBB1}, /* ��: GB2312 Code: 0xB1EB ==> Row:17 Col:75 */
    {0x5F6C,0xF2B1}, /* ��: GB2312 Code: 0xB1F2 ==> Row:17 Col:82 */
    {0x5F6D,0xEDC5}, /* ��: GB2312 Code: 0xC5ED ==> Row:37 Col:77 */
    {0x5F70,0xC3D5}, /* ��: GB2312 Code: 0xD5C3 ==> Row:53 Col:35 */
    {0x5F71,0xB0D3}, /* Ӱ: GB2312 Code: 0xD3B0 ==> Row:51 Col:16 */
    {0x5F73,0xDCE1}, /* ��: GB2312 Code: 0xE1DC ==> Row:65 Col:60 */
    {0x5F77,0xDDE1}, /* ��: GB2312 Code: 0xE1DD ==> Row:65 Col:61 */
    {0x5F79,0xDBD2}, /* ��: GB2312 Code: 0xD2DB ==> Row:50 Col:59 */
    {0x5F7B,0xB9B3}, /* ��: GB2312 Code: 0xB3B9 ==> Row:19 Col:25 */
    {0x5F7C,0xCBB1}, /* ��: GB2312 Code: 0xB1CB ==> Row:17 Col:43 */
    {0x5F80,0xF9CD}, /* ��: GB2312 Code: 0xCDF9 ==> Row:45 Col:89 */
    {0x5F81,0xF7D5}, /* ��: GB2312 Code: 0xD5F7 ==> Row:53 Col:87 */
    {0x5F82,0xDEE1}, /* ��: GB2312 Code: 0xE1DE ==> Row:65 Col:62 */
    {0x5F84,0xB6BE}, /* ��: GB2312 Code: 0xBEB6 ==> Row:30 Col:22 */
    {0x5F85,0xFDB4}, /* ��: GB2312 Code: 0xB4FD ==> Row:20 Col:93 */
    {0x5F87,0xDFE1}, /* ��: GB2312 Code: 0xE1DF ==> Row:65 Col:63 */
    {0x5F88,0xDCBA}, /* ��: GB2312 Code: 0xBADC ==> Row:26 Col:60 */
    {0x5F89,0xE0E1}, /* ��: GB2312 Code: 0xE1E0 ==> Row:65 Col:64 */
    {0x5F8A,0xB2BB}, /* ��: GB2312 Code: 0xBBB2 ==> Row:27 Col:18 */
    {0x5F8B,0xC9C2}, /* ��: GB2312 Code: 0xC2C9 ==> Row:34 Col:41 */
    {0x5F8C,0xE1E1}, /* ��: GB2312 Code: 0xE1E1 ==> Row:65 Col:65 */
    {0x5F90,0xECD0}, /* ��: GB2312 Code: 0xD0EC ==> Row:48 Col:76 */
    {0x5F92,0xBDCD}, /* ͽ: GB2312 Code: 0xCDBD ==> Row:45 Col:29 */
    {0x5F95,0xE2E1}, /* ��: GB2312 Code: 0xE1E2 ==> Row:65 Col:66 */
    {0x5F97,0xC3B5}, /* ��: GB2312 Code: 0xB5C3 ==> Row:21 Col:35 */
    {0x5F98,0xC7C5}, /* ��: GB2312 Code: 0xC5C7 ==> Row:37 Col:39 */
    {0x5F99,0xE3E1}, /* ��: GB2312 Code: 0xE1E3 ==> Row:65 Col:67 */
    {0x5F9C,0xE4E1}, /* ��: GB2312 Code: 0xE1E4 ==> Row:65 Col:68 */
    {0x5FA1,0xF9D3}, /* ��: GB2312 Code: 0xD3F9 ==> Row:51 Col:89 */
    {0x5FA8,0xE5E1}, /* ��: GB2312 Code: 0xE1E5 ==> Row:65 Col:69 */
    {0x5FAA,0xADD1}, /* ѭ: GB2312 Code: 0xD1AD ==> Row:49 Col:13 */
    {0x5FAD,0xE6E1}, /* ��: GB2312 Code: 0xE1E6 ==> Row:65 Col:70 */
    {0x5FAE,0xA2CE}, /* ΢: GB2312 Code: 0xCEA2 ==> Row:46 Col:02 */
    {0x5FB5,0xE7E1}, /* ��: GB2312 Code: 0xE1E7 ==> Row:65 Col:71 */
    {0x5FB7,0xC2B5}, /* ��: GB2312 Code: 0xB5C2 ==> Row:21 Col:34 */
    {0x5FBC,0xE8E1}, /* ��: GB2312 Code: 0xE1E8 ==> Row:65 Col:72 */
    {0x5FBD,0xD5BB}, /* ��: GB2312 Code: 0xBBD5 ==> Row:27 Col:53 */
    {0x5FC3,0xC4D0}, /* ��: GB2312 Code: 0xD0C4 ==> Row:48 Col:36 */
    {0x5FC4,0xE0E2}, /* ��: GB2312 Code: 0xE2E0 ==> Row:66 Col:64 */
    {0x5FC5,0xD8B1}, /* ��: GB2312 Code: 0xB1D8 ==> Row:17 Col:56 */
    {0x5FC6,0xE4D2}, /* ��: GB2312 Code: 0xD2E4 ==> Row:50 Col:68 */
    {0x5FC9,0xE1E2}, /* ��: GB2312 Code: 0xE2E1 ==> Row:66 Col:65 */
    {0x5FCC,0xC9BC}, /* ��: GB2312 Code: 0xBCC9 ==> Row:28 Col:41 */
    {0x5FCD,0xCCC8}, /* ��: GB2312 Code: 0xC8CC ==> Row:40 Col:44 */
    {0x5FCF,0xE3E2}, /* ��: GB2312 Code: 0xE2E3 ==> Row:66 Col:67 */
    {0x5FD0,0xFEEC}, /* ��: GB2312 Code: 0xECFE ==> Row:76 Col:94 */
    {0x5FD1,0xFDEC}, /* ��: GB2312 Code: 0xECFD ==> Row:76 Col:93 */
    {0x5FD2,0xAFDF}, /* ߯: GB2312 Code: 0xDFAF ==> Row:63 Col:15 */
    {0x5FD6,0xE2E2}, /* ��: GB2312 Code: 0xE2E2 ==> Row:66 Col:66 */
    {0x5FD7,0xBED6}, /* ־: GB2312 Code: 0xD6BE ==> Row:54 Col:30 */
    {0x5FD8,0xFCCD}, /* ��: GB2312 Code: 0xCDFC ==> Row:45 Col:92 */
    {0x5FD9,0xA6C3}, /* æ: GB2312 Code: 0xC3A6 ==> Row:35 Col:06 */
    {0x5FDD,0xC3E3}, /* ��: GB2312 Code: 0xE3C3 ==> Row:67 Col:35 */
    {0x5FE0,0xD2D6}, /* ��: GB2312 Code: 0xD6D2 ==> Row:54 Col:50 */
    {0x5FE1,0xE7E2}, /* ��: GB2312 Code: 0xE2E7 ==> Row:66 Col:71 */
    {0x5FE4,0xE8E2}, /* ��: GB2312 Code: 0xE2E8 ==> Row:66 Col:72 */
    {0x5FE7,0xC7D3}, /* ��: GB2312 Code: 0xD3C7 ==> Row:51 Col:39 */
    {0x5FEA,0xECE2}, /* ��: GB2312 Code: 0xE2EC ==> Row:66 Col:76 */
    {0x5FEB,0xECBF}, /* ��: GB2312 Code: 0xBFEC ==> Row:31 Col:76 */
    {0x5FED,0xEDE2}, /* ��: GB2312 Code: 0xE2ED ==> Row:66 Col:77 */
    {0x5FEE,0xE5E2}, /* ��: GB2312 Code: 0xE2E5 ==> Row:66 Col:69 */
    {0x5FF1,0xC0B3}, /* ��: GB2312 Code: 0xB3C0 ==> Row:19 Col:32 */
    {0x5FF5,0xEEC4}, /* ��: GB2312 Code: 0xC4EE ==> Row:36 Col:78 */
    {0x5FF8,0xEEE2}, /* ��: GB2312 Code: 0xE2EE ==> Row:66 Col:78 */
    {0x5FFB,0xC3D0}, /* ��: GB2312 Code: 0xD0C3 ==> Row:48 Col:35 */
    {0x5FFD,0xF6BA}, /* ��: GB2312 Code: 0xBAF6 ==> Row:26 Col:86 */
    {0x5FFE,0xE9E2}, /* ��: GB2312 Code: 0xE2E9 ==> Row:66 Col:73 */
    {0x5FFF,0xDEB7}, /* ��: GB2312 Code: 0xB7DE ==> Row:23 Col:62 */
    {0x6000,0xB3BB}, /* ��: GB2312 Code: 0xBBB3 ==> Row:27 Col:19 */
    {0x6001,0xACCC}, /* ̬: GB2312 Code: 0xCCAC ==> Row:44 Col:12 */
    {0x6002,0xCBCB}, /* ��: GB2312 Code: 0xCBCB ==> Row:43 Col:43 */
    {0x6003,0xE4E2}, /* ��: GB2312 Code: 0xE2E4 ==> Row:66 Col:68 */
    {0x6004,0xE6E2}, /* ��: GB2312 Code: 0xE2E6 ==> Row:66 Col:70 */
    {0x6005,0xEAE2}, /* ��: GB2312 Code: 0xE2EA ==> Row:66 Col:74 */
    {0x6006,0xEBE2}, /* ��: GB2312 Code: 0xE2EB ==> Row:66 Col:75 */
    {0x600A,0xF7E2}, /* ��: GB2312 Code: 0xE2F7 ==> Row:66 Col:87 */
    {0x600D,0xF4E2}, /* ��: GB2312 Code: 0xE2F4 ==> Row:66 Col:84 */
    {0x600E,0xF5D4}, /* ��: GB2312 Code: 0xD4F5 ==> Row:52 Col:85 */
    {0x600F,0xF3E2}, /* ��: GB2312 Code: 0xE2F3 ==> Row:66 Col:83 */
    {0x6012,0xADC5}, /* ŭ: GB2312 Code: 0xC5AD ==> Row:37 Col:13 */
    {0x6014,0xFAD5}, /* ��: GB2312 Code: 0xD5FA ==> Row:53 Col:90 */
    {0x6015,0xC2C5}, /* ��: GB2312 Code: 0xC5C2 ==> Row:37 Col:34 */
    {0x6016,0xC0B2}, /* ��: GB2312 Code: 0xB2C0 ==> Row:18 Col:32 */
    {0x6019,0xEFE2}, /* ��: GB2312 Code: 0xE2EF ==> Row:66 Col:79 */
    {0x601B,0xF2E2}, /* ��: GB2312 Code: 0xE2F2 ==> Row:66 Col:82 */
    {0x601C,0xAFC1}, /* ��: GB2312 Code: 0xC1AF ==> Row:33 Col:15 */
    {0x601D,0xBCCB}, /* ˼: GB2312 Code: 0xCBBC ==> Row:43 Col:28 */
    {0x6020,0xA1B5}, /* ��: GB2312 Code: 0xB5A1 ==> Row:21 Col:01 */
    {0x6021,0xF9E2}, /* ��: GB2312 Code: 0xE2F9 ==> Row:66 Col:89 */
    {0x6025,0xB1BC}, /* ��: GB2312 Code: 0xBCB1 ==> Row:28 Col:17 */
    {0x6026,0xF1E2}, /* ��: GB2312 Code: 0xE2F1 ==> Row:66 Col:81 */
    {0x6027,0xD4D0}, /* ��: GB2312 Code: 0xD0D4 ==> Row:48 Col:52 */
    {0x6028,0xB9D4}, /* Թ: GB2312 Code: 0xD4B9 ==> Row:52 Col:25 */
    {0x6029,0xF5E2}, /* ��: GB2312 Code: 0xE2F5 ==> Row:66 Col:85 */
    {0x602A,0xD6B9}, /* ��: GB2312 Code: 0xB9D6 ==> Row:25 Col:54 */
    {0x602B,0xF6E2}, /* ��: GB2312 Code: 0xE2F6 ==> Row:66 Col:86 */
    {0x602F,0xD3C7}, /* ��: GB2312 Code: 0xC7D3 ==> Row:39 Col:51 */
    {0x6035,0xF0E2}, /* ��: GB2312 Code: 0xE2F0 ==> Row:66 Col:80 */
    {0x603B,0xDCD7}, /* ��: GB2312 Code: 0xD7DC ==> Row:55 Col:60 */
    {0x603C,0xA1ED}, /* ��: GB2312 Code: 0xEDA1 ==> Row:77 Col:01 */
    {0x603F,0xF8E2}, /* ��: GB2312 Code: 0xE2F8 ==> Row:66 Col:88 */
    {0x6041,0xA5ED}, /* ��: GB2312 Code: 0xEDA5 ==> Row:77 Col:05 */
    {0x6042,0xFEE2}, /* ��: GB2312 Code: 0xE2FE ==> Row:66 Col:94 */
    {0x6043,0xD1CA}, /* ��: GB2312 Code: 0xCAD1 ==> Row:42 Col:49 */
    {0x604B,0xB5C1}, /* ��: GB2312 Code: 0xC1B5 ==> Row:33 Col:21 */
    {0x604D,0xD0BB}, /* ��: GB2312 Code: 0xBBD0 ==> Row:27 Col:48 */
    {0x6050,0xD6BF}, /* ��: GB2312 Code: 0xBFD6 ==> Row:31 Col:54 */
    {0x6052,0xE3BA}, /* ��: GB2312 Code: 0xBAE3 ==> Row:26 Col:67 */
    {0x6055,0xA1CB}, /* ˡ: GB2312 Code: 0xCBA1 ==> Row:43 Col:01 */
    {0x6059,0xA6ED}, /* ��: GB2312 Code: 0xEDA6 ==> Row:77 Col:06 */
    {0x605A,0xA3ED}, /* ��: GB2312 Code: 0xEDA3 ==> Row:77 Col:03 */
    {0x605D,0xA2ED}, /* ��: GB2312 Code: 0xEDA2 ==> Row:77 Col:02 */
    {0x6062,0xD6BB}, /* ��: GB2312 Code: 0xBBD6 ==> Row:27 Col:54 */
    {0x6063,0xA7ED}, /* ��: GB2312 Code: 0xEDA7 ==> Row:77 Col:07 */
    {0x6064,0xF4D0}, /* ��: GB2312 Code: 0xD0F4 ==> Row:48 Col:84 */
    {0x6067,0xA4ED}, /* ��: GB2312 Code: 0xEDA4 ==> Row:77 Col:04 */
    {0x6068,0xDEBA}, /* ��: GB2312 Code: 0xBADE ==> Row:26 Col:62 */
    {0x6069,0xF7B6}, /* ��: GB2312 Code: 0xB6F7 ==> Row:22 Col:87 */
    {0x606A,0xA1E3}, /* �: GB2312 Code: 0xE3A1 ==> Row:67 Col:01 */
    {0x606B,0xB2B6}, /* ��: GB2312 Code: 0xB6B2 ==> Row:22 Col:18 */
    {0x606C,0xF1CC}, /* ��: GB2312 Code: 0xCCF1 ==> Row:44 Col:81 */
    {0x606D,0xA7B9}, /* ��: GB2312 Code: 0xB9A7 ==> Row:25 Col:07 */
    {0x606F,0xA2CF}, /* Ϣ: GB2312 Code: 0xCFA2 ==> Row:47 Col:02 */
    {0x6070,0xA1C7}, /* ǡ: GB2312 Code: 0xC7A1 ==> Row:39 Col:01 */
    {0x6073,0xD2BF}, /* ��: GB2312 Code: 0xBFD2 ==> Row:31 Col:50 */
    {0x6076,0xF1B6}, /* ��: GB2312 Code: 0xB6F1 ==> Row:22 Col:81 */
    {0x6078,0xFAE2}, /* ��: GB2312 Code: 0xE2FA ==> Row:66 Col:90 */
    {0x6079,0xFBE2}, /* ��: GB2312 Code: 0xE2FB ==> Row:66 Col:91 */
    {0x607A,0xFDE2}, /* ��: GB2312 Code: 0xE2FD ==> Row:66 Col:93 */
    {0x607B,0xFCE2}, /* ��: GB2312 Code: 0xE2FC ==> Row:66 Col:92 */
    {0x607C,0xD5C4}, /* ��: GB2312 Code: 0xC4D5 ==> Row:36 Col:53 */
    {0x607D,0xA2E3}, /* �: GB2312 Code: 0xE3A2 ==> Row:67 Col:02 */
    {0x607F,0xC1D3}, /* ��: GB2312 Code: 0xD3C1 ==> Row:51 Col:33 */
    {0x6083,0xA7E3}, /* �: GB2312 Code: 0xE3A7 ==> Row:67 Col:07 */
    {0x6084,0xC4C7}, /* ��: GB2312 Code: 0xC7C4 ==> Row:39 Col:36 */
    {0x6089,0xA4CF}, /* Ϥ: GB2312 Code: 0xCFA4 ==> Row:47 Col:04 */
    {0x608C,0xA9E3}, /* �: GB2312 Code: 0xE3A9 ==> Row:67 Col:09 */
    {0x608D,0xB7BA}, /* ��: GB2312 Code: 0xBAB7 ==> Row:26 Col:23 */
    {0x6092,0xA8E3}, /* �: GB2312 Code: 0xE3A8 ==> Row:67 Col:08 */
    {0x6094,0xDABB}, /* ��: GB2312 Code: 0xBBDA ==> Row:27 Col:58 */
    {0x6096,0xA3E3}, /* �: GB2312 Code: 0xE3A3 ==> Row:67 Col:03 */
    {0x609A,0xA4E3}, /* �: GB2312 Code: 0xE3A4 ==> Row:67 Col:04 */
    {0x609B,0xAAE3}, /* �: GB2312 Code: 0xE3AA ==> Row:67 Col:10 */
    {0x609D,0xA6E3}, /* �: GB2312 Code: 0xE3A6 ==> Row:67 Col:06 */
    {0x609F,0xF2CE}, /* ��: GB2312 Code: 0xCEF2 ==> Row:46 Col:82 */
    {0x60A0,0xC6D3}, /* ��: GB2312 Code: 0xD3C6 ==> Row:51 Col:38 */
    {0x60A3,0xBCBB}, /* ��: GB2312 Code: 0xBBBC ==> Row:27 Col:28 */
    {0x60A6,0xC3D4}, /* ��: GB2312 Code: 0xD4C3 ==> Row:52 Col:35 */
    {0x60A8,0xFAC4}, /* ��: GB2312 Code: 0xC4FA ==> Row:36 Col:90 */
    {0x60AB,0xA8ED}, /* ��: GB2312 Code: 0xEDA8 ==> Row:77 Col:08 */
    {0x60AC,0xFCD0}, /* ��: GB2312 Code: 0xD0FC ==> Row:48 Col:92 */
    {0x60AD,0xA5E3}, /* �: GB2312 Code: 0xE3A5 ==> Row:67 Col:05 */
    {0x60AF,0xF5C3}, /* ��: GB2312 Code: 0xC3F5 ==> Row:35 Col:85 */
    {0x60B1,0xADE3}, /* �: GB2312 Code: 0xE3AD ==> Row:67 Col:13 */
    {0x60B2,0xAFB1}, /* ��: GB2312 Code: 0xB1AF ==> Row:17 Col:15 */
    {0x60B4,0xB2E3}, /* �: GB2312 Code: 0xE3B2 ==> Row:67 Col:18 */
    {0x60B8,0xC2BC}, /* ��: GB2312 Code: 0xBCC2 ==> Row:28 Col:34 */
    {0x60BB,0xACE3}, /* �: GB2312 Code: 0xE3AC ==> Row:67 Col:12 */
    {0x60BC,0xBFB5}, /* ��: GB2312 Code: 0xB5BF ==> Row:21 Col:31 */
    {0x60C5,0xE9C7}, /* ��: GB2312 Code: 0xC7E9 ==> Row:39 Col:73 */
    {0x60C6,0xB0E3}, /* �: GB2312 Code: 0xE3B0 ==> Row:67 Col:16 */
    {0x60CA,0xAABE}, /* ��: GB2312 Code: 0xBEAA ==> Row:30 Col:10 */
    {0x60CB,0xEFCD}, /* ��: GB2312 Code: 0xCDEF ==> Row:45 Col:79 */
    {0x60D1,0xF3BB}, /* ��: GB2312 Code: 0xBBF3 ==> Row:27 Col:83 */
    {0x60D5,0xE8CC}, /* ��: GB2312 Code: 0xCCE8 ==> Row:44 Col:72 */
    {0x60D8,0xAFE3}, /* �: GB2312 Code: 0xE3AF ==> Row:67 Col:15 */
    {0x60DA,0xB1E3}, /* �: GB2312 Code: 0xE3B1 ==> Row:67 Col:17 */
    {0x60DC,0xA7CF}, /* ϧ: GB2312 Code: 0xCFA7 ==> Row:47 Col:07 */
    {0x60DD,0xAEE3}, /* �: GB2312 Code: 0xE3AE ==> Row:67 Col:14 */
    {0x60DF,0xA9CE}, /* Ω: GB2312 Code: 0xCEA9 ==> Row:46 Col:09 */
    {0x60E0,0xDDBB}, /* ��: GB2312 Code: 0xBBDD ==> Row:27 Col:61 */
    {0x60E6,0xEBB5}, /* ��: GB2312 Code: 0xB5EB ==> Row:21 Col:75 */
    {0x60E7,0xE5BE}, /* ��: GB2312 Code: 0xBEE5 ==> Row:30 Col:69 */
    {0x60E8,0xD2B2}, /* ��: GB2312 Code: 0xB2D2 ==> Row:18 Col:50 */
    {0x60E9,0xCDB3}, /* ��: GB2312 Code: 0xB3CD ==> Row:19 Col:45 */
    {0x60EB,0xB9B1}, /* ��: GB2312 Code: 0xB1B9 ==> Row:17 Col:25 */
    {0x60EC,0xABE3}, /* �: GB2312 Code: 0xE3AB ==> Row:67 Col:11 */
    {0x60ED,0xD1B2}, /* ��: GB2312 Code: 0xB2D1 ==> Row:18 Col:49 */
    {0x60EE,0xACB5}, /* ��: GB2312 Code: 0xB5AC ==> Row:21 Col:12 */
    {0x60EF,0xDFB9}, /* ��: GB2312 Code: 0xB9DF ==> Row:25 Col:63 */
    {0x60F0,0xE8B6}, /* ��: GB2312 Code: 0xB6E8 ==> Row:22 Col:72 */
    {0x60F3,0xEBCF}, /* ��: GB2312 Code: 0xCFEB ==> Row:47 Col:75 */
    {0x60F4,0xB7E3}, /* �: GB2312 Code: 0xE3B7 ==> Row:67 Col:23 */
    {0x60F6,0xCCBB}, /* ��: GB2312 Code: 0xBBCC ==> Row:27 Col:44 */
    {0x60F9,0xC7C8}, /* ��: GB2312 Code: 0xC8C7 ==> Row:40 Col:39 */
    {0x60FA,0xCAD0}, /* ��: GB2312 Code: 0xD0CA ==> Row:48 Col:42 */
    {0x6100,0xB8E3}, /* �: GB2312 Code: 0xE3B8 ==> Row:67 Col:24 */
    {0x6101,0xEEB3}, /* ��: GB2312 Code: 0xB3EE ==> Row:19 Col:78 */
    {0x6106,0xA9ED}, /* ��: GB2312 Code: 0xEDA9 ==> Row:77 Col:09 */
    {0x6108,0xFAD3}, /* ��: GB2312 Code: 0xD3FA ==> Row:51 Col:90 */
    {0x6109,0xE4D3}, /* ��: GB2312 Code: 0xD3E4 ==> Row:51 Col:68 */
    {0x610D,0xAAED}, /* ��: GB2312 Code: 0xEDAA ==> Row:77 Col:10 */
    {0x610E,0xB9E3}, /* �: GB2312 Code: 0xE3B9 ==> Row:67 Col:25 */
    {0x610F,0xE2D2}, /* ��: GB2312 Code: 0xD2E2 ==> Row:50 Col:66 */
    {0x6115,0xB5E3}, /* �: GB2312 Code: 0xE3B5 ==> Row:67 Col:21 */
    {0x611A,0xDED3}, /* ��: GB2312 Code: 0xD3DE ==> Row:51 Col:62 */
    {0x611F,0xD0B8}, /* ��: GB2312 Code: 0xB8D0 ==> Row:24 Col:48 */
    {0x6120,0xB3E3}, /* �: GB2312 Code: 0xE3B3 ==> Row:67 Col:19 */
    {0x6123,0xB6E3}, /* �: GB2312 Code: 0xE3B6 ==> Row:67 Col:22 */
    {0x6124,0xDFB7}, /* ��: GB2312 Code: 0xB7DF ==> Row:23 Col:63 */
    {0x6126,0xB4E3}, /* �: GB2312 Code: 0xE3B4 ==> Row:67 Col:20 */
    {0x6127,0xA2C0}, /* ��: GB2312 Code: 0xC0A2 ==> Row:32 Col:02 */
    {0x612B,0xBAE3}, /* �: GB2312 Code: 0xE3BA ==> Row:67 Col:26 */
    {0x613F,0xB8D4}, /* Ը: GB2312 Code: 0xD4B8 ==> Row:52 Col:24 */
    {0x6148,0xC8B4}, /* ��: GB2312 Code: 0xB4C8 ==> Row:20 Col:40 */
    {0x614A,0xBBE3}, /* �: GB2312 Code: 0xE3BB ==> Row:67 Col:27 */
    {0x614C,0xC5BB}, /* ��: GB2312 Code: 0xBBC5 ==> Row:27 Col:37 */
    {0x614E,0xF7C9}, /* ��: GB2312 Code: 0xC9F7 ==> Row:41 Col:87 */
    {0x6151,0xE5C9}, /* ��: GB2312 Code: 0xC9E5 ==> Row:41 Col:69 */
    {0x6155,0xBDC4}, /* Ľ: GB2312 Code: 0xC4BD ==> Row:36 Col:29 */
    {0x615D,0xABED}, /* ��: GB2312 Code: 0xEDAB ==> Row:77 Col:11 */
    {0x6162,0xFDC2}, /* ��: GB2312 Code: 0xC2FD ==> Row:34 Col:93 */
    {0x6167,0xDBBB}, /* ��: GB2312 Code: 0xBBDB ==> Row:27 Col:59 */
    {0x6168,0xAEBF}, /* ��: GB2312 Code: 0xBFAE ==> Row:31 Col:14 */
    {0x6170,0xBFCE}, /* ο: GB2312 Code: 0xCEBF ==> Row:46 Col:31 */
    {0x6175,0xBCE3}, /* �: GB2312 Code: 0xE3BC ==> Row:67 Col:28 */
    {0x6177,0xB6BF}, /* ��: GB2312 Code: 0xBFB6 ==> Row:31 Col:22 */
    {0x618B,0xEFB1}, /* ��: GB2312 Code: 0xB1EF ==> Row:17 Col:79 */
    {0x618E,0xF7D4}, /* ��: GB2312 Code: 0xD4F7 ==> Row:52 Col:87 */
    {0x6194,0xBEE3}, /* �: GB2312 Code: 0xE3BE ==> Row:67 Col:30 */
    {0x619D,0xADED}, /* ��: GB2312 Code: 0xEDAD ==> Row:77 Col:13 */
    {0x61A7,0xBFE3}, /* �: GB2312 Code: 0xE3BF ==> Row:67 Col:31 */
    {0x61A8,0xA9BA}, /* ��: GB2312 Code: 0xBAA9 ==> Row:26 Col:09 */
    {0x61A9,0xACED}, /* ��: GB2312 Code: 0xEDAC ==> Row:77 Col:12 */
    {0x61AC,0xBDE3}, /* �: GB2312 Code: 0xE3BD ==> Row:67 Col:29 */
    {0x61B7,0xC0E3}, /* ��: GB2312 Code: 0xE3C0 ==> Row:67 Col:32 */
    {0x61BE,0xB6BA}, /* ��: GB2312 Code: 0xBAB6 ==> Row:26 Col:22 */
    {0x61C2,0xAEB6}, /* ��: GB2312 Code: 0xB6AE ==> Row:22 Col:14 */
    {0x61C8,0xB8D0}, /* и: GB2312 Code: 0xD0B8 ==> Row:48 Col:24 */
    {0x61CA,0xC3B0}, /* ��: GB2312 Code: 0xB0C3 ==> Row:16 Col:35 */
    {0x61CB,0xAEED}, /* ��: GB2312 Code: 0xEDAE ==> Row:77 Col:14 */
    {0x61D1,0xAFED}, /* ��: GB2312 Code: 0xEDAF ==> Row:77 Col:15 */
    {0x61D2,0xC1C0}, /* ��: GB2312 Code: 0xC0C1 ==> Row:32 Col:33 */
    {0x61D4,0xC1E3}, /* ��: GB2312 Code: 0xE3C1 ==> Row:67 Col:33 */
    {0x61E6,0xB3C5}, /* ų: GB2312 Code: 0xC5B3 ==> Row:37 Col:19 */
    {0x61F5,0xC2E3}, /* ��: GB2312 Code: 0xE3C2 ==> Row:67 Col:34 */
    {0x61FF,0xB2DC}, /* ܲ: GB2312 Code: 0xDCB2 ==> Row:60 Col:18 */
    {0x6206,0xB0ED}, /* ��: GB2312 Code: 0xEDB0 ==> Row:77 Col:16 */
    {0x6208,0xEAB8}, /* ��: GB2312 Code: 0xB8EA ==> Row:24 Col:74 */
    {0x620A,0xECCE}, /* ��: GB2312 Code: 0xCEEC ==> Row:46 Col:76 */
    {0x620B,0xA7EA}, /* �: GB2312 Code: 0xEAA7 ==> Row:74 Col:07 */
    {0x620C,0xE7D0}, /* ��: GB2312 Code: 0xD0E7 ==> Row:48 Col:71 */
    {0x620D,0xF9CA}, /* ��: GB2312 Code: 0xCAF9 ==> Row:42 Col:89 */
    {0x620E,0xD6C8}, /* ��: GB2312 Code: 0xC8D6 ==> Row:40 Col:54 */
    {0x620F,0xB7CF}, /* Ϸ: GB2312 Code: 0xCFB7 ==> Row:47 Col:23 */
    {0x6210,0xC9B3}, /* ��: GB2312 Code: 0xB3C9 ==> Row:19 Col:41 */
    {0x6211,0xD2CE}, /* ��: GB2312 Code: 0xCED2 ==> Row:46 Col:50 */
    {0x6212,0xE4BD}, /* ��: GB2312 Code: 0xBDE4 ==> Row:29 Col:68 */
    {0x6215,0xDEE3}, /* ��: GB2312 Code: 0xE3DE ==> Row:67 Col:62 */
    {0x6216,0xF2BB}, /* ��: GB2312 Code: 0xBBF2 ==> Row:27 Col:82 */
    {0x6217,0xA8EA}, /* �: GB2312 Code: 0xEAA8 ==> Row:74 Col:08 */
    {0x6218,0xBDD5}, /* ս: GB2312 Code: 0xD5BD ==> Row:53 Col:29 */
    {0x621A,0xDDC6}, /* ��: GB2312 Code: 0xC6DD ==> Row:38 Col:61 */
    {0x621B,0xA9EA}, /* �: GB2312 Code: 0xEAA9 ==> Row:74 Col:09 */
    {0x621F,0xAAEA}, /* �: GB2312 Code: 0xEAAA ==> Row:74 Col:10 */
    {0x6221,0xACEA}, /* �: GB2312 Code: 0xEAAC ==> Row:74 Col:12 */
    {0x6222,0xABEA}, /* �: GB2312 Code: 0xEAAB ==> Row:74 Col:11 */
    {0x6224,0xAEEA}, /* �: GB2312 Code: 0xEAAE ==> Row:74 Col:14 */
    {0x6225,0xADEA}, /* �: GB2312 Code: 0xEAAD ==> Row:74 Col:13 */
    {0x622A,0xD8BD}, /* ��: GB2312 Code: 0xBDD8 ==> Row:29 Col:56 */
    {0x622C,0xAFEA}, /* �: GB2312 Code: 0xEAAF ==> Row:74 Col:15 */
    {0x622E,0xBEC2}, /* ¾: GB2312 Code: 0xC2BE ==> Row:34 Col:30 */
    {0x6233,0xC1B4}, /* ��: GB2312 Code: 0xB4C1 ==> Row:20 Col:33 */
    {0x6234,0xF7B4}, /* ��: GB2312 Code: 0xB4F7 ==> Row:20 Col:87 */
    {0x6237,0xA7BB}, /* ��: GB2312 Code: 0xBBA7 ==> Row:27 Col:07 */
    {0x623D,0xE6EC}, /* ��: GB2312 Code: 0xECE6 ==> Row:76 Col:70 */
    {0x623E,0xE5EC}, /* ��: GB2312 Code: 0xECE5 ==> Row:76 Col:69 */
    {0x623F,0xBFB7}, /* ��: GB2312 Code: 0xB7BF ==> Row:23 Col:31 */
    {0x6240,0xF9CB}, /* ��: GB2312 Code: 0xCBF9 ==> Row:43 Col:89 */
    {0x6241,0xE2B1}, /* ��: GB2312 Code: 0xB1E2 ==> Row:17 Col:66 */
    {0x6243,0xE7EC}, /* ��: GB2312 Code: 0xECE7 ==> Row:76 Col:71 */
    {0x6247,0xC8C9}, /* ��: GB2312 Code: 0xC9C8 ==> Row:41 Col:40 */
    {0x6248,0xE8EC}, /* ��: GB2312 Code: 0xECE8 ==> Row:76 Col:72 */
    {0x6249,0xE9EC}, /* ��: GB2312 Code: 0xECE9 ==> Row:76 Col:73 */
    {0x624B,0xD6CA}, /* ��: GB2312 Code: 0xCAD6 ==> Row:42 Col:54 */
    {0x624C,0xD0DE}, /* ��: GB2312 Code: 0xDED0 ==> Row:62 Col:48 */
    {0x624D,0xC5B2}, /* ��: GB2312 Code: 0xB2C5 ==> Row:18 Col:37 */
    {0x624E,0xFAD4}, /* ��: GB2312 Code: 0xD4FA ==> Row:52 Col:90 */
    {0x6251,0xCBC6}, /* ��: GB2312 Code: 0xC6CB ==> Row:38 Col:43 */
    {0x6252,0xC7B0}, /* ��: GB2312 Code: 0xB0C7 ==> Row:16 Col:39 */
    {0x6253,0xF2B4}, /* ��: GB2312 Code: 0xB4F2 ==> Row:20 Col:82 */
    {0x6254,0xD3C8}, /* ��: GB2312 Code: 0xC8D3 ==> Row:40 Col:51 */
    {0x6258,0xD0CD}, /* ��: GB2312 Code: 0xCDD0 ==> Row:45 Col:48 */
    {0x625B,0xB8BF}, /* ��: GB2312 Code: 0xBFB8 ==> Row:31 Col:24 */
    {0x6263,0xDBBF}, /* ��: GB2312 Code: 0xBFDB ==> Row:31 Col:59 */
    {0x6266,0xA4C7}, /* Ǥ: GB2312 Code: 0xC7A4 ==> Row:39 Col:04 */
    {0x6267,0xB4D6}, /* ִ: GB2312 Code: 0xD6B4 ==> Row:54 Col:20 */
    {0x6269,0xA9C0}, /* ��: GB2312 Code: 0xC0A9 ==> Row:32 Col:09 */
    {0x626A,0xD1DE}, /* ��: GB2312 Code: 0xDED1 ==> Row:62 Col:49 */
    {0x626B,0xA8C9}, /* ɨ: GB2312 Code: 0xC9A8 ==> Row:41 Col:08 */
    {0x626C,0xEFD1}, /* ��: GB2312 Code: 0xD1EF ==> Row:49 Col:79 */
    {0x626D,0xA4C5}, /* Ť: GB2312 Code: 0xC5A4 ==> Row:37 Col:04 */
    {0x626E,0xE7B0}, /* ��: GB2312 Code: 0xB0E7 ==> Row:16 Col:71 */
    {0x626F,0xB6B3}, /* ��: GB2312 Code: 0xB3B6 ==> Row:19 Col:22 */
    {0x6270,0xC5C8}, /* ��: GB2312 Code: 0xC8C5 ==> Row:40 Col:37 */
    {0x6273,0xE2B0}, /* ��: GB2312 Code: 0xB0E2 ==> Row:16 Col:66 */
    {0x6276,0xF6B7}, /* ��: GB2312 Code: 0xB7F6 ==> Row:23 Col:86 */
    {0x6279,0xFAC5}, /* ��: GB2312 Code: 0xC5FA ==> Row:37 Col:90 */
    {0x627C,0xF3B6}, /* ��: GB2312 Code: 0xB6F3 ==> Row:22 Col:83 */
    {0x627E,0xD2D5}, /* ��: GB2312 Code: 0xD5D2 ==> Row:53 Col:50 */
    {0x627F,0xD0B3}, /* ��: GB2312 Code: 0xB3D0 ==> Row:19 Col:48 */
    {0x6280,0xBCBC}, /* ��: GB2312 Code: 0xBCBC ==> Row:28 Col:28 */
    {0x6284,0xADB3}, /* ��: GB2312 Code: 0xB3AD ==> Row:19 Col:13 */
    {0x6289,0xF1BE}, /* ��: GB2312 Code: 0xBEF1 ==> Row:30 Col:81 */
    {0x628A,0xD1B0}, /* ��: GB2312 Code: 0xB0D1 ==> Row:16 Col:49 */
    {0x6291,0xD6D2}, /* ��: GB2312 Code: 0xD2D6 ==> Row:50 Col:54 */
    {0x6292,0xE3CA}, /* ��: GB2312 Code: 0xCAE3 ==> Row:42 Col:67 */
    {0x6293,0xA5D7}, /* ץ: GB2312 Code: 0xD7A5 ==> Row:55 Col:05 */
    {0x6295,0xB6CD}, /* Ͷ: GB2312 Code: 0xCDB6 ==> Row:45 Col:22 */
    {0x6296,0xB6B6}, /* ��: GB2312 Code: 0xB6B6 ==> Row:22 Col:22 */
    {0x6297,0xB9BF}, /* ��: GB2312 Code: 0xBFB9 ==> Row:31 Col:25 */
    {0x6298,0xDBD5}, /* ��: GB2312 Code: 0xD5DB ==> Row:53 Col:59 */
    {0x629A,0xA7B8}, /* ��: GB2312 Code: 0xB8A7 ==> Row:24 Col:07 */
    {0x629B,0xD7C5}, /* ��: GB2312 Code: 0xC5D7 ==> Row:37 Col:55 */
    {0x629F,0xD2DE}, /* ��: GB2312 Code: 0xDED2 ==> Row:62 Col:50 */
    {0x62A0,0xD9BF}, /* ��: GB2312 Code: 0xBFD9 ==> Row:31 Col:57 */
    {0x62A1,0xD5C2}, /* ��: GB2312 Code: 0xC2D5 ==> Row:34 Col:53 */
    {0x62A2,0xC0C7}, /* ��: GB2312 Code: 0xC7C0 ==> Row:39 Col:32 */
    {0x62A4,0xA4BB}, /* ��: GB2312 Code: 0xBBA4 ==> Row:27 Col:04 */
    {0x62A5,0xA8B1}, /* ��: GB2312 Code: 0xB1A8 ==> Row:17 Col:08 */
    {0x62A8,0xEAC5}, /* ��: GB2312 Code: 0xC5EA ==> Row:37 Col:74 */
    {0x62AB,0xFBC5}, /* ��: GB2312 Code: 0xC5FB ==> Row:37 Col:91 */
    {0x62AC,0xA7CC}, /* ̧: GB2312 Code: 0xCCA7 ==> Row:44 Col:07 */
    {0x62B1,0xA7B1}, /* ��: GB2312 Code: 0xB1A7 ==> Row:17 Col:07 */
    {0x62B5,0xD6B5}, /* ��: GB2312 Code: 0xB5D6 ==> Row:21 Col:54 */
    {0x62B9,0xA8C4}, /* Ĩ: GB2312 Code: 0xC4A8 ==> Row:36 Col:08 */
    {0x62BB,0xD3DE}, /* ��: GB2312 Code: 0xDED3 ==> Row:62 Col:51 */
    {0x62BC,0xBAD1}, /* Ѻ: GB2312 Code: 0xD1BA ==> Row:49 Col:26 */
    {0x62BD,0xE9B3}, /* ��: GB2312 Code: 0xB3E9 ==> Row:19 Col:73 */
    {0x62BF,0xF2C3}, /* ��: GB2312 Code: 0xC3F2 ==> Row:35 Col:82 */
    {0x62C2,0xF7B7}, /* ��: GB2312 Code: 0xB7F7 ==> Row:23 Col:87 */
    {0x62C4,0xF4D6}, /* ��: GB2312 Code: 0xD6F4 ==> Row:54 Col:84 */
    {0x62C5,0xA3B5}, /* ��: GB2312 Code: 0xB5A3 ==> Row:21 Col:03 */
    {0x62C6,0xF0B2}, /* ��: GB2312 Code: 0xB2F0 ==> Row:18 Col:80 */
    {0x62C7,0xB4C4}, /* Ĵ: GB2312 Code: 0xC4B4 ==> Row:36 Col:20 */
    {0x62C8,0xE9C4}, /* ��: GB2312 Code: 0xC4E9 ==> Row:36 Col:73 */
    {0x62C9,0xADC0}, /* ��: GB2312 Code: 0xC0AD ==> Row:32 Col:13 */
    {0x62CA,0xD4DE}, /* ��: GB2312 Code: 0xDED4 ==> Row:62 Col:52 */
    {0x62CC,0xE8B0}, /* ��: GB2312 Code: 0xB0E8 ==> Row:16 Col:72 */
    {0x62CD,0xC4C5}, /* ��: GB2312 Code: 0xC5C4 ==> Row:37 Col:36 */
    {0x62CE,0xE0C1}, /* ��: GB2312 Code: 0xC1E0 ==> Row:33 Col:64 */
    {0x62D0,0xD5B9}, /* ��: GB2312 Code: 0xB9D5 ==> Row:25 Col:53 */
    {0x62D2,0xDCBE}, /* ��: GB2312 Code: 0xBEDC ==> Row:30 Col:60 */
    {0x62D3,0xD8CD}, /* ��: GB2312 Code: 0xCDD8 ==> Row:45 Col:56 */
    {0x62D4,0xCEB0}, /* ��: GB2312 Code: 0xB0CE ==> Row:16 Col:46 */
    {0x62D6,0xCFCD}, /* ��: GB2312 Code: 0xCDCF ==> Row:45 Col:47 */
    {0x62D7,0xD6DE}, /* ��: GB2312 Code: 0xDED6 ==> Row:62 Col:54 */
    {0x62D8,0xD0BE}, /* ��: GB2312 Code: 0xBED0 ==> Row:30 Col:48 */
    {0x62D9,0xBED7}, /* ׾: GB2312 Code: 0xD7BE ==> Row:55 Col:30 */
    {0x62DA,0xD5DE}, /* ��: GB2312 Code: 0xDED5 ==> Row:62 Col:53 */
    {0x62DB,0xD0D5}, /* ��: GB2312 Code: 0xD5D0 ==> Row:53 Col:48 */
    {0x62DC,0xDDB0}, /* ��: GB2312 Code: 0xB0DD ==> Row:16 Col:61 */
    {0x62DF,0xE2C4}, /* ��: GB2312 Code: 0xC4E2 ==> Row:36 Col:66 */
    {0x62E2,0xA3C2}, /* £: GB2312 Code: 0xC2A3 ==> Row:34 Col:03 */
    {0x62E3,0xF0BC}, /* ��: GB2312 Code: 0xBCF0 ==> Row:28 Col:80 */
    {0x62E5,0xB5D3}, /* ӵ: GB2312 Code: 0xD3B5 ==> Row:51 Col:21 */
    {0x62E6,0xB9C0}, /* ��: GB2312 Code: 0xC0B9 ==> Row:32 Col:25 */
    {0x62E7,0xA1C5}, /* š: GB2312 Code: 0xC5A1 ==> Row:37 Col:01 */
    {0x62E8,0xA6B2}, /* ��: GB2312 Code: 0xB2A6 ==> Row:18 Col:06 */
    {0x62E9,0xF1D4}, /* ��: GB2312 Code: 0xD4F1 ==> Row:52 Col:81 */
    {0x62EC,0xA8C0}, /* ��: GB2312 Code: 0xC0A8 ==> Row:32 Col:08 */
    {0x62ED,0xC3CA}, /* ��: GB2312 Code: 0xCAC3 ==> Row:42 Col:35 */
    {0x62EE,0xD7DE}, /* ��: GB2312 Code: 0xDED7 ==> Row:62 Col:55 */
    {0x62EF,0xFCD5}, /* ��: GB2312 Code: 0xD5FC ==> Row:53 Col:92 */
    {0x62F1,0xB0B9}, /* ��: GB2312 Code: 0xB9B0 ==> Row:25 Col:16 */
    {0x62F3,0xADC8}, /* ȭ: GB2312 Code: 0xC8AD ==> Row:40 Col:13 */
    {0x62F4,0xA9CB}, /* ˩: GB2312 Code: 0xCBA9 ==> Row:43 Col:09 */
    {0x62F6,0xD9DE}, /* ��: GB2312 Code: 0xDED9 ==> Row:62 Col:57 */
    {0x62F7,0xBDBF}, /* ��: GB2312 Code: 0xBFBD ==> Row:31 Col:29 */
    {0x62FC,0xB4C6}, /* ƴ: GB2312 Code: 0xC6B4 ==> Row:38 Col:20 */
    {0x62FD,0xA7D7}, /* ק: GB2312 Code: 0xD7A7 ==> Row:55 Col:07 */
    {0x62FE,0xB0CA}, /* ʰ: GB2312 Code: 0xCAB0 ==> Row:42 Col:16 */
    {0x62FF,0xC3C4}, /* ��: GB2312 Code: 0xC4C3 ==> Row:36 Col:35 */
    {0x6301,0xD6B3}, /* ��: GB2312 Code: 0xB3D6 ==> Row:19 Col:54 */
    {0x6302,0xD2B9}, /* ��: GB2312 Code: 0xB9D2 ==> Row:25 Col:50 */
    {0x6307,0xB8D6}, /* ָ: GB2312 Code: 0xD6B8 ==> Row:54 Col:24 */
    {0x6308,0xFCEA}, /* ��: GB2312 Code: 0xEAFC ==> Row:74 Col:92 */
    {0x6309,0xB4B0}, /* ��: GB2312 Code: 0xB0B4 ==> Row:16 Col:20 */
    {0x630E,0xE6BF}, /* ��: GB2312 Code: 0xBFE6 ==> Row:31 Col:70 */
    {0x6311,0xF4CC}, /* ��: GB2312 Code: 0xCCF4 ==> Row:44 Col:84 */
    {0x6316,0xDACD}, /* ��: GB2312 Code: 0xCDDA ==> Row:45 Col:58 */
    {0x631A,0xBFD6}, /* ֿ: GB2312 Code: 0xD6BF ==> Row:54 Col:31 */
    {0x631B,0xCEC2}, /* ��: GB2312 Code: 0xC2CE ==> Row:34 Col:46 */
    {0x631D,0xCECE}, /* ��: GB2312 Code: 0xCECE ==> Row:46 Col:46 */
    {0x631E,0xA2CC}, /* ̢: GB2312 Code: 0xCCA2 ==> Row:44 Col:02 */
    {0x631F,0xAED0}, /* Ю: GB2312 Code: 0xD0AE ==> Row:48 Col:14 */
    {0x6320,0xD3C4}, /* ��: GB2312 Code: 0xC4D3 ==> Row:36 Col:51 */
    {0x6321,0xB2B5}, /* ��: GB2312 Code: 0xB5B2 ==> Row:21 Col:18 */
    {0x6322,0xD8DE}, /* ��: GB2312 Code: 0xDED8 ==> Row:62 Col:56 */
    {0x6323,0xF5D5}, /* ��: GB2312 Code: 0xD5F5 ==> Row:53 Col:85 */
    {0x6324,0xB7BC}, /* ��: GB2312 Code: 0xBCB7 ==> Row:28 Col:23 */
    {0x6325,0xD3BB}, /* ��: GB2312 Code: 0xBBD3 ==> Row:27 Col:51 */
    {0x6328,0xA4B0}, /* ��: GB2312 Code: 0xB0A4 ==> Row:16 Col:04 */
    {0x632A,0xB2C5}, /* Ų: GB2312 Code: 0xC5B2 ==> Row:37 Col:18 */
    {0x632B,0xECB4}, /* ��: GB2312 Code: 0xB4EC ==> Row:20 Col:76 */
    {0x632F,0xF1D5}, /* ��: GB2312 Code: 0xD5F1 ==> Row:53 Col:81 */
    {0x6332,0xFDEA}, /* ��: GB2312 Code: 0xEAFD ==> Row:74 Col:93 */
    {0x6339,0xDADE}, /* ��: GB2312 Code: 0xDEDA ==> Row:62 Col:58 */
    {0x633A,0xA6CD}, /* ͦ: GB2312 Code: 0xCDA6 ==> Row:45 Col:06 */
    {0x633D,0xECCD}, /* ��: GB2312 Code: 0xCDEC ==> Row:45 Col:76 */
    {0x6342,0xE6CE}, /* ��: GB2312 Code: 0xCEE6 ==> Row:46 Col:70 */
    {0x6343,0xDCDE}, /* ��: GB2312 Code: 0xDEDC ==> Row:62 Col:60 */
    {0x6345,0xB1CD}, /* ͱ: GB2312 Code: 0xCDB1 ==> Row:45 Col:17 */
    {0x6346,0xA6C0}, /* ��: GB2312 Code: 0xC0A6 ==> Row:32 Col:06 */
    {0x6349,0xBDD7}, /* ׽: GB2312 Code: 0xD7BD ==> Row:55 Col:29 */
    {0x634B,0xDBDE}, /* ��: GB2312 Code: 0xDEDB ==> Row:62 Col:59 */
    {0x634C,0xC6B0}, /* ��: GB2312 Code: 0xB0C6 ==> Row:16 Col:38 */
    {0x634D,0xB4BA}, /* ��: GB2312 Code: 0xBAB4 ==> Row:26 Col:20 */
    {0x634E,0xD3C9}, /* ��: GB2312 Code: 0xC9D3 ==> Row:41 Col:51 */
    {0x634F,0xF3C4}, /* ��: GB2312 Code: 0xC4F3 ==> Row:36 Col:83 */
    {0x6350,0xE8BE}, /* ��: GB2312 Code: 0xBEE8 ==> Row:30 Col:72 */
    {0x6355,0xB6B2}, /* ��: GB2312 Code: 0xB2B6 ==> Row:18 Col:22 */
    {0x635E,0xCCC0}, /* ��: GB2312 Code: 0xC0CC ==> Row:32 Col:44 */
    {0x635F,0xF0CB}, /* ��: GB2312 Code: 0xCBF0 ==> Row:43 Col:80 */
    {0x6361,0xF1BC}, /* ��: GB2312 Code: 0xBCF1 ==> Row:28 Col:81 */
    {0x6362,0xBBBB}, /* ��: GB2312 Code: 0xBBBB ==> Row:27 Col:27 */
    {0x6363,0xB7B5}, /* ��: GB2312 Code: 0xB5B7 ==> Row:21 Col:23 */
    {0x6367,0xF5C5}, /* ��: GB2312 Code: 0xC5F5 ==> Row:37 Col:85 */
    {0x6369,0xE6DE}, /* ��: GB2312 Code: 0xDEE6 ==> Row:62 Col:70 */
    {0x636D,0xE3DE}, /* ��: GB2312 Code: 0xDEE3 ==> Row:62 Col:67 */
    {0x636E,0xDDBE}, /* ��: GB2312 Code: 0xBEDD ==> Row:30 Col:61 */
    {0x6371,0xDFDE}, /* ��: GB2312 Code: 0xDEDF ==> Row:62 Col:63 */
    {0x6376,0xB7B4}, /* ��: GB2312 Code: 0xB4B7 ==> Row:20 Col:23 */
    {0x6377,0xDDBD}, /* ��: GB2312 Code: 0xBDDD ==> Row:29 Col:61 */
    {0x637A,0xE0DE}, /* ��: GB2312 Code: 0xDEE0 ==> Row:62 Col:64 */
    {0x637B,0xEDC4}, /* ��: GB2312 Code: 0xC4ED ==> Row:36 Col:77 */
    {0x6380,0xC6CF}, /* ��: GB2312 Code: 0xCFC6 ==> Row:47 Col:38 */
    {0x6382,0xE0B5}, /* ��: GB2312 Code: 0xB5E0 ==> Row:21 Col:64 */
    {0x6387,0xDEB6}, /* ��: GB2312 Code: 0xB6DE ==> Row:22 Col:62 */
    {0x6388,0xDACA}, /* ��: GB2312 Code: 0xCADA ==> Row:42 Col:58 */
    {0x6389,0xF4B5}, /* ��: GB2312 Code: 0xB5F4 ==> Row:21 Col:84 */
    {0x638A,0xE5DE}, /* ��: GB2312 Code: 0xDEE5 ==> Row:62 Col:69 */
    {0x638C,0xC6D5}, /* ��: GB2312 Code: 0xD5C6 ==> Row:53 Col:38 */
    {0x638E,0xE1DE}, /* ��: GB2312 Code: 0xDEE1 ==> Row:62 Col:65 */
    {0x638F,0xCDCC}, /* ��: GB2312 Code: 0xCCCD ==> Row:44 Col:45 */
    {0x6390,0xFEC6}, /* ��: GB2312 Code: 0xC6FE ==> Row:38 Col:94 */
    {0x6392,0xC5C5}, /* ��: GB2312 Code: 0xC5C5 ==> Row:37 Col:37 */
    {0x6396,0xB4D2}, /* Ҵ: GB2312 Code: 0xD2B4 ==> Row:50 Col:20 */
    {0x6398,0xF2BE}, /* ��: GB2312 Code: 0xBEF2 ==> Row:30 Col:82 */
    {0x63A0,0xD3C2}, /* ��: GB2312 Code: 0xC2D3 ==> Row:34 Col:51 */
    {0x63A2,0xBDCC}, /* ̽: GB2312 Code: 0xCCBD ==> Row:44 Col:29 */
    {0x63A3,0xB8B3}, /* ��: GB2312 Code: 0xB3B8 ==> Row:19 Col:24 */
    {0x63A5,0xD3BD}, /* ��: GB2312 Code: 0xBDD3 ==> Row:29 Col:51 */
    {0x63A7,0xD8BF}, /* ��: GB2312 Code: 0xBFD8 ==> Row:31 Col:56 */
    {0x63A8,0xC6CD}, /* ��: GB2312 Code: 0xCDC6 ==> Row:45 Col:38 */
    {0x63A9,0xDAD1}, /* ��: GB2312 Code: 0xD1DA ==> Row:49 Col:58 */
    {0x63AA,0xEBB4}, /* ��: GB2312 Code: 0xB4EB ==> Row:20 Col:75 */
    {0x63AC,0xE4DE}, /* ��: GB2312 Code: 0xDEE4 ==> Row:62 Col:68 */
    {0x63AD,0xDDDE}, /* ��: GB2312 Code: 0xDEDD ==> Row:62 Col:61 */
    {0x63AE,0xE7DE}, /* ��: GB2312 Code: 0xDEE7 ==> Row:62 Col:71 */
    {0x63B0,0xFEEA}, /* ��: GB2312 Code: 0xEAFE ==> Row:74 Col:94 */
    {0x63B3,0xB0C2}, /* °: GB2312 Code: 0xC2B0 ==> Row:34 Col:16 */
    {0x63B4,0xE2DE}, /* ��: GB2312 Code: 0xDEE2 ==> Row:62 Col:66 */
    {0x63B7,0xC0D6}, /* ��: GB2312 Code: 0xD6C0 ==> Row:54 Col:32 */
    {0x63B8,0xA7B5}, /* ��: GB2312 Code: 0xB5A7 ==> Row:21 Col:07 */
    {0x63BA,0xF4B2}, /* ��: GB2312 Code: 0xB2F4 ==> Row:18 Col:84 */
    {0x63BC,0xE8DE}, /* ��: GB2312 Code: 0xDEE8 ==> Row:62 Col:72 */
    {0x63BE,0xF2DE}, /* ��: GB2312 Code: 0xDEF2 ==> Row:62 Col:82 */
    {0x63C4,0xEDDE}, /* ��: GB2312 Code: 0xDEED ==> Row:62 Col:77 */
    {0x63C6,0xF1DE}, /* ��: GB2312 Code: 0xDEF1 ==> Row:62 Col:81 */
    {0x63C9,0xE0C8}, /* ��: GB2312 Code: 0xC8E0 ==> Row:40 Col:64 */
    {0x63CD,0xE1D7}, /* ��: GB2312 Code: 0xD7E1 ==> Row:55 Col:65 */
    {0x63CE,0xEFDE}, /* ��: GB2312 Code: 0xDEEF ==> Row:62 Col:79 */
    {0x63CF,0xE8C3}, /* ��: GB2312 Code: 0xC3E8 ==> Row:35 Col:72 */
    {0x63D0,0xE1CC}, /* ��: GB2312 Code: 0xCCE1 ==> Row:44 Col:65 */
    {0x63D2,0xE5B2}, /* ��: GB2312 Code: 0xB2E5 ==> Row:18 Col:69 */
    {0x63D6,0xBED2}, /* Ҿ: GB2312 Code: 0xD2BE ==> Row:50 Col:30 */
    {0x63DE,0xEEDE}, /* ��: GB2312 Code: 0xDEEE ==> Row:62 Col:78 */
    {0x63E0,0xEBDE}, /* ��: GB2312 Code: 0xDEEB ==> Row:62 Col:75 */
    {0x63E1,0xD5CE}, /* ��: GB2312 Code: 0xCED5 ==> Row:46 Col:53 */
    {0x63E3,0xA7B4}, /* ��: GB2312 Code: 0xB4A7 ==> Row:20 Col:07 */
    {0x63E9,0xABBF}, /* ��: GB2312 Code: 0xBFAB ==> Row:31 Col:11 */
    {0x63EA,0xBEBE}, /* ��: GB2312 Code: 0xBEBE ==> Row:30 Col:30 */
    {0x63ED,0xD2BD}, /* ��: GB2312 Code: 0xBDD2 ==> Row:29 Col:50 */
    {0x63F2,0xE9DE}, /* ��: GB2312 Code: 0xDEE9 ==> Row:62 Col:73 */
    {0x63F4,0xAED4}, /* Ԯ: GB2312 Code: 0xD4AE ==> Row:52 Col:14 */
    {0x63F6,0xDEDE}, /* ��: GB2312 Code: 0xDEDE ==> Row:62 Col:62 */
    {0x63F8,0xEADE}, /* ��: GB2312 Code: 0xDEEA ==> Row:62 Col:74 */
    {0x63FD,0xBFC0}, /* ��: GB2312 Code: 0xC0BF ==> Row:32 Col:31 */
    {0x63FF,0xECDE}, /* ��: GB2312 Code: 0xDEEC ==> Row:62 Col:76 */
    {0x6400,0xF3B2}, /* ��: GB2312 Code: 0xB2F3 ==> Row:18 Col:83 */
    {0x6401,0xE9B8}, /* ��: GB2312 Code: 0xB8E9 ==> Row:24 Col:73 */
    {0x6402,0xA7C2}, /* §: GB2312 Code: 0xC2A7 ==> Row:34 Col:07 */
    {0x6405,0xC1BD}, /* ��: GB2312 Code: 0xBDC1 ==> Row:29 Col:33 */
    {0x640B,0xF5DE}, /* ��: GB2312 Code: 0xDEF5 ==> Row:62 Col:85 */
    {0x640C,0xF8DE}, /* ��: GB2312 Code: 0xDEF8 ==> Row:62 Col:88 */
    {0x640F,0xABB2}, /* ��: GB2312 Code: 0xB2AB ==> Row:18 Col:11 */
    {0x6410,0xA4B4}, /* ��: GB2312 Code: 0xB4A4 ==> Row:20 Col:04 */
    {0x6413,0xEAB4}, /* ��: GB2312 Code: 0xB4EA ==> Row:20 Col:74 */
    {0x6414,0xA6C9}, /* ɦ: GB2312 Code: 0xC9A6 ==> Row:41 Col:06 */
    {0x641B,0xF6DE}, /* ��: GB2312 Code: 0xDEF6 ==> Row:62 Col:86 */
    {0x641C,0xD1CB}, /* ��: GB2312 Code: 0xCBD1 ==> Row:43 Col:49 */
    {0x641E,0xE3B8}, /* ��: GB2312 Code: 0xB8E3 ==> Row:24 Col:67 */
    {0x6420,0xF7DE}, /* ��: GB2312 Code: 0xDEF7 ==> Row:62 Col:87 */
    {0x6421,0xFADE}, /* ��: GB2312 Code: 0xDEFA ==> Row:62 Col:90 */
    {0x6426,0xF9DE}, /* ��: GB2312 Code: 0xDEF9 ==> Row:62 Col:89 */
    {0x642A,0xC2CC}, /* ��: GB2312 Code: 0xCCC2 ==> Row:44 Col:34 */
    {0x642C,0xE1B0}, /* ��: GB2312 Code: 0xB0E1 ==> Row:16 Col:65 */
    {0x642D,0xEEB4}, /* ��: GB2312 Code: 0xB4EE ==> Row:20 Col:78 */
    {0x6434,0xBAE5}, /* �: GB2312 Code: 0xE5BA ==> Row:69 Col:26 */
    {0x643A,0xAFD0}, /* Я: GB2312 Code: 0xD0AF ==> Row:48 Col:15 */
    {0x643D,0xEBB2}, /* ��: GB2312 Code: 0xB2EB ==> Row:18 Col:75 */
    {0x643F,0xA1EB}, /* �: GB2312 Code: 0xEBA1 ==> Row:75 Col:01 */
    {0x6441,0xF4DE}, /* ��: GB2312 Code: 0xDEF4 ==> Row:62 Col:84 */
    {0x6444,0xE3C9}, /* ��: GB2312 Code: 0xC9E3 ==> Row:41 Col:67 */
    {0x6445,0xF3DE}, /* ��: GB2312 Code: 0xDEF3 ==> Row:62 Col:83 */
    {0x6446,0xDAB0}, /* ��: GB2312 Code: 0xB0DA ==> Row:16 Col:58 */
    {0x6447,0xA1D2}, /* ҡ: GB2312 Code: 0xD2A1 ==> Row:50 Col:01 */
    {0x6448,0xF7B1}, /* ��: GB2312 Code: 0xB1F7 ==> Row:17 Col:87 */
    {0x644A,0xAFCC}, /* ̯: GB2312 Code: 0xCCAF ==> Row:44 Col:15 */
    {0x6452,0xF0DE}, /* ��: GB2312 Code: 0xDEF0 ==> Row:62 Col:80 */
    {0x6454,0xA4CB}, /* ˤ: GB2312 Code: 0xCBA4 ==> Row:43 Col:04 */
    {0x6458,0xAAD5}, /* ժ: GB2312 Code: 0xD5AA ==> Row:53 Col:10 */
    {0x645E,0xFBDE}, /* ��: GB2312 Code: 0xDEFB ==> Row:62 Col:91 */
    {0x6467,0xDDB4}, /* ��: GB2312 Code: 0xB4DD ==> Row:20 Col:61 */
    {0x6469,0xA6C4}, /* Ħ: GB2312 Code: 0xC4A6 ==> Row:36 Col:06 */
    {0x646D,0xFDDE}, /* ��: GB2312 Code: 0xDEFD ==> Row:62 Col:93 */
    {0x6478,0xFEC3}, /* ��: GB2312 Code: 0xC3FE ==> Row:35 Col:94 */
    {0x6479,0xA1C4}, /* ġ: GB2312 Code: 0xC4A1 ==> Row:36 Col:01 */
    {0x647A,0xA1DF}, /* ߡ: GB2312 Code: 0xDFA1 ==> Row:63 Col:01 */
    {0x6482,0xCCC1}, /* ��: GB2312 Code: 0xC1CC ==> Row:33 Col:44 */
    {0x6484,0xFCDE}, /* ��: GB2312 Code: 0xDEFC ==> Row:62 Col:92 */
    {0x6485,0xEFBE}, /* ��: GB2312 Code: 0xBEEF ==> Row:30 Col:79 */
    {0x6487,0xB2C6}, /* Ʋ: GB2312 Code: 0xC6B2 ==> Row:38 Col:18 */
    {0x6491,0xC5B3}, /* ��: GB2312 Code: 0xB3C5 ==> Row:19 Col:37 */
    {0x6492,0xF6C8}, /* ��: GB2312 Code: 0xC8F6 ==> Row:40 Col:86 */
    {0x6495,0xBACB}, /* ˺: GB2312 Code: 0xCBBA ==> Row:43 Col:26 */
    {0x6496,0xFEDE}, /* ��: GB2312 Code: 0xDEFE ==> Row:62 Col:94 */
    {0x6499,0xA4DF}, /* ߤ: GB2312 Code: 0xDFA4 ==> Row:63 Col:04 */
    {0x649E,0xB2D7}, /* ײ: GB2312 Code: 0xD7B2 ==> Row:55 Col:18 */
    {0x64A4,0xB7B3}, /* ��: GB2312 Code: 0xB3B7 ==> Row:19 Col:23 */
    {0x64A9,0xC3C1}, /* ��: GB2312 Code: 0xC1C3 ==> Row:33 Col:35 */
    {0x64AC,0xCBC7}, /* ��: GB2312 Code: 0xC7CB ==> Row:39 Col:43 */
    {0x64AD,0xA5B2}, /* ��: GB2312 Code: 0xB2A5 ==> Row:18 Col:05 */
    {0x64AE,0xE9B4}, /* ��: GB2312 Code: 0xB4E9 ==> Row:20 Col:73 */
    {0x64B0,0xABD7}, /* ׫: GB2312 Code: 0xD7AB ==> Row:55 Col:11 */
    {0x64B5,0xECC4}, /* ��: GB2312 Code: 0xC4EC ==> Row:36 Col:76 */
    {0x64B7,0xA2DF}, /* ߢ: GB2312 Code: 0xDFA2 ==> Row:63 Col:02 */
    {0x64B8,0xA3DF}, /* ߣ: GB2312 Code: 0xDFA3 ==> Row:63 Col:03 */
    {0x64BA,0xA5DF}, /* ߥ: GB2312 Code: 0xDFA5 ==> Row:63 Col:05 */
    {0x64BC,0xB3BA}, /* ��: GB2312 Code: 0xBAB3 ==> Row:26 Col:19 */
    {0x64C0,0xA6DF}, /* ߦ: GB2312 Code: 0xDFA6 ==> Row:63 Col:06 */
    {0x64C2,0xDEC0}, /* ��: GB2312 Code: 0xC0DE ==> Row:32 Col:62 */
    {0x64C5,0xC3C9}, /* ��: GB2312 Code: 0xC9C3 ==> Row:41 Col:35 */
    {0x64CD,0xD9B2}, /* ��: GB2312 Code: 0xB2D9 ==> Row:18 Col:57 */
    {0x64CE,0xE6C7}, /* ��: GB2312 Code: 0xC7E6 ==> Row:39 Col:70 */
    {0x64D0,0xA7DF}, /* ߧ: GB2312 Code: 0xDFA7 ==> Row:63 Col:07 */
    {0x64D2,0xDCC7}, /* ��: GB2312 Code: 0xC7DC ==> Row:39 Col:60 */
    {0x64D7,0xA8DF}, /* ߨ: GB2312 Code: 0xDFA8 ==> Row:63 Col:08 */
    {0x64D8,0xA2EB}, /* �: GB2312 Code: 0xEBA2 ==> Row:75 Col:02 */
    {0x64DE,0xD3CB}, /* ��: GB2312 Code: 0xCBD3 ==> Row:43 Col:51 */
    {0x64E2,0xAADF}, /* ߪ: GB2312 Code: 0xDFAA ==> Row:63 Col:10 */
    {0x64E4,0xA9DF}, /* ߩ: GB2312 Code: 0xDFA9 ==> Row:63 Col:09 */
    {0x64E6,0xC1B2}, /* ��: GB2312 Code: 0xB2C1 ==> Row:18 Col:33 */
    {0x6500,0xCAC5}, /* ��: GB2312 Code: 0xC5CA ==> Row:37 Col:42 */
    {0x6509,0xABDF}, /* ߫: GB2312 Code: 0xDFAB ==> Row:63 Col:11 */
    {0x6512,0xDCD4}, /* ��: GB2312 Code: 0xD4DC ==> Row:52 Col:60 */
    {0x6518,0xC1C8}, /* ��: GB2312 Code: 0xC8C1 ==> Row:40 Col:33 */
    {0x6525,0xACDF}, /* ߬: GB2312 Code: 0xDFAC ==> Row:63 Col:12 */
    {0x652B,0xF0BE}, /* ��: GB2312 Code: 0xBEF0 ==> Row:30 Col:80 */
    {0x652E,0xADDF}, /* ߭: GB2312 Code: 0xDFAD ==> Row:63 Col:13 */
    {0x652F,0xA7D6}, /* ֧: GB2312 Code: 0xD6A7 ==> Row:54 Col:07 */
    {0x6534,0xB7EA}, /* �: GB2312 Code: 0xEAB7 ==> Row:74 Col:23 */
    {0x6535,0xB6EB}, /* �: GB2312 Code: 0xEBB6 ==> Row:75 Col:22 */
    {0x6536,0xD5CA}, /* ��: GB2312 Code: 0xCAD5 ==> Row:42 Col:53 */
    {0x6538,0xFCD8}, /* ��: GB2312 Code: 0xD8FC ==> Row:56 Col:92 */
    {0x6539,0xC4B8}, /* ��: GB2312 Code: 0xB8C4 ==> Row:24 Col:36 */
    {0x653B,0xA5B9}, /* ��: GB2312 Code: 0xB9A5 ==> Row:25 Col:05 */
    {0x653E,0xC5B7}, /* ��: GB2312 Code: 0xB7C5 ==> Row:23 Col:37 */
    {0x653F,0xFED5}, /* ��: GB2312 Code: 0xD5FE ==> Row:53 Col:94 */
    {0x6545,0xCAB9}, /* ��: GB2312 Code: 0xB9CA ==> Row:25 Col:42 */
    {0x6548,0xA7D0}, /* Ч: GB2312 Code: 0xD0A7 ==> Row:48 Col:07 */
    {0x6549,0xCDF4}, /* ��: GB2312 Code: 0xF4CD ==> Row:84 Col:45 */
    {0x654C,0xD0B5}, /* ��: GB2312 Code: 0xB5D0 ==> Row:21 Col:48 */
    {0x654F,0xF4C3}, /* ��: GB2312 Code: 0xC3F4 ==> Row:35 Col:84 */
    {0x6551,0xC8BE}, /* ��: GB2312 Code: 0xBEC8 ==> Row:30 Col:40 */
    {0x6555,0xB7EB}, /* �: GB2312 Code: 0xEBB7 ==> Row:75 Col:23 */
    {0x6556,0xBDB0}, /* ��: GB2312 Code: 0xB0BD ==> Row:16 Col:29 */
    {0x6559,0xCCBD}, /* ��: GB2312 Code: 0xBDCC ==> Row:29 Col:44 */
    {0x655B,0xB2C1}, /* ��: GB2312 Code: 0xC1B2 ==> Row:33 Col:18 */
    {0x655D,0xD6B1}, /* ��: GB2312 Code: 0xB1D6 ==> Row:17 Col:54 */
    {0x655E,0xA8B3}, /* ��: GB2312 Code: 0xB3A8 ==> Row:19 Col:08 */
    {0x6562,0xD2B8}, /* ��: GB2312 Code: 0xB8D2 ==> Row:24 Col:50 */
    {0x6563,0xA2C9}, /* ɢ: GB2312 Code: 0xC9A2 ==> Row:41 Col:02 */
    {0x6566,0xD8B6}, /* ��: GB2312 Code: 0xB6D8 ==> Row:22 Col:56 */
    {0x656B,0xB8EB}, /* �: GB2312 Code: 0xEBB8 ==> Row:75 Col:24 */
    {0x656C,0xB4BE}, /* ��: GB2312 Code: 0xBEB4 ==> Row:30 Col:20 */
    {0x6570,0xFDCA}, /* ��: GB2312 Code: 0xCAFD ==> Row:42 Col:93 */
    {0x6572,0xC3C7}, /* ��: GB2312 Code: 0xC7C3 ==> Row:39 Col:35 */
    {0x6574,0xFBD5}, /* ��: GB2312 Code: 0xD5FB ==> Row:53 Col:91 */
    {0x6577,0xF3B7}, /* ��: GB2312 Code: 0xB7F3 ==> Row:23 Col:83 */
    {0x6587,0xC4CE}, /* ��: GB2312 Code: 0xCEC4 ==> Row:46 Col:36 */
    {0x658B,0xABD5}, /* ի: GB2312 Code: 0xD5AB ==> Row:53 Col:11 */
    {0x658C,0xF3B1}, /* ��: GB2312 Code: 0xB1F3 ==> Row:17 Col:83 */
    {0x6590,0xB3EC}, /* �: GB2312 Code: 0xECB3 ==> Row:76 Col:19 */
    {0x6591,0xDFB0}, /* ��: GB2312 Code: 0xB0DF ==> Row:16 Col:63 */
    {0x6593,0xB5EC}, /* �: GB2312 Code: 0xECB5 ==> Row:76 Col:21 */
    {0x6597,0xB7B6}, /* ��: GB2312 Code: 0xB6B7 ==> Row:22 Col:23 */
    {0x6599,0xCFC1}, /* ��: GB2312 Code: 0xC1CF ==> Row:33 Col:47 */
    {0x659B,0xFAF5}, /* ��: GB2312 Code: 0xF5FA ==> Row:85 Col:90 */
    {0x659C,0xB1D0}, /* б: GB2312 Code: 0xD0B1 ==> Row:48 Col:17 */
    {0x659F,0xE5D5}, /* ��: GB2312 Code: 0xD5E5 ==> Row:53 Col:69 */
    {0x65A1,0xD3CE}, /* ��: GB2312 Code: 0xCED3 ==> Row:46 Col:51 */
    {0x65A4,0xEFBD}, /* ��: GB2312 Code: 0xBDEF ==> Row:29 Col:79 */
    {0x65A5,0xE2B3}, /* ��: GB2312 Code: 0xB3E2 ==> Row:19 Col:66 */
    {0x65A7,0xABB8}, /* ��: GB2312 Code: 0xB8AB ==> Row:24 Col:11 */
    {0x65A9,0xB6D5}, /* ն: GB2312 Code: 0xD5B6 ==> Row:53 Col:22 */
    {0x65AB,0xBDED}, /* ��: GB2312 Code: 0xEDBD ==> Row:77 Col:29 */
    {0x65AD,0xCFB6}, /* ��: GB2312 Code: 0xB6CF ==> Row:22 Col:47 */
    {0x65AF,0xB9CB}, /* ˹: GB2312 Code: 0xCBB9 ==> Row:43 Col:25 */
    {0x65B0,0xC2D0}, /* ��: GB2312 Code: 0xD0C2 ==> Row:48 Col:34 */
    {0x65B9,0xBDB7}, /* ��: GB2312 Code: 0xB7BD ==> Row:23 Col:29 */
    {0x65BC,0xB6EC}, /* �: GB2312 Code: 0xECB6 ==> Row:76 Col:22 */
    {0x65BD,0xA9CA}, /* ʩ: GB2312 Code: 0xCAA9 ==> Row:42 Col:09 */
    {0x65C1,0xD4C5}, /* ��: GB2312 Code: 0xC5D4 ==> Row:37 Col:52 */
    {0x65C3,0xB9EC}, /* �: GB2312 Code: 0xECB9 ==> Row:76 Col:25 */
    {0x65C4,0xB8EC}, /* �: GB2312 Code: 0xECB8 ==> Row:76 Col:24 */
    {0x65C5,0xC3C2}, /* ��: GB2312 Code: 0xC2C3 ==> Row:34 Col:35 */
    {0x65C6,0xB7EC}, /* �: GB2312 Code: 0xECB7 ==> Row:76 Col:23 */
    {0x65CB,0xFDD0}, /* ��: GB2312 Code: 0xD0FD ==> Row:48 Col:93 */
    {0x65CC,0xBAEC}, /* �: GB2312 Code: 0xECBA ==> Row:76 Col:26 */
    {0x65CE,0xBBEC}, /* �: GB2312 Code: 0xECBB ==> Row:76 Col:27 */
    {0x65CF,0xE5D7}, /* ��: GB2312 Code: 0xD7E5 ==> Row:55 Col:69 */
    {0x65D2,0xBCEC}, /* �: GB2312 Code: 0xECBC ==> Row:76 Col:28 */
    {0x65D6,0xBDEC}, /* �: GB2312 Code: 0xECBD ==> Row:76 Col:29 */
    {0x65D7,0xECC6}, /* ��: GB2312 Code: 0xC6EC ==> Row:38 Col:76 */
    {0x65E0,0xDECE}, /* ��: GB2312 Code: 0xCEDE ==> Row:46 Col:62 */
    {0x65E2,0xC8BC}, /* ��: GB2312 Code: 0xBCC8 ==> Row:28 Col:40 */
    {0x65E5,0xD5C8}, /* ��: GB2312 Code: 0xC8D5 ==> Row:40 Col:53 */
    {0x65E6,0xA9B5}, /* ��: GB2312 Code: 0xB5A9 ==> Row:21 Col:09 */
    {0x65E7,0xC9BE}, /* ��: GB2312 Code: 0xBEC9 ==> Row:30 Col:41 */
    {0x65E8,0xBCD6}, /* ּ: GB2312 Code: 0xD6BC ==> Row:54 Col:28 */
    {0x65E9,0xE7D4}, /* ��: GB2312 Code: 0xD4E7 ==> Row:52 Col:71 */
    {0x65EC,0xAED1}, /* Ѯ: GB2312 Code: 0xD1AE ==> Row:49 Col:14 */
    {0x65ED,0xF1D0}, /* ��: GB2312 Code: 0xD0F1 ==> Row:48 Col:81 */
    {0x65EE,0xB8EA}, /* �: GB2312 Code: 0xEAB8 ==> Row:74 Col:24 */
    {0x65EF,0xB9EA}, /* �: GB2312 Code: 0xEAB9 ==> Row:74 Col:25 */
    {0x65F0,0xBAEA}, /* �: GB2312 Code: 0xEABA ==> Row:74 Col:26 */
    {0x65F1,0xB5BA}, /* ��: GB2312 Code: 0xBAB5 ==> Row:26 Col:21 */
    {0x65F6,0xB1CA}, /* ʱ: GB2312 Code: 0xCAB1 ==> Row:42 Col:17 */
    {0x65F7,0xF5BF}, /* ��: GB2312 Code: 0xBFF5 ==> Row:31 Col:85 */
    {0x65FA,0xFACD}, /* ��: GB2312 Code: 0xCDFA ==> Row:45 Col:90 */
    {0x6600,0xC0EA}, /* ��: GB2312 Code: 0xEAC0 ==> Row:74 Col:32 */
    {0x6602,0xBAB0}, /* ��: GB2312 Code: 0xB0BA ==> Row:16 Col:26 */
    {0x6603,0xBEEA}, /* �: GB2312 Code: 0xEABE ==> Row:74 Col:30 */
    {0x6606,0xA5C0}, /* ��: GB2312 Code: 0xC0A5 ==> Row:32 Col:05 */
    {0x660A,0xBBEA}, /* �: GB2312 Code: 0xEABB ==> Row:74 Col:27 */
    {0x660C,0xFDB2}, /* ��: GB2312 Code: 0xB2FD ==> Row:18 Col:93 */
    {0x660E,0xF7C3}, /* ��: GB2312 Code: 0xC3F7 ==> Row:35 Col:87 */
    {0x660F,0xE8BB}, /* ��: GB2312 Code: 0xBBE8 ==> Row:27 Col:72 */
    {0x6613,0xD7D2}, /* ��: GB2312 Code: 0xD2D7 ==> Row:50 Col:55 */
    {0x6614,0xF4CE}, /* ��: GB2312 Code: 0xCEF4 ==> Row:46 Col:84 */
    {0x6615,0xBFEA}, /* �: GB2312 Code: 0xEABF ==> Row:74 Col:31 */
    {0x6619,0xBCEA}, /* �: GB2312 Code: 0xEABC ==> Row:74 Col:28 */
    {0x661D,0xC3EA}, /* ��: GB2312 Code: 0xEAC3 ==> Row:74 Col:35 */
    {0x661F,0xC7D0}, /* ��: GB2312 Code: 0xD0C7 ==> Row:48 Col:39 */
    {0x6620,0xB3D3}, /* ӳ: GB2312 Code: 0xD3B3 ==> Row:51 Col:19 */
    {0x6625,0xBAB4}, /* ��: GB2312 Code: 0xB4BA ==> Row:20 Col:26 */
    {0x6627,0xC1C3}, /* ��: GB2312 Code: 0xC3C1 ==> Row:35 Col:33 */
    {0x6628,0xF2D7}, /* ��: GB2312 Code: 0xD7F2 ==> Row:55 Col:82 */
    {0x662D,0xD1D5}, /* ��: GB2312 Code: 0xD5D1 ==> Row:53 Col:49 */
    {0x662F,0xC7CA}, /* ��: GB2312 Code: 0xCAC7 ==> Row:42 Col:39 */
    {0x6631,0xC5EA}, /* ��: GB2312 Code: 0xEAC5 ==> Row:74 Col:37 */
    {0x6634,0xC4EA}, /* ��: GB2312 Code: 0xEAC4 ==> Row:74 Col:36 */
    {0x6635,0xC7EA}, /* ��: GB2312 Code: 0xEAC7 ==> Row:74 Col:39 */
    {0x6636,0xC6EA}, /* ��: GB2312 Code: 0xEAC6 ==> Row:74 Col:38 */
    {0x663C,0xE7D6}, /* ��: GB2312 Code: 0xD6E7 ==> Row:54 Col:71 */
    {0x663E,0xD4CF}, /* ��: GB2312 Code: 0xCFD4 ==> Row:47 Col:52 */
    {0x6641,0xCBEA}, /* ��: GB2312 Code: 0xEACB ==> Row:74 Col:43 */
    {0x6643,0xCEBB}, /* ��: GB2312 Code: 0xBBCE ==> Row:27 Col:46 */
    {0x664B,0xFABD}, /* ��: GB2312 Code: 0xBDFA ==> Row:29 Col:90 */
    {0x664C,0xCEC9}, /* ��: GB2312 Code: 0xC9CE ==> Row:41 Col:46 */
    {0x664F,0xCCEA}, /* ��: GB2312 Code: 0xEACC ==> Row:74 Col:44 */
    {0x6652,0xB9C9}, /* ɹ: GB2312 Code: 0xC9B9 ==> Row:41 Col:25 */
    {0x6653,0xFECF}, /* ��: GB2312 Code: 0xCFFE ==> Row:47 Col:94 */
    {0x6654,0xCAEA}, /* ��: GB2312 Code: 0xEACA ==> Row:74 Col:42 */
    {0x6655,0xCED4}, /* ��: GB2312 Code: 0xD4CE ==> Row:52 Col:46 */
    {0x6656,0xCDEA}, /* ��: GB2312 Code: 0xEACD ==> Row:74 Col:45 */
    {0x6657,0xCFEA}, /* ��: GB2312 Code: 0xEACF ==> Row:74 Col:47 */
    {0x665A,0xEDCD}, /* ��: GB2312 Code: 0xCDED ==> Row:45 Col:77 */
    {0x665F,0xC9EA}, /* ��: GB2312 Code: 0xEAC9 ==> Row:74 Col:41 */
    {0x6661,0xCEEA}, /* ��: GB2312 Code: 0xEACE ==> Row:74 Col:46 */
    {0x6664,0xEECE}, /* ��: GB2312 Code: 0xCEEE ==> Row:46 Col:78 */
    {0x6666,0xDEBB}, /* ��: GB2312 Code: 0xBBDE ==> Row:27 Col:62 */
    {0x6668,0xBFB3}, /* ��: GB2312 Code: 0xB3BF ==> Row:19 Col:31 */
    {0x666E,0xD5C6}, /* ��: GB2312 Code: 0xC6D5 ==> Row:38 Col:53 */
    {0x666F,0xB0BE}, /* ��: GB2312 Code: 0xBEB0 ==> Row:30 Col:16 */
    {0x6670,0xFACE}, /* ��: GB2312 Code: 0xCEFA ==> Row:46 Col:90 */
    {0x6674,0xE7C7}, /* ��: GB2312 Code: 0xC7E7 ==> Row:39 Col:71 */
    {0x6676,0xA7BE}, /* ��: GB2312 Code: 0xBEA7 ==> Row:30 Col:07 */
    {0x6677,0xD0EA}, /* ��: GB2312 Code: 0xEAD0 ==> Row:74 Col:48 */
    {0x667A,0xC7D6}, /* ��: GB2312 Code: 0xD6C7 ==> Row:54 Col:39 */
    {0x667E,0xC0C1}, /* ��: GB2312 Code: 0xC1C0 ==> Row:33 Col:32 */
    {0x6682,0xDDD4}, /* ��: GB2312 Code: 0xD4DD ==> Row:52 Col:61 */
    {0x6684,0xD1EA}, /* ��: GB2312 Code: 0xEAD1 ==> Row:74 Col:49 */
    {0x6687,0xBECF}, /* Ͼ: GB2312 Code: 0xCFBE ==> Row:47 Col:30 */
    {0x668C,0xD2EA}, /* ��: GB2312 Code: 0xEAD2 ==> Row:74 Col:50 */
    {0x6691,0xEECA}, /* ��: GB2312 Code: 0xCAEE ==> Row:42 Col:78 */
    {0x6696,0xAFC5}, /* ů: GB2312 Code: 0xC5AF ==> Row:37 Col:15 */
    {0x6697,0xB5B0}, /* ��: GB2312 Code: 0xB0B5 ==> Row:16 Col:21 */
    {0x669D,0xD4EA}, /* ��: GB2312 Code: 0xEAD4 ==> Row:74 Col:52 */
    {0x66A7,0xD3EA}, /* ��: GB2312 Code: 0xEAD3 ==> Row:74 Col:51 */
    {0x66A8,0xDFF4}, /* ��: GB2312 Code: 0xF4DF ==> Row:84 Col:63 */
    {0x66AE,0xBAC4}, /* ĺ: GB2312 Code: 0xC4BA ==> Row:36 Col:26 */
    {0x66B4,0xA9B1}, /* ��: GB2312 Code: 0xB1A9 ==> Row:17 Col:09 */
    {0x66B9,0xDFE5}, /* ��: GB2312 Code: 0xE5DF ==> Row:69 Col:63 */
    {0x66BE,0xD5EA}, /* ��: GB2312 Code: 0xEAD5 ==> Row:74 Col:53 */
    {0x66D9,0xEFCA}, /* ��: GB2312 Code: 0xCAEF ==> Row:42 Col:79 */
    {0x66DB,0xD6EA}, /* ��: GB2312 Code: 0xEAD6 ==> Row:74 Col:54 */
    {0x66DC,0xD7EA}, /* ��: GB2312 Code: 0xEAD7 ==> Row:74 Col:55 */
    {0x66DD,0xD8C6}, /* ��: GB2312 Code: 0xC6D8 ==> Row:38 Col:56 */
    {0x66E6,0xD8EA}, /* ��: GB2312 Code: 0xEAD8 ==> Row:74 Col:56 */
    {0x66E9,0xD9EA}, /* ��: GB2312 Code: 0xEAD9 ==> Row:74 Col:57 */
    {0x66F0,0xBBD4}, /* Ի: GB2312 Code: 0xD4BB ==> Row:52 Col:27 */
    {0x66F2,0xFAC7}, /* ��: GB2312 Code: 0xC7FA ==> Row:39 Col:90 */
    {0x66F3,0xB7D2}, /* ҷ: GB2312 Code: 0xD2B7 ==> Row:50 Col:23 */
    {0x66F4,0xFCB8}, /* ��: GB2312 Code: 0xB8FC ==> Row:24 Col:92 */
    {0x66F7,0xC2EA}, /* ��: GB2312 Code: 0xEAC2 ==> Row:74 Col:34 */
    {0x66F9,0xDCB2}, /* ��: GB2312 Code: 0xB2DC ==> Row:18 Col:60 */
    {0x66FC,0xFCC2}, /* ��: GB2312 Code: 0xC2FC ==> Row:34 Col:92 */
    {0x66FE,0xF8D4}, /* ��: GB2312 Code: 0xD4F8 ==> Row:52 Col:88 */
    {0x66FF,0xE6CC}, /* ��: GB2312 Code: 0xCCE6 ==> Row:44 Col:70 */
    {0x6700,0xEED7}, /* ��: GB2312 Code: 0xD7EE ==> Row:55 Col:78 */
    {0x6708,0xC2D4}, /* ��: GB2312 Code: 0xD4C2 ==> Row:52 Col:34 */
    {0x6709,0xD0D3}, /* ��: GB2312 Code: 0xD3D0 ==> Row:51 Col:48 */
    {0x670A,0xC3EB}, /* ��: GB2312 Code: 0xEBC3 ==> Row:75 Col:35 */
    {0x670B,0xF3C5}, /* ��: GB2312 Code: 0xC5F3 ==> Row:37 Col:83 */
    {0x670D,0xFEB7}, /* ��: GB2312 Code: 0xB7FE ==> Row:23 Col:94 */
    {0x6710,0xD4EB}, /* ��: GB2312 Code: 0xEBD4 ==> Row:75 Col:52 */
    {0x6714,0xB7CB}, /* ˷: GB2312 Code: 0xCBB7 ==> Row:43 Col:23 */
    {0x6715,0xDEEB}, /* ��: GB2312 Code: 0xEBDE ==> Row:75 Col:62 */
    {0x6717,0xCAC0}, /* ��: GB2312 Code: 0xC0CA ==> Row:32 Col:42 */
    {0x671B,0xFBCD}, /* ��: GB2312 Code: 0xCDFB ==> Row:45 Col:91 */
    {0x671D,0xAFB3}, /* ��: GB2312 Code: 0xB3AF ==> Row:19 Col:15 */
    {0x671F,0xDAC6}, /* ��: GB2312 Code: 0xC6DA ==> Row:38 Col:58 */
    {0x6726,0xFCEB}, /* ��: GB2312 Code: 0xEBFC ==> Row:75 Col:92 */
    {0x6728,0xBEC4}, /* ľ: GB2312 Code: 0xC4BE ==> Row:36 Col:30 */
    {0x672A,0xB4CE}, /* δ: GB2312 Code: 0xCEB4 ==> Row:46 Col:20 */
    {0x672B,0xA9C4}, /* ĩ: GB2312 Code: 0xC4A9 ==> Row:36 Col:09 */
    {0x672C,0xBEB1}, /* ��: GB2312 Code: 0xB1BE ==> Row:17 Col:30 */
    {0x672D,0xFDD4}, /* ��: GB2312 Code: 0xD4FD ==> Row:52 Col:93 */
    {0x672F,0xF5CA}, /* ��: GB2312 Code: 0xCAF5 ==> Row:42 Col:85 */
    {0x6731,0xECD6}, /* ��: GB2312 Code: 0xD6EC ==> Row:54 Col:76 */
    {0x6734,0xD3C6}, /* ��: GB2312 Code: 0xC6D3 ==> Row:38 Col:51 */
    {0x6735,0xE4B6}, /* ��: GB2312 Code: 0xB6E4 ==> Row:22 Col:68 */
    {0x673A,0xFABB}, /* ��: GB2312 Code: 0xBBFA ==> Row:27 Col:90 */
    {0x673D,0xE0D0}, /* ��: GB2312 Code: 0xD0E0 ==> Row:48 Col:64 */
    {0x6740,0xB1C9}, /* ɱ: GB2312 Code: 0xC9B1 ==> Row:41 Col:17 */
    {0x6742,0xD3D4}, /* ��: GB2312 Code: 0xD4D3 ==> Row:52 Col:51 */
    {0x6743,0xA8C8}, /* Ȩ: GB2312 Code: 0xC8A8 ==> Row:40 Col:08 */
    {0x6746,0xCBB8}, /* ��: GB2312 Code: 0xB8CB ==> Row:24 Col:43 */
    {0x6748,0xBEE8}, /* �: GB2312 Code: 0xE8BE ==> Row:72 Col:30 */
    {0x6749,0xBCC9}, /* ɼ: GB2312 Code: 0xC9BC ==> Row:41 Col:28 */
    {0x674C,0xBBE8}, /* �: GB2312 Code: 0xE8BB ==> Row:72 Col:27 */
    {0x674E,0xEEC0}, /* ��: GB2312 Code: 0xC0EE ==> Row:32 Col:78 */
    {0x674F,0xD3D0}, /* ��: GB2312 Code: 0xD0D3 ==> Row:48 Col:51 */
    {0x6750,0xC4B2}, /* ��: GB2312 Code: 0xB2C4 ==> Row:18 Col:36 */
    {0x6751,0xE5B4}, /* ��: GB2312 Code: 0xB4E5 ==> Row:20 Col:69 */
    {0x6753,0xBCE8}, /* �: GB2312 Code: 0xE8BC ==> Row:72 Col:28 */
    {0x6756,0xC8D5}, /* ��: GB2312 Code: 0xD5C8 ==> Row:53 Col:40 */
    {0x675C,0xC5B6}, /* ��: GB2312 Code: 0xB6C5 ==> Row:22 Col:37 */
    {0x675E,0xBDE8}, /* �: GB2312 Code: 0xE8BD ==> Row:72 Col:29 */
    {0x675F,0xF8CA}, /* ��: GB2312 Code: 0xCAF8 ==> Row:42 Col:88 */
    {0x6760,0xDCB8}, /* ��: GB2312 Code: 0xB8DC ==> Row:24 Col:60 */
    {0x6761,0xF5CC}, /* ��: GB2312 Code: 0xCCF5 ==> Row:44 Col:85 */
    {0x6765,0xB4C0}, /* ��: GB2312 Code: 0xC0B4 ==> Row:32 Col:20 */
    {0x6768,0xEED1}, /* ��: GB2312 Code: 0xD1EE ==> Row:49 Col:78 */
    {0x6769,0xBFE8}, /* �: GB2312 Code: 0xE8BF ==> Row:72 Col:31 */
    {0x676A,0xC2E8}, /* ��: GB2312 Code: 0xE8C2 ==> Row:72 Col:34 */
    {0x676D,0xBCBA}, /* ��: GB2312 Code: 0xBABC ==> Row:26 Col:28 */
    {0x676F,0xADB1}, /* ��: GB2312 Code: 0xB1AD ==> Row:17 Col:13 */
    {0x6770,0xDCBD}, /* ��: GB2312 Code: 0xBDDC ==> Row:29 Col:60 */
    {0x6772,0xBDEA}, /* �: GB2312 Code: 0xEABD ==> Row:74 Col:29 */
    {0x6773,0xC3E8}, /* ��: GB2312 Code: 0xE8C3 ==> Row:72 Col:35 */
    {0x6775,0xC6E8}, /* ��: GB2312 Code: 0xE8C6 ==> Row:72 Col:38 */
    {0x6777,0xCBE8}, /* ��: GB2312 Code: 0xE8CB ==> Row:72 Col:43 */
    {0x677C,0xCCE8}, /* ��: GB2312 Code: 0xE8CC ==> Row:72 Col:44 */
    {0x677E,0xC9CB}, /* ��: GB2312 Code: 0xCBC9 ==> Row:43 Col:41 */
    {0x677F,0xE5B0}, /* ��: GB2312 Code: 0xB0E5 ==> Row:16 Col:69 */
    {0x6781,0xABBC}, /* ��: GB2312 Code: 0xBCAB ==> Row:28 Col:11 */
    {0x6784,0xB9B9}, /* ��: GB2312 Code: 0xB9B9 ==> Row:25 Col:25 */
    {0x6787,0xC1E8}, /* ��: GB2312 Code: 0xE8C1 ==> Row:72 Col:33 */
    {0x6789,0xF7CD}, /* ��: GB2312 Code: 0xCDF7 ==> Row:45 Col:87 */
    {0x678B,0xCAE8}, /* ��: GB2312 Code: 0xE8CA ==> Row:72 Col:42 */
    {0x6790,0xF6CE}, /* ��: GB2312 Code: 0xCEF6 ==> Row:46 Col:86 */
    {0x6795,0xEDD5}, /* ��: GB2312 Code: 0xD5ED ==> Row:53 Col:77 */
    {0x6797,0xD6C1}, /* ��: GB2312 Code: 0xC1D6 ==> Row:33 Col:54 */
    {0x6798,0xC4E8}, /* ��: GB2312 Code: 0xE8C4 ==> Row:72 Col:36 */
    {0x679A,0xB6C3}, /* ö: GB2312 Code: 0xC3B6 ==> Row:35 Col:22 */
    {0x679C,0xFBB9}, /* ��: GB2312 Code: 0xB9FB ==> Row:25 Col:91 */
    {0x679D,0xA6D6}, /* ֦: GB2312 Code: 0xD6A6 ==> Row:54 Col:06 */
    {0x679E,0xC8E8}, /* ��: GB2312 Code: 0xE8C8 ==> Row:72 Col:40 */
    {0x67A2,0xE0CA}, /* ��: GB2312 Code: 0xCAE0 ==> Row:42 Col:64 */
    {0x67A3,0xE6D4}, /* ��: GB2312 Code: 0xD4E6 ==> Row:52 Col:70 */
    {0x67A5,0xC0E8}, /* ��: GB2312 Code: 0xE8C0 ==> Row:72 Col:32 */
    {0x67A7,0xC5E8}, /* ��: GB2312 Code: 0xE8C5 ==> Row:72 Col:37 */
    {0x67A8,0xC7E8}, /* ��: GB2312 Code: 0xE8C7 ==> Row:72 Col:39 */
    {0x67AA,0xB9C7}, /* ǹ: GB2312 Code: 0xC7B9 ==> Row:39 Col:25 */
    {0x67AB,0xE3B7}, /* ��: GB2312 Code: 0xB7E3 ==> Row:23 Col:67 */
    {0x67AD,0xC9E8}, /* ��: GB2312 Code: 0xE8C9 ==> Row:72 Col:41 */
    {0x67AF,0xDDBF}, /* ��: GB2312 Code: 0xBFDD ==> Row:31 Col:61 */
    {0x67B0,0xD2E8}, /* ��: GB2312 Code: 0xE8D2 ==> Row:72 Col:50 */
    {0x67B3,0xD7E8}, /* ��: GB2312 Code: 0xE8D7 ==> Row:72 Col:55 */
    {0x67B5,0xD5E8}, /* ��: GB2312 Code: 0xE8D5 ==> Row:72 Col:53 */
    {0x67B6,0xDCBC}, /* ��: GB2312 Code: 0xBCDC ==> Row:28 Col:60 */
    {0x67B7,0xCFBC}, /* ��: GB2312 Code: 0xBCCF ==> Row:28 Col:47 */
    {0x67B8,0xDBE8}, /* ��: GB2312 Code: 0xE8DB ==> Row:72 Col:59 */
    {0x67C1,0xDEE8}, /* ��: GB2312 Code: 0xE8DE ==> Row:72 Col:62 */
    {0x67C3,0xDAE8}, /* ��: GB2312 Code: 0xE8DA ==> Row:72 Col:58 */
    {0x67C4,0xFAB1}, /* ��: GB2312 Code: 0xB1FA ==> Row:17 Col:90 */
    {0x67CF,0xD8B0}, /* ��: GB2312 Code: 0xB0D8 ==> Row:16 Col:56 */
    {0x67D0,0xB3C4}, /* ĳ: GB2312 Code: 0xC4B3 ==> Row:36 Col:19 */
    {0x67D1,0xCCB8}, /* ��: GB2312 Code: 0xB8CC ==> Row:24 Col:44 */
    {0x67D2,0xE2C6}, /* ��: GB2312 Code: 0xC6E2 ==> Row:38 Col:66 */
    {0x67D3,0xBEC8}, /* Ⱦ: GB2312 Code: 0xC8BE ==> Row:40 Col:30 */
    {0x67D4,0xE1C8}, /* ��: GB2312 Code: 0xC8E1 ==> Row:40 Col:65 */
    {0x67D8,0xCFE8}, /* ��: GB2312 Code: 0xE8CF ==> Row:72 Col:47 */
    {0x67D9,0xD4E8}, /* ��: GB2312 Code: 0xE8D4 ==> Row:72 Col:52 */
    {0x67DA,0xD6E8}, /* ��: GB2312 Code: 0xE8D6 ==> Row:72 Col:54 */
    {0x67DC,0xF1B9}, /* ��: GB2312 Code: 0xB9F1 ==> Row:25 Col:81 */
    {0x67DD,0xD8E8}, /* ��: GB2312 Code: 0xE8D8 ==> Row:72 Col:56 */
    {0x67DE,0xF5D7}, /* ��: GB2312 Code: 0xD7F5 ==> Row:55 Col:85 */
    {0x67E0,0xFBC4}, /* ��: GB2312 Code: 0xC4FB ==> Row:36 Col:91 */
    {0x67E2,0xDCE8}, /* ��: GB2312 Code: 0xE8DC ==> Row:72 Col:60 */
    {0x67E5,0xE9B2}, /* ��: GB2312 Code: 0xB2E9 ==> Row:18 Col:73 */
    {0x67E9,0xD1E8}, /* ��: GB2312 Code: 0xE8D1 ==> Row:72 Col:49 */
    {0x67EC,0xEDBC}, /* ��: GB2312 Code: 0xBCED ==> Row:28 Col:77 */
    {0x67EF,0xC2BF}, /* ��: GB2312 Code: 0xBFC2 ==> Row:31 Col:34 */
    {0x67F0,0xCDE8}, /* ��: GB2312 Code: 0xE8CD ==> Row:72 Col:45 */
    {0x67F1,0xF9D6}, /* ��: GB2312 Code: 0xD6F9 ==> Row:54 Col:89 */
    {0x67F3,0xF8C1}, /* ��: GB2312 Code: 0xC1F8 ==> Row:33 Col:88 */
    {0x67F4,0xF1B2}, /* ��: GB2312 Code: 0xB2F1 ==> Row:18 Col:81 */
    {0x67FD,0xDFE8}, /* ��: GB2312 Code: 0xE8DF ==> Row:72 Col:63 */
    {0x67FF,0xC1CA}, /* ��: GB2312 Code: 0xCAC1 ==> Row:42 Col:33 */
    {0x6800,0xD9E8}, /* ��: GB2312 Code: 0xE8D9 ==> Row:72 Col:57 */
    {0x6805,0xA4D5}, /* դ: GB2312 Code: 0xD5A4 ==> Row:53 Col:04 */
    {0x6807,0xEAB1}, /* ��: GB2312 Code: 0xB1EA ==> Row:17 Col:74 */
    {0x6808,0xBBD5}, /* ջ: GB2312 Code: 0xD5BB ==> Row:53 Col:27 */
    {0x6809,0xCEE8}, /* ��: GB2312 Code: 0xE8CE ==> Row:72 Col:46 */
    {0x680A,0xD0E8}, /* ��: GB2312 Code: 0xE8D0 ==> Row:72 Col:48 */
    {0x680B,0xB0B6}, /* ��: GB2312 Code: 0xB6B0 ==> Row:22 Col:16 */
    {0x680C,0xD3E8}, /* ��: GB2312 Code: 0xE8D3 ==> Row:72 Col:51 */
    {0x680E,0xDDE8}, /* ��: GB2312 Code: 0xE8DD ==> Row:72 Col:61 */
    {0x680F,0xB8C0}, /* ��: GB2312 Code: 0xC0B8 ==> Row:32 Col:24 */
    {0x6811,0xF7CA}, /* ��: GB2312 Code: 0xCAF7 ==> Row:42 Col:87 */
    {0x6813,0xA8CB}, /* ˨: GB2312 Code: 0xCBA8 ==> Row:43 Col:08 */
    {0x6816,0xDCC6}, /* ��: GB2312 Code: 0xC6DC ==> Row:38 Col:60 */
    {0x6817,0xF5C0}, /* ��: GB2312 Code: 0xC0F5 ==> Row:32 Col:85 */
    {0x681D,0xE9E8}, /* ��: GB2312 Code: 0xE8E9 ==> Row:72 Col:73 */
    {0x6821,0xA3D0}, /* У: GB2312 Code: 0xD0A3 ==> Row:48 Col:03 */
    {0x6829,0xF2E8}, /* ��: GB2312 Code: 0xE8F2 ==> Row:72 Col:82 */
    {0x682A,0xEAD6}, /* ��: GB2312 Code: 0xD6EA ==> Row:54 Col:74 */
    {0x6832,0xE0E8}, /* ��: GB2312 Code: 0xE8E0 ==> Row:72 Col:64 */
    {0x6833,0xE1E8}, /* ��: GB2312 Code: 0xE8E1 ==> Row:72 Col:65 */
    {0x6837,0xF9D1}, /* ��: GB2312 Code: 0xD1F9 ==> Row:49 Col:89 */
    {0x6838,0xCBBA}, /* ��: GB2312 Code: 0xBACB ==> Row:26 Col:43 */
    {0x6839,0xF9B8}, /* ��: GB2312 Code: 0xB8F9 ==> Row:24 Col:89 */
    {0x683C,0xF1B8}, /* ��: GB2312 Code: 0xB8F1 ==> Row:24 Col:81 */
    {0x683D,0xD4D4}, /* ��: GB2312 Code: 0xD4D4 ==> Row:52 Col:52 */
    {0x683E,0xEFE8}, /* ��: GB2312 Code: 0xE8EF ==> Row:72 Col:79 */
    {0x6840,0xEEE8}, /* ��: GB2312 Code: 0xE8EE ==> Row:72 Col:78 */
    {0x6841,0xECE8}, /* ��: GB2312 Code: 0xE8EC ==> Row:72 Col:76 */
    {0x6842,0xF0B9}, /* ��: GB2312 Code: 0xB9F0 ==> Row:25 Col:80 */
    {0x6843,0xD2CC}, /* ��: GB2312 Code: 0xCCD2 ==> Row:44 Col:50 */
    {0x6844,0xE6E8}, /* ��: GB2312 Code: 0xE8E6 ==> Row:72 Col:70 */
    {0x6845,0xA6CE}, /* Φ: GB2312 Code: 0xCEA6 ==> Row:46 Col:06 */
    {0x6846,0xF2BF}, /* ��: GB2312 Code: 0xBFF2 ==> Row:31 Col:82 */
    {0x6848,0xB8B0}, /* ��: GB2312 Code: 0xB0B8 ==> Row:16 Col:24 */
    {0x6849,0xF1E8}, /* ��: GB2312 Code: 0xE8F1 ==> Row:72 Col:81 */
    {0x684A,0xF0E8}, /* ��: GB2312 Code: 0xE8F0 ==> Row:72 Col:80 */
    {0x684C,0xC0D7}, /* ��: GB2312 Code: 0xD7C0 ==> Row:55 Col:32 */
    {0x684E,0xE4E8}, /* ��: GB2312 Code: 0xE8E4 ==> Row:72 Col:68 */
    {0x6850,0xA9CD}, /* ͩ: GB2312 Code: 0xCDA9 ==> Row:45 Col:09 */
    {0x6851,0xA3C9}, /* ɣ: GB2312 Code: 0xC9A3 ==> Row:41 Col:03 */
    {0x6853,0xB8BB}, /* ��: GB2312 Code: 0xBBB8 ==> Row:27 Col:24 */
    {0x6854,0xDBBD}, /* ��: GB2312 Code: 0xBDDB ==> Row:29 Col:59 */
    {0x6855,0xEAE8}, /* ��: GB2312 Code: 0xE8EA ==> Row:72 Col:74 */
    {0x6860,0xE2E8}, /* ��: GB2312 Code: 0xE8E2 ==> Row:72 Col:66 */
    {0x6861,0xE3E8}, /* ��: GB2312 Code: 0xE8E3 ==> Row:72 Col:67 */
    {0x6862,0xE5E8}, /* ��: GB2312 Code: 0xE8E5 ==> Row:72 Col:69 */
    {0x6863,0xB5B5}, /* ��: GB2312 Code: 0xB5B5 ==> Row:21 Col:21 */
    {0x6864,0xE7E8}, /* ��: GB2312 Code: 0xE8E7 ==> Row:72 Col:71 */
    {0x6865,0xC5C7}, /* ��: GB2312 Code: 0xC7C5 ==> Row:39 Col:37 */
    {0x6866,0xEBE8}, /* ��: GB2312 Code: 0xE8EB ==> Row:72 Col:75 */
    {0x6867,0xEDE8}, /* ��: GB2312 Code: 0xE8ED ==> Row:72 Col:77 */
    {0x6868,0xB0BD}, /* ��: GB2312 Code: 0xBDB0 ==> Row:29 Col:16 */
    {0x6869,0xAED7}, /* ׮: GB2312 Code: 0xD7AE ==> Row:55 Col:14 */
    {0x686B,0xF8E8}, /* ��: GB2312 Code: 0xE8F8 ==> Row:72 Col:88 */
    {0x6874,0xF5E8}, /* ��: GB2312 Code: 0xE8F5 ==> Row:72 Col:85 */
    {0x6876,0xB0CD}, /* Ͱ: GB2312 Code: 0xCDB0 ==> Row:45 Col:16 */
    {0x6877,0xF6E8}, /* ��: GB2312 Code: 0xE8F6 ==> Row:72 Col:86 */
    {0x6881,0xBAC1}, /* ��: GB2312 Code: 0xC1BA ==> Row:33 Col:26 */
    {0x6883,0xE8E8}, /* ��: GB2312 Code: 0xE8E8 ==> Row:72 Col:72 */
    {0x6885,0xB7C3}, /* ÷: GB2312 Code: 0xC3B7 ==> Row:35 Col:23 */
    {0x6886,0xF0B0}, /* ��: GB2312 Code: 0xB0F0 ==> Row:16 Col:80 */
    {0x688F,0xF4E8}, /* ��: GB2312 Code: 0xE8F4 ==> Row:72 Col:84 */
    {0x6893,0xF7E8}, /* ��: GB2312 Code: 0xE8F7 ==> Row:72 Col:87 */
    {0x6897,0xA3B9}, /* ��: GB2312 Code: 0xB9A3 ==> Row:25 Col:03 */
    {0x68A2,0xD2C9}, /* ��: GB2312 Code: 0xC9D2 ==> Row:41 Col:50 */
    {0x68A6,0xCEC3}, /* ��: GB2312 Code: 0xC3CE ==> Row:35 Col:46 */
    {0x68A7,0xE0CE}, /* ��: GB2312 Code: 0xCEE0 ==> Row:46 Col:64 */
    {0x68A8,0xE6C0}, /* ��: GB2312 Code: 0xC0E6 ==> Row:32 Col:70 */
    {0x68AD,0xF3CB}, /* ��: GB2312 Code: 0xCBF3 ==> Row:43 Col:83 */
    {0x68AF,0xDDCC}, /* ��: GB2312 Code: 0xCCDD ==> Row:44 Col:61 */
    {0x68B0,0xB5D0}, /* е: GB2312 Code: 0xD0B5 ==> Row:48 Col:21 */
    {0x68B3,0xE1CA}, /* ��: GB2312 Code: 0xCAE1 ==> Row:42 Col:65 */
    {0x68B5,0xF3E8}, /* ��: GB2312 Code: 0xE8F3 ==> Row:72 Col:83 */
    {0x68C0,0xECBC}, /* ��: GB2312 Code: 0xBCEC ==> Row:28 Col:76 */
    {0x68C2,0xF9E8}, /* ��: GB2312 Code: 0xE8F9 ==> Row:72 Col:89 */
    {0x68C9,0xDEC3}, /* ��: GB2312 Code: 0xC3DE ==> Row:35 Col:62 */
    {0x68CB,0xE5C6}, /* ��: GB2312 Code: 0xC6E5 ==> Row:38 Col:69 */
    {0x68CD,0xF7B9}, /* ��: GB2312 Code: 0xB9F7 ==> Row:25 Col:87 */
    {0x68D2,0xF4B0}, /* ��: GB2312 Code: 0xB0F4 ==> Row:16 Col:84 */
    {0x68D5,0xD8D7}, /* ��: GB2312 Code: 0xD7D8 ==> Row:55 Col:56 */
    {0x68D8,0xACBC}, /* ��: GB2312 Code: 0xBCAC ==> Row:28 Col:12 */
    {0x68DA,0xEFC5}, /* ��: GB2312 Code: 0xC5EF ==> Row:37 Col:79 */
    {0x68E0,0xC4CC}, /* ��: GB2312 Code: 0xCCC4 ==> Row:44 Col:36 */
    {0x68E3,0xA6E9}, /* �: GB2312 Code: 0xE9A6 ==> Row:73 Col:06 */
    {0x68EE,0xADC9}, /* ɭ: GB2312 Code: 0xC9AD ==> Row:41 Col:13 */
    {0x68F0,0xA2E9}, /* �: GB2312 Code: 0xE9A2 ==> Row:73 Col:02 */
    {0x68F1,0xE2C0}, /* ��: GB2312 Code: 0xC0E2 ==> Row:32 Col:66 */
    {0x68F5,0xC3BF}, /* ��: GB2312 Code: 0xBFC3 ==> Row:31 Col:35 */
    {0x68F9,0xFEE8}, /* ��: GB2312 Code: 0xE8FE ==> Row:72 Col:94 */
    {0x68FA,0xD7B9}, /* ��: GB2312 Code: 0xB9D7 ==> Row:25 Col:55 */
    {0x68FC,0xFBE8}, /* ��: GB2312 Code: 0xE8FB ==> Row:72 Col:91 */
    {0x6901,0xA4E9}, /* �: GB2312 Code: 0xE9A4 ==> Row:73 Col:04 */
    {0x6905,0xCED2}, /* ��: GB2312 Code: 0xD2CE ==> Row:50 Col:46 */
    {0x690B,0xA3E9}, /* �: GB2312 Code: 0xE9A3 ==> Row:73 Col:03 */
    {0x690D,0xB2D6}, /* ֲ: GB2312 Code: 0xD6B2 ==> Row:54 Col:18 */
    {0x690E,0xB5D7}, /* ׵: GB2312 Code: 0xD7B5 ==> Row:55 Col:21 */
    {0x6910,0xA7E9}, /* �: GB2312 Code: 0xE9A7 ==> Row:73 Col:07 */
    {0x6912,0xB7BD}, /* ��: GB2312 Code: 0xBDB7 ==> Row:29 Col:23 */
    {0x691F,0xFCE8}, /* ��: GB2312 Code: 0xE8FC ==> Row:72 Col:92 */
    {0x6920,0xFDE8}, /* ��: GB2312 Code: 0xE8FD ==> Row:72 Col:93 */
    {0x6924,0xA1E9}, /* �: GB2312 Code: 0xE9A1 ==> Row:73 Col:01 */
    {0x692D,0xD6CD}, /* ��: GB2312 Code: 0xCDD6 ==> Row:45 Col:54 */
    {0x6930,0xACD2}, /* Ҭ: GB2312 Code: 0xD2AC ==> Row:50 Col:12 */
    {0x6934,0xB2E9}, /* �: GB2312 Code: 0xE9B2 ==> Row:73 Col:18 */
    {0x6939,0xA9E9}, /* �: GB2312 Code: 0xE9A9 ==> Row:73 Col:09 */
    {0x693D,0xAAB4}, /* ��: GB2312 Code: 0xB4AA ==> Row:20 Col:10 */
    {0x693F,0xBBB4}, /* ��: GB2312 Code: 0xB4BB ==> Row:20 Col:27 */
    {0x6942,0xABE9}, /* �: GB2312 Code: 0xE9AB ==> Row:73 Col:11 */
    {0x6954,0xA8D0}, /* Ш: GB2312 Code: 0xD0A8 ==> Row:48 Col:08 */
    {0x6957,0xA5E9}, /* �: GB2312 Code: 0xE9A5 ==> Row:73 Col:05 */
    {0x695A,0xFEB3}, /* ��: GB2312 Code: 0xB3FE ==> Row:19 Col:94 */
    {0x695D,0xACE9}, /* �: GB2312 Code: 0xE9AC ==> Row:73 Col:12 */
    {0x695E,0xE3C0}, /* ��: GB2312 Code: 0xC0E3 ==> Row:32 Col:67 */
    {0x6960,0xAAE9}, /* �: GB2312 Code: 0xE9AA ==> Row:73 Col:10 */
    {0x6963,0xB9E9}, /* �: GB2312 Code: 0xE9B9 ==> Row:73 Col:25 */
    {0x6966,0xB8E9}, /* �: GB2312 Code: 0xE9B8 ==> Row:73 Col:24 */
    {0x696B,0xAEE9}, /* �: GB2312 Code: 0xE9AE ==> Row:73 Col:14 */
    {0x696E,0xFAE8}, /* ��: GB2312 Code: 0xE8FA ==> Row:72 Col:90 */
    {0x6971,0xA8E9}, /* �: GB2312 Code: 0xE9A8 ==> Row:73 Col:08 */
    {0x6977,0xACBF}, /* ��: GB2312 Code: 0xBFAC ==> Row:31 Col:12 */
    {0x6978,0xB1E9}, /* �: GB2312 Code: 0xE9B1 ==> Row:73 Col:17 */
    {0x6979,0xBAE9}, /* �: GB2312 Code: 0xE9BA ==> Row:73 Col:26 */
    {0x697C,0xA5C2}, /* ¥: GB2312 Code: 0xC2A5 ==> Row:34 Col:05 */
    {0x6980,0xAFE9}, /* �: GB2312 Code: 0xE9AF ==> Row:73 Col:15 */
    {0x6982,0xC5B8}, /* ��: GB2312 Code: 0xB8C5 ==> Row:24 Col:37 */
    {0x6984,0xADE9}, /* �: GB2312 Code: 0xE9AD ==> Row:73 Col:13 */
    {0x6986,0xDCD3}, /* ��: GB2312 Code: 0xD3DC ==> Row:51 Col:60 */
    {0x6987,0xB4E9}, /* �: GB2312 Code: 0xE9B4 ==> Row:73 Col:20 */
    {0x6988,0xB5E9}, /* �: GB2312 Code: 0xE9B5 ==> Row:73 Col:21 */
    {0x6989,0xB7E9}, /* �: GB2312 Code: 0xE9B7 ==> Row:73 Col:23 */
    {0x698D,0xC7E9}, /* ��: GB2312 Code: 0xE9C7 ==> Row:73 Col:39 */
    {0x6994,0xC6C0}, /* ��: GB2312 Code: 0xC0C6 ==> Row:32 Col:38 */
    {0x6995,0xC5E9}, /* ��: GB2312 Code: 0xE9C5 ==> Row:73 Col:37 */
    {0x6998,0xB0E9}, /* �: GB2312 Code: 0xE9B0 ==> Row:73 Col:16 */
    {0x699B,0xBBE9}, /* �: GB2312 Code: 0xE9BB ==> Row:73 Col:27 */
    {0x699C,0xF1B0}, /* ��: GB2312 Code: 0xB0F1 ==> Row:16 Col:81 */
    {0x69A7,0xBCE9}, /* �: GB2312 Code: 0xE9BC ==> Row:73 Col:28 */
    {0x69A8,0xA5D5}, /* ե: GB2312 Code: 0xD5A5 ==> Row:53 Col:05 */
    {0x69AB,0xBEE9}, /* �: GB2312 Code: 0xE9BE ==> Row:73 Col:30 */
    {0x69AD,0xBFE9}, /* �: GB2312 Code: 0xE9BF ==> Row:73 Col:31 */
    {0x69B1,0xC1E9}, /* ��: GB2312 Code: 0xE9C1 ==> Row:73 Col:33 */
    {0x69B4,0xF1C1}, /* ��: GB2312 Code: 0xC1F1 ==> Row:33 Col:81 */
    {0x69B7,0xB6C8}, /* ȶ: GB2312 Code: 0xC8B6 ==> Row:40 Col:22 */
    {0x69BB,0xBDE9}, /* �: GB2312 Code: 0xE9BD ==> Row:73 Col:29 */
    {0x69C1,0xC2E9}, /* ��: GB2312 Code: 0xE9C2 ==> Row:73 Col:34 */
    {0x69CA,0xC3E9}, /* ��: GB2312 Code: 0xE9C3 ==> Row:73 Col:35 */
    {0x69CC,0xB3E9}, /* �: GB2312 Code: 0xE9B3 ==> Row:73 Col:19 */
    {0x69CE,0xB6E9}, /* �: GB2312 Code: 0xE9B6 ==> Row:73 Col:22 */
    {0x69D0,0xB1BB}, /* ��: GB2312 Code: 0xBBB1 ==> Row:27 Col:17 */
    {0x69D4,0xC0E9}, /* ��: GB2312 Code: 0xE9C0 ==> Row:73 Col:32 */
    {0x69DB,0xF7BC}, /* ��: GB2312 Code: 0xBCF7 ==> Row:28 Col:87 */
    {0x69DF,0xC4E9}, /* ��: GB2312 Code: 0xE9C4 ==> Row:73 Col:36 */
    {0x69E0,0xC6E9}, /* ��: GB2312 Code: 0xE9C6 ==> Row:73 Col:38 */
    {0x69ED,0xCAE9}, /* ��: GB2312 Code: 0xE9CA ==> Row:73 Col:42 */
    {0x69F2,0xCEE9}, /* ��: GB2312 Code: 0xE9CE ==> Row:73 Col:46 */
    {0x69FD,0xDBB2}, /* ��: GB2312 Code: 0xB2DB ==> Row:18 Col:59 */
    {0x69FF,0xC8E9}, /* ��: GB2312 Code: 0xE9C8 ==> Row:73 Col:40 */
    {0x6A0A,0xAEB7}, /* ��: GB2312 Code: 0xB7AE ==> Row:23 Col:14 */
    {0x6A17,0xCBE9}, /* ��: GB2312 Code: 0xE9CB ==> Row:73 Col:43 */
    {0x6A18,0xCCE9}, /* ��: GB2312 Code: 0xE9CC ==> Row:73 Col:44 */
    {0x6A1F,0xC1D5}, /* ��: GB2312 Code: 0xD5C1 ==> Row:53 Col:33 */
    {0x6A21,0xA3C4}, /* ģ: GB2312 Code: 0xC4A3 ==> Row:36 Col:03 */
    {0x6A28,0xD8E9}, /* ��: GB2312 Code: 0xE9D8 ==> Row:73 Col:56 */
    {0x6A2A,0xE1BA}, /* ��: GB2312 Code: 0xBAE1 ==> Row:26 Col:65 */
    {0x6A2F,0xC9E9}, /* ��: GB2312 Code: 0xE9C9 ==> Row:73 Col:41 */
    {0x6A31,0xA3D3}, /* ӣ: GB2312 Code: 0xD3A3 ==> Row:51 Col:03 */
    {0x6A35,0xD4E9}, /* ��: GB2312 Code: 0xE9D4 ==> Row:73 Col:52 */
    {0x6A3D,0xD7E9}, /* ��: GB2312 Code: 0xE9D7 ==> Row:73 Col:55 */
    {0x6A3E,0xD0E9}, /* ��: GB2312 Code: 0xE9D0 ==> Row:73 Col:48 */
    {0x6A44,0xCFE9}, /* ��: GB2312 Code: 0xE9CF ==> Row:73 Col:47 */
    {0x6A47,0xC1C7}, /* ��: GB2312 Code: 0xC7C1 ==> Row:39 Col:33 */
    {0x6A50,0xD2E9}, /* ��: GB2312 Code: 0xE9D2 ==> Row:73 Col:50 */
    {0x6A58,0xD9E9}, /* ��: GB2312 Code: 0xE9D9 ==> Row:73 Col:57 */
    {0x6A59,0xC8B3}, /* ��: GB2312 Code: 0xB3C8 ==> Row:19 Col:40 */
    {0x6A5B,0xD3E9}, /* ��: GB2312 Code: 0xE9D3 ==> Row:73 Col:51 */
    {0x6A61,0xF0CF}, /* ��: GB2312 Code: 0xCFF0 ==> Row:47 Col:80 */
    {0x6A65,0xCDE9}, /* ��: GB2312 Code: 0xE9CD ==> Row:73 Col:45 */
    {0x6A71,0xF7B3}, /* ��: GB2312 Code: 0xB3F7 ==> Row:19 Col:87 */
    {0x6A79,0xD6E9}, /* ��: GB2312 Code: 0xE9D6 ==> Row:73 Col:54 */
    {0x6A7C,0xDAE9}, /* ��: GB2312 Code: 0xE9DA ==> Row:73 Col:58 */
    {0x6A80,0xB4CC}, /* ̴: GB2312 Code: 0xCCB4 ==> Row:44 Col:20 */
    {0x6A84,0xADCF}, /* ϭ: GB2312 Code: 0xCFAD ==> Row:47 Col:13 */
    {0x6A8E,0xD5E9}, /* ��: GB2312 Code: 0xE9D5 ==> Row:73 Col:53 */
    {0x6A90,0xDCE9}, /* ��: GB2312 Code: 0xE9DC ==> Row:73 Col:60 */
    {0x6A91,0xDBE9}, /* ��: GB2312 Code: 0xE9DB ==> Row:73 Col:59 */
    {0x6A97,0xDEE9}, /* ��: GB2312 Code: 0xE9DE ==> Row:73 Col:62 */
    {0x6AA0,0xD1E9}, /* ��: GB2312 Code: 0xE9D1 ==> Row:73 Col:49 */
    {0x6AA9,0xDDE9}, /* ��: GB2312 Code: 0xE9DD ==> Row:73 Col:61 */
    {0x6AAB,0xDFE9}, /* ��: GB2312 Code: 0xE9DF ==> Row:73 Col:63 */
    {0x6AAC,0xCAC3}, /* ��: GB2312 Code: 0xC3CA ==> Row:35 Col:42 */
    {0x6B20,0xB7C7}, /* Ƿ: GB2312 Code: 0xC7B7 ==> Row:39 Col:23 */
    {0x6B21,0xCEB4}, /* ��: GB2312 Code: 0xB4CE ==> Row:20 Col:46 */
    {0x6B22,0xB6BB}, /* ��: GB2312 Code: 0xBBB6 ==> Row:27 Col:22 */
    {0x6B23,0xC0D0}, /* ��: GB2312 Code: 0xD0C0 ==> Row:48 Col:32 */
    {0x6B24,0xA3EC}, /* �: GB2312 Code: 0xECA3 ==> Row:76 Col:03 */
    {0x6B27,0xB7C5}, /* ŷ: GB2312 Code: 0xC5B7 ==> Row:37 Col:23 */
    {0x6B32,0xFBD3}, /* ��: GB2312 Code: 0xD3FB ==> Row:51 Col:91 */
    {0x6B37,0xA4EC}, /* �: GB2312 Code: 0xECA4 ==> Row:76 Col:04 */
    {0x6B39,0xA5EC}, /* �: GB2312 Code: 0xECA5 ==> Row:76 Col:05 */
    {0x6B3A,0xDBC6}, /* ��: GB2312 Code: 0xC6DB ==> Row:38 Col:59 */
    {0x6B3E,0xEEBF}, /* ��: GB2312 Code: 0xBFEE ==> Row:31 Col:78 */
    {0x6B43,0xA6EC}, /* �: GB2312 Code: 0xECA6 ==> Row:76 Col:06 */
    {0x6B46,0xA7EC}, /* �: GB2312 Code: 0xECA7 ==> Row:76 Col:07 */
    {0x6B47,0xAAD0}, /* Ъ: GB2312 Code: 0xD0AA ==> Row:48 Col:10 */
    {0x6B49,0xB8C7}, /* Ǹ: GB2312 Code: 0xC7B8 ==> Row:39 Col:24 */
    {0x6B4C,0xE8B8}, /* ��: GB2312 Code: 0xB8E8 ==> Row:24 Col:72 */
    {0x6B59,0xA8EC}, /* �: GB2312 Code: 0xECA8 ==> Row:76 Col:08 */
    {0x6B62,0xB9D6}, /* ֹ: GB2312 Code: 0xD6B9 ==> Row:54 Col:25 */
    {0x6B63,0xFDD5}, /* ��: GB2312 Code: 0xD5FD ==> Row:53 Col:93 */
    {0x6B64,0xCBB4}, /* ��: GB2312 Code: 0xB4CB ==> Row:20 Col:43 */
    {0x6B65,0xBDB2}, /* ��: GB2312 Code: 0xB2BD ==> Row:18 Col:29 */
    {0x6B66,0xE4CE}, /* ��: GB2312 Code: 0xCEE4 ==> Row:46 Col:68 */
    {0x6B67,0xE7C6}, /* ��: GB2312 Code: 0xC6E7 ==> Row:38 Col:71 */
    {0x6B6A,0xE1CD}, /* ��: GB2312 Code: 0xCDE1 ==> Row:45 Col:65 */
    {0x6B79,0xF5B4}, /* ��: GB2312 Code: 0xB4F5 ==> Row:20 Col:85 */
    {0x6B7B,0xC0CB}, /* ��: GB2312 Code: 0xCBC0 ==> Row:43 Col:32 */
    {0x6B7C,0xDFBC}, /* ��: GB2312 Code: 0xBCDF ==> Row:28 Col:63 */
    {0x6B81,0xE2E9}, /* ��: GB2312 Code: 0xE9E2 ==> Row:73 Col:66 */
    {0x6B82,0xE3E9}, /* ��: GB2312 Code: 0xE9E3 ==> Row:73 Col:67 */
    {0x6B83,0xEAD1}, /* ��: GB2312 Code: 0xD1EA ==> Row:49 Col:74 */
    {0x6B84,0xE5E9}, /* ��: GB2312 Code: 0xE9E5 ==> Row:73 Col:69 */
    {0x6B86,0xF9B4}, /* ��: GB2312 Code: 0xB4F9 ==> Row:20 Col:89 */
    {0x6B87,0xE4E9}, /* ��: GB2312 Code: 0xE9E4 ==> Row:73 Col:68 */
    {0x6B89,0xB3D1}, /* ѳ: GB2312 Code: 0xD1B3 ==> Row:49 Col:19 */
    {0x6B8A,0xE2CA}, /* ��: GB2312 Code: 0xCAE2 ==> Row:42 Col:66 */
    {0x6B8B,0xD0B2}, /* ��: GB2312 Code: 0xB2D0 ==> Row:18 Col:48 */
    {0x6B8D,0xE8E9}, /* ��: GB2312 Code: 0xE9E8 ==> Row:73 Col:72 */
    {0x6B92,0xE6E9}, /* ��: GB2312 Code: 0xE9E6 ==> Row:73 Col:70 */
    {0x6B93,0xE7E9}, /* ��: GB2312 Code: 0xE9E7 ==> Row:73 Col:71 */
    {0x6B96,0xB3D6}, /* ֳ: GB2312 Code: 0xD6B3 ==> Row:54 Col:19 */
    {0x6B9A,0xE9E9}, /* ��: GB2312 Code: 0xE9E9 ==> Row:73 Col:73 */
    {0x6B9B,0xEAE9}, /* ��: GB2312 Code: 0xE9EA ==> Row:73 Col:74 */
    {0x6BA1,0xEBE9}, /* ��: GB2312 Code: 0xE9EB ==> Row:73 Col:75 */
    {0x6BAA,0xECE9}, /* ��: GB2312 Code: 0xE9EC ==> Row:73 Col:76 */
    {0x6BB3,0xAFEC}, /* �: GB2312 Code: 0xECAF ==> Row:76 Col:15 */
    {0x6BB4,0xB9C5}, /* Ź: GB2312 Code: 0xC5B9 ==> Row:37 Col:25 */
    {0x6BB5,0xCEB6}, /* ��: GB2312 Code: 0xB6CE ==> Row:22 Col:46 */
    {0x6BB7,0xF3D2}, /* ��: GB2312 Code: 0xD2F3 ==> Row:50 Col:83 */
    {0x6BBF,0xEEB5}, /* ��: GB2312 Code: 0xB5EE ==> Row:21 Col:78 */
    {0x6BC1,0xD9BB}, /* ��: GB2312 Code: 0xBBD9 ==> Row:27 Col:57 */
    {0x6BC2,0xB1EC}, /* �: GB2312 Code: 0xECB1 ==> Row:76 Col:17 */
    {0x6BC5,0xE3D2}, /* ��: GB2312 Code: 0xD2E3 ==> Row:50 Col:67 */
    {0x6BCB,0xE3CE}, /* ��: GB2312 Code: 0xCEE3 ==> Row:46 Col:67 */
    {0x6BCD,0xB8C4}, /* ĸ: GB2312 Code: 0xC4B8 ==> Row:36 Col:24 */
    {0x6BCF,0xBFC3}, /* ÿ: GB2312 Code: 0xC3BF ==> Row:35 Col:31 */
    {0x6BD2,0xBEB6}, /* ��: GB2312 Code: 0xB6BE ==> Row:22 Col:30 */
    {0x6BD3,0xB9D8}, /* ع: GB2312 Code: 0xD8B9 ==> Row:56 Col:25 */
    {0x6BD4,0xC8B1}, /* ��: GB2312 Code: 0xB1C8 ==> Row:17 Col:40 */
    {0x6BD5,0xCFB1}, /* ��: GB2312 Code: 0xB1CF ==> Row:17 Col:47 */
    {0x6BD6,0xD1B1}, /* ��: GB2312 Code: 0xB1D1 ==> Row:17 Col:49 */
    {0x6BD7,0xFEC5}, /* ��: GB2312 Code: 0xC5FE ==> Row:37 Col:94 */
    {0x6BD9,0xD0B1}, /* ��: GB2312 Code: 0xB1D0 ==> Row:17 Col:48 */
    {0x6BDB,0xABC3}, /* ë: GB2312 Code: 0xC3AB ==> Row:35 Col:11 */
    {0x6BE1,0xB1D5}, /* ձ: GB2312 Code: 0xD5B1 ==> Row:53 Col:17 */
    {0x6BEA,0xA4EB}, /* �: GB2312 Code: 0xEBA4 ==> Row:75 Col:04 */
    {0x6BEB,0xC1BA}, /* ��: GB2312 Code: 0xBAC1 ==> Row:26 Col:33 */
    {0x6BEF,0xBACC}, /* ̺: GB2312 Code: 0xCCBA ==> Row:44 Col:26 */
    {0x6BF3,0xA5EB}, /* �: GB2312 Code: 0xEBA5 ==> Row:75 Col:05 */
    {0x6BF5,0xA7EB}, /* �: GB2312 Code: 0xEBA7 ==> Row:75 Col:07 */
    {0x6BF9,0xA8EB}, /* �: GB2312 Code: 0xEBA8 ==> Row:75 Col:08 */
    {0x6BFD,0xA6EB}, /* �: GB2312 Code: 0xEBA6 ==> Row:75 Col:06 */
    {0x6C05,0xA9EB}, /* �: GB2312 Code: 0xEBA9 ==> Row:75 Col:09 */
    {0x6C06,0xABEB}, /* �: GB2312 Code: 0xEBAB ==> Row:75 Col:11 */
    {0x6C07,0xAAEB}, /* �: GB2312 Code: 0xEBAA ==> Row:75 Col:10 */
    {0x6C0D,0xACEB}, /* �: GB2312 Code: 0xEBAC ==> Row:75 Col:12 */
    {0x6C0F,0xCFCA}, /* ��: GB2312 Code: 0xCACF ==> Row:42 Col:47 */
    {0x6C10,0xB5D8}, /* ص: GB2312 Code: 0xD8B5 ==> Row:56 Col:21 */
    {0x6C11,0xF1C3}, /* ��: GB2312 Code: 0xC3F1 ==> Row:35 Col:81 */
    {0x6C13,0xA5C3}, /* å: GB2312 Code: 0xC3A5 ==> Row:35 Col:05 */
    {0x6C14,0xF8C6}, /* ��: GB2312 Code: 0xC6F8 ==> Row:38 Col:88 */
    {0x6C15,0xADEB}, /* �: GB2312 Code: 0xEBAD ==> Row:75 Col:13 */
    {0x6C16,0xCAC4}, /* ��: GB2312 Code: 0xC4CA ==> Row:36 Col:42 */
    {0x6C18,0xAEEB}, /* �: GB2312 Code: 0xEBAE ==> Row:75 Col:14 */
    {0x6C19,0xAFEB}, /* �: GB2312 Code: 0xEBAF ==> Row:75 Col:15 */
    {0x6C1A,0xB0EB}, /* �: GB2312 Code: 0xEBB0 ==> Row:75 Col:16 */
    {0x6C1B,0xD5B7}, /* ��: GB2312 Code: 0xB7D5 ==> Row:23 Col:53 */
    {0x6C1F,0xFAB7}, /* ��: GB2312 Code: 0xB7FA ==> Row:23 Col:90 */
    {0x6C21,0xB1EB}, /* �: GB2312 Code: 0xEBB1 ==> Row:75 Col:17 */
    {0x6C22,0xE2C7}, /* ��: GB2312 Code: 0xC7E2 ==> Row:39 Col:66 */
    {0x6C24,0xB3EB}, /* �: GB2312 Code: 0xEBB3 ==> Row:75 Col:19 */
    {0x6C26,0xA4BA}, /* ��: GB2312 Code: 0xBAA4 ==> Row:26 Col:04 */
    {0x6C27,0xF5D1}, /* ��: GB2312 Code: 0xD1F5 ==> Row:49 Col:85 */
    {0x6C28,0xB1B0}, /* ��: GB2312 Code: 0xB0B1 ==> Row:16 Col:17 */
    {0x6C29,0xB2EB}, /* �: GB2312 Code: 0xEBB2 ==> Row:75 Col:18 */
    {0x6C2A,0xB4EB}, /* �: GB2312 Code: 0xEBB4 ==> Row:75 Col:20 */
    {0x6C2E,0xAAB5}, /* ��: GB2312 Code: 0xB5AA ==> Row:21 Col:10 */
    {0x6C2F,0xC8C2}, /* ��: GB2312 Code: 0xC2C8 ==> Row:34 Col:40 */
    {0x6C30,0xE8C7}, /* ��: GB2312 Code: 0xC7E8 ==> Row:39 Col:72 */
    {0x6C32,0xB5EB}, /* �: GB2312 Code: 0xEBB5 ==> Row:75 Col:21 */
    {0x6C34,0xAECB}, /* ˮ: GB2312 Code: 0xCBAE ==> Row:43 Col:14 */
    {0x6C35,0xDFE3}, /* ��: GB2312 Code: 0xE3DF ==> Row:67 Col:63 */
    {0x6C38,0xC0D3}, /* ��: GB2312 Code: 0xD3C0 ==> Row:51 Col:32 */
    {0x6C3D,0xDBD9}, /* ��: GB2312 Code: 0xD9DB ==> Row:57 Col:59 */
    {0x6C40,0xA1CD}, /* ͡: GB2312 Code: 0xCDA1 ==> Row:45 Col:01 */
    {0x6C41,0xADD6}, /* ֭: GB2312 Code: 0xD6AD ==> Row:54 Col:13 */
    {0x6C42,0xF3C7}, /* ��: GB2312 Code: 0xC7F3 ==> Row:39 Col:83 */
    {0x6C46,0xE0D9}, /* ��: GB2312 Code: 0xD9E0 ==> Row:57 Col:64 */
    {0x6C47,0xE3BB}, /* ��: GB2312 Code: 0xBBE3 ==> Row:27 Col:67 */
    {0x6C49,0xBABA}, /* ��: GB2312 Code: 0xBABA ==> Row:26 Col:26 */
    {0x6C4A,0xE2E3}, /* ��: GB2312 Code: 0xE3E2 ==> Row:67 Col:66 */
    {0x6C50,0xABCF}, /* ϫ: GB2312 Code: 0xCFAB ==> Row:47 Col:11 */
    {0x6C54,0xE0E3}, /* ��: GB2312 Code: 0xE3E0 ==> Row:67 Col:64 */
    {0x6C55,0xC7C9}, /* ��: GB2312 Code: 0xC9C7 ==> Row:41 Col:39 */
    {0x6C57,0xB9BA}, /* ��: GB2312 Code: 0xBAB9 ==> Row:26 Col:25 */
    {0x6C5B,0xB4D1}, /* Ѵ: GB2312 Code: 0xD1B4 ==> Row:49 Col:20 */
    {0x6C5C,0xE1E3}, /* ��: GB2312 Code: 0xE3E1 ==> Row:67 Col:65 */
    {0x6C5D,0xEAC8}, /* ��: GB2312 Code: 0xC8EA ==> Row:40 Col:74 */
    {0x6C5E,0xAFB9}, /* ��: GB2312 Code: 0xB9AF ==> Row:25 Col:15 */
    {0x6C5F,0xADBD}, /* ��: GB2312 Code: 0xBDAD ==> Row:29 Col:13 */
    {0x6C60,0xD8B3}, /* ��: GB2312 Code: 0xB3D8 ==> Row:19 Col:56 */
    {0x6C61,0xDBCE}, /* ��: GB2312 Code: 0xCEDB ==> Row:46 Col:59 */
    {0x6C64,0xC0CC}, /* ��: GB2312 Code: 0xCCC0 ==> Row:44 Col:32 */
    {0x6C68,0xE8E3}, /* ��: GB2312 Code: 0xE3E8 ==> Row:67 Col:72 */
    {0x6C69,0xE9E3}, /* ��: GB2312 Code: 0xE3E9 ==> Row:67 Col:73 */
    {0x6C6A,0xF4CD}, /* ��: GB2312 Code: 0xCDF4 ==> Row:45 Col:84 */
    {0x6C70,0xADCC}, /* ̭: GB2312 Code: 0xCCAD ==> Row:44 Col:13 */
    {0x6C72,0xB3BC}, /* ��: GB2312 Code: 0xBCB3 ==> Row:28 Col:19 */
    {0x6C74,0xEAE3}, /* ��: GB2312 Code: 0xE3EA ==> Row:67 Col:74 */
    {0x6C76,0xEBE3}, /* ��: GB2312 Code: 0xE3EB ==> Row:67 Col:75 */
    {0x6C79,0xDAD0}, /* ��: GB2312 Code: 0xD0DA ==> Row:48 Col:58 */
    {0x6C7D,0xFBC6}, /* ��: GB2312 Code: 0xC6FB ==> Row:38 Col:91 */
    {0x6C7E,0xDAB7}, /* ��: GB2312 Code: 0xB7DA ==> Row:23 Col:58 */
    {0x6C81,0xDFC7}, /* ��: GB2312 Code: 0xC7DF ==> Row:39 Col:63 */
    {0x6C82,0xCAD2}, /* ��: GB2312 Code: 0xD2CA ==> Row:50 Col:42 */
    {0x6C83,0xD6CE}, /* ��: GB2312 Code: 0xCED6 ==> Row:46 Col:54 */
    {0x6C85,0xE4E3}, /* ��: GB2312 Code: 0xE3E4 ==> Row:67 Col:68 */
    {0x6C86,0xECE3}, /* ��: GB2312 Code: 0xE3EC ==> Row:67 Col:76 */
    {0x6C88,0xF2C9}, /* ��: GB2312 Code: 0xC9F2 ==> Row:41 Col:82 */
    {0x6C89,0xC1B3}, /* ��: GB2312 Code: 0xB3C1 ==> Row:19 Col:33 */
    {0x6C8C,0xE7E3}, /* ��: GB2312 Code: 0xE3E7 ==> Row:67 Col:71 */
    {0x6C8F,0xE3C6}, /* ��: GB2312 Code: 0xC6E3 ==> Row:38 Col:67 */
    {0x6C90,0xE5E3}, /* ��: GB2312 Code: 0xE3E5 ==> Row:67 Col:69 */
    {0x6C93,0xB3ED}, /* ��: GB2312 Code: 0xEDB3 ==> Row:77 Col:19 */
    {0x6C94,0xE6E3}, /* ��: GB2312 Code: 0xE3E6 ==> Row:67 Col:70 */
    {0x6C99,0xB3C9}, /* ɳ: GB2312 Code: 0xC9B3 ==> Row:41 Col:19 */
    {0x6C9B,0xE6C5}, /* ��: GB2312 Code: 0xC5E6 ==> Row:37 Col:70 */
    {0x6C9F,0xB5B9}, /* ��: GB2312 Code: 0xB9B5 ==> Row:25 Col:21 */
    {0x6CA1,0xBBC3}, /* û: GB2312 Code: 0xC3BB ==> Row:35 Col:27 */
    {0x6CA3,0xE3E3}, /* ��: GB2312 Code: 0xE3E3 ==> Row:67 Col:67 */
    {0x6CA4,0xBDC5}, /* Ž: GB2312 Code: 0xC5BD ==> Row:37 Col:29 */
    {0x6CA5,0xA4C1}, /* ��: GB2312 Code: 0xC1A4 ==> Row:33 Col:04 */
    {0x6CA6,0xD9C2}, /* ��: GB2312 Code: 0xC2D9 ==> Row:34 Col:57 */
    {0x6CA7,0xD7B2}, /* ��: GB2312 Code: 0xB2D7 ==> Row:18 Col:55 */
    {0x6CA9,0xEDE3}, /* ��: GB2312 Code: 0xE3ED ==> Row:67 Col:77 */
    {0x6CAA,0xA6BB}, /* ��: GB2312 Code: 0xBBA6 ==> Row:27 Col:06 */
    {0x6CAB,0xADC4}, /* ĭ: GB2312 Code: 0xC4AD ==> Row:36 Col:13 */
    {0x6CAD,0xF0E3}, /* ��: GB2312 Code: 0xE3F0 ==> Row:67 Col:80 */
    {0x6CAE,0xDABE}, /* ��: GB2312 Code: 0xBEDA ==> Row:30 Col:58 */
    {0x6CB1,0xFBE3}, /* ��: GB2312 Code: 0xE3FB ==> Row:67 Col:91 */
    {0x6CB2,0xF5E3}, /* ��: GB2312 Code: 0xE3F5 ==> Row:67 Col:85 */
    {0x6CB3,0xD3BA}, /* ��: GB2312 Code: 0xBAD3 ==> Row:26 Col:51 */
    {0x6CB8,0xD0B7}, /* ��: GB2312 Code: 0xB7D0 ==> Row:23 Col:48 */
    {0x6CB9,0xCDD3}, /* ��: GB2312 Code: 0xD3CD ==> Row:51 Col:45 */
    {0x6CBB,0xCED6}, /* ��: GB2312 Code: 0xD6CE ==> Row:54 Col:46 */
    {0x6CBC,0xD3D5}, /* ��: GB2312 Code: 0xD5D3 ==> Row:53 Col:51 */
    {0x6CBD,0xC1B9}, /* ��: GB2312 Code: 0xB9C1 ==> Row:25 Col:33 */
    {0x6CBE,0xB4D5}, /* մ: GB2312 Code: 0xD5B4 ==> Row:53 Col:20 */
    {0x6CBF,0xD8D1}, /* ��: GB2312 Code: 0xD1D8 ==> Row:49 Col:56 */
    {0x6CC4,0xB9D0}, /* й: GB2312 Code: 0xD0B9 ==> Row:48 Col:25 */
    {0x6CC5,0xF6C7}, /* ��: GB2312 Code: 0xC7F6 ==> Row:39 Col:86 */
    {0x6CC9,0xAAC8}, /* Ȫ: GB2312 Code: 0xC8AA ==> Row:40 Col:10 */
    {0x6CCA,0xB4B2}, /* ��: GB2312 Code: 0xB2B4 ==> Row:18 Col:20 */
    {0x6CCC,0xDAC3}, /* ��: GB2312 Code: 0xC3DA ==> Row:35 Col:58 */
    {0x6CD0,0xEEE3}, /* ��: GB2312 Code: 0xE3EE ==> Row:67 Col:78 */
    {0x6CD3,0xFCE3}, /* ��: GB2312 Code: 0xE3FC ==> Row:67 Col:92 */
    {0x6CD4,0xEFE3}, /* ��: GB2312 Code: 0xE3EF ==> Row:67 Col:79 */
    {0x6CD5,0xA8B7}, /* ��: GB2312 Code: 0xB7A8 ==> Row:23 Col:08 */
    {0x6CD6,0xF7E3}, /* ��: GB2312 Code: 0xE3F7 ==> Row:67 Col:87 */
    {0x6CD7,0xF4E3}, /* ��: GB2312 Code: 0xE3F4 ==> Row:67 Col:84 */
    {0x6CDB,0xBAB7}, /* ��: GB2312 Code: 0xB7BA ==> Row:23 Col:26 */
    {0x6CDE,0xA2C5}, /* Ţ: GB2312 Code: 0xC5A2 ==> Row:37 Col:02 */
    {0x6CE0,0xF6E3}, /* ��: GB2312 Code: 0xE3F6 ==> Row:67 Col:86 */
    {0x6CE1,0xDDC5}, /* ��: GB2312 Code: 0xC5DD ==> Row:37 Col:61 */
    {0x6CE2,0xA8B2}, /* ��: GB2312 Code: 0xB2A8 ==> Row:18 Col:08 */
    {0x6CE3,0xFCC6}, /* ��: GB2312 Code: 0xC6FC ==> Row:38 Col:92 */
    {0x6CE5,0xE0C4}, /* ��: GB2312 Code: 0xC4E0 ==> Row:36 Col:64 */
    {0x6CE8,0xA2D7}, /* ע: GB2312 Code: 0xD7A2 ==> Row:55 Col:02 */
    {0x6CEA,0xE1C0}, /* ��: GB2312 Code: 0xC0E1 ==> Row:32 Col:65 */
    {0x6CEB,0xF9E3}, /* ��: GB2312 Code: 0xE3F9 ==> Row:67 Col:89 */
    {0x6CEE,0xFAE3}, /* ��: GB2312 Code: 0xE3FA ==> Row:67 Col:90 */
    {0x6CEF,0xFDE3}, /* ��: GB2312 Code: 0xE3FD ==> Row:67 Col:93 */
    {0x6CF0,0xA9CC}, /* ̩: GB2312 Code: 0xCCA9 ==> Row:44 Col:09 */
    {0x6CF1,0xF3E3}, /* ��: GB2312 Code: 0xE3F3 ==> Row:67 Col:83 */
    {0x6CF3,0xBED3}, /* Ӿ: GB2312 Code: 0xD3BE ==> Row:51 Col:30 */
    {0x6CF5,0xC3B1}, /* ��: GB2312 Code: 0xB1C3 ==> Row:17 Col:35 */
    {0x6CF6,0xB4ED}, /* ��: GB2312 Code: 0xEDB4 ==> Row:77 Col:20 */
    {0x6CF7,0xF1E3}, /* ��: GB2312 Code: 0xE3F1 ==> Row:67 Col:81 */
    {0x6CF8,0xF2E3}, /* ��: GB2312 Code: 0xE3F2 ==> Row:67 Col:82 */
    {0x6CFA,0xF8E3}, /* ��: GB2312 Code: 0xE3F8 ==> Row:67 Col:88 */
    {0x6CFB,0xBAD0}, /* к: GB2312 Code: 0xD0BA ==> Row:48 Col:26 */
    {0x6CFC,0xC3C6}, /* ��: GB2312 Code: 0xC6C3 ==> Row:38 Col:35 */
    {0x6CFD,0xF3D4}, /* ��: GB2312 Code: 0xD4F3 ==> Row:52 Col:83 */
    {0x6CFE,0xFEE3}, /* ��: GB2312 Code: 0xE3FE ==> Row:67 Col:94 */
    {0x6D01,0xE0BD}, /* ��: GB2312 Code: 0xBDE0 ==> Row:29 Col:64 */
    {0x6D04,0xA7E4}, /* �: GB2312 Code: 0xE4A7 ==> Row:68 Col:07 */
    {0x6D07,0xA6E4}, /* �: GB2312 Code: 0xE4A6 ==> Row:68 Col:06 */
    {0x6D0B,0xF3D1}, /* ��: GB2312 Code: 0xD1F3 ==> Row:49 Col:83 */
    {0x6D0C,0xA3E4}, /* �: GB2312 Code: 0xE4A3 ==> Row:68 Col:03 */
    {0x6D0E,0xA9E4}, /* �: GB2312 Code: 0xE4A9 ==> Row:68 Col:09 */
    {0x6D12,0xF7C8}, /* ��: GB2312 Code: 0xC8F7 ==> Row:40 Col:87 */
    {0x6D17,0xB4CF}, /* ϴ: GB2312 Code: 0xCFB4 ==> Row:47 Col:20 */
    {0x6D19,0xA8E4}, /* �: GB2312 Code: 0xE4A8 ==> Row:68 Col:08 */
    {0x6D1A,0xAEE4}, /* �: GB2312 Code: 0xE4AE ==> Row:68 Col:14 */
    {0x6D1B,0xE5C2}, /* ��: GB2312 Code: 0xC2E5 ==> Row:34 Col:69 */
    {0x6D1E,0xB4B6}, /* ��: GB2312 Code: 0xB6B4 ==> Row:22 Col:20 */
    {0x6D25,0xF2BD}, /* ��: GB2312 Code: 0xBDF2 ==> Row:29 Col:82 */
    {0x6D27,0xA2E4}, /* �: GB2312 Code: 0xE4A2 ==> Row:68 Col:02 */
    {0x6D2A,0xE9BA}, /* ��: GB2312 Code: 0xBAE9 ==> Row:26 Col:73 */
    {0x6D2B,0xAAE4}, /* �: GB2312 Code: 0xE4AA ==> Row:68 Col:10 */
    {0x6D2E,0xACE4}, /* �: GB2312 Code: 0xE4AC ==> Row:68 Col:12 */
    {0x6D31,0xFDB6}, /* ��: GB2312 Code: 0xB6FD ==> Row:22 Col:93 */
    {0x6D32,0xDED6}, /* ��: GB2312 Code: 0xD6DE ==> Row:54 Col:62 */
    {0x6D33,0xB2E4}, /* �: GB2312 Code: 0xE4B2 ==> Row:68 Col:18 */
    {0x6D35,0xADE4}, /* �: GB2312 Code: 0xE4AD ==> Row:68 Col:13 */
    {0x6D39,0xA1E4}, /* �: GB2312 Code: 0xE4A1 ==> Row:68 Col:01 */
    {0x6D3B,0xEEBB}, /* ��: GB2312 Code: 0xBBEE ==> Row:27 Col:78 */
    {0x6D3C,0xDDCD}, /* ��: GB2312 Code: 0xCDDD ==> Row:45 Col:61 */
    {0x6D3D,0xA2C7}, /* Ǣ: GB2312 Code: 0xC7A2 ==> Row:39 Col:02 */
    {0x6D3E,0xC9C5}, /* ��: GB2312 Code: 0xC5C9 ==> Row:37 Col:41 */
    {0x6D41,0xF7C1}, /* ��: GB2312 Code: 0xC1F7 ==> Row:33 Col:87 */
    {0x6D43,0xA4E4}, /* �: GB2312 Code: 0xE4A4 ==> Row:68 Col:04 */
    {0x6D45,0xB3C7}, /* ǳ: GB2312 Code: 0xC7B3 ==> Row:39 Col:19 */
    {0x6D46,0xACBD}, /* ��: GB2312 Code: 0xBDAC ==> Row:29 Col:12 */
    {0x6D47,0xBDBD}, /* ��: GB2312 Code: 0xBDBD ==> Row:29 Col:29 */
    {0x6D48,0xA5E4}, /* �: GB2312 Code: 0xE4A5 ==> Row:68 Col:05 */
    {0x6D4A,0xC7D7}, /* ��: GB2312 Code: 0xD7C7 ==> Row:55 Col:39 */
    {0x6D4B,0xE2B2}, /* ��: GB2312 Code: 0xB2E2 ==> Row:18 Col:66 */
    {0x6D4D,0xABE4}, /* �: GB2312 Code: 0xE4AB ==> Row:68 Col:11 */
    {0x6D4E,0xC3BC}, /* ��: GB2312 Code: 0xBCC3 ==> Row:28 Col:35 */
    {0x6D4F,0xAFE4}, /* �: GB2312 Code: 0xE4AF ==> Row:68 Col:15 */
    {0x6D51,0xEBBB}, /* ��: GB2312 Code: 0xBBEB ==> Row:27 Col:75 */
    {0x6D52,0xB0E4}, /* �: GB2312 Code: 0xE4B0 ==> Row:68 Col:16 */
    {0x6D53,0xA8C5}, /* Ũ: GB2312 Code: 0xC5A8 ==> Row:37 Col:08 */
    {0x6D54,0xB1E4}, /* �: GB2312 Code: 0xE4B1 ==> Row:68 Col:17 */
    {0x6D59,0xE3D5}, /* ��: GB2312 Code: 0xD5E3 ==> Row:53 Col:67 */
    {0x6D5A,0xA3BF}, /* ��: GB2312 Code: 0xBFA3 ==> Row:31 Col:03 */
    {0x6D5C,0xBAE4}, /* �: GB2312 Code: 0xE4BA ==> Row:68 Col:26 */
    {0x6D5E,0xB7E4}, /* �: GB2312 Code: 0xE4B7 ==> Row:68 Col:23 */
    {0x6D60,0xBBE4}, /* �: GB2312 Code: 0xE4BB ==> Row:68 Col:27 */
    {0x6D63,0xBDE4}, /* �: GB2312 Code: 0xE4BD ==> Row:68 Col:29 */
    {0x6D66,0xD6C6}, /* ��: GB2312 Code: 0xC6D6 ==> Row:38 Col:54 */
    {0x6D69,0xC6BA}, /* ��: GB2312 Code: 0xBAC6 ==> Row:26 Col:38 */
    {0x6D6A,0xCBC0}, /* ��: GB2312 Code: 0xC0CB ==> Row:32 Col:43 */
    {0x6D6E,0xA1B8}, /* ��: GB2312 Code: 0xB8A1 ==> Row:24 Col:01 */
    {0x6D6F,0xB4E4}, /* �: GB2312 Code: 0xE4B4 ==> Row:68 Col:20 */
    {0x6D74,0xA1D4}, /* ԡ: GB2312 Code: 0xD4A1 ==> Row:52 Col:01 */
    {0x6D77,0xA3BA}, /* ��: GB2312 Code: 0xBAA3 ==> Row:26 Col:03 */
    {0x6D78,0xFEBD}, /* ��: GB2312 Code: 0xBDFE ==> Row:29 Col:94 */
    {0x6D7C,0xBCE4}, /* �: GB2312 Code: 0xE4BC ==> Row:68 Col:28 */
    {0x6D82,0xBFCD}, /* Ϳ: GB2312 Code: 0xCDBF ==> Row:45 Col:31 */
    {0x6D85,0xF9C4}, /* ��: GB2312 Code: 0xC4F9 ==> Row:36 Col:89 */
    {0x6D88,0xFBCF}, /* ��: GB2312 Code: 0xCFFB ==> Row:47 Col:91 */
    {0x6D89,0xE6C9}, /* ��: GB2312 Code: 0xC9E6 ==> Row:41 Col:70 */
    {0x6D8C,0xBFD3}, /* ӿ: GB2312 Code: 0xD3BF ==> Row:51 Col:31 */
    {0x6D8E,0xD1CF}, /* ��: GB2312 Code: 0xCFD1 ==> Row:47 Col:49 */
    {0x6D91,0xB3E4}, /* �: GB2312 Code: 0xE4B3 ==> Row:68 Col:19 */
    {0x6D93,0xB8E4}, /* �: GB2312 Code: 0xE4B8 ==> Row:68 Col:24 */
    {0x6D94,0xB9E4}, /* �: GB2312 Code: 0xE4B9 ==> Row:68 Col:25 */
    {0x6D95,0xE9CC}, /* ��: GB2312 Code: 0xCCE9 ==> Row:44 Col:73 */
    {0x6D9B,0xCECC}, /* ��: GB2312 Code: 0xCCCE ==> Row:44 Col:46 */
    {0x6D9D,0xD4C0}, /* ��: GB2312 Code: 0xC0D4 ==> Row:32 Col:52 */
    {0x6D9E,0xB5E4}, /* �: GB2312 Code: 0xE4B5 ==> Row:68 Col:21 */
    {0x6D9F,0xB0C1}, /* ��: GB2312 Code: 0xC1B0 ==> Row:33 Col:16 */
    {0x6DA0,0xB6E4}, /* �: GB2312 Code: 0xE4B6 ==> Row:68 Col:22 */
    {0x6DA1,0xD0CE}, /* ��: GB2312 Code: 0xCED0 ==> Row:46 Col:48 */
    {0x6DA3,0xC1BB}, /* ��: GB2312 Code: 0xBBC1 ==> Row:27 Col:33 */
    {0x6DA4,0xD3B5}, /* ��: GB2312 Code: 0xB5D3 ==> Row:21 Col:51 */
    {0x6DA6,0xF3C8}, /* ��: GB2312 Code: 0xC8F3 ==> Row:40 Col:83 */
    {0x6DA7,0xA7BD}, /* ��: GB2312 Code: 0xBDA7 ==> Row:29 Col:07 */
    {0x6DA8,0xC7D5}, /* ��: GB2312 Code: 0xD5C7 ==> Row:53 Col:39 */
    {0x6DA9,0xACC9}, /* ɬ: GB2312 Code: 0xC9AC ==> Row:41 Col:12 */
    {0x6DAA,0xA2B8}, /* ��: GB2312 Code: 0xB8A2 ==> Row:24 Col:02 */
    {0x6DAB,0xCAE4}, /* ��: GB2312 Code: 0xE4CA ==> Row:68 Col:42 */
    {0x6DAE,0xCCE4}, /* ��: GB2312 Code: 0xE4CC ==> Row:68 Col:44 */
    {0x6DAF,0xC4D1}, /* ��: GB2312 Code: 0xD1C4 ==> Row:49 Col:36 */
    {0x6DB2,0xBAD2}, /* Һ: GB2312 Code: 0xD2BA ==> Row:50 Col:26 */
    {0x6DB5,0xADBA}, /* ��: GB2312 Code: 0xBAAD ==> Row:26 Col:13 */
    {0x6DB8,0xD4BA}, /* ��: GB2312 Code: 0xBAD4 ==> Row:26 Col:52 */
    {0x6DBF,0xC3E4}, /* ��: GB2312 Code: 0xE4C3 ==> Row:68 Col:35 */
    {0x6DC0,0xEDB5}, /* ��: GB2312 Code: 0xB5ED ==> Row:21 Col:77 */
    {0x6DC4,0xCDD7}, /* ��: GB2312 Code: 0xD7CD ==> Row:55 Col:45 */
    {0x6DC5,0xC0E4}, /* ��: GB2312 Code: 0xE4C0 ==> Row:68 Col:32 */
    {0x6DC6,0xFDCF}, /* ��: GB2312 Code: 0xCFFD ==> Row:47 Col:93 */
    {0x6DC7,0xBFE4}, /* �: GB2312 Code: 0xE4BF ==> Row:68 Col:31 */
    {0x6DCB,0xDCC1}, /* ��: GB2312 Code: 0xC1DC ==> Row:33 Col:60 */
    {0x6DCC,0xCACC}, /* ��: GB2312 Code: 0xCCCA ==> Row:44 Col:42 */
    {0x6DD1,0xE7CA}, /* ��: GB2312 Code: 0xCAE7 ==> Row:42 Col:71 */
    {0x6DD6,0xD7C4}, /* ��: GB2312 Code: 0xC4D7 ==> Row:36 Col:55 */
    {0x6DD8,0xD4CC}, /* ��: GB2312 Code: 0xCCD4 ==> Row:44 Col:52 */
    {0x6DD9,0xC8E4}, /* ��: GB2312 Code: 0xE4C8 ==> Row:68 Col:40 */
    {0x6DDD,0xC7E4}, /* ��: GB2312 Code: 0xE4C7 ==> Row:68 Col:39 */
    {0x6DDE,0xC1E4}, /* ��: GB2312 Code: 0xE4C1 ==> Row:68 Col:33 */
    {0x6DE0,0xC4E4}, /* ��: GB2312 Code: 0xE4C4 ==> Row:68 Col:36 */
    {0x6DE1,0xADB5}, /* ��: GB2312 Code: 0xB5AD ==> Row:21 Col:13 */
    {0x6DE4,0xD9D3}, /* ��: GB2312 Code: 0xD3D9 ==> Row:51 Col:57 */
    {0x6DE6,0xC6E4}, /* ��: GB2312 Code: 0xE4C6 ==> Row:68 Col:38 */
    {0x6DEB,0xF9D2}, /* ��: GB2312 Code: 0xD2F9 ==> Row:50 Col:89 */
    {0x6DEC,0xE3B4}, /* ��: GB2312 Code: 0xB4E3 ==> Row:20 Col:67 */
    {0x6DEE,0xB4BB}, /* ��: GB2312 Code: 0xBBB4 ==> Row:27 Col:20 */
    {0x6DF1,0xEEC9}, /* ��: GB2312 Code: 0xC9EE ==> Row:41 Col:78 */
    {0x6DF3,0xBEB4}, /* ��: GB2312 Code: 0xB4BE ==> Row:20 Col:30 */
    {0x6DF7,0xECBB}, /* ��: GB2312 Code: 0xBBEC ==> Row:27 Col:76 */
    {0x6DF9,0xCDD1}, /* ��: GB2312 Code: 0xD1CD ==> Row:49 Col:45 */
    {0x6DFB,0xEDCC}, /* ��: GB2312 Code: 0xCCED ==> Row:44 Col:77 */
    {0x6DFC,0xB5ED}, /* ��: GB2312 Code: 0xEDB5 ==> Row:77 Col:21 */
    {0x6E05,0xE5C7}, /* ��: GB2312 Code: 0xC7E5 ==> Row:39 Col:69 */
    {0x6E0A,0xA8D4}, /* Ԩ: GB2312 Code: 0xD4A8 ==> Row:52 Col:08 */
    {0x6E0C,0xCBE4}, /* ��: GB2312 Code: 0xE4CB ==> Row:68 Col:43 */
    {0x6E0D,0xD5D7}, /* ��: GB2312 Code: 0xD7D5 ==> Row:55 Col:53 */
    {0x6E0E,0xC2E4}, /* ��: GB2312 Code: 0xE4C2 ==> Row:68 Col:34 */
    {0x6E10,0xA5BD}, /* ��: GB2312 Code: 0xBDA5 ==> Row:29 Col:05 */
    {0x6E11,0xC5E4}, /* ��: GB2312 Code: 0xE4C5 ==> Row:68 Col:37 */
    {0x6E14,0xE6D3}, /* ��: GB2312 Code: 0xD3E6 ==> Row:51 Col:70 */
    {0x6E16,0xC9E4}, /* ��: GB2312 Code: 0xE4C9 ==> Row:68 Col:41 */
    {0x6E17,0xF8C9}, /* ��: GB2312 Code: 0xC9F8 ==> Row:41 Col:88 */
    {0x6E1A,0xBEE4}, /* �: GB2312 Code: 0xE4BE ==> Row:68 Col:30 */
    {0x6E1D,0xE5D3}, /* ��: GB2312 Code: 0xD3E5 ==> Row:51 Col:69 */
    {0x6E20,0xFEC7}, /* ��: GB2312 Code: 0xC7FE ==> Row:39 Col:94 */
    {0x6E21,0xC9B6}, /* ��: GB2312 Code: 0xB6C9 ==> Row:22 Col:41 */
    {0x6E23,0xFCD4}, /* ��: GB2312 Code: 0xD4FC ==> Row:52 Col:92 */
    {0x6E24,0xB3B2}, /* ��: GB2312 Code: 0xB2B3 ==> Row:18 Col:19 */
    {0x6E25,0xD7E4}, /* ��: GB2312 Code: 0xE4D7 ==> Row:68 Col:55 */
    {0x6E29,0xC2CE}, /* ��: GB2312 Code: 0xCEC2 ==> Row:46 Col:34 */
    {0x6E2B,0xCDE4}, /* ��: GB2312 Code: 0xE4CD ==> Row:68 Col:45 */
    {0x6E2D,0xBCCE}, /* μ: GB2312 Code: 0xCEBC ==> Row:46 Col:28 */
    {0x6E2F,0xDBB8}, /* ��: GB2312 Code: 0xB8DB ==> Row:24 Col:59 */
    {0x6E32,0xD6E4}, /* ��: GB2312 Code: 0xE4D6 ==> Row:68 Col:54 */
    {0x6E34,0xCABF}, /* ��: GB2312 Code: 0xBFCA ==> Row:31 Col:42 */
    {0x6E38,0xCED3}, /* ��: GB2312 Code: 0xD3CE ==> Row:51 Col:46 */
    {0x6E3A,0xECC3}, /* ��: GB2312 Code: 0xC3EC ==> Row:35 Col:76 */
    {0x6E43,0xC8C5}, /* ��: GB2312 Code: 0xC5C8 ==> Row:37 Col:40 */
    {0x6E44,0xD8E4}, /* ��: GB2312 Code: 0xE4D8 ==> Row:68 Col:56 */
    {0x6E4D,0xC4CD}, /* ��: GB2312 Code: 0xCDC4 ==> Row:45 Col:36 */
    {0x6E4E,0xCFE4}, /* ��: GB2312 Code: 0xE4CF ==> Row:68 Col:47 */
    {0x6E53,0xD4E4}, /* ��: GB2312 Code: 0xE4D4 ==> Row:68 Col:52 */
    {0x6E54,0xD5E4}, /* ��: GB2312 Code: 0xE4D5 ==> Row:68 Col:53 */
    {0x6E56,0xFEBA}, /* ��: GB2312 Code: 0xBAFE ==> Row:26 Col:94 */
    {0x6E58,0xE6CF}, /* ��: GB2312 Code: 0xCFE6 ==> Row:47 Col:70 */
    {0x6E5B,0xBFD5}, /* տ: GB2312 Code: 0xD5BF ==> Row:53 Col:31 */
    {0x6E5F,0xD2E4}, /* ��: GB2312 Code: 0xE4D2 ==> Row:68 Col:50 */
    {0x6E6B,0xD0E4}, /* ��: GB2312 Code: 0xE4D0 ==> Row:68 Col:48 */
    {0x6E6E,0xCEE4}, /* ��: GB2312 Code: 0xE4CE ==> Row:68 Col:46 */
    {0x6E7E,0xE5CD}, /* ��: GB2312 Code: 0xCDE5 ==> Row:45 Col:69 */
    {0x6E7F,0xAACA}, /* ʪ: GB2312 Code: 0xCAAA ==> Row:42 Col:10 */
    {0x6E83,0xA3C0}, /* ��: GB2312 Code: 0xC0A3 ==> Row:32 Col:03 */
    {0x6E85,0xA6BD}, /* ��: GB2312 Code: 0xBDA6 ==> Row:29 Col:06 */
    {0x6E86,0xD3E4}, /* ��: GB2312 Code: 0xE4D3 ==> Row:68 Col:51 */
    {0x6E89,0xC8B8}, /* ��: GB2312 Code: 0xB8C8 ==> Row:24 Col:40 */
    {0x6E8F,0xE7E4}, /* ��: GB2312 Code: 0xE4E7 ==> Row:68 Col:71 */
    {0x6E90,0xB4D4}, /* Դ: GB2312 Code: 0xD4B4 ==> Row:52 Col:20 */
    {0x6E98,0xDBE4}, /* ��: GB2312 Code: 0xE4DB ==> Row:68 Col:59 */
    {0x6E9C,0xEFC1}, /* ��: GB2312 Code: 0xC1EF ==> Row:33 Col:79 */
    {0x6E9F,0xE9E4}, /* ��: GB2312 Code: 0xE4E9 ==> Row:68 Col:73 */
    {0x6EA2,0xE7D2}, /* ��: GB2312 Code: 0xD2E7 ==> Row:50 Col:71 */
    {0x6EA5,0xDFE4}, /* ��: GB2312 Code: 0xE4DF ==> Row:68 Col:63 */
    {0x6EA7,0xE0E4}, /* ��: GB2312 Code: 0xE4E0 ==> Row:68 Col:64 */
    {0x6EAA,0xAACF}, /* Ϫ: GB2312 Code: 0xCFAA ==> Row:47 Col:10 */
    {0x6EAF,0xDDCB}, /* ��: GB2312 Code: 0xCBDD ==> Row:43 Col:61 */
    {0x6EB1,0xDAE4}, /* ��: GB2312 Code: 0xE4DA ==> Row:68 Col:58 */
    {0x6EB2,0xD1E4}, /* ��: GB2312 Code: 0xE4D1 ==> Row:68 Col:49 */
    {0x6EB4,0xE5E4}, /* ��: GB2312 Code: 0xE4E5 ==> Row:68 Col:69 */
    {0x6EB6,0xDCC8}, /* ��: GB2312 Code: 0xC8DC ==> Row:40 Col:60 */
    {0x6EB7,0xE3E4}, /* ��: GB2312 Code: 0xE4E3 ==> Row:68 Col:67 */
    {0x6EBA,0xE7C4}, /* ��: GB2312 Code: 0xC4E7 ==> Row:36 Col:71 */
    {0x6EBB,0xE2E4}, /* ��: GB2312 Code: 0xE4E2 ==> Row:68 Col:66 */
    {0x6EBD,0xE1E4}, /* ��: GB2312 Code: 0xE4E1 ==> Row:68 Col:65 */
    {0x6EC1,0xFCB3}, /* ��: GB2312 Code: 0xB3FC ==> Row:19 Col:92 */
    {0x6EC2,0xE8E4}, /* ��: GB2312 Code: 0xE4E8 ==> Row:68 Col:72 */
    {0x6EC7,0xE1B5}, /* ��: GB2312 Code: 0xB5E1 ==> Row:21 Col:65 */
    {0x6ECB,0xCCD7}, /* ��: GB2312 Code: 0xD7CC ==> Row:55 Col:44 */
    {0x6ECF,0xE6E4}, /* ��: GB2312 Code: 0xE4E6 ==> Row:68 Col:70 */
    {0x6ED1,0xACBB}, /* ��: GB2312 Code: 0xBBAC ==> Row:27 Col:12 */
    {0x6ED3,0xD2D7}, /* ��: GB2312 Code: 0xD7D2 ==> Row:55 Col:50 */
    {0x6ED4,0xCFCC}, /* ��: GB2312 Code: 0xCCCF ==> Row:44 Col:47 */
    {0x6ED5,0xF8EB}, /* ��: GB2312 Code: 0xEBF8 ==> Row:75 Col:88 */
    {0x6ED7,0xE4E4}, /* ��: GB2312 Code: 0xE4E4 ==> Row:68 Col:68 */
    {0x6EDA,0xF6B9}, /* ��: GB2312 Code: 0xB9F6 ==> Row:25 Col:86 */
    {0x6EDE,0xCDD6}, /* ��: GB2312 Code: 0xD6CD ==> Row:54 Col:45 */
    {0x6EDF,0xD9E4}, /* ��: GB2312 Code: 0xE4D9 ==> Row:68 Col:57 */
    {0x6EE0,0xDCE4}, /* ��: GB2312 Code: 0xE4DC ==> Row:68 Col:60 */
    {0x6EE1,0xFAC2}, /* ��: GB2312 Code: 0xC2FA ==> Row:34 Col:90 */
    {0x6EE2,0xDEE4}, /* ��: GB2312 Code: 0xE4DE ==> Row:68 Col:62 */
    {0x6EE4,0xCBC2}, /* ��: GB2312 Code: 0xC2CB ==> Row:34 Col:43 */
    {0x6EE5,0xC4C0}, /* ��: GB2312 Code: 0xC0C4 ==> Row:32 Col:36 */
    {0x6EE6,0xD0C2}, /* ��: GB2312 Code: 0xC2D0 ==> Row:34 Col:48 */
    {0x6EE8,0xF5B1}, /* ��: GB2312 Code: 0xB1F5 ==> Row:17 Col:85 */
    {0x6EE9,0xB2CC}, /* ̲: GB2312 Code: 0xCCB2 ==> Row:44 Col:18 */
    {0x6EF4,0xCEB5}, /* ��: GB2312 Code: 0xB5CE ==> Row:21 Col:46 */
    {0x6EF9,0xEFE4}, /* ��: GB2312 Code: 0xE4EF ==> Row:68 Col:79 */
    {0x6F02,0xAFC6}, /* Ư: GB2312 Code: 0xC6AF ==> Row:38 Col:15 */
    {0x6F06,0xE1C6}, /* ��: GB2312 Code: 0xC6E1 ==> Row:38 Col:65 */
    {0x6F09,0xF5E4}, /* ��: GB2312 Code: 0xE4F5 ==> Row:68 Col:85 */
    {0x6F0F,0xA9C2}, /* ©: GB2312 Code: 0xC2A9 ==> Row:34 Col:09 */
    {0x6F13,0xECC0}, /* ��: GB2312 Code: 0xC0EC ==> Row:32 Col:76 */
    {0x6F14,0xDDD1}, /* ��: GB2312 Code: 0xD1DD ==> Row:49 Col:61 */
    {0x6F15,0xEEE4}, /* ��: GB2312 Code: 0xE4EE ==> Row:68 Col:78 */
    {0x6F20,0xAEC4}, /* Į: GB2312 Code: 0xC4AE ==> Row:36 Col:14 */
    {0x6F24,0xEDE4}, /* ��: GB2312 Code: 0xE4ED ==> Row:68 Col:77 */
    {0x6F29,0xF6E4}, /* ��: GB2312 Code: 0xE4F6 ==> Row:68 Col:86 */
    {0x6F2A,0xF4E4}, /* ��: GB2312 Code: 0xE4F4 ==> Row:68 Col:84 */
    {0x6F2B,0xFEC2}, /* ��: GB2312 Code: 0xC2FE ==> Row:34 Col:94 */
    {0x6F2D,0xDDE4}, /* ��: GB2312 Code: 0xE4DD ==> Row:68 Col:61 */
    {0x6F2F,0xF0E4}, /* ��: GB2312 Code: 0xE4F0 ==> Row:68 Col:80 */
    {0x6F31,0xFECA}, /* ��: GB2312 Code: 0xCAFE ==> Row:42 Col:94 */
    {0x6F33,0xC4D5}, /* ��: GB2312 Code: 0xD5C4 ==> Row:53 Col:36 */
    {0x6F36,0xF1E4}, /* ��: GB2312 Code: 0xE4F1 ==> Row:68 Col:81 */
    {0x6F3E,0xFAD1}, /* ��: GB2312 Code: 0xD1FA ==> Row:49 Col:90 */
    {0x6F46,0xEBE4}, /* ��: GB2312 Code: 0xE4EB ==> Row:68 Col:75 */
    {0x6F47,0xECE4}, /* ��: GB2312 Code: 0xE4EC ==> Row:68 Col:76 */
    {0x6F4B,0xF2E4}, /* ��: GB2312 Code: 0xE4F2 ==> Row:68 Col:82 */
    {0x6F4D,0xABCE}, /* Ϋ: GB2312 Code: 0xCEAB ==> Row:46 Col:11 */
    {0x6F58,0xCBC5}, /* ��: GB2312 Code: 0xC5CB ==> Row:37 Col:43 */
    {0x6F5C,0xB1C7}, /* Ǳ: GB2312 Code: 0xC7B1 ==> Row:39 Col:17 */
    {0x6F5E,0xBAC2}, /* º: GB2312 Code: 0xC2BA ==> Row:34 Col:26 */
    {0x6F62,0xEAE4}, /* ��: GB2312 Code: 0xE4EA ==> Row:68 Col:74 */
    {0x6F66,0xCAC1}, /* ��: GB2312 Code: 0xC1CA ==> Row:33 Col:42 */
    {0x6F6D,0xB6CC}, /* ̶: GB2312 Code: 0xCCB6 ==> Row:44 Col:22 */
    {0x6F6E,0xB1B3}, /* ��: GB2312 Code: 0xB3B1 ==> Row:19 Col:17 */
    {0x6F72,0xFBE4}, /* ��: GB2312 Code: 0xE4FB ==> Row:68 Col:91 */
    {0x6F74,0xF3E4}, /* ��: GB2312 Code: 0xE4F3 ==> Row:68 Col:83 */
    {0x6F78,0xFAE4}, /* ��: GB2312 Code: 0xE4FA ==> Row:68 Col:90 */
    {0x6F7A,0xFDE4}, /* ��: GB2312 Code: 0xE4FD ==> Row:68 Col:93 */
    {0x6F7C,0xFCE4}, /* ��: GB2312 Code: 0xE4FC ==> Row:68 Col:92 */
    {0x6F84,0xCEB3}, /* ��: GB2312 Code: 0xB3CE ==> Row:19 Col:46 */
    {0x6F88,0xBAB3}, /* ��: GB2312 Code: 0xB3BA ==> Row:19 Col:26 */
    {0x6F89,0xF7E4}, /* ��: GB2312 Code: 0xE4F7 ==> Row:68 Col:87 */
    {0x6F8C,0xF9E4}, /* ��: GB2312 Code: 0xE4F9 ==> Row:68 Col:89 */
    {0x6F8D,0xF8E4}, /* ��: GB2312 Code: 0xE4F8 ==> Row:68 Col:88 */
    {0x6F8E,0xECC5}, /* ��: GB2312 Code: 0xC5EC ==> Row:37 Col:76 */
    {0x6F9C,0xBDC0}, /* ��: GB2312 Code: 0xC0BD ==> Row:32 Col:29 */
    {0x6FA1,0xE8D4}, /* ��: GB2312 Code: 0xD4E8 ==> Row:52 Col:72 */
    {0x6FA7,0xA2E5}, /* �: GB2312 Code: 0xE5A2 ==> Row:69 Col:02 */
    {0x6FB3,0xC4B0}, /* ��: GB2312 Code: 0xB0C4 ==> Row:16 Col:36 */
    {0x6FB6,0xA4E5}, /* �: GB2312 Code: 0xE5A4 ==> Row:69 Col:04 */
    {0x6FB9,0xA3E5}, /* �: GB2312 Code: 0xE5A3 ==> Row:69 Col:03 */
    {0x6FC0,0xA4BC}, /* ��: GB2312 Code: 0xBCA4 ==> Row:28 Col:04 */
    {0x6FC2,0xA5E5}, /* �: GB2312 Code: 0xE5A5 ==> Row:69 Col:05 */
    {0x6FC9,0xA1E5}, /* �: GB2312 Code: 0xE5A1 ==> Row:69 Col:01 */
    {0x6FD1,0xFEE4}, /* ��: GB2312 Code: 0xE4FE ==> Row:68 Col:94 */
    {0x6FD2,0xF4B1}, /* ��: GB2312 Code: 0xB1F4 ==> Row:17 Col:84 */
    {0x6FDE,0xA8E5}, /* �: GB2312 Code: 0xE5A8 ==> Row:69 Col:08 */
    {0x6FE0,0xA9E5}, /* �: GB2312 Code: 0xE5A9 ==> Row:69 Col:09 */
    {0x6FE1,0xA6E5}, /* �: GB2312 Code: 0xE5A6 ==> Row:69 Col:06 */
    {0x6FEE,0xA7E5}, /* �: GB2312 Code: 0xE5A7 ==> Row:69 Col:07 */
    {0x6FEF,0xAAE5}, /* �: GB2312 Code: 0xE5AA ==> Row:69 Col:10 */
    {0x7011,0xD9C6}, /* ��: GB2312 Code: 0xC6D9 ==> Row:38 Col:57 */
    {0x701A,0xABE5}, /* �: GB2312 Code: 0xE5AB ==> Row:69 Col:11 */
    {0x701B,0xADE5}, /* �: GB2312 Code: 0xE5AD ==> Row:69 Col:13 */
    {0x7023,0xACE5}, /* �: GB2312 Code: 0xE5AC ==> Row:69 Col:12 */
    {0x7035,0xAFE5}, /* �: GB2312 Code: 0xE5AF ==> Row:69 Col:15 */
    {0x7039,0xAEE5}, /* �: GB2312 Code: 0xE5AE ==> Row:69 Col:14 */
    {0x704C,0xE0B9}, /* ��: GB2312 Code: 0xB9E0 ==> Row:25 Col:64 */
    {0x704F,0xB0E5}, /* �: GB2312 Code: 0xE5B0 ==> Row:69 Col:16 */
    {0x705E,0xB1E5}, /* �: GB2312 Code: 0xE5B1 ==> Row:69 Col:17 */
    {0x706B,0xF0BB}, /* ��: GB2312 Code: 0xBBF0 ==> Row:27 Col:80 */
    {0x706C,0xE1EC}, /* ��: GB2312 Code: 0xECE1 ==> Row:76 Col:65 */
    {0x706D,0xF0C3}, /* ��: GB2312 Code: 0xC3F0 ==> Row:35 Col:80 */
    {0x706F,0xC6B5}, /* ��: GB2312 Code: 0xB5C6 ==> Row:21 Col:38 */
    {0x7070,0xD2BB}, /* ��: GB2312 Code: 0xBBD2 ==> Row:27 Col:50 */
    {0x7075,0xE9C1}, /* ��: GB2312 Code: 0xC1E9 ==> Row:33 Col:73 */
    {0x7076,0xEED4}, /* ��: GB2312 Code: 0xD4EE ==> Row:52 Col:78 */
    {0x7078,0xC4BE}, /* ��: GB2312 Code: 0xBEC4 ==> Row:30 Col:36 */
    {0x707C,0xC6D7}, /* ��: GB2312 Code: 0xD7C6 ==> Row:55 Col:38 */
    {0x707E,0xD6D4}, /* ��: GB2312 Code: 0xD4D6 ==> Row:52 Col:54 */
    {0x707F,0xD3B2}, /* ��: GB2312 Code: 0xB2D3 ==> Row:18 Col:51 */
    {0x7080,0xBEEC}, /* �: GB2312 Code: 0xECBE ==> Row:76 Col:30 */
    {0x7085,0xC1EA}, /* ��: GB2312 Code: 0xEAC1 ==> Row:74 Col:33 */
    {0x7089,0xAFC2}, /* ¯: GB2312 Code: 0xC2AF ==> Row:34 Col:15 */
    {0x708A,0xB6B4}, /* ��: GB2312 Code: 0xB4B6 ==> Row:20 Col:22 */
    {0x708E,0xD7D1}, /* ��: GB2312 Code: 0xD1D7 ==> Row:49 Col:55 */
    {0x7092,0xB4B3}, /* ��: GB2312 Code: 0xB3B4 ==> Row:19 Col:20 */
    {0x7094,0xB2C8}, /* Ȳ: GB2312 Code: 0xC8B2 ==> Row:40 Col:18 */
    {0x7095,0xBBBF}, /* ��: GB2312 Code: 0xBFBB ==> Row:31 Col:27 */
    {0x7096,0xC0EC}, /* ��: GB2312 Code: 0xECC0 ==> Row:76 Col:32 */
    {0x7099,0xCBD6}, /* ��: GB2312 Code: 0xD6CB ==> Row:54 Col:43 */
    {0x709C,0xBFEC}, /* �: GB2312 Code: 0xECBF ==> Row:76 Col:31 */
    {0x709D,0xC1EC}, /* ��: GB2312 Code: 0xECC1 ==> Row:76 Col:33 */
    {0x70AB,0xC5EC}, /* ��: GB2312 Code: 0xECC5 ==> Row:76 Col:37 */
    {0x70AC,0xE6BE}, /* ��: GB2312 Code: 0xBEE6 ==> Row:30 Col:70 */
    {0x70AD,0xBFCC}, /* ̿: GB2312 Code: 0xCCBF ==> Row:44 Col:31 */
    {0x70AE,0xDAC5}, /* ��: GB2312 Code: 0xC5DA ==> Row:37 Col:58 */
    {0x70AF,0xBCBE}, /* ��: GB2312 Code: 0xBEBC ==> Row:30 Col:28 */
    {0x70B1,0xC6EC}, /* ��: GB2312 Code: 0xECC6 ==> Row:76 Col:38 */
    {0x70B3,0xFEB1}, /* ��: GB2312 Code: 0xB1FE ==> Row:17 Col:94 */
    {0x70B7,0xC4EC}, /* ��: GB2312 Code: 0xECC4 ==> Row:76 Col:36 */
    {0x70B8,0xA8D5}, /* ը: GB2312 Code: 0xD5A8 ==> Row:53 Col:08 */
    {0x70B9,0xE3B5}, /* ��: GB2312 Code: 0xB5E3 ==> Row:21 Col:67 */
    {0x70BB,0xC2EC}, /* ��: GB2312 Code: 0xECC2 ==> Row:76 Col:34 */
    {0x70BC,0xB6C1}, /* ��: GB2312 Code: 0xC1B6 ==> Row:33 Col:22 */
    {0x70BD,0xE3B3}, /* ��: GB2312 Code: 0xB3E3 ==> Row:19 Col:67 */
    {0x70C0,0xC3EC}, /* ��: GB2312 Code: 0xECC3 ==> Row:76 Col:35 */
    {0x70C1,0xB8CB}, /* ˸: GB2312 Code: 0xCBB8 ==> Row:43 Col:24 */
    {0x70C2,0xC3C0}, /* ��: GB2312 Code: 0xC0C3 ==> Row:32 Col:35 */
    {0x70C3,0xFECC}, /* ��: GB2312 Code: 0xCCFE ==> Row:44 Col:94 */
    {0x70C8,0xD2C1}, /* ��: GB2312 Code: 0xC1D2 ==> Row:33 Col:50 */
    {0x70CA,0xC8EC}, /* ��: GB2312 Code: 0xECC8 ==> Row:76 Col:40 */
    {0x70D8,0xE6BA}, /* ��: GB2312 Code: 0xBAE6 ==> Row:26 Col:70 */
    {0x70D9,0xD3C0}, /* ��: GB2312 Code: 0xC0D3 ==> Row:32 Col:51 */
    {0x70DB,0xF2D6}, /* ��: GB2312 Code: 0xD6F2 ==> Row:54 Col:82 */
    {0x70DF,0xCCD1}, /* ��: GB2312 Code: 0xD1CC ==> Row:49 Col:44 */
    {0x70E4,0xBEBF}, /* ��: GB2312 Code: 0xBFBE ==> Row:31 Col:30 */
    {0x70E6,0xB3B7}, /* ��: GB2312 Code: 0xB7B3 ==> Row:23 Col:19 */
    {0x70E7,0xD5C9}, /* ��: GB2312 Code: 0xC9D5 ==> Row:41 Col:53 */
    {0x70E8,0xC7EC}, /* ��: GB2312 Code: 0xECC7 ==> Row:76 Col:39 */
    {0x70E9,0xE2BB}, /* ��: GB2312 Code: 0xBBE2 ==> Row:27 Col:66 */
    {0x70EB,0xCCCC}, /* ��: GB2312 Code: 0xCCCC ==> Row:44 Col:44 */
    {0x70EC,0xFDBD}, /* ��: GB2312 Code: 0xBDFD ==> Row:29 Col:93 */
    {0x70ED,0xC8C8}, /* ��: GB2312 Code: 0xC8C8 ==> Row:40 Col:40 */
    {0x70EF,0xA9CF}, /* ϩ: GB2312 Code: 0xCFA9 ==> Row:47 Col:09 */
    {0x70F7,0xE9CD}, /* ��: GB2312 Code: 0xCDE9 ==> Row:45 Col:73 */
    {0x70F9,0xEBC5}, /* ��: GB2312 Code: 0xC5EB ==> Row:37 Col:75 */
    {0x70FD,0xE9B7}, /* ��: GB2312 Code: 0xB7E9 ==> Row:23 Col:73 */
    {0x7109,0xC9D1}, /* ��: GB2312 Code: 0xD1C9 ==> Row:49 Col:41 */
    {0x710A,0xB8BA}, /* ��: GB2312 Code: 0xBAB8 ==> Row:26 Col:24 */
    {0x7110,0xC9EC}, /* ��: GB2312 Code: 0xECC9 ==> Row:76 Col:41 */
    {0x7113,0xCAEC}, /* ��: GB2312 Code: 0xECCA ==> Row:76 Col:42 */
    {0x7115,0xC0BB}, /* ��: GB2312 Code: 0xBBC0 ==> Row:27 Col:32 */
    {0x7116,0xCBEC}, /* ��: GB2312 Code: 0xECCB ==> Row:76 Col:43 */
    {0x7118,0xE2EC}, /* ��: GB2312 Code: 0xECE2 ==> Row:76 Col:66 */
    {0x7119,0xBAB1}, /* ��: GB2312 Code: 0xB1BA ==> Row:17 Col:26 */
    {0x711A,0xD9B7}, /* ��: GB2312 Code: 0xB7D9 ==> Row:23 Col:57 */
    {0x7126,0xB9BD}, /* ��: GB2312 Code: 0xBDB9 ==> Row:29 Col:25 */
    {0x712F,0xCCEC}, /* ��: GB2312 Code: 0xECCC ==> Row:76 Col:44 */
    {0x7130,0xE6D1}, /* ��: GB2312 Code: 0xD1E6 ==> Row:49 Col:70 */
    {0x7131,0xCDEC}, /* ��: GB2312 Code: 0xECCD ==> Row:76 Col:45 */
    {0x7136,0xBBC8}, /* Ȼ: GB2312 Code: 0xC8BB ==> Row:40 Col:27 */
    {0x7145,0xD1EC}, /* ��: GB2312 Code: 0xECD1 ==> Row:76 Col:49 */
    {0x714A,0xD3EC}, /* ��: GB2312 Code: 0xECD3 ==> Row:76 Col:51 */
    {0x714C,0xCDBB}, /* ��: GB2312 Code: 0xBBCD ==> Row:27 Col:45 */
    {0x714E,0xE5BC}, /* ��: GB2312 Code: 0xBCE5 ==> Row:28 Col:69 */
    {0x715C,0xCFEC}, /* ��: GB2312 Code: 0xECCF ==> Row:76 Col:47 */
    {0x715E,0xB7C9}, /* ɷ: GB2312 Code: 0xC9B7 ==> Row:41 Col:23 */
    {0x7164,0xBAC3}, /* ú: GB2312 Code: 0xC3BA ==> Row:35 Col:26 */
    {0x7166,0xE3EC}, /* ��: GB2312 Code: 0xECE3 ==> Row:76 Col:67 */
    {0x7167,0xD5D5}, /* ��: GB2312 Code: 0xD5D5 ==> Row:53 Col:53 */
    {0x7168,0xD0EC}, /* ��: GB2312 Code: 0xECD0 ==> Row:76 Col:48 */
    {0x716E,0xF3D6}, /* ��: GB2312 Code: 0xD6F3 ==> Row:54 Col:83 */
    {0x7172,0xD2EC}, /* ��: GB2312 Code: 0xECD2 ==> Row:76 Col:50 */
    {0x7173,0xCEEC}, /* ��: GB2312 Code: 0xECCE ==> Row:76 Col:46 */
    {0x7178,0xD4EC}, /* ��: GB2312 Code: 0xECD4 ==> Row:76 Col:52 */
    {0x717A,0xD5EC}, /* ��: GB2312 Code: 0xECD5 ==> Row:76 Col:53 */
    {0x717D,0xBFC9}, /* ɿ: GB2312 Code: 0xC9BF ==> Row:41 Col:31 */
    {0x7184,0xA8CF}, /* Ϩ: GB2312 Code: 0xCFA8 ==> Row:47 Col:08 */
    {0x718A,0xDCD0}, /* ��: GB2312 Code: 0xD0DC ==> Row:48 Col:60 */
    {0x718F,0xACD1}, /* Ѭ: GB2312 Code: 0xD1AC ==> Row:49 Col:12 */
    {0x7194,0xDBC8}, /* ��: GB2312 Code: 0xC8DB ==> Row:40 Col:59 */
    {0x7198,0xD6EC}, /* ��: GB2312 Code: 0xECD6 ==> Row:76 Col:54 */
    {0x7199,0xF5CE}, /* ��: GB2312 Code: 0xCEF5 ==> Row:46 Col:85 */
    {0x719F,0xECCA}, /* ��: GB2312 Code: 0xCAEC ==> Row:42 Col:76 */
    {0x71A0,0xDAEC}, /* ��: GB2312 Code: 0xECDA ==> Row:76 Col:58 */
    {0x71A8,0xD9EC}, /* ��: GB2312 Code: 0xECD9 ==> Row:76 Col:57 */
    {0x71AC,0xBEB0}, /* ��: GB2312 Code: 0xB0BE ==> Row:16 Col:30 */
    {0x71B3,0xD7EC}, /* ��: GB2312 Code: 0xECD7 ==> Row:76 Col:55 */
    {0x71B5,0xD8EC}, /* ��: GB2312 Code: 0xECD8 ==> Row:76 Col:56 */
    {0x71B9,0xE4EC}, /* ��: GB2312 Code: 0xECE4 ==> Row:76 Col:68 */
    {0x71C3,0xBCC8}, /* ȼ: GB2312 Code: 0xC8BC ==> Row:40 Col:28 */
    {0x71CE,0xC7C1}, /* ��: GB2312 Code: 0xC1C7 ==> Row:33 Col:39 */
    {0x71D4,0xDCEC}, /* ��: GB2312 Code: 0xECDC ==> Row:76 Col:60 */
    {0x71D5,0xE0D1}, /* ��: GB2312 Code: 0xD1E0 ==> Row:49 Col:64 */
    {0x71E0,0xDBEC}, /* ��: GB2312 Code: 0xECDB ==> Row:76 Col:59 */
    {0x71E5,0xEFD4}, /* ��: GB2312 Code: 0xD4EF ==> Row:52 Col:79 */
    {0x71E7,0xDDEC}, /* ��: GB2312 Code: 0xECDD ==> Row:76 Col:61 */
    {0x71EE,0xC6DB}, /* ��: GB2312 Code: 0xDBC6 ==> Row:59 Col:38 */
    {0x71F9,0xDEEC}, /* ��: GB2312 Code: 0xECDE ==> Row:76 Col:62 */
    {0x7206,0xACB1}, /* ��: GB2312 Code: 0xB1AC ==> Row:17 Col:12 */
    {0x721D,0xDFEC}, /* ��: GB2312 Code: 0xECDF ==> Row:76 Col:63 */
    {0x7228,0xE0EC}, /* ��: GB2312 Code: 0xECE0 ==> Row:76 Col:64 */
    {0x722A,0xA6D7}, /* צ: GB2312 Code: 0xD7A6 ==> Row:55 Col:06 */
    {0x722C,0xC0C5}, /* ��: GB2312 Code: 0xC5C0 ==> Row:37 Col:32 */
    {0x7230,0xBCEB}, /* �: GB2312 Code: 0xEBBC ==> Row:75 Col:28 */
    {0x7231,0xAEB0}, /* ��: GB2312 Code: 0xB0AE ==> Row:16 Col:14 */
    {0x7235,0xF4BE}, /* ��: GB2312 Code: 0xBEF4 ==> Row:30 Col:84 */
    {0x7236,0xB8B8}, /* ��: GB2312 Code: 0xB8B8 ==> Row:24 Col:24 */
    {0x7237,0xAFD2}, /* ү: GB2312 Code: 0xD2AF ==> Row:50 Col:15 */
    {0x7238,0xD6B0}, /* ��: GB2312 Code: 0xB0D6 ==> Row:16 Col:54 */
    {0x7239,0xF9B5}, /* ��: GB2312 Code: 0xB5F9 ==> Row:21 Col:89 */
    {0x723B,0xB3D8}, /* س: GB2312 Code: 0xD8B3 ==> Row:56 Col:19 */
    {0x723D,0xACCB}, /* ˬ: GB2312 Code: 0xCBAC ==> Row:43 Col:12 */
    {0x723F,0xDDE3}, /* ��: GB2312 Code: 0xE3DD ==> Row:67 Col:61 */
    {0x7247,0xACC6}, /* Ƭ: GB2312 Code: 0xC6AC ==> Row:38 Col:12 */
    {0x7248,0xE6B0}, /* ��: GB2312 Code: 0xB0E6 ==> Row:16 Col:70 */
    {0x724C,0xC6C5}, /* ��: GB2312 Code: 0xC5C6 ==> Row:37 Col:38 */
    {0x724D,0xB9EB}, /* �: GB2312 Code: 0xEBB9 ==> Row:75 Col:25 */
    {0x7252,0xBAEB}, /* �: GB2312 Code: 0xEBBA ==> Row:75 Col:26 */
    {0x7256,0xBBEB}, /* �: GB2312 Code: 0xEBBB ==> Row:75 Col:27 */
    {0x7259,0xC0D1}, /* ��: GB2312 Code: 0xD1C0 ==> Row:49 Col:32 */
    {0x725B,0xA3C5}, /* ţ: GB2312 Code: 0xC5A3 ==> Row:37 Col:03 */
    {0x725D,0xF2EA}, /* ��: GB2312 Code: 0xEAF2 ==> Row:74 Col:82 */
    {0x725F,0xB2C4}, /* Ĳ: GB2312 Code: 0xC4B2 ==> Row:36 Col:18 */
    {0x7261,0xB5C4}, /* ĵ: GB2312 Code: 0xC4B5 ==> Row:36 Col:21 */
    {0x7262,0xCEC0}, /* ��: GB2312 Code: 0xC0CE ==> Row:32 Col:46 */
    {0x7266,0xF3EA}, /* ��: GB2312 Code: 0xEAF3 ==> Row:74 Col:83 */
    {0x7267,0xC1C4}, /* ��: GB2312 Code: 0xC4C1 ==> Row:36 Col:33 */
    {0x7269,0xEFCE}, /* ��: GB2312 Code: 0xCEEF ==> Row:46 Col:79 */
    {0x726E,0xF0EA}, /* ��: GB2312 Code: 0xEAF0 ==> Row:74 Col:80 */
    {0x726F,0xF4EA}, /* ��: GB2312 Code: 0xEAF4 ==> Row:74 Col:84 */
    {0x7272,0xFCC9}, /* ��: GB2312 Code: 0xC9FC ==> Row:41 Col:92 */
    {0x7275,0xA3C7}, /* ǣ: GB2312 Code: 0xC7A3 ==> Row:39 Col:03 */
    {0x7279,0xD8CC}, /* ��: GB2312 Code: 0xCCD8 ==> Row:44 Col:56 */
    {0x727A,0xFECE}, /* ��: GB2312 Code: 0xCEFE ==> Row:46 Col:94 */
    {0x727E,0xF5EA}, /* ��: GB2312 Code: 0xEAF5 ==> Row:74 Col:85 */
    {0x727F,0xF6EA}, /* ��: GB2312 Code: 0xEAF6 ==> Row:74 Col:86 */
    {0x7280,0xACCF}, /* Ϭ: GB2312 Code: 0xCFAC ==> Row:47 Col:12 */
    {0x7281,0xE7C0}, /* ��: GB2312 Code: 0xC0E7 ==> Row:32 Col:71 */
    {0x7284,0xF7EA}, /* ��: GB2312 Code: 0xEAF7 ==> Row:74 Col:87 */
    {0x728A,0xBFB6}, /* ��: GB2312 Code: 0xB6BF ==> Row:22 Col:31 */
    {0x728B,0xF8EA}, /* ��: GB2312 Code: 0xEAF8 ==> Row:74 Col:88 */
    {0x728D,0xF9EA}, /* ��: GB2312 Code: 0xEAF9 ==> Row:74 Col:89 */
    {0x728F,0xFAEA}, /* ��: GB2312 Code: 0xEAFA ==> Row:74 Col:90 */
    {0x7292,0xFBEA}, /* ��: GB2312 Code: 0xEAFB ==> Row:74 Col:91 */
    {0x729F,0xF1EA}, /* ��: GB2312 Code: 0xEAF1 ==> Row:74 Col:81 */
    {0x72AC,0xAEC8}, /* Ȯ: GB2312 Code: 0xC8AE ==> Row:40 Col:14 */
    {0x72AD,0xEBE1}, /* ��: GB2312 Code: 0xE1EB ==> Row:65 Col:75 */
    {0x72AF,0xB8B7}, /* ��: GB2312 Code: 0xB7B8 ==> Row:23 Col:24 */
    {0x72B0,0xECE1}, /* ��: GB2312 Code: 0xE1EC ==> Row:65 Col:76 */
    {0x72B4,0xEDE1}, /* ��: GB2312 Code: 0xE1ED ==> Row:65 Col:77 */
    {0x72B6,0xB4D7}, /* ״: GB2312 Code: 0xD7B4 ==> Row:55 Col:20 */
    {0x72B7,0xEEE1}, /* ��: GB2312 Code: 0xE1EE ==> Row:65 Col:78 */
    {0x72B8,0xEFE1}, /* ��: GB2312 Code: 0xE1EF ==> Row:65 Col:79 */
    {0x72B9,0xCCD3}, /* ��: GB2312 Code: 0xD3CC ==> Row:51 Col:44 */
    {0x72C1,0xF1E1}, /* ��: GB2312 Code: 0xE1F1 ==> Row:65 Col:81 */
    {0x72C2,0xF1BF}, /* ��: GB2312 Code: 0xBFF1 ==> Row:31 Col:81 */
    {0x72C3,0xF0E1}, /* ��: GB2312 Code: 0xE1F0 ==> Row:65 Col:80 */
    {0x72C4,0xD2B5}, /* ��: GB2312 Code: 0xB5D2 ==> Row:21 Col:50 */
    {0x72C8,0xB7B1}, /* ��: GB2312 Code: 0xB1B7 ==> Row:17 Col:23 */
    {0x72CD,0xF3E1}, /* ��: GB2312 Code: 0xE1F3 ==> Row:65 Col:83 */
    {0x72CE,0xF2E1}, /* ��: GB2312 Code: 0xE1F2 ==> Row:65 Col:82 */
    {0x72D0,0xFCBA}, /* ��: GB2312 Code: 0xBAFC ==> Row:26 Col:92 */
    {0x72D2,0xF4E1}, /* ��: GB2312 Code: 0xE1F4 ==> Row:65 Col:84 */
    {0x72D7,0xB7B9}, /* ��: GB2312 Code: 0xB9B7 ==> Row:25 Col:23 */
    {0x72D9,0xD1BE}, /* ��: GB2312 Code: 0xBED1 ==> Row:30 Col:49 */
    {0x72DE,0xFCC4}, /* ��: GB2312 Code: 0xC4FC ==> Row:36 Col:92 */
    {0x72E0,0xDDBA}, /* ��: GB2312 Code: 0xBADD ==> Row:26 Col:61 */
    {0x72E1,0xC6BD}, /* ��: GB2312 Code: 0xBDC6 ==> Row:29 Col:38 */
    {0x72E8,0xF5E1}, /* ��: GB2312 Code: 0xE1F5 ==> Row:65 Col:85 */
    {0x72E9,0xF7E1}, /* ��: GB2312 Code: 0xE1F7 ==> Row:65 Col:87 */
    {0x72EC,0xC0B6}, /* ��: GB2312 Code: 0xB6C0 ==> Row:22 Col:32 */
    {0x72ED,0xC1CF}, /* ��: GB2312 Code: 0xCFC1 ==> Row:47 Col:33 */
    {0x72EE,0xA8CA}, /* ʨ: GB2312 Code: 0xCAA8 ==> Row:42 Col:08 */
    {0x72EF,0xF6E1}, /* ��: GB2312 Code: 0xE1F6 ==> Row:65 Col:86 */
    {0x72F0,0xF8D5}, /* ��: GB2312 Code: 0xD5F8 ==> Row:53 Col:88 */
    {0x72F1,0xFCD3}, /* ��: GB2312 Code: 0xD3FC ==> Row:51 Col:92 */
    {0x72F2,0xF8E1}, /* ��: GB2312 Code: 0xE1F8 ==> Row:65 Col:88 */
    {0x72F3,0xFCE1}, /* ��: GB2312 Code: 0xE1FC ==> Row:65 Col:92 */
    {0x72F4,0xF9E1}, /* ��: GB2312 Code: 0xE1F9 ==> Row:65 Col:89 */
    {0x72F7,0xFAE1}, /* ��: GB2312 Code: 0xE1FA ==> Row:65 Col:90 */
    {0x72F8,0xEAC0}, /* ��: GB2312 Code: 0xC0EA ==> Row:32 Col:74 */
    {0x72FA,0xFEE1}, /* ��: GB2312 Code: 0xE1FE ==> Row:65 Col:94 */
    {0x72FB,0xA1E2}, /* �: GB2312 Code: 0xE2A1 ==> Row:66 Col:01 */
    {0x72FC,0xC7C0}, /* ��: GB2312 Code: 0xC0C7 ==> Row:32 Col:39 */
    {0x7301,0xFBE1}, /* ��: GB2312 Code: 0xE1FB ==> Row:65 Col:91 */
    {0x7303,0xFDE1}, /* ��: GB2312 Code: 0xE1FD ==> Row:65 Col:93 */
    {0x730A,0xA5E2}, /* �: GB2312 Code: 0xE2A5 ==> Row:66 Col:05 */
    {0x730E,0xD4C1}, /* ��: GB2312 Code: 0xC1D4 ==> Row:33 Col:52 */
    {0x7313,0xA3E2}, /* �: GB2312 Code: 0xE2A3 ==> Row:66 Col:03 */
    {0x7315,0xA8E2}, /* �: GB2312 Code: 0xE2A8 ==> Row:66 Col:08 */
    {0x7316,0xFEB2}, /* ��: GB2312 Code: 0xB2FE ==> Row:18 Col:94 */
    {0x7317,0xA2E2}, /* �: GB2312 Code: 0xE2A2 ==> Row:66 Col:02 */
    {0x731B,0xCDC3}, /* ��: GB2312 Code: 0xC3CD ==> Row:35 Col:45 */
    {0x731C,0xC2B2}, /* ��: GB2312 Code: 0xB2C2 ==> Row:18 Col:34 */
    {0x731D,0xA7E2}, /* �: GB2312 Code: 0xE2A7 ==> Row:66 Col:07 */
    {0x731E,0xA6E2}, /* �: GB2312 Code: 0xE2A6 ==> Row:66 Col:06 */
    {0x7321,0xA4E2}, /* �: GB2312 Code: 0xE2A4 ==> Row:66 Col:04 */
    {0x7322,0xA9E2}, /* �: GB2312 Code: 0xE2A9 ==> Row:66 Col:09 */
    {0x7325,0xABE2}, /* �: GB2312 Code: 0xE2AB ==> Row:66 Col:11 */
    {0x7329,0xC9D0}, /* ��: GB2312 Code: 0xD0C9 ==> Row:48 Col:41 */
    {0x732A,0xEDD6}, /* ��: GB2312 Code: 0xD6ED ==> Row:54 Col:77 */
    {0x732B,0xA8C3}, /* è: GB2312 Code: 0xC3A8 ==> Row:35 Col:08 */
    {0x732C,0xACE2}, /* �: GB2312 Code: 0xE2AC ==> Row:66 Col:12 */
    {0x732E,0xD7CF}, /* ��: GB2312 Code: 0xCFD7 ==> Row:47 Col:55 */
    {0x7331,0xAEE2}, /* �: GB2312 Code: 0xE2AE ==> Row:66 Col:14 */
    {0x7334,0xEFBA}, /* ��: GB2312 Code: 0xBAEF ==> Row:26 Col:79 */
    {0x7337,0xE0E9}, /* ��: GB2312 Code: 0xE9E0 ==> Row:73 Col:64 */
    {0x7338,0xADE2}, /* �: GB2312 Code: 0xE2AD ==> Row:66 Col:13 */
    {0x7339,0xAAE2}, /* �: GB2312 Code: 0xE2AA ==> Row:66 Col:10 */
    {0x733E,0xABBB}, /* ��: GB2312 Code: 0xBBAB ==> Row:27 Col:11 */
    {0x733F,0xB3D4}, /* Գ: GB2312 Code: 0xD4B3 ==> Row:52 Col:19 */
    {0x734D,0xB0E2}, /* �: GB2312 Code: 0xE2B0 ==> Row:66 Col:16 */
    {0x7350,0xAFE2}, /* �: GB2312 Code: 0xE2AF ==> Row:66 Col:15 */
    {0x7352,0xE1E9}, /* ��: GB2312 Code: 0xE9E1 ==> Row:73 Col:65 */
    {0x7357,0xB1E2}, /* �: GB2312 Code: 0xE2B1 ==> Row:66 Col:17 */
    {0x7360,0xB2E2}, /* �: GB2312 Code: 0xE2B2 ==> Row:66 Col:18 */
    {0x736C,0xB3E2}, /* �: GB2312 Code: 0xE2B3 ==> Row:66 Col:19 */
    {0x736D,0xA1CC}, /* ̡: GB2312 Code: 0xCCA1 ==> Row:44 Col:01 */
    {0x736F,0xB4E2}, /* �: GB2312 Code: 0xE2B4 ==> Row:66 Col:20 */
    {0x737E,0xB5E2}, /* �: GB2312 Code: 0xE2B5 ==> Row:66 Col:21 */
    {0x7384,0xFED0}, /* ��: GB2312 Code: 0xD0FE ==> Row:48 Col:94 */
    {0x7387,0xCAC2}, /* ��: GB2312 Code: 0xC2CA ==> Row:34 Col:42 */
    {0x7389,0xF1D3}, /* ��: GB2312 Code: 0xD3F1 ==> Row:51 Col:81 */
    {0x738B,0xF5CD}, /* ��: GB2312 Code: 0xCDF5 ==> Row:45 Col:85 */
    {0x738E,0xE0E7}, /* ��: GB2312 Code: 0xE7E0 ==> Row:71 Col:64 */
    {0x7391,0xE1E7}, /* ��: GB2312 Code: 0xE7E1 ==> Row:71 Col:65 */
    {0x7396,0xC1BE}, /* ��: GB2312 Code: 0xBEC1 ==> Row:30 Col:33 */
    {0x739B,0xEAC2}, /* ��: GB2312 Code: 0xC2EA ==> Row:34 Col:74 */
    {0x739F,0xE4E7}, /* ��: GB2312 Code: 0xE7E4 ==> Row:71 Col:68 */
    {0x73A2,0xE3E7}, /* ��: GB2312 Code: 0xE7E3 ==> Row:71 Col:67 */
    {0x73A9,0xE6CD}, /* ��: GB2312 Code: 0xCDE6 ==> Row:45 Col:70 */
    {0x73AB,0xB5C3}, /* õ: GB2312 Code: 0xC3B5 ==> Row:35 Col:21 */
    {0x73AE,0xE2E7}, /* ��: GB2312 Code: 0xE7E2 ==> Row:71 Col:66 */
    {0x73AF,0xB7BB}, /* ��: GB2312 Code: 0xBBB7 ==> Row:27 Col:23 */
    {0x73B0,0xD6CF}, /* ��: GB2312 Code: 0xCFD6 ==> Row:47 Col:54 */
    {0x73B2,0xE1C1}, /* ��: GB2312 Code: 0xC1E1 ==> Row:33 Col:65 */
    {0x73B3,0xE9E7}, /* ��: GB2312 Code: 0xE7E9 ==> Row:71 Col:73 */
    {0x73B7,0xE8E7}, /* ��: GB2312 Code: 0xE7E8 ==> Row:71 Col:72 */
    {0x73BA,0xF4E7}, /* ��: GB2312 Code: 0xE7F4 ==> Row:71 Col:84 */
    {0x73BB,0xA3B2}, /* ��: GB2312 Code: 0xB2A3 ==> Row:18 Col:03 */
    {0x73C0,0xEAE7}, /* ��: GB2312 Code: 0xE7EA ==> Row:71 Col:74 */
    {0x73C2,0xE6E7}, /* ��: GB2312 Code: 0xE7E6 ==> Row:71 Col:70 */
    {0x73C8,0xECE7}, /* ��: GB2312 Code: 0xE7EC ==> Row:71 Col:76 */
    {0x73C9,0xEBE7}, /* ��: GB2312 Code: 0xE7EB ==> Row:71 Col:75 */
    {0x73CA,0xBAC9}, /* ɺ: GB2312 Code: 0xC9BA ==> Row:41 Col:26 */
    {0x73CD,0xE4D5}, /* ��: GB2312 Code: 0xD5E4 ==> Row:53 Col:68 */
    {0x73CF,0xE5E7}, /* ��: GB2312 Code: 0xE7E5 ==> Row:71 Col:69 */
    {0x73D0,0xA9B7}, /* ��: GB2312 Code: 0xB7A9 ==> Row:23 Col:09 */
    {0x73D1,0xE7E7}, /* ��: GB2312 Code: 0xE7E7 ==> Row:71 Col:71 */
    {0x73D9,0xEEE7}, /* ��: GB2312 Code: 0xE7EE ==> Row:71 Col:78 */
    {0x73DE,0xF3E7}, /* ��: GB2312 Code: 0xE7F3 ==> Row:71 Col:83 */
    {0x73E0,0xE9D6}, /* ��: GB2312 Code: 0xD6E9 ==> Row:54 Col:73 */
    {0x73E5,0xEDE7}, /* ��: GB2312 Code: 0xE7ED ==> Row:71 Col:77 */
    {0x73E7,0xF2E7}, /* ��: GB2312 Code: 0xE7F2 ==> Row:71 Col:82 */
    {0x73E9,0xF1E7}, /* ��: GB2312 Code: 0xE7F1 ==> Row:71 Col:81 */
    {0x73ED,0xE0B0}, /* ��: GB2312 Code: 0xB0E0 ==> Row:16 Col:64 */
    {0x73F2,0xF5E7}, /* ��: GB2312 Code: 0xE7F5 ==> Row:71 Col:85 */
    {0x7403,0xF2C7}, /* ��: GB2312 Code: 0xC7F2 ==> Row:39 Col:82 */
    {0x7405,0xC5C0}, /* ��: GB2312 Code: 0xC0C5 ==> Row:32 Col:37 */
    {0x7406,0xEDC0}, /* ��: GB2312 Code: 0xC0ED ==> Row:32 Col:77 */
    {0x7409,0xF0C1}, /* ��: GB2312 Code: 0xC1F0 ==> Row:33 Col:80 */
    {0x740A,0xF0E7}, /* ��: GB2312 Code: 0xE7F0 ==> Row:71 Col:80 */
    {0x740F,0xF6E7}, /* ��: GB2312 Code: 0xE7F6 ==> Row:71 Col:86 */
    {0x7410,0xF6CB}, /* ��: GB2312 Code: 0xCBF6 ==> Row:43 Col:86 */
    {0x741A,0xA2E8}, /* �: GB2312 Code: 0xE8A2 ==> Row:72 Col:02 */
    {0x741B,0xA1E8}, /* �: GB2312 Code: 0xE8A1 ==> Row:72 Col:01 */
    {0x7422,0xC1D7}, /* ��: GB2312 Code: 0xD7C1 ==> Row:55 Col:33 */
    {0x7425,0xFAE7}, /* ��: GB2312 Code: 0xE7FA ==> Row:71 Col:90 */
    {0x7426,0xF9E7}, /* ��: GB2312 Code: 0xE7F9 ==> Row:71 Col:89 */
    {0x7428,0xFBE7}, /* ��: GB2312 Code: 0xE7FB ==> Row:71 Col:91 */
    {0x742A,0xF7E7}, /* ��: GB2312 Code: 0xE7F7 ==> Row:71 Col:87 */
    {0x742C,0xFEE7}, /* ��: GB2312 Code: 0xE7FE ==> Row:71 Col:94 */
    {0x742E,0xFDE7}, /* ��: GB2312 Code: 0xE7FD ==> Row:71 Col:93 */
    {0x7430,0xFCE7}, /* ��: GB2312 Code: 0xE7FC ==> Row:71 Col:92 */
    {0x7433,0xD5C1}, /* ��: GB2312 Code: 0xC1D5 ==> Row:33 Col:53 */
    {0x7434,0xD9C7}, /* ��: GB2312 Code: 0xC7D9 ==> Row:39 Col:57 */
    {0x7435,0xFDC5}, /* ��: GB2312 Code: 0xC5FD ==> Row:37 Col:93 */
    {0x7436,0xC3C5}, /* ��: GB2312 Code: 0xC5C3 ==> Row:37 Col:35 */
    {0x743C,0xEDC7}, /* ��: GB2312 Code: 0xC7ED ==> Row:39 Col:77 */
    {0x7441,0xA3E8}, /* �: GB2312 Code: 0xE8A3 ==> Row:72 Col:03 */
    {0x7455,0xA6E8}, /* �: GB2312 Code: 0xE8A6 ==> Row:72 Col:06 */
    {0x7457,0xA5E8}, /* �: GB2312 Code: 0xE8A5 ==> Row:72 Col:05 */
    {0x7459,0xA7E8}, /* �: GB2312 Code: 0xE8A7 ==> Row:72 Col:07 */
    {0x745A,0xF7BA}, /* ��: GB2312 Code: 0xBAF7 ==> Row:26 Col:87 */
    {0x745B,0xF8E7}, /* ��: GB2312 Code: 0xE7F8 ==> Row:71 Col:88 */
    {0x745C,0xA4E8}, /* �: GB2312 Code: 0xE8A4 ==> Row:72 Col:04 */
    {0x745E,0xF0C8}, /* ��: GB2312 Code: 0xC8F0 ==> Row:40 Col:80 */
    {0x745F,0xAAC9}, /* ɪ: GB2312 Code: 0xC9AA ==> Row:41 Col:10 */
    {0x746D,0xA9E8}, /* �: GB2312 Code: 0xE8A9 ==> Row:72 Col:09 */
    {0x7470,0xE5B9}, /* ��: GB2312 Code: 0xB9E5 ==> Row:25 Col:69 */
    {0x7476,0xFED1}, /* ��: GB2312 Code: 0xD1FE ==> Row:49 Col:94 */
    {0x7477,0xA8E8}, /* �: GB2312 Code: 0xE8A8 ==> Row:72 Col:08 */
    {0x747E,0xAAE8}, /* �: GB2312 Code: 0xE8AA ==> Row:72 Col:10 */
    {0x7480,0xADE8}, /* �: GB2312 Code: 0xE8AD ==> Row:72 Col:13 */
    {0x7481,0xAEE8}, /* �: GB2312 Code: 0xE8AE ==> Row:72 Col:14 */
    {0x7483,0xA7C1}, /* ��: GB2312 Code: 0xC1A7 ==> Row:33 Col:07 */
    {0x7487,0xAFE8}, /* �: GB2312 Code: 0xE8AF ==> Row:72 Col:15 */
    {0x748B,0xB0E8}, /* �: GB2312 Code: 0xE8B0 ==> Row:72 Col:16 */
    {0x748E,0xACE8}, /* �: GB2312 Code: 0xE8AC ==> Row:72 Col:12 */
    {0x7490,0xB4E8}, /* �: GB2312 Code: 0xE8B4 ==> Row:72 Col:20 */
    {0x749C,0xABE8}, /* �: GB2312 Code: 0xE8AB ==> Row:72 Col:11 */
    {0x749E,0xB1E8}, /* �: GB2312 Code: 0xE8B1 ==> Row:72 Col:17 */
    {0x74A7,0xB5E8}, /* �: GB2312 Code: 0xE8B5 ==> Row:72 Col:21 */
    {0x74A8,0xB2E8}, /* �: GB2312 Code: 0xE8B2 ==> Row:72 Col:18 */
    {0x74A9,0xB3E8}, /* �: GB2312 Code: 0xE8B3 ==> Row:72 Col:19 */
    {0x74BA,0xB7E8}, /* �: GB2312 Code: 0xE8B7 ==> Row:72 Col:23 */
    {0x74D2,0xB6E8}, /* �: GB2312 Code: 0xE8B6 ==> Row:72 Col:22 */
    {0x74DC,0xCFB9}, /* ��: GB2312 Code: 0xB9CF ==> Row:25 Col:47 */
    {0x74DE,0xACF0}, /* �: GB2312 Code: 0xF0AC ==> Row:80 Col:12 */
    {0x74E0,0xADF0}, /* �: GB2312 Code: 0xF0AD ==> Row:80 Col:13 */
    {0x74E2,0xB0C6}, /* ư: GB2312 Code: 0xC6B0 ==> Row:38 Col:16 */
    {0x74E3,0xEAB0}, /* ��: GB2312 Code: 0xB0EA ==> Row:16 Col:74 */
    {0x74E4,0xBFC8}, /* ȿ: GB2312 Code: 0xC8BF ==> Row:40 Col:31 */
    {0x74E6,0xDFCD}, /* ��: GB2312 Code: 0xCDDF ==> Row:45 Col:63 */
    {0x74EE,0xCDCE}, /* ��: GB2312 Code: 0xCECD ==> Row:46 Col:45 */
    {0x74EF,0xB1EA}, /* �: GB2312 Code: 0xEAB1 ==> Row:74 Col:17 */
    {0x74F4,0xB2EA}, /* �: GB2312 Code: 0xEAB2 ==> Row:74 Col:18 */
    {0x74F6,0xBFC6}, /* ƿ: GB2312 Code: 0xC6BF ==> Row:38 Col:31 */
    {0x74F7,0xC9B4}, /* ��: GB2312 Code: 0xB4C9 ==> Row:20 Col:41 */
    {0x74FF,0xB3EA}, /* �: GB2312 Code: 0xEAB3 ==> Row:74 Col:19 */
    {0x7504,0xE7D5}, /* ��: GB2312 Code: 0xD5E7 ==> Row:53 Col:71 */
    {0x750D,0xF9DD}, /* ��: GB2312 Code: 0xDDF9 ==> Row:61 Col:89 */
    {0x750F,0xB4EA}, /* �: GB2312 Code: 0xEAB4 ==> Row:74 Col:20 */
    {0x7511,0xB5EA}, /* �: GB2312 Code: 0xEAB5 ==> Row:74 Col:21 */
    {0x7513,0xB6EA}, /* �: GB2312 Code: 0xEAB6 ==> Row:74 Col:22 */
    {0x7518,0xCAB8}, /* ��: GB2312 Code: 0xB8CA ==> Row:24 Col:42 */
    {0x7519,0xB0DF}, /* ߰: GB2312 Code: 0xDFB0 ==> Row:63 Col:16 */
    {0x751A,0xF5C9}, /* ��: GB2312 Code: 0xC9F5 ==> Row:41 Col:85 */
    {0x751C,0xF0CC}, /* ��: GB2312 Code: 0xCCF0 ==> Row:44 Col:80 */
    {0x751F,0xFAC9}, /* ��: GB2312 Code: 0xC9FA ==> Row:41 Col:90 */
    {0x7525,0xFBC9}, /* ��: GB2312 Code: 0xC9FB ==> Row:41 Col:91 */
    {0x7528,0xC3D3}, /* ��: GB2312 Code: 0xD3C3 ==> Row:51 Col:35 */
    {0x7529,0xA6CB}, /* ˦: GB2312 Code: 0xCBA6 ==> Row:43 Col:06 */
    {0x752B,0xA6B8}, /* ��: GB2312 Code: 0xB8A6 ==> Row:24 Col:06 */
    {0x752C,0xAEF0}, /* �: GB2312 Code: 0xF0AE ==> Row:80 Col:14 */
    {0x752D,0xC2B1}, /* ��: GB2312 Code: 0xB1C2 ==> Row:17 Col:34 */
    {0x752F,0xB8E5}, /* �: GB2312 Code: 0xE5B8 ==> Row:69 Col:24 */
    {0x7530,0xEFCC}, /* ��: GB2312 Code: 0xCCEF ==> Row:44 Col:79 */
    {0x7531,0xC9D3}, /* ��: GB2312 Code: 0xD3C9 ==> Row:51 Col:41 */
    {0x7532,0xD7BC}, /* ��: GB2312 Code: 0xBCD7 ==> Row:28 Col:55 */
    {0x7533,0xEAC9}, /* ��: GB2312 Code: 0xC9EA ==> Row:41 Col:74 */
    {0x7535,0xE7B5}, /* ��: GB2312 Code: 0xB5E7 ==> Row:21 Col:71 */
    {0x7537,0xD0C4}, /* ��: GB2312 Code: 0xC4D0 ==> Row:36 Col:48 */
    {0x7538,0xE9B5}, /* ��: GB2312 Code: 0xB5E9 ==> Row:21 Col:73 */
    {0x753A,0xAEEE}, /* �: GB2312 Code: 0xEEAE ==> Row:78 Col:14 */
    {0x753B,0xADBB}, /* ��: GB2312 Code: 0xBBAD ==> Row:27 Col:13 */
    {0x753E,0xDEE7}, /* ��: GB2312 Code: 0xE7DE ==> Row:71 Col:62 */
    {0x7540,0xAFEE}, /* �: GB2312 Code: 0xEEAF ==> Row:78 Col:15 */
    {0x7545,0xA9B3}, /* ��: GB2312 Code: 0xB3A9 ==> Row:19 Col:09 */
    {0x7548,0xB2EE}, /* �: GB2312 Code: 0xEEB2 ==> Row:78 Col:18 */
    {0x754B,0xB1EE}, /* �: GB2312 Code: 0xEEB1 ==> Row:78 Col:17 */
    {0x754C,0xE7BD}, /* ��: GB2312 Code: 0xBDE7 ==> Row:29 Col:71 */
    {0x754E,0xB0EE}, /* �: GB2312 Code: 0xEEB0 ==> Row:78 Col:16 */
    {0x754F,0xB7CE}, /* η: GB2312 Code: 0xCEB7 ==> Row:46 Col:23 */
    {0x7554,0xCFC5}, /* ��: GB2312 Code: 0xC5CF ==> Row:37 Col:47 */
    {0x7559,0xF4C1}, /* ��: GB2312 Code: 0xC1F4 ==> Row:33 Col:84 */
    {0x755A,0xCEDB}, /* ��: GB2312 Code: 0xDBCE ==> Row:59 Col:46 */
    {0x755B,0xB3EE}, /* �: GB2312 Code: 0xEEB3 ==> Row:78 Col:19 */
    {0x755C,0xF3D0}, /* ��: GB2312 Code: 0xD0F3 ==> Row:48 Col:83 */
    {0x7565,0xD4C2}, /* ��: GB2312 Code: 0xC2D4 ==> Row:34 Col:52 */
    {0x7566,0xE8C6}, /* ��: GB2312 Code: 0xC6E8 ==> Row:38 Col:72 */
    {0x756A,0xACB7}, /* ��: GB2312 Code: 0xB7AC ==> Row:23 Col:12 */
    {0x7572,0xB4EE}, /* �: GB2312 Code: 0xEEB4 ==> Row:78 Col:20 */
    {0x7574,0xEBB3}, /* ��: GB2312 Code: 0xB3EB ==> Row:19 Col:75 */
    {0x7578,0xFBBB}, /* ��: GB2312 Code: 0xBBFB ==> Row:27 Col:91 */
    {0x7579,0xB5EE}, /* �: GB2312 Code: 0xEEB5 ==> Row:78 Col:21 */
    {0x757F,0xDCE7}, /* ��: GB2312 Code: 0xE7DC ==> Row:71 Col:60 */
    {0x7583,0xB6EE}, /* �: GB2312 Code: 0xEEB6 ==> Row:78 Col:22 */
    {0x7586,0xAEBD}, /* ��: GB2312 Code: 0xBDAE ==> Row:29 Col:14 */
    {0x758B,0xE2F1}, /* ��: GB2312 Code: 0xF1E2 ==> Row:81 Col:66 */
    {0x758F,0xE8CA}, /* ��: GB2312 Code: 0xCAE8 ==> Row:42 Col:72 */
    {0x7591,0xC9D2}, /* ��: GB2312 Code: 0xD2C9 ==> Row:50 Col:41 */
    {0x7592,0xDAF0}, /* ��: GB2312 Code: 0xF0DA ==> Row:80 Col:58 */
    {0x7594,0xDBF0}, /* ��: GB2312 Code: 0xF0DB ==> Row:80 Col:59 */
    {0x7596,0xDCF0}, /* ��: GB2312 Code: 0xF0DC ==> Row:80 Col:60 */
    {0x7597,0xC6C1}, /* ��: GB2312 Code: 0xC1C6 ==> Row:33 Col:38 */
    {0x7599,0xEDB8}, /* ��: GB2312 Code: 0xB8ED ==> Row:24 Col:77 */
    {0x759A,0xCEBE}, /* ��: GB2312 Code: 0xBECE ==> Row:30 Col:46 */
    {0x759D,0xDEF0}, /* ��: GB2312 Code: 0xF0DE ==> Row:80 Col:62 */
    {0x759F,0xB1C5}, /* ű: GB2312 Code: 0xC5B1 ==> Row:37 Col:17 */
    {0x75A0,0xDDF0}, /* ��: GB2312 Code: 0xF0DD ==> Row:80 Col:61 */
    {0x75A1,0xF1D1}, /* ��: GB2312 Code: 0xD1F1 ==> Row:49 Col:81 */
    {0x75A3,0xE0F0}, /* ��: GB2312 Code: 0xF0E0 ==> Row:80 Col:64 */
    {0x75A4,0xCCB0}, /* ��: GB2312 Code: 0xB0CC ==> Row:16 Col:44 */
    {0x75A5,0xEABD}, /* ��: GB2312 Code: 0xBDEA ==> Row:29 Col:74 */
    {0x75AB,0xDFD2}, /* ��: GB2312 Code: 0xD2DF ==> Row:50 Col:63 */
    {0x75AC,0xDFF0}, /* ��: GB2312 Code: 0xF0DF ==> Row:80 Col:63 */
    {0x75AE,0xAFB4}, /* ��: GB2312 Code: 0xB4AF ==> Row:20 Col:15 */
    {0x75AF,0xE8B7}, /* ��: GB2312 Code: 0xB7E8 ==> Row:23 Col:72 */
    {0x75B0,0xE6F0}, /* ��: GB2312 Code: 0xF0E6 ==> Row:80 Col:70 */
    {0x75B1,0xE5F0}, /* ��: GB2312 Code: 0xF0E5 ==> Row:80 Col:69 */
    {0x75B2,0xA3C6}, /* ƣ: GB2312 Code: 0xC6A3 ==> Row:38 Col:03 */
    {0x75B3,0xE1F0}, /* ��: GB2312 Code: 0xF0E1 ==> Row:80 Col:65 */
    {0x75B4,0xE2F0}, /* ��: GB2312 Code: 0xF0E2 ==> Row:80 Col:66 */
    {0x75B5,0xC3B4}, /* ��: GB2312 Code: 0xB4C3 ==> Row:20 Col:35 */
    {0x75B8,0xE3F0}, /* ��: GB2312 Code: 0xF0E3 ==> Row:80 Col:67 */
    {0x75B9,0xEED5}, /* ��: GB2312 Code: 0xD5EE ==> Row:53 Col:78 */
    {0x75BC,0xDBCC}, /* ��: GB2312 Code: 0xCCDB ==> Row:44 Col:59 */
    {0x75BD,0xD2BE}, /* ��: GB2312 Code: 0xBED2 ==> Row:30 Col:50 */
    {0x75BE,0xB2BC}, /* ��: GB2312 Code: 0xBCB2 ==> Row:28 Col:18 */
    {0x75C2,0xE8F0}, /* ��: GB2312 Code: 0xF0E8 ==> Row:80 Col:72 */
    {0x75C3,0xE7F0}, /* ��: GB2312 Code: 0xF0E7 ==> Row:80 Col:71 */
    {0x75C4,0xE4F0}, /* ��: GB2312 Code: 0xF0E4 ==> Row:80 Col:68 */
    {0x75C5,0xA1B2}, /* ��: GB2312 Code: 0xB2A1 ==> Row:18 Col:01 */
    {0x75C7,0xA2D6}, /* ֢: GB2312 Code: 0xD6A2 ==> Row:54 Col:02 */
    {0x75C8,0xB8D3}, /* Ӹ: GB2312 Code: 0xD3B8 ==> Row:51 Col:24 */
    {0x75C9,0xB7BE}, /* ��: GB2312 Code: 0xBEB7 ==> Row:30 Col:23 */
    {0x75CA,0xACC8}, /* Ȭ: GB2312 Code: 0xC8AC ==> Row:40 Col:12 */
    {0x75CD,0xEAF0}, /* ��: GB2312 Code: 0xF0EA ==> Row:80 Col:74 */
    {0x75D2,0xF7D1}, /* ��: GB2312 Code: 0xD1F7 ==> Row:49 Col:87 */
    {0x75D4,0xCCD6}, /* ��: GB2312 Code: 0xD6CC ==> Row:54 Col:44 */
    {0x75D5,0xDBBA}, /* ��: GB2312 Code: 0xBADB ==> Row:26 Col:59 */
    {0x75D6,0xE9F0}, /* ��: GB2312 Code: 0xF0E9 ==> Row:80 Col:73 */
    {0x75D8,0xBBB6}, /* ��: GB2312 Code: 0xB6BB ==> Row:22 Col:27 */
    {0x75DB,0xB4CD}, /* ʹ: GB2312 Code: 0xCDB4 ==> Row:45 Col:20 */
    {0x75DE,0xA6C6}, /* Ʀ: GB2312 Code: 0xC6A6 ==> Row:38 Col:06 */
    {0x75E2,0xA1C1}, /* ��: GB2312 Code: 0xC1A1 ==> Row:33 Col:01 */
    {0x75E3,0xEBF0}, /* ��: GB2312 Code: 0xF0EB ==> Row:80 Col:75 */
    {0x75E4,0xEEF0}, /* ��: GB2312 Code: 0xF0EE ==> Row:80 Col:78 */
    {0x75E6,0xEDF0}, /* ��: GB2312 Code: 0xF0ED ==> Row:80 Col:77 */
    {0x75E7,0xF0F0}, /* ��: GB2312 Code: 0xF0F0 ==> Row:80 Col:80 */
    {0x75E8,0xECF0}, /* ��: GB2312 Code: 0xF0EC ==> Row:80 Col:76 */
    {0x75EA,0xBEBB}, /* ��: GB2312 Code: 0xBBBE ==> Row:27 Col:30 */
    {0x75EB,0xEFF0}, /* ��: GB2312 Code: 0xF0EF ==> Row:80 Col:79 */
    {0x75F0,0xB5CC}, /* ̵: GB2312 Code: 0xCCB5 ==> Row:44 Col:21 */
    {0x75F1,0xF2F0}, /* ��: GB2312 Code: 0xF0F2 ==> Row:80 Col:82 */
    {0x75F4,0xD5B3}, /* ��: GB2312 Code: 0xB3D5 ==> Row:19 Col:53 */
    {0x75F9,0xD4B1}, /* ��: GB2312 Code: 0xB1D4 ==> Row:17 Col:52 */
    {0x75FC,0xF3F0}, /* ��: GB2312 Code: 0xF0F3 ==> Row:80 Col:83 */
    {0x75FF,0xF4F0}, /* ��: GB2312 Code: 0xF0F4 ==> Row:80 Col:84 */
    {0x7600,0xF6F0}, /* ��: GB2312 Code: 0xF0F6 ==> Row:80 Col:86 */
    {0x7601,0xE1B4}, /* ��: GB2312 Code: 0xB4E1 ==> Row:20 Col:65 */
    {0x7603,0xF1F0}, /* ��: GB2312 Code: 0xF0F1 ==> Row:80 Col:81 */
    {0x7605,0xF7F0}, /* ��: GB2312 Code: 0xF0F7 ==> Row:80 Col:87 */
    {0x760A,0xFAF0}, /* ��: GB2312 Code: 0xF0FA ==> Row:80 Col:90 */
    {0x760C,0xF8F0}, /* ��: GB2312 Code: 0xF0F8 ==> Row:80 Col:88 */
    {0x7610,0xF5F0}, /* ��: GB2312 Code: 0xF0F5 ==> Row:80 Col:85 */
    {0x7615,0xFDF0}, /* ��: GB2312 Code: 0xF0FD ==> Row:80 Col:93 */
    {0x7617,0xF9F0}, /* ��: GB2312 Code: 0xF0F9 ==> Row:80 Col:89 */
    {0x7618,0xFCF0}, /* ��: GB2312 Code: 0xF0FC ==> Row:80 Col:92 */
    {0x7619,0xFEF0}, /* ��: GB2312 Code: 0xF0FE ==> Row:80 Col:94 */
    {0x761B,0xA1F1}, /* �: GB2312 Code: 0xF1A1 ==> Row:81 Col:01 */
    {0x761F,0xC1CE}, /* ��: GB2312 Code: 0xCEC1 ==> Row:46 Col:33 */
    {0x7620,0xA4F1}, /* �: GB2312 Code: 0xF1A4 ==> Row:81 Col:04 */
    {0x7622,0xA3F1}, /* �: GB2312 Code: 0xF1A3 ==> Row:81 Col:03 */
    {0x7624,0xF6C1}, /* ��: GB2312 Code: 0xC1F6 ==> Row:33 Col:86 */
    {0x7625,0xFBF0}, /* ��: GB2312 Code: 0xF0FB ==> Row:80 Col:91 */
    {0x7626,0xDDCA}, /* ��: GB2312 Code: 0xCADD ==> Row:42 Col:61 */
    {0x7629,0xF1B4}, /* ��: GB2312 Code: 0xB4F1 ==> Row:20 Col:81 */
    {0x762A,0xF1B1}, /* ��: GB2312 Code: 0xB1F1 ==> Row:17 Col:81 */
    {0x762B,0xB1CC}, /* ̱: GB2312 Code: 0xCCB1 ==> Row:44 Col:17 */
    {0x762D,0xA6F1}, /* �: GB2312 Code: 0xF1A6 ==> Row:81 Col:06 */
    {0x7630,0xA7F1}, /* �: GB2312 Code: 0xF1A7 ==> Row:81 Col:07 */
    {0x7633,0xACF1}, /* �: GB2312 Code: 0xF1AC ==> Row:81 Col:12 */
    {0x7634,0xCED5}, /* ��: GB2312 Code: 0xD5CE ==> Row:53 Col:46 */
    {0x7635,0xA9F1}, /* �: GB2312 Code: 0xF1A9 ==> Row:81 Col:09 */
    {0x7638,0xB3C8}, /* ȳ: GB2312 Code: 0xC8B3 ==> Row:40 Col:19 */
    {0x763C,0xA2F1}, /* �: GB2312 Code: 0xF1A2 ==> Row:81 Col:02 */
    {0x763E,0xABF1}, /* �: GB2312 Code: 0xF1AB ==> Row:81 Col:11 */
    {0x763F,0xA8F1}, /* �: GB2312 Code: 0xF1A8 ==> Row:81 Col:08 */
    {0x7640,0xA5F1}, /* �: GB2312 Code: 0xF1A5 ==> Row:81 Col:05 */
    {0x7643,0xAAF1}, /* �: GB2312 Code: 0xF1AA ==> Row:81 Col:10 */
    {0x764C,0xA9B0}, /* ��: GB2312 Code: 0xB0A9 ==> Row:16 Col:09 */
    {0x764D,0xADF1}, /* �: GB2312 Code: 0xF1AD ==> Row:81 Col:13 */
    {0x7654,0xAFF1}, /* �: GB2312 Code: 0xF1AF ==> Row:81 Col:15 */
    {0x7656,0xB1F1}, /* �: GB2312 Code: 0xF1B1 ==> Row:81 Col:17 */
    {0x765C,0xB0F1}, /* �: GB2312 Code: 0xF1B0 ==> Row:81 Col:16 */
    {0x765E,0xAEF1}, /* �: GB2312 Code: 0xF1AE ==> Row:81 Col:14 */
    {0x7663,0xA2D1}, /* Ѣ: GB2312 Code: 0xD1A2 ==> Row:49 Col:02 */
    {0x766B,0xB2F1}, /* �: GB2312 Code: 0xF1B2 ==> Row:81 Col:18 */
    {0x766F,0xB3F1}, /* �: GB2312 Code: 0xF1B3 ==> Row:81 Col:19 */
    {0x7678,0xEFB9}, /* ��: GB2312 Code: 0xB9EF ==> Row:25 Col:79 */
    {0x767B,0xC7B5}, /* ��: GB2312 Code: 0xB5C7 ==> Row:21 Col:39 */
    {0x767D,0xD7B0}, /* ��: GB2312 Code: 0xB0D7 ==> Row:16 Col:55 */
    {0x767E,0xD9B0}, /* ��: GB2312 Code: 0xB0D9 ==> Row:16 Col:57 */
    {0x7682,0xEDD4}, /* ��: GB2312 Code: 0xD4ED ==> Row:52 Col:77 */
    {0x7684,0xC4B5}, /* ��: GB2312 Code: 0xB5C4 ==> Row:21 Col:36 */
    {0x7686,0xD4BD}, /* ��: GB2312 Code: 0xBDD4 ==> Row:29 Col:52 */
    {0x7687,0xCABB}, /* ��: GB2312 Code: 0xBBCA ==> Row:27 Col:42 */
    {0x7688,0xA7F0}, /* �: GB2312 Code: 0xF0A7 ==> Row:80 Col:07 */
    {0x768B,0xDEB8}, /* ��: GB2312 Code: 0xB8DE ==> Row:24 Col:62 */
    {0x768E,0xA8F0}, /* �: GB2312 Code: 0xF0A8 ==> Row:80 Col:08 */
    {0x7691,0xA8B0}, /* ��: GB2312 Code: 0xB0A8 ==> Row:16 Col:08 */
    {0x7693,0xA9F0}, /* �: GB2312 Code: 0xF0A9 ==> Row:80 Col:09 */
    {0x7696,0xEECD}, /* ��: GB2312 Code: 0xCDEE ==> Row:45 Col:78 */
    {0x7699,0xAAF0}, /* �: GB2312 Code: 0xF0AA ==> Row:80 Col:10 */
    {0x76A4,0xABF0}, /* �: GB2312 Code: 0xF0AB ==> Row:80 Col:11 */
    {0x76AE,0xA4C6}, /* Ƥ: GB2312 Code: 0xC6A4 ==> Row:38 Col:04 */
    {0x76B1,0xE5D6}, /* ��: GB2312 Code: 0xD6E5 ==> Row:54 Col:69 */
    {0x76B2,0xE4F1}, /* ��: GB2312 Code: 0xF1E4 ==> Row:81 Col:68 */
    {0x76B4,0xE5F1}, /* ��: GB2312 Code: 0xF1E5 ==> Row:81 Col:69 */
    {0x76BF,0xF3C3}, /* ��: GB2312 Code: 0xC3F3 ==> Row:35 Col:83 */
    {0x76C2,0xDBD3}, /* ��: GB2312 Code: 0xD3DB ==> Row:51 Col:59 */
    {0x76C5,0xD1D6}, /* ��: GB2312 Code: 0xD6D1 ==> Row:54 Col:49 */
    {0x76C6,0xE8C5}, /* ��: GB2312 Code: 0xC5E8 ==> Row:37 Col:72 */
    {0x76C8,0xAFD3}, /* ӯ: GB2312 Code: 0xD3AF ==> Row:51 Col:15 */
    {0x76CA,0xE6D2}, /* ��: GB2312 Code: 0xD2E6 ==> Row:50 Col:70 */
    {0x76CD,0xC1EE}, /* ��: GB2312 Code: 0xEEC1 ==> Row:78 Col:33 */
    {0x76CE,0xBBB0}, /* ��: GB2312 Code: 0xB0BB ==> Row:16 Col:27 */
    {0x76CF,0xB5D5}, /* յ: GB2312 Code: 0xD5B5 ==> Row:53 Col:21 */
    {0x76D0,0xCED1}, /* ��: GB2312 Code: 0xD1CE ==> Row:49 Col:46 */
    {0x76D1,0xE0BC}, /* ��: GB2312 Code: 0xBCE0 ==> Row:28 Col:64 */
    {0x76D2,0xD0BA}, /* ��: GB2312 Code: 0xBAD0 ==> Row:26 Col:48 */
    {0x76D4,0xF8BF}, /* ��: GB2312 Code: 0xBFF8 ==> Row:31 Col:88 */
    {0x76D6,0xC7B8}, /* ��: GB2312 Code: 0xB8C7 ==> Row:24 Col:39 */
    {0x76D7,0xC1B5}, /* ��: GB2312 Code: 0xB5C1 ==> Row:21 Col:33 */
    {0x76D8,0xCCC5}, /* ��: GB2312 Code: 0xC5CC ==> Row:37 Col:44 */
    {0x76DB,0xA2CA}, /* ʢ: GB2312 Code: 0xCAA2 ==> Row:42 Col:02 */
    {0x76DF,0xCBC3}, /* ��: GB2312 Code: 0xC3CB ==> Row:35 Col:43 */
    {0x76E5,0xC2EE}, /* ��: GB2312 Code: 0xEEC2 ==> Row:78 Col:34 */
    {0x76EE,0xBFC4}, /* Ŀ: GB2312 Code: 0xC4BF ==> Row:36 Col:31 */
    {0x76EF,0xA2B6}, /* ��: GB2312 Code: 0xB6A2 ==> Row:22 Col:02 */
    {0x76F1,0xECED}, /* ��: GB2312 Code: 0xEDEC ==> Row:77 Col:76 */
    {0x76F2,0xA4C3}, /* ä: GB2312 Code: 0xC3A4 ==> Row:35 Col:04 */
    {0x76F4,0xB1D6}, /* ֱ: GB2312 Code: 0xD6B1 ==> Row:54 Col:17 */
    {0x76F8,0xE0CF}, /* ��: GB2312 Code: 0xCFE0 ==> Row:47 Col:64 */
    {0x76F9,0xEFED}, /* ��: GB2312 Code: 0xEDEF ==> Row:77 Col:79 */
    {0x76FC,0xCEC5}, /* ��: GB2312 Code: 0xC5CE ==> Row:37 Col:46 */
    {0x76FE,0xDCB6}, /* ��: GB2312 Code: 0xB6DC ==> Row:22 Col:60 */
    {0x7701,0xA1CA}, /* ʡ: GB2312 Code: 0xCAA1 ==> Row:42 Col:01 */
    {0x7704,0xEDED}, /* ��: GB2312 Code: 0xEDED ==> Row:77 Col:77 */
    {0x7707,0xF0ED}, /* ��: GB2312 Code: 0xEDF0 ==> Row:77 Col:80 */
    {0x7708,0xF1ED}, /* ��: GB2312 Code: 0xEDF1 ==> Row:77 Col:81 */
    {0x7709,0xBCC3}, /* ü: GB2312 Code: 0xC3BC ==> Row:35 Col:28 */
    {0x770B,0xB4BF}, /* ��: GB2312 Code: 0xBFB4 ==> Row:31 Col:20 */
    {0x770D,0xEEED}, /* ��: GB2312 Code: 0xEDEE ==> Row:77 Col:78 */
    {0x7719,0xF4ED}, /* ��: GB2312 Code: 0xEDF4 ==> Row:77 Col:84 */
    {0x771A,0xF2ED}, /* ��: GB2312 Code: 0xEDF2 ==> Row:77 Col:82 */
    {0x771F,0xE6D5}, /* ��: GB2312 Code: 0xD5E6 ==> Row:53 Col:70 */
    {0x7720,0xDFC3}, /* ��: GB2312 Code: 0xC3DF ==> Row:35 Col:63 */
    {0x7722,0xF3ED}, /* ��: GB2312 Code: 0xEDF3 ==> Row:77 Col:83 */
    {0x7726,0xF6ED}, /* ��: GB2312 Code: 0xEDF6 ==> Row:77 Col:86 */
    {0x7728,0xA3D5}, /* գ: GB2312 Code: 0xD5A3 ==> Row:53 Col:03 */
    {0x7729,0xA3D1}, /* ѣ: GB2312 Code: 0xD1A3 ==> Row:49 Col:03 */
    {0x772D,0xF5ED}, /* ��: GB2312 Code: 0xEDF5 ==> Row:77 Col:85 */
    {0x772F,0xD0C3}, /* ��: GB2312 Code: 0xC3D0 ==> Row:35 Col:48 */
    {0x7735,0xF7ED}, /* ��: GB2312 Code: 0xEDF7 ==> Row:77 Col:87 */
    {0x7736,0xF4BF}, /* ��: GB2312 Code: 0xBFF4 ==> Row:31 Col:84 */
    {0x7737,0xECBE}, /* ��: GB2312 Code: 0xBEEC ==> Row:30 Col:76 */
    {0x7738,0xF8ED}, /* ��: GB2312 Code: 0xEDF8 ==> Row:77 Col:88 */
    {0x773A,0xF7CC}, /* ��: GB2312 Code: 0xCCF7 ==> Row:44 Col:87 */
    {0x773C,0xDBD1}, /* ��: GB2312 Code: 0xD1DB ==> Row:49 Col:59 */
    {0x7740,0xC5D7}, /* ��: GB2312 Code: 0xD7C5 ==> Row:55 Col:37 */
    {0x7741,0xF6D5}, /* ��: GB2312 Code: 0xD5F6 ==> Row:53 Col:86 */
    {0x7743,0xFCED}, /* ��: GB2312 Code: 0xEDFC ==> Row:77 Col:92 */
    {0x7747,0xFBED}, /* ��: GB2312 Code: 0xEDFB ==> Row:77 Col:91 */
    {0x7750,0xF9ED}, /* ��: GB2312 Code: 0xEDF9 ==> Row:77 Col:89 */
    {0x7751,0xFAED}, /* ��: GB2312 Code: 0xEDFA ==> Row:77 Col:90 */
    {0x775A,0xFDED}, /* ��: GB2312 Code: 0xEDFD ==> Row:77 Col:93 */
    {0x775B,0xA6BE}, /* ��: GB2312 Code: 0xBEA6 ==> Row:30 Col:06 */
    {0x7761,0xAFCB}, /* ˯: GB2312 Code: 0xCBAF ==> Row:43 Col:15 */
    {0x7762,0xA1EE}, /* �: GB2312 Code: 0xEEA1 ==> Row:78 Col:01 */
    {0x7763,0xBDB6}, /* ��: GB2312 Code: 0xB6BD ==> Row:22 Col:29 */
    {0x7765,0xA2EE}, /* �: GB2312 Code: 0xEEA2 ==> Row:78 Col:02 */
    {0x7766,0xC0C4}, /* ��: GB2312 Code: 0xC4C0 ==> Row:36 Col:32 */
    {0x7768,0xFEED}, /* ��: GB2312 Code: 0xEDFE ==> Row:77 Col:94 */
    {0x776B,0xDEBD}, /* ��: GB2312 Code: 0xBDDE ==> Row:29 Col:62 */
    {0x776C,0xC7B2}, /* ��: GB2312 Code: 0xB2C7 ==> Row:18 Col:39 */
    {0x7779,0xC3B6}, /* ��: GB2312 Code: 0xB6C3 ==> Row:22 Col:35 */
    {0x777D,0xA5EE}, /* �: GB2312 Code: 0xEEA5 ==> Row:78 Col:05 */
    {0x777E,0xBAD8}, /* غ: GB2312 Code: 0xD8BA ==> Row:56 Col:26 */
    {0x777F,0xA3EE}, /* �: GB2312 Code: 0xEEA3 ==> Row:78 Col:03 */
    {0x7780,0xA6EE}, /* �: GB2312 Code: 0xEEA6 ==> Row:78 Col:06 */
    {0x7784,0xE9C3}, /* ��: GB2312 Code: 0xC3E9 ==> Row:35 Col:73 */
    {0x7785,0xF2B3}, /* ��: GB2312 Code: 0xB3F2 ==> Row:19 Col:82 */
    {0x778C,0xA7EE}, /* �: GB2312 Code: 0xEEA7 ==> Row:78 Col:07 */
    {0x778D,0xA4EE}, /* �: GB2312 Code: 0xEEA4 ==> Row:78 Col:04 */
    {0x778E,0xB9CF}, /* Ϲ: GB2312 Code: 0xCFB9 ==> Row:47 Col:25 */
    {0x7791,0xA8EE}, /* �: GB2312 Code: 0xEEA8 ==> Row:78 Col:08 */
    {0x7792,0xF7C2}, /* ��: GB2312 Code: 0xC2F7 ==> Row:34 Col:87 */
    {0x779F,0xA9EE}, /* �: GB2312 Code: 0xEEA9 ==> Row:78 Col:09 */
    {0x77A0,0xAAEE}, /* �: GB2312 Code: 0xEEAA ==> Row:78 Col:10 */
    {0x77A2,0xABDE}, /* ޫ: GB2312 Code: 0xDEAB ==> Row:62 Col:11 */
    {0x77A5,0xB3C6}, /* Ƴ: GB2312 Code: 0xC6B3 ==> Row:38 Col:19 */
    {0x77A7,0xC6C7}, /* ��: GB2312 Code: 0xC7C6 ==> Row:39 Col:38 */
    {0x77A9,0xF5D6}, /* ��: GB2312 Code: 0xD6F5 ==> Row:54 Col:85 */
    {0x77AA,0xC9B5}, /* ��: GB2312 Code: 0xB5C9 ==> Row:21 Col:41 */
    {0x77AC,0xB2CB}, /* ˲: GB2312 Code: 0xCBB2 ==> Row:43 Col:18 */
    {0x77B0,0xABEE}, /* �: GB2312 Code: 0xEEAB ==> Row:78 Col:11 */
    {0x77B3,0xABCD}, /* ͫ: GB2312 Code: 0xCDAB ==> Row:45 Col:11 */
    {0x77B5,0xACEE}, /* �: GB2312 Code: 0xEEAC ==> Row:78 Col:12 */
    {0x77BB,0xB0D5}, /* հ: GB2312 Code: 0xD5B0 ==> Row:53 Col:16 */
    {0x77BD,0xADEE}, /* �: GB2312 Code: 0xEEAD ==> Row:78 Col:13 */
    {0x77BF,0xC4F6}, /* ��: GB2312 Code: 0xF6C4 ==> Row:86 Col:36 */
    {0x77CD,0xC7DB}, /* ��: GB2312 Code: 0xDBC7 ==> Row:59 Col:39 */
    {0x77D7,0xA3B4}, /* ��: GB2312 Code: 0xB4A3 ==> Row:20 Col:03 */
    {0x77DB,0xACC3}, /* ì: GB2312 Code: 0xC3AC ==> Row:35 Col:12 */
    {0x77DC,0xE6F1}, /* ��: GB2312 Code: 0xF1E6 ==> Row:81 Col:70 */
    {0x77E2,0xB8CA}, /* ʸ: GB2312 Code: 0xCAB8 ==> Row:42 Col:24 */
    {0x77E3,0xD3D2}, /* ��: GB2312 Code: 0xD2D3 ==> Row:50 Col:51 */
    {0x77E5,0xAAD6}, /* ֪: GB2312 Code: 0xD6AA ==> Row:54 Col:10 */
    {0x77E7,0xF2EF}, /* ��: GB2312 Code: 0xEFF2 ==> Row:79 Col:82 */
    {0x77E9,0xD8BE}, /* ��: GB2312 Code: 0xBED8 ==> Row:30 Col:56 */
    {0x77EB,0xC3BD}, /* ��: GB2312 Code: 0xBDC3 ==> Row:29 Col:35 */
    {0x77EC,0xF3EF}, /* ��: GB2312 Code: 0xEFF3 ==> Row:79 Col:83 */
    {0x77ED,0xCCB6}, /* ��: GB2312 Code: 0xB6CC ==> Row:22 Col:44 */
    {0x77EE,0xABB0}, /* ��: GB2312 Code: 0xB0AB ==> Row:16 Col:11 */
    {0x77F3,0xAFCA}, /* ʯ: GB2312 Code: 0xCAAF ==> Row:42 Col:15 */
    {0x77F6,0xB6ED}, /* ��: GB2312 Code: 0xEDB6 ==> Row:77 Col:22 */
    {0x77F8,0xB7ED}, /* ��: GB2312 Code: 0xEDB7 ==> Row:77 Col:23 */
    {0x77FD,0xF9CE}, /* ��: GB2312 Code: 0xCEF9 ==> Row:46 Col:89 */
    {0x77FE,0xAFB7}, /* ��: GB2312 Code: 0xB7AF ==> Row:23 Col:15 */
    {0x77FF,0xF3BF}, /* ��: GB2312 Code: 0xBFF3 ==> Row:31 Col:83 */
    {0x7800,0xB8ED}, /* ��: GB2312 Code: 0xEDB8 ==> Row:77 Col:24 */
    {0x7801,0xEBC2}, /* ��: GB2312 Code: 0xC2EB ==> Row:34 Col:75 */
    {0x7802,0xB0C9}, /* ɰ: GB2312 Code: 0xC9B0 ==> Row:41 Col:16 */
    {0x7809,0xB9ED}, /* ��: GB2312 Code: 0xEDB9 ==> Row:77 Col:25 */
    {0x780C,0xF6C6}, /* ��: GB2312 Code: 0xC6F6 ==> Row:38 Col:86 */
    {0x780D,0xB3BF}, /* ��: GB2312 Code: 0xBFB3 ==> Row:31 Col:19 */
    {0x7811,0xBCED}, /* ��: GB2312 Code: 0xEDBC ==> Row:77 Col:28 */
    {0x7812,0xF8C5}, /* ��: GB2312 Code: 0xC5F8 ==> Row:37 Col:88 */
    {0x7814,0xD0D1}, /* ��: GB2312 Code: 0xD1D0 ==> Row:49 Col:48 */
    {0x7816,0xA9D7}, /* ש: GB2312 Code: 0xD7A9 ==> Row:55 Col:09 */
    {0x7817,0xBAED}, /* ��: GB2312 Code: 0xEDBA ==> Row:77 Col:26 */
    {0x7818,0xBBED}, /* ��: GB2312 Code: 0xEDBB ==> Row:77 Col:27 */
    {0x781A,0xE2D1}, /* ��: GB2312 Code: 0xD1E2 ==> Row:49 Col:66 */
    {0x781C,0xBFED}, /* ��: GB2312 Code: 0xEDBF ==> Row:77 Col:31 */
    {0x781D,0xC0ED}, /* ��: GB2312 Code: 0xEDC0 ==> Row:77 Col:32 */
    {0x781F,0xC4ED}, /* ��: GB2312 Code: 0xEDC4 ==> Row:77 Col:36 */
    {0x7823,0xC8ED}, /* ��: GB2312 Code: 0xEDC8 ==> Row:77 Col:40 */
    {0x7825,0xC6ED}, /* ��: GB2312 Code: 0xEDC6 ==> Row:77 Col:38 */
    {0x7826,0xCEED}, /* ��: GB2312 Code: 0xEDCE ==> Row:77 Col:46 */
    {0x7827,0xE8D5}, /* ��: GB2312 Code: 0xD5E8 ==> Row:53 Col:72 */
    {0x7829,0xC9ED}, /* ��: GB2312 Code: 0xEDC9 ==> Row:77 Col:41 */
    {0x782C,0xC7ED}, /* ��: GB2312 Code: 0xEDC7 ==> Row:77 Col:39 */
    {0x782D,0xBEED}, /* ��: GB2312 Code: 0xEDBE ==> Row:77 Col:30 */
    {0x7830,0xE9C5}, /* ��: GB2312 Code: 0xC5E9 ==> Row:37 Col:73 */
    {0x7834,0xC6C6}, /* ��: GB2312 Code: 0xC6C6 ==> Row:38 Col:38 */
    {0x7837,0xE9C9}, /* ��: GB2312 Code: 0xC9E9 ==> Row:41 Col:73 */
    {0x7838,0xD2D4}, /* ��: GB2312 Code: 0xD4D2 ==> Row:52 Col:50 */
    {0x7839,0xC1ED}, /* ��: GB2312 Code: 0xEDC1 ==> Row:77 Col:33 */
    {0x783A,0xC2ED}, /* ��: GB2312 Code: 0xEDC2 ==> Row:77 Col:34 */
    {0x783B,0xC3ED}, /* ��: GB2312 Code: 0xEDC3 ==> Row:77 Col:35 */
    {0x783C,0xC5ED}, /* ��: GB2312 Code: 0xEDC5 ==> Row:77 Col:37 */
    {0x783E,0xF9C0}, /* ��: GB2312 Code: 0xC0F9 ==> Row:32 Col:89 */
    {0x7840,0xA1B4}, /* ��: GB2312 Code: 0xB4A1 ==> Row:20 Col:01 */
    {0x7845,0xE8B9}, /* ��: GB2312 Code: 0xB9E8 ==> Row:25 Col:72 */
    {0x7847,0xD0ED}, /* ��: GB2312 Code: 0xEDD0 ==> Row:77 Col:48 */
    {0x784C,0xD1ED}, /* ��: GB2312 Code: 0xEDD1 ==> Row:77 Col:49 */
    {0x784E,0xCAED}, /* ��: GB2312 Code: 0xEDCA ==> Row:77 Col:42 */
    {0x7850,0xCFED}, /* ��: GB2312 Code: 0xEDCF ==> Row:77 Col:47 */
    {0x7852,0xF8CE}, /* ��: GB2312 Code: 0xCEF8 ==> Row:46 Col:88 */
    {0x7855,0xB6CB}, /* ˶: GB2312 Code: 0xCBB6 ==> Row:43 Col:22 */
    {0x7856,0xCCED}, /* ��: GB2312 Code: 0xEDCC ==> Row:77 Col:44 */
    {0x7857,0xCDED}, /* ��: GB2312 Code: 0xEDCD ==> Row:77 Col:45 */
    {0x785D,0xF5CF}, /* ��: GB2312 Code: 0xCFF5 ==> Row:47 Col:85 */
    {0x786A,0xD2ED}, /* ��: GB2312 Code: 0xEDD2 ==> Row:77 Col:50 */
    {0x786B,0xF2C1}, /* ��: GB2312 Code: 0xC1F2 ==> Row:33 Col:82 */
    {0x786C,0xB2D3}, /* Ӳ: GB2312 Code: 0xD3B2 ==> Row:51 Col:18 */
    {0x786D,0xCBED}, /* ��: GB2312 Code: 0xEDCB ==> Row:77 Col:43 */
    {0x786E,0xB7C8}, /* ȷ: GB2312 Code: 0xC8B7 ==> Row:40 Col:23 */
    {0x7877,0xEFBC}, /* ��: GB2312 Code: 0xBCEF ==> Row:28 Col:79 */
    {0x787C,0xF0C5}, /* ��: GB2312 Code: 0xC5F0 ==> Row:37 Col:80 */
    {0x7887,0xD6ED}, /* ��: GB2312 Code: 0xEDD6 ==> Row:77 Col:54 */
    {0x7889,0xEFB5}, /* ��: GB2312 Code: 0xB5EF ==> Row:21 Col:79 */
    {0x788C,0xB5C2}, /* µ: GB2312 Code: 0xC2B5 ==> Row:34 Col:21 */
    {0x788D,0xADB0}, /* ��: GB2312 Code: 0xB0AD ==> Row:16 Col:13 */
    {0x788E,0xE9CB}, /* ��: GB2312 Code: 0xCBE9 ==> Row:43 Col:73 */
    {0x7891,0xAEB1}, /* ��: GB2312 Code: 0xB1AE ==> Row:17 Col:14 */
    {0x7893,0xD4ED}, /* ��: GB2312 Code: 0xEDD4 ==> Row:77 Col:52 */
    {0x7897,0xEBCD}, /* ��: GB2312 Code: 0xCDEB ==> Row:45 Col:75 */
    {0x7898,0xE2B5}, /* ��: GB2312 Code: 0xB5E2 ==> Row:21 Col:66 */
    {0x789A,0xD5ED}, /* ��: GB2312 Code: 0xEDD5 ==> Row:77 Col:53 */
    {0x789B,0xD3ED}, /* ��: GB2312 Code: 0xEDD3 ==> Row:77 Col:51 */
    {0x789C,0xD7ED}, /* ��: GB2312 Code: 0xEDD7 ==> Row:77 Col:55 */
    {0x789F,0xFAB5}, /* ��: GB2312 Code: 0xB5FA ==> Row:21 Col:90 */
    {0x78A1,0xD8ED}, /* ��: GB2312 Code: 0xEDD8 ==> Row:77 Col:56 */
    {0x78A3,0xD9ED}, /* ��: GB2312 Code: 0xEDD9 ==> Row:77 Col:57 */
    {0x78A5,0xDCED}, /* ��: GB2312 Code: 0xEDDC ==> Row:77 Col:60 */
    {0x78A7,0xCCB1}, /* ��: GB2312 Code: 0xB1CC ==> Row:17 Col:44 */
    {0x78B0,0xF6C5}, /* ��: GB2312 Code: 0xC5F6 ==> Row:37 Col:86 */
    {0x78B1,0xEEBC}, /* ��: GB2312 Code: 0xBCEE ==> Row:28 Col:78 */
    {0x78B2,0xDAED}, /* ��: GB2312 Code: 0xEDDA ==> Row:77 Col:58 */
    {0x78B3,0xBCCC}, /* ̼: GB2312 Code: 0xCCBC ==> Row:44 Col:28 */
    {0x78B4,0xEAB2}, /* ��: GB2312 Code: 0xB2EA ==> Row:18 Col:74 */
    {0x78B9,0xDBED}, /* ��: GB2312 Code: 0xEDDB ==> Row:77 Col:59 */
    {0x78BE,0xEBC4}, /* ��: GB2312 Code: 0xC4EB ==> Row:36 Col:75 */
    {0x78C1,0xC5B4}, /* ��: GB2312 Code: 0xB4C5 ==> Row:20 Col:37 */
    {0x78C5,0xF5B0}, /* ��: GB2312 Code: 0xB0F5 ==> Row:16 Col:85 */
    {0x78C9,0xDFED}, /* ��: GB2312 Code: 0xEDDF ==> Row:77 Col:63 */
    {0x78CA,0xDAC0}, /* ��: GB2312 Code: 0xC0DA ==> Row:32 Col:58 */
    {0x78CB,0xE8B4}, /* ��: GB2312 Code: 0xB4E8 ==> Row:20 Col:72 */
    {0x78D0,0xCDC5}, /* ��: GB2312 Code: 0xC5CD ==> Row:37 Col:45 */
    {0x78D4,0xDDED}, /* ��: GB2312 Code: 0xEDDD ==> Row:77 Col:61 */
    {0x78D5,0xC4BF}, /* ��: GB2312 Code: 0xBFC4 ==> Row:31 Col:36 */
    {0x78D9,0xDEED}, /* ��: GB2312 Code: 0xEDDE ==> Row:77 Col:62 */
    {0x78E8,0xA5C4}, /* ĥ: GB2312 Code: 0xC4A5 ==> Row:36 Col:05 */
    {0x78EC,0xE0ED}, /* ��: GB2312 Code: 0xEDE0 ==> Row:77 Col:64 */
    {0x78F2,0xE1ED}, /* ��: GB2312 Code: 0xEDE1 ==> Row:77 Col:65 */
    {0x78F4,0xE3ED}, /* ��: GB2312 Code: 0xEDE3 ==> Row:77 Col:67 */
    {0x78F7,0xD7C1}, /* ��: GB2312 Code: 0xC1D7 ==> Row:33 Col:55 */
    {0x78FA,0xC7BB}, /* ��: GB2312 Code: 0xBBC7 ==> Row:27 Col:39 */
    {0x7901,0xB8BD}, /* ��: GB2312 Code: 0xBDB8 ==> Row:29 Col:24 */
    {0x7905,0xE2ED}, /* ��: GB2312 Code: 0xEDE2 ==> Row:77 Col:66 */
    {0x7913,0xE4ED}, /* ��: GB2312 Code: 0xEDE4 ==> Row:77 Col:68 */
    {0x791E,0xE6ED}, /* ��: GB2312 Code: 0xEDE6 ==> Row:77 Col:70 */
    {0x7924,0xE5ED}, /* ��: GB2312 Code: 0xEDE5 ==> Row:77 Col:69 */
    {0x7934,0xE7ED}, /* ��: GB2312 Code: 0xEDE7 ==> Row:77 Col:71 */
    {0x793A,0xBECA}, /* ʾ: GB2312 Code: 0xCABE ==> Row:42 Col:30 */
    {0x793B,0xEAEC}, /* ��: GB2312 Code: 0xECEA ==> Row:76 Col:74 */
    {0x793C,0xF1C0}, /* ��: GB2312 Code: 0xC0F1 ==> Row:32 Col:81 */
    {0x793E,0xE7C9}, /* ��: GB2312 Code: 0xC9E7 ==> Row:41 Col:71 */
    {0x7940,0xEBEC}, /* ��: GB2312 Code: 0xECEB ==> Row:76 Col:75 */
    {0x7941,0xEEC6}, /* ��: GB2312 Code: 0xC6EE ==> Row:38 Col:78 */
    {0x7946,0xECEC}, /* ��: GB2312 Code: 0xECEC ==> Row:76 Col:76 */
    {0x7948,0xEDC6}, /* ��: GB2312 Code: 0xC6ED ==> Row:38 Col:77 */
    {0x7949,0xEDEC}, /* ��: GB2312 Code: 0xECED ==> Row:76 Col:77 */
    {0x7953,0xF0EC}, /* ��: GB2312 Code: 0xECF0 ==> Row:76 Col:80 */
    {0x7956,0xE6D7}, /* ��: GB2312 Code: 0xD7E6 ==> Row:55 Col:70 */
    {0x7957,0xF3EC}, /* ��: GB2312 Code: 0xECF3 ==> Row:76 Col:83 */
    {0x795A,0xF1EC}, /* ��: GB2312 Code: 0xECF1 ==> Row:76 Col:81 */
    {0x795B,0xEEEC}, /* ��: GB2312 Code: 0xECEE ==> Row:76 Col:78 */
    {0x795C,0xEFEC}, /* ��: GB2312 Code: 0xECEF ==> Row:76 Col:79 */
    {0x795D,0xA3D7}, /* ף: GB2312 Code: 0xD7A3 ==> Row:55 Col:03 */
    {0x795E,0xF1C9}, /* ��: GB2312 Code: 0xC9F1 ==> Row:41 Col:81 */
    {0x795F,0xEECB}, /* ��: GB2312 Code: 0xCBEE ==> Row:43 Col:78 */
    {0x7960,0xF4EC}, /* ��: GB2312 Code: 0xECF4 ==> Row:76 Col:84 */
    {0x7962,0xF2EC}, /* ��: GB2312 Code: 0xECF2 ==> Row:76 Col:82 */
    {0x7965,0xE9CF}, /* ��: GB2312 Code: 0xCFE9 ==> Row:47 Col:73 */
    {0x7967,0xF6EC}, /* ��: GB2312 Code: 0xECF6 ==> Row:76 Col:86 */
    {0x7968,0xB1C6}, /* Ʊ: GB2312 Code: 0xC6B1 ==> Row:38 Col:17 */
    {0x796D,0xC0BC}, /* ��: GB2312 Code: 0xBCC0 ==> Row:28 Col:32 */
    {0x796F,0xF5EC}, /* ��: GB2312 Code: 0xECF5 ==> Row:76 Col:85 */
    {0x7977,0xBBB5}, /* ��: GB2312 Code: 0xB5BB ==> Row:21 Col:27 */
    {0x7978,0xF6BB}, /* ��: GB2312 Code: 0xBBF6 ==> Row:27 Col:86 */
    {0x797A,0xF7EC}, /* ��: GB2312 Code: 0xECF7 ==> Row:76 Col:87 */
    {0x7980,0xF7D9}, /* ��: GB2312 Code: 0xD9F7 ==> Row:57 Col:87 */
    {0x7981,0xFBBD}, /* ��: GB2312 Code: 0xBDFB ==> Row:29 Col:91 */
    {0x7984,0xBBC2}, /* »: GB2312 Code: 0xC2BB ==> Row:34 Col:27 */
    {0x7985,0xF8EC}, /* ��: GB2312 Code: 0xECF8 ==> Row:76 Col:88 */
    {0x798A,0xF9EC}, /* ��: GB2312 Code: 0xECF9 ==> Row:76 Col:89 */
    {0x798F,0xA3B8}, /* ��: GB2312 Code: 0xB8A3 ==> Row:24 Col:03 */
    {0x799A,0xFAEC}, /* ��: GB2312 Code: 0xECFA ==> Row:76 Col:90 */
    {0x79A7,0xFBEC}, /* ��: GB2312 Code: 0xECFB ==> Row:76 Col:91 */
    {0x79B3,0xFCEC}, /* ��: GB2312 Code: 0xECFC ==> Row:76 Col:92 */
    {0x79B9,0xEDD3}, /* ��: GB2312 Code: 0xD3ED ==> Row:51 Col:77 */
    {0x79BA,0xAED8}, /* خ: GB2312 Code: 0xD8AE ==> Row:56 Col:14 */
    {0x79BB,0xEBC0}, /* ��: GB2312 Code: 0xC0EB ==> Row:32 Col:75 */
    {0x79BD,0xDDC7}, /* ��: GB2312 Code: 0xC7DD ==> Row:39 Col:61 */
    {0x79BE,0xCCBA}, /* ��: GB2312 Code: 0xBACC ==> Row:26 Col:44 */
    {0x79C0,0xE3D0}, /* ��: GB2312 Code: 0xD0E3 ==> Row:48 Col:67 */
    {0x79C1,0xBDCB}, /* ˽: GB2312 Code: 0xCBBD ==> Row:43 Col:29 */
    {0x79C3,0xBACD}, /* ͺ: GB2312 Code: 0xCDBA ==> Row:45 Col:26 */
    {0x79C6,0xD1B8}, /* ��: GB2312 Code: 0xB8D1 ==> Row:24 Col:49 */
    {0x79C9,0xFCB1}, /* ��: GB2312 Code: 0xB1FC ==> Row:17 Col:92 */
    {0x79CB,0xEFC7}, /* ��: GB2312 Code: 0xC7EF ==> Row:39 Col:79 */
    {0x79CD,0xD6D6}, /* ��: GB2312 Code: 0xD6D6 ==> Row:54 Col:54 */
    {0x79D1,0xC6BF}, /* ��: GB2312 Code: 0xBFC6 ==> Row:31 Col:38 */
    {0x79D2,0xEBC3}, /* ��: GB2312 Code: 0xC3EB ==> Row:35 Col:75 */
    {0x79D5,0xF5EF}, /* ��: GB2312 Code: 0xEFF5 ==> Row:79 Col:85 */
    {0x79D8,0xD8C3}, /* ��: GB2312 Code: 0xC3D8 ==> Row:35 Col:56 */
    {0x79DF,0xE2D7}, /* ��: GB2312 Code: 0xD7E2 ==> Row:55 Col:66 */
    {0x79E3,0xF7EF}, /* ��: GB2312 Code: 0xEFF7 ==> Row:79 Col:87 */
    {0x79E4,0xD3B3}, /* ��: GB2312 Code: 0xB3D3 ==> Row:19 Col:51 */
    {0x79E6,0xD8C7}, /* ��: GB2312 Code: 0xC7D8 ==> Row:39 Col:56 */
    {0x79E7,0xEDD1}, /* ��: GB2312 Code: 0xD1ED ==> Row:49 Col:77 */
    {0x79E9,0xC8D6}, /* ��: GB2312 Code: 0xD6C8 ==> Row:54 Col:40 */
    {0x79EB,0xF8EF}, /* ��: GB2312 Code: 0xEFF8 ==> Row:79 Col:88 */
    {0x79ED,0xF6EF}, /* ��: GB2312 Code: 0xEFF6 ==> Row:79 Col:86 */
    {0x79EF,0xFDBB}, /* ��: GB2312 Code: 0xBBFD ==> Row:27 Col:93 */
    {0x79F0,0xC6B3}, /* ��: GB2312 Code: 0xB3C6 ==> Row:19 Col:38 */
    {0x79F8,0xD5BD}, /* ��: GB2312 Code: 0xBDD5 ==> Row:29 Col:53 */
    {0x79FB,0xC6D2}, /* ��: GB2312 Code: 0xD2C6 ==> Row:50 Col:38 */
    {0x79FD,0xE0BB}, /* ��: GB2312 Code: 0xBBE0 ==> Row:27 Col:64 */
    {0x7A00,0xA1CF}, /* ϡ: GB2312 Code: 0xCFA1 ==> Row:47 Col:01 */
    {0x7A02,0xFCEF}, /* ��: GB2312 Code: 0xEFFC ==> Row:79 Col:92 */
    {0x7A03,0xFBEF}, /* ��: GB2312 Code: 0xEFFB ==> Row:79 Col:91 */
    {0x7A06,0xF9EF}, /* ��: GB2312 Code: 0xEFF9 ==> Row:79 Col:89 */
    {0x7A0B,0xCCB3}, /* ��: GB2312 Code: 0xB3CC ==> Row:19 Col:44 */
    {0x7A0D,0xD4C9}, /* ��: GB2312 Code: 0xC9D4 ==> Row:41 Col:52 */
    {0x7A0E,0xB0CB}, /* ˰: GB2312 Code: 0xCBB0 ==> Row:43 Col:16 */
    {0x7A14,0xFEEF}, /* ��: GB2312 Code: 0xEFFE ==> Row:79 Col:94 */
    {0x7A17,0xDEB0}, /* ��: GB2312 Code: 0xB0DE ==> Row:16 Col:62 */
    {0x7A1A,0xC9D6}, /* ��: GB2312 Code: 0xD6C9 ==> Row:54 Col:41 */
    {0x7A1E,0xFDEF}, /* ��: GB2312 Code: 0xEFFD ==> Row:79 Col:93 */
    {0x7A20,0xEDB3}, /* ��: GB2312 Code: 0xB3ED ==> Row:19 Col:77 */
    {0x7A23,0xD5F6}, /* ��: GB2312 Code: 0xF6D5 ==> Row:86 Col:53 */
    {0x7A33,0xC8CE}, /* ��: GB2312 Code: 0xCEC8 ==> Row:46 Col:40 */
    {0x7A37,0xA2F0}, /* �: GB2312 Code: 0xF0A2 ==> Row:80 Col:02 */
    {0x7A39,0xA1F0}, /* �: GB2312 Code: 0xF0A1 ==> Row:80 Col:01 */
    {0x7A3B,0xBEB5}, /* ��: GB2312 Code: 0xB5BE ==> Row:21 Col:30 */
    {0x7A3C,0xDABC}, /* ��: GB2312 Code: 0xBCDA ==> Row:28 Col:58 */
    {0x7A3D,0xFCBB}, /* ��: GB2312 Code: 0xBBFC ==> Row:27 Col:92 */
    {0x7A3F,0xE5B8}, /* ��: GB2312 Code: 0xB8E5 ==> Row:24 Col:69 */
    {0x7A46,0xC2C4}, /* ��: GB2312 Code: 0xC4C2 ==> Row:36 Col:34 */
    {0x7A51,0xA3F0}, /* �: GB2312 Code: 0xF0A3 ==> Row:80 Col:03 */
    {0x7A57,0xEBCB}, /* ��: GB2312 Code: 0xCBEB ==> Row:43 Col:75 */
    {0x7A70,0xA6F0}, /* �: GB2312 Code: 0xF0A6 ==> Row:80 Col:06 */
    {0x7A74,0xA8D1}, /* Ѩ: GB2312 Code: 0xD1A8 ==> Row:49 Col:08 */
    {0x7A76,0xBFBE}, /* ��: GB2312 Code: 0xBEBF ==> Row:30 Col:31 */
    {0x7A77,0xEEC7}, /* ��: GB2312 Code: 0xC7EE ==> Row:39 Col:78 */
    {0x7A78,0xB6F1}, /* �: GB2312 Code: 0xF1B6 ==> Row:81 Col:22 */
    {0x7A79,0xB7F1}, /* �: GB2312 Code: 0xF1B7 ==> Row:81 Col:23 */
    {0x7A7A,0xD5BF}, /* ��: GB2312 Code: 0xBFD5 ==> Row:31 Col:53 */
    {0x7A7F,0xA9B4}, /* ��: GB2312 Code: 0xB4A9 ==> Row:20 Col:09 */
    {0x7A80,0xB8F1}, /* �: GB2312 Code: 0xF1B8 ==> Row:81 Col:24 */
    {0x7A81,0xBBCD}, /* ͻ: GB2312 Code: 0xCDBB ==> Row:45 Col:27 */
    {0x7A83,0xD4C7}, /* ��: GB2312 Code: 0xC7D4 ==> Row:39 Col:52 */
    {0x7A84,0xADD5}, /* խ: GB2312 Code: 0xD5AD ==> Row:53 Col:13 */
    {0x7A86,0xB9F1}, /* �: GB2312 Code: 0xF1B9 ==> Row:81 Col:25 */
    {0x7A88,0xBAF1}, /* �: GB2312 Code: 0xF1BA ==> Row:81 Col:26 */
    {0x7A8D,0xCFC7}, /* ��: GB2312 Code: 0xC7CF ==> Row:39 Col:47 */
    {0x7A91,0xA4D2}, /* Ҥ: GB2312 Code: 0xD2A4 ==> Row:50 Col:04 */
    {0x7A92,0xCFD6}, /* ��: GB2312 Code: 0xD6CF ==> Row:54 Col:47 */
    {0x7A95,0xBBF1}, /* �: GB2312 Code: 0xF1BB ==> Row:81 Col:27 */
    {0x7A96,0xD1BD}, /* ��: GB2312 Code: 0xBDD1 ==> Row:29 Col:49 */
    {0x7A97,0xB0B4}, /* ��: GB2312 Code: 0xB4B0 ==> Row:20 Col:16 */
    {0x7A98,0xBDBE}, /* ��: GB2312 Code: 0xBEBD ==> Row:30 Col:29 */
    {0x7A9C,0xDCB4}, /* ��: GB2312 Code: 0xB4DC ==> Row:20 Col:60 */
    {0x7A9D,0xD1CE}, /* ��: GB2312 Code: 0xCED1 ==> Row:46 Col:49 */
    {0x7A9F,0xDFBF}, /* ��: GB2312 Code: 0xBFDF ==> Row:31 Col:63 */
    {0x7AA0,0xBDF1}, /* �: GB2312 Code: 0xF1BD ==> Row:81 Col:29 */
    {0x7AA5,0xFABF}, /* ��: GB2312 Code: 0xBFFA ==> Row:31 Col:90 */
    {0x7AA6,0xBCF1}, /* �: GB2312 Code: 0xF1BC ==> Row:81 Col:28 */
    {0x7AA8,0xBFF1}, /* �: GB2312 Code: 0xF1BF ==> Row:81 Col:31 */
    {0x7AAC,0xBEF1}, /* �: GB2312 Code: 0xF1BE ==> Row:81 Col:30 */
    {0x7AAD,0xC0F1}, /* ��: GB2312 Code: 0xF1C0 ==> Row:81 Col:32 */
    {0x7AB3,0xC1F1}, /* ��: GB2312 Code: 0xF1C1 ==> Row:81 Col:33 */
    {0x7ABF,0xFEC1}, /* ��: GB2312 Code: 0xC1FE ==> Row:33 Col:94 */
    {0x7ACB,0xA2C1}, /* ��: GB2312 Code: 0xC1A2 ==> Row:33 Col:02 */
    {0x7AD6,0xFACA}, /* ��: GB2312 Code: 0xCAFA ==> Row:42 Col:90 */
    {0x7AD9,0xBED5}, /* վ: GB2312 Code: 0xD5BE ==> Row:53 Col:30 */
    {0x7ADE,0xBABE}, /* ��: GB2312 Code: 0xBEBA ==> Row:30 Col:26 */
    {0x7ADF,0xB9BE}, /* ��: GB2312 Code: 0xBEB9 ==> Row:30 Col:25 */
    {0x7AE0,0xC2D5}, /* ��: GB2312 Code: 0xD5C2 ==> Row:53 Col:34 */
    {0x7AE3,0xA2BF}, /* ��: GB2312 Code: 0xBFA2 ==> Row:31 Col:02 */
    {0x7AE5,0xAFCD}, /* ͯ: GB2312 Code: 0xCDAF ==> Row:45 Col:15 */
    {0x7AE6,0xB5F1}, /* �: GB2312 Code: 0xF1B5 ==> Row:81 Col:21 */
    {0x7AED,0xDFBD}, /* ��: GB2312 Code: 0xBDDF ==> Row:29 Col:63 */
    {0x7AEF,0xCBB6}, /* ��: GB2312 Code: 0xB6CB ==> Row:22 Col:43 */
    {0x7AF9,0xF1D6}, /* ��: GB2312 Code: 0xD6F1 ==> Row:54 Col:81 */
    {0x7AFA,0xC3F3}, /* ��: GB2312 Code: 0xF3C3 ==> Row:83 Col:35 */
    {0x7AFD,0xC4F3}, /* ��: GB2312 Code: 0xF3C4 ==> Row:83 Col:36 */
    {0x7AFF,0xCDB8}, /* ��: GB2312 Code: 0xB8CD ==> Row:24 Col:45 */
    {0x7B03,0xC6F3}, /* ��: GB2312 Code: 0xF3C6 ==> Row:83 Col:38 */
    {0x7B04,0xC7F3}, /* ��: GB2312 Code: 0xF3C7 ==> Row:83 Col:39 */
    {0x7B06,0xCAB0}, /* ��: GB2312 Code: 0xB0CA ==> Row:16 Col:42 */
    {0x7B08,0xC5F3}, /* ��: GB2312 Code: 0xF3C5 ==> Row:83 Col:37 */
    {0x7B0A,0xC9F3}, /* ��: GB2312 Code: 0xF3C9 ==> Row:83 Col:41 */
    {0x7B0B,0xF1CB}, /* ��: GB2312 Code: 0xCBF1 ==> Row:43 Col:81 */
    {0x7B0F,0xCBF3}, /* ��: GB2312 Code: 0xF3CB ==> Row:83 Col:43 */
    {0x7B11,0xA6D0}, /* Ц: GB2312 Code: 0xD0A6 ==> Row:48 Col:06 */
    {0x7B14,0xCAB1}, /* ��: GB2312 Code: 0xB1CA ==> Row:17 Col:42 */
    {0x7B15,0xC8F3}, /* ��: GB2312 Code: 0xF3C8 ==> Row:83 Col:40 */
    {0x7B19,0xCFF3}, /* ��: GB2312 Code: 0xF3CF ==> Row:83 Col:47 */
    {0x7B1B,0xD1B5}, /* ��: GB2312 Code: 0xB5D1 ==> Row:21 Col:49 */
    {0x7B1E,0xD7F3}, /* ��: GB2312 Code: 0xF3D7 ==> Row:83 Col:55 */
    {0x7B20,0xD2F3}, /* ��: GB2312 Code: 0xF3D2 ==> Row:83 Col:50 */
    {0x7B24,0xD4F3}, /* ��: GB2312 Code: 0xF3D4 ==> Row:83 Col:52 */
    {0x7B25,0xD3F3}, /* ��: GB2312 Code: 0xF3D3 ==> Row:83 Col:51 */
    {0x7B26,0xFBB7}, /* ��: GB2312 Code: 0xB7FB ==> Row:23 Col:91 */
    {0x7B28,0xBFB1}, /* ��: GB2312 Code: 0xB1BF ==> Row:17 Col:31 */
    {0x7B2A,0xCEF3}, /* ��: GB2312 Code: 0xF3CE ==> Row:83 Col:46 */
    {0x7B2B,0xCAF3}, /* ��: GB2312 Code: 0xF3CA ==> Row:83 Col:42 */
    {0x7B2C,0xDAB5}, /* ��: GB2312 Code: 0xB5DA ==> Row:21 Col:58 */
    {0x7B2E,0xD0F3}, /* ��: GB2312 Code: 0xF3D0 ==> Row:83 Col:48 */
    {0x7B31,0xD1F3}, /* ��: GB2312 Code: 0xF3D1 ==> Row:83 Col:49 */
    {0x7B33,0xD5F3}, /* ��: GB2312 Code: 0xF3D5 ==> Row:83 Col:53 */
    {0x7B38,0xCDF3}, /* ��: GB2312 Code: 0xF3CD ==> Row:83 Col:45 */
    {0x7B3A,0xE3BC}, /* ��: GB2312 Code: 0xBCE3 ==> Row:28 Col:67 */
    {0x7B3C,0xFDC1}, /* ��: GB2312 Code: 0xC1FD ==> Row:33 Col:93 */
    {0x7B3E,0xD6F3}, /* ��: GB2312 Code: 0xF3D6 ==> Row:83 Col:54 */
    {0x7B45,0xDAF3}, /* ��: GB2312 Code: 0xF3DA ==> Row:83 Col:58 */
    {0x7B47,0xCCF3}, /* ��: GB2312 Code: 0xF3CC ==> Row:83 Col:44 */
    {0x7B49,0xC8B5}, /* ��: GB2312 Code: 0xB5C8 ==> Row:21 Col:40 */
    {0x7B4B,0xEEBD}, /* ��: GB2312 Code: 0xBDEE ==> Row:29 Col:78 */
    {0x7B4C,0xDCF3}, /* ��: GB2312 Code: 0xF3DC ==> Row:83 Col:60 */
    {0x7B4F,0xA4B7}, /* ��: GB2312 Code: 0xB7A4 ==> Row:23 Col:04 */
    {0x7B50,0xF0BF}, /* ��: GB2312 Code: 0xBFF0 ==> Row:31 Col:80 */
    {0x7B51,0xFED6}, /* ��: GB2312 Code: 0xD6FE ==> Row:54 Col:94 */
    {0x7B52,0xB2CD}, /* Ͳ: GB2312 Code: 0xCDB2 ==> Row:45 Col:18 */
    {0x7B54,0xF0B4}, /* ��: GB2312 Code: 0xB4F0 ==> Row:20 Col:80 */
    {0x7B56,0xDFB2}, /* ��: GB2312 Code: 0xB2DF ==> Row:18 Col:63 */
    {0x7B58,0xD8F3}, /* ��: GB2312 Code: 0xF3D8 ==> Row:83 Col:56 */
    {0x7B5A,0xD9F3}, /* ��: GB2312 Code: 0xF3D9 ==> Row:83 Col:57 */
    {0x7B5B,0xB8C9}, /* ɸ: GB2312 Code: 0xC9B8 ==> Row:41 Col:24 */
    {0x7B5D,0xDDF3}, /* ��: GB2312 Code: 0xF3DD ==> Row:83 Col:61 */
    {0x7B60,0xDEF3}, /* ��: GB2312 Code: 0xF3DE ==> Row:83 Col:62 */
    {0x7B62,0xE1F3}, /* ��: GB2312 Code: 0xF3E1 ==> Row:83 Col:65 */
    {0x7B6E,0xDFF3}, /* ��: GB2312 Code: 0xF3DF ==> Row:83 Col:63 */
    {0x7B71,0xE3F3}, /* ��: GB2312 Code: 0xF3E3 ==> Row:83 Col:67 */
    {0x7B72,0xE2F3}, /* ��: GB2312 Code: 0xF3E2 ==> Row:83 Col:66 */
    {0x7B75,0xDBF3}, /* ��: GB2312 Code: 0xF3DB ==> Row:83 Col:59 */
    {0x7B77,0xEABF}, /* ��: GB2312 Code: 0xBFEA ==> Row:31 Col:74 */
    {0x7B79,0xEFB3}, /* ��: GB2312 Code: 0xB3EF ==> Row:19 Col:79 */
    {0x7B7B,0xE0F3}, /* ��: GB2312 Code: 0xF3E0 ==> Row:83 Col:64 */
    {0x7B7E,0xA9C7}, /* ǩ: GB2312 Code: 0xC7A9 ==> Row:39 Col:09 */
    {0x7B80,0xF2BC}, /* ��: GB2312 Code: 0xBCF2 ==> Row:28 Col:82 */
    {0x7B85,0xEBF3}, /* ��: GB2312 Code: 0xF3EB ==> Row:83 Col:75 */
    {0x7B8D,0xBFB9}, /* ��: GB2312 Code: 0xB9BF ==> Row:25 Col:31 */
    {0x7B90,0xE4F3}, /* ��: GB2312 Code: 0xF3E4 ==> Row:83 Col:68 */
    {0x7B94,0xADB2}, /* ��: GB2312 Code: 0xB2AD ==> Row:18 Col:13 */
    {0x7B95,0xFEBB}, /* ��: GB2312 Code: 0xBBFE ==> Row:27 Col:94 */
    {0x7B97,0xE3CB}, /* ��: GB2312 Code: 0xCBE3 ==> Row:43 Col:67 */
    {0x7B9C,0xEDF3}, /* ��: GB2312 Code: 0xF3ED ==> Row:83 Col:77 */
    {0x7B9D,0xE9F3}, /* ��: GB2312 Code: 0xF3E9 ==> Row:83 Col:73 */
    {0x7BA1,0xDCB9}, /* ��: GB2312 Code: 0xB9DC ==> Row:25 Col:60 */
    {0x7BA2,0xEEF3}, /* ��: GB2312 Code: 0xF3EE ==> Row:83 Col:78 */
    {0x7BA6,0xE5F3}, /* ��: GB2312 Code: 0xF3E5 ==> Row:83 Col:69 */
    {0x7BA7,0xE6F3}, /* ��: GB2312 Code: 0xF3E6 ==> Row:83 Col:70 */
    {0x7BA8,0xEAF3}, /* ��: GB2312 Code: 0xF3EA ==> Row:83 Col:74 */
    {0x7BA9,0xE1C2}, /* ��: GB2312 Code: 0xC2E1 ==> Row:34 Col:65 */
    {0x7BAA,0xECF3}, /* ��: GB2312 Code: 0xF3EC ==> Row:83 Col:76 */
    {0x7BAB,0xEFF3}, /* ��: GB2312 Code: 0xF3EF ==> Row:83 Col:79 */
    {0x7BAC,0xE8F3}, /* ��: GB2312 Code: 0xF3E8 ==> Row:83 Col:72 */
    {0x7BAD,0xFDBC}, /* ��: GB2312 Code: 0xBCFD ==> Row:28 Col:93 */
    {0x7BB1,0xE4CF}, /* ��: GB2312 Code: 0xCFE4 ==> Row:47 Col:68 */
    {0x7BB4,0xF0F3}, /* ��: GB2312 Code: 0xF3F0 ==> Row:83 Col:80 */
    {0x7BB8,0xE7F3}, /* ��: GB2312 Code: 0xF3E7 ==> Row:83 Col:71 */
    {0x7BC1,0xF2F3}, /* ��: GB2312 Code: 0xF3F2 ==> Row:83 Col:82 */
    {0x7BC6,0xADD7}, /* ׭: GB2312 Code: 0xD7AD ==> Row:55 Col:13 */
    {0x7BC7,0xAAC6}, /* ƪ: GB2312 Code: 0xC6AA ==> Row:38 Col:10 */
    {0x7BCC,0xF3F3}, /* ��: GB2312 Code: 0xF3F3 ==> Row:83 Col:83 */
    {0x7BD1,0xF1F3}, /* ��: GB2312 Code: 0xF3F1 ==> Row:83 Col:81 */
    {0x7BD3,0xA8C2}, /* ¨: GB2312 Code: 0xC2A8 ==> Row:34 Col:08 */
    {0x7BD9,0xDDB8}, /* ��: GB2312 Code: 0xB8DD ==> Row:24 Col:61 */
    {0x7BDA,0xF5F3}, /* ��: GB2312 Code: 0xF3F5 ==> Row:83 Col:85 */
    {0x7BDD,0xF4F3}, /* ��: GB2312 Code: 0xF3F4 ==> Row:83 Col:84 */
    {0x7BE1,0xDBB4}, /* ��: GB2312 Code: 0xB4DB ==> Row:20 Col:59 */
    {0x7BE5,0xF6F3}, /* ��: GB2312 Code: 0xF3F6 ==> Row:83 Col:86 */
    {0x7BE6,0xF7F3}, /* ��: GB2312 Code: 0xF3F7 ==> Row:83 Col:87 */
    {0x7BEA,0xF8F3}, /* ��: GB2312 Code: 0xF3F8 ==> Row:83 Col:88 */
    {0x7BEE,0xBAC0}, /* ��: GB2312 Code: 0xC0BA ==> Row:32 Col:26 */
    {0x7BF1,0xE9C0}, /* ��: GB2312 Code: 0xC0E9 ==> Row:32 Col:73 */
    {0x7BF7,0xF1C5}, /* ��: GB2312 Code: 0xC5F1 ==> Row:37 Col:81 */
    {0x7BFC,0xFBF3}, /* ��: GB2312 Code: 0xF3FB ==> Row:83 Col:91 */
    {0x7BFE,0xFAF3}, /* ��: GB2312 Code: 0xF3FA ==> Row:83 Col:90 */
    {0x7C07,0xD8B4}, /* ��: GB2312 Code: 0xB4D8 ==> Row:20 Col:56 */
    {0x7C0B,0xFEF3}, /* ��: GB2312 Code: 0xF3FE ==> Row:83 Col:94 */
    {0x7C0C,0xF9F3}, /* ��: GB2312 Code: 0xF3F9 ==> Row:83 Col:89 */
    {0x7C0F,0xFCF3}, /* ��: GB2312 Code: 0xF3FC ==> Row:83 Col:92 */
    {0x7C16,0xFDF3}, /* ��: GB2312 Code: 0xF3FD ==> Row:83 Col:93 */
    {0x7C1F,0xA1F4}, /* ��: GB2312 Code: 0xF4A1 ==> Row:84 Col:01 */
    {0x7C26,0xA3F4}, /* ��: GB2312 Code: 0xF4A3 ==> Row:84 Col:03 */
    {0x7C27,0xC9BB}, /* ��: GB2312 Code: 0xBBC9 ==> Row:27 Col:41 */
    {0x7C2A,0xA2F4}, /* ��: GB2312 Code: 0xF4A2 ==> Row:84 Col:02 */
    {0x7C38,0xA4F4}, /* ��: GB2312 Code: 0xF4A4 ==> Row:84 Col:04 */
    {0x7C3F,0xBEB2}, /* ��: GB2312 Code: 0xB2BE ==> Row:18 Col:30 */
    {0x7C40,0xA6F4}, /* ��: GB2312 Code: 0xF4A6 ==> Row:84 Col:06 */
    {0x7C41,0xA5F4}, /* ��: GB2312 Code: 0xF4A5 ==> Row:84 Col:05 */
    {0x7C4D,0xAEBC}, /* ��: GB2312 Code: 0xBCAE ==> Row:28 Col:14 */
    {0x7C73,0xD7C3}, /* ��: GB2312 Code: 0xC3D7 ==> Row:35 Col:55 */
    {0x7C74,0xE1D9}, /* ��: GB2312 Code: 0xD9E1 ==> Row:57 Col:65 */
    {0x7C7B,0xE0C0}, /* ��: GB2312 Code: 0xC0E0 ==> Row:32 Col:64 */
    {0x7C7C,0xCCF4}, /* ��: GB2312 Code: 0xF4CC ==> Row:84 Col:44 */
    {0x7C7D,0xD1D7}, /* ��: GB2312 Code: 0xD7D1 ==> Row:55 Col:49 */
    {0x7C89,0xDBB7}, /* ��: GB2312 Code: 0xB7DB ==> Row:23 Col:59 */
    {0x7C91,0xCEF4}, /* ��: GB2312 Code: 0xF4CE ==> Row:84 Col:46 */
    {0x7C92,0xA3C1}, /* ��: GB2312 Code: 0xC1A3 ==> Row:33 Col:03 */
    {0x7C95,0xC9C6}, /* ��: GB2312 Code: 0xC6C9 ==> Row:38 Col:41 */
    {0x7C97,0xD6B4}, /* ��: GB2312 Code: 0xB4D6 ==> Row:20 Col:54 */
    {0x7C98,0xB3D5}, /* ճ: GB2312 Code: 0xD5B3 ==> Row:53 Col:19 */
    {0x7C9C,0xD0F4}, /* ��: GB2312 Code: 0xF4D0 ==> Row:84 Col:48 */
    {0x7C9D,0xCFF4}, /* ��: GB2312 Code: 0xF4CF ==> Row:84 Col:47 */
    {0x7C9E,0xD1F4}, /* ��: GB2312 Code: 0xF4D1 ==> Row:84 Col:49 */
    {0x7C9F,0xDACB}, /* ��: GB2312 Code: 0xCBDA ==> Row:43 Col:58 */
    {0x7CA2,0xD2F4}, /* ��: GB2312 Code: 0xF4D2 ==> Row:84 Col:50 */
    {0x7CA4,0xC1D4}, /* ��: GB2312 Code: 0xD4C1 ==> Row:52 Col:33 */
    {0x7CA5,0xE0D6}, /* ��: GB2312 Code: 0xD6E0 ==> Row:54 Col:64 */
    {0x7CAA,0xE0B7}, /* ��: GB2312 Code: 0xB7E0 ==> Row:23 Col:64 */
    {0x7CAE,0xB8C1}, /* ��: GB2312 Code: 0xC1B8 ==> Row:33 Col:24 */
    {0x7CB1,0xBBC1}, /* ��: GB2312 Code: 0xC1BB ==> Row:33 Col:27 */
    {0x7CB2,0xD3F4}, /* ��: GB2312 Code: 0xF4D3 ==> Row:84 Col:51 */
    {0x7CB3,0xACBE}, /* ��: GB2312 Code: 0xBEAC ==> Row:30 Col:12 */
    {0x7CB9,0xE2B4}, /* ��: GB2312 Code: 0xB4E2 ==> Row:20 Col:66 */
    {0x7CBC,0xD4F4}, /* ��: GB2312 Code: 0xF4D4 ==> Row:84 Col:52 */
    {0x7CBD,0xD5F4}, /* ��: GB2312 Code: 0xF4D5 ==> Row:84 Col:53 */
    {0x7CBE,0xABBE}, /* ��: GB2312 Code: 0xBEAB ==> Row:30 Col:11 */
    {0x7CC1,0xD6F4}, /* ��: GB2312 Code: 0xF4D6 ==> Row:84 Col:54 */
    {0x7CC5,0xDBF4}, /* ��: GB2312 Code: 0xF4DB ==> Row:84 Col:59 */
    {0x7CC7,0xD7F4}, /* ��: GB2312 Code: 0xF4D7 ==> Row:84 Col:55 */
    {0x7CC8,0xDAF4}, /* ��: GB2312 Code: 0xF4DA ==> Row:84 Col:58 */
    {0x7CCA,0xFDBA}, /* ��: GB2312 Code: 0xBAFD ==> Row:26 Col:93 */
    {0x7CCC,0xD8F4}, /* ��: GB2312 Code: 0xF4D8 ==> Row:84 Col:56 */
    {0x7CCD,0xD9F4}, /* ��: GB2312 Code: 0xF4D9 ==> Row:84 Col:57 */
    {0x7CD5,0xE2B8}, /* ��: GB2312 Code: 0xB8E2 ==> Row:24 Col:66 */
    {0x7CD6,0xC7CC}, /* ��: GB2312 Code: 0xCCC7 ==> Row:44 Col:39 */
    {0x7CD7,0xDCF4}, /* ��: GB2312 Code: 0xF4DC ==> Row:84 Col:60 */
    {0x7CD9,0xDAB2}, /* ��: GB2312 Code: 0xB2DA ==> Row:18 Col:58 */
    {0x7CDC,0xD3C3}, /* ��: GB2312 Code: 0xC3D3 ==> Row:35 Col:51 */
    {0x7CDF,0xE3D4}, /* ��: GB2312 Code: 0xD4E3 ==> Row:52 Col:67 */
    {0x7CE0,0xB7BF}, /* ��: GB2312 Code: 0xBFB7 ==> Row:31 Col:23 */
    {0x7CE8,0xDDF4}, /* ��: GB2312 Code: 0xF4DD ==> Row:84 Col:61 */
    {0x7CEF,0xB4C5}, /* Ŵ: GB2312 Code: 0xC5B4 ==> Row:37 Col:20 */
    {0x7CF8,0xE9F4}, /* ��: GB2312 Code: 0xF4E9 ==> Row:84 Col:73 */
    {0x7CFB,0xB5CF}, /* ϵ: GB2312 Code: 0xCFB5 ==> Row:47 Col:21 */
    {0x7D0A,0xC9CE}, /* ��: GB2312 Code: 0xCEC9 ==> Row:46 Col:41 */
    {0x7D20,0xD8CB}, /* ��: GB2312 Code: 0xCBD8 ==> Row:43 Col:56 */
    {0x7D22,0xF7CB}, /* ��: GB2312 Code: 0xCBF7 ==> Row:43 Col:87 */
    {0x7D27,0xF4BD}, /* ��: GB2312 Code: 0xBDF4 ==> Row:29 Col:84 */
    {0x7D2B,0xCFD7}, /* ��: GB2312 Code: 0xD7CF ==> Row:55 Col:47 */
    {0x7D2F,0xDBC0}, /* ��: GB2312 Code: 0xC0DB ==> Row:32 Col:59 */
    {0x7D6E,0xF5D0}, /* ��: GB2312 Code: 0xD0F5 ==> Row:48 Col:85 */
    {0x7D77,0xEAF4}, /* ��: GB2312 Code: 0xF4EA ==> Row:84 Col:74 */
    {0x7DA6,0xEBF4}, /* ��: GB2312 Code: 0xF4EB ==> Row:84 Col:75 */
    {0x7DAE,0xECF4}, /* ��: GB2312 Code: 0xF4EC ==> Row:84 Col:76 */
    {0x7E3B,0xE3F7}, /* ��: GB2312 Code: 0xF7E3 ==> Row:87 Col:67 */
    {0x7E41,0xB1B7}, /* ��: GB2312 Code: 0xB7B1 ==> Row:23 Col:17 */
    {0x7E47,0xEDF4}, /* ��: GB2312 Code: 0xF4ED ==> Row:84 Col:77 */
    {0x7E82,0xEBD7}, /* ��: GB2312 Code: 0xD7EB ==> Row:55 Col:75 */
    {0x7E9B,0xEEF4}, /* ��: GB2312 Code: 0xF4EE ==> Row:84 Col:78 */
    {0x7E9F,0xF9E6}, /* ��: GB2312 Code: 0xE6F9 ==> Row:70 Col:89 */
    {0x7EA0,0xC0BE}, /* ��: GB2312 Code: 0xBEC0 ==> Row:30 Col:32 */
    {0x7EA1,0xFAE6}, /* ��: GB2312 Code: 0xE6FA ==> Row:70 Col:90 */
    {0x7EA2,0xECBA}, /* ��: GB2312 Code: 0xBAEC ==> Row:26 Col:76 */
    {0x7EA3,0xFBE6}, /* ��: GB2312 Code: 0xE6FB ==> Row:70 Col:91 */
    {0x7EA4,0xCBCF}, /* ��: GB2312 Code: 0xCFCB ==> Row:47 Col:43 */
    {0x7EA5,0xFCE6}, /* ��: GB2312 Code: 0xE6FC ==> Row:70 Col:92 */
    {0x7EA6,0xBCD4}, /* Լ: GB2312 Code: 0xD4BC ==> Row:52 Col:28 */
    {0x7EA7,0xB6BC}, /* ��: GB2312 Code: 0xBCB6 ==> Row:28 Col:22 */
    {0x7EA8,0xFDE6}, /* ��: GB2312 Code: 0xE6FD ==> Row:70 Col:93 */
    {0x7EA9,0xFEE6}, /* ��: GB2312 Code: 0xE6FE ==> Row:70 Col:94 */
    {0x7EAA,0xCDBC}, /* ��: GB2312 Code: 0xBCCD ==> Row:28 Col:45 */
    {0x7EAB,0xD2C8}, /* ��: GB2312 Code: 0xC8D2 ==> Row:40 Col:50 */
    {0x7EAC,0xB3CE}, /* γ: GB2312 Code: 0xCEB3 ==> Row:46 Col:19 */
    {0x7EAD,0xA1E7}, /* �: GB2312 Code: 0xE7A1 ==> Row:71 Col:01 */
    {0x7EAF,0xBFB4}, /* ��: GB2312 Code: 0xB4BF ==> Row:20 Col:31 */
    {0x7EB0,0xA2E7}, /* �: GB2312 Code: 0xE7A2 ==> Row:71 Col:02 */
    {0x7EB1,0xB4C9}, /* ɴ: GB2312 Code: 0xC9B4 ==> Row:41 Col:20 */
    {0x7EB2,0xD9B8}, /* ��: GB2312 Code: 0xB8D9 ==> Row:24 Col:57 */
    {0x7EB3,0xC9C4}, /* ��: GB2312 Code: 0xC4C9 ==> Row:36 Col:41 */
    {0x7EB5,0xDDD7}, /* ��: GB2312 Code: 0xD7DD ==> Row:55 Col:61 */
    {0x7EB6,0xDAC2}, /* ��: GB2312 Code: 0xC2DA ==> Row:34 Col:58 */
    {0x7EB7,0xD7B7}, /* ��: GB2312 Code: 0xB7D7 ==> Row:23 Col:55 */
    {0x7EB8,0xBDD6}, /* ֽ: GB2312 Code: 0xD6BD ==> Row:54 Col:29 */
    {0x7EB9,0xC6CE}, /* ��: GB2312 Code: 0xCEC6 ==> Row:46 Col:38 */
    {0x7EBA,0xC4B7}, /* ��: GB2312 Code: 0xB7C4 ==> Row:23 Col:36 */
    {0x7EBD,0xA6C5}, /* Ŧ: GB2312 Code: 0xC5A6 ==> Row:37 Col:06 */
    {0x7EBE,0xA3E7}, /* �: GB2312 Code: 0xE7A3 ==> Row:71 Col:03 */
    {0x7EBF,0xDFCF}, /* ��: GB2312 Code: 0xCFDF ==> Row:47 Col:63 */
    {0x7EC0,0xA4E7}, /* �: GB2312 Code: 0xE7A4 ==> Row:71 Col:04 */
    {0x7EC1,0xA5E7}, /* �: GB2312 Code: 0xE7A5 ==> Row:71 Col:05 */
    {0x7EC2,0xA6E7}, /* �: GB2312 Code: 0xE7A6 ==> Row:71 Col:06 */
    {0x7EC3,0xB7C1}, /* ��: GB2312 Code: 0xC1B7 ==> Row:33 Col:23 */
    {0x7EC4,0xE9D7}, /* ��: GB2312 Code: 0xD7E9 ==> Row:55 Col:73 */
    {0x7EC5,0xF0C9}, /* ��: GB2312 Code: 0xC9F0 ==> Row:41 Col:80 */
    {0x7EC6,0xB8CF}, /* ϸ: GB2312 Code: 0xCFB8 ==> Row:47 Col:24 */
    {0x7EC7,0xAFD6}, /* ֯: GB2312 Code: 0xD6AF ==> Row:54 Col:15 */
    {0x7EC8,0xD5D6}, /* ��: GB2312 Code: 0xD6D5 ==> Row:54 Col:53 */
    {0x7EC9,0xA7E7}, /* �: GB2312 Code: 0xE7A7 ==> Row:71 Col:07 */
    {0x7ECA,0xEDB0}, /* ��: GB2312 Code: 0xB0ED ==> Row:16 Col:77 */
    {0x7ECB,0xA8E7}, /* �: GB2312 Code: 0xE7A8 ==> Row:71 Col:08 */
    {0x7ECC,0xA9E7}, /* �: GB2312 Code: 0xE7A9 ==> Row:71 Col:09 */
    {0x7ECD,0xDCC9}, /* ��: GB2312 Code: 0xC9DC ==> Row:41 Col:60 */
    {0x7ECE,0xEFD2}, /* ��: GB2312 Code: 0xD2EF ==> Row:50 Col:79 */
    {0x7ECF,0xADBE}, /* ��: GB2312 Code: 0xBEAD ==> Row:30 Col:13 */
    {0x7ED0,0xAAE7}, /* �: GB2312 Code: 0xE7AA ==> Row:71 Col:10 */
    {0x7ED1,0xF3B0}, /* ��: GB2312 Code: 0xB0F3 ==> Row:16 Col:83 */
    {0x7ED2,0xDEC8}, /* ��: GB2312 Code: 0xC8DE ==> Row:40 Col:62 */
    {0x7ED3,0xE1BD}, /* ��: GB2312 Code: 0xBDE1 ==> Row:29 Col:65 */
    {0x7ED4,0xABE7}, /* �: GB2312 Code: 0xE7AB ==> Row:71 Col:11 */
    {0x7ED5,0xC6C8}, /* ��: GB2312 Code: 0xC8C6 ==> Row:40 Col:38 */
    {0x7ED7,0xACE7}, /* �: GB2312 Code: 0xE7AC ==> Row:71 Col:12 */
    {0x7ED8,0xE6BB}, /* ��: GB2312 Code: 0xBBE6 ==> Row:27 Col:70 */
    {0x7ED9,0xF8B8}, /* ��: GB2312 Code: 0xB8F8 ==> Row:24 Col:88 */
    {0x7EDA,0xA4D1}, /* Ѥ: GB2312 Code: 0xD1A4 ==> Row:49 Col:04 */
    {0x7EDB,0xADE7}, /* �: GB2312 Code: 0xE7AD ==> Row:71 Col:13 */
    {0x7EDC,0xE7C2}, /* ��: GB2312 Code: 0xC2E7 ==> Row:34 Col:71 */
    {0x7EDD,0xF8BE}, /* ��: GB2312 Code: 0xBEF8 ==> Row:30 Col:88 */
    {0x7EDE,0xCABD}, /* ��: GB2312 Code: 0xBDCA ==> Row:29 Col:42 */
    {0x7EDF,0xB3CD}, /* ͳ: GB2312 Code: 0xCDB3 ==> Row:45 Col:19 */
    {0x7EE0,0xAEE7}, /* �: GB2312 Code: 0xE7AE ==> Row:71 Col:14 */
    {0x7EE1,0xAFE7}, /* �: GB2312 Code: 0xE7AF ==> Row:71 Col:15 */
    {0x7EE2,0xEEBE}, /* ��: GB2312 Code: 0xBEEE ==> Row:30 Col:78 */
    {0x7EE3,0xE5D0}, /* ��: GB2312 Code: 0xD0E5 ==> Row:48 Col:69 */
    {0x7EE5,0xE7CB}, /* ��: GB2312 Code: 0xCBE7 ==> Row:43 Col:71 */
    {0x7EE6,0xD0CC}, /* ��: GB2312 Code: 0xCCD0 ==> Row:44 Col:48 */
    {0x7EE7,0xCCBC}, /* ��: GB2312 Code: 0xBCCC ==> Row:28 Col:44 */
    {0x7EE8,0xB0E7}, /* �: GB2312 Code: 0xE7B0 ==> Row:71 Col:16 */
    {0x7EE9,0xA8BC}, /* ��: GB2312 Code: 0xBCA8 ==> Row:28 Col:08 */
    {0x7EEA,0xF7D0}, /* ��: GB2312 Code: 0xD0F7 ==> Row:48 Col:87 */
    {0x7EEB,0xB1E7}, /* �: GB2312 Code: 0xE7B1 ==> Row:71 Col:17 */
    {0x7EED,0xF8D0}, /* ��: GB2312 Code: 0xD0F8 ==> Row:48 Col:88 */
    {0x7EEE,0xB2E7}, /* �: GB2312 Code: 0xE7B2 ==> Row:71 Col:18 */
    {0x7EEF,0xB3E7}, /* �: GB2312 Code: 0xE7B3 ==> Row:71 Col:19 */
    {0x7EF0,0xC2B4}, /* ��: GB2312 Code: 0xB4C2 ==> Row:20 Col:34 */
    {0x7EF1,0xB4E7}, /* �: GB2312 Code: 0xE7B4 ==> Row:71 Col:20 */
    {0x7EF2,0xB5E7}, /* �: GB2312 Code: 0xE7B5 ==> Row:71 Col:21 */
    {0x7EF3,0xFEC9}, /* ��: GB2312 Code: 0xC9FE ==> Row:41 Col:94 */
    {0x7EF4,0xACCE}, /* ά: GB2312 Code: 0xCEAC ==> Row:46 Col:12 */
    {0x7EF5,0xE0C3}, /* ��: GB2312 Code: 0xC3E0 ==> Row:35 Col:64 */
    {0x7EF6,0xB7E7}, /* �: GB2312 Code: 0xE7B7 ==> Row:71 Col:23 */
    {0x7EF7,0xC1B1}, /* ��: GB2312 Code: 0xB1C1 ==> Row:17 Col:33 */
    {0x7EF8,0xF1B3}, /* ��: GB2312 Code: 0xB3F1 ==> Row:19 Col:81 */
    {0x7EFA,0xB8E7}, /* �: GB2312 Code: 0xE7B8 ==> Row:71 Col:24 */
    {0x7EFB,0xB9E7}, /* �: GB2312 Code: 0xE7B9 ==> Row:71 Col:25 */
    {0x7EFC,0xDBD7}, /* ��: GB2312 Code: 0xD7DB ==> Row:55 Col:59 */
    {0x7EFD,0xC0D5}, /* ��: GB2312 Code: 0xD5C0 ==> Row:53 Col:32 */
    {0x7EFE,0xBAE7}, /* �: GB2312 Code: 0xE7BA ==> Row:71 Col:26 */
    {0x7EFF,0xCCC2}, /* ��: GB2312 Code: 0xC2CC ==> Row:34 Col:44 */
    {0x7F00,0xBAD7}, /* ׺: GB2312 Code: 0xD7BA ==> Row:55 Col:26 */
    {0x7F01,0xBBE7}, /* �: GB2312 Code: 0xE7BB ==> Row:71 Col:27 */
    {0x7F02,0xBCE7}, /* �: GB2312 Code: 0xE7BC ==> Row:71 Col:28 */
    {0x7F03,0xBDE7}, /* �: GB2312 Code: 0xE7BD ==> Row:71 Col:29 */
    {0x7F04,0xEABC}, /* ��: GB2312 Code: 0xBCEA ==> Row:28 Col:74 */
    {0x7F05,0xE5C3}, /* ��: GB2312 Code: 0xC3E5 ==> Row:35 Col:69 */
    {0x7F06,0xC2C0}, /* ��: GB2312 Code: 0xC0C2 ==> Row:32 Col:34 */
    {0x7F07,0xBEE7}, /* �: GB2312 Code: 0xE7BE ==> Row:71 Col:30 */
    {0x7F08,0xBFE7}, /* �: GB2312 Code: 0xE7BF ==> Row:71 Col:31 */
    {0x7F09,0xA9BC}, /* ��: GB2312 Code: 0xBCA9 ==> Row:28 Col:09 */
    {0x7F0B,0xC0E7}, /* ��: GB2312 Code: 0xE7C0 ==> Row:71 Col:32 */
    {0x7F0C,0xC1E7}, /* ��: GB2312 Code: 0xE7C1 ==> Row:71 Col:33 */
    {0x7F0D,0xB6E7}, /* �: GB2312 Code: 0xE7B6 ==> Row:71 Col:22 */
    {0x7F0E,0xD0B6}, /* ��: GB2312 Code: 0xB6D0 ==> Row:22 Col:48 */
    {0x7F0F,0xC2E7}, /* ��: GB2312 Code: 0xE7C2 ==> Row:71 Col:34 */
    {0x7F11,0xC3E7}, /* ��: GB2312 Code: 0xE7C3 ==> Row:71 Col:35 */
    {0x7F12,0xC4E7}, /* ��: GB2312 Code: 0xE7C4 ==> Row:71 Col:36 */
    {0x7F13,0xBABB}, /* ��: GB2312 Code: 0xBBBA ==> Row:27 Col:26 */
    {0x7F14,0xDEB5}, /* ��: GB2312 Code: 0xB5DE ==> Row:21 Col:62 */
    {0x7F15,0xC6C2}, /* ��: GB2312 Code: 0xC2C6 ==> Row:34 Col:38 */
    {0x7F16,0xE0B1}, /* ��: GB2312 Code: 0xB1E0 ==> Row:17 Col:64 */
    {0x7F17,0xC5E7}, /* ��: GB2312 Code: 0xE7C5 ==> Row:71 Col:37 */
    {0x7F18,0xB5D4}, /* Ե: GB2312 Code: 0xD4B5 ==> Row:52 Col:21 */
    {0x7F19,0xC6E7}, /* ��: GB2312 Code: 0xE7C6 ==> Row:71 Col:38 */
    {0x7F1A,0xBFB8}, /* ��: GB2312 Code: 0xB8BF ==> Row:24 Col:31 */
    {0x7F1B,0xC8E7}, /* ��: GB2312 Code: 0xE7C8 ==> Row:71 Col:40 */
    {0x7F1C,0xC7E7}, /* ��: GB2312 Code: 0xE7C7 ==> Row:71 Col:39 */
    {0x7F1D,0xECB7}, /* ��: GB2312 Code: 0xB7EC ==> Row:23 Col:76 */
    {0x7F1F,0xC9E7}, /* ��: GB2312 Code: 0xE7C9 ==> Row:71 Col:41 */
    {0x7F20,0xF8B2}, /* ��: GB2312 Code: 0xB2F8 ==> Row:18 Col:88 */
    {0x7F21,0xCAE7}, /* ��: GB2312 Code: 0xE7CA ==> Row:71 Col:42 */
    {0x7F22,0xCBE7}, /* ��: GB2312 Code: 0xE7CB ==> Row:71 Col:43 */
    {0x7F23,0xCCE7}, /* ��: GB2312 Code: 0xE7CC ==> Row:71 Col:44 */
    {0x7F24,0xCDE7}, /* ��: GB2312 Code: 0xE7CD ==> Row:71 Col:45 */
    {0x7F25,0xCEE7}, /* ��: GB2312 Code: 0xE7CE ==> Row:71 Col:46 */
    {0x7F26,0xCFE7}, /* ��: GB2312 Code: 0xE7CF ==> Row:71 Col:47 */
    {0x7F27,0xD0E7}, /* ��: GB2312 Code: 0xE7D0 ==> Row:71 Col:48 */
    {0x7F28,0xA7D3}, /* ӧ: GB2312 Code: 0xD3A7 ==> Row:51 Col:07 */
    {0x7F29,0xF5CB}, /* ��: GB2312 Code: 0xCBF5 ==> Row:43 Col:85 */
    {0x7F2A,0xD1E7}, /* ��: GB2312 Code: 0xE7D1 ==> Row:71 Col:49 */
    {0x7F2B,0xD2E7}, /* ��: GB2312 Code: 0xE7D2 ==> Row:71 Col:50 */
    {0x7F2C,0xD3E7}, /* ��: GB2312 Code: 0xE7D3 ==> Row:71 Col:51 */
    {0x7F2D,0xD4E7}, /* ��: GB2312 Code: 0xE7D4 ==> Row:71 Col:52 */
    {0x7F2E,0xC9C9}, /* ��: GB2312 Code: 0xC9C9 ==> Row:41 Col:41 */
    {0x7F2F,0xD5E7}, /* ��: GB2312 Code: 0xE7D5 ==> Row:71 Col:53 */
    {0x7F30,0xD6E7}, /* ��: GB2312 Code: 0xE7D6 ==> Row:71 Col:54 */
    {0x7F31,0xD7E7}, /* ��: GB2312 Code: 0xE7D7 ==> Row:71 Col:55 */
    {0x7F32,0xD8E7}, /* ��: GB2312 Code: 0xE7D8 ==> Row:71 Col:56 */
    {0x7F33,0xD9E7}, /* ��: GB2312 Code: 0xE7D9 ==> Row:71 Col:57 */
    {0x7F34,0xC9BD}, /* ��: GB2312 Code: 0xBDC9 ==> Row:29 Col:41 */
    {0x7F35,0xDAE7}, /* ��: GB2312 Code: 0xE7DA ==> Row:71 Col:58 */
    {0x7F36,0xBEF3}, /* �: GB2312 Code: 0xF3BE ==> Row:83 Col:30 */
    {0x7F38,0xD7B8}, /* ��: GB2312 Code: 0xB8D7 ==> Row:24 Col:55 */
    {0x7F3A,0xB1C8}, /* ȱ: GB2312 Code: 0xC8B1 ==> Row:40 Col:17 */
    {0x7F42,0xBFF3}, /* �: GB2312 Code: 0xF3BF ==> Row:83 Col:31 */
    {0x7F44,0xC0F3}, /* ��: GB2312 Code: 0xF3C0 ==> Row:83 Col:32 */
    {0x7F45,0xC1F3}, /* ��: GB2312 Code: 0xF3C1 ==> Row:83 Col:33 */
    {0x7F50,0xDEB9}, /* ��: GB2312 Code: 0xB9DE ==> Row:25 Col:62 */
    {0x7F51,0xF8CD}, /* ��: GB2312 Code: 0xCDF8 ==> Row:45 Col:88 */
    {0x7F54,0xE8D8}, /* ��: GB2312 Code: 0xD8E8 ==> Row:56 Col:72 */
    {0x7F55,0xB1BA}, /* ��: GB2312 Code: 0xBAB1 ==> Row:26 Col:17 */
    {0x7F57,0xDEC2}, /* ��: GB2312 Code: 0xC2DE ==> Row:34 Col:62 */
    {0x7F58,0xB7EE}, /* �: GB2312 Code: 0xEEB7 ==> Row:78 Col:23 */
    {0x7F5A,0xA3B7}, /* ��: GB2312 Code: 0xB7A3 ==> Row:23 Col:03 */
    {0x7F5F,0xB9EE}, /* �: GB2312 Code: 0xEEB9 ==> Row:78 Col:25 */
    {0x7F61,0xB8EE}, /* �: GB2312 Code: 0xEEB8 ==> Row:78 Col:24 */
    {0x7F62,0xD5B0}, /* ��: GB2312 Code: 0xB0D5 ==> Row:16 Col:53 */
    {0x7F68,0xBBEE}, /* �: GB2312 Code: 0xEEBB ==> Row:78 Col:27 */
    {0x7F69,0xD6D5}, /* ��: GB2312 Code: 0xD5D6 ==> Row:53 Col:54 */
    {0x7F6A,0xEFD7}, /* ��: GB2312 Code: 0xD7EF ==> Row:55 Col:79 */
    {0x7F6E,0xC3D6}, /* ��: GB2312 Code: 0xD6C3 ==> Row:54 Col:35 */
    {0x7F71,0xBDEE}, /* �: GB2312 Code: 0xEEBD ==> Row:78 Col:29 */
    {0x7F72,0xF0CA}, /* ��: GB2312 Code: 0xCAF0 ==> Row:42 Col:80 */
    {0x7F74,0xBCEE}, /* �: GB2312 Code: 0xEEBC ==> Row:78 Col:28 */
    {0x7F79,0xBEEE}, /* �: GB2312 Code: 0xEEBE ==> Row:78 Col:30 */
    {0x7F7E,0xC0EE}, /* ��: GB2312 Code: 0xEEC0 ==> Row:78 Col:32 */
    {0x7F81,0xBFEE}, /* �: GB2312 Code: 0xEEBF ==> Row:78 Col:31 */
    {0x7F8A,0xF2D1}, /* ��: GB2312 Code: 0xD1F2 ==> Row:49 Col:82 */
    {0x7F8C,0xBCC7}, /* Ǽ: GB2312 Code: 0xC7BC ==> Row:39 Col:28 */
    {0x7F8E,0xC0C3}, /* ��: GB2312 Code: 0xC3C0 ==> Row:35 Col:32 */
    {0x7F94,0xE1B8}, /* ��: GB2312 Code: 0xB8E1 ==> Row:24 Col:65 */
    {0x7F9A,0xE7C1}, /* ��: GB2312 Code: 0xC1E7 ==> Row:33 Col:71 */
    {0x7F9D,0xC6F4}, /* ��: GB2312 Code: 0xF4C6 ==> Row:84 Col:38 */
    {0x7F9E,0xDFD0}, /* ��: GB2312 Code: 0xD0DF ==> Row:48 Col:63 */
    {0x7F9F,0xC7F4}, /* ��: GB2312 Code: 0xF4C7 ==> Row:84 Col:39 */
    {0x7FA1,0xDBCF}, /* ��: GB2312 Code: 0xCFDB ==> Row:47 Col:59 */
    {0x7FA4,0xBAC8}, /* Ⱥ: GB2312 Code: 0xC8BA ==> Row:40 Col:26 */
    {0x7FA7,0xC8F4}, /* ��: GB2312 Code: 0xF4C8 ==> Row:84 Col:40 */
    {0x7FAF,0xC9F4}, /* ��: GB2312 Code: 0xF4C9 ==> Row:84 Col:41 */
    {0x7FB0,0xCAF4}, /* ��: GB2312 Code: 0xF4CA ==> Row:84 Col:42 */
    {0x7FB2,0xCBF4}, /* ��: GB2312 Code: 0xF4CB ==> Row:84 Col:43 */
    {0x7FB8,0xFAD9}, /* ��: GB2312 Code: 0xD9FA ==> Row:57 Col:90 */
    {0x7FB9,0xFEB8}, /* ��: GB2312 Code: 0xB8FE ==> Row:24 Col:94 */
    {0x7FBC,0xF1E5}, /* ��: GB2312 Code: 0xE5F1 ==> Row:69 Col:81 */
    {0x7FBD,0xF0D3}, /* ��: GB2312 Code: 0xD3F0 ==> Row:51 Col:80 */
    {0x7FBF,0xE0F4}, /* ��: GB2312 Code: 0xF4E0 ==> Row:84 Col:64 */
    {0x7FC1,0xCCCE}, /* ��: GB2312 Code: 0xCECC ==> Row:46 Col:44 */
    {0x7FC5,0xE1B3}, /* ��: GB2312 Code: 0xB3E1 ==> Row:19 Col:65 */
    {0x7FCA,0xB4F1}, /* �: GB2312 Code: 0xF1B4 ==> Row:81 Col:20 */
    {0x7FCC,0xEED2}, /* ��: GB2312 Code: 0xD2EE ==> Row:50 Col:78 */
    {0x7FCE,0xE1F4}, /* ��: GB2312 Code: 0xF4E1 ==> Row:84 Col:65 */
    {0x7FD4,0xE8CF}, /* ��: GB2312 Code: 0xCFE8 ==> Row:47 Col:72 */
    {0x7FD5,0xE2F4}, /* ��: GB2312 Code: 0xF4E2 ==> Row:84 Col:66 */
    {0x7FD8,0xCCC7}, /* ��: GB2312 Code: 0xC7CC ==> Row:39 Col:44 */
    {0x7FDF,0xD4B5}, /* ��: GB2312 Code: 0xB5D4 ==> Row:21 Col:52 */
    {0x7FE0,0xE4B4}, /* ��: GB2312 Code: 0xB4E4 ==> Row:20 Col:68 */
    {0x7FE1,0xE4F4}, /* ��: GB2312 Code: 0xF4E4 ==> Row:84 Col:68 */
    {0x7FE5,0xE3F4}, /* ��: GB2312 Code: 0xF4E3 ==> Row:84 Col:67 */
    {0x7FE6,0xE5F4}, /* ��: GB2312 Code: 0xF4E5 ==> Row:84 Col:69 */
    {0x7FE9,0xE6F4}, /* ��: GB2312 Code: 0xF4E6 ==> Row:84 Col:70 */
    {0x7FEE,0xE7F4}, /* ��: GB2312 Code: 0xF4E7 ==> Row:84 Col:71 */
    {0x7FF0,0xB2BA}, /* ��: GB2312 Code: 0xBAB2 ==> Row:26 Col:18 */
    {0x7FF1,0xBFB0}, /* ��: GB2312 Code: 0xB0BF ==> Row:16 Col:31 */
    {0x7FF3,0xE8F4}, /* ��: GB2312 Code: 0xF4E8 ==> Row:84 Col:72 */
    {0x7FFB,0xADB7}, /* ��: GB2312 Code: 0xB7AD ==> Row:23 Col:13 */
    {0x7FFC,0xEDD2}, /* ��: GB2312 Code: 0xD2ED ==> Row:50 Col:77 */
    {0x8000,0xABD2}, /* ҫ: GB2312 Code: 0xD2AB ==> Row:50 Col:11 */
    {0x8001,0xCFC0}, /* ��: GB2312 Code: 0xC0CF ==> Row:32 Col:47 */
    {0x8003,0xBCBF}, /* ��: GB2312 Code: 0xBFBC ==> Row:31 Col:28 */
    {0x8004,0xA3EB}, /* �: GB2312 Code: 0xEBA3 ==> Row:75 Col:03 */
    {0x8005,0xDFD5}, /* ��: GB2312 Code: 0xD5DF ==> Row:53 Col:63 */
    {0x8006,0xC8EA}, /* ��: GB2312 Code: 0xEAC8 ==> Row:74 Col:40 */
    {0x800B,0xF3F1}, /* ��: GB2312 Code: 0xF1F3 ==> Row:81 Col:83 */
    {0x800C,0xF8B6}, /* ��: GB2312 Code: 0xB6F8 ==> Row:22 Col:88 */
    {0x800D,0xA3CB}, /* ˣ: GB2312 Code: 0xCBA3 ==> Row:43 Col:03 */
    {0x8010,0xCDC4}, /* ��: GB2312 Code: 0xC4CD ==> Row:36 Col:45 */
    {0x8012,0xE7F1}, /* ��: GB2312 Code: 0xF1E7 ==> Row:81 Col:71 */
    {0x8014,0xE8F1}, /* ��: GB2312 Code: 0xF1E8 ==> Row:81 Col:72 */
    {0x8015,0xFBB8}, /* ��: GB2312 Code: 0xB8FB ==> Row:24 Col:91 */
    {0x8016,0xE9F1}, /* ��: GB2312 Code: 0xF1E9 ==> Row:81 Col:73 */
    {0x8017,0xC4BA}, /* ��: GB2312 Code: 0xBAC4 ==> Row:26 Col:36 */
    {0x8018,0xC5D4}, /* ��: GB2312 Code: 0xD4C5 ==> Row:52 Col:37 */
    {0x8019,0xD2B0}, /* ��: GB2312 Code: 0xB0D2 ==> Row:16 Col:50 */
    {0x801C,0xEAF1}, /* ��: GB2312 Code: 0xF1EA ==> Row:81 Col:74 */
    {0x8020,0xEBF1}, /* ��: GB2312 Code: 0xF1EB ==> Row:81 Col:75 */
    {0x8022,0xECF1}, /* ��: GB2312 Code: 0xF1EC ==> Row:81 Col:76 */
    {0x8025,0xEDF1}, /* ��: GB2312 Code: 0xF1ED ==> Row:81 Col:77 */
    {0x8026,0xEEF1}, /* ��: GB2312 Code: 0xF1EE ==> Row:81 Col:78 */
    {0x8027,0xEFF1}, /* ��: GB2312 Code: 0xF1EF ==> Row:81 Col:79 */
    {0x8028,0xF1F1}, /* ��: GB2312 Code: 0xF1F1 ==> Row:81 Col:81 */
    {0x8029,0xF0F1}, /* ��: GB2312 Code: 0xF1F0 ==> Row:81 Col:80 */
    {0x802A,0xD5C5}, /* ��: GB2312 Code: 0xC5D5 ==> Row:37 Col:53 */
    {0x8031,0xF2F1}, /* ��: GB2312 Code: 0xF1F2 ==> Row:81 Col:82 */
    {0x8033,0xFAB6}, /* ��: GB2312 Code: 0xB6FA ==> Row:22 Col:90 */
    {0x8035,0xF4F1}, /* ��: GB2312 Code: 0xF1F4 ==> Row:81 Col:84 */
    {0x8036,0xAED2}, /* Ү: GB2312 Code: 0xD2AE ==> Row:50 Col:14 */
    {0x8037,0xC7DE}, /* ��: GB2312 Code: 0xDEC7 ==> Row:62 Col:39 */
    {0x8038,0xCACB}, /* ��: GB2312 Code: 0xCBCA ==> Row:43 Col:42 */
    {0x803B,0xDCB3}, /* ��: GB2312 Code: 0xB3DC ==> Row:19 Col:60 */
    {0x803D,0xA2B5}, /* ��: GB2312 Code: 0xB5A2 ==> Row:21 Col:02 */
    {0x803F,0xA2B9}, /* ��: GB2312 Code: 0xB9A2 ==> Row:25 Col:02 */
    {0x8042,0xF4C4}, /* ��: GB2312 Code: 0xC4F4 ==> Row:36 Col:84 */
    {0x8043,0xF5F1}, /* ��: GB2312 Code: 0xF1F5 ==> Row:81 Col:85 */
    {0x8046,0xF6F1}, /* ��: GB2312 Code: 0xF1F6 ==> Row:81 Col:86 */
    {0x804A,0xC4C1}, /* ��: GB2312 Code: 0xC1C4 ==> Row:33 Col:36 */
    {0x804B,0xFBC1}, /* ��: GB2312 Code: 0xC1FB ==> Row:33 Col:91 */
    {0x804C,0xB0D6}, /* ְ: GB2312 Code: 0xD6B0 ==> Row:54 Col:16 */
    {0x804D,0xF7F1}, /* ��: GB2312 Code: 0xF1F7 ==> Row:81 Col:87 */
    {0x8052,0xF8F1}, /* ��: GB2312 Code: 0xF1F8 ==> Row:81 Col:88 */
    {0x8054,0xAAC1}, /* ��: GB2312 Code: 0xC1AA ==> Row:33 Col:10 */
    {0x8058,0xB8C6}, /* Ƹ: GB2312 Code: 0xC6B8 ==> Row:38 Col:24 */
    {0x805A,0xDBBE}, /* ��: GB2312 Code: 0xBEDB ==> Row:30 Col:59 */
    {0x8069,0xF9F1}, /* ��: GB2312 Code: 0xF1F9 ==> Row:81 Col:89 */
    {0x806A,0xCFB4}, /* ��: GB2312 Code: 0xB4CF ==> Row:20 Col:47 */
    {0x8071,0xFAF1}, /* ��: GB2312 Code: 0xF1FA ==> Row:81 Col:90 */
    {0x807F,0xB2ED}, /* ��: GB2312 Code: 0xEDB2 ==> Row:77 Col:18 */
    {0x8080,0xB1ED}, /* ��: GB2312 Code: 0xEDB1 ==> Row:77 Col:17 */
    {0x8083,0xE0CB}, /* ��: GB2312 Code: 0xCBE0 ==> Row:43 Col:64 */
    {0x8084,0xDED2}, /* ��: GB2312 Code: 0xD2DE ==> Row:50 Col:62 */
    {0x8086,0xC1CB}, /* ��: GB2312 Code: 0xCBC1 ==> Row:43 Col:33 */
    {0x8087,0xD8D5}, /* ��: GB2312 Code: 0xD5D8 ==> Row:53 Col:56 */
    {0x8089,0xE2C8}, /* ��: GB2312 Code: 0xC8E2 ==> Row:40 Col:66 */
    {0x808B,0xDFC0}, /* ��: GB2312 Code: 0xC0DF ==> Row:32 Col:63 */
    {0x808C,0xA1BC}, /* ��: GB2312 Code: 0xBCA1 ==> Row:28 Col:01 */
    {0x8093,0xC1EB}, /* ��: GB2312 Code: 0xEBC1 ==> Row:75 Col:33 */
    {0x8096,0xA4D0}, /* Ф: GB2312 Code: 0xD0A4 ==> Row:48 Col:04 */
    {0x8098,0xE2D6}, /* ��: GB2312 Code: 0xD6E2 ==> Row:54 Col:66 */
    {0x809A,0xC7B6}, /* ��: GB2312 Code: 0xB6C7 ==> Row:22 Col:39 */
    {0x809B,0xD8B8}, /* ��: GB2312 Code: 0xB8D8 ==> Row:24 Col:56 */
    {0x809C,0xC0EB}, /* ��: GB2312 Code: 0xEBC0 ==> Row:75 Col:32 */
    {0x809D,0xCEB8}, /* ��: GB2312 Code: 0xB8CE ==> Row:24 Col:46 */
    {0x809F,0xBFEB}, /* �: GB2312 Code: 0xEBBF ==> Row:75 Col:31 */
    {0x80A0,0xA6B3}, /* ��: GB2312 Code: 0xB3A6 ==> Row:19 Col:06 */
    {0x80A1,0xC9B9}, /* ��: GB2312 Code: 0xB9C9 ==> Row:25 Col:41 */
    {0x80A2,0xABD6}, /* ֫: GB2312 Code: 0xD6AB ==> Row:54 Col:11 */
    {0x80A4,0xF4B7}, /* ��: GB2312 Code: 0xB7F4 ==> Row:23 Col:84 */
    {0x80A5,0xCAB7}, /* ��: GB2312 Code: 0xB7CA ==> Row:23 Col:42 */
    {0x80A9,0xE7BC}, /* ��: GB2312 Code: 0xBCE7 ==> Row:28 Col:71 */
    {0x80AA,0xBEB7}, /* ��: GB2312 Code: 0xB7BE ==> Row:23 Col:30 */
    {0x80AB,0xC6EB}, /* ��: GB2312 Code: 0xEBC6 ==> Row:75 Col:38 */
    {0x80AD,0xC7EB}, /* ��: GB2312 Code: 0xEBC7 ==> Row:75 Col:39 */
    {0x80AE,0xB9B0}, /* ��: GB2312 Code: 0xB0B9 ==> Row:16 Col:25 */
    {0x80AF,0xCFBF}, /* ��: GB2312 Code: 0xBFCF ==> Row:31 Col:47 */
    {0x80B1,0xC5EB}, /* ��: GB2312 Code: 0xEBC5 ==> Row:75 Col:37 */
    {0x80B2,0xFDD3}, /* ��: GB2312 Code: 0xD3FD ==> Row:51 Col:93 */
    {0x80B4,0xC8EB}, /* ��: GB2312 Code: 0xEBC8 ==> Row:75 Col:40 */
    {0x80B7,0xC9EB}, /* ��: GB2312 Code: 0xEBC9 ==> Row:75 Col:41 */
    {0x80BA,0xCEB7}, /* ��: GB2312 Code: 0xB7CE ==> Row:23 Col:46 */
    {0x80BC,0xC2EB}, /* ��: GB2312 Code: 0xEBC2 ==> Row:75 Col:34 */
    {0x80BD,0xC4EB}, /* ��: GB2312 Code: 0xEBC4 ==> Row:75 Col:36 */
    {0x80BE,0xF6C9}, /* ��: GB2312 Code: 0xC9F6 ==> Row:41 Col:86 */
    {0x80BF,0xD7D6}, /* ��: GB2312 Code: 0xD6D7 ==> Row:54 Col:55 */
    {0x80C0,0xCDD5}, /* ��: GB2312 Code: 0xD5CD ==> Row:53 Col:45 */
    {0x80C1,0xB2D0}, /* в: GB2312 Code: 0xD0B2 ==> Row:48 Col:18 */
    {0x80C2,0xCFEB}, /* ��: GB2312 Code: 0xEBCF ==> Row:75 Col:47 */
    {0x80C3,0xB8CE}, /* θ: GB2312 Code: 0xCEB8 ==> Row:46 Col:24 */
    {0x80C4,0xD0EB}, /* ��: GB2312 Code: 0xEBD0 ==> Row:75 Col:48 */
    {0x80C6,0xA8B5}, /* ��: GB2312 Code: 0xB5A8 ==> Row:21 Col:08 */
    {0x80CC,0xB3B1}, /* ��: GB2312 Code: 0xB1B3 ==> Row:17 Col:19 */
    {0x80CD,0xD2EB}, /* ��: GB2312 Code: 0xEBD2 ==> Row:75 Col:50 */
    {0x80CE,0xA5CC}, /* ̥: GB2312 Code: 0xCCA5 ==> Row:44 Col:05 */
    {0x80D6,0xD6C5}, /* ��: GB2312 Code: 0xC5D6 ==> Row:37 Col:54 */
    {0x80D7,0xD3EB}, /* ��: GB2312 Code: 0xEBD3 ==> Row:75 Col:51 */
    {0x80D9,0xD1EB}, /* ��: GB2312 Code: 0xEBD1 ==> Row:75 Col:49 */
    {0x80DA,0xDFC5}, /* ��: GB2312 Code: 0xC5DF ==> Row:37 Col:63 */
    {0x80DB,0xCEEB}, /* ��: GB2312 Code: 0xEBCE ==> Row:75 Col:46 */
    {0x80DC,0xA4CA}, /* ʤ: GB2312 Code: 0xCAA4 ==> Row:42 Col:04 */
    {0x80DD,0xD5EB}, /* ��: GB2312 Code: 0xEBD5 ==> Row:75 Col:53 */
    {0x80DE,0xFBB0}, /* ��: GB2312 Code: 0xB0FB ==> Row:16 Col:91 */
    {0x80E1,0xFABA}, /* ��: GB2312 Code: 0xBAFA ==> Row:26 Col:90 */
    {0x80E4,0xB7D8}, /* ط: GB2312 Code: 0xD8B7 ==> Row:56 Col:23 */
    {0x80E5,0xE3F1}, /* ��: GB2312 Code: 0xF1E3 ==> Row:81 Col:67 */
    {0x80E7,0xCAEB}, /* ��: GB2312 Code: 0xEBCA ==> Row:75 Col:42 */
    {0x80E8,0xCBEB}, /* ��: GB2312 Code: 0xEBCB ==> Row:75 Col:43 */
    {0x80E9,0xCCEB}, /* ��: GB2312 Code: 0xEBCC ==> Row:75 Col:44 */
    {0x80EA,0xCDEB}, /* ��: GB2312 Code: 0xEBCD ==> Row:75 Col:45 */
    {0x80EB,0xD6EB}, /* ��: GB2312 Code: 0xEBD6 ==> Row:75 Col:54 */
    {0x80EC,0xC0E6}, /* ��: GB2312 Code: 0xE6C0 ==> Row:70 Col:32 */
    {0x80ED,0xD9EB}, /* ��: GB2312 Code: 0xEBD9 ==> Row:75 Col:57 */
    {0x80EF,0xE8BF}, /* ��: GB2312 Code: 0xBFE8 ==> Row:31 Col:72 */
    {0x80F0,0xC8D2}, /* ��: GB2312 Code: 0xD2C8 ==> Row:50 Col:40 */
    {0x80F1,0xD7EB}, /* ��: GB2312 Code: 0xEBD7 ==> Row:75 Col:55 */
    {0x80F2,0xDCEB}, /* ��: GB2312 Code: 0xEBDC ==> Row:75 Col:60 */
    {0x80F3,0xECB8}, /* ��: GB2312 Code: 0xB8EC ==> Row:24 Col:76 */
    {0x80F4,0xD8EB}, /* ��: GB2312 Code: 0xEBD8 ==> Row:75 Col:56 */
    {0x80F6,0xBABD}, /* ��: GB2312 Code: 0xBDBA ==> Row:29 Col:26 */
    {0x80F8,0xD8D0}, /* ��: GB2312 Code: 0xD0D8 ==> Row:48 Col:56 */
    {0x80FA,0xB7B0}, /* ��: GB2312 Code: 0xB0B7 ==> Row:16 Col:23 */
    {0x80FC,0xDDEB}, /* ��: GB2312 Code: 0xEBDD ==> Row:75 Col:61 */
    {0x80FD,0xDCC4}, /* ��: GB2312 Code: 0xC4DC ==> Row:36 Col:60 */
    {0x8102,0xACD6}, /* ֬: GB2312 Code: 0xD6AC ==> Row:54 Col:12 */
    {0x8106,0xE0B4}, /* ��: GB2312 Code: 0xB4E0 ==> Row:20 Col:64 */
    {0x8109,0xF6C2}, /* ��: GB2312 Code: 0xC2F6 ==> Row:34 Col:86 */
    {0x810A,0xB9BC}, /* ��: GB2312 Code: 0xBCB9 ==> Row:28 Col:25 */
    {0x810D,0xDAEB}, /* ��: GB2312 Code: 0xEBDA ==> Row:75 Col:58 */
    {0x810E,0xDBEB}, /* ��: GB2312 Code: 0xEBDB ==> Row:75 Col:59 */
    {0x810F,0xE0D4}, /* ��: GB2312 Code: 0xD4E0 ==> Row:52 Col:64 */
    {0x8110,0xEAC6}, /* ��: GB2312 Code: 0xC6EA ==> Row:38 Col:74 */
    {0x8111,0xD4C4}, /* ��: GB2312 Code: 0xC4D4 ==> Row:36 Col:52 */
    {0x8112,0xDFEB}, /* ��: GB2312 Code: 0xEBDF ==> Row:75 Col:63 */
    {0x8113,0xA7C5}, /* ŧ: GB2312 Code: 0xC5A7 ==> Row:37 Col:07 */
    {0x8114,0xF5D9}, /* ��: GB2312 Code: 0xD9F5 ==> Row:57 Col:85 */
    {0x8116,0xB1B2}, /* ��: GB2312 Code: 0xB2B1 ==> Row:18 Col:17 */
    {0x8118,0xE4EB}, /* ��: GB2312 Code: 0xEBE4 ==> Row:75 Col:68 */
    {0x811A,0xC5BD}, /* ��: GB2312 Code: 0xBDC5 ==> Row:29 Col:37 */
    {0x811E,0xE2EB}, /* ��: GB2312 Code: 0xEBE2 ==> Row:75 Col:66 */
    {0x812C,0xE3EB}, /* ��: GB2312 Code: 0xEBE3 ==> Row:75 Col:67 */
    {0x812F,0xACB8}, /* ��: GB2312 Code: 0xB8AC ==> Row:24 Col:12 */
    {0x8131,0xD1CD}, /* ��: GB2312 Code: 0xCDD1 ==> Row:45 Col:49 */
    {0x8132,0xE5EB}, /* ��: GB2312 Code: 0xEBE5 ==> Row:75 Col:69 */
    {0x8136,0xE1EB}, /* ��: GB2312 Code: 0xEBE1 ==> Row:75 Col:65 */
    {0x8138,0xB3C1}, /* ��: GB2312 Code: 0xC1B3 ==> Row:33 Col:19 */
    {0x813E,0xA2C6}, /* Ƣ: GB2312 Code: 0xC6A2 ==> Row:38 Col:02 */
    {0x8146,0xF3CC}, /* ��: GB2312 Code: 0xCCF3 ==> Row:44 Col:83 */
    {0x8148,0xE6EB}, /* ��: GB2312 Code: 0xEBE6 ==> Row:75 Col:70 */
    {0x814A,0xB0C0}, /* ��: GB2312 Code: 0xC0B0 ==> Row:32 Col:16 */
    {0x814B,0xB8D2}, /* Ҹ: GB2312 Code: 0xD2B8 ==> Row:50 Col:24 */
    {0x814C,0xE7EB}, /* ��: GB2312 Code: 0xEBE7 ==> Row:75 Col:71 */
    {0x8150,0xAFB8}, /* ��: GB2312 Code: 0xB8AF ==> Row:24 Col:15 */
    {0x8151,0xADB8}, /* ��: GB2312 Code: 0xB8AD ==> Row:24 Col:13 */
    {0x8153,0xE8EB}, /* ��: GB2312 Code: 0xEBE8 ==> Row:75 Col:72 */
    {0x8154,0xBBC7}, /* ǻ: GB2312 Code: 0xC7BB ==> Row:39 Col:27 */
    {0x8155,0xF3CD}, /* ��: GB2312 Code: 0xCDF3 ==> Row:45 Col:83 */
    {0x8159,0xEAEB}, /* ��: GB2312 Code: 0xEBEA ==> Row:75 Col:74 */
    {0x815A,0xEBEB}, /* ��: GB2312 Code: 0xEBEB ==> Row:75 Col:75 */
    {0x8160,0xEDEB}, /* ��: GB2312 Code: 0xEBED ==> Row:75 Col:77 */
    {0x8165,0xC8D0}, /* ��: GB2312 Code: 0xD0C8 ==> Row:48 Col:40 */
    {0x8167,0xF2EB}, /* ��: GB2312 Code: 0xEBF2 ==> Row:75 Col:82 */
    {0x8169,0xEEEB}, /* ��: GB2312 Code: 0xEBEE ==> Row:75 Col:78 */
    {0x816D,0xF1EB}, /* ��: GB2312 Code: 0xEBF1 ==> Row:75 Col:81 */
    {0x816E,0xF9C8}, /* ��: GB2312 Code: 0xC8F9 ==> Row:40 Col:89 */
    {0x8170,0xFCD1}, /* ��: GB2312 Code: 0xD1FC ==> Row:49 Col:92 */
    {0x8171,0xECEB}, /* ��: GB2312 Code: 0xEBEC ==> Row:75 Col:76 */
    {0x8174,0xE9EB}, /* ��: GB2312 Code: 0xEBE9 ==> Row:75 Col:73 */
    {0x8179,0xB9B8}, /* ��: GB2312 Code: 0xB8B9 ==> Row:24 Col:25 */
    {0x817A,0xD9CF}, /* ��: GB2312 Code: 0xCFD9 ==> Row:47 Col:57 */
    {0x817B,0xE5C4}, /* ��: GB2312 Code: 0xC4E5 ==> Row:36 Col:69 */
    {0x817C,0xEFEB}, /* ��: GB2312 Code: 0xEBEF ==> Row:75 Col:79 */
    {0x817D,0xF0EB}, /* ��: GB2312 Code: 0xEBF0 ==> Row:75 Col:80 */
    {0x817E,0xDACC}, /* ��: GB2312 Code: 0xCCDA ==> Row:44 Col:58 */
    {0x817F,0xC8CD}, /* ��: GB2312 Code: 0xCDC8 ==> Row:45 Col:40 */
    {0x8180,0xF2B0}, /* ��: GB2312 Code: 0xB0F2 ==> Row:16 Col:82 */
    {0x8182,0xF6EB}, /* ��: GB2312 Code: 0xEBF6 ==> Row:75 Col:86 */
    {0x8188,0xF5EB}, /* ��: GB2312 Code: 0xEBF5 ==> Row:75 Col:85 */
    {0x818A,0xB2B2}, /* ��: GB2312 Code: 0xB2B2 ==> Row:18 Col:18 */
    {0x818F,0xE0B8}, /* ��: GB2312 Code: 0xB8E0 ==> Row:24 Col:64 */
    {0x8191,0xF7EB}, /* ��: GB2312 Code: 0xEBF7 ==> Row:75 Col:87 */
    {0x8198,0xECB1}, /* ��: GB2312 Code: 0xB1EC ==> Row:17 Col:76 */
    {0x819B,0xC5CC}, /* ��: GB2312 Code: 0xCCC5 ==> Row:44 Col:37 */
    {0x819C,0xA4C4}, /* Ĥ: GB2312 Code: 0xC4A4 ==> Row:36 Col:04 */
    {0x819D,0xA5CF}, /* ϥ: GB2312 Code: 0xCFA5 ==> Row:47 Col:05 */
    {0x81A3,0xF9EB}, /* ��: GB2312 Code: 0xEBF9 ==> Row:75 Col:89 */
    {0x81A6,0xA2EC}, /* �: GB2312 Code: 0xECA2 ==> Row:76 Col:02 */
    {0x81A8,0xF2C5}, /* ��: GB2312 Code: 0xC5F2 ==> Row:37 Col:82 */
    {0x81AA,0xFAEB}, /* ��: GB2312 Code: 0xEBFA ==> Row:75 Col:90 */
    {0x81B3,0xC5C9}, /* ��: GB2312 Code: 0xC9C5 ==> Row:41 Col:37 */
    {0x81BA,0xDFE2}, /* ��: GB2312 Code: 0xE2DF ==> Row:66 Col:63 */
    {0x81BB,0xFEEB}, /* ��: GB2312 Code: 0xEBFE ==> Row:75 Col:94 */
    {0x81C0,0xCECD}, /* ��: GB2312 Code: 0xCDCE ==> Row:45 Col:46 */
    {0x81C1,0xA1EC}, /* �: GB2312 Code: 0xECA1 ==> Row:76 Col:01 */
    {0x81C2,0xDBB1}, /* ��: GB2312 Code: 0xB1DB ==> Row:17 Col:59 */
    {0x81C3,0xB7D3}, /* ӷ: GB2312 Code: 0xD3B7 ==> Row:51 Col:23 */
    {0x81C6,0xDCD2}, /* ��: GB2312 Code: 0xD2DC ==> Row:50 Col:60 */
    {0x81CA,0xFDEB}, /* ��: GB2312 Code: 0xEBFD ==> Row:75 Col:93 */
    {0x81CC,0xFBEB}, /* ��: GB2312 Code: 0xEBFB ==> Row:75 Col:91 */
    {0x81E3,0xBCB3}, /* ��: GB2312 Code: 0xB3BC ==> Row:19 Col:28 */
    {0x81E7,0xB0EA}, /* �: GB2312 Code: 0xEAB0 ==> Row:74 Col:16 */
    {0x81EA,0xD4D7}, /* ��: GB2312 Code: 0xD7D4 ==> Row:55 Col:52 */
    {0x81EC,0xABF4}, /* ��: GB2312 Code: 0xF4AB ==> Row:84 Col:11 */
    {0x81ED,0xF4B3}, /* ��: GB2312 Code: 0xB3F4 ==> Row:19 Col:84 */
    {0x81F3,0xC1D6}, /* ��: GB2312 Code: 0xD6C1 ==> Row:54 Col:33 */
    {0x81F4,0xC2D6}, /* ��: GB2312 Code: 0xD6C2 ==> Row:54 Col:34 */
    {0x81FB,0xE9D5}, /* ��: GB2312 Code: 0xD5E9 ==> Row:53 Col:73 */
    {0x81FC,0xCABE}, /* ��: GB2312 Code: 0xBECA ==> Row:30 Col:42 */
    {0x81FE,0xA7F4}, /* ��: GB2312 Code: 0xF4A7 ==> Row:84 Col:07 */
    {0x8200,0xA8D2}, /* Ҩ: GB2312 Code: 0xD2A8 ==> Row:50 Col:08 */
    {0x8201,0xA8F4}, /* ��: GB2312 Code: 0xF4A8 ==> Row:84 Col:08 */
    {0x8202,0xA9F4}, /* ��: GB2312 Code: 0xF4A9 ==> Row:84 Col:09 */
    {0x8204,0xAAF4}, /* ��: GB2312 Code: 0xF4AA ==> Row:84 Col:10 */
    {0x8205,0xCBBE}, /* ��: GB2312 Code: 0xBECB ==> Row:30 Col:43 */
    {0x8206,0xDFD3}, /* ��: GB2312 Code: 0xD3DF ==> Row:51 Col:63 */
    {0x820C,0xE0C9}, /* ��: GB2312 Code: 0xC9E0 ==> Row:41 Col:64 */
    {0x820D,0xE1C9}, /* ��: GB2312 Code: 0xC9E1 ==> Row:41 Col:65 */
    {0x8210,0xC2F3}, /* ��: GB2312 Code: 0xF3C2 ==> Row:83 Col:34 */
    {0x8212,0xE6CA}, /* ��: GB2312 Code: 0xCAE6 ==> Row:42 Col:70 */
    {0x8214,0xF2CC}, /* ��: GB2312 Code: 0xCCF2 ==> Row:44 Col:82 */
    {0x821B,0xB6E2}, /* �: GB2312 Code: 0xE2B6 ==> Row:66 Col:22 */
    {0x821C,0xB4CB}, /* ˴: GB2312 Code: 0xCBB4 ==> Row:43 Col:20 */
    {0x821E,0xE8CE}, /* ��: GB2312 Code: 0xCEE8 ==> Row:46 Col:72 */
    {0x821F,0xDBD6}, /* ��: GB2312 Code: 0xD6DB ==> Row:54 Col:59 */
    {0x8221,0xADF4}, /* ��: GB2312 Code: 0xF4AD ==> Row:84 Col:13 */
    {0x8222,0xAEF4}, /* ��: GB2312 Code: 0xF4AE ==> Row:84 Col:14 */
    {0x8223,0xAFF4}, /* ��: GB2312 Code: 0xF4AF ==> Row:84 Col:15 */
    {0x8228,0xB2F4}, /* ��: GB2312 Code: 0xF4B2 ==> Row:84 Col:18 */
    {0x822A,0xBDBA}, /* ��: GB2312 Code: 0xBABD ==> Row:26 Col:29 */
    {0x822B,0xB3F4}, /* ��: GB2312 Code: 0xF4B3 ==> Row:84 Col:19 */
    {0x822C,0xE3B0}, /* ��: GB2312 Code: 0xB0E3 ==> Row:16 Col:67 */
    {0x822D,0xB0F4}, /* ��: GB2312 Code: 0xF4B0 ==> Row:84 Col:16 */
    {0x822F,0xB1F4}, /* ��: GB2312 Code: 0xF4B1 ==> Row:84 Col:17 */
    {0x8230,0xA2BD}, /* ��: GB2312 Code: 0xBDA2 ==> Row:29 Col:02 */
    {0x8231,0xD5B2}, /* ��: GB2312 Code: 0xB2D5 ==> Row:18 Col:53 */
    {0x8233,0xB6F4}, /* ��: GB2312 Code: 0xF4B6 ==> Row:84 Col:22 */
    {0x8234,0xB7F4}, /* ��: GB2312 Code: 0xF4B7 ==> Row:84 Col:23 */
    {0x8235,0xE6B6}, /* ��: GB2312 Code: 0xB6E6 ==> Row:22 Col:70 */
    {0x8236,0xB0B2}, /* ��: GB2312 Code: 0xB2B0 ==> Row:18 Col:16 */
    {0x8237,0xCFCF}, /* ��: GB2312 Code: 0xCFCF ==> Row:47 Col:47 */
    {0x8238,0xB4F4}, /* ��: GB2312 Code: 0xF4B4 ==> Row:84 Col:20 */
    {0x8239,0xACB4}, /* ��: GB2312 Code: 0xB4AC ==> Row:20 Col:12 */
    {0x823B,0xB5F4}, /* ��: GB2312 Code: 0xF4B5 ==> Row:84 Col:21 */
    {0x823E,0xB8F4}, /* ��: GB2312 Code: 0xF4B8 ==> Row:84 Col:24 */
    {0x8244,0xB9F4}, /* ��: GB2312 Code: 0xF4B9 ==> Row:84 Col:25 */
    {0x8247,0xA7CD}, /* ͧ: GB2312 Code: 0xCDA7 ==> Row:45 Col:07 */
    {0x8249,0xBAF4}, /* ��: GB2312 Code: 0xF4BA ==> Row:84 Col:26 */
    {0x824B,0xBBF4}, /* ��: GB2312 Code: 0xF4BB ==> Row:84 Col:27 */
    {0x824F,0xBCF4}, /* ��: GB2312 Code: 0xF4BC ==> Row:84 Col:28 */
    {0x8258,0xD2CB}, /* ��: GB2312 Code: 0xCBD2 ==> Row:43 Col:50 */
    {0x825A,0xBDF4}, /* ��: GB2312 Code: 0xF4BD ==> Row:84 Col:29 */
    {0x825F,0xBEF4}, /* ��: GB2312 Code: 0xF4BE ==> Row:84 Col:30 */
    {0x8268,0xBFF4}, /* ��: GB2312 Code: 0xF4BF ==> Row:84 Col:31 */
    {0x826E,0xDEF4}, /* ��: GB2312 Code: 0xF4DE ==> Row:84 Col:62 */
    {0x826F,0xBCC1}, /* ��: GB2312 Code: 0xC1BC ==> Row:33 Col:28 */
    {0x8270,0xE8BC}, /* ��: GB2312 Code: 0xBCE8 ==> Row:28 Col:72 */
    {0x8272,0xABC9}, /* ɫ: GB2312 Code: 0xC9AB ==> Row:41 Col:11 */
    {0x8273,0xDED1}, /* ��: GB2312 Code: 0xD1DE ==> Row:49 Col:62 */
    {0x8274,0xF5E5}, /* ��: GB2312 Code: 0xE5F5 ==> Row:69 Col:85 */
    {0x8279,0xB3DC}, /* ܳ: GB2312 Code: 0xDCB3 ==> Row:60 Col:19 */
    {0x827A,0xD5D2}, /* ��: GB2312 Code: 0xD2D5 ==> Row:50 Col:53 */
    {0x827D,0xB4DC}, /* ܴ: GB2312 Code: 0xDCB4 ==> Row:60 Col:20 */
    {0x827E,0xACB0}, /* ��: GB2312 Code: 0xB0AC ==> Row:16 Col:12 */
    {0x827F,0xB5DC}, /* ܵ: GB2312 Code: 0xDCB5 ==> Row:60 Col:21 */
    {0x8282,0xDABD}, /* ��: GB2312 Code: 0xBDDA ==> Row:29 Col:58 */
    {0x8284,0xB9DC}, /* ܹ: GB2312 Code: 0xDCB9 ==> Row:60 Col:25 */
    {0x8288,0xC2D8}, /* ��: GB2312 Code: 0xD8C2 ==> Row:56 Col:34 */
    {0x828A,0xB7DC}, /* ܷ: GB2312 Code: 0xDCB7 ==> Row:60 Col:23 */
    {0x828B,0xF3D3}, /* ��: GB2312 Code: 0xD3F3 ==> Row:51 Col:83 */
    {0x828D,0xD6C9}, /* ��: GB2312 Code: 0xC9D6 ==> Row:41 Col:54 */
    {0x828E,0xBADC}, /* ܺ: GB2312 Code: 0xDCBA ==> Row:60 Col:26 */
    {0x828F,0xB6DC}, /* ܶ: GB2312 Code: 0xDCB6 ==> Row:60 Col:22 */
    {0x8291,0xBBDC}, /* ܻ: GB2312 Code: 0xDCBB ==> Row:60 Col:27 */
    {0x8292,0xA2C3}, /* â: GB2312 Code: 0xC3A2 ==> Row:35 Col:02 */
    {0x8297,0xBCDC}, /* ܼ: GB2312 Code: 0xDCBC ==> Row:60 Col:28 */
    {0x8298,0xC5DC}, /* ��: GB2312 Code: 0xDCC5 ==> Row:60 Col:37 */
    {0x8299,0xBDDC}, /* ܽ: GB2312 Code: 0xDCBD ==> Row:60 Col:29 */
    {0x829C,0xDFCE}, /* ��: GB2312 Code: 0xCEDF ==> Row:46 Col:63 */
    {0x829D,0xA5D6}, /* ֥: GB2312 Code: 0xD6A5 ==> Row:54 Col:05 */
    {0x829F,0xCFDC}, /* ��: GB2312 Code: 0xDCCF ==> Row:60 Col:47 */
    {0x82A1,0xCDDC}, /* ��: GB2312 Code: 0xDCCD ==> Row:60 Col:45 */
    {0x82A4,0xD2DC}, /* ��: GB2312 Code: 0xDCD2 ==> Row:60 Col:50 */
    {0x82A5,0xE6BD}, /* ��: GB2312 Code: 0xBDE6 ==> Row:29 Col:70 */
    {0x82A6,0xABC2}, /* «: GB2312 Code: 0xC2AB ==> Row:34 Col:11 */
    {0x82A8,0xB8DC}, /* ܸ: GB2312 Code: 0xDCB8 ==> Row:60 Col:24 */
    {0x82A9,0xCBDC}, /* ��: GB2312 Code: 0xDCCB ==> Row:60 Col:43 */
    {0x82AA,0xCEDC}, /* ��: GB2312 Code: 0xDCCE ==> Row:60 Col:46 */
    {0x82AB,0xBEDC}, /* ܾ: GB2312 Code: 0xDCBE ==> Row:60 Col:30 */
    {0x82AC,0xD2B7}, /* ��: GB2312 Code: 0xB7D2 ==> Row:23 Col:50 */
    {0x82AD,0xC5B0}, /* ��: GB2312 Code: 0xB0C5 ==> Row:16 Col:37 */
    {0x82AE,0xC7DC}, /* ��: GB2312 Code: 0xDCC7 ==> Row:60 Col:39 */
    {0x82AF,0xBED0}, /* о: GB2312 Code: 0xD0BE ==> Row:48 Col:30 */
    {0x82B0,0xC1DC}, /* ��: GB2312 Code: 0xDCC1 ==> Row:60 Col:33 */
    {0x82B1,0xA8BB}, /* ��: GB2312 Code: 0xBBA8 ==> Row:27 Col:08 */
    {0x82B3,0xBCB7}, /* ��: GB2312 Code: 0xB7BC ==> Row:23 Col:28 */
    {0x82B4,0xCCDC}, /* ��: GB2312 Code: 0xDCCC ==> Row:60 Col:44 */
    {0x82B7,0xC6DC}, /* ��: GB2312 Code: 0xDCC6 ==> Row:60 Col:38 */
    {0x82B8,0xBFDC}, /* ܿ: GB2312 Code: 0xDCBF ==> Row:60 Col:31 */
    {0x82B9,0xDBC7}, /* ��: GB2312 Code: 0xC7DB ==> Row:39 Col:59 */
    {0x82BD,0xBFD1}, /* ѿ: GB2312 Code: 0xD1BF ==> Row:49 Col:31 */
    {0x82BE,0xC0DC}, /* ��: GB2312 Code: 0xDCC0 ==> Row:60 Col:32 */
    {0x82C1,0xCADC}, /* ��: GB2312 Code: 0xDCCA ==> Row:60 Col:42 */
    {0x82C4,0xD0DC}, /* ��: GB2312 Code: 0xDCD0 ==> Row:60 Col:48 */
    {0x82C7,0xADCE}, /* έ: GB2312 Code: 0xCEAD ==> Row:46 Col:13 */
    {0x82C8,0xC2DC}, /* ��: GB2312 Code: 0xDCC2 ==> Row:60 Col:34 */
    {0x82CA,0xC3DC}, /* ��: GB2312 Code: 0xDCC3 ==> Row:60 Col:35 */
    {0x82CB,0xC8DC}, /* ��: GB2312 Code: 0xDCC8 ==> Row:60 Col:40 */
    {0x82CC,0xC9DC}, /* ��: GB2312 Code: 0xDCC9 ==> Row:60 Col:41 */
    {0x82CD,0xD4B2}, /* ��: GB2312 Code: 0xB2D4 ==> Row:18 Col:52 */
    {0x82CE,0xD1DC}, /* ��: GB2312 Code: 0xDCD1 ==> Row:60 Col:49 */
    {0x82CF,0xD5CB}, /* ��: GB2312 Code: 0xCBD5 ==> Row:43 Col:53 */
    {0x82D1,0xB7D4}, /* Է: GB2312 Code: 0xD4B7 ==> Row:52 Col:23 */
    {0x82D2,0xDBDC}, /* ��: GB2312 Code: 0xDCDB ==> Row:60 Col:59 */
    {0x82D3,0xDFDC}, /* ��: GB2312 Code: 0xDCDF ==> Row:60 Col:63 */
    {0x82D4,0xA6CC}, /* ̦: GB2312 Code: 0xCCA6 ==> Row:44 Col:06 */
    {0x82D5,0xE6DC}, /* ��: GB2312 Code: 0xDCE6 ==> Row:60 Col:70 */
    {0x82D7,0xE7C3}, /* ��: GB2312 Code: 0xC3E7 ==> Row:35 Col:71 */
    {0x82D8,0xDCDC}, /* ��: GB2312 Code: 0xDCDC ==> Row:60 Col:60 */
    {0x82DB,0xC1BF}, /* ��: GB2312 Code: 0xBFC1 ==> Row:31 Col:33 */
    {0x82DC,0xD9DC}, /* ��: GB2312 Code: 0xDCD9 ==> Row:60 Col:57 */
    {0x82DE,0xFAB0}, /* ��: GB2312 Code: 0xB0FA ==> Row:16 Col:90 */
    {0x82DF,0xB6B9}, /* ��: GB2312 Code: 0xB9B6 ==> Row:25 Col:22 */
    {0x82E0,0xE5DC}, /* ��: GB2312 Code: 0xDCE5 ==> Row:60 Col:69 */
    {0x82E1,0xD3DC}, /* ��: GB2312 Code: 0xDCD3 ==> Row:60 Col:51 */
    {0x82E3,0xC4DC}, /* ��: GB2312 Code: 0xDCC4 ==> Row:60 Col:36 */
    {0x82E4,0xD6DC}, /* ��: GB2312 Code: 0xDCD6 ==> Row:60 Col:54 */
    {0x82E5,0xF4C8}, /* ��: GB2312 Code: 0xC8F4 ==> Row:40 Col:84 */
    {0x82E6,0xE0BF}, /* ��: GB2312 Code: 0xBFE0 ==> Row:31 Col:64 */
    {0x82EB,0xBBC9}, /* ɻ: GB2312 Code: 0xC9BB ==> Row:41 Col:27 */
    {0x82EF,0xBDB1}, /* ��: GB2312 Code: 0xB1BD ==> Row:17 Col:29 */
    {0x82F1,0xA2D3}, /* Ӣ: GB2312 Code: 0xD3A2 ==> Row:51 Col:02 */
    {0x82F4,0xDADC}, /* ��: GB2312 Code: 0xDCDA ==> Row:60 Col:58 */
    {0x82F7,0xD5DC}, /* ��: GB2312 Code: 0xDCD5 ==> Row:60 Col:53 */
    {0x82F9,0xBBC6}, /* ƻ: GB2312 Code: 0xC6BB ==> Row:38 Col:27 */
    {0x82FB,0xDEDC}, /* ��: GB2312 Code: 0xDCDE ==> Row:60 Col:62 */
    {0x8301,0xC2D7}, /* ��: GB2312 Code: 0xD7C2 ==> Row:55 Col:34 */
    {0x8302,0xAFC3}, /* ï: GB2312 Code: 0xC3AF ==> Row:35 Col:15 */
    {0x8303,0xB6B7}, /* ��: GB2312 Code: 0xB7B6 ==> Row:23 Col:22 */
    {0x8304,0xD1C7}, /* ��: GB2312 Code: 0xC7D1 ==> Row:39 Col:49 */
    {0x8305,0xA9C3}, /* é: GB2312 Code: 0xC3A9 ==> Row:35 Col:09 */
    {0x8306,0xE2DC}, /* ��: GB2312 Code: 0xDCE2 ==> Row:60 Col:66 */
    {0x8307,0xD8DC}, /* ��: GB2312 Code: 0xDCD8 ==> Row:60 Col:56 */
    {0x8308,0xEBDC}, /* ��: GB2312 Code: 0xDCEB ==> Row:60 Col:75 */
    {0x8309,0xD4DC}, /* ��: GB2312 Code: 0xDCD4 ==> Row:60 Col:52 */
    {0x830C,0xDDDC}, /* ��: GB2312 Code: 0xDCDD ==> Row:60 Col:61 */
    {0x830E,0xA5BE}, /* ��: GB2312 Code: 0xBEA5 ==> Row:30 Col:05 */
    {0x830F,0xD7DC}, /* ��: GB2312 Code: 0xDCD7 ==> Row:60 Col:55 */
    {0x8311,0xE0DC}, /* ��: GB2312 Code: 0xDCE0 ==> Row:60 Col:64 */
    {0x8314,0xE3DC}, /* ��: GB2312 Code: 0xDCE3 ==> Row:60 Col:67 */
    {0x8315,0xE4DC}, /* ��: GB2312 Code: 0xDCE4 ==> Row:60 Col:68 */
    {0x8317,0xF8DC}, /* ��: GB2312 Code: 0xDCF8 ==> Row:60 Col:88 */
    {0x831A,0xE1DC}, /* ��: GB2312 Code: 0xDCE1 ==> Row:60 Col:65 */
    {0x831B,0xA2DD}, /* ݢ: GB2312 Code: 0xDDA2 ==> Row:61 Col:02 */
    {0x831C,0xE7DC}, /* ��: GB2312 Code: 0xDCE7 ==> Row:60 Col:71 */
    {0x8327,0xEBBC}, /* ��: GB2312 Code: 0xBCEB ==> Row:28 Col:75 */
    {0x8328,0xC4B4}, /* ��: GB2312 Code: 0xB4C4 ==> Row:20 Col:36 */
    {0x832B,0xA3C3}, /* ã: GB2312 Code: 0xC3A3 ==> Row:35 Col:03 */
    {0x832C,0xE7B2}, /* ��: GB2312 Code: 0xB2E7 ==> Row:18 Col:71 */
    {0x832D,0xFADC}, /* ��: GB2312 Code: 0xDCFA ==> Row:60 Col:90 */
    {0x832F,0xF2DC}, /* ��: GB2312 Code: 0xDCF2 ==> Row:60 Col:82 */
    {0x8331,0xEFDC}, /* ��: GB2312 Code: 0xDCEF ==> Row:60 Col:79 */
    {0x8333,0xFCDC}, /* ��: GB2312 Code: 0xDCFC ==> Row:60 Col:92 */
    {0x8334,0xEEDC}, /* ��: GB2312 Code: 0xDCEE ==> Row:60 Col:78 */
    {0x8335,0xF0D2}, /* ��: GB2312 Code: 0xD2F0 ==> Row:50 Col:80 */
    {0x8336,0xE8B2}, /* ��: GB2312 Code: 0xB2E8 ==> Row:18 Col:72 */
    {0x8338,0xD7C8}, /* ��: GB2312 Code: 0xC8D7 ==> Row:40 Col:55 */
    {0x8339,0xE3C8}, /* ��: GB2312 Code: 0xC8E3 ==> Row:40 Col:67 */
    {0x833A,0xFBDC}, /* ��: GB2312 Code: 0xDCFB ==> Row:60 Col:91 */
    {0x833C,0xEDDC}, /* ��: GB2312 Code: 0xDCED ==> Row:60 Col:77 */
    {0x8340,0xF7DC}, /* ��: GB2312 Code: 0xDCF7 ==> Row:60 Col:87 */
    {0x8343,0xF5DC}, /* ��: GB2312 Code: 0xDCF5 ==> Row:60 Col:85 */
    {0x8346,0xA3BE}, /* ��: GB2312 Code: 0xBEA3 ==> Row:30 Col:03 */
    {0x8347,0xF4DC}, /* ��: GB2312 Code: 0xDCF4 ==> Row:60 Col:84 */
    {0x8349,0xDDB2}, /* ��: GB2312 Code: 0xB2DD ==> Row:18 Col:61 */
    {0x834F,0xF3DC}, /* ��: GB2312 Code: 0xDCF3 ==> Row:60 Col:83 */
    {0x8350,0xF6BC}, /* ��: GB2312 Code: 0xBCF6 ==> Row:28 Col:86 */
    {0x8351,0xE8DC}, /* ��: GB2312 Code: 0xDCE8 ==> Row:60 Col:72 */
    {0x8352,0xC4BB}, /* ��: GB2312 Code: 0xBBC4 ==> Row:27 Col:36 */
    {0x8354,0xF3C0}, /* ��: GB2312 Code: 0xC0F3 ==> Row:32 Col:83 */
    {0x835A,0xD4BC}, /* ��: GB2312 Code: 0xBCD4 ==> Row:28 Col:52 */
    {0x835B,0xE9DC}, /* ��: GB2312 Code: 0xDCE9 ==> Row:60 Col:73 */
    {0x835C,0xEADC}, /* ��: GB2312 Code: 0xDCEA ==> Row:60 Col:74 */
    {0x835E,0xF1DC}, /* ��: GB2312 Code: 0xDCF1 ==> Row:60 Col:81 */
    {0x835F,0xF6DC}, /* ��: GB2312 Code: 0xDCF6 ==> Row:60 Col:86 */
    {0x8360,0xF9DC}, /* ��: GB2312 Code: 0xDCF9 ==> Row:60 Col:89 */
    {0x8361,0xB4B5}, /* ��: GB2312 Code: 0xB5B4 ==> Row:21 Col:20 */
    {0x8363,0xD9C8}, /* ��: GB2312 Code: 0xC8D9 ==> Row:40 Col:57 */
    {0x8364,0xE7BB}, /* ��: GB2312 Code: 0xBBE7 ==> Row:27 Col:71 */
    {0x8365,0xFEDC}, /* ��: GB2312 Code: 0xDCFE ==> Row:60 Col:94 */
    {0x8366,0xFDDC}, /* ��: GB2312 Code: 0xDCFD ==> Row:60 Col:93 */
    {0x8367,0xABD3}, /* ӫ: GB2312 Code: 0xD3AB ==> Row:51 Col:11 */
    {0x8368,0xA1DD}, /* ݡ: GB2312 Code: 0xDDA1 ==> Row:61 Col:01 */
    {0x8369,0xA3DD}, /* ݣ: GB2312 Code: 0xDDA3 ==> Row:61 Col:03 */
    {0x836A,0xA5DD}, /* ݥ: GB2312 Code: 0xDDA5 ==> Row:61 Col:05 */
    {0x836B,0xF1D2}, /* ��: GB2312 Code: 0xD2F1 ==> Row:50 Col:81 */
    {0x836C,0xA4DD}, /* ݤ: GB2312 Code: 0xDDA4 ==> Row:61 Col:04 */
    {0x836D,0xA6DD}, /* ݦ: GB2312 Code: 0xDDA6 ==> Row:61 Col:06 */
    {0x836E,0xA7DD}, /* ݧ: GB2312 Code: 0xDDA7 ==> Row:61 Col:07 */
    {0x836F,0xA9D2}, /* ҩ: GB2312 Code: 0xD2A9 ==> Row:50 Col:09 */
    {0x8377,0xC9BA}, /* ��: GB2312 Code: 0xBAC9 ==> Row:26 Col:41 */
    {0x8378,0xA9DD}, /* ݩ: GB2312 Code: 0xDDA9 ==> Row:61 Col:09 */
    {0x837B,0xB6DD}, /* ݶ: GB2312 Code: 0xDDB6 ==> Row:61 Col:22 */
    {0x837C,0xB1DD}, /* ݱ: GB2312 Code: 0xDDB1 ==> Row:61 Col:17 */
    {0x837D,0xB4DD}, /* ݴ: GB2312 Code: 0xDDB4 ==> Row:61 Col:20 */
    {0x8385,0xB0DD}, /* ݰ: GB2312 Code: 0xDDB0 ==> Row:61 Col:16 */
    {0x8386,0xCEC6}, /* ��: GB2312 Code: 0xC6CE ==> Row:38 Col:46 */
    {0x8389,0xF2C0}, /* ��: GB2312 Code: 0xC0F2 ==> Row:32 Col:82 */
    {0x838E,0xAFC9}, /* ɯ: GB2312 Code: 0xC9AF ==> Row:41 Col:15 */
    {0x8392,0xECDC}, /* ��: GB2312 Code: 0xDCEC ==> Row:60 Col:76 */
    {0x8393,0xAEDD}, /* ݮ: GB2312 Code: 0xDDAE ==> Row:61 Col:14 */
    {0x8398,0xB7DD}, /* ݷ: GB2312 Code: 0xDDB7 ==> Row:61 Col:23 */
    {0x839B,0xF0DC}, /* ��: GB2312 Code: 0xDCF0 ==> Row:60 Col:80 */
    {0x839C,0xAFDD}, /* ݯ: GB2312 Code: 0xDDAF ==> Row:61 Col:15 */
    {0x839E,0xB8DD}, /* ݸ: GB2312 Code: 0xDDB8 ==> Row:61 Col:24 */
    {0x83A0,0xACDD}, /* ݬ: GB2312 Code: 0xDDAC ==> Row:61 Col:12 */
    {0x83A8,0xB9DD}, /* ݹ: GB2312 Code: 0xDDB9 ==> Row:61 Col:25 */
    {0x83A9,0xB3DD}, /* ݳ: GB2312 Code: 0xDDB3 ==> Row:61 Col:19 */
    {0x83AA,0xADDD}, /* ݭ: GB2312 Code: 0xDDAD ==> Row:61 Col:13 */
    {0x83AB,0xAAC4}, /* Ī: GB2312 Code: 0xC4AA ==> Row:36 Col:10 */
    {0x83B0,0xA8DD}, /* ݨ: GB2312 Code: 0xDDA8 ==> Row:61 Col:08 */
    {0x83B1,0xB3C0}, /* ��: GB2312 Code: 0xC0B3 ==> Row:32 Col:19 */
    {0x83B2,0xABC1}, /* ��: GB2312 Code: 0xC1AB ==> Row:33 Col:11 */
    {0x83B3,0xAADD}, /* ݪ: GB2312 Code: 0xDDAA ==> Row:61 Col:10 */
    {0x83B4,0xABDD}, /* ݫ: GB2312 Code: 0xDDAB ==> Row:61 Col:11 */
    {0x83B6,0xB2DD}, /* ݲ: GB2312 Code: 0xDDB2 ==> Row:61 Col:18 */
    {0x83B7,0xF1BB}, /* ��: GB2312 Code: 0xBBF1 ==> Row:27 Col:81 */
    {0x83B8,0xB5DD}, /* ݵ: GB2312 Code: 0xDDB5 ==> Row:61 Col:21 */
    {0x83B9,0xA8D3}, /* Ө: GB2312 Code: 0xD3A8 ==> Row:51 Col:08 */
    {0x83BA,0xBADD}, /* ݺ: GB2312 Code: 0xDDBA ==> Row:61 Col:26 */
    {0x83BC,0xBBDD}, /* ݻ: GB2312 Code: 0xDDBB ==> Row:61 Col:27 */
    {0x83BD,0xA7C3}, /* ç: GB2312 Code: 0xC3A7 ==> Row:35 Col:07 */
    {0x83C0,0xD2DD}, /* ��: GB2312 Code: 0xDDD2 ==> Row:61 Col:50 */
    {0x83C1,0xBCDD}, /* ݼ: GB2312 Code: 0xDDBC ==> Row:61 Col:28 */
    {0x83C5,0xD1DD}, /* ��: GB2312 Code: 0xDDD1 ==> Row:61 Col:49 */
    {0x83C7,0xBDB9}, /* ��: GB2312 Code: 0xB9BD ==> Row:25 Col:29 */
    {0x83CA,0xD5BE}, /* ��: GB2312 Code: 0xBED5 ==> Row:30 Col:53 */
    {0x83CC,0xFABE}, /* ��: GB2312 Code: 0xBEFA ==> Row:30 Col:90 */
    {0x83CF,0xCABA}, /* ��: GB2312 Code: 0xBACA ==> Row:26 Col:42 */
    {0x83D4,0xCADD}, /* ��: GB2312 Code: 0xDDCA ==> Row:61 Col:42 */
    {0x83D6,0xC5DD}, /* ��: GB2312 Code: 0xDDC5 ==> Row:61 Col:37 */
    {0x83D8,0xBFDD}, /* ݿ: GB2312 Code: 0xDDBF ==> Row:61 Col:31 */
    {0x83DC,0xCBB2}, /* ��: GB2312 Code: 0xB2CB ==> Row:18 Col:43 */
    {0x83DD,0xC3DD}, /* ��: GB2312 Code: 0xDDC3 ==> Row:61 Col:35 */
    {0x83DF,0xCBDD}, /* ��: GB2312 Code: 0xDDCB ==> Row:61 Col:43 */
    {0x83E0,0xA4B2}, /* ��: GB2312 Code: 0xB2A4 ==> Row:18 Col:04 */
    {0x83E1,0xD5DD}, /* ��: GB2312 Code: 0xDDD5 ==> Row:61 Col:53 */
    {0x83E5,0xBEDD}, /* ݾ: GB2312 Code: 0xDDBE ==> Row:61 Col:30 */
    {0x83E9,0xD0C6}, /* ��: GB2312 Code: 0xC6D0 ==> Row:38 Col:48 */
    {0x83EA,0xD0DD}, /* ��: GB2312 Code: 0xDDD0 ==> Row:61 Col:48 */
    {0x83F0,0xD4DD}, /* ��: GB2312 Code: 0xDDD4 ==> Row:61 Col:52 */
    {0x83F1,0xE2C1}, /* ��: GB2312 Code: 0xC1E2 ==> Row:33 Col:66 */
    {0x83F2,0xC6B7}, /* ��: GB2312 Code: 0xB7C6 ==> Row:23 Col:38 */
    {0x83F8,0xCEDD}, /* ��: GB2312 Code: 0xDDCE ==> Row:61 Col:46 */
    {0x83F9,0xCFDD}, /* ��: GB2312 Code: 0xDDCF ==> Row:61 Col:47 */
    {0x83FD,0xC4DD}, /* ��: GB2312 Code: 0xDDC4 ==> Row:61 Col:36 */
    {0x8401,0xBDDD}, /* ݽ: GB2312 Code: 0xDDBD ==> Row:61 Col:29 */
    {0x8403,0xCDDD}, /* ��: GB2312 Code: 0xDDCD ==> Row:61 Col:45 */
    {0x8404,0xD1CC}, /* ��: GB2312 Code: 0xCCD1 ==> Row:44 Col:49 */
    {0x8406,0xC9DD}, /* ��: GB2312 Code: 0xDDC9 ==> Row:61 Col:41 */
    {0x840B,0xC2DD}, /* ��: GB2312 Code: 0xDDC2 ==> Row:61 Col:34 */
    {0x840C,0xC8C3}, /* ��: GB2312 Code: 0xC3C8 ==> Row:35 Col:40 */
    {0x840D,0xBCC6}, /* Ƽ: GB2312 Code: 0xC6BC ==> Row:38 Col:28 */
    {0x840E,0xAECE}, /* ή: GB2312 Code: 0xCEAE ==> Row:46 Col:14 */
    {0x840F,0xCCDD}, /* ��: GB2312 Code: 0xDDCC ==> Row:61 Col:44 */
    {0x8411,0xC8DD}, /* ��: GB2312 Code: 0xDDC8 ==> Row:61 Col:40 */
    {0x8418,0xC1DD}, /* ��: GB2312 Code: 0xDDC1 ==> Row:61 Col:33 */
    {0x841C,0xC6DD}, /* ��: GB2312 Code: 0xDDC6 ==> Row:61 Col:38 */
    {0x841D,0xDCC2}, /* ��: GB2312 Code: 0xC2DC ==> Row:34 Col:60 */
    {0x8424,0xA9D3}, /* ө: GB2312 Code: 0xD3A9 ==> Row:51 Col:09 */
    {0x8425,0xAAD3}, /* Ӫ: GB2312 Code: 0xD3AA ==> Row:51 Col:10 */
    {0x8426,0xD3DD}, /* ��: GB2312 Code: 0xDDD3 ==> Row:61 Col:51 */
    {0x8427,0xF4CF}, /* ��: GB2312 Code: 0xCFF4 ==> Row:47 Col:84 */
    {0x8428,0xF8C8}, /* ��: GB2312 Code: 0xC8F8 ==> Row:40 Col:88 */
    {0x8431,0xE6DD}, /* ��: GB2312 Code: 0xDDE6 ==> Row:61 Col:70 */
    {0x8438,0xC7DD}, /* ��: GB2312 Code: 0xDDC7 ==> Row:61 Col:39 */
    {0x843C,0xE0DD}, /* ��: GB2312 Code: 0xDDE0 ==> Row:61 Col:64 */
    {0x843D,0xE4C2}, /* ��: GB2312 Code: 0xC2E4 ==> Row:34 Col:68 */
    {0x8446,0xE1DD}, /* ��: GB2312 Code: 0xDDE1 ==> Row:61 Col:65 */
    {0x8451,0xD7DD}, /* ��: GB2312 Code: 0xDDD7 ==> Row:61 Col:55 */
    {0x8457,0xF8D6}, /* ��: GB2312 Code: 0xD6F8 ==> Row:54 Col:88 */
    {0x8459,0xD9DD}, /* ��: GB2312 Code: 0xDDD9 ==> Row:61 Col:57 */
    {0x845A,0xD8DD}, /* ��: GB2312 Code: 0xDDD8 ==> Row:61 Col:56 */
    {0x845B,0xF0B8}, /* ��: GB2312 Code: 0xB8F0 ==> Row:24 Col:80 */
    {0x845C,0xD6DD}, /* ��: GB2312 Code: 0xDDD6 ==> Row:61 Col:54 */
    {0x8461,0xCFC6}, /* ��: GB2312 Code: 0xC6CF ==> Row:38 Col:47 */
    {0x8463,0xADB6}, /* ��: GB2312 Code: 0xB6AD ==> Row:22 Col:13 */
    {0x8469,0xE2DD}, /* ��: GB2312 Code: 0xDDE2 ==> Row:61 Col:66 */
    {0x846B,0xF9BA}, /* ��: GB2312 Code: 0xBAF9 ==> Row:26 Col:89 */
    {0x846C,0xE1D4}, /* ��: GB2312 Code: 0xD4E1 ==> Row:52 Col:65 */
    {0x846D,0xE7DD}, /* ��: GB2312 Code: 0xDDE7 ==> Row:61 Col:71 */
    {0x8471,0xD0B4}, /* ��: GB2312 Code: 0xB4D0 ==> Row:20 Col:48 */
    {0x8473,0xDADD}, /* ��: GB2312 Code: 0xDDDA ==> Row:61 Col:58 */
    {0x8475,0xFBBF}, /* ��: GB2312 Code: 0xBFFB ==> Row:31 Col:91 */
    {0x8476,0xE3DD}, /* ��: GB2312 Code: 0xDDE3 ==> Row:61 Col:67 */
    {0x8478,0xDFDD}, /* ��: GB2312 Code: 0xDDDF ==> Row:61 Col:63 */
    {0x847A,0xDDDD}, /* ��: GB2312 Code: 0xDDDD ==> Row:61 Col:61 */
    {0x8482,0xD9B5}, /* ��: GB2312 Code: 0xB5D9 ==> Row:21 Col:57 */
    {0x8487,0xDBDD}, /* ��: GB2312 Code: 0xDDDB ==> Row:61 Col:59 */
    {0x8488,0xDCDD}, /* ��: GB2312 Code: 0xDDDC ==> Row:61 Col:60 */
    {0x8489,0xDEDD}, /* ��: GB2312 Code: 0xDDDE ==> Row:61 Col:62 */
    {0x848B,0xAFBD}, /* ��: GB2312 Code: 0xBDAF ==> Row:29 Col:15 */
    {0x848C,0xE4DD}, /* ��: GB2312 Code: 0xDDE4 ==> Row:61 Col:68 */
    {0x848E,0xE5DD}, /* ��: GB2312 Code: 0xDDE5 ==> Row:61 Col:69 */
    {0x8497,0xF5DD}, /* ��: GB2312 Code: 0xDDF5 ==> Row:61 Col:85 */
    {0x8499,0xC9C3}, /* ��: GB2312 Code: 0xC3C9 ==> Row:35 Col:41 */
    {0x849C,0xE2CB}, /* ��: GB2312 Code: 0xCBE2 ==> Row:43 Col:66 */
    {0x84A1,0xF2DD}, /* ��: GB2312 Code: 0xDDF2 ==> Row:61 Col:82 */
    {0x84AF,0xE1D8}, /* ��: GB2312 Code: 0xD8E1 ==> Row:56 Col:65 */
    {0x84B2,0xD1C6}, /* ��: GB2312 Code: 0xC6D1 ==> Row:38 Col:49 */
    {0x84B4,0xF4DD}, /* ��: GB2312 Code: 0xDDF4 ==> Row:61 Col:84 */
    {0x84B8,0xF4D5}, /* ��: GB2312 Code: 0xD5F4 ==> Row:53 Col:84 */
    {0x84B9,0xF3DD}, /* ��: GB2312 Code: 0xDDF3 ==> Row:61 Col:83 */
    {0x84BA,0xF0DD}, /* ��: GB2312 Code: 0xDDF0 ==> Row:61 Col:80 */
    {0x84BD,0xECDD}, /* ��: GB2312 Code: 0xDDEC ==> Row:61 Col:76 */
    {0x84BF,0xEFDD}, /* ��: GB2312 Code: 0xDDEF ==> Row:61 Col:79 */
    {0x84C1,0xE8DD}, /* ��: GB2312 Code: 0xDDE8 ==> Row:61 Col:72 */
    {0x84C4,0xEED0}, /* ��: GB2312 Code: 0xD0EE ==> Row:48 Col:78 */
    {0x84C9,0xD8C8}, /* ��: GB2312 Code: 0xC8D8 ==> Row:40 Col:56 */
    {0x84CA,0xEEDD}, /* ��: GB2312 Code: 0xDDEE ==> Row:61 Col:78 */
    {0x84CD,0xE9DD}, /* ��: GB2312 Code: 0xDDE9 ==> Row:61 Col:73 */
    {0x84D0,0xEADD}, /* ��: GB2312 Code: 0xDDEA ==> Row:61 Col:74 */
    {0x84D1,0xF2CB}, /* ��: GB2312 Code: 0xCBF2 ==> Row:43 Col:82 */
    {0x84D3,0xEDDD}, /* ��: GB2312 Code: 0xDDED ==> Row:61 Col:77 */
    {0x84D6,0xCDB1}, /* ��: GB2312 Code: 0xB1CD ==> Row:17 Col:45 */
    {0x84DD,0xB6C0}, /* ��: GB2312 Code: 0xC0B6 ==> Row:32 Col:22 */
    {0x84DF,0xBBBC}, /* ��: GB2312 Code: 0xBCBB ==> Row:28 Col:27 */
    {0x84E0,0xF1DD}, /* ��: GB2312 Code: 0xDDF1 ==> Row:61 Col:81 */
    {0x84E3,0xF7DD}, /* ��: GB2312 Code: 0xDDF7 ==> Row:61 Col:87 */
    {0x84E5,0xF6DD}, /* ��: GB2312 Code: 0xDDF6 ==> Row:61 Col:86 */
    {0x84E6,0xEBDD}, /* ��: GB2312 Code: 0xDDEB ==> Row:61 Col:75 */
    {0x84EC,0xEEC5}, /* ��: GB2312 Code: 0xC5EE ==> Row:37 Col:78 */
    {0x84F0,0xFBDD}, /* ��: GB2312 Code: 0xDDFB ==> Row:61 Col:91 */
    {0x84FC,0xA4DE}, /* ޤ: GB2312 Code: 0xDEA4 ==> Row:62 Col:04 */
    {0x84FF,0xA3DE}, /* ޣ: GB2312 Code: 0xDEA3 ==> Row:62 Col:03 */
    {0x850C,0xF8DD}, /* ��: GB2312 Code: 0xDDF8 ==> Row:61 Col:88 */
    {0x8511,0xEFC3}, /* ��: GB2312 Code: 0xC3EF ==> Row:35 Col:79 */
    {0x8513,0xFBC2}, /* ��: GB2312 Code: 0xC2FB ==> Row:34 Col:91 */
    {0x8517,0xE1D5}, /* ��: GB2312 Code: 0xD5E1 ==> Row:53 Col:65 */
    {0x851A,0xB5CE}, /* ε: GB2312 Code: 0xCEB5 ==> Row:46 Col:21 */
    {0x851F,0xFDDD}, /* ��: GB2312 Code: 0xDDFD ==> Row:61 Col:93 */
    {0x8521,0xCCB2}, /* ��: GB2312 Code: 0xB2CC ==> Row:18 Col:44 */
    {0x852B,0xE8C4}, /* ��: GB2312 Code: 0xC4E8 ==> Row:36 Col:72 */
    {0x852C,0xDFCA}, /* ��: GB2312 Code: 0xCADF ==> Row:42 Col:63 */
    {0x8537,0xBEC7}, /* Ǿ: GB2312 Code: 0xC7BE ==> Row:39 Col:30 */
    {0x8538,0xFADD}, /* ��: GB2312 Code: 0xDDFA ==> Row:61 Col:90 */
    {0x8539,0xFCDD}, /* ��: GB2312 Code: 0xDDFC ==> Row:61 Col:92 */
    {0x853A,0xFEDD}, /* ��: GB2312 Code: 0xDDFE ==> Row:61 Col:94 */
    {0x853B,0xA2DE}, /* ޢ: GB2312 Code: 0xDEA2 ==> Row:62 Col:02 */
    {0x853C,0xAAB0}, /* ��: GB2312 Code: 0xB0AA ==> Row:16 Col:10 */
    {0x853D,0xCEB1}, /* ��: GB2312 Code: 0xB1CE ==> Row:17 Col:46 */
    {0x8543,0xACDE}, /* ެ: GB2312 Code: 0xDEAC ==> Row:62 Col:12 */
    {0x8548,0xA6DE}, /* ަ: GB2312 Code: 0xDEA6 ==> Row:62 Col:06 */
    {0x8549,0xB6BD}, /* ��: GB2312 Code: 0xBDB6 ==> Row:29 Col:22 */
    {0x854A,0xEFC8}, /* ��: GB2312 Code: 0xC8EF ==> Row:40 Col:79 */
    {0x8556,0xA1DE}, /* ޡ: GB2312 Code: 0xDEA1 ==> Row:62 Col:01 */
    {0x8559,0xA5DE}, /* ޥ: GB2312 Code: 0xDEA5 ==> Row:62 Col:05 */
    {0x855E,0xA9DE}, /* ީ: GB2312 Code: 0xDEA9 ==> Row:62 Col:09 */
    {0x8564,0xA8DE}, /* ި: GB2312 Code: 0xDEA8 ==> Row:62 Col:08 */
    {0x8568,0xA7DE}, /* ާ: GB2312 Code: 0xDEA7 ==> Row:62 Col:07 */
    {0x8572,0xADDE}, /* ޭ: GB2312 Code: 0xDEAD ==> Row:62 Col:13 */
    {0x8574,0xCCD4}, /* ��: GB2312 Code: 0xD4CC ==> Row:52 Col:44 */
    {0x8579,0xB3DE}, /* ޳: GB2312 Code: 0xDEB3 ==> Row:62 Col:19 */
    {0x857A,0xAADE}, /* ު: GB2312 Code: 0xDEAA ==> Row:62 Col:10 */
    {0x857B,0xAEDE}, /* ޮ: GB2312 Code: 0xDEAE ==> Row:62 Col:14 */
    {0x857E,0xD9C0}, /* ��: GB2312 Code: 0xC0D9 ==> Row:32 Col:57 */
    {0x8584,0xA1B1}, /* ��: GB2312 Code: 0xB1A1 ==> Row:17 Col:01 */
    {0x8585,0xB6DE}, /* ޶: GB2312 Code: 0xDEB6 ==> Row:62 Col:22 */
    {0x8587,0xB1DE}, /* ޱ: GB2312 Code: 0xDEB1 ==> Row:62 Col:17 */
    {0x858F,0xB2DE}, /* ޲: GB2312 Code: 0xDEB2 ==> Row:62 Col:18 */
    {0x859B,0xA6D1}, /* Ѧ: GB2312 Code: 0xD1A6 ==> Row:49 Col:06 */
    {0x859C,0xB5DE}, /* ޵: GB2312 Code: 0xDEB5 ==> Row:62 Col:21 */
    {0x85A4,0xAFDE}, /* ޯ: GB2312 Code: 0xDEAF ==> Row:62 Col:15 */
    {0x85A8,0xB0DE}, /* ް: GB2312 Code: 0xDEB0 ==> Row:62 Col:16 */
    {0x85AA,0xBDD0}, /* н: GB2312 Code: 0xD0BD ==> Row:48 Col:29 */
    {0x85AE,0xB4DE}, /* ޴: GB2312 Code: 0xDEB4 ==> Row:62 Col:20 */
    {0x85AF,0xEDCA}, /* ��: GB2312 Code: 0xCAED ==> Row:42 Col:77 */
    {0x85B0,0xB9DE}, /* ޹: GB2312 Code: 0xDEB9 ==> Row:62 Col:25 */
    {0x85B7,0xB8DE}, /* ޸: GB2312 Code: 0xDEB8 ==> Row:62 Col:24 */
    {0x85B9,0xB7DE}, /* ޷: GB2312 Code: 0xDEB7 ==> Row:62 Col:23 */
    {0x85C1,0xBBDE}, /* ޻: GB2312 Code: 0xDEBB ==> Row:62 Col:27 */
    {0x85C9,0xE5BD}, /* ��: GB2312 Code: 0xBDE5 ==> Row:29 Col:69 */
    {0x85CF,0xD8B2}, /* ��: GB2312 Code: 0xB2D8 ==> Row:18 Col:56 */
    {0x85D0,0xEAC3}, /* ��: GB2312 Code: 0xC3EA ==> Row:35 Col:74 */
    {0x85D3,0xBADE}, /* ޺: GB2312 Code: 0xDEBA ==> Row:62 Col:26 */
    {0x85D5,0xBAC5}, /* ź: GB2312 Code: 0xC5BA ==> Row:37 Col:26 */
    {0x85DC,0xBCDE}, /* ޼: GB2312 Code: 0xDEBC ==> Row:62 Col:28 */
    {0x85E4,0xD9CC}, /* ��: GB2312 Code: 0xCCD9 ==> Row:44 Col:57 */
    {0x85E9,0xAAB7}, /* ��: GB2312 Code: 0xB7AA ==> Row:23 Col:10 */
    {0x85FB,0xE5D4}, /* ��: GB2312 Code: 0xD4E5 ==> Row:52 Col:69 */
    {0x85FF,0xBDDE}, /* ޽: GB2312 Code: 0xDEBD ==> Row:62 Col:29 */
    {0x8605,0xBFDE}, /* ޿: GB2312 Code: 0xDEBF ==> Row:62 Col:31 */
    {0x8611,0xA2C4}, /* Ģ: GB2312 Code: 0xC4A2 ==> Row:36 Col:02 */
    {0x8616,0xC1DE}, /* ��: GB2312 Code: 0xDEC1 ==> Row:62 Col:33 */
    {0x8627,0xBEDE}, /* ޾: GB2312 Code: 0xDEBE ==> Row:62 Col:30 */
    {0x8629,0xC0DE}, /* ��: GB2312 Code: 0xDEC0 ==> Row:62 Col:32 */
    {0x8638,0xBAD5}, /* պ: GB2312 Code: 0xD5BA ==> Row:53 Col:26 */
    {0x863C,0xC2DE}, /* ��: GB2312 Code: 0xDEC2 ==> Row:62 Col:34 */
    {0x864D,0xAEF2}, /* �: GB2312 Code: 0xF2AE ==> Row:82 Col:14 */
    {0x864E,0xA2BB}, /* ��: GB2312 Code: 0xBBA2 ==> Row:27 Col:02 */
    {0x864F,0xB2C2}, /* ²: GB2312 Code: 0xC2B2 ==> Row:34 Col:18 */
    {0x8650,0xB0C5}, /* Ű: GB2312 Code: 0xC5B0 ==> Row:37 Col:16 */
    {0x8651,0xC7C2}, /* ��: GB2312 Code: 0xC2C7 ==> Row:34 Col:39 */
    {0x8654,0xAFF2}, /* �: GB2312 Code: 0xF2AF ==> Row:82 Col:15 */
    {0x865A,0xE9D0}, /* ��: GB2312 Code: 0xD0E9 ==> Row:48 Col:73 */
    {0x865E,0xDDD3}, /* ��: GB2312 Code: 0xD3DD ==> Row:51 Col:61 */
    {0x8662,0xBDEB}, /* �: GB2312 Code: 0xEBBD ==> Row:75 Col:29 */
    {0x866B,0xE6B3}, /* ��: GB2312 Code: 0xB3E6 ==> Row:19 Col:70 */
    {0x866C,0xB0F2}, /* �: GB2312 Code: 0xF2B0 ==> Row:82 Col:16 */
    {0x866E,0xB1F2}, /* �: GB2312 Code: 0xF2B1 ==> Row:82 Col:17 */
    {0x8671,0xADCA}, /* ʭ: GB2312 Code: 0xCAAD ==> Row:42 Col:13 */
    {0x8679,0xE7BA}, /* ��: GB2312 Code: 0xBAE7 ==> Row:26 Col:71 */
    {0x867A,0xB3F2}, /* �: GB2312 Code: 0xF2B3 ==> Row:82 Col:19 */
    {0x867B,0xB5F2}, /* �: GB2312 Code: 0xF2B5 ==> Row:82 Col:21 */
    {0x867C,0xB4F2}, /* �: GB2312 Code: 0xF2B4 ==> Row:82 Col:20 */
    {0x867D,0xE4CB}, /* ��: GB2312 Code: 0xCBE4 ==> Row:43 Col:68 */
    {0x867E,0xBACF}, /* Ϻ: GB2312 Code: 0xCFBA ==> Row:47 Col:26 */
    {0x867F,0xB2F2}, /* �: GB2312 Code: 0xF2B2 ==> Row:82 Col:18 */
    {0x8680,0xB4CA}, /* ʴ: GB2312 Code: 0xCAB4 ==> Row:42 Col:20 */
    {0x8681,0xCFD2}, /* ��: GB2312 Code: 0xD2CF ==> Row:50 Col:47 */
    {0x8682,0xECC2}, /* ��: GB2312 Code: 0xC2EC ==> Row:34 Col:76 */
    {0x868A,0xC3CE}, /* ��: GB2312 Code: 0xCEC3 ==> Row:46 Col:35 */
    {0x868B,0xB8F2}, /* �: GB2312 Code: 0xF2B8 ==> Row:82 Col:24 */
    {0x868C,0xF6B0}, /* ��: GB2312 Code: 0xB0F6 ==> Row:16 Col:86 */
    {0x868D,0xB7F2}, /* �: GB2312 Code: 0xF2B7 ==> Row:82 Col:23 */
    {0x8693,0xBEF2}, /* �: GB2312 Code: 0xF2BE ==> Row:82 Col:30 */
    {0x8695,0xCFB2}, /* ��: GB2312 Code: 0xB2CF ==> Row:18 Col:47 */
    {0x869C,0xC1D1}, /* ��: GB2312 Code: 0xD1C1 ==> Row:49 Col:33 */
    {0x869D,0xBAF2}, /* �: GB2312 Code: 0xF2BA ==> Row:82 Col:26 */
    {0x86A3,0xBCF2}, /* �: GB2312 Code: 0xF2BC ==> Row:82 Col:28 */
    {0x86A4,0xE9D4}, /* ��: GB2312 Code: 0xD4E9 ==> Row:52 Col:73 */
    {0x86A7,0xBBF2}, /* �: GB2312 Code: 0xF2BB ==> Row:82 Col:27 */
    {0x86A8,0xB6F2}, /* �: GB2312 Code: 0xF2B6 ==> Row:82 Col:22 */
    {0x86A9,0xBFF2}, /* �: GB2312 Code: 0xF2BF ==> Row:82 Col:31 */
    {0x86AA,0xBDF2}, /* �: GB2312 Code: 0xF2BD ==> Row:82 Col:29 */
    {0x86AC,0xB9F2}, /* �: GB2312 Code: 0xF2B9 ==> Row:82 Col:25 */
    {0x86AF,0xC7F2}, /* ��: GB2312 Code: 0xF2C7 ==> Row:82 Col:39 */
    {0x86B0,0xC4F2}, /* ��: GB2312 Code: 0xF2C4 ==> Row:82 Col:36 */
    {0x86B1,0xC6F2}, /* ��: GB2312 Code: 0xF2C6 ==> Row:82 Col:38 */
    {0x86B4,0xCAF2}, /* ��: GB2312 Code: 0xF2CA ==> Row:82 Col:42 */
    {0x86B5,0xC2F2}, /* ��: GB2312 Code: 0xF2C2 ==> Row:82 Col:34 */
    {0x86B6,0xC0F2}, /* ��: GB2312 Code: 0xF2C0 ==> Row:82 Col:32 */
    {0x86BA,0xC5F2}, /* ��: GB2312 Code: 0xF2C5 ==> Row:82 Col:37 */
    {0x86C0,0xFBD6}, /* ��: GB2312 Code: 0xD6FB ==> Row:54 Col:91 */
    {0x86C4,0xC1F2}, /* ��: GB2312 Code: 0xF2C1 ==> Row:82 Col:33 */
    {0x86C6,0xF9C7}, /* ��: GB2312 Code: 0xC7F9 ==> Row:39 Col:89 */
    {0x86C7,0xDFC9}, /* ��: GB2312 Code: 0xC9DF ==> Row:41 Col:63 */
    {0x86C9,0xC8F2}, /* ��: GB2312 Code: 0xF2C8 ==> Row:82 Col:40 */
    {0x86CA,0xC6B9}, /* ��: GB2312 Code: 0xB9C6 ==> Row:25 Col:38 */
    {0x86CB,0xB0B5}, /* ��: GB2312 Code: 0xB5B0 ==> Row:21 Col:16 */
    {0x86CE,0xC3F2}, /* ��: GB2312 Code: 0xF2C3 ==> Row:82 Col:35 */
    {0x86CF,0xC9F2}, /* ��: GB2312 Code: 0xF2C9 ==> Row:82 Col:41 */
    {0x86D0,0xD0F2}, /* ��: GB2312 Code: 0xF2D0 ==> Row:82 Col:48 */
    {0x86D1,0xD6F2}, /* ��: GB2312 Code: 0xF2D6 ==> Row:82 Col:54 */
    {0x86D4,0xD7BB}, /* ��: GB2312 Code: 0xBBD7 ==> Row:27 Col:55 */
    {0x86D8,0xD5F2}, /* ��: GB2312 Code: 0xF2D5 ==> Row:82 Col:53 */
    {0x86D9,0xDCCD}, /* ��: GB2312 Code: 0xCDDC ==> Row:45 Col:60 */
    {0x86DB,0xEBD6}, /* ��: GB2312 Code: 0xD6EB ==> Row:54 Col:75 */
    {0x86DE,0xD2F2}, /* ��: GB2312 Code: 0xF2D2 ==> Row:82 Col:50 */
    {0x86DF,0xD4F2}, /* ��: GB2312 Code: 0xF2D4 ==> Row:82 Col:52 */
    {0x86E4,0xF2B8}, /* ��: GB2312 Code: 0xB8F2 ==> Row:24 Col:82 */
    {0x86E9,0xCBF2}, /* ��: GB2312 Code: 0xF2CB ==> Row:82 Col:43 */
    {0x86ED,0xCEF2}, /* ��: GB2312 Code: 0xF2CE ==> Row:82 Col:46 */
    {0x86EE,0xF9C2}, /* ��: GB2312 Code: 0xC2F9 ==> Row:34 Col:89 */
    {0x86F0,0xDDD5}, /* ��: GB2312 Code: 0xD5DD ==> Row:53 Col:61 */
    {0x86F1,0xCCF2}, /* ��: GB2312 Code: 0xF2CC ==> Row:82 Col:44 */
    {0x86F2,0xCDF2}, /* ��: GB2312 Code: 0xF2CD ==> Row:82 Col:45 */
    {0x86F3,0xCFF2}, /* ��: GB2312 Code: 0xF2CF ==> Row:82 Col:47 */
    {0x86F4,0xD3F2}, /* ��: GB2312 Code: 0xF2D3 ==> Row:82 Col:51 */
    {0x86F8,0xD9F2}, /* ��: GB2312 Code: 0xF2D9 ==> Row:82 Col:57 */
    {0x86F9,0xBCD3}, /* Ӽ: GB2312 Code: 0xD3BC ==> Row:51 Col:28 */
    {0x86FE,0xEAB6}, /* ��: GB2312 Code: 0xB6EA ==> Row:22 Col:74 */
    {0x8700,0xF1CA}, /* ��: GB2312 Code: 0xCAF1 ==> Row:42 Col:81 */
    {0x8702,0xE4B7}, /* ��: GB2312 Code: 0xB7E4 ==> Row:23 Col:68 */
    {0x8703,0xD7F2}, /* ��: GB2312 Code: 0xF2D7 ==> Row:82 Col:55 */
    {0x8707,0xD8F2}, /* ��: GB2312 Code: 0xF2D8 ==> Row:82 Col:56 */
    {0x8708,0xDAF2}, /* ��: GB2312 Code: 0xF2DA ==> Row:82 Col:58 */
    {0x8709,0xDDF2}, /* ��: GB2312 Code: 0xF2DD ==> Row:82 Col:61 */
    {0x870A,0xDBF2}, /* ��: GB2312 Code: 0xF2DB ==> Row:82 Col:59 */
    {0x870D,0xDCF2}, /* ��: GB2312 Code: 0xF2DC ==> Row:82 Col:60 */
    {0x8712,0xD1D1}, /* ��: GB2312 Code: 0xD1D1 ==> Row:49 Col:49 */
    {0x8713,0xD1F2}, /* ��: GB2312 Code: 0xF2D1 ==> Row:82 Col:49 */
    {0x8715,0xC9CD}, /* ��: GB2312 Code: 0xCDC9 ==> Row:45 Col:41 */
    {0x8717,0xCFCE}, /* ��: GB2312 Code: 0xCECF ==> Row:46 Col:47 */
    {0x8718,0xA9D6}, /* ֩: GB2312 Code: 0xD6A9 ==> Row:54 Col:09 */
    {0x871A,0xE3F2}, /* ��: GB2312 Code: 0xF2E3 ==> Row:82 Col:67 */
    {0x871C,0xDBC3}, /* ��: GB2312 Code: 0xC3DB ==> Row:35 Col:59 */
    {0x871E,0xE0F2}, /* ��: GB2312 Code: 0xF2E0 ==> Row:82 Col:64 */
    {0x8721,0xAFC0}, /* ��: GB2312 Code: 0xC0AF ==> Row:32 Col:15 */
    {0x8722,0xECF2}, /* ��: GB2312 Code: 0xF2EC ==> Row:82 Col:76 */
    {0x8723,0xDEF2}, /* ��: GB2312 Code: 0xF2DE ==> Row:82 Col:62 */
    {0x8725,0xE1F2}, /* ��: GB2312 Code: 0xF2E1 ==> Row:82 Col:65 */
    {0x8729,0xE8F2}, /* ��: GB2312 Code: 0xF2E8 ==> Row:82 Col:72 */
    {0x872E,0xE2F2}, /* ��: GB2312 Code: 0xF2E2 ==> Row:82 Col:66 */
    {0x8731,0xE7F2}, /* ��: GB2312 Code: 0xF2E7 ==> Row:82 Col:71 */
    {0x8734,0xE6F2}, /* ��: GB2312 Code: 0xF2E6 ==> Row:82 Col:70 */
    {0x8737,0xE9F2}, /* ��: GB2312 Code: 0xF2E9 ==> Row:82 Col:73 */
    {0x873B,0xDFF2}, /* ��: GB2312 Code: 0xF2DF ==> Row:82 Col:63 */
    {0x873E,0xE4F2}, /* ��: GB2312 Code: 0xF2E4 ==> Row:82 Col:68 */
    {0x873F,0xEAF2}, /* ��: GB2312 Code: 0xF2EA ==> Row:82 Col:74 */
    {0x8747,0xACD3}, /* Ӭ: GB2312 Code: 0xD3AC ==> Row:51 Col:12 */
    {0x8748,0xE5F2}, /* ��: GB2312 Code: 0xF2E5 ==> Row:82 Col:69 */
    {0x8749,0xF5B2}, /* ��: GB2312 Code: 0xB2F5 ==> Row:18 Col:85 */
    {0x874C,0xF2F2}, /* ��: GB2312 Code: 0xF2F2 ==> Row:82 Col:82 */
    {0x874E,0xABD0}, /* Ы: GB2312 Code: 0xD0AB ==> Row:48 Col:11 */
    {0x8753,0xF5F2}, /* ��: GB2312 Code: 0xF2F5 ==> Row:82 Col:85 */
    {0x8757,0xC8BB}, /* ��: GB2312 Code: 0xBBC8 ==> Row:27 Col:40 */
    {0x8759,0xF9F2}, /* ��: GB2312 Code: 0xF2F9 ==> Row:82 Col:89 */
    {0x8760,0xF0F2}, /* ��: GB2312 Code: 0xF2F0 ==> Row:82 Col:80 */
    {0x8763,0xF6F2}, /* ��: GB2312 Code: 0xF2F6 ==> Row:82 Col:86 */
    {0x8764,0xF8F2}, /* ��: GB2312 Code: 0xF2F8 ==> Row:82 Col:88 */
    {0x8765,0xFAF2}, /* ��: GB2312 Code: 0xF2FA ==> Row:82 Col:90 */
    {0x876E,0xF3F2}, /* ��: GB2312 Code: 0xF2F3 ==> Row:82 Col:83 */
    {0x8770,0xF1F2}, /* ��: GB2312 Code: 0xF2F1 ==> Row:82 Col:81 */
    {0x8774,0xFBBA}, /* ��: GB2312 Code: 0xBAFB ==> Row:26 Col:91 */
    {0x8776,0xFBB5}, /* ��: GB2312 Code: 0xB5FB ==> Row:21 Col:91 */
    {0x877B,0xEFF2}, /* ��: GB2312 Code: 0xF2EF ==> Row:82 Col:79 */
    {0x877C,0xF7F2}, /* ��: GB2312 Code: 0xF2F7 ==> Row:82 Col:87 */
    {0x877D,0xEDF2}, /* ��: GB2312 Code: 0xF2ED ==> Row:82 Col:77 */
    {0x877E,0xEEF2}, /* ��: GB2312 Code: 0xF2EE ==> Row:82 Col:78 */
    {0x8782,0xEBF2}, /* ��: GB2312 Code: 0xF2EB ==> Row:82 Col:75 */
    {0x8783,0xA6F3}, /* �: GB2312 Code: 0xF3A6 ==> Row:83 Col:06 */
    {0x8785,0xA3F3}, /* �: GB2312 Code: 0xF3A3 ==> Row:83 Col:03 */
    {0x8788,0xA2F3}, /* �: GB2312 Code: 0xF3A2 ==> Row:83 Col:02 */
    {0x878B,0xF4F2}, /* ��: GB2312 Code: 0xF2F4 ==> Row:82 Col:84 */
    {0x878D,0xDAC8}, /* ��: GB2312 Code: 0xC8DA ==> Row:40 Col:58 */
    {0x8793,0xFBF2}, /* ��: GB2312 Code: 0xF2FB ==> Row:82 Col:91 */
    {0x8797,0xA5F3}, /* �: GB2312 Code: 0xF3A5 ==> Row:83 Col:05 */
    {0x879F,0xF8C3}, /* ��: GB2312 Code: 0xC3F8 ==> Row:35 Col:88 */
    {0x87A8,0xFDF2}, /* ��: GB2312 Code: 0xF2FD ==> Row:82 Col:93 */
    {0x87AB,0xA7F3}, /* �: GB2312 Code: 0xF3A7 ==> Row:83 Col:07 */
    {0x87AC,0xA9F3}, /* �: GB2312 Code: 0xF3A9 ==> Row:83 Col:09 */
    {0x87AD,0xA4F3}, /* �: GB2312 Code: 0xF3A4 ==> Row:83 Col:04 */
    {0x87AF,0xFCF2}, /* ��: GB2312 Code: 0xF2FC ==> Row:82 Col:92 */
    {0x87B3,0xABF3}, /* �: GB2312 Code: 0xF3AB ==> Row:83 Col:11 */
    {0x87B5,0xAAF3}, /* �: GB2312 Code: 0xF3AA ==> Row:83 Col:10 */
    {0x87BA,0xDDC2}, /* ��: GB2312 Code: 0xC2DD ==> Row:34 Col:61 */
    {0x87BD,0xAEF3}, /* �: GB2312 Code: 0xF3AE ==> Row:83 Col:14 */
    {0x87C0,0xB0F3}, /* �: GB2312 Code: 0xF3B0 ==> Row:83 Col:16 */
    {0x87C6,0xA1F3}, /* �: GB2312 Code: 0xF3A1 ==> Row:83 Col:01 */
    {0x87CA,0xB1F3}, /* �: GB2312 Code: 0xF3B1 ==> Row:83 Col:17 */
    {0x87CB,0xACF3}, /* �: GB2312 Code: 0xF3AC ==> Row:83 Col:12 */
    {0x87D1,0xAFF3}, /* �: GB2312 Code: 0xF3AF ==> Row:83 Col:15 */
    {0x87D2,0xFEF2}, /* ��: GB2312 Code: 0xF2FE ==> Row:82 Col:94 */
    {0x87D3,0xADF3}, /* �: GB2312 Code: 0xF3AD ==> Row:83 Col:13 */
    {0x87DB,0xB2F3}, /* �: GB2312 Code: 0xF3B2 ==> Row:83 Col:18 */
    {0x87E0,0xB4F3}, /* �: GB2312 Code: 0xF3B4 ==> Row:83 Col:20 */
    {0x87E5,0xA8F3}, /* �: GB2312 Code: 0xF3A8 ==> Row:83 Col:08 */
    {0x87EA,0xB3F3}, /* �: GB2312 Code: 0xF3B3 ==> Row:83 Col:19 */
    {0x87EE,0xB5F3}, /* �: GB2312 Code: 0xF3B5 ==> Row:83 Col:21 */
    {0x87F9,0xB7D0}, /* з: GB2312 Code: 0xD0B7 ==> Row:48 Col:23 */
    {0x87FE,0xB8F3}, /* �: GB2312 Code: 0xF3B8 ==> Row:83 Col:24 */
    {0x8803,0xF9D9}, /* ��: GB2312 Code: 0xD9F9 ==> Row:57 Col:89 */
    {0x880A,0xB9F3}, /* �: GB2312 Code: 0xF3B9 ==> Row:83 Col:25 */
    {0x8813,0xB7F3}, /* �: GB2312 Code: 0xF3B7 ==> Row:83 Col:23 */
    {0x8815,0xE4C8}, /* ��: GB2312 Code: 0xC8E4 ==> Row:40 Col:68 */
    {0x8816,0xB6F3}, /* �: GB2312 Code: 0xF3B6 ==> Row:83 Col:22 */
    {0x881B,0xBAF3}, /* �: GB2312 Code: 0xF3BA ==> Row:83 Col:26 */
    {0x8821,0xBBF3}, /* �: GB2312 Code: 0xF3BB ==> Row:83 Col:27 */
    {0x8822,0xC0B4}, /* ��: GB2312 Code: 0xB4C0 ==> Row:20 Col:32 */
    {0x8832,0xC3EE}, /* ��: GB2312 Code: 0xEEC3 ==> Row:78 Col:35 */
    {0x8839,0xBCF3}, /* �: GB2312 Code: 0xF3BC ==> Row:83 Col:28 */
    {0x883C,0xBDF3}, /* �: GB2312 Code: 0xF3BD ==> Row:83 Col:29 */
    {0x8840,0xAAD1}, /* Ѫ: GB2312 Code: 0xD1AA ==> Row:49 Col:10 */
    {0x8844,0xACF4}, /* ��: GB2312 Code: 0xF4AC ==> Row:84 Col:12 */
    {0x8845,0xC6D0}, /* ��: GB2312 Code: 0xD0C6 ==> Row:48 Col:38 */
    {0x884C,0xD0D0}, /* ��: GB2312 Code: 0xD0D0 ==> Row:48 Col:48 */
    {0x884D,0xDCD1}, /* ��: GB2312 Code: 0xD1DC ==> Row:49 Col:60 */
    {0x8854,0xCECF}, /* ��: GB2312 Code: 0xCFCE ==> Row:47 Col:46 */
    {0x8857,0xD6BD}, /* ��: GB2312 Code: 0xBDD6 ==> Row:29 Col:54 */
    {0x8859,0xC3D1}, /* ��: GB2312 Code: 0xD1C3 ==> Row:49 Col:35 */
    {0x8861,0xE2BA}, /* ��: GB2312 Code: 0xBAE2 ==> Row:26 Col:66 */
    {0x8862,0xE9E1}, /* ��: GB2312 Code: 0xE1E9 ==> Row:65 Col:73 */
    {0x8863,0xC2D2}, /* ��: GB2312 Code: 0xD2C2 ==> Row:50 Col:34 */
    {0x8864,0xC2F1}, /* ��: GB2312 Code: 0xF1C2 ==> Row:81 Col:34 */
    {0x8865,0xB9B2}, /* ��: GB2312 Code: 0xB2B9 ==> Row:18 Col:25 */
    {0x8868,0xEDB1}, /* ��: GB2312 Code: 0xB1ED ==> Row:17 Col:77 */
    {0x8869,0xC3F1}, /* ��: GB2312 Code: 0xF1C3 ==> Row:81 Col:35 */
    {0x886B,0xC0C9}, /* ��: GB2312 Code: 0xC9C0 ==> Row:41 Col:32 */
    {0x886C,0xC4B3}, /* ��: GB2312 Code: 0xB3C4 ==> Row:19 Col:36 */
    {0x886E,0xF2D9}, /* ��: GB2312 Code: 0xD9F2 ==> Row:57 Col:82 */
    {0x8870,0xA5CB}, /* ˥: GB2312 Code: 0xCBA5 ==> Row:43 Col:05 */
    {0x8872,0xC4F1}, /* ��: GB2312 Code: 0xF1C4 ==> Row:81 Col:36 */
    {0x8877,0xD4D6}, /* ��: GB2312 Code: 0xD6D4 ==> Row:54 Col:52 */
    {0x887D,0xC5F1}, /* ��: GB2312 Code: 0xF1C5 ==> Row:81 Col:37 */
    {0x887E,0xC0F4}, /* ��: GB2312 Code: 0xF4C0 ==> Row:84 Col:32 */
    {0x887F,0xC6F1}, /* ��: GB2312 Code: 0xF1C6 ==> Row:81 Col:38 */
    {0x8881,0xACD4}, /* Ԭ: GB2312 Code: 0xD4AC ==> Row:52 Col:12 */
    {0x8882,0xC7F1}, /* ��: GB2312 Code: 0xF1C7 ==> Row:81 Col:39 */
    {0x8884,0xC0B0}, /* ��: GB2312 Code: 0xB0C0 ==> Row:16 Col:32 */
    {0x8885,0xC1F4}, /* ��: GB2312 Code: 0xF4C1 ==> Row:84 Col:33 */
    {0x8888,0xC2F4}, /* ��: GB2312 Code: 0xF4C2 ==> Row:84 Col:34 */
    {0x888B,0xFCB4}, /* ��: GB2312 Code: 0xB4FC ==> Row:20 Col:92 */
    {0x888D,0xDBC5}, /* ��: GB2312 Code: 0xC5DB ==> Row:37 Col:59 */
    {0x8892,0xBBCC}, /* ̻: GB2312 Code: 0xCCBB ==> Row:44 Col:27 */
    {0x8896,0xE4D0}, /* ��: GB2312 Code: 0xD0E4 ==> Row:48 Col:68 */
    {0x889C,0xE0CD}, /* ��: GB2312 Code: 0xCDE0 ==> Row:45 Col:64 */
    {0x88A2,0xC8F1}, /* ��: GB2312 Code: 0xF1C8 ==> Row:81 Col:40 */
    {0x88A4,0xF3D9}, /* ��: GB2312 Code: 0xD9F3 ==> Row:57 Col:83 */
    {0x88AB,0xBBB1}, /* ��: GB2312 Code: 0xB1BB ==> Row:17 Col:27 */
    {0x88AD,0xAECF}, /* Ϯ: GB2312 Code: 0xCFAE ==> Row:47 Col:14 */
    {0x88B1,0xA4B8}, /* ��: GB2312 Code: 0xB8A4 ==> Row:24 Col:04 */
    {0x88B7,0xCAF1}, /* ��: GB2312 Code: 0xF1CA ==> Row:81 Col:42 */
    {0x88BC,0xCBF1}, /* ��: GB2312 Code: 0xF1CB ==> Row:81 Col:43 */
    {0x88C1,0xC3B2}, /* ��: GB2312 Code: 0xB2C3 ==> Row:18 Col:35 */
    {0x88C2,0xD1C1}, /* ��: GB2312 Code: 0xC1D1 ==> Row:33 Col:49 */
    {0x88C5,0xB0D7}, /* װ: GB2312 Code: 0xD7B0 ==> Row:55 Col:16 */
    {0x88C6,0xC9F1}, /* ��: GB2312 Code: 0xF1C9 ==> Row:81 Col:41 */
    {0x88C9,0xCCF1}, /* ��: GB2312 Code: 0xF1CC ==> Row:81 Col:44 */
    {0x88CE,0xCEF1}, /* ��: GB2312 Code: 0xF1CE ==> Row:81 Col:46 */
    {0x88D2,0xF6D9}, /* ��: GB2312 Code: 0xD9F6 ==> Row:57 Col:86 */
    {0x88D4,0xE1D2}, /* ��: GB2312 Code: 0xD2E1 ==> Row:50 Col:65 */
    {0x88D5,0xA3D4}, /* ԣ: GB2312 Code: 0xD4A3 ==> Row:52 Col:03 */
    {0x88D8,0xC3F4}, /* ��: GB2312 Code: 0xF4C3 ==> Row:84 Col:35 */
    {0x88D9,0xB9C8}, /* ȹ: GB2312 Code: 0xC8B9 ==> Row:40 Col:25 */
    {0x88DF,0xC4F4}, /* ��: GB2312 Code: 0xF4C4 ==> Row:84 Col:36 */
    {0x88E2,0xCDF1}, /* ��: GB2312 Code: 0xF1CD ==> Row:81 Col:45 */
    {0x88E3,0xCFF1}, /* ��: GB2312 Code: 0xF1CF ==> Row:81 Col:47 */
    {0x88E4,0xE3BF}, /* ��: GB2312 Code: 0xBFE3 ==> Row:31 Col:67 */
    {0x88E5,0xD0F1}, /* ��: GB2312 Code: 0xF1D0 ==> Row:81 Col:48 */
    {0x88E8,0xD4F1}, /* ��: GB2312 Code: 0xF1D4 ==> Row:81 Col:52 */
    {0x88F0,0xD6F1}, /* ��: GB2312 Code: 0xF1D6 ==> Row:81 Col:54 */
    {0x88F1,0xD1F1}, /* ��: GB2312 Code: 0xF1D1 ==> Row:81 Col:49 */
    {0x88F3,0xD1C9}, /* ��: GB2312 Code: 0xC9D1 ==> Row:41 Col:49 */
    {0x88F4,0xE1C5}, /* ��: GB2312 Code: 0xC5E1 ==> Row:37 Col:65 */
    {0x88F8,0xE3C2}, /* ��: GB2312 Code: 0xC2E3 ==> Row:34 Col:67 */
    {0x88F9,0xFCB9}, /* ��: GB2312 Code: 0xB9FC ==> Row:25 Col:92 */
    {0x88FC,0xD3F1}, /* ��: GB2312 Code: 0xF1D3 ==> Row:81 Col:51 */
    {0x88FE,0xD5F1}, /* ��: GB2312 Code: 0xF1D5 ==> Row:81 Col:53 */
    {0x8902,0xD3B9}, /* ��: GB2312 Code: 0xB9D3 ==> Row:25 Col:51 */
    {0x890A,0xDBF1}, /* ��: GB2312 Code: 0xF1DB ==> Row:81 Col:59 */
    {0x8910,0xD6BA}, /* ��: GB2312 Code: 0xBAD6 ==> Row:26 Col:54 */
    {0x8912,0xFDB0}, /* ��: GB2312 Code: 0xB0FD ==> Row:16 Col:93 */
    {0x8913,0xD9F1}, /* ��: GB2312 Code: 0xF1D9 ==> Row:81 Col:57 */
    {0x8919,0xD8F1}, /* ��: GB2312 Code: 0xF1D8 ==> Row:81 Col:56 */
    {0x891A,0xD2F1}, /* ��: GB2312 Code: 0xF1D2 ==> Row:81 Col:50 */
    {0x891B,0xDAF1}, /* ��: GB2312 Code: 0xF1DA ==> Row:81 Col:58 */
    {0x8921,0xD7F1}, /* ��: GB2312 Code: 0xF1D7 ==> Row:81 Col:55 */
    {0x8925,0xECC8}, /* ��: GB2312 Code: 0xC8EC ==> Row:40 Col:76 */
    {0x892A,0xCACD}, /* ��: GB2312 Code: 0xCDCA ==> Row:45 Col:42 */
    {0x892B,0xDDF1}, /* ��: GB2312 Code: 0xF1DD ==> Row:81 Col:61 */
    {0x8930,0xBDE5}, /* �: GB2312 Code: 0xE5BD ==> Row:69 Col:29 */
    {0x8934,0xDCF1}, /* ��: GB2312 Code: 0xF1DC ==> Row:81 Col:60 */
    {0x8936,0xDEF1}, /* ��: GB2312 Code: 0xF1DE ==> Row:81 Col:62 */
    {0x8941,0xDFF1}, /* ��: GB2312 Code: 0xF1DF ==> Row:81 Col:63 */
    {0x8944,0xE5CF}, /* ��: GB2312 Code: 0xCFE5 ==> Row:47 Col:69 */
    {0x895E,0xC5F4}, /* ��: GB2312 Code: 0xF4C5 ==> Row:84 Col:37 */
    {0x895F,0xF3BD}, /* ��: GB2312 Code: 0xBDF3 ==> Row:29 Col:83 */
    {0x8966,0xE0F1}, /* ��: GB2312 Code: 0xF1E0 ==> Row:81 Col:64 */
    {0x897B,0xE1F1}, /* ��: GB2312 Code: 0xF1E1 ==> Row:81 Col:65 */
    {0x897F,0xF7CE}, /* ��: GB2312 Code: 0xCEF7 ==> Row:46 Col:87 */
    {0x8981,0xAAD2}, /* Ҫ: GB2312 Code: 0xD2AA ==> Row:50 Col:10 */
    {0x8983,0xFBF1}, /* ��: GB2312 Code: 0xF1FB ==> Row:81 Col:91 */
    {0x8986,0xB2B8}, /* ��: GB2312 Code: 0xB8B2 ==> Row:24 Col:18 */
    {0x89C1,0xFBBC}, /* ��: GB2312 Code: 0xBCFB ==> Row:28 Col:91 */
    {0x89C2,0xDBB9}, /* ��: GB2312 Code: 0xB9DB ==> Row:25 Col:59 */
    {0x89C4,0xE6B9}, /* ��: GB2312 Code: 0xB9E6 ==> Row:25 Col:70 */
    {0x89C5,0xD9C3}, /* ��: GB2312 Code: 0xC3D9 ==> Row:35 Col:57 */
    {0x89C6,0xD3CA}, /* ��: GB2312 Code: 0xCAD3 ==> Row:42 Col:51 */
    {0x89C7,0xE8EA}, /* ��: GB2312 Code: 0xEAE8 ==> Row:74 Col:72 */
    {0x89C8,0xC0C0}, /* ��: GB2312 Code: 0xC0C0 ==> Row:32 Col:32 */
    {0x89C9,0xF5BE}, /* ��: GB2312 Code: 0xBEF5 ==> Row:30 Col:85 */
    {0x89CA,0xE9EA}, /* ��: GB2312 Code: 0xEAE9 ==> Row:74 Col:73 */
    {0x89CB,0xEAEA}, /* ��: GB2312 Code: 0xEAEA ==> Row:74 Col:74 */
    {0x89CC,0xEBEA}, /* ��: GB2312 Code: 0xEAEB ==> Row:74 Col:75 */
    {0x89CE,0xECEA}, /* ��: GB2312 Code: 0xEAEC ==> Row:74 Col:76 */
    {0x89CF,0xEDEA}, /* ��: GB2312 Code: 0xEAED ==> Row:74 Col:77 */
    {0x89D0,0xEEEA}, /* ��: GB2312 Code: 0xEAEE ==> Row:74 Col:78 */
    {0x89D1,0xEFEA}, /* ��: GB2312 Code: 0xEAEF ==> Row:74 Col:79 */
    {0x89D2,0xC7BD}, /* ��: GB2312 Code: 0xBDC7 ==> Row:29 Col:39 */
    {0x89D6,0xFBF5}, /* ��: GB2312 Code: 0xF5FB ==> Row:85 Col:91 */
    {0x89DA,0xFDF5}, /* ��: GB2312 Code: 0xF5FD ==> Row:85 Col:93 */
    {0x89DC,0xFEF5}, /* ��: GB2312 Code: 0xF5FE ==> Row:85 Col:94 */
    {0x89DE,0xFCF5}, /* ��: GB2312 Code: 0xF5FC ==> Row:85 Col:92 */
    {0x89E3,0xE2BD}, /* ��: GB2312 Code: 0xBDE2 ==> Row:29 Col:66 */
    {0x89E5,0xA1F6}, /* ��: GB2312 Code: 0xF6A1 ==> Row:86 Col:01 */
    {0x89E6,0xA5B4}, /* ��: GB2312 Code: 0xB4A5 ==> Row:20 Col:05 */
    {0x89EB,0xA2F6}, /* ��: GB2312 Code: 0xF6A2 ==> Row:86 Col:02 */
    {0x89EF,0xA3F6}, /* ��: GB2312 Code: 0xF6A3 ==> Row:86 Col:03 */
    {0x89F3,0xB2EC}, /* �: GB2312 Code: 0xECB2 ==> Row:76 Col:18 */
    {0x8A00,0xD4D1}, /* ��: GB2312 Code: 0xD1D4 ==> Row:49 Col:52 */
    {0x8A07,0xEAD9}, /* ��: GB2312 Code: 0xD9EA ==> Row:57 Col:74 */
    {0x8A3E,0xA4F6}, /* ��: GB2312 Code: 0xF6A4 ==> Row:86 Col:04 */
    {0x8A48,0xBAEE}, /* �: GB2312 Code: 0xEEBA ==> Row:78 Col:26 */
    {0x8A79,0xB2D5}, /* ղ: GB2312 Code: 0xD5B2 ==> Row:53 Col:18 */
    {0x8A89,0xFED3}, /* ��: GB2312 Code: 0xD3FE ==> Row:51 Col:94 */
    {0x8A8A,0xDCCC}, /* ��: GB2312 Code: 0xCCDC ==> Row:44 Col:60 */
    {0x8A93,0xC4CA}, /* ��: GB2312 Code: 0xCAC4 ==> Row:42 Col:36 */
    {0x8B07,0xC0E5}, /* ��: GB2312 Code: 0xE5C0 ==> Row:69 Col:32 */
    {0x8B26,0xA5F6}, /* ��: GB2312 Code: 0xF6A5 ==> Row:86 Col:05 */
    {0x8B66,0xAFBE}, /* ��: GB2312 Code: 0xBEAF ==> Row:30 Col:15 */
    {0x8B6C,0xA9C6}, /* Ʃ: GB2312 Code: 0xC6A9 ==> Row:38 Col:09 */
    {0x8BA0,0xA5DA}, /* ڥ: GB2312 Code: 0xDAA5 ==> Row:58 Col:05 */
    {0x8BA1,0xC6BC}, /* ��: GB2312 Code: 0xBCC6 ==> Row:28 Col:38 */
    {0x8BA2,0xA9B6}, /* ��: GB2312 Code: 0xB6A9 ==> Row:22 Col:09 */
    {0x8BA3,0xBCB8}, /* ��: GB2312 Code: 0xB8BC ==> Row:24 Col:28 */
    {0x8BA4,0xCFC8}, /* ��: GB2312 Code: 0xC8CF ==> Row:40 Col:47 */
    {0x8BA5,0xA5BC}, /* ��: GB2312 Code: 0xBCA5 ==> Row:28 Col:05 */
    {0x8BA6,0xA6DA}, /* ڦ: GB2312 Code: 0xDAA6 ==> Row:58 Col:06 */
    {0x8BA7,0xA7DA}, /* ڧ: GB2312 Code: 0xDAA7 ==> Row:58 Col:07 */
    {0x8BA8,0xD6CC}, /* ��: GB2312 Code: 0xCCD6 ==> Row:44 Col:54 */
    {0x8BA9,0xC3C8}, /* ��: GB2312 Code: 0xC8C3 ==> Row:40 Col:35 */
    {0x8BAA,0xA8DA}, /* ڨ: GB2312 Code: 0xDAA8 ==> Row:58 Col:08 */
    {0x8BAB,0xFDC6}, /* ��: GB2312 Code: 0xC6FD ==> Row:38 Col:93 */
    {0x8BAD,0xB5D1}, /* ѵ: GB2312 Code: 0xD1B5 ==> Row:49 Col:21 */
    {0x8BAE,0xE9D2}, /* ��: GB2312 Code: 0xD2E9 ==> Row:50 Col:73 */
    {0x8BAF,0xB6D1}, /* Ѷ: GB2312 Code: 0xD1B6 ==> Row:49 Col:22 */
    {0x8BB0,0xC7BC}, /* ��: GB2312 Code: 0xBCC7 ==> Row:28 Col:39 */
    {0x8BB2,0xB2BD}, /* ��: GB2312 Code: 0xBDB2 ==> Row:29 Col:18 */
    {0x8BB3,0xE4BB}, /* ��: GB2312 Code: 0xBBE4 ==> Row:27 Col:68 */
    {0x8BB4,0xA9DA}, /* ک: GB2312 Code: 0xDAA9 ==> Row:58 Col:09 */
    {0x8BB5,0xAADA}, /* ڪ: GB2312 Code: 0xDAAA ==> Row:58 Col:10 */
    {0x8BB6,0xC8D1}, /* ��: GB2312 Code: 0xD1C8 ==> Row:49 Col:40 */
    {0x8BB7,0xABDA}, /* ګ: GB2312 Code: 0xDAAB ==> Row:58 Col:11 */
    {0x8BB8,0xEDD0}, /* ��: GB2312 Code: 0xD0ED ==> Row:48 Col:77 */
    {0x8BB9,0xEFB6}, /* ��: GB2312 Code: 0xB6EF ==> Row:22 Col:79 */
    {0x8BBA,0xDBC2}, /* ��: GB2312 Code: 0xC2DB ==> Row:34 Col:59 */
    {0x8BBC,0xCFCB}, /* ��: GB2312 Code: 0xCBCF ==> Row:43 Col:47 */
    {0x8BBD,0xEDB7}, /* ��: GB2312 Code: 0xB7ED ==> Row:23 Col:77 */
    {0x8BBE,0xE8C9}, /* ��: GB2312 Code: 0xC9E8 ==> Row:41 Col:72 */
    {0x8BBF,0xC3B7}, /* ��: GB2312 Code: 0xB7C3 ==> Row:23 Col:35 */
    {0x8BC0,0xF7BE}, /* ��: GB2312 Code: 0xBEF7 ==> Row:30 Col:87 */
    {0x8BC1,0xA4D6}, /* ֤: GB2312 Code: 0xD6A4 ==> Row:54 Col:04 */
    {0x8BC2,0xACDA}, /* ڬ: GB2312 Code: 0xDAAC ==> Row:58 Col:12 */
    {0x8BC3,0xADDA}, /* ڭ: GB2312 Code: 0xDAAD ==> Row:58 Col:13 */
    {0x8BC4,0xC0C6}, /* ��: GB2312 Code: 0xC6C0 ==> Row:38 Col:32 */
    {0x8BC5,0xE7D7}, /* ��: GB2312 Code: 0xD7E7 ==> Row:55 Col:71 */
    {0x8BC6,0xB6CA}, /* ʶ: GB2312 Code: 0xCAB6 ==> Row:42 Col:22 */
    {0x8BC8,0xA9D5}, /* թ: GB2312 Code: 0xD5A9 ==> Row:53 Col:09 */
    {0x8BC9,0xDFCB}, /* ��: GB2312 Code: 0xCBDF ==> Row:43 Col:63 */
    {0x8BCA,0xEFD5}, /* ��: GB2312 Code: 0xD5EF ==> Row:53 Col:79 */
    {0x8BCB,0xAEDA}, /* ڮ: GB2312 Code: 0xDAAE ==> Row:58 Col:14 */
    {0x8BCC,0xDFD6}, /* ��: GB2312 Code: 0xD6DF ==> Row:54 Col:63 */
    {0x8BCD,0xCAB4}, /* ��: GB2312 Code: 0xB4CA ==> Row:20 Col:42 */
    {0x8BCE,0xB0DA}, /* ڰ: GB2312 Code: 0xDAB0 ==> Row:58 Col:16 */
    {0x8BCF,0xAFDA}, /* گ: GB2312 Code: 0xDAAF ==> Row:58 Col:15 */
    {0x8BD1,0xEBD2}, /* ��: GB2312 Code: 0xD2EB ==> Row:50 Col:75 */
    {0x8BD2,0xB1DA}, /* ڱ: GB2312 Code: 0xDAB1 ==> Row:58 Col:17 */
    {0x8BD3,0xB2DA}, /* ڲ: GB2312 Code: 0xDAB2 ==> Row:58 Col:18 */
    {0x8BD4,0xB3DA}, /* ڳ: GB2312 Code: 0xDAB3 ==> Row:58 Col:19 */
    {0x8BD5,0xD4CA}, /* ��: GB2312 Code: 0xCAD4 ==> Row:42 Col:52 */
    {0x8BD6,0xB4DA}, /* ڴ: GB2312 Code: 0xDAB4 ==> Row:58 Col:20 */
    {0x8BD7,0xABCA}, /* ʫ: GB2312 Code: 0xCAAB ==> Row:42 Col:11 */
    {0x8BD8,0xB5DA}, /* ڵ: GB2312 Code: 0xDAB5 ==> Row:58 Col:21 */
    {0x8BD9,0xB6DA}, /* ڶ: GB2312 Code: 0xDAB6 ==> Row:58 Col:22 */
    {0x8BDA,0xCFB3}, /* ��: GB2312 Code: 0xB3CF ==> Row:19 Col:47 */
    {0x8BDB,0xEFD6}, /* ��: GB2312 Code: 0xD6EF ==> Row:54 Col:79 */
    {0x8BDC,0xB7DA}, /* ڷ: GB2312 Code: 0xDAB7 ==> Row:58 Col:23 */
    {0x8BDD,0xB0BB}, /* ��: GB2312 Code: 0xBBB0 ==> Row:27 Col:16 */
    {0x8BDE,0xAEB5}, /* ��: GB2312 Code: 0xB5AE ==> Row:21 Col:14 */
    {0x8BDF,0xB8DA}, /* ڸ: GB2312 Code: 0xDAB8 ==> Row:58 Col:24 */
    {0x8BE0,0xB9DA}, /* ڹ: GB2312 Code: 0xDAB9 ==> Row:58 Col:25 */
    {0x8BE1,0xEEB9}, /* ��: GB2312 Code: 0xB9EE ==> Row:25 Col:78 */
    {0x8BE2,0xAFD1}, /* ѯ: GB2312 Code: 0xD1AF ==> Row:49 Col:15 */
    {0x8BE3,0xE8D2}, /* ��: GB2312 Code: 0xD2E8 ==> Row:50 Col:72 */
    {0x8BE4,0xBADA}, /* ں: GB2312 Code: 0xDABA ==> Row:58 Col:26 */
    {0x8BE5,0xC3B8}, /* ��: GB2312 Code: 0xB8C3 ==> Row:24 Col:35 */
    {0x8BE6,0xEACF}, /* ��: GB2312 Code: 0xCFEA ==> Row:47 Col:74 */
    {0x8BE7,0xEFB2}, /* ��: GB2312 Code: 0xB2EF ==> Row:18 Col:79 */
    {0x8BE8,0xBBDA}, /* ڻ: GB2312 Code: 0xDABB ==> Row:58 Col:27 */
    {0x8BE9,0xBCDA}, /* ڼ: GB2312 Code: 0xDABC ==> Row:58 Col:28 */
    {0x8BEB,0xEBBD}, /* ��: GB2312 Code: 0xBDEB ==> Row:29 Col:75 */
    {0x8BEC,0xDCCE}, /* ��: GB2312 Code: 0xCEDC ==> Row:46 Col:60 */
    {0x8BED,0xEFD3}, /* ��: GB2312 Code: 0xD3EF ==> Row:51 Col:79 */
    {0x8BEE,0xBDDA}, /* ڽ: GB2312 Code: 0xDABD ==> Row:58 Col:29 */
    {0x8BEF,0xF3CE}, /* ��: GB2312 Code: 0xCEF3 ==> Row:46 Col:83 */
    {0x8BF0,0xBEDA}, /* ھ: GB2312 Code: 0xDABE ==> Row:58 Col:30 */
    {0x8BF1,0xD5D3}, /* ��: GB2312 Code: 0xD3D5 ==> Row:51 Col:53 */
    {0x8BF2,0xE5BB}, /* ��: GB2312 Code: 0xBBE5 ==> Row:27 Col:69 */
    {0x8BF3,0xBFDA}, /* ڿ: GB2312 Code: 0xDABF ==> Row:58 Col:31 */
    {0x8BF4,0xB5CB}, /* ˵: GB2312 Code: 0xCBB5 ==> Row:43 Col:21 */
    {0x8BF5,0xD0CB}, /* ��: GB2312 Code: 0xCBD0 ==> Row:43 Col:48 */
    {0x8BF6,0xC0DA}, /* ��: GB2312 Code: 0xDAC0 ==> Row:58 Col:32 */
    {0x8BF7,0xEBC7}, /* ��: GB2312 Code: 0xC7EB ==> Row:39 Col:75 */
    {0x8BF8,0xEED6}, /* ��: GB2312 Code: 0xD6EE ==> Row:54 Col:78 */
    {0x8BF9,0xC1DA}, /* ��: GB2312 Code: 0xDAC1 ==> Row:58 Col:33 */
    {0x8BFA,0xB5C5}, /* ŵ: GB2312 Code: 0xC5B5 ==> Row:37 Col:21 */
    {0x8BFB,0xC1B6}, /* ��: GB2312 Code: 0xB6C1 ==> Row:22 Col:33 */
    {0x8BFC,0xC2DA}, /* ��: GB2312 Code: 0xDAC2 ==> Row:58 Col:34 */
    {0x8BFD,0xCCB7}, /* ��: GB2312 Code: 0xB7CC ==> Row:23 Col:44 */
    {0x8BFE,0xCEBF}, /* ��: GB2312 Code: 0xBFCE ==> Row:31 Col:46 */
    {0x8BFF,0xC3DA}, /* ��: GB2312 Code: 0xDAC3 ==> Row:58 Col:35 */
    {0x8C00,0xC4DA}, /* ��: GB2312 Code: 0xDAC4 ==> Row:58 Col:36 */
    {0x8C01,0xADCB}, /* ˭: GB2312 Code: 0xCBAD ==> Row:43 Col:13 */
    {0x8C02,0xC5DA}, /* ��: GB2312 Code: 0xDAC5 ==> Row:58 Col:37 */
    {0x8C03,0xF7B5}, /* ��: GB2312 Code: 0xB5F7 ==> Row:21 Col:87 */
    {0x8C04,0xC6DA}, /* ��: GB2312 Code: 0xDAC6 ==> Row:58 Col:38 */
    {0x8C05,0xC2C1}, /* ��: GB2312 Code: 0xC1C2 ==> Row:33 Col:34 */
    {0x8C06,0xBBD7}, /* ׻: GB2312 Code: 0xD7BB ==> Row:55 Col:27 */
    {0x8C07,0xC7DA}, /* ��: GB2312 Code: 0xDAC7 ==> Row:58 Col:39 */
    {0x8C08,0xB8CC}, /* ̸: GB2312 Code: 0xCCB8 ==> Row:44 Col:24 */
    {0x8C0A,0xEAD2}, /* ��: GB2312 Code: 0xD2EA ==> Row:50 Col:74 */
    {0x8C0B,0xB1C4}, /* ı: GB2312 Code: 0xC4B1 ==> Row:36 Col:17 */
    {0x8C0C,0xC8DA}, /* ��: GB2312 Code: 0xDAC8 ==> Row:58 Col:40 */
    {0x8C0D,0xFDB5}, /* ��: GB2312 Code: 0xB5FD ==> Row:21 Col:93 */
    {0x8C0E,0xD1BB}, /* ��: GB2312 Code: 0xBBD1 ==> Row:27 Col:49 */
    {0x8C0F,0xC9DA}, /* ��: GB2312 Code: 0xDAC9 ==> Row:58 Col:41 */
    {0x8C10,0xB3D0}, /* г: GB2312 Code: 0xD0B3 ==> Row:48 Col:19 */
    {0x8C11,0xCADA}, /* ��: GB2312 Code: 0xDACA ==> Row:58 Col:42 */
    {0x8C12,0xCBDA}, /* ��: GB2312 Code: 0xDACB ==> Row:58 Col:43 */
    {0x8C13,0xBDCE}, /* ν: GB2312 Code: 0xCEBD ==> Row:46 Col:29 */
    {0x8C14,0xCCDA}, /* ��: GB2312 Code: 0xDACC ==> Row:58 Col:44 */
    {0x8C15,0xCDDA}, /* ��: GB2312 Code: 0xDACD ==> Row:58 Col:45 */
    {0x8C16,0xCEDA}, /* ��: GB2312 Code: 0xDACE ==> Row:58 Col:46 */
    {0x8C17,0xF7B2}, /* ��: GB2312 Code: 0xB2F7 ==> Row:18 Col:87 */
    {0x8C18,0xD1DA}, /* ��: GB2312 Code: 0xDAD1 ==> Row:58 Col:49 */
    {0x8C19,0xCFDA}, /* ��: GB2312 Code: 0xDACF ==> Row:58 Col:47 */
    {0x8C1A,0xE8D1}, /* ��: GB2312 Code: 0xD1E8 ==> Row:49 Col:72 */
    {0x8C1B,0xD0DA}, /* ��: GB2312 Code: 0xDAD0 ==> Row:58 Col:48 */
    {0x8C1C,0xD5C3}, /* ��: GB2312 Code: 0xC3D5 ==> Row:35 Col:53 */
    {0x8C1D,0xD2DA}, /* ��: GB2312 Code: 0xDAD2 ==> Row:58 Col:50 */
    {0x8C1F,0xD3DA}, /* ��: GB2312 Code: 0xDAD3 ==> Row:58 Col:51 */
    {0x8C20,0xD4DA}, /* ��: GB2312 Code: 0xDAD4 ==> Row:58 Col:52 */
    {0x8C21,0xD5DA}, /* ��: GB2312 Code: 0xDAD5 ==> Row:58 Col:53 */
    {0x8C22,0xBBD0}, /* л: GB2312 Code: 0xD0BB ==> Row:48 Col:27 */
    {0x8C23,0xA5D2}, /* ҥ: GB2312 Code: 0xD2A5 ==> Row:50 Col:05 */
    {0x8C24,0xF9B0}, /* ��: GB2312 Code: 0xB0F9 ==> Row:16 Col:89 */
    {0x8C25,0xD6DA}, /* ��: GB2312 Code: 0xDAD6 ==> Row:58 Col:54 */
    {0x8C26,0xABC7}, /* ǫ: GB2312 Code: 0xC7AB ==> Row:39 Col:11 */
    {0x8C27,0xD7DA}, /* ��: GB2312 Code: 0xDAD7 ==> Row:58 Col:55 */
    {0x8C28,0xF7BD}, /* ��: GB2312 Code: 0xBDF7 ==> Row:29 Col:87 */
    {0x8C29,0xA1C3}, /* á: GB2312 Code: 0xC3A1 ==> Row:35 Col:01 */
    {0x8C2A,0xD8DA}, /* ��: GB2312 Code: 0xDAD8 ==> Row:58 Col:56 */
    {0x8C2B,0xD9DA}, /* ��: GB2312 Code: 0xDAD9 ==> Row:58 Col:57 */
    {0x8C2C,0xFDC3}, /* ��: GB2312 Code: 0xC3FD ==> Row:35 Col:93 */
    {0x8C2D,0xB7CC}, /* ̷: GB2312 Code: 0xCCB7 ==> Row:44 Col:23 */
    {0x8C2E,0xDADA}, /* ��: GB2312 Code: 0xDADA ==> Row:58 Col:58 */
    {0x8C2F,0xDBDA}, /* ��: GB2312 Code: 0xDADB ==> Row:58 Col:59 */
    {0x8C30,0xBEC0}, /* ��: GB2312 Code: 0xC0BE ==> Row:32 Col:30 */
    {0x8C31,0xD7C6}, /* ��: GB2312 Code: 0xC6D7 ==> Row:38 Col:55 */
    {0x8C32,0xDCDA}, /* ��: GB2312 Code: 0xDADC ==> Row:58 Col:60 */
    {0x8C33,0xDDDA}, /* ��: GB2312 Code: 0xDADD ==> Row:58 Col:61 */
    {0x8C34,0xB4C7}, /* Ǵ: GB2312 Code: 0xC7B4 ==> Row:39 Col:20 */
    {0x8C35,0xDEDA}, /* ��: GB2312 Code: 0xDADE ==> Row:58 Col:62 */
    {0x8C36,0xDFDA}, /* ��: GB2312 Code: 0xDADF ==> Row:58 Col:63 */
    {0x8C37,0xC8B9}, /* ��: GB2312 Code: 0xB9C8 ==> Row:25 Col:40 */
    {0x8C41,0xEDBB}, /* ��: GB2312 Code: 0xBBED ==> Row:27 Col:77 */
    {0x8C46,0xB9B6}, /* ��: GB2312 Code: 0xB6B9 ==> Row:22 Col:25 */
    {0x8C47,0xF8F4}, /* ��: GB2312 Code: 0xF4F8 ==> Row:84 Col:88 */
    {0x8C49,0xF9F4}, /* ��: GB2312 Code: 0xF4F9 ==> Row:84 Col:89 */
    {0x8C4C,0xE3CD}, /* ��: GB2312 Code: 0xCDE3 ==> Row:45 Col:67 */
    {0x8C55,0xB9F5}, /* ��: GB2312 Code: 0xF5B9 ==> Row:85 Col:25 */
    {0x8C5A,0xE0EB}, /* ��: GB2312 Code: 0xEBE0 ==> Row:75 Col:64 */
    {0x8C61,0xF3CF}, /* ��: GB2312 Code: 0xCFF3 ==> Row:47 Col:83 */
    {0x8C62,0xBFBB}, /* ��: GB2312 Code: 0xBBBF ==> Row:27 Col:31 */
    {0x8C6A,0xC0BA}, /* ��: GB2312 Code: 0xBAC0 ==> Row:26 Col:32 */
    {0x8C6B,0xA5D4}, /* ԥ: GB2312 Code: 0xD4A5 ==> Row:52 Col:05 */
    {0x8C73,0xD9E1}, /* ��: GB2312 Code: 0xE1D9 ==> Row:65 Col:57 */
    {0x8C78,0xF4F5}, /* ��: GB2312 Code: 0xF5F4 ==> Row:85 Col:84 */
    {0x8C79,0xAAB1}, /* ��: GB2312 Code: 0xB1AA ==> Row:17 Col:10 */
    {0x8C7A,0xF2B2}, /* ��: GB2312 Code: 0xB2F2 ==> Row:18 Col:82 */
    {0x8C82,0xF5F5}, /* ��: GB2312 Code: 0xF5F5 ==> Row:85 Col:85 */
    {0x8C85,0xF7F5}, /* ��: GB2312 Code: 0xF5F7 ==> Row:85 Col:87 */
    {0x8C89,0xD1BA}, /* ��: GB2312 Code: 0xBAD1 ==> Row:26 Col:49 */
    {0x8C8A,0xF6F5}, /* ��: GB2312 Code: 0xF5F6 ==> Row:85 Col:86 */
    {0x8C8C,0xB2C3}, /* ò: GB2312 Code: 0xC3B2 ==> Row:35 Col:18 */
    {0x8C94,0xF9F5}, /* ��: GB2312 Code: 0xF5F9 ==> Row:85 Col:89 */
    {0x8C98,0xF8F5}, /* ��: GB2312 Code: 0xF5F8 ==> Row:85 Col:88 */
    {0x8D1D,0xB4B1}, /* ��: GB2312 Code: 0xB1B4 ==> Row:17 Col:20 */
    {0x8D1E,0xEAD5}, /* ��: GB2312 Code: 0xD5EA ==> Row:53 Col:74 */
    {0x8D1F,0xBAB8}, /* ��: GB2312 Code: 0xB8BA ==> Row:24 Col:26 */
    {0x8D21,0xB1B9}, /* ��: GB2312 Code: 0xB9B1 ==> Row:25 Col:17 */
    {0x8D22,0xC6B2}, /* ��: GB2312 Code: 0xB2C6 ==> Row:18 Col:38 */
    {0x8D23,0xF0D4}, /* ��: GB2312 Code: 0xD4F0 ==> Row:52 Col:80 */
    {0x8D24,0xCDCF}, /* ��: GB2312 Code: 0xCFCD ==> Row:47 Col:45 */
    {0x8D25,0xDCB0}, /* ��: GB2312 Code: 0xB0DC ==> Row:16 Col:60 */
    {0x8D26,0xCBD5}, /* ��: GB2312 Code: 0xD5CB ==> Row:53 Col:43 */
    {0x8D27,0xF5BB}, /* ��: GB2312 Code: 0xBBF5 ==> Row:27 Col:85 */
    {0x8D28,0xCAD6}, /* ��: GB2312 Code: 0xD6CA ==> Row:54 Col:42 */
    {0x8D29,0xB7B7}, /* ��: GB2312 Code: 0xB7B7 ==> Row:23 Col:23 */
    {0x8D2A,0xB0CC}, /* ̰: GB2312 Code: 0xCCB0 ==> Row:44 Col:16 */
    {0x8D2B,0xB6C6}, /* ƶ: GB2312 Code: 0xC6B6 ==> Row:38 Col:22 */
    {0x8D2C,0xE1B1}, /* ��: GB2312 Code: 0xB1E1 ==> Row:17 Col:65 */
    {0x8D2D,0xBAB9}, /* ��: GB2312 Code: 0xB9BA ==> Row:25 Col:26 */
    {0x8D2E,0xFCD6}, /* ��: GB2312 Code: 0xD6FC ==> Row:54 Col:92 */
    {0x8D2F,0xE1B9}, /* ��: GB2312 Code: 0xB9E1 ==> Row:25 Col:65 */
    {0x8D30,0xA1B7}, /* ��: GB2312 Code: 0xB7A1 ==> Row:23 Col:01 */
    {0x8D31,0xFABC}, /* ��: GB2312 Code: 0xBCFA ==> Row:28 Col:90 */
    {0x8D32,0xDAEA}, /* ��: GB2312 Code: 0xEADA ==> Row:74 Col:58 */
    {0x8D33,0xDBEA}, /* ��: GB2312 Code: 0xEADB ==> Row:74 Col:59 */
    {0x8D34,0xF9CC}, /* ��: GB2312 Code: 0xCCF9 ==> Row:44 Col:89 */
    {0x8D35,0xF3B9}, /* ��: GB2312 Code: 0xB9F3 ==> Row:25 Col:83 */
    {0x8D36,0xDCEA}, /* ��: GB2312 Code: 0xEADC ==> Row:74 Col:60 */
    {0x8D37,0xFBB4}, /* ��: GB2312 Code: 0xB4FB ==> Row:20 Col:91 */
    {0x8D38,0xB3C3}, /* ó: GB2312 Code: 0xC3B3 ==> Row:35 Col:19 */
    {0x8D39,0xD1B7}, /* ��: GB2312 Code: 0xB7D1 ==> Row:23 Col:49 */
    {0x8D3A,0xD8BA}, /* ��: GB2312 Code: 0xBAD8 ==> Row:26 Col:56 */
    {0x8D3B,0xDDEA}, /* ��: GB2312 Code: 0xEADD ==> Row:74 Col:61 */
    {0x8D3C,0xF4D4}, /* ��: GB2312 Code: 0xD4F4 ==> Row:52 Col:84 */
    {0x8D3D,0xDEEA}, /* ��: GB2312 Code: 0xEADE ==> Row:74 Col:62 */
    {0x8D3E,0xD6BC}, /* ��: GB2312 Code: 0xBCD6 ==> Row:28 Col:54 */
    {0x8D3F,0xDFBB}, /* ��: GB2312 Code: 0xBBDF ==> Row:27 Col:63 */
    {0x8D40,0xDFEA}, /* ��: GB2312 Code: 0xEADF ==> Row:74 Col:63 */
    {0x8D41,0xDEC1}, /* ��: GB2312 Code: 0xC1DE ==> Row:33 Col:62 */
    {0x8D42,0xB8C2}, /* ¸: GB2312 Code: 0xC2B8 ==> Row:34 Col:24 */
    {0x8D43,0xDFD4}, /* ��: GB2312 Code: 0xD4DF ==> Row:52 Col:63 */
    {0x8D44,0xCAD7}, /* ��: GB2312 Code: 0xD7CA ==> Row:55 Col:42 */
    {0x8D45,0xE0EA}, /* ��: GB2312 Code: 0xEAE0 ==> Row:74 Col:64 */
    {0x8D46,0xE1EA}, /* ��: GB2312 Code: 0xEAE1 ==> Row:74 Col:65 */
    {0x8D47,0xE4EA}, /* ��: GB2312 Code: 0xEAE4 ==> Row:74 Col:68 */
    {0x8D48,0xE2EA}, /* ��: GB2312 Code: 0xEAE2 ==> Row:74 Col:66 */
    {0x8D49,0xE3EA}, /* ��: GB2312 Code: 0xEAE3 ==> Row:74 Col:67 */
    {0x8D4A,0xDEC9}, /* ��: GB2312 Code: 0xC9DE ==> Row:41 Col:62 */
    {0x8D4B,0xB3B8}, /* ��: GB2312 Code: 0xB8B3 ==> Row:24 Col:19 */
    {0x8D4C,0xC4B6}, /* ��: GB2312 Code: 0xB6C4 ==> Row:22 Col:36 */
    {0x8D4D,0xE5EA}, /* ��: GB2312 Code: 0xEAE5 ==> Row:74 Col:69 */
    {0x8D4E,0xEACA}, /* ��: GB2312 Code: 0xCAEA ==> Row:42 Col:74 */
    {0x8D4F,0xCDC9}, /* ��: GB2312 Code: 0xC9CD ==> Row:41 Col:45 */
    {0x8D50,0xCDB4}, /* ��: GB2312 Code: 0xB4CD ==> Row:20 Col:45 */
    {0x8D53,0xD9E2}, /* ��: GB2312 Code: 0xE2D9 ==> Row:66 Col:57 */
    {0x8D54,0xE2C5}, /* ��: GB2312 Code: 0xC5E2 ==> Row:37 Col:66 */
    {0x8D55,0xE6EA}, /* ��: GB2312 Code: 0xEAE6 ==> Row:74 Col:70 */
    {0x8D56,0xB5C0}, /* ��: GB2312 Code: 0xC0B5 ==> Row:32 Col:21 */
    {0x8D58,0xB8D7}, /* ׸: GB2312 Code: 0xD7B8 ==> Row:55 Col:24 */
    {0x8D59,0xE7EA}, /* ��: GB2312 Code: 0xEAE7 ==> Row:74 Col:71 */
    {0x8D5A,0xACD7}, /* ׬: GB2312 Code: 0xD7AC ==> Row:55 Col:12 */
    {0x8D5B,0xFCC8}, /* ��: GB2312 Code: 0xC8FC ==> Row:40 Col:92 */
    {0x8D5C,0xD3D8}, /* ��: GB2312 Code: 0xD8D3 ==> Row:56 Col:51 */
    {0x8D5D,0xCDD8}, /* ��: GB2312 Code: 0xD8CD ==> Row:56 Col:45 */
    {0x8D5E,0xDED4}, /* ��: GB2312 Code: 0xD4DE ==> Row:52 Col:62 */
    {0x8D60,0xF9D4}, /* ��: GB2312 Code: 0xD4F9 ==> Row:52 Col:89 */
    {0x8D61,0xC4C9}, /* ��: GB2312 Code: 0xC9C4 ==> Row:41 Col:36 */
    {0x8D62,0xAED3}, /* Ӯ: GB2312 Code: 0xD3AE ==> Row:51 Col:14 */
    {0x8D63,0xD3B8}, /* ��: GB2312 Code: 0xB8D3 ==> Row:24 Col:51 */
    {0x8D64,0xE0B3}, /* ��: GB2312 Code: 0xB3E0 ==> Row:19 Col:64 */
    {0x8D66,0xE2C9}, /* ��: GB2312 Code: 0xC9E2 ==> Row:41 Col:66 */
    {0x8D67,0xF6F4}, /* ��: GB2312 Code: 0xF4F6 ==> Row:84 Col:86 */
    {0x8D6B,0xD5BA}, /* ��: GB2312 Code: 0xBAD5 ==> Row:26 Col:53 */
    {0x8D6D,0xF7F4}, /* ��: GB2312 Code: 0xF4F7 ==> Row:84 Col:87 */
    {0x8D70,0xDFD7}, /* ��: GB2312 Code: 0xD7DF ==> Row:55 Col:63 */
    {0x8D73,0xF1F4}, /* ��: GB2312 Code: 0xF4F1 ==> Row:84 Col:81 */
    {0x8D74,0xB0B8}, /* ��: GB2312 Code: 0xB8B0 ==> Row:24 Col:16 */
    {0x8D75,0xD4D5}, /* ��: GB2312 Code: 0xD5D4 ==> Row:53 Col:52 */
    {0x8D76,0xCFB8}, /* ��: GB2312 Code: 0xB8CF ==> Row:24 Col:47 */
    {0x8D77,0xF0C6}, /* ��: GB2312 Code: 0xC6F0 ==> Row:38 Col:80 */
    {0x8D81,0xC3B3}, /* ��: GB2312 Code: 0xB3C3 ==> Row:19 Col:35 */
    {0x8D84,0xF2F4}, /* ��: GB2312 Code: 0xF4F2 ==> Row:84 Col:82 */
    {0x8D85,0xACB3}, /* ��: GB2312 Code: 0xB3AC ==> Row:19 Col:12 */
    {0x8D8A,0xBDD4}, /* Խ: GB2312 Code: 0xD4BD ==> Row:52 Col:29 */
    {0x8D8B,0xF7C7}, /* ��: GB2312 Code: 0xC7F7 ==> Row:39 Col:87 */
    {0x8D91,0xF4F4}, /* ��: GB2312 Code: 0xF4F4 ==> Row:84 Col:84 */
    {0x8D94,0xF3F4}, /* ��: GB2312 Code: 0xF4F3 ==> Row:84 Col:83 */
    {0x8D9F,0xCBCC}, /* ��: GB2312 Code: 0xCCCB ==> Row:44 Col:43 */
    {0x8DA3,0xA4C8}, /* Ȥ: GB2312 Code: 0xC8A4 ==> Row:40 Col:04 */
    {0x8DB1,0xF5F4}, /* ��: GB2312 Code: 0xF4F5 ==> Row:84 Col:85 */
    {0x8DB3,0xE3D7}, /* ��: GB2312 Code: 0xD7E3 ==> Row:55 Col:67 */
    {0x8DB4,0xBFC5}, /* ſ: GB2312 Code: 0xC5BF ==> Row:37 Col:31 */
    {0x8DB5,0xC0F5}, /* ��: GB2312 Code: 0xF5C0 ==> Row:85 Col:32 */
    {0x8DB8,0xBBF5}, /* ��: GB2312 Code: 0xF5BB ==> Row:85 Col:27 */
    {0x8DBA,0xC3F5}, /* ��: GB2312 Code: 0xF5C3 ==> Row:85 Col:35 */
    {0x8DBC,0xC2F5}, /* ��: GB2312 Code: 0xF5C2 ==> Row:85 Col:34 */
    {0x8DBE,0xBAD6}, /* ֺ: GB2312 Code: 0xD6BA ==> Row:54 Col:26 */
    {0x8DBF,0xC1F5}, /* ��: GB2312 Code: 0xF5C1 ==> Row:85 Col:33 */
    {0x8DC3,0xBED4}, /* Ծ: GB2312 Code: 0xD4BE ==> Row:52 Col:30 */
    {0x8DC4,0xC4F5}, /* ��: GB2312 Code: 0xF5C4 ==> Row:85 Col:36 */
    {0x8DC6,0xCCF5}, /* ��: GB2312 Code: 0xF5CC ==> Row:85 Col:44 */
    {0x8DCB,0xCFB0}, /* ��: GB2312 Code: 0xB0CF ==> Row:16 Col:47 */
    {0x8DCC,0xF8B5}, /* ��: GB2312 Code: 0xB5F8 ==> Row:21 Col:88 */
    {0x8DCE,0xC9F5}, /* ��: GB2312 Code: 0xF5C9 ==> Row:85 Col:41 */
    {0x8DCF,0xCAF5}, /* ��: GB2312 Code: 0xF5CA ==> Row:85 Col:42 */
    {0x8DD1,0xDCC5}, /* ��: GB2312 Code: 0xC5DC ==> Row:37 Col:60 */
    {0x8DD6,0xC5F5}, /* ��: GB2312 Code: 0xF5C5 ==> Row:85 Col:37 */
    {0x8DD7,0xC6F5}, /* ��: GB2312 Code: 0xF5C6 ==> Row:85 Col:38 */
    {0x8DDA,0xC7F5}, /* ��: GB2312 Code: 0xF5C7 ==> Row:85 Col:39 */
    {0x8DDB,0xCBF5}, /* ��: GB2312 Code: 0xF5CB ==> Row:85 Col:43 */
    {0x8DDD,0xE0BE}, /* ��: GB2312 Code: 0xBEE0 ==> Row:30 Col:64 */
    {0x8DDE,0xC8F5}, /* ��: GB2312 Code: 0xF5C8 ==> Row:85 Col:40 */
    {0x8DDF,0xFAB8}, /* ��: GB2312 Code: 0xB8FA ==> Row:24 Col:90 */
    {0x8DE3,0xD0F5}, /* ��: GB2312 Code: 0xF5D0 ==> Row:85 Col:48 */
    {0x8DE4,0xD3F5}, /* ��: GB2312 Code: 0xF5D3 ==> Row:85 Col:51 */
    {0x8DE8,0xE7BF}, /* ��: GB2312 Code: 0xBFE7 ==> Row:31 Col:71 */
    {0x8DEA,0xF2B9}, /* ��: GB2312 Code: 0xB9F2 ==> Row:25 Col:82 */
    {0x8DEB,0xBCF5}, /* ��: GB2312 Code: 0xF5BC ==> Row:85 Col:28 */
    {0x8DEC,0xCDF5}, /* ��: GB2312 Code: 0xF5CD ==> Row:85 Col:45 */
    {0x8DEF,0xB7C2}, /* ·: GB2312 Code: 0xC2B7 ==> Row:34 Col:23 */
    {0x8DF3,0xF8CC}, /* ��: GB2312 Code: 0xCCF8 ==> Row:44 Col:88 */
    {0x8DF5,0xF9BC}, /* ��: GB2312 Code: 0xBCF9 ==> Row:28 Col:89 */
    {0x8DF7,0xCEF5}, /* ��: GB2312 Code: 0xF5CE ==> Row:85 Col:46 */
    {0x8DF8,0xCFF5}, /* ��: GB2312 Code: 0xF5CF ==> Row:85 Col:47 */
    {0x8DF9,0xD1F5}, /* ��: GB2312 Code: 0xF5D1 ==> Row:85 Col:49 */
    {0x8DFA,0xE5B6}, /* ��: GB2312 Code: 0xB6E5 ==> Row:22 Col:69 */
    {0x8DFB,0xD2F5}, /* ��: GB2312 Code: 0xF5D2 ==> Row:85 Col:50 */
    {0x8DFD,0xD5F5}, /* ��: GB2312 Code: 0xF5D5 ==> Row:85 Col:53 */
    {0x8E05,0xBDF5}, /* ��: GB2312 Code: 0xF5BD ==> Row:85 Col:29 */
    {0x8E09,0xD4F5}, /* ��: GB2312 Code: 0xF5D4 ==> Row:85 Col:52 */
    {0x8E0A,0xBBD3}, /* ӻ: GB2312 Code: 0xD3BB ==> Row:51 Col:27 */
    {0x8E0C,0xECB3}, /* ��: GB2312 Code: 0xB3EC ==> Row:19 Col:76 */
    {0x8E0F,0xA4CC}, /* ̤: GB2312 Code: 0xCCA4 ==> Row:44 Col:04 */
    {0x8E14,0xD6F5}, /* ��: GB2312 Code: 0xF5D6 ==> Row:85 Col:54 */
    {0x8E1D,0xD7F5}, /* ��: GB2312 Code: 0xF5D7 ==> Row:85 Col:55 */
    {0x8E1E,0xE1BE}, /* ��: GB2312 Code: 0xBEE1 ==> Row:30 Col:65 */
    {0x8E1F,0xD8F5}, /* ��: GB2312 Code: 0xF5D8 ==> Row:85 Col:56 */
    {0x8E22,0xDFCC}, /* ��: GB2312 Code: 0xCCDF ==> Row:44 Col:63 */
    {0x8E23,0xDBF5}, /* ��: GB2312 Code: 0xF5DB ==> Row:85 Col:59 */
    {0x8E29,0xC8B2}, /* ��: GB2312 Code: 0xB2C8 ==> Row:18 Col:40 */
    {0x8E2A,0xD9D7}, /* ��: GB2312 Code: 0xD7D9 ==> Row:55 Col:57 */
    {0x8E2C,0xD9F5}, /* ��: GB2312 Code: 0xF5D9 ==> Row:85 Col:57 */
    {0x8E2E,0xDAF5}, /* ��: GB2312 Code: 0xF5DA ==> Row:85 Col:58 */
    {0x8E2F,0xDCF5}, /* ��: GB2312 Code: 0xF5DC ==> Row:85 Col:60 */
    {0x8E31,0xE2F5}, /* ��: GB2312 Code: 0xF5E2 ==> Row:85 Col:66 */
    {0x8E35,0xE0F5}, /* ��: GB2312 Code: 0xF5E0 ==> Row:85 Col:64 */
    {0x8E39,0xDFF5}, /* ��: GB2312 Code: 0xF5DF ==> Row:85 Col:63 */
    {0x8E3A,0xDDF5}, /* ��: GB2312 Code: 0xF5DD ==> Row:85 Col:61 */
    {0x8E3D,0xE1F5}, /* ��: GB2312 Code: 0xF5E1 ==> Row:85 Col:65 */
    {0x8E40,0xDEF5}, /* ��: GB2312 Code: 0xF5DE ==> Row:85 Col:62 */
    {0x8E41,0xE4F5}, /* ��: GB2312 Code: 0xF5E4 ==> Row:85 Col:68 */
    {0x8E42,0xE5F5}, /* ��: GB2312 Code: 0xF5E5 ==> Row:85 Col:69 */
    {0x8E44,0xE3CC}, /* ��: GB2312 Code: 0xCCE3 ==> Row:44 Col:67 */
    {0x8E47,0xBFE5}, /* �: GB2312 Code: 0xE5BF ==> Row:69 Col:31 */
    {0x8E48,0xB8B5}, /* ��: GB2312 Code: 0xB5B8 ==> Row:21 Col:24 */
    {0x8E49,0xE3F5}, /* ��: GB2312 Code: 0xF5E3 ==> Row:85 Col:67 */
    {0x8E4A,0xE8F5}, /* ��: GB2312 Code: 0xF5E8 ==> Row:85 Col:72 */
    {0x8E4B,0xA3CC}, /* ̣: GB2312 Code: 0xCCA3 ==> Row:44 Col:03 */
    {0x8E51,0xE6F5}, /* ��: GB2312 Code: 0xF5E6 ==> Row:85 Col:70 */
    {0x8E52,0xE7F5}, /* ��: GB2312 Code: 0xF5E7 ==> Row:85 Col:71 */
    {0x8E59,0xBEF5}, /* ��: GB2312 Code: 0xF5BE ==> Row:85 Col:30 */
    {0x8E66,0xC4B1}, /* ��: GB2312 Code: 0xB1C4 ==> Row:17 Col:36 */
    {0x8E69,0xBFF5}, /* ��: GB2312 Code: 0xF5BF ==> Row:85 Col:31 */
    {0x8E6C,0xC5B5}, /* ��: GB2312 Code: 0xB5C5 ==> Row:21 Col:37 */
    {0x8E6D,0xE4B2}, /* ��: GB2312 Code: 0xB2E4 ==> Row:18 Col:68 */
    {0x8E6F,0xECF5}, /* ��: GB2312 Code: 0xF5EC ==> Row:85 Col:76 */
    {0x8E70,0xE9F5}, /* ��: GB2312 Code: 0xF5E9 ==> Row:85 Col:73 */
    {0x8E72,0xD7B6}, /* ��: GB2312 Code: 0xB6D7 ==> Row:22 Col:55 */
    {0x8E74,0xEDF5}, /* ��: GB2312 Code: 0xF5ED ==> Row:85 Col:77 */
    {0x8E76,0xEAF5}, /* ��: GB2312 Code: 0xF5EA ==> Row:85 Col:74 */
    {0x8E7C,0xEBF5}, /* ��: GB2312 Code: 0xF5EB ==> Row:85 Col:75 */
    {0x8E7F,0xDAB4}, /* ��: GB2312 Code: 0xB4DA ==> Row:20 Col:58 */
    {0x8E81,0xEAD4}, /* ��: GB2312 Code: 0xD4EA ==> Row:52 Col:74 */
    {0x8E85,0xEEF5}, /* ��: GB2312 Code: 0xF5EE ==> Row:85 Col:78 */
    {0x8E87,0xF9B3}, /* ��: GB2312 Code: 0xB3F9 ==> Row:19 Col:89 */
    {0x8E8F,0xEFF5}, /* ��: GB2312 Code: 0xF5EF ==> Row:85 Col:79 */
    {0x8E90,0xF1F5}, /* ��: GB2312 Code: 0xF5F1 ==> Row:85 Col:81 */
    {0x8E94,0xF0F5}, /* ��: GB2312 Code: 0xF5F0 ==> Row:85 Col:80 */
    {0x8E9C,0xF2F5}, /* ��: GB2312 Code: 0xF5F2 ==> Row:85 Col:82 */
    {0x8E9E,0xF3F5}, /* ��: GB2312 Code: 0xF5F3 ==> Row:85 Col:83 */
    {0x8EAB,0xEDC9}, /* ��: GB2312 Code: 0xC9ED ==> Row:41 Col:77 */
    {0x8EAC,0xAAB9}, /* ��: GB2312 Code: 0xB9AA ==> Row:25 Col:10 */
    {0x8EAF,0xFBC7}, /* ��: GB2312 Code: 0xC7FB ==> Row:39 Col:91 */
    {0x8EB2,0xE3B6}, /* ��: GB2312 Code: 0xB6E3 ==> Row:22 Col:67 */
    {0x8EBA,0xC9CC}, /* ��: GB2312 Code: 0xCCC9 ==> Row:44 Col:41 */
    {0x8ECE,0xA6EA}, /* �: GB2312 Code: 0xEAA6 ==> Row:74 Col:06 */
    {0x8F66,0xB5B3}, /* ��: GB2312 Code: 0xB3B5 ==> Row:19 Col:21 */
    {0x8F67,0xFED4}, /* ��: GB2312 Code: 0xD4FE ==> Row:52 Col:94 */
    {0x8F68,0xECB9}, /* ��: GB2312 Code: 0xB9EC ==> Row:25 Col:76 */
    {0x8F69,0xF9D0}, /* ��: GB2312 Code: 0xD0F9 ==> Row:48 Col:89 */
    {0x8F6B,0xEDE9}, /* ��: GB2312 Code: 0xE9ED ==> Row:73 Col:77 */
    {0x8F6C,0xAAD7}, /* ת: GB2312 Code: 0xD7AA ==> Row:55 Col:10 */
    {0x8F6D,0xEEE9}, /* ��: GB2312 Code: 0xE9EE ==> Row:73 Col:78 */
    {0x8F6E,0xD6C2}, /* ��: GB2312 Code: 0xC2D6 ==> Row:34 Col:54 */
    {0x8F6F,0xEDC8}, /* ��: GB2312 Code: 0xC8ED ==> Row:40 Col:77 */
    {0x8F70,0xE4BA}, /* ��: GB2312 Code: 0xBAE4 ==> Row:26 Col:68 */
    {0x8F71,0xEFE9}, /* ��: GB2312 Code: 0xE9EF ==> Row:73 Col:79 */
    {0x8F72,0xF0E9}, /* ��: GB2312 Code: 0xE9F0 ==> Row:73 Col:80 */
    {0x8F73,0xF1E9}, /* ��: GB2312 Code: 0xE9F1 ==> Row:73 Col:81 */
    {0x8F74,0xE1D6}, /* ��: GB2312 Code: 0xD6E1 ==> Row:54 Col:65 */
    {0x8F75,0xF2E9}, /* ��: GB2312 Code: 0xE9F2 ==> Row:73 Col:82 */
    {0x8F76,0xF3E9}, /* ��: GB2312 Code: 0xE9F3 ==> Row:73 Col:83 */
    {0x8F77,0xF5E9}, /* ��: GB2312 Code: 0xE9F5 ==> Row:73 Col:85 */
    {0x8F78,0xF4E9}, /* ��: GB2312 Code: 0xE9F4 ==> Row:73 Col:84 */
    {0x8F79,0xF6E9}, /* ��: GB2312 Code: 0xE9F6 ==> Row:73 Col:86 */
    {0x8F7A,0xF7E9}, /* ��: GB2312 Code: 0xE9F7 ==> Row:73 Col:87 */
    {0x8F7B,0xE1C7}, /* ��: GB2312 Code: 0xC7E1 ==> Row:39 Col:65 */
    {0x8F7C,0xF8E9}, /* ��: GB2312 Code: 0xE9F8 ==> Row:73 Col:88 */
    {0x8F7D,0xD8D4}, /* ��: GB2312 Code: 0xD4D8 ==> Row:52 Col:56 */
    {0x8F7E,0xF9E9}, /* ��: GB2312 Code: 0xE9F9 ==> Row:73 Col:89 */
    {0x8F7F,0xCEBD}, /* ��: GB2312 Code: 0xBDCE ==> Row:29 Col:46 */
    {0x8F81,0xFAE9}, /* ��: GB2312 Code: 0xE9FA ==> Row:73 Col:90 */
    {0x8F82,0xFBE9}, /* ��: GB2312 Code: 0xE9FB ==> Row:73 Col:91 */
    {0x8F83,0xCFBD}, /* ��: GB2312 Code: 0xBDCF ==> Row:29 Col:47 */
    {0x8F84,0xFCE9}, /* ��: GB2312 Code: 0xE9FC ==> Row:73 Col:92 */
    {0x8F85,0xA8B8}, /* ��: GB2312 Code: 0xB8A8 ==> Row:24 Col:08 */
    {0x8F86,0xBEC1}, /* ��: GB2312 Code: 0xC1BE ==> Row:33 Col:30 */
    {0x8F87,0xFDE9}, /* ��: GB2312 Code: 0xE9FD ==> Row:73 Col:93 */
    {0x8F88,0xB2B1}, /* ��: GB2312 Code: 0xB1B2 ==> Row:17 Col:18 */
    {0x8F89,0xD4BB}, /* ��: GB2312 Code: 0xBBD4 ==> Row:27 Col:52 */
    {0x8F8A,0xF5B9}, /* ��: GB2312 Code: 0xB9F5 ==> Row:25 Col:85 */
    {0x8F8B,0xFEE9}, /* ��: GB2312 Code: 0xE9FE ==> Row:73 Col:94 */
    {0x8F8D,0xA1EA}, /* �: GB2312 Code: 0xEAA1 ==> Row:74 Col:01 */
    {0x8F8E,0xA2EA}, /* �: GB2312 Code: 0xEAA2 ==> Row:74 Col:02 */
    {0x8F8F,0xA3EA}, /* �: GB2312 Code: 0xEAA3 ==> Row:74 Col:03 */
    {0x8F90,0xF8B7}, /* ��: GB2312 Code: 0xB7F8 ==> Row:23 Col:88 */
    {0x8F91,0xADBC}, /* ��: GB2312 Code: 0xBCAD ==> Row:28 Col:13 */
    {0x8F93,0xE4CA}, /* ��: GB2312 Code: 0xCAE4 ==> Row:42 Col:68 */
    {0x8F94,0xCEE0}, /* ��: GB2312 Code: 0xE0CE ==> Row:64 Col:46 */
    {0x8F95,0xAFD4}, /* ԯ: GB2312 Code: 0xD4AF ==> Row:52 Col:15 */
    {0x8F96,0xBDCF}, /* Ͻ: GB2312 Code: 0xCFBD ==> Row:47 Col:29 */
    {0x8F97,0xB7D5}, /* շ: GB2312 Code: 0xD5B7 ==> Row:53 Col:23 */
    {0x8F98,0xA4EA}, /* �: GB2312 Code: 0xEAA4 ==> Row:74 Col:04 */
    {0x8F99,0xDED5}, /* ��: GB2312 Code: 0xD5DE ==> Row:53 Col:62 */
    {0x8F9A,0xA5EA}, /* �: GB2312 Code: 0xEAA5 ==> Row:74 Col:05 */
    {0x8F9B,0xC1D0}, /* ��: GB2312 Code: 0xD0C1 ==> Row:48 Col:33 */
    {0x8F9C,0xBCB9}, /* ��: GB2312 Code: 0xB9BC ==> Row:25 Col:28 */
    {0x8F9E,0xC7B4}, /* ��: GB2312 Code: 0xB4C7 ==> Row:20 Col:39 */
    {0x8F9F,0xD9B1}, /* ��: GB2312 Code: 0xB1D9 ==> Row:17 Col:57 */
    {0x8FA3,0xB1C0}, /* ��: GB2312 Code: 0xC0B1 ==> Row:32 Col:17 */
    {0x8FA8,0xE6B1}, /* ��: GB2312 Code: 0xB1E6 ==> Row:17 Col:70 */
    {0x8FA9,0xE7B1}, /* ��: GB2312 Code: 0xB1E7 ==> Row:17 Col:71 */
    {0x8FAB,0xE8B1}, /* ��: GB2312 Code: 0xB1E8 ==> Row:17 Col:72 */
    {0x8FB0,0xBDB3}, /* ��: GB2312 Code: 0xB3BD ==> Row:19 Col:29 */
    {0x8FB1,0xE8C8}, /* ��: GB2312 Code: 0xC8E8 ==> Row:40 Col:72 */
    {0x8FB6,0xC1E5}, /* ��: GB2312 Code: 0xE5C1 ==> Row:69 Col:33 */
    {0x8FB9,0xDFB1}, /* ��: GB2312 Code: 0xB1DF ==> Row:17 Col:63 */
    {0x8FBD,0xC9C1}, /* ��: GB2312 Code: 0xC1C9 ==> Row:33 Col:41 */
    {0x8FBE,0xEFB4}, /* ��: GB2312 Code: 0xB4EF ==> Row:20 Col:79 */
    {0x8FC1,0xA8C7}, /* Ǩ: GB2312 Code: 0xC7A8 ==> Row:39 Col:08 */
    {0x8FC2,0xD8D3}, /* ��: GB2312 Code: 0xD3D8 ==> Row:51 Col:56 */
    {0x8FC4,0xF9C6}, /* ��: GB2312 Code: 0xC6F9 ==> Row:38 Col:89 */
    {0x8FC5,0xB8D1}, /* Ѹ: GB2312 Code: 0xD1B8 ==> Row:49 Col:24 */
    {0x8FC7,0xFDB9}, /* ��: GB2312 Code: 0xB9FD ==> Row:25 Col:93 */
    {0x8FC8,0xF5C2}, /* ��: GB2312 Code: 0xC2F5 ==> Row:34 Col:85 */
    {0x8FCE,0xADD3}, /* ӭ: GB2312 Code: 0xD3AD ==> Row:51 Col:13 */
    {0x8FD0,0xCBD4}, /* ��: GB2312 Code: 0xD4CB ==> Row:52 Col:43 */
    {0x8FD1,0xFCBD}, /* ��: GB2312 Code: 0xBDFC ==> Row:29 Col:92 */
    {0x8FD3,0xC2E5}, /* ��: GB2312 Code: 0xE5C2 ==> Row:69 Col:34 */
    {0x8FD4,0xB5B7}, /* ��: GB2312 Code: 0xB7B5 ==> Row:23 Col:21 */
    {0x8FD5,0xC3E5}, /* ��: GB2312 Code: 0xE5C3 ==> Row:69 Col:35 */
    {0x8FD8,0xB9BB}, /* ��: GB2312 Code: 0xBBB9 ==> Row:27 Col:25 */
    {0x8FD9,0xE2D5}, /* ��: GB2312 Code: 0xD5E2 ==> Row:53 Col:66 */
    {0x8FDB,0xF8BD}, /* ��: GB2312 Code: 0xBDF8 ==> Row:29 Col:88 */
    {0x8FDC,0xB6D4}, /* Զ: GB2312 Code: 0xD4B6 ==> Row:52 Col:22 */
    {0x8FDD,0xA5CE}, /* Υ: GB2312 Code: 0xCEA5 ==> Row:46 Col:05 */
    {0x8FDE,0xACC1}, /* ��: GB2312 Code: 0xC1AC ==> Row:33 Col:12 */
    {0x8FDF,0xD9B3}, /* ��: GB2312 Code: 0xB3D9 ==> Row:19 Col:57 */
    {0x8FE2,0xF6CC}, /* ��: GB2312 Code: 0xCCF6 ==> Row:44 Col:86 */
    {0x8FE4,0xC6E5}, /* ��: GB2312 Code: 0xE5C6 ==> Row:69 Col:38 */
    {0x8FE5,0xC4E5}, /* ��: GB2312 Code: 0xE5C4 ==> Row:69 Col:36 */
    {0x8FE6,0xC8E5}, /* ��: GB2312 Code: 0xE5C8 ==> Row:69 Col:40 */
    {0x8FE8,0xCAE5}, /* ��: GB2312 Code: 0xE5CA ==> Row:69 Col:42 */
    {0x8FE9,0xC7E5}, /* ��: GB2312 Code: 0xE5C7 ==> Row:69 Col:39 */
    {0x8FEA,0xCFB5}, /* ��: GB2312 Code: 0xB5CF ==> Row:21 Col:47 */
    {0x8FEB,0xC8C6}, /* ��: GB2312 Code: 0xC6C8 ==> Row:38 Col:40 */
    {0x8FED,0xFCB5}, /* ��: GB2312 Code: 0xB5FC ==> Row:21 Col:92 */
    {0x8FEE,0xC5E5}, /* ��: GB2312 Code: 0xE5C5 ==> Row:69 Col:37 */
    {0x8FF0,0xF6CA}, /* ��: GB2312 Code: 0xCAF6 ==> Row:42 Col:86 */
    {0x8FF3,0xC9E5}, /* ��: GB2312 Code: 0xE5C9 ==> Row:69 Col:41 */
    {0x8FF7,0xD4C3}, /* ��: GB2312 Code: 0xC3D4 ==> Row:35 Col:52 */
    {0x8FF8,0xC5B1}, /* ��: GB2312 Code: 0xB1C5 ==> Row:17 Col:37 */
    {0x8FF9,0xA3BC}, /* ��: GB2312 Code: 0xBCA3 ==> Row:28 Col:03 */
    {0x8FFD,0xB7D7}, /* ׷: GB2312 Code: 0xD7B7 ==> Row:55 Col:23 */
    {0x9000,0xCBCD}, /* ��: GB2312 Code: 0xCDCB ==> Row:45 Col:43 */
    {0x9001,0xCDCB}, /* ��: GB2312 Code: 0xCBCD ==> Row:43 Col:45 */
    {0x9002,0xCACA}, /* ��: GB2312 Code: 0xCACA ==> Row:42 Col:42 */
    {0x9003,0xD3CC}, /* ��: GB2312 Code: 0xCCD3 ==> Row:44 Col:51 */
    {0x9004,0xCCE5}, /* ��: GB2312 Code: 0xE5CC ==> Row:69 Col:44 */
    {0x9005,0xCBE5}, /* ��: GB2312 Code: 0xE5CB ==> Row:69 Col:43 */
    {0x9006,0xE6C4}, /* ��: GB2312 Code: 0xC4E6 ==> Row:36 Col:70 */
    {0x9009,0xA1D1}, /* ѡ: GB2312 Code: 0xD1A1 ==> Row:49 Col:01 */
    {0x900A,0xB7D1}, /* ѷ: GB2312 Code: 0xD1B7 ==> Row:49 Col:23 */
    {0x900B,0xCDE5}, /* ��: GB2312 Code: 0xE5CD ==> Row:69 Col:45 */
    {0x900D,0xD0E5}, /* ��: GB2312 Code: 0xE5D0 ==> Row:69 Col:48 */
    {0x900F,0xB8CD}, /* ͸: GB2312 Code: 0xCDB8 ==> Row:45 Col:24 */
    {0x9010,0xF0D6}, /* ��: GB2312 Code: 0xD6F0 ==> Row:54 Col:80 */
    {0x9011,0xCFE5}, /* ��: GB2312 Code: 0xE5CF ==> Row:69 Col:47 */
    {0x9012,0xDDB5}, /* ��: GB2312 Code: 0xB5DD ==> Row:21 Col:61 */
    {0x9014,0xBECD}, /* ;: GB2312 Code: 0xCDBE ==> Row:45 Col:30 */
    {0x9016,0xD1E5}, /* ��: GB2312 Code: 0xE5D1 ==> Row:69 Col:49 */
    {0x9017,0xBAB6}, /* ��: GB2312 Code: 0xB6BA ==> Row:22 Col:26 */
    {0x901A,0xA8CD}, /* ͨ: GB2312 Code: 0xCDA8 ==> Row:45 Col:08 */
    {0x901B,0xE4B9}, /* ��: GB2312 Code: 0xB9E4 ==> Row:25 Col:68 */
    {0x901D,0xC5CA}, /* ��: GB2312 Code: 0xCAC5 ==> Row:42 Col:37 */
    {0x901E,0xD1B3}, /* ��: GB2312 Code: 0xB3D1 ==> Row:19 Col:49 */
    {0x901F,0xD9CB}, /* ��: GB2312 Code: 0xCBD9 ==> Row:43 Col:57 */
    {0x9020,0xECD4}, /* ��: GB2312 Code: 0xD4EC ==> Row:52 Col:76 */
    {0x9021,0xD2E5}, /* ��: GB2312 Code: 0xE5D2 ==> Row:69 Col:50 */
    {0x9022,0xEAB7}, /* ��: GB2312 Code: 0xB7EA ==> Row:23 Col:74 */
    {0x9026,0xCEE5}, /* ��: GB2312 Code: 0xE5CE ==> Row:69 Col:46 */
    {0x902D,0xD5E5}, /* ��: GB2312 Code: 0xE5D5 ==> Row:69 Col:53 */
    {0x902E,0xFEB4}, /* ��: GB2312 Code: 0xB4FE ==> Row:20 Col:94 */
    {0x902F,0xD6E5}, /* ��: GB2312 Code: 0xE5D6 ==> Row:69 Col:54 */
    {0x9035,0xD3E5}, /* ��: GB2312 Code: 0xE5D3 ==> Row:69 Col:51 */
    {0x9036,0xD4E5}, /* ��: GB2312 Code: 0xE5D4 ==> Row:69 Col:52 */
    {0x9038,0xDDD2}, /* ��: GB2312 Code: 0xD2DD ==> Row:50 Col:61 */
    {0x903B,0xDFC2}, /* ��: GB2312 Code: 0xC2DF ==> Row:34 Col:63 */
    {0x903C,0xC6B1}, /* ��: GB2312 Code: 0xB1C6 ==> Row:17 Col:38 */
    {0x903E,0xE2D3}, /* ��: GB2312 Code: 0xD3E2 ==> Row:51 Col:66 */
    {0x9041,0xDDB6}, /* ��: GB2312 Code: 0xB6DD ==> Row:22 Col:61 */
    {0x9042,0xECCB}, /* ��: GB2312 Code: 0xCBEC ==> Row:43 Col:76 */
    {0x9044,0xD7E5}, /* ��: GB2312 Code: 0xE5D7 ==> Row:69 Col:55 */
    {0x9047,0xF6D3}, /* ��: GB2312 Code: 0xD3F6 ==> Row:51 Col:86 */
    {0x904D,0xE9B1}, /* ��: GB2312 Code: 0xB1E9 ==> Row:17 Col:73 */
    {0x904F,0xF4B6}, /* ��: GB2312 Code: 0xB6F4 ==> Row:22 Col:84 */
    {0x9050,0xDAE5}, /* ��: GB2312 Code: 0xE5DA ==> Row:69 Col:58 */
    {0x9051,0xD8E5}, /* ��: GB2312 Code: 0xE5D8 ==> Row:69 Col:56 */
    {0x9052,0xD9E5}, /* ��: GB2312 Code: 0xE5D9 ==> Row:69 Col:57 */
    {0x9053,0xC0B5}, /* ��: GB2312 Code: 0xB5C0 ==> Row:21 Col:32 */
    {0x9057,0xC5D2}, /* ��: GB2312 Code: 0xD2C5 ==> Row:50 Col:37 */
    {0x9058,0xDCE5}, /* ��: GB2312 Code: 0xE5DC ==> Row:69 Col:60 */
    {0x905B,0xDEE5}, /* ��: GB2312 Code: 0xE5DE ==> Row:69 Col:62 */
    {0x9062,0xDDE5}, /* ��: GB2312 Code: 0xE5DD ==> Row:69 Col:61 */
    {0x9063,0xB2C7}, /* ǲ: GB2312 Code: 0xC7B2 ==> Row:39 Col:18 */
    {0x9065,0xA3D2}, /* ң: GB2312 Code: 0xD2A3 ==> Row:50 Col:03 */
    {0x9068,0xDBE5}, /* ��: GB2312 Code: 0xE5DB ==> Row:69 Col:59 */
    {0x906D,0xE2D4}, /* ��: GB2312 Code: 0xD4E2 ==> Row:52 Col:66 */
    {0x906E,0xDAD5}, /* ��: GB2312 Code: 0xD5DA ==> Row:53 Col:58 */
    {0x9074,0xE0E5}, /* ��: GB2312 Code: 0xE5E0 ==> Row:69 Col:64 */
    {0x9075,0xF1D7}, /* ��: GB2312 Code: 0xD7F1 ==> Row:55 Col:81 */
    {0x907D,0xE1E5}, /* ��: GB2312 Code: 0xE5E1 ==> Row:69 Col:65 */
    {0x907F,0xDCB1}, /* ��: GB2312 Code: 0xB1DC ==> Row:17 Col:60 */
    {0x9080,0xFBD1}, /* ��: GB2312 Code: 0xD1FB ==> Row:49 Col:91 */
    {0x9082,0xE2E5}, /* ��: GB2312 Code: 0xE5E2 ==> Row:69 Col:66 */
    {0x9083,0xE4E5}, /* ��: GB2312 Code: 0xE5E4 ==> Row:69 Col:68 */
    {0x9088,0xE3E5}, /* ��: GB2312 Code: 0xE5E3 ==> Row:69 Col:67 */
    {0x908B,0xE5E5}, /* ��: GB2312 Code: 0xE5E5 ==> Row:69 Col:69 */
    {0x9091,0xD8D2}, /* ��: GB2312 Code: 0xD2D8 ==> Row:50 Col:56 */
    {0x9093,0xCBB5}, /* ��: GB2312 Code: 0xB5CB ==> Row:21 Col:43 */
    {0x9095,0xDFE7}, /* ��: GB2312 Code: 0xE7DF ==> Row:71 Col:63 */
    {0x9097,0xF5DA}, /* ��: GB2312 Code: 0xDAF5 ==> Row:58 Col:85 */
    {0x9099,0xF8DA}, /* ��: GB2312 Code: 0xDAF8 ==> Row:58 Col:88 */
    {0x909B,0xF6DA}, /* ��: GB2312 Code: 0xDAF6 ==> Row:58 Col:86 */
    {0x909D,0xF7DA}, /* ��: GB2312 Code: 0xDAF7 ==> Row:58 Col:87 */
    {0x90A1,0xFADA}, /* ��: GB2312 Code: 0xDAFA ==> Row:58 Col:90 */
    {0x90A2,0xCFD0}, /* ��: GB2312 Code: 0xD0CF ==> Row:48 Col:47 */
    {0x90A3,0xC7C4}, /* ��: GB2312 Code: 0xC4C7 ==> Row:36 Col:39 */
    {0x90A6,0xEEB0}, /* ��: GB2312 Code: 0xB0EE ==> Row:16 Col:78 */
    {0x90AA,0xB0D0}, /* а: GB2312 Code: 0xD0B0 ==> Row:48 Col:16 */
    {0x90AC,0xF9DA}, /* ��: GB2312 Code: 0xDAF9 ==> Row:58 Col:89 */
    {0x90AE,0xCAD3}, /* ��: GB2312 Code: 0xD3CA ==> Row:51 Col:42 */
    {0x90AF,0xAABA}, /* ��: GB2312 Code: 0xBAAA ==> Row:26 Col:10 */
    {0x90B0,0xA2DB}, /* ۢ: GB2312 Code: 0xDBA2 ==> Row:59 Col:02 */
    {0x90B1,0xF1C7}, /* ��: GB2312 Code: 0xC7F1 ==> Row:39 Col:81 */
    {0x90B3,0xFCDA}, /* ��: GB2312 Code: 0xDAFC ==> Row:58 Col:92 */
    {0x90B4,0xFBDA}, /* ��: GB2312 Code: 0xDAFB ==> Row:58 Col:91 */
    {0x90B5,0xDBC9}, /* ��: GB2312 Code: 0xC9DB ==> Row:41 Col:59 */
    {0x90B6,0xFDDA}, /* ��: GB2312 Code: 0xDAFD ==> Row:58 Col:93 */
    {0x90B8,0xA1DB}, /* ۡ: GB2312 Code: 0xDBA1 ==> Row:59 Col:01 */
    {0x90B9,0xDED7}, /* ��: GB2312 Code: 0xD7DE ==> Row:55 Col:62 */
    {0x90BA,0xFEDA}, /* ��: GB2312 Code: 0xDAFE ==> Row:58 Col:94 */
    {0x90BB,0xDAC1}, /* ��: GB2312 Code: 0xC1DA ==> Row:33 Col:58 */
    {0x90BE,0xA5DB}, /* ۥ: GB2312 Code: 0xDBA5 ==> Row:59 Col:05 */
    {0x90C1,0xF4D3}, /* ��: GB2312 Code: 0xD3F4 ==> Row:51 Col:84 */
    {0x90C4,0xA7DB}, /* ۧ: GB2312 Code: 0xDBA7 ==> Row:59 Col:07 */
    {0x90C5,0xA4DB}, /* ۤ: GB2312 Code: 0xDBA4 ==> Row:59 Col:04 */
    {0x90C7,0xA8DB}, /* ۨ: GB2312 Code: 0xDBA8 ==> Row:59 Col:08 */
    {0x90CA,0xBCBD}, /* ��: GB2312 Code: 0xBDBC ==> Row:29 Col:28 */
    {0x90CE,0xC9C0}, /* ��: GB2312 Code: 0xC0C9 ==> Row:32 Col:41 */
    {0x90CF,0xA3DB}, /* ۣ: GB2312 Code: 0xDBA3 ==> Row:59 Col:03 */
    {0x90D0,0xA6DB}, /* ۦ: GB2312 Code: 0xDBA6 ==> Row:59 Col:06 */
    {0x90D1,0xA3D6}, /* ֣: GB2312 Code: 0xD6A3 ==> Row:54 Col:03 */
    {0x90D3,0xA9DB}, /* ۩: GB2312 Code: 0xDBA9 ==> Row:59 Col:09 */
    {0x90D7,0xADDB}, /* ۭ: GB2312 Code: 0xDBAD ==> Row:59 Col:13 */
    {0x90DB,0xAEDB}, /* ۮ: GB2312 Code: 0xDBAE ==> Row:59 Col:14 */
    {0x90DC,0xACDB}, /* ۬: GB2312 Code: 0xDBAC ==> Row:59 Col:12 */
    {0x90DD,0xC2BA}, /* ��: GB2312 Code: 0xBAC2 ==> Row:26 Col:34 */
    {0x90E1,0xA4BF}, /* ��: GB2312 Code: 0xBFA4 ==> Row:31 Col:04 */
    {0x90E2,0xABDB}, /* ۫: GB2312 Code: 0xDBAB ==> Row:59 Col:11 */
    {0x90E6,0xAADB}, /* ۪: GB2312 Code: 0xDBAA ==> Row:59 Col:10 */
    {0x90E7,0xC7D4}, /* ��: GB2312 Code: 0xD4C7 ==> Row:52 Col:39 */
    {0x90E8,0xBFB2}, /* ��: GB2312 Code: 0xB2BF ==> Row:18 Col:31 */
    {0x90EB,0xAFDB}, /* ۯ: GB2312 Code: 0xDBAF ==> Row:59 Col:15 */
    {0x90ED,0xF9B9}, /* ��: GB2312 Code: 0xB9F9 ==> Row:25 Col:89 */
    {0x90EF,0xB0DB}, /* ۰: GB2312 Code: 0xDBB0 ==> Row:59 Col:16 */
    {0x90F4,0xBBB3}, /* ��: GB2312 Code: 0xB3BB ==> Row:19 Col:27 */
    {0x90F8,0xA6B5}, /* ��: GB2312 Code: 0xB5A6 ==> Row:21 Col:06 */
    {0x90FD,0xBCB6}, /* ��: GB2312 Code: 0xB6BC ==> Row:22 Col:28 */
    {0x90FE,0xB1DB}, /* ۱: GB2312 Code: 0xDBB1 ==> Row:59 Col:17 */
    {0x9102,0xF5B6}, /* ��: GB2312 Code: 0xB6F5 ==> Row:22 Col:85 */
    {0x9104,0xB2DB}, /* ۲: GB2312 Code: 0xDBB2 ==> Row:59 Col:18 */
    {0x9119,0xC9B1}, /* ��: GB2312 Code: 0xB1C9 ==> Row:17 Col:41 */
    {0x911E,0xB4DB}, /* ۴: GB2312 Code: 0xDBB4 ==> Row:59 Col:20 */
    {0x9122,0xB3DB}, /* ۳: GB2312 Code: 0xDBB3 ==> Row:59 Col:19 */
    {0x9123,0xB5DB}, /* ۵: GB2312 Code: 0xDBB5 ==> Row:59 Col:21 */
    {0x912F,0xB7DB}, /* ۷: GB2312 Code: 0xDBB7 ==> Row:59 Col:23 */
    {0x9131,0xB6DB}, /* ۶: GB2312 Code: 0xDBB6 ==> Row:59 Col:22 */
    {0x9139,0xB8DB}, /* ۸: GB2312 Code: 0xDBB8 ==> Row:59 Col:24 */
    {0x9143,0xB9DB}, /* ۹: GB2312 Code: 0xDBB9 ==> Row:59 Col:25 */
    {0x9146,0xBADB}, /* ۺ: GB2312 Code: 0xDBBA ==> Row:59 Col:26 */
    {0x9149,0xCFD3}, /* ��: GB2312 Code: 0xD3CF ==> Row:51 Col:47 */
    {0x914A,0xFAF4}, /* ��: GB2312 Code: 0xF4FA ==> Row:84 Col:90 */
    {0x914B,0xF5C7}, /* ��: GB2312 Code: 0xC7F5 ==> Row:39 Col:85 */
    {0x914C,0xC3D7}, /* ��: GB2312 Code: 0xD7C3 ==> Row:55 Col:35 */
    {0x914D,0xE4C5}, /* ��: GB2312 Code: 0xC5E4 ==> Row:37 Col:68 */
    {0x914E,0xFCF4}, /* ��: GB2312 Code: 0xF4FC ==> Row:84 Col:92 */
    {0x914F,0xFDF4}, /* ��: GB2312 Code: 0xF4FD ==> Row:84 Col:93 */
    {0x9150,0xFBF4}, /* ��: GB2312 Code: 0xF4FB ==> Row:84 Col:91 */
    {0x9152,0xC6BE}, /* ��: GB2312 Code: 0xBEC6 ==> Row:30 Col:38 */
    {0x9157,0xEFD0}, /* ��: GB2312 Code: 0xD0EF ==> Row:48 Col:79 */
    {0x915A,0xD3B7}, /* ��: GB2312 Code: 0xB7D3 ==> Row:23 Col:51 */
    {0x915D,0xCDD4}, /* ��: GB2312 Code: 0xD4CD ==> Row:52 Col:45 */
    {0x915E,0xAACC}, /* ̪: GB2312 Code: 0xCCAA ==> Row:44 Col:10 */
    {0x9161,0xA2F5}, /* ��: GB2312 Code: 0xF5A2 ==> Row:85 Col:02 */
    {0x9162,0xA1F5}, /* ��: GB2312 Code: 0xF5A1 ==> Row:85 Col:01 */
    {0x9163,0xA8BA}, /* ��: GB2312 Code: 0xBAA8 ==> Row:26 Col:08 */
    {0x9164,0xFEF4}, /* ��: GB2312 Code: 0xF4FE ==> Row:84 Col:94 */
    {0x9165,0xD6CB}, /* ��: GB2312 Code: 0xCBD6 ==> Row:43 Col:54 */
    {0x9169,0xA4F5}, /* ��: GB2312 Code: 0xF5A4 ==> Row:85 Col:04 */
    {0x916A,0xD2C0}, /* ��: GB2312 Code: 0xC0D2 ==> Row:32 Col:50 */
    {0x916C,0xEAB3}, /* ��: GB2312 Code: 0xB3EA ==> Row:19 Col:74 */
    {0x916E,0xAACD}, /* ͪ: GB2312 Code: 0xCDAA ==> Row:45 Col:10 */
    {0x916F,0xA5F5}, /* ��: GB2312 Code: 0xF5A5 ==> Row:85 Col:05 */
    {0x9170,0xA3F5}, /* ��: GB2312 Code: 0xF5A3 ==> Row:85 Col:03 */
    {0x9171,0xB4BD}, /* ��: GB2312 Code: 0xBDB4 ==> Row:29 Col:20 */
    {0x9172,0xA8F5}, /* ��: GB2312 Code: 0xF5A8 ==> Row:85 Col:08 */
    {0x9174,0xA9F5}, /* ��: GB2312 Code: 0xF5A9 ==> Row:85 Col:09 */
    {0x9175,0xCDBD}, /* ��: GB2312 Code: 0xBDCD ==> Row:29 Col:45 */
    {0x9176,0xB8C3}, /* ø: GB2312 Code: 0xC3B8 ==> Row:35 Col:24 */
    {0x9177,0xE1BF}, /* ��: GB2312 Code: 0xBFE1 ==> Row:31 Col:65 */
    {0x9178,0xE1CB}, /* ��: GB2312 Code: 0xCBE1 ==> Row:43 Col:65 */
    {0x9179,0xAAF5}, /* ��: GB2312 Code: 0xF5AA ==> Row:85 Col:10 */
    {0x917D,0xA6F5}, /* ��: GB2312 Code: 0xF5A6 ==> Row:85 Col:06 */
    {0x917E,0xA7F5}, /* ��: GB2312 Code: 0xF5A7 ==> Row:85 Col:07 */
    {0x917F,0xF0C4}, /* ��: GB2312 Code: 0xC4F0 ==> Row:36 Col:80 */
    {0x9185,0xACF5}, /* ��: GB2312 Code: 0xF5AC ==> Row:85 Col:12 */
    {0x9187,0xBCB4}, /* ��: GB2312 Code: 0xB4BC ==> Row:20 Col:28 */
    {0x9189,0xEDD7}, /* ��: GB2312 Code: 0xD7ED ==> Row:55 Col:77 */
    {0x918B,0xD7B4}, /* ��: GB2312 Code: 0xB4D7 ==> Row:20 Col:55 */
    {0x918C,0xABF5}, /* ��: GB2312 Code: 0xF5AB ==> Row:85 Col:11 */
    {0x918D,0xAEF5}, /* ��: GB2312 Code: 0xF5AE ==> Row:85 Col:14 */
    {0x9190,0xADF5}, /* ��: GB2312 Code: 0xF5AD ==> Row:85 Col:13 */
    {0x9191,0xAFF5}, /* ��: GB2312 Code: 0xF5AF ==> Row:85 Col:15 */
    {0x9192,0xD1D0}, /* ��: GB2312 Code: 0xD0D1 ==> Row:48 Col:49 */
    {0x919A,0xD1C3}, /* ��: GB2312 Code: 0xC3D1 ==> Row:35 Col:49 */
    {0x919B,0xA9C8}, /* ȩ: GB2312 Code: 0xC8A9 ==> Row:40 Col:09 */
    {0x91A2,0xB0F5}, /* ��: GB2312 Code: 0xF5B0 ==> Row:85 Col:16 */
    {0x91A3,0xB1F5}, /* ��: GB2312 Code: 0xF5B1 ==> Row:85 Col:17 */
    {0x91AA,0xB2F5}, /* ��: GB2312 Code: 0xF5B2 ==> Row:85 Col:18 */
    {0x91AD,0xB3F5}, /* ��: GB2312 Code: 0xF5B3 ==> Row:85 Col:19 */
    {0x91AE,0xB4F5}, /* ��: GB2312 Code: 0xF5B4 ==> Row:85 Col:20 */
    {0x91AF,0xB5F5}, /* ��: GB2312 Code: 0xF5B5 ==> Row:85 Col:21 */
    {0x91B4,0xB7F5}, /* ��: GB2312 Code: 0xF5B7 ==> Row:85 Col:23 */
    {0x91B5,0xB6F5}, /* ��: GB2312 Code: 0xF5B6 ==> Row:85 Col:22 */
    {0x91BA,0xB8F5}, /* ��: GB2312 Code: 0xF5B8 ==> Row:85 Col:24 */
    {0x91C7,0xC9B2}, /* ��: GB2312 Code: 0xB2C9 ==> Row:18 Col:41 */
    {0x91C9,0xD4D3}, /* ��: GB2312 Code: 0xD3D4 ==> Row:51 Col:52 */
    {0x91CA,0xCDCA}, /* ��: GB2312 Code: 0xCACD ==> Row:42 Col:45 */
    {0x91CC,0xEFC0}, /* ��: GB2312 Code: 0xC0EF ==> Row:32 Col:79 */
    {0x91CD,0xD8D6}, /* ��: GB2312 Code: 0xD6D8 ==> Row:54 Col:56 */
    {0x91CE,0xB0D2}, /* Ұ: GB2312 Code: 0xD2B0 ==> Row:50 Col:16 */
    {0x91CF,0xBFC1}, /* ��: GB2312 Code: 0xC1BF ==> Row:33 Col:31 */
    {0x91D1,0xF0BD}, /* ��: GB2312 Code: 0xBDF0 ==> Row:29 Col:80 */
    {0x91DC,0xAAB8}, /* ��: GB2312 Code: 0xB8AA ==> Row:24 Col:10 */
    {0x9274,0xF8BC}, /* ��: GB2312 Code: 0xBCF8 ==> Row:28 Col:88 */
    {0x928E,0xC6F6}, /* ��: GB2312 Code: 0xF6C6 ==> Row:86 Col:38 */
    {0x92AE,0xC7F6}, /* ��: GB2312 Code: 0xF6C7 ==> Row:86 Col:39 */
    {0x92C8,0xC8F6}, /* ��: GB2312 Code: 0xF6C8 ==> Row:86 Col:40 */
    {0x933E,0xC9F6}, /* ��: GB2312 Code: 0xF6C9 ==> Row:86 Col:41 */
    {0x936A,0xCAF6}, /* ��: GB2312 Code: 0xF6CA ==> Row:86 Col:42 */
    {0x938F,0xCCF6}, /* ��: GB2312 Code: 0xF6CC ==> Row:86 Col:44 */
    {0x93CA,0xCBF6}, /* ��: GB2312 Code: 0xF6CB ==> Row:86 Col:43 */
    {0x93D6,0xE9F7}, /* ��: GB2312 Code: 0xF7E9 ==> Row:87 Col:73 */
    {0x943E,0xCDF6}, /* ��: GB2312 Code: 0xF6CD ==> Row:86 Col:45 */
    {0x946B,0xCEF6}, /* ��: GB2312 Code: 0xF6CE ==> Row:86 Col:46 */
    {0x9485,0xC4EE}, /* ��: GB2312 Code: 0xEEC4 ==> Row:78 Col:36 */
    {0x9486,0xC5EE}, /* ��: GB2312 Code: 0xEEC5 ==> Row:78 Col:37 */
    {0x9487,0xC6EE}, /* ��: GB2312 Code: 0xEEC6 ==> Row:78 Col:38 */
    {0x9488,0xEBD5}, /* ��: GB2312 Code: 0xD5EB ==> Row:53 Col:75 */
    {0x9489,0xA4B6}, /* ��: GB2312 Code: 0xB6A4 ==> Row:22 Col:04 */
    {0x948A,0xC8EE}, /* ��: GB2312 Code: 0xEEC8 ==> Row:78 Col:40 */
    {0x948B,0xC7EE}, /* ��: GB2312 Code: 0xEEC7 ==> Row:78 Col:39 */
    {0x948C,0xC9EE}, /* ��: GB2312 Code: 0xEEC9 ==> Row:78 Col:41 */
    {0x948D,0xCAEE}, /* ��: GB2312 Code: 0xEECA ==> Row:78 Col:42 */
    {0x948E,0xA5C7}, /* ǥ: GB2312 Code: 0xC7A5 ==> Row:39 Col:05 */
    {0x948F,0xCBEE}, /* ��: GB2312 Code: 0xEECB ==> Row:78 Col:43 */
    {0x9490,0xCCEE}, /* ��: GB2312 Code: 0xEECC ==> Row:78 Col:44 */
    {0x9492,0xB0B7}, /* ��: GB2312 Code: 0xB7B0 ==> Row:23 Col:16 */
    {0x9493,0xF6B5}, /* ��: GB2312 Code: 0xB5F6 ==> Row:21 Col:86 */
    {0x9494,0xCDEE}, /* ��: GB2312 Code: 0xEECD ==> Row:78 Col:45 */
    {0x9495,0xCFEE}, /* ��: GB2312 Code: 0xEECF ==> Row:78 Col:47 */
    {0x9497,0xCEEE}, /* ��: GB2312 Code: 0xEECE ==> Row:78 Col:46 */
    {0x9499,0xC6B8}, /* ��: GB2312 Code: 0xB8C6 ==> Row:24 Col:38 */
    {0x949A,0xD0EE}, /* ��: GB2312 Code: 0xEED0 ==> Row:78 Col:48 */
    {0x949B,0xD1EE}, /* ��: GB2312 Code: 0xEED1 ==> Row:78 Col:49 */
    {0x949C,0xD2EE}, /* ��: GB2312 Code: 0xEED2 ==> Row:78 Col:50 */
    {0x949D,0xDBB6}, /* ��: GB2312 Code: 0xB6DB ==> Row:22 Col:59 */
    {0x949E,0xAEB3}, /* ��: GB2312 Code: 0xB3AE ==> Row:19 Col:14 */
    {0x949F,0xD3D6}, /* ��: GB2312 Code: 0xD6D3 ==> Row:54 Col:51 */
    {0x94A0,0xC6C4}, /* ��: GB2312 Code: 0xC4C6 ==> Row:36 Col:38 */
    {0x94A1,0xB5B1}, /* ��: GB2312 Code: 0xB1B5 ==> Row:17 Col:21 */
    {0x94A2,0xD6B8}, /* ��: GB2312 Code: 0xB8D6 ==> Row:24 Col:54 */
    {0x94A3,0xD3EE}, /* ��: GB2312 Code: 0xEED3 ==> Row:78 Col:51 */
    {0x94A4,0xD4EE}, /* ��: GB2312 Code: 0xEED4 ==> Row:78 Col:52 */
    {0x94A5,0xBFD4}, /* Կ: GB2312 Code: 0xD4BF ==> Row:52 Col:31 */
    {0x94A6,0xD5C7}, /* ��: GB2312 Code: 0xC7D5 ==> Row:39 Col:53 */
    {0x94A7,0xFBBE}, /* ��: GB2312 Code: 0xBEFB ==> Row:30 Col:91 */
    {0x94A8,0xD9CE}, /* ��: GB2312 Code: 0xCED9 ==> Row:46 Col:57 */
    {0x94A9,0xB3B9}, /* ��: GB2312 Code: 0xB9B3 ==> Row:25 Col:19 */
    {0x94AA,0xD6EE}, /* ��: GB2312 Code: 0xEED6 ==> Row:78 Col:54 */
    {0x94AB,0xD5EE}, /* ��: GB2312 Code: 0xEED5 ==> Row:78 Col:53 */
    {0x94AC,0xD8EE}, /* ��: GB2312 Code: 0xEED8 ==> Row:78 Col:56 */
    {0x94AD,0xD7EE}, /* ��: GB2312 Code: 0xEED7 ==> Row:78 Col:55 */
    {0x94AE,0xA5C5}, /* ť: GB2312 Code: 0xC5A5 ==> Row:37 Col:05 */
    {0x94AF,0xD9EE}, /* ��: GB2312 Code: 0xEED9 ==> Row:78 Col:57 */
    {0x94B0,0xDAEE}, /* ��: GB2312 Code: 0xEEDA ==> Row:78 Col:58 */
    {0x94B1,0xAEC7}, /* Ǯ: GB2312 Code: 0xC7AE ==> Row:39 Col:14 */
    {0x94B2,0xDBEE}, /* ��: GB2312 Code: 0xEEDB ==> Row:78 Col:59 */
    {0x94B3,0xAFC7}, /* ǯ: GB2312 Code: 0xC7AF ==> Row:39 Col:15 */
    {0x94B4,0xDCEE}, /* ��: GB2312 Code: 0xEEDC ==> Row:78 Col:60 */
    {0x94B5,0xA7B2}, /* ��: GB2312 Code: 0xB2A7 ==> Row:18 Col:07 */
    {0x94B6,0xDDEE}, /* ��: GB2312 Code: 0xEEDD ==> Row:78 Col:61 */
    {0x94B7,0xDEEE}, /* ��: GB2312 Code: 0xEEDE ==> Row:78 Col:62 */
    {0x94B8,0xDFEE}, /* ��: GB2312 Code: 0xEEDF ==> Row:78 Col:63 */
    {0x94B9,0xE0EE}, /* ��: GB2312 Code: 0xEEE0 ==> Row:78 Col:64 */
    {0x94BA,0xE1EE}, /* ��: GB2312 Code: 0xEEE1 ==> Row:78 Col:65 */
    {0x94BB,0xEAD7}, /* ��: GB2312 Code: 0xD7EA ==> Row:55 Col:74 */
    {0x94BC,0xE2EE}, /* ��: GB2312 Code: 0xEEE2 ==> Row:78 Col:66 */
    {0x94BD,0xE3EE}, /* ��: GB2312 Code: 0xEEE3 ==> Row:78 Col:67 */
    {0x94BE,0xD8BC}, /* ��: GB2312 Code: 0xBCD8 ==> Row:28 Col:56 */
    {0x94BF,0xE4EE}, /* ��: GB2312 Code: 0xEEE4 ==> Row:78 Col:68 */
    {0x94C0,0xCBD3}, /* ��: GB2312 Code: 0xD3CB ==> Row:51 Col:43 */
    {0x94C1,0xFACC}, /* ��: GB2312 Code: 0xCCFA ==> Row:44 Col:90 */
    {0x94C2,0xACB2}, /* ��: GB2312 Code: 0xB2AC ==> Row:18 Col:12 */
    {0x94C3,0xE5C1}, /* ��: GB2312 Code: 0xC1E5 ==> Row:33 Col:69 */
    {0x94C4,0xE5EE}, /* ��: GB2312 Code: 0xEEE5 ==> Row:78 Col:69 */
    {0x94C5,0xA6C7}, /* Ǧ: GB2312 Code: 0xC7A6 ==> Row:39 Col:06 */
    {0x94C6,0xADC3}, /* í: GB2312 Code: 0xC3AD ==> Row:35 Col:13 */
    {0x94C8,0xE6EE}, /* ��: GB2312 Code: 0xEEE6 ==> Row:78 Col:70 */
    {0x94C9,0xE7EE}, /* ��: GB2312 Code: 0xEEE7 ==> Row:78 Col:71 */
    {0x94CA,0xE8EE}, /* ��: GB2312 Code: 0xEEE8 ==> Row:78 Col:72 */
    {0x94CB,0xE9EE}, /* ��: GB2312 Code: 0xEEE9 ==> Row:78 Col:73 */
    {0x94CC,0xEAEE}, /* ��: GB2312 Code: 0xEEEA ==> Row:78 Col:74 */
    {0x94CD,0xEBEE}, /* ��: GB2312 Code: 0xEEEB ==> Row:78 Col:75 */
    {0x94CE,0xECEE}, /* ��: GB2312 Code: 0xEEEC ==> Row:78 Col:76 */
    {0x94D0,0xEDEE}, /* ��: GB2312 Code: 0xEEED ==> Row:78 Col:77 */
    {0x94D1,0xEEEE}, /* ��: GB2312 Code: 0xEEEE ==> Row:78 Col:78 */
    {0x94D2,0xEFEE}, /* ��: GB2312 Code: 0xEEEF ==> Row:78 Col:79 */
    {0x94D5,0xF0EE}, /* ��: GB2312 Code: 0xEEF0 ==> Row:78 Col:80 */
    {0x94D6,0xF1EE}, /* ��: GB2312 Code: 0xEEF1 ==> Row:78 Col:81 */
    {0x94D7,0xF2EE}, /* ��: GB2312 Code: 0xEEF2 ==> Row:78 Col:82 */
    {0x94D8,0xF4EE}, /* ��: GB2312 Code: 0xEEF4 ==> Row:78 Col:84 */
    {0x94D9,0xF3EE}, /* ��: GB2312 Code: 0xEEF3 ==> Row:78 Col:83 */
    {0x94DB,0xF5EE}, /* ��: GB2312 Code: 0xEEF5 ==> Row:78 Col:85 */
    {0x94DC,0xADCD}, /* ͭ: GB2312 Code: 0xCDAD ==> Row:45 Col:13 */
    {0x94DD,0xC1C2}, /* ��: GB2312 Code: 0xC2C1 ==> Row:34 Col:33 */
    {0x94DE,0xF6EE}, /* ��: GB2312 Code: 0xEEF6 ==> Row:78 Col:86 */
    {0x94DF,0xF7EE}, /* ��: GB2312 Code: 0xEEF7 ==> Row:78 Col:87 */
    {0x94E0,0xF8EE}, /* ��: GB2312 Code: 0xEEF8 ==> Row:78 Col:88 */
    {0x94E1,0xA1D5}, /* ա: GB2312 Code: 0xD5A1 ==> Row:53 Col:01 */
    {0x94E2,0xF9EE}, /* ��: GB2312 Code: 0xEEF9 ==> Row:78 Col:89 */
    {0x94E3,0xB3CF}, /* ϳ: GB2312 Code: 0xCFB3 ==> Row:47 Col:19 */
    {0x94E4,0xFAEE}, /* ��: GB2312 Code: 0xEEFA ==> Row:78 Col:90 */
    {0x94E5,0xFBEE}, /* ��: GB2312 Code: 0xEEFB ==> Row:78 Col:91 */
    {0x94E7,0xFCEE}, /* ��: GB2312 Code: 0xEEFC ==> Row:78 Col:92 */
    {0x94E8,0xFDEE}, /* ��: GB2312 Code: 0xEEFD ==> Row:78 Col:93 */
    {0x94E9,0xA1EF}, /* �: GB2312 Code: 0xEFA1 ==> Row:79 Col:01 */
    {0x94EA,0xFEEE}, /* ��: GB2312 Code: 0xEEFE ==> Row:78 Col:94 */
    {0x94EB,0xA2EF}, /* �: GB2312 Code: 0xEFA2 ==> Row:79 Col:02 */
    {0x94EC,0xF5B8}, /* ��: GB2312 Code: 0xB8F5 ==> Row:24 Col:85 */
    {0x94ED,0xFAC3}, /* ��: GB2312 Code: 0xC3FA ==> Row:35 Col:90 */
    {0x94EE,0xA3EF}, /* �: GB2312 Code: 0xEFA3 ==> Row:79 Col:03 */
    {0x94EF,0xA4EF}, /* �: GB2312 Code: 0xEFA4 ==> Row:79 Col:04 */
    {0x94F0,0xC2BD}, /* ��: GB2312 Code: 0xBDC2 ==> Row:29 Col:34 */
    {0x94F1,0xBFD2}, /* ҿ: GB2312 Code: 0xD2BF ==> Row:50 Col:31 */
    {0x94F2,0xF9B2}, /* ��: GB2312 Code: 0xB2F9 ==> Row:18 Col:89 */
    {0x94F3,0xA5EF}, /* �: GB2312 Code: 0xEFA5 ==> Row:79 Col:05 */
    {0x94F4,0xA6EF}, /* �: GB2312 Code: 0xEFA6 ==> Row:79 Col:06 */
    {0x94F5,0xA7EF}, /* �: GB2312 Code: 0xEFA7 ==> Row:79 Col:07 */
    {0x94F6,0xF8D2}, /* ��: GB2312 Code: 0xD2F8 ==> Row:50 Col:88 */
    {0x94F7,0xA8EF}, /* �: GB2312 Code: 0xEFA8 ==> Row:79 Col:08 */
    {0x94F8,0xFDD6}, /* ��: GB2312 Code: 0xD6FD ==> Row:54 Col:93 */
    {0x94F9,0xA9EF}, /* �: GB2312 Code: 0xEFA9 ==> Row:79 Col:09 */
    {0x94FA,0xCCC6}, /* ��: GB2312 Code: 0xC6CC ==> Row:38 Col:44 */
    {0x94FC,0xAAEF}, /* �: GB2312 Code: 0xEFAA ==> Row:79 Col:10 */
    {0x94FD,0xABEF}, /* �: GB2312 Code: 0xEFAB ==> Row:79 Col:11 */
    {0x94FE,0xB4C1}, /* ��: GB2312 Code: 0xC1B4 ==> Row:33 Col:20 */
    {0x94FF,0xACEF}, /* �: GB2312 Code: 0xEFAC ==> Row:79 Col:12 */
    {0x9500,0xFACF}, /* ��: GB2312 Code: 0xCFFA ==> Row:47 Col:90 */
    {0x9501,0xF8CB}, /* ��: GB2312 Code: 0xCBF8 ==> Row:43 Col:88 */
    {0x9502,0xAEEF}, /* �: GB2312 Code: 0xEFAE ==> Row:79 Col:14 */
    {0x9503,0xADEF}, /* �: GB2312 Code: 0xEFAD ==> Row:79 Col:13 */
    {0x9504,0xFAB3}, /* ��: GB2312 Code: 0xB3FA ==> Row:19 Col:90 */
    {0x9505,0xF8B9}, /* ��: GB2312 Code: 0xB9F8 ==> Row:25 Col:88 */
    {0x9506,0xAFEF}, /* �: GB2312 Code: 0xEFAF ==> Row:79 Col:15 */
    {0x9507,0xB0EF}, /* �: GB2312 Code: 0xEFB0 ==> Row:79 Col:16 */
    {0x9508,0xE2D0}, /* ��: GB2312 Code: 0xD0E2 ==> Row:48 Col:66 */
    {0x9509,0xB1EF}, /* �: GB2312 Code: 0xEFB1 ==> Row:79 Col:17 */
    {0x950A,0xB2EF}, /* �: GB2312 Code: 0xEFB2 ==> Row:79 Col:18 */
    {0x950B,0xE6B7}, /* ��: GB2312 Code: 0xB7E6 ==> Row:23 Col:70 */
    {0x950C,0xBFD0}, /* п: GB2312 Code: 0xD0BF ==> Row:48 Col:31 */
    {0x950D,0xB3EF}, /* �: GB2312 Code: 0xEFB3 ==> Row:79 Col:19 */
    {0x950E,0xB4EF}, /* �: GB2312 Code: 0xEFB4 ==> Row:79 Col:20 */
    {0x950F,0xB5EF}, /* �: GB2312 Code: 0xEFB5 ==> Row:79 Col:21 */
    {0x9510,0xF1C8}, /* ��: GB2312 Code: 0xC8F1 ==> Row:40 Col:81 */
    {0x9511,0xE0CC}, /* ��: GB2312 Code: 0xCCE0 ==> Row:44 Col:64 */
    {0x9512,0xB6EF}, /* �: GB2312 Code: 0xEFB6 ==> Row:79 Col:22 */
    {0x9513,0xB7EF}, /* �: GB2312 Code: 0xEFB7 ==> Row:79 Col:23 */
    {0x9514,0xB8EF}, /* �: GB2312 Code: 0xEFB8 ==> Row:79 Col:24 */
    {0x9515,0xB9EF}, /* �: GB2312 Code: 0xEFB9 ==> Row:79 Col:25 */
    {0x9516,0xBAEF}, /* �: GB2312 Code: 0xEFBA ==> Row:79 Col:26 */
    {0x9517,0xE0D5}, /* ��: GB2312 Code: 0xD5E0 ==> Row:53 Col:64 */
    {0x9518,0xBBEF}, /* �: GB2312 Code: 0xEFBB ==> Row:79 Col:27 */
    {0x9519,0xEDB4}, /* ��: GB2312 Code: 0xB4ED ==> Row:20 Col:77 */
    {0x951A,0xAAC3}, /* ê: GB2312 Code: 0xC3AA ==> Row:35 Col:10 */
    {0x951B,0xBCEF}, /* �: GB2312 Code: 0xEFBC ==> Row:79 Col:28 */
    {0x951D,0xBDEF}, /* �: GB2312 Code: 0xEFBD ==> Row:79 Col:29 */
    {0x951E,0xBEEF}, /* �: GB2312 Code: 0xEFBE ==> Row:79 Col:30 */
    {0x951F,0xBFEF}, /* �: GB2312 Code: 0xEFBF ==> Row:79 Col:31 */
    {0x9521,0xFDCE}, /* ��: GB2312 Code: 0xCEFD ==> Row:46 Col:93 */
    {0x9522,0xC0EF}, /* ��: GB2312 Code: 0xEFC0 ==> Row:79 Col:32 */
    {0x9523,0xE0C2}, /* ��: GB2312 Code: 0xC2E0 ==> Row:34 Col:64 */
    {0x9524,0xB8B4}, /* ��: GB2312 Code: 0xB4B8 ==> Row:20 Col:24 */
    {0x9525,0xB6D7}, /* ׶: GB2312 Code: 0xD7B6 ==> Row:55 Col:22 */
    {0x9526,0xF5BD}, /* ��: GB2312 Code: 0xBDF5 ==> Row:29 Col:85 */
    {0x9528,0xC7CF}, /* ��: GB2312 Code: 0xCFC7 ==> Row:47 Col:39 */
    {0x9529,0xC3EF}, /* ��: GB2312 Code: 0xEFC3 ==> Row:79 Col:35 */
    {0x952A,0xC1EF}, /* ��: GB2312 Code: 0xEFC1 ==> Row:79 Col:33 */
    {0x952B,0xC2EF}, /* ��: GB2312 Code: 0xEFC2 ==> Row:79 Col:34 */
    {0x952C,0xC4EF}, /* ��: GB2312 Code: 0xEFC4 ==> Row:79 Col:36 */
    {0x952D,0xA7B6}, /* ��: GB2312 Code: 0xB6A7 ==> Row:22 Col:07 */
    {0x952E,0xFCBC}, /* ��: GB2312 Code: 0xBCFC ==> Row:28 Col:92 */
    {0x952F,0xE2BE}, /* ��: GB2312 Code: 0xBEE2 ==> Row:30 Col:66 */
    {0x9530,0xCCC3}, /* ��: GB2312 Code: 0xC3CC ==> Row:35 Col:44 */
    {0x9531,0xC5EF}, /* ��: GB2312 Code: 0xEFC5 ==> Row:79 Col:37 */
    {0x9532,0xC6EF}, /* ��: GB2312 Code: 0xEFC6 ==> Row:79 Col:38 */
    {0x9534,0xC7EF}, /* ��: GB2312 Code: 0xEFC7 ==> Row:79 Col:39 */
    {0x9535,0xCFEF}, /* ��: GB2312 Code: 0xEFCF ==> Row:79 Col:47 */
    {0x9536,0xC8EF}, /* ��: GB2312 Code: 0xEFC8 ==> Row:79 Col:40 */
    {0x9537,0xC9EF}, /* ��: GB2312 Code: 0xEFC9 ==> Row:79 Col:41 */
    {0x9538,0xCAEF}, /* ��: GB2312 Code: 0xEFCA ==> Row:79 Col:42 */
    {0x9539,0xC2C7}, /* ��: GB2312 Code: 0xC7C2 ==> Row:39 Col:34 */
    {0x953A,0xF1EF}, /* ��: GB2312 Code: 0xEFF1 ==> Row:79 Col:81 */
    {0x953B,0xCDB6}, /* ��: GB2312 Code: 0xB6CD ==> Row:22 Col:45 */
    {0x953C,0xCBEF}, /* ��: GB2312 Code: 0xEFCB ==> Row:79 Col:43 */
    {0x953E,0xCCEF}, /* ��: GB2312 Code: 0xEFCC ==> Row:79 Col:44 */
    {0x953F,0xCDEF}, /* ��: GB2312 Code: 0xEFCD ==> Row:79 Col:45 */
    {0x9540,0xC6B6}, /* ��: GB2312 Code: 0xB6C6 ==> Row:22 Col:38 */
    {0x9541,0xBEC3}, /* þ: GB2312 Code: 0xC3BE ==> Row:35 Col:30 */
    {0x9542,0xCEEF}, /* ��: GB2312 Code: 0xEFCE ==> Row:79 Col:46 */
    {0x9544,0xD0EF}, /* ��: GB2312 Code: 0xEFD0 ==> Row:79 Col:48 */
    {0x9545,0xD1EF}, /* ��: GB2312 Code: 0xEFD1 ==> Row:79 Col:49 */
    {0x9546,0xD2EF}, /* ��: GB2312 Code: 0xEFD2 ==> Row:79 Col:50 */
    {0x9547,0xF2D5}, /* ��: GB2312 Code: 0xD5F2 ==> Row:53 Col:82 */
    {0x9549,0xD3EF}, /* ��: GB2312 Code: 0xEFD3 ==> Row:79 Col:51 */
    {0x954A,0xF7C4}, /* ��: GB2312 Code: 0xC4F7 ==> Row:36 Col:87 */
    {0x954C,0xD4EF}, /* ��: GB2312 Code: 0xEFD4 ==> Row:79 Col:52 */
    {0x954D,0xF8C4}, /* ��: GB2312 Code: 0xC4F8 ==> Row:36 Col:88 */
    {0x954E,0xD5EF}, /* ��: GB2312 Code: 0xEFD5 ==> Row:79 Col:53 */
    {0x954F,0xD6EF}, /* ��: GB2312 Code: 0xEFD6 ==> Row:79 Col:54 */
    {0x9550,0xE4B8}, /* ��: GB2312 Code: 0xB8E4 ==> Row:24 Col:68 */
    {0x9551,0xF7B0}, /* ��: GB2312 Code: 0xB0F7 ==> Row:16 Col:87 */
    {0x9552,0xD7EF}, /* ��: GB2312 Code: 0xEFD7 ==> Row:79 Col:55 */
    {0x9553,0xD8EF}, /* ��: GB2312 Code: 0xEFD8 ==> Row:79 Col:56 */
    {0x9554,0xD9EF}, /* ��: GB2312 Code: 0xEFD9 ==> Row:79 Col:57 */
    {0x9556,0xDAEF}, /* ��: GB2312 Code: 0xEFDA ==> Row:79 Col:58 */
    {0x9557,0xDBEF}, /* ��: GB2312 Code: 0xEFDB ==> Row:79 Col:59 */
    {0x9558,0xDCEF}, /* ��: GB2312 Code: 0xEFDC ==> Row:79 Col:60 */
    {0x9559,0xDDEF}, /* ��: GB2312 Code: 0xEFDD ==> Row:79 Col:61 */
    {0x955B,0xDEEF}, /* ��: GB2312 Code: 0xEFDE ==> Row:79 Col:62 */
    {0x955C,0xB5BE}, /* ��: GB2312 Code: 0xBEB5 ==> Row:30 Col:21 */
    {0x955D,0xE1EF}, /* ��: GB2312 Code: 0xEFE1 ==> Row:79 Col:65 */
    {0x955E,0xDFEF}, /* ��: GB2312 Code: 0xEFDF ==> Row:79 Col:63 */
    {0x955F,0xE0EF}, /* ��: GB2312 Code: 0xEFE0 ==> Row:79 Col:64 */
    {0x9561,0xE2EF}, /* ��: GB2312 Code: 0xEFE2 ==> Row:79 Col:66 */
    {0x9562,0xE3EF}, /* ��: GB2312 Code: 0xEFE3 ==> Row:79 Col:67 */
    {0x9563,0xCDC1}, /* ��: GB2312 Code: 0xC1CD ==> Row:33 Col:45 */
    {0x9564,0xE4EF}, /* ��: GB2312 Code: 0xEFE4 ==> Row:79 Col:68 */
    {0x9565,0xE5EF}, /* ��: GB2312 Code: 0xEFE5 ==> Row:79 Col:69 */
    {0x9566,0xE6EF}, /* ��: GB2312 Code: 0xEFE6 ==> Row:79 Col:70 */
    {0x9567,0xE7EF}, /* ��: GB2312 Code: 0xEFE7 ==> Row:79 Col:71 */
    {0x9568,0xE8EF}, /* ��: GB2312 Code: 0xEFE8 ==> Row:79 Col:72 */
    {0x9569,0xE9EF}, /* ��: GB2312 Code: 0xEFE9 ==> Row:79 Col:73 */
    {0x956A,0xEAEF}, /* ��: GB2312 Code: 0xEFEA ==> Row:79 Col:74 */
    {0x956B,0xEBEF}, /* ��: GB2312 Code: 0xEFEB ==> Row:79 Col:75 */
    {0x956C,0xECEF}, /* ��: GB2312 Code: 0xEFEC ==> Row:79 Col:76 */
    {0x956D,0xD8C0}, /* ��: GB2312 Code: 0xC0D8 ==> Row:32 Col:56 */
    {0x956F,0xEDEF}, /* ��: GB2312 Code: 0xEFED ==> Row:79 Col:77 */
    {0x9570,0xADC1}, /* ��: GB2312 Code: 0xC1AD ==> Row:33 Col:13 */
    {0x9571,0xEEEF}, /* ��: GB2312 Code: 0xEFEE ==> Row:79 Col:78 */
    {0x9572,0xEFEF}, /* ��: GB2312 Code: 0xEFEF ==> Row:79 Col:79 */
    {0x9573,0xF0EF}, /* ��: GB2312 Code: 0xEFF0 ==> Row:79 Col:80 */
    {0x9576,0xE2CF}, /* ��: GB2312 Code: 0xCFE2 ==> Row:47 Col:66 */
    {0x957F,0xA4B3}, /* ��: GB2312 Code: 0xB3A4 ==> Row:19 Col:04 */
    {0x95E8,0xC5C3}, /* ��: GB2312 Code: 0xC3C5 ==> Row:35 Col:37 */
    {0x95E9,0xC5E3}, /* ��: GB2312 Code: 0xE3C5 ==> Row:67 Col:37 */
    {0x95EA,0xC1C9}, /* ��: GB2312 Code: 0xC9C1 ==> Row:41 Col:33 */
    {0x95EB,0xC6E3}, /* ��: GB2312 Code: 0xE3C6 ==> Row:67 Col:38 */
    {0x95ED,0xD5B1}, /* ��: GB2312 Code: 0xB1D5 ==> Row:17 Col:53 */
    {0x95EE,0xCACE}, /* ��: GB2312 Code: 0xCECA ==> Row:46 Col:42 */
    {0x95EF,0xB3B4}, /* ��: GB2312 Code: 0xB4B3 ==> Row:20 Col:19 */
    {0x95F0,0xF2C8}, /* ��: GB2312 Code: 0xC8F2 ==> Row:40 Col:82 */
    {0x95F1,0xC7E3}, /* ��: GB2312 Code: 0xE3C7 ==> Row:67 Col:39 */
    {0x95F2,0xD0CF}, /* ��: GB2312 Code: 0xCFD0 ==> Row:47 Col:48 */
    {0x95F3,0xC8E3}, /* ��: GB2312 Code: 0xE3C8 ==> Row:67 Col:40 */
    {0x95F4,0xE4BC}, /* ��: GB2312 Code: 0xBCE4 ==> Row:28 Col:68 */
    {0x95F5,0xC9E3}, /* ��: GB2312 Code: 0xE3C9 ==> Row:67 Col:41 */
    {0x95F6,0xCAE3}, /* ��: GB2312 Code: 0xE3CA ==> Row:67 Col:42 */
    {0x95F7,0xC6C3}, /* ��: GB2312 Code: 0xC3C6 ==> Row:35 Col:38 */
    {0x95F8,0xA2D5}, /* բ: GB2312 Code: 0xD5A2 ==> Row:53 Col:02 */
    {0x95F9,0xD6C4}, /* ��: GB2312 Code: 0xC4D6 ==> Row:36 Col:54 */
    {0x95FA,0xEBB9}, /* ��: GB2312 Code: 0xB9EB ==> Row:25 Col:75 */
    {0x95FB,0xC5CE}, /* ��: GB2312 Code: 0xCEC5 ==> Row:46 Col:37 */
    {0x95FC,0xCBE3}, /* ��: GB2312 Code: 0xE3CB ==> Row:67 Col:43 */
    {0x95FD,0xF6C3}, /* ��: GB2312 Code: 0xC3F6 ==> Row:35 Col:86 */
    {0x95FE,0xCCE3}, /* ��: GB2312 Code: 0xE3CC ==> Row:67 Col:44 */
    {0x9600,0xA7B7}, /* ��: GB2312 Code: 0xB7A7 ==> Row:23 Col:07 */
    {0x9601,0xF3B8}, /* ��: GB2312 Code: 0xB8F3 ==> Row:24 Col:83 */
    {0x9602,0xD2BA}, /* ��: GB2312 Code: 0xBAD2 ==> Row:26 Col:50 */
    {0x9603,0xCDE3}, /* ��: GB2312 Code: 0xE3CD ==> Row:67 Col:45 */
    {0x9604,0xCEE3}, /* ��: GB2312 Code: 0xE3CE ==> Row:67 Col:46 */
    {0x9605,0xC4D4}, /* ��: GB2312 Code: 0xD4C4 ==> Row:52 Col:36 */
    {0x9606,0xCFE3}, /* ��: GB2312 Code: 0xE3CF ==> Row:67 Col:47 */
    {0x9608,0xD0E3}, /* ��: GB2312 Code: 0xE3D0 ==> Row:67 Col:48 */
    {0x9609,0xCBD1}, /* ��: GB2312 Code: 0xD1CB ==> Row:49 Col:43 */
    {0x960A,0xD1E3}, /* ��: GB2312 Code: 0xE3D1 ==> Row:67 Col:49 */
    {0x960B,0xD2E3}, /* ��: GB2312 Code: 0xE3D2 ==> Row:67 Col:50 */
    {0x960C,0xD3E3}, /* ��: GB2312 Code: 0xE3D3 ==> Row:67 Col:51 */
    {0x960D,0xD4E3}, /* ��: GB2312 Code: 0xE3D4 ==> Row:67 Col:52 */
    {0x960E,0xD6D1}, /* ��: GB2312 Code: 0xD1D6 ==> Row:49 Col:54 */
    {0x960F,0xD5E3}, /* ��: GB2312 Code: 0xE3D5 ==> Row:67 Col:53 */
    {0x9610,0xFBB2}, /* ��: GB2312 Code: 0xB2FB ==> Row:18 Col:91 */
    {0x9611,0xBBC0}, /* ��: GB2312 Code: 0xC0BB ==> Row:32 Col:27 */
    {0x9612,0xD6E3}, /* ��: GB2312 Code: 0xE3D6 ==> Row:67 Col:54 */
    {0x9614,0xABC0}, /* ��: GB2312 Code: 0xC0AB ==> Row:32 Col:11 */
    {0x9615,0xD7E3}, /* ��: GB2312 Code: 0xE3D7 ==> Row:67 Col:55 */
    {0x9616,0xD8E3}, /* ��: GB2312 Code: 0xE3D8 ==> Row:67 Col:56 */
    {0x9617,0xD9E3}, /* ��: GB2312 Code: 0xE3D9 ==> Row:67 Col:57 */
    {0x9619,0xDAE3}, /* ��: GB2312 Code: 0xE3DA ==> Row:67 Col:58 */
    {0x961A,0xDBE3}, /* ��: GB2312 Code: 0xE3DB ==> Row:67 Col:59 */
    {0x961C,0xB7B8}, /* ��: GB2312 Code: 0xB8B7 ==> Row:24 Col:23 */
    {0x961D,0xE2DA}, /* ��: GB2312 Code: 0xDAE2 ==> Row:58 Col:66 */
    {0x961F,0xD3B6}, /* ��: GB2312 Code: 0xB6D3 ==> Row:22 Col:51 */
    {0x9621,0xE4DA}, /* ��: GB2312 Code: 0xDAE4 ==> Row:58 Col:68 */
    {0x9622,0xE3DA}, /* ��: GB2312 Code: 0xDAE3 ==> Row:58 Col:67 */
    {0x962A,0xE6DA}, /* ��: GB2312 Code: 0xDAE6 ==> Row:58 Col:70 */
    {0x962E,0xEEC8}, /* ��: GB2312 Code: 0xC8EE ==> Row:40 Col:78 */
    {0x9631,0xE5DA}, /* ��: GB2312 Code: 0xDAE5 ==> Row:58 Col:69 */
    {0x9632,0xC0B7}, /* ��: GB2312 Code: 0xB7C0 ==> Row:23 Col:32 */
    {0x9633,0xF4D1}, /* ��: GB2312 Code: 0xD1F4 ==> Row:49 Col:84 */
    {0x9634,0xF5D2}, /* ��: GB2312 Code: 0xD2F5 ==> Row:50 Col:85 */
    {0x9635,0xF3D5}, /* ��: GB2312 Code: 0xD5F3 ==> Row:53 Col:83 */
    {0x9636,0xD7BD}, /* ��: GB2312 Code: 0xBDD7 ==> Row:29 Col:55 */
    {0x963B,0xE8D7}, /* ��: GB2312 Code: 0xD7E8 ==> Row:55 Col:72 */
    {0x963C,0xE8DA}, /* ��: GB2312 Code: 0xDAE8 ==> Row:58 Col:72 */
    {0x963D,0xE7DA}, /* ��: GB2312 Code: 0xDAE7 ==> Row:58 Col:71 */
    {0x963F,0xA2B0}, /* ��: GB2312 Code: 0xB0A2 ==> Row:16 Col:02 */
    {0x9640,0xD3CD}, /* ��: GB2312 Code: 0xCDD3 ==> Row:45 Col:51 */
    {0x9642,0xE9DA}, /* ��: GB2312 Code: 0xDAE9 ==> Row:58 Col:73 */
    {0x9644,0xBDB8}, /* ��: GB2312 Code: 0xB8BD ==> Row:24 Col:29 */
    {0x9645,0xCABC}, /* ��: GB2312 Code: 0xBCCA ==> Row:28 Col:42 */
    {0x9646,0xBDC2}, /* ½: GB2312 Code: 0xC2BD ==> Row:34 Col:29 */
    {0x9647,0xA4C2}, /* ¤: GB2312 Code: 0xC2A4 ==> Row:34 Col:04 */
    {0x9648,0xC2B3}, /* ��: GB2312 Code: 0xB3C2 ==> Row:19 Col:34 */
    {0x9649,0xEADA}, /* ��: GB2312 Code: 0xDAEA ==> Row:58 Col:74 */
    {0x964B,0xAAC2}, /* ª: GB2312 Code: 0xC2AA ==> Row:34 Col:10 */
    {0x964C,0xB0C4}, /* İ: GB2312 Code: 0xC4B0 ==> Row:36 Col:16 */
    {0x964D,0xB5BD}, /* ��: GB2312 Code: 0xBDB5 ==> Row:29 Col:21 */
    {0x9650,0xDECF}, /* ��: GB2312 Code: 0xCFDE ==> Row:47 Col:62 */
    {0x9654,0xEBDA}, /* ��: GB2312 Code: 0xDAEB ==> Row:58 Col:75 */
    {0x9655,0xC2C9}, /* ��: GB2312 Code: 0xC9C2 ==> Row:41 Col:34 */
    {0x965B,0xDDB1}, /* ��: GB2312 Code: 0xB1DD ==> Row:17 Col:61 */
    {0x965F,0xECDA}, /* ��: GB2312 Code: 0xDAEC ==> Row:58 Col:76 */
    {0x9661,0xB8B6}, /* ��: GB2312 Code: 0xB6B8 ==> Row:22 Col:24 */
    {0x9662,0xBAD4}, /* Ժ: GB2312 Code: 0xD4BA ==> Row:52 Col:26 */
    {0x9664,0xFDB3}, /* ��: GB2312 Code: 0xB3FD ==> Row:19 Col:93 */
    {0x9667,0xEDDA}, /* ��: GB2312 Code: 0xDAED ==> Row:58 Col:77 */
    {0x9668,0xC9D4}, /* ��: GB2312 Code: 0xD4C9 ==> Row:52 Col:41 */
    {0x9669,0xD5CF}, /* ��: GB2312 Code: 0xCFD5 ==> Row:47 Col:53 */
    {0x966A,0xE3C5}, /* ��: GB2312 Code: 0xC5E3 ==> Row:37 Col:67 */
    {0x966C,0xEEDA}, /* ��: GB2312 Code: 0xDAEE ==> Row:58 Col:78 */
    {0x9672,0xEFDA}, /* ��: GB2312 Code: 0xDAEF ==> Row:58 Col:79 */
    {0x9674,0xF0DA}, /* ��: GB2312 Code: 0xDAF0 ==> Row:58 Col:80 */
    {0x9675,0xEAC1}, /* ��: GB2312 Code: 0xC1EA ==> Row:33 Col:74 */
    {0x9676,0xD5CC}, /* ��: GB2312 Code: 0xCCD5 ==> Row:44 Col:53 */
    {0x9677,0xDDCF}, /* ��: GB2312 Code: 0xCFDD ==> Row:47 Col:61 */
    {0x9685,0xE7D3}, /* ��: GB2312 Code: 0xD3E7 ==> Row:51 Col:71 */
    {0x9686,0xA1C2}, /* ¡: GB2312 Code: 0xC2A1 ==> Row:34 Col:01 */
    {0x9688,0xF1DA}, /* ��: GB2312 Code: 0xDAF1 ==> Row:58 Col:81 */
    {0x968B,0xE5CB}, /* ��: GB2312 Code: 0xCBE5 ==> Row:43 Col:69 */
    {0x968D,0xF2DA}, /* ��: GB2312 Code: 0xDAF2 ==> Row:58 Col:82 */
    {0x968F,0xE6CB}, /* ��: GB2312 Code: 0xCBE6 ==> Row:43 Col:70 */
    {0x9690,0xFED2}, /* ��: GB2312 Code: 0xD2FE ==> Row:50 Col:94 */
    {0x9694,0xF4B8}, /* ��: GB2312 Code: 0xB8F4 ==> Row:24 Col:84 */
    {0x9697,0xF3DA}, /* ��: GB2312 Code: 0xDAF3 ==> Row:58 Col:83 */
    {0x9698,0xAFB0}, /* ��: GB2312 Code: 0xB0AF ==> Row:16 Col:15 */
    {0x9699,0xB6CF}, /* ϶: GB2312 Code: 0xCFB6 ==> Row:47 Col:22 */
    {0x969C,0xCFD5}, /* ��: GB2312 Code: 0xD5CF ==> Row:53 Col:47 */
    {0x96A7,0xEDCB}, /* ��: GB2312 Code: 0xCBED ==> Row:43 Col:77 */
    {0x96B0,0xF4DA}, /* ��: GB2312 Code: 0xDAF4 ==> Row:58 Col:84 */
    {0x96B3,0xC4E3}, /* ��: GB2312 Code: 0xE3C4 ==> Row:67 Col:36 */
    {0x96B6,0xA5C1}, /* ��: GB2312 Code: 0xC1A5 ==> Row:33 Col:05 */
    {0x96B9,0xBFF6}, /* ��: GB2312 Code: 0xF6BF ==> Row:86 Col:31 */
    {0x96BC,0xC0F6}, /* ��: GB2312 Code: 0xF6C0 ==> Row:86 Col:32 */
    {0x96BD,0xC1F6}, /* ��: GB2312 Code: 0xF6C1 ==> Row:86 Col:33 */
    {0x96BE,0xD1C4}, /* ��: GB2312 Code: 0xC4D1 ==> Row:36 Col:49 */
    {0x96C0,0xB8C8}, /* ȸ: GB2312 Code: 0xC8B8 ==> Row:40 Col:24 */
    {0x96C1,0xE3D1}, /* ��: GB2312 Code: 0xD1E3 ==> Row:49 Col:67 */
    {0x96C4,0xDBD0}, /* ��: GB2312 Code: 0xD0DB ==> Row:48 Col:59 */
    {0x96C5,0xC5D1}, /* ��: GB2312 Code: 0xD1C5 ==> Row:49 Col:37 */
    {0x96C6,0xAFBC}, /* ��: GB2312 Code: 0xBCAF ==> Row:28 Col:15 */
    {0x96C7,0xCDB9}, /* ��: GB2312 Code: 0xB9CD ==> Row:25 Col:45 */
    {0x96C9,0xF4EF}, /* ��: GB2312 Code: 0xEFF4 ==> Row:79 Col:84 */
    {0x96CC,0xC6B4}, /* ��: GB2312 Code: 0xB4C6 ==> Row:20 Col:38 */
    {0x96CD,0xBAD3}, /* Ӻ: GB2312 Code: 0xD3BA ==> Row:51 Col:26 */
    {0x96CE,0xC2F6}, /* ��: GB2312 Code: 0xF6C2 ==> Row:86 Col:34 */
    {0x96CF,0xFBB3}, /* ��: GB2312 Code: 0xB3FB ==> Row:19 Col:91 */
    {0x96D2,0xC3F6}, /* ��: GB2312 Code: 0xF6C3 ==> Row:86 Col:35 */
    {0x96D5,0xF1B5}, /* ��: GB2312 Code: 0xB5F1 ==> Row:21 Col:81 */
    {0x96E0,0xC5F6}, /* ��: GB2312 Code: 0xF6C5 ==> Row:86 Col:37 */
    {0x96E8,0xEAD3}, /* ��: GB2312 Code: 0xD3EA ==> Row:51 Col:74 */
    {0x96E9,0xA7F6}, /* ��: GB2312 Code: 0xF6A7 ==> Row:86 Col:07 */
    {0x96EA,0xA9D1}, /* ѩ: GB2312 Code: 0xD1A9 ==> Row:49 Col:09 */
    {0x96EF,0xA9F6}, /* ��: GB2312 Code: 0xF6A9 ==> Row:86 Col:09 */
    {0x96F3,0xA8F6}, /* ��: GB2312 Code: 0xF6A8 ==> Row:86 Col:08 */
    {0x96F6,0xE3C1}, /* ��: GB2312 Code: 0xC1E3 ==> Row:33 Col:67 */
    {0x96F7,0xD7C0}, /* ��: GB2312 Code: 0xC0D7 ==> Row:32 Col:55 */
    {0x96F9,0xA2B1}, /* ��: GB2312 Code: 0xB1A2 ==> Row:17 Col:02 */
    {0x96FE,0xEDCE}, /* ��: GB2312 Code: 0xCEED ==> Row:46 Col:77 */
    {0x9700,0xE8D0}, /* ��: GB2312 Code: 0xD0E8 ==> Row:48 Col:72 */
    {0x9701,0xABF6}, /* ��: GB2312 Code: 0xF6AB ==> Row:86 Col:11 */
    {0x9704,0xF6CF}, /* ��: GB2312 Code: 0xCFF6 ==> Row:47 Col:86 */
    {0x9706,0xAAF6}, /* ��: GB2312 Code: 0xF6AA ==> Row:86 Col:10 */
    {0x9707,0xF0D5}, /* ��: GB2312 Code: 0xD5F0 ==> Row:53 Col:80 */
    {0x9708,0xACF6}, /* ��: GB2312 Code: 0xF6AC ==> Row:86 Col:12 */
    {0x9709,0xB9C3}, /* ù: GB2312 Code: 0xC3B9 ==> Row:35 Col:25 */
    {0x970D,0xF4BB}, /* ��: GB2312 Code: 0xBBF4 ==> Row:27 Col:84 */
    {0x970E,0xAEF6}, /* ��: GB2312 Code: 0xF6AE ==> Row:86 Col:14 */
    {0x970F,0xADF6}, /* ��: GB2312 Code: 0xF6AD ==> Row:86 Col:13 */
    {0x9713,0xDEC4}, /* ��: GB2312 Code: 0xC4DE ==> Row:36 Col:62 */
    {0x9716,0xD8C1}, /* ��: GB2312 Code: 0xC1D8 ==> Row:33 Col:56 */
    {0x971C,0xAACB}, /* ˪: GB2312 Code: 0xCBAA ==> Row:43 Col:10 */
    {0x971E,0xBCCF}, /* ϼ: GB2312 Code: 0xCFBC ==> Row:47 Col:28 */
    {0x972A,0xAFF6}, /* ��: GB2312 Code: 0xF6AF ==> Row:86 Col:15 */
    {0x972D,0xB0F6}, /* ��: GB2312 Code: 0xF6B0 ==> Row:86 Col:16 */
    {0x9730,0xB1F6}, /* ��: GB2312 Code: 0xF6B1 ==> Row:86 Col:17 */
    {0x9732,0xB6C2}, /* ¶: GB2312 Code: 0xC2B6 ==> Row:34 Col:22 */
    {0x9738,0xD4B0}, /* ��: GB2312 Code: 0xB0D4 ==> Row:16 Col:52 */
    {0x9739,0xF9C5}, /* ��: GB2312 Code: 0xC5F9 ==> Row:37 Col:89 */
    {0x973E,0xB2F6}, /* ��: GB2312 Code: 0xF6B2 ==> Row:86 Col:18 */
    {0x9752,0xE0C7}, /* ��: GB2312 Code: 0xC7E0 ==> Row:39 Col:64 */
    {0x9753,0xA6F6}, /* ��: GB2312 Code: 0xF6A6 ==> Row:86 Col:06 */
    {0x9756,0xB8BE}, /* ��: GB2312 Code: 0xBEB8 ==> Row:30 Col:24 */
    {0x9759,0xB2BE}, /* ��: GB2312 Code: 0xBEB2 ==> Row:30 Col:18 */
    {0x975B,0xE5B5}, /* ��: GB2312 Code: 0xB5E5 ==> Row:21 Col:69 */
    {0x975E,0xC7B7}, /* ��: GB2312 Code: 0xB7C7 ==> Row:23 Col:39 */
    {0x9760,0xBFBF}, /* ��: GB2312 Code: 0xBFBF ==> Row:31 Col:31 */
    {0x9761,0xD2C3}, /* ��: GB2312 Code: 0xC3D2 ==> Row:35 Col:50 */
    {0x9762,0xE6C3}, /* ��: GB2312 Code: 0xC3E6 ==> Row:35 Col:70 */
    {0x9765,0xCCD8}, /* ��: GB2312 Code: 0xD8CC ==> Row:56 Col:44 */
    {0x9769,0xEFB8}, /* ��: GB2312 Code: 0xB8EF ==> Row:24 Col:79 */
    {0x9773,0xF9BD}, /* ��: GB2312 Code: 0xBDF9 ==> Row:29 Col:89 */
    {0x9774,0xA5D1}, /* ѥ: GB2312 Code: 0xD1A5 ==> Row:49 Col:05 */
    {0x9776,0xD0B0}, /* ��: GB2312 Code: 0xB0D0 ==> Row:16 Col:48 */
    {0x977C,0xB0F7}, /* ��: GB2312 Code: 0xF7B0 ==> Row:87 Col:16 */
    {0x9785,0xB1F7}, /* ��: GB2312 Code: 0xF7B1 ==> Row:87 Col:17 */
    {0x978B,0xACD0}, /* Ь: GB2312 Code: 0xD0AC ==> Row:48 Col:12 */
    {0x978D,0xB0B0}, /* ��: GB2312 Code: 0xB0B0 ==> Row:16 Col:16 */
    {0x9791,0xB2F7}, /* ��: GB2312 Code: 0xF7B2 ==> Row:87 Col:18 */
    {0x9792,0xB3F7}, /* ��: GB2312 Code: 0xF7B3 ==> Row:87 Col:19 */
    {0x9794,0xB4F7}, /* ��: GB2312 Code: 0xF7B4 ==> Row:87 Col:20 */
    {0x9798,0xCAC7}, /* ��: GB2312 Code: 0xC7CA ==> Row:39 Col:42 */
    {0x97A0,0xCFBE}, /* ��: GB2312 Code: 0xBECF ==> Row:30 Col:47 */
    {0x97A3,0xB7F7}, /* ��: GB2312 Code: 0xF7B7 ==> Row:87 Col:23 */
    {0x97AB,0xB6F7}, /* ��: GB2312 Code: 0xF7B6 ==> Row:87 Col:22 */
    {0x97AD,0xDEB1}, /* ��: GB2312 Code: 0xB1DE ==> Row:17 Col:62 */
    {0x97AF,0xB5F7}, /* ��: GB2312 Code: 0xF7B5 ==> Row:87 Col:21 */
    {0x97B2,0xB8F7}, /* ��: GB2312 Code: 0xF7B8 ==> Row:87 Col:24 */
    {0x97B4,0xB9F7}, /* ��: GB2312 Code: 0xF7B9 ==> Row:87 Col:25 */
    {0x97E6,0xA4CE}, /* Τ: GB2312 Code: 0xCEA4 ==> Row:46 Col:04 */
    {0x97E7,0xCDC8}, /* ��: GB2312 Code: 0xC8CD ==> Row:40 Col:45 */
    {0x97E9,0xABBA}, /* ��: GB2312 Code: 0xBAAB ==> Row:26 Col:11 */
    {0x97EA,0xB8E8}, /* �: GB2312 Code: 0xE8B8 ==> Row:72 Col:24 */
    {0x97EB,0xB9E8}, /* �: GB2312 Code: 0xE8B9 ==> Row:72 Col:25 */
    {0x97EC,0xBAE8}, /* �: GB2312 Code: 0xE8BA ==> Row:72 Col:26 */
    {0x97ED,0xC2BE}, /* ��: GB2312 Code: 0xBEC2 ==> Row:30 Col:34 */
    {0x97F3,0xF4D2}, /* ��: GB2312 Code: 0xD2F4 ==> Row:50 Col:84 */
    {0x97F5,0xCFD4}, /* ��: GB2312 Code: 0xD4CF ==> Row:52 Col:47 */
    {0x97F6,0xD8C9}, /* ��: GB2312 Code: 0xC9D8 ==> Row:41 Col:56 */
    {0x9875,0xB3D2}, /* ҳ: GB2312 Code: 0xD2B3 ==> Row:50 Col:19 */
    {0x9876,0xA5B6}, /* ��: GB2312 Code: 0xB6A5 ==> Row:22 Col:05 */
    {0x9877,0xEAC7}, /* ��: GB2312 Code: 0xC7EA ==> Row:39 Col:74 */
    {0x9878,0xFCF1}, /* ��: GB2312 Code: 0xF1FC ==> Row:81 Col:92 */
    {0x9879,0xEECF}, /* ��: GB2312 Code: 0xCFEE ==> Row:47 Col:78 */
    {0x987A,0xB3CB}, /* ˳: GB2312 Code: 0xCBB3 ==> Row:43 Col:19 */
    {0x987B,0xEBD0}, /* ��: GB2312 Code: 0xD0EB ==> Row:48 Col:75 */
    {0x987C,0xEFE7}, /* ��: GB2312 Code: 0xE7EF ==> Row:71 Col:79 */
    {0x987D,0xE7CD}, /* ��: GB2312 Code: 0xCDE7 ==> Row:45 Col:71 */
    {0x987E,0xCBB9}, /* ��: GB2312 Code: 0xB9CB ==> Row:25 Col:43 */
    {0x987F,0xD9B6}, /* ��: GB2312 Code: 0xB6D9 ==> Row:22 Col:57 */
    {0x9880,0xFDF1}, /* ��: GB2312 Code: 0xF1FD ==> Row:81 Col:93 */
    {0x9881,0xE4B0}, /* ��: GB2312 Code: 0xB0E4 ==> Row:16 Col:68 */
    {0x9882,0xCCCB}, /* ��: GB2312 Code: 0xCBCC ==> Row:43 Col:44 */
    {0x9883,0xFEF1}, /* ��: GB2312 Code: 0xF1FE ==> Row:81 Col:94 */
    {0x9884,0xA4D4}, /* Ԥ: GB2312 Code: 0xD4A4 ==> Row:52 Col:04 */
    {0x9885,0xADC2}, /* ­: GB2312 Code: 0xC2AD ==> Row:34 Col:13 */
    {0x9886,0xECC1}, /* ��: GB2312 Code: 0xC1EC ==> Row:33 Col:76 */
    {0x9887,0xC4C6}, /* ��: GB2312 Code: 0xC6C4 ==> Row:38 Col:36 */
    {0x9888,0xB1BE}, /* ��: GB2312 Code: 0xBEB1 ==> Row:30 Col:17 */
    {0x9889,0xA1F2}, /* �: GB2312 Code: 0xF2A1 ==> Row:82 Col:01 */
    {0x988A,0xD5BC}, /* ��: GB2312 Code: 0xBCD5 ==> Row:28 Col:53 */
    {0x988C,0xA2F2}, /* �: GB2312 Code: 0xF2A2 ==> Row:82 Col:02 */
    {0x988D,0xA3F2}, /* �: GB2312 Code: 0xF2A3 ==> Row:82 Col:03 */
    {0x988F,0xA4F2}, /* �: GB2312 Code: 0xF2A4 ==> Row:82 Col:04 */
    {0x9890,0xC3D2}, /* ��: GB2312 Code: 0xD2C3 ==> Row:50 Col:35 */
    {0x9891,0xB5C6}, /* Ƶ: GB2312 Code: 0xC6B5 ==> Row:38 Col:21 */
    {0x9893,0xC7CD}, /* ��: GB2312 Code: 0xCDC7 ==> Row:45 Col:39 */
    {0x9894,0xA5F2}, /* �: GB2312 Code: 0xF2A5 ==> Row:82 Col:05 */
    {0x9896,0xB1D3}, /* ӱ: GB2312 Code: 0xD3B1 ==> Row:51 Col:17 */
    {0x9897,0xC5BF}, /* ��: GB2312 Code: 0xBFC5 ==> Row:31 Col:37 */
    {0x9898,0xE2CC}, /* ��: GB2312 Code: 0xCCE2 ==> Row:44 Col:66 */
    {0x989A,0xA6F2}, /* �: GB2312 Code: 0xF2A6 ==> Row:82 Col:06 */
    {0x989B,0xA7F2}, /* �: GB2312 Code: 0xF2A7 ==> Row:82 Col:07 */
    {0x989C,0xD5D1}, /* ��: GB2312 Code: 0xD1D5 ==> Row:49 Col:53 */
    {0x989D,0xEEB6}, /* ��: GB2312 Code: 0xB6EE ==> Row:22 Col:78 */
    {0x989E,0xA8F2}, /* �: GB2312 Code: 0xF2A8 ==> Row:82 Col:08 */
    {0x989F,0xA9F2}, /* �: GB2312 Code: 0xF2A9 ==> Row:82 Col:09 */
    {0x98A0,0xDFB5}, /* ��: GB2312 Code: 0xB5DF ==> Row:21 Col:63 */
    {0x98A1,0xAAF2}, /* �: GB2312 Code: 0xF2AA ==> Row:82 Col:10 */
    {0x98A2,0xABF2}, /* �: GB2312 Code: 0xF2AB ==> Row:82 Col:11 */
    {0x98A4,0xFCB2}, /* ��: GB2312 Code: 0xB2FC ==> Row:18 Col:92 */
    {0x98A5,0xACF2}, /* �: GB2312 Code: 0xF2AC ==> Row:82 Col:12 */
    {0x98A6,0xADF2}, /* �: GB2312 Code: 0xF2AD ==> Row:82 Col:13 */
    {0x98A7,0xA7C8}, /* ȧ: GB2312 Code: 0xC8A7 ==> Row:40 Col:07 */
    {0x98CE,0xE7B7}, /* ��: GB2312 Code: 0xB7E7 ==> Row:23 Col:71 */
    {0x98D1,0xA9EC}, /* �: GB2312 Code: 0xECA9 ==> Row:76 Col:09 */
    {0x98D2,0xAAEC}, /* �: GB2312 Code: 0xECAA ==> Row:76 Col:10 */
    {0x98D3,0xABEC}, /* �: GB2312 Code: 0xECAB ==> Row:76 Col:11 */
    {0x98D5,0xACEC}, /* �: GB2312 Code: 0xECAC ==> Row:76 Col:12 */
    {0x98D8,0xAEC6}, /* Ʈ: GB2312 Code: 0xC6AE ==> Row:38 Col:14 */
    {0x98D9,0xADEC}, /* �: GB2312 Code: 0xECAD ==> Row:76 Col:13 */
    {0x98DA,0xAEEC}, /* �: GB2312 Code: 0xECAE ==> Row:76 Col:14 */
    {0x98DE,0xC9B7}, /* ��: GB2312 Code: 0xB7C9 ==> Row:23 Col:41 */
    {0x98DF,0xB3CA}, /* ʳ: GB2312 Code: 0xCAB3 ==> Row:42 Col:19 */
    {0x98E7,0xB8E2}, /* �: GB2312 Code: 0xE2B8 ==> Row:66 Col:24 */
    {0x98E8,0xCFF7}, /* ��: GB2312 Code: 0xF7CF ==> Row:87 Col:47 */
    {0x990D,0xD0F7}, /* ��: GB2312 Code: 0xF7D0 ==> Row:87 Col:48 */
    {0x9910,0xCDB2}, /* ��: GB2312 Code: 0xB2CD ==> Row:18 Col:45 */
    {0x992E,0xD1F7}, /* ��: GB2312 Code: 0xF7D1 ==> Row:87 Col:49 */
    {0x9954,0xD3F7}, /* ��: GB2312 Code: 0xF7D3 ==> Row:87 Col:51 */
    {0x9955,0xD2F7}, /* ��: GB2312 Code: 0xF7D2 ==> Row:87 Col:50 */
    {0x9963,0xBBE2}, /* �: GB2312 Code: 0xE2BB ==> Row:66 Col:27 */
    {0x9965,0xA2BC}, /* ��: GB2312 Code: 0xBCA2 ==> Row:28 Col:02 */
    {0x9967,0xBCE2}, /* �: GB2312 Code: 0xE2BC ==> Row:66 Col:28 */
    {0x9968,0xBDE2}, /* �: GB2312 Code: 0xE2BD ==> Row:66 Col:29 */
    {0x9969,0xBEE2}, /* �: GB2312 Code: 0xE2BE ==> Row:66 Col:30 */
    {0x996A,0xBFE2}, /* �: GB2312 Code: 0xE2BF ==> Row:66 Col:31 */
    {0x996B,0xC0E2}, /* ��: GB2312 Code: 0xE2C0 ==> Row:66 Col:32 */
    {0x996C,0xC1E2}, /* ��: GB2312 Code: 0xE2C1 ==> Row:66 Col:33 */
    {0x996D,0xB9B7}, /* ��: GB2312 Code: 0xB7B9 ==> Row:23 Col:25 */
    {0x996E,0xFBD2}, /* ��: GB2312 Code: 0xD2FB ==> Row:50 Col:91 */
    {0x996F,0xA4BD}, /* ��: GB2312 Code: 0xBDA4 ==> Row:29 Col:04 */
    {0x9970,0xCECA}, /* ��: GB2312 Code: 0xCACE ==> Row:42 Col:46 */
    {0x9971,0xA5B1}, /* ��: GB2312 Code: 0xB1A5 ==> Row:17 Col:05 */
    {0x9972,0xC7CB}, /* ��: GB2312 Code: 0xCBC7 ==> Row:43 Col:39 */
    {0x9974,0xC2E2}, /* ��: GB2312 Code: 0xE2C2 ==> Row:66 Col:34 */
    {0x9975,0xFCB6}, /* ��: GB2312 Code: 0xB6FC ==> Row:22 Col:92 */
    {0x9976,0xC4C8}, /* ��: GB2312 Code: 0xC8C4 ==> Row:40 Col:36 */
    {0x9977,0xC3E2}, /* ��: GB2312 Code: 0xE2C3 ==> Row:66 Col:35 */
    {0x997A,0xC8BD}, /* ��: GB2312 Code: 0xBDC8 ==> Row:29 Col:40 */
    {0x997C,0xFDB1}, /* ��: GB2312 Code: 0xB1FD ==> Row:17 Col:93 */
    {0x997D,0xC4E2}, /* ��: GB2312 Code: 0xE2C4 ==> Row:66 Col:36 */
    {0x997F,0xF6B6}, /* ��: GB2312 Code: 0xB6F6 ==> Row:22 Col:86 */
    {0x9980,0xC5E2}, /* ��: GB2312 Code: 0xE2C5 ==> Row:66 Col:37 */
    {0x9981,0xD9C4}, /* ��: GB2312 Code: 0xC4D9 ==> Row:36 Col:57 */
    {0x9984,0xC6E2}, /* ��: GB2312 Code: 0xE2C6 ==> Row:66 Col:38 */
    {0x9985,0xDACF}, /* ��: GB2312 Code: 0xCFDA ==> Row:47 Col:58 */
    {0x9986,0xDDB9}, /* ��: GB2312 Code: 0xB9DD ==> Row:25 Col:61 */
    {0x9987,0xC7E2}, /* ��: GB2312 Code: 0xE2C7 ==> Row:66 Col:39 */
    {0x9988,0xA1C0}, /* ��: GB2312 Code: 0xC0A1 ==> Row:32 Col:01 */
    {0x998A,0xC8E2}, /* ��: GB2312 Code: 0xE2C8 ==> Row:66 Col:40 */
    {0x998B,0xF6B2}, /* ��: GB2312 Code: 0xB2F6 ==> Row:18 Col:86 */
    {0x998D,0xC9E2}, /* ��: GB2312 Code: 0xE2C9 ==> Row:66 Col:41 */
    {0x998F,0xF3C1}, /* ��: GB2312 Code: 0xC1F3 ==> Row:33 Col:83 */
    {0x9990,0xCAE2}, /* ��: GB2312 Code: 0xE2CA ==> Row:66 Col:42 */
    {0x9991,0xCBE2}, /* ��: GB2312 Code: 0xE2CB ==> Row:66 Col:43 */
    {0x9992,0xF8C2}, /* ��: GB2312 Code: 0xC2F8 ==> Row:34 Col:88 */
    {0x9993,0xCCE2}, /* ��: GB2312 Code: 0xE2CC ==> Row:66 Col:44 */
    {0x9994,0xCDE2}, /* ��: GB2312 Code: 0xE2CD ==> Row:66 Col:45 */
    {0x9995,0xCEE2}, /* ��: GB2312 Code: 0xE2CE ==> Row:66 Col:46 */
    {0x9996,0xD7CA}, /* ��: GB2312 Code: 0xCAD7 ==> Row:42 Col:55 */
    {0x9997,0xB8D8}, /* ظ: GB2312 Code: 0xD8B8 ==> Row:56 Col:24 */
    {0x9998,0xE5D9}, /* ��: GB2312 Code: 0xD9E5 ==> Row:57 Col:69 */
    {0x9999,0xE3CF}, /* ��: GB2312 Code: 0xCFE3 ==> Row:47 Col:67 */
    {0x99A5,0xA5F0}, /* �: GB2312 Code: 0xF0A5 ==> Row:80 Col:05 */
    {0x99A8,0xB0DC}, /* ܰ: GB2312 Code: 0xDCB0 ==> Row:60 Col:16 */
    {0x9A6C,0xEDC2}, /* ��: GB2312 Code: 0xC2ED ==> Row:34 Col:77 */
    {0x9A6D,0xA6D4}, /* Ԧ: GB2312 Code: 0xD4A6 ==> Row:52 Col:06 */
    {0x9A6E,0xD4CD}, /* ��: GB2312 Code: 0xCDD4 ==> Row:45 Col:52 */
    {0x9A6F,0xB1D1}, /* ѱ: GB2312 Code: 0xD1B1 ==> Row:49 Col:17 */
    {0x9A70,0xDBB3}, /* ��: GB2312 Code: 0xB3DB ==> Row:19 Col:59 */
    {0x9A71,0xFDC7}, /* ��: GB2312 Code: 0xC7FD ==> Row:39 Col:93 */
    {0x9A73,0xB5B2}, /* ��: GB2312 Code: 0xB2B5 ==> Row:18 Col:21 */
    {0x9A74,0xBFC2}, /* ¿: GB2312 Code: 0xC2BF ==> Row:34 Col:31 */
    {0x9A75,0xE0E6}, /* ��: GB2312 Code: 0xE6E0 ==> Row:70 Col:64 */
    {0x9A76,0xBBCA}, /* ʻ: GB2312 Code: 0xCABB ==> Row:42 Col:27 */
    {0x9A77,0xE1E6}, /* ��: GB2312 Code: 0xE6E1 ==> Row:70 Col:65 */
    {0x9A78,0xE2E6}, /* ��: GB2312 Code: 0xE6E2 ==> Row:70 Col:66 */
    {0x9A79,0xD4BE}, /* ��: GB2312 Code: 0xBED4 ==> Row:30 Col:52 */
    {0x9A7A,0xE3E6}, /* ��: GB2312 Code: 0xE6E3 ==> Row:70 Col:67 */
    {0x9A7B,0xA4D7}, /* פ: GB2312 Code: 0xD7A4 ==> Row:55 Col:04 */
    {0x9A7C,0xD5CD}, /* ��: GB2312 Code: 0xCDD5 ==> Row:45 Col:53 */
    {0x9A7D,0xE5E6}, /* ��: GB2312 Code: 0xE6E5 ==> Row:70 Col:69 */
    {0x9A7E,0xDDBC}, /* ��: GB2312 Code: 0xBCDD ==> Row:28 Col:61 */
    {0x9A7F,0xE4E6}, /* ��: GB2312 Code: 0xE6E4 ==> Row:70 Col:68 */
    {0x9A80,0xE6E6}, /* ��: GB2312 Code: 0xE6E6 ==> Row:70 Col:70 */
    {0x9A81,0xE7E6}, /* ��: GB2312 Code: 0xE6E7 ==> Row:70 Col:71 */
    {0x9A82,0xEEC2}, /* ��: GB2312 Code: 0xC2EE ==> Row:34 Col:78 */
    {0x9A84,0xBEBD}, /* ��: GB2312 Code: 0xBDBE ==> Row:29 Col:30 */
    {0x9A85,0xE8E6}, /* ��: GB2312 Code: 0xE6E8 ==> Row:70 Col:72 */
    {0x9A86,0xE6C2}, /* ��: GB2312 Code: 0xC2E6 ==> Row:34 Col:70 */
    {0x9A87,0xA7BA}, /* ��: GB2312 Code: 0xBAA7 ==> Row:26 Col:07 */
    {0x9A88,0xE9E6}, /* ��: GB2312 Code: 0xE6E9 ==> Row:70 Col:73 */
    {0x9A8A,0xEAE6}, /* ��: GB2312 Code: 0xE6EA ==> Row:70 Col:74 */
    {0x9A8B,0xD2B3}, /* ��: GB2312 Code: 0xB3D2 ==> Row:19 Col:50 */
    {0x9A8C,0xE9D1}, /* ��: GB2312 Code: 0xD1E9 ==> Row:49 Col:73 */
    {0x9A8F,0xA5BF}, /* ��: GB2312 Code: 0xBFA5 ==> Row:31 Col:05 */
    {0x9A90,0xEBE6}, /* ��: GB2312 Code: 0xE6EB ==> Row:70 Col:75 */
    {0x9A91,0xEFC6}, /* ��: GB2312 Code: 0xC6EF ==> Row:38 Col:79 */
    {0x9A92,0xECE6}, /* ��: GB2312 Code: 0xE6EC ==> Row:70 Col:76 */
    {0x9A93,0xEDE6}, /* ��: GB2312 Code: 0xE6ED ==> Row:70 Col:77 */
    {0x9A96,0xEEE6}, /* ��: GB2312 Code: 0xE6EE ==> Row:70 Col:78 */
    {0x9A97,0xADC6}, /* ƭ: GB2312 Code: 0xC6AD ==> Row:38 Col:13 */
    {0x9A98,0xEFE6}, /* ��: GB2312 Code: 0xE6EF ==> Row:70 Col:79 */
    {0x9A9A,0xA7C9}, /* ɧ: GB2312 Code: 0xC9A7 ==> Row:41 Col:07 */
    {0x9A9B,0xF0E6}, /* ��: GB2312 Code: 0xE6F0 ==> Row:70 Col:80 */
    {0x9A9C,0xF1E6}, /* ��: GB2312 Code: 0xE6F1 ==> Row:70 Col:81 */
    {0x9A9D,0xF2E6}, /* ��: GB2312 Code: 0xE6F2 ==> Row:70 Col:82 */
    {0x9A9E,0xB9E5}, /* �: GB2312 Code: 0xE5B9 ==> Row:69 Col:25 */
    {0x9A9F,0xF3E6}, /* ��: GB2312 Code: 0xE6F3 ==> Row:70 Col:83 */
    {0x9AA0,0xF4E6}, /* ��: GB2312 Code: 0xE6F4 ==> Row:70 Col:84 */
    {0x9AA1,0xE2C2}, /* ��: GB2312 Code: 0xC2E2 ==> Row:34 Col:66 */
    {0x9AA2,0xF5E6}, /* ��: GB2312 Code: 0xE6F5 ==> Row:70 Col:85 */
    {0x9AA3,0xF6E6}, /* ��: GB2312 Code: 0xE6F6 ==> Row:70 Col:86 */
    {0x9AA4,0xE8D6}, /* ��: GB2312 Code: 0xD6E8 ==> Row:54 Col:72 */
    {0x9AA5,0xF7E6}, /* ��: GB2312 Code: 0xE6F7 ==> Row:70 Col:87 */
    {0x9AA7,0xF8E6}, /* ��: GB2312 Code: 0xE6F8 ==> Row:70 Col:88 */
    {0x9AA8,0xC7B9}, /* ��: GB2312 Code: 0xB9C7 ==> Row:25 Col:39 */
    {0x9AB0,0xBBF7}, /* ��: GB2312 Code: 0xF7BB ==> Row:87 Col:27 */
    {0x9AB1,0xBAF7}, /* ��: GB2312 Code: 0xF7BA ==> Row:87 Col:26 */
    {0x9AB6,0xBEF7}, /* ��: GB2312 Code: 0xF7BE ==> Row:87 Col:30 */
    {0x9AB7,0xBCF7}, /* ��: GB2312 Code: 0xF7BC ==> Row:87 Col:28 */
    {0x9AB8,0xA1BA}, /* ��: GB2312 Code: 0xBAA1 ==> Row:26 Col:01 */
    {0x9ABA,0xBFF7}, /* ��: GB2312 Code: 0xF7BF ==> Row:87 Col:31 */
    {0x9ABC,0xC0F7}, /* ��: GB2312 Code: 0xF7C0 ==> Row:87 Col:32 */
    {0x9AC0,0xC2F7}, /* ��: GB2312 Code: 0xF7C2 ==> Row:87 Col:34 */
    {0x9AC1,0xC1F7}, /* ��: GB2312 Code: 0xF7C1 ==> Row:87 Col:33 */
    {0x9AC2,0xC4F7}, /* ��: GB2312 Code: 0xF7C4 ==> Row:87 Col:36 */
    {0x9AC5,0xC3F7}, /* ��: GB2312 Code: 0xF7C3 ==> Row:87 Col:35 */
    {0x9ACB,0xC5F7}, /* ��: GB2312 Code: 0xF7C5 ==> Row:87 Col:37 */
    {0x9ACC,0xC6F7}, /* ��: GB2312 Code: 0xF7C6 ==> Row:87 Col:38 */
    {0x9AD1,0xC7F7}, /* ��: GB2312 Code: 0xF7C7 ==> Row:87 Col:39 */
    {0x9AD3,0xE8CB}, /* ��: GB2312 Code: 0xCBE8 ==> Row:43 Col:72 */
    {0x9AD8,0xDFB8}, /* ��: GB2312 Code: 0xB8DF ==> Row:24 Col:63 */
    {0x9ADF,0xD4F7}, /* ��: GB2312 Code: 0xF7D4 ==> Row:87 Col:52 */
    {0x9AE1,0xD5F7}, /* ��: GB2312 Code: 0xF7D5 ==> Row:87 Col:53 */
    {0x9AE6,0xD6F7}, /* ��: GB2312 Code: 0xF7D6 ==> Row:87 Col:54 */
    {0x9AEB,0xD8F7}, /* ��: GB2312 Code: 0xF7D8 ==> Row:87 Col:56 */
    {0x9AED,0xDAF7}, /* ��: GB2312 Code: 0xF7DA ==> Row:87 Col:58 */
    {0x9AEF,0xD7F7}, /* ��: GB2312 Code: 0xF7D7 ==> Row:87 Col:55 */
    {0x9AF9,0xDBF7}, /* ��: GB2312 Code: 0xF7DB ==> Row:87 Col:59 */
    {0x9AFB,0xD9F7}, /* ��: GB2312 Code: 0xF7D9 ==> Row:87 Col:57 */
    {0x9B03,0xD7D7}, /* ��: GB2312 Code: 0xD7D7 ==> Row:55 Col:55 */
    {0x9B08,0xDCF7}, /* ��: GB2312 Code: 0xF7DC ==> Row:87 Col:60 */
    {0x9B0F,0xDDF7}, /* ��: GB2312 Code: 0xF7DD ==> Row:87 Col:61 */
    {0x9B13,0xDEF7}, /* ��: GB2312 Code: 0xF7DE ==> Row:87 Col:62 */
    {0x9B1F,0xDFF7}, /* ��: GB2312 Code: 0xF7DF ==> Row:87 Col:63 */
    {0x9B23,0xE0F7}, /* ��: GB2312 Code: 0xF7E0 ==> Row:87 Col:64 */
    {0x9B2F,0xCBDB}, /* ��: GB2312 Code: 0xDBCB ==> Row:59 Col:43 */
    {0x9B32,0xAAD8}, /* ت: GB2312 Code: 0xD8AA ==> Row:56 Col:10 */
    {0x9B3B,0xF7E5}, /* ��: GB2312 Code: 0xE5F7 ==> Row:69 Col:87 */
    {0x9B3C,0xEDB9}, /* ��: GB2312 Code: 0xB9ED ==> Row:25 Col:77 */
    {0x9B41,0xFDBF}, /* ��: GB2312 Code: 0xBFFD ==> Row:31 Col:93 */
    {0x9B42,0xEABB}, /* ��: GB2312 Code: 0xBBEA ==> Row:27 Col:74 */
    {0x9B43,0xC9F7}, /* ��: GB2312 Code: 0xF7C9 ==> Row:87 Col:41 */
    {0x9B44,0xC7C6}, /* ��: GB2312 Code: 0xC6C7 ==> Row:38 Col:39 */
    {0x9B45,0xC8F7}, /* ��: GB2312 Code: 0xF7C8 ==> Row:87 Col:40 */
    {0x9B47,0xCAF7}, /* ��: GB2312 Code: 0xF7CA ==> Row:87 Col:42 */
    {0x9B48,0xCCF7}, /* ��: GB2312 Code: 0xF7CC ==> Row:87 Col:44 */
    {0x9B49,0xCBF7}, /* ��: GB2312 Code: 0xF7CB ==> Row:87 Col:43 */
    {0x9B4D,0xCDF7}, /* ��: GB2312 Code: 0xF7CD ==> Row:87 Col:45 */
    {0x9B4F,0xBACE}, /* κ: GB2312 Code: 0xCEBA ==> Row:46 Col:26 */
    {0x9B51,0xCEF7}, /* ��: GB2312 Code: 0xF7CE ==> Row:87 Col:46 */
    {0x9B54,0xA7C4}, /* ħ: GB2312 Code: 0xC4A7 ==> Row:36 Col:07 */
    {0x9C7C,0xE3D3}, /* ��: GB2312 Code: 0xD3E3 ==> Row:51 Col:67 */
    {0x9C7F,0xCFF6}, /* ��: GB2312 Code: 0xF6CF ==> Row:86 Col:47 */
    {0x9C81,0xB3C2}, /* ³: GB2312 Code: 0xC2B3 ==> Row:34 Col:19 */
    {0x9C82,0xD0F6}, /* ��: GB2312 Code: 0xF6D0 ==> Row:86 Col:48 */
    {0x9C85,0xD1F6}, /* ��: GB2312 Code: 0xF6D1 ==> Row:86 Col:49 */
    {0x9C86,0xD2F6}, /* ��: GB2312 Code: 0xF6D2 ==> Row:86 Col:50 */
    {0x9C87,0xD3F6}, /* ��: GB2312 Code: 0xF6D3 ==> Row:86 Col:51 */
    {0x9C88,0xD4F6}, /* ��: GB2312 Code: 0xF6D4 ==> Row:86 Col:52 */
    {0x9C8B,0xD6F6}, /* ��: GB2312 Code: 0xF6D6 ==> Row:86 Col:54 */
    {0x9C8D,0xABB1}, /* ��: GB2312 Code: 0xB1AB ==> Row:17 Col:11 */
    {0x9C8E,0xD7F6}, /* ��: GB2312 Code: 0xF6D7 ==> Row:86 Col:55 */
    {0x9C90,0xD8F6}, /* ��: GB2312 Code: 0xF6D8 ==> Row:86 Col:56 */
    {0x9C91,0xD9F6}, /* ��: GB2312 Code: 0xF6D9 ==> Row:86 Col:57 */
    {0x9C92,0xDAF6}, /* ��: GB2312 Code: 0xF6DA ==> Row:86 Col:58 */
    {0x9C94,0xDBF6}, /* ��: GB2312 Code: 0xF6DB ==> Row:86 Col:59 */
    {0x9C95,0xDCF6}, /* ��: GB2312 Code: 0xF6DC ==> Row:86 Col:60 */
    {0x9C9A,0xDDF6}, /* ��: GB2312 Code: 0xF6DD ==> Row:86 Col:61 */
    {0x9C9B,0xDEF6}, /* ��: GB2312 Code: 0xF6DE ==> Row:86 Col:62 */
    {0x9C9C,0xCACF}, /* ��: GB2312 Code: 0xCFCA ==> Row:47 Col:42 */
    {0x9C9E,0xDFF6}, /* ��: GB2312 Code: 0xF6DF ==> Row:86 Col:63 */
    {0x9C9F,0xE0F6}, /* ��: GB2312 Code: 0xF6E0 ==> Row:86 Col:64 */
    {0x9CA0,0xE1F6}, /* ��: GB2312 Code: 0xF6E1 ==> Row:86 Col:65 */
    {0x9CA1,0xE2F6}, /* ��: GB2312 Code: 0xF6E2 ==> Row:86 Col:66 */
    {0x9CA2,0xE3F6}, /* ��: GB2312 Code: 0xF6E3 ==> Row:86 Col:67 */
    {0x9CA3,0xE4F6}, /* ��: GB2312 Code: 0xF6E4 ==> Row:86 Col:68 */
    {0x9CA4,0xF0C0}, /* ��: GB2312 Code: 0xC0F0 ==> Row:32 Col:80 */
    {0x9CA5,0xE5F6}, /* ��: GB2312 Code: 0xF6E5 ==> Row:86 Col:69 */
    {0x9CA6,0xE6F6}, /* ��: GB2312 Code: 0xF6E6 ==> Row:86 Col:70 */
    {0x9CA7,0xE7F6}, /* ��: GB2312 Code: 0xF6E7 ==> Row:86 Col:71 */
    {0x9CA8,0xE8F6}, /* ��: GB2312 Code: 0xF6E8 ==> Row:86 Col:72 */
    {0x9CA9,0xE9F6}, /* ��: GB2312 Code: 0xF6E9 ==> Row:86 Col:73 */
    {0x9CAB,0xEAF6}, /* ��: GB2312 Code: 0xF6EA ==> Row:86 Col:74 */
    {0x9CAD,0xEBF6}, /* ��: GB2312 Code: 0xF6EB ==> Row:86 Col:75 */
    {0x9CAE,0xECF6}, /* ��: GB2312 Code: 0xF6EC ==> Row:86 Col:76 */
    {0x9CB0,0xEDF6}, /* ��: GB2312 Code: 0xF6ED ==> Row:86 Col:77 */
    {0x9CB1,0xEEF6}, /* ��: GB2312 Code: 0xF6EE ==> Row:86 Col:78 */
    {0x9CB2,0xEFF6}, /* ��: GB2312 Code: 0xF6EF ==> Row:86 Col:79 */
    {0x9CB3,0xF0F6}, /* ��: GB2312 Code: 0xF6F0 ==> Row:86 Col:80 */
    {0x9CB4,0xF1F6}, /* ��: GB2312 Code: 0xF6F1 ==> Row:86 Col:81 */
    {0x9CB5,0xF2F6}, /* ��: GB2312 Code: 0xF6F2 ==> Row:86 Col:82 */
    {0x9CB6,0xF3F6}, /* ��: GB2312 Code: 0xF6F3 ==> Row:86 Col:83 */
    {0x9CB7,0xF4F6}, /* ��: GB2312 Code: 0xF6F4 ==> Row:86 Col:84 */
    {0x9CB8,0xA8BE}, /* ��: GB2312 Code: 0xBEA8 ==> Row:30 Col:08 */
    {0x9CBA,0xF5F6}, /* ��: GB2312 Code: 0xF6F5 ==> Row:86 Col:85 */
    {0x9CBB,0xF6F6}, /* ��: GB2312 Code: 0xF6F6 ==> Row:86 Col:86 */
    {0x9CBC,0xF7F6}, /* ��: GB2312 Code: 0xF6F7 ==> Row:86 Col:87 */
    {0x9CBD,0xF8F6}, /* ��: GB2312 Code: 0xF6F8 ==> Row:86 Col:88 */
    {0x9CC3,0xFAC8}, /* ��: GB2312 Code: 0xC8FA ==> Row:40 Col:90 */
    {0x9CC4,0xF9F6}, /* ��: GB2312 Code: 0xF6F9 ==> Row:86 Col:89 */
    {0x9CC5,0xFAF6}, /* ��: GB2312 Code: 0xF6FA ==> Row:86 Col:90 */
    {0x9CC6,0xFBF6}, /* ��: GB2312 Code: 0xF6FB ==> Row:86 Col:91 */
    {0x9CC7,0xFCF6}, /* ��: GB2312 Code: 0xF6FC ==> Row:86 Col:92 */
    {0x9CCA,0xFDF6}, /* ��: GB2312 Code: 0xF6FD ==> Row:86 Col:93 */
    {0x9CCB,0xFEF6}, /* ��: GB2312 Code: 0xF6FE ==> Row:86 Col:94 */
    {0x9CCC,0xA1F7}, /* ��: GB2312 Code: 0xF7A1 ==> Row:87 Col:01 */
    {0x9CCD,0xA2F7}, /* ��: GB2312 Code: 0xF7A2 ==> Row:87 Col:02 */
    {0x9CCE,0xA3F7}, /* ��: GB2312 Code: 0xF7A3 ==> Row:87 Col:03 */
    {0x9CCF,0xA4F7}, /* ��: GB2312 Code: 0xF7A4 ==> Row:87 Col:04 */
    {0x9CD0,0xA5F7}, /* ��: GB2312 Code: 0xF7A5 ==> Row:87 Col:05 */
    {0x9CD3,0xA6F7}, /* ��: GB2312 Code: 0xF7A6 ==> Row:87 Col:06 */
    {0x9CD4,0xA7F7}, /* ��: GB2312 Code: 0xF7A7 ==> Row:87 Col:07 */
    {0x9CD5,0xA8F7}, /* ��: GB2312 Code: 0xF7A8 ==> Row:87 Col:08 */
    {0x9CD6,0xEEB1}, /* ��: GB2312 Code: 0xB1EE ==> Row:17 Col:78 */
    {0x9CD7,0xA9F7}, /* ��: GB2312 Code: 0xF7A9 ==> Row:87 Col:09 */
    {0x9CD8,0xAAF7}, /* ��: GB2312 Code: 0xF7AA ==> Row:87 Col:10 */
    {0x9CD9,0xABF7}, /* ��: GB2312 Code: 0xF7AB ==> Row:87 Col:11 */
    {0x9CDC,0xACF7}, /* ��: GB2312 Code: 0xF7AC ==> Row:87 Col:12 */
    {0x9CDD,0xADF7}, /* ��: GB2312 Code: 0xF7AD ==> Row:87 Col:13 */
    {0x9CDE,0xDBC1}, /* ��: GB2312 Code: 0xC1DB ==> Row:33 Col:59 */
    {0x9CDF,0xAEF7}, /* ��: GB2312 Code: 0xF7AE ==> Row:87 Col:14 */
    {0x9CE2,0xAFF7}, /* ��: GB2312 Code: 0xF7AF ==> Row:87 Col:15 */
    {0x9E1F,0xF1C4}, /* ��: GB2312 Code: 0xC4F1 ==> Row:36 Col:81 */
    {0x9E20,0xAFF0}, /* �: GB2312 Code: 0xF0AF ==> Row:80 Col:15 */
    {0x9E21,0xA6BC}, /* ��: GB2312 Code: 0xBCA6 ==> Row:28 Col:06 */
    {0x9E22,0xB0F0}, /* �: GB2312 Code: 0xF0B0 ==> Row:80 Col:16 */
    {0x9E23,0xF9C3}, /* ��: GB2312 Code: 0xC3F9 ==> Row:35 Col:89 */
    {0x9E25,0xB8C5}, /* Ÿ: GB2312 Code: 0xC5B8 ==> Row:37 Col:24 */
    {0x9E26,0xBBD1}, /* ѻ: GB2312 Code: 0xD1BB ==> Row:49 Col:27 */
    {0x9E28,0xB1F0}, /* �: GB2312 Code: 0xF0B1 ==> Row:80 Col:17 */
    {0x9E29,0xB2F0}, /* �: GB2312 Code: 0xF0B2 ==> Row:80 Col:18 */
    {0x9E2A,0xB3F0}, /* �: GB2312 Code: 0xF0B3 ==> Row:80 Col:19 */
    {0x9E2B,0xB4F0}, /* �: GB2312 Code: 0xF0B4 ==> Row:80 Col:20 */
    {0x9E2C,0xB5F0}, /* �: GB2312 Code: 0xF0B5 ==> Row:80 Col:21 */
    {0x9E2D,0xBCD1}, /* Ѽ: GB2312 Code: 0xD1BC ==> Row:49 Col:28 */
    {0x9E2F,0xECD1}, /* ��: GB2312 Code: 0xD1EC ==> Row:49 Col:76 */
    {0x9E31,0xB7F0}, /* �: GB2312 Code: 0xF0B7 ==> Row:80 Col:23 */
    {0x9E32,0xB6F0}, /* �: GB2312 Code: 0xF0B6 ==> Row:80 Col:22 */
    {0x9E33,0xA7D4}, /* ԧ: GB2312 Code: 0xD4A7 ==> Row:52 Col:07 */
    {0x9E35,0xD2CD}, /* ��: GB2312 Code: 0xCDD2 ==> Row:45 Col:50 */
    {0x9E36,0xB8F0}, /* �: GB2312 Code: 0xF0B8 ==> Row:80 Col:24 */
    {0x9E37,0xBAF0}, /* �: GB2312 Code: 0xF0BA ==> Row:80 Col:26 */
    {0x9E38,0xB9F0}, /* �: GB2312 Code: 0xF0B9 ==> Row:80 Col:25 */
    {0x9E39,0xBBF0}, /* �: GB2312 Code: 0xF0BB ==> Row:80 Col:27 */
    {0x9E3A,0xBCF0}, /* �: GB2312 Code: 0xF0BC ==> Row:80 Col:28 */
    {0x9E3D,0xEBB8}, /* ��: GB2312 Code: 0xB8EB ==> Row:24 Col:75 */
    {0x9E3E,0xBDF0}, /* �: GB2312 Code: 0xF0BD ==> Row:80 Col:29 */
    {0x9E3F,0xE8BA}, /* ��: GB2312 Code: 0xBAE8 ==> Row:26 Col:72 */
    {0x9E41,0xBEF0}, /* �: GB2312 Code: 0xF0BE ==> Row:80 Col:30 */
    {0x9E42,0xBFF0}, /* �: GB2312 Code: 0xF0BF ==> Row:80 Col:31 */
    {0x9E43,0xE9BE}, /* ��: GB2312 Code: 0xBEE9 ==> Row:30 Col:73 */
    {0x9E44,0xC0F0}, /* ��: GB2312 Code: 0xF0C0 ==> Row:80 Col:32 */
    {0x9E45,0xECB6}, /* ��: GB2312 Code: 0xB6EC ==> Row:22 Col:76 */
    {0x9E46,0xC1F0}, /* ��: GB2312 Code: 0xF0C1 ==> Row:80 Col:33 */
    {0x9E47,0xC2F0}, /* ��: GB2312 Code: 0xF0C2 ==> Row:80 Col:34 */
    {0x9E48,0xC3F0}, /* ��: GB2312 Code: 0xF0C3 ==> Row:80 Col:35 */
    {0x9E49,0xC4F0}, /* ��: GB2312 Code: 0xF0C4 ==> Row:80 Col:36 */
    {0x9E4A,0xB5C8}, /* ȵ: GB2312 Code: 0xC8B5 ==> Row:40 Col:21 */
    {0x9E4B,0xC5F0}, /* ��: GB2312 Code: 0xF0C5 ==> Row:80 Col:37 */
    {0x9E4C,0xC6F0}, /* ��: GB2312 Code: 0xF0C6 ==> Row:80 Col:38 */
    {0x9E4E,0xC7F0}, /* ��: GB2312 Code: 0xF0C7 ==> Row:80 Col:39 */
    {0x9E4F,0xF4C5}, /* ��: GB2312 Code: 0xC5F4 ==> Row:37 Col:84 */
    {0x9E51,0xC8F0}, /* ��: GB2312 Code: 0xF0C8 ==> Row:80 Col:40 */
    {0x9E55,0xC9F0}, /* ��: GB2312 Code: 0xF0C9 ==> Row:80 Col:41 */
    {0x9E57,0xCAF0}, /* ��: GB2312 Code: 0xF0CA ==> Row:80 Col:42 */
    {0x9E58,0xBDF7}, /* ��: GB2312 Code: 0xF7BD ==> Row:87 Col:29 */
    {0x9E5A,0xCBF0}, /* ��: GB2312 Code: 0xF0CB ==> Row:80 Col:43 */
    {0x9E5B,0xCCF0}, /* ��: GB2312 Code: 0xF0CC ==> Row:80 Col:44 */
    {0x9E5C,0xCDF0}, /* ��: GB2312 Code: 0xF0CD ==> Row:80 Col:45 */
    {0x9E5E,0xCEF0}, /* ��: GB2312 Code: 0xF0CE ==> Row:80 Col:46 */
    {0x9E63,0xCFF0}, /* ��: GB2312 Code: 0xF0CF ==> Row:80 Col:47 */
    {0x9E64,0xD7BA}, /* ��: GB2312 Code: 0xBAD7 ==> Row:26 Col:55 */
    {0x9E66,0xD0F0}, /* ��: GB2312 Code: 0xF0D0 ==> Row:80 Col:48 */
    {0x9E67,0xD1F0}, /* ��: GB2312 Code: 0xF0D1 ==> Row:80 Col:49 */
    {0x9E68,0xD2F0}, /* ��: GB2312 Code: 0xF0D2 ==> Row:80 Col:50 */
    {0x9E69,0xD3F0}, /* ��: GB2312 Code: 0xF0D3 ==> Row:80 Col:51 */
    {0x9E6A,0xD4F0}, /* ��: GB2312 Code: 0xF0D4 ==> Row:80 Col:52 */
    {0x9E6B,0xD5F0}, /* ��: GB2312 Code: 0xF0D5 ==> Row:80 Col:53 */
    {0x9E6C,0xD6F0}, /* ��: GB2312 Code: 0xF0D6 ==> Row:80 Col:54 */
    {0x9E6D,0xD8F0}, /* ��: GB2312 Code: 0xF0D8 ==> Row:80 Col:56 */
    {0x9E70,0xA5D3}, /* ӥ: GB2312 Code: 0xD3A5 ==> Row:51 Col:05 */
    {0x9E71,0xD7F0}, /* ��: GB2312 Code: 0xF0D7 ==> Row:80 Col:55 */
    {0x9E73,0xD9F0}, /* ��: GB2312 Code: 0xF0D9 ==> Row:80 Col:57 */
    {0x9E7E,0xBAF5}, /* ��: GB2312 Code: 0xF5BA ==> Row:85 Col:26 */
    {0x9E7F,0xB9C2}, /* ¹: GB2312 Code: 0xC2B9 ==> Row:34 Col:25 */
    {0x9E82,0xE4F7}, /* ��: GB2312 Code: 0xF7E4 ==> Row:87 Col:68 */
    {0x9E87,0xE5F7}, /* ��: GB2312 Code: 0xF7E5 ==> Row:87 Col:69 */
    {0x9E88,0xE6F7}, /* ��: GB2312 Code: 0xF7E6 ==> Row:87 Col:70 */
    {0x9E8B,0xE7F7}, /* ��: GB2312 Code: 0xF7E7 ==> Row:87 Col:71 */
    {0x9E92,0xE8F7}, /* ��: GB2312 Code: 0xF7E8 ==> Row:87 Col:72 */
    {0x9E93,0xB4C2}, /* ´: GB2312 Code: 0xC2B4 ==> Row:34 Col:20 */
    {0x9E9D,0xEAF7}, /* ��: GB2312 Code: 0xF7EA ==> Row:87 Col:74 */
    {0x9E9F,0xEBF7}, /* ��: GB2312 Code: 0xF7EB ==> Row:87 Col:75 */
    {0x9EA6,0xF3C2}, /* ��: GB2312 Code: 0xC2F3 ==> Row:34 Col:83 */
    {0x9EB4,0xF0F4}, /* ��: GB2312 Code: 0xF4F0 ==> Row:84 Col:80 */
    {0x9EB8,0xEFF4}, /* ��: GB2312 Code: 0xF4EF ==> Row:84 Col:79 */
    {0x9EBB,0xE9C2}, /* ��: GB2312 Code: 0xC2E9 ==> Row:34 Col:73 */
    {0x9EBD,0xE1F7}, /* ��: GB2312 Code: 0xF7E1 ==> Row:87 Col:65 */
    {0x9EBE,0xE2F7}, /* ��: GB2312 Code: 0xF7E2 ==> Row:87 Col:66 */
    {0x9EC4,0xC6BB}, /* ��: GB2312 Code: 0xBBC6 ==> Row:27 Col:38 */
    {0x9EC9,0xE4D9}, /* ��: GB2312 Code: 0xD9E4 ==> Row:57 Col:68 */
    {0x9ECD,0xF2CA}, /* ��: GB2312 Code: 0xCAF2 ==> Row:42 Col:82 */
    {0x9ECE,0xE8C0}, /* ��: GB2312 Code: 0xC0E8 ==> Row:32 Col:72 */
    {0x9ECF,0xA4F0}, /* �: GB2312 Code: 0xF0A4 ==> Row:80 Col:04 */
    {0x9ED1,0xDABA}, /* ��: GB2312 Code: 0xBADA ==> Row:26 Col:58 */
    {0x9ED4,0xADC7}, /* ǭ: GB2312 Code: 0xC7AD ==> Row:39 Col:13 */
    {0x9ED8,0xACC4}, /* Ĭ: GB2312 Code: 0xC4AC ==> Row:36 Col:12 */
    {0x9EDB,0xECF7}, /* ��: GB2312 Code: 0xF7EC ==> Row:87 Col:76 */
    {0x9EDC,0xEDF7}, /* ��: GB2312 Code: 0xF7ED ==> Row:87 Col:77 */
    {0x9EDD,0xEEF7}, /* ��: GB2312 Code: 0xF7EE ==> Row:87 Col:78 */
    {0x9EDF,0xF0F7}, /* ��: GB2312 Code: 0xF7F0 ==> Row:87 Col:80 */
    {0x9EE0,0xEFF7}, /* ��: GB2312 Code: 0xF7EF ==> Row:87 Col:79 */
    {0x9EE2,0xF1F7}, /* ��: GB2312 Code: 0xF7F1 ==> Row:87 Col:81 */
    {0x9EE5,0xF4F7}, /* ��: GB2312 Code: 0xF7F4 ==> Row:87 Col:84 */
    {0x9EE7,0xF3F7}, /* ��: GB2312 Code: 0xF7F3 ==> Row:87 Col:83 */
    {0x9EE9,0xF2F7}, /* ��: GB2312 Code: 0xF7F2 ==> Row:87 Col:82 */
    {0x9EEA,0xF5F7}, /* ��: GB2312 Code: 0xF7F5 ==> Row:87 Col:85 */
    {0x9EEF,0xF6F7}, /* ��: GB2312 Code: 0xF7F6 ==> Row:87 Col:86 */
    {0x9EF9,0xE9ED}, /* ��: GB2312 Code: 0xEDE9 ==> Row:77 Col:73 */
    {0x9EFB,0xEAED}, /* ��: GB2312 Code: 0xEDEA ==> Row:77 Col:74 */
    {0x9EFC,0xEBED}, /* ��: GB2312 Code: 0xEDEB ==> Row:77 Col:75 */
    {0x9EFE,0xBCF6}, /* ��: GB2312 Code: 0xF6BC ==> Row:86 Col:28 */
    {0x9F0B,0xBDF6}, /* ��: GB2312 Code: 0xF6BD ==> Row:86 Col:29 */
    {0x9F0D,0xBEF6}, /* ��: GB2312 Code: 0xF6BE ==> Row:86 Col:30 */
    {0x9F0E,0xA6B6}, /* ��: GB2312 Code: 0xB6A6 ==> Row:22 Col:06 */
    {0x9F10,0xBED8}, /* ؾ: GB2312 Code: 0xD8BE ==> Row:56 Col:30 */
    {0x9F13,0xC4B9}, /* ��: GB2312 Code: 0xB9C4 ==> Row:25 Col:36 */
    {0x9F17,0xBBD8}, /* ػ: GB2312 Code: 0xD8BB ==> Row:56 Col:27 */
    {0x9F19,0xB1DC}, /* ܱ: GB2312 Code: 0xDCB1 ==> Row:60 Col:17 */
    {0x9F20,0xF3CA}, /* ��: GB2312 Code: 0xCAF3 ==> Row:42 Col:83 */
    {0x9F22,0xF7F7}, /* ��: GB2312 Code: 0xF7F7 ==> Row:87 Col:87 */
    {0x9F2C,0xF8F7}, /* ��: GB2312 Code: 0xF7F8 ==> Row:87 Col:88 */
    {0x9F2F,0xF9F7}, /* ��: GB2312 Code: 0xF7F9 ==> Row:87 Col:89 */
    {0x9F37,0xFBF7}, /* ��: GB2312 Code: 0xF7FB ==> Row:87 Col:91 */
    {0x9F39,0xFAF7}, /* ��: GB2312 Code: 0xF7FA ==> Row:87 Col:90 */
    {0x9F3B,0xC7B1}, /* ��: GB2312 Code: 0xB1C7 ==> Row:17 Col:39 */
    {0x9F3D,0xFCF7}, /* ��: GB2312 Code: 0xF7FC ==> Row:87 Col:92 */
    {0x9F3E,0xFDF7}, /* ��: GB2312 Code: 0xF7FD ==> Row:87 Col:93 */
    {0x9F44,0xFEF7}, /* ��: GB2312 Code: 0xF7FE ==> Row:87 Col:94 */
    {0x9F50,0xEBC6}, /* ��: GB2312 Code: 0xC6EB ==> Row:38 Col:75 */
    {0x9F51,0xB4EC}, /* �: GB2312 Code: 0xECB4 ==> Row:76 Col:20 */
    {0x9F7F,0xDDB3}, /* ��: GB2312 Code: 0xB3DD ==> Row:19 Col:61 */
    {0x9F80,0xB3F6}, /* ��: GB2312 Code: 0xF6B3 ==> Row:86 Col:19 */
    {0x9F83,0xB4F6}, /* ��: GB2312 Code: 0xF6B4 ==> Row:86 Col:20 */
    {0x9F84,0xE4C1}, /* ��: GB2312 Code: 0xC1E4 ==> Row:33 Col:68 */
    {0x9F85,0xB5F6}, /* ��: GB2312 Code: 0xF6B5 ==> Row:86 Col:21 */
    {0x9F86,0xB6F6}, /* ��: GB2312 Code: 0xF6B6 ==> Row:86 Col:22 */
    {0x9F87,0xB7F6}, /* ��: GB2312 Code: 0xF6B7 ==> Row:86 Col:23 */
    {0x9F88,0xB8F6}, /* ��: GB2312 Code: 0xF6B8 ==> Row:86 Col:24 */
    {0x9F89,0xB9F6}, /* ��: GB2312 Code: 0xF6B9 ==> Row:86 Col:25 */
    {0x9F8A,0xBAF6}, /* ��: GB2312 Code: 0xF6BA ==> Row:86 Col:26 */
    {0x9F8B,0xA3C8}, /* ȣ: GB2312 Code: 0xC8A3 ==> Row:40 Col:03 */
    {0x9F8C,0xBBF6}, /* ��: GB2312 Code: 0xF6BB ==> Row:86 Col:27 */
    {0x9F99,0xFAC1}, /* ��: GB2312 Code: 0xC1FA ==> Row:33 Col:90 */
    {0x9F9A,0xA8B9}, /* ��: GB2312 Code: 0xB9A8 ==> Row:25 Col:08 */
    {0x9F9B,0xE8ED}, /* ��: GB2312 Code: 0xEDE8 ==> Row:77 Col:72 */
    {0x9F9F,0xEAB9}, /* ��: GB2312 Code: 0xB9EA ==> Row:25 Col:74 */
    {0x9FA0,0xDFD9}, /* ��: GB2312 Code: 0xD9DF ==> Row:57 Col:63 */
    {0xE810,0xFAD7}, /* ��: GB2312 Code: 0xD7FA ==> Row:55 Col:90 */
    {0xE811,0xFBD7}, /* �� : GB2312 Code: 0xD7FB ==> Row:55 Col:91 */
    {0xE812,0xFCD7}, /* ��: GB2312 Code: 0xD7FC ==> Row:55 Col:92 */
    {0xE813,0xFDD7}, /* �� : GB2312 Code: 0xD7FD ==> Row:55 Col:93 */
    {0xE814,0xFED7}, /* ��: GB2312 Code: 0xD7FE ==> Row:55 Col:94 */	
    {0xFF01,0xA1A3},/* ! */
    {0xFF02,0xA2A3},/* " */
    {0xFF03,0xA3A3},/*#*/
    {0xFF04,0xA4A3},/**/
    {0xFF05,0xA5A3},/*%*/
    {0xFF06,0xA5A6},/*&*/
    {0xFF07,0xA7A3},/* ' */
    {0xFF08,0xA8A3},/* ( */
    {0xFF09,0xA9A3},/* ) */
    {0xFF0A,0xAAA3},/* * */
    {0xFF0B,0xABA3},/* + */
    {0xFF0C,0xACA3},/*��*/
    {0xFF0D,0xADA3},/* - */
    {0xFF0E,0xAEA3},/* . */
    {0xFF0F,0xAFA3},/* / */
    {0xFF10,0xB0A3},/* 0 */
    {0xFF11,0xB1A3},/* 1 */ 
    {0xFF12,0xB2A3},/* 2 */
    {0xFF13,0xB3A3},/* 3 */
    {0xFF14,0xB4A3},/* 4 */
    {0xFF15,0xB5A3},/* 5 */
    {0xFF16,0xB6A3},/* 6 */
    {0xFF17,0xB7A3},/* 7 */
    {0xFF18,0xB8A3},/* 8 */
    {0xFF19,0xB9A3},/* 9 */  
    {0xFF1A,0xBAA3},/* : */
    {0xFF1B,0xBBA3},/* ; */
    {0xFF1C,0xBCA3},/* < */
    {0xFF1D,0xBDA3},/* = */
    {0xFF1E,0xBEA3},/* > */
    {0xFF1F,0xBFA3},/* ? */
    {0xFF3B,0xDBA3},/* [ */
    {0xFF3C,0xDCA3},/* \ */
    {0xFF3D,0xDDA3},/* ]  */
    {0xFF3E,0xDEA3},/* ^ */
    {0xFF3F,0xDFA3},/* _ */
    {0xFF5B,0xFBA3},/* { */
    {0xFF5C,0xFCA3},/* |  */
    {0xFF5D,0xFDA3},/* } */
    {0xFF5E,0xFEA3}, /* -  */    
	{0xFF6E, 0xFFA3} /* / */
    
};


static  int code_convert(char *from_charset,char *to_charset,char *inbuf,int inlen,char *outbuf,int outlen)
 {
	iconv_t cd;
	int rc,rel=0,i;
	char **pin = &inbuf;
	char **pout = &outbuf;
	cd = iconv_open(to_charset,from_charset);
	if (cd== (iconv_t)-1){
		return -1;
	}
	memset(outbuf,0,outlen);
	if (iconv(cd,pin,&inlen,pout,&outlen)==-1) 
		return -1;
	
	iconv_close(cd);
	return 0;
 }

/***_hanyi_2013/07/08 build
**filtrateCharacter to web 
*/
void filtrateCharacter(char *name,char *buf)
{
	if(name[0]>0 && name[0]<0x20)
	{
		if(name[0] == 0x10)
		{
			name+=3;
		}else{
			name++;
		}
	}

	#if 1
	strcpy(buf, name);
	#else
	while((*name) != '\0'){
		*buf++= *name++;
	}
	*buf = '\0';
	#endif
	
}

/*********************************************************************************************
*Authors & Time:_lixuezheng_2015/01/04_make function_
*Function :�ַ����16����
*Parameter:�ַ�
*Return :����
*********************************************************************************************/
 int String_into_Sixteen(u_char c)
{
    if (c>='0' && c<='9') return c - '0';
    if (c>='a' && c<='z') return c - 'a' + 10;//����+10��ԭ����:����16���Ƶ�aֵΪ10
    if (c>='A' && c<='Z') return c - 'A' + 10;
    
    printf("unexpected char: %c", c);
    return NON_NUM;
}


/*********************************************************************************************
*Authors & Time:_lixuezheng_2015/01/04_make function_
*Function : ���ַ���URL����,����������.
*Parameter:ԭ�ַ�����ԭ�ַ�����С������������\0����
����ַ����������������ַ�Ļ�������С(��������\0)
*Return :ʵ����Ч���ַ�������   (0 ����ʧ��)
*********************************************************************************************/
int URLDecode(const u_char* str, const int strSize, u_char* result, const int resultSize)
{
    unsigned char ch,ch1,ch2;
    int i;
    int j = 0;//record result index

    if ((str==NULL) || (result==NULL) || (strSize<=0) || (resultSize<=0)) {
        return 0;
    }

    for ( i=0; (i<strSize) && (j<resultSize); ++i) 
    {
        ch = str[i];
      
        switch (ch) 
        {
        /*
            case '+':
                result[j++] = ' ';
                break;
        */
            case '%':
                if (i+2<strSize) {
                    ch1 = String_into_Sixteen(str[i+1]);//��4λ
                    ch2 = String_into_Sixteen(str[i+2]);//��4λ

                    if ((ch1!=NON_NUM) && (ch2!=NON_NUM))
                      { 
                      	result[j++] = ((ch1<<4) | ch2)  ;
                      }
                       
                    i += 2;
                    break;
                } else {
                    break;
                }
            default:
               result[j++] = ch;
                break;
        }
    }
    
    result[j] = 0;

    return j;

}


/*********************************************************************************************
*Authors & Time:_lixuezheng_2014/11/22_make function_
*Function : gb2312ת��Ϊucs2
*Parameter:gb2312
*Return :ucs2����
*********************************************************************************************/
unsigned short gb2312_to_ucs2(unsigned short gb2312)
{
	int Index;

	for(Index = MAX_UNI_INDEX - 1; Index >= 0; Index--)
	{
		if(gb2312 == ucs2_gb2312_table[Index][1])
			return ucs2_gb2312_table[Index][0];
	}
	return 0;
}

#if 1

/*********************************************************************************************
*Authors & Time:_lixuezheng_2014/11/22_make function_
*Function : gb2312��ת��Ϊutf-8��
*Parameter:from_name,to_utf8_name
*Return :
*********************************************************************************************/
unsigned char *gb2312_to_utf8(const unsigned char *gb2312, unsigned char *utf)
{
	unsigned short unicodekey;
	unsigned short gb2312key;
	unsigned char *utf_base;
	
	utf_base = utf;
	while(*gb2312)
	{
		/* U-00000000 ~ U-0000007F --> 0xxxxxxx */
		if(gb2312[0] <= 0x7F)
		{
			utf[0] = gb2312[0];
			utf++;
			gb2312++;
			// printf("gb2312[0] = %#x\n", gb2312[0]);
			// printf("utf[0] = %#x\n", utf[0]);
		}
		else
		{
			/*U-00000800 ~ U-0000FFFF --> 1110xxxx 10xxxxxx 10xxxxxx*/
			/*GB2312 ռ2���ֽ�,����<<8 */
			gb2312key = ((unsigned short )gb2312[1])<<8
						|((unsigned short )(gb2312[0]));
			unicodekey = gb2312_to_ucs2(gb2312key);
			#if 0
			 printf("gb2312[0] = %#x\n", gb2312[0]);
			 printf("gb2312[1] = %#x\n", gb2312[1]);
			 printf("gb2312key = %#x\n", gb2312key);	
			 printf("unicodekey = %#x\n", unicodekey);
			#endif
			/* U-00000800 ~ U-0000FFFF --> 1110xxxx 10xxxxxx 10xxxxxx */
			if(0x0800 <= unicodekey)
			{
				
				utf[0] = (unicodekey>>12)|0x0E0;
				utf[1] = ((unicodekey&0x0FC0)>>6)|0x080;
				utf[2] = (unicodekey&0x3F)|0x080;
			#if 0
				 printf("utf[0] = %#x\n", utf[0]);
				 printf("utf[1] = %#x\n", utf[1]);
				 printf("utf[2] = %#x\n", utf[2]);
			#endif
				utf +=3;
				gb2312 += 2;
			}
			else
			{
				printf("gb2312 convert to utf8 , gb2312 code not found\n");
				return NULL;
			}
		}
	}
	//printf("utf_base:%s\n",utf_base);
	return utf_base;
}


/*************************************************************
*Authors & Time:_lixuezheng_2014/12/24 function_
* ˵��:
* UTF-8���ֽ�Ϊ��λ��Unicode���б��롣
* ��Unicode��UTF-8�ı��뷽ʽ���£�
* Unicode����(16����) --> UTF-8 �ֽ���(������) 
* U-00000000 ~ U-0000007F --> 0xxxxxxx  
* U-00000080 ~ U-000007FF --> 110xxxxx 10xxxxxx  
* U-00000800 ~ U-0000FFFF --> 1110xxxx 10xxxxxx 10xxxxxx  
* U-00010000 ~ U-001FFFFF --> 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx  
* U-00200000 ~ U-03FFFFFF --> 111110xx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx  
* U-04000000 ~ U-7FFFFFFF --> 1111110x 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx 
* �ʸ���ÿ�����ֵĵ�һ���ֽڣ�������֪��utf-8�����ɼ����ֽ����
*************************************************************/
int get_utf8_nbytes(unsigned char first_char)
{
	unsigned char temp = 0x080;
	int num = 0;

	while (temp & first_char)
	{
	    num++;
		//printf("in Get num=%d\n", num);
	    temp = (temp >> 1);
	}
	return num;
}

/*************************************************************
* �﷨��ʽ��unsigned short ucs2_to_gb2312(unsigned short ucs2)
* ʵ�ֹ��ܣ�ucs2תgb2312
* ������	ucs2: ��ת����ucs2����
* ����ֵ��	gb2312����
**************************************************************/
unsigned short ucs2_to_gb2312(unsigned short ucs2)
{
	int left = 0;
	int right = MAX_UNI_INDEX - 1;
	int middle;

	while(left <= right)
	{
		middle = (left+right)/2;
		if (ucs2 == ucs2_gb2312_table[middle][0])
			return ucs2_gb2312_table[middle][1];
		if (ucs2 > ucs2_gb2312_table[middle][0])
			left = middle + 1;
		else
			right = middle - 1;
	}
	/*����Ҳ������֣���ʹ��ȫ�ǵĿո����*/
	return 0xFED7;
}

/*********************************************************************************************
*Authors & Time:_lixuezheng_2014/12/24 function_
*Function : utf-8��ת��Ϊgb2312�� .
*Parameter:utf,gb2312
*Return : 0
*********************************************************************************************/
unsigned char *utf8_to_gb2312(const unsigned char *utf, unsigned char *gb2312)
{
	int num=0;
	unsigned short unicodekey;
	unsigned short gb2312key;
	unsigned char *gb2312_base;
	
	gb2312_base = gb2312;
	while(*utf)
	{
		/*U-00000000 ~ U-0000007F --> 0xxxxxxx*/
		if(utf[0] <= 0x7F)
		{
			gb2312[0] = utf[0];
			// printf("gb2312[0] = %#x\n", gb2312[0]);
			// printf("utf[0] = %#x\n", utf[0]);
			utf++;
			gb2312++;
		}
		else
		{
			num = get_utf8_nbytes(utf[0]);
			// printf("num = %d\n", num);
			/*U-00000800 ~ U-0000FFFF --> 1110xxxx 10xxxxxx 10xxxxxx*/
			if(num==3)
			{
				unicodekey = (((unsigned short)(((utf[0]&0x0f)<<4)|((utf[1]&0x3c)>>2)))<<8)
							|(((utf[1]&0x03)<<6)|(utf[2]&0x3F));
#if 0
	 printf("1111(utf[0]&0x0f) = %#x\n", (utf[0]&0x0f));
	 printf("2222((utf[0]&0x0f)<<4) = %#x\n", ((utf[0]&0x0f)<<4));
	 printf("1111 || 2222 = %#x\n", (((utf[0]&0x0f)<<4)|((utf[1]&0x3c)>>2)));
	 printf("(unsigned short) = %#x\n", ((unsigned short)(((utf[0]&0x0f)<<4)|((utf[1]&0x3c)>>2))));
	 printf("<<8 = %#x\n", (((unsigned short)(((utf[0]&0x0f)<<4)|((utf[1]&0x3c)>>2)))<<8));
#endif

				gb2312key = ucs2_to_gb2312(unicodekey);
				gb2312[0] = (unsigned char)(gb2312key&0x00ff);
				gb2312[1] = (unsigned char)((gb2312key&0xff00)>>8);

#if 0
 printf("utf[0] = %#x\n", utf[0]);
 printf("utf[1] = %#x\n", utf[1]);
 printf("utf[2] = %#x\n", utf[2]);
 printf("unicodekey = %#x\n", unicodekey);
 printf("gb2312key = %#x\n", gb2312key);	
 printf("gb2312[0] = %#x\n", gb2312[0]);
 printf("gb2312[1] = %#x\n", gb2312[1]);
#endif
				utf += num;
				gb2312 +=2;
			}
			else
			{
				printf("utf8 convert to gb2312 , gb2312 code not found\n");
				return NULL;
			}
		}
	}

	//printf("gb2312_base:___%X_%X_\n",*gb2312_base,*(gb2312_base+1));

	return gb2312_base;
}


/*********************************************************************************************
*Authors & Time:_lixuezheng_2014/12/24 function_
*Function : utf-8��ת��Ϊiso8859_1�� .
*Parameter:utf,iso8859_1
*Return : 0
*********************************************************************************************/
unsigned char *utf8_to_iso8859_1(const unsigned char *utf, unsigned char *iso8859_1)
{

	int num=0;
	unsigned char *iso8859_1_base;
	unsigned short unicodekey;

	iso8859_1_base = iso8859_1;

	while(*utf)
	{
	
		/*iso8859-1 ��Χ:0x00 ~ 0xFF */
		if(utf[0] <= 0x7F)
		{
			iso8859_1[0] = utf[0];
			utf++;
			iso8859_1++;
		}
		else if((utf[0] >= 0x80 ) && (utf[0] <= 0xFF))  
		{
	
		
			if(0xc3 == utf[0])     //��Ӧiso8859-1 ��0xC0 ~ 0xFF
			{
				iso8859_1[0] =(unsigned char)(utf[1]+0x40);
			}
	
			else if(0xc2 == utf[0])    //��Ӧiso8859-1 ��0x80 ~ 0xBF
			{
				iso8859_1[0] =(unsigned char)(utf[1]);
			}

			utf += 2;
			iso8859_1 ++;
		}
		else
		{
			printf("utf8 to ISO-8859-1, ISO-8859-1 code not found\n");
			return NULL;
		}
				
	}

	
	return iso8859_1_base;
}


/*********************************************************************************************
*Authors & Time:_lixuezheng_2014/12/24 function_
*Function : utf-8��ת��Ϊiso8859_5�� .
*Parameter:utf,iso8859_1
*Return : 0
*********************************************************************************************/
unsigned char *utf8_to_iso8859_5(const unsigned char *utf, unsigned char *iso8859_5)
{

	int num=0,flag = 0;
	unsigned char *iso8859_5_base;
	unsigned short unicodekey;

	iso8859_5_base = iso8859_5;

	while(*utf)
	{
	
		/*iso8859-1 ��Χ:0x00 ~ 0xFF */
		if(utf[0] <= 0x7F)
		{
			iso8859_5[0] = utf[0];
			utf++;
			iso8859_5++;
		}
		else if((utf[0] >= 0x80 ) && (utf[0] <= 0xFF))  
		{
			flag = 0;
			switch(utf[0])
			{
				case 0xC2:
					if(utf[1] == 0xA7 )
					{
						iso8859_5[0] =(unsigned char)(0xFD);
					}
					else
					{
						iso8859_5[0] =(unsigned char)(utf[1]);
					}
					flag = 1;
					break;
				case 0xD0:
					iso8859_5[0] =(unsigned char)(utf[1]+0x20);
					flag = 1;
					break;
				case 0xD1:
					iso8859_5[0] =(unsigned char)(utf[1]+0x60);
					flag = 1;
					break;	
				case 0xE2:
					iso8859_5[0] =(unsigned char)(0xF0);
					utf += 3;
					iso8859_5 ++;
					break;					
			}

			if(flag == 1)
			{
				utf += 2;
				iso8859_5 ++;
			}
		}
		else
		{
			printf("utf8 to ISO-8859-5, ISO-8859-5 code not found\n");
			return NULL;
		}
				
	}
	
	return iso8859_5_base;
}


/*********************************************************************************************
*Authors & Time:_lixuezheng_2014/11/22_make function_
*Function : iso8859-1��ת��Ϊutf-8  ��
*Parameter:from_name,to_utf8_name
*Return :
*********************************************************************************************/
unsigned char *iso8859_1_to_utf8(const unsigned char *iso8859_1, unsigned char *utf)
{
	unsigned char *utf_base;
	utf_base = utf;
	
	while(*iso8859_1)
	{

		if(iso8859_1[0] <= 0x7F)
		{
			utf[0] = iso8859_1[0];
			utf++;
			iso8859_1++;
		}
		else if((iso8859_1[0] >= 0x80 ) && (iso8859_1[0] <= 0xBF))  //utf-8[0] = 0xc2;
		{
			utf[0] = 0xc2;
			utf[1] = (unsigned char)iso8859_1[0];
			utf += 2;
			iso8859_1 ++;
		 }
		else if((iso8859_1[0] >= 0xC0 ) && (iso8859_1[0] <= 0xFF))   //utf-8[0] = 0xc3;
		{
			utf[0] = 0xc3;
			utf[1] = (unsigned char)(iso8859_1[0]-0x40);
			utf += 2;
			iso8859_1 ++;
		}
		else
		{
			printf("ISO-8859-1 to utf8 , ISO-8859-1 code not found\n");
			return NULL;
		}


	}


	return utf_base;
}
unsigned char *iso8859_5_to_utf8(const unsigned char *iso8859_5, unsigned char *utf)
{
	unsigned char *utf_base;
	utf_base = utf;
	
	while(*iso8859_5)
	{

		if(iso8859_5[0] <= 0x7F)
		{
			utf[0] = iso8859_5[0];
			utf++;
			iso8859_5++;
		}
		else if((iso8859_5[0] >= 0x80 ) && (iso8859_5[0] <= 0xA0))  //utf-8[0] = 0xc2;
		{
			utf[0] = 0xc2;
			utf[1] = (unsigned char)iso8859_5[0];
			utf += 2;
			iso8859_5 ++;
		}
		else if((iso8859_5[0] >= 0xA1 ) && (iso8859_5[0] <= 0xDF))  //utf-8[0] = 0xd0;
		{
			if(iso8859_5[0] == 0xAD)   //special
			{
				utf[0] = 0xc2;
				utf[1] = 0xAD;			
			}
			else						//normal
			{
				utf[0] = 0xd0;
				utf[1] = (unsigned char)iso8859_5[0]-0x20;
			}
			utf += 2;
			iso8859_5 ++;
		}
		else if(iso8859_5[0] >=0xE0 && iso8859_5[0] <= 0xFF)
		{
			if(iso8859_5[0] == 0xF0) // 0xF0 	0xE2 0x84 0x96  (special)
			{
				utf[0] = 0xe2;
				utf[1] = 0x84;
				utf[2] = 0x96;
				utf += 3;
				iso8859_5 ++;
			}
			else
			{
				if(iso8859_5[0] == 0xFD)	//special
				{
					utf[0] = 0xc2;
					utf[1] = 0xa7;
				}
				else						//normal
				{
					utf[0] = 0xd1;
					utf[1] = (unsigned char)iso8859_5[0]-0x60;
				}
				utf += 2;
				iso8859_5 ++;
			}
	}
		else
		{
			printf("ISO-8859-5 to utf8, ISO-8859-5 code not found\n");
			return NULL;
		}
	}

	return utf_base;
}

/*********************************************************************************************
*Authors & Time:_lixuezheng_2014/10/08_make function_
*Function : Special_Symbol_Convert for Encoder Top_menu OEM and Device.
*Parameter:Maybe need  covert  name
*Return : 0
*********************************************************************************************/
void  Special_Symbol_Convert(u_char *need_covert_name,u_char *after_covert_name,int num)
{	
	int i;
	char *p = NULL;

	p = after_covert_name;

	//special_symbol need convert ( & is 0x26 )( < is 0x3c )( > is 0x3e )( " is 0x22 )( ' is 0x27 )
	for(i = 0;i<num ;i++)
	{
		if(need_covert_name[i] == 0x26)
		{
			sprintf(p,"&#38;");//&#38;//&amp;&
			p=p+5;
		}
		else if(need_covert_name[i] == 0x3c)
		{
			sprintf(p,"&#60;");//&#60;//&lt;<
			p=p+5;
		}
		else if(need_covert_name[i] == 0x3e)
		{
			sprintf(p,"&#62;");//&#62;//&gt;>
			p=p+5;
		}
		else if(need_covert_name[i] == 0x22)
		{
			sprintf(p,"&#34;");//&#34;//&quot;"
			p=p+5;
		}		
		else if(need_covert_name[i] == 0x27)//Audio Bit Rate
		{
			sprintf(p,"&#39;");//&#39;//&apos;'
			p=p+5;
		}	
		else
		{
			*p = need_covert_name[i];
			p++;
		}
	}

}

int get_codetype(u_char *str)
{	

	if(str[0]>0 && str[0]<0x20)
	{

		switch(str[0])
		{
		
			case 0x13:
				return 2;
			case 0x15 :
				return 1;
			case 0x10:
				return 3;
			case 0x01: 
				return 4;
			case 0x11:
				return 5;
			case 0x14:
				return 6;
			default:
				return 0;
				break;
		}
	}
	else if(str[0]>=0x20 && str[0]<0x7f)
	{
		return 1;
	}
	else
	{
		return 2;
	}

}

/*********************************************************************************************
*Authors & Time:_lixuezheng_2014/09/22_make function_
*Function : name_convert_to_utf8
*Parameter:from_name,to_utf8_name
*Return : 0
*********************************************************************************************/
void name_convert_to_utf8(u_char *from_name,u_char *to_utf8_name)
{	

#if 0
	int i;

	for(i=0;i<10;i++)
	{
		printf("__11_____%X____\n",from_name[i]);
	}
#endif

	//printf("__service  name byte[0]  %#x\n",from_name[0]);

	if(from_name[0]>0 && from_name[0]<0x20)
	{

		switch(from_name[0])
		{
		
			case 0x13:
				/*__2_gb2312_0x13___*/	
				gb2312_to_utf8(&from_name[1],to_utf8_name);
				break;
			case 0x01: // ISO-8859-5
				iso8859_5_to_utf8(&from_name[1], to_utf8_name);
				break;
			case 0x10:
				/*__3_iso-8859-1_0x10___*/
				iso8859_1_to_utf8(&from_name[1], to_utf8_name);
				break;

			case 0x11:
			case 0x14:
				 /*_4_big5_0x11__&&__5_ISO_10646_0x14*/
				code_convert("UCS-2BE","utf-8",&from_name[1],32,to_utf8_name,32);
				break;
			default:
				filtrateCharacter(from_name,to_utf8_name);
				break;
		}
	}
	else if(from_name[0]>=0x20 && from_name[0]<0x7f)
	{
		filtrateCharacter(from_name,to_utf8_name);
		//iso8859_1_to_utf8(from_name, to_utf8_name);
	}
	else
	{
		if(gb2312_to_utf8(from_name, to_utf8_name) == NULL)
		{
			filtrateCharacter(from_name,to_utf8_name);
		}

	}
#if 0
	int i;

	for(i=0;i<10;i++)
	{
		printf("_______%X____\n",to_utf8_name[i]);
	}
	printf("_____%s__%d__\n",to_utf8_name,strlen(to_utf8_name));
#endif	

}

/*********************************************************************************************
*Authors & Time:_lixuezheng_2014/09/22_make function_
*Function : utf8_to_other_convert for Encoder
*Parameter:character_selection,from_utf8_name,to_other_name
*Return : 0
*********************************************************************************************/
void utf8_to_other_convert(int character_selection,u_char *from_utf8_name,u_char *to_other_name)
{	
int i,cd;
		
#if 0

	for(i=0;i<10;i++)
	{
		printf("__11_____%X____\n",from_utf8_name[i]);
	}
	printf("__11_____%s__%d__\n",from_utf8_name,strlen(from_utf8_name));
#endif
	
	switch(character_selection)
	{

		case 1:
			
			/*__1_UTF8_0x15___*/
			to_other_name[0] = 0x15;
			bcopy(from_utf8_name, &to_other_name[1], 19);
			break;

		case 2:

			/*__2_gb2312_0x13___*/	
			to_other_name[0] = 0x13;
			utf8_to_gb2312(from_utf8_name, &to_other_name[1]);
			break;
	
		case 3:
			/*__3_iso-8859-1_0x10___*/
			to_other_name[0] = 0x10;
			utf8_to_iso8859_1(from_utf8_name,&to_other_name[1]);
			break;
		case 4:
			/*__3_iso-8859-1_0x10___*/
			to_other_name[0] = 0x01;
			utf8_to_iso8859_5(from_utf8_name,&to_other_name[1]);
			break;

		case 5:
			
			 /*____4_big5_0x11________*/
			 to_other_name[0] = 0x11;
			 code_convert("utf-8","UCS-2BE",from_utf8_name,strlen(from_utf8_name),&to_other_name[1],18);
			break;
		case 6:			
			 /*___5_ISO_10646_0x14______*/
			 to_other_name[0] = 0x14;
			 code_convert("utf-8","UCS-2BE",from_utf8_name,strlen(from_utf8_name),&to_other_name[1],18);
			break;
		default:
			bcopy(from_utf8_name,to_other_name,20);
			break;
	}
#if 0
	for(i=0;i<10;i++)
	{
		printf("__22_____%X____\n",to_other_name[i]);
	}
	printf("__22_____%s__%d__\n",to_other_name,strlen(to_other_name));
#endif
	
}
#endif