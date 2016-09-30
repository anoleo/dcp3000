
#ifndef __HASHTABLE_H__
#define __HASHTABLE_H__

#include <sys/types.h>
#include <pthread.h>
#include "hashlib.h"


typedef  struct{
	HASH_TABLE *table;
	pthread_rwlock_t rwlock;
}hashtable_st;

typedef BUCKET_CONTENTS hashItem_st;

#define hashtable_size(table) 	hash_size(table)


int HASHTABLE_PUT(hashtable_st *table, hashkey_t key, void *value);
void *HASHTABLE_GET(hashtable_st *table, hashkey_t key);
int HASHTABLE_REMOVE(hashtable_st *table, hashkey_t key, void (*free_func)(void *));

#define hashtable_put(table, key, value)	HASHTABLE_PUT(table, (hashkey_t)(key), value)
#define hashtable_get(table, key)		HASHTABLE_GET(table, (hashkey_t)(key))
#define hashtable_remove(table, key, free_func)	HASHTABLE_REMOVE(table, (hashkey_t)(key), (void (*)(void *))(free_func))

void hashtable_walk(hashtable_st *table, hash_wfunc *wfunc);

void hashtable_init(hashtable_st *table, int keytype, int buckets);
hashtable_st *hashtable_create(int keytype, int buckets);
void hashtable_empty(hashtable_st *table, void (*free_func)(void *));
void hashtable_destroy(hashtable_st *table, void (*free_func)(void *));
	
#endif

