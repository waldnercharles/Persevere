#pragma once

#include <string.h>

#include "nerd_memory.h"
#include "nerd_typedefs.h"

struct dict_key_value
{
    char *key;
    void *value;
};


struct dict_bucket
{
    uint count;
    struct dict_key_value *pairs;
};


struct dict
{
    uint count;
    struct dict_bucket *buckets;
};


static uint dict__hash(char *str)
{
    uint hash = 0;
    while (*str)
    {
	hash = (hash << 7) + (hash >> 25) + *str++;
    }

    return hash + (hash >> 16);
}


static struct dict_key_value *dict__get_key_value(struct dict_bucket *bucket, char *key)
{
    struct dict_key_value *kv;
    
    if (bucket == NULL) return NULL;
    if (bucket->count == 0) return NULL;

    kv = bucket->pairs;
    for (uint i = 0; i < bucket->count; i++, kv++)
    {
	if (kv->key == NULL || kv->value == NULL) continue;
	if (strcmp(kv->key, key) == 0) return kv;
    }
    return NULL;
}


struct dict *dict_new(uint capacity)
{
    struct dict* dict = malloc(sizeof(struct dict));
    if (dict == NULL) return NULL;
    dict->count = capacity;
    dict->buckets = malloc(capacity * sizeof(struct dict_bucket));
    if (dict->buckets == NULL)
    {
	free(dict);
	return NULL;
    }

    memset(dict->buckets, 0, capacity * sizeof(struct dict_bucket));
    return dict;
}


void dict_delete(struct dict* dict)
{
    struct dict_bucket *bucket;
    struct dict_key_value *kv;

    if (dict == NULL) return;

    bucket = dict->buckets;
    for (uint i = 0; i < dict->count; i++, bucket++)
    {
	kv = bucket->pairs;
	for (uint j = 0; j < bucket->count; j++, kv++)
	{
	    free(kv->key);
	    free(kv->value);
	}
	free(bucket->pairs);
    }
    free(dict->buckets);
    free(dict);
}


void *dict_get(struct dict *dict, char *key)
{
    uint idx;
    struct dict_bucket *bucket;
    struct dict_key_value *kv;
    
    if (dict == NULL) return NULL;
    if (key == NULL) return NULL;

    idx = dict__hash(key) % dict->count;
    bucket = &(dict->buckets[idx]);
    kv = dict__get_key_value(bucket, key);

    if (kv == NULL) return NULL;
    return kv->value;
}


void dict_set(struct dict *dict, char *key, void *value)
{
    uint idx;
    struct dict_bucket *bucket;
    struct dict_key_value *kv;

    if (dict == NULL) return;
    if (key == NULL) return;
    if (value == NULL) return;

    idx = dict__hash(key) % dict->count;
    bucket = &(dict->buckets[idx]);
    kv = dict__get_key_value(bucket, key);

    if (kv != NULL)
    {
	kv->value = value;
	return;
    }

    if (bucket->count == 0)
	kv = malloc(sizeof(struct dict_key_value));
    else
	kv = realloc(bucket->pairs, (++bucket->count) * sizeof(struct dict_key_value));

    if (kv == NULL) return;

    kv = &(bucket->pairs[bucket->count - 1]);
    kv->key = malloc(strlen(key) + 1 * sizeof(char));
    kv->value = value;

    strcpy(kv->key, key);
}


bool dict_exists(struct dict *dict, char *key)
{
    uint idx;
    struct dict_bucket *bucket;
    struct dict_key_value *kv;

    if (dict == NULL) return false;
    if (key == NULL) return false;

    idx = dict__hash(key) % dict->count;
    bucket = &(dict->buckets[idx]);
    kv = dict__get_key_value(bucket, key);

    if (kv == NULL) return false;
    return true;
}
