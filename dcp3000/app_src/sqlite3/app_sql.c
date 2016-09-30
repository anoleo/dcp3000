
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

#include "app_sql.h"

sqlite3 *sqlite_database;


void create_base_table(void);
sqlite3 *sqlite_Init(char *db_filename)
{

	if (sqlite3_open(db_filename, &sqlite_database)) {
		//printf("Can't open Database: %s\n", sqlite3_errmsg(sqlite_database));
		fprintf(stderr, "Can't open Database: %s\n", sqlite3_errmsg(sqlite_database));
		sqlite3_close(sqlite_database);
		//exit(1);
	}
	
	create_base_table();
	
	return sqlite_database;
}

void sqlite_close(void)
{
	sqlite3_close(sqlite_database);
}

int exec_sql(char *sql)
{
	char *zErrMsg = 0;	
	int err = 0 ;
	
	printf("exec_sql : %s \n", sql);
	fflush(stdout);
	
	err = sqlite3_exec(sqlite_database , sql , 0 , 0 , &zErrMsg );
	if(err != SQLITE_OK){
		printf("exec  %s Error, message = %s\n", sql, zErrMsg);
		return -1;
	}	
	
	return 0;
}

int clean_db_table(char *table_name)
{	
	char buf[100];
	
	//memset(buf, 0, 100);
	sprintf(buf,"delete from %s;",table_name);	
	
	return exec_sql(buf);
}


int drop_db_table(char *table_name)
{
	char buf[100];

	//memset(buf, 0, 100);
	sprintf(buf,"drop table %s;", table_name);
	
	return exec_sql(buf);
}

int isExist_db_table(char *table_name)
{
	char buf[100];
	char *zErrMsg;
	int i = 0, ret;

	sqlite3_stmt *pStmt;
	
	sprintf(buf, "SELECT * FROM sqlite_master WHERE type='table' and name='%s';", table_name);

	ret = sqlite3_prepare(sqlite_database, buf, -1, &pStmt, NULL);
	if(ret != SQLITE_OK){
		printf("sqlite3_prepare::%s is failed\n", buf);
		sqlite3_finalize(pStmt);
		//exit(1);
		return -1;
	}

	ret = sqlite3_step(pStmt);
	sqlite3_finalize(pStmt);
	if(ret == SQLITE_ROW){
		printf("sqlite_database table %s already exist\n", table_name);
		return 1;
	}else if(ret != SQLITE_DONE){
		printf("select table sqlite_master failed\n");
		//exit(1);
		return -1;
	}


	return 0;
}

void create_base_table(void)
{

	int err;
	char *sql;


	exec_sql("CREATE TABLE IF NOT EXISTS integer_data_table ("
				"param_name text primary key,"
				"param_value integer"
			");");


	exec_sql("CREATE TABLE IF NOT EXISTS float_data_table ("
				"param_name text primary key,"
				"param_value real"
			");");

	
	exec_sql("CREATE TABLE IF NOT EXISTS text_data_table ("
				"param_name text primary key,"
				"param_value text"
			");");


	exec_sql("CREATE TABLE IF NOT EXISTS blob_data_table ("
				"param_name text primary key,"
				"param_value blob"
			");");

}

int select_db_integer(char *param_name, u_int idx, int *p_value)
{
	char sql[256];	
	sqlite3_stmt *pStmt;
	int ret = 0 ;

	if(param_name==NULL || p_value==NULL){
		return -1;
	}

	sprintf(sql,"SELECT param_value FROM integer_data_table WHERE param_name='%s_%d';", param_name, idx);	
	
	ret = sqlite3_prepare(sqlite_database, sql, -1, &pStmt, NULL);
	if(ret != SQLITE_OK){
		printf("sqlite3_prepare::%s is failed\n", sql);
		sqlite3_finalize(pStmt);
		return -1;
	}

	ret = sqlite3_step(pStmt);
	if(ret != SQLITE_ROW){
		printf("sqlite3_step::%s, is failed\n", sql);
		ret = -1;
	}else{
		*p_value = sqlite3_column_int(pStmt, 0);
		printf("select %s_%d success, value = %d\n", param_name, idx, *p_value);
		ret = 0;
	}

	sqlite3_finalize(pStmt);
	
	return ret;
}


