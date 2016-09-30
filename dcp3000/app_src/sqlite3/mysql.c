
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

#include "app_sql.h"

#include "../utils/utils.h"
#include "../utils/queue.h"
#include "../utils/hashmap.h"

#include "mysql.h"


static char *db_datatype_str[6] = {
	"none",
	"integer",
	"real",
	"text",
	"blob",
	"none"
};

static char *db_logical_str[3] = {
	"AND",
	"OR",
	"NOT"
};

static char *db_condition_str[8] = {
	"=",
	"!=",
	">",
	">=",
	"<",
	"<=",
	"like",
	"in"
};


#define add_db_result_row(db_result, db_row)		list_add(&((db_result)->rowlist), (db_row), ASSIGN_FLAG)


db_table_st *alloc_db_table(char *name, int field_qty)
{
	int i, datasize;
	db_table_st *table;

	//printf("alloc_db_table : %s %d fields \n", name, field_qty);
		
	datasize = sizeof(db_field_st)*field_qty;

	table = xMalloc(sizeof(db_table_st)+datasize);
	
	table->field_count = field_qty;
	table->name = cpstring(name);
	
	memset(table->fields, 0, datasize);
	
	for(i=0; i<field_qty; i++){
		table->fields[i].index = i;
	}

	return table;
}

void free_db_table(db_table_st *table)
{
	int i;
	
	//printf("free_db_table : %s %d fields \n", table->name, table->field_count);
	
	if(table){
		free(table->name);
		for(i=0; i<table->field_count; i++){
			free(table->fields[i].name);
		}
		free(table);
	}
}

void set_db_table_field(db_table_st *table, int idx , char *name, int data_type)
{
	db_field_st *field;

	if((table == NULL) || (table->field_count <= idx)){
		return;
	}

	//printf("set_db_table_field : %s table field[%d]  %s %s \n", table->name, idx, name, db_datatype_str[data_type]);
	
	field = &table->fields[idx];

	field->type = data_type;
	if(field->name){
		free(field->name);
	}
	field->name = cpstring(name);
	
}

void set_db_field_constraint(db_table_st *table, int idx, int constraint, void *constraint_value)
{
	db_field_st *field;
	
	if((table == NULL) || (table->field_count <= idx)){
		return;
	}


	field = &table->fields[idx];
	
	//printf("set_db_field_constraint : %s table field[%d] %s constraint %d %p\n", table->name, idx, field->name, constraint, constraint_value);
	
	field->constraint = constraint;
	field->constraint_value = (u_int)constraint_value;
}


void set_db_data(db_data_st *db_data, db_field_st *field, void *data, int size, int flag)
{
	if(db_data == NULL){
		return;
	}

	//printf("set_db_data : %p field[%d]  %s size %d \n", db_data, field->index, field->name, size);
	
	db_data->field = field;
	db_data->data = data;
	db_data->size = size;
	db_data->flag = flag;
	
}

void set_db_data_condition(db_data_st *db_data, int condition, int logical)
{
	if(db_data == NULL){
		return;
	}
	
	db_data->condition= condition;
	db_data->logical = logical;
	
}

db_data_st *alloc_db_data(db_field_st *field, void *data, int size, int freeflag)
{
	db_data_st *db_data;

	db_data = xMalloc(sizeof(db_data_st));

	set_db_data(db_data, field, data, size, freeflag);

	return db_data;
}

db_record_st *alloc_db_record(int data_count)
{
	int datasize;
	db_record_st *record;

	if(data_count<=0){
		return NULL;
	}

	datasize = sizeof(db_data_st)*data_count;
	
	record = xMalloc(sizeof(db_record_st) + datasize);
	record->count = data_count;
	memset(record->db_data, 0, datasize);
	
	return record;
}

