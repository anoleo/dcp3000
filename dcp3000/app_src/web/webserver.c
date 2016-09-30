 
#include "../appref.h"
#include "webserver.h"
#include "../event/board_event.h"



#include "ajax/cJSON.h"
#include "ajax/mux.h"
#include "ajax/mux_pro_data.h"



static  int websSetUserAction(HttpConn *conn) 
{

	httpLogout(conn);
	httpRedirect(conn, HTTP_CODE_MOVED_TEMPORARILY, "/");

	return set_web_user(conn, sysinfo->loginuser, sysinfo->loginpasswd);
}

int get_web_request_flag(char *json_text, int page_id)
{
	int type;
	type = (strlen(json_text)==0)?0:1;
	DEBUG_PRINT("get_web_request_id :page_id = %#x, type = %d\n\n",page_id,type);
	
	return (type & 0xff);
}

void printJson(cJSON * p_json)
{
	char *json_text;

	
	json_text =cJSON_Print(p_json);
	
	DebugPrintf("-- json_text :  %s \n", json_text);

	free(json_text);
}

//analyse json section
void anaJsonSection(cJSON * p_json){
	cJSON * p_temp_json;
	v_u8 type=0;
	v_u32 id=0; 
	cJSON * val_arr_wrap=NULL,* val_arr_wrap1=NULL;
	cJSON *rootJson, *fmt1,*dim1; 
	int len;

	if(p_temp_json = cJSON_GetObjectItem(p_json, "form_id")){
		id = p_temp_json->valueint;
		type = FORM_T;
	}else if(p_temp_json = cJSON_GetObjectItem(p_json, "data_id")){
		id = p_temp_json->valueint;
		type = DATA_T;
	}

	DebugPrintf("==anJsonSection:id=%d(type = %d)\n", id,type);
	if(type & 0x1){
		val_arr_wrap = cJSON_GetObjectItem(p_json, "value");
		if(val_arr_wrap == NULL){
			if(id != MISC_DEFAULT_PARAM_FORM_ID)
			{
			printf("request form : value is null\n");
			return;
		}
		}

		switch(id)
		{
			case SYS_PARA:
				Set_SysPara_func(id, val_arr_wrap);
				break;
			case MAN_NET:
				Set_ManNet_func(id, val_arr_wrap);
				break;
			case CLK_SET:
				Set_Clk_func(id, val_arr_wrap);
				break;					
			case TSIP_IN_SET:
				Set_Ipin_func(id, val_arr_wrap);
				val_arr_wrap1 = cJSON_GetObjectItem(p_json, "ipin_channel_map");
				Set_Ipin_idx_ref_func(id,val_arr_wrap1);
				break;
			case TSIPIN_BASIC_SET_FORM_ID:
				Set_Tsipin_Basic_Infos_func(val_arr_wrap);
				break;
			case TSIP_OUT_SET:
				Set_Ipout_func(id, val_arr_wrap);
				val_arr_wrap1 = cJSON_GetObjectItem(p_json, "ipout_channel_map");
				Set_Ipout_idx_ref_func(id,val_arr_wrap1);
				break;
			case TSIP_LOCAL_SET:
				Set_Iplocal_func(id, val_arr_wrap);
				break;
			case TRANSCEIVER_SET:
				if(p_temp_json = cJSON_GetObjectItem(p_json, "dest_card")){
					Set_Transceiver_Multi_func_NoClean(p_temp_json->valueint, val_arr_wrap);
				}else{
				Set_Transceiver_func(id, val_arr_wrap);
				}
				break;
			case TRANSCEIVER_MULTI_SET:
				if(p_temp_json = cJSON_GetObjectItem(p_json, "dest_card")){
					Set_Transceiver_Multi_func_Clean(p_temp_json->valueint, val_arr_wrap);
				}else{
					printf("wrong json...\n");
				}
				break;
			case CLEAR_LOG:
				//Clear_Log_Message(id, val_arr_wrap);
				break;
			case CARD_INFO:
				break;
			case NTP_SET:
				Set_NTP_func(id, val_arr_wrap);
				break;
			case WATCHDOG:
				Set_DOG_func(id, val_arr_wrap);
				break;				
			case MUXINFO:
				Set_Edit_Program_Infos_func(val_arr_wrap);
				break;
			case FILTERINFO:
				Set_FILTER_Infos_func(val_arr_wrap);
				break;				
			case PROG_INFOS_SET_FORM_ID:
				Set_Prog_Infos_func(val_arr_wrap);
				break;
			case REMAP_INFOS_SET_FORM_ID:
				Set_Remap_Infos_func(val_arr_wrap);
				break;				
			case MISC_DEFAULT_PARAM_FORM_ID:
				default_setting();	
				break;
			case MISC_RESET_SONBOARD_FORM_ID:
				ResetSonBoard();								
				break;
			case CAPTUREINFOS:
				CaptureInfos();
				break;
			default:
				break;
		}	
	}	
	DebugPrintf("==anJsonSection:data_type=%s,id=%x(%d),value=%d\n",((type)?"form":"data"),id,id, type&1);
}
char web_databuf[SZ_1M];
//parse json 
void parseJsonInfo(char *json_text)
{
	cJSON * p_get_json;
	cJSON *p_json;
	//printf("json_text = %s\n", json_text);
	p_get_json = cJSON_Parse(json_text);
	p_json = p_get_json->child;

	while(p_json){
		anaJsonSection(p_json);
		p_json = p_json->next;
	}
	cJSON_Delete(p_get_json);	
}
/*
** return 0:success  -1:error
*/
int proSubWrRequest(char *json_text,  v_u32 slot_id)
{
	cJSON * p_get_json;
	cJSON *p_json;
	int   id, len = 0, send_result = 0;
	
	p_get_json = cJSON_Parse(json_text);
	p_json = p_get_json->child;

	if(p_json){
		len = spi_send_json(slot_id, WEB_SND_SPI_JSON, json_text);
		if(len == -1){
			printf("==send_json form SPI Error===\n");
			send_result = -1;
		}
	}

	cJSON_Delete(p_get_json);	
	return send_result;
}

 
//process json info
char *proJsonInfo(v_u32 id, char *read_par)
{
	char *json_text = NULL, *sub_json_text = NULL;
	cJSON *rootJson, *fmt1, *dim1;  
	int slot_id, msg_length;
	int len;

//creat root json
	DebugPrintf("request id=%d\n", id);
	rootJson=cJSON_CreateObject();
	switch(id)
	{
		case 0x03:
			Get_Deviceinfo_func(rootJson);
			Get_SysPara_func(rootJson);
			Get_ManNet_func(rootJson);
			Get_Clk_func(rootJson);
			break;
		case 0x04:
			Get_Deviceinfo_func(rootJson);
			Get_Log_Event(rootJson);
		//	Get_Main_Log_Event(rootJson);
			Get_Hardware_Status(rootJson);
			Get_IPIN_Status(rootJson);
			Get_IPOUT_Status(rootJson);
			break;
		case 0x05:
			Get_SysPara_func(rootJson);
			Get_Version_func(rootJson);
			break;
		case 0x06:
			Get_Ipin_func(rootJson);
			Get_ipin_idx_ref_func(rootJson);
			break;
		case 0x07:
			Get_IpOut_func(rootJson);
			Get_Transceiver_func(rootJson);
			break;
		case 0x08:
			Get_Iplocal_func(rootJson);
			break;
		case REMUX_SRC_PROGRAM_PAGE:
			Get_Remux_BasicInfo_func(rootJson);
			Get_Input_Tree_func(rootJson);
			Get_Edit_Prog_Infos_func(rootJson);
			Get_FILTER_Infos_func(rootJson);
			Get_Mux_Refresh_Infos_func(rootJson);
			break;						
			
		case 0x20:
		case 0x21:
		case 0x22:
		case 0x23:
		case 0x24:
		case 0x25:
		case 0x26:
		case 0x27:
			slot_id = (id & 0x0f); // 子板号
			
			if(sysinfo->slot_status[slot_id] != 0)
			{
				//printf("sysinfo.slot_status[%d] :  %d --------------------------\n", slot_id, sysinfo.slot_status[slot_id]);
				/*
				if(read_par){
					printf("spi send parameter: %s  ...\n", read_par);
				}else{
					printf("spi send parameter: NULL ......\n");
				}
				//*/
				msg_length = spi_rcv_json(slot_id, WEB_RCV_SPI_JSON, read_par, &sub_json_text);
				if(msg_length < 0){
					printf("==rcv_json form %d SPI Error===\n", slot_id);
					cJSON_Delete(rootJson);
					rootJson = NULL;
				}else if(msg_length == 0){
					printf("==rcv_json NULL ===\n");
					cJSON_Delete(rootJson);
					rootJson = NULL;
				}else {

					fmt1 = cJSON_Parse(sub_json_text);
					cJSON_AddItemToObject(rootJson, "SUB_BORAD", fmt1);
					free(sub_json_text);
				}	
			}
			break;	
			
		default :
			cJSON_AddItemToObject(rootJson, "WEB_SETTING", fmt1=cJSON_CreateObject());
			cJSON_AddNumberToObject(fmt1, "form_id", WEB_SETTING);
			break;
	}

	if(rootJson){
		json_text =cJSON_Print(rootJson);
		cJSON_Delete(rootJson);
	}
	
	return json_text;

}