int insert_db_integer(char *param_name, u_int idx, int value)
{
	char sql[256];
	
	if(param_name==NULL){
		return -1;
	}

	sprintf(sql,"INSERT INTO integer_data_table (param_name,param_value) VALUES ('%s_%d',%d);", param_name, idx, value);	
	return exec_sql(sql);
}


int update_db_integer(char *param_name, u_int idx, int value)
{
	char sql[256];

	if(param_name==NULL){
		return -1;
	}

	sprintf(sql,"UPDATE integer_data_table SET param_value = %d WHERE param_name='%s_%d';", value, param_name, idx);	
	return exec_sql(sql);
}

int replace_db_integer(char *param_name, u_int idx, int value)
{
	char sql[256];

	if(param_name==NULL){
		return -1;
	}
	
	sprintf(sql,"REPLACE INTO integer_data_table (param_name,param_value) VALUES ('%s_%d',%d);", param_name, idx, value);	
	return exec_sql(sql);

}

int delete_db_integer(char *param_name, int idx)
{
	char sql[256];

	if(param_name==NULL){
		return -1;
	}
	if(idx<0){
		sprintf(sql, "DELETE FROM integer_data_table  WHERE param_name LIKE '%s';", param_name);	
	}else{
		sprintf(sql, "DELETE FROM integer_data_table  WHERE param_name='%s_%d';", param_name, idx);	
	}
	return exec_sql(sql);
}

int select_db_float(char *param_name, u_int idx, double *p_value)
{
	char sql[256];	
	sqlite3_stmt *pStmt;
	int ret = 0 ;

	if(param_name==NULL || p_value==NULL){
		return -1;
	}

	sprintf(sql,"SELECT param_value FROM float_data_table WHERE param_name='%s_%d';", param_name, idx);	
	
	ret = sqlite3_prepare(sqlite_database, sql, -1, &pStmt, NULL);
	if(ret != SQLITE_OK){
		printf("sqlite3_prepare::%s is failed\n", sql);
		sqlite3_finalize(pStmt);
		return -1;
	}

	ret = sqlite3_step(pStmt);
	if(ret != SQLITE_ROW){
		printf("sqlite3_step::%s, is failed\n", sql);
		ret = -1;
	}else{
		*p_value = sqlite3_column_double(pStmt, 0);
		printf("select %s_%d success, value = %f\n", param_name, idx, *p_value);
		ret = 0;
	}

	sqlite3_finalize(pStmt);
	
	return ret;
}

int insert_db_float(char *param_name, u_int idx, double value)
{
	char sql[256];

	if(param_name==NULL){
		return -1;
	}

	sprintf(sql,"INSERT INTO float_data_table (param_name,param_value) VALUES ('%s_%d',%f);", param_name, idx, value);	
	return exec_sql(sql);
}

int update_db_float(char *param_name, u_int idx, double value)
{
	char sql[256];

	if(param_name==NULL){
		return -1;
	}

	sprintf(sql,"UPDATE float_data_table SET param_value = %f WHERE param_name='%s_%d';", value, param_name, idx);
	return exec_sql(sql);
}


int delete_db_float(char *param_name, int idx)
{
	char sql[256];

	if(param_name==NULL){
		return -1;
	}
	if(idx<0){
		sprintf(sql, "DELETE FROM float_data_table WHERE param_name LIKE '%s';", param_name);
	}else{
		
		sprintf(sql, "DELETE FROM float_data_table WHERE param_name='%s_%d';", param_name, idx);
	}
	return exec_sql(sql);
}