db_result_st *alloc_db_result(int column_count)
{
	int datasize;
	db_result_st *result;

	if(column_count<=0){
		return NULL;
	}

	datasize = sizeof(db_column_st)*column_count;
	
	result = xMalloc(sizeof(db_result_st) + datasize);
	result->column_count = column_count;
	memset(result->column, 0, datasize);
	init_list(&result->rowlist, STRUCT_DATA);
	
	return result;
}

void free_db_row(db_row_st *db_row)
{
	int i;
	
	if(db_row){
		for(i=0; i<db_row->count; i++){
			free(db_row->value[i].data);
		}
		free(db_row);
	}
}

void free_db_result(db_result_st *db_result)
{
	int i;
	db_row_st *db_row;
	
	if(db_result){
		while(!list_isEmpty(&db_result->rowlist)){
			db_row = (db_row_st *)list_dequeue(&db_result->rowlist);
			free_db_row(db_row);
		}
		//list_empty(&db_result->rowlist);
		for(i=0; i<db_result->column_count; i++){
			free(db_result->column[i].name);
		}
		free(db_result);
	}
}

void init_db_sql(db_sql_st *db_sql, int sql_type)
{
	if(db_sql == NULL){
		return;
	}

	db_sql->type = sql_type;
	init_list(&db_sql->table_list, STRUCT_DATA);
	init_list(&db_sql->record_list, STRUCT_DATA);
	init_list(&db_sql->free_list, VOID_DATA);

	db_sql->result = NULL;
	db_sql->placeholder = NULL;
	//memset(&db_sql->placeholder, 0, sizeof(db_placeholder_st));

	db_sql->sql = NULL;

	printf("init_db_sql : type = %#x \n", sql_type);
}

db_sql_st *alloc_db_sql(int sql_type)
{
	db_sql_st *db_sql;

	db_sql = xMalloc(sizeof(db_sql_st));

	init_db_sql(db_sql, sql_type);

	return db_sql;
}

void clean_db_sql(db_sql_st *db_sql)
{
	if(db_sql){
		list_empty(&db_sql->table_list);
		list_empty(&db_sql->record_list);
		list_empty(&db_sql->free_list);
		free_db_result(db_sql->result);
		free(db_sql->placeholder);
		db_sql->result = NULL;
		db_sql->placeholder = NULL;
		db_sql->sql = NULL;
		db_sql->type = 0;
	}
	//printf("clean_db_sql \n");
}

void free_db_sql(db_sql_st *db_sql)
{
	if(db_sql){
		list_empty(&db_sql->table_list);
		list_empty(&db_sql->record_list);
		list_empty(&db_sql->free_list);
		free_db_result(db_sql->result);
		free(db_sql->placeholder);
		free(db_sql);
	}
	//printf("free_db_sql \n");
}

static int db_bind_value(sqlite3_stmt *pStmt, int idx, db_data_st *db_data)
{
	int ret;

	//printf("db_bind_value : %d , %s datasize %d \n", idx, db_datatype_str[db_data->field->type], db_data->size);

	switch(db_data->field->type){
				
		case DB_INTEGER :
			if(db_data->size <=4)
				ret = sqlite3_bind_int(pStmt, idx, (u_int)(db_data->data));
			else
				ret = sqlite3_bind_int64(pStmt, idx, *((long long int *)db_data->data));
			break;
		case DB_FLOAT :
			ret = sqlite3_bind_double(pStmt, idx, *((double *)db_data->data));
			break;
		case DB_TEXT :
			ret = sqlite3_bind_text(pStmt, idx, ((char *)db_data->data), db_data->size+1, NULL);
			break;
		case DB_BLOB:
			ret = sqlite3_bind_blob(pStmt, idx, ((void *)db_data->data), db_data->size, NULL);
			break;
		
	   	default: 
			ret = sqlite3_bind_null(pStmt, idx);
			break;
			
	}

	return ret;
}

