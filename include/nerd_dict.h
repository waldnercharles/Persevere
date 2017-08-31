#pragma once

#include "nerd.h"
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

struct dict *dict_new(uint capacity);
void dict_delete(struct dict *dict);
void *dict_get(struct dict *dict, char *key);
void dict_set(struct dict *dict, char *key, void *value);
bool dict_exists(struct dict *dict, char *key);