#ifndef PLATFORM_AUDIO_H
#define PLATFORM_AUDIO_H
#include "std.h"
#include "engine.h"
#include "mixer.h"

void platform_lock_audio_device(u32 audio_device, b32 lock);
void platform_mixer_event_handler(struct mixer_event *e);

#endif