static void db_column_value(sqlite3_stmt *pStmt, int idx, db_value_st *db_value)
{
	int datatype;

	datatype = sqlite3_column_type(pStmt, idx);

	//printf("db_column_value : %d , %s \n", idx, db_datatype_str[datatype]);
	
	switch(datatype){
				
		case DB_INTEGER :

			db_value->bytes = 4;
			db_value->data = xMalloc(db_value->bytes);

			if(db_value->bytes <=4)
				*((u_int *)db_value->data) = sqlite3_column_int(pStmt, idx);
			else
				*((long long int *)db_value->data) = sqlite3_column_int64(pStmt, idx);
			break;
		case DB_FLOAT :
			db_value->bytes = 8;
			db_value->data = xMalloc(db_value->bytes);
			*((double *)db_value->data) = sqlite3_column_double(pStmt, idx);
			break;
		case DB_TEXT :
			db_value->bytes = sqlite3_column_bytes(pStmt, idx);
			//printf("db_column_value text bytes %d \n", db_value->bytes);

			db_value->data = xMalloc(db_value->bytes);
			strcpy(((char *)db_value->data), sqlite3_column_text(pStmt, idx));
			//memcpy(((char *)db_value->data), sqlite3_column_blob(pStmt, idx), db_value->bytes);
			//((char *)db_value->data)[db_value->bytes] = 0;
			break;
		case DB_BLOB:
			db_value->bytes = sqlite3_column_bytes(pStmt, idx);
			db_value->data = xMalloc(db_value->bytes);
			memcpy(((void *)db_value->data), sqlite3_column_blob(pStmt, idx), db_value->bytes);
			break;
		
	   	default: 
			
			break;
			
	}

}

static db_placeholder_st *assemble_sqlite_dql_dml(db_record_st *record, char *sql_buf, char *cond_buf)
{
	int i, ret = 0;
	char *sql;
	db_table_st *table, *fktable;
	db_field_st *field, *fkfield;
	db_data_st *db_data;
	db_placeholder_st *placeholder;

	sql = sql_buf;
		
	sprintf(cond_buf, " WHERE 1=1");

	if(record == NULL){
		return NULL;
	}
	
	placeholder = xMalloc(sizeof(db_placeholder_st)+2*record->count);
	placeholder->after_where = 0;
	placeholder->befor_where = 0;
	placeholder->in_select = 0;
	placeholder->join_on = 0;
	memset(placeholder->place_flag, 0, 2*record->count);
	
	for(i=0; i<record->count; i++){

		db_data = db_record_data(record, i);
		field = db_data->field;
		table = tableOFfield(field);
		
		if(db_data->flag&DB_DATA_AS_RESULT){
			sprintf(sql+strlen(sql), " %s.%s,", table->name, field->name);
			placeholder->in_select++;
		}

		if(db_data->flag&DB_DATA_AS_VALUE){
			placeholder->place_flag[i] = (1<<8) | placeholder->befor_where;
			placeholder->befor_where++;
			sprintf(sql+strlen(sql), " %s=?,", field->name);
		}else if(db_data->flag&DB_DATA_AS_CONDITION){
			placeholder->place_flag[i] = (2<<8) | placeholder->after_where;
			placeholder->after_where++;
			if(db_data->condition<DB_CONDITION_IN){
				sprintf(cond_buf+strlen(cond_buf), " %s %s.%s %s ?", db_logical_str[db_data->logical]
				, table->name, field->name, db_condition_str[db_data->condition]);
			}
		}else if(db_data->flag&DB_DATA_AS_JOIN_ON){
			//placeholder->place_flag[i] = (2<<8) | placeholder->after_where;
			placeholder->join_on++;
			fkfield = (db_field_st *)(field->constraint_value);
			fktable = tableOFfield(fkfield);
			sprintf(cond_buf+strlen(cond_buf), " AND %s.%s = %s.%s"
			, table->name, field->name, fktable->name, fkfield->name);

		}
		//printf("placeholder : placeflag[%d] = %#x\n", i, placeholder->place_flag[i]);
	}

	//printf("placeholder : befor where = %d \n", placeholder->befor_where);
	//printf("placeholder : after where = %d \n", placeholder->after_where);
	//printf("placeholder : in_select = %d \n", placeholder->in_select);

	return placeholder;
	
}

