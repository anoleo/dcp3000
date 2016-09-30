
#ifndef __APP_SQL_H__
#define __APP_SQL_H__


#include "sqlite3.h"

//#include <pthread.h>


extern sqlite3 *sqlite_database;


sqlite3 *sqlite_Init(char *db_filename);
void sqlite_close(void);

int exec_sql(char *sql);

int clean_db_table(char *table_name);
int drop_db_table(char *table_name);

//-1 failed, 0 not exist, 1 exist
int isExist_db_table(char *table_name);



int select_db_integer(char *param_name, u_int idx, int *p_value);
int insert_db_integer(char *param_name, u_int idx, int value);
int update_db_integer(char *param_name, u_int idx, int value);
int replace_db_integer(char *param_name, u_int idx, int value);
int delete_db_integer(char *param_name, int idx);

int select_db_float(char *param_name, u_int idx, double *p_value);
int insert_db_float(char *param_name, u_int idx, double value);
int update_db_float(char *param_name, u_int idx, double value);
int replace_db_float(char *param_name, u_int idx, double value);
int delete_db_float(char *param_name, int idx);

int select_db_text(char *param_name, u_int idx, char *p_value);
int insert_db_text(char *param_name, u_int idx, char *p_value);
int update_db_text(char *param_name, u_int idx, char *p_value);
int replace_db_text(char *param_name, u_int idx, char *p_value);
int delete_db_text(char *param_name, int idx);

int select_db_blob(char *param_name, u_int idx, void *p_value, int len);
int insert_db_blob(char *param_name, u_int idx, void *p_value, int len);
int update_db_blob(char *param_name, u_int idx, void *p_value, int len);
int replace_db_blob(char *param_name, u_int idx, void *p_value, int len);
int delete_db_blob(char *param_name, int idx);


#endif