static char *readTransceiver()
{
	char *json_text = NULL;
	cJSON *rootJson = NULL;
	rootJson=cJSON_CreateObject();
	
	Get_Transceiver_func(rootJson);
	
	json_text =cJSON_Print(rootJson);
	cJSON_Delete(rootJson);
	return json_text;
}

static  int websMainBordAjaxHandler(HttpConn *conn)
{  
	char *id_temp;
	v_u32 id, do_flag ;
	char *json_text=NULL;
	HttpQueue   *q;
	int httpStatus = 200;

	q = conn->writeq;
	
	id_temp = httpGetParam(conn, ("id"), (""));
	id = (unsigned int)atoi(id_temp);
	json_text = httpGetParam(conn, ("json"), (""));
	
	do_flag = get_web_request_flag(json_text, id);
	if(do_flag == 0) // 0 是 读
	{
		json_text = proJsonInfo(id, NULL);
	}else{	// 写
		parseJsonInfo(json_text);
		json_text = proJsonInfo(id, NULL);
	}
	
	if(NULL == json_text )
		httpStatus = 404;

	httpSetStatus(conn, httpStatus);

	httpAddHeaderString(conn, "Content-Type", "application/json");
	 //httpSetHeaderString(conn, "Cache-Control", "no-cache");
	 if(json_text){
		httpWrite(q, ("%s"), json_text);
		free(json_text);
	 }

	httpFinalize(conn);  
	
    return 1;  
} 

