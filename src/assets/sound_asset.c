#include "nerd_mixer.h"

void *
asset_load_sound(const char *filename, void *udata)
{
    struct mixer *mixer = udata;
    void *src = mixer_new_source_from_file(mixer, filename);

    return src;
}