static int exec_sqlite_dql_dml(db_sql_st *db_sql)
{
	int i, idx, column_count, ret = 0;
	sqlite3_stmt *pStmt;

	list_st *record_list;
	char *sql;
	db_placeholder_st *placeholder;
		
	db_row_st *db_row;
	db_result_st *result = NULL;
	db_record_st *record;
	db_data_st *db_data;

	sql = db_sql->sql;
	record_list = &db_sql->record_list;
	placeholder = db_sql->placeholder;

	if(db_sql->type != DB_SELECT)
		exec_sql("BEGIN TRANSACTION;");
	
	ret = sqlite3_prepare(sqlite_database, sql, -1, &pStmt, NULL);
	if(ret != SQLITE_OK){
		sqlite3_finalize(pStmt);
		printf("sqlite3_prepare::%s is failed\n", sql);
		return -1;
	}
	
	do{

		record = (db_record_st *)list_dequeue(record_list);

		if(record){
		
			for(i=0; i<record->count; i++){
				
				db_data = db_record_data(record, i);

					if(placeholder){
						if((placeholder->place_flag[i]>>8)==1){
							idx = 1+placeholder->place_flag[i]&0xff;
						}else if((placeholder->place_flag[i]>>8)==2){
							idx = 1+placeholder->befor_where + (placeholder->place_flag[i]&0xff);
						}else{
							
							continue;
						}
					}else{
						idx = i + 1;
					}
				
				db_bind_value(pStmt, idx, db_data);
				
			}

			
			free(record);
		}
		
		do{
			ret = sqlite3_step(pStmt);

			if(ret == SQLITE_ROW){

				if(result==NULL){
					column_count = sqlite3_column_count(pStmt);
					result = alloc_db_result(column_count);
					db_sql->result = result;
					for(i=0; i<column_count; i++){
						result->column[i].index = i;
						result->column[i].type = sqlite3_column_type(pStmt, i);
						result->column[i].name = cpstring(sqlite3_column_name(pStmt, i));
					}
				}

				db_row = xMalloc(sizeof(db_row_st)+sizeof(db_value_st)*column_count);
				db_row->count = column_count;
				
				for(i=0; i<column_count; i++){
					db_column_value(pStmt, i, &db_row->value[i]);
				}

				add_db_result_row(result, db_row);
				
			}else if(ret == SQLITE_DONE){
				break;
			}else{
				printf("sqlite3_step::%s is failed\n", sql);
				sqlite3_finalize(pStmt);
				
				if(db_sql->type != DB_SELECT)
					exec_sql("ROLLBACK TRANSACTION;");
				
				list_empty(record_list);
				list_empty(&db_sql->free_list);
				return -1;
			}

		}while(1);

		sqlite3_reset(pStmt);
		
	}while(!list_isEmpty(record_list));

	if(db_sql->type != DB_SELECT)
		exec_sql("COMMIT TRANSACTION;");
	
	sqlite3_finalize(pStmt);
	list_empty(&db_sql->free_list);

	return 0;
}