static  void websSubBoardAjaxHandler(HttpConn *conn) 
{  
	char *id_temp, *read_parameter;
	v_u32 id, do_flag, subboard_id ;
	char *json_text=NULL;
	HttpQueue   *q;
	int httpStatus = 200;

	q = conn->writeq;
	
	id_temp = httpGetParam(conn, ("id"), (""));
	id = (unsigned int)atoi(id_temp);
	subboard_id =( id  & 0x0f);
		
	json_text = httpGetParam(conn, ("json"), (""));// json不为空是写
	do_flag = get_web_request_flag(json_text, id);

	if(do_flag == 0){
		read_parameter = httpGetParam(conn, ("read_id"), NULL);
		json_text = proJsonInfo(id, read_parameter);		
	}else{	
		proSubWrRequest(json_text, subboard_id);
		json_text = proJsonInfo(0, NULL);
	}

	if(NULL == json_text )
		httpStatus = 404;
	
	//printf("%s: json_text = %s\n", __FUNCTION__, json_text);

	httpSetStatus(conn, httpStatus);

	httpAddHeaderString(conn, "Content-Type", "application/json");
	 //httpSetHeaderString(conn, "Cache-Control", "no-cache");
	 if(json_text){
		httpWrite(q, ("%s"), json_text);
		free(json_text);
	 }

	httpFinalize(conn);  
	
}
/*
**预期功能:同时向主板提交(1条),向不同子板提交参数
**JSON结构: // 要符合接口结构
{
	"MainBoardConfig":{
		...
	},
	"SubBoardConfig":[
		{
			"slot":0,
			"message":{}
		},
		{
			"slot":1,
			"message":{}
		},
		.,
		.
	]
}
*/
static void websMultiBoardAjaxHandler(HttpConn *conn){
	char *json_text = NULL, *main_json_text = NULL, *sub_json_text = NULL;
	cJSON *p_get_json;
	cJSON *p_json;
	cJSON *p_json_main, *p_json_sub;
	cJSON *dim1, *dim2;
	int size, i, slot;
	HttpQueue   *q;

	q = conn->writeq;
	
	json_text = httpGetParam(conn, ("json"), (""));
	p_get_json = cJSON_Parse(json_text);
	p_json = p_get_json->child;

	DebugPrintf("%s: json_text = %s\n", __FUNCTION__, json_text);
  	if(p_json_main = cJSON_GetObjectItem(p_json, "MainBoardConfig")){
  		DebugPrintf("MainBoardConfig........\n");
		anaJsonSection(p_json_main);	
	}
	
  	if(p_json_sub = cJSON_GetObjectItem(p_json, "SubBoardConfig")){
  		DebugPrintf("SubBoardConfig........\n");
		size = cJSON_GetArraySize(p_json_sub);
		for(i=0; i<size; i++){
			if((dim1 = cJSON_GetArrayItem(p_json_sub, i)) != NULL){
				if((dim2 = cJSON_GetObjectItem(dim1, "slot")) != NULL){
					slot = dim2->valueint;
					sub_json_text = cJSON_Print(cJSON_GetObjectItem(dim1, "message"));
					proSubWrRequest(sub_json_text, slot);
					if(sub_json_text != NULL){
						free(sub_json_text);
						sub_json_text = NULL;
					}
				}
			}
		}
	}
	
	cJSON_Delete(p_get_json);
	
	//json_text = proJsonInfo(0, NULL);

	httpSetStatus(conn, 200);

	httpAddHeaderString(conn, "Content-Type", "application/json");
	 //httpSetHeaderString(conn, "Cache-Control", "no-cache");

	//httpWrite(q, ("%s"), json_text);
	//free(json_text);

	httpFinalize(conn);  
}

void formJsonHandler(cJSON *p_json)
{
	cJSON *id_json;
	cJSON *child_json;
	id_json = cJSON_GetObjectItem(p_json, "formId");
	if(id_json == NULL){
		//return;
	}

	printf("%s-------------\n", p_json->string);
	p_json = p_json->child;
	while(p_json){
		switch(p_json->type){
			/* cJSON Types: */
			case cJSON_False :
			case cJSON_True :
				printf("%s : %d\n", p_json->string, p_json->valueint);
				break;
			case cJSON_Number :
				printf("%s : %d\n", p_json->string, p_json->valueint);
				break;
			case cJSON_String :
				printf("%s : %s\n", p_json->string, p_json->valuestring);
				break;
			case cJSON_Array :
			case cJSON_Object :
				printf("%s : \n", p_json->string);
				child_json = p_json->child;
				while(child_json){
					printf("%s : %s\n", child_json->string, child_json->valuestring);
					child_json = child_json->next;
				}
				break;

			case cJSON_NULL :
				break;
			case cJSON_IsReference :
				break;
		}
		
		p_json = p_json->next;
	}

	switch(id_json->valueint){
		case 1:
			break;
	}
}

