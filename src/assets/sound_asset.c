#include "engine.h"
#include "mixer.h"

void *
asset_load_sound(const char *filename, void *udata)
{
    struct engine *engine = udata;
    void *src = mixer_new_source_from_file(engine->mixer,
                                           engine->platform->memory->permanent,
                                           filename);

    return src;
}