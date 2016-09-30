
#ifndef __DATA_FLAG_H__
#define __DATA_FLAG_H__

#define	DATA_SIZE_MAX		0xffffff

#define	NULL_DATA		0	// 空数据，只有
#define	IMMEDIATE_DATA	1	// 可以直接处理的整形数据 
#define	STATIC_DATA	2	// 不可以删除的数据
#define	ALLOC_DATA		3	// 内部分配的数据，创建时会进行数据拷贝
#define	ASSIGN_DATA	4	// 外部创建好的数据，只是将其指针赋给它

#define	NULL_FLAG		(NULL_DATA << 28)
#define	IMMEDIATE_FLAG	(IMMEDIATE_DATA << 28)
#define	STATIC_FLAG	(STATIC_DATA << 28)
#define	ALLOC_FLAG		(ALLOC_DATA << 28)
#define	ASSIGN_FLAG	(ASSIGN_DATA << 28)


#define	VOID_DATA		0
#define	CHAR_DATA		1
#define	UCHAR_DATA		2
#define	SHORT_DATA	3
#define	USHORT_DATA		4
#define	INT_DATA		5
#define	UINT_DATA			6
#define	LONG_DATA		5
#define	ULONG_DATA		6
#define	LLONG_DATA		7
#define	ULLONG_DATA		8
#define	FLOAT_DATA		9
#define	DOUBLE_DATA		10
#define	STRING_DATA		11
#define	STRUCT_DATA		12
#define	ARRAY_DATA		13
#define	POINTER_DATA		14

#define	VOID_FLAG		(VOID_DATA<<24)
#define	CHAR_FLAG		(CHAR_DATA<<24)
#define	UCHAR_FLAG		(UCHAR_DATA<<24)
#define	SHORT_FLAG		(SHORT_DATA<<24)
#define	USHORT_FLAG		(USHORT_DATA<<24)
#define	INT_FLAG		(INT_DATA<<24)
#define	UINT_FLAG		(UINT_DATA<<24)
#define	LONG_FLAG		(LONG_DATA<<24)
#define	ULONG_FLAG		(ULONG_DATA<<24)
#define	LLONG_FLAG		(LLONG_DATA<<24)
#define	ULLONG_FLAG		(ULLONG_DATA<<24)
#define	FLOAT_FLAG		(FLOAT_DATA<<24)
#define	DOUBLE_FLAG		(DOUBLE_DATA<<24)
#define	STRING_FLAG		(STRING_DATA<<24)
#define	STRUCT_FLAG		(STRUCT_DATA<<24)
#define	ARRAY_FLAG		(ARRAY_DATA<<24)
#define	POINTER_FLAG		(POINTER_DATA<<24)


#define	DATA_ATTR(data_flag)	(((data_flag)>>28)&0x0f)
#define	DATA_TYPE(data_flag)	(((data_flag)>>24)&0x0f)
#define	DATA_SIZE(data_flag)	((data_flag)&0xffffff)

#define	DATA_ATTR_FLAG(data_attr)		(((data_attr)&0x0f)<<28)
#define	DATA_TYPE_FLAG(data_type)		(((data_type)&0x0f)<<24)
#define	DATA_SIZE_FLAG(data_size)		((data_size)&0xffffff)

#define	DATA_FLAG(data_attr, data_type, data_size)	(DATA_ATTR_FLAG(data_attr) | DATA_TYPE_FLAG(data_type) | DATA_SIZE_FLAG(data_size))


#define	DATA_ATTR_MASK		((0x0f)<<28)
#define	DATA_TYPE_MASK		((0x0f)<<24)
#define	DATA_SIZE_MASK		(0xffffff)

extern char *dataattr_string[];
extern char *datatype_string[];

typedef int (*dowith_func)(void *, void *);
typedef dowith_func compare_func;

int DATA_EQUALS(int data_type, int lattr, int rattr, void * data1, void * data2);

int pdata_equals(int data_flag, void *pdata1, void *data2);
void print_data(void *pdata, int data_flag);

#endif

