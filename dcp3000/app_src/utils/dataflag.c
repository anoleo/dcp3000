#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

#include "dataflag.h"
#include "utils.h"

char *dataattr_string[] = {
	"null data",
	"immediate data",
	"static data",
	"alloc data",
	"assign data"
};

char *datatype_string[] = {
	"void",
	"char",
	"u_char",
	"short",
	"u_short",
	"int",
	"u_int",
	"long long",
	"unsigned long long",
	"float",
	"double",
	"string",
	"struct",
	"array"
};

int pdata_equals(int data_flag, void *pdata, void *data2)
{

	switch(DATA_TYPE(data_flag)){
			case	VOID_DATA :
			case	STRUCT_DATA :
			case	ARRAY_DATA :
				if(memcmp(pdata, data2, DATA_SIZE(data_flag))==0)
					return 1;
				break;
			case	CHAR_DATA :
				if((int)data2 == *((char *)pdata))
					return 1;
			case	UCHAR_DATA :
				if((int)data2 == *((u_char *)pdata))
					return 1;
			case	SHORT_DATA :
				if((int)data2 == *((short *)pdata))
					return 1;
			case	USHORT_DATA :
				if((int)data2 == *((u_short *)pdata))
					return 1;
			case	INT_DATA :
				if((int)data2 == *((int *)pdata))
					return 1;
			case	UINT_DATA :
			case	POINTER_DATA :
				if((u_int)data2 == *((u_int *)pdata))
					return 1;
				break;
			case	LLONG_DATA :
			case	ULLONG_DATA :
				if(memcmp(pdata, data2, sizeof(long long))==0)
					return 1;
				break;				
			case	FLOAT_DATA :
				if(floatcmp((float *)pdata, (float *) data2) == 0)
					return 1;
				break;
			case	DOUBLE_DATA :
				if(doublecmp((double *)pdata, (double *) data2) == 0)
					return 1;
				break;
			case	STRING_DATA :
				if(strcmp(data2, pdata) == 0)
					return 1;
				break;
			default :
				return 0;
				break;
		}

	return 0;
}

void print_data(void *pdata, int data_flag)
{
	int i;

	char data_type, data_attr;
	int data_size;
	
	if(pdata == NULL){
		return;
	}

	data_size = DATA_SIZE(data_flag);
	data_type = DATA_TYPE(data_flag);
	data_attr = DATA_ATTR(data_flag);

	printf("print_data : attr = %s, type %s, size = %d \n", dataattr_string[data_attr], datatype_string[data_type], data_size);

	switch(data_type){
			case	VOID_DATA :
			case	STRUCT_DATA :
			case	ARRAY_DATA :
				print_bytes(pdata, data_size);
				break;
			case	CHAR_DATA :
				if(data_attr == IMMEDIATE_DATA){
					printf("data = %c \n", (int) pdata);
				}else{
					printf("data = %c \n", *(char *) pdata);
				}
			case	UCHAR_DATA :
				if(data_attr == IMMEDIATE_DATA){
					printf("data = %#.2x \n", (int) pdata);
				}else{
					printf("data = %#.2x \n", *(u_char *) pdata);
				}
			case	SHORT_DATA :
				if(data_attr == IMMEDIATE_DATA){
					printf("data = %d \n", (int) pdata);
				}else{
					printf("data = %d \n", *(short *) pdata);
				}
			case	USHORT_DATA :
				if(data_attr == IMMEDIATE_DATA){
					printf("data = %#.4x \n", (int) pdata);
				}else{
					printf("data = %#.4x \n", *(u_short *) pdata);
				}
			case	INT_DATA :
				if(data_attr == IMMEDIATE_DATA){
					printf("data = %d \n", (int) pdata);
				}else{
					printf("data = %d \n", *(int *) pdata);
				}
				break;
			case	UINT_DATA :
			case	POINTER_DATA :
				if(data_attr == IMMEDIATE_DATA){
					printf("data = %#.8x \n", (u_int) pdata);
				}else{
					printf("data = %#.8x \n", *(u_int *) pdata);
				}
				break;
			case	LLONG_DATA :
				printf("data = %lld \n", *(long long *) pdata);
				break;
			case	ULLONG_DATA :
				printf("data = %#.16x \n", *(unsigned long long *) pdata);
			case	FLOAT_DATA :
				printf("data = %f \n", *((float *)pdata));
				break;
			case	DOUBLE_DATA :
				printf("data = %lf \n", *((double *)pdata));
				break;
			case	STRING_DATA :
				printf("data = %s \n", (char *)pdata);
				break;
			default :
				break;
		}

}


