/* hashlib.c -- functions to manage and access hash tables for bash. */

/* Copyright (C) 1987,1989,1991,1995,1998,2001,2003,2005,2006,2008,2009 Free Software Foundation, Inc.

   This file is part of GNU Bash, the Bourne Again SHell.

   Bash is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   Bash is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with Bash.  If not, see <http://www.gnu.org/licenses/>.
*/


#include <sys/types.h>

#include <unistd.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "hashlib.h"


#if !defined (STREQ)
#define STREQ(a, b, keytype)	((keytype) ? ((a) == (b)) \
				    : (((char *)(a))[0] ==((char *) (b))[0]) && (strcmp ((char *)(a), (char *)(b)) == 0))
#define STREQN(a, b, n)	(((n) == 0) ? (1) \
				    : ((a)[0] == (b)[0]) && (strncmp ((a), (b), (n)) == 0))
#endif

static void
memory_error_and_abort (fname)
     char *fname;
{
  fprintf (stderr, "%s: out of virtual memory\n", fname);
  exit (2);
}

/* Return a pointer to free()able block of memory large enough
   to hold BYTES number of bytes.  If the memory cannot be allocated,
   print an error message and abort. */
static PTR_T
xmalloc (bytes)
     size_t bytes;
{
  PTR_T temp;

  temp = malloc (bytes);
  if (temp == 0)
    memory_error_and_abort ("xmalloc");
  return (temp);
}

/* Backwards compatibility, now that savestring has been removed from
   all `public' readline header files. */
static char *
savestring (s)
     const char *s;
{
  char *ret;

  ret = (char *)xmalloc (strlen (s) + 1);
  strcpy (ret, s);
  return ret;
}


/* Rely on properties of unsigned division (unsigned/int -> unsigned) and
   don't discard the upper 32 bits of the value, if present. */
#define HASH_BUCKET(k, t, h) (((h) = (t)->keytype?hash_data(&k, sizeof(k)):hash_string ((char *)(k))) & ((t)->nbuckets - 1))

static BUCKET_CONTENTS *copy_bucket_array(BUCKET_CONTENTS *, sh_string_func_t *, int keytype);

/* Make a new hash table with BUCKETS number of buckets.  Initialize
   each slot in the table to NULL. */
HASH_TABLE *
hash_create (int keytype, int buckets)
{
  HASH_TABLE *new_table;
  register int i;

  new_table = (HASH_TABLE *)xmalloc (sizeof (HASH_TABLE));
  if (buckets == 0)
    buckets = DEFAULT_HASH_BUCKETS;

  new_table->bucket_array =
    (BUCKET_CONTENTS **)xmalloc (buckets * sizeof (BUCKET_CONTENTS *));
  new_table->nbuckets = buckets;
  new_table->nentries = 0;
  new_table->keytype = keytype;

  for (i = 0; i < buckets; i++)
    new_table->bucket_array[i] = (BUCKET_CONTENTS *)NULL;

  return (new_table);
}

int
hash_size (table)
     HASH_TABLE *table;
{
  return (HASH_ENTRIES(table));
}

static BUCKET_CONTENTS *
copy_bucket_array (ba, cpdata, keytype)
     BUCKET_CONTENTS *ba;
     sh_string_func_t *cpdata;	/* data copy function */
     int keytype;
{
  BUCKET_CONTENTS *new_bucket, *n, *e;

  if (ba == 0)
    return ((BUCKET_CONTENTS *)0);

  for (n = (BUCKET_CONTENTS *)0, e = ba; e; e = e->next)
    {
      if (n == 0)
        {
          new_bucket = (BUCKET_CONTENTS *)xmalloc (sizeof (BUCKET_CONTENTS));
          n = new_bucket;
        }
      else
        {
          n->next = (BUCKET_CONTENTS *)xmalloc (sizeof (BUCKET_CONTENTS));
          n = n->next;
        }
	if(keytype){
     		 n->key = e->key;
	}else{
     		 n->key = (hashkey_t)savestring ((char *)(e->key));
	}
      n->data = e->data ? (cpdata ? (*cpdata) (e->data) : savestring (e->data))
			: NULL;
      n->khash = e->khash;
      n->times_found = e->times_found;
      n->next = (BUCKET_CONTENTS *)NULL;
    }

  return new_bucket;  
}

