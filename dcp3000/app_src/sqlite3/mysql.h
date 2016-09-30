
#ifndef __MY_SQL_H__
#define __MY_SQL_H__

#include "../utils/list.h"

// primary key, foreign key, unique key, check , default, 
#define DB_DATA_AS_CONDITION	1
#define DB_DATA_AS_RESULT	2
#define DB_DATA_AS_VALUE	4
#define DB_DATA_AS_JOIN_ON	8

// primary key, foreign key, unique key, check , default, not null
#define DB_CONSTRAINT_PK	1
#define DB_CONSTRAINT_FK	2
#define DB_CONSTRAINT_UNIQUE	4
#define DB_CONSTRAINT_CHECH	8
#define DB_CONSTRAINT_DEFAULT	16
#define DB_CONSTRAINT_NOT_NULL	32


#define DB_LOGICAL_AND	0
#define DB_LOGICAL_OR	1
#define DB_LOGICAL_NOT	2

/*
±»Ωœ‘ÀÀ„∑˚  <=£¨<,=,>,>=,!=,<>,!>,!<  
∑∂Œß‘ÀÀ„∑˚  between... and,not between... and
¡–æŸ‘ÀÀ„∑˚  in,not in
ƒ£∫˝∆•≈‰‘ÀÀ„∑˚  like,not like
ø’÷µ‘ÀÀ„∑˚  is null,is not null
¬ﬂº≠‘ÀÀ„∑˚  and,or,not
//*/
#define DB_CONDITION_EQ	0
#define DB_CONDITION_NE	1
#define DB_CONDITION_GT	2
#define DB_CONDITION_GE	3
#define DB_CONDITION_LT	4
#define DB_CONDITION_LE	5
#define DB_CONDITION_LIKE	6
#define DB_CONDITION_IN	7


#define DB_INTEGER  1
#define DB_FLOAT    2
#define DB_TEXT     3
#define DB_BLOB     4
#define DB_NULL     5

#define DB_CREATE_INDEX          1   /* Index Name      Table Name      */
#define DB_CREATE_TABLE          2   /* Table Name      NULL            */
#define DB_CREATE_TEMP_INDEX     3   /* Index Name      Table Name      */
#define DB_CREATE_TEMP_TABLE     4   /* Table Name      NULL            */
#define DB_CREATE_TEMP_TRIGGER   5   /* Trigger Name    Table Name      */
#define DB_CREATE_TEMP_VIEW      6   /* View Name       NULL            */
#define DB_CREATE_TRIGGER        7   /* Trigger Name    Table Name      */
#define DB_CREATE_VIEW           8   /* View Name       NULL            */
#define DB_DELETE                9   /* Table Name      NULL            */
#define DB_DROP_INDEX           10   /* Index Name      Table Name      */
#define DB_DROP_TABLE           11   /* Table Name      NULL            */
#define DB_DROP_TEMP_INDEX      12   /* Index Name      Table Name      */
#define DB_DROP_TEMP_TABLE      13   /* Table Name      NULL            */
#define DB_DROP_TEMP_TRIGGER    14   /* Trigger Name    Table Name      */
#define DB_DROP_TEMP_VIEW       15   /* View Name       NULL            */
#define DB_DROP_TRIGGER         16   /* Trigger Name    Table Name      */
#define DB_DROP_VIEW            17   /* View Name       NULL            */
#define DB_INSERT               18   /* Table Name      NULL            */
#define DB_PRAGMA               19   /* Pragma Name     1st arg or NULL */
#define DB_READ                 20   /* Table Name      Column Name     */
#define DB_SELECT               21   /* NULL            NULL            */
#define DB_TRANSACTION          22   /* Operation       NULL            */
#define DB_UPDATE               23   /* Table Name      Column Name     */
#define DB_ATTACH               24   /* Filename        NULL            */
#define DB_DETACH               25   /* Database Name   NULL            */
#define DB_ALTER_TABLE          26   /* Database Name   Table Name      */
#define DB_REINDEX              27   /* Index Name      NULL            */
#define DB_ANALYZE              28   /* Table Name      NULL            */
#define DB_CREATE_VTABLE        29   /* Table Name      Module Name     */
#define DB_DROP_VTABLE          30   /* Table Name      Module Name     */
#define DB_FUNCTION             31   /* NULL            Function Name   */
#define DB_SAVEPOINT            32   /* Operation       Savepoint Name  */
#define DB_COPY                  0   /* No longer used */

