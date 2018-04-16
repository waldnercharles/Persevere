#ifndef ASSET_H
#define ASSET_H
#include "allocators/allocator.h"
#include "std.h"
#include "map.h"

struct path
{
    char str[512];
};

struct path path(const char *file_path);

struct asset_handler
{
    const char *extension;
    void *user_data;
    void *(*load_asset)(const char *, void *);
    void (*unload_asset)();
};

struct asset_manager
{
    struct allocator *allocator;
    struct map *map;
    struct array *handlers;  // struct asset_handler
};

struct asset_handle
{
    void *data;
};

void asset_init(struct asset_manager *am, struct allocator *allocator);

void asset_load_folder(struct asset_manager *am, char *folder);
void *asset_get(struct asset_manager *am, char *asset);

void asset_add_handler(struct asset_manager *am,
                       const char *ext,
                       void *(*load_func)(const char *, void *),
                       void (*unload_func)(),
                       void *user_data);

#endif