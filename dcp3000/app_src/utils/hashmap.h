
#ifndef __HASHMAP_H__
#define __HASHMAP_H__

#include "hashtable.h"


typedef hashtable_st hashmap_st;

#define hashmap_size(map) 	hashtable_size(map)



#define hashmap_put(map, key, value)	hashtable_put(map, key, value)
#define hashmap_get(map, key)		hashtable_get(map, key)
#define hashmap_remove(map, key, free_func)	hashtable_remove(map, key, free_func)

#define hashmap_walk(map, wfunc)	hashtable_walk(map, wfunc)


#define hashmap_init(map, keytype, buckets)	hashtable_init(map, keytype, buckets)
#define hashmap_create(keytype, buckets)		hashtable_create(keytype, buckets)

#define hashmap_empty(map, free_func)		hashtable_empty(map, (void (*)(void *))(free_func))
#define hashmap_destroy(map, free_func)	hashtable_destroy(map, (void (*)(void *))(free_func))

	
#endif

