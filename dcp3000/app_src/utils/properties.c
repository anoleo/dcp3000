#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

#include <unistd.h>
#include <fcntl.h>

#include "utils.h"

#include "properties.h"


hashmap_st *read_properties(hashmap_st *map, char *file, int keytype)
{
	FILE *fp;
	char buf[1024];
	char *pch, *value;
	u_int key;
	int ret;

	printf("read properties file %s \n", file);

	fp = fopen(file, "r");
	if(fp==NULL){
		perror("read_properties");
		return NULL;
	}

	if(map == NULL){
		map = hashmap_create(keytype, 0);
	}else{
		hashmap_init(map, keytype, 0);
	}

	while(fgets(buf, 1024, fp)){
		
		if((buf[0] == '#') || (buf[0] == ';')){
			continue;
		}
		
		if(strlen(buf) < 5){
			continue;
		}
		
		
		if(keytype){
			key = strtoul(buf, &pch, 16);
			pch = strchr(pch, '=');
			++pch;
		}else{
			pch = strtok(buf, "=");
			key = (u_int)cpstring(strtrim(pch));
			pch = strtok(NULL, "=");
		}
		
		if(pch){
			value = cpstring(strtrim(pch));
			ret = set_property(map, key, value);
			if((keytype == 0) && (ret!=0)){
				free((char *)key);
			}
		}
	}

	fclose(fp);

	return map;
}
