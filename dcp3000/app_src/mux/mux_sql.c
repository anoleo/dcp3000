

#include "../appref.h"
#include "mux.h"

void create_remux_table(void)
{
	
	int err;
	char *sql;

	/*
	sql = "create table remux_parameters ("
				"mux_channel integer,"
				"enable integer,"
				"use_default integer,"
				"insert_sdt_flag integer,"
				"insert_nit_flag integer,"
				"insert_eit_flag integer,"
				"packet_length integer,"
		    	"ts_id integer,"
		    	"network_id integer,"
		   		"ori_network_id integer,"
				"total_bitrate integer"
			");";
	//*/

	
	exec_sql("CREATE TABLE IF NOT EXISTS remux_parameters ("
				"mux_channel integer,"
				"parameters blob"
			");");


	exec_sql("CREATE TABLE IF NOT EXISTS remux_pid_filter("
				"mux_channel integer,"
				"in_channel integer,"
				"in_pid integer,"
				"mux_pid integer"
			");");
	

	exec_sql("CREATE TABLE IF NOT EXISTS remux_pass_pid("
				"mux_channel integer,"
				"in_channel integer,"
				"in_pid integer,"
				"mux_pid integer"
			");");
	
	
	exec_sql("CREATE TABLE IF NOT EXISTS remux_program("
				"mux_channel integer,"
				"in_channel integer,"
				"old_pn integer,"
				"new_pn integer,"
				"in_pmt_pid integer,"
				"mux_pmt_pid integer,"
				"in_pcr_pid integer,"
				"mux_pcr_pid integer,"
				
				"es_pid_mapping blob,"
				"service_info blob,"
				"old_service_info blob,"
				"char_type integer,"
				"free_ca_mode integer,"
				
				"cas_mode integer"
			");");

	exec_sql("CREATE TABLE IF NOT EXISTS remux_section("
				"mux_channel integer,"
				"pid integer,"
				"tableid integer,"
				"section_len integer,"
				"section_data blob"
			");");

}


int select_remux_section(int mux_chan, int pid, int tableid, mux_section_t **pp_mux_section)
{
	
	sqlite3_stmt *pStmt;
	char *sql;
	int res, record_count;
	mux_channel_t *p_mux_channel;

	mux_section_t *p_mux_section;
	mux_section_t *head_mux_section = NULL, *cur_mux_section = NULL;
	int in_chan, old_pn, new_pn;

	p_mux_channel = &mux_channels[mux_chan];


	if((pid<0) && (tableid<0)){
		sql = "select * from remux_section "
			"where mux_channel = ? ;";
	}else if((pid>=0) && (tableid>=0)){
		sql = "select * from remux_section "
			"where mux_channel = ? and pid = ? and tableid = ?;";
	}else if((pid>=0)){
		sql = "select * from remux_section "
			"where mux_channel = ? and pid = ?;";
	}else if( (tableid>=0)){
		sql = "select * from remux_section "
			"where mux_channel = ? and tableid = ?;";
	}


	res = sqlite3_prepare(sqlite_database, sql, -1, &pStmt, NULL);
	if(res != SQLITE_OK){
		printf("sqlite3_prepare::%s is failed\n", sql);
		sqlite3_finalize(pStmt);
		return -1;
	}
	
	sqlite3_bind_int(pStmt, 1, mux_chan);
	if((pid>=0) && (tableid>=0)){
		sqlite3_bind_int(pStmt, 2, pid);
		sqlite3_bind_int(pStmt, 3, tableid);
	}else if((pid>=0)){
		sqlite3_bind_int(pStmt, 2, pid);
	}else if((tableid>=0)){
		sqlite3_bind_int(pStmt, 2, tableid);
	}
	

	record_count = 0;
	do{
		res = sqlite3_step(pStmt);

		if(res == SQLITE_ROW){
			
			p_mux_section = create_mux_section();
			
			p_mux_section->pid = sqlite3_column_int(pStmt, 1);
			p_mux_section->len = sqlite3_column_int(pStmt, 3);
			memcpy(p_mux_section->data, sqlite3_column_blob(pStmt, 4), p_mux_section->len);
			
			if(head_mux_section){
				cur_mux_section->next = p_mux_section;
			}else{
				head_mux_section = p_mux_section;
			}
			cur_mux_section = p_mux_section;
		
			record_count++;
			
		}else if(res == SQLITE_DONE){
			break;
		}else{
			printf("sqlite3_step::%s is failed\n", sql);
			sqlite3_finalize(pStmt);
			free_all_mux_section(head_mux_section);
			return -1;
		}

	}while(res == SQLITE_ROW);
	

	sqlite3_finalize(pStmt);
	printf("Select %d mux_channel remux_remux_section :: pid = %#x, tableid = %#x,  record count = %d\n", mux_chan, pid, tableid, record_count);

	*pp_mux_section = head_mux_section;

	return record_count;
}