static void websFormJsonAjaxHandler(HttpConn *conn){
	char *json_text = NULL, *main_json_text = NULL, *sub_json_text = NULL;
	cJSON *root_json;
	cJSON *p_json;
	cJSON *p_json_sub;
	cJSON *dim1, *dim2;
	int size, i, slot;
	HttpQueue   *q;
	u_int reqId;
	//q = conn->writeq;

	reqId = httpGetParam(conn, ("reqId"), (""));
	json_text = httpGetParam(conn, ("json"), (""));

	printf("%s: %s , json_text = %s\n", __FUNCTION__, reqId, json_text);

	if(json_text == NULL){
		return;
	}
	
	root_json = cJSON_Parse(json_text);
	p_json = root_json->child;

	while(p_json){
		formJsonHandler(p_json);
		
		p_json = p_json->next;
	}

	cJSON_Delete(root_json);
	

	httpSetStatus(conn, 200);

	httpAddHeaderString(conn, "Content-Type", "application/json");
	 //httpSetHeaderString(conn, "Cache-Control", "no-cache");

	//httpWrite(q, ("%s"), json_text);
	//free(json_text);

	httpFinalize(conn);  
}


static void upldUpgradeForm(HttpConn *conn)
{
	int ret;
	char buf[512] = {0};
	char filesdir[100];
	char *filename, *file = filesdir;
	char *getname = NULL, *str = NULL;

	char *id_temp;
	u_int board_id = 0, upload_error_flag = 0;
	int status_code, index, fileqty = 0;

	HttpUploadFile  *uploadfile;
	
	HttpRoute   *route = conn->host->defaultRoute;
	
	id_temp = httpGetParam(conn, ("board_id"), (""));
	
	status_code = HTTP_CODE_OK;
	
	if(id_temp == NULL){
		printf("board_id = NULL\n");
		status_code = HTTP_CODE_RANGE_NOT_SATISFIABLE;
	}else{

		board_id = (unsigned int)atoi(id_temp);	

		if(board_id == 0){				
			sprintf(filesdir, UPGRADE_FILES_DIR"/mainborad");
		}else{
			sprintf(filesdir, UPGRADE_FILES_DIR"/slot%02d", board_id-1);
		}

		if(access(filesdir, F_OK)<0){
			mkdir(filesdir, 0644);
		}

		filename = &filesdir[strlen(filesdir)];
		*filename++ = '/';

		//uploadfile = mprGetFirstItem(conn->rx->files);
		if(conn->rx==NULL){
			status_code = HTTP_CODE_NO_CONTENT;
			goto BeginSendRes;
		}
		if(conn->rx->files){
			fileqty = conn->rx->files->length;
		}
		//printf("upload files qty = %d \n", fileqty);
		//for (ITERATE_ITEMS(conn->rx->files, uploadfile, index)) {
		for (index=0;index < fileqty; index++) {

			uploadfile = mprGetItem(conn->rx->files, index);
			
			if((uploadfile != NULL) && (uploadfile->filename != NULL) && (uploadfile->clientFilename != NULL)){
			
				getname = uploadfile->clientFilename;
				str = strtok(uploadfile->clientFilename, "\\");
				while(str != NULL){
					getname = str;
					str = strtok(NULL, "\\");
				}

				strcpy(filename, getname);

				if((ret = rename(uploadfile->filename, file) ) != 0){
					status_code = HTTP_CODE_RANGE_NOT_SATISFIABLE;
					upload_error_flag = 1;
					printf("Filename  %s : ret %d : %s\n", file, ret, strerror(errno));
				}
				//remove(uploadfile->filename);
			}else{
				printf("File error........\n");
				status_code = HTTP_CODE_RANGE_NOT_SATISFIABLE;
				upload_error_flag = 1;
			}
		}

		if(fileqty<=0){
			printf("there are not any upload File........\n");
			status_code = HTTP_CODE_RANGE_NOT_SATISFIABLE;
			upload_error_flag = 1;
		}

	}

	BeginSendRes:

	httpSetStatus(conn, status_code);
	
	if(upload_error_flag == 1){
		httpError(conn, HTTP_CODE_RANGE_NOT_SATISFIABLE, "File error");
	}

	httpFinalize(conn);

}

