#include "engine.h"
#include "log.h"

#include "allocators/allocator.h"

#include "assets/sound_asset.h"
#include "assets/shader_asset.h"
#include "assets/texture_asset.h"

void
engine__mixer_event_handler(struct mixer_event *e)
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
engine__audio_callback(void *userdata, u8 *stream, s32 len)
{
    struct mixer *mixer;
    if (userdata == NULL)
    {
        return;
    }

    mixer = userdata;
    mixer_process(mixer, (s16 *)stream, len / 2);
}

void
engine_ecs_init(struct engine *engine)
{
    struct platform *platform = engine->platform;

    log_debug("Allocating Entity Component System...");

    engine->ecs = alloc(platform->memory->permanent, sizeof(struct ecs));
    engine->ecs->allocator = platform->memory->permanent;
}

void
engine_open_audio_device(struct engine *engine)
{
    struct platform *platform = engine->platform;
    struct audio_spec want, have;

    want.frequency = 44100;
    want.channels = 2;
    want.samples = 512;

    platform->api->open_audio_device(0,
                                     &want,
                                     &have,
                                     engine__audio_callback,
                                     engine->mixer);

    mixer_set_master_audio_device(engine->mixer, have.audio_device);
    mixer_set_master_samplerate(engine->mixer, have.frequency);

    platform->api->pause_audio_device(have.audio_device, true);
    platform->api->pause_audio_device(have.audio_device, false);
}

void
engine_close_audio_device(struct engine *engine)
{
    u32 audio_device;
    audio_device = engine->mixer->audio_device;

    if (audio_device != 0)
    {
        engine->platform->api->lock_audio_device(audio_device, true);
        engine->platform->api->pause_audio_device(audio_device, true);
        engine->platform->api->close_audio_device(audio_device);
    }
}

void
engine_audio_init(struct engine *engine)
{
    struct platform *platform = engine->platform;

    log_debug("Allocating & Initializing Audio...");

    engine->mixer = alloc(platform->memory->permanent, sizeof(struct mixer));

    mixer_init(engine->mixer, platform->api->lock_audio_device);

    mixer_set_event_handler(engine->mixer, engine__mixer_event_handler);
    mixer_set_master_gain(engine->mixer, 1.0);

    engine_open_audio_device(engine);
}

void
engine_unbind(struct engine *engine)
{
    engine_close_audio_device(engine);
    mixer_set_event_handler(engine->mixer, NULL);
}

void
engine_bind(struct engine *engine)
{
    engine_open_audio_device(engine);
    mixer_set_event_handler(engine->mixer, engine__mixer_event_handler);
}

void
engine_renderer_init(struct engine *engine)
{
    struct platform *platform = engine->platform;

    log_debug("Allocating & Initializing Renderer...");

    engine->renderer =
        alloc(platform->memory->permanent, sizeof(struct renderer));

    renderer_init(engine->renderer, platform->memory->permanent);
}

void
engine_asset_init(struct engine *engine)
{
    struct platform *platform = engine->platform;

    log_debug("Allocating & Initializing Asset Manager...");

    engine->assets =
        alloc(platform->memory->permanent, sizeof(struct asset_manager));

    asset_init(engine->assets, platform->memory->permanent);

    asset_add_handler(engine->assets, "ogg", asset_load_sound, NULL, engine);
    asset_add_handler(engine->assets, "jpg", asset_load_texture, NULL, engine);
    asset_add_handler(engine->assets, "png", asset_load_texture, NULL, engine);

    asset_add_handler(engine->assets,
                      "frag",
                      asset_load_fragment_shader,
                      NULL,
                      engine);

    asset_add_handler(engine->assets,
                      "vert",
                      asset_load_vertex_shader,
                      NULL,
                      engine);
}

void
engine_init(struct platform *platform, struct engine **engine)
{
    log_debug("Allocating & Initializing Engine...");
    *engine = alloc(platform->memory->permanent, sizeof(struct engine));
    (*engine)->platform = platform;

    engine_ecs_init(*engine);
    engine_audio_init(*engine);
    engine_renderer_init(*engine);
    engine_asset_init(*engine);
}