int select_remux_programs(int mux_chan)
{
	
	sqlite3_stmt *pStmt;
	char *sql;
	int res, record_count;
	mux_channel_t *p_mux_channel;

	mux_program_t *p_mux_program;
	int in_chan, old_pn, new_pn;

	p_mux_channel = &mux_channels[mux_chan];


	
 	sql = "select * from remux_program "
			"where mux_channel = ?;";//" order by new_pn desc;";

	res = sqlite3_prepare(sqlite_database, sql, -1, &pStmt, NULL);
	if(res != SQLITE_OK){
		printf("sqlite3_prepare::%s is failed\n", sql);
		sqlite3_finalize(pStmt);
		return -1;
	}
	
	sqlite3_bind_int(pStmt, 1, mux_chan);

	record_count = 0;
	do{
		res = sqlite3_step(pStmt);

		if(res == SQLITE_ROW){

			in_chan = sqlite3_column_int(pStmt, 1);
			if((in_chan>=mux_info.total_tsin_channels)){
					return NULL;
			}
			old_pn = sqlite3_column_int(pStmt, 2);
			new_pn = sqlite3_column_int(pStmt, 3);

			p_mux_program = update_program_mapping(mux_chan, in_chan, old_pn, new_pn);

			p_mux_program->in_pmt_pid = sqlite3_column_int(pStmt, 4);
			p_mux_program->mux_pmt_pid = sqlite3_column_int(pStmt, 5);

			p_mux_program->in_pcr_pid = sqlite3_column_int(pStmt, 6);
			p_mux_program->mux_pcr_pid = sqlite3_column_int(pStmt, 7);
			
			memcpy(p_mux_program->es_pid_mapping, sqlite3_column_blob(pStmt, 8), ES_PID_QTY_IN_A_PROGRAM*2*2);
			memcpy(&p_mux_program->service_info, sqlite3_column_blob(pStmt, 9), sizeof(service_info_t));
			memcpy(&p_mux_program->old_service_info, sqlite3_column_blob(pStmt, 10), sizeof(service_info_t));

			p_mux_program->char_type = sqlite3_column_int(pStmt, 11);
			p_mux_program->free_CA_mode = sqlite3_column_int(pStmt, 12);
			p_mux_program->cas_mode = sqlite3_column_int(pStmt, 13);

	
			record_count++;
			
		}else if(res == SQLITE_DONE){
			break;
		}else{
			printf("sqlite3_step::%s is failed\n", sql);
			sqlite3_finalize(pStmt);
			reset_mux_programs(mux_chan);
			return -1;
		}

	}while(res == SQLITE_ROW);
	

	sqlite3_finalize(pStmt);
	printf("Select %d mux_channel remux_mux_program:: record count = %d\n", mux_chan, record_count);

	return record_count;
}


int select_remux_pid_filter(int mux_chan)
{

	sqlite3_stmt *pStmt;
	char *sql;
	int res, record_count;
	int in_chan, in_pid, mux_pid;

	mux_channel_t *p_mux_channel;

	p_mux_channel = &mux_channels[mux_chan];
	
	sql = "select in_channel, in_pid, mux_pid "
			"from remux_pid_filter "
			"where mux_channel = ? order by mux_pid desc;";

	res = sqlite3_prepare(sqlite_database, sql, -1, &pStmt, NULL);
	if(res != SQLITE_OK){
		printf("sqlite3_prepare::%s is failed\n", sql);
		sqlite3_finalize(pStmt);
		return -1;
	}
	
	sqlite3_bind_int(pStmt, 1, mux_chan);

	record_count = 0;
	do{
		res = sqlite3_step(pStmt);

		if(res == SQLITE_ROW){


			in_chan = sqlite3_column_int(pStmt, 0);
			in_pid = sqlite3_column_int(pStmt, 1);
			mux_pid = sqlite3_column_int(pStmt, 2);

			update_pid_mapping(mux_chan, in_chan, in_pid, mux_pid);

			
			record_count++;
			
		}else if(res == SQLITE_DONE){
			break;
		}else{
			printf("sqlite3_step::%s is failed\n", sql);
			sqlite3_finalize(pStmt);
			free_pid_filters(p_mux_channel->pid_mapping_list);
			return -1;
		}

	}while(res == SQLITE_ROW);

	p_mux_channel->pass_pid_count = record_count;

	sqlite3_finalize(pStmt);
	printf("Select %d mux_channel remux_pid_filter:: record count = %d \n", mux_chan, record_count);
	
	return record_count;
}


