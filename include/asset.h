#ifndef ASSET_H
#define ASSET_H
#include "std.h"

struct map;
struct path
{
    char str[512];
};

struct path path(const char *file_path);

struct asset_handler
{
    char *extension;
    void *user_data;
    void *(*load_asset)(const char *, void *);
    void (*unload_asset)();
};

struct asset_manager
{
    struct map *map;
    struct asset_handler *handlers;
};

struct asset_handle
{
    void *data;
};

struct asset_manager *asset_new();
void asset_init(struct asset_manager *am);

void asset_load_folder(struct asset_manager *am, char *folder);
void *asset_get(struct asset_manager *am, char *asset);

void asset_add_handler(struct asset_manager *am,
                       const char *ext,
                       void *(*load_func)(const char *, void *),
                       void (*unload_func)(),
                       void *user_data);

#endif