int replace_db_float(char *param_name, u_int idx, double value)
{
	char sql[256];

	if(param_name==NULL){
		return -1;
	}

	sprintf(sql,"REPLACE INTO float_data_table (param_name,param_value) VALUES ('%s_%d',%f);", param_name, idx, value);	
	return exec_sql(sql);
}



int select_db_text(char *param_name, u_int idx, char *p_value)
{
	char sql[256];	
	sqlite3_stmt *pStmt;
	int ret = 0 ;

	if(param_name==NULL || p_value==NULL){
		return -1;
	}

	sprintf(sql,"SELECT param_value FROM text_data_table WHERE param_name='%s_%d';", param_name, idx);	
	
	ret = sqlite3_prepare(sqlite_database, sql, -1, &pStmt, NULL);
	if(ret != SQLITE_OK){
		printf("sqlite3_prepare::%s is failed\n", sql);
		sqlite3_finalize(pStmt);
		return -1;
	}

	ret = sqlite3_step(pStmt);

	if(ret != SQLITE_ROW){
		printf("sqlite3_step::%s, is failed\n", sql);
		ret = -1;
	}else{
		strcpy(p_value, sqlite3_column_text(pStmt, 0));
		printf("select %s_%d success, value = %s\n", param_name, idx, p_value);
		ret = 0;
	}

	sqlite3_finalize(pStmt);
	
	return ret;
}

int insert_db_text(char *param_name, u_int idx, char *p_value)
{
	char sql[512];

	if(param_name==NULL || p_value==NULL){
		return -1;
	}

	sprintf(sql,"INSERT INTO text_data_table (param_name,param_value) VALUES ('%s_%d','%s');", param_name, idx, p_value);	
	return exec_sql(sql);

}

int update_db_text(char *param_name, u_int idx, char *p_value)
{
	char sql[512];

	if(param_name==NULL || p_value==NULL){
		return -1;
	}

	sprintf(sql,"UPDATE text_data_table SET param_value = '%s' WHERE param_name='%s_%d';", p_value, param_name, idx);	
	return exec_sql(sql);

}

int replace_db_text(char *param_name, u_int idx, char *p_value)
{
	char sql[512];

	if(param_name==NULL || p_value==NULL){
		return -1;
	}

	sprintf(sql,"REPLACE INTO text_data_table (param_name,param_value) VALUES ('%s_%d','%s');", param_name, idx, p_value);	
	return exec_sql(sql);

}

int delete_db_text(char *param_name, int idx)
{
	char sql[256];

	if(param_name==NULL){
		return -1;
	}

	if(idx<0){
		sprintf(sql,"DELETE FROM text_data_table  WHERE param_name LIKE '%s';", param_name);
	}else{
		sprintf(sql,"DELETE FROM text_data_table  WHERE param_name='%s_%d';", param_name, idx);
	}
	return exec_sql(sql);

}


int select_db_blob(char *param_name, u_int idx, void *p_value, int len)
{
	char sql[256];	
	sqlite3_stmt *pStmt;
	int ret = 0 ;

	if(param_name==NULL || p_value==NULL){
		return -1;
	}

	sprintf(sql,"SELECT param_value FROM blob_data_table WHERE param_name='%s_%d';", param_name, idx);	
	
	ret = sqlite3_prepare(sqlite_database, sql, -1, &pStmt, NULL);
	if(ret != SQLITE_OK){
		printf("sqlite3_prepare::%s is failed\n", sql);
		sqlite3_finalize(pStmt);
		return -1;
	}

	ret = sqlite3_step(pStmt);

	if(ret != SQLITE_ROW){
		printf("sqlite3_step::%s, is failed\n", sql);
		ret = -1;
	}else{
		memcpy(p_value, sqlite3_column_blob(pStmt, 0), len);
		//printf("select %s success, value = %s\n", param_name, p_value);
		printf("select %s_%d success \n", param_name, idx);
		ret = 0;
	}

	sqlite3_finalize(pStmt);
	
	return ret;
}


