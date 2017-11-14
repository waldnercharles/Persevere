#include "platform_audio.h"

#include "engine.h"
#include "mixer.h"
#include "log.h"

static void
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

static void
audio_lock_handler(struct mixer_event *e)
{
    u32 audio_device;
    audio_device = *((u32 *)e->udata);
    switch (e->type)
    {
        case MIXER_EVENT_LOCK:
            SDL_LockAudioDevice(audio_device);
            break;
        case MIXER_EVENT_UNLOCK:
            SDL_UnlockAudioDevice(audio_device);
            break;
    }
}

void
audio_init(struct game *game)
{
    SDL_AudioSpec want, have;
    want.format = AUDIO_S16SYS;
    want.freq = 44100;
    want.channels = 2;
    want.userdata = NULL;
    want.samples = 512;
    want.userdata = game->mixer;
    want.callback = audio_callback;

    game->mixer->audio_device =
        SDL_OpenAudioDevice(NULL,
                            0,
                            &want,
                            &have,
                            SDL_AUDIO_ALLOW_FREQUENCY_CHANGE);

    mixer_init(game->mixer, have.freq);
    mixer_set_lock(game->mixer, audio_lock_handler);
    mixer_set_master_gain(game->mixer, 1.0);

    SDL_PauseAudioDevice(game->mixer->audio_device, 0);
}