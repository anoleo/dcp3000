#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

#include <unistd.h>
#include <fcntl.h>

#include "utils.h"

#include "hashtable.h"

#define rdlock_hashtable(ptable)		pthread_rwlock_rdlock(&((ptable)->rwlock))
#define wrlock_hashtable(ptable)		pthread_rwlock_wrlock(&((ptable)->rwlock))
#define unlock_hashtable(ptable)		pthread_rwlock_unlock(&((ptable)->rwlock))


int HASHTABLE_PUT(hashtable_st *table, hashkey_t key, void *value)
{
	int ret = -1;
	BUCKET_CONTENTS *item;

	if(table==NULL){
		return -1;
	}
	
	wrlock_hashtable(table);
	
	item = hash_insert(key, table->table, 0);
	//DebugPrintf("key %#x, %p \n", key, item);
	if(item){
		//DebugPrintf("value  %p \n", value);
		item->data = value;
		ret = item->times_found;
	}

	unlock_hashtable(table);

	return ret;
}

void *HASHTABLE_GET(hashtable_st *table, hashkey_t key)
{
	void *value = NULL;
	BUCKET_CONTENTS *item;

	if(table==NULL){
		return NULL;
	}
	
	rdlock_hashtable(table);
	
	item = hash_search(key, table->table, 0);
	if(item){
		 value = item->data;
	}

	unlock_hashtable(table);

	return value;
}

int HASHTABLE_REMOVE(hashtable_st *table, hashkey_t key, void (*free_func)(void *))
{
	int ret = -1;
	BUCKET_CONTENTS *item;

	if(table==NULL){
		return -1;
	}
	
	wrlock_hashtable(table);
	
	item = hash_remove(key, table->table);
	if(item){
		if(free_func)
			free_func(item->data);
		else
			free (item->data);
		if(table->table->keytype == 0)
			free ((char *)item->key);
		free (item);
		ret = 0;
	}

	unlock_hashtable(table);

	return ret;
}

void hashtable_walk(hashtable_st *table, hash_wfunc *wfunc)
{
	
	BUCKET_CONTENTS *item;

	if(table==NULL){
		return;
	}
	
	rdlock_hashtable(table);
	
	hash_walk(table->table, wfunc);
	
	unlock_hashtable(table);

}

void hashtable_init(hashtable_st *table, int keytype, int buckets)
{
	if(table==NULL){
		return;
	}
	
	table->table = hash_create(keytype, buckets);

	pthread_rwlock_init(&(table->rwlock), NULL); 
}

hashtable_st *hashtable_create(int keytype, int buckets)
{
	hashtable_st *table;
	
	table = xMalloc(sizeof(hashtable_st));

	//printf("hashtable_create : %p size %d bytes \n", table, sizeof(hashtable_st));
	
	hashtable_init(table, keytype, buckets);

	return table;
}

void hashtable_empty(hashtable_st *table, void (*free_func)(void *))
{
	if(table==NULL){
		return;
	}
	
	wrlock_hashtable(table);
	
	hash_flush(table->table, free_func);

	unlock_hashtable(table);

}

void hashtable_destroy(hashtable_st *table, void (*free_func)(void *))
{
	if(table==NULL){
		return;
	}
	
	hashtable_empty(table, free_func);
	hash_dispose(table->table);
	pthread_rwlock_destroy(&table->rwlock);

	printf("hashtable_destroy : %p ---------------- \n", table);
	
	free(table);
}
