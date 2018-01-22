#include "engine.h"
#include "assets/shader_asset.h"
#include "shader.h"

void *
asset_load_fragment_shader(const char *filename, void *udata)
{
    struct engine *engine = udata;
    u32 *shader = alloc(engine->platform->memory->permanent, sizeof(u32));

    *shader = shader_load(engine->platform->memory->permanent,
                          GL_FRAGMENT_SHADER,
                          filename);

    return shader;
}

void *
asset_load_vertex_shader(const char *filename, void *udata)
{
    struct engine *engine = udata;
    u32 *shader = alloc(engine->platform->memory->permanent, sizeof(u32));

    *shader = shader_load(engine->platform->memory->permanent,
                          GL_VERTEX_SHADER,
                          filename);

    return shader;
}