int select_remux_pass_pid(int mux_chan)
{

	sqlite3_stmt *pStmt;
	char *sql;
	int res, record_count;

	pid_mapping_t *p_pid_mapping;

	mux_channel_t *p_mux_channel;

	p_mux_channel = &mux_channels[mux_chan];
	
	sql = "select in_channel, in_pid, mux_pid "
			"from remux_pass_pid "
			"where mux_channel = ? ;";

	res = sqlite3_prepare(sqlite_database, sql, -1, &pStmt, NULL);
	if(res != SQLITE_OK){
		printf("sqlite3_prepare::%s is failed\n", sql);
		sqlite3_finalize(pStmt);
		return -1;
	}
	
	sqlite3_bind_int(pStmt, 1, mux_chan);

	record_count = 0;
	do{
		res = sqlite3_step(pStmt);

		if(res == SQLITE_ROW){

			p_pid_mapping = create_pid_mapping();

			p_pid_mapping->in_chan = sqlite3_column_int(pStmt, 0);
			p_pid_mapping->in_pid = sqlite3_column_int(pStmt, 1);
			p_pid_mapping->mux_pid = sqlite3_column_int(pStmt, 2);

			p_pid_mapping->next = p_mux_channel->pass_pids;
			p_mux_channel->pass_pids = p_pid_mapping;
			
			record_count++;
			
		}else if(res == SQLITE_DONE){
			break;
		}else{
			printf("sqlite3_step::%s is failed\n", sql);
			sqlite3_finalize(pStmt);
			free_pid_filters(p_mux_channel->pass_pids);
			p_mux_channel->pass_pid_count = 0;
			return -1;
		}

	}while(res == SQLITE_ROW);

	p_mux_channel->pass_pid_count = record_count;

	sqlite3_finalize(pStmt);
	printf("Select %d mux_channel remux_pass_pid:: record count = %d \n", mux_chan, record_count);
	
	return record_count;
}

int select_remux_parameters(int mux_chan)
{

	sqlite3_stmt *pStmt;
	char *sql;
	int res, record_count;
	mux_channel_t *p_mux_channel;
	
	p_mux_channel = &mux_channels[mux_chan];

	sql = "select * "
		"from remux_parameters "
		"where mux_channel = ?;";

	res = sqlite3_prepare(sqlite_database, sql, -1, &pStmt, NULL);
	if(res != SQLITE_OK){
		printf("sqlite3_prepare::%s is failed\n", sql);
		sqlite3_finalize(pStmt);
		return -1;
	}
	
	sqlite3_bind_int(pStmt, 1, mux_chan);

	record_count = 0;
	do{
		res = sqlite3_step(pStmt);

		if(res == SQLITE_ROW){

			
			#if 1
			memcpy(p_mux_channel, sqlite3_column_blob(pStmt, 1), sizeof(mux_parameter_t));
			#else
			p_mux_channel->enable = sqlite3_column_int(pStmt, 1);
			p_mux_channel->pid_set_auto = sqlite3_column_int(pStmt, 2);
			p_mux_channel->insert_sdt_flag = sqlite3_column_int(pStmt, 3);
			p_mux_channel->insert_nit_flag = sqlite3_column_int(pStmt, 4);
			p_mux_channel->insert_eit_flag = sqlite3_column_int(pStmt, 5);
			p_mux_channel->packet_length = sqlite3_column_int(pStmt, 6);
			p_mux_channel->ts_id = sqlite3_column_int(pStmt, 7);
			p_mux_channel->network_id = sqlite3_column_int(pStmt, 8);
			p_mux_channel->ori_network_id = sqlite3_column_int(pStmt, 9);
			p_mux_channel->total_bitrate = sqlite3_column_int(pStmt, 10);
			#endif

			#if 0
			printf("----select_remux_parameters  %d channel :  enable = %d ...\n", mux_chan, p_mux_channel->enable);
			printf("----select_remux_parameters  %d channel :  pid_set_auto = %d ...\n", mux_chan, p_mux_channel->pid_set_auto);
			printf("----select_remux_parameters  %d channel :  insert_sdt_flag = %d ...\n", mux_chan, p_mux_channel->insert_sdt_flag);
			printf("----select_remux_parameters  %d channel :  insert_eit_flag = %d ...\n", mux_chan, p_mux_channel->insert_nit_flag);
			printf("----select_remux_parameters  %d channel :  insert_eit_flag = %d ...\n", mux_chan, p_mux_channel->insert_eit_flag);
			printf("----select_remux_parameters  %d channel :  packet_length = %d ...\n", mux_chan, p_mux_channel->packet_length);
			printf("----select_remux_parameters  %d channel :  ts_id = %d ...\n", mux_chan, p_mux_channel->ts_id);
			printf("----select_remux_parameters  %d channel :  network_id = %d ...\n", mux_chan, p_mux_channel->network_id);
			printf("----select_remux_parameters  %d channel :  ori_network_id = %d ...\n", mux_chan, p_mux_channel->ori_network_id);
			printf("----select_remux_parameters  %d channel :  total_bitrate = %d ...\n", mux_chan, p_mux_channel->total_bitrate);
			#endif

			record_count++;
			
		}else if(res == SQLITE_DONE){
			break;
		}else{
			printf("sqlite3_step::%s is failed\n", sql);
			sqlite3_finalize(pStmt);
			return -1;

		}

		

	}while(res == SQLITE_ROW);

	sqlite3_finalize(pStmt);
	printf("Select %d mux_channel remux_parameters:: record count = %d \n", mux_chan, record_count);

	return record_count;
}