HASH_TABLE *
hash_copy (table, cpdata)
     HASH_TABLE *table;
     sh_string_func_t *cpdata;
{
  HASH_TABLE *new_table;
  int i;

  if (table == 0)
    return ((HASH_TABLE *)NULL);

  new_table = hash_create (table->keytype, table->nbuckets);

  for (i = 0; i < table->nbuckets; i++)
    new_table->bucket_array[i] = copy_bucket_array (table->bucket_array[i], cpdata, table->keytype);

  new_table->nentries = table->nentries;
  return new_table;
}

/* The `khash' check below requires that strings that compare equally with
   strcmp hash to the same value. */
unsigned int
hash_string (s)
     const char *s;
{
  register unsigned int i;

  /* This is the best string hash function I found.

     The magic is in the interesting relationship between the special prime
     16777619 (2^24 + 403) and 2^32 and 2^8. */
 
  for (i = 0; *s; s++)
    {
      i *= 16777619;
      i ^= *s;
    }

  return i;
}

/* The `khash' check below requires that strings that compare equally with
   strcmp hash to the same value. */
unsigned int
hash_data (void *data, int datalen)
{
	char *s = data;
	register unsigned int i, khash = 0;
  /* This is the best string hash function I found.

     The magic is in the interesting relationship between the special prime
     16777619 (2^24 + 403) and 2^32 and 2^8. */

	for (i = 0; i<datalen; i++){
		khash*= 16777619;
		khash ^= s[i];
	}

	return khash;
}

/* Return the location of the bucket which should contain the data
   for STRING.  TABLE is a pointer to a HASH_TABLE. */

int
hash_bucket (key, table)
     hashkey_t key;
     HASH_TABLE *table;
{
  unsigned int h;

  return (HASH_BUCKET (key, table, h));
}

/* Return a pointer to the hashed item.  If the HASH_CREATE flag is passed,
   create a new hash table entry for STRING, otherwise return NULL. */
BUCKET_CONTENTS *
HASH_SEARCH (hashkey_t key, HASH_TABLE *table, int flags)
{
  BUCKET_CONTENTS *list;
  int bucket;
  unsigned int hv;

  if (table == 0 || ((flags & HASH_CREATE) == 0 && HASH_ENTRIES (table) == 0))
    return (BUCKET_CONTENTS *)NULL;

  bucket = HASH_BUCKET (key, table, hv);

  for (list = table->bucket_array ? table->bucket_array[bucket] : 0; list; list = list->next)
    {
      if (hv == list->khash && STREQ(list->key, key, table->keytype))
	{
	  list->times_found++;
	  return (list);
	}
    }

  if (flags & HASH_CREATE)
    {
      list = (BUCKET_CONTENTS *)xmalloc (sizeof (BUCKET_CONTENTS));
      list->next = table->bucket_array[bucket];
      table->bucket_array[bucket] = list;

      list->data = NULL;
      list->key = key;	/* XXX fix later */
      list->khash = hv;
      list->times_found = 0;

      table->nentries++;
      return (list);
    }
      
  return (BUCKET_CONTENTS *)NULL;
}

/* Remove the item specified by STRING from the hash table TABLE.
   The item removed is returned, so you can free its contents.  If
   the item isn't in this table NULL is returned. */
BUCKET_CONTENTS *
HASH_REMOVE (hashkey_t key, HASH_TABLE *table)
{
  int bucket;
  BUCKET_CONTENTS *prev, *temp;
  unsigned int hv;

  if (table == 0 || HASH_ENTRIES (table) == 0)
    return (BUCKET_CONTENTS *)NULL;

  bucket = HASH_BUCKET (key, table, hv);
  prev = (BUCKET_CONTENTS *)NULL;
  for (temp = table->bucket_array[bucket]; temp; temp = temp->next)
    {
      if (hv == temp->khash && STREQ (temp->key, key, table->keytype))
	{
	  if (prev)
	    prev->next = temp->next;
	  else
	    table->bucket_array[bucket] = temp->next;

	  table->nentries--;
	  return (temp);
	}
      prev = temp;
    }
  return ((BUCKET_CONTENTS *) NULL);
}

/* Create an entry for STRING, in TABLE.  If the entry already
   exists, then return it (unless the HASH_NOSRCH flag is set). */