int insert_db_blob(char *param_name, u_int idx, void *p_value, int len)
{
	char sql[256];	
	sqlite3_stmt *pStmt;
	int ret = 0 ;

	if(param_name==NULL || p_value==NULL){
		return -1;
	}

	sprintf(sql,"INSERT INTO blob_data_table (param_name,param_value) VALUES ('%s_%d',?);", param_name, idx);	
	
	ret = sqlite3_prepare(sqlite_database, sql, -1, &pStmt, NULL);
	if(ret != SQLITE_OK){
		sqlite3_finalize(pStmt);
		printf("sqlite3_prepare::%s is failed\n", sql);
		return -1;
	}
	
	sqlite3_bind_blob(pStmt, 1, p_value, len, NULL);
	
	ret = sqlite3_step(pStmt);
	sqlite3_finalize(pStmt);
	
	if(ret == SQLITE_DONE){
		//printf("insert %s success, value = %x\n", param_name, *p_value);
		printf("insert %s_%d success \n", param_name, idx);
		return 0;
	}else{
		printf("sqlite3_step::%s, is failed\n", sql);
		return -1;
	}

}

int update_db_blob(char *param_name, u_int idx, void *p_value, int len)
{
	char sql[256];	
	sqlite3_stmt *pStmt;
	int ret = 0 ;

	if(param_name==NULL || p_value==NULL){
		return -1;
	}
	
	sprintf(sql,"UPDATE  blob_data_table SET param_value=? WHERE param_name='%s_%d';", param_name, idx);
	
	ret = sqlite3_prepare(sqlite_database, sql, -1, &pStmt, NULL);
	if(ret != SQLITE_OK){
		sqlite3_finalize(pStmt);
		printf("sqlite3_prepare::%s is failed\n", sql);
		return -1;
	}
	
	sqlite3_bind_blob(pStmt, 1, p_value, len, NULL);
	
	ret = sqlite3_step(pStmt);
	sqlite3_finalize(pStmt);
	
	if(ret == SQLITE_DONE){
		//printf("update %s success, value = %x\n", param_name, *p_value);
		printf("update %s_%d success\n", param_name, idx);
		return 0;
	}else{
		printf("sqlite3_step::%s, is failed\n", sql);
		return -1;
	}

}

int replace_db_blob(char *param_name, u_int idx, void *p_value, int len)
{
	char sql[256];	
	sqlite3_stmt *pStmt;
	int ret = 0 ;

	if(param_name==NULL || p_value==NULL){
		return -1;
	}

	sprintf(sql,"REPLACE INTO blob_data_table (param_name,param_value) VALUES ('%s_%d',?);", param_name, idx);	
	
	ret = sqlite3_prepare(sqlite_database, sql, -1, &pStmt, NULL);
	if(ret != SQLITE_OK){
		sqlite3_finalize(pStmt);
		printf("sqlite3_prepare::%s is failed\n", sql);
		return -1;
	}
	
	sqlite3_bind_blob(pStmt, 1, p_value, len, NULL);
	
	ret = sqlite3_step(pStmt);
	sqlite3_finalize(pStmt);
	
	if(ret == SQLITE_DONE){
		//printf("insert %s success, value = %x\n", param_name, *p_value);
		printf("replace %s_%d success \n", param_name, idx);
		return 0;
	}else{
		printf("sqlite3_step::%s, is failed\n", sql);
		return -1;
	}

}

int delete_db_blob(char *param_name, int idx)
{
	char sql[256];

	if(param_name==NULL){
		return -1;
	}
	if(idx<0){
		sprintf(sql,"DELETE FROM blob_data_table  WHERE param_name LIKE '%s';", param_name);	
	}else{
		sprintf(sql,"DELETE FROM blob_data_table  WHERE param_name='%s_%d';", param_name, idx);
	}
	return exec_sql(sql);

}