static void saveLog(HttpConn *conn){
	int slot, j, ret;
	log_record_st *log_buf;
	event_logs_st *p_eventlogs;
	char databuf[512] = {0};
	HttpQueue   *q;

	q = conn->writeq;


	httpSetStatus(conn, 200);

	httpAddHeaderString(conn, "Content-Type", "application/octet-stream");
	 //httpSetHeaderString(conn, "Cache-Control", "no-cache");
	httpSetHeaderString(conn, "Content-Transfer-Encoding", "binary");
	httpSetHeaderString(conn, "Content-Disposition", "attachment; filename=LogMsg");
	
//	httpWrite(q, "01010100101001");
	
	for(slot=0; slot<MAX_SLOT_QTY; slot++)
	{	
		if((sysinfo->slot_info[slot].boardinfo_isok)&&(sysinfo->slot_status[slot] != 0))
		{
			p_eventlogs = &board_event_logs[slot];
			ret = get_board_logs_by_sn(slot, START_LOG, LOG_COUNT);
			if(ret > 0)
			{
				log_buf = p_eventlogs->logs_buf;
				for(j=0; j<ret; j++)
				{
					memset(databuf, 0, sizeof(databuf));
					get_boardlog_text(slot, log_buf+j, databuf);
					httpWrite(q, databuf);
					httpWrite(q, "\r\n");
				}
				free_board_logbuf(slot);
			}
		}
	}

	httpFinalize(conn);  

}

static  int transceiverrouteform(HttpConn *conn)
{  
	v_u32 id, do_flag ;
	char *json_text=NULL;
	HttpQueue   *q;

	q = conn->writeq;
	
	json_text = httpGetParam(conn, ("json"), (""));
	do_flag = get_web_request_flag(json_text, id);
  
	if(do_flag == 0) // 0 是 读
	{
		json_text = readTransceiver();
	}else{	// 写
		parseJsonInfo(json_text);
		json_text = readTransceiver();
	}
	if(NULL == json_text )
		return 0;
	else{
		httpSetStatus(conn, 200);

		httpAddHeaderString(conn, "Content-Type", "application/json");
		 //httpSetHeaderString(conn, "Cache-Control", "no-cache");
		 
		httpWrite(q, ("%s"), json_text);
		free(json_text);

		httpFinalize(conn);  
	}
    return 1;  
} 
static  void BoardTitleInfo(HttpConn *conn)
{  
	char *json_text = NULL;
	cJSON *rootJson;
	HttpQueue   *q;

	q = conn->writeq;
	
//	creat root json
	rootJson = cJSON_CreateObject();
	Get_Deviceinfo_func(rootJson);
	Get_BoardInfo_func(rootJson); 
	json_text = cJSON_Print(rootJson);
	cJSON_Delete(rootJson);
	
	if(NULL == json_text )
		return;
	else{
		httpSetStatus(conn, 200);

		httpAddHeaderString(conn, "Content-Type", "application/json");
		 //httpSetHeaderString(conn, "Cache-Control", "no-cache");
		
		httpWrite(q, ("%s"), json_text);
		free(json_text);

		httpFinalize(conn);  
	}

} 
/*	
** 获取当前升级进度/信息
*/
static void upgrade_percent(HttpConn *conn)
{  
	char *json_text = NULL;
	char *id_temp;
	cJSON *rootJson;  
	int board_id = 0;
	HttpQueue   *q;

	q = conn->writeq;
	
	if((id_temp = httpGetParam(conn, ("board_id"), (""))) != NULL)
	board_id = (unsigned int)atoi(id_temp);

	rootJson = cJSON_CreateObject();
	Get_Upgrade_Percent_func(rootJson, board_id);
	json_text = cJSON_Print(rootJson);
	cJSON_Delete(rootJson);
	
	httpSetStatus(conn, 200);

	httpAddHeaderString(conn, "Content-Type", "application/json");
	 //httpSetHeaderString(conn, "Cache-Control", "no-cache");
		 
	httpWrite(q, ("%s"), json_text);
	free(json_text);

	httpFinalize(conn);
  
} 
/*	
board_id:(模块升级)
0	:Main Board
1-6 :Sub Board 1-6
*/
static  int upgrade_process(HttpConn *conn)
{  
	char *json_text = NULL;
	char *id_temp;
	cJSON *rootJson;  
	int board_id = 0, upgrade_flag;
	HttpQueue   *q;

	q = conn->writeq;
	
	id_temp = httpGetParam(conn, ("board_id"), (""));
	board_id = (unsigned int)atoi(id_temp);
	int slot = board_id -1;
	
	if(board_id == 0)
	{
		upgrade_flag = add_async_task((taskHandler)upgrade_main_board, NULL, 0);
	}	
	else
	{
		upgrade_flag = add_async_task((taskHandler)upgrade_slot_board, (void *)slot, sizeof(int));
	}	
//	if(upgrade_flag == 0){
	if(1){
		rootJson = cJSON_CreateObject();
		Get_Deviceinfo_func(rootJson);
		json_text = cJSON_Print(rootJson);
		cJSON_Delete(rootJson);
		
		httpSetStatus(conn, 200);

		httpAddHeaderString(conn, "Content-Type", "application/json");
		 //httpSetHeaderString(conn, "Cache-Control", "no-cache");
		 
		httpWrite(q, ("%s"), json_text);
		
		httpFinalize(conn);  
	    return 1;
	}
	return 0;
} 