/*
	"PRIMARY KEY(field1,field2,...)",
	"FOREIGN KEY(field) REFERENCES f_table(f_fieled)",
	"UNIQUE",
	"CHECK(####)",
	"DEFAULT ###",
	"NOT NULL",
//*/
static int create_sqlite_db_table(db_table_st *table)
{	
	int i, ret;
	db_table_st *fktable;
	db_field_st *field, *fkfield;
	char sql[2048];
	char pk_count = 0, fk_count = 0;
	char pk_idx[12], fk_idx[20];
	
	if(table == NULL){
		return -1;
	}

	sprintf(sql, "CREATE TABLE IF NOT EXISTS %s ( ", table->name);
	for(i=0; i<table->field_count; i++){
		field = &table->fields[i];
		switch(field->constraint){
			case DB_CONSTRAINT_PK :
				pk_idx[pk_count++] = i;
				sprintf(sql+strlen(sql), "%s %s,", field->name, db_datatype_str[field->type]);
				break;
			case DB_CONSTRAINT_FK :
				if(field->constraint_value){
					fk_idx[fk_count++] = i;
				}
				sprintf(sql+strlen(sql), "%s %s,", field->name, db_datatype_str[field->type]);
				break;
			case DB_CONSTRAINT_UNIQUE :
				sprintf(sql+strlen(sql), "%s %s UNIQUE,", field->name, db_datatype_str[field->type]);
				break;
			case DB_CONSTRAINT_NOT_NULL :
				sprintf(sql+strlen(sql), "%s %s NOT NULL,", field->name, db_datatype_str[field->type]);
				break;
			case DB_CONSTRAINT_CHECH :
				if(field->constraint_value)
				sprintf(sql+strlen(sql), "%s %s CHECK(%s),", field->name, db_datatype_str[field->type], (char *)(field->constraint_value));
				break;
			case DB_CONSTRAINT_DEFAULT :
				switch(field->type){
					case DB_INTEGER :
						sprintf(sql+strlen(sql), "%s %s DEFAULT %u,", field->name, db_datatype_str[field->type]
							, (u_int)(field->constraint_value));
						break;
					case DB_FLOAT :
						sprintf(sql+strlen(sql), "%s %s DEFAULT %f,", field->name, db_datatype_str[field->type]
							, *((double *)(field->constraint_value)));
						break;
					case DB_TEXT :
						sprintf(sql+strlen(sql), "%s %s DEFAULT '%s',", field->name, db_datatype_str[field->type]
							, (char *)(field->constraint_value));
						break;
				}
				break;
			default :
				sprintf(sql+strlen(sql), "%s %s , ", field->name, db_datatype_str[field->type]);
		}
	}

	for(i=0; i<fk_count; i++){
		field = &table->fields[fk_idx[i]];
		fkfield = (db_field_st *)(field->constraint_value);
		fktable = tableOFfield(fkfield);
		sprintf(sql+strlen(sql), "FOREIGN KEY(%s) REFERENCES %s(%s),",  field->name, fktable->name, fkfield->name);
	}

	if(pk_count){
		sprintf(sql+strlen(sql), " PRIMARY KEY(");
		for(i=0; i<pk_count; i++){
			sprintf(sql+strlen(sql), "%s,", table->fields[pk_idx[i]].name);
		}
		sprintf(sql+strlen(sql)-1, "));");
	}else{
		sprintf(sql+strlen(sql)-1, ");");
	}


	return exec_sql(sql);
}

static int drop_sqlite_db_table(db_table_st *table)
{	
	int i, ret;
	db_field_st *field;
	char sql[128];
	
	
	if(table == NULL){
		return -1;
	}

	sprintf(sql, "DROP TABLE IF EXISTS %s;", table->name);

	return exec_sql(sql);
}

static int insert_or_replace_sqlite_db_record(db_sql_st *db_sql)
{
	int i, ret = 0;
	char sql[1024];	
	sqlite3_stmt *pStmt;
	
	db_table_st *table;
	db_field_st *field;
	db_record_st *record;
	db_data_st *db_data;

	list_st *record_list;

	record_list = &db_sql->record_list;

	if(record_list==NULL || list_isEmpty(record_list)){
		return -1;
	}

	record = (db_record_st *)list_head(record_list);
	
	field = db_record_field(record, 0);
	
	
	table = tableOFfield(field);

	if(db_sql->type == DB_INSERT)
		sprintf(sql, "INSERT INTO %s ( ", table->name);
	else if(db_sql->type == DB_REPLACE)
		sprintf(sql, "REPLACE INTO %s ( ", table->name);

	for(i=0; i<record->count-1; i++){
		field = db_record_field(record, i);
		sprintf(sql+strlen(sql), " %s,", field->name);
	}

	field = db_record_field(record, i);
	sprintf(sql+strlen(sql), " %s) VALUES (", field->name);

	for(i=1; i<record->count; i++){
		sprintf(sql+strlen(sql), " ?,");
	}
	
	sprintf(sql+strlen(sql), " ?);");

	printf("%s \n", sql);

	db_sql->sql = sql;

	return exec_sqlite_dql_dml(db_sql);
	
}
	
