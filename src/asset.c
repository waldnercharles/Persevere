#include <string.h>
#include <stdlib.h>
#include "dirent.h"

#include "asset.h"
#include "array.h"
#include "map.h"
#include "str.h"
#include "log.h"

void
asset_init(struct asset_manager *am, struct allocator *allocator)
{
    am->allocator = allocator;
    am->map = map_alloc(allocator, 64);

    array_alloc(allocator, 0, sizeof(struct asset_handler), &(am->handlers));
}

void
asset_load_folder(struct asset_manager *am, char *folder)
{
    struct dirent *entry;
    char filename[512] = { 0 };
    char folderpath[512] = { 0 };

    DIR *dir = opendir(folder);

    if (dir == NULL)
    {
        log_error("Could not open directory '%s'\n", folder);
    }

    strcpy(folderpath, folder);
    while ((entry = readdir(dir)) != NULL)
    {
        // Skip relative parent directories
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
        {
            continue;
        }

        memcpy(filename, folderpath, sizeof(char) * 512);

        if (filename[strlen(filename) - 1] != '/')
        {
            // Concat "/" if the filename does not already have it.
            strcat(filename, "/");
        }

        strcat(filename, entry->d_name);
        asset_get(am, filename);
    }
}

void *
asset_get(struct asset_manager *am, char *filename)
{
    char *ext;
    void *asset;
    struct asset_handler *handler;

    asset = map_get(am->map, filename);
    if (asset != NULL)
    {
        return asset;
    }

    ext = strrchr(filename, '.') + 1;
    if (am->handlers->len > 0)
    {
        array_for_each (handler, am->handlers)
        {
            if (strcmp(ext, handler->extension) == 0)
            {
                asset = handler->load_asset(filename, handler->user_data);
                map_set(am->map, filename, asset);

                log_debug("Loaded '%s'.", filename);
                return asset;
            }
        }
    }
    log_warning("No asset handler found for '%s'.", filename);
    return NULL;
}

void
asset_add_handler(struct asset_manager *am,
                  const char *ext,
                  void *(*load_func)(const char *, void *),
                  void (*unload_func)(),
                  void *user_data)
{
    struct asset_handler h;

    h.extension = ext;
    h.load_asset = load_func;
    h.unload_asset = unload_func;
    h.user_data = user_data;
    array_push(am->handlers, &h);
}