int select_capture_control_params(void)
{
	
	int res, record_count;
	int i, len;

	u_int ctl_params;

	record_count = 0;
	if(select_db_integer("capture_control", 0, &ctl_params) == 0){
		capture_control.auto_capture = (ctl_params >> 0) & 1;
		for(i=0; i<mux_info.total_tsin_channels; i++){
			if(select_db_integer("channel_capture_control", i, &ctl_params) == 0){
				ts_in_channels[i].dowith_eit_pf = (ctl_params>>8)&1;
				ts_in_channels[i].dowith_eit_s = (ctl_params>>9)&1;
				ts_in_channels[i].dowith_nit = (ctl_params>>10)&1;		
				record_count++;
			}
		}

	}

	return record_count;
}


int save_remux_section(int mux_chan, mux_section_t *head_mux_section)
{
	char buf[200];	
	char *zErrMsg = 0;	
	sqlite3_stmt *pInsertStmt;
	char *sql;
	int res;
	mux_section_t *p_mux_section;

	int temp_count = 0;

	mux_channel_t *p_mux_channel;
	
	p_mux_channel = &mux_channels[mux_chan];

	//printf("Save %d mux_channel save_remux_pn_filter:: pn_filter count = %d\n", mux_chan, p_mux_channel->pn_count);


	sqlite3_exec( sqlite_database , "BEGIN TRANSACTION;" , 0 , 0 , &zErrMsg );
	
	memset(buf, 0, 200);
	sprintf(buf,"delete from remux_section where mux_channel=%d;", mux_chan);
	res = sqlite3_exec( sqlite_database , buf , 0 , 0 , &zErrMsg );
	if(res != SQLITE_OK){
		sqlite3_exec( sqlite_database , "rollback;" , 0 , 0 , &zErrMsg );
		printf("execute sql:%s, faild\n", buf);
		printf("save %d mux_channel remux_section faild\n", mux_chan);
		return;
	}

	sql = "insert into remux_section "
				"(mux_channel, pid, tableid, section_len, section_data)"
				" values(?,?,?,?,?)";
	res = sqlite3_prepare(sqlite_database, sql, -1, &pInsertStmt, NULL);
	if(res != SQLITE_OK){
		sqlite3_exec( sqlite_database , "rollback;" , 0 , 0 , &zErrMsg );
		printf("sqlite3_prepare::%s is failed\n", sql);
		sqlite3_finalize(pInsertStmt);
		return;
	}

	p_mux_section = head_mux_section;

	while(p_mux_section){

		sqlite3_bind_int(pInsertStmt, 1, mux_chan);
		sqlite3_bind_int(pInsertStmt, 2, p_mux_section->pid);
		sqlite3_bind_int(pInsertStmt, 3, p_mux_section->data[0]);
		sqlite3_bind_int(pInsertStmt, 4, p_mux_section->len);
		sqlite3_bind_blob(pInsertStmt, 5, p_mux_section->data, p_mux_section->len, NULL);
		
		res = sqlite3_step(pInsertStmt);
		if(res != SQLITE_DONE){
			sqlite3_finalize(pInsertStmt);
			printf("sqlite3_step::%s is failed\n", sql);

			sqlite3_exec( sqlite_database , "rollback;" , 0 , 0 , &zErrMsg );
			
			printf("save %d mux_channel mux_program faild\n", mux_chan);
			return;
			
		}
		
		temp_count++;	
				
		sqlite3_reset(pInsertStmt);
		p_mux_section = p_mux_section->next;
	}

	sqlite3_exec( sqlite_database , "commit;" , 0 , 0 , &zErrMsg );
	sqlite3_finalize(pInsertStmt);
	printf("Save %d mux_channel remux_section::  success, count = %d\n", mux_chan, temp_count);
	
}


