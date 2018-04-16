#include "engine.h"
#include "log.h"

#include "array.h"
#include "shader.h"
#include "systems.h"
#include "components.h"

#include "fbo.h"

#include "allocators/allocator.h"

#include "assets/sound_asset.h"
#include "assets/shader_asset.h"
#include "assets/texture_asset.h"

static void
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

static void
engine__audio_callback(void *userdata, u8 *stream, s32 len)
{
    struct mixer *mixer = userdata;
    if (mixer == NULL)
    {
        return;
    }

    mixer_process(mixer, (s16 *)stream, len / 2);
}

void
engine_open_audio_device(struct platform_api *api, struct mixer *mixer)
{
    struct audio_spec want, have;

    want.frequency = 44100;
    want.channels = 2;
    want.samples = 512;

    api->open_audio_device(0, &want, &have, engine__audio_callback, mixer);

    mixer_set_master_audio_device(mixer, have.audio_device);
    mixer_set_master_samplerate(mixer, have.frequency);

    api->pause_audio_device(have.audio_device, true);
    api->pause_audio_device(have.audio_device, false);
}

void
engine_close_audio_device(struct platform_api *api, struct mixer *mixer)
{
    u32 audio_device = mixer->audio_device;

    if (audio_device != 0)
    {
        api->lock_audio_device(audio_device, true);
        api->pause_audio_device(audio_device, true);
        api->close_audio_device(audio_device);
    }
}

static void
engine_audio_init(struct engine *engine)
{
    struct mixer *mixer;

    log_debug("Allocating & Initializing Audio...");

    mixer = alloc(engine->platform->memory->permanent, sizeof(struct mixer));

    mixer_init(mixer, engine->platform->api->lock_audio_device);

    mixer_set_event_handler(mixer, engine__mixer_event_handler);
    mixer_set_master_gain(mixer, 1.0);

    engine_open_audio_device(engine->platform->api, mixer);

    engine->mixer = mixer;
}

static void
engine_ecs_init(struct engine *engine)
{
    struct ecs *ecs;

    log_debug("Allocating Entity Component System...");

    ecs = alloc(engine->platform->memory->permanent, sizeof(struct ecs));
    ecs_init(ecs, engine->platform->memory->permanent);

    ecs->system_handles =
        alloc(engine->platform->memory->permanent, sizeof(struct systems));

    ecs->component_handles =
        alloc(engine->platform->memory->permanent, sizeof(struct components));

    engine->ecs = ecs;
}

static void
engine_renderer_init(struct engine *engine)
{
    struct renderer *renderer;

    if (engine->assets == NULL)
    {
        log_error("Cannot initialize renderer before asset manager");
    }

    log_debug("Allocating & Initializing Renderer...");

    renderer =
        alloc(engine->platform->memory->permanent, sizeof(struct renderer));

    renderer_init(renderer, engine, engine->platform->memory->permanent);

    engine->renderer = renderer;
}

static void
engine_asset_init(struct engine *engine)
{
    struct platform *platform = engine->platform;
    struct asset_manager *assets;

    log_debug("Allocating & Initializing Asset Manager...");

    assets = alloc(platform->memory->permanent, sizeof(struct asset_manager));

    asset_init(assets, platform->memory->permanent);

    asset_add_handler(assets, "ogg", asset_load_sound, NULL, engine);
    asset_add_handler(assets, "jpg", asset_load_texture, NULL, engine);
    asset_add_handler(assets, "png", asset_load_texture, NULL, engine);

    asset_add_handler(assets, "frag", asset_load_fragment_shader, NULL, engine);
    asset_add_handler(assets, "vert", asset_load_vertex_shader, NULL, engine);

    engine->assets = assets;
}

void
engine_init(struct platform *platform, struct engine **engine)
{
    log_debug("Allocating & Initializing Engine...");
    *engine = alloc(platform->memory->permanent, sizeof(struct engine));
    (*engine)->platform = platform;

    engine_asset_init(*engine);
    engine_ecs_init(*engine);
    engine_audio_init(*engine);
    engine_renderer_init(*engine);
}

void
engine_unbind(struct engine *engine)
{
    struct ecs_system *system;

    struct platform_api *api = engine->platform->api;
    struct mixer *mixer = engine->mixer;
    struct ecs *ecs = engine->ecs;

    engine_close_audio_device(api, mixer);
    mixer_set_event_handler(mixer, NULL);

    array_for_each (system, ecs->systems)
    {
        ecs_bind_system_funcs(ecs, system->id, NULL);
    }
}

void
engine_bind(struct engine *engine)
{
    struct ecs_system *system;

    struct platform_api *api = engine->platform->api;
    struct mixer *mixer = engine->mixer;
    struct ecs *ecs = engine->ecs;

    engine_open_audio_device(api, mixer);
    mixer_set_event_handler(mixer, engine__mixer_event_handler);

    bind_system_funcs(ecs, light);
    bind_system_funcs(ecs, movement);
    bind_system_funcs(ecs, shadowcaster);
    bind_system_funcs(ecs, sprite);
    bind_system_funcs(ecs, mouse_follow);

    array_for_each (system, ecs->systems)
    {
        if (system->process == NULL)
        {
            log_error("System (%s, %i) process function not bound",
                      system->name,
                      system->id);
        }
    }
}

void
engine_resize(struct engine *engine)
{
    if (engine != NULL && engine->renderer != NULL &&
        engine->renderer->fbo != NULL)
    {
        fbo_resize(engine->renderer->fbo,
                   engine->platform->window_size_x,
                   engine->platform->window_size_y);
    }
}