static int update_sqlite_db_table(db_sql_st *db_sql)
{
	int i, ret = 0;
	char sql[2048];
	char cond_buf[1024];
	
	db_table_st *table;
	db_field_st *field;
	db_record_st *record;
	db_placeholder_st *placeholder;
		
	
	table = (db_table_st *)list_head(&db_sql->table_list);
	record = (db_record_st *)list_head(&db_sql->record_list);

	if(table==NULL){
		table = tableOFfield(db_record_field(record, 0));
	}
	
	sprintf(sql, "UPDATE %s SET ", table->name);
	
	
	placeholder = assemble_sqlite_dql_dml(record, sql, cond_buf);

	sprintf(sql+strlen(sql)-1, " %s ;", cond_buf);

	printf("%s \n", sql);

	db_sql->placeholder = placeholder;
	db_sql->sql = sql;
	
	ret = exec_sqlite_dql_dml(db_sql);
	
	return ret;

}

	
static int select_sqlite_db_table(db_sql_st *db_sql)
{
	int i, ret = 0;
	char sql[2048];
	char cond_buf[1024];
	
	db_table_st *table;
	db_field_st *field;
	db_record_st *record;
	db_placeholder_st *placeholder;
		

	sprintf(sql, "SELECT ");

	record = (db_record_st *)list_head(&db_sql->record_list);
	
	placeholder = assemble_sqlite_dql_dml(record, sql, cond_buf);
	
	if(placeholder && placeholder->in_select > 0){
		sprintf(sql+strlen(sql)-1, " FROM ");
	}else{
		sprintf(sql+strlen(sql)-1, " * FROM ");
	}
	
	while(!list_isEmpty(&db_sql->table_list)){
		table = (db_table_st *)list_dequeue(&db_sql->table_list);
		sprintf(sql+strlen(sql), " %s,", table->name);

	}

	sprintf(sql+strlen(sql)-1, " %s ;", cond_buf);

	printf("%s \n", sql);

	db_sql->placeholder = placeholder;
	db_sql->sql = sql;

	return exec_sqlite_dql_dml(db_sql);

}


static int delete_sqlite_db_record(db_sql_st *db_sql)
{
	int i, ret = 0;
	char sql[2048];
	char cond_buf[1024];
	
	db_table_st *table;
	db_field_st *field;
	db_record_st *record;
	db_placeholder_st *placeholder;
		

	sprintf(sql, "DELETE FROM ");

	while(!list_isEmpty(&db_sql->table_list)){
		table = (db_table_st *)list_dequeue(&db_sql->table_list);
		sprintf(sql+strlen(sql), " %s ,", table->name);
	}

	record = (db_record_st *)list_head(&db_sql->record_list);
	
	placeholder = assemble_sqlite_dql_dml(record, sql, cond_buf);

	sprintf(sql+strlen(sql)-1, " %s ;", cond_buf);

	printf("%s \n", sql);

	db_sql->placeholder = placeholder;
	db_sql->sql = sql;
	
	return exec_sqlite_dql_dml(db_sql);

}


