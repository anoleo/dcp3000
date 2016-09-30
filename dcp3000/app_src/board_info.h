#ifndef	__BOARD_INFO_H__
#define	__BOARD_INFO_H__

#include "utils/sArrList.h"
#include "utils/utils.h"


//*<!--channels type , 0 abstract interface, 1 one interface with more channels, 2 one inteface with one channel, 3 one channel maps more interface-->*//
#define ABSTRACT_CHANIF	0
#define ONE2MANY_CHANIF	1
#define ONE2ONE_CHANIF		2
#define MANY2MANY_CHANIF	3
#define MANY2ONE_CHANIF	4		


#define DEVIF_NAME_LEN		20
struct device_interface{
	u_int type;
	char name[DEVIF_NAME_LEN];
	int count;
};


struct func_channel{
	char trans_mode;
	u_char type;
	char ifno;
	char start_ifno;
	char end_ifno;
	short start_index;
	short end_index;
	struct device_interface *devif;
};


#define FUNC_NAME_LEN		20
struct function_module{
	u_int type;
	char name[FUNC_NAME_LEN];
	int channel_count;
	sArrList_st *channel_list;
};


#define BOARD_NAME_LEN		20
#define BOARD_DESC_LEN		40
struct board_card{
	int slot;
	u_int type;
	u_int version;
	char name[BOARD_NAME_LEN];
	char descriptions[BOARD_DESC_LEN];
	sArrList_st *devif_list;
	sArrList_st *func_list;
};

typedef struct device_interface		devif_st;
typedef struct func_channel		funcchan_st;
typedef struct function_module		funcmod_st;
typedef struct board_card			board_st;

/*
 ������*.xml����İ忨��Ϣ������Ϊ*.xml�ļ���·����������ɺ�᷵��һ��board_st�ṹ������ָ��
//*/
board_st *parse_board_info(char *board_file);
void free_boardinfo(struct board_card *pboard);

funcmod_st *get_board_funcmod(board_st *board, u_int func_type);

#define board_deviflist(board)			(sArrList_st *)((void *)(board) + (u_int)((board)->devif_list))
#define board_funclist(board)			(sArrList_st *)((void *)(board) + (u_int)((board)->func_list))
#define func_chanlist(func)			(sArrList_st *)((void *)(func) + (u_int)((func)->channel_list))
#define funcchan_devif(chan)			(devif_st *)((void *)(chan) - (u_int)((chan)->devif))

/*
	��board_st �ṹ�е�������䵽һ������buffer�У�������������ݵĳ���
//*/
int fill_board_info_into_buf(board_st *board, char *data_buf);
/*
	��board_st ������channel������devif ���ͱ���滻Ϊ��Ӧdevif ������ָ������
//*/
//void match_channel_devif(board_st *board);


void print_devif(void *data);
void print_funcmod(void *data);
void print_funcchan(void *data);
void print_boardinfo(struct board_card *pboard);

#endif
