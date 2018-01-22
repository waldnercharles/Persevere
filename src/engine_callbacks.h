#ifndef ENGINE_CALLBACKS_H
#define ENGINE_CALLBACKS_H
#include "mixer.h"

void
mixer_event_handler(struct mixer_event *e)
{
    switch (e->type)
    {
        case MIXER_EVENT_LOCK:
            break;
        case MIXER_EVENT_UNLOCK:
            break;
    }
}

void
audio_callback(void *userdata, u8 *stream, s32 len)
{
    struct mixer *mixer;
    if (userdata == NULL)
    {
        return;
    }

    mixer = userdata;
    mixer_process(mixer, (s16 *)stream, len / 2);
}

#endif