void save_remux_programs(int mux_chan)
{
	char buf[200];	
	char *zErrMsg = 0;	
	sqlite3_stmt *pInsertStmt;
	char *sql;
	int res;
	mux_program_t *p_mux_program;

	int temp_count = 0;

	mux_channel_t *p_mux_channel;
	
	p_mux_channel = &mux_channels[mux_chan];

	//printf("Save %d mux_channel save_remux_pn_filter:: pn_filter count = %d\n", mux_chan, p_mux_channel->pn_count);


	sqlite3_exec( sqlite_database , "BEGIN TRANSACTION;" , 0 , 0 , &zErrMsg );
	
	memset(buf, 0, 200);
	sprintf(buf,"delete from remux_program where mux_channel=%d;", mux_chan);
	res = sqlite3_exec( sqlite_database , buf , 0 , 0 , &zErrMsg );
	if(res != SQLITE_OK){
		sqlite3_exec( sqlite_database , "rollback;" , 0 , 0 , &zErrMsg );
		printf("execute sql:%s, faild\n", buf);
		printf("save %d mux_channel remux_program faild\n", mux_chan);
		return;
	}

	sql = "insert into remux_program "
				"(mux_channel, in_channel, old_pn, new_pn, in_pmt_pid"
				", mux_pmt_pid, in_pcr_pid, mux_pcr_pid,es_pid_mapping"
				", service_info, old_service_info, char_type, free_ca_mode, cas_mode )"
				" values(?,?,?,?,?,?,?,?,?,?,?,?,?,?)";
	res = sqlite3_prepare(sqlite_database, sql, -1, &pInsertStmt, NULL);
	if(res != SQLITE_OK){
		sqlite3_exec( sqlite_database , "rollback;" , 0 , 0 , &zErrMsg );
		printf("sqlite3_prepare::%s is failed\n", sql);
		sqlite3_finalize(pInsertStmt);
		return;
	}

	//p_mux_program = p_mux_channel->mux_programs;
	p_mux_program = p_mux_channel->mux_programs_raw;
	TagPrintf(mux_chan);
	while(p_mux_program){
		TagPrintf(mux_chan);
		sqlite3_bind_int(pInsertStmt, 1, mux_chan);
		sqlite3_bind_int(pInsertStmt, 2, p_mux_program->in_chan);
		sqlite3_bind_int(pInsertStmt, 3, p_mux_program->old_pn);
		sqlite3_bind_int(pInsertStmt, 4, p_mux_program->new_pn);
		sqlite3_bind_int(pInsertStmt, 5, p_mux_program->in_pmt_pid);
		sqlite3_bind_int(pInsertStmt, 6, p_mux_program->mux_pmt_pid);
		sqlite3_bind_int(pInsertStmt, 7, p_mux_program->in_pcr_pid);
		sqlite3_bind_int(pInsertStmt, 8, p_mux_program->mux_pcr_pid);
		sqlite3_bind_blob(pInsertStmt, 9, p_mux_program->es_pid_mapping, ES_PID_QTY_IN_A_PROGRAM*2*2, NULL);
		sqlite3_bind_blob(pInsertStmt, 10, &p_mux_program->service_info, sizeof(service_info_t), NULL);
		sqlite3_bind_blob(pInsertStmt, 11, &p_mux_program->old_service_info, sizeof(service_info_t), NULL);
		sqlite3_bind_int(pInsertStmt, 12, p_mux_program->char_type);
		sqlite3_bind_int(pInsertStmt, 13, p_mux_program->free_CA_mode);
		sqlite3_bind_int(pInsertStmt, 14, p_mux_program->cas_mode);


		res = sqlite3_step(pInsertStmt);
		if(res != SQLITE_DONE){
			sqlite3_finalize(pInsertStmt);
			printf("sqlite3_step::%s is failed\n", sql);

			sqlite3_exec( sqlite_database , "rollback;" , 0 , 0 , &zErrMsg );
			
			printf("save %d mux_channel mux_program faild\n", mux_chan);
			return;
			
		}
		
		temp_count++;	
				
		sqlite3_reset(pInsertStmt);
		
		//p_mux_program = p_mux_program->next;
		p_mux_program = p_mux_program->link;
	}

	sqlite3_exec( sqlite_database , "commit;" , 0 , 0 , &zErrMsg );
	sqlite3_finalize(pInsertStmt);
	printf("Save %d mux_channel remux_program::  success, count = %d\n", mux_chan, temp_count);
	
}


