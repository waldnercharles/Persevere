#include "std.h"
#include "log.h"
#include "platform_audio.h"

#include <SDL2/SDL.h>

void
platform_lock_audio_device(u32 audio_device, b32 lock)
{
    if (lock)
    {
        SDL_LockAudioDevice(audio_device);
    }
    else
    {
        SDL_UnlockAudioDevice(audio_device);
    }
}

void
platform_pause_audio_device(u32 audio_device, b32 pause)
{
    SDL_PauseAudioDevice(audio_device, pause);
}

void
platform_open_audio_device(u32 audio_device,
                           struct audio_spec *want,
                           struct audio_spec *have,
                           void (*callback)(void *udata, u8 *stream, s32 len),
                           void *udata)
{
    SDL_AudioDeviceID dev;
    SDL_AudioSpec sdl_want, sdl_have;
    const char *device_name = NULL;

    if (audio_device != 0)
    {
        device_name = SDL_GetAudioDeviceName(audio_device, 0);
    }

    memset(&sdl_want, 0, sizeof(SDL_AudioSpec));

    sdl_want.format = AUDIO_S16SYS;
    sdl_want.freq = want->frequency;
    sdl_want.channels = want->channels;
    sdl_want.samples = want->samples;
    sdl_want.userdata = udata;
    sdl_want.callback = callback;

    dev = SDL_OpenAudioDevice(device_name,
                              0,
                              &sdl_want,
                              &sdl_have,
                              SDL_AUDIO_ALLOW_FREQUENCY_CHANGE);

    if (dev == 0)
    {
        log_error("Failed to open audio device. %s", SDL_GetError());
    }

    have->audio_device = dev;
    have->frequency = sdl_have.freq;
    have->channels = sdl_have.channels;
    have->samples = sdl_have.samples;
}

void
platform_close_audio_device(u32 audio_device)
{
    SDL_CloseAudioDevice(audio_device);
}