BUCKET_CONTENTS *
HASH_INSERT (hashkey_t key, HASH_TABLE *table, int flags)
{
  BUCKET_CONTENTS *item;
  int bucket;
  unsigned int hv;

  if (table == NULL)
    return NULL;

  item = (flags & HASH_NOSRCH) ? (BUCKET_CONTENTS *)NULL
  			       : hash_search (key, table, 0);

  if (item == 0)
    {
      bucket = HASH_BUCKET (key, table, hv);

      item = (BUCKET_CONTENTS *)xmalloc (sizeof (BUCKET_CONTENTS));
      item->next = table->bucket_array[bucket];
      table->bucket_array[bucket] = item;

      item->data = NULL;
      item->key = key;
      item->khash = hv;
      item->times_found = 0;

      table->nentries++;
    }

  return (item);
}

/* Remove and discard all entries in TABLE.  If FREE_DATA is non-null, it
   is a function to call to dispose of a hash item's data.  Otherwise,
   free() is called. */
void
hash_flush (table, data_free_func)
     HASH_TABLE *table;
     sh_free_func_t *data_free_func;
{
  int i;
  register BUCKET_CONTENTS *bucket, *item;

  if (table == 0 || HASH_ENTRIES (table) == 0)
    return;

  for (i = 0; i < table->nbuckets; i++)
    {
      bucket = table->bucket_array[i];

      while (bucket)
	{
	  item = bucket;
	  bucket = bucket->next;

	  if (data_free_func)
	    (*data_free_func) (item->data);
	  else
	    free (item->data);
	  
	  if(table->keytype == 0)
	  free ((char *)item->key);
	  free (item);
	}
      table->bucket_array[i] = (BUCKET_CONTENTS *)NULL;
    }

  table->nentries = 0;
}

/* Free the hash table pointed to by TABLE. */
void
hash_dispose (HASH_TABLE *table)
{
	if (table){
		free (table->bucket_array);
		free (table);
	}
}

void
hash_walk (table, func)
     HASH_TABLE *table;
     hash_wfunc *func;
{
  register int i;
  BUCKET_CONTENTS *item;

  if (table == 0 || HASH_ENTRIES (table) == 0)
    return;

  for (i = 0; i < table->nbuckets; i++)
    {
      for (item = hash_items (i, table); item; item = item->next)
	if ((*func) (item) < 0)
	  return;
    }
}

void
hash_pstats (table, name)
     HASH_TABLE *table;
     char *name;
{
  register int slot, bcount;
  register BUCKET_CONTENTS *bc;

 if (table == NULL)
    return;

  if (name == 0)
    name = "unknown hash table";

  fprintf (stderr, "%s: %d buckets; %d items\n", name, table->nbuckets, table->nentries);

  /* Print out a count of how many strings hashed to each bucket, so we can
     see how even the distribution is. */
  for (slot = 0; slot < table->nbuckets; slot++)
    {
      bc = hash_items (slot, table);

      fprintf (stderr, "\tslot %3d: ", slot);
      for (bcount = 0; bc; bc = bc->next)
	bcount++;

      fprintf (stderr, "%d\n", bcount);
    }
}


#ifdef TEST_HASHING

/* link with xmalloc.o and lib/malloc/libmalloc.a */
#undef NULL
#include <stdio.h>

#ifndef NULL
#define NULL 0
#endif

HASH_TABLE *table, *ntable;

int interrupt_immediately = 0;

int
signal_is_trapped (s)
     int s;
{
  return (0);
}

void
programming_error (const char *format, ...)
{
  abort();
}

void
fatal_error (const char *format, ...)
{
  abort();
}

main ()
{
  char string[256];
  int count = 0;
  BUCKET_CONTENTS *tt;

  table = hash_create (0, 0);

  for (;;)
    {
      char *temp_string;
      if (fgets (string, sizeof (string), stdin) == 0)
	break;
      if (!*string)
	break;
      temp_string = savestring (string);
      tt = hash_insert (temp_string, table, 0);
      if (tt->times_found)
	{
	  fprintf (stderr, "You have already added item `%s'\n", string);
	  free (temp_string);
	}
      else
	{
	  count++;
	}
    }

  hash_pstats (table, "hash test");

  ntable = hash_copy (table, (sh_string_func_t *)NULL);
  hash_flush (table, (sh_free_func_t *)NULL);
  hash_pstats (ntable, "hash copy test");

  exit (0);
}

#endif /* TEST_HASHING */