void save_remux_pass_pid(int mux_chan)
{
	char buf[200];	
	char *zErrMsg = 0;	

	sqlite3_stmt *pInsertStmt;
	char *insert_sql;
	int res;
	pid_mapping_t *p_pid_mapping;

	int temp_count = 0;
	mux_channel_t *p_mux_channel;
		
	p_mux_channel = &mux_channels[mux_chan];

	
	//printf("Save %d mux_channel remux_pass_pid:: pid_filter count = %d\n", filter_table->mux_chan, filter_table->pid_count);

	sqlite3_exec( sqlite_database , "begin transaction;" , 0 , 0 , &zErrMsg );
	
	memset(buf, 0, 200);
	sprintf(buf,"delete from remux_pass_pid where mux_channel=%d;", mux_chan);
	res = sqlite3_exec( sqlite_database , buf , 0 , 0 , &zErrMsg );
	if(res != SQLITE_OK){
		sqlite3_exec( sqlite_database , "rollback;" , 0 , 0 , &zErrMsg );
		printf("execute sql:%s, faild\n", buf);
		printf("save %d mux_channel remux_pass_pid faild\n", mux_chan);
		return;
	}


	insert_sql = "insert into remux_pass_pid "
				"( mux_channel, in_channel"
				", in_pid, mux_pid) values(?,?,?,?)";
	res = sqlite3_prepare(sqlite_database, insert_sql, -1, &pInsertStmt, NULL);
	if(res != SQLITE_OK){
		sqlite3_exec( sqlite_database , "rollback;" , 0 , 0 , &zErrMsg );
		printf("sqlite3_prepare::%s is failed\n", insert_sql);
		sqlite3_finalize(pInsertStmt);
		return;
	}

	p_pid_mapping = p_mux_channel->pass_pids;


	while(p_pid_mapping){

		sqlite3_bind_int(pInsertStmt, 1, mux_chan);
		sqlite3_bind_int(pInsertStmt, 2, p_pid_mapping->in_chan);
		sqlite3_bind_int(pInsertStmt, 3, p_pid_mapping->in_pid);
		sqlite3_bind_int(pInsertStmt, 4, p_pid_mapping->mux_pid);

		//printf("save %d mux_channel RemuxPidFilter, %d in_chan, 0x%04x in_pid, 0x%04x mux_pid\n"
		//	, mux_chan, p_pid_mapping->in_chan, p_pid_mapping->in_pid, p_pid_mapping->mux_pid);

		res = sqlite3_step(pInsertStmt);
		if(res != SQLITE_DONE){
			sqlite3_finalize(pInsertStmt);
			printf("sqlite3_step::%s is failed\n", insert_sql);
			
			sqlite3_exec( sqlite_database , "rollback;" , 0 , 0 , &zErrMsg );
			
			printf("save %d mux_channel RemuxPidFilter faild\n", mux_chan);
			return;
			
		}

		temp_count++;
	
		sqlite3_reset(pInsertStmt);
		p_pid_mapping = p_pid_mapping->next;
	}
	
	sqlite3_exec( sqlite_database , "commit;" , 0 , 0 , &zErrMsg );
	sqlite3_finalize(pInsertStmt);
	printf("save %d mux_channel remux_pass_pid:: success, count = %d\n", mux_chan, temp_count);

}


void save_remux_pid_filter(int mux_chan)
{
	char buf[200];	
	char *zErrMsg = 0;	

	sqlite3_stmt *pInsertStmt;
	char *insert_sql;
	int res;
	pid_mapping_t *p_pid_mapping;

	int temp_count = 0;
	mux_channel_t *p_mux_channel;
		
	p_mux_channel = &mux_channels[mux_chan];

	
	//printf("Save %d mux_channel save_remux_pid_filter:: pid_filter count = %d\n", filter_table->mux_chan, filter_table->pid_count);

	sqlite3_exec( sqlite_database , "begin transaction;" , 0 , 0 , &zErrMsg );
	
	memset(buf, 0, 200);
	sprintf(buf,"delete from remux_pid_filter where mux_channel=%d;", mux_chan);
	res = sqlite3_exec( sqlite_database , buf , 0 , 0 , &zErrMsg );
	if(res != SQLITE_OK){
		sqlite3_exec( sqlite_database , "rollback;" , 0 , 0 , &zErrMsg );
		printf("execute sql:%s, faild\n", buf);
		printf("save %d mux_channel RemuxPidFilter faild\n", mux_chan);
		return;
	}


	insert_sql = "insert into remux_pid_filter "
				"( mux_channel, in_channel"
				", in_pid, mux_pid) values(?,?,?,?)";
	res = sqlite3_prepare(sqlite_database, insert_sql, -1, &pInsertStmt, NULL);
	if(res != SQLITE_OK){
		sqlite3_exec( sqlite_database , "rollback;" , 0 , 0 , &zErrMsg );
		printf("sqlite3_prepare::%s is failed\n", insert_sql);
		sqlite3_finalize(pInsertStmt);
		return;
	}

	p_pid_mapping = p_mux_channel->pid_mapping_list;


	while(p_pid_mapping){

		sqlite3_bind_int(pInsertStmt, 1, mux_chan);
		sqlite3_bind_int(pInsertStmt, 2, p_pid_mapping->in_chan);
		sqlite3_bind_int(pInsertStmt, 3, p_pid_mapping->in_pid);
		sqlite3_bind_int(pInsertStmt, 4, p_pid_mapping->mux_pid);

		//printf("save %d mux_channel RemuxPidFilter, %d in_chan, 0x%04x in_pid, 0x%04x mux_pid\n"
		//	, mux_chan, p_pid_mapping->in_chan, p_pid_mapping->in_pid, p_pid_mapping->mux_pid);

		res = sqlite3_step(pInsertStmt);
		if(res != SQLITE_DONE){
			sqlite3_finalize(pInsertStmt);
			printf("sqlite3_step::%s is failed\n", insert_sql);
			
			sqlite3_exec( sqlite_database , "rollback;" , 0 , 0 , &zErrMsg );
			
			printf("save %d mux_channel RemuxPidFilter faild\n", mux_chan);
			return;
			
		}

		temp_count++;
	
		sqlite3_reset(pInsertStmt);
		p_pid_mapping = p_pid_mapping->next;
	}
	
	sqlite3_exec( sqlite_database , "commit;" , 0 , 0 , &zErrMsg );
	sqlite3_finalize(pInsertStmt);
	printf("save %d mux_channel RemuxPidFilter:: success, count = %d\n", mux_chan, temp_count);

	/*
	filter_table->p_pid_mapping = NULL;
	filter_table->pid_count = 0;
	filter_table->tx_flag = TX_NULL;
	*/
	
}



