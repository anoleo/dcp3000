#ifndef MUX_H_
#define MUX_H_
#include "charset_ch.h" 
#include "mux_json_macros.h"
#include "mux_data_macros.h"
#include "cJSON.h"

#include "webmuxmsg.h"
#include "v_type.h"

#include "../webserver.h"
//#include "../../goahead/goahead.h"

#define CGI_VERSION				0

#define DEBUG				0
#define DE_TEST				0
#define CH_DEBUG			0
#define DE_JSON				0
#define DE_GIVE				0
#define DE_OUT				0

#define IPIN_MIN_CH					10
#define DEFAULT_PORT_MASK			1

#if		DEBUG
#define DEBUG_PRINT(fmt,...)			fprintf(fp_uart,fmt ,__VA_ARGS__)
#define DEBUG_N_PRINT(fmt)			fprintf(fp_uart,fmt)
#define DEBUG_CGIPRT(out,fmt,...)		fprintf(out,fmt, __VA_ARGS__)
#else
#define DEBUG_PRINT(fmt,...)	
#define DEBUG_N_PRINT(fmt)			
#define DEBUG_CGIPRT(out,fmt,...)
#endif

#if		DE_GIVE
#define DE_GIVE_PRINT(fmt,...)			fprintf(fp_uart,fmt ,__VA_ARGS__)
#define DE_GIVE_N_PRINT(fmt)			fprintf(fp_uart,fmt)
#define DE_GIVE_CGIPRT(out,fmt,...)		fprintf(out,fmt, __VA_ARGS__)
#else
#define DE_GIVE_PRINT(fmt,...)	
#define DE_GIVE_N_PRINT(fmt)			
#define DE_GIVE_CGIPRT(out,fmt,...)
#endif

#if		DE_OUT
#define DE_OUT_PRINT(fmt,...)			fprintf(fp_uart,fmt ,__VA_ARGS__)
#define DE_OUT_N_PRINT(fmt)			fprintf(fp_uart,fmt)
#define DE_OUT_CGIPRT(out,fmt,...)		fprintf(out,fmt, __VA_ARGS__)
#else
#define DE_OUT_PRINT(fmt,...)	
#define DE_OUT_N_PRINT(fmt)			
#define DE_OUT_CGIPRT(out,fmt,...)
#endif

#if	(DE_GIVE & DEBUG)
#define PRINT_JSON(P_TEXT, P_JSON)	do{P_TEXT = cJSON_Print(P_JSON);\
	DE_GIVE_PRINT("%s\n",P_TEXT);\
	free(P_TEXT);\
	P_TEXT=NULL;\
	}while(0)
#else
#define PRINT_JSON(P_TEXT, P_JSON)
#endif

#if	(DE_GIVE)
#define PRINT_GIVE_JSON(P_TEXT, P_JSON)	do{P_TEXT = cJSON_Print(P_JSON);\
	printf("%s\n",P_TEXT);\
	free(P_TEXT);\
	P_TEXT=NULL;\
}while(0)
#else
#define PRINT_GIVE_JSON(P_TEXT, P_JSON)
#endif	


#define CA_SERVER_NUM		4
#define CA_KEY_MUX_LEN		20

#define DEV_NAME_LENGTH	20
#define LOGIN_NAME_LENGTH	12
#define DEV_SN_LENGTH		20
#define TIME_LENGTH			14

#define MAX_JSON_LENGTH	(64*1024)
#define WEB_TYPE			100
#define MUX_MSG_NUM		50
#define V_ITEM_LEN			16
#define PIDS_LEN				10
#define LOG_NUM				100

#define FORM_DATA_NAME_LENGTH	40
#define CARSET_NAME_LENGTH	32

/******************************************/
#define PAGE_TYPE_MASK		0xfff

#define P_INDEX				0x0
#define P_INPUT				0x1
//#define P_MUX				0x2
#define P_SYSTEM			0x3
#define P_STATUS			0x4
#define P_MISC				0x5
//#define P_CA					0x6
//#define P_QAM				0xa
//#define P_CA_STATUS			0xb

//#define POP_MUX_NODE		0x7
#define UPLOAD_PARA		0x8
#define GET_ALL_LOG			0x9


//LAST = b
/******************************************/
//MSG TYPE	bit[0]write or read, bit[1:7]id_type
#define FORM_T				0x1
#define FORM_W				0x3
#define FORM_R				0x2

#define DATA_T				0x0
#define DATA_W				0x1
#define DATA_R				0x0
/*
#define MSG_SYSTEM			1
#define MSG_WEB_PARA		2
#define MSG_TSOIP_PARA		3
#define MSG_TSOIP_IN_CH	4
#define MSG_TSOIP_OUT_CH	5
#define MSG_ASI_IN_PARA	6
#define MSG_ASI_OUT_PARA	7
#define MSG_MODULATION	8
#define MSG_TS_IN			9
#define MSG_MUX				10
#define MSG_CAS				11*/
/******************************************/

/*Set for anaJsonSection*/
//INPUT
#define SHOW_SET			0
#define SET_IPIN			1
#define PORT_STATUS			2

//CARD Infor
#define INPUT_TREE			5
#define CARD_INFO			6
//ININT
#define INIT				7

//SYS
#define MAN_NET				8
#define CLK_SET				9
#define LOGIN_SET			10
#define SYS_PARA			11

//MISC
#define VERSION				12
#define CMD					13
#define LOG_MSG				14
//CA
#define CA_SERVER			15
#define CA_ACCESS_CRIERIA	16
//STATUS
#define MUX_BITRATE			17	//0x0a0002
#define TSOIP_STATUS		18	//0x030003  
#define UPGRADE_STATUS		20

//WEB SETTING
#define WEB_SETTING		21