#define DB_REPLACE               33   /* Table Name      Column Name     */

#define tableOFfield(field)		(db_table_st *)(((u_int)(field) - sizeof(db_field_st) * (field)->index) - sizeof(db_table_st))
#define tableField(table, idx)	&((table)->fields[(idx)])

typedef struct{
	char *name;
	u_char type;	// integer, float, text, block ... ... 
	u_char index;	// begin with 0
	u_char constraint;	// primary key, foreign key, unique key, check , default, 
	char reserved;
	u_int constraint_value;	// is a 4 bytes value, or a data point,  when it is check , the value is a char string, 
}db_field_st;

typedef struct {
	char *name;
	int field_count;
	db_field_st fields[0];
}db_table_st;

typedef struct{
	db_field_st *field;
	void *data;	// is a datapoint of will be assigned or fetch values from it
	int size;
	char flag;	// is as a reault or condition or both
	char condition;		// eq, lt , le, gt, ge, in ... ...  
	char logical;	// and , or, not
	char freeflag;
}db_data_st;


typedef struct {

	int count;
	db_data_st db_data[0];

}db_record_st;


typedef struct{
	void *data;
	int  bytes;
}db_value_st;

typedef struct{
	char *name;
	u_char type;	// integer, float, text, block ... ... 
	u_char index;	// begin with 0
	short reserved;
}db_column_st;


typedef struct{
	int count;
	db_value_st value[0];
}db_row_st;


typedef struct {
	int column_count;
	list_st rowlist;
	db_column_st column[0];

}db_result_st;


typedef struct {
	char join_on;
	char in_select;
	char befor_where;
	char after_where;
	short place_flag[0];	// high 8 bit : 0 is null, 1 is befor where , 2 is after where, low 8 bit : index in [befor|after] where
}db_placeholder_st;

typedef struct {
	int type;		// create, insert, update, delete, drop, ... ...
	char *sql;
	list_st table_list;
	list_st record_list;
	db_result_st *result;
	db_placeholder_st *placeholder;
	list_st free_list;
}db_sql_st;


#define	db_record_data(db_record, idx)	&((db_record)->db_data[(idx)])
#define	db_record_field(db_record, idx)	(db_record)->db_data[(idx)].field

#define add_db_sql_table(db_sql, db_table)	list_add(&((db_sql)->table_list), (db_table), STATIC_FLAG)
#define add_db_sql_record(db_sql, db_record)		list_add(&((db_sql)->record_list), (db_record), ASSIGN_FLAG)
#define add_db_sql_freedata(db_sql, data)		list_add(&((db_sql)->free_list), (data), ASSIGN_FLAG)


int exec_db_sql(db_sql_st *db_sql);

db_sql_st *alloc_db_sql(int sql_type);
void init_db_sql(db_sql_st *db_sql, int sql_type);
void free_db_sql(db_sql_st *db_sql);
void clean_db_sql(db_sql_st *db_sql);

db_result_st *alloc_db_result(int column_count);
void free_db_result(db_result_st *db_result);
void free_db_row(db_row_st *db_row);


db_record_st *alloc_db_record(int data_count);

db_data_st *alloc_db_data(db_field_st *field, void *data, int size, int freeflag);

void set_db_data(db_data_st *db_data, db_field_st *field, void *data, int size, int flag);
void set_db_data_condition(db_data_st *db_data, int condition, int logical);

void set_db_table_field(db_table_st *table, int idx , char *name, int data_type);
void set_db_field_constraint(db_table_st *table, int idx, int constraint, void *constraint_value);

db_table_st *alloc_db_table(char *name, int field_qty);
void free_db_table(db_table_st *table);


int debug_mysql(int argc, char *argv[]);

#endif