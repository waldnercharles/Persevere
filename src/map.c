#include "map.h"
#include "log.h"

static u32
map_hash(char *str)
{
    u32 hash = 5381;
    while (*str)
    {
        hash = ((hash << 5) + hash) ^ *str++;
    }
    return hash;
}

static struct map_node *
map_node_alloc(struct map *map, char *key, void *value)
{
    u32 key_size;
    struct map_node *node;

    key_size = strlen(key) + 1;
    node = alloc(map->allocator, sizeof(struct map_node) + key_size);

    node->hash = map_hash(key);
    node->key = (char *)(node + 1);
    memcpy(node->key, key, key_size);
    node->value = value;

    return node;
}

static u32
map_bucket_idx(struct map *m, u32 hash)
{
    return hash % m->capacity;
}

static void
map_node_add(struct map *m, struct map_node *node)
{
    u32 idx = map_bucket_idx(m, node->hash);
    node->next = m->buckets[idx];
    m->buckets[idx] = node;
}

static void
map_resize(struct map *m, u32 new_capacity)
{
    u32 i;
    struct map_node *nodes, *node, *tmp;

    // Chain nodes
    nodes = NULL;
    for (i = m->capacity; i > 0; --i)
    {
        node = m->buckets[i];
        while (node != NULL)
        {
            tmp = node->next;
            node->next = nodes;
            nodes = node;
            node = tmp;
        }
    }

    // Reset buckets
    m->buckets = realloc(m->buckets, sizeof(*m->buckets) * new_capacity);
    m->capacity = new_capacity;

    memset(m->buckets, 0, sizeof(*m->buckets) * m->capacity);

    // Add nodes to buckets
    node = nodes;
    while (node != NULL)
    {
        tmp = node->next;
        map_node_add(m, node);
        node = tmp;
    }
}

static struct map_node **
map_get_node_ptr(struct map *m, char *key)
{
    u32 hash, idx;
    struct map_node **node;

    hash = map_hash(key);
    idx = map_bucket_idx(m, hash);

    for (node = &m->buckets[idx]; *node != NULL; node = &(*node)->next)
    {
        if ((*node)->hash == hash && strcmp((*node)->key, key) == 0)
        {
            return node;
        }
    }
    return NULL;
}

struct map *
map_alloc(struct allocator *allocator, u32 capacity)
{
    struct map *m;

    m = alloc(allocator, sizeof(struct map));
    m->allocator = allocator;
    m->buckets = alloc(allocator, capacity * sizeof(struct map_node));
    m->capacity = capacity;
    m->count = 0;

    return m;
}

void
map_free(struct map *m)
{
    unused(m);
    // TODO: Free all items in map?
    // free(m);
    // free(m->buckets);
}

b32
map_contains(struct map *m, char *key)
{
    return map_get_node_ptr(m, key) != NULL;
}

void *
map_get(struct map *m, char *key)
{
    struct map_node **node;

    node = map_get_node_ptr(m, key);
    return node != NULL ? (*node)->value : NULL;
}

void
map_set(struct map *m, char *key, void *value)
{
    u32 new_capacity;
    struct map_node **node, *new_node;
    unused(new_capacity);

    node = map_get_node_ptr(m, key);

    if (node != NULL)
    {
        (*node)->value = value;
        return;
    }

    new_node = map_node_alloc(m, key, value);
    if (m->count >= m->capacity)
    {
        new_capacity = m->capacity > 0 ? m->capacity << 1 : 1;
        log_error("Resize not yet implemented.");
        return;
        map_resize(m, new_capacity);
    }
    map_node_add(m, new_node);
    m->count++;
}

void
map_remove(struct map *m, char *key)
{
    struct map_node **node;  //, *tmp;
    node = map_get_node_ptr(m, key);

    if (node)
    {
        // tmp = *node;
        *node = (*node)->next;
        // free(tmp);
        m->count--;
    }
}
