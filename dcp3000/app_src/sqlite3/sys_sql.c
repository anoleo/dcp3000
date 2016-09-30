
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "app_sql.h"
#include "sys_sql.h"

#include "../appref.h"


void create_system_table(void)
{
	
	int err;
	char *sql;

	
	exec_sql("CREATE TABLE IF NOT EXISTS transceiver_route_table("
				"dest_slot integer,"
				"route_id integer,"
				"src_slot integer,"
				"trans_chan integer,"
				"primary key (dest_slot, route_id)"
			");");

}


int select_slot_webversion(int slot, int *version)
{	
	return select_db_integer("slot_webversion", slot, version);
	}

int update_slot_webversion(int slot, int version)
{
	return replace_db_integer("slot_webversion", slot, version);
}

#ifdef SUBROUTINE
void save_transceiver_route(trans_route_st*p_route)
{
	char buf[200];	
	char *zErrMsg = 0;	
	sqlite3_stmt *pInsertStmt;
	char *sql;
	int res;

	if(p_route == NULL){
		return;
	}

	InfoPrintf("dest_trans_if = %d, route_id = %d ,  trans_chan = %#x \n"
		, p_route->trans_if, p_route->route_id, p_route->trans_chan);

	
 	sql = "REPLACE INTO transceiver_route_table ("
				"dest_slot, route_id, src_slot, trans_chan) "
			"VALUES (?, ?, ?, ?);";

	res = sqlite3_prepare(sqlite_database, sql, -1, &pInsertStmt, NULL);
	if(res != SQLITE_OK){
		printf("sqlite3_prepare::%s is failed\n", sql);
		sqlite3_finalize(pInsertStmt);
		return;
	}

	sqlite3_bind_int(pInsertStmt, 1, transceiver_slot[p_route->trans_if]);
	sqlite3_bind_int(pInsertStmt, 2, p_route->route_id);
	//sqlite3_bind_int(pInsertStmt, 3, transceiver_slot[p_route->psrc_chan->trans_if]);
	sqlite3_bind_int(pInsertStmt, 4, p_route->trans_chan);
	
	res = sqlite3_step(pInsertStmt);
	if(res != SQLITE_DONE){
		printf("sqlite3_step::%s is failed\n", sql);
	}
	
	sqlite3_finalize(pInsertStmt);


}

void delete_transceiver_route(int dest_slot, int route_id)
{
	char buf[200];	
	char *zErrMsg = 0;	
	sqlite3_stmt *pInsertStmt;
	char *sql;
	int res;


 	sprintf(buf,"DELETE FROM transceiver_route_table WHERE dest_slot=%d AND route_id=%d ;", dest_slot, route_id);
	//sprintf(buf,"delete from transceiver_route_table where trans_if=%d ;", trans_if);

	exec_sql(buf);
		
}


void delete_transceiver_routetable(int slot)
{
	char buf[200];	
	char *zErrMsg = 0;	
	sqlite3_stmt *pInsertStmt;
	char *sql;
	int res;
 
	sprintf(buf,"DELETE FROM transceiver_route_table WHERE dest_slot=%d ;", slot);

	exec_sql(buf);

}

int transceiver_route_add(int dest_slot, int route_id, int trans_chan);
int select_transceiver_routetable(int slot)
{
	
	sqlite3_stmt *pStmt;
	char *sql;
	int res, record_count;


	transceiver_st *p_transceiver;

	int src_slot;
	int trans_chan, route_id;


	printf("select %d transceiver route table \n", slot);
 
	p_transceiver = &transceiver[slot];

	if((sysinfo->slot_info[slot].old_param_isvalid == 0)
		//|| (sysinfo.slot_info[slot].board_changed == 0)
	){
		//del_trasceiver_channels(trans_if);
		return 0;
	}
	
	sql = "SELECT route_id, src_slot, trans_chan FROM  transceiver_route_table "
				"WHERE dest_slot=?;";

	res = sqlite3_prepare(sqlite_database, sql, -1, &pStmt, NULL);
	if(res != SQLITE_OK){
		printf("sqlite3_prepare::%s is failed\n", sql);
		sqlite3_finalize(pStmt);
		return -1;
	}
	
	sqlite3_bind_int(pStmt, 1, slot);

	record_count = 0;
	do{
		res = sqlite3_step(pStmt);

		if(res == SQLITE_ROW){

			route_id = sqlite3_column_int(pStmt, 0);
			//src_slot  = sqlite3_column_int(pStmt, 1);
			trans_chan = sqlite3_column_int(pStmt, 2);
			
			transceiver_route_add(slot, route_id, trans_chan);
			
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
	
	printf("select %d transceiver routetable :: record count = %d\n", slot, record_count);

	return record_count;
}
#endif