#define TSIP_IN_SET				22
#define TSIP_OUT_SET			23
#define TSIP_LOCAL_SET			24
//Add Transceiver Route
#define TRANSCEIVER_SET 		25


#define CLEAR_LOG				26
// NTP
#define NTP_SET					27
#define TRANSCEIVER_MULTI_SET	28

#define MUXINFO								29
#define FILTERINFO							30
#define PROG_INFOS_SET_FORM_ID			31
#define REMAP_INFOS_SET_FORM_ID			32

#define TSIPIN_BASIC_SET_FORM_ID                 33
#define MISC_DEFAULT_PARAM_FORM_ID             34
#define MISC_RESET_SONBOARD_FORM_ID             36
#define WATCHDOG							37
#define CAPTUREINFOS						38



/*End Set for anaJsonSection*/

/*for proJsonInfo */
#define IPIN_CHANNEL_IDX_GET					28
#define IPIN_IDX_CHANNEL_GET					29
#define INIT_INPUT_TREE_FORM_ID		  	7
#define INIT_Edit_Prog_Infos_FORM_ID			  	8
#define Get_MUX_SET_INFOS_FORM_ID			  	9
#define Get_Mux_Refresh_Infos_FORM_ID			  	10


#define BASIC_INFO_FORM_ID					33


#define REMUX_SRC_PROGRAM_PAGE			0x09


#define MISC_SAVE_PARAM_ID             0x10
/********************Module PAGE ID **********************/
#define SLOT1_PAGE_ID		0x21
#define SLOT2_PAGE_ID		0x22
#define SLOT3_PAGE_ID		0x23
#define SLOT4_PAGE_ID		0x24

#define SLOT5_PAGE_ID		0x25
#define SLOT6_PAGE_ID		0x26
#define SLOT7_PAGE_ID		0x27
#define SLOT8_PAGE_ID		0x28

#define WEB_SND_SPI_JSON			0x101
#define WEB_RCV_SPI_JSON			0x102


#define CARD_STATUS_INFO		0x100
#define CARD_PARA_INFO			0x101
#define CARD_PARA_SET			0x102


/******************************************/
//LSAT=21

/******************SET_PORT type***************/
#define SET_PORT_ALL		1
#define SET_PORT_BITRATE	0

/*******************mcu***********************/
//INPUT
#define PARA_IP_CH		2


/******************************************/
#define INDEX_TYPE_FLAG	12
#define INDEX_TYPE_MASK	0xf
#define JSON_TYPE			0
#define GET_FILE				1

/******************************************/
#define READ					0
#define WRITE				1
#define OK					2
#define ERROR				3

/******************************************/
#define JSON_NAME			""
/******************************************/

/******************************************/	
//define func
/******************************************/


#define GET_INT_JARRAY(p_json,index) (cJSON_GetArrayItem((p_json),(index))->valueint)
#define GET_DOUBLE_JARRAY(p_json,index) (cJSON_GetArrayItem((p_json),(index))->valuedouble)
#define GET_STR_JARRAY(p_json,index) (cJSON_GetArrayItem((p_json),(index))->valuestring)


/******************************************/

//JSON STRUCT
/******************************************/
//point
typedef struct _json_info_t * p_json_info_t;
typedef struct _id_map_t* p_id_map_t;
typedef struct _pro_id_t * p_pro_id_t;

typedef struct _web_setting_t{
	v_u8 input_p_show;	
	v_u8 input_port;
	v_u8 version;
	v_u8 instr;
	v_u8 outstr;
}web_setting_t;


typedef struct _pro_id_t{
	v_u32 id;
	v_u16 sidex;
	v_u16 item_num;
	p_pro_id_t next;
}pro_id_t;
//info name struct
typedef struct _id_map_t{
	char name[FORM_DATA_NAME_LENGTH];
	v_u32 id;
	//JsonFunc_t JsonFunc;
	p_pro_id_t p_pro_id;
	p_id_map_t next;
}id_map_t;

//
typedef struct _json_info_t{
	v_u8	data_flag;//	1:form 2:data
	v_u8	item_num;
	v_u8	type;
	v_16	sidex;

	v_u32	mux_ena;
	
	p_id_map_t p_id_map;
	
	cJSON * p_json_section;
	cJSON * 	val_arr_wrap;

	p_json_info_t next;
}json_info_t;

typedef struct _charset_type_t{
	v_u8	id;
	v_u8	first_byte;	
	char		charset_str[CARSET_NAME_LENGTH];
	struct _charset_type_t *	next;
}charset_type_t;

typedef struct _global_data_t{



	char *json_text;
		
}global_data_t;

/******************************************/



v_u32 getMenuId();
//v_u32 getPageType(webs_t wp);
//v_u8 getRPage(webs_t wp, v_u32 id);
//init
//int global_init(webs_t wp);
//free
void free_json_info_list(p_json_info_t * p_json_info_list);
void free_id_map_list(p_id_map_t * p_id_map_list);
void free_pro_id_list(p_pro_id_t * p_pro_id_list);
//json
int default_info_list(p_json_info_t * info_list, v_u16 form_id);
void mapping_wrapper();
p_id_map_t find_id_map(v_u32 id,v_u8 data_flag);
//shift
//void shift_paging(paging_info_t *p_paging, int data_type, p_json_info_t info);
int shift_default(v_u32 data_type);
//msg
char *str_to_ch(char * json_str,char *buf, int len);
char *GET_CH_JARRAY(cJSON * p_json, int index,char *buf, int len);
//charset
charset_type_t * gen_charset_list();
charset_type_t *  find_charset(v_u8 id, char * * p_char_add);
charset_type_t * check_str_first_byte(v_u8 f_byte, v_u8 id);
void free_charset_list();
#endif