int exec_db_sql(db_sql_st *db_sql)
{
	int ret = 0;
	db_table_st *table;
	
	if(db_sql == NULL){
		return -1;
	}

	printf("exec_db_sql : %d \n", db_sql->type);

	switch(db_sql->type){
		case	DB_CREATE_TABLE :
			while(!list_isEmpty(&db_sql->table_list)){
				table = (db_table_st *)list_dequeue(&db_sql->table_list);
				ret = create_sqlite_db_table(table);
			}
			break;
		case	DB_DROP_TABLE :
			while(!list_isEmpty(&db_sql->table_list)){
				table = (db_table_st *)list_dequeue(&db_sql->table_list);
				ret = drop_sqlite_db_table(table);
			}
			break;
		case	DB_INSERT :
		case	DB_REPLACE :
			ret = insert_or_replace_sqlite_db_record(db_sql);
			break;
		case	DB_UPDATE :
			ret = update_sqlite_db_table(db_sql);
			break;
		case	DB_DELETE :
			ret = delete_sqlite_db_record(db_sql);
			break;
		case	DB_SELECT :
			ret = select_sqlite_db_table(db_sql);
			break;
	}

	return ret;
}

void print_blob(void *data, int len)
{
	int i;
	char *pdata = data;

	for(i=0; i<len; i++){
		printf("%#.2x,", pdata[i]);
	}
}

void print_db_result(db_result_st *result)
{
	int i;
	db_row_st *db_row;
	
	if(result == NULL){
		return;
	}
	for(i=0; i<result->column_count; i++){
		printf("%s \t", db_datatype_str[result->column[i].type]);
	}
	printf("\n");
	for(i=0; i<result->column_count; i++){
		printf("%s \t", result->column[i].name);
	}
	printf("\n");

	db_row = (db_row_st *)list_access(&result->rowlist, 0);
	
	while(db_row){
		for(i=0; i<result->column_count; i++){
			switch(result->column[i].type){
				case	DB_INTEGER :
					printf("%#x \t", *((u_int *)(db_row->value[i].data)));
					break;
				case	DB_FLOAT :
					printf("%f \t", *((double *)(db_row->value[i].data)));
					break;
				case	DB_TEXT :
					printf("%s \t", (char *)(db_row->value[i].data));
					break;
				case	DB_BLOB:
					print_blob(db_row->value[i].data, db_row->value[i].bytes);
					break;
			}
			
		}
		printf("\n");
		db_row = (db_row_st *)list_next(&result->rowlist);
	}

	list_access_end(&result->rowlist);
	
}


#ifdef USE_CMD_LINE
hashmap_st *dbtable_map;