static void reboot_process(HttpConn *conn)
{  

	char *json_text = NULL;
	cJSON *rootJson;

	HttpQueue   *q;

	q = conn->writeq;
	
//	creat root json
	rootJson = cJSON_CreateObject();
	Get_Deviceinfo_func(rootJson);
	json_text = cJSON_Print(rootJson);
	cJSON_Delete(rootJson);
	
	if(NULL == json_text )
		return 0;
	else{

   		httpSetStatus(conn, 200);

		httpAddHeaderString(conn, "Content-Type", "application/json");
		 //httpSetHeaderString(conn, "Cache-Control", "no-cache");
    
		
		httpWrite(q, ("%s"), json_text);

		httpFinalize(conn);  
	}
	
	system("reboot");
	return 1;
} 

/*lxzheng add for misc web page 2016/07/28*/
int writeDataToFile_misc(char *filename, char *databuf, int len)
{
	int fd;
	int wrlen=0, ret=0;
	char *pdata = databuf;
	
	if((filename == NULL) || (databuf == NULL) || (len<=0)){
		return -1;
	}

	fd = open(filename, O_RDWR | O_TRUNC| O_CREAT, 0644);
	if(fd<0){
		printf(" open %s failed \n", filename);
		return -1;
	}

	while(len>0){
		wrlen = write(fd, pdata, len);
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
/*lxzheng add for misc web page 2016/07/28*/
int  uploadParamData_misc(char *filename,char *web_databuf)
{
	FILE * fd;
	int len=0, rdlen=0;
	char *pdata;

	fd = fopen(filename, "r");   
	if(fd<0){
		printf(" open %s failed \n", filename);
		return -1;
	}
	pdata = web_databuf;
	do{
		rdlen = fread(pdata,1024,1,fd);
		if(rdlen>0){
			pdata += rdlen;
			len += rdlen;
		}
	}while(rdlen>0);

	close(fd);

	return (len>0)?writeDataToFile_misc(DBFILE,  web_databuf, len):-1;
	
			
}
static void uploadParamsHandler(HttpConn *conn)
{
	HttpQueue   *q;
	HttpUploadFile 	 *file;
	char         	   *path;
	int           	  index=0;
	int 			fileqty=0;
	int len1=0, ret=0;
	int fd;

	int status_code=0,upload_error_flag=0;
	
	q = conn->writeq;

	if(conn->rx->files){
		fileqty = conn->rx->files->length;
	}

	for (index=0;index < fileqty; index++) 
	{
		upload_error_flag = 1;
		file = mprGetItem(conn->rx->files, index);
	
		if((file != NULL) && (file->size > 0) &&  (file->filename != NULL)  && (file->clientFilename != NULL)  && (strcmp("para.dat", file->clientFilename) == 0))
		{
			if((len1 = uploadParamData_misc(file->filename,web_databuf))  >= 0 )
			{
				upload_error_flag = 0;
			}			
		}
	}
	
	if(fileqty<=0 || upload_error_flag == 1){
		status_code = HTTP_CODE_RANGE_NOT_SATISFIABLE;
	}else
	{
		status_code =  HTTP_CODE_OK;
	//	printf(" ----[%s] \n",file->filename);
	//	sys_cmd("rm -f %s ", file->filename);
	}	

	printf("----status_code=%d\r\n", status_code);

	httpSetStatus(conn, status_code);
	
	if(upload_error_flag == 1){
		httpError(conn, HTTP_CODE_RANGE_NOT_SATISFIABLE, "File error");
	}

	httpFinalize(conn);
}

static void uploadTest(HttpConn *conn)
{
	HttpQueue   *q;

	HttpUploadFile  *file;
	char            *path;
	int             index;
	int 			len;


	q = conn->writeq;
    /*
        Set the HTTP response status
     */

      httpSetStatus(conn, 200);

    /*
        Add desired headers. "Set" will overwrite, add will create if not already defined.
     */
    httpAddHeaderString(conn, "Content-Type", "text/plain"); //不转码
    //httpSetHeaderString(conn, "Cache-Control", "no-cache");

	
    if (scaselessmatch(conn->rx->method, "POST")) {
	 for (ITERATE_ITEMS(conn->rx->files, file, index)) {
            httpWrite(q, "FILE: %s\r\n", file->name);
            httpWrite(q, "FILENAME=%s\r\n", file->filename);
            httpWrite(q, "CLIENT=%s\r\n", file->clientFilename);
            httpWrite(q, "TYPE=%s\r\n", file->contentType);
            httpWrite(q, "SIZE=%d\r\n", file->size);
		path = mprJoinPath(httpGetRouteVar(httpGetDefaultRoute(NULL), "UPLOAD_DIR"), file->clientFilename);
			//httpGetParam(conn, "UPLOAD_DIR", "");

		printf("upload : path = %s \n", path);
 		rename(file->filename, path);
            /*
                Just for this unit test. Don't copy this pattern.
                We are being lazy and using one route for uploads to *.html and *.esp.
                HTML needs renameUploads and ESP does not.
             */
            conn->rx->renameUploads = 0;
        }
        if (conn->rx->files->length > 0) {
            httpWrite(q,"Upload Complete\r\n");
        }

    }
    httpFinalize(conn);
}

static int show_upgrade_status(HttpConn *conn)
{
 HttpQueue   *q;

    q = conn->writeq;
	
	httpWrite(q, "progress_ratio %d , flag = %d<br>", upgrade_status[0].progress_ratio,upgrade_status[0].flag);
	httpWrite(q, "status[%d] : %s <br>",  upgrade_status[0].status, upgrade_status[0].status_msg);

	
}
/*
**页面升级是否继续提示
*/
static void input_upgrade_value_form(HttpConn *conn){
	int value;
	int board_id = 0;
	value = atoi(httpGetParam(conn, ("value"), ("")));
	board_id = atoi(httpGetParam(conn, ("board_id"), ("")));

	upgrade_input_value(board_id, value);

	httpFinalize(conn);
	 
	return 0;
}

/*
此函数可借鉴之处:可以向web返回error,但请求的数据结构不是json
*/
static void web_save_params_handler(HttpConn *conn)
{
	int status_code = 200;
	HttpRoute   *route;
	int len;

	HttpQueue   *q;

	q = conn->writeq;
	
	len = download_parameters(web_databuf);
	if(len<=0){
 		httpSetStatus(conn, HTTP_CODE_NOT_FOUND);
	}else{
		 httpSetStatus(conn, HTTP_CODE_OK);
		 httpAddHeaderString(conn, "Content-Type", "application/octet-stream");
		 httpAddHeaderString(conn, "Content-Disposition", "attachment;filename=\"para.dat\"");
		 httpAddHeaderString(conn, "Connection", "close");
	    	 httpSetHeaderString(conn, "Cache-Control", "no-cache");	

		printf("httpWriteBlock(q,web_databuf,len,HTTP_BLOCK)\n");
			 
		 httpWriteBlock(q,web_databuf,len,HTTP_BLOCK);	 
	}

	 httpFinalize(conn);

	
} 
/*
static void upldParamForm(HttpConn *conn)
{
	HttpQueue   *q;
	HttpUploadFile 	 *file;
	char         	   *path;
	int           	  index=0;
	int 			fileqty=0;
	int len, rdlen;
	int fd;

	int status_code=0,upload_error_flag=0;
	
	q = conn->writeq;

	if(conn->rx->files){
		fileqty = conn->rx->files->length;
	}
	printf("-----fileqty=%d \n", fileqty);
	for (index=0;index < fileqty; index++) 
	{
		
		file = mprGetItem(conn->rx->files, index);
	
		printf("--FILE: %s\r\n", file->name);
		printf("--FILENAME=%s\r\n", file->filename);
		printf("--CLIENT=%s\r\n", file->clientFilename);
		printf("--TYPE=%s\r\n", file->contentType);
		printf("--SIZE=%d\r\n", file->size);

		if((file != NULL) && (file->size > 0) &&  (file->filename != NULL)  && (file->clientFilename != NULL)  && (strcmp("para.dat", file->clientFilename) == 0))
		{
			upload_error_flag = 0;
			status_code =  HTTP_CODE_OK;
		}else
		{
			printf("File Error........\n");
			status_code = HTTP_CODE_RANGE_NOT_SATISFIABLE;
			upload_error_flag = 1;
		}
	}
	if(fileqty<=0){
		printf("there are not any upload File........\n");
		status_code = HTTP_CODE_RANGE_NOT_SATISFIABLE;
		upload_error_flag = 1;
	}
	printf("----status_code=%d\r\n", status_code);

	httpSetStatus(conn, status_code);
	
	if(upload_error_flag == 1){
		printf(".......httpError ........\n");
		httpError(conn, HTTP_CODE_RANGE_NOT_SATISFIABLE, "File error");
	}

	httpFinalize(conn);


}
*/
/*
此函数可借鉴之处:可以向web返回error,但请求的数据结构不是json
*/
static void WebLogin_process(HttpConn *conn)
{

	char *json_text = NULL;
	cJSON *p_get_json;
	cJSON *p_json;
	cJSON *val_arr_wrap = NULL;
	int web_set_login_flag;
	int status_code = 200;
	HttpRoute   *route;
	char username[20];
	
	route = httpGetDefaultRoute(NULL);

	json_text = httpGetParam(conn, "json", "");
	
	p_get_json = cJSON_Parse(json_text);
	p_json = p_get_json->child;

	val_arr_wrap = cJSON_GetObjectItem(p_json, "value");

	strcpy(username, sysinfo->loginuser);
	
	web_set_login_flag = Set_Login_func(val_arr_wrap);
	if(web_set_login_flag == 1){
		httpRemoveUser(route->auth, username);
		websSetUserAction(conn); 

	}else{
		 httpSetStatus(conn, HTTP_CODE_RANGE_NOT_SATISFIABLE);

	 httpAddHeaderString(conn, "Content-Type", "text/html");
    	 httpSetHeaderString(conn, "Cache-Control", "no-cache");

	    /*
	        Call finalize output and close the request.
	        Delay closing if you want to do asynchronous output and close later.
	     */
	 httpFinalize(conn);

	}

	cJSON_Delete(p_get_json);   
	
} 


/*
    This method is run when the action form is called from the web page. 
 */

static void myaction(HttpConn *conn)
{
    HttpQueue   *q;

    q = conn->writeq;
    /*
        Set the HTTP response status
     */
    httpSetStatus(conn, 200);

    /*
        Add desired headers. "Set" will overwrite, add will create if not already defined.
     */
    httpAddHeaderString(conn, "Content-Type", "text/html");
    httpSetHeaderString(conn, "Cache-Control", "no-cache");

    httpWrite(q, "<html><title>simpleAction</title><body>\r\n");
    httpWrite(q, "<p>Name: %s</p>\n", httpGetParam(conn, "name", "-"));
    httpWrite(q, "<p>Address: %s</p>\n", httpGetParam(conn, "address", "-"));
    httpWrite(q, "</body></html>\r\n");

    /*
        Call finalize output and close the request.
        Delay closing if you want to do asynchronous output and close later.
     */
    httpFinalize(conn);

#if POSSIBLE
    /*
        Useful things to do in actions
     */
    httpRedirect(conn, 302, "/other-uri");
    httpError(conn, 409, "My message : %d", 5);
#endif
}


void appweb_httpAction_defines(void)
{
	httpDefineAction("/goform/my", myaction);

	httpDefineAction(("/goform/mainBoard"), websMainBordAjaxHandler);
	httpDefineAction(("/goform/subBoard"),websSubBoardAjaxHandler);
	httpDefineAction(("/goform/multiBoard"),websMultiBoardAjaxHandler);

	
	httpDefineAction(("/goform/formjson"),websFormJsonAjaxHandler);

	httpDefineAction(("/goform/setroute"), transceiverrouteform);
	httpDefineAction(("/goform/BoardTitleInfo"), BoardTitleInfo);
	httpDefineAction(("/goform/upldForm"), upldUpgradeForm);
	httpDefineAction(("/goform/saveLog"),saveLog);
	
	httpDefineAction("/goform/uploadTest", uploadTest);

	httpDefineAction(("/goform/upgrade"),upgrade_process);
	httpDefineAction(("/goform/upgrade_percent"),upgrade_percent);
	httpDefineAction(("/goform/reboot"),reboot_process);
	
	httpDefineAction("/goform/inputUpgradeValue", input_upgrade_value_form);

	httpDefineAction(("/goform/WebLogin"),WebLogin_process);
	
	httpDefineAction(("/goform/saveparams"),web_save_params_handler);
	//httpDefineAction(("/goform/upldParamForm"),upldParamForm);
	httpDefineAction("/goform/uploadParams", uploadParamsHandler);

}

int set_web_user(HttpConn *conn, char *user, char *passwd)
{
	HttpAuth    *auth;
   	HttpRoute   *route;
	 char        *encodedPassword, *realm = "", *cipher;

	#if 0
	return authpass(NULL, user, passwd, role);
	#else
	cipher = "blowfish";	// Use "md5" or "blowfish"
	realm = "example.com";

	if(conn){
		auth = conn->rx->route->auth;
	}else{
		//auth = httpCreateAuth();
		route = httpGetDefaultRoute(NULL);
		auth = route->auth;
	if(auth == NULL){
		printf("http create auth failed \n");
		return -1;
	}
	}

	if(httpRemoveUser(auth, user)<0){
		printf("httpRemoveUser %s error \n", user);
	}

	encodedPassword = makeAuthPassword(cipher, user, passwd, realm);

	printf("set_webuser :  %s %s  cipher[%s] %s %s \n", user, passwd, cipher, realm, encodedPassword);

	if (httpAddUser(auth, user, encodedPassword, "manage") == 0) {
		printf("http add user failed \n");
		return (-8);
	}

	/*
	if (maWriteAuthFile(auth, "/var/run/appweb/auth.conf") < 0) {
		return (-9);
	}
	//*/
	#endif

	return 0;
}


void start_webserver(char *configFile)
{

	if((configFile == NULL) || (strlen(configFile) == 0)){
		configFile = WEBROOT"/conf/appweb.conf";
	}

	appweb_account(sysinfo->loginuser, sysinfo->loginpasswd);
	appweb_maxThreads(4);
	//create_detached_pthread(appweb, configFile, -1);
	appweb(configFile);
	
}

void stop_webserver(void)
{
	int status;
	mprShutdown(MPR_EXIT_NORMAL, 0, 2000);
	status = mprGetExitStatus();
	mprLog("info appweb", 1, "Stopping Appweb ... : %d", status);
	mprDestroy();
}



int debug_web(int argc, char *argv[])
{
	char *authFile = "auth.conf";
	char *username;
	char *password;
	char *roles = "manage";


	if(argc <2){
		return -1;
	}
	
	if(strcmp("setuser", argv[1]) == 0){
		if(argc <4){
			return -1;
		}
		username = argv[2];
		password = argv[3];
		set_web_user(NULL, username, password);
	}else if(strcmp("authpass", argv[1]) == 0){

		if(argc <4){
			return -1;
		}
		username = argv[2];
		password = argv[3];

		if(argc>4){
			authFile = argv[4];
		}

		if(argc>5){
			roles = argv[5];
		}
		authpass(authFile, username, password, roles);
	}

	
	return 0;

}


