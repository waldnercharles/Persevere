#include "platform_audio.h"

#include "engine.h"
#include "log.h"

void
platform_mixer_event_handler(struct mixer_event *e)
{
    u32 audio_device;
    audio_device = *((u32 *)e->udata);
    switch (e->type)
    {
        case MIXER_EVENT_LOCK:
            platform_lock_audio_device(audio_device, true);
            break;
        case MIXER_EVENT_UNLOCK:
            platform_lock_audio_device(audio_device, false);
            break;
    }
}
