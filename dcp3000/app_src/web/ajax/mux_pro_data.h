

#include "mux.h"

#define START_LOG 0
#define LOG_COUNT 5


extern char *web_json_text;

v_32  Get_Deviceinfo_func(cJSON * fmt);
v_32 Get_Upgrade_Percent_func(cJSON * fmt, int board_id);


//#define random(x) (rand()%x)

#if 1
v_32  Get_BoardInfo_func(cJSON * fmt);
#endif
v_32 Get_Log_Event(cJSON *fmt);
v_32 Get_Main_Log_Event(cJSON *fmt);

v_32 Get_Hardware_Status(cJSON *fmt);
v_32 Get_IPIN_Status(cJSON *fmt);
v_32 Get_IPOUT_Status(cJSON *fmt);

v_32 Set_Login_func(cJSON *val_arr_wrap);


v_32 Get_SysPara_func(cJSON * fmt);
v_32 Set_SysPara_func(v_u32 id, cJSON * val_arr_wrap);

v_32 Get_ManNet_func(cJSON * fmt);
v_32 Set_ManNet_func(v_u32 id, cJSON * val_arr_wrap);
v_32 Set_NTP_func(v_u32 id, cJSON * val_arr_wrap);


v_32 Get_Clk_func(cJSON * fmt);
v_32 Set_Clk_func(v_u32 id, cJSON * val_arr_wrap);

v_32 Get_Version_func(cJSON * fmt);

v_32 Get_Iplocal_func(cJSON * fmt);
v_32 Get_IpOut_func(cJSON * fmt);
v_32 Get_Ipin_func(cJSON * fmt);

v_32 Set_Ipin_func(v_u32 id, cJSON * val_arr_wrap);
v_32 Set_Ipout_func(v_u32 id, cJSON * val_arr_wrap);
v_32 Set_Iplocal_func(v_u32 id, cJSON * val_arr_wrap);

v_32 Get_Transceiver_func(cJSON * fmt);
v_32 Set_Transceiver_func(v_u32 id, cJSON *val_arr_wrap);

v_32 Set_Transceiver_Multi_func_Clean(int dest_trans_card, cJSON *val_arr_wrap);
v_32 Set_Transceiver_Multi_func_NoClean(int dest_trans_card, cJSON *val_arr_wrap);

v_32 Clear_Log_Message(v_u32 id, cJSON *val_arr_wrap);

v_32 Get_Remux_BasicInfo_func(cJSON * fmt);
v_32 Get_Input_Tree_func(cJSON * fmt);
v_32 Get_Mux_Tree_func(cJSON * fmt);
v_32 Get_Program_Infos_func(cJSON * fmt);
v_32 Get_Pid_Mapping_func(cJSON * fmt);
v_32 Set_Mux_Infos_func(cJSON * val_arr_wrap);
v_32 Set_FILTER_Infos_func(cJSON * val_arr_wrap);
v_32 Set_Prog_Infos_func(cJSON * val_arr_wrap);
v_32 Set_Remap_Infos_func(cJSON * val_arr_wrap);


int my_system(const char * cmd);


