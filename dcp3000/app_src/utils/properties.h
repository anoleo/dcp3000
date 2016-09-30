
#ifndef __PROPERTIES_H__
#define __PROPERTIES_H__

#include "hashmap.h"



#define properties_size(properties) 	hashmap_size(properties)




hashmap_st *read_properties(hashmap_st *map, char *file, int keytype);

#define get_property(map, key)	(char *)hashmap_get(map, key)
#define set_property(map, key, value)	hashmap_put(map, key, value)
#define del_property(map, key)	hashmap_remove(map, key)

	
#endif