void save_remux_parameters(int mux_chan)
{
	char buf[200];	
	char *zErrMsg = 0;	

	sqlite3_stmt *pInsertStmt;
	char *insert_sql;
	int res;

	mux_channel_t *p_mux_channel;
		
	p_mux_channel = &mux_channels[mux_chan];

	
	//printf("Save %d mux_channel save_remux_parameters:: ", mux_chan);

	sqlite3_exec( sqlite_database , "begin transaction;" , 0 , 0 , &zErrMsg );

	memset(buf, 0, 200);
	sprintf(buf,"delete from remux_parameters where mux_channel=%d;", mux_chan);
	res = sqlite3_exec( sqlite_database , buf , 0 , 0 , &zErrMsg );
	if(res != SQLITE_OK){
		sqlite3_exec( sqlite_database , "rollback;" , 0 , 0 , &zErrMsg );
		printf("execute sql:%s, faild\n", buf);
		printf("save %d mux_channel remux_parameters faild\n", mux_chan);
		return;
	}


	#if 1
	insert_sql = "insert into remux_parameters "
				"(mux_channel, parameters ) values (?,?)";
	#else
	insert_sql = "insert into remux_parameters "
				"(mux_channel, enable, use_default, insert_sdt_flag, insert_nit_flag"
				", insert_eit_flag, packet_length, ts_id, network_id, ori_network_id"
				", total_bitrate) values(?,?,?,?,?,?,?,?,?,?,?)";
	#endif
	
	res = sqlite3_prepare(sqlite_database, insert_sql, -1, &pInsertStmt, NULL);
	if(res != SQLITE_OK){
		exec_sql("rollback;");
		printf("sqlite3_prepare::%s is failed\n", insert_sql);
		sqlite3_finalize(pInsertStmt);
		return;
	}

	#if 1
	sqlite3_bind_int(pInsertStmt, 1, mux_chan);
	sqlite3_bind_blob(pInsertStmt, 2, p_mux_channel, sizeof(mux_parameter_t), NULL);
	#else
	sqlite3_bind_int(pInsertStmt, 1, mux_chan);
	sqlite3_bind_int(pInsertStmt, 2, p_mux_channel->enable);
	sqlite3_bind_int(pInsertStmt, 3, p_mux_channel->pid_set_auto);
	sqlite3_bind_int(pInsertStmt, 4, p_mux_channel->insert_sdt_flag);
	sqlite3_bind_int(pInsertStmt, 5, p_mux_channel->insert_nit_flag);
	sqlite3_bind_int(pInsertStmt, 6, p_mux_channel->insert_eit_flag);
	sqlite3_bind_int(pInsertStmt, 7, p_mux_channel->packet_length);
	sqlite3_bind_int(pInsertStmt, 8, p_mux_channel->ts_id);
	sqlite3_bind_int(pInsertStmt, 9, p_mux_channel->network_id);
	sqlite3_bind_int(pInsertStmt, 10, p_mux_channel->ori_network_id);
	sqlite3_bind_int(pInsertStmt, 11, p_mux_channel->total_bitrate);
	#endif

	#if 0
	printf("----save_remux_parameters  %d channel :  enable = %d ...\n", mux_chan, p_mux_channel->enable);
	printf("----save_remux_parameters  %d channel :  pid_set_auto = %d ...\n", mux_chan, p_mux_channel->pid_set_auto);
	printf("----save_remux_parameters  %d channel :  insert_sdt_flag = %d ...\n", mux_chan, p_mux_channel->insert_sdt_flag);
	printf("----save_remux_parameters  %d channel :  insert_eit_flag = %d ...\n", mux_chan, p_mux_channel->insert_nit_flag);
	printf("----save_remux_parameters  %d channel :  insert_eit_flag = %d ...\n", mux_chan, p_mux_channel->insert_eit_flag);
	printf("----save_remux_parameters  %d channel :  packet_length = %d ...\n", mux_chan, p_mux_channel->packet_length);
	printf("----save_remux_parameters  %d channel :  ts_id = %d ...\n", mux_chan, p_mux_channel->ts_id);
	printf("----save_remux_parameters  %d channel :  network_id = %d ...\n", mux_chan, p_mux_channel->network_id);
	printf("----save_remux_parameters  %d channel :  ori_network_id = %d ...\n", mux_chan, p_mux_channel->ori_network_id);
	printf("----save_remux_parameters  %d channel :  total_bitrate = %d ...\n", mux_chan, p_mux_channel->total_bitrate);
	#endif

	res = sqlite3_step(pInsertStmt);
	if(res != SQLITE_DONE){
		sqlite3_finalize(pInsertStmt);
		printf("sqlite3_step::%s is failed\n", insert_sql);
		
		exec_sql("rollback;");
		
		printf("save %d mux_channel remux_parameters faild\n", mux_chan);
		return;
		
	}

	
	exec_sql("commit;");
	sqlite3_finalize(pInsertStmt);
	printf("save %d mux_channel remux_parameters:: success\n", mux_chan);
	
}