int debug_mysql(int argc, char *argv[])
{
	u_int v_u32, *pv_u32;
	double v_lf, *pv_lf;
	int idx, len, size, v_s32;
	void *pdata;
	u_char databuf[1024];
	static db_sql_st *db_sql;
	static db_record_st *record;
	db_data_st *db_data;
	db_table_st *db_table, *db_table2;
	db_field_st *db_field;
	int constraint, datatype;


	if(strcmp("init", argv[1])==0){
		dbtable_map = hashmap_create(STRING_KEY, 0);
		//init_db_sql(db_sql, 0);
		db_sql = alloc_db_sql(0);
	}else if(strcmp("end", argv[1])==0){
		free_db_sql(db_sql);
		hashmap_destroy(dbtable_map, free_db_table);
		dbtable_map = NULL;
		db_sql = NULL;
	}else if(strcmp("table", argv[1])==0){
		if(argc<4){return -1;}
		db_table  = alloc_db_table(argv[2], atoi(argv[3]));
		hashmap_put(dbtable_map, db_table->name, db_table);
	}else if(strcmp("field", argv[1])==0){
		if(argc<6){return -1;}
		idx = atoi(argv[3]);
		datatype = atoi(argv[5]);
		db_table = hashmap_get(dbtable_map, argv[2]);
		set_db_table_field(db_table, idx, argv[4], datatype);

		if(argc>6){
			constraint = atoi(argv[6]);
			switch(constraint){
				case DB_CONSTRAINT_DEFAULT :
					switch(datatype){
						case DB_INTEGER :
							pdata = (void *)strtoul(argv[7], NULL, 16);
							break;
						case DB_FLOAT :
							pdata = xMalloc(8);
							*((double *)pdata) = atof(argv[7]);
							add_db_sql_freedata(db_sql, pdata);
							break;
						case DB_TEXT :
							pdata = cpstring(argv[7]);
							add_db_sql_freedata(db_sql, pdata);
							break;
						default :
							pdata = NULL;
					}
					break;
				case DB_CONSTRAINT_CHECH :
					pdata = cpstring(argv[7]);
					add_db_sql_freedata(db_sql, pdata);
					break;
				case DB_CONSTRAINT_FK :
					pdata = NULL;
					if(argc>7)
						db_table2 = hashmap_get(dbtable_map, argv[7]);
					if(db_table2 && (argc>8))
						pdata = tableField(db_table2, atoi(argv[8]));
					break;
				default :
					pdata = NULL;
			}
			set_db_field_constraint(db_table, idx, constraint, pdata);
		}
		
	}else if(strcmp("freetable", argv[1])==0){
		if(argc<3){return -1;}
		hashmap_remove(dbtable_map, argv[2], free_db_table);

	}else if(strcmp("addtable", argv[1])==0){
		if(argc<3){return -1;}
		db_table = hashmap_get(dbtable_map, argv[2]);
		add_db_sql_table(db_sql, db_table);
	}else if(strcmp("addrecord", argv[1])==0){
		if(argc<3){return -1;}
		record = alloc_db_record(atoi(argv[2]));
		printf("addrecord : %p \n", record);
		add_db_sql_record(db_sql, record);
	}else if(strcmp("setdata", argv[1])==0){
	
		if(argc<6){return -1;}

		idx = atoi(argv[2]);
		db_table = hashmap_get(dbtable_map, argv[3]);
		db_field = tableField(db_table, atoi(argv[4]));
	
		if(argc>6){
			record->db_data[idx].flag = atoi(argv[6]);
		}else{
			record->db_data[idx].flag = 0;
		}

		if(argc>7){
			record->db_data[idx].condition = atoi(argv[7]);
		}else{
			record->db_data[idx].condition = 0;
		}
		
		if(argc>8){
			record->db_data[idx].logical = atoi(argv[8]);
		}else{
			record->db_data[idx].logical = 0;
		}

		if((argc==6) || (record->db_data[idx].flag & 5)){
			switch(db_field->type){
				case DB_INTEGER :
					v_u32 = strtoul(argv[5], NULL, 16);
					pdata = (void *)v_u32;
					size = 4;
					break;
				case DB_FLOAT :
					size = 8;
					pdata = xMalloc(size);
					*((double *)pdata) = atof(argv[5]);
					add_db_sql_freedata(db_sql, pdata);
					break;
				case DB_TEXT :
					//pdata = argv[5];
					size = strlen(argv[5]);
					pdata = cpstring(argv[5]);
					//record->db_data[idx].freeflag = 1;
					add_db_sql_freedata(db_sql, pdata);
					break;
				case DB_BLOB :
					size = hex2byte(argv[5], strlen(argv[5]), databuf);
					pdata = databuf;
					break;
				default :
					return -1;
			}
		}else{
			size = 4;
			pdata = NULL;
		}

		set_db_data(&record->db_data[idx], db_field, pdata, size, 0);
		printf("set_db_data : flag %#x,  condition %d  logical %d \n", record->db_data[idx].flag, record->db_data[idx].condition, record->db_data[idx].logical);
	
	}else if(strcmp("exec", argv[1])==0){
		if(argc<3){return -1;}
		db_sql->type = atoi(argv[2]);
		exec_db_sql(db_sql);
	}else if(strcmp("reset", argv[1])==0){
		clean_db_sql(db_sql);
	}else if(strcmp("result", argv[1])==0){
		print_db_result(db_sql->result);
	}
	
	return 0;
}
#endif