int save_capture_ctrl(void)
{
	u_int ctl_params;

	ctl_params = (capture_control.auto_capture & 1) << 0;
	
	return replace_db_integer("capture_control", 0, ctl_params);
}

int save_channel_capture_ctrl(int idx)
{
	u_int ctl_params;
	
	ctl_params = (ts_in_channels[idx].dowith_eit_pf & 1) << 8;
	ctl_params |= (ts_in_channels[idx].dowith_eit_s & 1) << 9;
	ctl_params |= (ts_in_channels[idx].dowith_nit & 1) << 10;
	return replace_db_integer("channel_capture_control", idx, ctl_params);
}

void save_capture_control_params(void)
{
	int i;
	
	save_capture_ctrl();

	for(i=0; i<mux_info.total_tsin_channels; i++){
		save_channel_capture_ctrl(i);
	}
	
}

void read_remux_parameters(void)
{
	int mux_chan;
	for(mux_chan = 0; mux_chan < mux_info.mux_channels; mux_chan++){
		select_remux_parameters(mux_chan);
	}
	
}

void read_pid_filter_table(void)
{

	int mux_chan;
	for(mux_chan = 0; mux_chan < mux_info.mux_channels; mux_chan++){
		select_remux_pid_filter(mux_chan);
		select_remux_pass_pid(mux_chan);
	}

}

void read_mux_programs(void)
{
	
	int mux_chan;

	
	for(mux_chan = 0; mux_chan < mux_info.mux_channels; mux_chan++){
		
		select_remux_programs(mux_chan);
		
		//print_all_mux_program(mux_chan);

	}

}

static int remux_parameters_version = 5;

void read_all_remux_data(void)
{
	#if 1
	int mux_chan;
	int ret;
	int version;
	mux_program_t *p_mux_program;
	mux_section_t *p_mux_section;

	if(select_db_integer("remux_parameters_version", 0, &version)==0){
		if(version != remux_parameters_version){	
			drop_db_table("remux_parameters");
			drop_db_table("remux_pid_filter");
			drop_db_table("remux_pass_pid");
			drop_db_table("remux_program");
			drop_db_table("remux_section");
			create_remux_table();
			replace_db_integer("remux_parameters_version", 0, remux_parameters_version);
			return;
		}
	}else{
		drop_db_table("remux_parameters");
		drop_db_table("remux_pid_filter");
		drop_db_table("remux_pass_pid");
		drop_db_table("remux_program");
		drop_db_table("remux_section");
		create_remux_table();
		replace_db_integer("remux_parameters_version", 0, remux_parameters_version);
		return;
	}

	
	select_capture_control_params();
	
	for(mux_chan = 0; mux_chan < mux_info.mux_channels; mux_chan++){
	
		select_remux_parameters(mux_chan);
		select_remux_pid_filter(mux_chan);
		select_remux_pass_pid(mux_chan);
		select_remux_programs(mux_chan);
		
		ret = select_remux_section(mux_chan, -1, -1, &mux_channels[mux_chan].mux_sections);
		if(ret<=0){
			continue;
		}
		mux_channels[mux_chan].section_count = ret;

		p_mux_program = mux_channels[mux_chan].mux_programs;
		while(p_mux_program){

			p_mux_section = mux_channels[mux_chan].mux_sections;
			while(p_mux_section){

				if(p_mux_section->pid == (p_mux_program->mux_pmt_pid & 0x1fff)){
					p_mux_program->p_section = p_mux_section;
					break;
				}

				p_mux_section = p_mux_section->next;
			}

			p_mux_program = p_mux_program->next;
		}
		
		//print_all_mux_program(mux_chan);

	}
	#else
	read_remux_parameters();
	read_pid_filter_table();
	read_mux_programs();
	#endif
	
	
}

void save_remux_data(int mux_chan)
{
	
	save_remux_parameters(mux_chan);
	save_remux_pid_filter(mux_chan);
	save_remux_pass_pid(mux_chan);
	save_remux_programs(mux_chan);
	save_remux_section(mux_chan, mux_channels[mux_chan].mux_sections);
}

